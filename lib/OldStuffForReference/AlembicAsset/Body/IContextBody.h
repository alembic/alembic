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

#ifndef _AlembicAsset_Body_IContextBody_h_
#define _AlembicAsset_Body_IContextBody_h_

#include <AlembicAsset/Base/ISlabCache.h>
#include <AlembicAsset/Body/DataTypeTuple.h>

namespace AlembicAsset {

//-*****************************************************************************
// This class is a repository for global data that is needed by objects
// at read time. This amounts to the ISlabBufferCache and the
// DataTypeTupleMap, for now. Obvious place to put configuration data
// and performance statistics.
class IContextBody
{
public:
    IContextBody() {}

    const ISlabCache &slabCache() const
    { return m_slabCache; }
    ISlabCache &slabCache()
    { return m_slabCache; }

    const DataTypeTupleMap &dataTypeTupleMap() const
    { return m_dataTypeTupleMap; }
    DataTypeTupleMap &dataTypeTupleMap()
    { return m_dataTypeTupleMap; }

protected:
    ISlabCache m_slabCache;
    DataTypeTupleMap m_dataTypeTupleMap;
};

typedef boost::shared_ptr<IContextBody> SharedIContextBody;

} // End namespace AlembicAsset

#endif
