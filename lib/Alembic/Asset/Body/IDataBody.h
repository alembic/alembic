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

#ifndef _Alembic_Asset_IDataBody_h_
#define _Alembic_Asset_IDataBody_h_

#include <Alembic/Asset/Body/FoundationBody.h>
#include <Alembic/Asset/Body/IContextBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
// Scalars are always Attributes.
// The various forms here amount to different amounts of type validation.

//-*****************************************************************************
void ReadScalar( const H5A &attr,
                 const H5T &memType,
                 void *data,

                 const H5T *CHECK_FILE_TYPE = NULL );

//-*****************************************************************************
void ReadScalar( const H5AttrObj &parent,
                 const std::string &attrName,
                 const H5T &memType,
                 void *data,

                 const H5T *CHECK_FILE_TYPE = NULL );

//-*****************************************************************************
void ReadScalar( const H5AttrObj &grandparent,
                 const std::string &parentName,
                 const std::string &attrName,
                 const H5T &memType,
                 void *data,

                 const H5T *CHECK_FILE_TYPE = NULL );

//-*****************************************************************************
//-*****************************************************************************
char ReadEnumChar( const H5AttrObj &parent,
                   const std::string &attrName );

//-*****************************************************************************
char ReadEnumChar( const H5AttrObj &grandparent,
                   const std::string &parentName,
                   const std::string &attrName );

//-*****************************************************************************
//-*****************************************************************************
std::string ReadString( const H5A &attr );
std::string ReadString( const H5AttrObj &parent,
                        const std::string &attrName );

std::string ReadString( const H5AttrObj &grandparent,
                        const std::string &parentName,
                        const std::string &attrName );

//-*****************************************************************************
//-*****************************************************************************
// Hash ID
void ReadHashID( SharedIContextBody ctx,
                 const H5D &slabDset,
                 Slab::HashID &hashID,
                 const std::string &attrName = "hashID" );

void ReadHashID( SharedIContextBody ctx,
                 const H5G &grandparent,
                 const std::string &dsetName,
                 Slab::HashID &hashID,
                 const std::string &attrName = "hashID" );

//-*****************************************************************************
//-*****************************************************************************
DataType ReadDataTypeFromAttrs( const H5AttrObj &grandparent,
                                const std::string &parentName,
                                const std::string &podAttrName,
                                const std::string &extentAttrName );

//-*****************************************************************************
DataType ReadDataTypeFromAttrs( const H5AttrObj &parent,
                                const std::string &podAttrName,
                                const std::string &extentAttrName );
                                

} // End namespace Asset
} // End namespace Alembic

#endif
