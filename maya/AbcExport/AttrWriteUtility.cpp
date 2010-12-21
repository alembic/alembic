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

bool CreatePropertyFromNumeric(MFnNumericData::Type iType,
    const MPlug& iPlug, Alembic::Abc::OCompoundProperty & iParent,
    std::vector < PlugAndObjScalar > & oScalarVec,
    std::vector < PlugAndObjArray > & oArrayVec)
{
    bool isArray =  iPlug.isArray();
    std::string plugName = iPlug.partialName(0, 0, 0, 0, 0, 1).asChar();
    switch (iType)
    {
        case MFnNumericData::kBoolean:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OBoolArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OBoolProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OCharArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OCharProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::kShort:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OInt16ArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OInt16Property(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::kInt:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OInt32ArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OInt32Property(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }

        case MFnNumericData::kFloat:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OFloatArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OFloatProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::kDouble:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = ODoubleArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = ODoubleProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::k2Short:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV2sArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV2sProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::k3Short:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV3sArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV3sProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::k2Int:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV2iArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV2iProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::k3Int:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV3iArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV3iProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::k2Float:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV2fArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV2fProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::k3Float:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;

                MFnAttribute mfnAttr(attr);
                if (mfnAttr.isUsedAsColor())
                {
                    p.prop = OC3fArrayProperty(iParent, plugName, iTimeType);
                }
                else
                {
                    p.prop = OV3fArrayProperty(iParent, plugName, iTimeType);
                }

                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                MFnAttribute mfnAttr(attr);
                if (mfnAttr.isUsedAsColor())
                {
                    p.prop = OC3fProperty(iParent, plugName, iTimeType);
                }
                else
                {
                    p.prop = OV3fProperty(iParent, plugName, iTimeType);
                }

                oStaticVec.push_back(p);
            }

            return true;
        }
        break;

        case MFnNumericData::k2Double:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV2dArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV2dProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::k3Double:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV3dArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV3dProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        case MFnNumericData::k4Double:
        {
            if (isArray)
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV4dArrayProperty(iParent, plugName, iTimeType);
                oArrayVec.push_back(p);
            }
            else
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OV4dProperty(iParent, plugName, iTimeType);
                oStaticVec.push_back(p);
            }
        }
        break;

        default:
        break;
    }

    return false;
}


template <class PROP>
bool MFnNumericDataToSample(MFnNumericData::Type iType,
    const MPlug& iPlug,
    const OSampleSelector & iSelect,
    PROP & oProp)
{
    size_t numElements =  iPlug.numElements();
    bool isScalar = oProp.isScalar();

    switch (iType)
    {
        case MFnNumericData::kBoolean:
        {
            if (numElements == 0)
            {
                bool_t val = iPlug.asBool();
                if ( isScalar )
                {
                    oProp.set(&val, iSelect);
                }
                else
                {
                    AbcA::ArraySample samp(&iVal, oProp.getDataType(),
                        Dimensions(1));
                    oProp.set(samp, iSelect);
            }
            else
            {
                // store in int16_t to avoid storing it in a string
                std::vector<bool_t> val(numElements);
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asBool();
                }
                oProp.first = val;
            }
            return true;
        }
        break;

        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        {
            if (numElements == 0)
            {
                int8_t val = static_cast<int8_t>(iPlug.asShort());
                oSamp.
                oProp.first = val;
            }
            else
            {
                // store in int16_t to avoid storing it in a string
                std::vector<int16_t> val(numElements);
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asShort();
                }
                oProp.first = val;
            }
            return true;
        }
        break;

        case MFnNumericData::kShort:
        {
            if (numElements == 0)
            {
                int16_t val = iPlug.asShort();
                oProp.first = val;
            }
            else
            {
                std::vector<int16_t> val(numElements);
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asShort();
                }
                oProp.first = val;
            }
            return true;
        }
        break;

        case MFnNumericData::kInt:
        {
            if (numElements == 0)
            {
                int32_t val = iPlug.asInt();
                oProp.first = val;
            }
            else
            {
                std::vector<int32_t> val(numElements);
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asInt();
                }
                oProp.first = val;
            }

            return true;
        }

        case MFnNumericData::kFloat:
        {
            if (numElements == 0)
            {
                float val = iPlug.asFloat();
                oProp.first = val;
            }
            else
            {
                std::vector<float> val(numElements);
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asFloat();
                }
                oProp.first = val;
            }

            return true;
        }
        break;

        case MFnNumericData::kDouble:
        {
            if (numElements == 0)
            {
                double val = iPlug.asDouble();
                oProp.first = val;
            }
            else
            {
                std::vector<double> val(numElements);
                for (size_t i = 0; i < numElements; ++i)
                {
                    val[i] = iPlug[i].asFloat();
                }
                oProp.first = val;
            }
            return true;
        }
        break;

        case MFnNumericData::k2Short:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <int16_t> val(2);
                numdFn.getData(val[0], val[1]);
                oProp.first = val;
            }
            else
            {
                std::vector<int16_t> val(numElements*2);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[2*i], val[2*i+1]);
                }
                oProp.first = val;
            }
            return true;
        }
        break;

        case MFnNumericData::k3Short:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <int16_t> val(3);
                numdFn.getData(val[0], val[1], val[2]);
                oProp.first = val;
            }
            else
            {
                std::vector<int16_t> val(numElements*3);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[3*i], val[3*i+1], val[3*i+2]);
                }
                oProp.first = val;
            }
            return true;
        }
        break;

        case MFnNumericData::k2Int:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <int32_t> val(2);
                numdFn.getData(val[0], val[1]);
                oProp.first = val;
            }
            else
            {
                std::vector<int32_t> val(numElements*2);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[2*i], val[2*i+1]);
                }
                oProp.first = val;
            }

            return true;
        }
        break;

        case MFnNumericData::k3Int:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <int32_t> val(3);
                numdFn.getData(val[0], val[1], val[2]);
                oProp.first = val;
            }
            else
            {
                std::vector<int32_t> val(numElements*3);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[3*i], val[3*i+1], val[3*i+2]);
                }
                oProp.first = val;
            }

            return true;
        }
        break;

        case MFnNumericData::k2Float:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <float> val(2);
                numdFn.getData(val[0], val[1]);
                oProp.first = val;
            }
            else
            {
                std::vector<float> val(numElements*2);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[2*i], val[2*i+1]);
                }
                oProp.first = val;
            }

            return true;
        }
        break;

        case MFnNumericData::k3Float:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <float> val(3);
                numdFn.getData(val[0], val[1], val[2]);
                oProp.first = val;
            }
            else
            {
                std::vector<float> val(numElements*3);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[3*i], val[3*i+1], val[3*i+2]);
                }
                oProp.first = val;
            }

            return true;
        }
        break;

        case MFnNumericData::k2Double:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <double> val(2);
                numdFn.getData(val[0], val[1]);
                oProp.first = val;
            }
            else
            {
                std::vector<double> val(numElements*2);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[2*i], val[2*i+1]);
                }
                oProp.first = val;
            }

            return true;
        }
        break;

        case MFnNumericData::k3Double:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <double> val(3);
                numdFn.getData(val[0], val[1], val[2]);
                oProp.first = val;
            }
            else
            {
                std::vector<double> val(numElements*3);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[3*i], val[3*i+1], val[3*i+2]);
                }
                oProp.first = val;
            }
            return true;
        }
        break;

        case MFnNumericData::k4Double:
        {
            if (numElements == 0)
            {
                MFnNumericData numdFn(iPlug.asMObject());

                std::vector <double> val(4);
                numdFn.getData(val[0], val[1], val[2], val[3]);
                oProp.first = val;
            }
            else
            {
                std::vector<double> val(numElements*4);
                for (size_t i = 0; i < numElements; ++i)
                {
                    MFnNumericData numdFn(iPlug[i].asMObject());
                    numdFn.getData(val[4*i], val[4*i+1], val[4*i+2],
                        val[4*i+3]);
                }
                oProp.first = val;
            }
            return true;
        }
        break;

        default:
        break;
    }
}


void CreatePropertyFromMFnAttr(const MObject& iAttr, const MPlug& iPlug,
    Alembic::Abc::OCompoundProperty & iParent,
    std::vector < PlugAndObjScalar > & oScalarVec,
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

        CreatePropertyFromNumeric(numFn.unitType(), iPlug, iParent,
            oScalarVec, oArrayVec);
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
                p.prop = OStringProperty(iParent, plugName, iTimeType);
                oScalarVec.push_back(p);
            }
            break;

            case MFnData::kStringArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OStringArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kDoubleArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = ODoubleArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kIntArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;
                p.prop = OInt32ArrayProperty(iParent, plugName,
                    iTimeType);
                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kPointArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;

                // should one day be OP4dArray when it exists
                p.prop = ODoubleArrayProperty(iParent, plugName,
                    iTimeType);

                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kVectorArray:
            {
                PlugAndObjArray p;
                p.plug = iPlug;
                p.obj = iAttr;

                // should one day be OP4dArray when it exists
                p.prop = OV3dArrayProperty(iParent, plugName,
                    iTimeType);

                oArrayVec.push_back(p);
            }
            break;

            case MFnData::kMatrix:
            {
                PlugAndObjScalar p;
                p.plug = iPlug;
                p.obj = iAttr;

                // should one day be OP4dArray when it exists
                p.prop = OM44dProperty(iParent, plugName, iTimeType);

                oScalarVec.push_back(p);
            }
            break;

            case MFnData::kNumeric:
            {
                CreatePropertyFromNumeric(numObj.numericType(), iPlug, iParent,
                    oScalarVec, oArrayVec);
            }
            break;

            default:
            break;
        }
    }

}

bool MFnAttrToPropertyPair(const MObject& iAttr, const MPlug& iPlug,
    SPI::SceneGraph::PropertyPair& oProp)
{
    MStatus stat;

    if (iAttr.hasFn(MFn::kNumericAttribute))
    {
        MFnNumericAttribute numFn(iAttr, &stat);

        if (!stat)
        {
            MString err = "Couldn't instantiate MFnNumericAttribute\n\tType: ";
            err += iAttr.apiTypeStr();
            MGlobal::displayError(err);

            return false;
        }

        return MFnNumericDataToPropertyPair(numFn.unitType(), iPlug, oProp);
    }
    else if (iAttr.hasFn(MFn::kTypedAttribute))
    {
        MFnTypedAttribute typeFn(iAttr, &stat);

        if (!stat)
        {
            MString err = "Couldn't instantiate MFnTypedAttribute\n\tType: ";
            err += iAttr.apiTypeStr();

            MGlobal::displayError(err);

            return false;
        }

        switch (typeFn.attrType())
        {
            case MFnData::kString:
            {
                std::string val = iPlug.asString().asChar();
                oProp.first = val;
                return true;
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
                oProp.first = val;
                return true;
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
                oProp.first = val;
                return true;
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
                oProp.first = val;
                return true;
            }
            break;

            case MFnData::kPointArray:
            {
                MFnPointArrayData arr(iPlug.asMObject());

                unsigned int i = 0;
                unsigned int length = arr.length();
                std::vector< double > val(length*4);
                for (; i < length; i++)
                {
                    MPoint pt(arr[i]);
                    val[4*i] = pt.x;
                    val[4*i+1] = pt.y;
                    val[4*i+2] = pt.z;
                    val[4*i+3] = pt.w;
                }
                oProp.first = val;
                oProp.second.inputType = SPI::SceneGraph::ARBATTR_POINT4;
                return true;
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
                    MVector v = arr[i];
                    val[3*i] = v.x;
                    val[3*i+1] = v.y;
                    val[3*i+2] = v.z;
                }
                oProp.first = val;
                oProp.second.inputType = SPI::SceneGraph::ARBATTR_VECTOR3;
                return true;
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
                oProp.first = val;
                return true;
            }
            break;

            case MFnData::kNumeric:
            {
                MFnNumericData numObj(iPlug.asMObject());
                return MFnNumericDataToPropertyPair(numObj.numericType(),
                    iPlug, oProp);
            }
            break;

            default:
            break;
        }

        return false;
    }
    return false;
}

}


bool util::attributeToPropertyPair(
    const MObject& iAttr,
    const MPlug& iPlug,
    SPI::SceneGraph::PropertyPair& oProp)
{
    if (iAttr.hasFn(MFn::kNumericAttribute) ||
        iAttr.hasFn(MFn::kTypedAttribute))
    {
        return MFnAttrToPropertyPair(iAttr, iPlug, oProp);
    }
    else if (iAttr.hasFn(MFn::kUnitAttribute))
    {
        double val = iPlug.asDouble();
        oProp.first = val;
        return true;
    }
    else if (iAttr.hasFn(MFn::kGenericAttribute))
    {
        return false;
    }
    else if (iAttr.hasFn(MFn::kEnumAttribute))
    {
        int16_t val = iPlug.asShort();
        oProp.first = val;
        return true;
    }

    return false;
}


// Converts a Maya attribute to a SceneGraph Property
void attributeToPropertyPair(const MObject& iAttr, const MPlug& iPlug,
    std::vector < PlugAndObjScalar > & oScalarProps, );
