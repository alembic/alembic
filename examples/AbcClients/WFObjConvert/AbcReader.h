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

#ifndef AbcClients_WFObjConvert_AbcReader_h
#define AbcClients_WFObjConvert_AbcReader_h

#include <AbcClients/WFObjConvert/Export.h>
#include <AbcClients/WFObjConvert/Foundation.h>
#include <AbcClients/WFObjConvert/Reader.h>

namespace AbcClients {
namespace WFObjConvert {

//-*****************************************************************************
// For now, just going to create vertices, indices, and counts. will
// worry about the rest later.
class ABC_WFOBJ_CONVERT_EXPORT AbcReader : public Reader
{
public:
    AbcReader( OObject &iParentObject,
               const std::string &iDefaultObjectName = "OBJ_polymesh" )
      : Reader()
      , m_parentObject( iParentObject )
      , m_defaultObjectName( iDefaultObjectName )
      , m_currentObjectName( iDefaultObjectName ) {}

    virtual void parsingEnd( const std::string &iStreamName,
                             size_t iNumLines );

    virtual void v( index_t iIndex, const V3d &iVal );

    virtual void vt( index_t iIndex, double iVal );

    virtual void vt( index_t iIndex, const V2d &iVal );

    virtual void vt( index_t iIndex, const V3d &iVal );

    virtual void vn( index_t iIndex, const V3d &iVal );

    virtual void f( const IndexVec &iVertexIndices,
                    const IndexVec &iTextureIndices,
                    const IndexVec &iNormalIndices );

    virtual void activeObject( const std::string &iObjectName );

protected:
    void makeCurrentObject();

    OObject m_parentObject;
    std::string m_defaultObjectName;

    std::string m_currentObjectName;

    std::vector<V3f> m_vertices;
    std::vector<V2f> m_texVertices;
    std::vector<N3f> m_normals;
    std::vector<Alembic::Util::int32_t> m_indices;
    std::vector<Alembic::Util::int32_t> m_texIndices;
    std::vector<Alembic::Util::int32_t> m_normIndices;
    std::vector<Alembic::Util::int32_t> m_counts;
};

} // End namespace WFObjConvert
} // End namespace AbcClients

#endif
