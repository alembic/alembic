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

#include "util.h"

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

using namespace Alembic::AbcGeom;
using namespace Alembic::Abc;
using namespace Alembic::AbcCoreAbstract;

index_t getIndexSample(index_t iCurOutIndex, TimeSamplingPtr iOutTime,
    index_t iInNumSamples, TimeSamplingPtr iInTime)
{
    // borrowed from TimeSampling.cpp
    static const chrono_t kCHRONO_TOLERANCE =
        std::numeric_limits<chrono_t>::epsilon() * 32.0 * 32.0;

    if (iCurOutIndex == 0)
    {
        return 0;
    }

    chrono_t curTime = iOutTime->getSampleTime(iCurOutIndex);

    for (index_t i = 0; i < iInNumSamples; ++i)
    {
        chrono_t inChrono = iInTime->getSampleTime(i);
        if (curTime <= inChrono ||
            Imath::equalWithAbsError(curTime, inChrono, kCHRONO_TOLERANCE))
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
                     OCompoundProperty & oCompoundProp)
{
    const DataType & dataType = propHeader.getDataType();
    const MetaData & metaData = propHeader.getMetaData();
    const std::string & propName = propHeader.getName();

    OArrayProperty writer(oCompoundProp, propName, dataType, metaData);

    size_t numInputs = iCompoundProps.size();
    for (size_t iCpIndex = 0; iCpIndex < numInputs; iCpIndex++)
    {
        IArrayProperty reader(iCompoundProps[iCpIndex], propName);
        index_t numSamples = reader.getNumSamples();

        ArraySamplePtr dataPtr;
        index_t k = getIndexSample(writer.getNumSamples(),
            writer.getTimeSampling(), numSamples, reader.getTimeSampling());
        for (; k < numSamples; k++)
        {
            reader.get(dataPtr, k);
            writer.set(*dataPtr);
        }

        if (iCpIndex == 0)
        {
            writer.setTimeSampling(reader.getTimeSampling());
        }
    }
}

template< typename T >
void scalarPropIO(IScalarProperty & reader,
                  Alembic::Util::uint8_t extent,
                  OScalarProperty & writer)
{
    T * dataPtr = new T[extent];
    if (!dataPtr)
    {
        std::cerr << "failed to allocate data pointer" << std::endl;
        exit(1);
    }

    index_t numSamples = reader.getNumSamples();
    index_t k = getIndexSample(writer.getNumSamples(),
        writer.getTimeSampling(), numSamples, reader.getTimeSampling());
    for (; k < numSamples; ++k)
    {
        void * vPtr = static_cast< void* >(dataPtr);
        reader.get(vPtr, k);
        writer.set(vPtr);
    }
    delete [] dataPtr;
}

void stitchScalarProp(const PropertyHeader & propHeader,
                      const ICompoundPropertyVec & iCompoundProps,
                      OCompoundProperty & oCompoundProp)
{
    const DataType & dataType = propHeader.getDataType();
    const MetaData & metaData = propHeader.getMetaData();
    const std::string & propName = propHeader.getName();

    OScalarProperty writer(oCompoundProp, propName, dataType, metaData);

    size_t numInputs = iCompoundProps.size();
    for (size_t iCpIndex = 0; iCpIndex < numInputs; iCpIndex++)
    {
        IScalarProperty reader(iCompoundProps[iCpIndex], propName);
        Alembic::Util::uint8_t extent = dataType.getExtent();  // dimension of the data

        switch(dataType.getPod())
        {
            case Alembic::Util::kBooleanPOD:
                scalarPropIO< bool >(reader, extent, writer);
                break;
            case Alembic::Util::kUint8POD:
                scalarPropIO< unsigned char >(reader, extent, writer);
                break;
            case Alembic::Util::kInt8POD:
                scalarPropIO< char >(reader, extent, writer);
                break;
            case Alembic::Util::kUint16POD:
                scalarPropIO< unsigned short >(reader, extent, writer);
                break;
            case Alembic::Util::kInt16POD:
                scalarPropIO< short >(reader, extent, writer);
                break;
            case Alembic::Util::kUint32POD:
                scalarPropIO< unsigned int >(reader, extent, writer);
                break;
            case Alembic::Util::kInt32POD:
                scalarPropIO< int >(reader, extent, writer);
                break;
            case Alembic::Util::kUint64POD:
                scalarPropIO< unsigned long >(reader, extent, writer);
                break;
            case Alembic::Util::kInt64POD:
                scalarPropIO< long >(reader, extent, writer);
                break;
            case Alembic::Util::kFloat16POD:
                scalarPropIO< half >(reader, extent, writer);
                break;
            case Alembic::Util::kFloat32POD:
                scalarPropIO< float >(reader, extent, writer);
                break;
            case Alembic::Util::kFloat64POD:
                scalarPropIO< double >(reader, extent, writer);
                break;
            case Alembic::Util::kStringPOD:
                scalarPropIO< std::string >(reader, extent, writer);
                break;
            case Alembic::Util::kWstringPOD:
                scalarPropIO< std::wstring >(reader, extent, writer);
                break;
            default:
                break;
        }

        if (iCpIndex == 0)
        {
            writer.setTimeSampling(reader.getTimeSampling());
        }
    }
}

void stitchCompoundProp(ICompoundPropertyVec & iCompoundProps,
                        OCompoundProperty & oCompoundProp)
{
    size_t numProps = iCompoundProps[0].getNumProperties();
    for (size_t propIndex = 0; propIndex < numProps; propIndex++)
    {
        const PropertyHeader & propHeader =
            iCompoundProps[0].getPropertyHeader(propIndex);

        if (propHeader.isCompound())
        {
            ICompoundPropertyVec childProps(iCompoundProps.size());
            for (size_t i = 0; i < childProps.size(); ++i)
            {
                childProps[i] = ICompoundProperty(iCompoundProps[i],
                    propHeader.getName());
            }
            OCompoundProperty child(oCompoundProp, propHeader.getName(),
                propHeader.getMetaData());
            stitchCompoundProp(childProps, child);
        }
        else if (propHeader.isScalar())
        {
            stitchScalarProp(propHeader, iCompoundProps, oCompoundProp);
        }
        else if (propHeader.isArray())
        {
            stitchArrayProp(propHeader, iCompoundProps, oCompoundProp);
        }
    }
}

