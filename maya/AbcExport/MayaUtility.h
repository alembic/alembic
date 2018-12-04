//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#ifndef AlembicExport_MayaUtility_h
#define AlembicExport_MayaUtility_h

#include "Foundation.h"
#include <Alembic/Abc/OArrayProperty.h>
#include <Alembic/Abc/OScalarProperty.h>

namespace util
{

struct cmpDag
{
    bool operator()( const MDagPath& lhs, const MDagPath& rhs ) const
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

// safely inverse a scale component
inline double inverseScale(double scale)
{
    const double kScaleEpsilon = 1.0e-12;

    if (scale < kScaleEpsilon && scale >= 0.0)
        return 1.0 / kScaleEpsilon;
    else if (scale > -kScaleEpsilon && scale < 0.0)
        return 1.0 / -kScaleEpsilon;
    else
        return 1.0 / scale;
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

// determine if a joint is driven by FBIK.
// The joint is animated but has no input connections.
bool isDrivenByFBIK(const MFnIkJoint & iJoint);

// determine if a joint is driven by a spline ik.
// The joint's is animated but has no input connections.
bool isDrivenBySplineIK(const MFnIkJoint & iJoint);

// determine if a Maya Object is intermediate
bool isIntermediate(const MObject & object);

// returns true for visible and lod invisible and not templated objects
bool isRenderable(const MObject & object);

// strip iDepth namespaces from the node name, go from taco:foo:bar to bar
// for iDepth > 1
MString stripNamespaces(const MString & iNodeName, unsigned int iDepth);

// returns the Help string for AbcExport
MString getHelpText();

} // namespace util


#define MCHECKERROR(_status)        \
{                                                         \
    MStatus _maya_status = (_status);                    \
    if ( MStatus::kSuccess != _maya_status )             \
    {                                                    \
        std::cout << "\nAPI error detected in " << __FILE__     \
             <<    " at line "    << __LINE__ << std::endl;        \
        _maya_status.perror ( "" );                        \
        return (_status);                                \
    }                                                    \
}

#define MCHECKERROR_NO_RET(_status)        \
{                                                         \
    MStatus _maya_status = (_status);                    \
    if ( MStatus::kSuccess != _maya_status )             \
    {                                                    \
        std::cout << "\nAPI error detected in " << __FILE__     \
             <<    " at line "    << __LINE__ << std::endl;        \
        _maya_status.perror ( "" );                        \
    }                                                    \
}

struct PlugAndObjScalar
{
    MPlug plug;
    MObject obj;
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
        filterEulerRotations = false;
        noNormals = false;
        setFirstAnimShape = false;
        stripNamespace = 0;
        useSelectionList = false;
        worldSpace = false;
        writeVisibility = false;
        writeUVs = false;
        writeColorSets = false;
        writeFaceSets = false;
        writeUVSets = false;
        writeGeometry = true;
        writeCurvesGroup = true;
        writeTransforms = true;
        writeLocators = true;
        writeParticles = true;
        writeMeshes = true;
        writeCameras = true;
        writeNurbsSurfaces = true;
        writeNurbsCurves = true;
        autoSubd = false;
    }

    bool excludeInvisible;
    bool filterEulerRotations;
    bool noNormals;
    bool setFirstAnimShape;
    unsigned int stripNamespace;
    bool useSelectionList;
    bool writeGeometry;
    bool worldSpace;
    bool writeVisibility;
    bool writeUVs;
    bool writeColorSets;
    bool writeFaceSets;
    bool writeUVSets;
    bool writeCurvesGroup;
    bool writeTransforms;
    bool writeLocators;
    bool writeParticles;
    bool writeMeshes;
    bool writeCameras;
    bool writeNurbsSurfaces;
    bool writeNurbsCurves;
    bool autoSubd;

    std::string melPerFrameCallback;
    std::string melPostCallback;
    std::string pythonPerFrameCallback;
    std::string pythonPostCallback;

    // to put into .arbGeomParam
    std::vector< std::string > prefixFilters;
    std::set< std::string > attribs;

    // to put into .userProperties
    std::vector< std::string > userPrefixFilters;
    std::set< std::string > userAttribs;

    util::ShapeSet dagPaths;
};

struct FrameRangeArgs
{
    FrameRangeArgs()
    {
        startTime = 0.0;
        endTime = 0.0;
        strideTime = 1.0;
        preRoll = false;
    }

    double startTime;
    double endTime;
    double strideTime;

    std::set< double > shutterSamples;

    bool preRoll;
};

#endif  // AlembicExport_MayaUtility_h
