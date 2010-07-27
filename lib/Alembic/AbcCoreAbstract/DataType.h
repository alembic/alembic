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

#ifndef _Alembic_AbcCoreAbstract_DataType_h_
#define _Alembic_AbcCoreAbstract_DataType_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! The DataType class is a description of how an element of a sample in a
//! Scalar or an Array property is stored. It does not contain an interpretation
//! this is left to the metadata of the properties themselves.
class DataType
{
public:
    //! Default constructor
    //! Sets the DataType to an unknown DataType with extent 0.
    //! This is obviously an invalid storage description, and is used in
    //! cases where we need to indicated that a DataType could not be
    //! determined.
    DataType()
      : m_pod( kUnknownPOD ), m_extent( 0 ) {}

    //! Copy constructor.
    //! Simple copy of pod and extent.
    DataType( const DataType & iCopy )
      : m_pod( iCopy.m_pod ), m_extent( iCopy.m_extent ) {}

    //! Explicit constructor.
    //! Takes a pod and an extent.
    DataType( PlainOldDataType iPod, uint8_t iExtent )
      : m_pod( iPod ), m_extent( iExtent ) {}

    //! Assignment operator.
    //! Simple copy of POD and extent.
    DataType& operator=( const DataType &iCopy )
    {
        m_pod = iCopy.m_pod;
        m_extent = iCopy.m_extent;
        return *this;
    }

    //! Return the PlainOldDataType enum
    //! ...
    PlainOldDataType pod() const { return m_pod; }

    //! Return the 8-bit extent
    //! ...
    uint8_t extent() const { return m_extent; }

    //! Returns the number of bytes occupied by a single datum. (element)
    //! The assumption that each element has a fixed size in memory is a
    //! core assumption in Alembic. In the case of null-terminated string
    //! elements, Alembic treats them as pointers (const char *), which have
    //! a fixed size. An Array of Strings is an array of const char pointers,
    //! like: const char **. The library handles memory management of string
    //! data carefully, but this assumption here can remain.
    size_t getNumBytes() const
    { return PODNumBytes( m_pod ) * ( size_t )m_extent; }

private:
    //! An Enum indicating which PlainOldDataType is our
    //! super-storage-class.
    PlainOldDataType m_pod;

    //! An 8-bit extent indicating the cardinality of
    //! a single element of data.
    uint8_t m_extent;
};

//-*****************************************************************************
//! Outputs DataType to a std::ostream
//! Makes use of PlainOldDataType's string conversion functions
inline std::ostream &operator<<( std::ostream &ostr, const DataType &a )
{
    ostr << PODName( a.pod() );
    if ( a.extent() > 1 )
    {
        ostr << "[" << ( size_t )a.extent() << "]";
    }
    return ostr;
}

//-*****************************************************************************
//! Returns whether two datatypes are exactly equal
//! ...
inline bool operator==( const DataType &a, const DataType &b )
{
    return ( ( a.pod() == b.pod() ) && ( a.extent() == b.extent() ) );
}

//-*****************************************************************************
//! Returns whether two datatypes are not equal
//! ...
inline bool operator!=( const DataType &a, const DataType &b )
{
    return ( ( a.pod() != b.pod() ) || ( a.extent() != b.extent() ) );
}

//-*****************************************************************************
//! Returns whether one datatype is lexigraphically "less" than
//! another - this has meaning only so that DataType instances can
//! be meaningfully sorted.
inline bool operator<( const DataType &a, const DataType &b )
{
    if ( a.pod() < b.pod() ) { return true; }
    else if ( a.pod() > b.pod() ) { return false; }
    else { return ( a.extent() < b.extent() ); }
}

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
