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

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcCollection/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcMaterial/All.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace Abc  = ::Alembic::Abc;
namespace AbcF = ::Alembic::AbcCoreFactory;
namespace AbcG = ::Alembic::AbcGeom;

//-*****************************************************************************
bool is_leaf( AbcG::IObject iObj )
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
bool is_leaf( Abc::ICompoundProperty iProp, Abc::PropertyHeader iHeader ) {

    if ( !iProp.valid() ) {
        return true;
    }

    int last = iProp.getNumProperties() - 1;
    Abc::PropertyHeader header = iProp.getPropertyHeader( last );
    if ( header.getName() == iHeader.getName() )
        return true;

    return false;

}

//-*****************************************************************************
int index( Abc::ICompoundProperty iProp, Abc::PropertyHeader iHeader ) {

    for ( size_t i = 0 ; i < iProp.getNumProperties() ; i++ ) {
        Abc::PropertyHeader header = iProp.getPropertyHeader( i );
        if ( header.getName() == iHeader.getName() ) {
            return i;
        }
    }
    return -1;
}

//-*****************************************************************************
void tree( Abc::IScalarProperty iProp, std::string prefix = "" )
{
    if ( iProp.getObject().getFullName() != "/" ) {
        prefix = prefix + "   ";
    }
    if ( is_leaf( iProp.getParent(), iProp.getHeader() ) &&
         ( iProp.getObject().getNumChildren() == 0 ||
         iProp.getParent().getName() != "" )
       ) {
        std::cout << prefix << " `--";
    } else {
        std::cout << prefix << " :--";
        prefix = prefix + " :";
    }

    std::cout << iProp.getName() << "\r" << std::endl;
}

//-*****************************************************************************
void tree( Abc::IArrayProperty iProp, std::string prefix = "" )
{
    if ( iProp.getObject().getFullName() != "/" ) {
        prefix = prefix + "   ";
    }
    if ( is_leaf( iProp.getParent(), iProp.getHeader() ) &&
         ( iProp.getObject().getNumChildren() == 0 ||
         iProp.getParent().getName() != "" )
       ) {
        std::cout << prefix << " `--";
    } else {
        std::cout << prefix << " :--";
        prefix = prefix + " :";
    }

    std::cout << iProp.getName() << "\r" << std::endl;
}

//-*****************************************************************************
void tree( Abc::ICompoundProperty iProp, std::string prefix = "" )
{
    if ( iProp.getObject().getFullName() != "/" ) {
        prefix = prefix + "   ";
    }
    if ( is_leaf( iProp.getParent(), iProp.getHeader() ) &&
         iProp.getObject().getNumChildren() == 0
       ) {
        std::cout << prefix << " `--";
        prefix = prefix + " ";
    } else {
        if ( is_leaf( iProp.getParent(), iProp.getHeader() ) ) {
            std::cout << prefix << " | `--";
            prefix = prefix + " |";
        } else if ( iProp.getObject().getNumChildren() == 0  ) {
            std::cout << prefix << " :--";
            prefix = prefix + " :";
        } else if ( is_leaf( iProp, iProp.getHeader() ) ) {
            std::cout << prefix << " | `--";
            prefix = prefix + " |";
        } else {
            std::cout << prefix << " | :--";
            prefix = prefix + " | :";
        }
    }

    std::cout << iProp.getName() << "\r" << std::endl;

    for ( size_t i = 0 ; i < iProp.getNumProperties() ; i++ ) {
        Abc::PropertyHeader header = iProp.getPropertyHeader( i );
        if ( header.isScalar() ) {
            tree( Abc::IScalarProperty( iProp, header.getName() ), prefix );
        } else if ( header.isArray() ) {
            tree( Abc::IArrayProperty( iProp, header.getName() ), prefix );
        } else {
            tree( Abc::ICompoundProperty( iProp, header.getName() ), prefix );
        }
    }
}

//-*****************************************************************************
void tree( AbcG::IObject iObj, bool showProps = false, std::string prefix = "" )
{
    std::string path = iObj.getFullName();

    if ( path == "/" ) {
        prefix = "";
    }
    else {
        if ( iObj.getParent().getFullName() != "/" ) {
            prefix = prefix + "   ";
        }
        if ( is_leaf( iObj ) ) {
            std::cout << prefix << " `--";
            prefix = prefix + " ";
        } else {
            std::cout << prefix << " |--";
            prefix = prefix + " |";
        }
    };

    std::cout << iObj.getName() << "\r" << std::endl;

    // property tree
    if ( showProps ) {
        Abc::ICompoundProperty props = iObj.getProperties();
        for ( size_t i = 0 ; i < props.getNumProperties() ; i++ ) {
            Abc::PropertyHeader header = props.getPropertyHeader( i );
            if ( header.isScalar() ) {
                tree( Abc::IScalarProperty( props, header.getName() ), prefix );
            } else if ( header.isArray() ) {
                tree( Abc::IArrayProperty( props, header.getName() ), prefix );
            } else {
                tree( Abc::ICompoundProperty( props, header.getName() ), prefix );
            }
        }
    }

    // object tree
    for ( size_t i = 0 ; i < iObj.getNumChildren() ; i++ ) {
        tree( AbcG::IObject( iObj, iObj.getChildHeader(i).getName() ),
              showProps, prefix );
    };
}

//-*****************************************************************************
char* getOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool optionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    std::string filepath;
    bool metadata;
    bool properties;
    bool verbose;
    std::string desc( "abctree: <file.abc>\n"
    "  -h [ --help ]         prints this help message\n"
    "  -m [ --metadata ]     print metadata\n"
    "  -p [ --properties ]   include properties in tree\n"
    //"  -v [ --verbose ]      verbose output"
    );

    // check for min args
    if ( argc < 2 ) {
        std::cout << desc << std::endl;
        return 0;
    };

    // help
    if ( argc < 2 ||
         optionExists( argv, argv + argc, "-h" ) ||
         optionExists( argv, argv + argc, "--help" )
       ) {
        std::cout << desc << std::endl;
        return 0;
    };

    // file arg
    for(int i = 1; i < argc; i++) {
        if ( std::string(argv[i]).at(0) != '-' && std::string(argv[i-1]).at(0) != '-' ) {
            filepath = argv[i];
            break;
        };
    };

    // options
    metadata = optionExists( argv, argv + argc, "-m") ||
               optionExists( argv, argv + argc, "--metadata");
    properties = optionExists( argv, argv + argc, "-p") ||
                 optionExists( argv, argv + argc, "--properties");
    verbose = optionExists( argv, argv + argc, "-v") ||
              optionExists( argv, argv + argc, "--verbose");

    {
        Abc::IArchive archive;
        AbcF::IFactory factory;
        factory.setPolicy(Abc::ErrorHandler::kQuietNoopPolicy);
        AbcF::IFactory::CoreType coreType;
        archive = factory.getArchive(filepath, coreType);

        // metadata
        if ( metadata ) {
            std::cout  << "Using "
                       << Alembic::AbcCoreAbstract::GetLibraryVersion ()
                       << std::endl;;

            std::string appName;
            std::string libraryVersionString;
            Alembic::Util::uint32_t libraryVersion;
            std::string whenWritten;
            std::string userDescription;
            std::string coreName;
            GetArchiveInfo (archive,
                            appName,
                            libraryVersionString,
                            libraryVersion,
                            whenWritten,
                            userDescription);

            if ( coreType == AbcF::IFactory::kOgawa ) {
                coreName = "Ogawa";
            } else if ( coreType == AbcF::IFactory::kHDF5 ) {
                coreName = "HDF5";
            } else {
                coreName = "Unknown";
            };

            if (appName != "")
            {
                std::cout << "  file written by: " << appName << std::endl;
                std::cout << "  using Alembic : " << libraryVersionString << std::endl;
                std::cout << "  written on : " << whenWritten << std::endl;
                std::cout << "  user description : " << userDescription << std::endl;
            }
            else
            {
                std::cout << "  (file doesn't have any ArchiveInfo)"
                          << std::endl;
            }
            std::cout << "  core type : " << coreName << std::endl;
            std::cout << std::endl;
        };

        // walk the archive tree
        tree( archive.getTop(), properties, "" );
    }

    return 0;
}
