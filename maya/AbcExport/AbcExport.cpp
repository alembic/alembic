//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
#include <maya/MFileObject.h>
#include <maya/MItDependencyNodes.h>

namespace AbcA = Alembic::AbcCoreAbstract;

AbcExport::AbcExport()
{
}

AbcExport::~AbcExport()
{
}

MSyntax AbcExport::createSyntax()
{
    MSyntax syntax;

    syntax.addFlag("-v",  "-verbose", MSyntax::kNoArg);
    syntax.addFlag("-h",  "-help", MSyntax::kNoArg);
    syntax.addFlag("-prs", "-preRollStartFrame", MSyntax::kDouble);
    syntax.addFlag("-duf", "-dontSkipUnwrittenFrames", MSyntax::kNoArg);
    syntax.addFlag("-j", "-jobArg", MSyntax::kString);

    syntax.makeFlagMultiUse("-j");
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
try
{
    MStatus status;

    MTime oldCurTime = MAnimControl::currentTime();

    MArgParser argData(syntax(), args, &status);

    if (argData.isFlagSet("help"))
    {
        MGlobal::displayInfo(util::getHelpText());
        return MS::kSuccess;
    }

    bool verbose = argData.isFlagSet("verbose");

    // If skipFrame is true, when going through the playback range of the
    // scene, as much frames are skipped when possible.  This could cause
    // a problem for, time dependent solutions like
    // particle system / hair simulation
    bool skipFrame = true;
    if (argData.isFlagSet("dontSkipUnwrittenFrames"))
        skipFrame = false;

    double startEvaluationTime = DBL_MAX;
    if (argData.isFlagSet("preRollStartFrame"))
    {
        double startAt = 0.0;
        argData.getFlagArgument("preRollStartFrame", 0, startAt);
        startEvaluationTime = startAt;
    }

    unsigned int jobSize = argData.numberOfFlagUses("jobArg");

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
        JobArgs jobArgs;
        MArgList jobArgList;
        argData.getFlagArgumentList("jobArg", jobIndex, jobArgList);
        MString jobArgsStr = jobArgList.asString(0);
        MStringArray jobArgsArray;

        {
            // parse the job arguments
            // e.g. -perFrameCallbackMel "print \"something\"" will be splitted to
            //    [0] -perFrameCallbackMel
            //    [1] print "something"
            enum State {
                kArgument,               // parsing an argument (not quoted)
                kDoubleQuotedString,     // parsing a double quoted string
                kSingleQuotedString,     // parsing a single quoted string
            };

            State state = kArgument;
            MString stringBuffer;
            for (unsigned int charIdx = 0; charIdx < jobArgsStr.numChars();
                charIdx++)
            {
                MString ch = jobArgsStr.substringW(charIdx, charIdx);
                switch (state)
                {
                case kArgument:
                    if (ch == " ")
                    {
                        // space terminates the current argument
                        if (stringBuffer.length() > 0) {
                            jobArgsArray.append(stringBuffer);
                            stringBuffer.clear();
                        }
                        // goto another argument
                        state = kArgument;
                    }
                    else if (ch == "\"")
                    {
                        if (stringBuffer.length() > 0)
                        {
                            // double quote is part of the argument
                            stringBuffer += ch;
                        }
                        else
                        {
                            // goto double quoted string
                            state = kDoubleQuotedString;
                        }
                    }
                    else if (ch == "'")
                    {
                        if (stringBuffer.length() > 0)
                        {
                            // single quote is part of the argument
                            stringBuffer += ch;
                        }
                        else
                        {
                            // goto single quoted string
                            state = kSingleQuotedString;
                        }
                    }
                    else
                    {
                        stringBuffer += ch;
                    }
                break;

                case kDoubleQuotedString:
                    // double quote terminates the current string
                    if (ch == "\"")
                    {
                        jobArgsArray.append(stringBuffer);
                        stringBuffer.clear();
                        state = kArgument;
                    }
                    else if (ch == "\\")
                    {
                        // escaped character
                        MString nextCh = (++charIdx < jobArgsStr.numChars())
                            ? jobArgsStr.substringW(charIdx, charIdx) : "\\";
                        if (nextCh == "n")       stringBuffer += "\n";
                        else if (nextCh == "t")  stringBuffer += "\t";
                        else if (nextCh == "r")  stringBuffer += "\r";
                        else if (nextCh == "\\") stringBuffer += "\\";
                        else if (nextCh == "'")  stringBuffer += "'";
                        else if (nextCh == "\"") stringBuffer += "\"";
                        else                     stringBuffer += nextCh;
                    }
                    else
                    {
                        stringBuffer += ch;
                    }
                break;

                case kSingleQuotedString:
                    // single quote terminates the current string
                    if (ch == "'")
                    {
                        jobArgsArray.append(stringBuffer);
                        stringBuffer.clear();
                        state = kArgument;
                    }
                    else if (ch == "\\")
                    {
                        // escaped character
                        MString nextCh = (++charIdx < jobArgsStr.numChars())
                            ? jobArgsStr.substringW(charIdx, charIdx) : "\\";
                        if (nextCh == "n")       stringBuffer += "\n";
                        else if (nextCh == "t")  stringBuffer += "\t";
                        else if (nextCh == "r")  stringBuffer += "\r";
                        else if (nextCh == "\\") stringBuffer += "\\";
                        else if (nextCh == "'")  stringBuffer += "'";
                        else if (nextCh == "\"") stringBuffer += "\"";
                        else                     stringBuffer += nextCh;
                    }
                    else
                    {
                        stringBuffer += ch;
                    }
                break;
                }
            }

            // the rest of the argument
            if (stringBuffer.length() > 0)
            {
                jobArgsArray.append(stringBuffer);
            }
        }

        double startTime = oldCurTime.value();
        double endTime = oldCurTime.value();
        double strideTime = 1.0;
        bool hasRange = false;
        bool hasRoot = false;
        std::set <double> shutterSamples;
        bool sampleGeo  = true; // whether or not to subsample geometry
        std::string fileName;
        bool asOgawa = true;

        unsigned int numJobArgs = jobArgsArray.length();
        for (unsigned int i = 0; i < numJobArgs; ++i)
        {
            MString arg = jobArgsArray[i];
            arg.toLowerCase();

            if (arg == "-f" || arg == "-file")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError("File incorrectly specified.");
                    return MS::kFailure;
                }
                fileName = jobArgsArray[++i].asUTF8();
            }

            else if (arg == "-fr" || arg == "-framerange")
            {
                if (i+2 >= numJobArgs || !jobArgsArray[i+1].isDouble() ||
                    !jobArgsArray[i+2].isDouble())
                {
                    MGlobal::displayError("Frame Range incorrectly specified.");
                    return MS::kFailure;
                }

                hasRange = true;
                startTime = jobArgsArray[++i].asDouble();
                endTime = jobArgsArray[++i].asDouble();

                // make sure start frame is smaller or equal to endTime
                if (startTime > endTime)
                {
                    double temp = startTime;
                    startTime = endTime;
                    endTime = temp;
                }
            }

            else if (arg == "-frs" || arg == "-framerelativesample")
            {
                if (i+1 >= numJobArgs || !jobArgsArray[i+1].isDouble())
                {
                    MGlobal::displayError(
                        "Frame Relative Sample incorrectly specified.");
                    return MS::kFailure;
                }
                shutterSamples.insert(jobArgsArray[++i].asDouble());
            }

            else if (arg == "-nn" || arg == "-nonormals")
            {
                jobArgs.noNormals = true;
            }

            else if (arg == "-ro" || arg == "-renderableonly")
            {
                jobArgs.excludeInvisible = true;
            }

            else if (arg == "-s" || arg == "-step")
            {
                if (i+1 >= numJobArgs || !jobArgsArray[i+1].isDouble())
                {
                    MGlobal::displayError("Step incorrectly specified.");
                    return MS::kFailure;
                }
                strideTime = jobArgsArray[++i].asDouble();
            }

            else if (arg == "-sl" || arg == "-selection")
            {
                jobArgs.useSelectionList = true;
            }

            else if (arg == "-sn" || arg == "-stripnamespaces")
            {
                if (i+1 >= numJobArgs || !jobArgsArray[i+1].isUnsigned())
                {
                    // the strip all namespaces case
                    // so we pick a very LARGE number
                    jobArgs.stripNamespace = 0xffffffff;
                }
                else
                {
                    jobArgs.stripNamespace = jobArgsArray[++i].asUnsigned();
                }
            }

            else if (arg == "-uv" || arg == "-uvwrite")
            {
                jobArgs.writeUVs = true;
            }

            else if (arg == "-wcs" || arg == "-writecolorsets")
            {
                jobArgs.writeColorSets = true;
            }

            else if (arg == "-wfs" || arg == "-writefacesets")
            {
                jobArgs.writeFaceSets = true;
            }

            else if (arg == "-wfg" || arg == "-wholeframegeo")
            {
                sampleGeo = false;
            }

            else if (arg == "-ws" || arg == "-worldspace")
            {
                jobArgs.worldSpace = true;
            }

            else if (arg == "-wv" || arg == "-writevisibility")
            {
                jobArgs.writeVisibility = true;
            }

            else if (arg == "-mfc" || arg == "-melperframecallback")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "melPerFrameCallback incorrectly specified.");
                    return MS::kFailure;
                }
                jobArgs.melPerFrameCallback = jobArgsArray[++i].asChar();
            }

            else if (arg == "-pfc" || arg == "-pythonperframecallback")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "pythonPerFrameCallback incorrectly specified.");
                    return MS::kFailure;
                }
                jobArgs.pythonPerFrameCallback = jobArgsArray[++i].asChar();
            }

            else if (arg == "-mpc" || arg == "-melpostjobcallback")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "melPostJobCallback incorrectly specified.");
                    return MS::kFailure;
                }
                jobArgs.melPostCallback = jobArgsArray[++i].asChar();
            }

            else if (arg == "-ppc" || arg == "-pythonpostjobcallback")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "pythonPostJobCallback incorrectly specified.");
                    return MS::kFailure;
                }
                jobArgs.pythonPostCallback = jobArgsArray[++i].asChar();
            }

            // geomArbParams - attribute filtering stuff
            else if (arg == "-atp" || arg == "-attrprefix")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "attrPrefix incorrectly specified.");
                    return MS::kFailure;
                }
                jobArgs.prefixFilters.push_back(jobArgsArray[++i].asChar());
            }

            else if (arg == "-a" || arg == "-attr")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "attr incorrectly specified.");
                    return MS::kFailure;
                }
                jobArgs.attribs.insert(jobArgsArray[++i].asChar());
            }

            // userProperties - attribute filtering stuff
            else if (arg == "-uatp" || arg == "-userattrprefix")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "userAttrPrefix incorrectly specified.");
                    return MS::kFailure;
                }
                jobArgs.userPrefixFilters.push_back(jobArgsArray[++i].asChar());
            }

            else if (arg == "-u" || arg == "-userattr")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "userAttr incorrectly specified.");
                    return MS::kFailure;
                }
                jobArgs.userAttribs.insert(jobArgsArray[++i].asChar());
            }

            else if (arg == "-rt" || arg == "-root")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "root incorrectly specified.");
                    return MS::kFailure;
                }
                hasRoot = true;
                MString root = jobArgsArray[++i];

                MSelectionList sel;
                if (sel.add(root) != MS::kSuccess)
                {
                    MString warn = root;
                    warn += " could not be select, skipping.";
                    MGlobal::displayWarning(warn);
                    continue;
                }

                unsigned int numRoots = sel.length();
                for (unsigned int j = 0; j < numRoots; ++j)
                {
                    MDagPath path;
                    if (sel.getDagPath(j, path) != MS::kSuccess)
                    {
                        MString warn = path.fullPathName();
                        warn += " (part of ";
                        warn += root;
                        warn += " ) not a DAG Node, skipping.";
                        MGlobal::displayWarning(warn);
                        continue;
                    }
                    jobArgs.dagPaths.insert(path);
                }
            }
            else if (arg == "-ef" || arg == "-eulerfilter")
            {
                jobArgs.filterEulerRotations = true;
            }
            else if (arg == "-df" || arg == "-dataformat")
            {
                if (i+1 >= numJobArgs)
                {
                    MGlobal::displayError(
                        "dataFormat incorrectly specified.");
                    return MS::kFailure;
                }
                MString dataFormat = jobArgsArray[++i];
                dataFormat.toLowerCase();
                if (dataFormat == "hdf")
                {
                    asOgawa = false;
                }
                else if (dataFormat == "ogawa")
                {
                    asOgawa = true;
                }
            }
            else
            {
                MString warn = "Ignoring unsupported flag: ";
                warn += jobArgsArray[i];
                MGlobal::displayWarning(warn);
            }
        } //  for i

        if (fileName == "")
        {
            MString error = "-file not specified.";
            MGlobal::displayError(error);
            return MS::kFailure;
        }

        {
            MString fileRule, expandName;
            MString alembicFileRule = "alembicCache";
            MString alembicFilePath = "cache/alembic";

            MString queryFileRuleCmd;
            queryFileRuleCmd.format("workspace -q -fre \"^1s\"",
                alembicFileRule);

            MString queryFolderCmd;
            queryFolderCmd.format("workspace -en `workspace -q -fre \"^1s\"`",
                alembicFileRule);

            // query the file rule for alembic cache
            MGlobal::executeCommand(queryFileRuleCmd, fileRule);
            if (fileRule.length() > 0)
            {
                // we have alembic file rule, query the folder
                MGlobal::executeCommand(queryFolderCmd, expandName);
            }
            else
            {
                // alembic file rule does not exist, create it
                MString addFileRuleCmd;
                addFileRuleCmd.format("workspace -fr \"^1s\" \"^2s\"",
                    alembicFileRule, alembicFilePath);
                MGlobal::executeCommand(addFileRuleCmd);

                // save the workspace. maya may discard file rules on exit
                MGlobal::executeCommand("workspace -s");

                // query the folder
                MGlobal::executeCommand(queryFolderCmd, expandName);
            }

            // resolve the expanded file rule
            if (expandName.length() == 0)
            {
                expandName = alembicFilePath;
            }

            // get the path to the alembic file rule
            MFileObject directory;
            directory.setRawFullName(expandName);
            MString directoryName = directory.resolvedFullName();

            // make sure the cache folder exists
            if (!directory.exists())
            {
                // create the cache folder
                MString createFolderCmd;
                createFolderCmd.format("sysFile -md \"^1s\"", directoryName);
                MGlobal::executeCommand(createFolderCmd);
            }

            // resolve the relative path
            MFileObject absoluteFile;
            absoluteFile.setRawFullName(fileName.c_str());
#if MAYA_API_VERSION < 201300
            if (absoluteFile.resolvedFullName() !=
                absoluteFile.expandedFullName())
            {
#else
            if (!MFileObject::isAbsolutePath(fileName.c_str())) {
#endif
                // this is a relative path
                MString absoluteFileName = directoryName + "/" +
                    fileName.c_str();
                absoluteFile.setRawFullName(absoluteFileName);
                fileName = absoluteFile.resolvedFullName().asChar();
            }
            else
            {
                fileName = absoluteFile.resolvedFullName().asChar();
            }

            // check the path must exist before writing
            MFileObject absoluteFilePath;
            absoluteFilePath.setRawFullName(absoluteFile.path());
            if (!absoluteFilePath.exists()) {
                MString error;
                error.format("Path ^1s does not exist!", absoluteFilePath.resolvedFullName());
                MGlobal::displayError(error);
                return MS::kFailure;
            }
        }

        if (shutterSamples.empty())
        {
            shutterSamples.insert(0.0);
        }

        if (jobArgs.prefixFilters.empty())
        {
            jobArgs.prefixFilters.push_back("ABC_");
        }

        // the list of frames written into the abc file
        std::set<double> geoSamples;
        std::set<double> transSamples;
        std::set <double>::const_iterator shutter;
        std::set <double>::const_iterator shutterStart = shutterSamples.begin();
        std::set <double>::const_iterator shutterEnd = shutterSamples.end();
        for (double frame = startTime; frame <= endTime; frame += strideTime)
        {
            for (shutter = shutterStart; shutter != shutterEnd; ++shutter)
            {
                double curFrame = *shutter + frame;
                if (!sampleGeo)
                {
                    double intFrame = (double)(int)(
                        curFrame >= 0 ? curFrame + .5 : curFrame - .5);

                    // only insert samples that are close to being an integer
                    if (fabs(curFrame - intFrame) < 1e-4)
                    {
                        geoSamples.insert(curFrame);
                    }
                }
                else
                {
                    geoSamples.insert(curFrame);
                }
                transSamples.insert(curFrame);
            }
        }

        if (geoSamples.empty())
        {
            geoSamples.insert(startTime);
        }

        if (transSamples.empty())
        {
            transSamples.insert(startTime);
        }

        if (jobArgs.dagPaths.size() > 1)
        {
            // check for validity of the DagPath relationships complexity : n^2

            util::ShapeSet::const_iterator m, n;
            util::ShapeSet::const_iterator end = jobArgs.dagPaths.end();
            for (m = jobArgs.dagPaths.begin(); m != end; )
            {
                MDagPath path1 = *m;
                m++;
                for (n = m; n != end; n++)
                {
                    MDagPath path2 = *n;
                    if (util::isAncestorDescendentRelationship(path1,path2))
                    {
                        MString errorMsg = path1.fullPathName();
                        errorMsg += " and ";
                        errorMsg += path2.fullPathName();
                        errorMsg += " have an ancestor relationship.";
                        MGlobal::displayError(errorMsg);
                        return MS::kFailure;
                    }
                }  // for n
            }  // for m
        }
        // no root is specified, and we aren't using a selection
        // so we'll try to translate the whole Maya scene by using all
        // children of the world as roots.
        else if (!hasRoot && !jobArgs.useSelectionList)
        {
            MSelectionList sel;
#if MAYA_API_VERSION >= 201100
            sel.add("|*", true);
#else
            // older versions of Maya will not be able to find top level nodes
            // within namespaces
            sel.add("|*");
#endif
            unsigned int numRoots = sel.length();
            for (unsigned int i = 0; i < numRoots; ++i)
            {
                MDagPath path;
                sel.getDagPath(i, path);
                jobArgs.dagPaths.insert(path);
            }
        }
        else if (hasRoot && jobArgs.dagPaths.empty())
        {
            MString errorMsg = "No valid root nodes were specified.";
            MGlobal::displayError(errorMsg);
            return MS::kFailure;
        }
        else if (jobArgs.useSelectionList)
        {
            MSelectionList activeList;
            MGlobal::getActiveSelectionList(activeList);
            if (activeList.length() == 0)
            {
                MString errorMsg =
                    "-selection specified but nothing is actively selected.";
                MGlobal::displayError(errorMsg);
                return MS::kFailure;
            }
        }

        AbcA::TimeSamplingPtr transTime, geoTime;

        std::vector<double> samples;
        for (shutter = shutterStart; shutter != shutterEnd; ++shutter)
        {
            samples.push_back((startTime + *shutter) * util::spf());
        }

        if (hasRange)
        {
            transTime.reset(new AbcA::TimeSampling(AbcA::TimeSamplingType(
                static_cast<Alembic::Util::uint32_t>(samples.size()),
                strideTime * util::spf()), samples));
        }
        else
        {
            transTime.reset(new AbcA::TimeSampling());
        }

        if (sampleGeo || !hasRange)
        {
            geoTime = transTime;
        }
        else
        {
            double geoStride = strideTime;
            if (geoStride < 1.0)
                geoStride = 1.0;

            samples.clear();
            samples.push_back(*geoSamples.begin() * util::spf());
            geoTime.reset(new AbcA::TimeSampling(AbcA::TimeSamplingType(
                geoStride * util::spf()), samples));
        }

        AbcWriteJobPtr job(new AbcWriteJob(fileName.c_str(), asOgawa,
            transSamples, transTime, geoSamples, geoTime, jobArgs));

       jobList.push_front(job);

        // make sure we add additional whole frames, if we arent skipping
        // the inbetween ones
        if (!skipFrame && !allFrameRange.empty())
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
        allFrameRange.insert(transSamples.begin(), transSamples.end());
    }

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

    MComputation computation;
    computation.beginComputation();

    // loop through every frame in the list, if a job has that frame in it's
    // list of transform or shape frames, then it will write out data and
    // call the perFrameCallback, if that frame is also the last one it has
    // to work on then it will also call the postCallback.
    // If it doesn't have this frame, then it does nothing
    for (; it != itEnd; it++)
    {
        if (verbose)
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
            if (computation.isInterruptRequested())
                return MS::kFailure;

            bool lastFrame = (*j)->eval(*it);

            if (lastFrame)
            {
                j = jobList.erase(j);
            }
            else
                j++;
        }
    }
    computation.endComputation();

    // set the time back
    MGlobal::viewFrame(oldCurTime);

    return MS::kSuccess;
}
catch (Alembic::Util::Exception & e)
{
    MString theError("Alembic Exception encountered: ");
    theError += e.what();
    MGlobal::displayError(theError);
    return MS::kFailure;
}
catch (std::exception & e)
{
    MString theError("std::exception encountered: ");
    theError += e.what();
    MGlobal::displayError(theError);
    return MS::kFailure;
}

}



MStatus initializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin(obj, "Alembic", ABCEXPORT_VERSION, "Any");

    status = plugin.registerCommand(
        "AbcExport", AbcExport::creator,
        AbcExport::createSyntax );

    if (!status)
    {
        status.perror("registerCommand");
    }

    MString info = "AbcExport v";
    info += ABCEXPORT_VERSION;
    info += " using ";
    info += Alembic::Abc::GetLibraryVersion().c_str();
    MGlobal::displayInfo(info);

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
