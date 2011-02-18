//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
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


// Alembic Includes
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

// Other includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Assert.h"

#include <Alembic/AbcGeom/Tests/MeshData.h>

using namespace Alembic::AbcGeom;

static const chrono_t g_dt = 1.0 / 24.0;
static const chrono_t g_t0 = 0.25;

static std::vector<double> g_ry;

static const double VAL_EPSILON = std::numeric_limits<double>::epsilon() \
    * 1024;

static const chrono_t CHRONO_EPSILON = std::numeric_limits<chrono_t>::epsilon() \
    * 1024;

//-*****************************************************************************
void Example1_MeshOut()
{
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      "transformingMesh1.abc" );

    OObject top = archive.getTop();

    TimeSamplingType tst( g_dt );

    OSimpleXform xf( top, "xf", tst );

    OPolyMesh meshyObj( xf, "meshy" );
    OPolyMeshSchema &mesh = meshyObj.getSchema();

    SimpleXformSample xsamp;

    xsamp.setTranslation( V3d( 0.0, 0.0, 4.04 ) );

    g_ry.push_back( 0 );
    g_ry.push_back( 32.44 );
    g_ry.push_back( 64.88 );
    g_ry.push_back( 82.66 );
    g_ry.push_back( 50.2225 );
    g_ry.push_back( 17.7781 );
    g_ry.push_back( -14.6662 );
    g_ry.push_back( -47.1106 );
    g_ry.push_back( -79.555 );
    g_ry.push_back( -68.0007 );

    for ( size_t i = 0 ; i < 10 ; i++ )
    {
        chrono_t t = g_t0 + ( i * g_dt );

        if ( i == 9 )
        {
            xsamp.setTranslation( V3d( 2.0, 0.0, 4.04 ) );
        }

        OSampleSelector oss( i, t );

        xsamp.setXYZRotation( V3d( 180.0, g_ry[i], 180.0 ) );

        std::cout << "Writing " << i << "th xform sample:" << std::endl
                  << xsamp << std::endl;

        xf.getSchema().set( xsamp, oss );

    }

    OPolyMeshSchema::Sample mesh_samp(
        V3fArraySample( ( const V3f * )g_verts, g_numVerts ),
        Int32ArraySample( g_indices, g_numIndices ),
        Int32ArraySample( g_counts, g_numCounts ) );

    // Set the sample.
    mesh.set( mesh_samp );

    std::cout << std::endl;
}

//-*****************************************************************************
void Example1_MeshIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                      "transformingMesh1.abc" );

    ISimpleXform xf( archive.getTop(), "xf" );

    ISimpleXformSchema xfs = xf.getSchema();

    TESTING_ASSERT( xfs.getNumSamples() == 10 );

    TimeSampling ts = xfs.getTimeSampling();

    TESTING_ASSERT( ts.getTimeSamplingType().isUniform() );

    for ( size_t i = 0 ; i < xfs.getNumSamples() ; i++ )
    {
        chrono_t t = g_t0 + ( i * g_dt );

        TESTING_ASSERT( Imath::equalWithAbsError( t, ts.getSampleTime( i ),
                                                  CHRONO_EPSILON ) );

        SimpleXformSample samp = xfs.getValue( i );

        std::cout << "Reading " << i << "th xform sample:" << std::endl
                  << samp << std::endl;

        double ry = samp.getXYZRotation().y;

        TESTING_ASSERT( Imath::equalWithAbsError( ry, g_ry[i], VAL_EPSILON ) );
    }

}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    Example1_MeshOut();
    Example1_MeshIn();

    return 0;
}
