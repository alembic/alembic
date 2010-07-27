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

#include <AlembicTakoSPIImport/MeshHelper.h>
#include <AlembicTakoSPIImport/Util.h>
#include <AlembicTakoSPIImport/NodeIteratorVisitorHelper.h>

namespace Alembic {

namespace
{
    const bool displayEnabled = false;
    const bool wantNetworkedPlug = true;

    const std::string EMPTYSTR("");

    const std::string STSTR("st");

    // normal vector is packed different in file from the format Maya
    // accepts directly
    void setPolyNormals(MFnMesh & fnMesh, const std::vector<float> & normals)
    {
        int size = normals.size()/3;
        if (size == 0) return;

        int numFaces = fnMesh.numPolygons();
        if (size == fnMesh.numVertices())
        {
            // per vertex normal
            MVectorArray normalsIn;
            MIntArray vertexList;
            for (int i = 0, nIndex = 0; i < size; i++, nIndex += 3)
            {
                MVector normal(normals[nIndex],
                                normals[nIndex+1],
                                normals[nIndex+2]);
                normalsIn.append(normal);
                vertexList.append(i);
            }
            MStatus status = fnMesh.setVertexNormals(normalsIn, vertexList);
            if (status != MS::kSuccess)
                printError("Error setting per vertex normals");
        }
        else if (size == fnMesh.numFaceVertices())
        {
            // per vertex per-polygon normal
            int nIndex = 0;
            for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
            {
                MIntArray vertexList;
                fnMesh.getPolygonVertices(faceIndex, vertexList);
                unsigned int numVertices = vertexList.length();
                // re-pack the order of normals in this vector before writing
                // into prop so that Renderman can also use it
                for ( int v = numVertices-1; v >= 0; v-- )
                {
                    unsigned int vertexIndex = vertexList[v];
                    MVector normal(normals[nIndex],
                                normals[nIndex+1],
                                normals[nIndex+2]);
                    fnMesh.setFaceVertexNormal(normal, faceIndex, vertexIndex);
                    nIndex += 3;
                }
            }
        }
        else
        {
            printError("Normal vector is not of the correct length");
        }
    }

    bool fillTopology(MFnMesh & ioMesh, MObject & iParent,
        const std::string & iName,
        const std::vector<float> & points,
        const std::vector<TakoSPI::index_t> & faceList,
        const std::vector<TakoSPI::index_t> & facePoints,
        bool loadUVs, bool loadNormals)
    {
        unsigned int numVertices = points.size() / 3;
        MFloatPointArray vertexArray;
        vertexArray.setLength(numVertices);
        for (unsigned int i = 0; i < numVertices; ++i)
        {
            unsigned int cur = i*3;
            vertexArray.set(i,
                points[cur], points[cur+1], points[cur+2]);
        }

        // in this case we are just updating the points on ioMesh
        bool retVal = false;
        if (iName == EMPTYSTR)
        {
            ioMesh.setPoints(vertexArray, MSpace::kObject);
        }
        else
        // if we passed in a name, then we are creating the mesh from scratch
        {
            // since we are changing the topology we will be creating a new mesh
            // and this is the only time that we apply uvs/normals

            // Get face count info
            unsigned int numPolys = faceList.size()-1;
            MIntArray polyCounts;
            polyCounts.setLength(numPolys);

            unsigned int curFace = faceList[0];
            for (unsigned int i = 0; i < numPolys; ++i)
            {
                unsigned int nextFace = faceList[i+1];
                polyCounts[i] = nextFace-curFace;
                curFace = nextFace;
            }

            unsigned int numConnects = facePoints.size();

            MIntArray polyConnects;
            polyConnects.setLength(numConnects);

            unsigned int facePointIndex = 0, base = 0;
            for (unsigned int i = 0; i < numPolys; ++i)
            {
                // reverse the order of the faces
                unsigned int curNum = polyCounts[i];
                for (unsigned int j = 0; j < curNum; ++j, ++facePointIndex)
                    polyConnects[facePointIndex] = facePoints[base+curNum-j-1];

                base += curNum;
            }

            MObject shape = ioMesh.create(numVertices, numPolys, vertexArray,
                polyCounts, polyConnects, iParent);

            ioMesh.setName(iName.c_str());

            retVal = true;
        }
        return retVal;
    }
}

bool readPoly(MFnMesh & ioMesh, MObject & iParent,
    const std::string & iName,
    const TakoSPI::PolyMeshReaderPtr iNode,
    bool loadUVs, bool loadNormals)
{
    bool retVal = fillTopology(ioMesh, iParent, iName,
        iNode->getPoints(), iNode->getFaceList(), iNode->getFacePoints(),
        loadUVs, loadNormals);

    if (loadNormals)
        setPolyNormals(ioMesh, iNode->getNormals());

    return retVal;
}

bool readSubD(MFnMesh & ioMesh, MObject & iParent,
    const std::string & iName,
    const TakoSPI::SubDReaderPtr iNode,
    bool loadUVs, bool loadNormals)
{
    return fillTopology(ioMesh, iParent, iName,
        iNode->getPoints(), iNode->getFaceList(), iNode->getFacePoints(),
        loadUVs, loadNormals);
}

MStatus createPoly(float iFrame, TakoSPI::PolyMeshReaderPtr iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList, bool iSwap,
    bool loadUVs, bool loadNormals)
{
    MStatus status = MS::kSuccess;

    iNode->read(iFrame);
    MString name(iNode->getName().c_str());

    MFnMesh fnMesh;

    if ( iSwap )
    {
        fnMesh.setObject(iObject);

        unsigned int numVertices = fnMesh.numVertices();

        // simple test for change in topology
        if (iNode->getPoints().size() != numVertices*3)
        {
            MString theWarning("Processing fnMesh ");
            theWarning += name;
            theWarning += ", number of vertices changed: ";
            theWarning += "number of fnMesh vertices is ";
            theWarning += numVertices;
            theWarning +=
                MString(", number of fnMesh vertices last frame is ");
            theWarning += static_cast<unsigned int>(iNode->getPoints().size()/3);
            printWarning(theWarning);
        }

        // disconnect old connection from AlembicTakoCacheNode or some other nodes
        // to inMesh if one such connection exist
        MPlug dstPlug = fnMesh.findPlug("inMesh");
        disconnectAllPlugsTo(dstPlug);
        // disconnect connections to animated props
        iNode->readProperties(iFrame);
        TakoSPI::PropertyMap::const_iterator propIter =
            iNode->beginSampledProperties();
        while ( propIter != iNode->endSampledProperties() )
        {
            std::string propName = propIter->first;
            // SPT_xxColor is special
            if ( propName.find("SPT_") != std::string::npos
                && propName.find("Color") != std::string::npos )
            {
                std::string colorR = propName+std::string("R");
                dstPlug = fnMesh.findPlug(colorR.c_str());
                disconnectAllPlugsTo(dstPlug);
                std::string colorG = propName+std::string("G");
                dstPlug = fnMesh.findPlug(colorG.c_str());
                disconnectAllPlugsTo(dstPlug);
                std::string colorB = propName+std::string("B");
                dstPlug = fnMesh.findPlug(colorB.c_str());
                disconnectAllPlugsTo(dstPlug);
            }
            else
            {
                dstPlug = fnMesh.findPlug(propIter->first.c_str());
                disconnectAllPlugsTo(dstPlug);
            }
            propIter++;
        }

        readPoly(fnMesh, iParent, EMPTYSTR, iNode, loadUVs, loadNormals);
        addPolyProperties(iFrame, iNode, iObject, iSampledPropNameList);
        if (iNode->getNormals().size() > 0)
        {
            MPlug plug = fnMesh.findPlug("noNormals", &status);
            if (status == MS::kSuccess)
                plug.setValue(false);
        }

        if ( loadUVs )
        {
            std::vector<float> uvs;
            std::vector<TakoSPI::index_t> uvIndices;
            TakoSPI::PropertyMap::const_iterator propIter =
                iNode->beginNonSampledProperties();
            TakoSPI::PropertyPair pPair;
            while (propIter != iNode->endNonSampledProperties())
            {
                pPair = propIter->second;
                if (propIter->first == STSTR &&
                    pPair.second.inputType == TakoSPI::ARBATTR_POINT2)
                {
                    getPolyUVsVisitor attrVisitor;
                    boost::apply_visitor(attrVisitor, pPair.first);
                    uvs = attrVisitor.mFloatVec;
                    uvIndices = pPair.second.index;
                    break;
                }
                propIter++;
            }
            setUVs(fnMesh, uvs, uvIndices);
        }
    }
    else
    {
        readPoly(fnMesh, iParent, iNode->getName(),
            iNode, loadUVs, loadNormals);

        iObject = fnMesh.object();
        fnMesh.setName(name);

        setInitialShadingGroup(fnMesh.partialPathName());

        addPolyProperties(iFrame, iNode, iObject, iSampledPropNameList);
        if (iNode->getNormals().size() > 0)
        {
            MPlug plug = fnMesh.findPlug("noNormals", &status);
            if (status == MS::kSuccess)
                plug.setValue(false);
        }

        if ( loadUVs )
        {
            std::vector<float> uvs;
            std::vector<TakoSPI::index_t> uvIndices;
            TakoSPI::PropertyMap::const_iterator propIter =
                iNode->beginNonSampledProperties();
            TakoSPI::PropertyPair pPair;
            while (propIter != iNode->endNonSampledProperties())
            {
                pPair = propIter->second;
                if (propIter->first == STSTR &&
                    pPair.second.inputType == TakoSPI::ARBATTR_POINT2)
                {
                    getPolyUVsVisitor attrVisitor;
                    boost::apply_visitor(attrVisitor, pPair.first);
                    uvs = attrVisitor.mFloatVec;
                    uvIndices = pPair.second.index;
                    break;
                }
                propIter++;
            }
            setUVs(fnMesh, uvs, uvIndices);
        }
    }

    return status;
}


MStatus createSubD(float iFrame, TakoSPI::SubDReaderPtr iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList, bool iSwap,
    bool loadUVs, bool loadNormals)
{
    MStatus status = MS::kSuccess;

    iNode->read(iFrame);
    MString name(iNode->getName().c_str());

    MFnMesh fnMesh;
    std::vector<float> dummyNormals;

    if ( iSwap )
    {
        fnMesh.setObject(iObject);

        unsigned int numVertices = fnMesh.numVertices();
        // simple test for change in topology
        if (iNode->getPoints().size() != numVertices*3)
        {
            MString theWarning("Processing fnMesh ");
            theWarning += name;
            theWarning += ", number of vertices changed: ";
            theWarning += "number of fnMesh vertices is ";
            theWarning += numVertices;
            theWarning +=
                MString(", number of fnMesh vertices last frame is ");
            theWarning += static_cast<unsigned int>(iNode->getPoints().size()/3);
            printWarning(theWarning);
        }

        // disconnect old connection from AlembicTakoCacheNode or some other nodes
        // to inMesh if one such connection exist
        MPlug dstPlug = fnMesh.findPlug("inMesh");
        disconnectAllPlugsTo(dstPlug);

        // disconnect connections to animated props
        iNode->readProperties(iFrame);
        TakoSPI::PropertyMap::const_iterator propIter =
            iNode->beginSampledProperties();
        while ( propIter != iNode->endSampledProperties() )
        {
            std::string propName = propIter->first;
            // SPT_xxColor is special
            if ( propName.find("SPT_") != std::string::npos
                && propName.find("Color") != std::string::npos )
            {
                std::string colorR = propName+std::string("R");
                dstPlug = fnMesh.findPlug(colorR.c_str());
                disconnectAllPlugsTo(dstPlug);
                std::string colorG = propName+std::string("G");
                dstPlug = fnMesh.findPlug(colorG.c_str());
                disconnectAllPlugsTo(dstPlug);
                std::string colorB = propName+std::string("B");
                dstPlug = fnMesh.findPlug(colorB.c_str());
                disconnectAllPlugsTo(dstPlug);
            }
            else
            {
                dstPlug = fnMesh.findPlug(propIter->first.c_str());
                disconnectAllPlugsTo(dstPlug);
            }
            propIter++;
        }

        readSubD(fnMesh, iParent, EMPTYSTR, iNode, loadUVs, loadNormals);
        addProperties(iFrame, *iNode, iObject, iSampledPropNameList);
        if ( loadUVs )
        {
            std::vector<float> uvs;
            std::vector<TakoSPI::index_t> uvIndices;
            TakoSPI::PropertyMap::const_iterator propIter =
                iNode->beginNonSampledProperties();
            TakoSPI::PropertyPair pPair;
            while (propIter != iNode->endNonSampledProperties())
            {
                pPair = propIter->second;
                if (propIter->first == STSTR &&
                    pPair.second.inputType == TakoSPI::ARBATTR_POINT2)
                {
                    getPolyUVsVisitor attrVisitor;
                    boost::apply_visitor(attrVisitor, pPair.first);
                    uvs = attrVisitor.mFloatVec;
                    uvIndices = pPair.second.index;
                    break;
                }
                propIter++;
            }
            setUVs(fnMesh, uvs, uvIndices);
        }
    }
    else
    {
        readSubD(fnMesh, iParent, iNode->getName(),
            iNode, loadUVs, loadNormals);

        iObject = fnMesh.object();
        fnMesh.setName(name);

        setInitialShadingGroup(fnMesh.partialPathName());
        addProperties(iFrame, *iNode, iObject, iSampledPropNameList);
        if ( loadUVs )
        {
            std::vector<float> uvs;
            std::vector<TakoSPI::index_t> uvIndices;
            TakoSPI::PropertyMap::const_iterator propIter =
                iNode->beginNonSampledProperties();
            TakoSPI::PropertyPair pPair;
            while (propIter != iNode->endNonSampledProperties())
            {
                pPair = propIter->second;
                if (propIter->first == STSTR &&
                    pPair.second.inputType == TakoSPI::ARBATTR_POINT2)
                {
                    getPolyUVsVisitor attrVisitor;
                    boost::apply_visitor(attrVisitor, pPair.first);
                    uvs = attrVisitor.mFloatVec;
                    uvIndices = pPair.second.index;
                    break;
                }
                propIter++;
            }
            setUVs(fnMesh, uvs, uvIndices);
        }
    }

    // add the mFn-specific attributes to fnMesh node
    // ?? noDefaultUVs, faceVarying, facevaryingType
    MFnNumericAttribute facevaryingAttrPtr;
    MString attrName("SubDivisionMesh");
    MObject attrObj = facevaryingAttrPtr.create(attrName, attrName,
        MFnNumericData::kBoolean, 1);
    facevaryingAttrPtr.setKeyable(true);
    facevaryingAttrPtr.setHidden(false);
    fnMesh.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);

    if (iNode->getInterpolateBoundary() != -1)
    {
        attrName = MString("interpolateBoundary");
        attrObj = facevaryingAttrPtr.create(attrName, attrName,
            MFnNumericData::kBoolean,
             iNode->getInterpolateBoundary());

        facevaryingAttrPtr.setKeyable(true);
        facevaryingAttrPtr.setHidden(false);
        fnMesh.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    }

    if (iNode->getFaceVaryingInterpolateBoundary() != -1)
    {
        attrName = MString("faceVaryingInterpolateBoundary");
        attrObj = facevaryingAttrPtr.create(attrName, attrName,
            MFnNumericData::kBoolean,
            iNode->getFaceVaryingInterpolateBoundary());

        facevaryingAttrPtr.setKeyable(true);
        facevaryingAttrPtr.setHidden(false);
        fnMesh.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    }

    if (iNode->getFaceVaryingPropagateCorners() != -1)
    {
        attrName = MString("faceVaryingPropagateCorners");
        attrObj = facevaryingAttrPtr.create(attrName, attrName,
            MFnNumericData::kBoolean,
            iNode->getFaceVaryingPropagateCorners());

        facevaryingAttrPtr.setKeyable(true);
        facevaryingAttrPtr.setHidden(false);
        fnMesh.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    }

    if (!iNode->getHolePolyIndices().empty())
    {
        printWarning("Hole Poly Indices not yet supported.");
    }

    if (!iNode->getCreaseSharpness().empty())
    {
        printWarning("Creases not yet supported.");
    }

    if (!iNode->getCornerSharpness().empty())
    {
        printWarning("Corners not yet supported.");
    }

    return status;
}

MStatus setMeshUVs(MFnMesh & fnMesh,
    const MFloatArray & uArray, const MFloatArray & vArray,
    const MIntArray & uvCounts, const MIntArray & uvIds)
{
    MStatus status = MS::kSuccess;

    // Create uv set on fnMesh object
    MString uvSetName("uvset1");
    status = fnMesh.getCurrentUVSetName(uvSetName);
    if (status != MS::kSuccess)
    {
        uvSetName = MString("uvset1");
        status = fnMesh.createUVSet(uvSetName);
        status = fnMesh.setCurrentUVSetName(uvSetName);
    }
    status = fnMesh.clearUVs();
    status = fnMesh.setUVs(uArray, vArray, &uvSetName);
    status = fnMesh.assignUVs(uvCounts, uvIds);
    if (status != MS::kSuccess)
        printError("Assign UVs failed");

    return status;
}

MStatus setUVs(MFnMesh & fnMesh,
    const std::vector<float> & uvs, const std::vector<TakoSPI::index_t> & uvIndices)
{
    MStatus status;

    // Get uv values
    const int numUVs = uvs.size()/2;
    MFloatArray uArray(numUVs, 0.0);
    MFloatArray vArray(numUVs, 0.0);
    int uvIndex = 0;
    int valueIndex = 0;
    while ( uvIndex < numUVs )
    {
        uArray[uvIndex] = uvs[ valueIndex++ ];
        vArray[uvIndex] = 1.0 - uvs[ valueIndex++ ];
        uvIndex++;
    }

    const int numConnects = uvIndices.size();
    const int numFaceVertices = fnMesh.numFaceVertices();
    const int faceCount = fnMesh.numPolygons();
    if (numConnects == 0 && (numUVs == numFaceVertices))  // no indices found
    {
        MIntArray uvCounts(faceCount, 0);
        MIntArray uvIds(numFaceVertices, 0);

        // Set connectivity info
        uvIndex = 0;
        int uvCountsIndex = 0;
        unsigned int vIndex = 0;
        MItMeshPolygon meshPolygonIter(fnMesh.object());
        do
        {
            const int numPolygonVertices(
                meshPolygonIter.polygonVertexCount(&status) );
            uvCounts[uvCountsIndex++] = numPolygonVertices;

            for (int vertexIndex = 0;
                vertexIndex < numPolygonVertices; vertexIndex++)
            {
                uvIds[uvIndex] = vIndex;
                uvIndex++;
                vIndex++;
            }
        }
        while (meshPolygonIter.next() == MS::kSuccess
             && !meshPolygonIter.isDone());

        status = setMeshUVs(fnMesh, uArray, vArray, uvCounts, uvIds);
    }
    else if (numConnects == numFaceVertices)
    {
        MIntArray uvCounts(faceCount, 0);
        MIntArray uvIds(numFaceVertices, 0);

        // Set connectivity info
        uvIndex = 0;
        int uvCountsIndex = 0;
        MItMeshPolygon meshPolygonIter(fnMesh.object());
        do
        {
            const int numPolygonVertices(
                meshPolygonIter.polygonVertexCount(&status) );
            uvCounts[uvCountsIndex++] = numPolygonVertices;
            const int startPoint = uvIndex + numPolygonVertices - 1;

            for (int vertexIndex = 0;
                vertexIndex < numPolygonVertices; vertexIndex++)
            {
                uvIds[uvIndex] = uvIndices[startPoint - vertexIndex];
                uvIndex++;
            }
        }
        while (meshPolygonIter.next() == MS::kSuccess
             && !meshPolygonIter.isDone());

        status = setMeshUVs(fnMesh, uArray, vArray, uvCounts, uvIds);
    }
    else
    {
        printWarning("size of uv indices not right to be used as \
            per-polygon per-vertex uv");
    }

    return status;
}

} // End namespace Alembic


