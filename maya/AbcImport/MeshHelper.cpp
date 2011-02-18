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

#include <maya/MString.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MVector.h>
#include <maya/MDagModifier.h>

#include "util.h"
#include "MeshHelper.h"
#include "NodeIteratorVisitorHelper.h"


namespace
{

    MStatus setMeshUVs(MFnMesh & ioMesh,
        const MFloatArray & uArray, const MFloatArray & vArray,
        const MIntArray & uvCounts, const MIntArray & uvIds)
    {
        MStatus status = MS::kSuccess;

        // Create uv set on ioMesh object
        MString uvSetName("uvset1");
        status = ioMesh.getCurrentUVSetName(uvSetName);
        if ( status != MS::kSuccess )
        {
            uvSetName = MString("uvset1");
            status = ioMesh.createUVSet(uvSetName);
            status = ioMesh.setCurrentUVSetName(uvSetName);
        }
        status = ioMesh.clearUVs();
        status = ioMesh.setUVs(uArray, vArray, &uvSetName);
        status = ioMesh.assignUVs(uvCounts, uvIds);
        if (status != MS::kSuccess)
            printError("Assign UVs failed");

        return status;
    }  // setMeshUVs


    void setUVs(double iFrame, MFnMesh & ioMesh,
        Alembic::Abc::IObject & iParent)
    {
        // this will need to change once UVs are part of the schema
        Alembic::Abc::IV3fArrayProperty stValProp(iParent.getProperties(),
            "st", Alembic::Abc::ErrorHandler::kQuietNoopPolicy);
        Alembic::Abc::IInt32ArrayProperty stIndexProp(iParent.getProperties(),
            "st.index", Alembic::Abc::ErrorHandler::kQuietNoopPolicy);

        if (!stValProp.valid())
            return;

        Alembic::Abc::V3fArraySamplePtr stValPtr;
        int64_t zero = 0;
        stValProp.get(stValPtr, Alembic::Abc::ISampleSelector(zero));

        Alembic::Abc::Int32ArraySamplePtr stIndexPtr;
        if (stIndexProp.valid())
            stIndexProp.get(stIndexPtr, Alembic::Abc::ISampleSelector(zero));

        // Get uv values
        size_t numUVs = stValPtr->size();
        MFloatArray uArray(numUVs, 0.0);
        MFloatArray vArray(numUVs, 0.0);
        int uvIndex = 0;
        int valueIndex = 0;

        for (size_t i = 0; i < numUVs; ++i)
        {
            uArray[i] = (*stValPtr)[i].x;
            vArray[i] = 1.0 - (*stValPtr)[i].y;
        }

        const int numFaceVertices = ioMesh.numFaceVertices();
        const int faceCount = ioMesh.numPolygons();

        // no indices found
        if ((stIndexPtr != NULL) && (numUVs == numFaceVertices))
        {
            MIntArray uvCounts(faceCount, 0);
            MIntArray uvIds(numFaceVertices, 0);

            // Set connectivity info
            uvIndex = 0;
            int uvCountsIndex = 0;
            unsigned int vIndex = 0;
            MItMeshPolygon meshPolygonIter(ioMesh.object());
            do
            {
                const int numPolygonVertices(
                    meshPolygonIter.polygonVertexCount() );
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

            setMeshUVs(ioMesh, uArray, vArray, uvCounts, uvIds);
        }
        else if (stIndexPtr->size() == numFaceVertices)
        {
            MIntArray uvCounts(faceCount, 0);
            MIntArray uvIds(numFaceVertices, 0);

            // Set connectivity info
            uvIndex = 0;
            int uvCountsIndex = 0;
            MItMeshPolygon meshPolygonIter(ioMesh.object());
            do
            {
                int numPolygonVertices(meshPolygonIter.polygonVertexCount());
                uvCounts[uvCountsIndex++] = numPolygonVertices;
                int startPoint = uvIndex + numPolygonVertices - 1;

                for (int vertexIndex = 0;
                    vertexIndex < numPolygonVertices; vertexIndex++)
                {
                    uvIds[uvIndex] = (*stIndexPtr)[startPoint - vertexIndex];
                    uvIndex++;
                }
            }
            while (meshPolygonIter.next() == MS::kSuccess
                && !meshPolygonIter.isDone());

            setMeshUVs(ioMesh, uArray, vArray, uvCounts, uvIds);
        }
        else
        {
            printWarning(
                "UV indices can't be used as per-polygon per-vertex uv");
        }
    }  // setUVs

    // utility to clear pt when doing a swap otherwise
    // the new swap position could get messed up
    void clearPt(MFnMesh & ioMesh)
    {
        MPlug ptPlug = ioMesh.findPlug("pt");
        unsigned int numElements = ptPlug.numElements();
        if (ptPlug.isArray() && (numElements > 0))
        {
            for (unsigned int i = 0; i < numElements; i++)
            {
                MPlug elementPlug = ptPlug[i];
                MPlug childx = elementPlug.child(0);
                childx.setValue(0.0);
                MPlug childy = elementPlug.child(1);
                childy.setValue(0.0);
                MPlug childz = elementPlug.child(2);
                childz.setValue(0.0);
            }
        }
    }

    // normal vector is packed differently in file
    // from the format Maya accepts directly
    void setPolyNormals(MFnMesh & ioMesh, std::vector<float> & normals)
    {
        int size = normals.size()/3;
        if (size == 0) return;

        int numFaces = ioMesh.numPolygons();
        if (size == ioMesh.numVertices())
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
            MStatus status = ioMesh.setVertexNormals(normalsIn, vertexList);
            if (status != MS::kSuccess)
                printError("Error setting per vertex normals");
        }
        else if (size == ioMesh.numFaceVertices())
        {
            // per vertex per-polygon normal
            int nIndex = 0;
            for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
            {
                MIntArray vertexList;
                ioMesh.getPolygonVertices(faceIndex, vertexList);
                unsigned int numVertices = vertexList.length();
                // re-pack the order of normals in this vector before writing
                // into prop so that Renderman can also use it
                for ( int v = numVertices-1; v >= 0; v-- )
                {
                    unsigned int vertexIndex = vertexList[v];
                    MVector normal(normals[nIndex],
                                normals[nIndex+1],
                                normals[nIndex+2]);
                    ioMesh.setFaceVertexNormal(normal, faceIndex, vertexIndex);
                    nIndex += 3;
                }
            }
        }
        else
        {
            printError("Normal vector is not of the correct length");
        }
    }

    void fillPoints(MFloatPointArray & oPointArray,
        Alembic::Abc::V3fArraySamplePtr iPoints,
        Alembic::Abc::V3fArraySamplePtr iCeilPoints, double alpha)
    {
        size_t numPoints = iPoints->size();
        oPointArray.setLength(numPoints);

        if (alpha == 0 || iCeilPoints == NULL)
        {
            for (size_t i = 0; i < numPoints; ++i)
            {
                oPointArray.set(i,
                    (*iPoints)[i].x, (*iPoints)[i].y, (*iPoints)[i].z);
            }
        }
        else
        {
            for (size_t i = 0; i < numPoints; ++i)
            {
                oPointArray.set(i,
                    simpleLerp<float>(alpha,
                        (*iPoints)[i].x, (*iCeilPoints)[i].x),
                    simpleLerp<float>(alpha,
                        (*iPoints)[i].y, (*iCeilPoints)[i].y),
                    simpleLerp<float>(alpha,
                        (*iPoints)[i].z, (*iCeilPoints)[i].z));
            }
        }

    }

    void fillTopology(MFnMesh & ioMesh, MObject & iParent,
        const std::string & iName,
        MFloatPointArray & iPoints,
        Alembic::Abc::Int32ArraySamplePtr iIndices,
        Alembic::Abc::Int32ArraySamplePtr iCounts)
    {
        // since we are changing the topology we will be creating a new mesh

        // Get face count info
        size_t numPolys = iCounts->size();
        MIntArray polyCounts;
        polyCounts.setLength(numPolys);

        for (size_t i = 0; i < numPolys; ++i)
        {
            polyCounts[i] = (*iCounts)[i];
        }

        size_t numConnects = iIndices->size();

        MIntArray polyConnects;
        polyConnects.setLength(numConnects);

        size_t facePointIndex = 0, base = 0;
        for (unsigned int i = 0; i < numPolys; ++i)
        {
            // reverse the order of the faces
            size_t curNum = polyCounts[i];
            for (size_t j = 0; j < curNum; ++j, ++facePointIndex)
                polyConnects[facePointIndex] = (*iIndices)[base+curNum-j-1];

            base += curNum;
        }

        MObject shape = ioMesh.create(iPoints.length(), numPolys, iPoints,
            polyCounts, polyConnects, iParent);

        ioMesh.setName(iName.c_str());

    }

}  // namespace

// once normals are supported in the polyMesh schema, polyMesh will look 
// different than readSubD
void readPoly(double iFrame, MFnMesh & ioMesh, MObject & iParent,
    Alembic::AbcGeom::IPolyMesh & iNode, bool iInitialized)
{
    Alembic::AbcGeom::IPolyMeshSchema schema = iNode.getSchema();
    Alembic::AbcGeom::MeshTopologyVariance ttype = schema.getTopologyVariance();

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
        schema.getTimeSampling(), index, ceilIndex);

    MFloatPointArray pointArray;
    Alembic::Abc::V3fArraySamplePtr ceilPoints;

    // we can just read the points
    if (ttype != Alembic::AbcGeom::kHeterogenousTopology && iInitialized)
    {

        Alembic::Abc::V3fArraySamplePtr points = schema.getPositions().getValue(
            Alembic::Abc::ISampleSelector(index) );

        if (alpha != 0.0)
        {
            ceilPoints = schema.getPositions().getValue(
                Alembic::Abc::ISampleSelector(ceilIndex) );
        }

        fillPoints(pointArray, points, ceilPoints, alpha);
        ioMesh.setPoints(pointArray, MSpace::kObject);
        return;
    }

    // we need to read the topology
    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    if (alpha != 0.0 && ttype != Alembic::AbcGeom::kHeterogenousTopology)
    {
        ceilPoints = schema.getPositions().getValue(
            Alembic::Abc::ISampleSelector(ceilIndex) );
    }

    fillPoints(pointArray, samp.getPositions(), ceilPoints, alpha);

    fillTopology(ioMesh, iParent, iNode.getName(), pointArray,
        samp.getIndices(), samp.getCounts());

}

void readSubD(double iFrame, MFnMesh & ioMesh, MObject & iParent,
    Alembic::AbcGeom::ISubD & iNode, bool iInitialized)
{
    Alembic::AbcGeom::ISubDSchema schema = iNode.getSchema();
    Alembic::AbcGeom::MeshTopologyVariance ttype = schema.getTopologyVariance();

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
        schema.getTimeSampling(), index, ceilIndex);

    MFloatPointArray pointArray;
    Alembic::Abc::V3fArraySamplePtr ceilPoints;

    // we can just read the points
    if (ttype != Alembic::AbcGeom::kHeterogenousTopology && iInitialized)
    {

        Alembic::Abc::V3fArraySamplePtr points = schema.getPositions().getValue(
            Alembic::Abc::ISampleSelector(index) );

        if (alpha != 0.0)
        {
            ceilPoints = schema.getPositions().getValue(
                Alembic::Abc::ISampleSelector(ceilIndex) );
        }

        fillPoints(pointArray, points, ceilPoints, alpha);
        ioMesh.setPoints(pointArray, MSpace::kObject);
        return;
    }

    // we need to read the topology
    Alembic::AbcGeom::ISubDSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    if (alpha != 0.0 && ttype != Alembic::AbcGeom::kHeterogenousTopology)
    {
        ceilPoints = schema.getPositions().getValue(
            Alembic::Abc::ISampleSelector(ceilIndex) );
    }

    fillPoints(pointArray, samp.getPositions(), ceilPoints, alpha);

    fillTopology(ioMesh, iParent, iNode.getName(), pointArray,
        samp.getFaceIndices(), samp.getFaceCounts());

}

void connectToPoly(double iFrame, Alembic::AbcGeom::IPolyMesh & iNode,
    std::vector<std::string> & oSampledPropNameList, MObject & iMeshObject)
{
    Alembic::AbcGeom::IPolyMeshSchema schema = iNode.getSchema();
    MString name(iNode.getName().c_str());

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(), index,
        ceilIndex);

    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    MFnMesh fnMesh;
    fnMesh.setObject(iMeshObject);

    size_t numVertices = fnMesh.numVertices();

    // simple test for change in topology
    if (samp.getPositions()->size() != numVertices)
    {
        MString theWarning("Processing fnMesh ");
        theWarning += name;
        theWarning += ", number of vertices changed: ";
        theWarning += "number of fnMesh vertices is ";
        theWarning += (unsigned int)(numVertices);
        theWarning +=
            MString(", number of fnMesh vertices last frame is ");
        theWarning += static_cast<unsigned int>
            (samp.getPositions()->size());
        printWarning(theWarning);
    }

    // disconnect old connection from AlembicNode or some other nodes
    // to inMesh if one such connection exist
    MPlug dstPlug = fnMesh.findPlug("inMesh");
    disconnectAllPlugsTo(dstPlug);

    // get prop names and make sure they are disconnected before
    // trying to connect to them
    // disconnect connections to animated props
    //dstPlug = fnMesh.findPlug(propName.c_str());
    // disconnectAllPlugsTo(dstPlug);

    clearPt(fnMesh);

    MFloatPointArray ptArray;
    Alembic::Abc::V3fArraySamplePtr ceilPoints;
    if (index != ceilIndex)
    {
        Alembic::AbcGeom::IPolyMeshSchema::Sample ceilSamp;
        schema.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));
        ceilPoints = ceilSamp.getPositions();
    }

    fillPoints(ptArray, samp.getPositions(), ceilPoints, alpha);
    fnMesh.setPoints(ptArray, MSpace::kObject);

    /*
    if ( schema.hasNormals() )
    {
        setPolyNormals(fnMesh, normals);
    }
    */

    //else
    {
        MStatus status = MS::kSuccess;
        MPlug plug = fnMesh.findPlug("noNormals", &status);
        if (status == MS::kSuccess)
            plug.setValue(false);
    }

    setUVs(iFrame, fnMesh, iNode);

}

MObject createPoly(double iFrame, Alembic::AbcGeom::IPolyMesh & iNode,
    MObject & iParent, std::vector<std::string> & oSampledPropNameList)
{
    Alembic::AbcGeom::IPolyMeshSchema schema = iNode.getSchema();
    MString name(iNode.getName().c_str());

    MStatus status = MS::kSuccess;

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(), index,
        ceilIndex);

    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    MFnMesh fnMesh;
    MObject obj;

    MFloatPointArray ptArray;
    Alembic::Abc::V3fArraySamplePtr ceilPoints;
    if (index != ceilIndex)
    {
        Alembic::AbcGeom::IPolyMeshSchema::Sample ceilSamp;
        schema.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));
        ceilPoints = ceilSamp.getPositions();
    }

    fillPoints(ptArray, samp.getPositions(), ceilPoints, alpha);
    fillTopology(fnMesh, iParent, iNode.getName(),
        ptArray, samp.getIndices(), samp.getCounts());

    obj = fnMesh.object();
    fnMesh.setName(name);

    MString pathName = fnMesh.partialPathName();
    setInitialShadingGroup(pathName);

    //if ( schema.hasNormals() )
    //    setPolyNormals(fnMesh, samp.getNormals());
    //else
    {
        MPlug plug = fnMesh.findPlug("noNormals", &status);
        if (status == MS::kSuccess)
            plug.setValue(false);
    }

    setUVs(iFrame, fnMesh, iNode);

    //if (!schema.hasNormals())
    {
        MFnNumericAttribute attr;
        MString attrName("noNormals");
        MObject attrObj = attr.create(attrName, attrName,
        MFnNumericData::kBoolean, true, &status);
        attr.setKeyable(true);
        attr.setHidden(false);
        MFnMesh fnMesh(obj);
        fnMesh.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
    }

    // add other properties

    return obj;
}

void connectToSubD(double iFrame, Alembic::AbcGeom::ISubD & iNode,
    std::vector<std::string> & oSampledPropNameList, MObject & iMeshObject)
{
    Alembic::AbcGeom::ISubDSchema schema = iNode.getSchema();

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(), index,
        ceilIndex);

    Alembic::AbcGeom::ISubDSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    MString name(iNode.getName().c_str());

    MFnMesh fnMesh;

    fnMesh.setObject(iMeshObject);

    unsigned int numVertices = fnMesh.numVertices();

    // simple test for change in topology
    if (samp.getPositions()->size() != numVertices)
    {
        MString theWarning("Processing fnMesh ");
        theWarning += name;
        theWarning += ", number of vertices changed: ";
        theWarning += "number of fnMesh vertices is ";
        theWarning += numVertices;
        theWarning +=
            MString(", number of fnMesh vertices last frame is ");
        theWarning += static_cast<unsigned int>
            (samp.getPositions()->size());
        printWarning(theWarning);
    }

    // disconnect old connection from AlembicNode or some other nodes
    // to inMesh if one such connection exist
    MPlug dstPlug = fnMesh.findPlug("inMesh");
    disconnectAllPlugsTo(dstPlug);

    // get prop names and make sure they are disconnected before
    // trying to connect to them
    // disconnect connections to animated props
    //dstPlug = fnMesh.findPlug(propName.c_str());
    // disconnectAllPlugsTo(dstPlug);

    clearPt(fnMesh);

    MFloatPointArray ptArray;
    Alembic::Abc::V3fArraySamplePtr ceilPoints;
    if (index != ceilIndex)
    {
        Alembic::AbcGeom::ISubDSchema::Sample ceilSamp;
        schema.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));
        ceilPoints = ceilSamp.getPositions();
    }

    fillPoints(ptArray, samp.getPositions(), ceilPoints, alpha);
    fnMesh.setPoints(ptArray, MSpace::kObject);
    setUVs(iFrame, fnMesh, iNode);

}

MObject createSubD(double iFrame, Alembic::AbcGeom::ISubD & iNode,
    MObject & iParent, std::vector<std::string> & oSampledPropNameList)
{
    Alembic::AbcGeom::ISubDSchema schema = iNode.getSchema();

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(), index,
        ceilIndex);

    Alembic::AbcGeom::ISubDSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    MString name(iNode.getName().c_str());

    MFnMesh fnMesh;

    MFloatPointArray pointArray;
    Alembic::Abc::V3fArraySamplePtr emptyPtr;
    fillPoints(pointArray, samp.getPositions(), emptyPtr, 0.0);

    fillTopology(fnMesh, iParent, iNode.getName(),
        pointArray, samp.getFaceIndices(), samp.getFaceCounts());

    MObject obj = fnMesh.object();
    fnMesh.setName(name);

    setInitialShadingGroup(fnMesh.partialPathName());

    //addProperties(iFrame, iNode, obj, oSampledPropNameList);

    setUVs(iFrame, fnMesh, iNode);

    // add the mFn-specific attributes to fnMesh node
    MFnNumericAttribute numAttr;
    MString attrName("SubDivisionMesh");
    MObject attrObj = numAttr.create(attrName, attrName,
        MFnNumericData::kBoolean, 1);
    numAttr.setKeyable(true);
    numAttr.setHidden(false);
    fnMesh.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);

    if (samp.getInterpolateBoundary() > 0)
    {
        attrName = MString("interpolateBoundary");
        attrObj = numAttr.create(attrName, attrName, MFnNumericData::kBoolean,
            samp.getInterpolateBoundary());

        numAttr.setKeyable(true);
        numAttr.setHidden(false);
        fnMesh.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    }

    if (samp.getFaceVaryingInterpolateBoundary() > 0)
    {
        attrName = MString("faceVaryingInterpolateBoundary");
        attrObj = numAttr.create(attrName, attrName, MFnNumericData::kBoolean,
            samp.getFaceVaryingInterpolateBoundary());

        numAttr.setKeyable(true);
        numAttr.setHidden(false);
        fnMesh.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    }

    if (samp.getFaceVaryingPropagateCorners() > 0)
    {
        attrName = MString("faceVaryingPropagateCorners");
        attrObj = numAttr.create(attrName, attrName, MFnNumericData::kBoolean,
            samp.getFaceVaryingPropagateCorners());

        numAttr.setKeyable(true);
        numAttr.setHidden(false);
        fnMesh.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    }

    if (!samp.getHoles()->size() == 0)
    {
        printWarning("Hole Poly Indices not yet supported.");
    }

    if (!samp.getCreaseSharpnesses()->size() == 0)
    {
        printWarning("Creases not yet supported.");
    }

    if (!samp.getCornerSharpnesses()->size() == 0)
    {
        printWarning("Corners not yet supported.");
    }

    return obj;
}
