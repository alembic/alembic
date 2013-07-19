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

#include "util.h"
#include "MeshHelper.h"
#include "NodeIteratorVisitorHelper.h"

#include <maya/MTypes.h>
#include <maya/MString.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MUintArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MVector.h>
#include <maya/MDagModifier.h>


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
        Alembic::AbcGeom::IV2fGeomParam iUVs)
    {

        if (!iUVs.valid())
            return;

        // no interpolation for now
        Alembic::AbcCoreAbstract::index_t index, ceilIndex;
        getWeightAndIndex(iFrame, iUVs.getTimeSampling(),
            iUVs.getNumSamples(), index, ceilIndex);

        MFloatArray uArray;
        MFloatArray vArray;

        unsigned int numFaceVertices = ioMesh.numFaceVertices();
        unsigned int numVertices = ioMesh.numVertices();

        MIntArray uvCounts(ioMesh.numPolygons(), 0);
        MIntArray uvIds(numFaceVertices, 0);

        Alembic::AbcGeom::IV2fGeomParam::Sample samp;
        iUVs.getIndexed(samp, Alembic::Abc::ISampleSelector(index));

        Alembic::AbcGeom::V2fArraySamplePtr uvPtr = samp.getVals();
        Alembic::Abc::UInt32ArraySamplePtr indexPtr = samp.getIndices();

        if (numFaceVertices != indexPtr->size() &&
            numVertices != indexPtr->size())
        {
            printWarning(
                ioMesh.fullPathName() +
                " UVs aren't per-vertex or per-polygon per-vertex, skipping");

            return;
        }

        unsigned int numUVs = (unsigned int)uvPtr->size();
        uArray.setLength(numUVs);
        vArray.setLength(numUVs);
        for (unsigned int i = 0; i < numUVs; ++i)
        {
            uArray[i] = (*uvPtr)[i].x;
            vArray[i] = (*uvPtr)[i].y;
        }

        int uvIndex = 0;
        int uvCountsIndex = 0;

        int numPolys = ioMesh.numPolygons();

        // per-polygon per-vertex
        if (numFaceVertices == indexPtr->size())
        {
            for (int pIndex = 0; pIndex < numPolys; ++pIndex)
            {
                int numPolygonVertices = ioMesh.polygonVertexCount(pIndex);
                uvCounts[uvCountsIndex++] = numPolygonVertices;
                if (numPolygonVertices == 0)
                    continue;

                int startPoint = uvIndex + numPolygonVertices - 1;

                for (int vertexIndex = 0;
                    vertexIndex < numPolygonVertices; vertexIndex++)
                {
                    uvIds[uvIndex++] = (*indexPtr)[startPoint - vertexIndex];
                }
            }
        }
        // per-vertex
        else
        {
            MIntArray vertexCount, vertexList;
            ioMesh.getVertices(vertexCount, vertexList);
            for (int pIndex = 0; pIndex < numPolys; ++pIndex)
            {
                int numPolygonVertices = ioMesh.polygonVertexCount(pIndex);
                uvCounts[uvCountsIndex++] = numPolygonVertices;
                if (numPolygonVertices == 0)
                    continue;

                int startPoint = uvIndex + numPolygonVertices - 1;

                for (int vertexIndex = 0;
                    vertexIndex < numPolygonVertices; vertexIndex++)
                {
                    uvIds[uvIndex++] = (*indexPtr)[
                        vertexList[startPoint - vertexIndex]];
                }
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
            iNormals.getScope() != Alembic::AbcGeom::kVaryingScope &&
            iNormals.getScope() != Alembic::AbcGeom::kFacevaryingScope)
        {
            printWarning(ioMesh.fullPathName() +
                " normal vector has an unsupported scope, skipping normals");
            return;
        }

        Alembic::AbcCoreAbstract::index_t index, ceilIndex;
        double alpha = getWeightAndIndex(iFrame,
            iNormals.getTimeSampling(), iNormals.getNumSamples(),
            index, ceilIndex);

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

        if ((iNormals.getScope() == Alembic::AbcGeom::kVertexScope || 
            iNormals.getScope() == Alembic::AbcGeom::kVaryingScope) &&
            sampSize == ( std::size_t ) ioMesh.numVertices())
        {
            MIntArray vertexList;
            int iEnd = static_cast<int>(sampSize);
            for (int i = 0; i < iEnd; ++i)
            {
                vertexList.append(i);
            }

            ioMesh.setVertexNormals(normalsIn, vertexList);

        }
        else if (sampSize == ( std::size_t ) ioMesh.numFaceVertices() &&
            iNormals.getScope() == Alembic::AbcGeom::kFacevaryingScope)
        {

            MIntArray faceList(static_cast<unsigned int>(sampSize));
            MIntArray vertexList(static_cast<unsigned int>(sampSize));

            // per vertex per-polygon normal
            int numFaces = ioMesh.numPolygons();
            int nIndex = 0;
            for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
            {
                MIntArray polyVerts;
                ioMesh.getPolygonVertices(faceIndex, polyVerts);
                int numVertices = polyVerts.length();
                for (int v = numVertices - 1; v >= 0; v--, ++nIndex)
                {
                    faceList[nIndex] = faceIndex;
                    vertexList[nIndex] = polyVerts[v];
                }
            }

            ioMesh.setFaceVertexNormals(normalsIn, faceList, vertexList);
        }
        else if (sampSize != 0)
        {
            printWarning(ioMesh.fullPathName() +
                " normal vector scope does not match size of data, " +
                "skipping normals");
        }
    }

    void fillPoints(MFloatPointArray & oPointArray,
        Alembic::Abc::P3fArraySamplePtr iPoints,
        Alembic::Abc::P3fArraySamplePtr iCeilPoints, double alpha)
    {
        unsigned int numPoints = static_cast<unsigned int>(iPoints->size());
        oPointArray.setLength(numPoints);

        if (alpha == 0 || iCeilPoints == NULL)
        {
            for (unsigned int i = 0; i < numPoints; ++i)
            {
                oPointArray.set(i,
                    (*iPoints)[i].x, (*iPoints)[i].y, (*iPoints)[i].z);
            }
        }
        else
        {
            for (unsigned int i = 0; i < numPoints; ++i)
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
        unsigned int numPolys = static_cast<unsigned int>(iCounts->size());
        MIntArray polyCounts;
        polyCounts.setLength(numPolys);

        for (unsigned int i = 0; i < numPolys; ++i)
        {
            polyCounts[i] = (*iCounts)[i];
        }

        unsigned int numConnects = static_cast<unsigned int>(iIndices->size());

        MIntArray polyConnects;
        polyConnects.setLength(numConnects);

        unsigned int facePointIndex = 0;
        unsigned int base = 0;
        for (unsigned int i = 0; i < numPolys; ++i)
        {
            // reverse the order of the faces
            int curNum = polyCounts[i];
            for (int j = 0; j < curNum; ++j, ++facePointIndex)
                polyConnects[facePointIndex] = (*iIndices)[base+curNum-j-1];

            base += curNum;
        }

        if (ioMesh.parentCount() != 0)
        {
            ioMesh.createInPlace(iPoints.length(), numPolys, iPoints,
                polyCounts, polyConnects);
        }
        else
        {
            ioMesh.create(iPoints.length(), numPolys, iPoints,
               polyCounts, polyConnects, iParent);
        }

    }

    void createColorSet(MFnMesh & meshIO, MString & iSetName,
        const Alembic::AbcCoreAbstract::MetaData & iMetaData)
    {

        MStatus status = meshIO.createColorSetDataMesh(iSetName);

        if (status != MS::kSuccess)
        {
            meshIO.createColorSetWithName(iSetName, NULL, NULL, NULL);
            if (iMetaData.get("mayaColorSet") == "1")
            {
                meshIO.setCurrentColorSetName(iSetName);
            }
#if MAYA_API_VERSION > 201200
            meshIO.setDisplayColors(true);
#endif
        }

    }

    void setColor(MFnMesh & ioMesh, MColorArray & iColorList,
        Alembic::Abc::UInt32ArraySamplePtr & iSampIndices,
        MString & iColorSet,
        MFnMesh::MColorRepresentation iRepr)
    {
        // per vertex per-polygon color
        int numFaces = ioMesh.numPolygons();
        int nIndex = 0;
        MIntArray assignmentList((unsigned int)iSampIndices->size());
        for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
        {
            int numVertices = ioMesh.polygonVertexCount(faceIndex);
            int curIndex = nIndex;
            for (int v = numVertices - 1; v >= 0; v--, ++nIndex)
            {
                assignmentList[nIndex] = (int) (*iSampIndices)[curIndex + v];
            }
        }

        MStatus status = ioMesh.setColors(iColorList, &iColorSet, iRepr);
        if (status != MStatus::kSuccess)
        {
            MGlobal::displayWarning("Couldnt set colors for " + iColorSet +
                status.errorString());
            return;
        }
        else
        {
            status = ioMesh.assignColors(assignmentList, &iColorSet);
        }

        if (status != MStatus::kSuccess)
        {
            MGlobal::displayWarning("Couldnt assign colors for " + iColorSet + 
                status.errorString());
        }
    }

    void setColor3f(double iFrame, MFnMesh & ioMesh,
        Alembic::AbcGeom::IC3fGeomParam & iC3f)
    {
        //Get the floor sample values
        Alembic::AbcCoreAbstract::index_t index, ceilIndex;
        double alpha = getWeightAndIndex(iFrame, iC3f.getTimeSampling(),
            iC3f.getNumSamples(), index, ceilIndex);

        Alembic::AbcGeom::IC3fGeomParam::Sample samp;
        iC3f.getIndexed(samp, Alembic::Abc::ISampleSelector(index));
        Alembic::Abc::C3fArraySamplePtr sampVal = samp.getVals();
        size_t sampSize = sampVal->size();

        if (ioMesh.numFaceVertices() != (int) samp.getIndices()->size())
        {
            MGlobal::displayWarning(
                "Color sample size != num face vertices");
            return;
        }

        MColorArray colorList;

        //Interpolate between 2 samples (as long as the indices are constant)
        //(non index is considered constant)
        if ( alpha != 0 && index != ceilIndex &&
            (!iC3f.getIndexProperty() || iC3f.getIndexProperty().isConstant()) )
        {
            Alembic::AbcGeom::IC3fGeomParam::Sample ceilSamp;
            iC3f.getIndexed(ceilSamp,
                    Alembic::Abc::ISampleSelector(ceilIndex));
            Alembic::Abc::C3fArraySamplePtr ceilVal = ceilSamp.getVals();
            // Make sure the point count hasn't changed
            if (sampSize == ceilVal->size())
            {
                for (size_t i = 0; i < sampSize; ++i)
                {
                    colorList.append(
                            simpleLerp<float>(alpha, (*sampVal)[i].x,
                                    (*ceilVal)[i].x),
                            simpleLerp<float>(alpha, (*sampVal)[i].y,
                                    (*ceilVal)[i].y),
                            simpleLerp<float>(alpha, (*sampVal)[i].z,
                                    (*ceilVal)[i].z));
                }
            }
            else
            {
                for (size_t i = 0; i < sampSize; ++i)
                {
                    colorList.append((*sampVal)[i].x, (*sampVal)[i].y,
                            (*sampVal)[i].z);
                }
            }
        }
        else
        {
            for (size_t i = 0; i < sampSize; ++i)
            {
                colorList.append((*sampVal)[i].x, (*sampVal)[i].y,
                        (*sampVal)[i].z);
            }
        }

        MStatus status = MStatus::kSuccess;
        MString colorSetName(iC3f.getName().c_str());
        Alembic::Abc::UInt32ArraySamplePtr indices = samp.getIndices();
        setColor(ioMesh, colorList, indices, colorSetName, MFnMesh::kRGB);
    }

    void setColor4f(double iFrame, MFnMesh & ioMesh,
        Alembic::AbcGeom::IC4fGeomParam & iC4f)
    {
        //Get the floor sample values
        Alembic::AbcCoreAbstract::index_t index, ceilIndex;
        double alpha = getWeightAndIndex(iFrame, iC4f.getTimeSampling(),
            iC4f.getNumSamples(), index, ceilIndex);

        Alembic::AbcGeom::IC4fGeomParam::Sample samp;
        iC4f.getIndexed(samp, Alembic::Abc::ISampleSelector(index));
        Alembic::Abc::C4fArraySamplePtr sampVal = samp.getVals();
        size_t sampSize = sampVal->size();

        if (ioMesh.numFaceVertices() != (int)samp.getIndices()->size())
        {
            MGlobal::displayWarning(
                "Color sample size != num face vertices");
            return;
        }

        MColorArray colorList;

        //Interpolate between 2 samples
        if ( alpha != 0 && index != ceilIndex &&
            (!iC4f.getIndexProperty() || iC4f.getIndexProperty().isConstant()) )
        {
            Alembic::AbcGeom::IC4fGeomParam::Sample ceilSamp;
            iC4f.getIndexed(ceilSamp,
                    Alembic::Abc::ISampleSelector(ceilIndex));
            Alembic::Abc::C4fArraySamplePtr ceilVal = ceilSamp.getVals();
            // Make sure the point count hasn't changed
            if (sampSize == ceilVal->size())
            {
                for (size_t i = 0; i < sampSize; ++i)
                {
                    colorList.append(
                            simpleLerp<float>(alpha, (*sampVal)[i].r,
                                    (*ceilVal)[i].r),
                            simpleLerp<float>(alpha, (*sampVal)[i].g,
                                    (*ceilVal)[i].g),
                            simpleLerp<float>(alpha, (*sampVal)[i].b,
                                    (*ceilVal)[i].b),
                            simpleLerp<float>(alpha, (*sampVal)[i].a,
                                    (*ceilVal)[i].a)
                                    );
                }
            }
            else
            {
                for (size_t i = 0; i < sampSize; ++i)
                {
                    colorList.append((*sampVal)[i].r, (*sampVal)[i].g,
                            (*sampVal)[i].b, (*sampVal)[i].a);
                }
            }
        }
        else
        {
            for (size_t i = 0; i < sampSize; ++i)
            {
                colorList.append((*sampVal)[i].r, (*sampVal)[i].g,
                        (*sampVal)[i].b, (*sampVal)[i].a);
            }
        }

        MStatus status = MStatus::kSuccess;
        MString colorSetName(iC4f.getName().c_str());
        Alembic::Abc::UInt32ArraySamplePtr indices = samp.getIndices();
        setColor(ioMesh, colorList, indices, colorSetName, MFnMesh::kRGBA);
    }

    bool inStrArray( const MStringArray & iArray, const MString & iStr )
    {
        unsigned int arrLength = iArray.length();

        for (unsigned int i = 0; i < arrLength; ++i)
        {
            if (iArray[i] == iStr)
            {
                return true;
            }
        }
        return false;
    }

    void setColors(double iFrame, MFnMesh & ioMesh,
        std::vector< Alembic::AbcGeom::IC3fGeomParam > iC3s,
        std::vector< Alembic::AbcGeom::IC4fGeomParam > iC4s,
        bool iSetStatic)
    {

        if (iC3s.empty() && iC4s.empty())
        {
            return;
        }

        std::vector< Alembic::AbcGeom::IC3fGeomParam >::iterator c3s;
        std::vector< Alembic::AbcGeom::IC3fGeomParam >::iterator c3sEnd =
            iC3s.end();

        MStringArray allSetNames;
        ioMesh.getColorSetNames(allSetNames);

        for (c3s = iC3s.begin(); c3s != c3sEnd; ++c3s)
        {
            if (c3s->getNumSamples() > 0 && (iSetStatic || !c3s->isConstant()))
            {
                MString setName(c3s->getName().c_str());
                if (!inStrArray(allSetNames, setName))
                {
                    createColorSet(ioMesh, setName, c3s->getMetaData());
                    allSetNames.append(setName);
                }

                setColor3f(iFrame, ioMesh, *c3s);
            }
        }

        std::vector< Alembic::AbcGeom::IC4fGeomParam >::iterator c4s;
        std::vector< Alembic::AbcGeom::IC4fGeomParam >::iterator c4sEnd =
            iC4s.end();

        for (c4s = iC4s.begin(); c4s != c4sEnd; ++c4s)
        {
            if (c4s->getNumSamples() > 0 && (iSetStatic || !c4s->isConstant()))
            {
                MString setName(c4s->getName().c_str());
                if (!inStrArray(allSetNames, setName))
                {
                    createColorSet(ioMesh, setName, c4s->getMetaData());
                    allSetNames.append(setName);
                }

                setColor4f(iFrame, ioMesh, *c4s);
            }
        }
    }
}  // namespace

void readPoly(double iFrame, MFnMesh & ioMesh, MObject & iParent,
    PolyMeshAndColors & iNode, bool iInitialized)
{
    Alembic::AbcGeom::IPolyMeshSchema schema = iNode.mMesh.getSchema();
    Alembic::AbcGeom::MeshTopologyVariance ttype = schema.getTopologyVariance();

    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
        schema.getTimeSampling(), schema.getNumSamples(), index, ceilIndex);

    MFloatPointArray pointArray;
    Alembic::Abc::P3fArraySamplePtr ceilPoints;

    // we can just read the points
    if (ttype != Alembic::AbcGeom::kHeterogenousTopology && iInitialized)
    {

        Alembic::Abc::P3fArraySamplePtr points = schema.getPositionsProperty(
            ).getValue(Alembic::Abc::ISampleSelector(index));

        if (alpha != 0.0)
        {
            ceilPoints = schema.getPositionsProperty().getValue(
                Alembic::Abc::ISampleSelector(ceilIndex) );
        }

        fillPoints(pointArray, points, ceilPoints, alpha);
        ioMesh.setPoints(pointArray, MSpace::kObject);
        setColors(iFrame, ioMesh, iNode.mC3s, iNode.mC4s, !iInitialized);

        if (schema.getNormalsParam().getNumSamples() > 1)
        {
            setPolyNormals(iFrame, ioMesh, schema.getNormalsParam());
        }

        if (schema.getUVsParam().getNumSamples() > 1)
        {
            setUVs(iFrame, ioMesh, schema.getUVsParam());
        }

        return;
    }

    // we need to read the topology
    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    if (alpha != 0.0 && ttype != Alembic::AbcGeom::kHeterogenousTopology)
    {
        ceilPoints = schema.getPositionsProperty().getValue(
            Alembic::Abc::ISampleSelector(ceilIndex) );
    }

    fillPoints(pointArray, samp.getPositions(), ceilPoints, alpha);

    fillTopology(ioMesh, iParent, pointArray, samp.getFaceIndices(),
        samp.getFaceCounts());

    setPolyNormals(iFrame, ioMesh, schema.getNormalsParam());
    setUVs(iFrame, ioMesh, schema.getUVsParam());
    setColors(iFrame, ioMesh, iNode.mC3s, iNode.mC4s, !iInitialized);
}

void readSubD(double iFrame, MFnMesh & ioMesh, MObject & iParent,
    SubDAndColors & iNode, bool iInitialized)
{
    Alembic::AbcGeom::ISubDSchema schema = iNode.mMesh.getSchema();
    Alembic::AbcGeom::MeshTopologyVariance ttype = schema.getTopologyVariance();

    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
        schema.getTimeSampling(), schema.getNumSamples(), index, ceilIndex);

    MFloatPointArray pointArray;
    Alembic::Abc::P3fArraySamplePtr ceilPoints;

    // we can just read the points
    if (ttype != Alembic::AbcGeom::kHeterogenousTopology && iInitialized)
    {

        Alembic::Abc::P3fArraySamplePtr points = schema.getPositionsProperty(
            ).getValue(Alembic::Abc::ISampleSelector(index));

        if (alpha != 0.0)
        {
            ceilPoints = schema.getPositionsProperty().getValue(
                Alembic::Abc::ISampleSelector(ceilIndex) );
        }

        fillPoints(pointArray, points, ceilPoints, alpha);
        ioMesh.setPoints(pointArray, MSpace::kObject);

        if (schema.getUVsParam().getNumSamples() > 1)
        {
            setUVs(iFrame, ioMesh, schema.getUVsParam());
        }

        setColors(iFrame, ioMesh, iNode.mC3s, iNode.mC4s, !iInitialized);
        return;
    }

    // we need to read the topology
    Alembic::AbcGeom::ISubDSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    if (alpha != 0.0 && ttype != Alembic::AbcGeom::kHeterogenousTopology)
    {
        ceilPoints = schema.getPositionsProperty().getValue(
            Alembic::Abc::ISampleSelector(ceilIndex) );
    }

    fillPoints(pointArray, samp.getPositions(), ceilPoints, alpha);

    fillTopology(ioMesh, iParent, pointArray, samp.getFaceIndices(),
        samp.getFaceCounts());

    setUVs(iFrame, ioMesh, schema.getUVsParam());
    setColors(iFrame, ioMesh, iNode.mC3s, iNode.mC4s, !iInitialized);
}

void disconnectMesh(MObject & iMeshObject,
    std::vector<Prop> & iSampledPropList,
    std::size_t iFirstProp)
{
    MFnMesh fnMesh;
    fnMesh.setObject(iMeshObject);

    // disconnect old connection from AlembicNode or some other nodes
    // to inMesh if one such connection exist
    MPlug dstPlug = fnMesh.findPlug("inMesh");
    disconnectAllPlugsTo(dstPlug);

    disconnectProps(fnMesh, iSampledPropList, iFirstProp);

    clearPt(fnMesh);

    return;

}

MObject createPoly(double iFrame, PolyMeshAndColors & iNode,
    MObject & iParent)
{
    Alembic::AbcGeom::IPolyMeshSchema schema = iNode.mMesh.getSchema();
    MString name(iNode.mMesh.getName().c_str());

    MObject obj;

    // add other properties
    if (!schema.isConstant())
    {
        MFloatPointArray emptyPt;
        MIntArray emptyInt;
        MFnMesh fnMesh;
        obj = fnMesh.create(0, 0, emptyPt, emptyInt, emptyInt, iParent);
        fnMesh.setName(name);
    }
    else
    {
        Alembic::AbcCoreAbstract::index_t index, ceilIndex;
        double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(),
            schema.getNumSamples(), index, ceilIndex);

        Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
        schema.get(samp, Alembic::Abc::ISampleSelector(index));

        MFloatPointArray ptArray;
        Alembic::Abc::P3fArraySamplePtr ceilPoints;
        if (index != ceilIndex)
        {
            Alembic::AbcGeom::IPolyMeshSchema::Sample ceilSamp;
            schema.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));
            ceilPoints = ceilSamp.getPositions();
        }

        fillPoints(ptArray, samp.getPositions(), ceilPoints, alpha);

        MFnMesh fnMesh;
        fillTopology(fnMesh, iParent, ptArray, samp.getFaceIndices(),
            samp.getFaceCounts());
        fnMesh.setName(iNode.mMesh.getName().c_str());
        setPolyNormals(iFrame, fnMesh, schema.getNormalsParam());
        setUVs(iFrame, fnMesh, schema.getUVsParam());
        obj = fnMesh.object();
    }

    MFnMesh fnMesh(obj);

    MString pathName = fnMesh.partialPathName();
    setInitialShadingGroup(pathName);

    setColors(iFrame, fnMesh, iNode.mC3s, iNode.mC4s, true);


    if ( !schema.getNormalsParam().valid() )
    {
        MFnNumericAttribute attr;
        MString attrName("noNormals");
        MObject attrObj = attr.create(attrName, attrName,
        MFnNumericData::kBoolean, true);
        attr.setKeyable(true);
        attr.setHidden(false);
        fnMesh.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
    }

    return obj;
}

MObject createSubD(double iFrame, SubDAndColors & iNode,
    MObject & iParent)
{
    Alembic::AbcGeom::ISubDSchema schema = iNode.mMesh.getSchema();

    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    getWeightAndIndex(iFrame, schema.getTimeSampling(),
        schema.getNumSamples(), index, ceilIndex);

    Alembic::AbcGeom::ISubDSchema::Sample samp;
    schema.get(samp, Alembic::Abc::ISampleSelector(index));

    MString name(iNode.mMesh.getName().c_str());

    MFnMesh fnMesh;

    MFloatPointArray pointArray;
    Alembic::Abc::P3fArraySamplePtr emptyPtr;
    fillPoints(pointArray, samp.getPositions(), emptyPtr, 0.0);

    fillTopology(fnMesh, iParent, pointArray, samp.getFaceIndices(),
        samp.getFaceCounts());
    fnMesh.setName(iNode.mMesh.getName().c_str());

    setInitialShadingGroup(fnMesh.partialPathName());

    MObject obj = fnMesh.object();

    setUVs(iFrame, fnMesh, schema.getUVsParam());

    setColors(iFrame, fnMesh, iNode.mC3s, iNode.mC4s, true);

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

#if MAYA_API_VERSION >= 201100
    Alembic::Abc::Int32ArraySamplePtr holes = samp.getHoles();
    if (holes && !holes->size() == 0)
    {
        unsigned int numHoles = (unsigned int)holes->size();
        MUintArray holeData(numHoles);
        for (unsigned int i = 0; i < numHoles; ++i)
        {
            holeData[i] = (*holes)[i];
        }

        if (fnMesh.setInvisibleFaces(holeData) != MS::kSuccess)
        {
            MString warn = "Failed to set holes on: ";
            warn += iNode.mMesh.getName().c_str();
            printWarning(warn);
        }
    }
#endif

    Alembic::Abc::FloatArraySamplePtr creases = samp.getCreaseSharpnesses();
    if (creases && !creases->size() == 0)
    {
        Alembic::Abc::Int32ArraySamplePtr indices = samp.getCreaseIndices();
        Alembic::Abc::Int32ArraySamplePtr lengths = samp.getCreaseLengths();
        std::size_t numLengths = lengths->size();

        MUintArray edgeIds;
        MDoubleArray creaseData;

        std::size_t curIndex = 0;
        // curIndex incremented here to move on to the next crease length
        for (std::size_t i = 0; i < numLengths; ++i, ++curIndex)
        {
            std::size_t len = (*lengths)[i] - 1;
            float creaseSharpness = (*creases)[i];

            // curIndex incremented here to go between all the edges that make
            // up a given length
            for (std::size_t j = 0; j < len; ++j, ++curIndex)
            {
                Alembic::Util::int32_t vertA = (*indices)[curIndex];
                Alembic::Util::int32_t vertB = (*indices)[curIndex+1];
                MItMeshVertex itv(obj);

                int prev;
                itv.setIndex(vertA, prev);

                MIntArray edges;
                itv.getConnectedEdges(edges);
                std::size_t numEdges = edges.length();
                for (unsigned int k = 0; k < numEdges; ++k)
                {
                    int oppVert = -1;
                    itv.getOppositeVertex(oppVert, edges[k]);
                    if (oppVert == vertB)
                    {
                        creaseData.append(creaseSharpness);
                        edgeIds.append(edges[k]);
                        break;
                    }
                }
            }
        }
        if (fnMesh.setCreaseEdges(edgeIds, creaseData) != MS::kSuccess)
        {
            MString warn = "Failed to set creases on: ";
            warn += iNode.mMesh.getName().c_str();
            printWarning(warn);
        }
    }

    Alembic::Abc::FloatArraySamplePtr corners = samp.getCornerSharpnesses();
    if (corners && !corners->size() == 0)
    {
        Alembic::Abc::Int32ArraySamplePtr cornerVerts = samp.getCornerIndices();
        unsigned int numCorners = static_cast<unsigned int>(corners->size());
        MUintArray vertIds(numCorners);
        MDoubleArray cornerData(numCorners);

        for (unsigned int i = 0; i < numCorners; ++i)
        {
            cornerData[i] = (*corners)[i];
            vertIds[i] = (*cornerVerts)[i];
        }
        if (fnMesh.setCreaseVertices(vertIds, cornerData) != MS::kSuccess)
        {
            MString warn = "Failed to set corners on: ";
            warn += iNode.mMesh.getName().c_str();
            printWarning(warn);
        }
    }

    return obj;
}
