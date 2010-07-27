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

#ifndef _AlembicTakoExport_MayaMeshWriter_h_
#define _AlembicTakoExport_MayaMeshWriter_h_

#include <AlembicTakoExport/Foundation.h>
#include <AlembicTakoExport/AttributesWriter.h>
#include <AlembicTakoExport/MayaTransformWriter.h>

namespace Alembic {

// Writes an MFnMesh as a poly mesh OR a subd mesh
class MayaMeshWriter
{
  public:

    MayaMeshWriter(float iFrame, MDagPath & iDag, TransformWriterPtr iParent,
        bool iWriteVisibilty, bool iWriteUVs);
    void write(float iFrame);
    bool isAnimated() const;
    bool isSubD();
    unsigned int getNumCVs();
    unsigned int getNumFaces();

  private:

    void fillTopology(
        std::vector<float> & oPoints,
        std::vector< Tako::index_t > & oFacePoints,
        std::vector< Tako::index_t > & oFaceList);

    void writePoly(float iFrame);
    void writeSubD(float iFrame, MDagPath & iDag);

    void getUVs(std::vector<float> & uvs, std::vector<int32_t> & indices);

    void getPolyNormals(std::vector<float> & oNormals);

    typedef boost::shared_ptr < Tako::PolyMeshWriter >
        PolyMeshWriterPtr;
    typedef boost::shared_ptr < Tako::SubDWriter >
        SubDWriterPtr;

    bool    mIsGeometryAnimated;
    bool    mIsSubD;
    MDagPath mDagPath;
    //MFnMesh mMesh;
    size_t  mNumPoints;

    AttributesWriterPtr mAttrs;
    PolyMeshWriterPtr   mPolyWriter;
    SubDWriterPtr       mSubDWriter;
};

typedef boost::shared_ptr < MayaMeshWriter > MayaMeshWriterPtr;

} // End namespace Alembic

#endif  // _AlembicTakoExport_MayaMeshWriter_h_
