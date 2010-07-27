//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks Inc. and
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
// Industrial Light & Magic, nor the names of their contributors may be used
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

#include <Alembic/TraitsGeom/TraitsGeomAll.h>
#include <Alembic/Asset/AssetAll.h>

#include <boost/format.hpp>
#include <boost/timer.hpp>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <exception>
#include <string>

//-*****************************************************************************
namespace Abc = Alembic::Asset;
namespace Atg = Alembic::TraitsGeom;

//-*****************************************************************************
// Forwards.
template <class MESHY>
void VisitMesh( const MESHY &mesh,
                Abc::Box3d &bnds );

//-*****************************************************************************
void VisitObject( const Abc::IParentObject &obj, Abc::Box3d &bnds )
{   
    size_t numChildren = obj.numChildren();
    for ( size_t c = 0; c < numChildren; ++c )
    {
        Abc::ObjectInfo oinfo = obj.childInfo( c );
        if ( !oinfo ) { continue; }

        if ( oinfo->protocol == "AlembicSimplePolyMesh_v0001" )
        {
            Atg::ISimplePolyMesh mesh( obj, oinfo );
            VisitMesh( mesh, bnds );
        }
        else if ( oinfo->protocol == "AlembicSimpleSubd_v0001" )
        {
            Atg::ISimpleSubd mesh( obj, oinfo );
            VisitMesh( mesh, bnds );
        }
        else
        {
            Abc::IObject childObj( obj, oinfo );
            VisitObject( childObj, bnds );
        }
    }
}

//-*****************************************************************************
template <class T>
static inline bool ISNANORINF( const T &v )
{
    //int a = fpclassify( v );
    //if ( a == FP_NAN || a == FP_INFINITE ) { return true; }
    //else { return false; }
    return std::isnan( v ) || std::isinf( v );
}

//-*****************************************************************************
template <class MESHY>
void VisitMesh( const MESHY &mesh, Abc::Box3d &bnds )
{
    if ( !mesh ) { return; }
    
    std::cout << "Visiting mesh: " << mesh.name() << std::endl;

    Abc::M44d local = mesh->xform().get();
    std::cout << "\tLocal = " << local << std::endl;
    
    Abc::IV3fSample points = mesh->mesh().positions().get();

    if ( (( bool )points) && points.size() > 0 )
    {
        size_t numPoints = points.size();
        for ( size_t pnt = 0; pnt < numPoints; ++pnt )
        {
            const Abc::V3f &vertIn = points[pnt];
            if ( ISNANORINF( vertIn[0] ) ||
                 ISNANORINF( vertIn[1] ) ||
                 ISNANORINF( vertIn[2] ) )
            {
                std::cout << "WARNING: MESH: " << mesh.name()
                          << "has a nan/inf vertex in position: " << pnt
                          << std::endl;
                continue;
            }
            
            Abc::V3f vert;
            local.multVecMatrix( vertIn, vert );
            bnds.extendBy( vert );
        }
    }

    std::cout << "Bounds so far = " << bnds.min << " - " << bnds.max
              << std::endl;

    VisitObject( mesh, bnds );
}

//-*****************************************************************************
//-*****************************************************************************
void doIt( const std::string &abcFileName )
{
    Abc::Init();
    Abc::IAsset asset( abcFileName, Abc::kThrowException );
    Abc::Box3d bounds;
    bounds.makeEmpty();

    VisitObject( asset, bounds );

    // Bounds have been formed!
    std::cout << "\nFinal Bounds: " << bounds.min << " to "
              << bounds.max << std::endl;
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        std::cerr << "Returns the bounding box of meshes contained in file.\n"
                  << "Usage: " << argv[0] << " <meshes.abc> " << std::endl;
        exit( -1 );
    }

    std::cout << "Beginning bounds calculation." << std::endl;
    boost::timer tmr;
    doIt( argv[1] );
    std::cout << "Elapsed time in seconds." << tmr.elapsed() << std::endl;
    return 0;
}
