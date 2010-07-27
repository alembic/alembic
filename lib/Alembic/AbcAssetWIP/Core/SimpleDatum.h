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

#ifndef _Alembic_Core_SimpleDatum_h_
#define _Alembic_Core_SimpleDatum_h_

#include <Alembic/Core/Foundation.h>
#include <Alembic/Core/DataType.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
template <PlainOldDataType POD, uint8_t EXTENT>
class SimpleDatum
{
public:
    typedef SimpleDatum<POD,EXTENT> this_type;
    typedef typename PODTraitsFromEnum<POD>::value_type value_type;

    static inline PlainOldDataType pod() { return POD; }
    static inline uint8_t extent() { return EXTENT; }

    SimpleDatum() { std::fill( m_data, m_data + EXTENT,
                               PODTraitsFromEnum<POD>::default_value() ); }

    explicit SimpleDatum( const value_type &val )
    {
        std::fill( m_data, m_data + EXTENT, val );
    }

    explicit SimpleDatum( const value_type *val )
    {
        std::copy( val, val + EXTENT, m_data );
    }

    SimpleDatum( const SimpleDatum<POD,EXTENT> &copy )
    {
        std::copy( copy.m_data, copy.m_data + EXTENT, m_data );
    }

    SimpleDatum& operator=( const SimpleDatum<POD,EXTENT> &copy )
    {
        std::copy( copy.m_data, copy.m_data + EXTENT, m_data );
        return *this;
    }

    value_type& operator[]( size_t idx ) { return m_data[idx]; }
    const value_type& operator[]( size_t idx ) const { return m_data[idx]; }
    
    value_type* get() const { return ( value_type* )m_data; }

    value_type* operator->() const
    {
        return ( value_type* )m_data;
    }

protected:                     
    value_type m_data[EXTENT];
};

//-*****************************************************************************
template <PlainOldDataType POD>
class SimpleDatum<POD,1>
{
public:
    typedef SimpleDatum<POD,1> this_type;
    typedef typename PODTraitsFromEnum<POD>::value_type value_type;

    static inline PlainOldDataType pod() { return POD; }
    static inline uint8_t extent() { return 1; }

    SimpleDatum() { m_data = PODTraitsFromEnum<POD>::default_value(); }

    explicit SimpleDatum( const value_type &val )
      : m_data( val )
    {
        // Nothing
    }

    explicit SimpleDatum( const value_type *val )
      : m_data( *val )
    {
        // Nothing
    }

    SimpleDatum( const SimpleDatum<POD,1> &copy )
    {
        m_data = copy.m_data;
    }

    SimpleDatum& operator=( const SimpleDatum<POD,1> &copy )
    {
        m_data = copy.m_data;
        return *this;
    }

    value_type& operator[]( size_t idx ) { return m_data; }
    const value_type& operator[]( size_t idx ) const { return m_data; }
    
    value_type* get() const { return ( value_type* )&m_data; }

    value_type* operator->() const
    {
        return ( value_type* )&m_data;
    }

protected:                     
    value_type m_data;
};

} // End namespace Core
} // End namespace Alembic

#endif
