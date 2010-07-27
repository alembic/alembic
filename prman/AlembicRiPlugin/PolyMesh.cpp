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

#include "PolyMesh.h"
#include "Xform.h"
#include "Request.h"
#include "RiUtil.h"
#include "AbcUtil.h"
#include "RenderPropertyInfo.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
PolyMesh::PolyMesh( Factory &factory,
                    ParentObject &parent,
                    const std::string &meshName )
  : ParentObject(),
    m_mesh( parent.alembicParentObject(), meshName, "" )
{
    ParentObject::init( factory, m_mesh );
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
void PolyMesh::instantiateLocalNoMB( const Abc::Time &time,
                                     Request &rq,
                                     const Abc::IIntSample &faceCounts,
                                     const Abc::IIntSample &indices,
                                     const Abc::IV2fSample &uvs )
{
    Abc::IV3dSample points;
    Abc::IV3fSample normals;
    {
        MutexLock mlock( rq.mutex() );

        points = m_mesh->mesh().positions().getAnimLowerBound( time );
        if ( !points || points.size() < 3 )
        {
            return;
        }

        if ( m_mesh->normals() )
        {
            normals = m_mesh->normals().getAnimLowerBound( time );
        }
    }

    size_t numProps = m_mesh.numProperties();
    std::vector<NamedSample> propData;
    propData.reserve( numProps );

    for ( size_t propNum = 0; propNum < numProps; ++propNum )
    {
        Abc::PropertyInfo pi = m_mesh.propertyInfo( propNum );
        std::string pnameName = GetRManPropertyNameName( pi );

        if ( pnameName == "P" ||
             pnameName == "N" ||
             pnameName == "uv" ||
             pnameName == "count" ||
             pnameName == "index" ||
             pnameName == "xform|local" ||
             pnameName == "bounds|local" ||
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
            Abc::ISingularProperty prop( m_mesh, pi );

            // This will return rest pose for unanimated property
            namedSamp.sample = Abc::ISample( prop.animSampleLowerBound( time ),
                                             Abc::Dimensions() );
        }
        else if ( pi->ptype == Abc::kMultiProperty )
        {
            MutexLock mlock( rq.mutex() );
            Abc::IMultiProperty prop( m_mesh, pi );

            // This will return rest pose for unanimated property
            namedSamp.sample = prop.animSampleLowerBound( time );
        }

        propData.push_back( namedSamp );
    }

    // We got some Very Special cases here.

    size_t numPoints = points.size();
    std::vector<RtFloat> PdataF( numPoints * 3 );
    const double *PdataD = ( const double * )points.getRaw();
    for ( size_t i = 0, s = numPoints*3; i < s; ++i )
    {
        PdataF[i] = ( RtFloat )PdataD[i];
    }

    propData.push_back(
        NamedSample( "P", "P", ( const void * )&PdataF.front() ) );

    //-*************************************************************************
    //-*************************************************************************
    // NORMALS
    //-*************************************************************************
    //-*************************************************************************
    std::vector<Abc::V3f> SmoothNormals;
    std::vector<Abc::V3f> SmoothNormalsFV;
    if ( !normals )
    {
        SmoothNormals.resize( points.size() );
        SmoothNormalsFV.resize( indices.size() );
        std::fill( SmoothNormals.begin(),
                   SmoothNormals.end(),
                   Abc::V3f( 0.0f ) );

        size_t firstVertex = 0;
        for ( size_t face = 0; face < faceCounts.size(); ++face )
        {
            size_t count = faceCounts[face];
            for ( size_t tcount = 2; tcount < count; ++tcount )
            {
                size_t iA = indices[firstVertex];
                size_t iB = indices[firstVertex+tcount-1];
                size_t iC = indices[firstVertex+tcount];

                const Abc::V3d &A = points[iA];
                const Abc::V3d &B = points[iB];
                const Abc::V3d &C = points[iC];

                Abc::V3d AB = B - A;
                Abc::V3d AC = C - A;

                Abc::V3d wN = AB.cross( AC );
                Abc::V3f wNf( wN.x, wN.y, wN.z );
                SmoothNormals[iA] += wNf;
                SmoothNormals[iB] += wNf;
                SmoothNormals[iC] += wNf;
            }
            firstVertex += count;
        }

        // Normalize normals.
        for ( size_t nidx = 0; nidx < SmoothNormals.size(); ++nidx )
        {
            SmoothNormals[nidx].normalize();
        }

        // Stuff the normalized normals into the facevarying form.
        for ( size_t iidx = 0; iidx < indices.size(); ++iidx )
        {
            SmoothNormalsFV[iidx] = SmoothNormals[indices[iidx]];
        }

        propData.push_back(
            NamedSample( "N",
                         "facevarying normal N",
                         ( const void * )&SmoothNormalsFV.front() ) );
    }
    else
    {
        propData.push_back(
            NamedSample( "N",
                         "facevarying normal N",
                         normals.getRaw() ) );
    }

    if ( uvs )
    {
        propData.push_back(
            NamedSample( "uv",
                         "facevarying float[2] uv",
                         uvs.getRaw() ) );
    }

    // Okay, turn propData into tokens & pointers.
    std::vector<RtToken> tokens( propData.size() );
    std::vector<RtPointer> pointers( propData.size() );
    for ( size_t piter = 0; piter < propData.size(); ++piter )
    {
        tokens[piter] = const_cast<char *>( propData[piter].rmanDesc.c_str() );
        pointers[piter] = const_cast<void *>( propData[piter].sample.getRaw() );
    }

    RiPointsPolygonsV( ( RtInt )faceCounts.size(),
                       ( RtInt * )( faceCounts.get() ),
                       ( RtInt * )( indices.get() ),
                       tokens.size(),
                       ( RtToken * )&tokens.front(),
                       ( RtPointer * )&pointers.front() );

    // Release points and normals
    {
        MutexLock mlock( rq.mutex() );
        points.release();
        normals.release();
    }
}

//-*****************************************************************************
const Abc::IParentObject &PolyMesh::alembicParentObject() const throw()
{
    return ( const Abc::IParentObject & )m_mesh;
}

//-*****************************************************************************
void PolyMesh::instantiateAttributeState( Request &rq )
{
    XformInstantiateAttributeState( rq, m_mesh->xform() );
}

//-*****************************************************************************
void PolyMesh::instantiateLocal( Request &rq )
{
    if ( !m_mesh || !m_mesh->mesh() )
    {
        return;
    }

    // These properties are not animated, so just get them once.
    Abc::IIntSample faceCounts;
    Abc::IIntSample indices;
    Abc::IV2fSample uvs;
    {
        MutexLock mlock( rq.mutex() );
        faceCounts = m_mesh->mesh().faceCounts().get();
        indices = m_mesh->mesh().vertexIndices().get();
        uvs = m_mesh->uvs().get();

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
