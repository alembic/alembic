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

#ifndef _Alembic_MD5Hash_MD5_h_
#define _Alembic_MD5Hash_MD5_h_

#include <Alembic/MD5Hash/Foundation.h>
#include <Alembic/MD5Hash/Process.h>

namespace Alembic {
namespace MD5Hash {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class MD5
{
public:
    MD5() : m_process() {}

    // Default copy constructor.
    // Default assignment operator.

    void update( const uint8_t *iData, size_t iNum );
    void update( const int8_t *iData, size_t iNum );
    void update( const uint16_t *iData, size_t iNum );
    void update( const int16_t *iData, size_t iNum );
    void update( const uint32_t *iData, size_t iNum );
    void update( const int32_t *iData, size_t iNum );
    void update( const uint64_t *iData, size_t iNum );
    void update( const int64_t *iData, size_t iNum );
    void update( const float16_t *iData, size_t iNum );
    void update( const float32_t *iData, size_t iNum );
    void update( const float64_t *iData, size_t iNum );

    // From iterators.
    template <class ITERATOR>
    void updateIter( ITERATOR begin, ITERATOR end )
    {
        const size_t numElems = end - begin;
        this->update( &(*begin), numElems );
    }
    
    // This bakes the current thing down into a digest.
    // It is doing final calculations that aren't completely cheap,
    // so cache the results!
    MD5Digest digest() const
    {
        return m_process.digest();
    }

    Process m_process;
};

//-*****************************************************************************
//-*****************************************************************************
// FUNCTIONS FOR MAKING MD5 HASHES FROM COMMON THINGS
//-*****************************************************************************
//-*****************************************************************************
MD5 &operator<<( MD5 &md5, const std::string &str );
MD5 &operator<<( MD5 &md5, std::istream &istr );

template <class ITERATOR>
inline MD5 &operator<<( MD5 &md5, const std::pair<ITERATOR,ITERATOR> &range )
{
    md5.updateIter<ITERATOR>( range.first, range.second );
    return md5;
}

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace MD5Hash
} // End namespace Alembic


#endif

