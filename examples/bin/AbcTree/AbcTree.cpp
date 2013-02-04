//-*****************************************************************************
//
// Copyright (c) 2013,
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
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Util/All.h>

#include <iostream>
#include <sstream>

//-*****************************************************************************
using namespace ::Alembic::AbcGeom;

//-*****************************************************************************
bool is_last( IObject iObj )
{
    if ( !iObj.getParent().valid() ) {
        return true;
    }

    Abc::IObject parent = iObj.getParent();
    int numChildren = parent.getNumChildren();
    
    Abc::IObject test = parent.getChild(numChildren - 1);
    if ( test.valid() && test.getName() != iObj.getName() ) {
        return false;
    }
    return true;
}

//-*****************************************************************************
void tree( IObject iObj, std::string prefix = "" )
{
    std::string path = iObj.getFullName();

    if ( path == "/" ) {
        prefix = "";
    } 
    else 
    {
        if ( iObj.getParent().getFullName() != "/" ) {
            prefix = prefix + "   ";
        }
        if ( is_last( iObj ) ) {
            std::cout << prefix << " `--";
            prefix = prefix + " ";
        } else {
            std::cout << prefix << " |--";
            prefix = prefix + " |";
        }
    };

    std::cout << iObj.getName() << "\r" << std::endl;

    for ( size_t i = 0 ; i < iObj.getNumChildren() ; i++ )
    {
        tree( IObject( iObj, iObj.getChildHeader(i).getName() ),
              prefix );
    }
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        std::cerr << "USAGE: " << argv[0] << " <file.abc>"
                  << std::endl;
        exit( -1 );
    }
    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                          argv[1], ErrorHandler::kQuietNoopPolicy );
        std::cout << argv[1] << std::endl;
        tree( archive.getTop(), "" );
    }

    return 0;
}
