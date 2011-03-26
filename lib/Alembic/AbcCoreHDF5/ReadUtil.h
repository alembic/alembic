//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _Alembic_AbcCoreHDF5_ReadUtil_h_
#define _Alembic_AbcCoreHDF5_ReadUtil_h_

#include <Alembic/AbcCoreHDF5/Foundation.h>
#include <Alembic/AbcCoreHDF5/StringReadUtil.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// UTILITY THING
//-*****************************************************************************

//-*****************************************************************************
bool
ReadKey( hid_t iHashDset,
         const std::string &iAttrName,
         AbcA::ArraySample::Key &oKey );

//-*****************************************************************************
bool
ReadMetaData( hid_t iGroup,
              const std::string &iMetaDataName,
              AbcA::MetaData &oMetaData );

//-*****************************************************************************
void
ReadPropertyHeader( hid_t iGroup,
                    const std::string &iPropName,
                    AbcA::PropertyHeader &oHeader );

//-*****************************************************************************
void
ReadScalar( hid_t iGroup,
            const std::string &iScalarName,
            hid_t iFileType,
            hid_t iNativeType,
            void *oData );

//-*****************************************************************************
void
ReadSmallArray( hid_t iGroup,
                const std::string &iAttrName,
                hid_t iFileType,
                hid_t iNativeType,
                size_t iMaxElems,
                size_t &oReadElems,
                void *oData );

//-*****************************************************************************
void
ReadDimensions( hid_t iParent,
                const std::string &iName,
                Dimensions &oDims );

//-*****************************************************************************
AbcA::ArraySamplePtr 
ReadArray( AbcA::ReadArraySampleCachePtr iCache,
           hid_t iGroup,
           const std::string &iArrayName,
           const AbcA::DataType &iDataType,
           hid_t iFileType,
           hid_t iNativeType );

//-*****************************************************************************
// Returns whether or not info was read directly, or inferred.
bool
ReadNumSamples( hid_t iGroup,
                const std::string &iPropName,
                bool isScalar,
                uint32_t &oNumSamples,
                uint32_t &oNumUniqueSamples );

//-*****************************************************************************
// Returns whether it actually found the data you're writing out (true)
// or whether you're faking an array of 1 double == 0.0 (false)
// It returns them as an ArraySamplePtr, because that is the object which
// contains the resource management and therefore we want store a reference
// to it for storage. It is trivially convertable to a ChronoArrayPtr.
AbcA::ArraySamplePtr
ReadTimeSamples( AbcA::ReadArraySampleCachePtr iCache,
                 hid_t iGroup,
                 const std::string &iPropName );

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
