//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
#include "SampleUtil.h"
#include <ri.h>

//-*****************************************************************************
void WriteMotionBegin( ProcArgs &args, const SampleTimeSet &sampleTimes )
{
    std::vector<RtFloat> outputTimes;
    outputTimes.reserve( sampleTimes.size() );

    chrono_t frameTime = args.frame / args.fps;

    for ( SampleTimeSet::const_iterator iter = sampleTimes.begin();
          iter != sampleTimes.end() ; ++iter )
    {
        // why is this static?
        static const chrono_t epsilon = 1.0 / 10000.0;

        RtFloat value = ( (*iter) - frameTime ) * args.fps;

        if ( fabs( value ) < epsilon )
        {
            value = 0.0f;
        }

        outputTimes.push_back( value );
    }

    RiMotionBeginV( outputTimes.size(), &outputTimes[0] );
}

//-*****************************************************************************
void WriteConcatTransform( const M44d &m )
{
    RtMatrix rtm;

    for ( int row = 0; row < 4; ++row )
    {
        for ( int col = 0; col < 4; ++col )
        {
            rtm[row][col] = (RtFloat)( m[row][col] );
        }
    }

    RiConcatTransform( rtm );
}

//-*****************************************************************************
void GetRelevantSampleTimes( ProcArgs &args, TimeSamplingPtr timeSampling,
                            size_t numSamples, SampleTimeSet &output )
{
    if ( numSamples < 2 )
    {
        output.insert( 0.0 );
        return;
    }

    chrono_t frameTime = args.frame / args.fps;

    chrono_t shutterOpenTime = ( args.frame + args.shutterOpen ) / args.fps;

    chrono_t shutterCloseTime = ( args.frame + args.shutterClose ) / args.fps;

    std::pair<index_t, chrono_t> shutterOpenFloor =
        timeSampling->getFloorIndex( shutterOpenTime, numSamples );

    std::pair<index_t, chrono_t> shutterCloseCeil =
        timeSampling->getCeilIndex( shutterCloseTime, numSamples );

    //TODO, what's a reasonable episilon?
    static const chrono_t epsilon = 1.0 / 10000.0;

    //check to see if our second sample is really the
    //floor that we want due to floating point slop
    //first make sure that we have at least two samples to work with
    if ( shutterOpenFloor.first < shutterCloseCeil.first )
    {
        //if our open sample is less than open time,
        //look at the next index time
        if ( shutterOpenFloor.second < shutterOpenTime )
        {
            chrono_t nextSampleTime =
                     timeSampling->getSampleTime( shutterOpenFloor.first + 1 );

            if ( fabs( nextSampleTime - shutterOpenTime ) < epsilon )
            {
                shutterOpenFloor.first += 1;
                shutterOpenFloor.second = nextSampleTime;
            }
        }
    }


    for ( index_t i = shutterOpenFloor.first; i < shutterCloseCeil.first; ++i )
    {
        output.insert( timeSampling->getSampleTime( i ) );
    }

    //no samples above? put frame time in there and get out
    if ( output.size() == 0 )
    {
        output.insert( frameTime );
        return;
    }

    chrono_t lastSample = *(output.rbegin() );

    //determine whether we need the extra sample at the end
    if ( ( fabs( lastSample - shutterCloseTime ) > epsilon )
         && lastSample < shutterCloseTime )
    {
        output.insert( shutterCloseCeil.second );
    }
}
