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

#ifndef _AlembicRiPlugin_Instancer_h_
#define _AlembicRiPlugin_Instancer_h_

#include "Foundation.h"
#include "Plugin.h"
#include "TokenMap.h"
#include "Asset.h"

namespace AlembicRiPlugin {

typedef std::vector<std::string> StringVec;

//-*****************************************************************************
// INSTANCER CLASS
class Instancer
{
public:
    Instancer( PluginPtr pptr,
               Mutex &mtx,
               const std::string &configStr )
      : m_plugin( pptr ),
        m_mutex( mtx ),
        m_tokenMap( configStr ) {}

    void subdivide( RtFloat detail );

    const std::string makeConfigString( const std::string &assetName,
                                        const std::string &objectName,
                                        const float samplesPerSecond,
                                        const float motionSampleBias,
                                        const float motionSampleGain,
                                        const ChronoSamples &csamps,
                                        const bool isBounded );

    Abc::Box3d getBounds( const std::string &objectName,
                          const TimeSamples &tsamps );

protected:
    PluginPtr m_plugin;
    TokenMap m_tokenMap;
    AssetPtr m_asset;
    Mutex &m_mutex;
};

} // End namespace AlembicRiPlugin

#endif
