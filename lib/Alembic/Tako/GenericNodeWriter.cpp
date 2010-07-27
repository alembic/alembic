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

#include <Alembic/Tako/GenericNodeWriter.h>
#include <Alembic/Tako/TransformWriter.h>
#include <Alembic/Tako/NodeTypeId.h>

namespace Alembic {

using Tako::GenericNodeWriter;
using Tako::TransformWriter;

class  Tako::GenericNodeWriter::PrivateData
{
    public:
        PrivateData()
        {
            mHasWrittenUserType = false;
        }

        bool mHasWrittenUserType;
};

GenericNodeWriter::GenericNodeWriter(const char * iName,
    TransformWriter & iParent)
    : HDFWriterNode(iName, iParent.getHid()), mData(new PrivateData())
{
    int typeVal = NODE_TYPE_GENERIC_NODE;
    H5LTset_attribute_int(mGroup, ".", "type", &typeVal, 1);

    int version = GENERIC_NODE_INITIAL_VERSION;
    H5LTset_attribute_int(mGroup, ".", "vers", &version, 1);
}

GenericNodeWriter::~GenericNodeWriter() {}


void GenericNodeWriter::write(const std::string & iUserType) const
{
    if (!mData->mHasWrittenUserType)
    {
        H5LTset_attribute_char(mGroup, ".", "userType", iUserType.c_str(),
            iUserType.size());
    }

    mData->mHasWrittenUserType = true;
}

} // End namespace Alembic
