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

#include <Alembic/AbcCoreOgawa/WriteUtil.h>
#include <Alembic/AbcCoreOgawa/AwImpl.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
void pushUint32WithHint( std::vector< uint8_t > & ioData,
                         uint32_t iVal, uint32_t iHint )
{
    uint8_t * data = ( uint8_t * ) &iVal;
    if ( iHint == 0)
    {
        ioData.push_back( data[0] );
    }
    else if ( iHint == 1 )
    {
        ioData.push_back( data[0] );
        ioData.push_back( data[1] );
    }
    else if ( iHint == 2 )
    {
        ioData.push_back( data[0] );
        ioData.push_back( data[1] );
        ioData.push_back( data[2] );
        ioData.push_back( data[3] );
    }
}

//-*****************************************************************************
void pushChrono( std::vector< uint8_t > & ioData, chrono_t iVal )
{
    uint8_t * data = ( uint8_t * ) &iVal;

    for ( std::size_t i = 0; i < sizeof( chrono_t ); ++i )
    {
        ioData.push_back( data[i] );
    }
}


//-*****************************************************************************
WrittenSampleMap &
GetWrittenSampleMap( AbcA::ArchiveWriterPtr iVal )
{
    AwImpl *ptr = dynamic_cast<AwImpl*>( iVal.get() );
    ABCA_ASSERT( ptr, "NULL Impl Ptr" );
    return ptr->getWrittenSampleMap();
}

//-*****************************************************************************
void WriteDimensions( Ogawa::OGroupPtr iGroup,
                      const AbcA::Dimensions & iDims,
                      Alembic::Util::PlainOldDataType iPod )
{

    size_t rank = iDims.rank();

    if ( iPod != Alembic::Util::kStringPOD &&
         iPod != Alembic::Util::kWstringPOD &&
         rank == 1 )
    {
        // we can figure out the dimensions based on the size  of the data
        // so just set empty data.
        iGroup->addEmptyData();
        return;
    }

    // Create temporary storage to write
    std::vector<uint32_t> dimStorage( rank );

    // Copy into it.
    for ( size_t r = 0; r < rank; ++r )
    {
        dimStorage[r] = ( uint32_t ) iDims[r];
    }

    iGroup->addData( rank * sizeof( uint32_t ),
                     ( const void * )&dimStorage.front() );
}

//-*****************************************************************************
WrittenSampleIDPtr
WriteData( WrittenSampleMap &iMap,
           Ogawa::OGroupPtr iGroup,
           const AbcA::ArraySample &iSamp,
           const AbcA::ArraySample::Key &iKey )
{

    // Okay, need to actually store it.
    // Write out the hash id, and the data together

    const AbcA::Dimensions & dims = iSamp.getDimensions();

    // See whether or not we've already stored this.
    WrittenSampleIDPtr writeID = iMap.find( iKey );
    if ( writeID )
    {
        CopyWrittenData( iGroup, writeID );
        return writeID;
    }

    Ogawa::ODataPtr dataPtr;

    const AbcA::DataType &dataType = iSamp.getDataType();

    if ( dataType.getPod() == Alembic::Util::kStringPOD )
    {
        size_t numPods = dataType.getExtent() * dims.numPoints();
        std::vector <int8_t> v;
        for ( size_t j = 0; j < numPods; ++j )
        {
            const std::string &str =
                static_cast<const std::string*>( iSamp.getData() )[j];

            ABCA_ASSERT( str.find( '\0' ) == std::string::npos,
                     "Illegal NULL character found in string data " );

            size_t strLen = str.length();
            for ( size_t k = 0; k < strLen; ++k )
            {
                v.push_back(str[k]);
            }

            // append a 0 for the NULL seperator character
            v.push_back(0);
        }

        const void * datas[2] = { &iKey.digest, &v.front() };
        std::size_t sizes[2] = { 16, v.size() };
        dataPtr =  iGroup->addData( 2, sizes, datas );
    }
    else if ( dataType.getPod() == Alembic::Util::kWstringPOD )
    {
        size_t numPods = dataType.getExtent() * dims.numPoints();
        std::vector <int32_t> v;
        for ( size_t j = 0; j < numPods; ++j )
        {
            const std::wstring &str =
                static_cast<const std::wstring*>( iSamp.getData() )[j];

            wchar_t nullChar = 0;
            ABCA_ASSERT( str.find( nullChar ) == std::wstring::npos,
                     "Illegal NULL character found in wstring data" );

            size_t strLen = str.length();
            for ( size_t k = 0; k < strLen; ++k )
            {
                v.push_back(str[k]);
            }

            // append a 0 for the NULL seperator character
            v.push_back(0);
        }

        const void * datas[2] = { &iKey.digest, &v.front() };
        std::size_t sizes[2] = { 16, v.size() * sizeof(int32_t) };
        dataPtr =  iGroup->addData( 2, sizes, datas );
    }
    else
    {
        const void * datas[2] = { &iKey.digest, iSamp.getData() };
        std::size_t sizes[2] = { 16, iKey.numBytes };

        dataPtr = iGroup->addData( 2, sizes, datas );
    }

    writeID.reset( new WrittenSampleID( iKey, dataPtr,
                        dataType.getExtent() * dims.numPoints() ) );
    iMap.store( writeID );

    // Return the reference.
    return writeID;
}

//-*****************************************************************************
void CopyWrittenData( Ogawa::OGroupPtr iGroup,
                      WrittenSampleIDPtr iRef )
{
    ABCA_ASSERT( ( bool )iRef,
                  "CopyWrittenData() passed a bogus ref" );

    ABCA_ASSERT( iGroup,
                "CopyWrittenData() passed in a bogus OGroupPtr" );

    iGroup->addData(iRef->getObjectLocation());
}

//-*****************************************************************************
void WritePropertyInfo( std::vector< uint8_t > & ioData,
                    const AbcA::PropertyHeader &iHeader,
                    bool isScalarLike,
                    bool isHomogenous,
                    uint32_t iTimeSamplingIndex,
                    uint32_t iNumSamples,
                    uint32_t iFirstChangedIndex,
                    uint32_t iLastChangedIndex,
                    MetaDataMapPtr iMap )
{

    uint32_t info = 0;

    // 0000 0000 0000 0000 0000 0000 0000 0011
    static const uint32_t ptypeMask = 0x0003;

    // 0000 0000 0000 0000 0000 0000 0000 1100
    static const uint32_t sizeHintMask = 0x000c;

    // 0000 0000 0000 0000 0000 0000 1111 0000
    static const uint32_t podMask = 0x00f0;

    // 0000 0000 0000 0000 0000 0001 0000 0000
    static const uint32_t hasTsidxMask = 0x0100;

    // 0000 0000 0000 0000 0000 0010 0000 0000
    static const uint32_t needsFirstLastMask = 0x0200;

    // 0000 0000 0000 0000 0000 0100 0000 0000
    static const uint32_t homogenousMask = 0x400;

    // 0000 0000 0000 0000 0000 1000 0000 0000
    static const uint32_t constantMask = 0x800;

    // 0000 0000 0000 1111 1111 0000 0000 0000
    static const uint32_t extentMask = 0xff000;

    // 0000 1111 1111 0000 0000 0000 0000 0000
    static const uint32_t metaDataIndexMask = 0xff00000;

    std::string metaData = iHeader.getMetaData().serialize();
    uint32_t metaDataSize = metaData.size();

    // keep track of the longest thing for byteSizeHint and so we don't
    // have to use 4 byte sizes if we don't need it
    uint32_t maxSize = metaDataSize;

    uint32_t nameSize = iHeader.getName().size();
    if ( maxSize < nameSize )
    {
        maxSize = nameSize;
    }

    if ( maxSize < iNumSamples )
    {
        maxSize = iNumSamples;
    }

    if ( maxSize < iTimeSamplingIndex )
    {
        maxSize = iTimeSamplingIndex;
    }

    // size hint helps determine how many bytes are enough to store the
    // various string size, num samples and time sample indices
    // 0 for 1 byte
    // 1 for 2 bytes
    // 2 for 4 bytes
    uint32_t sizeHint = 0;
    if ( maxSize > 255 && maxSize < 65536 )
    {
        sizeHint = 1;
    }
    else if ( maxSize >= 65536)
    {
        sizeHint = 2;
    }

    info |= sizeHintMask & ( sizeHint << 2 );

    uint32_t metaDataIndex = iMap->getIndex( metaData );

    info |= metaDataIndexMask & ( metaDataIndex << 20 );

    // compounds are treated differently
    if ( !iHeader.isCompound() )
    {
        // Slam the property type in there.
        info |= ptypeMask & ( uint32_t )iHeader.getPropertyType();

        // arrays may be scalar like, scalars are already scalar like
        info |= ( uint32_t ) isScalarLike;

        uint32_t pod = ( uint32_t )iHeader.getDataType().getPod();
        info |= podMask & ( pod << 4 );

        if (iTimeSamplingIndex != 0)
        {
            info |= hasTsidxMask;
        }

        bool needsFirstLast = false;

        if (iFirstChangedIndex == 0 && iLastChangedIndex == 0)
        {
            info |= constantMask;
        }
        else if (iFirstChangedIndex != 1 ||
                 iLastChangedIndex != iNumSamples - 1)
        {
            info |= needsFirstLastMask;
            needsFirstLast = true;
        }

        uint32_t extent = ( uint32_t )iHeader.getDataType().getExtent();
        info |= extentMask & ( extent << 12 );

        if ( isHomogenous )
        {
            info |= homogenousMask;
        }

        ABCA_ASSERT( iFirstChangedIndex <= iNumSamples &&
            iLastChangedIndex <= iNumSamples &&
            iFirstChangedIndex <= iLastChangedIndex,
            "Illegal Sampling!" << std::endl <<
            "Num Samples: " << iNumSamples << std::endl <<
            "First Changed Index: " << iFirstChangedIndex << std::endl <<
            "Last Changed Index: " << iLastChangedIndex << std::endl );

        // info is always 4 bytes so use hint 2
        pushUint32WithHint( ioData, info, 2 );

        pushUint32WithHint( ioData, iNumSamples, sizeHint );

        // don't bother writing out first and last change if every sample
        // was different
        if ( needsFirstLast )
        {
            pushUint32WithHint( ioData, iFirstChangedIndex, sizeHint );
            pushUint32WithHint( ioData, iLastChangedIndex, sizeHint );
        }

        // finally set time sampling index on the end if necessary
        if (iTimeSamplingIndex != 0)
        {
            pushUint32WithHint( ioData, iTimeSamplingIndex, sizeHint );
        }

    }
    // compound, shove in just info (hint is 2)
    else
    {
        pushUint32WithHint( ioData, info, 2 );
    }

    pushUint32WithHint( ioData, nameSize, sizeHint );

    ioData.insert( ioData.end(), iHeader.getName().begin(),
                   iHeader.getName().end() );

    if ( metaDataIndex == 0xff )
    {
        pushUint32WithHint( ioData, metaDataSize, sizeHint );

        if ( metaDataSize )
        {
            ioData.insert( ioData.end(), metaData.begin(), metaData.end() );
        }
    }

}

//-*****************************************************************************
void WriteObjectHeader( std::vector< uint8_t > & ioData,
                    const AbcA::ObjectHeader &iHeader,
                    MetaDataMapPtr iMap )
{
    uint32_t nameSize = iHeader.getName().size();
    pushUint32WithHint( ioData, nameSize, 2 );
    ioData.insert( ioData.end(), iHeader.getName().begin(),
                   iHeader.getName().end() );

    std::string metaData = iHeader.getMetaData().serialize();
    uint32_t metaDataSize = (uint32_t) metaData.size();

    uint32_t metaDataIndex = iMap->getIndex( metaData );

    // write 1 byte for the meta data index
    pushUint32WithHint( ioData, metaDataIndex, 0 );

    // write the size and meta data IF necessary
    if ( metaDataIndex == 0xff )
    {
        pushUint32WithHint( ioData, metaDataSize, 2 );
        if ( metaDataSize )
        {
            ioData.insert( ioData.end(), metaData.begin(), metaData.end() );
        }
    }
}

//-*****************************************************************************
void WriteTimeSampling( std::vector< uint8_t > & ioData,
                    uint32_t  iMaxSample,
                    const AbcA::TimeSampling &iTsmp )
{
    pushUint32WithHint( ioData, iMaxSample, 2 );

    AbcA::TimeSamplingType tst = iTsmp.getTimeSamplingType();

    chrono_t tpc = tst.getTimePerCycle();

    pushChrono( ioData, tpc );

    const std::vector < chrono_t > & samps = iTsmp.getStoredTimes();
    ABCA_ASSERT( samps.size() > 0, "No TimeSamples to write!");

    uint32_t spc = (uint32_t) samps.size();

    pushUint32WithHint( ioData, spc, 2 );

    for ( std::size_t i = 0; i < samps.size(); ++i )
    {
        pushChrono( ioData, samps[i] );
    }

}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
