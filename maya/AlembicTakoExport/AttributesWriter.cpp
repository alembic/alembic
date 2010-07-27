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

#include <AlembicTakoExport/AttributesWriter.h>
#include <AlembicTakoExport/MayaUtility.h>

namespace Alembic {

namespace {

static const char * cAttrIn = "_AttrInput";
static const char * cAttrOut = "_AttrOutput";
static const char * cAttrScope = "_AttrScope";

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

Tako::ScopeType strToScope(MString iStr)
{
    iStr.toLowerCase();
    if (iStr == "point")
    {
        return Tako::SCOPE_POINT;
    }
    else if (iStr == "vertex")
    {
        return Tako::SCOPE_VERTEX;
    }
    else if (iStr == "face")
    {
        return Tako::SCOPE_FACE;
    }

    return Tako::SCOPE_PRIMITIVE;
}

Tako::ArbAttrType strToAttr(MString iStr)
{
    iStr.toLowerCase();

    if (iStr == "color3")
    {
        return Tako::ARBATTR_COLOR3;
    }
    else if (iStr == "color4")
    {
        return Tako::ARBATTR_COLOR4;
    }
    else if (iStr == "normal2")
    {
        return Tako::ARBATTR_NORMAL2;
    }
    else if (iStr == "normal3")
    {
        return Tako::ARBATTR_NORMAL3;
    }
    else if (iStr == "vector2")
    {
        return Tako::ARBATTR_VECTOR2;
    }
    else if (iStr == "vector3")
    {
        return Tako::ARBATTR_VECTOR3;
    }
    else if (iStr == "vector4")
    {
        return Tako::ARBATTR_VECTOR4;
    }
    else if (iStr == "point2")
    {
        return Tako::ARBATTR_POINT2;
    }
    else if (iStr == "point3")
    {
        return Tako::ARBATTR_POINT3;
    }
    else if (iStr == "point4")
    {
        return Tako::ARBATTR_POINT4;
    }
    else if (iStr == "int")
    {
        return Tako::ARBATTR_INT;
    }
    else if (iStr == "float")
    {
        return Tako::ARBATTR_FLOAT;
    }
    else if (iStr == "double")
    {
        return Tako::ARBATTR_DOUBLE;
    }
    else if (iStr == "string")
    {
        return Tako::ARBATTR_STRING;
    }

    // default to NONE because a match wasn't found
    return Tako::ARBATTR_NONE;
}

// returns true if the string ends with _AttrInput,
// _AttrOutput, or _AttrScope
bool endsWithArbAttr(const std::string & iStr)
{
    size_t len = iStr.size();

    return ((len >= 10 && iStr.compare(len - 10, 10, cAttrIn) == 0) ||
        (len >= 11 && iStr.compare(len - 11, 11, cAttrOut) == 0) ||
        (len >= 10 && iStr.compare(len - 10, 10, cAttrScope) == 0));
}

class ColorTypeVisitor : public boost::static_visitor< Tako::ArbAttrType >
{
    public:

        ColorTypeVisitor()
        {
        };

        Tako::ArbAttrType checkSize(size_t iSize)
        {
            switch (iSize)
            {
                case 3:
                return Tako::ARBATTR_COLOR3;

                case 4:
                return Tako::ARBATTR_COLOR4;

                default:
                break;
            }
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(int8_t  iProp)
        {
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(int16_t iProp)
        {
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(int32_t iProp)
        {
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(int64_t iProp)
        {
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(float iProp)
        {
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(double iProp)
        {
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(std::string & iProp)
        {
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(std::vector<std::string> & iProp)
        {
            return Tako::ARBATTR_NONE;
        }

        Tako::ArbAttrType operator()(std::vector<int16_t> & iProp)
        {
            return checkSize(iProp.size());
        }

        Tako::ArbAttrType operator()(std::vector<int32_t> & iProp)
        {
            return checkSize(iProp.size());
        }

        Tako::ArbAttrType operator()(std::vector<int64_t> & iProp)
        {
            return checkSize(iProp.size());
        }

        Tako::ArbAttrType operator()(std::vector<float> & iProp)
        {
            return checkSize(iProp.size());
        }

        Tako::ArbAttrType operator()(std::vector<double> & iProp)
        {
            return checkSize(iProp.size());
        }
};
}

std::string * AttributesWriter::mFilter = NULL;
std::set<std::string> * AttributesWriter::mAttribs = NULL;

AttributesWriter::AttributesWriter(
    float iFrame,
    Tako::HDFWriterNode & iWriter,
    const MFnDagNode & iNode,
    bool iWriteVisibility) :
    mWriter(iWriter)
{
    ColorTypeVisitor colorVisit;

    PlugAndObj visPlug;

    unsigned int attrCount = iNode.attributeCount();
    unsigned int i;

    int filtLen = 0;
    if (mFilter != NULL)
        filtLen = mFilter->length();

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
        if ( (filtLen == 0 ||
            propStr.compare(0, filtLen, *mFilter) != 0 ||
            endsWithArbAttr(propStr) ||
            (plug.isChild() && isDataAttr(plug.parent())) ) &&
            (!mAttribs || mAttribs->find(propStr) == mAttribs->end()) )
        {
            continue;
        }

        int sampType = 0;

        // in this case we are forcing everything to be static
        if (iFrame != FLT_MAX)
        {
            sampType = util::getSampledType(plug);
        }

        Tako::PropertyPair prop;
        bool filledProp = util::attributeToPropertyPair(attr, plug, prop);

        if (!filledProp)
        {
            // get the full property name for the warning
            MString msg = "WARNING: Couldn't convert ";
            msg += plug.partialName(1, 0, 0, 0, 1, 1);
            msg += " to a property, so skipping.";
            MGlobal::displayWarning(msg);
            continue;
        }

        if (mfnAttr.isUsedAsColor())
        {
            prop.second.inputType = boost::apply_visitor(colorVisit,
                prop.first);
        }

        MPlug inPlug = iNode.findPlug(propName + cAttrIn);
        if (!inPlug.isNull())
        {
            prop.second.inputType = strToAttr(inPlug.asString());
        }

        MPlug outPlug = iNode.findPlug(propName + cAttrOut);
        if (!outPlug.isNull())
        {
            prop.second.outputType = strToAttr(outPlug.asString());
        }

        MPlug scopePlug = iNode.findPlug(propName + cAttrScope);
        if (!scopePlug.isNull())
        {
            prop.second.scope = strToScope(scopePlug.asString());
        }

        switch (sampType)
        {
            // static
            case 0:
            {
                mWriter.setNonSampledProperty(propName.asChar(), prop);
            }
            break;

            // sampled
            case 1:
            // curve treat like sampled
            case 2:
            {
                PlugAndObj p;
                p.plug = plug;
                p.obj = attr;
                p.inputArb = prop.second.inputType;
                p.outputArb = prop.second.outputType;
                p.scope = prop.second.scope;

                mWriter.setSampledProperty(propName.asChar(), prop);
                mPlugObjVec.push_back(p);
            }
            break;
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
                Tako::PropertyPair visProp;
                int8_t visVal = 0;
                visProp.first = visVal;
                mWriter.setNonSampledProperty("visible",  visProp);
            }
            break;

            // animated visibility 0 case
            case 2:
            {
                Tako::PropertyPair visProp;
                int8_t visVal = 0;
                visProp.first = visVal;

                if (iFrame != FLT_MAX)
                {
                    mWriter.setSampledProperty("visible",  visProp);
                    mAnimVisibility = visPlug.plug;
                }
                // force static case
                else
                {
                    mWriter.setNonSampledProperty("visible",  visProp);
                }
            }
            break;

            // animated visibility 1 case
            case 3:
            {
                // dont add if we are forcing static
                if (iFrame == FLT_MAX)
                {
                    break;
                }

                mAnimVisibility = visPlug.plug;
                Tako::PropertyPair visProp;
                int8_t visVal = -1;
                visProp.first = visVal;
                mWriter.setSampledProperty("visible",  visProp);
            }
            break;

            // dont write any visibility
            default:
            break;
        }
    }
    mWriter.writeProperties(iFrame);
}

AttributesWriter::~AttributesWriter()
{
}

bool AttributesWriter::isAnimated()
{
    return  !mPlugObjVec.empty() || !mAnimVisibility.isNull();
}

void AttributesWriter::write(float iFrame)
{
    std::vector< PlugAndObj >::const_iterator i = mPlugObjVec.begin();
    std::vector< PlugAndObj >::const_iterator end = mPlugObjVec.end();

    for (; i != end; i++)
    {
        Tako::PropertyPair prop;

        prop.second.inputType = i->inputArb;
        prop.second.outputType = i->outputArb;
        prop.second.scope = i->scope;

        MString propName = i->plug.partialName(0, 0, 0, 0, 0, 1);
        bool filledProp = util::attributeToPropertyPair(i->obj, i->plug, prop);

        if (!filledProp)
        {
            MString msg = "WARNING: Couldn't get sampled property ";
            msg += i->plug.partialName(1, 0, 0, 0, 1, 1);
            msg += ", so skipping.";
            MGlobal::displayWarning(msg);
            continue;
        }
        mWriter.updateSample(propName.asChar(), prop);
    }

    if (!mAnimVisibility.isNull())
    {
        int8_t visVal = -1;
        if (!mAnimVisibility.asBool())
        {
            visVal = 0;
        }

        Tako::PropertyPair visProp;
        visProp.first = visVal;
        mWriter.updateSample("visible", visProp);
    }

    mWriter.writeProperties(iFrame);
}

} // End namespace Alembic
