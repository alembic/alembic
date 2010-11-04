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

#include "Instancer.h"
#include "RiUtil.h"
#include "AbcUtil.h"
#include "Factory.h"
#include "Request.h"

#include <ImathBoxAlgo.h>
#include <exception>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

// for the call to RiProcedural
extern "C" RtVoid Free( RtPointer data );
extern "C" RtVoid Subdivide( RtPointer data, RtFloat detail );

namespace AlembicRiPlugin {

namespace ba = boost::algorithm;

//-*****************************************************************************
static bool keepPathElem( const std::string &prev,
                          const std::string &next )
{

    bool keep = ( ( prev == "Alembic" && next == "Children" ) ||
                  ( prev == "Children" && ( next == prev || next == "" ) ) );

    return keep;
}

//-*****************************************************************************
static std::string hdfPathToAlembic( const std::string &p )
{
    StringVec pvec;
    StringVec nvec;
    nvec.push_back( "" );

    ba::split( pvec, p,  ba::is_any_of( "/" ) );

    if ( pvec.size() < 2 ) { return "/"; }

    for ( size_t i = 1 ; i < pvec.size() ; ++i )
    {
        if ( i == ( pvec.size() - 1 ) ) // the last element is wanted
        {
            nvec.push_back( pvec[i] );
        }
        else if ( keepPathElem( pvec[i - 1], pvec[i + 1] ) )
        {
            nvec.push_back( pvec[i] );
        }
    }

    return ba::join( nvec, "/" );
}

//-*****************************************************************************
static std::string fullPathName( const Abc::IObject &o )
{
    return hdfPathToAlembic( o.fullPathName() );
}

//-*****************************************************************************
static void getAllChildren( const Abc::IParentObject &p, StringVec &results )
{
    size_t numChildren = p.numChildren();
    if ( numChildren > 0 )
    {
        for ( size_t i = 0 ; i < numChildren ; ++i )
        {
            Abc::IObject obj( p, i );
            std::string prot = obj.protocol();
            if ( prot == "AlembicSimplePolyMesh_v0001" ||
                 prot == "AlembicSimpleSubd_v0001" )
            {
                results.push_back( fullPathName( obj ) );
            }
            getAllChildren( obj, results );
        }
    }
}

//-*****************************************************************************
static Abc::Box3d getFinalBounds( const Abc::IParentObject &p,
                                  const std::string &path,
                                  const Abc::Time &t,
                                  Mutex &mtx )
{
    Abc::IObject child;
    Abc::M44d xform;
    Abc::Box3d bnds;

    bnds.makeEmpty();

    StringVec pvec;
    ba::split( pvec, path, ba::is_any_of( "/" ) );

    // If there's a leading '/' in the path, get rid of the blank
    // first element of the resulting split.
    if (  pvec.size() > 0 && pvec.front() == "" )
    {
        pvec.erase( pvec.begin() );
    }

    // For the first element, make a child IObject out of the
    // IParentObject and the first child-path element. Get the xform of
    // that child object and initialize our xform accumulator with it.
    child = Abc::IObject( p, pvec.front() );
    pvec.erase( pvec.begin() );

    Atg::IXformLocalTrait xft( child );
    {
        MutexLock lck( mtx );
        xform = xft.getAnimLowerBound( t );
    }

    for ( StringVec::const_iterator iter = pvec.begin() ;
          iter != pvec.end() ; ++iter )
    {
        // We're somewhere in the middle of the path. Make a child,
        // get its xform, and accumulate it into our xform.
        child = Abc::IObject( child, *iter );

        Atg::IXformLocalTrait xftrait( child );
        {
            MutexLock lck( mtx );
            xform = xftrait.getAnimLowerBound( t ) * xform;
        }
    }

    // OK, we've iterated through the path, and we have our final child and
    // final accumulated transform.  Get the local bounds and transform
    // by our xform.
    Atg::IBoundsLocalTrait btrait( child );
    if ( btrait )
    {
        MutexLock lck( mtx );
        Abc::Box3d b = btrait.getAnimLowerBound( t );
        bnds.extendBy( Imath::transform( b , xform ) );
    }

    return bnds;
}

//-*****************************************************************************
Abc::Box3d Instancer::getBounds( const std::string &objectName,
                                 const TimeSamples &tsamps )
{
    // Use the first time sample. Prman only shades the first
    // sample anyway. (except in multisegment, but whatevs.)
    Abc::Time time( Abc::kRestTime );
    if ( tsamps.size() > 0 )
    {
        time = tsamps[0];
    }

    return getFinalBounds( m_asset->alembicParentObject(), objectName, time,
                           m_mutex );
}

//-*****************************************************************************
const std::string Instancer::makeConfigString( const std::string &assetName,
                                               const std::string &objectName,
                                               const float samplesPerSecond,
                                               const float motionSampleBias,
                                               const float motionSampleGain,
                                               const ChronoSamples &csamps,
                                               const bool isBounded )
{
    std::stringstream ss;

    ss << "assetName=" << assetName << ";";

    if ( objectName != "" )
    {
        ss << "objectName=" << objectName << ";";
    }

    ss << "samplesPerSecond=" << samplesPerSecond << ";";

    ss << "motionSampleBias=" << motionSampleBias << ";";

    ss << "motionSampleGain=" << motionSampleGain << ";";

    ss << "isBounded=";
    if ( isBounded )
    {
        ss << "true";
    }
    else
    {
        ss << "false";
    }
    ss << ";";

    if ( csamps.size() > 0 )
    {
        ss << "samples=";
        for ( ChronoSamples::const_iterator iter = csamps.begin() ;
              iter != csamps.end() ; ++iter )
        {
            ss << *iter;
            if ( iter != --( csamps.end() ) )
            {
                ss << ",";
            }
        }
    }

    return ss.str();
}


//-*****************************************************************************
void Instancer::subdivide( RtFloat detail )
{
    try
    {
        // Check plugin
        if ( !m_plugin )
        {
            ABCRI_ERROR( "Null plugin pointer in Instancer::subdivide()" );
            return;
        }

        std::string assetName;
        // Check asset
        if ( !m_asset )
        {
            if ( !m_tokenMap.find( "assetName", assetName ) )
            {
                ABCRI_ERROR( "Config String: NO assetName specified" );
                return;
            }

            m_asset = m_plugin->asset( assetName );

            if ( !m_asset )
            {
                ABCRI_ERROR( "Could not load asset: " << assetName );
            }
        }

        // Get samplesPerSecond.
        float samplesPerSecond = 24.0f;
        m_tokenMap.find( "samplesPerSecond", samplesPerSecond );

        float motionSampleBias = 0.0f;
        m_tokenMap.find( "motionSampleBias", motionSampleBias );

        float motionSampleGain = 1.0f;
        m_tokenMap.find( "motionSampleGain", motionSampleGain );

        // Get chrono samples
        ChronoSamples csamps;
        m_tokenMap.find( "samples", csamps );

        // Convert chrono samples into time samples
        TimeSamples tsamps;
        ConvertSamples( csamps, tsamps,
                        ( Abc::chrono_t )samplesPerSecond );

        std::string objectName = "";
        m_tokenMap.find( "objectName", objectName );

        // bounded?
        std::string isBounded = "false";
        m_tokenMap.find( "isBounded", isBounded );


        // Do it?
        if ( isBounded == "false" || objectName == "" )
        {
            StringVec onames;
            onames.clear();
            onames.resize( 0 );

            if ( objectName == "" )
            {
                getAllChildren( m_asset->alembicParentObject(), onames );
            }
            else
            {
                onames.push_back( objectName );
            }

            for ( StringVec::const_iterator iter = onames.begin() ;
                  iter != onames.end() ; ++iter )
            {
                Abc::Box3d bnds = getBounds( *iter, tsamps );
                const Abc::V3d min = bnds.min;
                const Abc::V3d max = bnds.max;
                RtBound bnd = { min[0], max[0],
                                min[1], max[1],
                                min[2], max[2] };

                std::string configStr = makeConfigString( assetName,
                                                          *iter,
                                                          samplesPerSecond,
                                                          motionSampleBias,
                                                          motionSampleGain,
                                                          csamps,
                                                          true );

                RiProcedural( ( RtPointer )( new Instancer( m_plugin,
                                                            m_mutex,
                                                            configStr ) ),
                              bnd, Subdivide, Free );
            }
        }
        else
        {
            // Create a request.
            Request request( tsamps, objectName, motionSampleBias,
                             motionSampleGain, m_mutex );
            m_asset->instantiate( request );
        }

    }
    catch ( std::exception &exc )
    {
        ABCRI_ERROR( "Instancer::subdivide() Exception: " << exc.what() );
    }
    catch ( ... )
    {
        ABCRI_ERROR( "Instancer::subdivide() Unknown Exception" );
    }
}

} // End namespace AlembicRiPlugin
