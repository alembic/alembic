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

#include "util.h"

#include <Alembic/AbcGeom/All.h>

using namespace Alembic::AbcGeom;
using namespace Alembic::Abc;
using namespace Alembic::AbcCoreAbstract;

void TimeAndSamplesMap::add(TimeSamplingPtr iTime, size_t iNumSamples)
{

    if (iNumSamples == 0)
    {
        iNumSamples = 1;
    }

    for (size_t i = 0; i < mTimeSampling.size(); ++i)
    {
        if (mTimeSampling[i]->getTimeSamplingType() ==
            iTime->getTimeSamplingType())
        {
            chrono_t curLastTime =
                mTimeSampling[i]->getSampleTime(mExpectedSamples[i]);

            chrono_t lastTime = iTime->getSampleTime(iNumSamples);
            if (lastTime < curLastTime)
            {
                lastTime = curLastTime;
            }

            if (mTimeSampling[i]->getSampleTime(0) > iTime->getSampleTime(0))
            {
                mTimeSampling[i] = iTime;
            }

            mExpectedSamples[i] = mTimeSampling[i]->getNearIndex(lastTime,
                std::numeric_limits< index_t >::max()).first;

            return;
        }
    }

    mTimeSampling.push_back(iTime);
    mExpectedSamples.push_back(iNumSamples);
}

TimeSamplingPtr TimeAndSamplesMap::get(TimeSamplingPtr iTime,
                                       std::size_t & oNumSamples) const
{
    for (size_t i = 0; i < mTimeSampling.size(); ++i)
    {
        if (mTimeSampling[i]->getTimeSamplingType() ==
            iTime->getTimeSamplingType())
        {
            oNumSamples = mExpectedSamples[i];
            return mTimeSampling[i];
        }
    }

    oNumSamples = 0;
    return TimeSamplingPtr();
}

index_t getIndexSample(index_t iCurOutIndex, TimeSamplingPtr iOutTime,
    index_t iInNumSamples, TimeSamplingPtr iInTime, index_t & oNumEmpty)
{

    // see if we are missing any samples for oNumEmpty
    chrono_t curTime = iOutTime->getSampleTime(iCurOutIndex);
    chrono_t inChrono = iInTime->getSampleTime(0);
    if (curTime < inChrono)
    {
        index_t emptyEnd = iOutTime->getNearIndex(inChrono,
            std::numeric_limits<index_t>::max()).first;
        if (emptyEnd > iCurOutIndex)
        {
            oNumEmpty = emptyEnd - iCurOutIndex;
        }
        else
        {
            oNumEmpty = 0;
        }
    }
    else
    {
        oNumEmpty = 0;
    }

    for (index_t i = 0; i < iInNumSamples; ++i)
    {
        inChrono = iInTime->getSampleTime(i);
        if (curTime <= inChrono ||
            Imath::equalWithAbsError(curTime, inChrono, 1e-5))
        {
            return i;
        }
    }

    return iInNumSamples;
}

void checkAcyclic(const TimeSamplingType & tsType,
                  const std::string & fullNodeName)
{
    if (tsType.isAcyclic())
    {
        std::cerr << "No support for stitching acyclic sampling node "
            << fullNodeName << std::endl;
        exit(1);
    }
}


void stitchArrayProp(const PropertyHeader & propHeader,
                     const ICompoundPropertyVec & iCompoundProps,
                     OCompoundProperty & oCompoundProp,
                     const TimeAndSamplesMap & iTimeMap)
{

    size_t totalSamples = 0;
    TimeSamplingPtr timePtr =
        iTimeMap.get(propHeader.getTimeSampling(), totalSamples);

    const DataType & dataType = propHeader.getDataType();
    const MetaData & metaData = propHeader.getMetaData();
    const std::string & propName = propHeader.getName();

    Dimensions emptyDims(0);
    ArraySample emptySample(NULL, dataType, emptyDims);

    OArrayProperty writer(oCompoundProp, propName, dataType, metaData, timePtr);

    size_t numInputs = iCompoundProps.size();
    for (size_t iCpIndex = 0; iCpIndex < numInputs; iCpIndex++)
    {

        if (!iCompoundProps[iCpIndex].valid())
        {
            continue;
        }

        const PropertyHeader * childHeader =
            iCompoundProps[iCpIndex].getPropertyHeader(propName);

        if (!childHeader || dataType != childHeader->getDataType())
        {
            continue;
        }

        IArrayProperty reader(iCompoundProps[iCpIndex], propName);
        index_t numSamples = reader.getNumSamples();

        ArraySamplePtr dataPtr;
        index_t numEmpty;
        index_t k = getIndexSample(writer.getNumSamples(),
            writer.getTimeSampling(), numSamples,
            reader.getTimeSampling(), numEmpty);


        for (index_t j = 0; j < numEmpty; ++j)
        {
                writer.set(emptySample);
        }

        for (; k < numSamples; k++)
        {
            reader.get(dataPtr, k);
            writer.set(*dataPtr);
        }
    }

    // fill in any other empties
    for (size_t i = writer.getNumSamples(); i < totalSamples; ++i)
    {
        writer.set(emptySample);
    }
}

// return true if we needed to stitch the geom param
bool stitchArbGeomParam(const PropertyHeader & propHeader,
                        const ICompoundPropertyVec & iCompoundProps,
                        OCompoundProperty & oCompoundProp,
                        const TimeAndSamplesMap & iTimeMap)
{
    // go through all the inputs to see if all the property types are the same
    size_t numInputs = iCompoundProps.size();
    const std::string & propName = propHeader.getName();
    PropertyType ptype = propHeader.getPropertyType();
    bool diffProp = false;

    for (size_t iCpIndex = 1; iCpIndex < numInputs && diffProp == false;
         iCpIndex++)
    {
        if (!iCompoundProps[iCpIndex].valid())
        {
            continue;
        }

        const PropertyHeader * childHeader =
            iCompoundProps[iCpIndex].getPropertyHeader(propName);

        if (childHeader && childHeader->getPropertyType() != ptype)
        {
            diffProp = true;
        }
    }

    // all of the props are the same, lets stitch them like normal
    if (!diffProp)
    {
        return false;
    }


    // we have a mismatch of indexed and non-index geom params, lets stitch them
    // together AS indexed
    std::vector< IArrayProperty > valsProp(numInputs);
    std::vector< IArrayProperty > indicesProp(numInputs);

    bool firstVals = true;

    DataType dataType;
    MetaData metaData;
    TimeSamplingPtr timePtr;

    // first we need to get our attrs
    for (size_t iCpIndex = 0; iCpIndex < numInputs; iCpIndex++)
    {
        if (!iCompoundProps[iCpIndex].valid())
        {
            continue;
        }

        const PropertyHeader * childHeader =
            iCompoundProps[iCpIndex].getPropertyHeader(propName);

        if (childHeader && childHeader->isArray())
        {
            valsProp[iCpIndex] = IArrayProperty(iCompoundProps[iCpIndex],
                                                propName);

            if (firstVals)
            {
                firstVals = false;
                dataType = valsProp[iCpIndex].getDataType();
                metaData = valsProp[iCpIndex].getMetaData();
                timePtr = valsProp[iCpIndex].getTimeSampling();
            }
        }
        else if (childHeader && childHeader->isCompound())
        {
            ICompoundProperty cprop(iCompoundProps[iCpIndex], propName);
            if (cprop.getPropertyHeader(".vals") != NULL &&
                cprop.getPropertyHeader(".indices") != NULL)
            {
                valsProp[iCpIndex] = IArrayProperty(cprop, ".vals");
                indicesProp[iCpIndex] = IArrayProperty(cprop, ".indices");

                if (firstVals)
                {
                    firstVals = false;
                    dataType = valsProp[iCpIndex].getDataType();
                    metaData = valsProp[iCpIndex].getMetaData();
                    timePtr = valsProp[iCpIndex].getTimeSampling();
                }
            }
        }
    }


    size_t totalSamples = 0;
    timePtr = iTimeMap.get(timePtr, totalSamples);

    DataType indicesType(Alembic::Util::kUint32POD);
    Dimensions emptyDims(0);
    ArraySample emptySample(NULL, dataType, emptyDims);
    ArraySample emptyIndicesSample(NULL, indicesType, emptyDims);

    // we write indices and vals together
    OCompoundProperty ocProp(oCompoundProp, propName, metaData);
    OArrayProperty valsWriter(ocProp, ".vals", dataType, metaData, timePtr);
    OArrayProperty indicesWriter(ocProp, ".indices", indicesType, timePtr);

    for (size_t index = 0; index < numInputs; index++)
    {

        if (!valsProp[index].valid())
        {
            continue;
        }

        index_t numSamples = valsProp[index].getNumSamples();

        ArraySamplePtr dataPtr;
        index_t numEmpty;
        index_t k = getIndexSample(valsWriter.getNumSamples(),
            valsWriter.getTimeSampling(), numSamples,
            valsProp[index].getTimeSampling(), numEmpty);

        for (index_t j = 0; j < numEmpty; ++j)
        {
            valsWriter.set(emptySample);
            indicesWriter.set(emptyIndicesSample);
        }

        for (; k < numSamples; k++)
        {
            valsProp[index].get(dataPtr, k);
            valsWriter.set(*dataPtr);

            if (indicesProp[index].valid())
            {
                indicesProp[index].get(dataPtr, k);
                indicesWriter.set(*dataPtr);
            }
            else
            {
                // we need to construct our indices manually
                Dimensions dataDims = dataPtr->getDimensions();
                std::vector<Alembic::Util::uint32_t> indicesVec(
                    dataDims.numPoints());
                for (size_t dataIdx = 0; dataIdx < indicesVec.size(); ++dataIdx)
                {
                    indicesVec[dataIdx] = (Alembic::Util::uint32_t) dataIdx;
                }

                // set the empty sample
                if (indicesVec.empty())
                {
                    indicesWriter.set(emptyIndicesSample);
                }
                else
                {
                    ArraySample indicesSamp(&indicesVec.front(), indicesType,
                                            dataDims);
                    indicesWriter.set(indicesSamp);
                }
            }
        }
    }

    // fill in any other empties
    for (size_t i = valsWriter.getNumSamples(); i < totalSamples; ++i)
    {
        valsWriter.set(emptySample);
        indicesWriter.set(emptyIndicesSample);
    }
    return true;
}

template< typename T >
void scalarPropIO(IScalarProperty & reader,
                  Alembic::Util::uint8_t extent,
                  OScalarProperty & writer)
{
    std::vector< T > data(extent);
    std::vector< T > emptyData(extent);
    void * emptyPtr = static_cast< void* >(&emptyData.front());

    index_t numSamples = reader.getNumSamples();
    index_t numEmpty;
    index_t k = getIndexSample(writer.getNumSamples(), writer.getTimeSampling(),
        numSamples, reader.getTimeSampling(), numEmpty);

    // not really empty, but set to a default 0 or empty string value
    for (index_t i = 0; i < numEmpty; ++i)
    {
        writer.set(emptyPtr);
    }

    void * vPtr = static_cast< void* >(&data.front());

    for (; k < numSamples; ++k)
    {
        reader.get(vPtr, k);
        writer.set(vPtr);
    }
}

void stitchScalarProp(const PropertyHeader & propHeader,
                      const ICompoundPropertyVec & iCompoundProps,
                      OCompoundProperty & oCompoundProp,
                      const TimeAndSamplesMap & iTimeMap)
{
    size_t totalSamples = 0;
    TimeSamplingPtr timePtr =
        iTimeMap.get(propHeader.getTimeSampling(), totalSamples);

    const DataType & dataType = propHeader.getDataType();
    const MetaData & metaData = propHeader.getMetaData();
    const std::string & propName = propHeader.getName();
    Alembic::Util::PlainOldDataType pod = dataType.getPod();

    OScalarProperty writer(oCompoundProp, propName, dataType, metaData,
                           timePtr);

    size_t numInputs = iCompoundProps.size();
    for (size_t iCpIndex = 0; iCpIndex < numInputs; iCpIndex++)
    {

        if (!iCompoundProps[iCpIndex].valid())
        {
            continue;
        }

        const PropertyHeader * childHeader =
            iCompoundProps[iCpIndex].getPropertyHeader(propName);

        if (!childHeader || dataType != childHeader->getDataType())
        {
            continue;
        }

        IScalarProperty reader(iCompoundProps[iCpIndex], propName);
        Alembic::Util::uint8_t extent = dataType.getExtent();
        switch(pod)
        {
            case Alembic::Util::kBooleanPOD:
                scalarPropIO< Alembic::Util::bool_t >(reader, extent, writer);
                break;
            case Alembic::Util::kUint8POD:
                scalarPropIO< Alembic::Util::uint8_t >(reader, extent, writer);
                break;
            case Alembic::Util::kInt8POD:
                scalarPropIO< Alembic::Util::int8_t >(reader, extent, writer);
                break;
            case Alembic::Util::kUint16POD:
                scalarPropIO< Alembic::Util::uint16_t >(reader, extent, writer);
                break;
            case Alembic::Util::kInt16POD:
                scalarPropIO< Alembic::Util::int16_t >(reader, extent, writer);
                break;
            case Alembic::Util::kUint32POD:
                scalarPropIO< Alembic::Util::uint32_t >(reader, extent, writer);
                break;
            case Alembic::Util::kInt32POD:
                scalarPropIO< Alembic::Util::int32_t >(reader, extent, writer);
                break;
            case Alembic::Util::kUint64POD:
                scalarPropIO< Alembic::Util::uint64_t >(reader, extent, writer);
                break;
            case Alembic::Util::kInt64POD:
                scalarPropIO< Alembic::Util::int64_t >(reader, extent, writer);
                break;
            case Alembic::Util::kFloat16POD:
                scalarPropIO< Alembic::Util::float16_t >(reader, extent, writer);
                break;
            case Alembic::Util::kFloat32POD:
                scalarPropIO< Alembic::Util::float32_t >(reader, extent, writer);
                break;
            case Alembic::Util::kFloat64POD:
                scalarPropIO< Alembic::Util::float64_t >(reader, extent, writer);
                break;
            case Alembic::Util::kStringPOD:
                scalarPropIO< Alembic::Util::string >(reader, extent, writer);
                break;
            case Alembic::Util::kWstringPOD:
                scalarPropIO< Alembic::Util::wstring >(reader, extent, writer);
                break;
            default:
                break;
        }
    }

    // set any extra empties
    std::vector< Alembic::Util::string > emptyStr(dataType.getExtent());
    std::vector< Alembic::Util::wstring > emptyWstr(dataType.getExtent());
    std::vector< Alembic::Util::uint8_t > emptyBuffer(dataType.getNumBytes());
    for (size_t i = writer.getNumSamples(); i < totalSamples; ++i)
    {
        if (pod == Alembic::Util::kStringPOD)
        {
            writer.set(&emptyStr.front());
        }
        else if (pod == Alembic::Util::kWstringPOD)
        {
            writer.set(&emptyWstr.front());
        }
        else
        {
            writer.set(&emptyBuffer.front());
        }
    }
}

void stitchCompoundProp(ICompoundPropertyVec & iCompoundProps,
                        OCompoundProperty & oCompoundProp,
                        const TimeAndSamplesMap & iTimeMap)
{
    size_t numCompounds = iCompoundProps.size();
    for (size_t i = 0; i < numCompounds; ++i)
    {
        if (!iCompoundProps[i].valid())
        {
            continue;
        }

        size_t numProps = iCompoundProps[i].getNumProperties();
        for (size_t propIndex = 0; propIndex < numProps; propIndex++)
        {
            const PropertyHeader & propHeader =
                iCompoundProps[i].getPropertyHeader(propIndex);

            if (oCompoundProp.getPropertyHeader(propHeader.getName()) != NULL)
            {
                continue;
            }

            if (propHeader.getMetaData().get("isGeomParam") == "true" &&
                stitchArbGeomParam(propHeader, iCompoundProps, oCompoundProp,
                                   iTimeMap))
            {
                continue;
            }
            else if (propHeader.isCompound())
            {
                ICompoundPropertyVec childProps;
                for (size_t j = i; j < numCompounds; ++j)
                {
                    if (!iCompoundProps[j].valid() ||
                        iCompoundProps[j].getPropertyHeader(
                            propHeader.getName()) == NULL)
                    {
                        continue;
                    }

                    childProps.push_back(ICompoundProperty(iCompoundProps[j],
                        propHeader.getName()));
                }
                OCompoundProperty child(oCompoundProp, propHeader.getName(),
                    propHeader.getMetaData());
                stitchCompoundProp(childProps, child, iTimeMap);
            }
            else if (propHeader.isScalar())
            {
                stitchScalarProp(propHeader, iCompoundProps,
                                 oCompoundProp, iTimeMap);
            }
            else if (propHeader.isArray())
            {
                stitchArrayProp(propHeader, iCompoundProps,
                                oCompoundProp, iTimeMap);
            }
        }
    }
}

