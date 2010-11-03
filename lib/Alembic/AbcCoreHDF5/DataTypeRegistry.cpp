//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <Alembic/AbcCoreHDF5/DataTypeRegistry.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>

namespace Alembic {
namespace AbcCoreHDF5 {

//-*****************************************************************************
using namespace AbcA;

//-*****************************************************************************
static hid_t GetNativeBoolH5T()
{
    hid_t ret = H5Tcopy( H5T_NATIVE_UINT8 );
    H5Tset_size( ret, 1 );
    H5Tset_precision( ret, 1 );
    H5Tset_sign( ret, H5T_SGN_NONE );
    H5Tset_offset( ret, 0 );
    H5Tset_pad( ret, H5T_PAD_ZERO, H5T_PAD_ZERO );
    return ret;
}

//-*****************************************************************************
static hid_t GetFileBoolH5T()
{
    hid_t ret = H5Tcopy( H5T_STD_U8LE );
    H5Tset_size( ret, 1 );
    H5Tset_precision( ret, 1 );
    H5Tset_sign( ret, H5T_SGN_NONE );
    H5Tset_offset( ret, 0 );
    H5Tset_pad( ret, H5T_PAD_ZERO, H5T_PAD_ZERO );
    return ret;
}

//-*****************************************************************************
static hid_t GetNativeHalfH5T()
{
    hid_t ret = H5Tcopy( H5T_NATIVE_FLOAT );
    H5Tset_fields( ret,
                   15,   // sign bit position
                   10,   // exponent lsb position
                   5,    // exponent size
                   0,    // mantissa lsb position
                   10 ); // mantissa size
    H5Tset_size( ret, 2 );
    return ret;
}

//-*****************************************************************************
static hid_t GetFileHalfH5T()
{
    hid_t ret = H5Tcopy( H5T_IEEE_F32LE );
    H5Tset_fields( ret,
                   15,   // sign bit position
                   10,   // exponent lsb position
                   5,    // exponent size
                   0,    // mantissa lsb position
                   10 ); // mantissa size
    H5Tset_size( ret, 2 );
    return ret;
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SUPPORT FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// In order to read or write a slab of a given DataType, we need an H5::Datatype
// for the dataset in the file, and a 'native' one for how it will be
// stored in local memory. We are not bothering with named datatypes for now.
// Though, it may prove timesaving later on.
//
// So - we need some sort of structure which allows you to take an
// AlembicAsset::DataType and come up with an H5::Datatype for it.
hid_t GetNativeH5T( const AbcA::DataType &adt, bool &oCleanUp )
{
    oCleanUp = false;

    hid_t baseDtype = -1;
    switch ( adt.getPod() )
    {
    case kBooleanPOD:
        oCleanUp = true;
        baseDtype = GetNativeBoolH5T();
        break;
    case kUint8POD:     baseDtype = H5T_NATIVE_UINT8; break;
    case kInt8POD:      baseDtype = H5T_NATIVE_INT8; break;
    case kUint16POD:    baseDtype = H5T_NATIVE_UINT16; break;
    case kInt16POD:     baseDtype = H5T_NATIVE_INT16; break;
    case kUint32POD:    baseDtype = H5T_NATIVE_UINT32; break;
    case kInt32POD:     baseDtype = H5T_NATIVE_INT32; break;
    case kUint64POD:    baseDtype = H5T_NATIVE_UINT64; break;
    case kInt64POD:     baseDtype = H5T_NATIVE_INT64; break;
    case kFloat16POD:
        oCleanUp = true;
        baseDtype = GetNativeHalfH5T();
        break;
    case kFloat32POD:   baseDtype = H5T_NATIVE_FLOAT; break;
    case kFloat64POD:   baseDtype = H5T_NATIVE_DOUBLE; break;
    default:
        ABCA_THROW( "Unsuppored POD type: " << PODName( adt.getPod() ) );
    }

    ABCA_ASSERT( baseDtype >= 0, "Bad base datatype id" );

    if ( adt.getExtent() == 1 )
    {
        // Non-arrayed. The return object is shared,
        // but it just a light envelope, id
        return baseDtype;
    }
    else
    {
        ABCA_ASSERT( adt.getExtent() != 0,
                     "Cannot create a DataType of extent 0!" );

        // Have to make an arrayed type.
        Dimensions dims( adt.getExtent() );
        HDimensions hdims( dims );
        hid_t ret = H5Tarray_create2( baseDtype, hdims.rank(),
                                      hdims.rootPtr() );
        if ( oCleanUp )
        {
            H5Tclose( baseDtype );
        }
        oCleanUp = true;
        ABCA_ASSERT( ret >= 0, "Bad array datatype id" );
        return ret;
    }
}

//-*****************************************************************************
// Same as above, only this time for the files.
// Alembic uses LittleEndian by default.
hid_t GetFileH5T( const AbcA::DataType &adt, bool &oCleanUp )
{
    oCleanUp = false;
    hid_t baseDtype = -1;
    switch ( adt.getPod() )
    {
    case kBooleanPOD:
        oCleanUp = true;
        baseDtype = GetFileBoolH5T(); break;
    case kUint8POD:     baseDtype = H5T_STD_U8LE; break;
    case kInt8POD:      baseDtype = H5T_STD_I8LE; break;
    case kUint16POD:    baseDtype = H5T_STD_U16LE; break;
    case kInt16POD:     baseDtype = H5T_STD_I16LE; break;
    case kUint32POD:    baseDtype = H5T_STD_U32LE; break;
    case kInt32POD:     baseDtype = H5T_STD_I32LE; break;
    case kUint64POD:    baseDtype = H5T_STD_U64LE; break;
    case kInt64POD:     baseDtype = H5T_STD_I64LE; break;
    case kFloat16POD:
        oCleanUp = true;
        baseDtype = GetFileHalfH5T(); break;
    case kFloat32POD:   baseDtype = H5T_IEEE_F32LE; break;
    case kFloat64POD:   baseDtype = H5T_IEEE_F64LE; break;
    default:
        ABCA_THROW( "Unsuppored POD type: " << PODName( adt.getPod() ) );
    }

    ABCA_ASSERT( baseDtype >= 0, "Bad base datatype id" );

    if ( adt.getExtent() == 1 )
    {
        // Non-arrayed.
        return baseDtype;
    }
    else
    {
        ABCA_ASSERT( adt.getExtent() != 0,
                     "Cannot create a DataType of extent 0!" );

        // Have to make an arrayed type.
        Dimensions dims( adt.getExtent() );
        HDimensions hdims( dims );
        hid_t ret = H5Tarray_create2( baseDtype, hdims.rank(),
                                      hdims.rootPtr() );
        if ( oCleanUp )
        {
            H5Tclose( baseDtype );
        }
        oCleanUp = true;
        ABCA_ASSERT( ret >= 0, "Bad array datatype id" );
        return ret;
    }
}

//-*****************************************************************************
static AbcA::DataType interpretH5TNonArrayed( hid_t dtype )
{
    AbcA::DataType ret;

    // Determine class to switch over.
    H5T_class_t dtypeClass = H5Tget_class( dtype );
    ABCA_ASSERT( dtypeClass != H5T_NO_CLASS, "H5Tget_class failed" );

    //-*************************************************************************
    //-*************************************************************************
    // INTEGER:
    // Choices are signed/unsigned, 8,16,32,64
    if ( dtypeClass == H5T_INTEGER )
    {
        // Not arrayed.
        ret.setExtent( 1 );

        bool isSigned = ( bool )H5Tget_sign( dtype );
        size_t bytes = H5Tget_size( dtype );
        if ( bytes == 1 )
        {
            if ( H5Tget_precision( dtype ) < 8 )
            {
                ret.setPod( kBooleanPOD );
            }
            else
            {
                ret.setPod( isSigned ? kInt8POD : kUint8POD );
            }
        }
        else if ( bytes == 2 )
        {
            ret.setPod( isSigned ? kInt16POD : kUint16POD );
        }
        else if ( bytes == 4 )
        {
            ret.setPod( isSigned ? kInt32POD : kUint32POD );
        }
        else if ( bytes == 8 )
        {
            ret.setPod( isSigned ? kInt64POD : kUint64POD );
        }
        else
        {
            ABCA_THROW( "Unknown dtype bit depth: " << (8*bytes) );
        }
    }
    //-*************************************************************************
    //-*************************************************************************
    // FLOAT:
    // Choices are 16/32/64 bit
    else if ( dtypeClass == H5T_FLOAT )
    {
        // Not arrayed.
        ret.setExtent( 1 );

        // Figure out num bytes
        size_t bytes = H5Tget_size( dtype );
        if ( bytes == 2 )
        {
            ret.setPod( kFloat16POD );
        }
        else if ( bytes == 4 )
        {
            ret.setPod( kFloat32POD );
        }
        else if ( bytes == 8 )
        {
            ret.setPod( kFloat64POD );
        }
        else
        {
            ABCA_THROW( "Unknown dtype bit depth: " << (8*bytes) );
        }
    }
    //-*************************************************************************
    //-*************************************************************************
    // EVERYTHING ELSE is an error
    else
    {
        ABCA_THROW( "Invalid non-arrayed datatype" );
    }

    return ret;
}

//-*****************************************************************************
DataType InterpretH5T( hid_t dtype )
{
    if ( H5Tget_class( dtype ) == H5T_ARRAY )
    {
        AbcA::DataType baseType =
            interpretH5TNonArrayed( H5Tget_super( dtype ) );

        int ndims = H5Tget_array_ndims( dtype );
        ABCA_ASSERT( ndims > 0, "H5Tget_array_ndims failed" );

        HDimensions hdims( ( size_t )ndims );
        ndims = H5Tget_array_dims2( dtype, hdims.rootPtr() );
        ABCA_ASSERT( ndims > 0, "H5Tget_array_dims2 failed" );
        ABCA_ASSERT( ndims == hdims.rank(),
                     "H5Tget_array_dims2 inconsistent ranks" );

        return AbcA::DataType( baseType.getPod(), hdims.numPoints() );
    }
    else
    {
        return interpretH5TNonArrayed( dtype );
    }
}

} // End namespace AbcCoreHDF5
} // End namespace Alembic
