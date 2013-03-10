//-*****************************************************************************
//
// Copyright (c) 2013,
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

#include <Alembic/AbcCoreOgawa/ReadUtil.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// NON-PUBLICLY VISIBLE HELPERS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void
ReadDimensions( Ogawa::IGroupPtr iGroup,
                size_t iIndex,
                size_t iThreadId,
                const AbcA::DataType &iDataType,
                Util::Dimensions & oDim )
{
    // find it based on of the size of the data
    if ( iGroup->isEmptyChildData( iIndex ) )
    {
        IGroup::IDataPtr data = iGroup->getData( iIndex + 1, iThreadId );
        oDim = Util::Dimensions( ( data->getSize() - 16 ) /
                                 iDataType.getNumBytes() );
    }
    else
    {
        // we need to read our dimensions
        IGroup::IDataPtr data = iGroup->getData( iIndex, iThreadId );

        // we write them as uint32_t so / 4
        std::size_t numRanks = data->getSize() / 4;

        oDim.setRank( numRanks );

        std::vector< uint32_t > dims( numRanks );
        data->read( numRanks * 4, &( dims.front() ) );
        for ( std::size_t i = 0; i < numRanks; ++i )
        {
            oDim[i] = dims[i];
        }
    }
}

//-*****************************************************************************
void
ReadData( void * iIntoLocation,
          Ogawa::IGroupPtr iGroup,
          size_t iIndex,
          size_t iThreadId,
          const AbcA::DataType &iDataType,
          Util::PlainOldDataType iAsPod )
{
    PlainOldDataType curPod = iDataType.getPod();
    ABCA_ASSERT( ( iAsPod != kStringPOD && iAsPod != kWstringPOD &&
        curPod != kStringPOD && curPod != kWstringPOD )
        || ( iAsPod == curPod ),
        "Cannot convert the data to or from a string, wstring." );

    Ogawa::IDataPtr data = iGroup->getData( iIndex );
    std::size_t dataSize = data->getSize();
    if ( dataSize <= 16 )
    {
        ABCA_ASSERT( dataSize == 0,
            "Incorrect data, expected to be empty or to have a key and data");
        return;
    }

    if ( curPod == kStringPOD )
    {
        std::string * strPtr =
            reinterpret_cast< std::string * > ( iIntoLocation );

        std::size_t numChars = dataSize - 16;
        char * buf = new char[ numChars ];
        data->read( numChars, buf, 16, iThreadId );

        std::size_t startStr = 0;
        std::size_t strPos = 0;

        for ( std::size_t i = 0; i < numChars; ++i )
        {
            if ( buf[i] == 0 )
            {
                strPtr[strPos] = buf + startStr;
                startStr = i + 1;
                strPos ++;
            }
        }

        delete [] buf;
    }
    else if ( curPod == kWStringPOD )
    {
        std::size_t numChars = ( dataSize - 16 ) / 4;
        uint32_t * buf = new uint32_t[ numChars ];
        data->read( dataSize - 16, buf, 16, iThreadId );

        std::size_t startStr = 0;
        std::size_t strPos = 0;

        for ( std::size_t i = 0; i < numChars; ++i )
        {
            if ( buf[i] == 0 )
            {
                strPtr[strPos] = buf + startStr;
                startStr = i + 1;
                strPos ++;
            }
        }

        delete [] buf;
    }
    else if ( iAsPod == curPod )
    {
        // don't read the key
        data->read( dataSize - 16, iIntoLocation, 16, iThreadId );
    }
    else
    {
        // read into a temporary buffer and cast them one at a time
        char * buf = new char[ dataSize - 16 ];
        data->read( dataSize - 16, buf, 16, iThreadId );
        size_t numToCast = ( dataSize - 16 ) / PODNumBytes( curPod );

        PODTraitsFromEnum< curPod > FromPod;
        PODTraitsFromEnum< iAsPod > ToPod;

        FromPod::value_type * fromBuf =
            reinterpret_cast< FromPod::value_type * > ( buf );

        ToPod::value_type * toBuf =
            reinterpret_cast< ToPod::value_type * > ( iIntoLocation );

        for ( size_t i = 0; i < numToCast; ++i )
        {
            toBuf[i] = static_cast< ToPod::value_type > ( fromBuf[i] );
        }
        delete [] buf;
    }

}

//-*****************************************************************************
void
ReadTimeSamplesAndMax( Ogawa::IDataPtr iData,
                       std::vector <  AbcA::TimeSamplingPtr > & oTimeSamples,
                       std::vector <  AbcA::index_t > & oMaxSamples )
{
    std::vector< char > buf( iData->getSize() );
    iData->read( iData->getSize(), &( buf.front() ) );
    std::size_t pos = 0;
    while ( pos < buf.size() )
    {
        uint32_t maxSample = *( (uint32_t *)( &buf[pos] ) );
        pos += 4;

        oMaxSamples.push_back( maxSample );

        chrono_t tpc = *( ( chrono_t * )( &buf[pos] ) );
        pos += sizeof( chrono_t );

        uint32_t numSamples = *( (uint32_t *)( &buf[pos] ) );
        pos += 4;

        std::vector< chrono_t > sampleTimes( numSamples );
        memcpy( &( sampleTimes.front() ), &buf[pos],
                sizeof( chrono_t ) * numSamples );
        pos += sizeof( chrono_t ) * numSamples;

        TimeSamplingType tst( AbcA::kAcyclic );
        if ( tpc != AcyclicTimePerCycle() )
        {
            tst = AbcA::TimeSamplingType( numSamples, tpc );
        }

        AbcA::TimeSamplingPtr tptr( new TimeSampling( tst, sampleTimes ) );
        oTimeSamples.push_back( tptr );
    }
}

//-*****************************************************************************
void
ReadObjectHeaders( Ogawa::IGroupPtr iGroup,
                   size_t iIndex,
                   size_t iThreadId,
                   const std::string & iParentName,
                   std::vector< ObjectHeaderPtr > & oHeaders )
{
    std::vector< char > buf( iData->getSize() );
    iData->read( iData->getSize(), &( buf.front() ), iThreadId );
    std::size_t pos = 0;
    while ( pos < buf.size() )
    {
        uint32_t nameSize = *( (uint32_t *)( &buf[pos] ) );
        pos += 4;

        std::string name( &buf[pos], nameSize );
        pos += nameSize;

        uint32_t metaDataSize = *( (uint32_t *)( &buf[pos] ) );
        pos += 4;

        std::string metaData( &buf[pos], metaDataSize );
        pos += metaDataSize;

        AbcA::MetaData md;
        md.deserialize( metaData );
        ObjectHeaderPtr objPtr( new ObjectHeader() );
        objPtr->setName( name );
        objPtr->setFullName( iParentName + "/" + name );
        objPtr->getMetaData().deserialize( metaData );
        oHeaders.push_back( objPtr );
    }
}

//-*****************************************************************************
void
ReadPropertyHeaders( Ogawa::IGroupPtr iGroup,
                     size_t iIndex,
                     size_t iThreadId,
                     AbcA::ArchiveReaderPtr & iArchive,
                     PropertyHeaderPtrs & oHeaders )
{
    // 0000 0000 0000 0000 0000 0000 0000 0011
    static const uint32_t ptypeMask = 0x0003;

    // 0000 0000 0000 0000 0000 0000 0011 1100
    static const uint32_t podMask = 0x003c;

    // 0000 0000 0000 0000 0000 0000 0100 0000
    static const uint32_t hasTsidxMask = 0x0040;

    // 0000 0000 0000 0000 0000 0000 1000 0000
    static const uint32_t noRepeatsMask = 0x0080;

    // 0000 0000 0000 0000 1111 1111 0000 0000
    static const uint32_t extentMask = 0xff00;

    // 0000 0000 0000 0001 0000 0000 0000 0000
    static const uint32_t homogenousMask = 0x10000;

    std::vector< char > buf( iData->getSize() );
    iData->read( iData->getSize(), &( buf.front() ), iThreadId );
    std::size_t pos = 0;
    while ( pos < buf.size() )
    {
        PropertyHeaderPtr header( new PropertyHeaderAndFriends() );
        uint32_t info =  *( (uint32_t *)( &buf[pos] ) );
        pos += 4;

        uint32_t ptype = info & ptypeMask;
        header->isScalarLike = ptype & 1;
        if ( ptype == 0 )
        {
            header->header.setPropertyType( AbcA::kCompoundProperty );
        }
        else if ( ptype == 1 )
        {
            header->header.setPropertyType( AbcA::kScalarProperty );
        }
        else
        {
            header->header.setPropertyType( AbcA::kArrayProperty );
        }

        // if we aren't a compound we may need to do a bunch of other work
        if ( ptype > 0 )
        {
            // Read the pod type out of bits 2-5
            char podt = ( char )( ( info[0] & podMask ) >> 2 );
            if ( podt != ( char )kBooleanPOD &&
                 podt != ( char )kUint8POD &&
                 podt != ( char )kInt8POD &&
                 podt != ( char )kUint16POD &&
                 podt != ( char )kInt16POD &&
                 podt != ( char )kUint32POD &&
                 podt != ( char )kInt32POD &&
                 podt != ( char )kUint64POD &&
                 podt != ( char )kInt64POD &&
                 podt != ( char )kFloat16POD &&
                 podt != ( char )kFloat32POD &&
                 podt != ( char )kFloat64POD &&
                 podt != ( char )kStringPOD &&
                 podt != ( char )kWstringPOD )
            {
                ABCA_THROW( "Read invalid POD type: " << ( int )podt );
            }

            uint8_t extent = ( info & extentMask ) >> 8;
            header->header.setDataType( AbcA::DataType(
                ( Util::PlainOldDataType ) podt, extent ) );

            header->isHomogenous = ( info & homogenousMask ) != 0;

            header->nextSampleIndex =  *( (uint32_t *)( &buf[pos] ) );
            pos += 4;

            if ( ( info & noRepeatsMask ) == 0 )
            {
                header->firstChangedIndex =  *( (uint32_t *)( &buf[pos] ) );
                pos += 4;

                header->lastChangedIndex =  *( (uint32_t *)( &buf[pos] ) );
                pos += 4;
            }
            else
            {
                header->firstChangedIndex = 1;
                header->lastChangedIndex = header->nextSampleIndex - 1;
            }

            if ( ( info & hasTsidxMask ) != 0 )
            {
                header->timeSamplingIndex =  *( (uint32_t *)( &buf[pos] ) );
                header->header.setTimeSampling(
                    iArchive->getTimeSampling( header->timeSamplingIndex ) );
                pos += 4;
            }
        }

        uint32_t nameSize = *( (uint32_t *)( &buf[pos] ) );
        pos += 4;

        std::string name( &buf[pos], nameSize );
        header->header.setName( name );
        pos += nameSize;

        uint32_t metaDataSize = *( (uint32_t *)( &buf[pos] ) );
        pos += 4;

        std::string metaData( &buf[pos], metaDataSize );
        pos += metaDataSize;

        AbcA::MetaData md;
        md.deserialize( metaData );
        oHeaders.push_back( objPtr );
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
