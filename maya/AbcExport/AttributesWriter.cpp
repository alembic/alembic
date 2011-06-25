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
    Alembic::Util::uint32_t iTimeIndex,
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
                    iTimeIndex);
                p.prop = gp.getValueProperty();
            }
            else
            {
                AbcGeom::OV3fGeomParam gp(iParent, plugName, false, iScope, 1,
                    iTimeIndex);
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
                iTimeIndex);
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
                iTimeIndex);
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
            p.prop = Abc::OArrayProperty(iParent, plugName, dtype, iTimeIndex);
            oArrayVec.push_back(p);
        }
        break;

        default:
        break;
    }
}

bool MFnNumericDataToSample(MFnNumericData::Type iType,
    const MPlug& iPlug,
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
            oProp.set(samp);
        }
        break;

        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        {
            std::vector< Alembic::Util::int8_t > val(dimSize);
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
            oProp.set(samp);
        }
        break;

        case MFnNumericData::kShort:
        {
            std::vector< Alembic::Util::int16_t > val(dimSize);
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
            oProp.set(samp);
        }
        break;

        case MFnNumericData::kInt:
        {
            std::vector< Alembic::Util::int32_t > val(dimSize);
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
            oProp.set(samp);
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
            oProp.set(samp);
        }
        break;

        case MFnNumericData::kDouble:
        {
            std::vector<double> val(dimSize);
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
            oProp.set(samp);
        }
        break;

        case MFnNumericData::k2Short:
        {
            std::vector< Alembic::Util::int16_t > val(dimSize*2);
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
            oProp.set(samp);
        }
        break;

        case MFnNumericData::k3Short:
        {
            std::vector< Alembic::Util::int16_t > val(dimSize*3);
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
            oProp.set(samp);
        }
        break;

        case MFnNumericData::k2Int:
        {
            std::vector< Alembic::Util::int32_t > val(dimSize*2);
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
            oProp.set(samp);
        }
        break;

        case MFnNumericData::k3Int:
        {
            std::vector< Alembic::Util::int32_t > val(dimSize*3);
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
            oProp.set(samp);
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
            oProp.set(samp);
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
            oProp.set(samp);
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
            oProp.set(samp);
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
            oProp.set(samp);
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
            oProp.set(samp);
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
            return MFnNumericDataToSample(numObj.numericType(), iPlug,
                oProp);
        }
        break;

        case MFnData::kString:
        {
            std::vector< std::string > val(1);
            val[0] = iPlug.asString().asChar();
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(1));
            oProp.set(samp);
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
            oProp.set(samp);
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
            oProp.set(samp);
        }
        break;

        case MFnData::kIntArray:
        {
            MFnIntArrayData arr(iPlug.asMObject());

            unsigned int i = 0;
            unsigned int length = arr.length();
            std::vector< Alembic::Util::int32_t > val(length);
            for (; i < length; i++)
            {
                val[i] = arr[i];
            }
            AbcA::ArraySample samp(&(val.front()), oProp.getDataType(),
                Alembic::Util::Dimensions(length));
            oProp.set(samp);
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
            oProp.set(samp);
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
            oProp.set(samp);
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
            oProp.set(samp);
        }
        break;

        default:
            return false;
        break;
    }

    return true;
}


bool attributeToPropertyPair(const MObject& iAttr, const MPlug& iPlug,
    Abc::OArrayProperty & oProp)
{
    if (iAttr.hasFn(MFn::kTypedAttribute))
    {
        MFnTypedAttribute typedAttr(iAttr);
        return MFnTypedDataToSample(typedAttr.attrType(), iPlug,
            oProp);
    }
    else if (iAttr.hasFn(MFn::kNumericAttribute))
    {
        MFnNumericAttribute numAttr(iAttr);
        return MFnNumericDataToSample(numAttr.unitType(), iPlug,
            oProp);
    }
    else if (iAttr.hasFn(MFn::kUnitAttribute))
    {
        double val = iPlug.asDouble();
        AbcA::ArraySample samp(&val, oProp.getDataType(),
            Alembic::Util::Dimensions(1));
        oProp.set(samp);
        return true;
    }
    else if (iAttr.hasFn(MFn::kEnumAttribute))
    {
        Alembic::Util::int16_t val = iPlug.asShort();
        AbcA::ArraySample samp(&val, oProp.getDataType(),
            Alembic::Util::Dimensions(1));
        oProp.set(samp);
        return true;
    }

    return false;
}

void createPropertyFromMFnAttr(const MObject& iAttr, const MPlug& iPlug,
    Abc::OCompoundProperty & iParent,
    Alembic::Util::uint32_t iTimeIndex,
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
            iTimeIndex, iScope, oArrayVec);
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
                    1, iTimeIndex);
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
                    1, iTimeIndex);
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
                    1, iTimeIndex);
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
                    1, iTimeIndex);
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
                    iTimeIndex);
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
                    iTimeIndex);
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
                    iTimeIndex);
                p.prop = gp.getValueProperty();
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kNumeric:
            {
                MFnNumericAttribute numAttr(iPlug.asMObject());
                createPropertyFromNumeric(numAttr.unitType(), iAttr,
                    iPlug, iParent, iTimeIndex, iScope, oArrayVec);
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
            iTimeIndex);
        p.prop = gp.getValueProperty();
        oArrayVec.push_back(p);
    }
    else if (iAttr.hasFn(MFn::kEnumAttribute))
    {
        PlugAndObjArray p;
        p.plug = iPlug;
        p.obj = iAttr;
        AbcGeom::OInt16GeomParam gp(iParent, plugName, false, iScope, 1,
            iTimeIndex);
        p.prop = gp.getValueProperty();
        oArrayVec.push_back(p);
    }
}

}

AttributesWriter::AttributesWriter(
    Alembic::Abc::OCompoundProperty & iParent,
    Alembic::Abc::OObject & iParentObj,
    const MFnDagNode & iNode,
    Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs)
{
    PlugAndObjScalar visPlug;

    unsigned int attrCount = iNode.attributeCount();
    unsigned int i;

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
            if (iArgs.writeVisibility)
            {
                visPlug.plug = plug;
                visPlug.obj = attr;
                visPlug.propParent = iParentObj;
            }
            continue;
        }

        if (!iParent.valid() || !matchFilterOrAttribs(plug, iArgs))
        {
            continue;
        }

        int sampType = util::getSampledType(plug);

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
                createPropertyFromMFnAttr(attr, plug, iParent, 0,
                    scope, staticPlugObjArrayVec);
            }
            break;

            // sampled
            case 1:
            // curve treat like sampled
            case 2:
            {
                createPropertyFromMFnAttr(attr, plug, iParent, iTimeIndex,
                    scope, mPlugObjArrayVec);
            }
            break;
        }
    }

    std::vector< PlugAndObjArray >::iterator j =
        staticPlugObjArrayVec.begin();
    std::vector< PlugAndObjArray >::iterator jend =
        staticPlugObjArrayVec.end();

    // write the statics
    for (; j != jend; j++)
    {
        MString propName = j->plug.partialName(0, 0, 0, 0, 0, 1);
        bool filledProp = attributeToPropertyPair(j->obj, j->plug, j->prop);

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
        bool filledProp = attributeToPropertyPair(j->obj, j->plug,j->prop);

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

        // visible will go on the top most compound
        Abc::OCompoundProperty parent = visPlug.propParent.getProperties();

        switch (retVis)
        {
            // static visibility 0 case
            case 1:
            {
                Alembic::Util::int8_t visVal = 0;

                Abc::OCharProperty bp(parent, "visible");
                bp.set(visVal);
            }
            break;

            // animated visibility 0 case
            case 2:
            {
                Alembic::Util::int8_t visVal = 0;

                Abc::OCharProperty bp(parent, "visible", iTimeIndex);
                bp.set(visVal);
                visPlug.prop = bp;
                mAnimVisibility = visPlug;
            }
            break;

            // animated visibility 1 case
            case 3:
            {
                // dont add if we are forcing static (no frame range specified)
                if (iTimeIndex == 0)
                {
                    break;
                }

                mAnimVisibility = visPlug;
                Alembic::Util::int8_t visVal = -1;
                Abc::OCharProperty bp(parent, "visible", iTimeIndex);
                bp.set(visVal);
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

bool AttributesWriter::matchFilterOrAttribs(const MPlug & iPlug,
    const JobArgs & iArgs)
{

    MString propName = iPlug.partialName(0, 0, 0, 0, 0, 1);
    std::string name = propName.asChar();

    if (name.find("[") != std::string::npos)
    {
        return false;
    }

    std::vector<std::string>::const_iterator f;
    std::vector<std::string>::const_iterator fEnd =
        iArgs.prefixFilters.end();
    for (f = iArgs.prefixFilters.begin(); f != fEnd; ++f)
    {
        // check the prefilter and ignore those that match but end with
        // arb attr
        if (f->length() > 0 &&
            name.compare(0, f->length(), *f) == 0 &&
            !endsWithArbAttr(name) &&
            ( !iPlug.isChild() || !isDataAttr(iPlug.parent()) ))
        {
            return true;
        }
    }

    // check our specific list of attributes
    if (iArgs.attribs.find(name) != iArgs.attribs.end())
    {
        return true;
    }

    return false;
}

bool AttributesWriter::hasAnyAttr(const MFnDagNode & iNode,
    const JobArgs & iArgs)
{
    unsigned int attrCount = iNode.attributeCount();
    unsigned int i;

    std::vector< PlugAndObjArray > staticPlugObjArrayVec;

    for (i = 0; i < attrCount; i++)
    {
        MObject attr = iNode.attribute(i);
        MFnAttribute mfnAttr(attr);
        MPlug plug = iNode.findPlug(attr, true);

        // if it is not readable, then bail without any more checking
        if (!mfnAttr.isReadable() || plug.isNull())
        {
            continue;
        }

        if (matchFilterOrAttribs(plug, iArgs))
        {
            return true;
        }
    }

    return false;
}

AttributesWriter::~AttributesWriter()
{
    // if this happens to be set, clear it out before propParent goes out
    // of scope to work around issue 171
    mAnimVisibility.prop.reset();
}

bool AttributesWriter::isAnimated()
{
    return  !mPlugObjArrayVec.empty() || !mAnimVisibility.plug.isNull();
}

void AttributesWriter::write()
{

    std::vector< PlugAndObjArray >::iterator j =
        mPlugObjArrayVec.begin();
    std::vector< PlugAndObjArray >::iterator jend =
        mPlugObjArrayVec.end();

    for (; j != jend; j++)
    {
        MString propName = j->plug.partialName(0, 0, 0, 0, 0, 1);
        bool filledProp = attributeToPropertyPair(j->obj, j->plug, j->prop);

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
        Alembic::Util::int8_t visVal = -1;
        if (!mAnimVisibility.plug.asBool())
        {
            visVal = 0;
        }

        mAnimVisibility.prop.set(&visVal);
    }

}


