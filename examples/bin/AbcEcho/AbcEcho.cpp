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

#include <iostream>
#include <sstream>

//-*****************************************************************************
using namespace ::Alembic::AbcGeom;

static const std::string g_sep( ";" );

//-*****************************************************************************
// FORWARD
void visitProperties( ICompoundProperty, std::string & );

//-*****************************************************************************
template <class PROP>
void visitSimpleProperty( PROP iProp, const std::string &iIndent )
{
    std::string ptype = "ScalarProperty ";
    if ( iProp.isArray() ) { ptype = "ArrayProperty "; }


    std::string mdstring = "interpretation=";
    mdstring += iProp.getMetaData().get( "interpretation" );

    std::stringstream dtype;
    dtype << "datatype=";
    dtype << iProp.getDataType();

    mdstring += g_sep;

    mdstring += dtype.str();

    std::cout << iIndent << "  " << ptype << "name=" << iProp.getName()
              << g_sep << mdstring << std::endl;
}

//-*****************************************************************************
void visitCompoundProperty( ICompoundProperty iProp, std::string &ioIndent )
{
    std::string oldIndent = ioIndent;
    ioIndent += "  ";

    std::string interp = "schema=";
    interp += iProp.getMetaData().get( "schema" );

    std::cout << ioIndent << "CompoundProperty " << "name=" << iProp.getName()
              << g_sep << interp << std::endl;

    visitProperties( iProp, ioIndent );

    ioIndent = oldIndent;
}

//-*****************************************************************************
void visitProperties( ICompoundProperty iParent,
                      std::string &ioIndent )
{
    std::string oldIndent = ioIndent;
    for ( size_t i = 0 ; i < iParent.getNumProperties() ; i++ )
    {
        PropertyHeader header = iParent.getPropertyHeader( i );

        if ( header.isCompound() )
        {
            visitCompoundProperty( ICompoundProperty( iParent,
                                                      header.getName() ),
                                   ioIndent );
        }
        else if ( header.isScalar() )
        {
            visitSimpleProperty( IScalarProperty( iParent, header.getName() ),
                                 ioIndent );
        }
        else
        {
            assert( header.isArray() );
            visitSimpleProperty( IArrayProperty( iParent, header.getName() ),
                                 ioIndent );
        }
    }

    ioIndent = oldIndent;
}

//-*****************************************************************************
void visitObject( IObject iObj,
                  std::string iIndent )
{
    // Object has a name, a full name, some meta data,
    // and then it has a compound property full of properties.
    std::string path = iObj.getFullName();

    if ( path != "/" )
    {
        std::cout << "Object " << "name=" << path << std::endl;
    }

    // Get the properties.
    ICompoundProperty props = iObj.getProperties();
    visitProperties( props, iIndent );

    // now the child objects
    for ( size_t i = 0 ; i < iObj.getNumChildren() ; i++ )
    {
        visitObject( IObject( iObj, iObj.getChildHeader( i ).getName() ),
                     iIndent );
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
        visitObject( archive.getTop(), "" );
    }

    return 0;
}
