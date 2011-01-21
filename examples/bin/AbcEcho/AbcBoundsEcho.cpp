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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <ImathBoxAlgo.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <iostream>

//-*****************************************************************************
using namespace ::Alembic::AbcGeom;

typedef std::vector<std::string> StringVec;

namespace ba = boost::algorithm;

//-*****************************************************************************
void accumXform( M44d &xf, IObject obj )
{
    if ( ISimpleXform::matches( obj.getMetaData() ) )
    {
        ISimpleXform x( obj, kWrapExisting );
        xf = x.getSchema().getValue().getMatrix() * xf;
    }
    else if ( IXform::matches( obj.getMetaData() ) )
    {
        IXform x( obj, kWrapExisting );
        xf = x.getSchema().getMatrix() * xf;
    }
}

//-*****************************************************************************
M44d getFinalMatrix( IObject iObj )
{
    M44d xf;
    xf.makeIdentity();

    StringVec pvec;
    ba::split( pvec, iObj.getFullName(), ba::is_any_of( "/" ) );

    if ( pvec.size() > 0 && pvec.front() == "" )
    {
        pvec.erase( pvec.begin() );
    }

    IObject child;
    IObject parent = iObj.getArchive().getTop();

    for ( size_t i = 0 ; i < pvec.size() ; ++i )
    {
        child = IObject( parent, pvec[i] );
        accumXform( xf, child );
        parent = child;
    }

    return xf;
}

//-*****************************************************************************
Box3d getBounds( IObject iObj )
{
    Box3d bnds;
    bnds.makeEmpty();

    M44d xf = getFinalMatrix( iObj );

    if ( IPolyMesh::matches( iObj.getMetaData() ) )
    {
        IPolyMesh mesh( iObj, kWrapExisting );
        IPolyMeshSchema ms = mesh.getSchema();
        V3fArraySamplePtr positions = ms.getValue().getPositions();
        size_t numPoints = positions->size();

        for ( size_t i = 0 ; i < numPoints ; ++i )
        {
            bnds.extendBy( (*positions)[i] );
        }
    }
    else if ( ISubD::matches( iObj.getMetaData() ) )
    {
        ISubD mesh( iObj, kWrapExisting );
        ISubDSchema ms = mesh.getSchema();
        V3fArraySamplePtr positions = ms.getValue().getPositions();
        size_t numPoints = positions->size();

        for ( size_t i = 0 ; i < numPoints ; ++i )
        {
            bnds.extendBy( (*positions)[i] );
        }
    }

    bnds.extendBy( Imath::transform( bnds, xf ) );

    return bnds;
}

//-*****************************************************************************
void visitObject( IObject iObj )
{
    std::string path = iObj.getFullName();

    const MetaData &md = iObj.getMetaData();

    if ( IPolyMeshSchema::matches( md ) || ISubDSchema::matches( md ) )
    {
        Box3d bnds = getBounds( iObj );
        std::cout << path << " " << bnds.min << " " << bnds.max << std::endl;
    }

    // now the child objects
    for ( size_t i = 0 ; i < iObj.getNumChildren() ; i++ )
    {
        visitObject( IObject( iObj, iObj.getChildHeader( i ).getName() ) );
    }
}

//-*****************************************************************************
//-*****************************************************************************
// DO IT.
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        std::cerr << "USAGE: " << argv[0] << " <AlembicArchive.abc>"
                  << std::endl;
        exit( -1 );
    }

    // Scoped.
    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                          argv[1], ErrorHandler::kQuietNoopPolicy );
        visitObject( archive.getTop() );
    }

    return 0;
}
