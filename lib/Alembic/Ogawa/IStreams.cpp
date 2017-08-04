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

#include <fcntl.h>

#ifdef _MSC_VER

#include <io.h>

Alembic::Util::int32_t OPENFILE(const char * iFileName, Alembic::Util::int32_t iFlag)
{
    Alembic::Util::int32_t fid = -1;

    // One way to prevent writing over a file opened for reading would be to pass in
    // _SH_DENYWR instead of _SH_DENYNO.  If we can find a posix equivalent we may
    // have an interesting solution for that problem.
    _sopen_s(&fid, iFileName, iFlag | _O_RANDOM, _SH_DENYNO, _S_IREAD);
    return fid;
}

void CLOSEFILE(Alembic::Util::int32_t iFid)
{
    if (iFid > -1)
    {
        _close(iFid);
    }
}

#else
#include <unistd.h>
#define OPENFILE open
#define CLOSEFILE close
#endif

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

class IStream
{
public:
    IStream(std::istream * iStream)
    {
        stream = iStream;
        offset = 0;

        if (stream != NULL)
        {
            offset = stream->tellg();
        }

        fd = -1;
        isGood = true;
    }

    IStream(Alembic::Util::int32_t iFileDescriptor)
    {
        stream = NULL;
        offset = 0;

        fd = iFileDescriptor;
        isGood = true;
    }

    void seekg(Alembic::Util::uint64_t iOffset)
    {
        if (stream)
        {
            stream->seekg(iOffset + offset);
            return;
        }

        offset = iOffset;
    }

    void read(void * oBuf, Alembic::Util::uint64_t iSize)
    {
        if (stream)
        {
            stream->read((char*)oBuf, iSize);
            return;
        }

        Alembic::Util::uint64_t totalRead = 0;
        void * buf = oBuf;

#ifdef _MSC_VER
        HANDLE hFile = reinterpret_cast<HANDLE>(_get_osfhandle(fd));

        DWORD numRead = 0;
        do
        {
            DWORD numToRead = 0;
            if ((iSize - totalRead) > MAXDWORD)
            {
                numToRead = MAXDWORD;
            }
            else
            {
                numToRead = static_cast<DWORD>(iSize - totalRead);
            }

            OVERLAPPED overlapped;
            memset( &overlapped, 0, sizeof(overlapped));
            overlapped.Offset = static_cast<DWORD>(offset);
            overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

            if (!ReadFile(hFile, buf, numToRead, &numRead, &overlapped))
            {
                isGood = false;
                return;
            }
            totalRead += numRead;
            offset += numRead;
            buf = static_cast< char * >( buf ) + numRead;
        }
        while(numRead > 0 && totalRead < iSize);
#else
        ssize_t numRead = 0;
        do
        {
            numRead = pread(fd, buf, iSize - totalRead, offset);
            if (numRead > 0)
            {
                totalRead += numRead;
                offset += numRead;
                buf = static_cast< char * >( buf ) + numRead;
            }
        }
        while(numRead > 0 && totalRead < iSize);
#endif

        // if we couldn't read what we needed to then something went wrong
        if (totalRead < iSize)
        {
            isGood = false;
        }
    }

    bool good()
    {
        if (stream)
        {
            return stream->good();
        }

        return isGood;
    }

    bool fail()
    {
        if (stream)
        {
            return stream->fail();
        }

        return !isGood;
    }

private:
    std::istream * stream;

    Alembic::Util::int32_t fd;
    Alembic::Util::uint64_t offset;
    bool isGood;
};

class IStreams::PrivateData
{
public:
    PrivateData()
    {
        locks = NULL;
        valid = false;
        frozen = false;
        version = 0;
        fid = -1;
    }

    ~PrivateData()
    {
        if (locks)
        {
            delete [] locks;
        }

        if (fid != -1)
        {
            CLOSEFILE(fid);
        }
    }

    std::vector<IStream> streams;
    std::vector<Alembic::Util::uint64_t> offsets;
    Alembic::Util::mutex * locks;
    bool valid;
    bool frozen;
    Alembic::Util::uint16_t version;

    Alembic::Util::int32_t fid;
};

IStreams::IStreams(const std::string & iFileName, std::size_t iNumStreams) :
    mData(new IStreams::PrivateData())
{

    mData->fid = OPENFILE(iFileName.c_str(), O_RDONLY);

    if (mData->fid > -1)
    {
        mData->streams.push_back(IStream(mData->fid));
    }

    init();
    if (!mData->valid || mData->version != 1)
    {
        mData->streams.clear();
        CLOSEFILE(mData->fid);
        mData->fid = -1;
    }
    else
    {
        // we are valid, so fill in the rest
        mData->streams.reserve(iNumStreams);
        for (std::size_t i = 1; i < iNumStreams; ++i)
        {
            mData->streams.push_back(IStream(mData->fid));
        }
    }

    mData->locks = new Alembic::Util::mutex[mData->streams.size()];
}

IStreams::IStreams(const std::vector< std::istream * > & iStreams) :
    mData(new IStreams::PrivateData())
{
    mData->streams.reserve(iStreams.size());
    for (std::vector< std::istream * >::const_iterator it = iStreams.begin();
         it != iStreams.end(); ++it)
    {
        mData->streams.push_back(*it);
    }
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
        mData->streams[i].read(header, 16);
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
        IStream & stream = mData->streams[threadId];
        stream.seekg(iPos);

        // make sure our seek was good and not at the end of the file before
        // trying to do the read
        if (stream.good())
        {
            stream.read(oBuf, iSize);
        }

        // if the seekg or read failed throw an exception
        if (stream.fail())
        {
            throw std::runtime_error(
                "Ogawa IStreams::read failed.");
        }
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
