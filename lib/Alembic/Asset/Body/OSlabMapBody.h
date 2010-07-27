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

#ifndef _Alembic_Asset_Body_OSlabMapBody_h_
#define _Alembic_Asset_Body_OSlabMapBody_h_

#include <Alembic/Asset/Body/FoundationBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
// For Output, there isn't really a cache per-se.  There is just a record
// or whether or not a specific key has been interred, and if so, where...
class OSlabMapBody
{
public:
    OSlabMapBody() {}

    // I want to be able to try out different things for referencing
    // slabs.
    struct SlabReference
    {
        typedef SlabReference this_type;
        
        SlabReference() throw() : obj_loc_id( 0 ), obj_name( "" ) {}
        SlabReference( hid_t id, const std::string &nme ) throw()
          : obj_loc_id( id ), obj_name( nme ) {}
        
        hid_t obj_loc_id;
        std::string obj_name;

        ALEMBIC_OPERATOR_BOOL( ( obj_loc_id > 0 &&
                                         obj_name != "" ) );
    };

    // Returns 0 if it can't find it.
    SlabReference find( const Slab::HashID &key ) const
    {
        Map::const_iterator miter = m_map.find( key );
        if ( miter != m_map.end() )
        {
            return (*miter).second;
        }
        else
        {
            return SlabReference();
        }
    }

    // Store. Will clobber if you've already stored it.
    void store( const Slab::HashID &key, const SlabReference &r )
    {
        m_map[key] = r;
    }

protected:
    typedef MD5Hash::UnorderedMapUtil<SlabReference>::umap_type Map;
    
    Map m_map;
};

} // End namespace Asset
} // End namespace Alembic

#endif
