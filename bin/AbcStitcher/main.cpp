//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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


// include ALL of our types, since we could be reading or writing any of them
//
#include <Alembic/Tako/CameraReader.h>
#include <Alembic/Tako/CameraWriter.h>
#include <Alembic/Tako/GenericNodeReader.h>
#include <Alembic/Tako/GenericNodeWriter.h>
#include <Alembic/Tako/NurbsCurveReader.h>
#include <Alembic/Tako/NurbsCurveWriter.h>
#include <Alembic/Tako/NurbsSurfaceReader.h>
#include <Alembic/Tako/NurbsSurfaceWriter.h>
#include <Alembic/Tako/PolyMeshReader.h>
#include <Alembic/Tako/PolyMeshWriter.h>
#include <Alembic/Tako/PointPrimitiveReader.h>
#include <Alembic/Tako/PointPrimitiveWriter.h>
#include <Alembic/Tako/SubDReader.h>
#include <Alembic/Tako/SubDWriter.h>
#include <Alembic/Tako/TransformReader.h>
#include <Alembic/Tako/TransformWriter.h>

#include <boost/variant.hpp>


#include <stdexcept>

using Alembic::Tako::PropertyMap;
using Alembic::Tako::PropertyPair;
using Alembic::Tako::index_t;

typedef boost::shared_ptr <Alembic::Tako::TransformWriter > TransformWriterPtr;

// Assumptions:
// 1. the hierarchy of each hdf file is the same
// 2. the frameranges of each individual hdf file added together
// will be a consistent range

namespace
{
    void initProperties(
        float frame, bool & propSampled,
        Alembic::Tako::HDFReaderNode & reader,
        Alembic::Tako::HDFWriterNode & writer)
    {
        reader.readProperties(frame);

        PropertyMap::const_iterator p = reader.beginNonSampledProperties();
        PropertyMap::const_iterator pend = reader.endNonSampledProperties();
        for (; p != pend; ++p)
            writer.setNonSampledProperty(p->first.c_str(), p->second);

        p = reader.beginSampledProperties();
        pend = reader.endSampledProperties();
        for (; p != pend; ++p)
        {
            propSampled = true;
            writer.setSampledProperty(p->first.c_str(), p->second);
        }

        writer.writeProperties(frame);

        // we are done with the non sampled properties, so clear them
        reader.clearNonSampledProperties();
    }

    void updateSampledProperties(
        std::set<float>::iterator fStart, std::set<float>::iterator fEnd,
        Alembic::Tako::HDFReaderNode & reader,
        Alembic::Tako::HDFWriterNode & writer)
    {
        for (std::set<float>::iterator f = fStart; f != fEnd; ++f)
        {
            reader.readProperties(*f);

            PropertyMap::const_iterator p = reader.beginSampledProperties();
            PropertyMap::const_iterator pend = reader.endSampledProperties();
            for (; p != pend; ++p)
                writer.updateSample(p->first.c_str(), p->second);
            writer.writeProperties(*f);
        }
    }
}

class ReadWriteVisitor : public boost::static_visitor <>
{
    public:

    // constructor creates the result file and write the frame range prop
    // under the root
    ReadWriteVisitor(const char * iFile,
        std::vector< Alembic::Tako::TransformReaderPtr > & iRoots)
    {
        // build up the frame ranges first
        TransformWriterPtr root(new Alembic::Tako::TransformWriter(iFile));
        mWriteParents.push_back(root);

        mTransStack.push_back(iRoots);
        mCurPos.push_back(0);

        std::set<float> transFrames;
        std::set<float> shapeFrames;

        std::vector< Alembic::Tako::TransformReaderPtr >::iterator itEnd =
            iRoots.end();

        for (std::vector< Alembic::Tako::TransformReaderPtr >::iterator it =
            iRoots.begin(); it != itEnd; ++it)
        {
            (*it)->readProperties(0.0);
            PropertyMap::const_iterator p = (*it)->beginNonSampledProperties();
            PropertyMap::const_iterator pend = (*it)->endNonSampledProperties();

            std::set <float> tf;
            std::set <float> sf;

            // loop through the properties looking for frameRange related
            // attributes, these provide the hint of what samples we need
            // to write out
            for (; p != pend; ++p)
            {
                if (p->first == "frameRange")
                {
                    std::vector <float> fr = boost::get<std::vector<float> >
                        (p->second.first);

                    // check each frame, make sure it isn't in the global list
                    std::vector <float>::iterator f = fr.begin();
                    std::vector <float>::iterator fend = fr.end();
                    for (; f != fend; ++f)
                    {
                        if (transFrames.find(*f) == transFrames.end())
                        {
                            tf.insert(*f);
                            transFrames.insert(*f);
                        }

                        if (shapeFrames.find(*f) == shapeFrames.end())
                        {
                            sf.insert(*f);
                            shapeFrames.insert(*f);
                        }
                    }
                }
                else if (p->first == "frameRangeShape")
                {
                    std::vector <float> fr = boost::get<std::vector<float> >
                        (p->second.first);

                    // check each frame, make sure it isn't in the global
                    // shape frame list
                    std::vector <float>::iterator f = fr.begin();
                    std::vector <float>::iterator fend = fr.end();
                    for (; f != fend; ++f)
                    {
                        if (shapeFrames.find(*f) == shapeFrames.end())
                        {
                            sf.insert(*f);
                            shapeFrames.insert(*f);
                        }
                    }
                }
                else if (p->first == "frameRangeTrans")
                {
                    std::vector <float> fr = boost::get<std::vector<float> >
                        (p->second.first);

                    // check each frame, make sure it isn't in the global
                    // shape frame list
                    std::vector <float>::iterator f = fr.begin();
                    std::vector <float>::iterator fend = fr.end();
                    for (; f != fend; ++f)
                    {
                        if (transFrames.find(*f) == transFrames.end())
                        {
                            tf.insert(*f);
                            transFrames.insert(*f);
                        }
                    }
                }
            }  // for p

            // push each set to the appropriate vector
            if (!sf.empty())
                mShapeFrames.push_back(sf);
            if (!tf.empty())
                mTransFrames.push_back(tf);
        }  // for iRoot

        // set the frame ranges in our root node
        if (!transFrames.empty() && transFrames.size() == shapeFrames.size())
        {
            PropertyPair p;
            p.first = std::vector<float>(transFrames.begin(),
                transFrames.end());

            root->setNonSampledProperty("frameRange", p);
            root->writeProperties(0.0);
        }
        else if (!transFrames.empty() && !shapeFrames.empty())
        {
            PropertyPair p1;
            p1.first = std::vector<float>(transFrames.begin(),
                transFrames.end());

            root->setNonSampledProperty("frameRangeTrans", p1);

            PropertyPair p2;
            p2.first = std::vector<float>(shapeFrames.begin(),
                shapeFrames.end());
            root->setNonSampledProperty("frameRangeShape", p2);
            root->writeProperties(0.0);
        }
        else
        {
            throw std::runtime_error("Error all input files are static.");
        }
    };

    void operator() (Alembic::Tako::CameraReaderPtr & iNode)
    {
        size_t numSets = mShapeFrames.size();

        bool shapesSampled = true;
        bool propSampled = false;

        Alembic::Tako::CameraWriter camWrite(iNode->getName(),
            *(mWriteParents.back()) );

        size_t curPos = mCurPos.back();

        for (size_t i = 0; i < numSets; ++i)
        {
            std::set<float>::iterator fstart = mShapeFrames[i].begin();
            std::set<float>::iterator fend = mShapeFrames[i].end();
            Alembic::Tako::CameraReaderPtr camRead = 
                boost::get< Alembic::Tako::CameraReaderPtr >
                (mTransStack.back()[i]->getChild(curPos));

            // build up the property map and read the initial camera data
            if (i == 0)
            {
                unsigned int readHint = camRead->read(*fstart);

                if (readHint == Alembic::Tako::CameraReader::READ_ERROR)
                    throw std::logic_error("Error reading camera ");

                float initFrame = *fstart;

                if ( !camRead->hasFrames() )
                {
                    shapesSampled = false;
                    initFrame = FLT_MAX;
                }

                camWrite.write(initFrame, camRead->mCameraData);

                // set up the property map
                initProperties(*fstart, propSampled, *camRead, camWrite);

                // increment by 1 because we did the setup
                fstart++;

                if (!shapesSampled && !propSampled)
                    i = numSets;
            }

            if (shapesSampled)
            {
                for (std::set<float>::iterator f = fstart; f != fend; ++f)
                {
                    unsigned int readHint = camRead->read(*f);

                    if (readHint == Alembic::Tako::CameraReader::READ_ERROR)
                    {
                        throw std::logic_error("Error reading camera sample");
                    }
                    else
                    {
                        camWrite.write(*f, camRead->mCameraData);
                    }
                }
            }

            if (propSampled)
                updateSampledProperties(fstart, fend, *camRead, camWrite);
        }
    };

    void operator() (Alembic::Tako::GenericNodeReaderPtr & iNode)
    {
        size_t numSets = mShapeFrames.size();

        bool propSampled = false;

        Alembic::Tako::GenericNodeWriter writer(
            iNode->getName(), *(mWriteParents.back()));
        size_t curPos = mCurPos.back();

        for (size_t i = 0; i < numSets; ++i)
        {
            std::set<float>::iterator fstart = mShapeFrames[i].begin();
            std::set<float>::iterator fend = mShapeFrames[i].end();
            Alembic::Tako::GenericNodeReaderPtr readerPtr =
                boost::get< Alembic::Tako::GenericNodeReaderPtr >
                    (mTransStack.back()[i]->getChild(curPos));

            if (readerPtr->mUserType != iNode->mUserType)
            {
                throw std::logic_error("Generic node types differ.");
            }

            writer.write(readerPtr->mUserType);

            // build up the property map
            if (i == 0)
            {
                initProperties(*fstart, propSampled, *readerPtr, writer);

                // increment by 1 because we did the setup
                fstart++;

                if (!propSampled)
                    i = numSets;
            }

            if (propSampled)
                updateSampledProperties(fstart, fend, *readerPtr, writer);
        }
    };

    void operator() (Alembic::Tako::NurbsCurveReaderPtr & iNode)
    {
        size_t numSets = mShapeFrames.size();

        bool shapesSampled = true;
        bool propSampled = false;

        Alembic::Tako::NurbsCurveWriter ncWrite(
            iNode->getName(), *(mWriteParents.back()));
        size_t curPos = mCurPos.back();

        for (size_t i = 0; i < numSets; ++i)
        {
            std::set<float>::iterator fstart = mShapeFrames[i].begin();
            std::set<float>::iterator fend = mShapeFrames[i].end();
            Alembic::Tako::NurbsCurveReaderPtr ncRead =
                boost::get< Alembic::Tako::NurbsCurveReaderPtr >
                    (mTransStack.back()[i]->getChild(curPos));

            // build up the property map and read the initial curve data
            if (i == 0)
            {
                unsigned int readHint = ncRead->read(*fstart);

                if (readHint == Alembic::Tako::NurbsCurveReader::READ_ERROR)
                    throw std::logic_error("Error reading nurbs curves");

                float initFrame = *fstart;

                // this trancform is not sampled so no need to loop over
                // the other sample sets
                if (readHint ==
                    Alembic::Tako::NurbsCurveReader::GEOMETRY_STATIC)
                {
                    shapesSampled = false;
                    initFrame = FLT_MAX;
                }

                ncWrite.write(initFrame, ncRead->mCurveGrp);

                // set up the property map
                initProperties(*fstart, propSampled, *ncRead, ncWrite);

                // because we did the setup, increment
                fstart++;

                if (!shapesSampled && !propSampled)
                    i = numSets;
            }

            if (shapesSampled)
            {
                for (std::set<float>::iterator f = fstart; f != fend; ++f)
                {
                    unsigned int readHint = ncRead->read(*f);

                    if (readHint == Alembic::Tako::NurbsCurveReader::READ_ERROR)
                    {
                        throw std::logic_error(
                            "Error reading curve data sample");
                    }

                    // for now the nurbs curve is rewritten at every frame,
                    // thus no need to preserve information about topology.
                    // This may change one day for performance reason

                    else
                    {
                        ncWrite.write(*f, ncRead->mCurveGrp);
                    }
                }
            }

            if (propSampled)
                updateSampledProperties(fstart, fend, *ncRead, ncWrite);
        }  // for each num set
    };

    void operator() (Alembic::Tako::NurbsSurfaceReaderPtr & iNode)
    {
        size_t numSets = mShapeFrames.size();

        bool shapesSampled = true;
        bool propSampled = false;

        Alembic::Tako::NurbsSurfaceWriter nsWrite(
            iNode->getName(), *(mWriteParents.back()));
        size_t curPos = mCurPos.back();

        for (size_t i = 0; i < numSets; ++i)
        {
            std::set<float>::iterator fstart = mShapeFrames[i].begin();
            std::set<float>::iterator fend = mShapeFrames[i].end();
            Alembic::Tako::NurbsSurfaceReaderPtr nsRead =
                boost::get< Alembic::Tako::NurbsSurfaceReaderPtr >
                    (mTransStack.back()[i]->getChild(curPos));

            // build up the property map and read the initial surface data
            if (i == 0)
            {
                unsigned int readHint = nsRead->read(*fstart);

                if (readHint == Alembic::Tako::NurbsSurfaceReader::READ_ERROR)
                    throw std::logic_error("Error reading nurbs surface");

                float initFrame = *fstart;

                // this transform is not sampled so no need to loop over
                // the other sample sets
                if (readHint ==
                    Alembic::Tako::NurbsSurfaceReader::GEOMETRY_STATIC)
                {
                    shapesSampled = false;
                    initFrame = FLT_MAX;
                }

                nsWrite.start(initFrame);
                nsWrite.write(nsRead->mCV,
                    nsRead->mKnotsInU, nsRead->mKnotsInV, nsRead->mMiscInfo);
                if (nsRead->mTrimCurve.size() > 0)
                    nsWrite.writeTrimCurve(nsRead->mTrimCurve);
                nsWrite.end();

                // set up the property map
                initProperties(*fstart, propSampled, *nsRead, nsWrite);

                // increment because we did the initial setup
                fstart++;

                if (!shapesSampled && !propSampled)
                    i = numSets;
            }

            if (shapesSampled)
            {
                for (std::set<float>::iterator f = fstart; f != fend; ++f)
                {
                    unsigned int readHint = nsRead->read(*f);

                    if (readHint ==
                        Alembic::Tako::NurbsSurfaceReader::READ_ERROR)
                    {
                        throw std::logic_error(
                            "Error reading surface data sample");
                    }

                    // for now the nurbs surface is rewritten at every frame,
                    // thus no need to preserve information about topology.
                    // This may change one day for performance reason

                    else
                    {
                        nsWrite.start(*f);
                        nsWrite.write(nsRead->mCV,
                            nsRead->mKnotsInU,
                            nsRead->mKnotsInV,
                            nsRead->mMiscInfo);
                        if (nsRead->mTrimCurve.size() > 0)
                            nsWrite.writeTrimCurve(nsRead->mTrimCurve);
                        nsWrite.end();
                    }
                }
            }

            if (propSampled)
                updateSampledProperties(fstart, fend, *nsRead, nsWrite);
        }  // for each num set
    };

    void operator() (Alembic::Tako::PointPrimitiveReaderPtr & iNode)
    {
        size_t numSets = mShapeFrames.size();

        bool shapesSampled = true;
        bool propSampled = false;

        Alembic::Tako:: PointPrimitiveWriter ppWrite(
            iNode->getName(), *(mWriteParents.back()));
        size_t curPos = mCurPos.back();

        for (size_t i = 0; i < numSets; ++i)
        {
            std::set<float>::iterator fstart = mShapeFrames[i].begin();
            std::set<float>::iterator fend = mShapeFrames[i].end();
            Alembic::Tako::PointPrimitiveReaderPtr ppRead =
                boost::get< Alembic::Tako::PointPrimitiveReaderPtr >
                    (mTransStack.back()[i]->getChild(curPos));

            // build up the property map and
            // read the initial point primitive data
            if (i == 0)
            {
                unsigned int readHint = ppRead->read(*fstart);

                if (readHint == Alembic::Tako::PointPrimitiveReader::READ_ERROR)
                {
                    throw std::logic_error(
                        "Error reading point primitive data ");
                }

                float initFrame = *fstart;

                if ( !ppRead->hasFrames() )
                {
                    shapesSampled = false;
                    initFrame = FLT_MAX;
                }

                if (ppRead->mWidth.size() > 0)
                {
                    ppWrite.write(initFrame,
                        ppRead->mPosition,
                        ppRead->mVelocity,
                        ppRead->mParticleIds,
                        ppRead->mWidth);
                }
                else
                {
                    ppWrite.write(initFrame,
                        ppRead->mPosition,
                        ppRead->mVelocity,
                        ppRead->mParticleIds,
                        ppRead->mConstantWidth);
                }

                // set up the property map
                initProperties(*fstart, propSampled, *ppRead, ppWrite);

                // increment since we did the setup
                fstart++;

                if (!shapesSampled && !propSampled)
                    i = numSets;
            }

            if (shapesSampled)
            {
                for (std::set<float>::iterator f = fstart; f != fend; ++f)
                {
                    unsigned int readHint = ppRead->read(*f);

                    if (readHint ==
                        Alembic::Tako::PointPrimitiveReader::READ_ERROR)
                    {
                        throw std::logic_error(
                            "Error reading point primitive sample");
                    }
                    else
                    {
                        if (ppRead->mWidth.size() > 0)
                        {
                            ppWrite.write(*f,
                                ppRead->mPosition,
                                ppRead->mVelocity,
                                ppRead->mParticleIds,
                                ppRead->mWidth);
                        }
                        else
                        {
                            ppWrite.write(*f,
                                ppRead->mPosition,
                                ppRead->mVelocity,
                                ppRead->mParticleIds,
                                ppRead->mConstantWidth);
                        }
                    }
                }
            }

            if (propSampled)
                updateSampledProperties(fstart, fend, *ppRead, ppWrite);
        }
    };

    void operator() (Alembic::Tako::SubDReaderPtr & iNode)
    {
        size_t numSets = mShapeFrames.size();

        bool shapesSampled = true;
        bool propSampled = false;

        Alembic::Tako::SubDWriter subDWrite(iNode->getName(),
            *(mWriteParents.back()) );
        size_t curPos = mCurPos.back();

        // the following information decides
        // whether the topology of a subD mesh changes
        std::vector<float>      points;
        std::vector<index_t>  facePoints;
        std::vector<index_t>  faceList;

        std::vector<float>      creaseSharpness;
        std::vector<index_t>  creaseIndices;
        std::vector<index_t>  creaseLengths;

        std::vector<float>      cornerSharpness;
        std::vector<index_t>  cornerIndices;

        for (size_t i = 0; i < numSets; ++i)
        {
            std::set<float>::iterator fstart = mShapeFrames[i].begin();
            std::set<float>::iterator fend = mShapeFrames[i].end();
            Alembic::Tako::SubDReaderPtr subDRead =
                boost::get< Alembic::Tako::SubDReaderPtr >
                (mTransStack.back()[i]->getChild(curPos));

            // build up the property map and read the initial subD data
            if (i == 0)
            {
                unsigned int readHint = subDRead->read(*fstart);

                if (readHint == Alembic::Tako::SubDReader::READ_ERROR)
                    throw std::logic_error("Error reading subD mesh");

                int faceVaryingInterpolateBoundary =
                    subDRead->getFaceVaryingInterpolateBoundary();
                if (faceVaryingInterpolateBoundary != -1)
                {
                    subDWrite.writeFaceVaryingInterpolateBoundary(
                        faceVaryingInterpolateBoundary);
                }

                int faceVaryingPropagateCorners =
                    subDRead->getFaceVaryingPropagateCorners();
                if (faceVaryingPropagateCorners != -1)
                {
                    subDWrite.writeFaceVaryingPropagateCorners(
                        faceVaryingPropagateCorners);
                }

                int interpolateBoundary = subDRead->getInterpolateBoundary();
                if (interpolateBoundary != -1)
                    subDWrite.writeInterpolateBoundary(interpolateBoundary);

                // this shape is not sampled so no need to loop over
                // the other sample sets
                if ( !subDRead->hasFrames() )
                {
                    shapesSampled = false;

                    subDWrite.start(FLT_MAX);
                    subDWrite.writeGeometry(
                        subDRead->mPoints,
                        subDRead->mFacePoints,
                        subDRead->mFaceList);

                    if (subDRead->mHolePolyIndices.size() > 0)
                    {
                        subDWrite.writeHolePolyIndices(
                            subDRead->mHolePolyIndices);
                    }
                    if (subDRead->mCreaseIndices.size() > 0)
                    {
                        subDWrite.writeCreases(
                            subDRead->mCreaseSharpness,
                            subDRead->mCreaseIndices,
                            subDRead->mCreaseLengths);
                    }
                    if (subDRead->mCornerIndices.size() > 0)
                    {
                        subDWrite.writeCorners(
                            subDRead->mCornerSharpness,
                            subDRead->mCornerIndices);
                    }
                    subDWrite.end();
                }
                else
                {
                    float geometryInitFrame = FLT_MAX;
                    float creaseInitFrame   = FLT_MAX;
                    float cornerInitFrame   = FLT_MAX;
                    float holeInitFrame     = FLT_MAX;

                    if (readHint &
                        Alembic::Tako::SubDReader::GEOMETRY_HOMOGENOUS)
                    {
                        geometryInitFrame = *fstart;
                    }

                    points      = subDRead->mPoints;
                    facePoints  = subDRead->mFacePoints;
                    faceList    = subDRead->mFaceList;

                    subDWrite.start(geometryInitFrame);
                    subDWrite.writeGeometry(
                        subDRead->mPoints,
                        subDRead->mFacePoints,
                        subDRead->mFaceList);
                    subDWrite.end();

                    if (readHint &
                        Alembic::Tako::SubDReader::CREASES_HOMOGENOUS)
                    {
                        creaseInitFrame = *fstart;
                    }

                    creaseSharpness = subDRead->mCreaseSharpness;
                    creaseIndices   = subDRead->mCreaseIndices;
                    creaseLengths   = subDRead->mCreaseLengths;

                    subDWrite.start(creaseInitFrame);
                    if (subDRead->mCreaseIndices.size() > 0)
                    {
                        subDWrite.writeCreases(
                            subDRead->mCreaseSharpness,
                            subDRead->mCreaseIndices,
                            subDRead->mCreaseLengths);
                    }
                    subDWrite.end();

                    if (readHint & 
                        Alembic::Tako::SubDReader::CORNERS_HOMOGENOUS)
                    {
                       cornerInitFrame = *fstart;
                    }

                    cornerSharpness = subDRead->mCornerSharpness;
                    cornerIndices   = subDRead->mCornerIndices;

                    subDWrite.start(cornerInitFrame);
                    if (subDRead->mCornerIndices.size() > 0)
                    {
                        subDWrite.writeCorners(
                            subDRead->mCornerSharpness,
                            subDRead->mCornerIndices);
                    }
                    subDWrite.end();

                    if (readHint &
                        Alembic::Tako::SubDReader::HOLEPOLY_HETEROGENOUS)
                    {
                        holeInitFrame = *fstart;
                    }

                    subDWrite.start(holeInitFrame);
                    if (subDRead->mHolePolyIndices.size() > 0)
                    {
                        subDWrite.writeHolePolyIndices(
                            subDRead->mHolePolyIndices);
                    }
                    subDWrite.end();
                }

                // set up the property map
                initProperties(*fstart, propSampled, *subDRead, subDWrite);

                // increment because we've done the setup
                fstart++;

                if (!shapesSampled && !propSampled)
                    i = numSets;
            }

            if (shapesSampled)
            {
                for (std::set<float>::iterator f = fstart; f != fend; ++f)
                {
                    unsigned int readHint = subDRead->read(*f);

                    if (readHint == Alembic::Tako::SubDReader::READ_ERROR)
                    {
                        throw std::logic_error(
                            "Error reading subD mesh sample");
                    }
                    else
                    {
                        subDWrite.start(*f);

                        // geo
                        if (readHint !=
                            Alembic::Tako::SubDReader::GEOMETRY_STATIC)
                        {
                            if ((f == fstart && points.size() ==
                                subDRead->mPoints.size()
                                && subDRead->mFacePoints == facePoints
                                && subDRead->mFaceList == faceList)
                                || (f != fstart && readHint &
                                Alembic::Tako::SubDReader::GEOMETRY_HOMOGENOUS))
                            {
                                subDWrite.writeGeometry(subDRead->mPoints);
                            }
                            else
                            {
                                points = subDRead->mPoints;
                                facePoints = subDRead->mFacePoints;
                                faceList = subDRead->mFaceList;

                                subDWrite.writeGeometry(
                                    subDRead->mPoints,
                                    subDRead->mFacePoints,
                                    subDRead->mFaceList);
                            }
                        }

                        // crease
                        if (!readHint &&
                            !(readHint &
                            Alembic::Tako::SubDReader::CREASES_STATIC))
                        {
                            if ((f == fstart && creaseSharpness.size() ==
                                subDRead->mCreaseSharpness.size()
                                && creaseIndices == subDRead->mCreaseIndices
                                && creaseLengths == subDRead->mCreaseLengths
                                ) || (f != fstart && readHint &
                                Alembic::Tako::SubDReader::CREASES_HOMOGENOUS))
                            {
                                subDWrite.writeCreases(
                                    subDRead->mCreaseSharpness);
                            }
                            else
                            {
                                creaseSharpness = subDRead->mCreaseSharpness;
                                creaseIndices   = subDRead->mCreaseIndices;
                                creaseLengths   = subDRead->mCreaseLengths;

                                subDWrite.writeCreases(
                                    subDRead->mCreaseSharpness,
                                    subDRead->mCreaseIndices,
                                    subDRead->mCreaseLengths);
                            }
                        }

                        // corner
                        if (!readHint && !(readHint &
                            Alembic::Tako::SubDReader::CORNERS_STATIC))
                        {
                            if ((f == fstart && cornerSharpness.size() ==
                                subDRead->mCornerSharpness.size() &&
                                cornerIndices == subDRead->mCornerIndices) ||
                                (f != fstart && readHint &
                                Alembic::Tako::SubDReader::CORNERS_HOMOGENOUS))
                            {
                                subDWrite.writeCorners(
                                    subDRead->mCornerSharpness);
                            }
                            else
                            {
                                cornerSharpness = subDRead->mCornerSharpness;
                                cornerIndices   = subDRead->mCornerIndices;

                                subDWrite.writeCorners(
                                    subDRead->mCornerSharpness,
                                    subDRead->mCornerIndices);
                            }
                        }

                        // hole
                        if (readHint & 
                            Alembic::Tako::SubDReader::HOLEPOLY_HETEROGENOUS)
                        {
                            subDWrite.writeHolePolyIndices(
                                subDRead->mHolePolyIndices);
                        }

                        subDWrite.end();
                    }
                }
            }

            if (propSampled)
                updateSampledProperties(fstart, fend, *subDRead, subDWrite);
        }
    };

    void operator() (Alembic::Tako::PolyMeshReaderPtr & iNode)
    {
        size_t numSets = mShapeFrames.size();

        bool shapesSampled  = true;
        bool propSampled    = false;

        Alembic::Tako::PolyMeshWriter polyWrite(
            iNode->getName(), *(mWriteParents.back()));
        size_t curPos = mCurPos.back();

        // keep a copy of the topology around so we can compare at the start
        // of each file, to make sure topology didn't change between files
        std::vector<float>      points;
        std::vector<float>      normals;
        std::vector<index_t>  facePoints;
        std::vector<index_t>  faceList;

        for (size_t i = 0; i < numSets; ++i)
        {
            std::set<float>::iterator fstart = mShapeFrames[i].begin();
            std::set<float>::iterator fend = mShapeFrames[i].end();
            Alembic::Tako::PolyMeshReaderPtr polyRead =
                boost::get< Alembic::Tako::PolyMeshReaderPtr >
                (mTransStack.back()[i]->getChild(curPos));

            // build up the property map and read the initial poly data
            if (i == 0)
            {
                Alembic::Tako::PolyMeshReader::TopologyType readHint =
                    polyRead->read(*fstart);

                if (readHint == Alembic::Tako::PolyMeshReader::READ_ERROR)
                    throw std::logic_error("Error reading polymesh");

                float initFrame = *fstart;

                // this transform is not sampled so no need to loop over
                // the other sample sets
                if (readHint == Alembic::Tako::PolyMeshReader::TOPOLOGY_STATIC)
                {
                    shapesSampled = false;
                    initFrame = FLT_MAX;
                }
                else
                {
                    // copy topology so we can check the boundary between
                    // the different files
                    points = polyRead->mPoints;
                    normals = polyRead->mNormals;
                    facePoints = polyRead->mFacePoints;
                    faceList = polyRead->mFaceList;
                }

                polyWrite.write(initFrame,
                    polyRead->mPoints,
                    polyRead->mNormals,
                    polyRead->mFacePoints,
                    polyRead->mFaceList);

                // set up the property map
                initProperties(*fstart, propSampled, *polyRead, polyWrite);

                // increment because we have done the setup
                fstart++;

                if (!shapesSampled && !propSampled)
                    i = numSets;
            }
            // array is built up, just write out any samples

            if (shapesSampled)
            {
                for (std::set<float>::iterator f = fstart; f != fend; ++f)
                {
                    Alembic::Tako::PolyMeshReader::TopologyType readHint =
                        polyRead->read(*f);
                    if (readHint == Alembic::Tako::PolyMeshReader::READ_ERROR)
                    {
                        throw std::logic_error("Error reading polymesh sample");
                    }
                    // we first need to check to make sure that this is the
                    // first frame for this file, if it is check to make sure
                    // the topology matches the last time the topology has
                    // changed before trying to write it as homogenous

                    // If we aren't at the first frame and the frame is
                    // homogenous then we write it as homogenous as well
                    else if ((f == fstart
                        && points.size()  == polyRead->mPoints.size()
                        && normals.size() == polyRead->mNormals.size()
                        && facePoints == polyRead->mFacePoints
                        && faceList == polyRead->mFaceList)
                        || (f != fstart && readHint ==
                        Alembic::Tako::PolyMeshReader::TOPOLOGY_HOMOGENOUS))
                    {
                        polyWrite.write(*f,
                            polyRead->mPoints, polyRead->mNormals);
                    }
                    // we know the topology has changed
                    else
                    {
                        // copy topology so we can check the boundary between
                        // the different files
                        points = polyRead->mPoints;
                        normals = polyRead->mNormals;
                        facePoints = polyRead->mFacePoints;
                        faceList = polyRead->mFaceList;
                        polyWrite.write(*f,
                            polyRead->mPoints,
                            polyRead->mNormals,
                            polyRead->mFacePoints,
                            polyRead->mFaceList);
                    }
                }
            }

            if (propSampled)
                updateSampledProperties(fstart, fend, *polyRead, polyWrite);
        }  // for each num set
    };

    // iNode, readerPtrs, transWrite are the "same" nodes in the scene
    void operator() (Alembic::Tako::TransformReaderPtr & iNode)
    {
        std::vector< Alembic::Tako::TransformReaderPtr > readerPtrs;

        std::vector< Alembic::Tako::TransformReaderPtr >::iterator t =
            mTransStack.back().begin();
        std::vector< Alembic::Tako::TransformReaderPtr >::iterator tend =
            mTransStack.back().end();

        size_t curPos = mCurPos.back();
        for (; t != tend; ++t)
        {
            Alembic::Tako::TransformReaderPtr child =
                boost::get< Alembic::Tako::TransformReaderPtr >
                ((*t)->getChild(curPos));
            readerPtrs.push_back(child);
        }

        TransformWriterPtr transWrite(new Alembic::Tako::TransformWriter(
            iNode->getName(), *(mWriteParents.back())));

        // ================= copy the transform data ==========================

        std::vector<double> samples;

        // its easier to assume that the transform data is initially sampled
        // and that the properties are not
        bool transSampled = true;
        bool propSampled = false;

        // numSets is the number of hdf files to be merged together
        size_t numSets = mTransFrames.size();
        for (size_t i = 0; i < numSets; ++i)
        {
            std::set<float>::iterator fstart = mTransFrames[i].begin();
            std::set<float>::iterator fend = mTransFrames[i].end();

            Alembic::Tako::TransformReaderPtr transRead = readerPtrs[i];

            // build up the property map and transform operation stack
            if (i == 0)
            {
                // we wont be stitching together curves at this time
                Alembic::Tako::TransformReader::SampleType readHint =
                    transRead->read(*fstart);

                // this transform is not sampled so no need to loop over
                // the other sample sets
                if (readHint != Alembic::Tako::TransformReader::SAMPLES_READ)
                    transSampled = false;

                Alembic::Tako::TransformReader::const_stack_iterator op, opEnd;
                op = transRead->begin();
                opEnd = transRead->end();
                for (; op != opEnd; ++op)
                {
                    transWrite->push(*op);
                }
                transWrite->writeTransformStack(*fstart,
                    transRead->inheritsTransform());

                // set up the property map
                initProperties(*fstart, propSampled, *transRead, *transWrite);

                // we increment this since when writing out the transform stack
                // and properties we also write out any sampled data
                fstart++;

                // no sampled data of any kind, dont loop through any
                // more of the frames
                if (!transSampled && !propSampled)
                    i = numSets;
            }

            // array is built up, just write out any samples
            if (transSampled)
            {
                for (std::set<float>::iterator f = fstart; f != fend; ++f)
                {
                    transRead->getSamples(*f, samples);
                    transWrite->writeSamples(*f, samples);
                }
            }

            if (propSampled)
                updateSampledProperties(fstart, fend, *transRead, *transWrite);
        }  // for each num set


        // data copying is over, now traverse down the hierarchy and
        // apply the visitor to all the child nodes

        mTransStack.push_back(readerPtrs);
        mWriteParents.push_back(transWrite);
        mCurPos.push_back(0);

        size_t numChildren = iNode->numChildren();
        for (; mCurPos.back() < numChildren; ++mCurPos.back())
        {
            Alembic::Tako::ChildNodePtr child = iNode->getChild(mCurPos.back());
            boost::apply_visitor(*this, child);
        }

        // we are past the recursive part, pop stuff off our stacks
        mCurPos.pop_back();
        mWriteParents.pop_back();
        mTransStack.pop_back();
    };

    void incr() {  mCurPos.back()++; }

    private:
        // each set is the framerange of one hdf file
        std::vector< std::set<float> > mShapeFrames;
        std::vector< std::set<float> > mTransFrames;
        // stack:
        // the last element is the parent of the current node being written
        std::vector< TransformWriterPtr > mWriteParents;
        // stack:
        // the ith element is the list of reader pointers that have the same
        // position in the scene hierarchy as the current node being written
        std::vector< std::vector< Alembic::Tako::TransformReaderPtr > > 
            mTransStack;

        // stack: the ith element is the index of child currently
        // being visited at the ith level of the scene hierarchy
        std::vector <size_t> mCurPos;
};

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: tako_stitcher outFile inFile1 inFile2 (inFile3 ....)\n");
        printf("Used to combine 2 or more HDF Scene Graph files into 1 file\n");
        return 0;
    }

    int i;

    std::vector< Alembic::Tako::TransformReaderPtr > roots;
    for (i = 2; i < argc; ++i)
    {
        Alembic::Tako::TransformReaderPtr t(
            new Alembic::Tako::TransformReader(argv[i]));

        if (t->numChildren())
        {
            roots.push_back(t);
        }
        else
        {
            printf("Error: %s not valid\n", argv[i]);
            return 1;
        }
    }

    ReadWriteVisitor mVisitor(argv[1], roots);

    Alembic::Tako::TransformReaderPtr root = roots[0];
    for (unsigned int i = 0; i < root->numChildren(); i++)
    {
        Alembic::Tako::ChildNodePtr child = root->getChild(i);
        boost::apply_visitor(mVisitor, child);
        mVisitor.incr();
    }

    return 0;
}
