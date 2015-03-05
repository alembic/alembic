//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
#include <Alembic/AbcCoreOgawa/All.h>

// Other includes
#include <math.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#include <Alembic/AbcGeom/Tests/MeshData.h>

using namespace Alembic::AbcGeom;

static const chrono_t g_dt = 1.0 / 24.0;
static const chrono_t g_t0 = 0.25;

static TimeSamplingPtr g_ts;

static OPolyMeshSchema::Sample g_meshsamp(
    V3fArraySample( ( const V3f * )g_verts, g_numVerts ),
    Int32ArraySample( g_indices, g_numIndices ),
    Int32ArraySample( g_counts, g_numCounts ) );

//-*****************************************************************************
OXform recurseCreateXform( OObject &iParent, size_t children, Alembic::Util::int32_t level,
                           size_t numSamps )
{
    OXform xform;

    for ( size_t i = 0; i < children; ++i )
    {
        std::ostringstream strm;
        strm << level << "_" << i;
        std::string xformName = strm.str();
        xform = OXform( iParent, xformName, g_ts );

        OPolyMesh opm( xform, "meshy", g_ts );

        XformSample samp;
        XformOp transop( kTranslateOperation, kTranslateHint );
        XformOp rotatop( kRotateOperation, kRotateHint );

        for ( size_t j = 0 ; j < numSamps ; ++j )
        {
            V3d translation( pow( -1.0, (double)(j) ) + 2.0 * j, level * j,
                             pow( -1.0, (double)(j) ) + 3.0 * j );

            samp.addOp( transop, translation );
            samp.addOp( rotatop, V3d(0.0, 0.0, 1.0), (double)level * 10.0 * j );
            samp.addOp( rotatop, V3d(0.0, 1.0, 0.0), (double)level * 10.0 * j );
            samp.addOp( rotatop, V3d(1.0, 0.0, 0.0), (double)level * 20.0 * j );
            xform.getSchema().set( samp );
            opm.getSchema().set( g_meshsamp );
        }
        if ( level > 0 )
        {
            recurseCreateXform( xform, children, level - 1, numSamps );
        }
    }

    return xform;
}

//-*****************************************************************************
void Example1_MeshOut()
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
                      "transformingMesh1.abc" );

    OObject top = archive.getTop();

    OXform parentXform = recurseCreateXform( top, 3, 5, 10 );

    OXform xf( parentXform, "xf", g_ts );

    OPolyMesh meshyObj( xf, "meshy" );
    OPolyMeshSchema &mesh = meshyObj.getSchema();

    XformOp transop( kTranslateOperation, kTranslateHint );

    // rotate y axis
    XformOp rotYop( kRotateYOperation );

    XformSample xfsamp;

    for ( size_t i = 0 ; i < 100 ; ++i )
    {
        chrono_t t = g_t0 + ( i * g_dt );

        xfsamp.addOp( transop, V3d( t, 0.0, 0.0 ) );
        xfsamp.addOp( rotYop, 200.0 * t );

        xf.getSchema().set( xfsamp );

        mesh.set( g_meshsamp );
    }
}


//-*****************************************************************************
int main( int argc, char *argv[] )
{
    g_ts =  TimeSamplingPtr( new TimeSampling( g_dt, g_t0 ) );
    Example1_MeshOut();

    return 0;
}
