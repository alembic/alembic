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

#ifndef _Alembic_TakoAbstract_GenMeshReader_h_
#define _Alembic_TakoAbstract_GenMeshReader_h_

#include <Alembic/TakoAbstract/Foundation.h>
#include <Alembic/TakoAbstract/HDFReaderNode.h>

namespace Alembic {

namespace TakoAbstract {

namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
// The GenMesh is a generic base class for SubDMesh and PolyMesh, which allows
// client APIs to treat them both homogenously. This is useful for sim code
// or drawing code.
class GenMeshReader : public HDFReaderNode
{
public:
    // For PolyMesh, creases, corners & holepoly is ignored.
    enum {
        
        GEOMETRY_STATIC       = 0x0000,
        GEOMETRY_HOMOGENOUS   = 0x0001,
        GEOMETRY_HETEROGENOUS = 0x0002,

        // Aliases of above
        TOPOLOGY_STATIC       = 0x0000,
        TOPOLOGY_HOMOGENOUS   = 0x0001,
        TOPOLOGY_HETEROGENOUS = 0x0002,

        CREASES_STATIC        = 0x0010,
        CREASES_HOMOGENOUS    = 0x0020,
        CREASES_HETEROGENOUS  = 0x0030,

        CORNERS_STATIC        = 0x0100,
        CORNERS_HOMOGENOUS    = 0x0200,
        CORNERS_HETEROGENOUS  = 0x0300,

        HOLEPOLY_STATIC       = 0x1000,
        HOLEPOLY_HETEROGENOUS = 0x3000,
        
        READ_ERROR            = 0x80000000
    };

    virtual ~GenMeshReader();
    
    virtual unsigned int read( float iFrame ) = 0;

    virtual const std::vector<float> & getPoints() = 0;
    virtual const std::vector<index_t> & getFacePoints() = 0;
    virtual const std::vector<index_t> & getFaceList() = 0;

    // Normals are optional and don't exist on SubDs.
    // This will just return an empty VectorPtr if they don't
    // exist.
    virtual const std::vector<float> & getNormals() = 0;
    virtual bool hasNormals() const = 0;

    // Also, just answer whether you're a subd.
    virtual bool isSubD() const = 0;
};

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS

} // End namespace TakoAbstract

} // End namespace Alembic

#endif  // _Alembic_TakoAbstract_GenMeshReader_h_
