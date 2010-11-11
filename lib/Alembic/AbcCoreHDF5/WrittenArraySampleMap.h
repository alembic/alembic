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

#ifndef _Alembic_AbcCoreHDF5_WrittenArraySampleMap_h_
#define _Alembic_AbcCoreHDF5_WrittenArraySampleMap_h_

#include <Alembic/AbcCoreHDF5/Foundation.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>

namespace Alembic {
namespace AbcCoreHDF5 {

//-*****************************************************************************
// A Written Array Sample ID is a receipt that contains information that
// refers to the exact location in an HDF5 file that an array sample was written
// to. It also contains the Key of the array sample and the DataType, so
// it may be verified.
//
// This object is used to "reuse" an already written array sample by linking
// it from the previous usage.
//-*****************************************************************************
class WrittenArraySampleID
{
public:
    WrittenArraySampleID()
      : m_sampleKey(),
        m_objectLocationID( 0 ) {}

    WrittenArraySampleID( const AbcA::ArraySample::Key &iKey,
                          hid_t iObjLocID)
      : m_sampleKey( iKey ),
        m_objectLocationID( iObjLocID ) {}

    const AbcA::ArraySample::Key &getKey() const { return m_sampleKey; }
    hid_t getObjectLocationID() const { return m_objectLocationID.m_id; }

private:
    AbcA::ArraySample::Key m_sampleKey;
    DsetCloser m_objectLocationID;
};

//-*****************************************************************************
typedef boost::shared_ptr<WrittenArraySampleID> WrittenArraySampleIDPtr;

//-*****************************************************************************
// This class handles the mapping.
class WrittenArraySampleMap
{
protected:
    friend class AwImpl;
    
    WrittenArraySampleMap() {}

public:
    // Returns 0 if it can't find it
    WrittenArraySampleIDPtr find( const AbcA::ArraySample::Key &key ) const
    {
        Map::const_iterator miter = m_map.find( key );
        if ( miter != m_map.end() )
        {
            return (*miter).second;
        }
        else
        {
            return WrittenArraySampleIDPtr();
        }
    }

    // Store. Will clobber if you've already stored it.
    void store( WrittenArraySampleIDPtr r )
    {
        if ( !r )
        {
            ABCA_THROW( "Invalid WrittenArraySampleIDPtr" );
        }
        
        m_map[r->getKey()] = r;
    }

protected:
    typedef UnorderedMapUtil<WrittenArraySampleIDPtr>::umap_type Map;
    
    Map m_map;
};

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
