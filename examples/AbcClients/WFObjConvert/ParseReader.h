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

#ifndef AbcClients_WFObjConvert_ParseReader_h
#define AbcClients_WFObjConvert_ParseReader_h

//-*****************************************************************************
// Breaking my rules here about Foundation.h, to minimize recompiling
// of the parser, which takes FOREVER. Stupid templates.
#include <ImathVec.h>
#include <vector>
#include <string>
#include <set>
#include <sys/types.h>

namespace AbcClients {
namespace WFObjConvert {

//-*****************************************************************************
// This is a helper class used by the parser, that redirects input into
// the virtual reader class.
class Reader;
typedef long long pindex_t;

//-*****************************************************************************
class ParseReader
{
public:
    typedef Imath::Vec3<pindex_t> V3idx;
    typedef std::vector<double>::size_type size_type;

    ParseReader( Reader &iReadInto );

    void start( const std::string &iStringName );
    void error( const std::string &iStringName,
                const std::string &iError,
                size_type iLine );
    void finish( const std::string &iStringName,
                 size_type iNumLines );

    void v( std::vector<double> const & vals );
    void vt( std::vector<double> const & vals );
    void vn( std::vector<double> const & vals );
    void vp( std::vector<double> const & vals );

    void f( std::vector<V3idx> const &vals );
    void l( std::vector<V3idx> const &vals );
    void p( std::vector<V3idx> const &vals );

    void g( std::vector<std::string> const &vals );
    void o( std::string const &val );

    void mtllib( std::string const &val );
    void maplib( std::string const &val );
    void usemtl( std::string const &val );
    void usemap( std::string const &val );

    void trace_obj( std::string const &val );
    void shadow_obj( std::string const &val );

    void bevel( bool b );
    void cinterp( bool b );
    void dinterp( bool b );

    void sB( bool b );
    void s( int s );

    void lod( int l );

protected:
    Reader &m_readInto;

    size_type m_numV;
    size_type m_numVt;
    size_type m_numVn;
    size_type m_numVp;

    std::set<std::string> m_groups;

};

} // End namespace WFObjConvert
} // End namespace AbcClients

#endif
