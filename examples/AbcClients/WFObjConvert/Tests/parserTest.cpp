//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <ImathVec.h>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <string>

namespace client {

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
// Define a 'parser index', which is a long-long.
typedef long long pindex_t;

typedef Imath::Vec3<pindex_t> V3idx;

//-*****************************************************************************
typedef boost::fusion::vector<pindex_t, pindex_t, pindex_t> index_triplet;

inline V3idx toV3( index_triplet const &iVal )
{
    return V3idx( boost::fusion::at_c<0>( iVal ),
                  boost::fusion::at_c<1>( iVal ),
                  boost::fusion::at_c<2>( iVal ) );
}

//-*****************************************************************************
struct PreReader
{
    void v( std::vector<double> const & vals )
    {
        std::cout << "FOUND v ";
        for ( std::vector<double>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            std::cout << (*iter) << " ";
        }
        std::cout << std::endl;
    }
    void vt( std::vector<double> const & vals )
    {
        std::cout << "FOUND vt ";
        for ( std::vector<double>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            std::cout << (*iter) << " ";
        }
        std::cout << std::endl;
    }
    void vn( std::vector<double> const & vals )
    {
        std::cout << "FOUND vn ";
        for ( std::vector<double>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            std::cout << (*iter) << " ";
        }
        std::cout << std::endl;
    }
    void vp( std::vector<double> const & vals )
    {
        std::cout << "FOUND vp ";
        for ( std::vector<double>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            std::cout << (*iter) << " ";
        }
        std::cout << std::endl;
    }

    void f( std::vector<index_triplet> const &vals )
    {
        std::cout << "FOUND f ";
        for ( std::vector<index_triplet>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            std::cout << toV3(*iter) << " ";
        }
        std::cout << std::endl;
    }
    
    void l( std::vector<index_triplet> const &vals )
    {
        std::cout << "FOUND l, size = " << vals.size() << std::endl;
    }
    
    void p( std::vector<index_triplet> const &vals )
    {
        std::cout << "FOUND p, size = " << vals.size() << std::endl;
    }


    void g( std::vector<std::string> const &vals )
    {
        std::cout << "FOUND g ";
        for ( std::vector<std::string>::const_iterator iter = vals.begin();
              iter != vals.end(); ++iter )
        {
            std::cout << (*iter) << " ";
        }
        std::cout << std::endl;
    }

    void o( std::string const &val )
    {
        std::cout << "FOUND o " << val << std::endl;
    }
    
    void mtllib( std::string const &val )
    {
        std::cout << "FOUND mtllib " << val << std::endl;
    }
    
    void maplib( std::string const &val )
    {
        std::cout << "FOUND maplib " << val << std::endl;
    }
    
    void usemtl( std::string const &val )
    {
        std::cout << "FOUND usemtl " << val << std::endl;
    }
    
    void usemap( std::string const &val )
    {
        std::cout << "FOUND usemap " << val << std::endl;
    }
    
    void trace_obj( std::string const &val )
    {
        std::cout << "FOUND trace_obj " << val << std::endl;
    }
    
    void shadow_obj( std::string const &val )
    {
        std::cout << "FOUND shadow)obj " << val << std::endl;
    }

    void bevel( bool b )
    {
        std::cout << "BEVEL: " << b << std::endl;
    }

    void cinterp( bool b )
    {
        std::cout << "CINTERP: " << b << std::endl;
    }

    void dinterp( bool b )
    {
        std::cout << "DINTERP: " << b << std::endl;
    }

    void sB( bool b )
    {
        std::cout << "smooth: " << b << std::endl;
    }

    void s( int s )
    {
        std::cout << "smooth: " << s << std::endl;
    }

    void lod( int l )
    {
        std::cout << "lod: " << l << std::endl;
    }

};


//-*****************************************************************************
template <typename Iterator>
struct indexTriplet : qi::grammar<Iterator, index_triplet(),space_type >
{
    qi::rule<Iterator, index_triplet(),space_type > start;

#define LL long_long
    
    indexTriplet() : indexTriplet::base_type( start )
    {
        start = lexeme[( ( LL >> '/' >> LL >> '/' >> LL ) |
                         ( LL >> '/' >> LL >> attr( -1 ) ) |
                         ( LL >> '/' >> attr(-1) >> '/' >> LL ) )];
    }

#undef LL
};

//-*****************************************************************************
// This parses one line of an OBJ file, accumulating results into the reader.
template <typename Iterator>
bool parseObjLine( Iterator iFirst, Iterator iLast, PreReader& iPreReader )
{
    // Create an index-triplet parser, call it "IDX"
    indexTriplet<Iterator> IDX;
    
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
#define REACT( F ) boost::bind( &PreReader:: F , &iPreReader, ::_1 )

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

    // Parse the rules!
    bool r = phrase_parse( iFirst, iLast,
                           (
                               VERTEX[REACT(v)] |
                               VTEX[REACT(vt)] |
                               VNORM[REACT(vn)] |
                               VPARAM[REACT(vp)] |
                             
                               FACE[REACT(f)] |
                               LINE[REACT(l)] |
                               POINT[REACT(p)] |

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


                               ),
                           space );
    if ( !r || iFirst != iLast )
    {
        return false;
    }
    
    return true;
}

} // End namespace client

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tRoman Numerals Parser\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type a Roman Numeral ...or [q or Q] to quit\n\n";

    typedef std::string::const_iterator iterator_type;

    std::string str;
    while ( std::getline( std::cin, str ) )
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        
        client::PreReader reader;
        bool r = client::parseObjLine(iter, end, reader);

        if ( r )
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "-------------------------\n";
        }
        else
        {
            std::cout << "Parsing failed, man." << std::endl;
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}


