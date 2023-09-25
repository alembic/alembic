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

#include <Alembic/AbcGeom/All.h>

#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif

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
                          OCompoundProperty & oCompoundProp,
                          const TimeAndSamplesMap & iTimeMap)
{
    const PropertyHeader * propHeaderPtr =
            iCompoundProps[0].getPropertyHeader("visible");
    stitchScalarProp(*propHeaderPtr, iCompoundProps, oCompoundProp, iTimeMap);
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
          ODataSchema & oSchema, const TimeAndSamplesMap & iTimeMap,
          std::size_t & oTotalSamples)
{

    // find the first valid IObject
    IObject inObj;
    for (std::size_t i = 0; i < iObjects.size(); ++i)
    {
        if (iObjects[i].valid())
        {
            inObj = iObjects[i];
            break;
        }
    }

    const std::string fullNodeName = inObj.getFullName();

    // gather information from the first input node in the list:
    IDataSchema iSchema0 = IData(inObj).getSchema();

    TimeSamplingPtr tsPtr0 = iTimeMap.get(iSchema0.getTimeSampling(),
                                          oTotalSamples);

    TimeSamplingType tsType0 = tsPtr0->getTimeSamplingType();
    checkAcyclic(tsType0, fullNodeName);

    ICompoundPropertyVec iCompoundProps;
    iCompoundProps.reserve(iObjects.size());

    ICompoundPropertyVec iArbGeomCompoundProps;
    iArbGeomCompoundProps.reserve(iObjects.size());

    ICompoundPropertyVec iUserCompoundProps;
    iUserCompoundProps.reserve(iObjects.size());

    ICompoundPropertyVec iSchemaProps;
    iSchemaProps.reserve(iObjects.size());

    Abc::IBox3dProperty childBounds = iSchema0.getChildBoundsProperty();
    TimeSamplingPtr ctsPtr0;
    TimeSamplingType ctsType0;
    if (childBounds)
    {
        ctsPtr0 = childBounds.getTimeSampling();
        ctsType0 = ctsPtr0->getTimeSamplingType();
        std::string nameAndBounds = fullNodeName + " child bounds";
        checkAcyclic(ctsType0, nameAndBounds);
    }

    bool hasVisible = inObj.getProperties().getPropertyHeader("visible") != NULL;

    // sanity check (no frame range checking here)
    //      - timesamplying type has to be the same and can't be acyclic
    for (size_t i = 0; i < iObjects.size(); i++)
    {
        if (!iObjects[i].valid())
        {
            continue;
        }

        IDataSchema iSchema =
            IData(iObjects[i]).getSchema();

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

    OData oData(oParentObj, inObj.getName(), tsPtr0);
    oSchema = oData.getSchema();

    // stitch "visible" if it's points
    //
    if (hasVisible)
    {
        OCompoundProperty oCompoundProp = oData.getProperties();
        stitchVisible(iCompoundProps, oCompoundProp, iTimeMap);
    }

    // stitch ArbGeomParams and User Properties
    //
    if (iArbGeomCompoundProps.size() == iObjects.size())
    {
        OCompoundProperty oArbGeomCompoundProp = oSchema.getArbGeomParams();
        stitchCompoundProp(iArbGeomCompoundProps, oArbGeomCompoundProp, iTimeMap);
    }

    if (iUserCompoundProps.size() == iObjects.size())
    {
        OCompoundProperty oUserCompoundProp = oSchema.getUserProperties();
        stitchCompoundProp(iUserCompoundProps, oUserCompoundProp, iTimeMap);
    }

    if (!iSchemaProps.empty())
    {
        stitchScalarProp(childBounds.getHeader(), iSchemaProps, oSchema, iTimeMap);
    }
}

};

//-*****************************************************************************
// a recursive function that reads all inputs and write to the given oObject
// node if there's no gap in the frame range for animated nodes
//
void visitObjects(std::vector< IObject > & iObjects, OObject & oParentObj,
                  const TimeAndSamplesMap & iTimeMap, bool atRoot)
{
    OObject outObj;

    IObject inObj;
    std::vector< IObject >::iterator it;
    for (it = iObjects.begin(); it != iObjects.end(); ++it)
    {
        if (it->valid())
        {
            inObj = *it;
            break;
        }
    }

    assert(inObj.valid());

    if (iTimeMap.isVerbose())
    {
        std::cout << inObj.getFullName() << std::endl;
    }

    const AbcA::ObjectHeader & header = inObj.getHeader();
    std::size_t totalSamples = 0;

    // keep track of how many we have set so far to work around issue 430
    std::size_t totalSet = 0;

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
        init< IXform, IXformSchema, OXform, OXformSchema >(
            iObjects, oParentObj, oSchema, iTimeMap, totalSamples);

        outObj = oSchema.getObject();

        ICompoundPropertyVec iCompoundProps;
        iCompoundProps.reserve(iObjects.size());

        const PropertyHeader * locHeader = NULL;

        for (size_t i = 0; i < iObjects.size(); i++)
        {
            if (!iObjects[i].valid())
            {
                iCompoundProps.push_back(ICompoundProperty());
                continue;
            }

            ICompoundProperty cp = iObjects[i].getProperties();
            iCompoundProps.push_back(cp);

            const PropertyHeader * childLocHeader =
                cp.getPropertyHeader("locator");
            if (!locHeader && childLocHeader)
            {
                locHeader = childLocHeader;
            }
        }

        // stitch the operations if this is an xform node
        size_t i = 0;
        for (i = 0; i < iObjects.size(); i++)
        {
            if (!iObjects[i].valid())
            {
                continue;
            }

            IXformSchema iSchema =
                IXform(iObjects[i]).getSchema();
            index_t numSamples = iSchema.getNumSamples();
            index_t numEmpty = 0;
            index_t reqIdx = getIndexSample(totalSet,
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling(), numEmpty);

            // write empties only if we are also writing a sample, as the
            // first sample will be repeated over and over again
            for (index_t emptyIdx = 0;
                 reqIdx < numSamples && emptyIdx < numEmpty; ++emptyIdx)
            {
                XformSample samp = iSchema.getValue(reqIdx);
                oSchema.set(samp);
            }
            totalSet += numEmpty;

            for (; reqIdx < numSamples; reqIdx++, totalSet++)
            {
                XformSample samp = iSchema.getValue(reqIdx);
                oSchema.set(samp);
            }
        }

        // make sure we've set a sample, if we are going to extend them
        for (i = totalSet; i != 0 && i < totalSamples; ++i)
        {
            oSchema.setFromPrevious();
        }

        // stitch "locator" if it's a locator
        OCompoundProperty oCompoundProp = outObj.getProperties();
        if (locHeader)
        {
            stitchScalarProp(*locHeader, iCompoundProps, oCompoundProp,
                             iTimeMap);
        }
    }
    else if (ISubD::matches(header))
    {
        OSubDSchema oSchema;
        init< ISubD, ISubDSchema, OSubD, OSubDSchema >(
            iObjects, oParentObj, oSchema, iTimeMap, totalSamples);
        outObj = oSchema.getObject();

        OSubDSchema::Sample emptySample(P3fArraySample::emptySample(),
            Int32ArraySample::emptySample(), Int32ArraySample::emptySample());

        // stitch the SubDSchema
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            if (!iObjects[i].valid())
            {
                continue;
            }

            ISubDSchema iSchema =
                ISubD(iObjects[i]).getSchema();
            index_t numSamples = iSchema.getNumSamples();
            IV2fGeomParam uvs = iSchema.getUVsParam();
            if (totalSet == 0 && uvs)
            {
                oSchema.setUVSourceName(GetSourceName(uvs.getMetaData()));
            }
            index_t numEmpty = 0;
            index_t reqIdx = getIndexSample(totalSet,
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling(), numEmpty);

            // not hold, then set empties, other set previous if we have previous samples
            if (!iTimeMap.isHold() || totalSet > 0)
            {
                for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                {
                    if (!iTimeMap.isHold())
                    {
                        oSchema.set(emptySample);
                    }
                    else
                    {
                        oSchema.setFromPrevious();
                    }
                }
                totalSet += numEmpty;
            }

            for (; reqIdx < numSamples; reqIdx++, totalSet++)
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
                    oSamp.setCornerSharpnesses(*cornerSpPtr);

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
                // our first sample was written, AND we want to hold so set our previous samples to this one!
                if (iTimeMap.isHold() && totalSet == 0)
                {
                    for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                    {
                        oSchema.setFromPrevious();
                    }
                    totalSet += numEmpty;
                }
            }
        }

        for (size_t i = totalSet; i < totalSamples; ++i)
        {
            if (!iTimeMap.isHold())
            {
                oSchema.set(emptySample);
            }
            else
            {
                oSchema.setFromPrevious();
            }
        }
    }
    else if (IPolyMesh::matches(header))
    {

        OPolyMeshSchema oSchema;
        init< IPolyMesh, IPolyMeshSchema, OPolyMesh, OPolyMeshSchema >(
            iObjects, oParentObj, oSchema, iTimeMap, totalSamples);
        outObj = oSchema.getObject();

        OPolyMeshSchema::Sample emptySample(P3fArraySample::emptySample(),
            Int32ArraySample::emptySample(), Int32ArraySample::emptySample());

        // stitch the PolySchema
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {

            if (!iObjects[i].valid())
            {
                continue;
            }

            IPolyMeshSchema iSchema =
                IPolyMesh(iObjects[i]).getSchema();
            index_t numSamples = iSchema.getNumSamples();

            IN3fGeomParam normals = iSchema.getNormalsParam();
            IV2fGeomParam uvs = iSchema.getUVsParam();
            if (totalSet == 0 && uvs)
            {
                oSchema.setUVSourceName(GetSourceName(uvs.getMetaData()));
            }

            index_t numEmpty = 0;
            index_t reqIdx = getIndexSample(totalSet,
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling(), numEmpty);

            // not hold, then set empties, other set previous if we have previous samples
            if (!iTimeMap.isHold() || totalSet > 0)
            {
                for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                {
                    if (!iTimeMap.isHold())
                    {
                        oSchema.set(emptySample);
                    }
                    else
                    {
                        oSchema.setFromPrevious();
                    }
                }
                totalSet += numEmpty;
            }

            for (; reqIdx < numSamples; reqIdx++, totalSet++)
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

                // our first sample was written, AND we want to hold so set our previous samples to this one!
                if (iTimeMap.isHold() && totalSet == 0)
                {
                    for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                    {
                        oSchema.setFromPrevious();
                    }
                    totalSet += numEmpty;
                }
            }
        }

        for (size_t i = totalSet; i < totalSamples; ++i)
        {
            if (!iTimeMap.isHold())
            {
                oSchema.set(emptySample);
            }
            else
            {
                oSchema.setFromPrevious();
            }
        }
    }
    else if (ICamera::matches(header))
    {
        OCameraSchema oSchema;
        init< ICamera, ICameraSchema, OCamera, OCameraSchema >(
            iObjects, oParentObj, oSchema, iTimeMap, totalSamples);

        outObj = oSchema.getObject();

        // stitch the CameraSchemas, we dont need to worry about isHold
        // because we set from previous here
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            if (!iObjects[i].valid())
            {
                continue;
            }

            ICameraSchema iSchema =
                ICamera(iObjects[i]).getSchema();
            index_t numSamples = iSchema.getNumSamples();
            index_t numEmpty = 0;
            index_t reqIdx = getIndexSample(totalSet,
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling(), numEmpty);

            // write empties only if we are also writing a sample, as the
            // first sample will be repeated over and over again
            for (index_t emptyIdx = 0;
                 reqIdx < numSamples && emptyIdx < numEmpty; ++emptyIdx)
            {
                oSchema.set(iSchema.getValue(reqIdx));
            }
            totalSet += numEmpty;

            for (; reqIdx < numSamples; reqIdx++, totalSet++)
            {
                oSchema.set(iSchema.getValue(reqIdx));
            }
        }

        // for the rest of the samples just set the last one as long as
        // a sample has been already set
        for (size_t i = totalSet; i != 0 && i < totalSamples;++i)
        {
            oSchema.setFromPrevious();
        }
    }
    else if (ICurves::matches(header))
    {
        OCurvesSchema oSchema;
        init< ICurves, ICurvesSchema, OCurves, OCurvesSchema >(
            iObjects, oParentObj, oSchema, iTimeMap, totalSamples);
        outObj = oSchema.getObject();
        OCurvesSchema::Sample emptySample(P3fArraySample::emptySample(),
            Int32ArraySample::emptySample());

        // stitch the CurvesSchemas
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            if (!iObjects[i].valid())
            {
                continue;
            }
            ICurvesSchema iSchema =
                ICurves(iObjects[i]).getSchema();
            IV2fGeomParam iUVs = iSchema.getUVsParam();
            IN3fGeomParam iNormals = iSchema.getNormalsParam();
            IFloatGeomParam iWidths = iSchema.getWidthsParam();
            IFloatArrayProperty iKnots = iSchema.getKnotsProperty();
            IUcharArrayProperty iOrders = iSchema.getOrdersProperty();

            index_t numSamples = iSchema.getNumSamples();

            index_t numEmpty = 0;
            index_t reqIdx = getIndexSample(totalSet,
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling(), numEmpty);

            // not hold, then set empties, other set previous if we have previous samples
            if (!iTimeMap.isHold() || totalSet > 0)
            {
                for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                {
                    if (!iTimeMap.isHold())
                    {
                        oSchema.set(emptySample);
                    }
                    else
                    {
                        oSchema.setFromPrevious();
                    }
                }
                totalSet += numEmpty;
            }

            for (; reqIdx < numSamples; reqIdx++, totalSet++)
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

                // our first sample was written, AND we want to hold so set our previous samples to this one!
                if (iTimeMap.isHold() && totalSet == 0)
                {
                    for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                    {
                        oSchema.setFromPrevious();
                    }
                    totalSet += numEmpty;
                }
            }
        }

        for (size_t i = totalSet; i < totalSamples; ++i)
        {
            if (!iTimeMap.isHold())
            {
                oSchema.set(emptySample);
            }
            else
            {
                oSchema.setFromPrevious();
            }
        }
    }
    else if (IPoints::matches(header))
    {
        OPointsSchema oSchema;
        init< IPoints, IPointsSchema, OPoints, OPointsSchema >(
            iObjects, oParentObj, oSchema, iTimeMap, totalSamples);
        outObj = oSchema.getObject();
        OPointsSchema::Sample emptySample(P3fArraySample::emptySample(),
            UInt64ArraySample::emptySample());

        // stitch the PointsSchemas
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            if (!iObjects[i].valid())
            {
                continue;
            }

            IPointsSchema iSchema =
                IPoints(iObjects[i]).getSchema();
            IFloatGeomParam iWidths = iSchema.getWidthsParam();
            index_t numSamples = iSchema.getNumSamples();
            index_t numEmpty = 0;
            index_t reqIdx = getIndexSample(totalSet,
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling(), numEmpty);

            // not hold, then set empties, other set previous if we have previous samples
            if (!iTimeMap.isHold() || totalSet > 0)
            {
                for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                {
                    if (!iTimeMap.isHold())
                    {
                        oSchema.set(emptySample);
                    }
                    else
                    {
                        oSchema.setFromPrevious();
                    }
                }
                totalSet += numEmpty;
            }

            for (; reqIdx < numSamples; reqIdx++, totalSet++)
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

                // our first sample was written, AND we want to hold so set our previous samples to this one!
                if (iTimeMap.isHold() && totalSet == 0)
                {
                    for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                    {
                        oSchema.setFromPrevious();
                    }
                    totalSet += numEmpty;
                }
            }
        }

        for (size_t i = totalSet; i < totalSamples; ++i)
        {
            if (!iTimeMap.isHold())
            {
                oSchema.set(emptySample);
            }
            else
            {
                oSchema.setFromPrevious();
            }
        }
    }
    else if (INuPatch::matches(header))
    {
        ONuPatchSchema oSchema;
        init< INuPatch, INuPatchSchema, ONuPatch, ONuPatchSchema >(
            iObjects, oParentObj, oSchema, iTimeMap, totalSamples);
        outObj = oSchema.getObject();

        Alembic::Util::int32_t zeroVal = 0;
        ONuPatchSchema::Sample emptySample(P3fArraySample::emptySample(),
            zeroVal, zeroVal, zeroVal, zeroVal,
            FloatArraySample::emptySample(), FloatArraySample::emptySample());

        // stitch the NuPatchSchemas
        //
        for (size_t i = 0; i < iObjects.size(); i++)
        {
            if (!iObjects[i].valid())
            {
                continue;
            }

            INuPatchSchema iSchema =
                INuPatch(iObjects[i]).getSchema();
            index_t numSamples = iSchema.getNumSamples();

            IN3fGeomParam normals = iSchema.getNormalsParam();
            IV2fGeomParam uvs = iSchema.getUVsParam();

            index_t numEmpty = 0;
            index_t reqIdx = getIndexSample(totalSet,
                oSchema.getTimeSampling(), numSamples,
                iSchema.getTimeSampling(), numEmpty);

            // not hold, then set empties, other set previous if we have previous samples
            if (!iTimeMap.isHold() || totalSet > 0)
            {
                for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                {
                    if (!iTimeMap.isHold())
                    {
                        oSchema.set(emptySample);
                    }
                    else
                    {
                        oSchema.setFromPrevious();
                    }
                }
                totalSet += numEmpty;
            }

            for (; reqIdx < numSamples; reqIdx++, totalSet++)
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

                // our first sample was written, AND we want to hold so set our previous samples to this one!
                if (iTimeMap.isHold() && totalSet == 0)
                {
                    for (index_t emptyIdx = 0; emptyIdx < numEmpty; ++emptyIdx)
                    {
                        oSchema.setFromPrevious();
                    }
                    totalSet += numEmpty;
                }
            }
        }

        for (size_t i = totalSet; i < totalSamples; ++i)
        {
            if (!iTimeMap.isHold())
            {
                oSchema.set(emptySample);
            }
            else
            {
                oSchema.setFromPrevious();
            }
        }
    }
    else
    {
        if (!atRoot)
        {
            outObj = OObject(oParentObj, header.getName(), header.getMetaData());
        }
        else
        {
            // for stitching properties of the top level objects
            outObj = oParentObj;
        }

        // collect the top level compound property
        ICompoundPropertyVec iCompoundProps(iObjects.size());
        for (size_t i = 0; i < iObjects.size(); ++i)
        {
            if (!iObjects[i].valid())
            {
                continue;
            }

            iCompoundProps[i] = iObjects[i].getProperties();
        }

        OCompoundProperty oCompoundProperty = outObj.getProperties();
        stitchCompoundProp(iCompoundProps, oCompoundProperty, iTimeMap);
    }

    // After done writing THIS OObject node, if input nodes have children,
    // go deeper, otherwise we are done here
    for (size_t i = 0 ; i < iObjects.size(); i++ )
    {
        if (!iObjects[i].valid())
        {
            continue;
        }

        for (size_t j = 0; j < iObjects[i].getNumChildren(); ++j)
        {
            std::vector< IObject > childObjects;
            std::string childName = iObjects[i].getChildHeader(j).getName();
            // skip names that we've already written out
            if (outObj.getChildHeader(childName) != NULL)
            {
                continue;
            }

            for (size_t k =i; k < iObjects.size(); ++k)
            {
                childObjects.push_back(iObjects[k].getChild(childName));
            }

            visitObjects(childObjects, outObj, iTimeMap, false);
        }
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
        std::cerr << "USAGE: " << argv[0] << " [-v] [-hold] outFile.abc inFile1.abc"
            << " inFile2.abc (inFile3.abc ...)" << std::endl;
        std::cerr << "Where -v is a verbosity flag which prints the IObject"
            << " being processed." << std::endl;
        std::cerr << "And -hold flags whether to fill in the first or previous "
            << "sample instead of an empty one for missing samples." << std::endl;
        return -1;
    }

    {
        size_t numInputs = argc - 2;

        std::string fileName;
        TimeAndSamplesMap timeMap;

        std::vector< chrono_t > minVec;
        minVec.reserve(numInputs);

        std::vector< IArchive > iArchives;
        iArchives.reserve(numInputs);

        std::map< chrono_t, size_t > minIndexMap;

        Alembic::AbcCoreFactory::IFactory factory;
        factory.setPolicy(ErrorHandler::kThrowPolicy);
        Alembic::AbcCoreFactory::IFactory::CoreType coreType;

        for (int i = 1; i < argc; ++i)
        {
            // check for optional verbose
            if (argv[i] == std::string("-v"))
            {
                timeMap.setVerbose(true);
                numInputs --;
                continue;
            }
            // check for optional hold
            else if (argv[i] == std::string("-hold"))
            {
                timeMap.setHold(true);
                numInputs --;
                continue;
            }
            // first non flag argument is our output file name
            else if (fileName.empty())
            {
                fileName = argv[i];
                continue;
            }

            IArchive archive = factory.getArchive(argv[i], coreType);
            if (!archive.valid())
            {
                std::cerr << "ERROR: " << argv[i] <<
                    " not a valid Alembic file" << std::endl;
                return 1;
            }

            // reorder the input files according to their mins
            chrono_t min = DBL_MAX;
            Alembic::Util::uint32_t numSamplings = archive.getNumTimeSamplings();
            timeMap.add(archive.getTimeSampling(0),
                        archive.getMaxNumSamplesForTimeSamplingIndex(0));

            if (numSamplings > 1)
            {
                // timesampling index 0 is special, so it will be skipped
                // use the first time on the next time sampling to determine
                // our archive order the archive order
                min = archive.getTimeSampling(1)->getSampleTime(0);

                for (Alembic::Util::uint32_t s = 1; s < numSamplings; ++s)
                {
                    timeMap.add(archive.getTimeSampling(s),
                        archive.getMaxNumSamplesForTimeSamplingIndex(s));
                }

                minVec.push_back(min);
                if (minIndexMap.count(min) == 0)
                {
                    minIndexMap.insert(std::make_pair(min, minVec.size() - 1));
                }
                else if (!iArchives.empty())
                {
                    std::cerr << "ERROR: overlapping frame range between "
                        << iArchives[0].getName() << " and " << argv[i] << std::endl;
                    return 1;
                }
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

        // since important meta data hints can be on the archive
        // and will likely be part of every input in the sequence
        // propagate the one from the first archive to our output
        MetaData md = iOrderedArchives[0].getTop().getMetaData();
        std::string appWriter = "AbcStitcher";
        std::string userStr = md.get(Abc::kUserDescriptionKey);
        if (!userStr.empty())
        {
            userStr =  "AbcStitcher: " + userStr;
        }

        // Create an archive with the default writer
        OArchive oArchive;
        if (coreType == Alembic::AbcCoreFactory::IFactory::kOgawa)
        {
            oArchive = CreateArchiveWithInfo(
                Alembic::AbcCoreOgawa::WriteArchive(),
                fileName, appWriter, userStr, md, ErrorHandler::kThrowPolicy);
        }
#ifdef ALEMBIC_WITH_HDF5
        else if (coreType == Alembic::AbcCoreFactory::IFactory::kHDF5)
        {
            oArchive = CreateArchiveWithInfo(
                Alembic::AbcCoreHDF5::WriteArchive(),
                fileName, appWriter, userStr, md, ErrorHandler::kThrowPolicy);
        }
#endif

        OObject oRoot = oArchive.getTop();
        if (!oRoot.valid())
        {
            return -1;
        }

        std::vector<IObject> iRoots(numInputs);

        for (size_t e = 0; e < numInputs; ++e)
        {
            iRoots[e] = iOrderedArchives[e].getTop();
        }

        visitObjects(iRoots, oRoot, timeMap, true);
    }

    return 0;
}
