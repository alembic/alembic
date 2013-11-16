//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/All.h>

#include "util.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace Alembic::AbcGeom;
using namespace Alembic::AbcCoreAbstract;

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
          ODataSchema & oSchema)
{
    const std::string fullNodeName = iObjects[0].getFullName();

    // gather information from the first input node in the list:
    IDataSchema iSchema0 = IData(iObjects[0], Alembic::Abc::kWrapExisting).getSchema();

    TimeSamplingPtr tsPtr0 = iSchema0.getTimeSampling();
    TimeSamplingType tsType0 = tsPtr0->getTimeSamplingType();
    checkAcyclic(tsType0, fullNodeName);

    ICompoundPropertyVec iCompoundProps;
    iCompoundProps.reserve(iObjects.size());

    ICompoundProperty cp = iObjects[0].getProperties();
    iCompoundProps.push_back(cp);

    ICompoundPropertyVec iArbGeomCompoundProps;
    iArbGeomCompoundProps.reserve(iObjects.size());

    ICompoundProperty arbProp = iSchema0.getArbGeomParams();
    if (arbProp)  // might be empty
        iArbGeomCompoundProps.push_back(arbProp);

    ICompoundPropertyVec iUserCompoundProps;
    iUserCompoundProps.reserve(iObjects.size());

    ICompoundProperty userProp = iSchema0.getUserProperties();
    if (userProp)  // might be empty
        iUserCompoundProps.push_back(userProp);

    ICompoundPropertyVec iSchemaProps;
    iSchemaProps.reserve(iObjects.size());

    Abc::IBox3dProperty childBounds = iSchema0.getChildBoundsProperty();
    TimeSamplingPtr ctsPtr0;
    TimeSamplingType ctsType0;
    if (childBounds)
    {
        ctsPtr0 = childBounds.getTimeSampling();
        ctsType0 = tsPtr0->getTimeSamplingType();
        std::string nameAndBounds = fullNodeName + " child bounds";
        checkAcyclic(ctsType0, nameAndBounds);
        iSchemaProps.push_back(iSchema0);
    }

    bool hasVisible = cp.getPropertyHeader("visible")?true:false;

    // sanity check (no frame range checking here)
    //      - timesamplying type has to be the same and can't be acyclic
    for (size_t i = 1; i < iObjects.size(); i++)
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

        ICompoundProperty userProp = iSchema.getUserProperties();
        if (userProp)  // might be empty
            iUserCompoundProps.push_back(userProp);

        Abc::IBox3dProperty childBounds = iSchema.getChildBoundsProperty();
        TimeSamplingPtr ctsPtr;
        TimeSamplingType ctsType;
        if (childBounds)
        {
            ctsPtr = childBounds.getTimeSampling();
            ctsType = ctsPtr->getTimeSamplingType();
            iSchemaProps.push_back(iSchema);
        }

        if (!(ctsType0 == ctsType))
        {
            std::cerr <<
                "Can not stitch different sampling type for child bounds on\""
                << fullNodeName << "\"" << std::endl;
            // more details on this
            if (ctsType.getNumSamplesPerCycle()
                != ctsType0.getNumSamplesPerCycle())
            {
                std::cerr << "\tnumSamplesPerCycle values are different"
                    << std::endl;
            }
            if (ctsType.getTimePerCycle() != ctsType0.getTimePerCycle())
            {
                std::cerr << "\ttimePerCycle values are different"
                    << std::endl;
            }
            if (!ctsPtr0 || !ctsPtr)
            {
                std::cerr << "\tchild bounds are missing on some archives"
                    << std::endl;
            }
            exit(1);
        }

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

    // stitch ArbGeomParams and User Properties
    //
    if (iArbGeomCompoundProps.size() == iObjects.size())
    {
        OCompoundProperty oArbGeomCompoundProp = oSchema.getArbGeomParams();
        stitchCompoundProp(iArbGeomCompoundProps, oArbGeomCompoundProp);
    }

    if (iUserCompoundProps.size() == iObjects.size())
    {
        OCompoundProperty oUserCompoundProp = oSchema.getUserProperties();
        stitchCompoundProp(iUserCompoundProps, oUserCompoundProp);
    }

    if (iSchemaProps.size() == iObjects.size())
    {
        stitchScalarProp(childBounds.getHeader(), iSchemaProps, oSchema);
    }
    else if (iSchemaProps.size() != 0)
    {
        std::cerr << "Child bounds are missing on some archives for:\""
            << fullNodeName << "\"" << std::endl;

        exit(1);
    }
}

};

//-*****************************************************************************
// a recursive function that reads all inputs and write to the given oObject
// node if there's no gap in the frame range for animated nodes
//
void visitObjects(std::vector< IObject > & iObjects, OObject & oParentObj)
{
    OObject outObj;

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
        OXformSchema oSchema;
        init< IXform,
              IXformSchema,
              OXform,
              OXformSchema >(iObjects, oParentObj, oSchema);
        outObj = oSchema.getObject();

        ICompoundPropertyVec iCompoundProps;
        iCompoundProps.reserve(iObjects.size());
        ICompoundProperty cp = iObjects[0].getProperties();
        iCompoundProps.push_back(cp);

        bool isLocator = cp.getPropertyHeader("locator")?true:false;

        for (size_t i = 1; i < iObjects.size(); i++)
        {
            ICompoundProperty cp = iObjects[i].getProperties();
            iCompoundProps.push_back(cp);
        }

        // stitch the operations if this is an xform node

        for (size_t i = 0; i < iObjects.size(); i++)
        {
            IXformSchema iSchema =
                IXform(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();
            index_t reqIdx = getIndexSample(oSchema.getNumSamples(),
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling());
            for (; reqIdx < numSamples; reqIdx++)
            {
                XformSample samp = iSchema.getValue(reqIdx);
                oSchema.set(samp);
            }
        }

        // stitch "locator" if it's a locator
        OCompoundProperty oCompoundProp = outObj.getProperties();
        if (isLocator)
        {
            const PropertyHeader * propHeaderPtr =
                iCompoundProps[0].getPropertyHeader("locator");
            stitchScalarProp(*propHeaderPtr,
                             iCompoundProps,
                             oCompoundProp);
        }
    }
    else if (ISubD::matches(header))
    {
        OSubDSchema oSchema;
        init< ISubD,
              ISubDSchema,
              OSubD,
              OSubDSchema >(iObjects, oParentObj, oSchema);
        outObj = oSchema.getObject();

        // stitch the SubDSchema
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            ISubDSchema iSchema =
                ISubD(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();
            IV2fGeomParam uvs = iSchema.getUVsParam();
            index_t reqIdx = getIndexSample(oSchema.getNumSamples(),
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling());
            for (; reqIdx < numSamples; reqIdx++)
            {
                ISubDSchema::Sample iSamp = iSchema.getValue(reqIdx);
                OSubDSchema::Sample oSamp;

                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);

                Abc::V3fArraySamplePtr velocPtr = iSamp.getVelocities();
                if (velocPtr)
                    oSamp.setVelocities(*velocPtr);

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

                // set uvs
                IV2fGeomParam::Sample iUVSample;
                OV2fGeomParam::Sample oUVSample;
                if (uvs)
                {
                    getOGeomParamSamp <IV2fGeomParam, IV2fGeomParam::Sample,
                        OV2fGeomParam::Sample>(uvs, iUVSample,
                                               oUVSample, reqIdx);
                    oSamp.setUVs(oUVSample);
                }

                oSchema.set(oSamp);
            }
        }
    }
    else if (IPolyMesh::matches(header))
    {
        OPolyMeshSchema oSchema;
        init< IPolyMesh,
              IPolyMeshSchema,
              OPolyMesh,
              OPolyMeshSchema >(iObjects, oParentObj, oSchema);
        outObj = oSchema.getObject();

        // stitch the PolySchema
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            IPolyMeshSchema iSchema =
                IPolyMesh(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();

            IN3fGeomParam normals = iSchema.getNormalsParam();
            IV2fGeomParam uvs = iSchema.getUVsParam();
            index_t reqIdx = getIndexSample(oSchema.getNumSamples(),
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling());
            for (; reqIdx < numSamples; reqIdx++)
            {
                IPolyMeshSchema::Sample iSamp = iSchema.getValue(reqIdx);
                OPolyMeshSchema::Sample oSamp;

                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);

                Abc::V3fArraySamplePtr velocPtr = iSamp.getVelocities();
                if (velocPtr)
                    oSamp.setVelocities(*velocPtr);

                Abc::Int32ArraySamplePtr faceIndicesPtr = iSamp.getFaceIndices();
                if (faceIndicesPtr)
                    oSamp.setFaceIndices(*faceIndicesPtr);

                Abc::Int32ArraySamplePtr faceCntPtr = iSamp.getFaceCounts();
                if (faceCntPtr)
                    oSamp.setFaceCounts(*faceCntPtr);

                // set uvs
                IV2fGeomParam::Sample iUVSample;
                OV2fGeomParam::Sample oUVSample;
                if (uvs)
                {
                    getOGeomParamSamp <IV2fGeomParam, IV2fGeomParam::Sample,
                        OV2fGeomParam::Sample>(uvs, iUVSample,
                                               oUVSample, reqIdx);
                    oSamp.setUVs(oUVSample);
                }

                // set normals
                IN3fGeomParam::Sample iNormalsSample;
                ON3fGeomParam::Sample oNormalsSample;
                if (normals)
                {
                    getOGeomParamSamp <IN3fGeomParam, IN3fGeomParam::Sample,
                        ON3fGeomParam::Sample>(normals, iNormalsSample,
                                               oNormalsSample, reqIdx);
                    oSamp.setNormals(oNormalsSample);
                }

                oSchema.set(oSamp);
            }
        }
    }
    else if (ICamera::matches(header))
    {
        OCameraSchema oSchema;
        init< ICamera,
              ICameraSchema,
              OCamera,
              OCameraSchema >(iObjects, oParentObj, oSchema);
        outObj = oSchema.getObject();

        // stitch the CameraSchemas
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            ICameraSchema iSchema =
                ICamera(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();
            index_t reqIdx = getIndexSample(oSchema.getNumSamples(),
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling());
            for (; reqIdx < numSamples; reqIdx++)
            {
                oSchema.set(iSchema.getValue(reqIdx));
            }
        }
    }
    else if (ICurves::matches(header))
    {
        OCurvesSchema oSchema;
        init< ICurves,
              ICurvesSchema,
              OCurves,
              OCurvesSchema >(iObjects, oParentObj, oSchema);
        outObj = oSchema.getObject();

        // stitch the CurvesSchemas
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            ICurvesSchema iSchema =
                ICurves(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            IV2fGeomParam iUVs = iSchema.getUVsParam();
            IN3fGeomParam iNormals = iSchema.getNormalsParam();
            IFloatGeomParam iWidths = iSchema.getWidthsParam();
            IFloatArrayProperty iKnots = iSchema.getKnotsProperty();
            IUcharArrayProperty iOrders = iSchema.getOrdersProperty();

            index_t numSamples = iSchema.getNumSamples();

            index_t reqIdx = getIndexSample(oSchema.getNumSamples(),
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling());
            for (; reqIdx < numSamples; reqIdx++)
            {
                ICurvesSchema::Sample iSamp = iSchema.getValue(reqIdx);

                OCurvesSchema::Sample oSamp;
                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);

                Abc::V3fArraySamplePtr velocPtr = iSamp.getVelocities();
                if (velocPtr)
                    oSamp.setVelocities(*velocPtr);

                oSamp.setType(iSamp.getType());
                Abc::Int32ArraySamplePtr curvsNumPtr = iSamp.getCurvesNumVertices();
                if (curvsNumPtr)
                    oSamp.setCurvesNumVertices(*curvsNumPtr);
                oSamp.setWrap(iSamp.getWrap());
                oSamp.setBasis(iSamp.getBasis());

                Abc::FloatArraySamplePtr knotsPtr = iSamp.getKnots();
                if (knotsPtr)
                {
                    oSamp.setKnots(*knotsPtr);
                }

                Abc::UcharArraySamplePtr ordersPtr = iSamp.getOrders();
                if (ordersPtr)
                {
                    oSamp.setOrders(*ordersPtr);
                }

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

                oSchema.set(oSamp);
            }
        }
    }
    else if (IPoints::matches(header))
    {
        OPointsSchema oSchema;
        init< IPoints,
              IPointsSchema,
              OPoints,
              OPointsSchema >(iObjects, oParentObj, oSchema);
        outObj = oSchema.getObject();

        // stitch the PointsSchemas
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            IPointsSchema iSchema =
                IPoints(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            IFloatGeomParam iWidths = iSchema.getWidthsParam();
            index_t numSamples = iSchema.getNumSamples();
            index_t reqIdx = getIndexSample(oSchema.getNumSamples(),
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling());
            for (; reqIdx < numSamples; reqIdx++)
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
                    oSamp.setVelocities(*velocPtr);

                IFloatGeomParam::Sample iWidthSample;
                OFloatGeomParam::Sample oWidthSample;
                if (iWidths)
                {
                    getOGeomParamSamp <IFloatGeomParam, IFloatGeomParam::Sample,
                        OFloatGeomParam::Sample>(iWidths, iWidthSample,
                                                 oWidthSample, reqIdx);
                    oSamp.setWidths(oWidthSample);
                }

                oSchema.set(oSamp);
            }
        }
    }
    else if (INuPatch::matches(header))
    {
        ONuPatchSchema oSchema;
        init< INuPatch,
              INuPatchSchema,
              ONuPatch,
              ONuPatchSchema >(iObjects, oParentObj, oSchema);
        outObj = oSchema.getObject();

        // stitch the NuPatchSchemas
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            INuPatchSchema iSchema =
                INuPatch(iObjects[i], Alembic::Abc::kWrapExisting).getSchema();
            index_t numSamples = iSchema.getNumSamples();

            IN3fGeomParam normals = iSchema.getNormalsParam();
            IV2fGeomParam uvs = iSchema.getUVsParam();

            index_t reqIdx = getIndexSample(oSchema.getNumSamples(),
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling());
            for (; reqIdx < numSamples; reqIdx++)
            {
                INuPatchSchema::Sample iSamp = iSchema.getValue(reqIdx);
                ONuPatchSchema::Sample oSamp;

                Abc::P3fArraySamplePtr posPtr = iSamp.getPositions();
                if (posPtr)
                    oSamp.setPositions(*posPtr);

                Abc::V3fArraySamplePtr velocPtr = iSamp.getVelocities();
                if (velocPtr)
                    oSamp.setVelocities(*velocPtr);

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

                IV2fGeomParam::Sample iUVSample;
                OV2fGeomParam::Sample oUVSample;
                if (uvs)
                {
                    getOGeomParamSamp <IV2fGeomParam, IV2fGeomParam::Sample,
                        OV2fGeomParam::Sample>(uvs, iUVSample,
                                               oUVSample, reqIdx);
                    oSamp.setUVs(oUVSample);
                }

                IN3fGeomParam::Sample iNormalsSample;
                ON3fGeomParam::Sample oNormalsSample;
                if (normals)
                {
                    getOGeomParamSamp <IN3fGeomParam, IN3fGeomParam::Sample,
                        ON3fGeomParam::Sample>(normals, iNormalsSample,
                                               oNormalsSample, reqIdx);
                    oSamp.setNormals(oNormalsSample);
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
                oSchema.set(oSamp);
            }
        }
    }
    else
    {
        outObj = OObject(oParentObj, header.getName(), header.getMetaData());

        // collect the top level compound property
        ICompoundPropertyVec iCompoundProps(iObjects.size());
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            iCompoundProps[i] = iObjects[i].getProperties();
        }

        OCompoundProperty oCompoundProperty = outObj.getProperties();
        stitchCompoundProp(iCompoundProps, oCompoundProperty);
    }

    // After done writing THIS OObject node, if input nodes have children,
    // go deeper.
    // Otherwise we are done here
    size_t numChildren =  iObjects[0].getNumChildren();

    // check to make sure all of our iObjects have the same number of children
    for (size_t j = 1; j < iObjects.size(); j++)
    {
        if (numChildren != iObjects[j].getNumChildren())
        {
            std::cerr << "ERROR: " << iObjects[j].getFullName() << " in " <<
                iObjects[j].getArchive().getName() <<
                " has a different number of children than " <<
                iObjects[0].getFullName() << " in " <<
                iObjects[0].getArchive().getName() << std::endl;

            exit(1);
        }
    }

    for (size_t i = 0 ; i < numChildren; i++ )
    {
        std::vector< IObject > iChildObjects;
        for (size_t f = 0; f < iObjects.size(); f++)
        {
            iChildObjects.push_back(iObjects[f].getChild(i));
        }
        visitObjects(iChildObjects, outObj);
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
        size_t numInputs = argc - 2;
        std::vector< chrono_t > minVec;

        minVec.reserve(numInputs);

        std::vector< IArchive > iArchives;
        iArchives.reserve(numInputs);

        std::map< chrono_t, size_t > minIndexMap;
        size_t rootChildren = 0;

        Alembic::AbcCoreFactory::IFactory factory;
        factory.setPolicy(ErrorHandler::kThrowPolicy);
        Alembic::AbcCoreFactory::IFactory::CoreType coreType;

        for (int i = 2; i < argc; ++i)
        {

            IArchive archive = factory.getArchive(argv[i], coreType);
            if (!archive.valid() || archive.getTop().getNumChildren() < 1)
            {
                std::cerr << "ERROR: " << argv[i] <<
                    " not a valid Alembic file" << std::endl;
                return 1;
            }

            IObject iRoot = archive.getTop();
            size_t numChildren = iRoot.getNumChildren();

            if (i == 2)
            {
                rootChildren = numChildren;
            }
            else if (rootChildren != numChildren)
            {
                std::cerr << "ERROR: " << argv[i] <<
                    " doesn't have the same number of children as: " <<
                    argv[i-1] << std::endl;
            }

            // reorder the input files according to their mins
            chrono_t min = DBL_MAX;
            Alembic::Util::uint32_t numSamplings = archive.getNumTimeSamplings();
            if (numSamplings > 1)
            {
                // timesampling index 0 is special, so it will be skipped
                //
                // make sure all the other timesampling objects start at
                // the same time or throw here
                //
                min = archive.getTimeSampling(1)->getSampleTime(0);

                for (Alembic::Util::uint32_t s = 2; s < numSamplings; ++s)
                {
                    chrono_t thisMin =
                        archive.getTimeSampling(s)->getSampleTime(0);

                    if (fabs(thisMin - min) > 1e-5)
                    {
                        std::cerr << "ERROR: " << argv[i]
                            << " has non-default TimeSampling objects"
                            << " that don't start at the same time."
                            << std::endl;
                        return 1;
                    }
                }

                minVec.push_back(min);
                if (minIndexMap.count(min) == 0)
                {
                    minIndexMap.insert(std::make_pair(min, i-2));
                }
                else if (argv[2] != argv[i])
                {
                    std::cerr << "ERROR: overlapping frame range between "
                        << argv[2] << " and " << argv[i] << std::endl;
                    return 1;
                }
            }
            else
            {
                std::cerr << "ERROR: " << archive.getName() <<
                    " only has default (static) TimeSampling." << std::endl;
                return 1;
            }

            iArchives.push_back(archive);
        }

        // now reorder the input nodes so they are in increasing order of their
        // min values in the frame range
        std::sort(minVec.begin(), minVec.end());
        std::vector< IArchive > iOrderedArchives;
        iOrderedArchives.reserve(numInputs);

        for (size_t f = 0; f < numInputs; ++f)
        {
            size_t index = minIndexMap.find(minVec[f])->second;
            iOrderedArchives.push_back(iArchives[index]);
        }

        std::string appWriter = "AbcStitcher";
        std::string fileName = argv[1];
        std::string userStr;

        // Create an archive with the default writer
        OArchive oArchive;
        if (coreType == Alembic::AbcCoreFactory::IFactory::kHDF5)
        {
            oArchive = CreateArchiveWithInfo(
                Alembic::AbcCoreHDF5::WriteArchive(),
                fileName, appWriter, userStr, ErrorHandler::kThrowPolicy);
        }
        else if (coreType == Alembic::AbcCoreFactory::IFactory::kOgawa)
        {
            oArchive = CreateArchiveWithInfo(
                Alembic::AbcCoreOgawa::WriteArchive(),
                fileName, appWriter, userStr, ErrorHandler::kThrowPolicy);
        }

        OObject oRoot = oArchive.getTop();
        if (!oRoot.valid())
            return -1;

        std::vector<IObject> iRoots;
        iRoots.resize(numInputs);
        for (size_t f = 0; f < rootChildren; ++f)
        {
            for (size_t g = 0; g < numInputs; ++g)
            {
                iRoots[g] = iOrderedArchives[g].getTop().getChild(f);
            }

            visitObjects(iRoots, oRoot);
        }

        // collect the top level compound property
        ICompoundPropertyVec iCompoundProps;
        iCompoundProps.reserve(numInputs);
        for (size_t f = 0; f < numInputs; ++f)
        {
            iCompoundProps.push_back(iRoots[f].getParent().getProperties());
        }

        OCompoundProperty oCompoundProperty = oRoot.getProperties();
        stitchCompoundProp(iCompoundProps, oCompoundProperty);

    }

    return 0;
}
