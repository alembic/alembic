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

#ifndef _AlembicRiPlugin_Request_h_
#define _AlembicRiPlugin_Request_h_

#include "Foundation.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
class Request
{
public:
    Request( const TimeSamples &ts, const std::string &objectName,
             float motionSampleBias, float motionSampleGain,
             Mutex &mtx );
    virtual ~Request() {}

    struct Keep
    {
        Keep() : local( false ), recurse( false ) {}
        Keep( bool l, bool r ) : local( l ), recurse( r ) {}

        bool local;
        bool recurse;
        bool active() { return ( local || recurse ); }
    };

    Keep keep( const std::string &name,
               const std::string &fullPathName,
               const std::string &protocol );

    const TimeSamples &timeSamples() const { return m_timeSamples; }
    bool restPose() const { return ( m_timeSamples.size() == 0 ); }
    bool motionBlur() const { return ( m_timeSamples.size() > 1 ); }
    Mutex &mutex() { return m_mutex; }
    float motionSampleGain() const { return m_motionSampleGain; }
    float motionSampleBias() const { return m_motionSampleBias; }

protected:
    TimeSamples m_timeSamples;
    std::string m_objectName;
    bool m_doEverything;
    float m_motionSampleBias;
    float m_motionSampleGain;
    Mutex &m_mutex;
};

} // End namespace AlembicRiPlugin

#endif
