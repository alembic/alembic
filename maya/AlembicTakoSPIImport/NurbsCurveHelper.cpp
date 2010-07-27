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

#include <AlembicTakoSPIImport/NurbsCurveHelper.h>
#include <AlembicTakoSPIImport/Util.h>
#include <AlembicTakoSPIImport/NodeIteratorVisitorHelper.h>

namespace Alembic {

namespace
{
    const bool create2D = false;
    const bool createRational = true;

    MStatus fillTopology(const TakoSPI::NurbsCurveGrp & iCurveGrp,
        MObject & iParent, const MString & iName,
        std::vector<MObject> & ioCurveObjects,
        bool forRead = true)
    {
        MFnNurbsCurve mFn;
        MStatus status;

        unsigned int numCurves = iCurveGrp.numCurve;
        std::vector<float>::const_iterator cvIter = iCurveGrp.cv.begin();
        std::vector<float>::const_iterator knotIter = iCurveGrp.knot.begin();
        std::vector<float>::const_iterator widthIter = iCurveGrp.width.begin();
        float constantWidth = iCurveGrp.constantWidth;

        if (!forRead)
            ioCurveObjects.reserve(numCurves);

        for (unsigned int i = 0; i < numCurves; i++)
        {
            TakoSPI::NurbsCurveMiscInfo miscInfo = iCurveGrp.miscInfo[i];
            unsigned int numCV = miscInfo.numCV;
            unsigned int span = miscInfo.span;
            unsigned int degree = miscInfo.degree;
            MFnNurbsCurve::Form form = MFnNurbsCurve::kInvalid;
            if (miscInfo.form == 0)
                form = MFnNurbsCurve::kOpen;
            else if (miscInfo.form == 1)
                form = MFnNurbsCurve::kClosed;
            else if (miscInfo.form == 2)
                form = MFnNurbsCurve::kPeriodic;

            MPointArray controlVertices;
            controlVertices.setLength(numCV);
            for (unsigned int j = 0; j < numCV; j++)
            {
                double x = *(cvIter++);
                double y = *(cvIter++);
                double z = *(cvIter++);
                double w = *(cvIter++);
                controlVertices.set(j, x, y, z, w);
            }

            unsigned int numKnots = span + 2*degree - 1;
            MDoubleArray knots;
            knots.setLength(numKnots);
            knotIter++;
            for (unsigned int j = 0; j < numKnots; j++)
                knots.set(*(knotIter++), j);
            knotIter++;

            if (forRead)
            {
                mFn.create(controlVertices, knots, degree, form,
                    create2D, createRational, ioCurveObjects[i], &status);

                if (status != MS::kSuccess)
                {
                    MString theError("Creating Nurbs Curve #");
                    theError += i;
                    theError += " failed.\n";
                    theError += getMStatus(status);
                    printError(theError);
                    return status;
                }
            }
            else
            {
                MObject curve = mFn.create(controlVertices, knots, degree,
                    form, create2D, createRational, iParent, &status);

                if (status != MS::kSuccess)
                {
                    MString theError("Creating Nurbs Curve #");
                    theError += i;
                    theError += " failed.\n";
                    theError += getMStatus(status);
                    printError(theError);
                    return status;
                }

                MString name = iName;
                if (numCurves > 1)
                {
                    // name of individual curve shapes might be different from
                    // the original Maya scene after the grouping occurs during
                    // writing
                    MFnTransform fnTrans(iParent);
                    name = fnTrans.name();
                    name += "Shape";
                    name += i+1;
                }
                ioCurveObjects.push_back(curve);
                mFn.setName(name);

                // add attribute for width if any
                if (constantWidth > 0)
                {
                    // add a numerical attribute to this curve
                    MFnNumericAttribute attr;
                    MObject attrObj =
                        attr.create("constantwidth", "constantwidth",
                        MFnNumericData::kFloat, constantWidth, &status);
                    MFnDependencyNode mParentFn(curve, &status);
                    status = mParentFn.addAttribute(attrObj,
                        MFnDependencyNode::kLocalDynamicAttr);
                    if (status != MS::kSuccess)
                    {
                        printError("Add constantwidth attr failed");
                    }
                }
                else if (iCurveGrp.width.size()>0)
                {
                    unsigned length = span;
                    if (miscInfo.form == 0)
                        length += 1;

                    // add a float array attribute
                    MDoubleArray array;
                    array.setLength(length);
                    for (unsigned int j = 0; j < length; j++)
                        array[j] = *(widthIter++);

                    MFnDoubleArrayData mFn;
                    MObject attrObject = mFn.create(array, &status);
                    MFnGenericAttribute attr(attrObject, &status);
                    MObject attrObj = attr.create("width", "width", &status);
                    attr.addDataAccept(MFnData::kDoubleArray);
                    MFnDependencyNode mParentFn(curve, &status);
                    status = mParentFn.addAttribute(attrObj,
                        MFnDependencyNode::kLocalDynamicAttr);

                    MPlug plug(curve, attrObj);
                    status = plug.setValue(attrObject);
                }
            }
        }
        return status;
    }
}

MStatus read(float iFrame, const TakoSPI::NurbsCurveReaderPtr & iNode,
    std::vector<MObject> & ioCurveObjects)
{
    iNode->read(iFrame);

    MObject dummyParent = MObject::kNullObj;
    MString nodeName = iNode->getName().c_str();
    MStatus status =
        fillTopology(iNode->getNurbsCurveGrp(), dummyParent, nodeName,
        ioCurveObjects);

    return status;
}

// mObject: if only one curve in the curve group, returns the curve node;
// otherwise returns the transform node of the curve group
MObject create(float iFrame, const TakoSPI::NurbsCurveReaderPtr & iNode,
    MObject & iParent, std::vector<MObject> & ioCurveObjects,
    std::vector<std::string> & iSampledPropNameList, bool iSwap)
{
    MObject returnObj = iParent;

    iNode->read(iFrame);

    MString nodeName = iNode->getName().c_str();
    unsigned int numCurves = iNode->getNurbsCurveGrp().numCurve;

    MStatus status;
    bool isCurveGrpInScene;

    if (iSwap)
    {
        MFnDagNode fnDag(iParent);
        unsigned int numChild = fnDag.childCount();
        MObject childOfParent = fnDag.child(0);
        MFnDagNode fnChild(childOfParent);
        bool isCurveGrpFromFile = (numCurves > 1);
        // iParent can either be the direct parent of a curveShape,
        // or the parent of the transform node that's the direct parent of curve
        // shapes
        isCurveGrpInScene = childOfParent.hasFn(MFn::kTransform);
        if (!isCurveGrpInScene && numChild > 1)
        {
            bool allCurves = true;
            for (unsigned int i; i < numChild; i++)
            {
                childOfParent = fnDag.child(i);
                if (!childOfParent.hasFn(MFn::kNurbsCurve))
                {
                    allCurves = false;
                    break;
                }
            }
            if (allCurves)
                isCurveGrpInScene = true;
        }

        if (!isCurveGrpInScene)
        {
            status = MGlobal::deleteNode(childOfParent);
            if (isCurveGrpFromFile)
            {
                MObject parentOfParent = fnDag.parent(0, &status);
                // get the parent of the current node
                MGlobal::deleteNode(iParent);
                if (parentOfParent.apiType() == MFn::kWorld)
                    parentOfParent = MObject::kNullObj;
                returnObj = parentOfParent;
            }
        }
        else
        {
            for (int i = numChild-1; i >= 0; i--)
            {
                childOfParent = fnDag.child(i);
                status = MGlobal::deleteNode(childOfParent);
            }
        }
    }

    if (numCurves > 1 && (!iSwap || (iSwap && !isCurveGrpInScene)))
    {
        // One more transform node is created as the direct parent
        // transform of curve shapes
        MFnTransform fnTrans;
        returnObj = fnTrans.create(iParent, &status);
        fnTrans.setName(nodeName);

        // add attribute riCurve so the current scene will still be written
        // out as a curve group
        MFnNumericAttribute attr;
        MObject attrObj = attr.create("riCurves", "riCurves",
                    MFnNumericData::kBoolean, 1, &status);
        status = fnTrans.addAttribute(attrObj,
                    MFnDependencyNode::kLocalDynamicAttr);
        if (status != MS::kSuccess)
        {
            printError("Add riCurves attr failed when creating curve group");
            return returnObj;
        }
    }

    fillTopology(iNode->getNurbsCurveGrp(), returnObj, nodeName,
        ioCurveObjects, false);

    if (numCurves == 1)
        returnObj = ioCurveObjects[0];

    addProperties(iFrame, *iNode, returnObj, iSampledPropNameList);

    return returnObj;
}

} // End namespace Alembic

