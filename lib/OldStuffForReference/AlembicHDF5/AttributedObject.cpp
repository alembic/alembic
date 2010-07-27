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

#include <AlembicHDF5/AttributedObject.h>

namespace AlembicHDF5 {

//-*****************************************************************************
size_t AttributedObject::numAttributes() const
{
    // Make sure we exist.
    ABCH5_CHECK_VALID( *this,
                       "AttributedObject::numAttributes() called on "
                       "invalid object" );

    // Get the object info.
    H5O_info_t oinfo;
    herr_t status = H5Oget_info( id(), &oinfo );
    ABCH5_ASSERT( status >= 0,
                  "AttributedObject::numAttributes() H5Oget_info failed." );

    return ( size_t )( oinfo.num_attrs );
}

//-*****************************************************************************
bool AttributedObject::attributeExists( const std::string &attrName ) const
{
    // Make sure we exist.
    ABCH5_CHECK_VALID( *this,
                       "AttributedObject::attributeExists() called on "
                       "invalid object" );

    // See if attr exists.
    htri_t tf = H5Aexists( id(), attrName.c_str() );
    ABCH5_ASSERT( tf >= 0,
                  "AttributedObject::attributeExists() H5Aexists failed" );
    return ( bool )tf;
}

//-*****************************************************************************
bool AttributedObject::attributeExists( const std::string &objName,
                                        const std::string &attrName ) const
{
    // Make sure we exist.
    ABCH5_CHECK_VALID( *this,
                       "AttributedObject::attributeExists() called on "
                       "invalid object" );

    // See if attr exists.
    htri_t tf = H5Aexists_by_name( id(), objName.c_str(),
                                   attrName.c_str(),
                                   H5P_DEFAULT );
    ABCH5_ASSERT( tf >= 0,
                  "AttributedObject::attributeExists() "
                  "H5Aexists_by_name failed" );
    return ( bool )tf;
}

} // End namespace AlembicHDF5
