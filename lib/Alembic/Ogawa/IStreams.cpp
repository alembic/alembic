//-*****************************************************************************
//
// Copyright (c) 2013,
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <Alembic/Ogawa/IStreams.h>
#include <fstream>
#include <stdexcept>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

class IStreams::PrivateData
{
public:
    PrivateData()
    {
        locks = NULL;
        valid = false;
        frozen = false;
        version = 0;
    }

    ~PrivateData()
    {
        if (locks)
        {
            delete [] locks;
        }

        // only cleanup if we were the ones who opened it
        if (!fileName.empty())
        {
            std::vector<std::istream *>::iterator it;
            for (it = streams.begin(); it != streams.end(); ++it)
            {
                std::ifstream * filestream = dynamic_cast<std::ifstream *>(*it);
                if (filestream)
                {
                    filestream->close();
                    delete filestream;
                }
            }
        }
    }

    std::vector<std::istream *> streams;
    std::vector<Alembic::Util::uint64_t> offsets;
    Alembic::Util::mutex * locks;
    std::string fileName;
    bool valid;
    bool frozen;
    Alembic::Util::uint16_t version;
};

IStreams::IStreams(const std::string & iFileName, std::size_t iNumStreams) :
    mData(new IStreams::PrivateData())
{

    std::ifstream * filestream = new std::ifstream;
    filestream->open(iFileName.c_str(), std::ios::binary);

    if (filestream->is_open())
    {
        mData->fileName = iFileName;
    }
    else
    {
        delete filestream;
        return;
    }

    mData->streams.push_back(filestream);
    init();
    if (!mData->valid || mData->version != 1)
    {
        mData->streams.clear();
        filestream->close();
        delete filestream;
    }
    else
    {
        // we are valid, so we'll allocate (but not open) the others
        mData->streams.resize(iNumStreams, NULL);
        mData->offsets.resize(iNumStreams, 0);
    }
    mData->locks = new Alembic::Util::mutex[mData->streams.size()];
}

IStreams::IStreams(const std::vector< std::istream * > & iStreams) :
    mData(new IStreams::PrivateData())
{
    mData->streams = iStreams;
    init();
    if (!mData->valid || mData->version != 1)
    {
        mData->streams.clear();
        return;
    }

    mData->locks = new Alembic::Util::mutex[mData->streams.size()];
}

void IStreams::init()
{
    // simple temporary endian check
    union {
        Util::uint32_t l;
        char c[4];
    } u;

    u.l = 0x01234567;

    if (u.c[0] != 0x67)
    {
        throw std::runtime_error(
            "Ogawa currently only supports little-endian reading.");
    }

    if (mData->streams.empty())
    {
        return;
    }

    Alembic::Util::uint64_t firstGroupPos = 0;

    for (std::size_t i = 0; i < mData->streams.size(); ++i)
    {
        char header[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        mData->offsets.push_back(mData->streams[i]->tellg());
        mData->streams[i]->read(header, 16);
        std::string magicStr(header, 5);
        if (magicStr != "Ogawa")
        {
            mData->frozen = false;
            mData->valid = false;
            mData->version = 0;
            return;
        }
        bool frozen = (header[5] == char(0xff));
        Alembic::Util::uint16_t version = (header[6] << 8) | header[7];
        Alembic::Util::uint64_t groupPos =
            *((Alembic::Util::uint64_t *)(&(header[8])));

        if (i == 0)
        {
            firstGroupPos = groupPos;
            mData->frozen = frozen;
            mData->version = version;
        }
        // all the streams have to agree, or we are invalid
        else if (firstGroupPos != groupPos || mData->frozen != frozen ||
                 mData->version != version)
        {
            mData->frozen = false;
            mData->valid = false;
            mData->version = 0;
            return;
        }
    }
    mData->valid = true;
}

IStreams::~IStreams()
{
}

bool IStreams::isValid()
{
    return mData->valid;
}

bool IStreams::isFrozen()
{
    return mData->frozen;
}

Alembic::Util::uint16_t IStreams::getVersion()
{
    return mData->version;
}

void IStreams::read(std::size_t iThreadId, Alembic::Util::uint64_t iPos,
                    Alembic::Util::uint64_t iSize, void * oBuf)
{
    if (!isValid())
    {
        return;
    }

    std::size_t threadId = 0;
    if (iThreadId < mData->streams.size())
    {
        threadId = iThreadId;
    }

    {
        Alembic::Util::scoped_lock l(mData->locks[threadId]);
        std::istream * stream = mData->streams[threadId];

        // the file hasn't been opened for this id yet
        if (stream == NULL && !mData->fileName.empty())
        {
            std::ifstream * filestream = new std::ifstream;
            filestream->open(mData->fileName.c_str(), std::ios::binary);

            if (filestream->is_open())
            {
                stream = filestream;
                mData->streams[threadId] = filestream;
                mData->offsets[threadId] = filestream->tellg();
            }
            // couldnt open the file, do cleanup
            else
            {
                delete filestream;

                // read from thread 0 instead until it can be opened
                if (threadId != 0)
                {
                    read(0, iPos, iSize, oBuf);
                }
                return;
            }
        }
        stream->seekg(iPos + mData->offsets[threadId]);
        stream->read((char *)oBuf, iSize);
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
