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

// not done, needs to work, then needs more polish

namespace {

void CreatePropertyFromNumeric(MFnNumericData::Type iType,
    const MPlug& iPlug, Alembic::Abc::OCompoundProperty & iParent,
    Alembic::AbcCoreAbstract::TimeSamplingType & iTimeType,
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
            p.prop = Alembic::Abc::OBoolArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OCharArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kShort:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OInt16ArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kInt:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OInt32ArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }

        case MFnNumericData::kFloat:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OFloatArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::kDouble:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::ODoubleArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k2Short:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OV2sArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k3Short:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OV3sArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k2Int:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OV2iArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k3Int:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OV3iArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k2Float:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OV2fArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k3Float:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;

            MFnAttribute mfnAttr(attr);
            if (mfnAttr.isUsedAsColor())
            {
                p.prop = Alembic::Abc::OC3fArrayProperty(iParent, plugName,
                    iTimeType);
            }
            else
            {
                p.prop = Alembic::Abc::OV3fArrayProperty(iParent, plugName,
                    iTimeType);
            }

            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k2Double:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OV2dArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k3Double:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            p.prop = Alembic::Abc::OV3dArrayProperty(iParent, plugName,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        case MFnNumericData::k4Double:
        {
            PlugAndObjArray p;
            p.plug = iPlug;
            p.obj = iAttr;
            Alembic::AbcCoreAbstract::v1::DataType dtype(
                Alembic::Util::kFloat64POD, 4);
            p.prop = Alembic::Abc::OArrayProperty(iParent, plugName, dtype,
                iTimeType);
            oArrayVec.push_back(p);
        }
        break;

        default:
        break;
    }
}

bool MFnNumericDataToSample(MFnNumericData::Type iType,
    const MPlug& iPlug,
    const OSampleSelector & iSelect,
    OArrayProperty & oProp)
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
            std::vector<bool_t> val(dimSize);
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }

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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
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
                Dimensions(dimSize));
            oProp.set(samp, iSelect);
        }
        break;

        default:
            return false;
        break;
    }

    return true;
}

bool MFnNumericDataToSample(MFnData::Type iType,
    const MPlug& iPlug,
    const OSampleSelector & iSelect,
    OArrayProperty & oProp)
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
                Dimensions(1));
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
                Dimensions(length));
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
                Dimensions(length));
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
                Dimensions(length));
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
                Dimensions(length));
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
                Dimensions(length));
            oProp.set(samp, iSelect);
        }
        break;

        class MFnData::kMatrix:
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
                Dimensions(1));
            oProp.set(samp, iSelect);
        }
        break;

        default:
            return false;
        break;
    }

    return true;
}

}

bool attributeToPropertyPair(const MObject& iAttr, const MPlug& iPlug,
    const Alembic::Abc::OSampleSelector & iSelect,
    Alembic::Abc::OArrayProperty & oProp)
{
    if (iAttr.hasFn(MFn::kTypedAttribute))
    {
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
        AbcA::ArraySample samp(&val, oProp.getDataType(), Dimensions(1));
        oProp.set(samp, iSelect);
        return true;
    }
    else if (iAttr.hasFn(MFn::kEnumAttribute))
    {
        int16_t val = iPlug.asShort();
        AbcA::ArraySample samp(&val, oProp.getDataType(), Dimensions(1));
        oProp.set(samp, iSelect);
        return true;
    }

    return false;
}

void createPropertyFromMFnAttr(const MObject& iAttr, const MPlug& iPlug,
    Alembic::Abc::OCompoundProperty & iParent,
    Alembic::AbcCoreAbstract::TimeSamplingType & iTimeType,
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

        CreatePropertyFromNumeric(numFn.unitType(), iPlug, iParent, iTimeType,
            oArrayVec);
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
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = Alembic::Abc::OStringArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kStringArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = Alembic::Abc::OStringArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kDoubleArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = Alembic::Abc::ODoubleArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kIntArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = Alembic::Abc::OInt32ArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kPointArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = Alembic::Abc::OP3dArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kVectorArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = Alembic::Abc::OV3dArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kMatrix:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = Alembic::Abc::OM44dArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kNumeric:
            {
                CreatePropertyFromNumeric(numObj.numericType(), iPlug, iParent,
                    iTimeType, oArrayVec);
            }
            break;

            default:
            break;
        }
    }
    else if (iAttr.hasFn(MFn::kUnitAttribute))
    {
        PlugAndObjArray p;
        p.plug = iPlug;
        p.obj = iAttr;
        p.prop = Alembic::Abc::ODoubleArrayProperty(iParent, plugName,
            iTimeType);
        oArrayVec.push_back(p);
    }
    else if (iAttr.hasFn(MFn::kEnumAttribute))
    {
        PlugAndObjArray p;
        p.plug = iPlug;
        p.obj = iAttr;
        p.prop = Alembic::Abc::OInt16ArrayProperty(iParent, plugName,
            iTimeType);
        oArrayVec.push_back(p);
    }
}
