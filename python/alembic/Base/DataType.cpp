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

#ifndef _AlembicAsset_Base_DataType_h_
#define _AlembicAsset_Base_DataType_h_

#include <AlembicAsset/Base/PlainOldDataType.h>
#include <iostream>

namespace AlembicAsset {

//-*****************************************************************************
// Slabs should not carry data type interpretations (like V3f vs Rgb3f) because
// we want the slabs to be as reusable as conceivably possible.
// Only at the property level will Data Type Interpretations be needed.
// So, then, a DataType is just a PlainOldDataType enum and an extent.
//
// At this level, the DataType still refers to a specific kind of storage,
// corresponding to what is 'one element' in a list. There is not any
// 'interpretation' added here - so, a Quatf is no different than a Vec4f
// or an RgbaF.
//-*****************************************************************************
struct DataType
{
    PlainOldDataType pod;
    uint8_t extent;

    DataType() throw()
      : pod( kUnknownPOD ), extent( 0 ) {}
    DataType( const DataType &copy ) throw()
      : pod( copy.pod ), extent( copy.extent ) {}
    DataType( PlainOldDataType pd, uint8_t ex ) throw()
      : pod( pd ), extent( ex ) {}

    DataType& operator=( const DataType &copy ) throw()
    {
        pod = copy.pod;
        extent = copy.extent;
        return *this;
    }

    size_t bytes() const throw()
    { return PODBytes( pod ) * ( size_t )extent; }

    void setDefaultBytes( void *into ) const throw()
    {
        char *intoc = ( char * )into;
        const size_t podb = PODBytes( pod );
        for ( size_t c = 0; c < extent; ++c, intoc += podb )
        {
            PODSetDefaultBytes( pod, ( void * )intoc );
        }
    }
};

//-*****************************************************************************
inline std::ostream &operator<<( std::ostream &ostr,
                                 const DataType &a )
{
    ostr << PODName( a.pod );
    if ( a.extent > 1 )
    {
        ostr << "[" << ( size_t )a.extent << "]";
    }
    return ostr;
}

//-*****************************************************************************
inline bool operator==( const DataType &a, const DataType &b ) throw()
{
    return ( ( a.pod == b.pod ) && ( a.extent == b.extent ) );
}

//-*****************************************************************************
inline bool operator!=( const DataType &a, const DataType &b ) throw()
{
    return ( ( a.pod != b.pod ) || ( a.extent != b.extent ) );
}

//-*****************************************************************************
inline bool operator<( const DataType &a, const DataType &b ) throw()
{
    if ( a.pod < b.pod ) { return true; }
    else if ( a.pod > b.pod ) { return false; }
    else { return ( a.extent < b.extent ); }
}

} // End namespace AlembicAsset

#endif
