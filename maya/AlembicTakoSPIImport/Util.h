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

#ifndef _AlembicTakoImport_Util_h_
#define _AlembicTakoImport_Util_h_

#include <AlembicTakoSPIImport/Foundation.h>

namespace Alembic {

// given the time in current Maya session, find the closest frame
// in the input file's frame range
float getValidCurrentFrame(float inputTime,
  const std::set<float> & iFrameRange);

// read the frame range of the input file
void getTakoFrameRange(const TakoSPI::TransformReaderPtr & iRootNode,
    double & oSeqStartFrame, double & oSeqEndFrame,
    std::set<float> & oFrameRange);

// replace one MObject with another, while keeping all the old hierarchy intact
// The objects have to be a Dag object
MStatus replaceDagObject(MObject & oldObject, MObject & newObject,
    const MString & name);

// disconnect all the plugs that are connected to this plug
MStatus disconnectAllPlugsTo(MPlug & dstPlug);

// given a string of names separated by space, this function returns
// a list of Dagpath for the nodes found and prints errors for those not found
MStatus getDagPathListByName(const MString & objectNames,
    std::vector<MDagPath> & dagPathList);

MStatus getObjectByName(const MString & name, MObject & object);

MStatus getDagPathByName(const MString & name, MDagPath & dagPath);

MStatus getPlugByName(const MString & objName,
    const MString & attrName, MPlug & plug);

// set playback range for the current Maya session
MStatus setPlayback(double min, double max, double cur);

// grab the set "initialShadingGroup" and add the the dag path to it
MStatus setInitialShadingGroup(const MString & dagNodeName);

MStatus deleteDagNode(MDagPath & dagPath);

MStatus deleteCurrentSelection();

// assuming this is a valid input of /xxx/xxx/xxx/xxx.hdf
bool stripTakoFileName(const MString & FilePath, MString & FileName);

// remove AlembicTakoCacheNodes that have no outgoing connections
bool removeDangleAlembicTakoCacheNodes();

// * currently a debug mode does nothing
void setDebug(bool debug);

inline void swap(double & a, double & b)
{
    double temp; temp = b; b = a; a = temp;
}

inline double getMin(double a, double b)
{
    return ((a < b)?a:b);
}

inline double getMax(double a, double b)
{
    return ((a > b)?a:b);
}

// check if the current frame is within the given range
// if not, clamp it to the range
inline void validCur(double & min, double & max, double & cur)
{
    if (min > max)
        swap(min, max);

    cur = getMax(cur, min);
    cur = getMin(cur, max);
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

} // End namespace Alembic

#endif  // _AlembicTakoImport_Util_h_
