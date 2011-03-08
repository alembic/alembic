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

#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MPlug.h>
#include <maya/MUint64Array.h>
#include <maya/MStringArray.h>
#include <maya/MFnData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnStringData.h>
#include <maya/MFnStringArrayData.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MObjectArray.h>
#include <maya/MDGModifier.h>
#include <maya/MSelectionList.h>
#include <maya/MFnLight.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnCamera.h>
#include <maya/MTime.h>

#include <Alembic/AbcCoreHDF5/ReadWrite.h>

#include "util.h"
#include "AlembicNode.h"
#include "CreateSceneHelper.h"
#include "NodeIteratorVisitorHelper.h"

void unsupportedWarning(const MString & iName,
    const Alembic::AbcCoreAbstract::v1::DataType & iDataType)
{
    MString warn = "Unsupported attr, skipping: ";
    warn += iName + " " + PODName(iDataType.getPod());
    warn += "[";
    warn += iDataType.getExtent();
    warn += "]";

    printWarning(warn);
}

void addString(MObject & iParent, const std::string & iAttrName,
    const std::string & iValue)
{
    MFnStringData fnStringData;
    MString attrValue(iValue.c_str());
    MString attrName(iAttrName.c_str());
    MObject strAttrObject = fnStringData.create(attrValue);

    MFnTypedAttribute attr;
    MObject attrObj = attr.create(attrName, attrName, MFnData::kString,
        strAttrObject);
    MFnDependencyNode parentFn(iParent);
    parentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);
}

void addArbAttrAndScope(MObject & iParent, const std::string & iAttrName,
    const std::string & iScope, const std::string & iInterp, uint8_t iExtent)
{

    std::string attrStr;

    if (iInterp == "rgb")
        attrStr = "color3";
    else if (iInterp == "rgba")
        attrStr = "color4";
    else if (iInterp == "vector")
    {
        if (iExtent == 2)
            attrStr = "vector2";
        else if (iExtent == 3)
            attrStr = "vector3";
        else if (iExtent == 4)
            attrStr = "vector4";
    }
    else if (iInterp == "point")
    {
        if (iExtent == 2)
            attrStr = "point2";
        else if (iExtent == 3)
            attrStr = "point3";
        else if (iExtent == 4)
            attrStr = "point4";
    }
    else if (iInterp == "normal")
    {
        if (iExtent == 2)
            attrStr = "normal2";
        else if (iExtent == 3)
            attrStr = "normal3";
    }

    if (attrStr != "")
    {
        std::string attrName = iAttrName + "_AbcType";
        addString(iParent, attrName, attrStr);
    }

    if (iScope != "")
    {
        std::string attrName = iAttrName + "_AbcGeoScope";
        addString(iParent, attrName, iScope);
    }
}

void addAttr(double iFrame, Alembic::Abc::IScalarProperty & iProp,
    MObject & iParent)
{
    MFnDependencyNode parentFn(iParent);
    MString attrName(iProp.getName().c_str());
    MFnNumericAttribute attr;
    MObject attrObj;
    Alembic::AbcCoreAbstract::v1::DataType dtype = iProp.getDataType();
    Alembic::Util::uint8_t extent = dtype.getExtent();

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, iProp.getTimeSampling(), index,
        ceilIndex);

    switch (dtype.getPod())
    {
        case Alembic::Util::kBooleanPOD:
        {
            if (extent != 1)
            {
                unsupportedWarning(attrName, dtype);
                return;
            }

            Alembic::Util::bool_t val;
            iProp.get(&val, Alembic::Abc::ISampleSelector(index));

            bool bval = (val != false);
            attrObj = attr.create(attrName, attrName,
                MFnNumericData::kBoolean, bval);
        }
        break;

        case Alembic::Util::kUint8POD:
        case Alembic::Util::kInt8POD:
        {
            if (extent != 1)
            {
                unsupportedWarning(attrName, dtype);
                return;
            }

            int8_t val;

            // visibility is handled differently from Maya's
            // visibility model
            if (attrName == "visible")
            {
                MPlug plug = parentFn.findPlug("visibility");
                plug.setValue(val != 0);
                return;
            }

            if (index != ceilIndex && alpha != 0.0)
            {
                int8_t lo, hi;
                iProp.get(&lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(&hi, Alembic::Abc::ISampleSelector(ceilIndex));
                val = simpleLerp<int8_t>(alpha, lo, hi);
            }
            else
            {
                iProp.get(&val, Alembic::Abc::ISampleSelector(index));
            }

            attrObj = attr.create(attrName, attrName,
                MFnNumericData::kByte, val);
        }
        break;

        case Alembic::Util::kInt16POD:
        case Alembic::Util::kUint16POD:
        {
            // MFnNumericData::kShort or k2Short or k3Short
            if (extent > 3)
            {
                unsupportedWarning(attrName, dtype);
                return;
            }

            int16_t val[3];

            if (index != ceilIndex && alpha != 0.0)
            {
                int16_t lo[3];
                int16_t hi[3];
                iProp.get(lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(hi, Alembic::Abc::ISampleSelector(ceilIndex));
                for (int8_t i = 0; i < extent; ++i)
                {
                     val[i] = simpleLerp<int16_t>(alpha, lo[i], hi[i]);
                }
            }
            else
            {
                iProp.get(val, Alembic::Abc::ISampleSelector(index));
            }

            if (extent == 1)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::kShort);
                attr.setDefault(val[0]);
            }
            else if (extent == 2)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k2Short);
                attr.setDefault(val[0], val[1]);
            }
            else if (extent == 3)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k3Short);
                attr.setDefault(val[0], val[1], val[2]);
            }
        }
        break;

        case Alembic::Util::kUint32POD:
        case Alembic::Util::kInt32POD:
        {
            if (extent > 3)
            {
                unsupportedWarning(attrName, dtype);
                return;
            }

            int32_t val[3];

            if (index != ceilIndex && alpha != 0.0)
            {
                int32_t lo[3];
                int32_t hi[3];
                iProp.get(lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(hi, Alembic::Abc::ISampleSelector(ceilIndex));
                for (int8_t i = 0; i < extent; ++i)
                {
                     val[i] = simpleLerp<int32_t>(alpha, lo[i], hi[i]);
                }
            }
            else
            {
                iProp.get(val, Alembic::Abc::ISampleSelector(index));
            }

            if (extent == 1)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::kInt);
                attr.setDefault(val[0]);
            }
            else if (extent == 2)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k2Int);
                attr.setDefault(val[0], val[1]);
            }
            else if (extent == 3)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k3Int);
                attr.setDefault(val[0], val[1], val[2]);
            }
        }
        break;

        case Alembic::Util::kFloat32POD:
        {
            if (extent > 3)
            {
                unsupportedWarning(attrName, dtype);
                return;
            }

            float val[3];

            if (index != ceilIndex && alpha != 0.0)
            {
                float lo[3];
                float hi[3];
                iProp.get(lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(hi, Alembic::Abc::ISampleSelector(ceilIndex));
                for (int8_t i = 0; i < extent; ++i)
                {
                     val[i] = simpleLerp<float>(alpha, lo[i], hi[i]);
                }
            }
            else
            {
                iProp.get(val, Alembic::Abc::ISampleSelector(index));
            }

            if (extent == 1)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::kFloat);
                attr.setDefault(val[0]);
            }
            else if (extent == 2)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k2Float);
                attr.setDefault(val[0], val[1]);
            }
            else if (extent == 3)
            {
                // look for color and point?
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k3Float);
                attr.setDefault(val[0], val[1], val[2]);
            }
        }
        break;

        case Alembic::Util::kFloat64POD:
        {
            if (extent > 4)
            {
                unsupportedWarning(attrName, dtype);
                return;
            }

            double val[4];

            if (index != ceilIndex && alpha != 0.0)
            {
                double lo[3];
                double hi[3];
                iProp.get(lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(hi, Alembic::Abc::ISampleSelector(ceilIndex));
                for (int8_t i = 0; i < extent; ++i)
                {
                     val[i] = simpleLerp<double>(alpha, lo[i], hi[i]);
                }
            }
            else
            {
                iProp.get(val, Alembic::Abc::ISampleSelector(index));
            }

            if (extent == 1)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::kDouble);
                attr.setDefault(val[0]);
            }
            else if (extent == 2)
            {
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k2Double);
                attr.setDefault(val[0], val[1]);
            }
            else if (extent == 3)
            {
                // look for color and point?
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k3Double);
                attr.setDefault(val[0], val[1], val[2]);
            }
            else if (extent == 4)
            {
                // look for color and point?
                attrObj = attr.create(attrName, attrName,
                    MFnNumericData::k4Double);
                attr.setDefault(val[0], val[1], val[2], val[4]);
            }
        }
        break;

        case Alembic::Util::kStringPOD:
        {
            if (extent != 1)
            {
                unsupportedWarning(attrName, dtype);
                return;
            }

            Alembic::Util::string val;
            iProp.get(&val, Alembic::Abc::ISampleSelector(index));
            MFnStringData fnStringData;
            MString attrValue(val.c_str());
            MObject strAttrObject = fnStringData.create(attrValue);
            MFnTypedAttribute tattr;
            attrObj = tattr.create(attrName, attrName, MFnData::kString,
                strAttrObject);
        }
        break;

        case Alembic::Util::kWstringPOD:
        {
            if (extent != 1)
            {
                unsupportedWarning(attrName, dtype);
                return;
            }

            Alembic::Util::wstring val;
            iProp.get(&val, Alembic::Abc::ISampleSelector(index));
            MFnStringData fnStringData;
            MString attrValue((wchar_t *)val.c_str());
            MObject strAttrObject = fnStringData.create(attrValue);
            MFnTypedAttribute tattr;
            attrObj = tattr.create(attrName, attrName, MFnData::kString,
                strAttrObject);
        }
        break;

        default:
        {
            // Not sure what to do with kFloat16POD, kInt64POD, kUInt64POD
            // so we'll just skip them for now

            unsupportedWarning(attrName, dtype);
            return;
        }
        break;
    }

    attr.setKeyable(true);
    attr.setHidden(false);
    parentFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);

}

void addAttr(double iFrame, Alembic::Abc::IArrayProperty & iProp,
    MObject & iParent)
{
    MFnDependencyNode parentFn(iParent);
    MString attrName(iProp.getName().c_str());
    MFnTypedAttribute attr;
    MObject attrObj;
    Alembic::AbcCoreAbstract::v1::DataType dtype = iProp.getDataType();
    Alembic::Util::uint8_t extent = dtype.getExtent();

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, iProp.getTimeSampling(), index,
        ceilIndex);

    switch (dtype.getPod())
    {
        case Alembic::Util::kUint32POD:
        case Alembic::Util::kInt32POD:
        {
            MFnIntArrayData fnData;
            Alembic::AbcCoreAbstract::v1::ArraySamplePtr samp;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            if (alpha != 0.0 && index != ceilIndex)
            {
                Alembic::AbcCoreAbstract::v1::ArraySamplePtr ceilSamp;
                iProp.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));

                MIntArray arr((int *) samp->getData(), samp->size());
                size_t sampSize = samp->size();

                // size is different don't lerp
                if (sampSize != ceilSamp->size())
                {
                    attrObj = fnData.create(arr);
                }
                else
                {
                    int * hi = (int *) ceilSamp->getData();
                    for (size_t i = 0; i < sampSize; ++i)
                    {
                        arr[i] = simpleLerp<int>(alpha, arr[i], hi[i]);
                    }
                }
                attrObj = fnData.create(arr);
            }
            else
            {
                MIntArray arr((int *) samp->getData(), samp->size());
                attrObj = fnData.create(arr);
            }
            attr.create(attrName, attrName, MFnData::kIntArray, attrObj);
        }
        break;

        // look for MFnVectorArrayData?
        case Alembic::Util::kFloat32POD:
        {
            // need to differentiate between vectors, points, and color array?

            MFnDoubleArrayData fnData;
            Alembic::AbcCoreAbstract::v1::ArraySamplePtr samp;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            if (alpha != 0.0 && index != ceilIndex)
            {
                Alembic::AbcCoreAbstract::v1::ArraySamplePtr ceilSamp;
                iProp.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));

                MDoubleArray arr((float *) samp->getData(), samp->size());
                size_t sampSize = samp->size();

                // size is different don't lerp
                if (sampSize != ceilSamp->size())
                {
                    attrObj = fnData.create(arr);
                }
                else
                {
                    float * hi = (float *) ceilSamp->getData();
                    for (size_t i = 0; i < sampSize; ++i)
                    {
                        arr[i] = simpleLerp<double>(alpha, arr[i], hi[i]);
                    }
                }
                attrObj = fnData.create(arr);
            }
            else
            {
                MDoubleArray arr((float *) samp->getData(), samp->size());
                attrObj = fnData.create(arr);
            }
            attr.create(attrName, attrName, MFnData::kDoubleArray, attrObj);
        }
        break;

        case Alembic::Util::kFloat64POD:
        {
            // need to differentiate between vectors, points, and color array?

            MFnDoubleArrayData fnData;
            Alembic::AbcCoreAbstract::v1::ArraySamplePtr samp;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            if (alpha != 0.0 && index != ceilIndex)
            {
                Alembic::AbcCoreAbstract::v1::ArraySamplePtr ceilSamp;
                iProp.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));

                MDoubleArray arr((double *) samp->getData(), samp->size());
                size_t sampSize = samp->size();

                // size is different don't lerp
                if (sampSize != ceilSamp->size())
                {
                    attrObj = fnData.create(arr);
                }
                else
                {
                    double * hi = (double *) ceilSamp->getData();
                    for (size_t i = 0; i < sampSize; ++i)
                    {
                        arr[i] = simpleLerp<double>(alpha, arr[i], hi[i]);
                    }
                }
                attrObj = fnData.create(arr);
            }
            else
            {
                MDoubleArray arr((double *) samp->getData(), samp->size());
                attrObj = fnData.create(arr);
            }
            attr.create(attrName, attrName, MFnData::kDoubleArray, attrObj);
        }
        break;

        // MFnStringArrayData
        case Alembic::Util::kStringPOD:
        {

            MFnStringArrayData fnData;
            Alembic::AbcCoreAbstract::v1::ArraySamplePtr samp;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            size_t sampSize = samp->size();
            MStringArray arr;
            arr.setLength(sampSize);
            attrObj = fnData.create(arr);
            Alembic::Util::string * strData =
                (Alembic::Util::string *) samp->getData();

            for (size_t i = 0; i < sampSize; ++i)
            {
                arr[i] = strData[i].c_str();
            }

            attr.create(attrName, attrName, MFnData::kStringArray, attrObj);
        }
        break;

        // MFnStringArrayData
        case Alembic::Util::kWstringPOD:
        {
            MFnStringArrayData fnData;
            Alembic::AbcCoreAbstract::v1::ArraySamplePtr samp;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            size_t sampSize = samp->size();
            MStringArray arr;
            arr.setLength(sampSize);
            attrObj = fnData.create(arr);
            Alembic::Util::wstring * strData =
                (Alembic::Util::wstring *) samp->getData();

            for (size_t i = 0; i < sampSize; ++i)
            {
                arr[i] = (wchar_t *)strData[i].c_str();
            }

            attr.create(attrName, attrName, MFnData::kStringArray, attrObj);
        }
        break;

        default:
        {
            unsupportedWarning(attrName, dtype);
            return;
        }
        break;
    }

    attr.setKeyable(true);
    parentFn.addAttribute(attrObj,  MFnDependencyNode::kLocalDynamicAttr);
    addArbAttrAndScope(iParent, iProp.getName(),
        iProp.getMetaData().get("geoScope"),
        iProp.getMetaData().get("interpretation"), extent);
}

//=============================================================================


void addProperties(double iFrame, Alembic::Abc::IObject & iNode,
    MObject iObject, std::vector<std::string> & iSampledPropNameList)
{
// loop over the "arbitrary" attrs in iNode, cast to an arb attr schema?
}

//=============================================================================

void readAttr(double iFrame, Alembic::Abc::IScalarProperty & iProp,
    MDataHandle & iHandle)
{

    Alembic::AbcCoreAbstract::v1::DataType dtype = iProp.getDataType();
    Alembic::Util::uint8_t extent = dtype.getExtent();

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, iProp.getTimeSampling(), index,
        ceilIndex);

    switch (dtype.getPod())
    {
        case Alembic::Util::kBooleanPOD:
        {

            Alembic::Util::bool_t val;
            iProp.get(&val, Alembic::Abc::ISampleSelector(index));

            iHandle.setBool(val != false);
        }
        break;

        case Alembic::Util::kUint8POD:
        case Alembic::Util::kInt8POD:
        {

            int8_t val;

            if (iProp.getName() == "visible")
            {
                iProp.get(&val, Alembic::Abc::ISampleSelector(index));

                iHandle.setBool(val != 0);
                return;
            }
            else if (index != ceilIndex && alpha != 0.0)
            {
                int8_t lo, hi;
                iProp.get(&lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(&hi, Alembic::Abc::ISampleSelector(ceilIndex));
                val = simpleLerp<int8_t>(alpha, lo, hi);
            }
            else
            {
                iProp.get(&val, Alembic::Abc::ISampleSelector(index));
            }

            iHandle.setChar(val);

        }
        break;

        case Alembic::Util::kInt16POD:
        case Alembic::Util::kUint16POD:
        {
            int16_t val[3];

            if (index != ceilIndex && alpha != 0.0)
            {
                int16_t lo[3];
                int16_t hi[3];
                iProp.get(lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(hi, Alembic::Abc::ISampleSelector(ceilIndex));
                for (int8_t i = 0; i < extent; ++i)
                {
                     val[i] = simpleLerp<int16_t>(alpha, lo[i], hi[i]);
                }
            }
            else
            {
                iProp.get(val, Alembic::Abc::ISampleSelector(index));
            }

            if (extent == 1)
            {
                iHandle.setShort(val[0]);
            }
            else if (extent == 2)
            {
                iHandle.set2Short(val[0], val[1]);
            }
            else if (extent == 3)
            {
                iHandle.set3Short(val[0], val[1], val[2]);
            }
        }
        break;

        case Alembic::Util::kUint32POD:
        case Alembic::Util::kInt32POD:
        {
            int32_t val[3];

            if (index != ceilIndex && alpha != 0.0)
            {
                int32_t lo[3];
                int32_t hi[3];
                iProp.get(lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(hi, Alembic::Abc::ISampleSelector(ceilIndex));
                for (int8_t i = 0; i < extent; ++i)
                {
                     val[i] = simpleLerp<int32_t>(alpha, lo[i], hi[i]);
                }
            }
            else
            {
                iProp.get(val, Alembic::Abc::ISampleSelector(index));
            }

            if (extent == 1)
            {
                iHandle.setInt(val[0]);
            }
            else if (extent == 2)
            {
                iHandle.set2Int(val[0], val[1]);
            }
            else if (extent == 3)
            {
                iHandle.set3Int(val[0], val[1], val[2]);
            }
        }
        break;

        case Alembic::Util::kFloat32POD:
        {

            float val[3];

            if (index != ceilIndex && alpha != 0.0)
            {
                float lo[3];
                float hi[3];
                iProp.get(lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(hi, Alembic::Abc::ISampleSelector(ceilIndex));
                for (int8_t i = 0; i < extent; ++i)
                {
                     val[i] = simpleLerp<float>(alpha, lo[i], hi[i]);
                }
            }
            else
            {
                iProp.get(val, Alembic::Abc::ISampleSelector(index));
            }

            if (extent == 1)
            {
                iHandle.setFloat(val[0]);
            }
            else if (extent == 2)
            {
                iHandle.set2Float(val[0], val[1]);
            }
            else if (extent == 3)
            {
                iHandle.set3Float(val[0], val[1], val[2]);
            }
        }
        break;

        case Alembic::Util::kFloat64POD:
        {
            double val[4];

            if (index != ceilIndex && alpha != 0.0)
            {
                double lo[3];
                double hi[3];
                iProp.get(lo, Alembic::Abc::ISampleSelector(index));
                iProp.get(hi, Alembic::Abc::ISampleSelector(ceilIndex));
                for (int8_t i = 0; i < extent; ++i)
                {
                     val[i] = simpleLerp<double>(alpha, lo[i], hi[i]);
                }
            }
            else
            {
                iProp.get(val, Alembic::Abc::ISampleSelector(index));
            }

            if (extent == 1)
            {
                iHandle.setDouble(val[0]);
            }
            else if (extent == 2)
            {
                iHandle.set2Double(val[0], val[1]);
            }
            else if (extent == 3)
            {
                iHandle.set3Double(val[0], val[1], val[2]);
            }
            else if (extent == 4)
            {
                // dont have a call for set4Double
            }
        }
        break;

        case Alembic::Util::kStringPOD:
        {

            Alembic::Util::string val;
            iProp.get(&val, Alembic::Abc::ISampleSelector(index));
            MString attrValue(val.c_str());
            iHandle.setString(val.c_str());
        }
        break;

        case Alembic::Util::kWstringPOD:
        {
            Alembic::Util::wstring val;
            iProp.get(&val, Alembic::Abc::ISampleSelector(index));
            MString attrValue((wchar_t *)val.c_str());
            iHandle.setString(val.c_str());
        }
        break;

        default:
        break;
    }
}

void readAttr(double iFrame, Alembic::Abc::IArrayProperty & iProp,
    MDataHandle & iHandle)
{
    MObject attrObj;
    Alembic::AbcCoreAbstract::v1::DataType dtype = iProp.getDataType();
    Alembic::Util::uint8_t extent = dtype.getExtent();

    Alembic::AbcCoreAbstract::v1::ArraySamplePtr samp, ceilSamp;

    int64_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame, iProp.getTimeSampling(), index,
        ceilIndex);

    switch (dtype.getPod())
    {
        case Alembic::Util::kUint32POD:
        case Alembic::Util::kInt32POD:
        {
            MFnIntArrayData fnData;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            if (alpha != 0.0 && index != ceilIndex)
            {
                iProp.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));

                MIntArray arr((int *) samp->getData(), samp->size());
                size_t sampSize = samp->size();

                // size is different don't lerp
                if (sampSize != ceilSamp->size())
                {
                    attrObj = fnData.create(arr);
                }
                else
                {
                    int * hi = (int *) ceilSamp->getData();
                    for (size_t i = 0; i < sampSize; ++i)
                    {
                        arr[i] = simpleLerp<int>(alpha, arr[i], hi[i]);
                    }
                }
                attrObj = fnData.create(arr);
            }
            else
            {
                MIntArray arr((int *) samp->getData(), samp->size());
                attrObj = fnData.create(arr);
            }
        }
        break;

        // look for MFnVectorArrayData?
        case Alembic::Util::kFloat32POD:
        {
            // need to differentiate between vectors, points, and color array?

            MFnDoubleArrayData fnData;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            if (alpha != 0.0 && index != ceilIndex)
            {
                iProp.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));

                MDoubleArray arr((float *) samp->getData(), samp->size());
                size_t sampSize = samp->size();

                // size is different don't lerp
                if (sampSize != ceilSamp->size())
                {
                    attrObj = fnData.create(arr);
                }
                else
                {
                    float * hi = (float *) ceilSamp->getData();
                    for (size_t i = 0; i < sampSize; ++i)
                    {
                        arr[i] = simpleLerp<double>(alpha, arr[i], hi[i]);
                    }
                }
                attrObj = fnData.create(arr);
            }
            else
            {
                MDoubleArray arr((float *) samp->getData(), samp->size());
                attrObj = fnData.create(arr);
            }
        }
        break;

        case Alembic::Util::kFloat64POD:
        {
            // need to differentiate between vectors, points, and color array?

            MFnDoubleArrayData fnData;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            if (alpha != 0.0 && index != ceilIndex)
            {
                iProp.get(ceilSamp, Alembic::Abc::ISampleSelector(ceilIndex));

                MDoubleArray arr((double *) samp->getData(), samp->size());
                size_t sampSize = samp->size();

                // size is different don't lerp
                if (sampSize != ceilSamp->size())
                {
                    attrObj = fnData.create(arr);
                }
                else
                {
                    double * hi = (double *) ceilSamp->getData();
                    for (size_t i = 0; i < sampSize; ++i)
                    {
                        arr[i] = simpleLerp<double>(alpha, arr[i], hi[i]);
                    }
                }
                attrObj = fnData.create(arr);
            }
            else
            {
                MDoubleArray arr((double *) samp->getData(), samp->size());
                attrObj = fnData.create(arr);
            }
        }
        break;

        // MFnStringArrayData
        case Alembic::Util::kStringPOD:
        {
            MFnStringArrayData fnData;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            size_t sampSize = samp->size();
            MStringArray arr;
            arr.setLength(sampSize);
            attrObj = fnData.create(arr);
            Alembic::Util::string * strData =
                (Alembic::Util::string *) samp->getData();

            for (size_t i = 0; i < sampSize; ++i)
            {
                arr[i] = strData[i].c_str();
            }
        }
        break;

        // MFnStringArrayData
        case Alembic::Util::kWstringPOD:
        {
            MFnStringArrayData fnData;
            iProp.get(samp, Alembic::Abc::ISampleSelector(index));

            size_t sampSize = samp->size();
            MStringArray arr;
            arr.setLength(sampSize);
            attrObj = fnData.create(arr);
            Alembic::Util::wstring * strData =
                (Alembic::Util::wstring *) samp->getData();

            for (size_t i = 0; i < sampSize; ++i)
            {
                arr[i] = (wchar_t *)strData[i].c_str();
            }
        }
        break;

        default:
        break;
    }

    if (!attrObj.isNull())
        iHandle.set(attrObj);
}

WriterData::WriterData()
{
}

WriterData::WriterData(const WriterData & rhs)
{
    *this = rhs;
}

WriterData & WriterData::operator=(const WriterData & rhs)
{

    mPointsList = rhs.mPointsList;
    mPolyMeshList = rhs.mPolyMeshList;
    mSubDList = rhs.mSubDList;
    mXformList = rhs.mXformList;

    // get all the sampled Maya objects
    mPointsObjList = rhs.mPointsObjList;
    mPolyMeshObjList = rhs.mPolyMeshObjList;
    mSubDObjList = rhs.mSubDObjList;
    mXformOpList = rhs.mXformOpList;

    mIsComplexXform = rhs.mIsComplexXform;
    mIsSampledXformOpAngle = rhs.mIsSampledXformOpAngle;

    return *this;
}

void WriterData::getFrameRange(double & oMin, double & oMax)
{
    oMin = DBL_MAX;
    oMax = -DBL_MAX;

    size_t i, iEnd;
    Alembic::AbcCoreAbstract::v1::TimeSampling ts;

    iEnd = mPointsList.size();
    for (i = 0; i < iEnd; ++i)
    {
        ts = mPointsList[i].getSchema().getTimeSampling();
        oMin = std::min(ts.getSampleTime(0), oMin);
        oMax = std::max(ts.getSampleTime(ts.getNumSamples()-1), oMax);
    }

    iEnd = mPolyMeshList.size();
    for (i = 0; i < iEnd; ++i)
    {
        ts = mPolyMeshList[i].getSchema().getTimeSampling();
        oMin = std::min(ts.getSampleTime(0), oMin);
        oMax = std::max(ts.getSampleTime(ts.getNumSamples()-1), oMax);
    }

    iEnd = mSubDList.size();
    for (i = 0; i < iEnd; ++i)
    {
        ts = mSubDList[i].getSchema().getTimeSampling();
        oMin = std::min(ts.getSampleTime(0), oMin);
        oMax = std::max(ts.getSampleTime(ts.getNumSamples()-1), oMax);
    }

    iEnd = mXformList.size();
    for (i = 0; i < iEnd; ++i)
    {
        ts = mXformList[i].getSchema().getTimeSampling();
        if (ts.getNumSamples() > 1)
        {
            oMin = std::min(ts.getSampleTime(0), oMin);
            oMax = std::max(ts.getSampleTime(ts.getNumSamples()-1), oMax);
        }
    }
}

ArgData::ArgData(MString iFileName,
    bool iDebugOn, MObject iReparentObj, bool iConnect,
    MString iConnectRootNodes, bool iCreateIfNotFound, bool iRemoveIfNoUpdate) :
        mFileName(iFileName),
        mDebugOn(iDebugOn), mReparentObj(iReparentObj), mConnect(iConnect),
        mConnectRootNodes(iConnectRootNodes),
        mCreateIfNotFound(iCreateIfNotFound),
        mRemoveIfNoUpdate(iRemoveIfNoUpdate)
{
    mSequenceStartTime = -DBL_MAX;
    mSequenceEndTime = DBL_MAX;
}

ArgData::ArgData(const ArgData & rhs)
{
    *this = rhs;
}

ArgData & ArgData::operator=(const ArgData & rhs)
{
    mFileName = rhs.mFileName;
    mSequenceStartTime = rhs.mSequenceStartTime;
    mSequenceEndTime = rhs.mSequenceEndTime;

    mDebugOn = rhs.mDebugOn;

    mReparentObj = rhs.mReparentObj;

    // optional information for the "connect" flag
    mConnect = rhs.mConnect;
    mConnectRootNodes = rhs.mConnectRootNodes;
    mCreateIfNotFound = rhs.mCreateIfNotFound;
    mRemoveIfNoUpdate = rhs.mRemoveIfNoUpdate;

    mData = rhs.mData;

    return *this;
}

MString createScene(ArgData & iArgData)
{
    MString returnName("");

    // no caching!
    Alembic::Abc::IArchive archive(Alembic::AbcCoreHDF5::ReadArchive(),
        iArgData.mFileName.asChar(),
        Alembic::AbcCoreAbstract::v1::ReadArraySampleCachePtr());

    CreateSceneVisitor::Action action = CreateSceneVisitor::CREATE;
    if (iArgData.mRemoveIfNoUpdate && iArgData.mCreateIfNotFound)
        action = CreateSceneVisitor::CREATE_REMOVE;
    else if (iArgData.mRemoveIfNoUpdate)
        action = CreateSceneVisitor::REMOVE;
    else if (iArgData.mCreateIfNotFound)
        action = CreateSceneVisitor::CREATE;
    else if (iArgData.mConnect)
        action = CreateSceneVisitor::CONNECT;

    CreateSceneVisitor visitor(iArgData.mSequenceStartTime,
        iArgData.mReparentObj, action, iArgData.mConnectRootNodes);

    visitor.walk(archive);

    if (visitor.hasSampledData())
    {
        visitor.getData(iArgData.mData);

        iArgData.mData.getFrameRange(iArgData.mSequenceStartTime,
            iArgData.mSequenceEndTime);

        returnName = connectAttr(iArgData);
    }

    if (iArgData.mConnect)
    {
        visitor.applyShaderSelection();
    }

    return returnName;
}

MString connectAttr(ArgData & iArgData)
{
    MStatus status = MS::kSuccess;

    // create a new AlembicNode and initialize all its input attributes
    MDGModifier modifier;
    MPlug srcPlug, dstPlug;

    MObject alembicNodeObj = modifier.createNode("AlembicNode", &status);
    MFnDependencyNode alembicNodeFn(alembicNodeObj, &status);

    AlembicNode *alembicNodePtr =
        reinterpret_cast<AlembicNode*>(alembicNodeFn.userNode(&status));
    if (status == MS::kSuccess)
    {
        alembicNodePtr->setReaderPtrList(iArgData.mData);
        alembicNodePtr->setDebugMode(iArgData.mDebugOn);
    }

    // set AlembicNode name
    MString fileName;
    stripFileName(iArgData.mFileName, fileName);
    MString alembicNodeName = fileName +"_AlembicNode";
    alembicNodeFn.setName(alembicNodeName, &status);

    // set input file name
    MPlug plug = alembicNodeFn.findPlug("abc_File", true, &status);
    plug.setValue(iArgData.mFileName);

    // set sequence start and end in frames
    MTime sec(1.0, MTime::kSeconds);
    plug = alembicNodeFn.findPlug("startFrame", true, &status);
    plug.setValue(iArgData.mSequenceStartTime * sec.as(MTime::uiUnit()));

    plug = alembicNodeFn.findPlug("endFrame", true, &status);
    plug.setValue(iArgData.mSequenceEndTime * sec.as(MTime::uiUnit()));

    // set connect input info
    plug = alembicNodeFn.findPlug("connect", true, &status);
    plug.setValue(iArgData.mConnect);
    plug = alembicNodeFn.findPlug("createIfNotFound", true, &status);
    plug.setValue(iArgData.mCreateIfNotFound);
    plug = alembicNodeFn.findPlug("removeIfNoUpdate", true, &status);
    plug.setValue(iArgData.mRemoveIfNoUpdate);
    plug = alembicNodeFn.findPlug("connectRoot", true, &status);
    plug.setValue(iArgData.mConnectRootNodes);

    MFnIntArrayData fnIntArray;
    fnIntArray.create();
    MObject intArrayObj;
    MIntArray intArray;
    unsigned length;

    // set the mIsSampledXformOpAngle
    length = iArgData.mData.mIsSampledXformOpAngle.size();
    if (length > 0)
    {
        plug = alembicNodeFn.findPlug("transOpAngle", true, &status);
        intArray.setLength(length);
        for (unsigned int i = 0; i < length; i++)
            intArray.set(iArgData.mData.mIsSampledXformOpAngle[i], i);
        status = fnIntArray.set(intArray);
        intArrayObj = fnIntArray.object(&status);
        status = plug.setValue(intArrayObj);
    }

    // set the mNurbsCurveNumCurveList
    length = 0;//iArgData.mData.mNurbsCurveNumCurveList.size();
    if (length > 0)
    {
        plug = alembicNodeFn.findPlug("numCurve", true, &status);
        intArray.setLength(length);
        /*
        for (unsigned int i = 0; i < length; i++)
            intArray.set(iArgData.mData.mNumCurveList[i], i);
        */
        status = fnIntArray.set(intArray);
        intArrayObj = fnIntArray.object(&status);
        status = plug.setValue(intArrayObj);
    }

    // make connection: time1.outTime --> alembicNode.intime
    dstPlug = alembicNodeFn.findPlug("time", true, &status);
    status = getPlugByName("time1", "outTime", srcPlug);
    status = modifier.connect(srcPlug, dstPlug);
    status = modifier.doIt();

    unsigned int subDSize       = iArgData.mData.mSubDObjList.size();
    unsigned int polySize       = iArgData.mData.mPolyMeshObjList.size();
    unsigned int cameraSize     = 0;//iArgData.mData.mCameraObjList.size();
    unsigned int particleSize   = iArgData.mData.mPointsObjList.size();
    unsigned int xformSize      = iArgData.mData.mXformOpList.size();
    unsigned int nSurfaceSize   = 0;//iArgData.mData.mNurbsSurfaceObjList.size();
    unsigned int nCurveSize     = 0;//iArgData.mData.mNurbsCurveObjList.size();
    unsigned int propSize       = 0;//iArgData.mData.mPropList.size();

    // making dynamic connections
    if (particleSize > 0)
    {
        printWarning("Currently no support for animated particle system");
    }
    if (cameraSize > 0)
    {
        MPlug srcArrayPlug = alembicNodeFn.findPlug("outCamera", true);

        unsigned int logicalIndex = 0;
        for (unsigned int i = 0; i < cameraSize; i++)
        {
            MFnCamera fnCamera;//(iArgData.mData.mCameraObjList[i]);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("centerOfInterest", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("focalLength", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("lensSqueezeRatio", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("cameraScale", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("horizontalFilmAperture", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("verticalFilmAperture", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("horizontalFilmOffset", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("verticalFilmOffset", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("filmFitOffset", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("overscan", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("nearClipPlane", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("farClipPlane", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("preScale", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug  = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("filmTranslateH", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("filmTranslateV", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("horizontalRollPivot", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("verticalRollPivot", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("filmRollValue", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("postScale", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("orthographicWidth", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("fStop", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("focusDistance", true);
            status = modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("shutterAngle", true);
            status = modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("tumblePivotX", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("tumblePivotY", true);
            modifier.connect(srcPlug, dstPlug);

            srcPlug = srcArrayPlug.elementByLogicalIndex(logicalIndex++);
            dstPlug = fnCamera.findPlug("tumblePivotZ", true);
            modifier.connect(srcPlug, dstPlug);

            status = modifier.doIt();
        }
    }
    if (nSurfaceSize > 0)
    {
        /*
        MPlug srcArrayPlug = alembicNodeFn.findPlug("outNSurface", true);
        for (unsigned int i = 0; i < nSurfaceSize; i++)
        {
            srcPlug = srcArrayPlug.elementByLogicalIndex(i);
            MFnNurbsSurface fnNSurface(iArgData.mData.mNurbsSurfaceObjList[i]);
            dstPlug = fnNSurface.findPlug("create", true);
            modifier.connect(srcPlug, dstPlug);
            status = modifier.doIt();
        }
        */
    }
    if (nCurveSize > 0)
    {
        /*
        MPlug srcArrayPlug = alembicNodeFn.findPlug("outNCurveGrp", true);
        for (unsigned int i = 0; i < nCurveSize; i++)
        {
            srcPlug = srcArrayPlug.elementByLogicalIndex(i);
            MFnNurbsCurve fnNCurve(iArgData.mData.mNurbsCurveObjList[i]);
            dstPlug = fnNCurve.findPlug("create", true);
            modifier.connect(srcPlug, dstPlug);
            status = modifier.doIt();
        }
        */
    }

    if (propSize > 0)
    {
        /*
        MPlug srcArrayPlug = alembicNodeFn.findPlug("prop", true, &status);
        unsigned int index = 0;
        for (unsigned int i = 0 ; i < propSize; i++)
        {
            SampledPair mSampledPair = iArgData.mData.mPropList[i];
            MObject mObject = mSampledPair.getObject();
            MFnDependencyNode mFn(mObject);

            unsigned int sampleSize = mSampledPair.sampledChannelSize();
            for (unsigned int j = 0; j < sampleSize; j ++)
            {
                std::string attrName = mSampledPair.getSampleElement(j);
                if (attrName == "visible")
                    dstPlug = mFn.findPlug("visibility", true, &status);
                else
                    dstPlug = mFn.findPlug(attrName.c_str(), true, &status);

                if (status != MS::kSuccess)
                {
                    MString theError(attrName.c_str());
                    theError += MString(" not found for connection");
                    printError(theError);
                    continue;
                }

                if (!dstPlug.isCompound())
                {
                    srcPlug = srcArrayPlug.elementByLogicalIndex(index++);

                    if (!dstPlug.isConnected())
                    {
                        status = modifier.connect(srcPlug, dstPlug);
                        status = modifier.doIt();
                    }

                    if (status != MS::kSuccess)
                    {
                        MString theError(srcPlug.name());
                        theError += MString(" --> ");
                        theError += dstPlug.name();
                        theError += MString(" connection not made");
                        printError(theError);
                    }
                }
                else
                {
                    unsigned int numChildren = dstPlug.numChildren();
                    for (unsigned int i = 0; i < numChildren; i++)
                    {
                        srcPlug = srcArrayPlug.elementByLogicalIndex(index++);
                        MPlug childPlug = dstPlug.child(i, &status);
                        if (status == MS::kSuccess)
                        {
                            status = modifier.connect(srcPlug, childPlug);
                            status = modifier.doIt();
                            if (status != MS::kSuccess)
                            {
                                MString theError(srcPlug.name());
                                theError += MString(" --> ");
                                theError += childPlug.name();
                                theError +=
                                    MString(" child plug connection not made");
                                printError(theError);
                            }
                        }
                    }
                }
            }
        }
        */
    }

    if (subDSize > 0)
    {
        MPlug srcArrayPlug = alembicNodeFn.findPlug("outSubDMesh", true);
        for (unsigned int i = 0; i < subDSize; i++)
        {
            srcPlug = srcArrayPlug.elementByLogicalIndex(i);
            MFnMesh mFn(iArgData.mData.mSubDObjList[i], &status);
            dstPlug = mFn.findPlug("inMesh", true, &status);
            status = modifier.connect(srcPlug, dstPlug);
            status = modifier.doIt();
            if (status != MS::kSuccess)
            {
                MString theError("AlembicNode.outSubDMesh[");
                theError += i;
                theError += "] --> ";
                theError += mFn.name();
                theError += ".inMesh connection not made";
                printError(theError);
            }
        }
    }

    if (polySize > 0)
    {
        MPlug srcArrayPlug = alembicNodeFn.findPlug("outPolyMesh", true);
        for (unsigned int i = 0; i < polySize; i++)
        {
            srcPlug = srcArrayPlug.elementByLogicalIndex(i);
            MFnMesh mFn(iArgData.mData.mPolyMeshObjList[i]);
            dstPlug = mFn.findPlug("inMesh", true);
            modifier.connect(srcPlug, dstPlug);
            status = modifier.doIt();
        }
    }

    if (xformSize > 0)
    {
        unsigned int isSampledAngleIndex = 0;

        unsigned int logicalIndex = 0;
        MPlug srcArrayPlug = alembicNodeFn.findPlug("transOp", true);

        // for angleArray Attribute
        unsigned int angleLogicalIndex = 0;
        MPlug srcAngleArrayPlug =
            alembicNodeFn.findPlug("outTransOpAngle", true);

        for (unsigned int i = 0 ; i < xformSize; i++)
        {
            SampledPair mSampledPair = iArgData.mData.mXformOpList[i];
            MObject mObject = mSampledPair.getObject();
            MFnTransform mFn(mObject, &status);
            unsigned int sampleSize = mSampledPair.sampledChannelSize();
            for (unsigned int j = 0; j < sampleSize; j ++)
            {
                // is angle channel
                if (iArgData.mData.mIsSampledXformOpAngle[isSampledAngleIndex++]
                    == true)
                {
                    srcPlug = srcAngleArrayPlug.elementByLogicalIndex(
                        angleLogicalIndex++);
                }
                else
                {
                    srcPlug = srcArrayPlug.elementByLogicalIndex(
                        logicalIndex++);
                }

                std::string attrName = mSampledPair.getSampleElement(j);
                dstPlug = mFn.findPlug(attrName.c_str(), true);
                if (dstPlug.isNull())
                    continue;

                modifier.connect(srcPlug, dstPlug);
                status = modifier.doIt();
            }
        }
    }

    return alembicNodeFn.name();
}
