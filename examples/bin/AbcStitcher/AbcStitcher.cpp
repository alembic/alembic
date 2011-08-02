//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include "util.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace ::Alembic::AbcGeom;
using namespace ::Alembic::AbcCoreAbstract;

typedef std::vector< chrono_t > chrono_tVec;
size_t NUMINPUTS = 0;
const chrono_t ZERO = 1e-5;
chrono_tVec minVec;

namespace{

inline void stitchVisible(ICompoundPropertyVec & iCompoundProps,
                          OCompoundProperty & oCompoundProp)
{
    const PropertyHeader * propHeaderPtr =
            iCompoundProps[0].getPropertyHeader("visible");
    stitchScalarProp(*propHeaderPtr, iCompoundProps, oCompoundProp);
}

template< class IPARAM, class IPARAMSAMP, class OPARAMSAMP >
void getOGeomParamSamp(IPARAM & iGeomParam, IPARAMSAMP & iGeomSamp,
                     OPARAMSAMP & oGeomSamp, index_t iIndex)
{
    if (iGeomParam.isIndexed())
    {
        iGeomParam.getIndexed(iGeomSamp, iIndex);
        oGeomSamp.setVals(*(iGeomSamp.getVals()));
        oGeomSamp.setScope(iGeomSamp.getScope());
        oGeomSamp.setIndices(*(iGeomSamp.getIndices()));
    }
    else
    {
        iGeomParam.getExpanded(iGeomSamp, iIndex);
        oGeomSamp.setVals(*(iGeomSamp.getVals()));
        oGeomSamp.setScope(iGeomSamp.getScope());
    }
}

template< class IData, class IDataSchema, class OData, class ODataSchema >
void init(std::vector< IObject > & iObjects, OObject & oParentObj,
          ODataSchema & oSchema, int NUMINPUTS)
{
    const std::string fullNodeName = iObjects[0].getFullName();

    // gether information from the first input node in the list:
    IDataSchema iSchema0 = IData(iObjects[0], Alembic::Abc::kWrapExisting).getSchema();

    TimeSamplingPtr tsPtr0 = iSchema0.getTimeSampling();
    TimeSamplingType tsType0 = tsPtr0->getTimeSamplingType();
    checkAcyclic(tsType0, fullNodeName);

    ICompoundPropertyVec iCompoundProps;
    iCompoundProps.reserve(NUMINPUTS);

    ICompoundProperty cp = iObjects[0].getProperties();
    iCompoundProps.push_back(cp);

    ICompoundPropertyVec iArbGeomCompoundProps;
    iArbGeomCompoundProps.reserve(NUMINPUTS);

    ICompoundProperty arbProp = iSchema0.getArbGeomParams();
    if (arbProp)  // might be empty
        iArbGeomCompoundProps.push_back(arbProp);

    bool hasVisible = cp.getPropertyHeader("visible")?true:false;

    // sanity check (no frame range checking here)
    //      - timesamplying type has to be the same and can't be acyclic
    for (int i = 1; i < NUMINPUTS; i++)
    {
        IDataSchema iSchema =
            IData(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();

        TimeSamplingPtr tsPtr = iSchema.getTimeSampling();
        TimeSamplingType tsType = tsPtr->getTimeSamplingType();
        checkAcyclic(tsType, fullNodeName);
        if (!(tsType0 == tsType))
        {
            std::cerr << "Can not stitch different sampling type for node \""
                << fullNodeName << "\"" << std::endl;
            // more details on this
            if (tsType.getNumSamplesPerCycle()
                != tsType0.getNumSamplesPerCycle())
            {
                std::cerr << "\tnumSamplesPerCycle values are different"
                    << std::endl;
            }
            if (tsType.getTimePerCycle() != tsType0.getTimePerCycle())
            {
                std::cerr << "\ttimePerCycle values are different"
                    << std::endl;
            }
            exit(1);
        }

        ICompoundProperty cp = iObjects[i].getProperties();
        iCompoundProps.push_back(cp);

        ICompoundProperty arbProp = iSchema.getArbGeomParams();
        if (arbProp)  // might be empty
            iArbGeomCompoundProps.push_back(arbProp);
    }

    OData oData(oParentObj, iObjects[0].getName(), tsPtr0);
    oSchema = oData.getSchema();

    // stitch "visible" if it's points
    //
    if (hasVisible)
    {
        OCompoundProperty oCompoundProp = oData.getProperties();
        stitchVisible(iCompoundProps, oCompoundProp);
    }

    // stitch arbGeoCompoundParams
    //
    if (iArbGeomCompoundProps.size() == (size_t) NUMINPUTS)
    {
        OCompoundProperty oArbGeomCompoundProp = oSchema.getArbGeomParams();
        stitchCompoundProp(iArbGeomCompoundProps, oArbGeomCompoundProp);
    }
}

};

//-*****************************************************************************
// a recursive function that reads all inputs and write to the given oObject
// node if there's no gap in the frame range for animated nodes
//
void visitObjects(std::vector< IObject > & iObjects, OObject & oParentObj)
{
    const AbcA::ObjectHeader & header = iObjects[0].getHeader();

    // there are a number of things that needs to be checked for each node
    // to make sure they can be properly stitched together
    //
    // for xform node:
    //      locator or normal xform node
    //      if an xform node, numOps and type of ops match
    //      static or no, and if not, timesampling type matches
    //      if sampled, timesampling type should match
    //      if sampled, no frame gaps
    //
    if (IXform::matches(header))
    {
        std::string fullNodeName = iObjects[0].getFullName();

        // cache information starting from the first input node
        std::vector< TimeSamplingPtr > tsPtrVec;
        tsPtrVec.reserve(NUMINPUTS);

        // gether information from the first input node in the list:
        //
        IXformSchema xformSchema0 = IXform(iObjects[0], Alembic::Abc::kWrapExisting).getSchema();
        size_t numSamples = xformSchema0.getNumSamples();
        size_t numOps0 = xformSchema0.getNumOps();
        XformSample samp0 = xformSchema0.getValue(0);

        TimeSamplingPtr tsPtr0 = xformSchema0.getTimeSampling();
        tsPtrVec.push_back(tsPtr0);
        chrono_t lastMax = std::max(tsPtr0->getSampleTime(numSamples-1), -DBL_MAX);

        TimeSamplingType tsType0 = tsPtr0->getTimeSamplingType();
        checkAcyclic(tsType0, fullNodeName);
        chrono_t spf = tsType0.getTimePerCycle();

        ICompoundPropertyVec iCompoundProps;
        iCompoundProps.reserve(NUMINPUTS);
        ICompoundProperty cp = iObjects[0].getProperties();
        iCompoundProps.push_back(cp);

        bool hasVisible = cp.getPropertyHeader("visible")?true:false;
        bool isLocator = cp.getPropertyHeader("locator")?true:false;

        ICompoundPropertyVec iArbGeomCompoundProps;
        iArbGeomCompoundProps.reserve(NUMINPUTS);
        ICompoundProperty arbProp = xformSchema0.getArbGeomParams();
        if (arbProp)  // might be empty
            iArbGeomCompoundProps.push_back(arbProp);

        // sanity check
        //      - frame ranges to be stitched have no gaps
        //      - if this is a normal xform node, number order and type of operations should be exactly the same
        //      - timesamplying type has to be the same and can't be acyclic
        for (size_t i = 1; i < NUMINPUTS; i++)
        {
            IXformSchema xformSchema = IXform(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            TimeSamplingPtr tsPtr = xformSchema.getTimeSampling();
            tsPtrVec.push_back(tsPtr);

            TimeSamplingType tsType = tsPtr->getTimeSamplingType();
            checkAcyclic(tsType, fullNodeName);
            if (!(tsType0 == tsType))
            {
                std::cerr << "Can not stitch different sampling type for node \""
                    << fullNodeName << "\"" << std::endl;
                // more details on this
                if (tsType.getNumSamplesPerCycle() != tsType0.getNumSamplesPerCycle())
                    std::cerr << "\tnumSamplesPerCycle values are different" << std::endl;
                if (tsType.getTimePerCycle() != tsType0.getTimePerCycle())
                    std::cerr << "\ttimePerCycle values are different" << std::endl;
                exit(1);
            }

            ICompoundProperty cp = iObjects[i].getProperties();
            iCompoundProps.push_back(cp);

            ICompoundProperty arbProp = xformSchema.getArbGeomParams();
            if (arbProp)  // might be empty
                iArbGeomCompoundProps.push_back(arbProp);

            chrono_t lastMin = minVec[i-1];
            chrono_t min = minVec[i];
            if (!xformSchema.isConstant() && fabs(min - lastMax - spf) > ZERO)
            {
                std::cerr << "there's a gap between [" << lastMin << ", "
                    << lastMax << "] to " << min << std::endl;
                exit(1);
            }
            lastMax = std::max(tsPtr->getSampleTime(numSamples-1), -DBL_MAX);

            if (!isLocator)
            {
                if (numOps0 != xformSchema.getNumOps())
                {
                    std::cerr << "Xform nodes have different numOps" << std::endl;
                    exit(1);
                }
                XformSample samp = xformSchema.getValue(0);
                size_t sampIndex = 0;
                for (; sampIndex < numOps0; sampIndex++)
                {
                    if (samp[sampIndex].getType() != samp0[sampIndex].getType())
                    {
                        std::cerr << "Can not stitch xform nodes that have different types of ops" << std::endl;
                        exit(1);
                    }
                }
            }  // end of operation checking
        }

        OXform oXform(oParentObj, iObjects[0].getName(), tsPtr0);
        OXformSchema oXformSchema = oXform.getSchema();

        // stitch the operations if this is an xform node
        //
        if (!isLocator)
        {
            if (xformSchema0.isConstant())
            {
                IXform xform(iObjects[0], Alembic::Abc::kWrapExisting);
                XformSample samp;
                xform.getSchema().get(samp);
                if (samp.getNumOps() > 0)
                    oXformSchema.set(samp);
            }
            else
            {
                for (size_t i = 0; i < NUMINPUTS; i++)
                {
                    IXformSchema iSchema =
                        IXform(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
                    index_t numSamples = iSchema.getNumSamples();
                    for (index_t reqIdx = 0; reqIdx < numSamples; reqIdx++)
                    {
                        XformSample samp = iSchema.getValue(reqIdx);
                        oXformSchema.set(samp);
                    }
                }
            }
        }

        // stitch "visible" if it's an xform or "locator" if it's a locator
        //
        OCompoundProperty oCompoundProp = oXform.getProperties();
        if (isLocator)
        {
            const PropertyHeader * propHeaderPtr =
                iCompoundProps[0].getPropertyHeader("locator");
            stitchScalarProp(*propHeaderPtr,
                             iCompoundProps,
                             oCompoundProp);
        }
        if (hasVisible)
        {
            stitchVisible(iCompoundProps, oCompoundProp);
        }

        // stitch arbGeoCompoundParams
        //
        if (iArbGeomCompoundProps.size() == NUMINPUTS)
        {
            OCompoundProperty oArbGeomCompoundProp = oXformSchema.getArbGeomParams();
            stitchCompoundProp(iArbGeomCompoundProps, oArbGeomCompoundProp);
        }

        // After done writing THIS OObject node, if input nodes have children,
        // go deeper.
        // Otherwise we are done here
        size_t numChildren =  iObjects[0].getNumChildren();
        for (size_t i = 0 ; i < numChildren; i++ )
        {
            std::vector< IObject > iChildObjects;
            for (size_t f = 0; f < NUMINPUTS; f++)
            {
                iChildObjects.push_back(iObjects[f].getChild(i));
            }
            visitObjects(iChildObjects, oXform);
        }
    }
    else if (ISubD::matches(header))
    {
        OSubDSchema oSchema;
        init< ISubD,
              ISubDSchema,
              OSubD,
              OSubDSchema >(iObjects, oParentObj, oSchema, NUMINPUTS);

        // stitch the CurvesSchemas
        //
        for (size_t i = 0; i < NUMINPUTS; i++)
        {
            ISubDSchema iSchema =
                ISubD(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();
            for (index_t reqIdx = 0; reqIdx < numSamples; reqIdx++)
            {
                ISubDSchema::Sample iSamp = iSchema.getValue(reqIdx);
                OSubDSchema::Sample oSamp;

                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);

                Abc::Int32ArraySamplePtr faceIndicesPtr = iSamp.getFaceIndices();
                if (faceIndicesPtr)
                    oSamp.setFaceIndices(*faceIndicesPtr);

                Abc::Int32ArraySamplePtr faceCntPtr = iSamp.getFaceCounts();
                if (faceCntPtr)
                    oSamp.setFaceCounts(*faceCntPtr);

                oSamp.setFaceVaryingInterpolateBoundary(iSamp.getFaceVaryingInterpolateBoundary());
                oSamp.setFaceVaryingPropagateCorners(iSamp.getFaceVaryingPropagateCorners());
                oSamp.setInterpolateBoundary(iSamp.getInterpolateBoundary());

                Abc::Int32ArraySamplePtr creaseIndicesPtr = iSamp.getCreaseIndices();
                if (creaseIndicesPtr)
                    oSamp.setCreaseIndices(*creaseIndicesPtr);

                Abc::Int32ArraySamplePtr creaseLenPtr = iSamp.getCreaseLengths();
                if (creaseLenPtr)
                    oSamp.setCreaseLengths(*creaseLenPtr);

                Abc::FloatArraySamplePtr creaseSpPtr = iSamp.getCreaseSharpnesses();
                if (creaseSpPtr)
                    oSamp.setCreaseSharpnesses(*creaseSpPtr);

                Abc::Int32ArraySamplePtr cornerIndicesPtr = iSamp.getCornerIndices();
                if (cornerIndicesPtr)
                    oSamp.setCornerIndices(*cornerIndicesPtr);

                Abc::FloatArraySamplePtr cornerSpPtr = iSamp.getCornerSharpnesses();
                if (cornerSpPtr)
                    oSamp.setCreaseSharpnesses(*cornerSpPtr);

                Abc::Int32ArraySamplePtr holePtr = iSamp.getHoles();
                if (holePtr)
                    oSamp.setHoles(*holePtr);

                oSamp.setSubdivisionScheme(iSamp.getSubdivisionScheme());
                oSamp.setChildBounds(iSamp.getChildBounds());

                oSchema.set(oSamp);
            }
            if (iSchema.isConstant())
                break;
        }
    }
    else if (IPolyMesh::matches(header))
    {
        OPolyMeshSchema oSchema;
        init< IPolyMesh,
              IPolyMeshSchema,
              OPolyMesh,
              OPolyMeshSchema >(iObjects, oParentObj, oSchema, NUMINPUTS);

        // stitch the PolySchema
        //
        for (size_t i = 0; i < NUMINPUTS; i++)
        {
            IPolyMeshSchema iSchema =
                IPolyMesh(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();

            IN3fGeomParam normals = iSchema.getNormalsParam();
            IV2fGeomParam uvs = iSchema.getUVsParam();
            for (index_t reqIdx = 0; reqIdx < numSamples; reqIdx++)
            {
                IPolyMeshSchema::Sample iSamp = iSchema.getValue(reqIdx);
                OPolyMeshSchema::Sample oSamp;

                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);

                Abc::Int32ArraySamplePtr faceIndicesPtr = iSamp.getFaceIndices();
                if (faceIndicesPtr)
                    oSamp.setFaceIndices(*faceIndicesPtr);

                Abc::Int32ArraySamplePtr faceCntPtr = iSamp.getFaceCounts();
                if (faceCntPtr)
                    oSamp.setFaceCounts(*faceCntPtr);

                // set normals
                if (normals && normals.isIndexed())
                {
                    IN3fGeomParam::Sample iNormal = normals.getIndexedValue(reqIdx);
                    ON3fGeomParam::Sample oNormal(*(iNormal.getVals()),
                                                  *(iNormal.getIndices()),
                                                  iNormal.getScope());
                    oSamp.setNormals(oNormal);
                }
                else if (normals)
                {
                    IN3fGeomParam::Sample iNormal = normals.getExpandedValue(reqIdx);
                    ON3fGeomParam::Sample oNormal(*(iNormal.getVals()),
                                                  iNormal.getScope());
                    oSamp.setNormals(oNormal);
                }

                // set uvs
                if (uvs && uvs.isIndexed())
                {
                    IV2fGeomParam::Sample iUV = uvs.getIndexedValue(reqIdx);
                    OV2fGeomParam::Sample oUV(*(iUV.getVals()),
                                                  *(iUV.getIndices()),
                                                  iUV.getScope());
                    oSamp.setUVs(oUV);
                }
                else if (uvs)
                {
                    IV2fGeomParam::Sample iUV = uvs.getExpandedValue(reqIdx);
                    OV2fGeomParam::Sample oUV(*(iUV.getVals()),
                                                  iUV.getScope());
                    oSamp.setUVs(oUV);
                }

                oSamp.setChildBounds(iSamp.getChildBounds());

                oSchema.set(oSamp);
            }
            if (iSchema.isConstant())
                break;
        }
    }
    else if (ICamera::matches(header))
    {
        OCameraSchema oSchema;
        init< ICamera,
              ICameraSchema,
              OCamera,
              OCameraSchema >(iObjects, oParentObj, oSchema, NUMINPUTS);

        // stitch the CameraSchemas
        //
        for (size_t i = 0; i < NUMINPUTS; i++)
        {
            ICameraSchema iSchema =
                ICamera(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();
            for (index_t reqIdx = 0; reqIdx < numSamples; reqIdx++)
            {
                oSchema.set(iSchema.getValue(reqIdx));
            }
            if (iSchema.isConstant())
                break;
        }
    }
    else if (ICurves::matches(header))
    {
        OCurvesSchema oSchema;
        init< ICurves,
              ICurvesSchema,
              OCurves,
              OCurvesSchema >(iObjects, oParentObj, oSchema, NUMINPUTS);

        // stitch the CurvesSchemas
        //
        for (size_t i = 0; i < NUMINPUTS; i++)
        {
            ICurvesSchema iSchema =
                ICurves(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            IV2fGeomParam iUVs = iSchema.getUVsParam();
            IN3fGeomParam iNormals = iSchema.getNormalsParam();
            IFloatGeomParam iWidths = iSchema.getWidthsParam();
            index_t numSamples = iSchema.getNumSamples();

            for (index_t reqIdx = 0; reqIdx < numSamples; reqIdx++)
            {
                ICurvesSchema::Sample iSamp = iSchema.getValue(reqIdx);

                OCurvesSchema::Sample oSamp;
                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);
                oSamp.setType(iSamp.getType());
                Abc::Int32ArraySamplePtr curvsNumPtr = iSamp.getCurvesNumVertices();
                if (curvsNumPtr)
                    oSamp.setCurvesNumVertices(*curvsNumPtr);
                oSamp.setWrap(iSamp.getWrap());
                oSamp.setBasis(iSamp.getBasis());

                IFloatGeomParam::Sample iWidthSample;
                OFloatGeomParam::Sample oWidthSample;
                if (iWidths)
                {
                    getOGeomParamSamp <IFloatGeomParam, IFloatGeomParam::Sample,
                        OFloatGeomParam::Sample>(iWidths, iWidthSample,
                                                 oWidthSample, reqIdx);
                    oSamp.setWidths(oWidthSample);
                }

                IV2fGeomParam::Sample iUVSample;
                OV2fGeomParam::Sample oUVSample;
                if (iUVs)
                {
                    getOGeomParamSamp <IV2fGeomParam, IV2fGeomParam::Sample,
                        OV2fGeomParam::Sample>(iUVs, iUVSample, 
                                               oUVSample, reqIdx);
                    oSamp.setUVs(oUVSample);
                }

                IN3fGeomParam::Sample iNormalsSample;
                ON3fGeomParam::Sample oNormalsSample;
                if (iNormals)
                {
                    getOGeomParamSamp <IN3fGeomParam, IN3fGeomParam::Sample,
                        ON3fGeomParam::Sample>(iNormals, iNormalsSample,
                                               oNormalsSample, reqIdx);
                    oSamp.setNormals(oNormalsSample);
                }

                oSamp.setChildBounds(iSamp.getChildBounds());
                oSchema.set(oSamp);
            }
            if (iSchema.isConstant())
                break;
        }
    }
    else if (IPoints::matches(header))
    {
        OPointsSchema oSchema;
        init< IPoints,
              IPointsSchema,
              OPoints,
              OPointsSchema >(iObjects, oParentObj, oSchema, NUMINPUTS);

        // stitch the PointsSchemas
        //
        for (size_t i = 0; i < NUMINPUTS; i++)
        {
            IPointsSchema iSchema =
                IPoints(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            IFloatGeomParam iWidths = iSchema.getWidthsParam();
            index_t numSamples = iSchema.getNumSamples();
            for (index_t reqIdx = 0; reqIdx < numSamples; reqIdx++)
            {
                IPointsSchema::Sample iSamp = iSchema.getValue(reqIdx);
                OPointsSchema::Sample oSamp;
                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);
                Abc::UInt64ArraySamplePtr idPtr = iSamp.getIds();
                if (idPtr)
                    oSamp.setIds(*idPtr);
                Abc::V3fArraySamplePtr velocPtr = iSamp.getVelocities();
                if (velocPtr)
                    oSamp.setPositions(*velocPtr);

                IFloatGeomParam::Sample iWidthSample;
                OFloatGeomParam::Sample oWidthSample;
                if (iWidths)
                {
                    getOGeomParamSamp <IFloatGeomParam, IFloatGeomParam::Sample,
                        OFloatGeomParam::Sample>(iWidths, iWidthSample,
                                                 oWidthSample, reqIdx);
                    oSamp.setWidths(oWidthSample);
                }

                oSamp.setChildBounds(iSamp.getChildBounds());
                oSchema.set(oSamp);
            }
            if (iSchema.isConstant())
                break;
        }
    }
    else if (INuPatch::matches(header))
    {
        ONuPatchSchema oSchema;
        init< INuPatch,
              INuPatchSchema,
              ONuPatch,
              ONuPatchSchema >(iObjects, oParentObj, oSchema, NUMINPUTS);

        // stitch the NuPatchSchemas
        //
        for (size_t i = 0; i < NUMINPUTS; i++)
        {
            INuPatchSchema iSchema =
                INuPatch(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();

            IN3fGeomParam normals = iSchema.getNormalsParam();
            IV2fGeomParam uvs = iSchema.getUVsParam();

            for (index_t reqIdx = 0; reqIdx < numSamples; reqIdx++)
            {
                INuPatchSchema::Sample iSamp = iSchema.getValue(reqIdx);
                ONuPatchSchema::Sample oSamp;

                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);

                oSamp.setNu(iSamp.getNumU());
                oSamp.setNv(iSamp.getNumV());
                oSamp.setUOrder(iSamp.getUOrder());
                oSamp.setVOrder(iSamp.getVOrder());

                Abc::FloatArraySamplePtr uKnotsPtr = iSamp.getUKnot();
                if (uKnotsPtr)
                    oSamp.setUKnot(*uKnotsPtr);

                Abc::FloatArraySamplePtr vKnotsPtr = iSamp.getVKnot();
                if (vKnotsPtr)
                    oSamp.setVKnot(*vKnotsPtr);

                // set normals
                if (normals && normals.isIndexed())
                {
                    IN3fGeomParam::Sample iNormal = normals.getIndexedValue(reqIdx);
                    ON3fGeomParam::Sample oNormal(*(iNormal.getVals()),
                                                  *(iNormal.getIndices()),
                                                  iNormal.getScope());
                    oSamp.setNormals(oNormal);
                }
                else if (normals)
                {
                    IN3fGeomParam::Sample iNormal = normals.getExpandedValue(reqIdx);
                    ON3fGeomParam::Sample oNormal(*(iNormal.getVals()),
                                                  iNormal.getScope());
                    oSamp.setNormals(oNormal);
                }

                // set uvs
                if (uvs && uvs.isIndexed())
                {
                    IV2fGeomParam::Sample iUV = uvs.getIndexedValue(reqIdx);
                    OV2fGeomParam::Sample oUV(*(iUV.getVals()),
                                                  *(iUV.getIndices()),
                                                  iUV.getScope());
                    oSamp.setUVs(oUV);
                }
                else if (uvs)
                {
                    IV2fGeomParam::Sample iUV = uvs.getExpandedValue(reqIdx);
                    OV2fGeomParam::Sample oUV(*(iUV.getVals()),
                                                  iUV.getScope());
                    oSamp.setUVs(oUV);
                }

                if (iSchema.hasTrimCurve())
                {
                    oSamp.setTrimCurve(iSamp.getTrimNumLoops(),
                                       *(iSamp.getTrimNumCurves()),
                                       *(iSamp.getTrimNumVertices()),
                                       *(iSamp.getTrimOrders()),
                                       *(iSamp.getTrimKnots()),
                                       *(iSamp.getTrimMins()),
                                       *(iSamp.getTrimMaxes()),
                                       *(iSamp.getTrimU()),
                                       *(iSamp.getTrimV()),
                                       *(iSamp.getTrimW()));
                }
                oSamp.setChildBounds(iSamp.getChildBounds());
                oSchema.set(oSamp);
            }
            if (iSchema.isConstant())
                break;
        }
    }
    else
    {
        std::cerr << iObjects[0].getFullName() << " is an unsupported schema" << std::endl;
    }
}

//-*****************************************************************************
//-*****************************************************************************
// DO IT.
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    if (argc < 4)
    {
        std::cerr << "USAGE: " << argv[0] << " outFile.abc inFile1.abc"
            << " inFile2.abc (inFile3.abc ...)" << std::endl;
        return -1;
    }

    {
        NUMINPUTS = argc - 2;

        minVec.reserve(NUMINPUTS);

        std::vector< IObject > iRoots;
        iRoots.reserve(NUMINPUTS);

        std::map< chrono_t, size_t > minIndexMap;

        for (int i = 2; i < argc; i++)
        {
            IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                argv[i], ErrorHandler::kThrowPolicy );
            IObject iRoot = archive.getTop();
            if (!iRoot.valid())
                return -1;

            // reorder the input files according to their mins
            chrono_t min = DBL_MAX;
            uint32_t numSamplings = archive.getNumTimeSamplings();
            if (numSamplings > 1)
            {
                // timesampling index 0 is special, so it will be skipped
                //
                // make sure all the other timesampling objects start at
                // the same time or throw here
                //
                min = archive.getTimeSampling(1)->getSampleTime(0);
                if (numSamplings > 2)
                {
                    for (uint32_t s = 2; s < numSamplings; s++)
                    {
                        chrono_t thisMin = archive.getTimeSampling(1)->getSampleTime(0);
                        if (fabs(thisMin - min) > ZERO)
                        {
                            std::cerr << "WARN: " << argv[i]
                                << " has more than 2 timesampling objects"
                                << " that don't start at the same time"
                                << std::endl;
                            return 1;
                        }
                    }
                }
                minVec.push_back(min);
                if (minIndexMap.count(min) == 0)
                {
                    minIndexMap.insert(std::make_pair(min, i-2));
                }
                else if (argv[2] != argv[i-2])
                {
                    std::cerr << "WARN: overlapping frame range between "
                        << argv[2] << " and " << argv[i-2] << std::endl;
                    return 1;
                }
            }

            if (iRoot.getNumChildren() == 1)
            {
                iRoots.push_back(iRoot.getChild(0));
            }
            else
            {
                std::cerr << "Error: " << argv[i] << " not valid" << std::endl;
                return 1;
            }
        }

        // now reorder the input nodes so they are in increasing order of their
        // min values in the frame range
        std::sort(minVec.begin(), minVec.end());
        std::vector< IObject > iOrderedRoots;
        iOrderedRoots.reserve(NUMINPUTS);
        // collect the top level compound property
        //
        ICompoundPropertyVec iCompoundProps;
        iCompoundProps.reserve(NUMINPUTS);
        //
        for (size_t f = 0; f < NUMINPUTS; f++)
        {
            size_t index = minIndexMap.find(minVec[f])->second;
            iOrderedRoots.push_back(iRoots[index]);

            ICompoundProperty cp = iRoots[index].getParent().getProperties();
            iCompoundProps.push_back(cp);
        }

        // Create an archive with the default writer
        OArchive oArchive(Alembic::AbcCoreHDF5::WriteArchive(),
            argv[1], ErrorHandler::kThrowPolicy);
        OObject oRoot = oArchive.getTop();
        if (!oRoot.valid())
            return -1;

        OCompoundProperty oCompoundProperty = oRoot.getProperties();
        stitchCompoundProp(iCompoundProps, oCompoundProperty);

        visitObjects(iOrderedRoots, oRoot);
    }

    return 0;
}
