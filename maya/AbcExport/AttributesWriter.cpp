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

Alembic::AbcGeom::GeometryScope strToScope(MString iStr)
{
    iStr.toLowerCase();
    if (iStr == "point")
    {
        return Alembic::AbcGeom::kVertexScope;
    }
    else if (iStr == "vertex")
    {
        return Alembic::AbcGeom::kFacevaryingScope;
    }
    else if (iStr == "face")
    {
        return Alembic::AbcGeom::kUniformScope;
    }

    return Alembic::AbcGeom::kConstantScope;
}

// returns true if the string ends with _AttrScope
bool endsWithArbAttr(const std::string & iStr)
{
    size_t len = iStr.size();

    return (len >= 10 && iStr.compare(len - 10, 10, cAttrScope) == 0);
}

}

std::string * AttributesWriter::mFilter = NULL;
std::set<std::string> * AttributesWriter::mAttribs = NULL;

AttributesWriter::AttributesWriter(
    double iFrame,
    Alembic::Abc::OObject & iParent,
    const MFnDagNode & iNode,
    Alembic::AbcCoreAbstract::v1::TimeSamplingType & iTimeType,
    bool iWriteVisibility)
{
    mCurIndex = 0;

    PlugAndObjScalar visPlug;

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

        // if we aren't forcing everything to be static
        if (iFrame != DBL_MAX)
        {
            sampType = util::getSampledType(plug);
        }

        bool filledProp = true; // util::attributeToPropertyPair(attr, plug, prop);

        if (!filledProp)
        {
            // get the full property name for the warning
            MString msg = "WARNING: Couldn't convert ";
            msg += plug.partialName(1, 0, 0, 0, 1, 1);
            msg += " to a property, so skipping.";
            MGlobal::displayWarning(msg);
            continue;
        }

        MPlug scopePlug = iNode.findPlug(propName + cAttrScope);
        Alembic::AbcGeom::GeometryScope scope =
            Alembic::AbcGeom::kConstantScope;

        if (!scopePlug.isNull())
        {
            scope = strToScope(scopePlug.asString());
        }

        switch (sampType)
        {
            // static
            case 0:
            {
                // create a static plug
            }
            break;

            // sampled
            case 1:
            // curve treat like sampled
            case 2:
            {
                // add the plug to mPlugObjScalarVec or mPlugObjArrayVec
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
                int8_t visVal = 0;

                Alembic::Abc::OCharProperty bp(iParent.getProperties(),
                    "visible");
                bp.set(visVal);
            }
            break;

            // animated visibility 0 case
            case 2:
            {
                int8_t visVal = 0;

                if (iFrame != DBL_MAX)
                {
                    Alembic::Abc::OCharProperty bp(iParent.getProperties(),
                        "visible", iTimeType);
                    bp.set(visVal,
                        Alembic::Abc::OSampleSelector(mCurIndex, iFrame));
                    visPlug.prop = bp;
                    mAnimVisibility = visPlug;
                }
                // force static case
                else
                {
                    Alembic::Abc::OCharProperty bp(iParent.getProperties(),
                        "visible");
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
                Alembic::Abc::OCharProperty bp(iParent.getProperties(),
                    "visible", iTimeType);
                bp.set(visVal,
                    Alembic::Abc::OSampleSelector(mCurIndex, iFrame));
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
    return  !mPlugObjScalarVec.empty() || !mPlugObjArrayVec.empty() ||
        !mAnimVisibility.plug.isNull();
}

void AttributesWriter::write(double iFrame)
{
    mCurIndex ++;

    std::vector< PlugAndObjScalar >::const_iterator i =
        mPlugObjScalarVec.begin();
    std::vector< PlugAndObjScalar >::const_iterator iend =
        mPlugObjScalarVec.end();

    for (; i != iend; i++)
    {
        MString propName = i->plug.partialName(0, 0, 0, 0, 0, 1);
        bool filledProp = true; //util::attributeToPropertyPair(i->obj, i->plug, prop);

        if (!filledProp)
        {
            MString msg = "WARNING: Couldn't get sampled property ";
            msg += i->plug.partialName(1, 0, 0, 0, 1, 1);
            msg += ", so skipping.";
            MGlobal::displayWarning(msg);
            continue;
        }
    }

    std::vector< PlugAndObjArray >::const_iterator j =
        mPlugObjArrayVec.begin();
    std::vector< PlugAndObjArray >::const_iterator jend =
        mPlugObjArrayVec.end();

    for (; j != jend; j++)
    {
        MString propName = j->plug.partialName(0, 0, 0, 0, 0, 1);
        bool filledProp = true; //util::attributeToPropertyPair(j->obj, j->plug, prop);

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

        mAnimVisibility.prop.set(&visVal,
            Alembic::Abc::OSampleSelector(mCurIndex, iFrame));
    }

}


