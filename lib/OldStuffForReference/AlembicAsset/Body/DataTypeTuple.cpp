//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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

#include <AlembicAsset/Body/DataTypeTuple.h>

namespace AlembicAsset {

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
SharedH5T GetNativeH5T( const DataType &adt )
{
    H5T baseDtype;
    switch ( adt.pod )
    {
    case kUint8POD:     baseDtype = H5T_NATIVE_UINT8; break;
    case kInt8POD:      baseDtype = H5T_NATIVE_INT8; break;
    case kUint16POD:    baseDtype = H5T_NATIVE_UINT16; break;
    case kInt16POD:     baseDtype = H5T_NATIVE_INT16; break;
    case kUint32POD:    baseDtype = H5T_NATIVE_UINT32; break;
    case kInt32POD:     baseDtype = H5T_NATIVE_INT32; break;
    case kUint64POD:    baseDtype = H5T_NATIVE_UINT64; break;
    case kInt64POD:     baseDtype = H5T_NATIVE_INT64; break;
    case kFloat32POD:   baseDtype = H5T_NATIVE_FLOAT; break;
    case kFloat64POD:   baseDtype = H5T_NATIVE_DOUBLE; break;
    default:
        AAH5_THROW( "Unsuppored POD type: " << PODName( adt.pod ) );
    }

    if ( adt.extent == 1 )
    {
        // Non-arrayed. The return object is shared,
        // but it just a light envelope, id
        return MakeSharedH5T( baseDtype.id() );
    }
    else
    {
        // Have to make an arrayed type.
        return MakeSharedH5T( baseDtype, ( hsize_t )adt.extent );
    }
}

//-*****************************************************************************
// Same as above, only this time for the files.
// Alembic uses LittleEndian by default.
SharedH5T GetFileH5T( const DataType &adt )
{
    H5T baseDtype;
    switch ( adt.pod )
    {
    case kUint8POD:     baseDtype = H5T_STD_U8LE; break;
    case kInt8POD:      baseDtype = H5T_STD_I8LE; break;
    case kUint16POD:    baseDtype = H5T_STD_U16LE; break;
    case kInt16POD:     baseDtype = H5T_STD_I16LE; break;
    case kUint32POD:    baseDtype = H5T_STD_U32LE; break;
    case kInt32POD:     baseDtype = H5T_STD_I32LE; break;
    case kUint64POD:    baseDtype = H5T_STD_U64LE; break;
    case kInt64POD:     baseDtype = H5T_STD_I64LE; break;
    case kFloat32POD:   baseDtype = H5T_IEEE_F32LE; break;
    case kFloat64POD:   baseDtype = H5T_IEEE_F64LE; break;
    default:
        AAH5_THROW( "Unsuppored POD type: " << PODName( adt.pod ) );
    }

    if ( adt.extent == 1 )
    {
        // Non-arrayed.
        return MakeSharedH5T( baseDtype.id() );
    }
    else
    {
        // Have to make an arrayed type.
        return MakeSharedH5T( baseDtype, ( hsize_t )adt.extent );
    }
}

//-*****************************************************************************
static DataType interpretH5TNonArrayed( const H5T &dtype )
{
    DataType ret;

    // Determine class to switch over.
    H5T_class_t dtypeClass = dtype.getClass();

    //-*************************************************************************
    //-*************************************************************************
    // INTEGER:
    // Choices are signed/unsigned, 8,16,32,64
    if ( dtypeClass == H5T_INTEGER )
    {
        // Not arrayed.
        ret.extent = 1;

        bool isSigned = dtype.isSigned();
        size_t bytes = dtype.getSize();
        if ( bytes == 1 )
        {
            ret.pod = isSigned ? kInt8POD : kUint8POD;
        }
        else if ( bytes == 2 )
        {
            ret.pod = isSigned ? kInt16POD : kUint16POD;
        }
        else if ( bytes == 4 )
        {
            ret.pod = isSigned ? kInt32POD : kUint32POD;
        }
        else if ( bytes == 8 )
        {
            ret.pod = isSigned ? kInt64POD : kUint64POD;
        }
        else
        {
            AAH5_THROW( "Unknown dtype bit depth: " << (8*bytes) );
        }
    }
    //-*************************************************************************
    //-*************************************************************************
    // FLOAT:
    // Choices are 16/32/64 bit
    else if ( dtypeClass == H5T_FLOAT )
    {
        // Not arrayed.
        ret.extent = 1;

        // Figure out num bytes
        size_t bytes = dtype.getSize();
        if ( bytes == 2 )
        {
            ret.pod = kFloat16POD;
        }
        else if ( bytes == 4 )
        {
            ret.pod = kFloat32POD;
        }
        else if ( bytes == 8 )
        {
            ret.pod = kFloat64POD;
        }
        else
        {
            AAH5_THROW( "Unknown dtype bit depth: " << (8*bytes) );
        }
    }
    //-*************************************************************************
    //-*************************************************************************
    // EVERYTHING ELSE is an error
    else
    {
        AAH5_THROW( "Invalid non-arrayed datatype" );
    }

    return ret;
}

//-*****************************************************************************
DataType InterpretH5T( const H5T &dtype )
{
    if ( dtype.isArrayClass() )
    {
        DataType baseType = interpretH5TNonArrayed( dtype.getSuper() );
        return DataType( baseType.pod, dtype.getRank0Dim() );
    }
    else
    {
        return interpretH5TNonArrayed( dtype );
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// DATA TYPE TUPLE CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
DataTypeTuple::DataTypeTuple( SharedH5T fH5T )
{
    AAH5_ASSERT( ( ( bool )fH5T && fH5T->valid() ),
                  "DataTypeTuple::DataTypeTuple() passed invalid file H5T" );
    m_fileH5T = fH5T;
    m_dataType = InterpretH5T( *m_fileH5T );
    m_nativeH5T = GetNativeH5T( m_dataType );
}

//-*****************************************************************************
// This differs from the one above - the above simple shares the shared fH5T.
// This one creates a new one. Possibly wasteful, but so be it.
// CJH: Fix Me.
DataTypeTuple::DataTypeTuple( const H5T &fH5T )
{
    AAH5_ASSERT( fH5T.valid(),
                  "DataTypeTuple::DataTypeTuple() passed invalid file H5T" );
    m_dataType = InterpretH5T( fH5T );
    m_fileH5T = GetFileH5T( m_dataType );
    m_nativeH5T = GetNativeH5T( m_dataType );
}

//-*****************************************************************************
DataTypeTuple::DataTypeTuple( const DataType &dtype )
{
    m_dataType = dtype;
    m_fileH5T = GetFileH5T( m_dataType );
    m_nativeH5T = GetNativeH5T( m_dataType );
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// DATA TYPE TUPLE MAP
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
bool DataTypeTupleMap::exists( const H5T &fH5T ) const
{
    return m_map.count( InterpretH5T( fH5T ) ) > 0;
}

//-*****************************************************************************
bool DataTypeTupleMap::exists( const DataType &dtype ) const
{
    return m_map.count( dtype ) > 0;
}

//-*****************************************************************************
const DataTypeTuple &DataTypeTupleMap::find( SharedH5T fH5T )
{
    DataType dtype = InterpretH5T( *fH5T );
    InternalMap::iterator fiter = m_map.find( dtype );
    if ( fiter == m_map.end() )
    {
        // Doesn't exist.
        return ( m_map[dtype] = DataTypeTuple( fH5T ) );
    }
    else
    {
        // It does exist.
        return (*fiter).second;
    }
}

//-*****************************************************************************
const DataTypeTuple &DataTypeTupleMap::find( const H5T &fH5T )
{
    DataType dtype = InterpretH5T( fH5T );
    InternalMap::iterator fiter = m_map.find( dtype );
    if ( fiter == m_map.end() )
    {
        // Doesn't exist.
        return ( m_map[dtype] = DataTypeTuple( fH5T ) );
    }
    else
    {
        // It does exist.
        return (*fiter).second;
    }
}

//-*****************************************************************************
const DataTypeTuple &DataTypeTupleMap::find( const DataType &dtype )
{
    InternalMap::iterator fiter = m_map.find( dtype );
    if ( fiter == m_map.end() )
    {
        // Doesn't exist.
        return ( m_map[dtype] = DataTypeTuple( dtype ) );
    }
    else
    {
        // It does exist.
        return (*fiter).second;
    }
}

} // End namespace AlembicAsset
