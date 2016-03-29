//-*****************************************************************************
//
// Copyright (c) 2013,
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

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreFactory/All.h>

double getTimeSec()
{
    timeval t;
    gettimeofday(&t, 0);
    return (double) t.tv_sec + (double) t.tv_usec / 1000000.0;
}

using namespace Alembic;

struct ArchiveAndData
{
    Abc::IArchive archive;
    std::vector< Abc::IArrayProperty > arrays;
    std::vector< Abc::IScalarProperty > scalars;
    std::size_t maxNumSamples;
};

class ArchiveWalker;

// used two different ways, one when traverse the archives
// (int archive is ignored), and one where we read samples
struct WorkUnit
{
    int start;
    int end;
    int archive;
    ArchiveWalker * walker;
};

class ArchiveWalker
{

public:

std::vector< ArchiveAndData > mArchives;

void addArchive(Abc::IArchive iArchive)
{
    ArchiveAndData data;
    data.archive = iArchive;
    data.maxNumSamples = 0;
    mArchives.push_back(data);
}

void readProps(WorkUnit & data)
{
    size_t numArchives = mArchives.size();

    printf("readProps Work Unit Start %d %d\n",  data.start, data.end);

    double startTime = getTimeSec();
    for (size_t a = 0; a < numArchives; ++a)
    {
        std::size_t numScalars = mArchives[a].scalars.size();
        std::size_t numArrays = mArchives[a].arrays.size();
        for (std::size_t i = 0; i < numScalars; ++i)
        {
            Abc::IScalarProperty & prop = mArchives[a].scalars[i];
            int propEnd = std::min(data.end, (int) prop.getNumSamples());
            if (prop.isConstant())
            {
                propEnd = 1;
            }

            for (int j = data.start; j < propEnd; ++j)
            {
                if (prop.getDataType().getPod() != Alembic::Util::kStringPOD)
                {
                    char buffer[4096];
                    prop.get(buffer, j);
                }
                else
                {
                    std::vector< std::string > buffer(
                        prop.getDataType().getExtent());

                    prop.get(&buffer.front(), j);
                }
            }
        }

        for (std::size_t i = 0; i < numArrays; ++i)
        {
            Abc::IArrayProperty & prop = mArchives[a].arrays[i];
            int propEnd = std::min(data.end, (int) prop.getNumSamples());
            if (prop.isConstant())
            {
                propEnd = 1;
            }

            for (int j = data.start; j < propEnd; ++j)
            {
                Alembic::AbcCoreAbstract::ArraySamplePtr samp;
                prop.get(samp, j);
            }
        }
    }

    printf("readProps Work Unit End %d %d %f\n",
           data.start, data.end, getTimeSec() - startTime);

    pthread_exit(0);
}

void findProps(int iArchiveNum, Abc::ICompoundProperty & iParent)
{
    size_t numProps = iParent.getNumProperties();
    for (size_t i = 0; i < numProps; ++i)
    {
        const Alembic::AbcCoreAbstract::PropertyHeader & childHeader =
            iParent.getPropertyHeader(i);
        if (childHeader.isScalar())
        {
            Alembic::Abc::IScalarProperty prop(iParent, childHeader.getName());
            if (prop.getNumSamples() > 0)
            {
                mArchives[iArchiveNum].scalars.push_back(prop);
                mArchives[iArchiveNum].maxNumSamples = std::max(
                    mArchives[iArchiveNum].maxNumSamples, prop.getNumSamples());
            }
        }
        else if (childHeader.isArray())
        {
            Alembic::Abc::IArrayProperty prop(iParent, childHeader.getName());
            if (prop.getNumSamples() > 0)
            {
                mArchives[iArchiveNum].arrays.push_back(prop);
                mArchives[iArchiveNum].maxNumSamples = std::max(
                    mArchives[iArchiveNum].maxNumSamples, prop.getNumSamples());
            }
        }
        else
        {
            Alembic::Abc::ICompoundProperty prop(iParent,
                childHeader.getName());
            findProps(iArchiveNum, prop);
        }
    }
}

void walkObjects(int iArchiveNum, Abc::IObject & iParent)
{
    size_t numChildren = iParent.getNumChildren();
    for (size_t i = 0; i < numChildren; i++)
    {
        const Abc::ObjectHeader & header = iParent.getChildHeader(i);
        Abc::IObject child(iParent, header.getName());

        Abc::ICompoundProperty prop = child.getProperties();
        findProps(iArchiveNum, prop);
        walkObjects(iArchiveNum, child);
    }
}

void walkArchives(WorkUnit & data)
{
    printf("walkArchives Work Unit Start %d %d\n",  data.start, data.end);

    for (int i = data.start; i < data.end; ++i)
    {
        Abc::IObject top = mArchives[i].archive.getTop();
        walkObjects(i, top);
    }

    printf("walkArchives Work Unit End %d %d\n",  data.start, data.end);
}

};

void * readPropsWrap(void * ptr)
{
    WorkUnit * data = (WorkUnit *) ptr;
    data->walker->readProps(*data);
    pthread_exit(0);
}

void * walkArchivesWrap(void * ptr)
{
    WorkUnit * data = (WorkUnit *) ptr;
    data->walker->walkArchives(*data);
    pthread_exit(0);
}

int main(int argc, char ** argv)
{
    if (argc < 4)
    {
        printf ("sceneWalk maxNumThreads ogawaStreams fileName [fileName ...]\n");
        return 0;
    }

    int maxThreads = atoi(argv[1]);
    int ogawaStreams = atoi(argv[2]);
{
    ArchiveWalker walker;

    Alembic::AbcCoreFactory::IFactory factory;
    Alembic::AbcCoreFactory::IFactory::CoreType coreType;
    factory.setOgawaNumStreams(ogawaStreams);
    double time_start = getTimeSec();
    for (int i = 3; i < argc; ++i)
    {
        Abc::IArchive archive = factory.getArchive(argv[i], coreType);

        if (archive.valid())
        {
            walker.addArchive(archive);
        }
    }

    int numTraverse = std::min(maxThreads, (int) walker.mArchives.size());
    std::vector<WorkUnit> traverseArchives(numTraverse);
    int lastEnd = 0;
    int workSize = walker.mArchives.size() / numTraverse;
    int workRem = 0;
    if ((int) walker.mArchives.size() > numTraverse)
        workRem = walker.mArchives.size() % numTraverse;

    pthread_t * walkThreads = new pthread_t[numTraverse];

    for (int i = 0; i < numTraverse; ++i)
    {
        traverseArchives[i].archive = i;
        traverseArchives[i].start = lastEnd;
        traverseArchives[i].walker = &walker;
        lastEnd += workSize;
        if (i < workRem)
            lastEnd ++;
        traverseArchives[i].end = lastEnd;

        pthread_create(&(walkThreads[i]), NULL, walkArchivesWrap,
            (void *) &(traverseArchives[i]));
    }

    for (int i = 0; i < numTraverse; ++i)
    {
        pthread_join(walkThreads[i], NULL);
    }

    delete [] walkThreads;

    double totalTime = getTimeSec() - time_start;
    printf ("Property Collection Wall Time: %f\n\n", totalTime);

    time_start = getTimeSec();

    size_t maxSamples = 0;
    for (size_t i = 0; i < walker.mArchives.size(); ++i)
    {
        maxSamples = std::max(walker.mArchives[i].maxNumSamples, maxSamples);
    }

    int numSamples = std::min(maxThreads, (int) maxSamples);
    std::vector<WorkUnit> readSamples(numSamples);
    lastEnd = 0;
    workSize = maxSamples / numSamples;
    workRem = 0;
    if ((int) maxSamples > numSamples)
        workRem = maxSamples % numSamples;

    pthread_t * readThreads = new pthread_t[numSamples];
    for (int i = 0; i < numSamples; ++i)
    {
        readSamples[i].archive = i;  // ignored
        readSamples[i].start = lastEnd;
        readSamples[i].walker = &walker;
        lastEnd += workSize;
        if (i < workRem)
            lastEnd ++;
        readSamples[i].end = lastEnd;
        pthread_create(&(readThreads[i]), NULL, readPropsWrap,
            (void *) &(readSamples[i]));
    }

    for (int i = 0; i < numSamples; ++i)
    {
        pthread_join(readThreads[i], NULL);
    }

    delete [] readThreads;

    totalTime = getTimeSec()-time_start;
    printf ("Property Read Wall Time: %f\n\n", totalTime);
}
    return 0;
}
