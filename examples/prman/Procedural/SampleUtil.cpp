#include "SampleUtil.h"
#include <ri.h>

using namespace Alembic;

///////////////////////////////////////////////////////////////////////////////

void WriteMotionBegin(ProcArgs & args,
        const SampleTimeSet & sampleTimes)
{
    std::vector<RtFloat> outputTimes;
    outputTimes.reserve(sampleTimes.size());
    
    Abc::chrono_t frameTime = args.frame / args.fps;
    
    for (SampleTimeSet::const_iterator I = sampleTimes.begin();
            I != sampleTimes.end(); ++I)
    {
        static const Abc::chrono_t epsilon = 1.0/10000.0;
        RtFloat value = ((*I) - frameTime) * args.fps;
        if (fabs(value) < epsilon)
        {
            value = 0;
        }
        
        outputTimes.push_back(value);
    }
    
    RiMotionBeginV(outputTimes.size(), &outputTimes[0]);
}

///////////////////////////////////////////////////////////////////////////////

void WriteConcatTransform(const Abc::M44d & m)
{
    RtMatrix rtm;
    
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            rtm[row][col] = (RtFloat)(m[row][col]);
        }
    }
    
    RiConcatTransform(rtm);
}

///////////////////////////////////////////////////////////////////////////////

void GetRelevantSampleTimes(
        ProcArgs & args,
        Abc::TimeSampling & timeSampling,
        SampleTimeSet & output)
{
    if (timeSampling.isStatic())
    {
        output.insert(0.0);
        return;
    }

    Abc::chrono_t frameTime = args.frame / args.fps;
    Abc::chrono_t shutterOpenTime =
            (args.frame + args.shutterOpen) / args.fps;
    Abc::chrono_t shutterCloseTime =
            (args.frame + args.shutterClose) / args.fps;
    
    std::pair<Abc::index_t, Abc::chrono_t> shutterOpenFloor =
            timeSampling.getFloorIndex(shutterOpenTime);
    std::pair<Abc::index_t, Abc::chrono_t> shutterCloseCeil =
            timeSampling.getCeilIndex(shutterCloseTime);
    
    //TODO, what's a reasonable episilon?
    static const Abc::chrono_t epsilon = 1.0/10000.0;
    
    //check to see if our second sample is really the
    //floor that we want due to floating point slop
    //first make sure that we have at least two samples to work with
    if (shutterOpenFloor.first < shutterCloseCeil.first)
    {
        //if our open sample is less than open time,
        //look at the next index time
        if (shutterOpenFloor.second < shutterOpenTime)
        {
            Abc::chrono_t nextSampleTime =
                     timeSampling.getSampleTime(shutterOpenFloor.first+1);
            
            if (fabs(nextSampleTime - shutterOpenTime) <
                    epsilon)
            {
                shutterOpenFloor.first += 1;
                shutterOpenFloor.second = nextSampleTime;
            }
        }
    }
    
    
    for (Abc::index_t i = shutterOpenFloor.first;
            i < shutterCloseCeil.first; ++i)
    {
        output.insert(timeSampling.getSampleTime(i));
    }
    
    //no samples above? put frame time in there and get out
    if (output.size() == 0)
    {
        output.insert(frameTime);
        return;
    }
    
    Abc::chrono_t lastSample = *(output.rbegin());
    
    //determine whether we need the extra sample at the end
    if ((fabs(lastSample-shutterCloseTime) > epsilon)
            && lastSample<shutterCloseTime)
    {
        output.insert(shutterCloseCeil.second);
    }
}
