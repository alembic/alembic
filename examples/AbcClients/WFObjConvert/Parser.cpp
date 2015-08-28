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

#include <AbcClients/WFObjConvert/ParseReader.h>
#include <AbcClients/WFObjConvert/Parser.h>

//-*****************************************************************************
// I'm breaking with the Alembic "Foundation" and "FoundationPrivate"
// conventions here because I want to limit the exposure to these files,
// as they take FOREVER to parse/compile, because of all the template-hoo-haa.

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <vector>

#include <stdlib.h>
#include <stdio.h>

namespace AbcClients {
namespace WFObjConvert {

//-*****************************************************************************
// Create a namespace to hide the parser stuff in.
// Following spirit convention, calling this 'client'
namespace client {

// Bring in all the pieces we need.
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

using qi::double_;
using qi::int_;
using qi::long_long;
using qi::phrase_parse;
using qi::_1;
using qi::_val;
using qi::lit;
using qi::eol;
using qi::eps;
using qi::lexeme;
using qi::attr;
using ascii::space;
using ascii::space_type;
using ascii::char_;
using ascii::string;
using phoenix::ref;
using phoenix::push_back;

//-*****************************************************************************
// Symbol table for 'on off'
struct onOff_ : qi::symbols<char,bool>
{
    onOff_()
    {
        add
            ( "on" , true )
            ( "On" , true )
            ( "ON" , true )
            ( "true", true )
            ( "True", true )
            ( "TRUE" , true )
            
            ( "off", false )
            ( "Off", false )
            ( "OFF", false )
            ( "false", false )
            ( "False", false )
            ( "FALSE", false )
            ;
    }
};

//-*****************************************************************************
typedef boost::fusion::vector<pindex_t, pindex_t, pindex_t> index_triplet;

//-*****************************************************************************
// Create a local parse reader that uses the boost::fusion index-triplet.
class LclParseReader : public ParseReader
{
public:
    LclParseReader( Reader &iReadInto )
      : ParseReader( iReadInto ) {}
    LclParseReader( const ParseReader &iCopy )
      : ParseReader( iCopy ) {}
    
    static V3idx toV3( index_triplet const &iVal )
    {
        return V3idx( ( pindex_t )boost::fusion::at_c<0>( iVal ),
                      ( pindex_t )boost::fusion::at_c<1>( iVal ),
                      ( pindex_t )boost::fusion::at_c<2>( iVal ) );
    }

    void f_( std::vector<index_triplet> const &vals )
    {
        std::vector<V3idx> cvt;
        for ( std::vector<index_triplet>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            cvt.push_back( toV3(*iter) );
        }
        this->f( cvt );
    }
    
    void l_( std::vector<index_triplet> const &vals )
    {
        std::vector<V3idx> cvt;
        for ( std::vector<index_triplet>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            cvt.push_back( toV3(*iter) );
        }
        this->l( cvt );
    }
    
    void p_( std::vector<index_triplet> const &vals )
    {
        std::vector<V3idx> cvt;
        for ( std::vector<index_triplet>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            cvt.push_back( toV3(*iter) );
        }
        this->p( cvt );
    }
};

//-*****************************************************************************
typedef std::string::const_iterator Iterator;

//-*****************************************************************************
// This grammar parses OBJ's different styles of index-triplets.
// template <typename Iterator>
struct indexTriplet : qi::grammar<Iterator, index_triplet(),space_type >
{
    qi::rule<Iterator, index_triplet(),space_type > start;

#define LL long_long
    
    indexTriplet() : indexTriplet::base_type( start )
    {
        //start = lexeme[( ( LL >> attr( -1 ) >> attr( -1 ) ) |
        //                 ( LL >> '/' >> LL >> '/' >> LL ) |
        //                 ( LL >> '/' >> LL >> attr( -1 ) ) |
        //                 ( LL >> '/' >> attr(-1) >> '/' >> LL ) )];
        start = lexeme[( ( LL >> '/' >> LL >> '/' >> LL ) |
                         ( LL >> '/' >> attr( -1 ) >> '/' >> LL ) |
                         ( LL >> '/' >> LL >> '/' >> attr( -1 ) ) |
                         ( LL >> attr( -1 ) >> attr( -1 ) ) )];
    }

#undef LL
};

//-*****************************************************************************
// REAL PARSE FUNCTION
void doParseOBJ( Reader &iReadInto,
                 const std::string &iName, std::istream &iStream )
{
    // Create an index-triplet parser, call it "IDX"
    //indexTriplet<Iterator> IDX;
    indexTriplet IDX;
    
    // Create a 'space-less' name parser.
    qi::rule<Iterator,std::string()> STR = lexeme[+(char_)];

    // Make an on-off symbol table
    onOff_ ON_OFF;

    // Create types of rules. the PT_RULE is one which reads point
    // declarations.
    typedef qi::rule<Iterator, std::vector<double>(), space_type> PT_RULE;

    // The ELEM_RULE is one which reads element declarations with
    // lists of index triplets.
    typedef qi::rule<Iterator, std::vector<index_triplet>(),
        space_type> ELEM_RULE;

    // The NAME RULE is one which reads a single name
    typedef qi::rule<Iterator, std::string(), space_type> NAME_RULE;

    // The NAMES_RULE is one which reads lists of names
    typedef qi::rule<Iterator, std::vector<std::string>(),
        space_type> NAMES_RULE;

    // The BOOL_RULE is one which reads 'on' or 'off' or 'true' or 'false'
    // or 'ON' or 'OFF' ... etc.
    typedef qi::rule<Iterator, bool(), space_type> BOOL_RULE;

    // The INT_RULE is one which reads a single int
    typedef qi::rule<Iterator, int(), space_type> INT_RULE;

    // The REACT macro binds the result of a successfully parsed rule
    // to a member function of the PreReader
    LclParseReader preReader( iReadInto );
#define REACT( F ) boost::bind( &LclParseReader:: F , &preReader, ::_1 )

    // Declare our rules.
    PT_RULE VERTEX = "v" >> +(double_);
    PT_RULE VTEX = "vt" >> +(double_);
    PT_RULE VNORM = "vn" >> +(double_);
    PT_RULE VPARAM = "vp" >> +(double_);
    
    ELEM_RULE FACE = "f" >> IDX >> IDX >> +(IDX);
    ELEM_RULE LINE = "l" >> IDX >> +(IDX);
    ELEM_RULE POINT = "p" >> +(IDX);

    NAMES_RULE GROUP = "g" >> +(STR);
    NAME_RULE OBJECT = "o" >> (STR);
    NAME_RULE MTLLIB = "mtllib" >> STR;
    NAME_RULE MAPLIB = "maplib" >> STR;
    NAME_RULE USEMAP = "usemap" >> STR;
    NAME_RULE USEMTL = "usemtl" >> STR;
    NAME_RULE TRACE_OBJ = "trace_obj" >> STR;
    NAME_RULE SHADOW_OBJ = "shadow_obj" >> STR;

    BOOL_RULE BEVEL = "bevel" >> ON_OFF;
    BOOL_RULE CINTERP = "cinterp" >> ON_OFF;
    BOOL_RULE DINTERP = "dinterp" >> ON_OFF;

    BOOL_RULE SMOOTHB = "s" >> ON_OFF;
    INT_RULE SMOOTHI = "s" >> int_;

    INT_RULE LOD = "lod" >> int_;

    qi::rule<Iterator,space_type> ALL =
        qi::omit[ (
                               VERTEX[REACT(v)] |
                               VTEX[REACT(vt)] |
                               VNORM[REACT(vn)] |
                               VPARAM[REACT(vp)] |
                             
                               FACE[REACT(f_)] |
                               LINE[REACT(l_)] |
                               POINT[REACT(p_)] |

                               GROUP[REACT(g)] |
                               OBJECT[REACT(o)] |
                               MTLLIB[REACT(mtllib)] |
                               MAPLIB[REACT(maplib)] |
                               USEMTL[REACT(usemtl)] |
                               USEMAP[REACT(usemap)] |
                               TRACE_OBJ[REACT(trace_obj)] |
                               SHADOW_OBJ[REACT(shadow_obj)] |

                               BEVEL[REACT(bevel)] |
                               CINTERP[REACT(cinterp)] |
                               DINTERP[REACT(dinterp)] |

                               SMOOTHB[REACT(sB)] |
                               SMOOTHI[REACT(s)] |

                               LOD[REACT(lod)] |

                               // Comments
                               ( '#' >> *(char_) )


                               ) ];


    //-*************************************************************************
    // BEGIN THE PARSING!
    //-*************************************************************************
    
    preReader.start( iName );
    
    std::string str;
    size_t lineCount = 1;
    while ( std::getline( iStream, str ) )
    {
        if ( iStream.eof() )
        {
            break;
        }
        
        if ( str.empty() )
        {
            continue;
        }

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        
        try
        {
            //bool r = client::parseObjLine( iter, end, reader );
            bool r = phrase_parse( iter, end, ALL, space );
            
            if ( !r )
            {
                std::runtime_error exc( "Syntax Error" );
                throw( exc );
            }

            ++lineCount;
        }
        catch ( std::exception &exc )
        {
            std::stringstream sstr;
            
            sstr << "ERROR: OBJ stream \"" << iName
                 << "\": " << std::endl
                 << "LINE: " << lineCount << std::endl
                 << "---> " << str << std::endl
                 << "REASON: " << exc.what() << std::endl;

            preReader.error( iName, sstr.str(), lineCount );
            return;
        }
        catch ( ... )
        {
            std::stringstream sstr;
            
            sstr << "ERROR: OBJ stream \"" << iName
                 << "\": " << std::endl
                 << "LINE: " << lineCount << std::endl
                 << "---> " << str << std::endl;

            preReader.error( iName, sstr.str(), lineCount );
            return;
        }
    }

    preReader.finish( iName, lineCount );
}

} // End namespace client

//-*****************************************************************************
// PARSE FUNCTION
void ParseOBJ( Reader &iReadInto,
               const std::string &iName, std::istream &iStream )
{
    client::doParseOBJ( iReadInto, iName, iStream );
}

//-*****************************************************************************
void ParseOBJ( Reader &iReadInto,
               const std::string &iFileName )
{
    std::ifstream fStr( iFileName.c_str() );
    
    if ( !fStr )
    {
        ParseReader reader( iReadInto );
        reader.start( iFileName );
        std::stringstream sstr;
        sstr << "ERROR: OBJ stream \"" << iFileName
             << "\": " << std::endl
             << "Couldn't open file: " << iFileName << std::endl;
        reader.error( iFileName, sstr.str(), 0 );
        return;
    }

    ParseOBJ( iReadInto, iFileName, fStr );
}

} // End namespace WFObjConvert
} // End namespace AbcClients

