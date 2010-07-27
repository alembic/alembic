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

//-*****************************************************************************
//! \file lib/AlembicAsset/Base/DataType.h
//! \brief Specification of the \ref DataType class
#ifndef _Alembic_Core_DataType_h_
#define _Alembic_Core_DataType_h_

#include <Alembic/Core/Foundation.h>
#include <Alembic/Core/PlainOldDataType.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
//! \brief The DataType is an explicit description of how a single
//! "data element" in an array of data that might live in a property
//! is stored in memory or on disk.
//!
//! \details The DataType descibes precisely how a single data element, or
//! datum, is laid out in memory, and how it is laid out in the file. There is
//! a 1-1 mapping from memory layout to file layout, though the two may not be
//! identical. Alembic is defined as little-endian on disk, regardless of the
//! endian-ness of the host computer. The data conversion when serializing
//! and deserializing is handled automatically.
//!
//! This DataType does not distinguish between, say, a Vec3f interpretation
//! of 3 floats vs. an Rgbf interpretation. This is to maximize theoretical
//! reuse of data slabs.
//-*****************************************************************************
class DataType
{
public:
    //! \brief Default constructor.
    //! What's a good default data type?
    //! Is it ever useful to construct one of these without
    //! knowing what you're making?
    //! By default, makes pod = kUnknownPOD and extent = 0,
    //! which will create errors everywhere.
    DataType()
      : m_pod( kUnknownPOD ), m_extent( 0 ) {}

    //! \brief Copy constructor.
    //! ...
    DataType( const DataType &copy )
      : m_pod( copy.m_pod ),
        m_extent( copy.m_extent )
    {
        assert( m_extent > 0 );
    }


    //! \brief Explicit constructor. Extent defaults to 1.
    //! Checks to make sure if it's not a string that the extent
    //! is greater than 0.
    explicit DataType( PlainOldDataType pd, uint8_t ex = 1 )
      : m_pod( pd ),
        m_extent( ex )
    {
        assert( m_extent > 0 );
    }

    //! \brief Assignment operator.
    //! ...
    DataType& operator=( const DataType &copy )
    {
        m_pod = copy.m_pod;
        m_extent = copy.m_extent;
        return *this;
    }

    PlainOldDataType pod() const { return m_pod; }
    uint8_t extent() const { return m_extent; }


    //! \brief Returns whether or not this is a StringDataType.
    //! ...
    bool isString() const
    {
        return ( m_pod == kCStringPOD ) && ( m_extent == 1 );
    }

    //! \brief Returns the number of bytes that each data element
    //! will occupy. It is an error to call this for a string data type.
    size_t numBytes() const
    {
        assert( m_extent > 0 );
        return PODNumBytes( m_pod ) * ( size_t )m_extent;
    }

    //! \brief Sets the bytes of a data element to a default
    //! value. Not a particularly elegant way to do this - inefficient,
    //! but suitably generic when upcasting is not available.
    //! It is an error to call this for a string data type.
    void setDefault( void *into ) const
    {
        assert( m_extent > 0 );
        char *intoc = ( char * )into;
        const size_t podb = PODNumBytes( m_pod );
        for ( uint8_t c = 0; c < m_extent; ++c, intoc += podb )
        {
            PODSetDefault( m_pod, ( void * )intoc );
        }
    }

protected:
    
    //! \brief The enum representing which \ref PlainOldDataType we
    //! are composed from
    PlainOldDataType m_pod;

    //! \brief An extent - a single-dimensional array value, allowing for
    //! DataTypes to be non-scalar up to 255 elements. This value is 0
    //! for StringDataTypes, and 1 for Scalar DataTypes.
    uint8_t m_extent;
};

//-*****************************************************************************
//! \brief std::iostream output support for DataType instances.
//! ...
inline std::ostream &operator<<( std::ostream &ostr,
                                 const DataType &a )
{
    if ( a.isString() )
    {
        ostr << "string";
    }
    else
    {
        ostr << PODName( a.pod() );
        if ( a.extent() > 1 )
        {
            ostr << "[" << ( size_t )a.extent() << "]";
        }
    }
    return ostr;
}

//-*****************************************************************************
//! \brief Equality comparison of two DataType instances
//! ...
inline bool operator==( const DataType &a, const DataType &b )
{
    return ( ( a.pod() == b.pod() ) && ( a.extent() == b.extent() ) );
}

//-*****************************************************************************
//! \brief Inequality comparison of two DataType instances
//! ...
inline bool operator!=( const DataType &a, const DataType &b )
{
    return ( ( a.pod() != b.pod() ) || ( a.extent() != b.extent() ) );
}

//-*****************************************************************************
//! \brief Ordering comparator for two DataType instances.
//! This exists solely for sorting data type instances, which is used by
//! the DataTypeTupleMap.
inline bool operator<( const DataType &a, const DataType &b )
{
    if ( a.pod() < b.pod() ) { return true; }
    else if ( a.pod() > b.pod() ) { return false; }
    else { return ( a.extent() < b.extent() ); }
}

} // End namespace Core
} // End namespace Alembic

#endif
