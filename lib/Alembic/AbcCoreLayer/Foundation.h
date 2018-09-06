//-*****************************************************************************
//
// Copyright (c) 2016,
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

#ifndef Alembic_AbcCoreLayer_Foundation_h
#define Alembic_AbcCoreLayer_Foundation_h

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/Util/All.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

namespace AbcA = ::Alembic::AbcCoreAbstract;

typedef std::map< std::string, size_t > ChildNameMap;

class ArImpl;
typedef Alembic::Util::shared_ptr< ArImpl > ArImplPtr;

class OrImpl;
typedef Alembic::Util::shared_ptr< OrImpl > OrImplPtr;

typedef Alembic::Util::shared_ptr<AbcA::ObjectHeader> ObjectHeaderPtr;

class CprImpl;
typedef Alembic::Util::shared_ptr< CprImpl > CprImplPtr;

typedef std::vector< Alembic::AbcCoreAbstract::ArchiveReaderPtr >
        ArchiveReaderPtrs;

typedef std::vector< Alembic::AbcCoreAbstract::CompoundPropertyReaderPtr >
        CompoundReaderPtrs;

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_Foundation_h_
