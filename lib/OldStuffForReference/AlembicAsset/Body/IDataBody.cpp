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

#include <AlembicAsset/Body/IDataBody.h>

namespace AlembicAsset {

//-*****************************************************************************
void ReadScalar( const H5A &attr,
                 const H5T &memType,
                 void *data,

                 const H5T *CHECK_FILE_TYPE )
{
    if ( CHECK_FILE_TYPE )
    {
        H5T attrFtype( attr );
        assert( attrFtype.equivalent( *CHECK_FILE_TYPE ) );
        AAH5_ASSERT( attrFtype.equivalent( *CHECK_FILE_TYPE ),
                     "Non equivalent datatypes for scalar attribute: "
                     << attr.name() );
    }

    H5S attrSpace( attr );
    AAH5_ASSERT( attrSpace.isScalar(),
                 "Invalid size for scalar attribute: " << attr.name() );
    attr.readAll( memType, data );
}

//-*****************************************************************************
void ReadScalar( const H5AttrObj &parent,
                 const std::string &attrName,
                 const H5T &memType,
                 void *data,

                 const H5T *CHECK_FILE_TYPE )
{
    H5A attr( parent, attrName );
    ReadScalar( attr, memType, data, CHECK_FILE_TYPE );
}

//-*****************************************************************************
void ReadScalar( const H5AttrObj &grandparent,
                 const std::string &parentName,
                 const std::string &attrName,
                 const H5T &memType,
                 void *data,

                 const H5T *CHECK_FILE_TYPE )
{
    H5A attr( grandparent, parentName, attrName );
    ReadScalar( attr, memType, data, CHECK_FILE_TYPE );
}

//-*****************************************************************************
//-*****************************************************************************
std::string ReadString( const H5A &attr )
{
    // For scalar strings, we just use H5T_C_S1 x bytes
    // don't bother checking. yet.
    H5T dtype( attr );
    H5S dspace( attr );
    
    AAH5_ASSERT( dspace.isScalar(),
                 "ReadString() called on non-scalar attribute: "
                 << attr.name() );
    
    size_t len = dtype.getSize();
    std::vector<char> sbuf( len + 1, ( char )0 );
    attr.readAll( dtype, ( void * )&sbuf.front() );
    std::string ret( ( const char * )&sbuf.front() );
    return ret;
}

//-*****************************************************************************
std::string ReadString( const H5AttrObj &parent, const std::string &attrName )
{
    H5A attr( parent, attrName );
    return ReadString( attr );
}

//-*****************************************************************************
std::string ReadString( const H5AttrObj &grandparent,
                        const std::string &parentName,
                        const std::string &attrName )
{
    H5A attr( grandparent, parentName, attrName );
    return ReadString( attr );
}

//-*****************************************************************************
//-*****************************************************************************
char ReadEnumChar( const H5AttrObj &parent,
                   const std::string &attrName )
{
    char data;
    H5T CheckI8LE( H5T_STD_I8LE );
    ReadScalar( parent, attrName, H5T_NATIVE_CHAR,
                ( void * )&data, &CheckI8LE );
    return data;
}

//-*****************************************************************************
char ReadEnumChar( const H5AttrObj &grandparent,
                   const std::string &parentName,
                   const std::string &attrName )
{
    char data;
    H5T CheckI8LE( H5T_STD_I8LE );
    ReadScalar( grandparent, parentName, attrName, H5T_NATIVE_CHAR,
                ( void * )&data, &CheckI8LE );
    return data;
}

//-*****************************************************************************
//-*****************************************************************************
void ReadHashID( SharedIContextBody ctx,
                 const H5D &hashDset,
                 Slab::HashID &hashID,
                 const std::string &attrName )
{
    AAH5_ASSERT( ( bool )ctx,
                 "ReadHashID() passed invalid context" );
    
    
    // Okay - slab contains an attribute called "hashID"
    // that provides the hashID
    DataType hashIDdtype( kUint8POD, 16 );
    const DataTypeTuple &hidDtt =
        ctx->dataTypeTupleMap().find( hashIDdtype );
    
    ReadScalar( hashDset, attrName,
                hidDtt.nativeH5T(),
                ( void * )&hashID,
                
                // Error checking
                &hidDtt.fileH5T() );
}

//-*****************************************************************************
void ReadHashID( SharedIContextBody ctx,
                 const H5G &grandparentGrp,
                 const std::string &parentName,
                 Slab::HashID &hashID,
                 const std::string &attrName )
{
    AAH5_ASSERT( ( bool )ctx,
                 "ReadHashID() passed invalid context" );
    
    
    // Okay - slab contains an attribute called "hashID"
    // that provides the hashID
    DataType hashIDdtype( kUint8POD, 16 );
    const DataTypeTuple &hidDtt =
        ctx->dataTypeTupleMap().find( hashIDdtype );
    
    ReadScalar( grandparentGrp, parentName, attrName,
                hidDtt.nativeH5T(),
                ( void * )&hashID,
                
                // Error checking
                &hidDtt.fileH5T() );
}

//-*****************************************************************************
//-*****************************************************************************
DataType ReadDataTypeFromAttrs( const H5AttrObj &parent,
                                const std::string &podAttrName,
                                const std::string &extentAttrName )
{
    char podEnumC = ReadEnumChar( parent, podAttrName );
    AAH5_ASSERT( podEnumC < ( char )kNumPlainOldDataTypes,
                 "Invalid POD Enum in ReadDataTypeFromAttrs()" );

    H5A extAttr( parent, extentAttrName );
    H5T CheckU8LE( H5T_STD_I8LE );
    uint8_t extent;
    ReadScalar( extAttr, H5T_NATIVE_UCHAR, ( void * )&extent,
                &CheckU8LE );

    return DataType( ( PlainOldDataType )podEnumC, extent );
}

//-*****************************************************************************
DataType ReadDataTypeFromAttrs( const H5AttrObj &grandparent,
                                const std::string &parentName,
                                const std::string &podAttrName,
                                const std::string &extentAttrName )
{
    char podEnumC = ReadEnumChar( grandparent, parentName, podAttrName );
    AAH5_ASSERT( podEnumC < ( char )kNumPlainOldDataTypes,
                 "Invalid POD Enum in ReadDataTypeFromAttrs()" );

    H5A extAttr( grandparent, parentName, extentAttrName );
    H5T CheckU8LE( H5T_STD_I8LE );
    uint8_t extent;
    ReadScalar( extAttr, H5T_NATIVE_UCHAR, ( void * )&extent,
                &CheckU8LE );

    return DataType( ( PlainOldDataType )podEnumC, extent );
}

} // End namespace AlembicAsset
