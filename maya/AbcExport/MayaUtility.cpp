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

#include "MayaUtility.h"

// return seconds per frame
double util::spf()
{
    static const MTime sec(1.0, MTime::kSeconds);
    return 1.0 / sec.as(MTime::uiUnit());
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

    // it's possible that only some element of an array plug or
    // some component of a compound plus is connected
    if (conns.length() == 0)
    {
        if (iPlug.isArray())
        {
            unsigned int numConnectedElements = iPlug.numConnectedElements();
            for (unsigned int e = 0; e < numConnectedElements; e++)
            {
                int retVal = getSampledType(iPlug.connectionByPhysicalIndex(e));
                if (retVal > 0)
                    return retVal;
            }
        }
        else if (iPlug.isCompound() && iPlug.numConnectedChildren() > 0)
        {
            unsigned int numChildren = iPlug.numChildren();
            for (unsigned int c = 0; c < numChildren; c++)
            {
                int retVal = getSampledType(iPlug.child(c));
                if (retVal > 0)
                    return retVal;
            }
        }
        return 0;
    }

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
    }

    return 1;
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
                node.hasFn(MFn::kCluster) ||
                node.hasFn(MFn::kFluid))
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
