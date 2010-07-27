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

#include <Alembic/Asset/Body/ODataBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
void WriteScalar( H5A &attr,
                  const H5T &memType,
                  const void *data )
{
    attr.writeAll( memType, data );
}

//-*****************************************************************************
void WriteScalar( H5AttrObj &parent,
                  const std::string &attrName,
                  const H5T &memType,
                  const H5T &fileType,
                  const void *data )
{
    H5S dspace( H5S::kScalar );
    H5A attr( parent, attrName, fileType, dspace );
    attr.writeAll( memType, data );
}

//-*****************************************************************************
void WriteString( H5AttrObj &parent,
                  const std::string &attrName,
                  const std::string &str )
{
    H5T dtype;
    size_t len = str.length();
    if ( len < 1 ) { len = 1; }
    dtype.copyFrom( H5T_C_S1 );
    dtype.setSize( len );

    H5S dspace( H5S::kScalar );
    H5A attr( parent, attrName, dtype, dspace );
    attr.writeAll( dtype, ( const void * )str.c_str() );
    // CJH H5Fflush( attr.id(), H5F_SCOPE_GLOBAL );
}

//-*****************************************************************************
//-*****************************************************************************
void WriteEnumChar( H5AttrObj &parent,
                    const std::string &attrName,
                    char thing )
{
    WriteScalar( parent, attrName,
                 H5T_NATIVE_CHAR,
                 H5T_STD_I8LE,
                 ( const void * )&thing );
}

//-*****************************************************************************
//-*****************************************************************************
void WriteHashID( OContextBody &ctx,
                  H5D &hashDset,
                  const Slab::HashID &hashID,
                  const std::string &attrName )
{
    // Okay - slab contains an attribute called "hashID"
    // that provides the hashID
    DataType hashIDdtype( kUint8POD, 16 );
    const DataTypeTuple &hidDtt =
        ctx.dataTypeTupleMap().find( hashIDdtype );
    
    WriteScalar( hashDset, attrName,
                 hidDtt.nativeH5T(),
                 hidDtt.fileH5T(),
                 ( const void * )&hashID );
}

//-*****************************************************************************
//-*****************************************************************************
void WriteDataTypeAsAttrs( H5AttrObj &parent,
                           const std::string &podAttrName,
                           const std::string &extentAttrName,
                           const DataType &dtype )
{
    WriteEnumChar( parent, podAttrName, ( char )dtype.pod );
    WriteScalar( parent, extentAttrName,
                 H5T_NATIVE_UCHAR, H5T_STD_I8LE,
                 ( const void * )&dtype.extent );
}

} // End namespace Asset
} // End namespace Alembic

