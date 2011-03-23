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
            printError(ioMesh.fullPathName() + " Assign UVs failed");

        return status;
    }  // setMeshUVs


    void setUVs(double iFrame, MFnMesh & ioMesh,
        Alembic::AbcGeom::IV2fGeomParam & iUVs)
    {

        if (!iUVs.valid())
            return;

        // no interpolation for now
        int64_t index, ceilIndex;
        double alpha = getWeightAndIndex(iFrame, iUVs.getTimeSampling(),
            index, ceilIndex);

        MFloatArray uArray;
        MFloatArray vArray;

        size_t numFaceVertices = ioMesh.numFaceVertices();
        MIntArray uvCounts(ioMesh.numPolygons(), 0);
        MIntArray uvIds(numFaceVertices, 0);

        Alembic::AbcGeom::IV2fGeomParam::Sample samp;
        iUVs.getIndexed(samp, Alembic::Abc::ISampleSelector(index));

        Alembic::AbcGeom::V2fArraySamplePtr uvPtr = samp.getVals();
        Alembic::Abc::UInt32ArraySamplePtr indexPtr = samp.getIndices();

        if (numFaceVertices != indexPtr->size())
        {
            printWarning(
                ioMesh.fullPathName() +
                    " UVs aren't per-polygon per-vertex, skipping");
            return;
        }

        size_t numUVs = uvPtr->size();
        uArray.setLength(numUVs);
        vArray.setLength(numUVs);
        for (size_t i = 0; i < numUVs; ++i)
        {
            uArray[i] = (*uvPtr)[i].x;
            vArray[i] = 1.0 - (*uvPtr)[i].y;
        }

        size_t uvIndex = 0;
        size_t uvCountsIndex = 0;
        size_t vIndex = 0;

        size_t numPolys = ioMesh.numPolygons();
        for (size_t pIndex = 0; pIndex < numPolys; ++pIndex)
        {
            size_t numPolygonVertices = ioMesh.polygonVertexCount(pIndex);
            uvCounts[uvCountsIndex++] = numPolygonVertices;
            if (numPolygonVertices == 0)
                continue;

            size_t startPoint = uvIndex + numPolygonVertices - 1;

            for (size_t vertexIndex = 0;
                vertexIndex < numPolygonVertices; vertexIndex++)
            {
                uvIds[uvIndex++] = (*indexPtr)[startPoint - vertexIndex];
            }
        }


        setMeshUVs(ioMesh, uArray, vArray, uvCounts, uvIds);
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
    void setPolyNormals(double iFrame, MFnMesh & ioMesh,
        Alembic::AbcGeom::IN3fGeomParam iNormals)
    {
        // no normals to set?  bail early
        if (!iNormals)
            return;

        if (iNormals.getScope() != Alembic::AbcGeom::kVertexScope &&
            iNormals.getScope() != Alembic::AbcGeom::kFacevaryingScope)
        {
            printWarning(ioMesh.fullPathName() +
                " normal vector has an unsupported scope, skipping normals");
            return;
        }

        int64_t index, ceilIndex;
        double alpha = getWeightAndIndex(iFrame,
            iNormals.getTimeSampling(), index, ceilIndex);

        Alembic::AbcGeom::IN3fGeomParam::Sample samp;
        iNormals.getExpanded(samp, Alembic::Abc::ISampleSelector(index));

        MVectorArray normalsIn;

        Alembic::Abc::N3fArraySamplePtr sampVal = samp.getVals();
        size_t sampSize = sampVal->size();

        Alembic::Abc::N3fArraySamplePtr ceilVals;
        if (alpha != 0 && index != ceilIndex)
        {
            Alembic::AbcGeom::IN3fGeomParam::Sample ceilSamp;
            iNormals.getExpanded(ceilSamp,
                Alembic::Abc::ISampleSelector(ceilIndex));
            ceilVals = ceilSamp.getVals();
            if (sampSize == ceilVals->size())
            {
                Alembic::Abc::N3fArraySamplePtr ceilVal = ceilSamp.getVals();
                for (size_t i = 0; i < sampSize; ++i)
                {
                    MVector normal(
                        simpleLerp<float>(alpha, (*sampVal)[i].x,
                            (*ceilVal)[i].x),
                        simpleLerp<float>(alpha, (*sampVal)[i].y,
                            (*ceilVal)[i].y),
                        simpleLerp<float>(alpha, (*sampVal)[i].z,
                            (*ceilVal)[i].z));
                    normalsIn.append(normal);
                }
            }
            else
            {
                for (size_t i = 0; i < sampSize; ++i)
                {
                    MVector normal((*sampVal)[i].x, (*sampVal)[i].y,
                        (*sampVal)[i].z);
                    normalsIn.append(normal);
                }
            }
        }
        else
        {
            for (size_t i = 0; i < sampSize; ++i)
            {
                MVector normal((*sampVal)[i].x, (*sampVal)[i].y,
                    (*sampVal)[i].z);
                normalsIn.append(normal);
            }
        }

        if (iNormals.getScope() == Alembic::AbcGeom::kVertexScope &&
            sampSize == ioMesh.numVertices())
        {
            MIntArray vertexList;
            for (size_t i = 0; i < sampSize; ++i)
            {
                vertexList.append(i);
            }

            ioMesh.setVertexNormals(normalsIn, vertexList);

        }
        else if (sampSize == ioMesh.numFaceVertices() &&
            iNormals.getScope() == Alembic::AbcGeom::kFacevaryingScope)
        {
            // per vertex per-polygon normal
            int numFaces = ioMesh.numPolygons();
            int nIndex = 0;
            for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
            {
                MIntArray vertexList;
                ioMesh.getPolygonVertices(faceIndex, vertexList);
                unsigned int numVertices = vertexList.length();

                for ( int v = numVertices-1; v >= 0; --v, ++nIndex )
                {
                    unsigned int vertexIndex = vertexList[v];

                    ioMesh.setFaceVertexNormal(normalsIn[nIndex],
                        faceIndex, vertexIndex);
                }
            }
        }
        else
        {
            printWarning(ioMesh.fullPathName() +
                " normal vector scope does not match size of data, " +
                "skipping normals");
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

        if (schema.getNormals().getNumSamples() > 1)
        {
            setPolyNormals(iFrame, ioMesh, schema.getNormals());
        }

        if (schema.getUVs().getNumSamples() > 1)
        {
            setUVs(iFrame, ioMesh, schema.getUVs());
        }

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

    fillTopology(ioMesh, iParent, pointArray, samp.getIndices(),
        samp.getCounts());

    setPolyNormals(iFrame, ioMesh, schema.getNormals());
    setUVs(iFrame, ioMesh, schema.getUVs());
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

        if (schema.getUVs().getNumSamples() > 1)
        {
            setUVs(iFrame, ioMesh, schema.getUVs());
        }

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

    fillTopology(ioMesh, iParent, pointArray, samp.getFaceIndices(),
        samp.getFaceCounts());

    setUVs(iFrame, ioMesh, schema.getUVs());
}

void disconnectMesh(MObject & iMeshObject,
    std::vector<std::string> & oSampledPropNameList)
{
    MFnMesh fnMesh;
    fnMesh.setObject(iMeshObject);

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

    return;

}

MObject createPoly(double iFrame, Alembic::AbcGeom::IPolyMesh & iNode,
    MObject & iParent, std::vector<std::string> & oSampledPropNameList)
{
    Alembic::AbcGeom::IPolyMeshSchema schema = iNode.getSchema();
    MString name(iNode.getName().c_str());

    MStatus status = MS::kSuccess;

    MFnMesh fnMesh;
    MObject obj;

    // add other properties
    if (schema.getNumSamples() > 1)
    {
        MFloatPointArray emptyPt;
        MIntArray emptyInt;
        obj = fnMesh.create(0, 0, emptyPt, emptyInt, emptyInt, iParent);
        fnMesh.setName(name);
    }
    else
    {
        int64_t index, ceilIndex;
        double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(),
            index, ceilIndex);

        Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
        schema.get(samp, Alembic::Abc::ISampleSelector(index));

        MFloatPointArray ptArray;
        Alembic::Abc::V3fArraySamplePtr ceilPoints;
        if (index != ceilIndex)
        {
            Alembic::AbcGeom::IPolyMeshSchema::Sample ceilSamp;
            schema.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));
            ceilPoints = ceilSamp.getPositions();
        }

        fillPoints(ptArray, samp.getPositions(), ceilPoints, alpha);
        fillTopology(fnMesh, iParent, ptArray, samp.getIndices(),
            samp.getCounts());
        fnMesh.setName(iNode.getName().c_str());
        setPolyNormals(iFrame, fnMesh, schema.getNormals());
        setUVs(iFrame, fnMesh, schema.getUVs());
        obj = fnMesh.object();
    }

    MString pathName = fnMesh.partialPathName();
    setInitialShadingGroup(pathName);

    if ( !schema.getNormals().valid() )
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

    return obj;
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

    fillTopology(fnMesh, iParent, pointArray, samp.getFaceIndices(),
        samp.getFaceCounts());
    fnMesh.setName(iNode.getName().c_str());

    setInitialShadingGroup(fnMesh.partialPathName());

    MObject obj = fnMesh.object();
    //addProperties(iFrame, iNode, obj, oSampledPropNameList);

    setUVs(iFrame, fnMesh, schema.getUVs());

    // add the mFn-specific attributes to fnMesh node
    MFnNumericAttribute numAttr;
    MString attrName("SubDivisionMesh");
    MObject attrObj = numAttr.create(attrName, attrName,
        MFnNumericData::kBoolean, 1);
    numAttr.setKeyable(true);
    numAttr.setHidden(false);
    fnMesh.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);

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

    if (samp.getHoles() && !samp.getHoles()->size() == 0)
    {
        printWarning("Hole Poly Indices not yet supported.");
    }

    if (samp.getCreaseSharpnesses() && 
        !samp.getCreaseSharpnesses()->size() == 0)
    {
        printWarning("Creases not yet supported.");
    }

    if (samp.getCornerSharpnesses() && 
        !samp.getCornerSharpnesses()->size() == 0)
    {
        printWarning("Corners not yet supported.");
    }

    return obj;
}
