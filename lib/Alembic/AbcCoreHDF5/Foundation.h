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

#ifndef _Alembic_AbcCoreHDF5_Foundation_h_
#define _Alembic_AbcCoreHDF5_Foundation_h_

#include <Alembic/AbcCoreAbstract/All.h>

#include <Alembic/MD5Hash/All.h>

#include <Alembic/Util/All.h>

#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/operators.hpp>
#include <boost/utility.hpp>

#include <vector>
#include <string>
#include <map>

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#if 0

//-*****************************************************************************
// HDF5 Header Files have lots of pre-processor definitions.
// Rather than precariously rely on the build system to propagate these
// at all times, we'll just wrap HDF5 includes with cpp directives.
//-*****************************************************************************

//-*****************************************************************************
// Make it so we can restore.
#ifdef _POSIX_SOURCE
#define RESTORE__POSIX_SOURCE _POSIX_SOURCE
#undef _POSIX_SOURCE
#endif

#ifdef NDEBUG
#define RESTORE_NDEBUG NDEBUG
#undef NDEBUG
#endif

#ifdef H5_DEBUG_API
#define RESTORE_H5_DEBUG_API H5_DEBUG_API
#undef H5_DEBUG_API
#endif

#ifdef _LARGEFILE_SOURCE
#define RESTORE__LARGEFILE_SOURCE _LARGEFILE_SOURCE
#undef _LARGEFILE_SOURCE
#endif

#ifdef _LARGEFILE64_SOURCE
#define RESTORE__LARGEFILE64_SOURCE _LARGEFILE64_SOURCE
#undef _LARGEFILE64_SOURCE
#endif

#ifdef _FILE_OFFSET_BITS
#define RESTORE__FILE_OFFSET_BITS _FILE_OFFSET_BITS
#undef _FILE_OFFSET_BITS
#endif

#ifdef _BSD_SOURCE
#define RESTORE__BSD_SOURCE _BSD_SOURCE
#undef _BSD_SOURCE
#endif

//-*****************************************************************************
// Define and include
#define _POSIX_SOURCE 1
#define NDEBUG 1
#undef H5_DEBUG_API
#define _LARGEFILE_SOURCE 1
#define _LARGEFILE64_SOURCE 1
#define _FILE_OFFSET_BITS 64
#define _BSD_SOURCE 1

#endif

#include <hdf5.h>
#include <hdf5_hl.h>

#include <H5LTpublic.h>

#if 0

#undef _POSIX_SOURCE
#undef NDEBUG
#undef _LARGEFILE_SOURCE
#undef _LARGEFILE64_SOURCE
#undef _FILE_OFFSET_BITS
#undef _BSD_SOURCE

//-*****************************************************************************
// Restore
#ifdef RESTORE__POSIX_SOURCE
#define _POSIX_SOURCE RESTORE__POSIX_SOURCE
#undef RESTORE__POSIX_SOURCE
#endif

#ifdef RESTORE_NDEBUG
#define NDEBUG RESTORE_NDEBUG
#undef RESTORE_NDEBUG
#endif

#ifdef RESTORE_H5_DEBUG_API
#define H5_DEBUG_API RESTORE_H5_DEBUG_API
#undef RESTORE_H5_DEBUG_API
#endif

#ifdef RESTORE__LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE RESTORE__LARGEFILE_SOURCE
#undef RESTORE__LARGEFILE_SOURCE
#endif

#ifdef RESTORE__LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE RESTORE__LARGEFILE64_SOURCE
#undef RESTORE__LARGEFILE64_SOURCE
#endif

#ifdef RESTORE__FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS RESTORE__FILE_OFFSET_BITS
#undef RESTORE__FILE_OFFSET_BITS
#endif

#ifdef RESTORE__BSD_SOURCE
#define _BSD_SOURCE RESTORE__BSD_SOURCE
#undef RESTORE__BSD_SOURCE
#endif

#endif

//-*****************************************************************************

namespace Alembic {
namespace AbcCoreHDF5 {

//-*****************************************************************************
namespace AbcA = ::Alembic::AbcCoreAbstract::v1;

using namespace ::Alembic::Util;
using AbcA::index_t;
using AbcA::chrono_t;

using namespace ::Alembic::MD5Hash;

//-*****************************************************************************
// MAGIC NUMBER FOR COMPOUND PROPERTY TYPES
static const uint16_t COMPOUND_MAGIC =
    ( uint16_t )BOOST_BINARY( 1101 1011 1101 1100 );

//-*****************************************************************************
typedef boost::weak_ptr<AbcA::ObjectWriter> WeakOwPtr;
typedef boost::weak_ptr<AbcA::BasePropertyWriter> WeakBpwPtr;

typedef boost::weak_ptr<AbcA::ObjectReader> WeakOrPtr;
typedef boost::weak_ptr<AbcA::BasePropertyReader> WeakBprPtr;

//-*****************************************************************************
typedef boost::shared_ptr<AbcA::PropertyHeader> PropertyHeaderPtr;
typedef std::vector<PropertyHeaderPtr> PropertyHeaderPtrs;

typedef boost::shared_ptr<AbcA::ObjectHeader> ObjectHeaderPtr;

//-*****************************************************************************
inline std::string getSampleName( const std::string &iName,
                                  index_t iSampleIndex )
{
    if ( iSampleIndex == 0 )
    {
        return iName + ".smp0";
    }
    else
    {
        return ( boost::format( ".smp_%08d" ) % iSampleIndex ).str();
    }
}

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
