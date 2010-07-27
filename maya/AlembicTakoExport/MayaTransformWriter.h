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

#ifndef _AlembicTakoExport_MayaTransformWriter_h_
#define _AlembicTakoExport_MayaTransformWriter_h_

#include <AlembicTakoExport/Foundation.h>
#include <AlembicTakoExport/AttributesWriter.h>
#include <AlembicTakoExport/MayaTransformWriter.h>

namespace Alembic {

typedef boost::shared_ptr < Tako::TransformWriter >
    TransformWriterPtr;

// Writes an MFnTransform
class MayaTransformWriter
{
  public:

    MayaTransformWriter(float iFrame, TransformWriterPtr iParent,
        MDagPath & iDag, bool addWorld, bool iWriteVisibility);

    MayaTransformWriter(float iFrame, MayaTransformWriter & iParent,
        MDagPath & iDag, bool iWriteVisibility);

    ~MayaTransformWriter();
    void write(float iFrame);
    bool isAnimated() const;

    TransformWriterPtr getWriter() const;

  private:

    TransformWriterPtr mWriter;
    AttributesWriterPtr mAttrs;

    void pushTransformStack(float iFrame, const MFnTransform & iTrans,
        bool writeStack);

    void pushTransformStack(float iFrame, const MFnIkJoint & iTrans,
        bool writeStack);

    void addTranslate(const MFnDependencyNode& iTrans, MString parentName,
        MString xName, MString yName, MString zName,
        Tako::Translate::TranslateType iType, bool inverse,
        bool forceStatic);

    // names need to be passed in x,y,z order, iOrder is the order to
    // use these indices
    void addRotate(const MFnDependencyNode & iTrans, MString parentName,
        const MString * iNames, const unsigned int * iOrder,
        Tako::Rotate::RotateType iType, bool forceStatic, bool forceAnimated);

    void addShear(const MFnDependencyNode & iTrans, bool forceStatic);
    void addScale(const MFnDependencyNode & iTrans, MString parentName,
        MString xName, MString yName, MString zName, bool forceStatic);

    // list of plugs to get as doubles, the bool indicates
    // whether we need to flip the value for an inverse evaluation
    std::vector < std::pair < MPlug, bool > > mSampledList;
};

typedef boost::shared_ptr < MayaTransformWriter > MayaTransformWriterPtr;

} // End namespace Alembic

#endif  // _AlembicTakoExport_MayaTransformWriter_h_
