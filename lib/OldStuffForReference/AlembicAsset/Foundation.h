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

#ifndef _AlembicAsset_Foundation_h_
#define _AlembicAsset_Foundation_h_

#include <AlembicAsset/Base/FoundationBase.h>
#include <AlembicAsset/Base/ITimeSampling.h>
#include <boost/smart_ptr.hpp>
#include <string>

namespace AlembicAsset {

//-*****************************************************************************
// Forwards
class OParentBody;
class OAssetBody;
class OObjectBody;
class OPropertyBody;
class OSingularPropertyBody;
class OMultiPropertyBody;

class IContextBody;
class IParentBody;
class IAssetBody;
class IObjectBody;
class IPropertyBody;
class ISingularPropertyBody;
class IMultiPropertyBody;

//-*****************************************************************************
typedef boost::shared_ptr<OParentBody> SharedOParentBody;
typedef boost::shared_ptr<OAssetBody> SharedOAssetBody;
typedef boost::shared_ptr<OObjectBody> SharedOObjectBody;
typedef boost::shared_ptr<OPropertyBody> SharedOPropertyBody;
typedef boost::shared_ptr<OSingularPropertyBody> SharedOSingularPropertyBody;
typedef boost::shared_ptr<OMultiPropertyBody> SharedOMultiPropertyBody;

typedef boost::shared_ptr<IContextBody> SharedIContextBody;
typedef boost::shared_ptr<IParentBody> SharedIParentBody;
typedef boost::shared_ptr<IAssetBody> SharedIAssetBody;
typedef boost::shared_ptr<IObjectBody> SharedIObjectBody;
typedef boost::shared_ptr<IPropertyBody> SharedIPropertyBody;
typedef boost::shared_ptr<ISingularPropertyBody> SharedISingularPropertyBody;
typedef boost::shared_ptr<IMultiPropertyBody> SharedIMultiPropertyBody;

//-*****************************************************************************
enum ThrowExceptionFlag { kThrowException = 0 };
enum NoThrowExceptionFlag { kNoThrowException = 1 };

} // End namespace AlembicAsset

#endif
