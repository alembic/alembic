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

#ifndef _AlembicAsset_Base_ISlabCache_h_
#define _AlembicAsset_Base_ISlabCache_h_

#include <AlembicAsset/Base/Slab.h>
#include <MD5Hash/MapUtil.h>

namespace AlembicAsset {

//-*****************************************************************************
// CJH: This needs fixin'.
// It's a total mess - I know this...
class ISlabCache
{
public:
    //-*************************************************************************
    // PROMOTED TYPES
    //-*************************************************************************
    typedef Slab::HashID Key;

    //-*************************************************************************
    // INTERNAL STORAGE
    //-*************************************************************************
protected:
    struct Record
    {
        Key key;
        SharedSlab slab;
        int64_t count;
    };

public:
    //-*************************************************************************
    // LOCK PROTECTION
    //-*************************************************************************
    class LockGuard : public boost::noncopyable
    {
    private:
        friend class ISlabCache;
        
        ISlabCache &m_cache;
        Record &m_record;
        
        LockGuard( ISlabCache &c, Record &r );

    public:
        ~LockGuard();
        
        const Key &key() const { return m_record.key; }
        const Slab &slab() const { return *(m_record.slab); }
    };

    typedef boost::shared_ptr<LockGuard> Handle;
    

    //-*************************************************************************
    // PUBLIC INTERFACE
    //-*************************************************************************
    ISlabCache();
    ~ISlabCache();

    Handle find( const Key &key );
    Handle store( const Key &key, SharedSlab sbuf );

protected:
    friend class LockGuard;
    void increment( Record &r );
    void decrement( Record &r );

    //-*************************************************************************
    // INTERNAL STORAGE
    // Using the unordered map (hash map)
    //-*************************************************************************
    typedef boost::shared_ptr<Record> SharedRecord;
    typedef MD5Hash::UnorderedMapUtil<SharedRecord>::umap_type Map;

    Map m_map;
};

} // End namespace AlembicAsset

#endif
