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

#include "RiUtil.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
void ConcatTransform( const Abc::M44f &xform )
{
    RtMatrix rtm;
    for ( int row = 0; row < 4; ++row )
    {
        for ( int col = 0; col < 4; ++col )
        {
            rtm[row][col] = ( RtFloat )( xform[row][col] );
        }
    }
    ::RiConcatTransform( rtm );
}

//-*****************************************************************************
void ConcatTransform( const Abc::M44d &xform )
{
    RtMatrix rtm;
    for ( int row = 0; row < 4; ++row )
    {
        for ( int col = 0; col < 4; ++col )
        {
            rtm[row][col] = ( RtFloat )( xform[row][col] );
        }
    }
    ::RiConcatTransform( rtm );
}

//-*****************************************************************************
void Transform( const Abc::M44f &xform )
{
    RtMatrix rtm;
    for ( int row = 0; row < 4; ++row )
    {
        for ( int col = 0; col < 4; ++col )
        {
            rtm[row][col] = ( RtFloat )( xform[row][col] );
        }
    }
    ::RiTransform( rtm );
}

//-*****************************************************************************
void Transform( const Abc::M44d &xform )
{
    RtMatrix rtm;
    for ( int row = 0; row < 4; ++row )
    {
        for ( int col = 0; col < 4; ++col )
        {
            rtm[row][col] = ( RtFloat )( xform[row][col] );
        }
    }
    ::RiTransform( rtm );
}

//-*****************************************************************************
void MotionBegin( const TimeSamples &tsmp, const float motionSampleBias = 0.0f,
                  const float motionSampleGain = 1.0f )
{
    size_t numVals = tsmp.size();
    if ( numVals < 2 )
    {
        ABCRI_ERROR( "Cannot call RiMotionBegin with < 2 time samples." );
        return;
    }

    std::vector<RtFloat> vals( numVals );
    for ( size_t i = 0; i < numVals; ++i )
    {
        vals[i] = ( RtFloat )( ( tsmp[i].value() * motionSampleGain )
                               + motionSampleBias );
    }

    ::RiMotionBeginV( numVals, &vals.front() );
}

} // End namespace AlembicRiPlugin
