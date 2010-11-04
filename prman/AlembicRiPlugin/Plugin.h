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

#ifndef _AlembicRiPlugin_Plugin_h_
#define _AlembicRiPlugin_Plugin_h_

#include "Foundation.h"
#include "Factory.h"
#include "ParentObject.h"
#include "Asset.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
// The plugin is a singleton that only one may exist of in a given
// load of the dynamic shared object's object space.
// Assets should only be loaded once per instantiation, or the underlying
// libraries will have difficulty with their resource management.
// Therefore, this global instance maintains a list of assets by filename.
// It is not clever enough to distinguish between multiple paths to the
// same filename.
//
// Just using a standard factory right now.
class Plugin : public boost::noncopyable
{
public:
    Plugin();

    AssetPtr asset( const std::string &fileName );

private:
    typedef std::map<std::string, AssetPtr> AssetMap;

    StdFactory m_factory;
    Abc::IContext m_context;
    AssetMap m_map;
    Mutex m_mutex;
};

//-*****************************************************************************
typedef boost::shared_ptr<Plugin> PluginPtr;

} // End namespace AlembicRiPlugin

#endif
