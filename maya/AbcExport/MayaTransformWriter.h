//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#ifndef AlembicExport_MayaTransformWriter_h
#define AlembicExport_MayaTransformWriter_h

#include "Foundation.h"

#include <Alembic/AbcGeom/OXform.h>
#include <Alembic/AbcGeom/XformOp.h>

#include "AttributesWriter.h"

// AnimChan contains what animated plugs to get as a double, and the helper
// info about what operation and which channel to set in mSample
struct AnimChan
{
    MPlug plug;

    // extra value to multiply the data off of the plug by, used to invert
    // certain operations, and convert radians to degrees
    double scale;

    std::size_t opNum;
    Alembic::Util::uint32_t channelNum;
};

// Writes an MFnTransform
class MayaTransformWriter
{
  public:

    MayaTransformWriter(Alembic::Abc::OObject & iParent, MDagPath & iDag,
        Alembic::Util::uint32_t iTimeIndex, const JobArgs & iArgs);

    MayaTransformWriter(MayaTransformWriter & iParent, MDagPath & iDag,
        Alembic::Util::uint32_t iTimeIndex, const JobArgs & iArgs);

    ~MayaTransformWriter();
    void write();
    bool isAnimated() const;
    Alembic::Abc::OObject getObject();
    AttributesWriterPtr getAttrs() {return mAttrs;};

  private:

    Alembic::AbcGeom::OXformSchema mSchema;
    Alembic::AbcGeom::OObject mObject;

    AttributesWriterPtr mAttrs;

    void pushTransformStack(const MFnTransform & iTrans, bool iForceStatic);

    void pushTransformStack(const MFnIkJoint & iTrans, bool iForceStatic);

    Alembic::AbcGeom::XformSample mSample;

    std::vector < AnimChan > mAnimChanList;
    MPlug mInheritsPlug;

    size_t mJointOrientOpIndex[3];
    size_t mRotateOpIndex[3];
    size_t mRotateAxisOpIndex[3];

    bool mFilterEulerRotations;
    MEulerRotation mPrevJointOrientSolution;
    MEulerRotation mPrevRotateSolution;
    MEulerRotation mPrevRotateAxisSolution;
};

typedef Alembic::Util::shared_ptr < MayaTransformWriter >
    MayaTransformWriterPtr;

#endif  // AlembicExport_MayaTransformWriter_h
