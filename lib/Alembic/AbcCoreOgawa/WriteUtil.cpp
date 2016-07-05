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
void pushUint32WithHint( std::vector< Util::uint8_t > & ioData,
                         Util::uint32_t iVal, Util::uint32_t iHint )
{
    Util::uint8_t * data = ( Util::uint8_t * ) &iVal;
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
void pushChrono( std::vector< Util::uint8_t > & ioData, chrono_t iVal )
{
    Util::uint8_t * data = ( Util::uint8_t * ) &iVal;

    for ( std::size_t i = 0; i < sizeof( chrono_t ); ++i )
    {
        ioData.push_back( data[i] );
    }
}

//-*****************************************************************************
void HashPropertyHeader( const AbcA::PropertyHeader & iHeader,
                         Util::SpookyHash & ioHash )
{

    // turn our header into something to add to our hash
    std::vector< Util::uint8_t > data;

    data.insert( data.end(), iHeader.getName().begin(),
                 iHeader.getName().end() );

    std::string metaData = iHeader.getMetaData().serialize();
    data.insert( data.end(), metaData.begin(), metaData.end() );

    if ( !iHeader.isCompound() )
    {
        data.push_back( ( Util::uint8_t )iHeader.getDataType().getPod() );
        data.push_back( iHeader.getDataType().getExtent() );

        // toss this in to differentiate between array and scalar props that
        // have no samples
        if ( iHeader.isScalar() )
        {
            data.push_back( 0 );
        }

        AbcA::TimeSamplingPtr ts = iHeader.getTimeSampling();
        AbcA::TimeSamplingType tst = ts->getTimeSamplingType();

        chrono_t tpc = tst.getTimePerCycle();

        pushChrono( data, tpc );

        const std::vector < chrono_t > & samps = ts->getStoredTimes();

        Util::uint32_t spc = ( Util::uint32_t ) samps.size();

        pushUint32WithHint( data, spc, 2 );

        for ( std::size_t i = 0; i < samps.size(); ++i )
        {
            pushChrono( data, samps[i] );
        }
    }

    if ( !data.empty() )
    {
        ioHash.Update( &data.front(), data.size() );
    }
}

//-*****************************************************************************
void HashDimensions( const AbcA::Dimensions & iDims,
                     Util::Digest & ioHash )
{
    size_t rank = iDims.rank();

    if ( rank > 0 )
    {
        Util::SpookyHash hash;
        hash.Init(0, 0);

        hash.Update( iDims.rootPtr(), rank * 8 );
        hash.Update( ioHash.d, 16 );

        Util::uint64_t hash0, hash1;
        hash.Final( &hash0, &hash1 );
        ioHash.words[0] = hash0;
        ioHash.words[1] = hash1;
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
         rank <= 1 )
    {
        // we can figure out the dimensions based on the size  of the data
        // so just set empty data.
        iGroup->addEmptyData();
        return;
    }

    iGroup->addData( rank * sizeof( Util::uint64_t ),
                     ( const void * )iDims.rootPtr() );
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
        std::vector <Util::int8_t> v;
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
        Alembic::Util::uint64_t sizes[2] = { 16, v.size() };
        dataPtr =  iGroup->addData( 2, sizes, datas );
    }
    else if ( dataType.getPod() == Alembic::Util::kWstringPOD )
    {
        size_t numPods = dataType.getExtent() * dims.numPoints();
        std::vector <Util::int32_t> v;
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
        Alembic::Util::uint64_t sizes[2] = { 16,
            v.size() * sizeof(Util::int32_t) };
        dataPtr =  iGroup->addData( 2, sizes, datas );
    }
    else
    {
        const void * datas[2] = { &iKey.digest, iSamp.getData() };
        Alembic::Util::uint64_t sizes[2] = { 16, iKey.numBytes };

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
void WritePropertyInfo( std::vector< Util::uint8_t > & ioData,
                    const AbcA::PropertyHeader &iHeader,
                    bool isScalarLike,
                    bool isHomogenous,
                    Util::uint32_t iTimeSamplingIndex,
                    Util::uint32_t iNumSamples,
                    Util::uint32_t iFirstChangedIndex,
                    Util::uint32_t iLastChangedIndex,
                    MetaDataMapPtr iMap )
{

    Util::uint32_t info = 0;

    // Our bitmasks look like this:
    //
    // Property Type mask (Scalar, Array, or Compound) 0x0003
    // 0000 0000 0000 0000 0000 0000 0000 0011
    //
    // Our Pod type mask 0x003c
    // 0000 0000 0000 0000 0000 0000 0011 1100
    //
    // Has a time sampling index mask 0x0040
    // 0000 0000 0000 0000 0000 0000 0100 0000
    //
    // no repeats mask 0x0080
    // 0000 0000 0000 0000 0000 0000 1000 0000
    //
    // Extent value mask 0xff00
    // 0000 0000 0000 0000 1111 1111 0000 0000
    //
    // Our bitmasks look like this:
    //
    // Property Type mask (Scalar, Array, or Compound) 0x0003
    // 0000 0000 0000 0000 0000 0000 0000 0011
    //
    // Byte size hint for first/last non repeated sample 0x000c
    // 0000 0000 0000 0000 0000 0000 0000 1100
    //
    // Our Pod type mask 0x00f0
    // 0000 0000 0000 0000 0000 0000 1111 0000
    //
    // Has a time sampling index mask 0x0100
    // 0000 0000 0000 0000 0000 0001 0000 0000
    //
    // Whether the first/last index exists mask 0x200
    // 0000 0000 0000 0000 0000 0010 0000 0000
    //
    // Whether the size of the data changes from sample to sample mask 0x400
    // 0000 0000 0000 0000 0000 0100 0000 0000
    //
    // Mask for whether the data is the same over all samples 0x800
    // 0000 0000 0000 0000 0000 1000 0000 0000
    //
    // Extent value mask 0xff000
    // 0000 0000 0000 1111 1111 0000 0000 0000
    //
    // Meta data index mask 0xff00000
    // 0000 1111 1111 0000 0000 0000 0000 0000

    std::string metaData = iHeader.getMetaData().serialize();
    Util::uint32_t metaDataSize = metaData.size();

    // keep track of the longest thing for byteSizeHint and so we don't
    // have to use 4 byte sizes if we don't need it
    Util::uint32_t maxSize = metaDataSize;

    Util::uint32_t nameSize = iHeader.getName().size();
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
    Util::uint32_t sizeHint = 0;
    if ( maxSize > 255 && maxSize < 65536 )
    {
        sizeHint = 1;
    }
    else if ( maxSize >= 65536)
    {
        sizeHint = 2;
    }

    info |= 0x000c & ( sizeHint << 2 );

    Util::uint32_t metaDataIndex = iMap->getIndex( metaData );

    info |= 0xff00000 & ( metaDataIndex << 20 );

    // compounds are treated differently
    if ( !iHeader.isCompound() )
    {
        // Slam the property type in there.
        info |= 0x0003 & ( Util::uint32_t )iHeader.getPropertyType();

        // arrays may be scalar like, scalars are already scalar like
        info |= ( Util::uint32_t ) isScalarLike;

        Util::uint32_t pod = ( Util::uint32_t )iHeader.getDataType().getPod();
        info |= 0x00f0 & ( pod << 4 );

        if (iTimeSamplingIndex != 0)
        {
            info |= 0x0100;
        }

        bool needsFirstLast = false;

        if (iFirstChangedIndex == 0 && iLastChangedIndex == 0)
        {
            info |= 0x800;
        }
        else if (iFirstChangedIndex != 1 ||
                 iLastChangedIndex != iNumSamples - 1)
        {
            info |= 0x0200;
            needsFirstLast = true;
        }

        Util::uint32_t extent =
            ( Util::uint32_t )iHeader.getDataType().getExtent();
        info |= 0xff000 & ( extent << 12 );

        if ( isHomogenous )
        {
            info |= 0x400;
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
void WriteObjectHeader( std::vector< Util::uint8_t > & ioData,
                    const AbcA::ObjectHeader &iHeader,
                    MetaDataMapPtr iMap )
{
    Util::uint32_t nameSize = iHeader.getName().size();
    pushUint32WithHint( ioData, nameSize, 2 );
    ioData.insert( ioData.end(), iHeader.getName().begin(),
                   iHeader.getName().end() );

    std::string metaData = iHeader.getMetaData().serialize();
    Util::uint32_t metaDataSize = ( Util::uint32_t ) metaData.size();

    Util::uint32_t metaDataIndex = iMap->getIndex( metaData );

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
void WriteTimeSampling( std::vector< Util::uint8_t > & ioData,
                    Util::uint32_t  iMaxSample,
                    const AbcA::TimeSampling &iTsmp )
{
    pushUint32WithHint( ioData, iMaxSample, 2 );

    AbcA::TimeSamplingType tst = iTsmp.getTimeSamplingType();

    chrono_t tpc = tst.getTimePerCycle();

    pushChrono( ioData, tpc );

    const std::vector < chrono_t > & samps = iTsmp.getStoredTimes();
    ABCA_ASSERT( samps.size() > 0, "No TimeSamples to write!");

    Util::uint32_t spc = ( Util::uint32_t ) samps.size();

    pushUint32WithHint( ioData, spc, 2 );

    for ( std::size_t i = 0; i < samps.size(); ++i )
    {
        pushChrono( ioData, samps[i] );
    }

}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
