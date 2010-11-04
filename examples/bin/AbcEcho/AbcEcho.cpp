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

// NOTE: SUPER NOT WORKING RIGHT NOW!

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <iostream>
#include <stdlib.h>
#include <vector>

//-*****************************************************************************
using namespace ::Alembic::Abc;

static std::string g_assetName;

#if 0
//-*****************************************************************************
void VisitProperty( std::ostream &ostr,
                    const std::string &indent,
                    ICompoundProperty parent,
                    const PropertyHeader &header )
{
    if ( header.isCompound() )
    {
        VisitCompoundProperty( ostr, indent,
                               ICompoundProperty( parent,
                                                  header.getName() ) );
    }
    else if ( header.isScalar() )
    {
        VisitScalarProperty( ostr, indent,
                             IScalarProperty( parent,
                                              header.getName() ) );
    }
    else
    {
        assert( header.isArray() );
        VisitArrayProperty( ostr, indent,
                            IArrayProperty( parent,
                                            header.getName() ) );
    }
}
#endif

//-*****************************************************************************
void VisitObject( std::ostream &ostr,
                  std::string &indent,
                  IObject iObj )
{
    // Object has a name, a full name, some meta data,
    // and then it has a compound property full of properties.
    ostr << indent << "Object: \"" << iObj.getName() << "\"" << std::endl;

    std::string oldIndent = indent;
    indent += "  ";

    // Write the name and meta data.
    ostr << indent << "Full Name: \"" << iObj.getFullName() << "\""
         << std::endl
         << indent << "Data Type: " << iObj.getMetaData().serialize()
         << std::endl;

    #if 0
    // Get the properties.
    ICompoundProperty props( iObj );
    size_t numProps = props.getNumProperties();
    if ( numProps > 0 )
    {
        ostr << indent << "Properties: " << std::endl;
        indent += "  ";
        for ( size_t p = 0; p < numProps; ++p )
        {
            VisitProperty( ostr, indent,
                           props, props.getPropertyHeader( p ) );
        }
    }
    #endif
}


//-*****************************************************************************
// Forwards.
std::ostream &operator<<( std::ostream &ostr, IArchive &p );
std::ostream &operator<<( std::ostream &ostr, IObject p );

//-*****************************************************************************
// Output children
template <class PARENT>
void childrenOut( std::ostream &ostr, PARENT p )
{
    const size_t numChildren = p.getNumChildren();

    if ( numChildren > 0 )
    {
        for ( size_t c = 0; c < numChildren; ++c )
        {
            ostr << g_assetName << " ";
            IObject obj( p, p.getChildHeader( c ).getName() );
            ostr << obj;
        }
    }
}

//-*****************************************************************************
std::ostream &operator<<( std::ostream &ostr, IArchive &p )
{
    childrenOut( ostr, IObject( p, kTop ) );
    return ostr;
}

//-*****************************************************************************
std::ostream &operator<<( std::ostream &ostr, IObject p )
{
    // Children
    childrenOut( ostr, p );
    return ostr;
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
        g_assetName = archive.getName();
        std::cout << archive << std::endl;
    }

    return 0;
}
