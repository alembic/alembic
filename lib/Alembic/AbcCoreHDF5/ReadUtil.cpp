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

#include <Alembic/AbcCoreHDF5/ReadUtil.h>
#include <Alembic/AbcCoreHDF5/DataTypeRegistry.h>
#include <Alembic/AbcCoreHDF5/ArImpl.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>

namespace Alembic {
namespace AbcCoreHDF5 {
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
ReadScalar( hid_t iParent,
            const std::string &iAttrName,
            hid_t iFileType,
            hid_t iNativeType,
            void *oData )
{
    ABCA_ASSERT( iParent >= 0, "Invalid parent" );

    hid_t attrId = H5Aopen( iParent, iAttrName.c_str(), H5P_DEFAULT );
    ABCA_ASSERT( attrId >= 0,
                 "Couldn't open attribute named: " << iAttrName );
    AttrCloser attrCloser( attrId );

    // This is all just checking code
    {
        hid_t attrFtype = H5Aget_type( attrId );
        ABCA_ASSERT( attrFtype >= 0,
                     "Couldn't get file datatype for attribute: "
                     << iAttrName );
        DtypeCloser dtypeCloser( attrFtype );

        ABCA_ASSERT( EquivalentDatatypes( attrFtype, iFileType ),
                     "File DataType clash for scalar attribute: "
                     << iAttrName );

        hid_t attrSpace = H5Aget_space( attrId );
        ABCA_ASSERT( attrSpace >= 0,
                     "Couldn't get dataspace for attribute: " << iAttrName );
        DspaceCloser dspaceCloser( attrSpace );

        H5S_class_t attrSpaceClass = H5Sget_simple_extent_type( attrSpace );
        ABCA_ASSERT( attrSpaceClass == H5S_SCALAR,
                     "Tried to read non-scalar attribute: " << iAttrName
                     << " as scalar" );
    }

    herr_t status = H5Aread( attrId, iNativeType, oData );
    ABCA_ASSERT( status >= 0, "Couldn't read from attribute: " << iAttrName );
}

//-*****************************************************************************
void
ReadSmallArray( hid_t iParent,
                const std::string &iAttrName,
                hid_t iFileType,
                hid_t iNativeType,
                size_t iMaxElems,
                size_t &oNumElems,
                void *oData )
{
    ABCA_ASSERT( iParent >= 0, "Invalid parent" );

    hid_t attrId = H5Aopen( iParent, iAttrName.c_str(), H5P_DEFAULT );
    ABCA_ASSERT( attrId >= 0,
                 "Couldn't open attribute named: " << iAttrName );
    AttrCloser attrCloser( attrId );

    // This is all just checking code
    {
        hid_t attrFtype = H5Aget_type( attrId );
        ABCA_ASSERT( attrFtype >= 0,
                     "Couldn't get file datatype for attribute: "
                     << iAttrName );
        DtypeCloser dtypeCloser( attrFtype );

        ABCA_ASSERT( EquivalentDatatypes( attrFtype, iFileType ),
                     "File DataType clash for scalar attribute: "
                     << iAttrName );

        hid_t attrSpace = H5Aget_space( attrId );
        ABCA_ASSERT( attrSpace >= 0,
                     "Couldn't get dataspace for attribute: " << iAttrName );
        DspaceCloser dspaceCloser( attrSpace );

        H5S_class_t attrSpaceClass = H5Sget_simple_extent_type( attrSpace );
        ABCA_ASSERT( attrSpaceClass == H5S_SIMPLE,
                     "Tried to read non-simple attribute: " << iAttrName
                     << " as scalar" );

        hssize_t numPoints = H5Sget_simple_extent_npoints( attrSpace );
        ABCA_ASSERT( numPoints <= iMaxElems && numPoints > -1,
                     "Too many points in SmallArrayRead" );

        oNumElems = ( size_t )numPoints;
    }

    herr_t status = H5Aread( attrId, iNativeType, oData );
    ABCA_ASSERT( status >= 0, "Couldn't read from attribute: " << iAttrName );
}

//-*****************************************************************************
// Dimensions aren't a scalar, and thus must be read carefully.
void
ReadDimensions( hid_t iParent,
                const std::string &iAttrName,
                Dimensions &oDims )
{
    // Assume a maximum rank of 128. This is totally reasonable.
    static const size_t maxRank = 128;
    static uint32_t dimVals[128];

    size_t readRank;
    ReadSmallArray( iParent, iAttrName, H5T_STD_U32LE, H5T_NATIVE_UINT32,
                    maxRank, readRank, ( void * )dimVals );

    Dimensions retDims;
    retDims.setRank( readRank );
    for ( size_t r = 0; r < readRank; ++r )
    {
        retDims[r] = ( size_t )dimVals[r];
    }

    oDims = retDims;
}

//-*****************************************************************************
bool
ReadKey( hid_t iParent,
         const std::string &iAttrName,
         AbcA::ArraySample::Key &oKey )
{
    ABCA_ASSERT( iParent >= 0, "Invalid parent in ReadKey" );
    if ( H5Aexists( iParent, iAttrName.c_str() ) > 0 )
    {
        size_t numRead;
        ReadSmallArray( iParent, iAttrName,
                        H5T_STD_U8LE,
                        H5T_NATIVE_UINT8,
                        16,
                        numRead,
                        ( void * )&oKey.digest );
        ABCA_ASSERT( numRead == 16, "Didn't read enough key bits" );

        return true;
    }
    else
    {
        return false;
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
bool
ReadMetaData( hid_t iParent,
              const std::string &iMetaDataName,
              AbcA::MetaData &oMetaData )
{
    std::string str;
    ABCA_ASSERT( iParent >= 0, "Invalid parent in ReadMetaData" );
    if ( H5Aexists( iParent, iMetaDataName.c_str() ) > 0 )
    {
        ReadString( iParent, iMetaDataName, str );
        oMetaData.deserialize( str );
        return true;
    }
    else
    {
        oMetaData = AbcA::MetaData();
        return false;
    }
}

//-*****************************************************************************
static void
ReadPropertyAndDataType( hid_t iParent,
                         const std::string &iPADName,
                         AbcA::PropertyType &oPtype,
                         AbcA::DataType &oDtype )
{
    ABCA_ASSERT( iParent >= 0, "Invalid parent in ReadPropertyAndDataType" );

    // It is an error for this to not exist.
    ABCA_ASSERT( H5Aexists( iParent, iPADName.c_str() ) > 0,
                 "Nonexistent property type attr: " << iPADName );

    uint16_t bitField = 0;
    ReadScalar( iParent, iPADName, H5T_STD_U16LE, H5T_NATIVE_UINT16,
                ( void * )&bitField );

    static const uint16_t ptypeMask =
        ( uint16_t )BOOST_BINARY( 0000 0000 0000 0011 );
    static const uint16_t podMask =
        ( uint16_t )BOOST_BINARY( 0000 0000 0000 1111 );
    static const uint16_t extentMask =
        ( uint16_t )BOOST_BINARY( 0000 0000 1111 1111 );

    // Read the property type from the low two bits.
    char ipt = ( char )( bitField & ptypeMask );
    if ( ipt != ( char )AbcA::kScalarProperty &&
         ipt != ( char )AbcA::kArrayProperty &&
         ipt != ( char )AbcA::kCompoundProperty )
    {
        ABCA_THROW( "Read invalid property type: " << ( int )ipt );
    }

    if ( ipt == ( char )AbcA::kCompoundProperty )
    {
        // Read a magic number from the back 12 bits
        // TINY bit of version locking & synchro here.
        uint16_t magic = bitField & ~ptypeMask;
        if ( magic != COMPOUND_MAGIC )
        {
            ABCA_THROW( "Read invalid compound property type magic: "
                        << magic << ", was expecting: "
                        << COMPOUND_MAGIC );
        }

        oPtype = AbcA::kCompoundProperty;
        return;
    }
    else
    {
        // Read the pod type out of bits 2-5
        char podt = ( char )( ( bitField >> 2 ) & podMask );
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

        // Read the extent out of bits 8-15
        uint8_t extent = ( uint8_t )( ( bitField >> 8 ) & extentMask );
        if ( extent == 0 )
        {
            ABCA_THROW( "Degenerate extent 0" );
        }

        oPtype = ( AbcA::PropertyType )ipt;
        oDtype = AbcA::DataType( ( PlainOldDataType )podt, extent );
    }
}

//-*****************************************************************************
static bool
ReadTimeSamplingType( hid_t iParent,
                      const std::string &iPropName,
                      AbcA::TimeSamplingType &oTimeSamplingType )
{
    const std::string nameSPC = iPropName + ".tspc";
    const std::string nameTPC = iPropName + ".ttpc";

    ABCA_ASSERT( iParent >= 0, "Invalid parent in ReadTimeSamplingType" );

    if ( H5Aexists( iParent, nameSPC.c_str() ) > 0 )
    {
        // Read the samples per cycle. If it is 1, we don't need to read
        // time per cycle because we can assume that it is 1.0.
        // If time per cycle is other than 1.0, but the num samples is 1,
        // Alembic writes out just the time per cycle, and not the num samples.
        uint32_t spc = 0;
        ReadScalar( iParent, nameSPC,
                    H5T_STD_U32LE,
                    H5T_NATIVE_UINT32,
                    ( void * )&spc );
        ABCA_ASSERT( spc > 0, "Invalid Samples Per Cycle: " << spc );

        if ( spc == AbcA::TimeSamplingType::ACYCLIC_NUM_SAMPLES )
        {
            // Acyclic.
            oTimeSamplingType = AbcA::TimeSamplingType(
                AbcA::TimeSamplingType::kAcyclic );
            return true;
        }
        else if ( spc == 1 )
        {
            // Uniform with time per cycle == 1.0
            oTimeSamplingType = AbcA::TimeSamplingType( 1, 1.0 );
            return true;
        }
        ABCA_ASSERT( spc > 1, "Corrupt TimeSamplingType spc: " << spc );

        chrono_t tpc = 1.0;
        ABCA_ASSERT( H5Aexists( iParent, nameTPC.c_str() ) > 0,
                     "Missing time per cycle attribute: " << nameTPC );

        ReadScalar( iParent, nameTPC,
                    H5T_IEEE_F64LE,
                    H5T_NATIVE_DOUBLE,
                    ( void * )&tpc );

        ABCA_ASSERT( tpc > 0.0 && tpc <
                     AbcA::TimeSamplingType::ACYCLIC_TIME_PER_CYCLE,
                     "Invalid Time Per Cycle: " << tpc );

        // Cyclic with time per cycle
        oTimeSamplingType = AbcA::TimeSamplingType( spc, tpc );
        return true;
    }
    else if ( H5Aexists( iParent, nameTPC.c_str() ) > 0 )
    {
        // Uniform with time per cycle
        chrono_t tpc = 1.0;
        ReadScalar( iParent, nameTPC,
                    H5T_IEEE_F64LE,
                    H5T_NATIVE_DOUBLE,
                    ( void * )&tpc );

        // Uniform
        oTimeSamplingType = AbcA::TimeSamplingType( tpc );
        return true;
    }
    else
    {
        // Identity
        oTimeSamplingType = AbcA::TimeSamplingType();
        return false;
    }
}

//-*****************************************************************************
void
ReadPropertyHeader( hid_t iParent,
                    const std::string &iPropName,
                    AbcA::PropertyHeader &oHeader )
{
    // First the type.
    AbcA::PropertyType oPropertyType;
    AbcA::DataType oDataType;
    ReadPropertyAndDataType( iParent, iPropName + ".type",
                             oPropertyType, oDataType );

    // Now read the meta data.
    AbcA::MetaData oMetaData;
    ReadMetaData( iParent, iPropName + ".meta", oMetaData );

    // Lastly, the time sampling
    if ( oPropertyType != AbcA::kCompoundProperty )
    {
        AbcA::TimeSamplingType oTsType;
        ReadTimeSamplingType( iParent, iPropName, oTsType );

        oHeader = AbcA::PropertyHeader( iPropName,
                                        oPropertyType,
                                        oMetaData,
                                        oDataType,
                                        oTsType );
    }
    else
    {
        oHeader = AbcA::PropertyHeader( iPropName,
                                        oMetaData );
    }
}

//-*****************************************************************************
AbcA::ArraySamplePtr
ReadArray( AbcA::ReadArraySampleCachePtr iCache,
           hid_t iParent,
           const std::string &iName,
           const AbcA::DataType &iDataType,
           hid_t iFileType,
           hid_t iNativeType )
{
    // Dispatch string stuff.
    if ( iDataType.getPod() == kStringPOD )
    {
        return ReadStringArray( iCache, iParent, iName, iDataType );
    }
    else if ( iDataType.getPod() == kWstringPOD )
    {
        return ReadWstringArray( iCache, iParent, iName, iDataType );
    }
    assert( iDataType.getPod() != kStringPOD &&
            iDataType.getPod() != kWstringPOD );

    // Open the data set.
    hid_t dsetId = H5Dopen( iParent, iName.c_str(), H5P_DEFAULT );
    ABCA_ASSERT( dsetId >= 0, "Cannot open dataset: " << iName );
    DsetCloser dsetCloser( dsetId );

    // Read the data space.
    hid_t dspaceId = H5Dget_space( dsetId );
    ABCA_ASSERT( dspaceId >= 0, "Could not get dataspace for dataSet: "
                 << iName );
    DspaceCloser dspaceCloser( dspaceId );

    AbcA::ArraySample::Key key;
    bool foundDigest = false;

    // if we are caching, get the key and see if it is being used
    if ( iCache )
    {
        key.origPOD = iDataType.getPod();
        key.readPOD = key.origPOD;

        key.numBytes = iDataType.getNumBytes() *
            H5Sget_simple_extent_npoints( dspaceId );

        foundDigest = ReadKey( dsetId, "key", key );

        AbcA::ReadArraySampleID found = iCache->find( key );

        if ( found )
        {
            AbcA::ArraySamplePtr ret = found.getSample();
            assert( ret );
            if ( ret->getDataType() != iDataType )
            {
                ABCA_THROW( "ERROR: Read data type for dset: " << iName
                            << ": " << ret->getDataType()
                            << " does not match expected data type: "
                            << iDataType );
            }

            // Got it!
            return ret;
        }
    }

    // Okay, we haven't found it in a cache.

    // Read the data type.
    hid_t dtypeId = H5Dget_type( dsetId );
    ABCA_ASSERT( dtypeId >= 0, "Could not get datatype for dataSet: "
                 << iName );
    DtypeCloser dtypeCloser( dtypeId );

    ABCA_ASSERT( EquivalentDatatypes( iFileType, dtypeId ),
                 "File DataType clash for array dataset: "
                 << iName );

    AbcA::ArraySamplePtr ret;

    H5S_class_t dspaceClass = H5Sget_simple_extent_type( dspaceId );
    if ( dspaceClass == H5S_SIMPLE )
    {
        // Get the dimensions
        int rank = H5Sget_simple_extent_ndims( dspaceId );
        ABCA_ASSERT( rank == 1,
                     "H5Sget_simple_extent_ndims() must be 1." );

        hsize_t hdim = 0;

        rank = H5Sget_simple_extent_dims( dspaceId, &hdim, NULL );

        Dimensions dims;
        std::string dimName = iName + ".dims";
        if ( H5Aexists( iParent, dimName.c_str() ) )
        {
            ReadDimensions( iParent, dimName, dims );
        }
        else
        {
            dims.setRank(1);
            dims[0] = hdim / iDataType.getExtent();
        }

        ABCA_ASSERT( dims.numPoints() > 0,
                     "Degenerate dims in Dataset read" );

        // Create a buffer into which we shall read.
        ret = AbcA::AllocateArraySample( iDataType, dims );
        assert( ret->getData() );

        // And... read into it.
        herr_t status = H5Dread( dsetId, iNativeType,
                                 H5S_ALL, H5S_ALL, H5P_DEFAULT,
                                 const_cast<void*>( ret->getData() ) );

        ABCA_ASSERT( status >= 0, "H5Dread() failed." );
    }
    else if ( dspaceClass == H5S_NULL )
    {
        Dimensions dims;
        std::string dimName = iName + ".dims";
        if ( H5Aexists( iParent, dimName.c_str() ) )
        {
            ReadDimensions( iParent, dimName, dims );
            ABCA_ASSERT( dims.rank() > 0,
                     "Degenerate rank in Dataset read" );
            // Num points should be zero here.
            ABCA_ASSERT( dims.numPoints() == 0,
                     "Expecting zero points in dimensions" );
        }
        else
        {
            dims.setRank(1);
            dims[0] = 0;
        }

        ret = AbcA::AllocateArraySample( iDataType, dims );
    }
    else
    {
        ABCA_THROW( "Unexpected scalar dataspace encountered." );
    }

    // Store if there is a cache.
    if ( foundDigest && iCache )
    {
        AbcA::ReadArraySampleID stored = iCache->store( key, ret );
        if ( stored )
        {
            return stored.getSample();
        }
    }

    // Otherwise, just leave! ArraySamplePtr returned by AllocateArraySample
    // already has fancy-dan deleter built in.
    // I REALLY LOVE SMART PTRS.
    return ret;
}

//-*****************************************************************************
bool ReadNumSamples( hid_t iParent,
                     const std::string &iPropName,
                     bool isScalar,
                     uint32_t &oNumSamples,
                     uint32_t &oNumUniqueSamples )
{
    ABCA_ASSERT( iParent >= 0, "Invalid parent in ReadNumSamples" );

    // First, look to see whether the attribute exists in the first place.
    std::string numSamplesName = iPropName + ".nums";
    if ( H5Aexists( iParent, numSamplesName.c_str() ) > 0 )
    {
        // We have a num samples attr, read it.
        uint32_t numSamps[2];
        size_t sampsRead;
        ReadSmallArray( iParent, numSamplesName,
                        H5T_STD_U32LE,
                        H5T_NATIVE_UINT32,
                        2, sampsRead,
                        ( void * )numSamps );

        if ( sampsRead == 1 )
        {
            oNumSamples = numSamps[0];
            oNumUniqueSamples = numSamps[0];
            return true;
        }
        else
        {
            assert( sampsRead == 2 );
            oNumSamples = numSamps[0];
            oNumUniqueSamples = numSamps[1];
            return true;
        }
    }

    // If we get here, we have to infer the number of samples based
    // on whether sample0 exists or not.
    std::string samp0name = iPropName + ".smp0";
    if ( isScalar )
    {
        if ( H5Aexists( iParent, samp0name.c_str() ) > 0 )
        {
            oNumSamples = 1;
            oNumUniqueSamples = 1;
        }
        else
        {
            oNumSamples = 0;
            oNumUniqueSamples = 0;
        }
    }
    else
    {
        if ( DatasetExists( iParent, samp0name ) )
        {
            oNumSamples = 1;
            oNumUniqueSamples = 1;
        }
        else
        {
            oNumSamples = 0;
            oNumUniqueSamples = 0;
        }
    }

    return false;
}

//-*****************************************************************************
// returns whether or not it is reading the default.
AbcA::ArraySamplePtr
ReadTimeSamples( AbcA::ReadArraySampleCachePtr iCache,
                 hid_t iParent,
                 const std::string &iTimeAttrName )
{
    // Check to see if the times are stored as an attr.
    if ( H5Aexists( iParent, iTimeAttrName.c_str() ) > 0 )
    {
        // Create a buffer into which we shall read.
        AbcA::ArraySamplePtr ret =
            AbcA::AllocateArraySample( AbcA::DataType( kFloat64POD, 1 ),
                                       Dimensions( 1 ) );
        assert( ret->getData() );

        ReadScalar( iParent, iTimeAttrName,
                    H5T_IEEE_F64LE,
                    H5T_NATIVE_DOUBLE,
                    const_cast<void*>( ret->getData() ) );

        return ret;
    }
    else if ( DatasetExists( iParent, iTimeAttrName ) )
    {
        return ReadArray( iCache, iParent, iTimeAttrName,
                          AbcA::DataType( kFloat64POD, 1 ),
                          H5T_IEEE_F64LE,
                          H5T_NATIVE_DOUBLE );
    }
    else
    {
        // Create a buffer of 1, fill it with zero.
        // CJH: I'm not sure this is wise anymore.
        AbcA::ArraySamplePtr ret =
            AbcA::AllocateArraySample( AbcA::DataType( kFloat64POD, 1 ),
                                       Dimensions( 1 ) );
        assert( ret->getData() );
        chrono_t *cdata = reinterpret_cast<chrono_t*>(
            const_cast<void*>( ret->getData() ) );
        *cdata = 0.0;

        return ret;
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
