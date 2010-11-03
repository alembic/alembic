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

#ifndef _AlembicSimpleAbcExport_Foundation_h_
#define _AlembicSimpleAbcExport_Foundation_h_

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <ImathBoxAlgo.h>

#include <maya/MStatus.h>

#include <maya/MGlobal.h>
#include <maya/MAnimUtil.h>
#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MAnimControl.h>
#include <maya/MFileIO.h>
#include <maya/MComputation.h>
#include <maya/MArgList.h>
#include <maya/M3dView.h>

#include <maya/MPxCommand.h>

#include <maya/MSelectionList.h>

#include <maya/MObject.h>
#include <maya/MObjectHandle.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>

#include <maya/MFnAttribute.h>
#include <maya/MFnTransform.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>

#include <maya/MItDag.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItSelectionList.h>
#include <maya/MItSurfaceCV.h>
#include <maya/MItCurveCV.h>
#include <maya/MItDependencyNodes.h>

#include <maya/MDataHandle.h>
#include <maya/MMatrix.h>
#include <maya/MColor.h>
#include <maya/MBoundingBox.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MString.h>
#include <maya/MSyntax.h>

#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>

#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <string>

#include <assert.h>
#include <math.h>

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
namespace Abc = ::Alembic::AbcGeom;

//-*****************************************************************************
// This is declared and defined in MayaUtil.h/cpp
extern void CheckMayaStatus( MStatus&, const std::string &, size_t );

//-*****************************************************************************
#define CHECK_MAYA_STATUS AlembicSimpleAbcExport::CheckMayaStatus( status,      \
                                           __FILE__,            \
                                           __LINE__ )

//-*****************************************************************************
template <class T>
inline T degrees( const T &rad )
{
    return rad * ( T )(180.0/M_PI);
}

//-*****************************************************************************
template <class T>
inline T radians( const T &rad )
{
    return rad * ( T )(M_PI/180.0);
}

} // End namespace AlembicSimpleAbcExport

#endif
