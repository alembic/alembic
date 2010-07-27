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

#include <AlembicTakoSPIExport/MayaUtility.h>

namespace Alembic {

namespace {

bool MFnNumericDataToPropertyPair(MFnNumericData::Type iType,
    const MPlug& iPlug, TakoSPI::PropertyPair& oProp)
{
    size_t numElements =  iPlug.numElements();

    switch (iType)
    {
        case MFnNumericData::kBoolean:
        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        {
            if (numElements == 0)
            {
                int8_t val = static_cast<int8_t>(iPlug.asShort());
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

    return false;
}

bool MFnAttrToPropertyPair(const MObject& iAttr, const MPlug& iPlug,
    TakoSPI::PropertyPair& oProp)
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
                oProp.second.inputType = TakoSPI::ARBATTR_POINT4;
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
                oProp.second.inputType = TakoSPI::ARBATTR_VECTOR3;
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


bool util::isAncestorDescendentRelationship(const MDagPath & path1,
    const MDagPath & path2)
{
    unsigned int length1 = path1.length();
    unsigned int length2 = path2.length();
    unsigned int diff;

    if (length1 == length2 && !(path1 == path2))
        return false;

    MDagPath ancestor, descendent;
    if (length1 > length2)
    {
        ancestor = path2;
        descendent = path1;
        diff = length1 - length2;
    }
    else
    {
        ancestor = path1;
        descendent = path2;
        diff = length2 - length1;
    }

    descendent.pop(diff);

    bool ret = (ancestor == descendent);

    if (ret)
    {
        MString err = path1.fullPathName() + " and ";
        err += path2.fullPathName() + " have parenting relationships";
        MGlobal::displayError(err);
    }
    return ret;
}


// returns 0 if static, 1 if sampled, and 2 if a curve
int util::getSampledType(const MPlug& iPlug)
{
    MPlugArray conns;

    iPlug.connectedTo(conns, true, false);

    // this plug is not a destination, so it is static
    if (conns.length() == 0)
        return 0;

    MObject ob;
    MFnDependencyNode nodeFn;

    for (unsigned i = 0; i < conns.length(); i++)
    {
        ob = conns[i].node();
        MFn::Type type = ob.apiType();

        switch (type)
        {
            case MFn::kAnimCurveTimeToAngular:
            case MFn::kAnimCurveTimeToDistance:
            case MFn::kAnimCurveTimeToTime:
            case MFn::kAnimCurveTimeToUnitless:
            {
                nodeFn.setObject(ob);
                MPlug incoming = nodeFn.findPlug("i", true);

                // sampled
                if (incoming.isConnected())
                    return 1;

                // curve
                else
                    return 2;
            }
            break;

            case MFn::kMute:
            {
                nodeFn.setObject(ob);
                MPlug mutePlug = nodeFn.findPlug("mute", true);

                // static
                if (mutePlug.asBool())
                    return 0;
                // curve
                else
                   return 2;
            }
            break;

            default:
            break;
        }

        return 1;
    }
    return 1;
}

bool util::attributeToPropertyPair(
    const MObject& iAttr,
    const MPlug& iPlug,
    TakoSPI::PropertyPair& oProp)
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

bool util::getRotOrder(MTransformationMatrix::RotationOrder iOrder,
    unsigned int & oXAxis, unsigned int & oYAxis, unsigned int & oZAxis)
{
    switch (iOrder)
    {
        case MTransformationMatrix::kXYZ:
        {
            oXAxis = 0;
            oYAxis = 1;
            oZAxis = 2;
        }
        break;

        case MTransformationMatrix::kYZX:
        {
            oXAxis = 1;
            oYAxis = 2;
            oZAxis = 0;
        }
        break;

        case MTransformationMatrix::kZXY:
        {
            oXAxis = 2;
            oYAxis = 0;
            oZAxis = 1;
        }
        break;

        case MTransformationMatrix::kXZY:
        {
            oXAxis = 0;
            oYAxis = 2;
            oZAxis = 1;
        }
        break;

        case MTransformationMatrix::kYXZ:
        {
            oXAxis = 1;
            oYAxis = 0;
            oZAxis = 2;
        }
        break;

        case MTransformationMatrix::kZYX:
        {
            oXAxis = 2;
            oYAxis = 1;
            oZAxis = 0;
        }
        break;

        default:
        {
            return false;
        }
    }
    return true;
}

// 0 dont write, 1 write static 0, 2 write anim 0, 3 write anim -1
int util::getVisibilityType(const MPlug & iPlug)
{
    int type = getSampledType(iPlug);

    // static case
    if (type == 0)
    {
        // dont write anything
        if (iPlug.asBool())
            return 0;

        // write static 0
        return 1;
    }
    else
    {
        // anim write -1
        if (iPlug.asBool())
            return 3;

        // write anim 0
        return 2;
    }
}

// does this cover all cases?
bool util::isAnimated(MObject & object, bool checkParent)
{
    MStatus stat;
    MItDependencyGraph iter(object, MFn::kInvalid,
        MItDependencyGraph::kUpstream,
        MItDependencyGraph::kDepthFirst,
        MItDependencyGraph::kPlugLevel,
        &stat);

    if (stat!= MS::kSuccess)
    {
        MGlobal::displayError("Unable to create DG iterator ");
    }

    for (; !iter.isDone(); iter.next())
    {
        MObject node = iter.thisNode();
        MPlug plug = iter.thisPlug();

        if (node.hasFn(MFn::kExpression))
        {
            MFnExpression fn(node, &stat);
            if (stat == MS::kSuccess && fn.isAnimated())
            {
                return true;
            }
        }
        if (MAnimUtil::isAnimated(node, checkParent))
        {
            return true;
        }
        if (node.hasFn( MFn::kConstraint ) ||
                node.hasFn(MFn::kPointConstraint) ||
                node.hasFn(MFn::kAimConstraint) ||
                node.hasFn(MFn::kOrientConstraint) ||
                node.hasFn(MFn::kScaleConstraint) ||
                node.hasFn(MFn::kGeometryConstraint) ||
                node.hasFn(MFn::kNormalConstraint) ||
                node.hasFn(MFn::kTangentConstraint) ||
                node.hasFn(MFn::kParentConstraint) ||
                node.hasFn(MFn::kPoleVectorConstraint) ||
                node.hasFn(MFn::kParentConstraint) ||
                node.hasFn(MFn::kTime) ||
                node.hasFn(MFn::kJoint) ||
                node.hasFn(MFn::kGeometryFilt) ||
                node.hasFn(MFn::kTweak) ||
                node.hasFn(MFn::kPolyTweak) ||
                node.hasFn(MFn::kSubdTweak) ||
                node.hasFn(MFn::kCluster))
        {
            return true;
        }
    }

    return false;
}

bool util::isIntermediate(const MObject & object)
{
    MStatus stat;
    MFnDagNode mFn(object);

    MPlug plug = mFn.findPlug("intermediateObject", false, &stat);
    if (stat == MS::kSuccess && plug.asBool())
        return true;
    else
        return false;
}

} // End namespace Alembic
