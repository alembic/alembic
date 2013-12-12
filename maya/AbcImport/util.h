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

#ifndef ABCIMPORT_UTIL_H_
#define ABCIMPORT_UTIL_H_

#include <cmath>
#include <vector>
#include <set>
#include <string>

#include <Alembic/AbcGeom/All.h>
#include "NodeIteratorVisitorHelper.h"

#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MFnDependencyNode.h>

// Return the output connected shading groups from a shape object
MObjectArray getOutConnectedSG( const MDagPath &shapeDPath );

// Create a new shading group with the given name
MObject createShadingGroup(const MString& iName);

// replace one MObject with another, while keeping all the old hierarchy intact
// The objects have to be a Dag object
MStatus replaceDagObject(MObject & oldObject, MObject & newObject,
    const MString & name);

// disconnect all plugs based on the name of the IArrayProperty
// from iSampledPropList[iFirstProp] to the end of iSampledPropList
void disconnectProps(MFnDependencyNode & iNode,
    std::vector<Prop> & iSampledPropList,
    std::size_t iFirstProp);

// disconnect all the plugs that are connected to this plug
MStatus disconnectAllPlugsTo(MPlug & dstPlug);

// given a string of names separated by space, this function returns
// a list of Dagpath for the nodes found and prints errors for those not found
MStatus getDagPathListByName(const MString & objectNames,
    std::vector<MDagPath> & dagPathList);

MStatus getObjectByName(const MString & name, MObject & object);

MStatus getDagPathByName(const MString & name, MDagPath & dagPath);

bool getDagPathByChildName(MDagPath & ioDagPath,
    const std::string & iChildName);

// returns the Maya style leaf name minus the namespace
std::string stripPathAndNamespace(const std::string & iPath);

MStatus getPlugByName(const MString & objName,
    const MString & attrName, MPlug & plug);

// set playback range for the current Maya session
MStatus setPlayback(double min, double max);

// grab the set "initialShadingGroup" and add the the dag path to it
MStatus setInitialShadingGroup(const MString & dagNodeName);

MStatus deleteDagNode(MDagPath & dagPath);

MStatus deleteCurrentSelection();

// assuming this is a valid input of /xxx/xxx/xxx/xxx.abc
bool stripFileName(const MString & filePath, MString & fileName);

double getWeightAndIndex(double iFrame,
    Alembic::AbcCoreAbstract::TimeSamplingPtr iTime, size_t numSamps,
    Alembic::AbcCoreAbstract::index_t & oIndex,
    Alembic::AbcCoreAbstract::index_t & oCeilIndex);

bool isColorSet(const Alembic::AbcCoreAbstract::PropertyHeader & iHeader,
    bool iUnmarkedFaceVaryingColors);

template<typename T>
void clamp(T & min, T & max, T & cur)
{
    if (min > max)
        std::swap(min, max);

    cur = std::max(cur, min);
    cur = std::min(cur, max);
}

template<typename T>
T simpleLerp(double alpha, T val1, T val2)
{
    double dv = static_cast<double>( val1 );
    return static_cast<T>( dv + alpha * (static_cast<double>(val2) - dv) );
}

template<typename T>
void vectorLerp(double alpha, std::vector<T> & vec,
    std::vector<T> & vec1, std::vector<T> & vec2)
{
    vec.clear();
    if (vec1.size() != vec2.size())
        return;
    unsigned int size = vec1.size();
    vec.reserve(size);
    for (unsigned int i = 0; i < size; i++)
    {
        T v1 = vec1[i];
        T val = v1 + alpha * (vec2[i] - vec1[i]);
        vec.push_back(val);
    }
}

// convert the status to an MString
inline MString getMStatus(MStatus status)
{
    MString theError("Return status: ");
    theError += status.errorString();
    theError += MString("\n");
    return theError;
}

inline void printError(const MString & theError)
{
    MGlobal::displayError(theError);
}

inline void printWarning(const MString & theWarning)
{
    MGlobal::displayWarning(theWarning);
}

#endif  // ABCIMPORT_UTIL_H_
