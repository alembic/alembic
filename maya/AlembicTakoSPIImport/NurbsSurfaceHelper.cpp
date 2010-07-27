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

#include <AlembicTakoSPIImport/NurbsSurfaceHelper.h>
#include <AlembicTakoSPIImport/Util.h>
#include <AlembicTakoSPIImport/NodeIteratorVisitorHelper.h>

namespace Alembic {

namespace
{
    bool create2D = true;
    bool createRational = true;

    unsigned int curveCnt = 0;
    float offsetV;

    MString form2MString(MFnNurbsSurface::Form val)
    {
        std::string tmp =
            boost::lexical_cast<std::string, MFnNurbsSurface::Form>(val);
        return tmp.c_str();
    }

MStatus getReverseNurbsCurveGrp(TakoSPI::NurbsCurveGrp curveGrp,
    MObjectArray & boundary, MObjectArray & deleteAfterTrim)
{
    MStatus status = MS::kSuccess;

    unsigned int numCurve = curveGrp.numCurve;
    std::vector<float>::iterator cvIter = curveGrp.cv.begin();
    std::vector<float>::iterator knotIter = curveGrp.knot.begin();

    boundary.setLength(numCurve);
    for (unsigned int i = 0; i < numCurve; i++)
    {
        TakoSPI::NurbsCurveMiscInfo miscInfo = curveGrp.miscInfo[i];
        unsigned int numCV      = miscInfo.numCV;
        unsigned int span       = miscInfo.span;
        unsigned char degree    = miscInfo.degree;
        unsigned int numKnots   = span+2*degree-1;

        MFnNurbsCurve::Form form = MFnNurbsCurve::kInvalid;
        if (miscInfo.form == 0)
            form = MFnNurbsCurve::kOpen;
        else if (miscInfo.form == 1)
            form = MFnNurbsCurve::kClosed;
        else if (miscInfo.form == 2)
            form = MFnNurbsCurve::kPeriodic;

        MPointArray controlVertices;
        controlVertices.setLength(numCV);
        for (unsigned int j = 0 ; j < numCV; j++)
        {
            double x = *(cvIter++);
            double y = *(cvIter++);
            double z = *(cvIter++);
            double w = *(cvIter++);
            controlVertices.set(j, x, offsetV-y, z, w);
        }

        MDoubleArray knots;
        knots.setLength(numKnots);
        knotIter++;
        for (unsigned int j = 0; j < numKnots; j++)
            knots.set(*(knotIter++), j);
        knotIter++;

        MFnNurbsCurve fnCurve;
        // when a 2D curve is created without a parent, the function returns
        // the transform node of the curve created.
        // The transform node as well as the curve node need to be deleted once
        // the trim is done, because after all this is not the equivalent of
        // "curveOnSurface" command
        MObject curve2D = fnCurve.create(controlVertices, knots, degree, form,
                    create2D, createRational, MObject::kNullObj, &status);
        if (status == MS::kSuccess)
        {
            MFnTransform trans(curve2D, &status);
            if (status == MS::kSuccess)
            {
                /*
                status = boundary.set(trans.child(0), i);
                */
                status = boundary.set(trans.child(0), numCurve-1-i);

                // we are doing this in MEL because there was a bug where if
                // the API were used, it could crash Maya.  This bug has been
                // fixed so we should port it back to the API
                MString cmd("reverseCurve -constructionHistory 0 ");
                cmd += MString("-replaceOriginal 1 \"");
                MString name = "curve2D_";
                name += ++curveCnt;
                trans.setName(name);
                cmd += name;
                cmd += "\";";
                MGlobal::executeCommand(cmd, 1);

                deleteAfterTrim.append(curve2D);
            }
        }
        else
        {
            printError("Failed to create 2D curve");
            return status;
        }
    }

    return status;
}  // getReverseNurbsCurveGrp
}



MStatus read(float iFrame, const TakoSPI::NurbsSurfaceReaderPtr& iNode,
    MObject& iObject)
{
    MStatus status;

    iNode->readProperties(iFrame);
    iNode->read(iFrame);

    MString surfaceName(iNode->getName().c_str());

    const TakoSPI::NurbsSurfaceMiscInfo & miscInfo = iNode->getMiscInfo();
    unsigned int  spanU    = miscInfo.spanU;
    unsigned int  spanV    = miscInfo.spanV;
    unsigned char degreeU  = miscInfo.degreeU;
    unsigned char degreeV  = miscInfo.degreeV;
    unsigned int  numCVInU = spanU + degreeU;
    unsigned int  numCVInV = spanV + degreeV;

    offsetV = miscInfo.minV + miscInfo.maxV;

    MFnNurbsSurface::Form formU = MFnNurbsSurface::kInvalid;
    if (miscInfo.formU == 0)
        formU = MFnNurbsSurface::kOpen;
    else if (miscInfo.formU == 1)
        formU = MFnNurbsSurface::kClosed;
    else if (miscInfo.formU == 2)
        formU = MFnNurbsSurface::kPeriodic;

    MFnNurbsSurface::Form formV = MFnNurbsSurface::kInvalid;
    if (miscInfo.formV == 0)
        formV = MFnNurbsSurface::kOpen;
    else if (miscInfo.formV == 1)
        formV = MFnNurbsSurface::kClosed;
    else if (miscInfo.formV == 2)
        formV = MFnNurbsSurface::kPeriodic;

    // cv points
    unsigned int numCV = numCVInU * numCVInV;
    float cv[numCVInV][numCVInU][4];
    unsigned int index = 0;
    const std::vector< float > & nodeCV = iNode->getCV();
    for (int n = numCVInV-1; n >= 0; n--)
    {
        for (unsigned int m = 0; m < numCVInU; m++, index++)
        {
            cv[n][m][0] = nodeCV[4*index+0];
            cv[n][m][1] = nodeCV[4*index+1];
            cv[n][m][2] = nodeCV[4*index+2];
            cv[n][m][3] = nodeCV[4*index+3];
        }
    }

    MPointArray controlVertices;
    controlVertices.setLength(numCV);
    for (index = 0; index < numCV; index++)
    {
        unsigned int m = index/numCVInV;
        unsigned int n = index%numCVInV;
        controlVertices.set(MPoint(cv[n][m]), index);
    }

    // knot vectors
    const std::vector<double> & iKnotsInU = iNode->getKnotsInU();
    unsigned int numKnotsInU = iKnotsInU.size()-2;
    MDoubleArray uKnotSequences;
    uKnotSequences.setLength(numKnotsInU);
    for (unsigned int j = 0; j < numKnotsInU; j++)
        uKnotSequences.set(iKnotsInU[j+1], j);

    const std::vector<double> & iKnotsInV = iNode->getKnotsInV();
    unsigned int numKnotsInV = iKnotsInV.size()-2;
    MDoubleArray vKnotSequences;
    vKnotSequences.setLength(numKnotsInV);
    for (unsigned int j = 0; j < numKnotsInV; j++)
        vKnotSequences.set(iKnotsInV[j+1], j);

    // Node creation try the API first
    MFnNurbsSurface mFn;
    mFn.create(controlVertices, uKnotSequences, vKnotSequences,
        degreeU, degreeV, formU, formV, createRational, iObject, &status);

    if (status == MS::kSuccess)
    {
        mFn.setName(surfaceName);
    }
    else
    {
        // that failed, probably because of knots shenanigans, try making
        // it open, open instead
        mFn.create(controlVertices, uKnotSequences, vKnotSequences,
            degreeU, degreeV, MFnNurbsSurface::kOpen, MFnNurbsSurface::kOpen,
            createRational, iObject, &status);

        if (status == MS::kSuccess)
        {
            mFn.setName(surfaceName);
            MString theWarning("Creating Nurbs Surface ");
             theWarning += surfaceName;
            theWarning += MString(" failed, forcing it to be open in U and V");
            printWarning(theWarning);
        }
        else
        {
            MString theError("Creating Nurbs Surface ");
            theError += surfaceName;
            theError += MString(" when both forms are set to be ");
            theError += MString("MFnNurbsSurface::kOpen failed");
            theError += getMStatus(status);
            printError(theError);

            // The API failed lets try try again with the MEL script
            MFnTransform fnParent(iObject, &status);
            MString parentName = fnParent.partialPathName(&status);
            MObject grandparent = fnParent.parent(0, &status);
            MString cmd;
            cmd += "surface -du ";
            cmd += degreeU;
            cmd += " -dv ";
            cmd += degreeV;
            cmd += " -formU ";
            if (miscInfo.formU == 0)
                cmd += "\"open\"";
            else if (miscInfo.formU == 1)
                cmd += "\"closed\"";
            else if (miscInfo.formU == 2)
                cmd += "\"periodic\"";

            cmd += " -formV ";
            if (miscInfo.formV == 0)
                cmd += "\"open\"";
            else if (miscInfo.formV == 1)
                cmd += "\"closed\"";
            else if (miscInfo.formV == 2)
                cmd += "\"periodic\"";
            cmd += "\n";

            for (unsigned int k = 0; k < numCV; k++)
            {
                cmd += " -pw ";
                cmd += controlVertices[k].x; cmd += " ";
                cmd += controlVertices[k].y; cmd += " ";
                cmd += controlVertices[k].z; cmd += " ";
                cmd += controlVertices[k].w; cmd += "\n";
            }

            for (unsigned int k = 0; k < numKnotsInU; k++)
            {
                cmd += " -ku ";
                cmd += uKnotSequences[k];
            }
            cmd += "\n";

            for (unsigned int k = 0; k < numKnotsInV; k++)
            {
                cmd += " -kv ";
                cmd += vKnotSequences[k];
            }
            cmd += " -n ";
            cmd += parentName;
            cmd += ";";
            MCommandResult result;
            status = MGlobal::executeCommand(cmd, result, true);
            if (status == MS::kFailure)
            {
                MString theError("Creating Nurbs Surface ");
                theError += surfaceName;
                theError += MString(" with MEL script still fails");
                printError(theError);
            }
            MString tempName;
            status = result.getResult(tempName);

            // Since the nurbs surface is created with MEL, so might as well
            // use MEL to rename instead of doing this the API way
            cmd = "rename ";
            cmd += tempName;
            cmd += " ";
            cmd += surfaceName;
            cmd += ";\n";
            cmd += "parent ";
            cmd += parentName;
            cmd += " ";
            fnParent.setObject(grandparent);
            cmd += fnParent.partialPathName();
            cmd += ";";
            status = MGlobal::executeCommand(cmd, result, true);
        }  // else
    }

    // Trim curve part
    const std::vector<TakoSPI::NurbsCurveGrp> & tc = iNode->getTrimCurve();
    unsigned int numRegions = tc.size();
    MObjectArray deleteAfterTrim;
    if (numRegions > 0)
    {
        MTrimBoundaryArray trimBoundaryArray;
        trimBoundaryArray.reserve(numRegions+1);
        MObjectArray emptyBoundary;
        status = trimBoundaryArray.append(emptyBoundary);

        for (unsigned int i = 0; i < numRegions; i++)
        {
            TakoSPI::NurbsCurveGrp curveGrp = tc[i];
            MObjectArray boundary;
            getReverseNurbsCurveGrp(curveGrp, boundary, deleteAfterTrim);
            trimBoundaryArray.append(boundary);
        }

        status = mFn.trimWithBoundaries(trimBoundaryArray);
        if (status == MS::kSuccess)
        {
            unsigned int length = deleteAfterTrim.length();
            for (unsigned int l = 0; l < length; l++)
                MGlobal::deleteNode(deleteAfterTrim[l]);
        }
    }

    return status;
}

MStatus create(float iFrame, const TakoSPI::NurbsSurfaceReaderPtr & iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList, bool iSwap)
{
    MStatus status;

    iNode->read(iFrame);

    MFnNurbsSurface mFn;
    MObject swapObj = iObject;

    const TakoSPI::NurbsSurfaceMiscInfo & miscInfo = iNode->getMiscInfo();
    unsigned int spanU  = miscInfo.spanU;
    unsigned int spanV  = miscInfo.spanV;
    unsigned char degreeU = miscInfo.degreeU;
    unsigned char degreeV = miscInfo.degreeV;
    const unsigned int numCVInU = spanU + degreeU;
    const unsigned int numCVInV = spanV + degreeV;

    // cv points
    unsigned int numCV = numCVInU*numCVInV;
    float cv[numCVInV][numCVInU][4];
    unsigned int index = 0;
    const std::vector<float> & nodeCV = iNode->getCV();
    for (int n = numCVInV-1; n >= 0; n--)
    {
        for (unsigned int m = 0; m < numCVInU; m++, index++)
        {
            cv[n][m][0] = nodeCV[4*index+0];
            cv[n][m][1] = nodeCV[4*index+1];
            cv[n][m][2] = nodeCV[4*index+2];
            cv[n][m][3] = nodeCV[4*index+3];
        }
    }

    MPointArray controlVertices;
    controlVertices.setLength(numCV);
    for (index = 0; index < numCV; index++)
    {
        unsigned int m = index/numCVInV;
        unsigned int n = index%numCVInV;
        controlVertices.set(MPoint(cv[n][m]), index);
    }

    if (iSwap)
    {
        mFn.setObject(iObject);

        MPlug dstPlug = mFn.findPlug("create");
        disconnectAllPlugsTo(dstPlug);

        // disconnect connections to animated props
        iNode->readProperties(iFrame);
        TakoSPI::PropertyMap::const_iterator propIter =
            iNode->beginSampledProperties();
        while ( propIter != iNode->endSampledProperties() )
        {
            std::string propName = propIter->first;
            // SPT_xxColor is special
            if ( propName.find("SPT_") != std::string::npos
                && propName.find("Color") != std::string::npos )
            {
                std::string colorR = propName+std::string("R");
                dstPlug = mFn.findPlug(colorR.c_str());
                disconnectAllPlugsTo(dstPlug);
                std::string colorG = propName+std::string("G");
                dstPlug = mFn.findPlug(colorG.c_str());
                disconnectAllPlugsTo(dstPlug);
                std::string colorB = propName+std::string("B");
                dstPlug = mFn.findPlug(colorB.c_str());
                disconnectAllPlugsTo(dstPlug);
            }
            else
            {
                dstPlug = mFn.findPlug(propIter->first.c_str());
                disconnectAllPlugsTo(dstPlug);
            }
            propIter++;
        }

        mFn.setCVs(controlVertices, MSpace::kObject);
        addProperties(iFrame, *iNode, iObject, iSampledPropNameList);

        return status;
    }

    offsetV = miscInfo.minV + miscInfo.maxV;

    MFnNurbsSurface::Form formU = MFnNurbsSurface::kInvalid;
    if (miscInfo.formU == 0)
        formU = MFnNurbsSurface::kOpen;
    else if (miscInfo.formU == 1)
        formU = MFnNurbsSurface::kClosed;
    else if (miscInfo.formU == 2)
        formU = MFnNurbsSurface::kPeriodic;

    MFnNurbsSurface::Form formV = MFnNurbsSurface::kInvalid;
    if (miscInfo.formV == 0)
        formV = MFnNurbsSurface::kOpen;
    else if (miscInfo.formV == 1)
        formV = MFnNurbsSurface::kClosed;
    else if (miscInfo.formV == 2)
        formV = MFnNurbsSurface::kPeriodic;

    // knot vectors
    const std::vector<double> & iKnotsInU = iNode->getKnotsInU();
    unsigned int numKnotsInU = iKnotsInU.size()-2;
    MDoubleArray uKnotSequences;
    uKnotSequences.setLength(numKnotsInU);
    for (unsigned int j = 0; j < numKnotsInU; j++)
        uKnotSequences.set(iKnotsInU[j+1], j);

    const std::vector<double> & iKnotsInV = iNode->getKnotsInV();
    unsigned int numKnotsInV = iKnotsInV.size()-2;
    MDoubleArray vKnotSequences;
    vKnotSequences.setLength(numKnotsInV);
    for (unsigned int j = 0; j < numKnotsInV; j++)
        vKnotSequences.set(iKnotsInV[j+1], j);

    iObject = mFn.create(controlVertices, uKnotSequences, vKnotSequences,
        degreeU, degreeV, formU, formV, createRational, iParent, &status);

    if (status == MS::kSuccess)
    {
        MString name(iNode->getName().c_str());
        if (iSwap)
        {
            MFnNurbsSurface mFnNurbs(iObject);
            name = mFnNurbs.name();

            // add all the children of oldTransformObject to mObject
            // then delete oldTransformObject
            status = replaceDagObject(swapObj, iObject, name);
            MDagModifier modifier;
            status = modifier.deleteNode(iObject);
            status = modifier.doIt();
        }

        mFn.setName(name);
        setInitialShadingGroup(mFn.partialPathName());

        if (formU != mFn.formInU())
        {
            MString theWarning("formU ");
            theWarning += form2MString(formU);
            theWarning += MString(" != ");
            theWarning += mFn.name();
            theWarning += MString(".formInU() ");
            theWarning += form2MString(mFn.formInU());
            printWarning(theWarning);
        }

        if (formV != mFn.formInV())
        {
            MString theWarning("formV ");
            theWarning += form2MString(formV);
            theWarning += MString(" != ");
            theWarning += mFn.name();
            theWarning += MString(".formInV() ");
            theWarning += form2MString(mFn.formInV());
            printWarning(theWarning);
        }
    }
    else
    {
        // the API failed try again with an open/open surface
        // it most likely failed because it didn't like the knot values
        iObject = mFn.create(controlVertices, uKnotSequences, vKnotSequences,
            degreeU, degreeV, MFnNurbsSurface::kOpen, MFnNurbsSurface::kOpen,
            createRational, iParent, &status);

        if (status == MS::kSuccess)
        {
            mFn.setName(MString(iNode->getName().c_str()));
            MString theWarning("Creating Nurbs Surface ");
            theWarning += MString(iNode->getName().c_str());
            theWarning += MString(" failed, forcing it to be open in U and V");
            printWarning(theWarning);

            setInitialShadingGroup(mFn.partialPathName());
        }
        else
        {
            MString theError("Creating Nurbs Surface ");
            theError += MString(iNode->getName().c_str());
            theError += MString(
            " failed again even when both forms are MFnNurbsSurface::kOpen\n");
            theError += getMStatus(status);
            printError(theError);

            // Some nurbs surfaces cannot be created through the API, but can be
            // created through python or MEL with the same data.
            // But pit2maya doesn't have a problem with the loading
            // Hopefully this is just a temporary fix

            // try again with MEL script
            MFnTransform fnParent(iParent, &status);
            MString parentName = fnParent.partialPathName(&status);
            MObject grandparent = fnParent.parent(0, &status);
            MString cmd;
            cmd += "delete ";
            cmd += parentName;
            cmd += ";\n";
            cmd += "surface -du "; cmd += degreeU;
            cmd += " -dv "; cmd += degreeV;

            cmd += " -formU ";
            if (miscInfo.formU == 0)
                cmd += "\"open\"";
            else if (miscInfo.formU == 1)
                cmd += "\"closed\"";
            else if (miscInfo.formU == 2)
                cmd += "\"periodic\"";

            cmd += " -formV ";
            if (miscInfo.formV == 0)
                cmd += "\"open\"";
            else if (miscInfo.formV == 1)
                cmd += "\"closed\"";
            else if (miscInfo.formV == 2)
                cmd += "\"periodic\"";
            cmd += "\n";

            for (unsigned int k = 0; k < numCV; k++)
            {
                cmd += " -pw ";
                cmd += controlVertices[k].x;
                cmd += " ";
                cmd += controlVertices[k].y;
                cmd += " ";
                cmd += controlVertices[k].z;
                cmd += " ";
                cmd += controlVertices[k].w;
                cmd += "\n";
            }

            for (unsigned int k = 0; k < numKnotsInU; k++)
            {
                cmd += " -ku ";
                cmd += uKnotSequences[k];
            }
            cmd += "\n";

            for (unsigned int k = 0; k < numKnotsInV; k++)
            {
                cmd += " -kv ";
                cmd += vKnotSequences[k];
            }
            cmd += " -n ";
            cmd += parentName;
            cmd += ";";
            MCommandResult result;
            status = MGlobal::executeCommand(cmd, result, true);
            if (status == MS::kFailure)
            {
                MString theError("Creating Nurbs Surface ");
                theError += MString(iNode->getName().c_str());
                theError += MString(" again with MEL script still fails");
                printError(theError);
            }

            MString surfaceName;
            status = result.getResult(surfaceName);

            cmd = "rename ";
            cmd += surfaceName;
            cmd += " ";
            cmd += iNode->getName().c_str();
            cmd += ";\n";
            cmd += "parent ";
            cmd += parentName;
            cmd += " ";
            fnParent.setObject(grandparent);
            cmd += fnParent.partialPathName();
            cmd += ";";
            status = MGlobal::executeCommand(cmd, result, true);
        }  // else, the MEL creation way
    }  // else, the open/open API way

    // Trim curve part
    const std::vector<TakoSPI::NurbsCurveGrp> & tc = iNode->getTrimCurve();
    unsigned int numRegions = tc.size();
    MObjectArray deleteAfterTrim;
    if (numRegions > 0)
    {
        MTrimBoundaryArray trimBoundaryArray;
        trimBoundaryArray.reserve(numRegions+1);
        MObjectArray emptyBoundary;
        trimBoundaryArray.append(emptyBoundary);

        for (unsigned int i = 0; i < numRegions; i++)
        {
            TakoSPI::NurbsCurveGrp curveGrp = tc[i];
            MObjectArray boundary;
            getReverseNurbsCurveGrp(curveGrp, boundary, deleteAfterTrim);
            trimBoundaryArray.append(boundary);
        }
        status = mFn.trimWithBoundaries(trimBoundaryArray);
        if (status == MS::kSuccess)
        {
            unsigned int length = deleteAfterTrim.length();
            for (unsigned int l = 0; l < length; l++)
            {
                MGlobal::deleteNode(deleteAfterTrim[l]);
            }
        }
        else
        {
            printError("Surface trimmed failed");
        }
    }

    addProperties(iFrame, *iNode, iObject, iSampledPropNameList);

    return status;
}

} // End namespace Alembic
