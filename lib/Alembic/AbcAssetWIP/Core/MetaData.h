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

#ifndef _Alembic_Core_MetaData_h_
#define _Alembic_Core_MetaData_h_

#include <Alembic/Core/Foundation.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
// Meta Data is a write-once, read-many string-to-string dictionary.
// (for now). It serializes and deserializes to a string.
class MetaData
{
public:
    MetaData() {}
    MetaData( const MetaData &copy )
      : m_tokenMap( copy.m_tokenMap ) {}
    MetaData& operator=( const MetaData &copy )
    {
        m_tokenMap = copy.m_tokenMap;
        return *this;
    }

    // This is deserialization, basically.
    MetaData( const std::string &fromString )
      : m_tokenMap( fromString ) {}

    bool tokenExists( const std::string &token ) const
    {
        return m_tokenMap.tokenExists( token );
    }

    // It is an error to set a value that has already been set.
    void setValue( const std::string &token,
                   const std::string &value )
    {
        if ( m_tokenMap.tokenExists( token ) )
        {
            ALEMBIC_CORE_THROW( "MetaData::setValue(): Token "
                                << token << " has already been set to: "
                                << m_tokenMap[token]
                                << ". Cannot overwrite to: "
                                << value );
        }
        m_tokenMap.setValue( token, value );
    }

    std::string getValue( const std::string &token ) const
    {
        return m_map.value();
    }

    // Serialization!
    std::string serialize() const
    {
        return m_tokenMap.get();
    }

protected:
    
    AlembicUtil::TokenMap m_tokenMap;
};

} // End namespace Core
} // End namespace Alembic

#endif
