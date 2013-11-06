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
#include <sys/stat.h>

#ifdef _MSC_VER
// set up _S_ISDIR()
#if !defined(S_ISDIR)
#  if defined( _S_IFDIR) && !defined( __S_IFDIR)
#    define __S_IFDIR _S_IFDIR
#  endif
#  define S_ISDIR(mode)    (mode&__S_IFDIR)
#endif
#endif // _MSC_VER

namespace Abc  = ::Alembic::Abc;;
namespace AbcA = ::Alembic::AbcCoreAbstract;
namespace AbcF = ::Alembic::AbcCoreFactory;
namespace AbcG = ::Alembic::AbcGeom;

#define RESETCOLOR "\033[0m"
#define GREENCOLOR "\033[1;32m"
#define BLUECOLOR "\033[1;34m"
#define BOLD "\033[1m"

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

    if ( showProps )
        std::cout << GREENCOLOR;
    std::cout << iObj.getName();
    if ( showProps )
        std::cout << RESETCOLOR;
    std::cout << "\r" << std::endl;

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
bool isFile( const std::string& filename )
{
    struct stat buf;
    if ( stat(filename.c_str(), &buf) == 0 && !S_ISDIR( buf.st_mode ) ) {
        return true;
    }
    return false;
}

//-*****************************************************************************
bool optionExists(std::vector<std::string> options, std::string option)
{
    for ( std::size_t i = 0; i < options.size(); i++ ) 
        if ( options[i].find(option) != std::string::npos )
            return true;
    return false;
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    bool opt_all = false;
    bool opt_meta = false;
    std::string desc( "abctree [OPTION] FILE[/NAME]\n"
    "  -a          include properties listings\n"
    "  -h, --help  prints this help message\n"
    "  -m          print metadata\n"
    );

    // check for min args
    if ( argc < 2 ) {
        std::cout << desc << std::endl;
        return 0;
    };

    // parse args
    std::vector<std::string> arguments(argv, argv + argc);
    std::vector<std::string> options;
    std::vector<std::string> files;

    // separate file args from option args 
    for ( std::size_t i = 1; i < arguments.size(); i++ ) {
        if ( arguments[ i ].substr( 0, 1 ) == "-" )
            options.push_back( arguments[ i ] );
        else
            files.push_back( arguments[ i ] );
    }

    // help
    if ( argc < 2 ||
         optionExists( options, "h" ) ||
         optionExists( options, "help" )
       ) {
        std::cout << desc << std::endl;
        return 0;
    };

    // set some flags
    opt_all = optionExists( options, "a");
    opt_meta = optionExists( options, "m");

    // open each file
    size_t count = 0;
    for ( std::size_t i = 0; i < files.size(); i++ ) {
        if ( files.size() > 1 )
            std::cout << BOLD << files[i] << ':' << RESETCOLOR << std::endl;

        std::stringstream ss( files[i] );
        std::stringstream fp;
        std::string segment;
        std::vector<std::string> seglist;

        /* 
         * separate file and object paths, e.g.
         *
         *   ../dir1/foo.abc/bar/baz
         *   \_____________/\______/
         *        file         obj
         */
        int j = 0;
        while ( std::getline( ss, segment, '/' ) ) {
            if ( !isFile ( fp.str() ) ) {
                if ( j != 0 )
                    fp << "/";
                fp << segment;
            } else {
                seglist.push_back( segment );
            }
            ++j;
        }

        // open the iarchive
        Abc::IArchive archive;
        AbcF::IFactory factory;
        factory.setPolicy(Abc::ErrorHandler::kQuietNoopPolicy);
        AbcF::IFactory::CoreType coreType;
        archive = factory.getArchive(std::string( fp.str() ), coreType);

        // display file metadata 
        if ( opt_meta ) {
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

            if ( appName != "" ) {
                std::cout << "  file written by: " << appName << std::endl;
                std::cout << "  using Alembic : " << libraryVersionString << std::endl;
                std::cout << "  written on : " << whenWritten << std::endl;
                std::cout << "  user description : " << userDescription << std::endl;
            } else {
                std::cout << "  (file doesn't have any ArchiveInfo)"
                          << std::endl;
            }
            std::cout << "  core type : " << coreName << std::endl;
        };

        // walk object hierarchy and find valid objects
        AbcG::IObject test = archive.getTop();
        AbcG::IObject iObj = test;
        while ( test.valid() && seglist.size() > 0 ) {
            test = test.getChild( seglist.front() );
            if ( test.valid() ) {
                iObj = test;
                seglist.erase( seglist.begin() );
            }
        }

        // walk property hierarchy for most recent valid object
        Abc::ICompoundProperty props = iObj.getProperties();
        const Abc::PropertyHeader* header;
        bool found = false;
        for ( std::size_t i = 0; i < seglist.size(); ++i ) {
            header = props.getPropertyHeader( seglist[i] );
            if ( header && header->isCompound() ) {
                Abc::ICompoundProperty ptest( props, header->getName() );
                if ( ptest.valid() ) {
                    props = ptest;
                    found = true;
                }
            } else if ( header && header->isSimple() ) {
                found = true;
            } else {
                std::cout << seglist[i] 
                          << ": Invalid object or property" 
                          << std::endl;
                return 1;
            }
        }

        // walk the archive tree
        if ( found ) 
            if ( header->isCompound() )
                tree( props );
            else
                tree( Abc::IScalarProperty( props, header->getName() ) );
        else
            tree( iObj, opt_all );

        ++count;
    }

    return 0;
}
