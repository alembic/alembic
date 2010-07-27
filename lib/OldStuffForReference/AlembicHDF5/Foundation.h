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

#ifndef _AlembicHDF5_Foundation_h_
#define _AlembicHDF5_Foundation_h_

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

#include <hdf5.h>
#include <hdf5_hl.h>

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
