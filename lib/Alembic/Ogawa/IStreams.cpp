
#include <Ogawa/IStreams.h>
#include <fstream>

namespace Ogawa {
namespace OGAWA_LIB_VERSION_NS {

class IStreams::PrivateData
{
public:
    PrivateData()
    {
        valid = false;
        frozen = false;
        version = 0;
    }

    ~PrivateData()
    {
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
    std::vector<uint64_t> offsets;
    std::string fileName;
    bool valid;
    bool frozen;
    uint16_t version;
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
        // we are valid, so we'll open up any others
        for (std::size_t i = 1; i < iNumStreams; ++i)
        {
            filestream = new std::ifstream;
            filestream->open(iFileName.c_str(),
                std::ios::out | std::ios::trunc | std::ios::binary);
            mData->streams.push_back(filestream);
        }
    }
}

IStreams::IStreams(const std::vector< std::istream * > & iStreams) :
    mData(new IStreams::PrivateData())
{
    mData->streams = iStreams;
    init();
}

void IStreams::init()
{
    if (mData->streams.empty())
    {
        return;
    }

    // TODO record group start pos for each stream

    uint64_t firstGroupPos = 0;

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
        uint16_t version = (header[6] << 8) | header[7];
        uint64_t groupPos = *((uint64_t *)(&(header[8])));
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

uint16_t IStreams::getVersion()
{
    return mData->version;
}

void IStreams::read(std::size_t iThreadId, uint64_t iPos,
                    std::size_t iSize, void * oBuf)
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
        // TODO autolock goes here
        mData->streams[threadId]->seekg(iPos + mData->offsets[threadId]);
        mData->streams[threadId]->read((char *)oBuf, iSize);
    }
}

}
}