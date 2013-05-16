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

namespace Abc  = ::Alembic::Abc;
namespace AbcA = ::Alembic::AbcCoreAbstract;
namespace AbcF = ::Alembic::AbcCoreFactory;
namespace AbcG = ::Alembic::AbcGeom;

#define RESETCOLOR "\033[0m"
#define GREENCOLOR "\033[1;32m"
#define BLUECOLOR "\033[1;34m"
#define CYANCOLOR "\033[1;36m"
#define BOLD "\033[1m"
#define COL_1 20
#define COL_2 15

//-*****************************************************************************
void printParent( Abc::ICompoundProperty iProp, 
                  bool all = false, 
                  bool long_list = false, 
                  bool recursive = false,
                  bool first = false ) {
    std::cout << CYANCOLOR
              << iProp.getObject().getFullName() << "/" 
              << iProp.getName() << ":"
              << RESETCOLOR
              << std::endl;
}
//-*****************************************************************************
void printParent( AbcG::IObject iObj,
                  bool all = false, 
                  bool long_list = false, 
                  bool recursive = false,
                  bool first = false ) {
    if ( !first && !long_list )
        std::cout << std::endl;
    std::cout << CYANCOLOR
              << iObj.getFullName() << ":"
              << RESETCOLOR
              << std::endl;
}

//-*****************************************************************************
void printChild( Abc::ICompoundProperty iParent, Abc::PropertyHeader header, 
                 bool all = false, bool long_list = false ) {
   
    std::string ptype = "CompoundProperty"; 
    if ( long_list ) {
        if ( header.isScalar() )
            ptype = "ScalarProperty";
        else if (header.isArray() )
            ptype = "ArrayProperty";
        std::stringstream ss;
        ss << header.getDataType();
        std::cout << ptype
                  << std::string(COL_1 - ptype.length(), ' ')
                  << ss.str()
                  << std::string(COL_2 - ss.str().length(), ' ');
    }
    std::cout << BLUECOLOR << header.getName();
    if ( long_list ) {
        if ( header.isScalar() || header.isArray() ) {
            std::cout << "[";
            if ( header.isScalar() ) {
                Abc::IScalarProperty iProp( iParent, header.getName() );
                std::cout << iProp.getNumSamples();
            } else if ( header.isArray() ) {
                Abc::IArrayProperty iProp( iParent, header.getName() );
                std::cout << iProp.getNumSamples();
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    else
        std::cout << "   ";

    std::cout << RESETCOLOR;
}

//-*****************************************************************************
void printChild( AbcG::IObject iParent, AbcG::IObject iObj, 
                 bool all = false, bool long_list = false ) {
    
    if ( long_list ) {
        std::string schema = iObj.getMetaData().get( "schema" );
        int spacing = COL_1;
        if ( all )
            spacing = COL_1 + COL_2;
        std::cout << schema << std::string(spacing - schema.length(), ' ');
    }
    std::cout << GREENCOLOR << iObj.getName() << RESETCOLOR << "   ";
    if ( long_list )
        std::cout << std::endl;
}

//-*****************************************************************************
void visit( Abc::ICompoundProperty iProp, 
            bool all = false, 
            bool long_list = false, 
            bool meta = false,
            bool recursive = false,
            bool first = false )
{
    // header
    if ( recursive && iProp.getNumProperties() > 0 ) {
        printParent( iProp, all, long_list, recursive, first );
    }

    // children
    for( size_t i = 0; i < iProp.getNumProperties(); ++i ) {
        printChild( iProp, iProp.getPropertyHeader( i ), all, long_list );
    }

    // visit children
    if ( recursive && all && iProp.getNumProperties() > 0 ) {
        for( size_t i = 0; i < iProp.getNumProperties(); ++i ) {
            Abc::PropertyHeader header = iProp.getPropertyHeader( i );
            if ( header.isCompound() )
                visit( Abc::ICompoundProperty( iProp, header.getName() ), 
                       all, long_list, meta, recursive, false );
        }
    }
}

//-*****************************************************************************
void visit( AbcG::IObject iObj, 
            bool all = false, 
            bool long_list = false, 
            bool meta = false,
            bool recursive = false,
            bool first = false )

{
    Abc::ICompoundProperty props = iObj.getProperties();
    
    // header
    if ( recursive && 
       ( iObj.getNumChildren() > 0 || 
       ( all && props.getNumProperties() > 0 ) ) ) {
        printParent( iObj, all, long_list, recursive, first);
    }

    // children
    for( size_t i = 0; i < iObj.getNumChildren(); ++i ) {
        printChild( iObj, iObj.getChild( i ), all, long_list );
    }

    // properties
    if ( all ) {
        for( size_t i = 0; i < props.getNumProperties(); ++i ) {
            printChild( props, props.getPropertyHeader( i ), all, long_list );
        }
    }

    // visit property children
    if ( recursive && all && props.getNumProperties() > 0 ) {
        for( size_t i = 0; i < props.getNumProperties(); ++i ) {
            Abc::PropertyHeader header = props.getPropertyHeader( i );
            if ( header.isCompound() ) {
                if ( !long_list )
                    std::cout << std::endl;
                visit( Abc::ICompoundProperty( props, header.getName() ), 
                       all, long_list, meta, recursive, false );
            }
        }
    }

    // visit object children
    if ( recursive && iObj.getNumChildren() > 0 ) {
        for( size_t i = 0; i < iObj.getNumChildren(); ++i ) {
            visit( iObj.getChild( i ), all, long_list, meta, recursive, false );
        }
    }
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
    for ( int i = 0; i < options.size(); i++ ) 
        if ( options[i].find(option) != std::string::npos )
            return true;
    return false;
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    std::vector<std::string> filepaths;
    std::vector<std::string>::iterator it;
    float opt_fps = 24.0;
    bool opt_all = false;
    bool opt_long = false;
    bool opt_meta = false;
    bool opt_recursive = false;
    std::string desc( "abcls [OPTION] FILE[/NAME] \n"
    "  -a          include property listings\n"
    //"  --fps[=FPS] show values in frames per second\n"
    "  -h, --help  show this help message\n"
    "  -l          long listing format\n"
    "  -m          show archive metadata\n"
    "  -r          list entries recursively\n"
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
    for ( int i = 1; i < arguments.size(); i++ ) {
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
    opt_long = optionExists( options, "l");
    opt_meta = optionExists( options, "m");
    opt_recursive = optionExists( options, "r");

    // open each file
    size_t count = 0;
    for ( int i = 0; i < files.size(); i++ ) {
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
        int i = 0;
        while ( std::getline( ss, segment, '/' ) ) {
            if ( !isFile ( fp.str() ) ) {
                if ( i != 0 )
                    fp << "/";
                fp << segment;
            } else {
                seglist.push_back( segment );
            }
            ++i;
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
        for ( int i = 0; i < seglist.size(); ++i ) {
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

        // do stuff
        if ( found && header->isCompound() ) {
            visit( props, opt_all, opt_long, opt_meta, opt_recursive, true );
        } else if ( found && header->isSimple() ) {
            printChild( props, *header, opt_all, opt_long );
        } else
            visit( iObj, opt_all, opt_long, opt_meta, opt_recursive, true );
        
        ++count;

        if ( !opt_long )
            std::cout << std::endl;
    };
    
    return 0;
}
