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

#ifndef _AlembicExport_MayaUtility_h_
#define _AlembicExport_MayaUtility_h_

#include "Foundation.h"
#include <Alembic/Abc/OArrayProperty.h>
#include <Alembic/Abc/OScalarProperty.h>

namespace util
{

struct cmpDag
{
    bool operator()( const MDagPath& lhs, const MDagPath& rhs )
    {
            std::string name1(lhs.fullPathName().asChar());
            std::string name2(rhs.fullPathName().asChar());
            return (name1.compare(name2) < 0);
     }
};
typedef std::set< MDagPath, cmpDag > ShapeSet;

inline MStatus isFloat(MString str, const MString & usage)
{
    MStatus status = MS::kSuccess;

    if (!str.isFloat())
    {
        MGlobal::displayInfo(usage);
        status = MS::kFailure;
    }

    return status;
}

inline MStatus isUnsigned(MString str, const MString & usage)
{
    MStatus status = MS::kSuccess;

    if (!str.isUnsigned())
    {
        MGlobal::displayInfo(usage);
        status = MS::kFailure;
    }

    return status;
}

// seconds per frame
double spf();

bool isAncestorDescendentRelationship(const MDagPath & path1,
    const MDagPath & path2);

// returns 0 if static, 1 if sampled, and 2 if a curve
int getSampledType(const MPlug& iPlug);

// 0 dont write, 1 write static 0, 2 write anim 0, 3 write anim 1
int getVisibilityType(const MPlug & iPlug);

// determines what order we do the rotation in, returns false if iOrder is
// kInvalid or kLast
bool getRotOrder(MTransformationMatrix::RotationOrder iOrder,
    unsigned int & oXAxis, unsigned int & oYAxis, unsigned int & oZAxis);

// determine if a Maya Object is animated or not
// copy from mayapit code (MayaPit.h .cpp)
bool isAnimated(MObject & object, bool checkParent = false);

// determine if a Maya Object is intermediate
bool isIntermediate(const MObject & object);

// returns true for visible and lod invisible and not templated objects
bool isRenderable(const MObject & object);

// strip all namespaces from the node name, go from taco:foo:bar to bar
MString stripNamespaces(const MString & iNodeName);

// returns the Help string for AbcExport
MString getHelpText();

} // namespace util

struct PlugAndObjScalar
{
    MPlug plug;
    MObject obj;
    Alembic::Abc::OObject propParent;
    Alembic::Abc::OScalarProperty prop;
};

struct PlugAndObjArray
{
    MPlug plug;
    MObject obj;
    Alembic::Abc::OArrayProperty prop;
};

struct JobArgs
{
    JobArgs()
    {
        excludeInvisible = false;
        noNormals = false;
        stripNamespace = false;
        useSelectionList = false;
        worldSpace = false;
        writeVisibility = false;
        writeUVs = false;
    }

    bool excludeInvisible;
    bool noNormals;
    bool stripNamespace;
    bool useSelectionList;
    bool worldSpace;
    bool writeVisibility;
    bool writeUVs;

    std::string melPerFrameCallback;
    std::string melPostCallback;
    std::string pythonPerFrameCallback;
    std::string pythonPostCallback;

    std::vector< std::string > prefixFilters;
    std::set< std::string > attribs;
    util::ShapeSet dagPaths;
};

#endif  // _AlembicExport_MayaUtility_h_
