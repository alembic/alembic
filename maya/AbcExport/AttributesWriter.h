//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef maya2hdf_AttributesWriter_h
#define maya2hdf_AttributesWriter_h

#include "Foundation.h"
#include "MayaUtility.h"

// class which holds a mapping of sampled attribute names to the attribute
// plugs and will fill in the property map on various HDF nodes
class AttributesWriter
{
  public:
    // fills in the property maps for both static and sampled, and does
    // the initial write at iFrame for sampled data
    AttributesWriter(Alembic::Abc::OCompoundProperty & iArgGeom,
                     Alembic::Abc::OCompoundProperty & iUserProps,
                     Alembic::Abc::OObject & iParentObj,
                     const MFnDependencyNode & iNode,
                     Alembic::Util::uint32_t iTimeIndex,
                     const JobArgs & iArgs, bool isShape);

    ~AttributesWriter();

    void write();
    bool isAnimated();

    static bool matchFilterOrAttribs(const MPlug & iPlug,
                                     const JobArgs & iArgs,
                                     bool& userAttrOut);

    static bool hasAnyAttr(const MFnDependencyNode & iNode,
                           const JobArgs & iArgs);

  private:

    std::vector < PlugAndObjArray >     mPlugObjArrayVec;
    std::vector < PlugAndObjScalar >    mPlugObjScalarVec;

    // animated visibility plug
    PlugAndObjScalar mAnimVisibility;
};

typedef Alembic::Util::shared_ptr<AttributesWriter> AttributesWriterPtr;

#endif  // maya2hdf_AttributesWriter_h
