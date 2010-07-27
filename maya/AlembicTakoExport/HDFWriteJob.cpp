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

#include <AlembicTakoExport/HDFWriteJob.h>
#include <AlembicTakoExport/MayaUtility.h>

namespace Alembic {

namespace
{
    bool hasDuplicates(const std::vector< MDagPath > & mDagPath)
    {
        std::set<std::string> roots;
        unsigned int cnt = mDagPath.size();
        for (unsigned int i = 0; i < cnt; i++)
        {
            MFnTransform mFn(mDagPath[i].node());
            if (roots.count(mFn.name().asChar()) > 0)
                return true;
            else
                roots.insert(mFn.name().asChar());
        }
        return false;
    }

    class CallWriteVisitor : public boost::static_visitor<>
    {
        public:
            explicit CallWriteVisitor(float iFrame): mFrame(iFrame) {}

            void operator()(MayaNurbsSurfaceWriterPtr & iNode)
            {
                iNode->write(mFrame);
            }

            void operator()(MayaNurbsCurveWriterPtr & iNode)
            {
                iNode->write(mFrame);
            }

            void operator()(MayaPointPrimitiveWriterPtr & iNode)
            {
                iNode->write(mFrame);
            }

            void operator()(MayaCameraWriterPtr & iNode)
            {
                iNode->write(mFrame);
            }

            void operator()(MayaLightWriterPtr & iNode)
            {
                iNode->write(mFrame);
            }

            void operator()(MayaLocatorWriterPtr & iNode)
            {
                iNode->write(mFrame);
            }

            void operator()(MayaMeshWriterPtr & iNode)
            {
                iNode->write(mFrame);
            }

        private:
            float mFrame;
    };

    void addToString(std::string & str,
        const std::string & name, unsigned int value)
    {
        if (value > 0)
        {
            std::stringstream ss;
            ss << value;
            str += name + std::string(" ") + ss.str() + std::string(" ");
        }
    }

    // increment each CV counter according to the node type in mShapeList
    class IncrementCVCountsVisitor : public boost::static_visitor<>
    {
        public:
            IncrementCVCountsVisitor()
            {
                mCVsArray[0] = 0;   // increment onto NurbsAnimCVs
                mCVsArray[1] = 0;   // increment onto CurveAnimCVs
                mCVsArray[2] = 0;   // increment onto PointAnimCVs
                mCVsArray[3] = 0;   // increment onto SubDAnimCVs
                mCVsArray[4] = 0;   // increment onto PolyAnimCVs
            }

            void operator()(MayaNurbsSurfaceWriterPtr & iNode)
            {
                mCVsArray[0] += iNode->getNumCVs();
            }

            void operator()(MayaNurbsCurveWriterPtr & iNode)
            {
                mCVsArray[1] += iNode->getNumCVs();
            }

            void operator()(MayaPointPrimitiveWriterPtr & iNode)
            {
                mCVsArray[2] += iNode->getNumCVs();
            }

            void operator()(MayaCameraWriterPtr & iNode) {}

            void operator()(MayaLightWriterPtr & iNode) {}

            void operator()(MayaLocatorWriterPtr & iNode) {}

            void operator()(MayaMeshWriterPtr & iNode)
            {
                if (iNode->isSubD())
                    mCVsArray[3] += iNode->getNumCVs();
                else
                    mCVsArray[4] += iNode->getNumCVs();
            }

        unsigned int mCVsArray[5];
    };

    void getActiveSelectionListWithAllAncestors(MSelectionList & iList)
    {
        MStatus status;
        MSelectionList oldList;
        MGlobal::getActiveSelectionList(oldList);
        unsigned int selectionSize = oldList.length();

        iList = oldList;

        for (unsigned int index = 0; index < selectionSize; index ++)
        {
            MDagPath dagPath;
            status = oldList.getDagPath(index, dagPath);
            if (status == MS::kSuccess)
            {
                unsigned int length = dagPath.length();
                while (--length)
                {
                    dagPath.pop();
                    iList.add(dagPath, MObject::kNullObj, true);
                }
            }
        }
    }

    void processCallback(std::string iCallback, bool isMelCallback,
        float iFrame, const MBoundingBox & iBbox)
    {
        if (iCallback.empty())
            return;

        size_t pos = iCallback.find("#FRAME#");
        if ( pos != std::string::npos )
        {
            std::stringstream sstrm;
            sstrm.precision(std::numeric_limits<float>::digits10);
            sstrm << iFrame;
            std::string str = sstrm.str();

            iCallback.replace(pos, 7, str);
        }

        pos = iCallback.find("#BOUNDS#");
        if ( pos != std::string::npos )
        {
            std::stringstream sstrm;
            sstrm.precision(std::numeric_limits<float>::digits10);
            sstrm << " " << iBbox.min().x << " " << iBbox.min().y << " " <<
                iBbox.min().z << " " << iBbox.max().x << " " <<
                iBbox.max().y << " " <<iBbox.max().z;

            std::string str = sstrm.str();

            iCallback.replace(pos, 8, str);
        }

        pos = iCallback.find("#BOUNDSARRAY#");
        if ( pos != std::string::npos )
        {
            std::stringstream sstrm;
            sstrm.precision(std::numeric_limits<float>::digits10);

            if (isMelCallback)
            {
                sstrm << " {";
            }
            else
            {
                sstrm << " [";
            }

            sstrm << iBbox.min().x << "," << iBbox.min().y << "," <<
                iBbox.min().z << "," << iBbox.max().x << "," <<
                iBbox.max().y << "," << iBbox.max().z;

            if (isMelCallback)
            {
                sstrm << "} ";
            }
            else
            {
                sstrm << "] ";
            }
            std::string str = sstrm.str();
            iCallback.replace(pos, 13, str);
        }

        if (isMelCallback)
            MGlobal::executeCommand(iCallback.c_str(), true);
        else
            MGlobal::executePythonCommand(iCallback.c_str(), true);
    }
}

HDFWriteJob::HDFWriteJob(const std::vector< MDagPath > & iDagPath,
    const char * iFileName,
    bool iUseSelectionList,
    bool iWorldSpace,
    bool iWriteVisibility,
    bool iWriteUVs,
    std::set<float> & iTransFrames,
    std::set<float> & iShapeFrames,
    std::string & iMelPerFrameCallback,
    std::string & iMelPostCallback,
    std::string & iPythonPerFrameCallback,
    std::string & iPythonPostCallback,
    std::string & iPrefixFilter,
    std::set< std::string > & iAttribs)
{
    mDagPath = iDagPath;
    mFileName = iFileName;

    mUseSelectionList = iUseSelectionList;
    if (mUseSelectionList)
        getActiveSelectionListWithAllAncestors(mSList);

    mWorldSpace = iWorldSpace;
    mWriteVisibility = iWriteVisibility;
    mWriteUVs = iWriteUVs;

    mTransFrames = iTransFrames;
    mShapeFrames = iShapeFrames;

    mFilter = iPrefixFilter;
    mAttribs = iAttribs;

    // should have at least 1 value
    assert(!mTransFrames.empty() && !mShapeFrames.empty());

    // First transform sample and shape sample MUST be equal. Otherwise
    // setup would write a sample that is in one but not the other.
    assert(*(mTransFrames.begin()) == *(mShapeFrames.begin()));

    mFirstFrame = *(mTransFrames.begin());
    std::set<float>::iterator last = mTransFrames.end();
    last--;
    mLastFrame = *last;
    last = mShapeFrames.end();
    last--;

    float lastShapeFrame = *last;
    if (lastShapeFrame > mLastFrame)
        mLastFrame = lastShapeFrame;
    // if there is only one sample, then force static
    mShapesStatic = mShapeFrames.size() < 2;
    mTransStatic = mTransFrames.size() < 2;

    mMelPerFrameCallback = iMelPerFrameCallback;
    mMelPostCallback = iMelPostCallback;
    mPythonPerFrameCallback = iPythonPerFrameCallback;
    mPythonPostCallback = iPythonPostCallback;
    // shortcut to see if we ever need to calculate the bounds
    // when writing per frame geometry
    mPerFrameBounds = false;
    if (mMelPerFrameCallback.find("#BOUNDS#") != std::string::npos ||
        mPythonPerFrameCallback.find("#BOUNDS#") != std::string::npos ||
        mMelPerFrameCallback.find("#BOUNDSARRAY#") != std::string::npos ||
        mPythonPerFrameCallback.find("#BOUNDSARRAY#") != std::string::npos)
    {
        mPerFrameBounds = true;
    }
}

void HDFWriteJob::getBoundingBox(const MMatrix & eMInvMat)
{
    MStatus status;

    // short-circuit if the selection flag is on but this node is not in the
    // active selection

    // MGlobal::isSelected(ob) doesn't work, because DG node and DAG node is
    // not the same even if they refer to the same MObject
    if (mUseSelectionList && !mSList.hasItem(mCurDag))
        return;

    MObject ob = mCurDag.node();

    // check for riCurves flag for flattening all curve object to
    // one curve group
    MFnDependencyNode fnDepNode(ob, &status);
    MPlug riCurvesPlug = fnDepNode.findPlug("riCurves", &status);
    if ( status == MS::kSuccess && riCurvesPlug.asBool() == true)
    {
        MFnDagNode mFn(mCurDag, &status);
        if (status == MS::kSuccess)
        {
            MBoundingBox box = mFn.boundingBox();
            box.transformUsing(mCurDag.exclusiveMatrix()*eMInvMat);
            mCurBBox.expand(box);
        }
    }
    else if (ob.hasFn(MFn::kTransform))
    {
        MFnTransform fnTrans(ob);

        // loop through the children, making sure to push and pop them
        // from the MDagPath
        unsigned int numChild = mCurDag.childCount();
        for (unsigned int i = 0; i < numChild; ++i)
        {
            mCurDag.push(mCurDag.child(i));
            getBoundingBox(eMInvMat);
            mCurDag.pop();
        }
    }
    else if (ob.hasFn(MFn::kParticle) || ob.hasFn(MFn::kMesh)
        || ob.hasFn(MFn::kNurbsSurface) || ob.hasFn(MFn::kNurbsCurve))
    {
        if (util::isIntermediate(mCurDag.node()))
            return;

        MFnDagNode mFn(mCurDag, &status);
        if (status == MS::kSuccess)
        {
            MBoundingBox box = mFn.boundingBox();
            box.transformUsing(mCurDag.exclusiveMatrix()*eMInvMat);
            mCurBBox.expand(box);
        }
    }

    /*
     * The bounding box of a camera or light object is calculated differently.
     * Instead of using Maya's boundingBox() function, a [-1, -1, -1, 1, 1, 1]
     * box translated first in object space and then world space will be used
     * as the bounding box
     */
    else if (ob.hasFn(MFn::kCamera) || ob.hasFn(MFn::kLight))
    {
        MFnDagNode mFn(mCurDag, &status);
        if (status == MS::kSuccess)
        {
            MBoundingBox box(MPoint(-1.0, -1.0, -1.0), MPoint(1.0, 1.0, 1.0));
            box.transformUsing(mCurDag.exclusiveMatrix()*eMInvMat);
            mCurBBox.expand(box);
        }
    }
}

bool HDFWriteJob::checkCurveGrp()
{
    MItDag itDag(MItDag::kBreadthFirst, MFn::kNurbsCurve);
    itDag.reset(mCurDag, MItDag::kBreadthFirst, MFn::kNurbsCurve);

    bool init = false;
    int degree;
    MFnNurbsCurve::Form form;
    for (; !itDag.isDone(); itDag.next())
    {
        MDagPath curvePath;
        if (itDag.getPath(curvePath) == MS::kSuccess)
        {
            MObject curve = curvePath.node();
            if (!util::isIntermediate(curve) && curve.hasFn(MFn::kNurbsCurve))
            {
                MFnNurbsCurve fn(curvePath);
                if (!init)
                {
                    degree = fn.degree();
                    form = fn.form();
                    init = true;
                }
                else
                {
                    if (degree != fn.degree() || form != fn.form())
                        return false;
                }
            }
        }
    }
    return true;
}

void HDFWriteJob::setup(float iFrame, MayaTransformWriterPtr iParent)
{
    MStatus status;

    // short-circuit if selection flag is on but this node isn't actively
    // selected
    if (mUseSelectionList && !mSList.hasItem(mCurDag))
        return;

    MObject ob = mCurDag.node();

    float transFrame = iFrame;
    if (mTransStatic)
        transFrame  = FLT_MAX;

    float shapesFrame = iFrame;
    if (mShapesStatic)
        shapesFrame = FLT_MAX;

    // skip all intermediate nodes (and their children)
    if (util::isIntermediate(ob))
    {
        return;
    }

    // look for riCurves flag for flattening all curve objects to a curve group
    MFnDependencyNode fnDepNode(ob, &status);
    MPlug riCurvesPlug = fnDepNode.findPlug("riCurves", &status);
    bool riCurvesVal = riCurvesPlug.asBool();
    bool writeOutAsGroup = false;
    if (riCurvesVal)
    {
        writeOutAsGroup = checkCurveGrp();
        if (writeOutAsGroup == false)
        {
            MString msg = "Curves have different degrees or close ";
            msg += "states, not written out as curve group";
            MGlobal::displayWarning(msg);
        }
    }
    if ( status == MS::kSuccess && riCurvesVal && writeOutAsGroup)
    {
        MayaNurbsCurveWriterPtr nurbsCurve;
        if (iParent == NULL)
        {
            nurbsCurve = MayaNurbsCurveWriterPtr(new MayaNurbsCurveWriter(
                shapesFrame, mCurDag, mRoot, true, mWriteVisibility));
        }
        else
        {
            nurbsCurve = MayaNurbsCurveWriterPtr(new MayaNurbsCurveWriter(
                shapesFrame, mCurDag, iParent->getWriter(), true,
                mWriteVisibility));
        }

        if (nurbsCurve->isAnimated() && !mShapesStatic)
        {
            MayaNodePtr nd = nurbsCurve;
            mShapeList.push_back(nd);
            mStats.mCurveAnimNum++;
            mStats.mCurveAnimCurves += nurbsCurve->getNumCurves();
            mStats.mCurveAnimCVs += nurbsCurve->getNumCVs();
        }
        else
        {
            mStats.mCurveStaticNum++;
            mStats.mCurveStaticCurves += nurbsCurve->getNumCurves();
            mStats.mCurveStaticCVs += nurbsCurve->getNumCVs();
        }
    }
    else if (ob.hasFn(MFn::kTransform))
    {
        MFnTransform fnTrans(ob, &status);
        if (status != MS::kSuccess)
        {
            MString msg = "Initialize transform node ";
            msg += mCurDag.fullPathName();
            msg += " failed, skipping.";
            MGlobal::displayWarning(msg);
            return;
        }

        MayaTransformWriterPtr trans;

        // parented to the root case
        if (iParent == NULL)
        {
            trans = MayaTransformWriterPtr(new MayaTransformWriter(transFrame,
                mRoot, mCurDag, mWorldSpace, mWriteVisibility));
        }
        else
        {
            trans = MayaTransformWriterPtr(new MayaTransformWriter(transFrame,
                *iParent, mCurDag, mWriteVisibility));
        }

        if (trans->isAnimated() && !mTransStatic)
        {
            mTransList.push_back(trans);
            mStats.mTransAnimNum++;
        }
        else
            mStats.mTransStaticNum++;

        // loop through the children, making sure to push and pop them
        // from the MDagPath
        unsigned int numChild = mCurDag.childCount();
        for (unsigned int i = 0; i < numChild; ++i)
        {
            mCurDag.push(mCurDag.child(i));
            setup(iFrame, trans);
            mCurDag.pop();
        }
    }
    else if (ob.hasFn(MFn::kCamera))
    {
        MFnCamera fnCamera(ob, &status);
        if (status != MS::kSuccess)
        {
            MString msg = "Initialize camera node ";
            msg += mCurDag.fullPathName();
            msg += " failed, skipping.";
            MGlobal::displayWarning(msg);
            return;
        }

        if (iParent != NULL)
        {
            MayaCameraWriterPtr camera(new MayaCameraWriter(shapesFrame,
                mCurDag, iParent->getWriter(), mWriteVisibility));

            if (camera->isAnimated() && !mShapesStatic)
            {
                MayaNodePtr nd = camera;
                mShapeList.push_back(nd);
                mStats.mCameraAnimNum++;
            }
            else
                mStats.mCameraStaticNum++;
        }
        else
        {
            MString err = "Can't translate ";
            err += fnCamera.name() + " since it doesn't have a parent.";
            MGlobal::displayError(err);
        }
    }
    else if (ob.hasFn(MFn::kLight))
    {
        MFnLight fnLight(ob, &status);
        if (status != MS::kSuccess)
        {
            MString msg = "Initialize light node ";
            msg += mCurDag.fullPathName();
            msg += " failed, skipping.";
            MGlobal::displayWarning(msg);
            return;
        }

        if (iParent != NULL)
        {
            MayaLightWriterPtr light(new MayaLightWriter(shapesFrame, mCurDag,
                iParent->getWriter(), mWriteVisibility));

            if (light->isAnimated() && !mShapesStatic)
            {
                MayaNodePtr nd = light;
                mShapeList.push_back(nd);
            }

            mStats.mGenericNum++;
        }
        else
        {
            MString err = "Can't translate ";
            err += fnLight.name();
            err += " since it doesn't have a parent.";
            MGlobal::displayError(err);
        }
    }
    else if (ob.hasFn(MFn::kLocator))
    {
        MFnDependencyNode fnLocator(ob, & status);
        if (status != MS::kSuccess)
        {
            MString msg = "Initialize locator node ";
            msg += mCurDag.fullPathName();
            msg += " failed, skipping.";
            MGlobal::displayWarning(msg);
            return;
        }

        if (iParent != NULL)
        {
            MayaLocatorWriterPtr locator(
                new MayaLocatorWriter(shapesFrame, mCurDag,
                iParent->getWriter(), mWriteVisibility));

            if (locator->isAnimated() && !mShapesStatic)
            {
                MayaNodePtr nd = locator;
                mShapeList.push_back(nd);
            }

            mStats.mGenericNum++;
        }
        else
        {
            MString err = "Can't translate ";
            err += fnLocator.name();
            err += " since it doesn't have a parent.";
            MGlobal::displayError(err);
        }
    }
    else if (ob.hasFn(MFn::kParticle))
    {
        MFnParticleSystem mFnParticle(ob, &status);
        if (status != MS::kSuccess)
        {
            MString msg = "Initialize particle system ";
            msg += mCurDag.fullPathName();
            msg += " failed, skipping.";
            MGlobal::displayWarning(msg);
            return;
        }

        if (iParent != NULL)
        {
            MayaPointPrimitiveWriterPtr particle(new MayaPointPrimitiveWriter(
                shapesFrame, mCurDag, iParent->getWriter(), mWriteVisibility));

            if (particle->isAnimated() && !mShapesStatic)
            {
                MayaNodePtr nd = particle;
                mShapeList.push_back(nd);
                mStats.mPointAnimNum++;
                mStats.mPointAnimCVs += particle->getNumCVs();
            }
            else
            {
                mStats.mPointStaticNum++;
                mStats.mPointStaticCVs += particle->getNumCVs();
            }
        }
        else
        {
            MString err = "Can't translate ";
            err += mFnParticle.name() + " since it doesn't have a parent.";
            MGlobal::displayError(err);
        }
    }
    else if (ob.hasFn(MFn::kMesh))
    {
        MFnMesh fnMesh(ob, &status);
        if (status != MS::kSuccess)
        {
            MString msg = "Initialize mesh node ";
            msg += mCurDag.fullPathName();
            msg += " failed, skipping.";
            MGlobal::displayWarning(msg);
            return;
        }

        if (iParent != NULL)
        {
            MayaMeshWriterPtr mesh(new MayaMeshWriter(shapesFrame, mCurDag,
                iParent->getWriter(), mWriteVisibility, mWriteUVs));

            if (mesh->isAnimated() && !mShapesStatic)
            {
                MayaNodePtr nd = mesh;
                mShapeList.push_back(nd);
                if (mesh->isSubD())
                {
                    mStats.mSubDAnimNum++;
                    mStats.mSubDAnimCVs += mesh->getNumCVs();
                    mStats.mSubDAnimFaces += mesh->getNumFaces();
                }
                else
                {
                    mStats.mPolyAnimNum++;
                    mStats.mPolyAnimCVs += mesh->getNumCVs();
                    mStats.mPolyAnimFaces += mesh->getNumFaces();
                }
            }
            else
            {
                if (mesh->isSubD())
                {
                    mStats.mSubDStaticNum++;
                    mStats.mSubDStaticCVs += mesh->getNumCVs();
                    mStats.mSubDStaticFaces += mesh->getNumFaces();
                }
                else
                {
                    mStats.mPolyStaticNum++;
                    mStats.mPolyStaticCVs += mesh->getNumCVs();
                    mStats.mPolyStaticFaces += mesh->getNumFaces();
                }
            }
        }
        else
        {
            MString err = "Can't translate ";
            err += fnMesh.name() + " since it doesn't have a parent.";
            MGlobal::displayError(err);
        }
    }
    else if (ob.hasFn(MFn::kNurbsSurface))
    {
        MFnNurbsSurface fnNurbsSurface(ob, &status);
        if (status != MS::kSuccess)
        {
            MString msg = "Initialize nurbs surface ";
            msg += mCurDag.fullPathName();
            msg += " failed, skipping.";
            MGlobal::displayWarning(msg);
            return;
        }

        if (iParent != NULL)
        {
            MayaNurbsSurfaceWriterPtr nurbsSurface(new MayaNurbsSurfaceWriter(
                shapesFrame, mCurDag, iParent->getWriter(), mWriteVisibility));

            if (nurbsSurface->isAnimated() && !mShapesStatic)
            {
                MayaNodePtr nd = nurbsSurface;
                mShapeList.push_back(nd);
                mStats.mNurbsAnimNum++;
                mStats.mNurbsAnimCVs += nurbsSurface->getNumCVs();
            }
            else
            {
                mStats.mNurbsStaticNum++;
                mStats.mNurbsStaticCVs += nurbsSurface->getNumCVs();
            }
        }
        else
        {
            MString err = "Can't translate ";
            err += fnNurbsSurface.name() + " since it doesn't have a parent.";
            MGlobal::displayError(err);
        }
    }
    else if (ob.hasFn(MFn::kNurbsCurve))
    {
        MFnNurbsCurve fnNurbsCurve(ob, &status);
        if (status != MS::kSuccess)
        {
            MString msg = "Initialize curve node ";
            msg += mCurDag.fullPathName();
            msg += " failed, skipping.";
            MGlobal::displayWarning(msg);
            return;
        }

        if (iParent != NULL)
        {
            MayaNurbsCurveWriterPtr nurbsCurve(new MayaNurbsCurveWriter(
                shapesFrame, mCurDag, iParent->getWriter(), false,
                mWriteVisibility));

            if (nurbsCurve->isAnimated() && !mShapesStatic)
            {
                MayaNodePtr nd = nurbsCurve;
                mShapeList.push_back(nd);
                mStats.mCurveAnimNum++;
                mStats.mCurveAnimCurves++;
                mStats.mCurveAnimCVs += nurbsCurve->getNumCVs();
            }
            else
            {
                mStats.mCurveStaticNum++;
                mStats.mCurveStaticCurves++;
                mStats.mCurveStaticCVs += nurbsCurve->getNumCVs();
            }
        }
        else
        {
            MString err = "Can't translate ";
            err += fnNurbsCurve.name() + " since it doesn't have a parent.";
            MGlobal::displayError(err);
        }
    }
    else
    {
        MString warn = mCurDag.fullPathName() + " is an unsupported type of ";
        warn += ob.apiTypeStr();
        MGlobal::displayWarning(warn);
    }
}


HDFWriteJob::~HDFWriteJob()
{
}

bool HDFWriteJob::eval(float iFrame)
{
    if (iFrame == mFirstFrame)
    {
        // check if the shortnames of any two nodes are the same
        // if so, exit here
        if (hasDuplicates(mDagPath))
        {
            throw std::runtime_error("The names of root nodes are the same");
        }

        mRoot = TransformWriterPtr(new Tako::TransformWriter(
            mFileName.c_str()));

        if (mRoot->getHid() < 0)
        {
            MGlobal::displayError("Unable to create hdf file");
            throw std::runtime_error("Unable to create hdf file");
        }

        AttributesWriter::mFilter = &mFilter;
        AttributesWriter::mAttribs = &mAttribs;

        unsigned int nodeSize = mDagPath.size();
        for (unsigned int obIndex = 0; obIndex < nodeSize; obIndex++)
        {
            mCurDag = mDagPath[obIndex];
            setup(iFrame, MayaTransformWriterPtr());
        }
        perFrameCallback(iFrame);
    }
    else
    {
        std::set<float>::iterator checkFrame = mShapeFrames.find(iFrame);
        bool foundShapeFrame = false;
        if (checkFrame != mShapeFrames.end() && !mShapeList.empty())
        {
            assert(mRoot != NULL);
            foundShapeFrame = true;
            std::vector< MayaNodePtr >::iterator it = mShapeList.begin();
            std::vector< MayaNodePtr >::iterator end = mShapeList.end();
            CallWriteVisitor visit(iFrame);
            IncrementCVCountsVisitor cntVisitor;
            for (; it != end; it++)
            {
                boost::apply_visitor(visit, *it);
                boost::apply_visitor(cntVisitor, *it);
            }
            mStats.mNurbsAnimCVs += cntVisitor.mCVsArray[0];
            mStats.mCurveAnimCVs += cntVisitor.mCVsArray[1];
            mStats.mPointAnimCVs += cntVisitor.mCVsArray[2];
            mStats.mSubDAnimCVs  += cntVisitor.mCVsArray[3];
            mStats.mPolyAnimCVs  += cntVisitor.mCVsArray[4];
        }

        checkFrame = mTransFrames.find(iFrame);
        bool foundTransFrame = false;
        if (checkFrame != mTransFrames.end() && !mTransList.empty())
        {
            assert(mRoot != NULL);
            foundTransFrame = true;
            std::vector< MayaTransformWriterPtr >::iterator tcur =
                mTransList.begin();

            std::vector< MayaTransformWriterPtr >::iterator tend =
                mTransList.end();

            for (; tcur != tend; tcur++)
            {
                (*tcur)->write(iFrame);
            }
        }

        if (foundTransFrame || foundShapeFrame)
            perFrameCallback(iFrame);
    }

    if (iFrame == mLastFrame)
    {
        postCallback(iFrame);
        return true;
    }

    return false;
}

void HDFWriteJob::perFrameCallback(float iFrame)
{
    MBoundingBox bbox;

    if (mPerFrameBounds)
    {
        unsigned int nodeSize = mDagPath.size();
        for (unsigned int obIndex = 0; obIndex < nodeSize; obIndex++)
        {
            mCurDag = mDagPath[obIndex];

            mCurBBox.clear();
            MMatrix eMInvMat;
            if (mWorldSpace)
            {
                eMInvMat.setToIdentity();
            }
            else
            {
                eMInvMat = mCurDag.exclusiveMatrixInverse();
            }

            getBoundingBox(eMInvMat);
            bbox.expand(mCurBBox);
        }
    }

    processCallback(mMelPerFrameCallback, true, iFrame, bbox);
    processCallback(mPythonPerFrameCallback, false, iFrame, bbox);
}


// write the frame ranges and statistic string on the root
// Also call the post callbacks
void HDFWriteJob::postCallback(float iFrame)
{
    bool writeStatisticStr = false;
    std::string statsStr = "";

    addToString(statsStr, "SubDStaticNum", mStats.mSubDStaticNum);
    addToString(statsStr, "SubDAnimNum", mStats.mSubDAnimNum);
    addToString(statsStr, "SubDStaticCVs", mStats.mSubDStaticCVs);
    addToString(statsStr, "SubDAnimCVs", mStats.mSubDAnimCVs);
    addToString(statsStr, "SubDStaticFaces", mStats.mSubDStaticFaces);
    addToString(statsStr, "SubDAnimFaces", mStats.mSubDAnimFaces);

    addToString(statsStr, "PolyStaticNum", mStats.mPolyStaticNum);
    addToString(statsStr, "PolyAnimNum", mStats.mPolyAnimNum);
    addToString(statsStr, "PolyStaticCVs", mStats.mPolyStaticCVs);
    addToString(statsStr, "PolyAnimCVs", mStats.mPolyAnimCVs);
    addToString(statsStr, "PolyStaticFaces", mStats.mPolyStaticFaces);
    addToString(statsStr, "PolyAnimFaces", mStats.mPolyAnimFaces);

    addToString(statsStr, "CurveStaticNum", mStats.mCurveStaticNum);
    addToString(statsStr, "CurveStaticCurves", mStats.mCurveStaticCurves);
    addToString(statsStr, "CurveAnimNum", mStats.mCurveAnimNum);
    addToString(statsStr, "CurveAnimCurves", mStats.mCurveAnimCurves);
    addToString(statsStr, "CurveStaticCVs", mStats.mCurveStaticCVs);
    addToString(statsStr, "CurveAnimCVs", mStats.mCurveAnimCVs);

    addToString(statsStr, "PointStaticNum", mStats.mPointStaticNum);
    addToString(statsStr, "PointAnimNum", mStats.mPointAnimNum);
    addToString(statsStr, "PointStaticCVs", mStats.mPointStaticCVs);
    addToString(statsStr, "PointAnimCVs", mStats.mPointAnimCVs);

    addToString(statsStr, "NurbsStaticNum", mStats.mNurbsStaticNum);
    addToString(statsStr, "NurbsAnimNum", mStats.mNurbsAnimNum);
    addToString(statsStr, "NurbsStaticCVs", mStats.mNurbsStaticCVs);
    addToString(statsStr, "NurbsAnimCVs", mStats.mNurbsAnimCVs);

    addToString(statsStr, "TransStaticNum", mStats.mTransStaticNum);
    addToString(statsStr, "TransAnimNum", mStats.mTransAnimNum);

    addToString(statsStr, "CameraStaticNum", mStats.mCameraStaticNum);
    addToString(statsStr, "CameraAnimNum", mStats.mCameraAnimNum);

    addToString(statsStr, "GenericNum", mStats.mGenericNum);

    if (statsStr.length() > 0)
    {
        Tako::PropertyPair p;
        p.first = statsStr;
        mRoot->setNonSampledProperty("statistics", p);
        writeStatisticStr = true;
    }

    bool writeframes = false;
    size_t transFrameSize = mTransFrames.size();
    size_t shapeFrameSize = mShapeFrames.size();
    bool staticShape = mShapeList.empty();
    bool staticTrans = mTransList.empty();

    if (staticShape && staticTrans && statsStr.length() > 0)
        mRoot->writeProperties(0.0);

    // same number of transform frames as shape frames AND something is
    // animated
    if (transFrameSize > 1 && transFrameSize == shapeFrameSize
        && (!staticTrans || !staticShape) )
    {
        Tako::PropertyPair p;
        p.first = std::vector<float>(mTransFrames.begin(),
            mTransFrames.end());

        mRoot->setNonSampledProperty("frameRange", p);
        mRoot->writeProperties(0.0);
        writeframes = true;
    }

    // different number of transform and shape frames animated transforms
    // but static shapes, so advertise the transforms frames
    else if (transFrameSize > 1 && shapeFrameSize > 1 &&
        transFrameSize != shapeFrameSize && !staticTrans && staticShape)
    {
        Tako::PropertyPair p;
        p.first = std::vector<float>(mTransFrames.begin(),
            mTransFrames.end());

        mRoot->setNonSampledProperty("frameRange", p);
        mRoot->writeProperties(0.0);
        writeframes = true;
    }

    // different number of transform and shape frames animated shapes
    // but static transforms, so advertise the shapes frames
    else if (transFrameSize > 1 && shapeFrameSize > 1 &&
        transFrameSize != shapeFrameSize && staticTrans && !staticShape)
    {
        Tako::PropertyPair p;
        p.first = std::vector<float>(mShapeFrames.begin(),
            mShapeFrames.end());

        mRoot->setNonSampledProperty("frameRange", p);
        mRoot->writeProperties(0.0);
        writeframes = true;
    }

    // different number of transform frames and shape frames and both are
    // actually animated, write out both
    else if (transFrameSize > 1 && shapeFrameSize > 1 &&
        transFrameSize != shapeFrameSize && !staticTrans && !staticShape )
    {
        Tako::PropertyPair p1;
        p1.first = std::vector<float>(mTransFrames.begin(),
            mTransFrames.end());

        mRoot->setNonSampledProperty("frameRangeTrans", p1);

        Tako::PropertyPair p2;
        p2.first = std::vector<float>(mShapeFrames.begin(),
            mShapeFrames.end());

        mRoot->setNonSampledProperty("frameRangeShape", p2);
        mRoot->writeProperties(0.0);
        writeframes = true;
    }

    if (!writeframes && writeStatisticStr)
        mRoot->writeProperties(0.0);


    MBoundingBox bbox;

    if (mMelPostCallback.find("#BOUNDS#") != std::string::npos ||
        mPythonPostCallback.find("#BOUNDS#") != std::string::npos ||
        mMelPostCallback.find("#BOUNDSARRAY#") != std::string::npos ||
        mPythonPostCallback.find("#BOUNDSARRAY#") != std::string::npos)
    {
        unsigned int nodeSize = mDagPath.size();
        for (unsigned int obIndex = 0; obIndex < nodeSize; obIndex++)
        {
            mCurDag = mDagPath[obIndex];

            mCurBBox.clear();
            MMatrix eMInvMat;
            if (mWorldSpace)
            {
                eMInvMat.setToIdentity();
            }
            else
            {
                eMInvMat = mCurDag.exclusiveMatrixInverse();
            }

            getBoundingBox(eMInvMat);
            bbox.expand(mCurBBox);
        }
    }

    processCallback(mMelPostCallback, true, iFrame, bbox);
    processCallback(mPythonPostCallback, false, iFrame, bbox);
}

} // End namespace Alembic
