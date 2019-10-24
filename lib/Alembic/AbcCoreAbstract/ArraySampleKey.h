//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef Alembic_AbcCoreAbstract_ArraySampleKey_h
#define Alembic_AbcCoreAbstract_ArraySampleKey_h

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/DataType.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace ALEMBIC_VERSION_NS {

struct ArraySampleKey : public Alembic::Util::totally_ordered<ArraySampleKey>
{
    //! total number of bytes of the sample as originally stored
    uint64_t numBytes;

    //! Original POD as stored
    PlainOldDataType origPOD;

    //! POD used at read time
    PlainOldDataType readPOD;

    Digest digest;

    bool operator==( const ArraySampleKey &iRhs ) const
    {
        return ( ( numBytes == iRhs.numBytes ) &&
                 ( origPOD  == iRhs.origPOD  ) &&
                 ( readPOD  == iRhs.readPOD  ) &&
                 ( digest ==   iRhs.digest ) );
    };

    bool operator<( const ArraySampleKey &iRhs ) const
    {
        return ( numBytes < iRhs.numBytes ? true :
                 ( numBytes > iRhs.numBytes ? false :

                   ( origPOD < iRhs.origPOD ? true :
                     ( origPOD > iRhs.origPOD ? false :

                       ( readPOD < iRhs.readPOD ? true :
                         ( readPOD > iRhs.readPOD ? false :

                           ( digest < iRhs.digest ) ) ) ) ) ) );
    };

};

//-*****************************************************************************
// Equality operator.
struct ArraySampleKeyEqualTo
{
    typedef ArraySampleKey first_argument_type;
    typedef ArraySampleKey second_argument_type;
    typedef bool result_type;

    bool operator()( ArraySampleKey const &a,
                     ArraySampleKey const &b ) const
    {
        return a == b;
    }
};

//-*****************************************************************************
// Hash function
inline size_t StdHash( ArraySampleKey const &a )
{
    // Theoretically, the bits of our hash are uniformly
    // randomly distributed, so it doesn't matter which of the 128
    // bits we use to generate the 64 bits that we return as the hash
    // key. So, I'll just do the simple thing.
    return *(( const size_t * )&a.digest);
}

//-*****************************************************************************
struct ArraySampleKeyStdHash
{
    typedef ArraySampleKey argument_type;
    typedef size_t result_type;

    size_t operator()( ArraySampleKey const &a ) const
    {
        return StdHash( a );
    }
};

//-*****************************************************************************
template <class MAPPED>
struct UnorderedMapUtil
{
    typedef Alembic::Util::unordered_map<ArraySampleKey,
                                         MAPPED,
                                         ArraySampleKeyStdHash,
                                         ArraySampleKeyEqualTo> umap_type;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
