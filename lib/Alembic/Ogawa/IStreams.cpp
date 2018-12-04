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


#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))

    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #include <cstring>

#elif defined(_WIN32)

    #include <windows.h>
    #include <fcntl.h>
    #include <io.h>
    #include <sys/stat.h>

#else
    #error Platform not supported.
#endif




namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS
{

namespace
{

class IStreamReader
{
public:
    virtual ~IStreamReader() {}

    virtual std::size_t numStreams() const = 0;

    virtual bool isOpen() const = 0;

    virtual bool read(std::size_t iThreadId, Alembic::Util::uint64_t iPos,
                      Alembic::Util::uint64_t iSize, void* oBuf) = 0;
};

typedef Alembic::Util::shared_ptr<IStreamReader> IStreamReaderPtr;


class StdIStreamReader : public IStreamReader
{
public:
    StdIStreamReader(const std::vector<std::istream*>& iStreams) : streams(
        iStreams)
    {
        locks = new Alembic::Util::mutex[streams.size()];

        // preserve the initial position of these streams
        offsets.reserve(streams.size());
        for (size_t i = 0; i < streams.size(); i++)
        {
            offsets.push_back(streams[i]->tellg());
        }
    }

    ~StdIStreamReader()
    {
        delete[] locks;
    }

    size_t numStreams() const
    {
        return streams.size();
    }

    bool isOpen() const
    {
        return !streams.empty();
    }

    bool read(std::size_t iTheadId, Alembic::Util::uint64_t iPos,
              Alembic::Util::uint64_t iSize, void* oBuf)
    {
        std::size_t streamIndex = 0;
        if (iTheadId < streams.size())
        {
            streamIndex = iTheadId;
        }

        Alembic::Util::scoped_lock l(locks[streamIndex]);
        std::istream* stream = streams[streamIndex];

        stream->seekg(iPos + offsets[streamIndex]);
        if (!stream->good()) return false;

        stream->read(static_cast<char*>(oBuf), iSize);
        if (!stream->good()) return false;

        return true;
    }

private:
    std::vector<std::istream*> streams;
    std::vector<Alembic::Util::uint64_t> offsets;
    Alembic::Util::mutex* locks;
};


class FileIStreamReader : public IStreamReader
{
private:

// Platform support functions for file access
#ifdef _WIN32
    typedef int FileDescriptor;

    static FileDescriptor openFile(const char * iFileName,
                            Alembic::Util::int32_t iFlag)
    {
        FileDescriptor fid = -1;

        // One way to prevent writing over a file opened for reading would be to
        // pass in _SH_DENYWR instead of _SH_DENYNO.  If we can find a posix
        // equivalent we may have an interesting solution for that problem.
        _sopen_s(&fid, iFileName, iFlag | _O_RANDOM, _SH_DENYNO, _S_IREAD);
        return fid;
    }

    static void closeFile(FileDescriptor iFid)
    {
        if (iFid > -1)
        {
            _close(iFid);
        }
    }

    static bool readFile(FileDescriptor iFid,
                  void * oBuf,
                  Alembic::Util::uint64_t iOffset,
                  Alembic::Util::uint64_t iSize)
	{
        void * buf = oBuf;
        Alembic::Util::uint64_t offset = iOffset;
        Alembic::Util::uint64_t totalRead = 0;

        HANDLE hFile = reinterpret_cast<HANDLE>(_get_osfhandle(iFid));
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
                return false;
            }
            totalRead += numRead;
            offset += numRead;
            buf = static_cast< char * >( buf ) + numRead;
        }
        while(numRead > 0 && totalRead < iSize);

        // if we couldn't read what we needed to then something went wrong
        if (totalRead < iSize)
        {
            return false;
        }

        return true;
    }

#else
    typedef int FileDescriptor;

    static FileDescriptor
    openFile(const char* iFileName, Alembic::Util::int32_t iFlag)
    {
        return open(iFileName, iFlag);
    }

    static void closeFile(FileDescriptor iFid)
    {
        if (iFid > -1)
        {
            close(iFid);
        }
    }

    static bool
    readFile(FileDescriptor iFid, void* oBuf, Alembic::Util::uint64_t iOffset,
             Alembic::Util::uint64_t iSize)
    {
        Alembic::Util::uint64_t totalRead = 0;
        void* buf = oBuf;
        off_t offset = iOffset;

        ssize_t numRead = 0;
        do
        {
            Alembic::Util::uint64_t readCount = iSize - totalRead;
            // if over 1 GB read it 1 GB chunk at a time to accomodate OSX
            if (readCount > 1073741824)
            {
                readCount = 1073741824;
            }
            numRead = pread(iFid, buf, readCount, offset);
            if (numRead > 0)
            {
                totalRead += numRead;
                offset += numRead;
                buf = static_cast< char* >( buf ) + numRead;
            }

            if (numRead < 0 && errno != EINTR)
            {
                return false;
            }
        } while (numRead > 0 && totalRead < iSize);

        // if we couldn't read what we needed to then something went wrong
        if (totalRead < iSize)
        {
            return false;
        }

        return true;
    }

#endif

public:
    FileIStreamReader(const std::string& iFileName, std::size_t iNumStreams)
        : nstreams(iNumStreams)
    {
        fid = openFile(iFileName.c_str(), O_RDONLY);

        // don't check the return value here
        // IStream::init() will check isOpen
    }

    ~FileIStreamReader()
    {
        closeFile(fid);
    }

    size_t numStreams() const
    {
        return nstreams;
    }

    bool isOpen() const
    {
        return (fid > -1);
    }

    bool read(std::size_t /*iTheadId*/, Alembic::Util::uint64_t iPos,
              Alembic::Util::uint64_t iSize, void* oBuf)
    {
        // Ignore the iThread. There's no need to lock.
        if (!isOpen()) return false;

        return readFile(fid, oBuf, iPos, iSize);
    }

private:
    FileDescriptor fid;
    size_t nstreams;
};



class MemoryMappedIStreamReader : public IStreamReader
{
private:

#ifndef _WIN32
    typedef int FileHandle;
    #define BAD_FILE_HANDLE (-1)

    static FileHandle openFile(const std::string& iFileName)
    {
        int err = open(iFileName.c_str(), O_RDONLY);
        return err < 0 ? BAD_FILE_HANDLE : err;
    }

    static void closeFile(FileHandle iFile)
    {
        if (iFile != BAD_FILE_HANDLE)
        {
            close(iFile);
        }
    }

    static int getFileLength(FileHandle iFile, size_t& oLength)
    {
        struct stat buf;

        int err = fstat(iFile, &buf);
        if (err < 0) return -1;
        if (buf.st_size < 0) return -1;

        oLength = static_cast<size_t>(buf.st_size);
        return 0;
    }

    struct MappedRegion
    {
        size_t len;
        void* p;

        MappedRegion() : len(0), p(NULL)
        {
        }

        ~MappedRegion()
        {
            close();
        }

        bool isMapped() const
        {
            return p != NULL;
        }

        void map(FileHandle iFile, size_t iLength)
        {
            close();

            void* m = mmap(NULL, iLength, PROT_READ, MAP_PRIVATE, iFile, 0);
            if (m == MAP_FAILED) return;

            p = m;
            len = iLength;
        }

        void close()
        {
            if (p)
            {
                munmap(p, len);
                p = NULL;
            }
        }

    };

#else // _WIN32 defined
    typedef HANDLE FileHandle;
    #define BAD_FILE_HANDLE (INVALID_HANDLE_VALUE)

    static FileHandle openFile(const std::string& iFileName)
    {
        // Use both FILE_SHARE_READ and FILE_SHARE_WRITE as the share mode.
        // Without FILE_SHARE_WRITE, this will fail when trying to open a file that is already open for writing.
        return CreateFile(iFileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    static void closeFile(FileHandle iFile)
    {
        if (iFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(iFile);
        }
    }

    static int getFileLength(FileHandle iFile, size_t& oLength)
    {
        LARGE_INTEGER length;
        length.QuadPart = 0;

        BOOL success = GetFileSizeEx(iFile, &length);
        if (!success) return -1;
        if (length.QuadPart < 0) return -1;

        oLength = static_cast<size_t>(length.QuadPart);
        return 0;
    }

    struct MappedRegion
    {
        size_t len;
        void* p;

        MappedRegion() : len(0), p(NULL)
        {
        }

        ~MappedRegion()
        {
            close();
        }

        bool isMapped() const
        {
            return p != NULL;
        }

        void map(FileHandle iFile, size_t iLength)
        {
            close();

            DWORD sizeHigh = static_cast<DWORD>(iLength >> 32);
            DWORD sizeLow = static_cast<DWORD>(iLength);
            HANDLE mapping = CreateFileMapping(iFile, NULL, PAGE_READONLY, sizeHigh, sizeLow, NULL);
            if (mapping == NULL) return;

            LPVOID view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, iLength);

            // regardless of whether we have a successful view, close the mapping
            // the underlying file mapping will remain open as long as the view is open
            CloseHandle(mapping);

            if (view != NULL)
            {
                p = view;
                len = iLength;
            }
        }

        void close()
        {
            if (p)
            {
                UnmapViewOfFile(p);
                p = NULL;
            }
        }

    };
#endif


public:
    MemoryMappedIStreamReader(const std::string& iFileName,
                              std::size_t iNumStreams)
        : nstreams(iNumStreams), fileName(iFileName),
          fileHandle(BAD_FILE_HANDLE)
    {
        fileHandle = openFile(iFileName);
        if (fileHandle == BAD_FILE_HANDLE) return;

        size_t len = 0;
        int err = getFileLength(fileHandle, len);
        if (err < 0) return;

        mappedRegion.map(fileHandle, len);
    }

    ~MemoryMappedIStreamReader()
    {
        mappedRegion.close();
        closeFile(fileHandle);
    }

    bool isOpen() const
    {
        return mappedRegion.isMapped();
    }

    size_t numStreams() const
    {
        // memory mapped files support 'unlimited' streams, but just report
        // the number of streams we were opened with
        return nstreams;
    }

    bool read(std::size_t iStream, Alembic::Util::uint64_t iPos,
              Alembic::Util::uint64_t iSize, void* oBuf)
    {
        if (iPos + iSize > mappedRegion.len) return false;

        const char* p = static_cast<const char*>(mappedRegion.p) + iPos;
        std::memcpy(oBuf, p, iSize);

        return true;
    }

private:
    std::size_t nstreams;
    std::string fileName;
    FileHandle fileHandle;
    MappedRegion mappedRegion;
};


IStreamReaderPtr constructStreamReader(
    const std::string & iFileName,
    std::size_t iNumStreams,
    bool iUseMMap)
{
    // if allowed by the options, use memory mapped file access
    if (iUseMMap)
    {
        return IStreamReaderPtr(
            new MemoryMappedIStreamReader(iFileName, iNumStreams));
    }

    // otherwise, use file streams
    return IStreamReaderPtr(new FileIStreamReader(iFileName, iNumStreams));
}

IStreamReaderPtr constructStreamReader(
    const std::vector< std::istream * > & iStreams)
{
    // This construction method only supports the std::istream reader
    return IStreamReaderPtr(new StdIStreamReader(iStreams));
}


}  // anonymous namespace



class IStreams::PrivateData
{
public:
    PrivateData()
    {
        valid = false;
        frozen = false;
        version = 0;
    }

    void init(IStreamReaderPtr iReader, size_t iNumStreams)
    {
        // simple temporary endian check
        union
        {
            Util::uint32_t l;
            char c[4];
        } u;

        u.l = 0x01234567;

        if (u.c[0] != 0x67)
        {
            throw std::runtime_error(
                "Ogawa currently only supports little-endian reading.");
        }

        if (iNumStreams == 0 || iReader == NULL || !iReader->isOpen()) return;

        Alembic::Util::uint64_t firstGroupPos = 0;

        for (std::size_t i = 0; i < iNumStreams; ++i)
        {
            char header[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

            iReader->read(i, 0, 16, static_cast<void*>(header));
            std::string magicStr(header, 5);
            if (magicStr != "Ogawa")
            {
                frozen = false;
                valid = false;
                version = 0;
                return;
            }
            bool filefrozen = (header[5] == char(0xff));
            Alembic::Util::uint16_t fileversion = (header[6] << 8) | header[7];
            Alembic::Util::uint64_t groupPos = *((Alembic::Util::uint64_t*) (&(header[8])));

            if (i == 0)
            {
                firstGroupPos = groupPos;
                frozen = filefrozen;
                version = fileversion;
            }
                // all the streams have to agree, or we are invalid
            else if (firstGroupPos != groupPos || frozen != filefrozen ||
                     version != fileversion)
            {
                frozen = false;
                valid = false;
                version = 0;
                return;
            }
        }

        // if we reach here, and we're a known version, then we're valid
        if (version == 1)
        {
            reader = iReader;        // preserve the reader
            valid = true;
        }
    }


    bool valid;
    bool frozen;
    Alembic::Util::uint16_t version;

    IStreamReaderPtr reader;
};


namespace
{
}  // anonymous namespace




IStreams::IStreams(const std::string & iFileName, std::size_t iNumStreams,
                   bool iUseMMap) :
    mData(new IStreams::PrivateData())
{
    IStreamReaderPtr reader = constructStreamReader(iFileName, iNumStreams,
                                                    iUseMMap);
    mData->init(reader, 1);
}

IStreams::IStreams(const std::vector< std::istream * > & iStreams) :
    mData(new IStreams::PrivateData())
{
    IStreamReaderPtr reader = constructStreamReader(iStreams);
    mData->init(reader, reader->numStreams());
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

    bool success = mData->reader->read(iThreadId, iPos, iSize, oBuf);
    if (!success)
    {
        throw std::runtime_error(
            "Ogawa IStreams::read failed.");
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Ogawa
} // End namespace Alembic
