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

#include "AttributesWriter.h"
#include "MayaUtility.h"

namespace Abc = Alembic::Abc;
namespace AbcGeom = Alembic::AbcGeom;
namespace AbcA = Alembic::AbcCoreAbstract;

namespace {

static const char * cAttrScope = "_AttrScope";

// returns true if a plug is of a simple numeric data type
bool isDataAttr(const MPlug & iParent)
{
    MObject obj = iParent.asMObject();
    switch (obj.apiType())
    {
        case MFn::kData2Double:
        case MFn::kData3Double:
        case MFn::kData4Double:
        case MFn::kData2Float:
        case MFn::kData3Float:
        case MFn::kData2Int:
        case MFn::kData3Int:
        case MFn::kData2Short:
        case MFn::kData3Short:
            return true;
        break;

        default:
            return false;
        break;
    }
    return false;
}

AbcGeom::GeometryScope strToScope(MString iStr)
{
    iStr.toLowerCase();
    if (iStr == "point")
    {
        return AbcGeom::kVertexScope;
    }
    else if (iStr == "vertex")
    {
        return AbcGeom::kFacevaryingScope;
    }
    else if (iStr == "face")
    {
        return AbcGeom::kUniformScope;
    }

    return AbcGeom::kConstantScope;
}

// returns true if the string ends with _AttrScope
bool endsWithArbAttr(const std::string & iStr)
{
    size_t len = iStr.size();

    return (len >= 10 && iStr.compare(len - 10, 10, cAttrScope) == 0);
}


void createPropertyFromNumeric(MFnNumericData::Type iType, const MObject& iAttr,
    const MPlug& iPlug, Abc::OCompoundProperty & iParent,
    AbcA::TimeSamplingType & iTimeType,
    AbcGeom::GeometryScope iScope,
    std::vector < PlugAndObjArray > & oArrayVec)
{
    std::string plugName = iPlug.partialName(0, 0, 0, 0, 0, 1).asChar();
    switch (iType)
    {
        case MFnNumericData::kBoolean:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OBoolGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OCharGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kShort:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OInt16GeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kInt:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OInt32GeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kFloat:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OFloatGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kDouble:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::ODoubleGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k2Short:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OV2sGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k3Short:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OV3sGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k2Int:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OV2iGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k3Int:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OV3iGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k2Float:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OV2fGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k3Float:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;

            MFnAttribute mfnAttr(iAttr);
            if (mfnAttr.isUsedAsColor())
            {
                AbcGeom::OC3fGeomParam gp(iParent, plugName, false, iScope, 1,
                    iTimeType);
                p.prop = gp.getValueProperty();
            }
            else
            {
                AbcGeom::OV3fGeomParam gp(iParent, plugName, false, iScope, 1,
                    iTimeType);
                p.prop = gp.getValueProperty();
            }

            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k2Double:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OV2dGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k3Double:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcGeom::OV3dGeomParam gp(iParent, plugName, false, iScope, 1,
                iTimeType);
            p.prop = gp.getValueProperty();
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k4Double:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            AbcA::DataType dtype(Alembic::Util::kFloat64POD, 4);
            p.prop = Abc::OArrayProperty(iParent, plugName, dtype, iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        default:
        break;
    }
}

bool MFnNumericDataToSample(MFnNumericData::Type iType,
    const MPlug& iPlug,
    const Abc::OSampleSelector & iSelect,
    Abc::OArrayProperty & oProp)
{
    size_t numElements =  iPlug.numElements();
    bool isArray = iPlug.isArray();

    size_t dimSize = numElements;
    if (!isArray)
        dimSize = 1;

    switch (iType)
    {
        case MFnNumericData::kBoolean:
        {
            std::vector< Alembic::Util::bool_t > val(dimSize);
            if (!isArray)
            {
                val[0] = iPlug.asBool();
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asBool();
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        {
            std::vector<int8_t> val(dimSize);
            if (!isArray)
            {
                val[0] = iPlug.asChar();
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asChar();
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::kShort:
        {
            std::vector<int16_t> val(dimSize);
            if (!isArray)
            {
                val[0] = iPlug.asShort();
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asShort();
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::kInt:
        {
            std::vector<int32_t> val(dimSize);
            if (!isArray)
            {
                val[0] = iPlug.asInt();
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asInt();
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::kFloat:
        {
            std::vector<float> val(dimSize);
            if (!isArray)
            {
                val[0] = iPlug.asFloat();
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asFloat();
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::kDouble:
        {
            std::vector<float> val(dimSize);
            if (!isArray)
            {
                val[0] = iPlug.asDouble();
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asDouble();
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k2Short:
        {
            std::vector<int16_t> val(dimSize*2);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[2*i], val[2*i+1]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k3Short:
        {
            std::vector<int16_t> val(dimSize*3);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1], val[2]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[3*i], val[3*i+1], val[3*i+2]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k2Int:
        {
            std::vector<int32_t> val(dimSize*2);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[2*i], val[2*i+1]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k3Int:
        {
            std::vector<int32_t> val(dimSize*3);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1], val[2]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[3*i], val[3*i+1], val[3*i+2]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k2Float:
        {
            std::vector<float> val(dimSize*2);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[2*i], val[2*i+1]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k3Float:
        {
            std::vector<float> val(dimSize*3);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1], val[2]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[3*i], val[3*i+1], val[3*i+2]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k2Double:
        {
            std::vector<double> val(dimSize*2);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[2*i], val[2*i+1]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k3Double:
        {
            std::vector<double> val(dimSize*3);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1], val[2]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[3*i], val[3*i+1], val[3*i+2]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnNumericData::k4Double:
        {
            std::vector<double> val(dimSize*4);
            if (!isArray)
            {
                MFnNumericData numdFn(iPlug.asMObject());
                numdFn.getData(val[0], val[1], val[2], val[3]);
            }
            else
            {
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[4*i], val[4*i+1], val[4*i+2],
                        val[4*i+3]);
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        default:
            return false;
        break;
    }

    return true;
}

bool MFnTypedDataToSample(MFnData::Type iType,
    const MPlug& iPlug,
    const Abc::OSampleSelector & iSelect,
    Abc::OArrayProperty & oProp)
{
    size_t numElements =  iPlug.numElements();
    bool isArray = iPlug.isArray();

    size_t dimSize = numElements;
    if (!isArray)
        dimSize = 1;

    switch (iType)
    {
        case MFnData::kNumeric:
        {
            MFnNumericData numObj(iPlug.asMObject());
            return MFnNumericDataToSample(numObj.numericType(), iPlug, iSelect,
                oProp);
        }
        break;

        case MFnData::kString:
        {
            std::vector< std::string > val(1);
            val[0] = iPlug.asString().asChar();
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(1));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnData::kStringArray:
        {
            MFnStringArrayData arr(iPlug.asMObject());

            unsigned int i = 0;
            unsigned int length = arr.length();
            std::vector< std::string > val(length);
            for (; i < length; i++)
            {
                val[i] = arr[i].asChar();
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(length));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnData::kDoubleArray:
        {
            MFnDoubleArrayData arr(iPlug.asMObject());

            unsigned int i = 0;
            unsigned int length = arr.length();
            std::vector< double > val(length);
            for (; i < length; i++)
            {
                val[i] = arr[i];
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(length));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnData::kIntArray:
        {
            MFnIntArrayData arr(iPlug.asMObject());

            unsigned int i = 0;
            unsigned int length = arr.length();
            std::vector< int32_t > val(length);
            for (; i < length; i++)
            {
                val[i] = arr[i];
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(length));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnData::kPointArray:
        {
            MFnPointArrayData arr(iPlug.asMObject());

            unsigned int i = 0;
            unsigned int length = arr.length();
            std::vector< double > val(length*3);
            for (; i < length; i++)
            {
                MPoint pt(arr[i]);
                val[3*i] = pt.x;
                val[3*i+1] = pt.y;
                val[3*i+2] = pt.z;
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(length));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnData::kVectorArray:
        {
            MFnVectorArrayData arr(iPlug.asMObject());

            unsigned int i = 0;
            unsigned int length = arr.length();
            std::vector< double > val(length*3);
            for (; i < length; i++)
            {
                MVector v(arr[i]);
                val[3*i] = v.x;
                val[3*i+1] = v.y;
                val[3*i+2] = v.z;
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(length));
            oProp.set(samp, iSelect);
        }
        break;

        case MFnData::kMatrix:
        {
            MFnMatrixData arr(iPlug.asMObject());
            MMatrix mat = arr.matrix();
            std::vector<double> val(16);

            unsigned int r, c, i = 0;
            for (r = 0; r < 4; r++)
            {
                for (c = 0; c < 4; c++, i++)
                {
                    val[i] = mat[r][c];
                }
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(1));
            oProp.set(samp, iSelect);
        }
        break;

        default:
            return false;
        break;
    }

    return true;
}


bool attributeToPropertyPair(const MObject& iAttr, const MPlug& iPlug,
    const Abc::OSampleSelector & iSelect,
    Abc::OArrayProperty & oProp)
{
    if (iAttr.hasFn(MFn::kTypedAttribute))
    {
        MFnTypedAttribute typedAttr(iAttr);
        return MFnTypedDataToSample(typedAttr.attrType(), iPlug, iSelect,
            oProp);
    }
    else if (iAttr.hasFn(MFn::kNumericAttribute))
    {
        MFnNumericAttribute numAttr(iAttr);
        return MFnNumericDataToSample(numAttr.unitType(), iPlug, iSelect,
            oProp);
    }
    else if (iAttr.hasFn(MFn::kUnitAttribute))
    {
        double val = iPlug.asDouble();
        AbcA::ArraySample samp(&val, oProp.getDataType(),
            Alembic::Util::Dimensions(1));
        oProp.set(samp, iSelect);
        return true;
    }
    else if (iAttr.hasFn(MFn::kEnumAttribute))
    {
        int16_t val = iPlug.asShort();
        AbcA::ArraySample samp(&val, oProp.getDataType(),
            Alembic::Util::Dimensions(1));
        oProp.set(samp, iSelect);
        return true;
    }

    return false;
}

void createPropertyFromMFnAttr(const MObject& iAttr, const MPlug& iPlug,
    Abc::OCompoundProperty & iParent,
    AbcA::TimeSamplingType & iTimeType,
    AbcGeom::GeometryScope iScope,
    std::vector < PlugAndObjArray > & oArrayVec)
{
    // for some reason we have just 1 of the elements of an array, bail
    if (iPlug.isElement())
        return;

    MStatus stat;

    std::string plugName = iPlug.partialName(0, 0, 0, 0, 0, 1).asChar();

    if (iAttr.hasFn(MFn::kNumericAttribute))
    {
        MFnNumericAttribute numFn(iAttr, &stat);

        if (!stat)
        {
            MString err = "Couldn't instantiate MFnNumericAttribute\n\tType: ";
            err += iAttr.apiTypeStr();
            MGlobal::displayError(err);

            return;
        }

        createPropertyFromNumeric(numFn.unitType(), iAttr, iPlug, iParent,
            iTimeType, iScope, oArrayVec);
    }
    else if (iAttr.hasFn(MFn::kTypedAttribute))
    {
        MFnTypedAttribute typeFn(iAttr, &stat);

        if (!stat)
        {
            MString err = "Couldn't instantiate MFnTypedAttribute\n\tType: ";
            err += iAttr.apiTypeStr();

            MGlobal::displayError(err);

            return;
        }

        switch (typeFn.attrType())
        {
            case MFnData::kString:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                AbcGeom::OStringGeomParam gp(iParent, plugName, false, iScope,
                    1, iTimeType);
                p.prop = gp.getValueProperty();
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kStringArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                AbcGeom::OStringGeomParam gp(iParent, plugName, false, iScope,
                    1, iTimeType);
                p.prop = gp.getValueProperty();
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kDoubleArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                AbcGeom::ODoubleGeomParam gp(iParent, plugName, false, iScope,
                    1, iTimeType);
                p.prop = gp.getValueProperty();
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kIntArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                AbcGeom::OInt32GeomParam gp(iParent, plugName, false, iScope,
                    1, iTimeType);
                p.prop = gp.getValueProperty();
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kPointArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                AbcGeom::OP3dGeomParam gp(iParent, plugName, false, iScope, 1,
                    iTimeType);
                p.prop = gp.getValueProperty();
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kVectorArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                AbcGeom::OV3dGeomParam gp(iParent, plugName, false, iScope, 1,
                    iTimeType);
                p.prop = gp.getValueProperty();
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kMatrix:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                AbcGeom::OM44dGeomParam gp(iParent, plugName, false, iScope, 1,
                    iTimeType);
                p.prop = gp.getValueProperty();
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kNumeric:
            {
                MFnNumericAttribute numAttr(iPlug.asMObject());
                createPropertyFromNumeric(numAttr.unitType(), iAttr,
                    iPlug, iParent, iTimeType, iScope, oArrayVec);
            }
            break;

            default:
            {
                // get the full property name for the warning
                MString msg = "WARNING: Couldn't convert ";
                msg += iPlug.partialName(1, 0, 0, 0, 1, 1);
                msg += " to a property, so skipping.";
                MGlobal::displayWarning(msg);
            }
            break;
        }
    }
    else if (iAttr.hasFn(MFn::kUnitAttribute))
    {
        PlugAndObjArray p;
        p.plug = iPlug;
        p.obj = iAttr;
        AbcGeom::ODoubleGeomParam gp(iParent, plugName, false, iScope, 1,
            iTimeType);
        p.prop = gp.getValueProperty();
        oArrayVec.push_back(p);
    }
    else if (iAttr.hasFn(MFn::kEnumAttribute))
    {
        PlugAndObjArray p;
        p.plug = iPlug;
        p.obj = iAttr;
        AbcGeom::OInt16GeomParam gp(iParent, plugName, false, iScope, 1,
            iTimeType);
        p.prop = gp.getValueProperty();
        oArrayVec.push_back(p);
    }
}

}

std::string * AttributesWriter::mFilter = NULL;
std::set<std::string> * AttributesWriter::mAttribs = NULL;

AttributesWriter::AttributesWriter(
    double iFrame,
    Abc::OCompoundProperty & iParent,
    const MFnDagNode & iNode,
    AbcA::TimeSamplingType & iTimeType,
    bool iWriteVisibility)
{
    mCurIndex = 0;

    PlugAndObjScalar visPlug;

    unsigned int attrCount = iNode.attributeCount();
    unsigned int i;

    int filtLen = 0;
    if (mFilter != NULL)
        filtLen = mFilter->length();

    std::vector< PlugAndObjArray > staticPlugObjArrayVec;

    for (i = 0; i < attrCount; i++)
    {
        MObject attr = iNode.attribute(i);
        MFnAttribute mfnAttr(attr);
        MPlug plug = iNode.findPlug(attr, true);

        // if it is not readable, then bail without any more checking
        if (!mfnAttr.isReadable() || plug.isNull())
            continue;

        MString propName = plug.partialName(0, 0, 0, 0, 0, 1);

        std::string propStr = propName.asChar();

        // we handle visibility in a special way
        if (propStr == "visibility")
        {
            if (iWriteVisibility)
            {
                visPlug.plug = plug;
                visPlug.obj = attr;
            }
            continue;
        }

        // skip it if it doesn't start with our filter, or it ends with our
        // special case attrs or it is a child attr and the parent is a
        // kData* and it is not in our attribute set
        if ( propStr.find("[") != std::string::npos || ((filtLen == 0 ||
            propStr.compare(0, filtLen, *mFilter) != 0 ||
            endsWithArbAttr(propStr) ||
            (plug.isChild() && isDataAttr(plug.parent())) ) &&
            (!mAttribs || mAttribs->find(propStr) == mAttribs->end())) )
        {
            continue;
        }

        int sampType = 0;

        // if we aren't forcing everything to be static
        if (iFrame != DBL_MAX)
        {
            sampType = util::getSampledType(plug);
        }

        MPlug scopePlug = iNode.findPlug(propName + cAttrScope);
        AbcGeom::GeometryScope scope = AbcGeom::kUnknownScope;

        if (!scopePlug.isNull())
        {
            scope = strToScope(scopePlug.asString());
        }

        switch (sampType)
        {
            // static
            case 0:
            {
                createPropertyFromMFnAttr(attr, plug, iParent, iTimeType, scope,
                    staticPlugObjArrayVec);
            }
            break;

            // sampled
            case 1:
            // curve treat like sampled
            case 2:
            {
                createPropertyFromMFnAttr(attr, plug, iParent, iTimeType, scope,
                    mPlugObjArrayVec);
            }
            break;
        }
    }

    std::vector< PlugAndObjArray >::iterator j =
        staticPlugObjArrayVec.begin();
    std::vector< PlugAndObjArray >::iterator jend =
        staticPlugObjArrayVec.end();

    double frame = iFrame;
    if (frame == DBL_MAX)
        frame = 0;

    Abc::OSampleSelector sel(0, frame);

    // write the statics
    for (; j != jend; j++)
    {
        MString propName = j->plug.partialName(0, 0, 0, 0, 0, 1);
        bool filledProp = attributeToPropertyPair(j->obj, j->plug, sel, 
            j->prop);

        if (!filledProp)
        {
            MString msg = "WARNING: Couldn't get static property ";
            msg += j->plug.partialName(1, 0, 0, 0, 1, 1);
            msg += ", so skipping.";
            MGlobal::displayWarning(msg);
            continue;
        }
    }

    j = mPlugObjArrayVec.begin();
    jend = mPlugObjArrayVec.end();
    for (; j != jend; j++)
    {
        MString propName = j->plug.partialName(0, 0, 0, 0, 0, 1);
        bool filledProp = attributeToPropertyPair(j->obj, j->plug, sel, 
            j->prop);

        if (!filledProp)
        {
            MString msg = "WARNING: Couldn't get static property ";
            msg += j->plug.partialName(1, 0, 0, 0, 1, 1);
            msg += ", so skipping.";
            MGlobal::displayWarning(msg);
            continue;
        }
    }

    if (!visPlug.plug.isNull())
    {
        int retVis = util::getVisibilityType(visPlug.plug);

        switch (retVis)
        {
            // static visibility 0 case
            case 1:
            {
                int8_t visVal = 0;

                Abc::OCharProperty bp(iParent, "visible");
                bp.set(visVal);
            }
            break;

            // animated visibility 0 case
            case 2:
            {
                int8_t visVal = 0;

                if (iFrame != DBL_MAX)
                {
                    Abc::OCharProperty bp(iParent, "visible",
                        iTimeType);
                    bp.set(visVal, Abc::OSampleSelector(mCurIndex, iFrame));
                    visPlug.prop = bp;
                    mAnimVisibility = visPlug;
                }
                // force static case
                else
                {
                    Abc::OCharProperty bp(iParent, "visible");
                    bp.set(visVal);
                }
            }
            break;

            // animated visibility 1 case
            case 3:
            {
                // dont add if we are forcing static
                if (iFrame == DBL_MAX)
                {
                    break;
                }

                mAnimVisibility = visPlug;
                int8_t visVal = -1;
                Abc::OCharProperty bp(iParent, "visible", iTimeType);
                bp.set(visVal, Abc::OSampleSelector(mCurIndex, iFrame));
                visPlug.prop = bp;
                mAnimVisibility = visPlug;

            }
            break;

            // dont write any visibility
            default:
            break;
        }
    }
}

AttributesWriter::~AttributesWriter()
{
}

bool AttributesWriter::isAnimated()
{
    return  !mPlugObjArrayVec.empty() || !mAnimVisibility.plug.isNull();
}

void AttributesWriter::write(double iFrame)
{
    mCurIndex ++;

    std::vector< PlugAndObjArray >::iterator j =
        mPlugObjArrayVec.begin();
    std::vector< PlugAndObjArray >::iterator jend =
        mPlugObjArrayVec.end();

    Abc::OSampleSelector sel(mCurIndex, iFrame);

    for (; j != jend; j++)
    {
        MString propName = j->plug.partialName(0, 0, 0, 0, 0, 1);
        bool filledProp = attributeToPropertyPair(j->obj, j->plug, sel, 
            j->prop);

        if (!filledProp)
        {
            MString msg = "WARNING: Couldn't get sampled property ";
            msg += j->plug.partialName(1, 0, 0, 0, 1, 1);
            msg += ", so skipping.";
            MGlobal::displayWarning(msg);
            continue;
        }
    }

    if (!mAnimVisibility.plug.isNull())
    {
        int8_t visVal = -1;
        if (!mAnimVisibility.plug.asBool())
        {
            visVal = 0;
        }

        mAnimVisibility.prop.set(&visVal, sel);
    }

}


