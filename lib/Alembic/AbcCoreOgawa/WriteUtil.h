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

#ifndef _Alembic_AbcCoreOgawa_WriteUtil_h_
#define _Alembic_AbcCoreOgawa_WriteUtil_h_

#include <Alembic/AbcCoreOgawa/Foundation.h>
#include <Alembic/AbcCoreOgawa/WrittenSampleMap.h>
#include <Alembic/AbcCoreOgawa/MetaDataMap.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
void HashPropertyHeader( const AbcA::PropertyHeader & iHeader,
                         Util::SpookyHash & ioHash );

//-*****************************************************************************
// Hashes the dimensions and the current sample hash (ioHash), and then stores
// the result on ioHash.
void HashDimensions( const AbcA::Dimensions & iDims,
                     Util::Digest & ioHash );

//-*****************************************************************************
WrittenSampleMap& GetWrittenSampleMap(
    AbcA::ArchiveWriterPtr iArchive );

//-*****************************************************************************
void
WriteDimensions( Ogawa::OGroupPtr iGroup,
                 const AbcA::Dimensions & iDims,
                 Alembic::Util::PlainOldDataType iPod );

//-*****************************************************************************
void
CopyWrittenData( Ogawa::OGroupPtr iParent,
                 WrittenSampleIDPtr iRef );

//-*****************************************************************************
WrittenSampleIDPtr
WriteData( WrittenSampleMap &iMap,
           Ogawa::OGroupPtr iGroup,
           const AbcA::ArraySample &iSamp,
           const AbcA::ArraySample::Key &iKey );

//-*****************************************************************************
void
WritePropertyInfo( std::vector< Util::uint8_t > & ioData,
                   const AbcA::PropertyHeader &iHeader,
                   bool isScalarLike,
                   bool isHomogenous,
                   Util::uint32_t iTimeSamplingIndex,
                   Util::uint32_t iNumSamples,
                   Util::uint32_t iFirstChangedIndex,
                   Util::uint32_t iLastChangedIndex,
                   MetaDataMapPtr iMap );

//-*****************************************************************************
void
WriteObjectHeader( std::vector< Util::uint8_t > & ioData,
                   const AbcA::ObjectHeader &iHeader,
                   MetaDataMapPtr iMap );

//-*****************************************************************************
void
WriteTimeSampling( std::vector< Util::uint8_t > & ioData,
                   Util::uint32_t  iMaxSample,
                   const AbcA::TimeSampling &iTsmp );

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif

