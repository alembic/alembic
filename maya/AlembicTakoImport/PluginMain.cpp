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

#include <AlembicTakoImport/AlembicTakoCacheNode.h>
#include <AlembicTakoImport/AlembicTakoImport.h>
#include <maya/MFnPlugin.h>

namespace Alembic {

const MTypeId AlembicTakoCacheNode::mMayaNodeId(0x00083201);

} // End namespace Alembic

MStatus initializePlugin(MObject obj)
{
    MFnPlugin plugin(obj, "Alembic", "1.0", "Any");

    MStatus status = plugin.registerCommand(
        "AlembicTakoImport",
        Alembic::tako2maya::creator,
        Alembic::tako2maya::createSyntax);

    status = plugin.registerNode(
        "AlembicTakoCacheNode",
        Alembic::AlembicTakoCacheNode::mMayaNodeId,
        &Alembic::AlembicTakoCacheNode::creator,
        &Alembic::AlembicTakoCacheNode::initialize);

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);

    MStatus status;

    status = plugin.deregisterCommand("AlembicTakoImport");
    status = plugin.deregisterNode(
        Alembic::AlembicTakoCacheNode::mMayaNodeId);

    return status;
}
