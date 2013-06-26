//-*****************************************************************************
//
// Copyright (c) 2013,
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

#ifndef _Alembic_AbcCoreOgawa_WrittenSampleMap_h_
#define _Alembic_AbcCoreOgawa_WrittenSampleMap_h_

#include <Alembic/AbcCoreAbstract/ArraySampleKey.h>
#include <Alembic/AbcCoreOgawa/Foundation.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// A Written Sample ID is a receipt that contains information that
// refers to the exact location in an Ogawa file that a sample was written to.
//
// It also contains the Key of the sample, so it may be verified.
//
// This object is used to "reuse" an already written sample by linking
// it from the previous usage.
//-*****************************************************************************
class WrittenSampleID
{
public:
    WrittenSampleID()
    {
        m_sampleKey.numBytes = 0;
        m_sampleKey.origPOD = Alembic::Util::kInt8POD;
        m_sampleKey.readPOD = Alembic::Util::kInt8POD;
        m_numPoints = 0;
    }

    WrittenSampleID( const AbcA::ArraySample::Key &iKey,
                     Ogawa::ODataPtr iData,
                     std::size_t iNumPoints )
      : m_sampleKey( iKey ), m_data( iData ), m_numPoints( iNumPoints )
    {
    }

    const AbcA::ArraySample::Key &getKey() const { return m_sampleKey; }

    Ogawa::ODataPtr getObjectLocation() const { return m_data; }

    std::size_t getNumPoints() { return m_numPoints; }

private:
    AbcA::ArraySample::Key m_sampleKey;
    Ogawa::ODataPtr m_data;
    std::size_t m_numPoints;
};

//-*****************************************************************************
typedef Alembic::Util::shared_ptr<WrittenSampleID> WrittenSampleIDPtr;

//-*****************************************************************************
// This class handles the mapping.
class WrittenSampleMap
{
protected:
    friend class AwImpl;

    WrittenSampleMap() {}

public:

    // Returns 0 if it can't find it
    WrittenSampleIDPtr find( const AbcA::ArraySample::Key &key ) const
    {
        Map::const_iterator miter = m_map.find( key );
        if ( miter != m_map.end() )
        {
            return (*miter).second;
        }
        else
        {
            return WrittenSampleIDPtr();
        }
    }

    // Store. Will clobber if you've already stored it.
    void store( WrittenSampleIDPtr r )
    {
        if ( !r )
        {
            ABCA_THROW( "Invalid WrittenSampleIDPtr" );
        }

        m_map[r->getKey()] = r;
    }

    void clear()
    {
        m_map.clear();
    }

protected:
    typedef AbcA::UnorderedMapUtil<WrittenSampleIDPtr>::umap_type Map;
    Map m_map;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
