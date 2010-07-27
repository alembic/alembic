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

#ifndef _AlembicTakoImport_MeshHelper_h_
#define _AlembicTakoImport_MeshHelper_h_

#include <AlembicTakoImport/Foundation.h>

namespace Alembic {

// return if a mesh object is created or not
bool readPoly(MFnMesh & ioMesh, MObject & iParent,
    const std::string & iName, const Tako::PolyMeshReaderPtr iNode,
    bool loadUVs = false, bool loadNormals = false);

// return if a mesh object is created or not
bool readSubD(MFnMesh & ioMesh, MObject & iParent,
    const std::string & iName, const Tako::SubDReaderPtr iNode,
    bool loadUVs = false, bool loadNormals = false);

MStatus createPoly(float iFrame, Tako::PolyMeshReaderPtr iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList, bool iSwap = false,
    bool loadUVs = false, bool loadNormals = false);

MStatus createSubD(float iFrame, Tako::SubDReaderPtr iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList, bool iSwap = false,
    bool loadUVs = false, bool loadNormals = false);

MStatus setUVs(MFnMesh & fnMesh,
    const std::vector<float> & uvs, const std::vector<int32_t> & uvIndices);

MStatus setMeshUVs(MFnMesh & fnMesh,
    const MFloatArray & uArray, const MFloatArray & vArray,
    const MIntArray & uvCounts, const MIntArray & uvIds);

} // End namespace Alembic

#endif  // _AlembicTakoImport_MeshHelper_h_
