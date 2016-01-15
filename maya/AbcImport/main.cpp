//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include "AlembicNode.h"
#include "AbcImport.h"
#include "AlembicImportFileTranslator.h"
#include "Export.h"

#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>

// Interesting trivia: 0x2697 is the unicode character for Alembic
const MTypeId AlembicNode::mMayaNodeId(0x00082697);

ALEMBIC_MAYA_PLUGIN_EXPORT MStatus initializePlugin(MObject obj)
{
    const char * pluginVersion = "1.0";
    MFnPlugin plugin(obj, "Alembic", pluginVersion, "Any");

    MStatus status;

    status = plugin.registerCommand("AbcImport",
                                AbcImport::creator,
                                AbcImport::createSyntax);
    if (!status)
    {
        status.perror("registerCommand");
    }

    status = plugin.registerNode("AlembicNode",
                                AlembicNode::mMayaNodeId,
                                &AlembicNode::creator,
                                &AlembicNode::initialize);
    if (!status)
    {
        status.perror("registerNode");
    }

    status = plugin.registerFileTranslator("Alembic",
                                NULL,
                                AlembicImportFileTranslator::creator,
                                NULL,
                                NULL,
                                true);
    if (!status)
    {
        status.perror("registerFileTranslator");
    }

    MString info = "AbcImport v";
    info += pluginVersion;
    info += " using ";
    info += Alembic::AbcCoreAbstract::GetLibraryVersion().c_str();
    MGlobal::displayInfo(info);

    return status;
}

ALEMBIC_MAYA_PLUGIN_EXPORT  MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);

    MStatus status;

    status = plugin.deregisterFileTranslator("Alembic");
    if (!status)
    {
        status.perror("deregisterFileTranslator");
    }

    status = plugin.deregisterNode(AlembicNode::mMayaNodeId);
    if (!status)
    {
        status.perror("deregisterNode");
    }

    status = plugin.deregisterCommand("AbcImport");
    if (!status)
    {
        status.perror("deregisterCommand");
    }

    return status;
}
