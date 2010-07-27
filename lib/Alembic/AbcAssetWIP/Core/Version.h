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

#ifndef _Alembic_Core_Version_h_
#define _Alembic_Core_Version_h_

#include <Alembic/Core/Foundation.h>

#define ALEMBIC_CORE_MAJOR_VERSION (Alembic::Core::MajorVersion())
#define ALEMBIC_CORE_MINOR_VERSION (Alembic::Core::MinorVersion())
#define ALEMBIC_CORE_PATCH_VERSION (Alembic::Core::PatchVersion())
#define ALEMBIC_CORE_TWEAK_VERSION (Alembic::Core::TweakVersion())
#define ALEMBIC_CORE_VERSION_STRING (Alembic::Core::FullVersionString())

extern "C" {

    extern int Alembic_Core_Major_Version;
    extern int Alembic_Core_Minor_Version;
    extern int Alembic_Core_Patch_Version;
    extern int Alembic_Core_Tweak_Version;

}; // End extern "C"

namespace Alembic {
namespace Core {

inline int MajorVersion() { return Alembic_Core_Major_Version; }
inline int MinorVersion() { return Alembic_Core_Minor_Version; }
inline int PatchVersion() { return Alembic_Core_Patch_Version; }
inline int TweakVersion() { return Alembic_Core_Tweak_Version; }

const char *FullVersionString();

} // End namespace Core
} // End namespace Alembic

#endif
