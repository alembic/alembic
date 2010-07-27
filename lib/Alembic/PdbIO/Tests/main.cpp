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

#include <Alembic/PdbIO/PdbIO.h>
#include <Alembic/PdbIO/Filter.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <stdio.h>
#include <stdlib.h>

namespace Alembic {
namespace PdbIO {

//-*****************************************************************************
typedef std::vector<std::string> AttrList;

//-*****************************************************************************
class ListFilter : public Filter
{
public:
    ListFilter( const AttrList &keep, const AttrList &discard )
      : Filter(),
        m_keepList( keep ),
        m_discardList( discard )
    {
        // Nothing
    }

    virtual bool keepAttribute( const std::string &nme,
                                AttributeType typ );

protected:
    AttrList m_keepList;
    AttrList m_discardList;
};

//-*****************************************************************************
bool ListFilter::keepAttribute( const std::string &nme,
                                AttributeType typ )
{
    // Try to find it in the keep list.
    // If there IS a keep list, and we're not in it, we nuke.
    if ( m_keepList.size() > 0 )
    {
        if ( std::find( m_keepList.begin(),
                        m_keepList.end(),
                        nme ) == m_keepList.end() )
        {
            return false;
        }
    }

    // Got past keep list. If we're not in the discard list, awesome!
    if ( std::find( m_discardList.begin(),
                    m_discardList.end(),
                    nme ) != m_discardList.end() )
    {
        return false;
    }

    // We are in the keep list if there is one, and we're not in the
    // discard list. Return keep!
    return true;
}

} // End namespace PdbIO
} // End namespace Alembic

namespace po = boost::program_options;

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    std::string inputFile = "UNSPECIFIED";
    std::string outputFile = "UNSPECIFIED";
    std::vector<std::string> keepList;
    std::vector<std::string> discardList;
    
    po::options_description desc( "PDB Filter options" );
    desc.add_options()

        ( "help,h", "prints this help message" )

        ( "input,i",
          po::value<std::string>( &inputFile ),
          "input PDB file to filter" )

        ( "output,o",
          po::value<std::string>( &outputFile ),
          "output PDB file to write" )

        ( "keep,k",
          po::value<std::vector<std::string> >( &keepList ),
          "attributes to keep" )

        ( "discard,d",
          po::value<std::vector<std::string> >( &discardList ),
          "attributes to discard" )

        ;

    po::variables_map vm;
    po::store( po::command_line_parser( argc, argv ).
               options( desc ).run(), vm );
    po::notify( vm );

    //-*************************************************************************
    if ( vm.count( "help" ) ||
         !vm.count( "input" ) ||
         !vm.count( "output" ) )
    {
        std::cout << desc << std::endl;
        return -1;
    }

    Alembic::PdbIO::ListFilter listFilter( keepList, discardList );
    listFilter.filter( inputFile, outputFile );
    return 0;
}
