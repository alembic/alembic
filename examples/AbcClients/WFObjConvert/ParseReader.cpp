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

#include <AbcClients/WFObjConvert/ParseReader.h>
#include <AbcClients/WFObjConvert/Reader.h>

namespace AbcClients {
namespace WFObjConvert {

//-*****************************************************************************
#define FAIL( TEXT )                                    \
do                                                      \
{                                                       \
    std::stringstream sstr;                             \
    sstr << TEXT;                                       \
    ::std::runtime_error exc( sstr.str() );             \
    throw( exc );                                       \
}                                                       \
while( 0 )

//-*****************************************************************************
ParseReader::ParseReader( Reader &iReader )
  : m_readInto( iReader )
  , m_numV( 1 ) // vertex 0 is special
  , m_numVt( 1 ) // vertex 0 is special
  , m_numVn( 1 ) // vertex 0 is special
  , m_numVp( 1 ) // vertex 0 is special
{
    // Nothing
}

//-*****************************************************************************
void ParseReader::start( const std::string &iStreamName )
{
    m_readInto.parsingBegin( iStreamName );
}

//-*****************************************************************************
void ParseReader::error( const std::string &iStreamName,
                         const std::string &iError,
                         size_type iLine )
{
    m_readInto.parsingError( iStreamName, iError, ( size_t )iLine );
}

//-*****************************************************************************
void ParseReader::finish( const std::string &iStreamName,
                          size_type iNumLines )
{
    m_readInto.parsingEnd( iStreamName, ( size_t )iNumLines );
}

//-*****************************************************************************
void ParseReader::v( std::vector<double> const & vals )
{
    size_t numV = vals.size();
    index_t vIdx = ( index_t )m_numV;

    if ( numV == 3 )
    {
        m_readInto.v( vIdx, V3d( vals[0], vals[1], vals[2] ) );
    }
    else if ( numV == 4 )
    {
        m_readInto.v( vIdx, V3d( vals[0], vals[1], vals[2] ), vals[3] );
    }
    else
    {
        FAIL( "vertices must have 3 or 4 doubles" );
    }

    ++m_numV;
}

//-*****************************************************************************
void ParseReader::vt( std::vector<double> const & vals )
{
    size_t numV = vals.size();
    index_t vtIdx = ( index_t )m_numVt;

    if ( numV == 1 )
    {
        m_readInto.vt( vtIdx, vals[0] );
    }
    else if ( numV == 2 )
    {
        m_readInto.vt( vtIdx, V2d( vals[0], vals[1] ) );
    }
    else if ( numV == 3 )
    {
        m_readInto.vt( vtIdx, V3d( vals[0], vals[1], vals[2] ) );
    }
    else
    {
        FAIL( "texture vertices must have 1, 2, or 3 doubles" );
    }

    ++m_numVt;
}

//-*****************************************************************************
void ParseReader::vn( std::vector<double> const & vals )
{
    size_t numV = vals.size();
    index_t vnIdx = ( index_t )m_numVn;

    if ( numV == 3 )
    {
        m_readInto.vn( vnIdx, V3d( vals[0], vals[1], vals[2] ) );
    }
    else
    {
        FAIL( "normals must have 3 doubles" );
    }

    ++m_numVn;
}

//-*****************************************************************************
void ParseReader::vp( std::vector<double> const & vals )
{
    size_t numV = vals.size();
    index_t vpIdx = ( index_t )m_numVp;

    if ( numV == 1 )
    {
        m_readInto.vp( vpIdx, vals[0] );
    }
    else if ( numV == 2 )
    {
        m_readInto.vp( vpIdx, V2d( vals[0], vals[1] ) );
    }
    else
    {
        FAIL( "param vertices must have 1 or 2 doubles" );
    }

    ++m_numVp;
}

//-*****************************************************************************
void ParseReader::f( std::vector<V3idx> const &vals )
{
    Reader::IndexVec vIndices;
    Reader::IndexVec vtIndices;
    Reader::IndexVec vnIndices;
    
    for ( std::vector<V3idx>::const_iterator iter = vals.begin();
          iter != vals.end(); ++iter )
    {
        index_t v = ( index_t )(*iter).x;
        index_t vt = ( index_t )(*iter).y;
        index_t vn = ( index_t )(*iter).z;

        if ( v < 1 || v >= ( index_t )m_numV )
        {
            FAIL( "Invalid vertex index: " << v << ", must be 0 < v < "
                  << m_numV );
        }
        vIndices.push_back( v );

        if ( vt != -1 )
        {
            if ( vt < 1 || vt >= ( index_t )m_numVt )
            {
                FAIL( "Invalid texture vertex index: " << vt
                      << ", must be 0 < vt < " << m_numVt );
            }
            vtIndices.push_back( vt );
        }
        
        if ( vn != -1 )
        {
            if ( vn < 1 || vn >= ( index_t )m_numVn )
            {
                FAIL( "Invalid normal vertex index: " << vn
                      << ", must be 0 < vn < " << m_numVn );
            }
            vnIndices.push_back( vn );
        }
    }

    if ( vIndices.size() < 3 )
    {
        FAIL( "Insufficient number of face vertices: "
              << vIndices.size() );
    }

    if ( ( vtIndices.size() != 0 && vtIndices.size() != vIndices.size() ) ||
         ( vnIndices.size() != 0 && vnIndices.size() != vIndices.size() ) )
    {
        FAIL( "Improper mixed use of vertex/texture/normal syntax." );
    }

    m_readInto.f( vIndices, vtIndices, vnIndices );
}

//-*****************************************************************************
void ParseReader::l( std::vector<V3idx> const &vals )
{
    Reader::IndexVec vIndices;
    Reader::IndexVec vtIndices;
    Reader::IndexVec vnIndices;
    
    for ( std::vector<V3idx>::const_iterator iter = vals.begin();
          iter != vals.end(); ++iter )
    {
        index_t v = ( index_t )(*iter).x;
        index_t vt = ( index_t )(*iter).y;
        index_t vn = ( index_t )(*iter).z;

        if ( v < 1 || v >= ( index_t )m_numV )
        {
            FAIL( "Invalid vertex index: " << v << ", must be 0 < v < "
                  << m_numV );
        }
        vIndices.push_back( v );

        if ( vt != -1 )
        {
            if ( vt < 1 || vt >= ( index_t )m_numVt )
            {
                FAIL( "Invalid texture vertex index: " << vt
                      << ", must be 0 < vt < " << m_numVt );
            }
            vtIndices.push_back( vt );
        }
        
        if ( vn != -1 )
        {
            if ( vn < 1 || vn >= ( index_t )m_numVn )
            {
                FAIL( "Invalid normal vertex index: " << vn
                      << ", must be 0 < vn < " << m_numVn );
            }
            vnIndices.push_back( vn );
        }
    }

    if ( vIndices.size() < 2 )
    {
        FAIL( "Insufficient number of line vertices: "
              << vIndices.size() );
    }

    if ( ( vtIndices.size() != 0 && vtIndices.size() != vIndices.size() ) ||
         ( vnIndices.size() != 0 && vnIndices.size() != vIndices.size() ) )
    {
        FAIL( "Improper mixed use of vertex/texture/normal syntax." );
    }

    m_readInto.l( vIndices, vtIndices, vnIndices );
}

//-*****************************************************************************
void ParseReader::p( std::vector<V3idx> const &vals )
{
    Reader::IndexVec vIndices;
    Reader::IndexVec vtIndices;
    Reader::IndexVec vnIndices;
    
    for ( std::vector<V3idx>::const_iterator iter = vals.begin();
          iter != vals.end(); ++iter )
    {
        index_t v = ( index_t )(*iter).x;
        index_t vt = ( index_t )(*iter).y;
        index_t vn = ( index_t )(*iter).z;

        if ( v < 1 || v >= ( index_t )m_numV )
        {
            FAIL( "Invalid vertex index: " << v << ", must be 0 < v < "
                  << m_numV );
        }
        vIndices.push_back( v );

        if ( vt != -1 )
        {
            if ( vt < 1 || vt >= ( index_t )m_numVt )
            {
                FAIL( "Invalid texture vertex index: " << vt
                      << ", must be 0 < vt < " << m_numVt );
            }
            vtIndices.push_back( vt );
        }
        
        if ( vn != -1 )
        {
            if ( vn < 1 || vn >= ( index_t )m_numVn )
            {
                FAIL( "Invalid normal vertex index: " << vn
                      << ", must be 0 < vn < " << m_numVn );
            }
            vnIndices.push_back( vn );
        }
    }

    if ( ( vtIndices.size() != 0 && vtIndices.size() != vIndices.size() ) ||
         ( vnIndices.size() != 0 && vnIndices.size() != vIndices.size() ) )
    {
        FAIL( "Improper mixed use of vertex/texture/normal syntax." );
    }

    m_readInto.p( vIndices, vtIndices, vnIndices );
}

//-*****************************************************************************
void ParseReader::g( std::vector<std::string> const &vals )
{
    for ( std::vector<std::string>::const_iterator iter = vals.begin();
          iter != vals.end(); ++iter )
    {
        if ( m_groups.count( (*iter) ) < 1 )
        {
            m_groups.insert( (*iter) );
            m_readInto.newGroup( (*iter) );
        }
    }
    m_readInto.activeGroups( vals );
}

//-*****************************************************************************
void ParseReader::o( std::string const &val )
{
    m_readInto.activeObject( val );
}

//-*****************************************************************************
void ParseReader::mtllib( std::string const &val )
{
    m_readInto.mtllib( val );
}

//-*****************************************************************************
void ParseReader::maplib( std::string const &val )
{
    m_readInto.maplib( val );
}

//-*****************************************************************************
void ParseReader::usemtl( std::string const &val )
{
    m_readInto.usemtl( val );
}

//-*****************************************************************************
void ParseReader::usemap( std::string const &val )
{
    m_readInto.usemap( val );
}

//-*****************************************************************************
void ParseReader::trace_obj( std::string const &val )
{
    m_readInto.trace_obj( val );
}

//-*****************************************************************************
void ParseReader::shadow_obj( std::string const &val )
{
    m_readInto.shadow_obj( val );
}

//-*****************************************************************************
void ParseReader::bevel( bool b )
{
    m_readInto.bevel( b );
}

//-*****************************************************************************
void ParseReader::cinterp( bool b )
{
    m_readInto.cinterp( b );
}

//-*****************************************************************************
void ParseReader::dinterp( bool b )
{
    m_readInto.dinterp( b );
}

//-*****************************************************************************
void ParseReader::sB( bool b )
{
    m_readInto.smoothingGroup( ( int )b );
}

//-*****************************************************************************
void ParseReader::s( int s )
{
    m_readInto.smoothingGroup( s );
}

//-*****************************************************************************
void ParseReader::lod( int l )
{
    m_readInto.lod( l );
}

} // End namespace WFObjConvert
} // End namespace AbcClients
