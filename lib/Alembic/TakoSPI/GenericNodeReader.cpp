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

#include <Alembic/TakoSPI/GenericNodeReader.h>
#include <Alembic/TakoSPI/TransformReader.h>
#include <Alembic/TakoSPI/PrivateUtil.h>

namespace Alembic {

namespace TakoSPI {

class GenericNodeReaderIMPL::PrivateData
{
    public:
        PrivateData()
        {
            mVersion = -1;
        }
        int mVersion;
};

GenericNodeReaderIMPL::GenericNodeReaderIMPL(const std::string & iName,
                                             TransformReaderIMPL & iParent)
  : ReaderGlue<TakoAbstractV1::GenericNodeReader>(iName, iParent.getHid()),
    mData(new PrivateData())
{
    if (H5Aexists(getGroup(), "vers") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "vers", H5P_DEFAULT);
        H5Aread(aid, H5T_NATIVE_INT, &mData->mVersion);
        H5Aclose(aid);
    }

    if (H5Aexists(getGroup(), "userType") == 1)
    {
        hid_t aid = H5Aopen(getGroup(), "userType", H5P_DEFAULT);
        hsize_t strSize = H5Aget_storage_size(aid);
        mUserType.resize(strSize);
        char * charPtr = &mUserType[0];
        H5Aread(aid, H5T_NATIVE_CHAR, charPtr);
        H5Aclose(aid);
    }
}

GenericNodeReaderIMPL::~GenericNodeReaderIMPL() {}

} // End namespace TakoSPI

} // End namespace Alembic

