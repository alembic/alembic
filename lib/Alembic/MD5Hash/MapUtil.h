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

#ifndef _Alembic_MD5Hash_MapUtil_h_
#define _Alembic_MD5Hash_MapUtil_h_

#include <Alembic/MD5Hash/Foundation.h>
#include <Alembic/MD5Hash/Digest.h>

namespace Alembic {
namespace MD5Hash {

//-*****************************************************************************
// The tr1 standard supports a hash map, called unordered_map. Here is some
// documentation on it:
//
//-*****************************************************************************
// namespace std {
// namespace tr1 {
// 
// // [6.3.4.4] Class template unordered_map
// template <class Key,
//          class T,
//          class Hash = hash<Key>,
//          class Pred = std::equal_to<Key>,
//          class Alloc = std::allocator<std::pair<const Key, T> > >
//          class unordered_map;
// 
// // [6.3.4.6] Class template unordered_multimap
// template <class Key,
//           class T,
//           class Hash = hash<Key>,
//           class Pred = std::equal_to<Key>,
//           class Alloc = std::allocator<std::pair<const Key, T> > >
// class unordered_multimap;
//
// template <class Key, class T, class Hash, class Pred, class Alloc>
// void swap(unordered_map<Key, T, Hash, Pred, Alloc>& x,
//           unordered_map<Key, T, Hash, Pred, Alloc>& y);
//
// template <class Key, class T, class Hash, class Pred, class Alloc>
// void swap(unordered_multimap<Key, T, Hash, Pred, Alloc>& x,
//           unordered_multimap<Key, T, Hash, Pred, Alloc>& y);
// 
// } // namespace tr1
// } // namespace std
//-*****************************************************************************
//
// We'd like to support basic support for unordered maps and unordered sets
// using MD5 Hash Digests as keys. In order to do this, we need
// an STL-Hash function and an STL-Pred function for MD5 Hash Digests
//
//-*****************************************************************************

//-*****************************************************************************
// Equality operator.
struct MD5DigestEqualTo :
        public std::binary_function<MD5Digest,MD5Digest,bool>
{
    bool operator()( MD5Digest const &a,
                     MD5Digest const &b ) const
    {
        return ( ( a.words[0] == b.words[0] ) &&
                 ( a.words[1] == b.words[1] ) &&
                 ( a.words[2] == b.words[2] ) &&
                 ( a.words[3] == b.words[3] ) );
    }
};

//-*****************************************************************************
// Hash function
inline std::size_t StdHash( MD5Digest const &a )
{
    // Theoretically, the bits of an MD5 Hash are uniformly
    // randomly distributed, so it doesn't matter which of the 128
    // bits we use to generate the 64 bits that we return as the hash
    // key. So, I'll just do the simple thing.
    return *(( const std::size_t * )&a);
}

//-*****************************************************************************
struct MD5DigestStdHash :
        public std::unary_function<MD5Digest,std::size_t>
{
    std::size_t operator()( MD5Digest const &a ) const
    {
        return StdHash( a );
    }     
};

//-*****************************************************************************
template <class MAPPED>
struct UnorderedMapUtil
{
    typedef boost::unordered_map<MD5Digest,
                                 MAPPED,
                                 MD5DigestStdHash,
                                 MD5DigestEqualTo> umap_type;
    typedef boost::unordered_multimap<MD5Digest,
                                      MAPPED,
                                      MD5DigestStdHash,
                                      MD5DigestEqualTo> umultimap_type;
};

//-*****************************************************************************
// Unordered sets don't need a wrapping template.
// This isn't a terribly useful type. And it's meaningless to have
// multisets in this context.
typedef boost::unordered_set<MD5Digest,
                             MD5DigestStdHash,
                             MD5DigestEqualTo> UnorderedMD5DigestSet;

} // End namespace MD5Hash
} // End namespace Alembic

#endif
