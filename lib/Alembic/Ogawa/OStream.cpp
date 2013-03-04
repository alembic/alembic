
#include <Ogawa/OStream.h>
#include <fstream>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class OStream::PrivateData
{
public:
    PrivateData(const std::string & iFileName) :
        stream(NULL), fileName(iFileName), startPos(0)
    {
        std::ofstream * filestream = new std::ofstream(fileName.c_str(),
            std::ios_base::trunc | std::ios_base::binary);
        if (filestream->is_open())
        {
            stream = filestream;
        }
        else
        {
            filestream->close();
            delete filestream;
        }
    }

    PrivateData(std::ostream * iStream) : stream(iStream), startPos(0)
    {
        if (stream)
        {
            startPos = stream->tellp();
        }
    }

    ~PrivateData()
    {
        // if this was done via file, try to clean it up
        if (!fileName.empty() && stream)
        {
            std::ofstream * filestream = dynamic_cast<std::ofstream *>(stream);
            if (filestream)
            {
                filestream->close();
                delete filestream;
            }
        }
    }

    std::ostream * stream;
    std::string fileName;
    uint64_t startPos;
};

OStream::OStream(const std::string & iFileName) :
    mData(new PrivateData(iFileName))
{
    init();
}

// we'll be writing from this already open stream which we don't own
OStream::OStream(std::ostream * iStream) : mData(new PrivateData(iStream))
{
    init();
}

OStream::~OStream()
{
    // write our "frozen" byte (totally done writing)
    if (isValid())
    {
        char frozen = 0xff;
        mData->stream->seekp(mData->startPos + 5).write(&frozen, 1).flush();
    }
}

bool OStream::isValid()
{
    return mData->stream != NULL;
}

void OStream::init()
{
    if (isValid())
    {
        const char header[] = {
            'O', 'g', 'a', 'w', 'a',  // special magic number
            0,       // this will be 0xff when the entire archive is done
            0, 1,    // 16 bit format version number
            0, 0, 0, 0, 0, 0, 0, 0}; // position of the first group
        mData->stream->write(header, sizeof(header)).flush();
    }
}

uint64_t OStream::getAndSeekEndPos()
{
    if (isValid())
    {
        // TODO autolock here
        uint64_t lastp = mData->stream->seekp(0, std::ios_base::end).tellp();
        return lastp - mData->startPos;
    }
    return 0;
}

void OStream::seek(uint64_t iPos)
{
    if (isValid())
    {
        // TODO autolock here
        mData->stream->seekp(iPos + mData->startPos);
    }
}

void OStream::write(const void * iBuf, std::size_t iSize)
{
    if (isValid())
    {
        // TODO autolock here
        mData->stream->write((const char *)iBuf, iSize).flush();
    }
}

}
}