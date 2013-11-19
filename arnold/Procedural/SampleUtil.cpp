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

#include <algorithm>
#include <ImathMatrix.h>
#include <ImathMatrixAlgo.h>
#include <ImathQuat.h>
#include <ImathEuler.h>

//-*****************************************************************************
void GetRelevantSampleTimes( ProcArgs &args, TimeSamplingPtr timeSampling,
                            size_t numSamples, SampleTimeSet &output,
                            MatrixSampleMap * inheritedSamples)
{
    if ( numSamples < 2 )
    {
        output.insert( 0.0 );
        return;
    }

    chrono_t frameTime = args.frame / args.fps;

    chrono_t shutterOpenTime = ( args.frame + args.shutterOpen ) / args.fps;

    chrono_t shutterCloseTime = ( args.frame + args.shutterClose ) / args.fps;


    // For interpolating and concatenating samples, we need to consider
    // possible inherited sample times outside of our natural shutter range
    if (inheritedSamples && inheritedSamples->size() > 1)
    {
        shutterOpenTime = std::min(shutterOpenTime,
                inheritedSamples->begin()->first);
        shutterCloseTime = std::max(shutterCloseTime,
                inheritedSamples->rbegin()->first);
    }



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

//-*****************************************************************************

namespace
{

    void DecomposeXForm(
            const Imath::M44d &mat,
            Imath::V3d &scale,
            Imath::V3d &shear,
            Imath::Quatd &rotation,
            Imath::V3d &translation
    )
    {
        Imath::M44d mat_remainder(mat);

        // Extract Scale, Shear
        Imath::extractAndRemoveScalingAndShear(mat_remainder, scale, shear);

        // Extract translation
        translation.x = mat_remainder[3][0];
        translation.y = mat_remainder[3][1];
        translation.z = mat_remainder[3][2];

        // Extract rotation
        rotation = extractQuat(mat_remainder);
    }

    M44d RecomposeXForm(
            const Imath::V3d &scale,
            const Imath::V3d &shear,
            const Imath::Quatd &rotation,
            const Imath::V3d &translation
    )
    {
        Imath::M44d scale_mtx, shear_mtx, rotation_mtx, translation_mtx;

        scale_mtx.setScale(scale);
        shear_mtx.setShear(shear);
        rotation_mtx = rotation.toMatrix44();
        translation_mtx.setTranslation(translation);

        return scale_mtx * shear_mtx * rotation_mtx * translation_mtx;
    }


    // when amt is 0, a is returned
    inline double lerp(double a, double b, double amt)
    {
        return (a + (b-a)*amt);
    }


    Imath::V3d lerp(const Imath::V3d &a, const Imath::V3d &b, double amt)
    {
        return Imath::V3d(lerp(a[0], b[0], amt),
                          lerp(a[1], b[1], amt),
                          lerp(a[2], b[2], amt));
    }


    M44d GetNaturalOrInterpolatedSampleForTime(const MatrixSampleMap & samples,
            Abc::chrono_t sampleTime)
    {
        MatrixSampleMap::const_iterator I = samples.find(sampleTime);
        if (I != samples.end())
        {
            return (*I).second;
        }

        if (samples.empty())
        {
            return M44d();
        }

        if (samples.size() == 1)
        {
            return samples.begin()->second;
        }

        if (sampleTime <= samples.begin()->first)
        {
            return samples.begin()->second;
        }

        if (sampleTime >= samples.rbegin()->first)
        {
            return samples.rbegin()->second;
        }

        //find the floor and ceiling samples and interpolate
        Abc::chrono_t lTime = samples.begin()->first;
        Abc::chrono_t rTime = samples.rbegin()->first;



        for (MatrixSampleMap::const_iterator I = samples.begin();
                I != samples.end(); ++I)
        {
            Abc::chrono_t testSampleTime= (*I).first;

            if (testSampleTime > lTime && testSampleTime <= sampleTime)
            {
                lTime = testSampleTime;
            }
            if (testSampleTime > rTime && testSampleTime >= sampleTime)
            {
                rTime = testSampleTime;
            }
        }


        M44d mtx_l;
        M44d mtx_r;

        {
            MatrixSampleMap::const_iterator I;

            I = samples.find(lTime);
            if (I != samples.end())
            {
                mtx_l = (*I).second;
            }

            I = samples.find(rTime);
            if (I != samples.end())
            {
                mtx_r = (*I).second;
            }




        }

        Imath::V3d s_l,s_r,h_l,h_r,t_l,t_r;
        Imath::Quatd quat_l,quat_r;

        DecomposeXForm(mtx_l, s_l, h_l, quat_l, t_l);
        DecomposeXForm(mtx_r, s_r, h_r, quat_r, t_r);

        Abc::chrono_t amt = (sampleTime-lTime) / (rTime-lTime);

        if ((quat_l ^ quat_r) < 0)
        {
            quat_r = -quat_r;
        }

        return RecomposeXForm(lerp(s_l, s_r, amt),
                                 lerp(h_l, h_r, amt),
                                 Imath::slerp(quat_l, quat_r, amt),
                                 lerp(t_l, t_r, amt));



    }



}

//-*****************************************************************************

void ConcatenateXformSamples( ProcArgs &args,
        const MatrixSampleMap & parentSamples,
        const MatrixSampleMap & localSamples,
        MatrixSampleMap & outputSamples)
{
    SampleTimeSet unionOfSampleTimes;

    for (MatrixSampleMap::const_iterator I = parentSamples.begin();
            I != parentSamples.end(); ++I)
    {
        unionOfSampleTimes.insert((*I).first);
    }

    for (MatrixSampleMap::const_iterator I = localSamples.begin();
            I != localSamples.end(); ++I)
    {
        unionOfSampleTimes.insert((*I).first);
    }

    for (SampleTimeSet::iterator I = unionOfSampleTimes.begin();
            I != unionOfSampleTimes.end(); ++I)
    {
        M44d parentMtx = GetNaturalOrInterpolatedSampleForTime(parentSamples,
                (*I));
        M44d localMtx = GetNaturalOrInterpolatedSampleForTime(localSamples,
                (*I));

        outputSamples[(*I)] = localMtx * parentMtx;
    }
}

//-*****************************************************************************

Abc::chrono_t GetRelativeSampleTime( ProcArgs &args, Abc::chrono_t sampleTime)
{
    const chrono_t epsilon = 1.0 / 10000.0;


    chrono_t frameTime = args.frame / args.fps;

    Abc::chrono_t result = ( sampleTime - frameTime ) * args.fps;

    if ( fabs( result ) < epsilon )
    {
        result = 0.0;
    }

    return result;
}


