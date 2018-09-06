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

#ifndef Alembic_AbcCoreLayer_Util_h
#define Alembic_AbcCoreLayer_Util_h

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreLayer/Foundation.h>
#include <Alembic/Util/Export.h>

namespace Alembic {
namespace AbcCoreLayer {
namespace ALEMBIC_VERSION_NS {

// Convience functions for marking prune and replace on MetaData to be used
// with layering.

//! Used to mark that an Alembic object or property is meant to be pruned
//! when read via AbcCoreLayer.  Pruning an object or compound property will
//! also removes all of their children.
ALEMBIC_EXPORT void SetPrune(
    Alembic::AbcCoreAbstract::MetaData & oMetaData, bool shouldPrune );

//! Used to mark that an Alembic object or property is meant to be replaced
//! when read via AbcCoreLayer.  Replacing an object or compound property will
//! also replace all of the children encountered so far.  Since pruning is more
//! destructive it trumps replace.
ALEMBIC_EXPORT void SetReplace(
    Alembic::AbcCoreAbstract::MetaData & oMetaData, bool shouldReplace );

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreLayer
} // End namespace Alembic

#endif //_Alembic_AbcCoreLayer_Util_h_
