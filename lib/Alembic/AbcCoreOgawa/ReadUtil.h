//-*****************************************************************************
//
// Copyright (c) 2013,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#ifndef _Alembic_AbcCoreOgawa_ReadUtil_h_
#define _Alembic_AbcCoreOgawa_ReadUtil_h_

#include <Alembic/AbcCoreOgawa/Foundation.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// UTILITY THING
//-*****************************************************************************

//-*****************************************************************************
void
ReadDimensions( Ogawa::IDataPtr iDims,
                Ogawa::IDataPtr iData,
                size_t iThreadId,
                const AbcA::DataType &iDataType,
                Util::Dimensions & oDim );

//-*****************************************************************************
void
ReadData( void * iIntoLocation,
          Ogawa::IDataPtr iData,
          size_t iThreadId,
          const AbcA::DataType &iDataType,
          Util::PlainOldDataType iAsPod );

//-*****************************************************************************
void
ReadArraySample( Ogawa::IDataPtr iDims,
                 Ogawa::IDataPtr iData,
                 size_t iThreadId,
                 const AbcA::DataType &iDataType,
                 AbcA::ArraySamplePtr &oSample );

//-*****************************************************************************
void
ReadTimeSamplesAndMax( Ogawa::IDataPtr iData,
                       std::vector <  AbcA::TimeSamplingPtr > & oTimeSamples,
                       std::vector <  AbcA::index_t > & oMaxSamples );

//-*****************************************************************************
void
ReadObjectHeaders( Ogawa::IGroupPtr iGroup,
                   size_t iIndex,
                   size_t iThreadId,
                   const std::string & iParentName,
                   const std::vector< AbcA::MetaData > & iMetaDataVec,
                   std::vector< ObjectHeaderPtr > & oHeaders );

//-*****************************************************************************
void
ReadPropertyHeaders( Ogawa::IGroupPtr iGroup,
                     size_t iIndex,
                     size_t iThreadId,
                     AbcA::ArchiveReader & iArchive,
                     const std::vector< AbcA::MetaData > & iMetaDataVec,
                     PropertyHeaderPtrs & oHeaders );

//-*****************************************************************************
void
ReadIndexedMetaData( Ogawa::IDataPtr iData,
                     std::vector< AbcA::MetaData > & oMetaDataVec );

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
