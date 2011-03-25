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

#ifndef _AlembicExport_MayaTransformWriter_h_
#define _AlembicExport_MayaTransformWriter_h_

#include "Foundation.h"

#include <Alembic/AbcGeom/OXform.h>
#include <Alembic/AbcGeom/XformOp.h>

#include "AttributesWriter.h"

// Writes an MFnTransform
class MayaTransformWriter
{
  public:

    MayaTransformWriter(double iFrame, Alembic::Abc::OObject & iParent,
        MDagPath & iDag, 
        Alembic::AbcCoreAbstract::v1::TimeSamplingType & iTimeType,
        bool addWorld, bool iWriteVisibility);

    MayaTransformWriter(double iFrame, MayaTransformWriter & iParent,
        MDagPath & iDag,
        Alembic::AbcCoreAbstract::v1::TimeSamplingType & iTimeType,
        bool iWriteVisibility);

    ~MayaTransformWriter();
    void write(double iFrame);
    bool isAnimated() const;
    Alembic::Abc::OObject getObject() {return mSchema.getObject();};
    AttributesWriterPtr getAttrs() {return mAttrs;};

  private:

    Alembic::AbcGeom::OXformSchema mSchema;
    AttributesWriterPtr mAttrs;
    size_t mCurIndex;

    void pushTransformStack(double iFrame, const MFnTransform & iTrans,
        Alembic::AbcGeom::XformOpVec & oOpVec,
        std::vector<double> & oStatic, std::vector<double> & oAnim);

    void pushTransformStack(double iFrame, const MFnIkJoint & iTrans,
        Alembic::AbcGeom::XformOpVec & oOpVec,
        std::vector<double> & oStatic, std::vector<double> & oAnim);

    // list of plugs to get as doubles, the bool indicates
    // whether we need to flip the value for an inverse evaluation
    std::vector < std::pair < MPlug, bool > > mSampledList;
};

typedef boost::shared_ptr < MayaTransformWriter > MayaTransformWriterPtr;

#endif  // _AlembicExport_MayaTransformWriter_h_
