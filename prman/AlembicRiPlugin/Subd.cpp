//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#include "Subd.h"
#include "Xform.h"
#include "Request.h"
#include "RiUtil.h"
#include "AbcUtil.h"
#include "RenderPropertyInfo.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
Subd::Subd( Factory &factory,
            ParentObject &parent,
            const std::string &meshName )
  : ParentObject(),
    m_subd( parent.alembicParentObject(), meshName, "" )
{
    ParentObject::init( factory, m_subd );
}

//-*****************************************************************************
//-*****************************************************************************
// Fun little helper struct.
//-*****************************************************************************
//-*****************************************************************************
struct NamedSample
{
    NamedSample() : name(), rmanDesc(), sample() {}

    NamedSample( const std::string &n,
                 const std::string &rd,
                 const void *data = NULL,
                 const Abc::Dimensions &dims = Abc::Dimensions() )
      : name( n ),
        rmanDesc( rd ),
        sample( data, dims ) {}

    NamedSample( const std::string &n,
                 const std::string &rd,
                 const Abc::ISample &samp )
      : name( n ),
        rmanDesc( rd ),
        sample( samp ) {}

    NamedSample( const NamedSample &copy )
      : name( copy.name ),
        rmanDesc( copy.rmanDesc ),
        sample( copy.sample ) {}

    NamedSample &operator=( const NamedSample &copy )
    {
        name = copy.name;
        rmanDesc = copy.rmanDesc;
        sample = copy.sample;
        return *this;
    }

    std::string name;
    std::string rmanDesc;
    Abc::ISample sample;
};

//-*****************************************************************************
void Subd::instantiateLocalNoMB( const Abc::Time &time,
                                 Request &rq,
                                 const Abc::IIntSample &faceCounts,
                                 const Abc::IIntSample &indices,
                                 const Abc::IV2fSample &uvs )
{
    Abc::IV3dSample points;
    Abc::IIntSample holes;
    Abc::IIntSample corners;
    Abc::IIntSample creaseCounts;
    Abc::IIntSample creaseIndices;
    {
        MutexLock mlock( rq.mutex() );

        points = m_subd->mesh().positions().getAnimLowerBound( time );
        if ( !points || points.size() < 3 )
        {
            return;
        }

        holes = m_subd->subd().holes().get();
        corners = m_subd->subd().corners().get();
        creaseCounts = m_subd->subd().creases().count().get();
        creaseIndices = m_subd->subd().creases().index().get();
    }

    size_t numProps = m_subd.numProperties();
    std::vector<NamedSample> propData;
    propData.reserve( numProps );

    for ( size_t propNum = 0 ; propNum < numProps ; ++propNum )
    {
        Abc::PropertyInfo pi = m_subd.propertyInfo( propNum );
        std::string pnameName = GetRManPropertyNameName( pi );

        if ( pnameName == "P" ||
             pnameName == "N" ||
             pnameName == "uv" ||
             pnameName == "count" ||
             pnameName == "index" ||
             pnameName == "xform|local" ||
             pnameName == "bounds|local" ||
             pi->name == "subd|scheme" ||
             pi->name == "subd|interpolateBoundary" ||
             pi->name == "subd|hole" ||
             pi->name == "subd|corner" ||
             pi->name == "subd|creaseCount" ||
             pi->name == "subd|creaseIndex" ||
             pnameName == "" )
        {
            // we handle those outside here
            continue;
        }

        std::string rmanAttrDecStr = GetRManAttrType( pi );
        if ( "" == rmanAttrDecStr )
        {
            continue;
        }

        NamedSample namedSamp( pnameName, rmanAttrDecStr );

        if ( pi->ptype == Abc::kSingularProperty )
        {
            MutexLock mlock( rq.mutex() );
            Abc::ISingularProperty prop( m_subd, pi );
            if ( prop.isAnimated() )
            {
                namedSamp.sample = Abc::ISample(
                    prop.animSampleLowerBound( time ),
                    Abc::Dimensions() );
            }
            else
            {
                namedSamp.sample = Abc::ISample( prop.restSample(),
                                                 Abc::Dimensions() );
            }
        }
        else if ( pi->ptype == Abc::kMultiProperty )
        {
            MutexLock mlock( rq.mutex() );
            Abc::IMultiProperty prop( m_subd, pi );

            if ( prop.isAnimated() )
            {
                namedSamp.sample = prop.animSampleLowerBound( time );
            }
            else
            {
                namedSamp.sample = prop.restSample();
            }
        }

        propData.push_back( namedSamp );
    }

    //-*************************************************************************
    // POINTS! (Have to convert from V3d to RtFloat)
    //-*************************************************************************
    size_t numPoints = points.size();
    std::vector<RtFloat> PdataF( numPoints * 3 );
    const double *PdataD = ( const double * )points.getRaw();
    for ( size_t i = 0, s = numPoints*3; i < s; ++i )
    {
        PdataF[i] = ( RtFloat )PdataD[i];
    }

    propData.push_back(
        NamedSample( "P", "P", ( const void * )&PdataF.front() ) );

    //Abc::Box3d bnds;
    //bnds.makeEmpty();
    //for ( size_t i = 0; i < numPoints; ++i )
    //{
    //    bnds.extendBy( points[i] );
    //}

    if ( uvs )
    {
        propData.push_back(
            NamedSample( "uv", "facevarying float[2] uv", uvs.getRaw() ) );
    }

    RtToken scheme = NULL;

    Atg::SubdScheme sscheme =
        ( Atg::SubdScheme )( m_subd->subd().scheme().get() );
    switch ( sscheme )
    {
    case Atg::kLoopSubd:
        scheme = "loop";
        break;
    default:
    case Atg::kCatmullClarkSubd:
        scheme = "catmull-clark";
        break;
    };

    Atg::SubdBoundaryInterpolation sbnd =
        ( Atg::SubdBoundaryInterpolation )
        m_subd->subd().boundary().get();

    // tags are tricky, because of creases, corners, and holes.
    //
    // One tag for interpolateboundary, one if holes, one if corners,
    // then n creases.
    RtInt ntags = 1 + creaseCounts.size();
    if ( corners ) { ntags += 1; }
    if ( holes ) { ntags += 1; }
    RtInt numTagArgs = 2 * ntags;

    // One arg for interpolateboundary, one for each hole,
    // one for each corner, and one for each crease index.
    RtInt numIntArgs = 1 + holes.size() + corners.size()
        +  creaseIndices.size();

    RtInt numCreasesAndCorners = creaseCounts.size() + corners.size();
    RtInt numFloatArgs = numCreasesAndCorners;
    if ( numFloatArgs == 0 ) { numFloatArgs = 1; }

    RtToken tags[ntags];
    RtInt nargs[numTagArgs];
    RtInt intargs[numIntArgs];
    RtFloat floatargs[numFloatArgs];

    size_t tagindex = 0;
    size_t intindex = 0;
    size_t floatindex = 0;
    size_t nargsindex = 0;

    // interpolateboundary
    tags[tagindex++] = "interpolateboundary";

    intargs[intindex++] = ( RtInt )sbnd;

    nargs[nargsindex++] = 1;
    nargs[nargsindex++] = 0;

    // holes
    if ( holes )
    {
        tags[tagindex++] = "hole";

        nargs[nargsindex++] = holes.size();
        nargs[nargsindex++] = 0;

        for ( size_t i = 0 ; i < holes.size() ; ++i )
        {
            intargs[intindex++] = ( RtInt )holes[i];
        }
    }

    // corners
    if ( corners )
    {
        tags[tagindex++] = "corner";

        nargs[nargsindex++] = corners.size();
        nargs[nargsindex++] = corners.size();

        for ( size_t i = 0 ; i < corners.size() ; ++i )
        {
            intargs[intindex++] = ( RtInt )corners[i];

            floatargs[floatindex++] = RI_INFINITY;
        }
    }

    // creases
    if ( creaseCounts )
    {
        size_t creaseindex = 0;

        for ( size_t i = 0 ; i < creaseCounts.size() ; ++i )
        {
            tags[tagindex++] = "crease";

            size_t count = creaseCounts[i];

            nargs[nargsindex++] = count;
            nargs[nargsindex++] = 1;

            floatargs[floatindex++] = RI_INFINITY;

            for ( size_t j = 0 ; j < count ; ++j )
            {
                intargs[intindex++] = creaseIndices[creaseindex++];
            }
        }
    }

    if ( numCreasesAndCorners == 0 ) { floatargs[0] = 0.0f; }

    // Okay, turn propData into tokens & pointers.
    std::vector<RtToken> tokens( propData.size() );
    std::vector<RtPointer> pointers( propData.size() );
    for ( size_t piter = 0; piter < propData.size(); ++piter )
    {
        tokens[piter] = const_cast<char *>( propData[piter].rmanDesc.c_str() );
        pointers[piter] = const_cast<void *>( propData[piter].sample.getRaw() );
    }

    RiSubdivisionMeshV( scheme,
                        ( RtInt )faceCounts.size(),
                        ( RtInt * )( faceCounts.get() ),
                        ( RtInt * )( indices.get() ),
                        ntags, tags, nargs, intargs, floatargs,
                        tokens.size(),
                        ( RtToken * )&tokens.front(),
                        ( RtPointer * )&pointers.front() );

    // Release points
    {
        MutexLock mlock( rq.mutex() );
        points.release();
    }
}

//-*****************************************************************************
const Abc::IParentObject &Subd::alembicParentObject() const throw()
{
    return ( const Abc::IParentObject & )m_subd;
}

//-*****************************************************************************
void Subd::instantiateAttributeState( Request &rq )
{
    XformInstantiateAttributeState( rq, m_subd->xform() );
}

//-*****************************************************************************
void Subd::instantiateLocal( Request &rq )
{
    if ( !m_subd || !m_subd->mesh() )
    {
        return;
    }

    // These properties are not animated, so just get them once.
    Abc::IIntSample faceCounts;
    Abc::IIntSample indices;
    Abc::IV2fSample uvs;
    {
        MutexLock mlock( rq.mutex() );
        faceCounts = m_subd->mesh().faceCounts().get();
        indices = m_subd->mesh().vertexIndices().get();
        uvs = m_subd->uvs().get();

        if ( !faceCounts || faceCounts.size() < 1 ||
             !indices || indices.size() < 3 )
        {
            return;
        }
    }

    if ( rq.restPose() )
    {
        instantiateLocalNoMB( Abc::Time( Abc::kRestTime ),
                              rq, faceCounts, indices, uvs );
    }
    else
    {
        const TimeSamples &tsamps = rq.timeSamples();
        const float motionSampleBias = rq.motionSampleBias();
        const float motionSampleGain = rq.motionSampleGain();

        if ( tsamps.size() > 1 )
        {
            MotionBegin( tsamps, motionSampleBias, motionSampleGain );
        }

        // No interpolation for now. Just lower bounds.
        for ( TimeSamples::const_iterator iter = tsamps.begin();
              iter != tsamps.end(); ++iter )
        {
            instantiateLocalNoMB( (*iter),
                                  rq, faceCounts, indices, uvs );
        }

        if ( tsamps.size() > 1 )
        {
            RiMotionEnd();
        }
    }

    // Release!
    {
        MutexLock mlock( rq.mutex() );
        faceCounts.release();
        indices.release();
        uvs.release();
    }
}

} // namespace AlembicRi
