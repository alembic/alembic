//-*****************************************************************************
//
// Copyright (c) 2009-2019,
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

#include "MayaFaceSetWriter.h"
#include "MayaUtility.h"

MayaFaceSetWriter::MayaFaceSetWriter(MObject & iNameObj,
    std::vector<MPlug> & iPlugVec,
    Alembic::Abc::OObject & iParent,
    Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs)
{
    mPlugVec = iPlugVec;

    MFnDependencyNode fnDepNode(iNameObj);
    MString faceSetName = fnDepNode.name();
    faceSetName = util::stripNamespaces(faceSetName, iArgs.stripNamespace);
    MPlug abcFacesetNamePlug = fnDepNode.findPlug("AbcFacesetName", true);
    if (!abcFacesetNamePlug.isNull())
    {
        faceSetName = abcFacesetNamePlug.asString();
    }

    Alembic::AbcGeom::OFaceSet fs(iParent, faceSetName.asChar(), iTimeIndex);
    mSchema = fs.getSchema();
    mSchema.setFaceExclusivity(Alembic::AbcGeom::kFaceSetExclusive);

    Alembic::Abc::OCompoundProperty cp;
    Alembic::Abc::OCompoundProperty up;
    if (AttributesWriter::hasAnyAttr(fnDepNode, iArgs))
    {
        cp = mSchema.getArbGeomParams();
        up = mSchema.getUserProperties();
        mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, fs,
            fnDepNode, iTimeIndex, iArgs, true));
    }

    write();
}


void MayaFaceSetWriter::write()
{
    std::vector<Alembic::Util::int32_t> faceIndices;
    for (std::size_t i = 0; i < mPlugVec.size(); ++i)
    {
        MObject indexedObj;

        MFnComponentListData cmptList(mPlugVec[i].asMObject());

        unsigned int numPolyCmpt = 0;
        for(unsigned int i = 0; i < cmptList.length(); ++i)
        {
            if (cmptList[i].apiType() == MFn::kMeshPolygonComponent)
            {
                indexedObj = cmptList[i];
                numPolyCmpt ++;
            }
        }

        MIntArray indices;

        // retrieve the face indices
        MFnSingleIndexedComponent compFn;
        compFn.setObject(indexedObj);
        compFn.getElements(indices);
        unsigned int numData = indices.length();
        std::size_t curIndex = faceIndices.size();
        faceIndices.resize(curIndex + numData);

        for (unsigned int j = 0; j < numData; ++j)
        {
            faceIndices[j + curIndex] = indices[j];
        }
    }

    Alembic::AbcGeom::OFaceSetSchema::Sample samp;
    samp.setFaces(Alembic::Abc::Int32ArraySample(faceIndices));
    mSchema.set(samp);
}