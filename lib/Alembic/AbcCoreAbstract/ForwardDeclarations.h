//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef Alembic_AbcCoreAbstract_ForwardDeclarations_h
#define Alembic_AbcCoreAbstract_ForwardDeclarations_h

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/PropertyHeader.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
//! Helper types forward declared.
//! ...
class TimeSampling;
class ArraySample;

//-*****************************************************************************
//! Writer types forward declared.
//! ...
class ArchiveWriter;
class ObjectWriter;
class CompoundPropertyWriter;
class ArrayPropertyWriter;
class ScalarPropertyWriter;
class BasePropertyWriter;

//-*****************************************************************************
//! Reader types forward declared.
//! ...
class ArchiveReader;
class ObjectReader;
class CompoundPropertyReader;
class ArrayPropertyReader;
class ScalarPropertyReader;
class BasePropertyReader;

//-*****************************************************************************
//! Smart Ptrs to Helper types.
//! The Ptr suffix in Alembic _ALWAYS_ refers to a shared_ptr of whatever
//! class name precedes the Ptr suffix.
typedef Alembic::Util::shared_ptr<ArraySample> ArraySamplePtr;

//-*****************************************************************************
//! Smart Ptrs to Writers.
typedef Alembic::Util::shared_ptr<ArchiveWriter> ArchiveWriterPtr;
typedef Alembic::Util::shared_ptr<ObjectWriter> ObjectWriterPtr;
typedef Alembic::Util::shared_ptr<CompoundPropertyWriter> CompoundPropertyWriterPtr;
typedef Alembic::Util::shared_ptr<ArrayPropertyWriter> ArrayPropertyWriterPtr;
typedef Alembic::Util::shared_ptr<ScalarPropertyWriter> ScalarPropertyWriterPtr;
typedef Alembic::Util::shared_ptr<BasePropertyWriter> BasePropertyWriterPtr;

//-*****************************************************************************
//! Smart Ptrs to Readers.
typedef Alembic::Util::shared_ptr<ArchiveReader> ArchiveReaderPtr;
typedef Alembic::Util::shared_ptr<ObjectReader> ObjectReaderPtr;
typedef Alembic::Util::shared_ptr<CompoundPropertyReader> CompoundPropertyReaderPtr;
typedef Alembic::Util::shared_ptr<ArrayPropertyReader> ArrayPropertyReaderPtr;
typedef Alembic::Util::shared_ptr<ScalarPropertyReader> ScalarPropertyReaderPtr;
typedef Alembic::Util::shared_ptr<BasePropertyReader> BasePropertyReaderPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
