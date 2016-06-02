//-*****************************************************************************
//
// Copyright (c) 2009-2014,
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

#ifndef _AbcExport_MayaMeshWriter_h_
#define _AbcExport_MayaMeshWriter_h_

#include "Foundation.h"
#include "AttributesWriter.h"
#include "MayaTransformWriter.h"

#include <Alembic/AbcGeom/OPolyMesh.h>
#include <Alembic/AbcGeom/OSubD.h>

// Mechanism to cache the MFnSet::getMembers results
struct mObjectCmp
{
    bool operator()(const MObject& o1, const MObject& o2) const
    {
        return strcmp(MFnDependencyNode(o1).name().asChar(), MFnDependencyNode(o2).name().asChar()) < 0;
    }
};

typedef std::map <MObject, MSelectionList, mObjectCmp> GetMembersMap;

// Writes an MFnMesh as a poly mesh OR a subd mesh
class MayaMeshWriter
{
  public:

    MayaMeshWriter(MDagPath & iDag, Alembic::Abc::OObject & iParent,
        Alembic::Util::uint32_t iTimeIndex, const JobArgs & iArgs,
        GetMembersMap& gmMap);
    void write();
    bool isAnimated() const;
    bool isSubD();
    unsigned int getNumCVs();
    unsigned int getNumFaces();
    AttributesWriterPtr getAttrs() {return mAttrs;};

  private:

    void fillTopology(
        std::vector<float> & oPoints,
        std::vector<Alembic::Util::int32_t> & oFacePoints,
        std::vector<Alembic::Util::int32_t> & oPointCounts);

    void writePoly(const Alembic::AbcGeom::OV2fGeomParam::Sample & iUVs);

    void writeSubD(const Alembic::AbcGeom::OV2fGeomParam::Sample & iUVs);

    void getUVs(std::vector<float> & uvs,
        std::vector<Alembic::Util::uint32_t> & indices,
        std::string & name);

    void getPolyNormals(std::vector<float> & oNormals);
    bool mNoNormals;
    bool mWriteGeometry;
    bool mWriteUVs;
    bool mWriteColorSets;
    bool mWriteUVSets;

    bool mIsGeometryAnimated;
    MDagPath mDagPath;

    AttributesWriterPtr mAttrs;
    Alembic::AbcGeom::OPolyMeshSchema mPolySchema;
    Alembic::AbcGeom::OSubDSchema     mSubDSchema;

    void writeColor();
    std::vector<Alembic::AbcGeom::OC3fGeomParam> mRGBParams;
    std::vector<Alembic::AbcGeom::OC4fGeomParam> mRGBAParams;

    void writeUVSets();
    typedef std::vector<Alembic::AbcGeom::OV2fGeomParam> UVParamsVec;
    UVParamsVec mUVparams;
};

#endif  // _AbcExport_MayaMeshWriter_h_
