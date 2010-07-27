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

#include <AlembicTakoImport/AlembicTakoImport.h>
#include <AlembicTakoImport/Util.h>
#include <AlembicTakoImport/NodeIteratorVisitorHelper.h>

namespace Alembic {

namespace
{
    MString usage(
"                                                                           \n\
AlembicTakoImport  [options] File                                                 \n\n\
Options:                                                                    \n\
-rpr/ reparent      DagPath                                                 \n\
                    reparent the whole hierarchy under a node in the        \n\
                    current Maya scene                                      \n\
-ftr/ fitTimeRange                                                          \n\
                    Change Maya time slider to fit the range of input file. \n\
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
 and connections to the AlembicTakoCacheNode will be made or updated accordingly.  \n\
                    If string \"/\" is used as the root name,  all top level  \
nodes from the input file will be used for updating the current Maya scene. \n\
                    Again if certain node doesn't exist in the current scene, \
a warning will be given and nothing will be done.                           \n\
-crt/ createIfNotFound                                                      \n\
                    Used only when -connect flag is set.                    \n\
-rm / removeIfNoUpdate                                                      \n\
                    Used only when -connect flag is set.                    \n\
-uv / loadUVs                                                               \n\
                    Load mesh uv coordinates from the specified file if -cmp\n\
 is set. Otherwise load uv from the input file.                             \n\
-n  / loadNormals                                                           \n\
                    Load mesh normals from the specified file if -cmp is set\n\
. Otherwise load normals from the input file.                               \n\
-sts/ setToStartFrame                                                       \n\
                    Set the current time to the start of the frame range    \n\
-t  / timeRange     double beginFrame double endFrame                       \n\
                    Specify the frame range of the imported file.           \n\
-m  / mode          string (\"open\"|\"import\"|\"replace\")                \n\
                    Set read mode to open/import/replace (default to import)\n\
-h  / help          Print this message                                      \n\
-d  / debug         Turn on debug message printout                        \n\n\
Example:                                                                    \n\
AlembicTakoImport -h;                                                               \n\
AlembicTakoImport -d -m open \"/tmp/test.hdf\";                                     \n\
AlembicTakoImport -t 1 24 -ftr -ct \"/\" -crt -rm \"/mcp/test.hdf\";                \n\
AlembicTakoImport -ct \"root1 root2 root3 ...\"\"/mcp/test.hdf\";                   \n\
AlembicTakoImport -t 1 251 -ftr -sts -cmp \"/mcp/comp.attr\" -uv -n \"/mcp/test.hdf\";"
);  // usage

};


tako2maya::tako2maya()
{
}

tako2maya::~tako2maya()
{
}

MSyntax tako2maya::createSyntax()
{
    MSyntax syntax;

    syntax.addFlag("-d",    "-debug",        MSyntax::kNoArg);
    syntax.addFlag("-ftr",  "-fitTimeRange", MSyntax::kNoArg);
    syntax.addFlag("-h",    "-help",         MSyntax::kNoArg);
    syntax.addFlag("-m",    "-mode",         MSyntax::kString);
    syntax.addFlag("-n",    "-loadNormals",  MSyntax::kNoArg);
    syntax.addFlag("-uv",   "-loadUVs",      MSyntax::kNoArg);

    syntax.addFlag("-ct",   "-connect",          MSyntax::kString);
    syntax.addFlag("-crt",  "-createIfNotFound", MSyntax::kNoArg);
    syntax.addFlag("-rm",   "-removeIfNoUpdate", MSyntax::kNoArg);

    syntax.addFlag("-rpr",  "-reparent",     MSyntax::kString);
    syntax.addFlag("-sts",  "-setToStartFrame",  MSyntax::kNoArg);
    syntax.addFlag("-t",    "-timeRange",    MSyntax::kDouble,
                                             MSyntax::kDouble);

    syntax.addArg(MSyntax::kString);

    syntax.enableQuery(true);
    syntax.enableEdit(false);

    return syntax;
}


void* tako2maya::creator()
{
    return new tako2maya();
}


MStatus tako2maya::doIt(const MArgList & args)
{
    MStatus status;

    MArgParser argData(syntax(), args, &status);

    MString filename("");
    MString connectRootNodes("");

    MObject reparentObj = MObject::kNullObj;

    bool    loadUVs = false;
    bool    loadNormals = false;
    MString compAttrFileName("");

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

    if (argData.isFlagSet("loadUVs"))
        loadUVs = true;

    if (argData.isFlagSet("loadNormals"))
        loadNormals = true;

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

    double sequenceStartFrame(0.0), sequenceEndFrame(0.0);
    if (argData.isFlagSet("timeRange"))
    {
        status = argData.getFlagArgument("timeRange", 0, sequenceStartFrame);
        status = argData.getFlagArgument("timeRange", 1, sequenceEndFrame);

        if (sequenceStartFrame < sequenceEndFrame)
        {
            if (argData.isFlagSet("fitTimeRange"))
                setPlayback(sequenceStartFrame, sequenceEndFrame,
                    sequenceStartFrame);

            if (argData.isFlagSet("setToStartFrame"))
                MGlobal::viewFrame(sequenceStartFrame);
        }
    }

    status = argData.getCommandArgument(0, filename);
    MString takoCacheNodeName;
    if (status == MS::kSuccess)
    {
        MFileObject fileObj;
        status = fileObj.setRawFullName(filename);
        if (status == MS::kSuccess && fileObj.exists())
        {
            ArgData inputData(filename,
                sequenceStartFrame, sequenceEndFrame, debugOn, reparentObj,
                swap, connectRootNodes, createIfNotFound, removeIfNoUpdate,
                loadUVs, loadNormals);
            takoCacheNodeName = createScene(inputData);
        }
        else
        {
            MString theError("In AlembicTakoImport::doIt(), \"");
            theError += filename;
            theError += MString("\" doesn't exist");
            theError += "\""+filename+"\" doesn't exist";
            printError(theError);
        }
    }

    MPxCommand::setResult(takoCacheNodeName);

    return status;
}

} // End namespace Alembic


