//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include "AbcExport.h"
#include "AbcWriteJob.h"
#include "MayaUtility.h"

#include <maya/MFnPlugin.h>

AbcExport::AbcExport()
{
}

AbcExport::~AbcExport()
{
}

MSyntax AbcExport::createSyntax()
{
    MSyntax syntax;

    syntax.addFlag("-d",  "-debug", MSyntax::kNoArg);
    syntax.addFlag("-h",  "-help", MSyntax::kNoArg);
    syntax.addFlag("-sa", "-startAt", MSyntax::kDouble);
    syntax.addFlag("-sf", "-skipFrame", MSyntax::kNoArg);
    syntax.addFlag("-sl", "-selection", MSyntax::kNoArg);
    syntax.addArg(MSyntax::kString);

    syntax.enableQuery(true);
    syntax.enableEdit(false);

    return syntax;
}


void* AbcExport::creator()
{
    return new AbcExport();
}

MStatus AbcExport::doIt(const MArgList & args)
{
    MStatus status;

    MTime oldCurTime = MAnimControl::currentTime();

    MArgParser argData(syntax(), args, &status);
    if (status != MS::kSuccess)
        return status;

    unsigned int numberOfArguments = args.length();

    MString msg;
    msg += "AbcExport  [options] tranlation_jobs_description_string\n\n";
    msg += "Options:\n";
    msg += "-h  / help  Print this message.\n";
    msg += "\n";
    msg += "-sa / startAt float (default: 0.0f)\n";
    msg += "The frame to start scene evaluation at. This is used to set\n";
    msg += "the starting frame for time dependent translations and can\n";
    msg += "be used to add run-up that isn't actually translated.\n";
    msg += "\n";
    msg += "-sf / skipFrame boolean (default: false)\n";
    msg += "When evaluating multiple translate jobs, this flag decides\n";
    msg += "whether or not to skip frame if possible.\n";
    msg += "\n";
    msg += "-sl / selection\n";
    msg += "If this flag is present: if tranlation_jobs_description_string\n";
    msg += "is empty, write out all nodes from the active selection list;\n";
    msg += "if tranlation_jobs_description_string is not empty, write out\n";
    msg += "only the nodes in the active selection list.\n";
    msg += "\n";
    msg += "-d  / debug  Print debug log\n";
    msg += "\n";
    msg += "(Each translation job is seperated by ;)\n";
    msg += "\n";
    msg += "per translation job optional flags:\n";
    msg += "\n";
    msg += "range float startTime float endTime\n";
    msg += "The frame range to write.\n";
    msg += "\n";
    msg += "uv\n";
    msg += "If set, AbcExport will bake the current uv set of polygons\n";
    msg += "and subD meshes into property \"st\" on the nodes.\n";
    msg += "By default this flag is not set.\n";
    msg += "\n";
    msg += "shutterOpen float (default: 0.0)\n";
    msg += "Motion blur starting time.\n";
    msg += "\n";
    msg += "shutterClose float (default: 0.0)\n";
    msg += "Motion blur end time\n";
    msg += "\n";
    msg += "numSamples unsigned int (default: 2)\n";
    msg += "The number of times to sample within a given frame with\n";
    msg += "motion blur applied.  If shutterOpen is equal to\n";
    msg += "shutterClose then numSamples is ignored.\n";
    msg += "\n";
    msg += "noSampleGeo\n";
    msg += "If set, only write out geometry on whole frames, not\n";
    msg += "subframes. This flag is not set by default.\n";
    msg += "Transforms may still be written out on subframes.\n";
    msg += "\n";
    msg += "attrPrefix string (default: SPT_)\n";
    msg += "Prefix filter for determining which attributes to write out\n";
    msg += "\n";
    msg += "attrs string\n";
    msg += "Comma seperated list of attributes to write out, these\n";
    msg += "attributes will ignore the attr prefix filter.\n";
    msg += "\n";
    msg += "writeVisibility bool (default: false)\n";
    msg += "Whether or not to write the visibility state to the file.\n";
    msg += "If false then visibility is not written and everything is\n";
    msg += "assumed to be visible.\n";
    msg += "\n";
    msg += "worldSpace\n";
    msg += "If set, the root nodes will be stored in world space.\n";
    msg += "By default it is stored in local space.\n";
    msg += "\n";
    msg += "melPerFrameCallback string (default: "")\n";
    msg += "When each frame (and the static frame) is evaluated the\n";
    msg += "string specified is evaluated as a Mel command.\n";
    msg += "See below for special processing rules.\n";
    msg += "Example: melPerFrameCallback print(\"#FRAME#\")\n";
    msg += "\n";
    msg += "melPostCallback string (default: "")\n";
    msg += "When the translation has finished the string specified is\n";
    msg += "evaluated as a Mel command.\n";
    msg += "See below for special processing rules.\n";
    msg += "Example: melPostCallback print(\"Done!\")\n";
    msg += "\n";
    msg += "pythonPerFrameCallback string (default: "")\n";
    msg += "When each frame (and the static frame) is evaluated the\n";
    msg += "string specified is evaluated as a python command.\n";
    msg += "See below for special processing rules.\n";
    msg += "Example: pythonPerFrameCallback print(\"#FRAME#\")\n";
    msg += "\n";
    msg += "pythonPostCallback string (default: "")\n";
    msg += "When the translation has finished the string specified is\n";
    msg += "evaluated as a python command.\n";
    msg += "See below for special processing rules.\n";
    msg += "Example: pythonPostCallback print(\"Done!\")\n";
    msg += "\n";
    msg += "On the callbacks, special tokens are replaced with other\n";
    msg += "data, these tokens and what they are replaced with are as\n";
    msg += "follows:\n";
    msg += "\n";
    msg += "#FRAME# replaced with the frame number being evaluated, if\n";
    msg += "the static frame is being evaluated then #FRAME# is not\n";
    msg += "replaced.  #FRAME# is ignored in the post callbacks.\n";
    msg += "\n";
    msg += "#BOUNDS# replaced with the bounding box values in minX minY\n";
    msg += "minZ maxX maxY maxZ space seperated order.\n";
    msg += "\n";
    msg += "#BOUNDSARRAY# replaced with the bounding box values as\n";
    msg += "above, but in array form. In Mel:\n";
    msg += "In Mel: {minX, minY, minZ, maxX, maxY, maxZ}\n";
    msg += "In Python: [minX, minY, minZ, maxX, maxY, maxZ]";
    msg += "\n";
    msg += "Command Examples:\n";
    msg += "AbcExport -d -sf \"range 1 24 test_hi test_lo /tmp/test.abc\"\n";
    msg += "AbcExport \"worldSpace test_hi /tmp/test_hi.abc\"\n";
    msg += "AbcExport \"range 1 24 shutterOpen 0.0 shutterClose 0.5 ";
    msg += "numSamples 2 test_hi test_lo /tmp/test.abc\"\n";
    msg += "AbcExport -d \"range 101 700 test_hi /tmp/test.abc; range 10";
    msg += " 55 test_lo /tmp/test1.abc\"\n";
    msg += "\n";
    msg += "Note that multiple nodes can be written to the same file,\n";
    msg += "but these nodes should not have any parenting relationships\n";
    msg += "or the job will not be written out.\n";

    if (argData.isFlagSet("help"))
    {
        MGlobal::displayInfo(msg);
        return MS::kSuccess;
    }

    bool debug = argData.isFlagSet("debug");

    // If skipFrame is true, when going through the playback range of the
    // scene, as much frames are skipped when possible.  This could cause
    // a problem for, time dependent solutions like
    // particle system / hair simulation
    bool skipFrame = false;
    if (argData.isFlagSet("skipFrame"))
        skipFrame = true;

    bool useSelectionList = false;
    if (argData.isFlagSet("selection"))
        useSelectionList = true;

    double startEvaluationTime = DBL_MAX;
    if (argData.isFlagSet("startAt"))
    {
        double startAt = 0.0;
        argData.getFlagArgument("startAt", 0, startAt);
        startEvaluationTime = startAt;
    }

    // Very rudimentary argument parser: no syntax checking at all !!!
    MString argStr;

    // status = argData.getCommandArgument(0, argStr);
    argStr = args.asString(numberOfArguments-1, &status);
    MStringArray jobStringArray;
    status = argStr.split(';', jobStringArray);
    unsigned int jobSize = jobStringArray.length();

    if (jobSize == 0)
        return status;

    // the frame range we will be iterating over for all jobs,
    // includes frames which are not skipped and the startAt offset
    std::set<double> allFrameRange;

    // this will eventually hold only the animated jobs.
    // its a list because we will be removing jobs from it
    std::list < AbcWriteJobPtr > jobList;

    for (unsigned int jobIndex = 0; jobIndex < jobSize; jobIndex++)
    {
        unsigned int argc = 0;

        // parse the string
        MString tstr = jobStringArray[jobIndex];
        MStringArray strArr;
        status = tstr.split(' ', strArr);
        unsigned int length = strArr.length();

        double  startTime = oldCurTime.value();
        double  endTime = oldCurTime.value();

        double  shutterOpen = 0.0;
        double  shutterClose = 0.0;
        int     numSamples = 1;
        bool    sampleGeo  = true;     // whether or not to subsample geometry

        bool    worldSpace = false;
        bool    writeVisibility = false;
        bool    writeUVs   = false;

        // DAG path array of nodes to be written out as root nodes in the file
        util::ShapeSet dagPath;

       // name of the abc file the job will be written into
        std::string fileName;

        // the list of frames written into the abc file
        std::set<double> geoSamples;
        std::set<double> transSamples;

        std::string melPerFrameCallback;
        std::string melPostCallback;
        std::string pythonPerFrameCallback;
        std::string pythonPostCallback;

        // attribute filtering stuff
        std::string prefixFilter = "SPT_";
        std::set<std::string> attribsSet;

        // parser for each job
        while (argc < length)
        {
            if (strArr[argc] == "range")  // range start end
            {
                // guard against overruns
                if (argc + 2 >= length)
                    return MS::kFailure;

                // looking for two floating point numbers
                util::isFloat(strArr[argc+1], msg);
                util::isFloat(strArr[argc+2], msg);

                startTime = floor(strArr[argc+1].asDouble());
                endTime = ceil(strArr[argc+2].asDouble());

                // make sure start frame is smaller or equal to endTime
                if (startTime > endTime)
                {
                    double temp = startTime;
                    startTime = endTime;
                    endTime = temp;
                }

                argc += 3;
            }
            else if (strArr[argc] == "uv")
            {
                writeUVs = true;
                argc++;
            }
            else if (strArr[argc] == "shutterOpen")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                util::isFloat(strArr[argc+1], msg);
                shutterOpen = strArr[argc+1].asDouble();
                argc += 2;
            }
            else if (strArr[argc] == "shutterClose")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                util::isFloat(strArr[argc+1], msg);
                shutterClose = strArr[argc+1].asDouble();
                argc += 2;
            }
            else if (strArr[argc] == "numSamples")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                util::isUnsigned(strArr[argc+1], msg);
                numSamples = strArr[argc+1].asInt();
                argc += 2;
            }
            else if (strArr[argc] == "writeVisibility")
            {
                writeVisibility = true;
                argc++;
            }
            else if (strArr[argc] == "worldSpace")
            {
                worldSpace = true;
                argc++;
            }
            else if (strArr[argc] == "noSampleGeo")
            {
                sampleGeo = false;
                argc++;
            }
            else if (strArr[argc] == "melPerFrameCallback")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                melPerFrameCallback = strArr[argc+1].asChar();
                argc += 2;
            }
            else if (strArr[argc] == "melPostCallback")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                melPostCallback = strArr[argc+1].asChar();
                argc += 2;
            }
            else if (strArr[argc] == "pythonPerFrameCallback")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                pythonPerFrameCallback = strArr[argc+1].asChar();
                argc += 2;
            }
            else if (strArr[argc] == "pythonPostCallback")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                pythonPostCallback = strArr[argc+1].asChar();
                argc += 2;
            }

            else if (strArr[argc] == "attrPrefix")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                prefixFilter = strArr[argc+1].asChar();
                argc += 2;
            }

            else if (strArr[argc] == "attrs")
            {
                if (argc + 1 >= length)
                    return MS::kFailure;

                MString attrString = strArr[argc+1];

                MStringArray attribs;
                attrString.split(',', attribs);
                unsigned int attribsLength = attrString.length();
                for (unsigned int i = 0; i < attribsLength; ++i)
                {
                    MString & attrib = attribs[i];
                    if (attrib != "" && attrib != "visibility")
                    {
                        attribsSet.insert(attrib.asChar());
                    }
                }
                argc += 2;
            }
            else
            // assume in the order of node names and then abc file name
            {
                for (; argc < length-1; argc++)
                {
                    MSelectionList sel;
                    if (!sel.add(strArr[argc]))
                    {
                        MString warn = "Could not select ";
                        warn += strArr[argc];
                        warn += ". Skipping...";
                        MGlobal::displayWarning(warn);
                        continue;
                    }

                    MDagPath path;
                    if (!sel.getDagPath(0, path))
                    {
                        MGlobal::displayWarning(
                            "Not a DAG Node. Skipping... ");
                        continue;
                    }

                    dagPath.insert(path);
                }

                // check for validity of the DagPath relationships
                // complexity : n^2
                bool isAncestor = false;
                if (dagPath.size() > 1)
                {
                    util::ShapeSet::iterator m, n;
                    const util::ShapeSet::iterator end = dagPath.end();
                    for (m = dagPath.begin(); m != end; )
                    {
                        MDagPath path1 = *m;
                        m++;
                        for (n = m; n != end; n++)
                        {
                            MDagPath path2 = *n;
                            if (util::isAncestorDescendentRelationship(path1, path2))
                                isAncestor = true;
                        }  // for n
                    }  // for m
                }
                if (isAncestor == true)
                    return MS::kFailure;

                if (argc >= length)
                    return MS::kFailure;

                fileName = strArr[argc++].asChar();
            }
        }

        std::set <double> origSamples;
        for (double f = startTime; f <= endTime; f++)
            origSamples.insert(f);

        transSamples = origSamples;
        geoSamples = origSamples;

        Alembic::AbcCoreAbstract::v1::TimeSamplingType transTime(util::spf());
        Alembic::AbcCoreAbstract::v1::TimeSamplingType geoTime(util::spf());

        // post process, add extra motion blur samples
        if (numSamples > 1 && shutterOpen < shutterClose)
        {
            transTime = Alembic::AbcCoreAbstract::v1::TimeSamplingType(
                numSamples, util::spf());

            // if we are't subsampling the geometry, leave it as uniform
            if (sampleGeo)
                geoTime = transTime;

            std::set<double> offsetSamples;
            offsetSamples.insert(shutterOpen);
            offsetSamples.insert(shutterClose);

            double offset = (shutterClose - shutterOpen) / (numSamples-1);
            double curVal = shutterOpen + offset;
            for (int i = 0; i < numSamples - 2; ++i, curVal += offset)
            {
                offsetSamples.insert(curVal);
            }

            // Add an extra leading or trailing frame on an
            // integer boundary for the rest of the pipeline
            double floorVal = floor(startTime + shutterOpen);
            double ceilVal = ceil(endTime + shutterClose);

            transSamples.insert(floorVal);
            transSamples.insert(ceilVal);
            geoSamples.insert(floorVal);
            geoSamples.insert(ceilVal);

            std::set<double>::iterator samp = origSamples.begin();
            std::set<double>::iterator sampEnd = origSamples.end();
            for (; samp != sampEnd; ++samp)
            {
                double curSamp = *samp;
                std::set<double>::iterator offset = offsetSamples.begin();
                std::set<double>::iterator offsetEnd = offsetSamples.end();
                for (; offset != offsetEnd; ++offset)
                {
                    double curVal = curSamp + (*offset);
                    double rndVal = roundf(curVal);

                    // if the value is close enough to the integer value
                    // insert the integer value
                    if (fabs(curVal - rndVal) < 1e-4)
                    {
                        transSamples.insert(rndVal);

                        // ignore geometry sampling flag because it is a whole
                        // frame and for some reason we always want to
                        // translate the whole frames
                        geoSamples.insert(rndVal);
                    }
                    else if (sampleGeo)
                    {
                        transSamples.insert(curVal);
                        geoSamples.insert(curVal);
                    }
                    else
                    {
                        // we aren't include subsampled geometry
                        transSamples.insert(curVal);
                    }
                }  // for offset
            }  // for samp
        }  // if we need to apply motion blur

        AbcWriteJobPtr job(new AbcWriteJob(dagPath, fileName.c_str(),
            useSelectionList, worldSpace, writeVisibility, writeUVs,
            transSamples, transTime, geoSamples, geoTime,
            melPerFrameCallback, melPostCallback,
            pythonPerFrameCallback, pythonPostCallback, prefixFilter,
            attribsSet));

       jobList.push_front(job);

        // make sure we add additional whole frames, if we arent skipping
        // the inbetween ones
        if (!skipFrame && !allFrameRange.empty() && !transSamples.empty())
        {
            double localMin = *(transSamples.begin());
            std::set<double>::iterator last = transSamples.end();
            last--;
            double localMax = *last;

            double globalMin = *(allFrameRange.begin());
            last = allFrameRange.end();
            last--;
            double globalMax = *last;

            // if the min of our current frame range is beyond
            // what we know about, pad a few more frames
            if (localMin > globalMax)
            {
                for (double f = globalMax; f < localMin; f++)
                {
                    allFrameRange.insert(f);
                }
            }

            // if the max of our current frame range is beyond
            // what we know about, pad a few more frames
            if (localMax < globalMin)
            {
                for (double f = localMax; f < globalMin; f++)
                {
                    allFrameRange.insert(f);
                }
            }
        }

        // right now we just copy over the translation samples since
        // they are guaranteed to contain all the geometry samples
        if (!transSamples.empty())
            allFrameRange.insert(transSamples.begin(), transSamples.end());
    }

    // ================ end of argument parsing =========================

    // add extra evaluation run up, if necessary
    if (startEvaluationTime != DBL_MAX && !allFrameRange.empty())
    {
        double firstFrame = *allFrameRange.begin();
        for (double f = startEvaluationTime; f < firstFrame; ++f)
        {
            allFrameRange.insert(f);
        }
    }

    std::set<double>::iterator it = allFrameRange.begin();
    std::set<double>::iterator itEnd = allFrameRange.end();

    // loop through every frame in the list, if a job has that frame in it's
    // list of transform or shape frames, then it will write out data and
    // call the perFrameCallback, if that frame is also the last one it has
    // to work on then it will also call the postCallback.
    // If it doesn't have this frame, then it does nothing
    for (; it != itEnd; it++)
    {
        if (debug)
        {
            double frame = *it;
            MString info;
            info = frame;
            MGlobal::displayInfo(info);
        }

        MGlobal::viewFrame(*it);
        std::list< AbcWriteJobPtr >::iterator j = jobList.begin();
        std::list< AbcWriteJobPtr >::iterator jend = jobList.end();
        while (j != jend)
        {
            bool lastFrame = (*j)->eval(*it);

            //
            if (lastFrame)
            {
                j = jobList.erase(j);
            }
            else
                j++;
        }
    }

    // set the time back
    MGlobal::viewFrame(oldCurTime);

    return MS::kSuccess;
}



MStatus initializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin(obj, "Alembic", "1.0", "Any");

    status = plugin.registerCommand(
        "AbcExport", AbcExport::creator,
        AbcExport::createSyntax );

    if (!status)
    {
        status.perror("registerCommand");
    }


    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin(obj);

    status = plugin.deregisterCommand("AbcExport");
    if (!status)
    {
        status.perror("deregisterCommand");
    }

    return status;
}
