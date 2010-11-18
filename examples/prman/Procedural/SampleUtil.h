#ifndef _Alembic_Prman_SampleUtil_h_
#define _Alembic_Prman_SampleUtil_h_

#include <Alembic/AbcGeom/All.h>

#include "ProcArgs.h"
#include <set>

using namespace Alembic;

typedef std::set<Abc::chrono_t> SampleTimeSet;

void GetRelevantSampleTimes(
        ProcArgs & args,
        Abc::TimeSampling & timeSampling,
        SampleTimeSet & output);

void WriteMotionBegin(ProcArgs & args,
        const SampleTimeSet & sampleTimes);

void WriteConcatTransform(const Abc::M44d & m);

#endif
