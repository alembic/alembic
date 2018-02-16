//-*****************************************************************************
//
// Copyright (c) 2009-2012,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#include "util.h"
#include "NodeIteratorVisitorHelper.h"
#include "AbcImport.h"

#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MFileIO.h>
#include <maya/MFileObject.h>
#include <maya/MGlobal.h>
#include <maya/MDGModifier.h>
#include <maya/MSelectionList.h>
#include <maya/MStringArray.h>
#include <maya/MString.h>
#include <maya/MSyntax.h>
#include <maya/MTime.h>


namespace
{
    MString usage(
"                                                                           \n\
AbcImport  [options] File [File2 File3 ... ]                              \n\n\
Options:                                                                    \n\
-rpr/ reparent      DagPath                                                 \n\
                    reparent the whole hierarchy under a node in the        \n\
                    current Maya scene                                      \n\
-ftr/ fitTimeRange                                                          \n\
                    Change Maya time slider to fit the range of input file. \n\
-rcs / recreateAllColorSets                                                 \n\
                    IC3/4fArrayProperties with face varying scope on        \n\
                    IPolyMesh and ISubD are treated as color sets even if   \n\
                    they weren't written out of Maya.                       \n\
-ct / connect       string node1 node2 ...                                  \n\
                    The nodes specified in the argument string are supposed to\
 be the names of top level nodes from the input file.                       \n\
                    If such a node doesn't exist in the provided input file, a\
warning will be given and nothing will be done.                             \n\
                    If Maya DAG node of the same name doesn't exist in the    \
current Maya scene,  a warning will be given and nothing will be done.      \n\
                    If such a node exists both in the input file and in the   \
current Maya scene, data for the whole hierarchy from the nodes down        \n\
                    (inclusive) will be substituted by data from the input file,\
 and connections to the AlembicNode will be made or updated accordingly.    \n\
                    If string \"/\" is used as the root name,  all top level  \
nodes from the input file will be used for updating the current Maya scene. \n\
                    Again if certain node doesn't exist in the current scene, \
a warning will be given and nothing will be done.                           \n\
                    If a single node is specified and it exists in the Maya \
scene but doesn't exist in the archive, children of that node will be connected\
 to the children of the archive.                                            \n\
-crt/ createIfNotFound                                                      \n\
                    Used only when -connect flag is set.                    \n\
-rm / removeIfNoUpdate                                                      \n\
                    Used only when -connect flag is set.                    \n\
-sts/ setToStartFrame                                                       \n\
                    Set the current time to the start of the frame range    \n\
-m  / mode          string (\"open\"|\"import\"|\"replace\")                \n\
                    Set read mode to open/import/replace (default to import)\n\
-ft / filterObjects \"regex1 regex2 ...\"                                   \n\
                    Selective import cache objects whose name matches with  \n\
-eft / excludeFilterObjects \"regex1 regex2 ...\"                          \n\
                    Selective exclude cache objects whose name matches with \n\
the input regular expressions.                                              \n\
-h  / help          Print this message                                      \n\
-d  / debug         Turn on debug message printout                        \n\n\
Specifying more than one file will layer those files together.            \n\n\
Example:                                                                    \n\
AbcImport -h;                                                               \n\
AbcImport -d -m open \"/tmp/test.abc\";                                     \n\
AbcImport -ftr -ct \"/\" -crt -rm \"/tmp/test.abc\";                        \n\
AbcImport -ct \"root1 root2 root3 ...\" \"/tmp/test.abc\";                  \n\
AbcImport \"/tmp/test.abc\" \"/tmp/justUVs.abc\" \"/tmp/other.abc\"         \n"
);  // usage

};


AbcImport::AbcImport()
{
}

AbcImport::~AbcImport()
{
}

MSyntax AbcImport::createSyntax()
{
    MSyntax syntax;

    syntax.addFlag("-d",    "-debug",        MSyntax::kNoArg);
    syntax.addFlag("-ftr",  "-fitTimeRange", MSyntax::kNoArg);
    syntax.addFlag("-h",    "-help",         MSyntax::kNoArg);
    syntax.addFlag("-m",    "-mode",         MSyntax::kString);
    syntax.addFlag("-rcs",  "-recreateAllColorSets", MSyntax::kNoArg);

    syntax.addFlag("-ct",   "-connect",          MSyntax::kString);
    syntax.addFlag("-crt",  "-createIfNotFound", MSyntax::kNoArg);
    syntax.addFlag("-rm",   "-removeIfNoUpdate", MSyntax::kNoArg);

    syntax.addFlag("-rpr",  "-reparent",     MSyntax::kString);
    syntax.addFlag("-sts",  "-setToStartFrame",  MSyntax::kNoArg);

    syntax.addFlag("-ft",   "-filterObjects",    MSyntax::kString);
    syntax.addFlag("-eft",  "-excludeFilterObjects",    MSyntax::kString);

    syntax.setObjectType( MSyntax::kStringObjects, 1, 1024 );

    syntax.enableQuery(true);
    syntax.enableEdit(false);

    return syntax;
}


void* AbcImport::creator()
{
    return new AbcImport();
}

MStatus AbcImport::doIt(const MArgList & args)
{
    MStatus status;

    MArgParser argData(syntax(), args, &status);

    MString filename("");
    MString connectRootNodes("");
    MString filterString("");
    MString excludeFilterString("");

    MObject reparentObj = MObject::kNullObj;

    bool    swap = false;
    bool    createIfNotFound = false;
    bool    removeIfNoUpdate = false;

    bool    debugOn = false;

    if (argData.isFlagSet("help"))
    {
        MGlobal::displayInfo(usage);
        return status;
    }

    if (argData.isFlagSet("debug"))
        debugOn = true;

    if (argData.isFlagSet("reparent"))
    {
        MString parent("");
        MDagPath reparentDagPath;
        status = argData.getFlagArgument("reparent", 0, parent);
        if (status == MS::kSuccess
            && getDagPathByName(parent, reparentDagPath) == MS::kSuccess)
        {
            reparentObj = reparentDagPath.node();
        }
        else
        {
            MString theWarning = parent;
            theWarning += MString(" is not a valid DagPath");
            printWarning(theWarning);
        }
    }

    if (!argData.isFlagSet("connect") && argData.isFlagSet("mode"))
    {
        MString modeStr;
        argData.getFlagArgument("mode", 0, modeStr);
        if (modeStr == "replace")
            deleteCurrentSelection();
        else if (modeStr == "open")
        {
            MFileIO fileIo;
            fileIo.newFile(true);
        }
    }
    else if (argData.isFlagSet("connect"))
    {
        swap = true;
        argData.getFlagArgument("connect", 0, connectRootNodes);

        if (argData.isFlagSet("createIfNotFound"))
        {
            createIfNotFound = true;
        }

        if (argData.isFlagSet("removeIfNoUpdate"))
            removeIfNoUpdate = true;
    }

    if (argData.isFlagSet("filterObjects"))
    {
        argData.getFlagArgument("filterObjects", 0, filterString);
    }

    if (argData.isFlagSet("excludeFilterObjects"))
    {
        argData.getFlagArgument("excludeFilterObjects", 0, excludeFilterString);
    }

    // if the flag isn't specified we'll only do stuff marked with the Maya
    // meta data
    bool recreateColorSets = false;
    if (argData.isFlagSet("recreateAllColorSets"))
    {
        recreateColorSets = true;
    }

    MString abcNodeName;

    MStringArray filenameArray;
    status = argData.getObjects( filenameArray );

    if( ( status == MStatus::kSuccess ) && ( filenameArray.length() > 0 ) )
    {
        std::vector< std::string > filenameList;

        for(unsigned int fileNum = 0; fileNum < filenameArray.length(); fileNum++)
        {
            filename = filenameArray[ fileNum ];

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

            // resolve the expanded file rule
            if (expandName.length() == 0)
            {
                expandName = alembicFilePath;
            }

            // get the path to the alembic file rule
            MFileObject directory;
            directory.setRawFullName(expandName);
            MString directoryName = directory.resolvedFullName();

            // resolve the relative path
            MFileObject absoluteFile;
            absoluteFile.setRawFullName(filename);
            absoluteFile.setResolveMethod(MFileObject::kInputFile);
#if MAYA_API_VERSION < 201300
            if (absoluteFile.resolvedFullName() !=
                absoluteFile.expandedFullName())
            {
#else
            if (!MFileObject::isAbsolutePath(filename)) {
#endif
                // this is a relative path
                MString absoluteFileName = directoryName + "/" + filename;
                absoluteFile.setRawFullName(absoluteFileName);
                filename = absoluteFile.resolvedFullName();
            }
            else
            {
                filename = absoluteFile.resolvedFullName();
            }

            MFileObject fileObj;
            status = fileObj.setRawFullName(filename);
            if (status == MS::kSuccess && fileObj.exists())
            {
                filenameList.push_back( filename.asUTF8() );
            }
            else
            {
                MString theError("In AbcImport::doIt(), ");
                theError += filename;
                theError += MString(" doesn't exist");
                printError(theError);
            }
        }

        if( filenameList.size() > 0 )
        {
            ArgData inputData(filenameList, debugOn, reparentObj,
                swap, connectRootNodes, createIfNotFound, removeIfNoUpdate,
                recreateColorSets, filterString, excludeFilterString);
            abcNodeName = createScene(inputData);

            if (inputData.mSequenceStartTime != inputData.mSequenceEndTime &&
                inputData.mSequenceStartTime != -DBL_MAX &&
                inputData.mSequenceEndTime != DBL_MAX)
            {
                if (argData.isFlagSet("fitTimeRange"))
                {
                    MTime sec(1.0, MTime::kSeconds);
                    setPlayback(
                        inputData.mSequenceStartTime * sec.as(MTime::uiUnit()),
                        inputData.mSequenceEndTime * sec.as(MTime::uiUnit()) );
                }

                if (argData.isFlagSet("setToStartFrame"))
                {
                    MTime sec(1.0, MTime::kSeconds);
                    MGlobal::viewFrame( inputData.mSequenceStartTime *
                        sec.as(MTime::uiUnit()) );
                }
            }
        }
    }

    MPxCommand::setResult(abcNodeName);

    return status;
}

