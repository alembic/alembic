//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _Alembic_TakoAbstract_SharedTypes_h_
#define _Alembic_TakoAbstract_SharedTypes_h_

#include <Alembic/TakoAbstract/Foundation.h>

namespace Alembic {

namespace TakoAbstract {

namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
// Reader typedefs
class PolyMeshReader;
class SubDReader;
class NurbsCurveReader;
class NurbsSurfaceReader;
class TransformReader;
class CameraReader;
class GenericNodeReader;
class PointPrimitiveReader;

typedef boost::shared_ptr< PolyMeshReader > PolyMeshReaderPtr;
typedef boost::shared_ptr< SubDReader > SubDReaderPtr;
typedef boost::shared_ptr< NurbsSurfaceReader > NurbsSurfaceReaderPtr;
typedef boost::shared_ptr< NurbsCurveReader > NurbsCurveReaderPtr;
typedef boost::shared_ptr< TransformReader > TransformReaderPtr;
typedef boost::shared_ptr< CameraReader > CameraReaderPtr;
typedef boost::shared_ptr< GenericNodeReader > GenericNodeReaderPtr;
typedef boost::shared_ptr< PointPrimitiveReader > PointPrimitiveReaderPtr;

typedef boost::variant
    <   PolyMeshReaderPtr,
        SubDReaderPtr,
        NurbsSurfaceReaderPtr,
        NurbsCurveReaderPtr,
        TransformReaderPtr,
        CameraReaderPtr,
        GenericNodeReaderPtr,
        PointPrimitiveReaderPtr > ChildNodePtr;

//-*****************************************************************************
// Writer typedefs
class PolyMeshWriter;
class SubDWriter;
class NurbsSurfaceWriter;
class NurbsCurveWriter;
class TransformWriter;
class CameraWriter;
class GenericNodeWriter;
class PointPrimitiveWriter;

typedef boost::shared_ptr< PolyMeshWriter > PolyMeshWriterPtr;
typedef boost::shared_ptr< SubDWriter > SubDWriterPtr;
typedef boost::shared_ptr< NurbsSurfaceWriter > NurbsSurfaceWriterPtr;
typedef boost::shared_ptr< NurbsCurveWriter > NurbsCurveWriterPtr;
typedef boost::shared_ptr< TransformWriter > TransformWriterPtr;
typedef boost::shared_ptr< CameraWriter > CameraWriterPtr;
typedef boost::shared_ptr< GenericNodeWriter > GenericNodeWriterPtr;
typedef boost::shared_ptr< PointPrimitiveWriter > PointPrimitiveWriterPtr;


//-*****************************************************************************
//-*****************************************************************************
// CONSTRUCTOR FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// WRITER

//-*****************************************************************************
//-*****************************************************************************
// PolyMesh
PolyMeshWriterPtr
MakePolyMeshWriterPtr( const std::string & iName,
                       TransformWriter & iParent );

//-*****************************************************************************
PolyMeshWriterPtr
MakePolyMeshWriterPtr( const std::string & iName,
                       TransformWriterPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// SubD
SubDWriterPtr
MakeSubDWriterPtr( const std::string & iName,
                   TransformWriter & iParent );

//-*****************************************************************************
SubDWriterPtr
MakeSubDWriterPtr( const std::string & iName,
                   TransformWriterPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// Transform
TransformWriterPtr
MakeTransformWriterPtr( const std::string & iName,
                        TransformWriter & iParent );

//-*****************************************************************************
TransformWriterPtr
MakeTransformWriterPtr( const std::string & iName,
                        TransformWriterPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// NurbsCurve
NurbsCurveWriterPtr
MakeNurbsCurveWriterPtr( const std::string & iName,
                         TransformWriter & iParent );

//-*****************************************************************************
NurbsCurveWriterPtr
MakeNurbsCurveWriterPtr( const std::string & iName,
                         TransformWriterPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// NurbsSurface
NurbsSurfaceWriterPtr
MakeNurbsSurfaceWriterPtr( const std::string & iName,
                           TransformWriter & iParent );

//-*****************************************************************************
NurbsSurfaceWriterPtr
MakeNurbsSurfaceWriterPtr( const std::string & iName,
                           TransformWriterPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// Camera
CameraWriterPtr
MakeCameraWriterPtr( const std::string & iName,
                     TransformWriter & iParent );

//-*****************************************************************************
CameraWriterPtr
MakeCameraWriterPtr( const std::string & iName,
                     TransformWriterPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// PointPrimitive
PointPrimitiveWriterPtr
MakePointPrimitiveWriterPtr( const std::string & iName,
                             TransformWriter & iParent );

//-*****************************************************************************
PointPrimitiveWriterPtr
MakePointPrimitiveWriterPtr( const std::string & iName,
                             TransformWriterPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// GenericNode
GenericNodeWriterPtr
MakeGenericNodeWriterPtr( const std::string & iName,
                          TransformWriter & iParent );

//-*****************************************************************************
GenericNodeWriterPtr
MakeGenericNodeWriterPtr( const std::string & iName,
                          TransformWriterPtr iParent );


//-*****************************************************************************
// READER

//-*****************************************************************************
//-*****************************************************************************
// PolyMesh
PolyMeshReaderPtr
MakePolyMeshReaderPtr( const std::string & iName,
                       TransformReader & iParent );

//-*****************************************************************************
PolyMeshReaderPtr
MakePolyMeshReaderPtr( const std::string & iName,
                       TransformReaderPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// SubD
SubDReaderPtr
MakeSubDReaderPtr( const std::string & iName,
                   TransformReader & iParent );

//-*****************************************************************************
SubDReaderPtr
MakeSubDReaderPtr( const std::string & iName,
                   TransformReaderPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// Transform
TransformReaderPtr
MakeTransformReaderPtr( const std::string & iName,
                        TransformReader & iParent );

//-*****************************************************************************
TransformReaderPtr
MakeTransformReaderPtr( const std::string & iName,
                        TransformReaderPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// NurbsCurve
NurbsCurveReaderPtr
MakeNurbsCurveReaderPtr( const std::string & iName,
                         TransformReader & iParent );

//-*****************************************************************************
NurbsCurveReaderPtr
MakeNurbsCurveReaderPtr( const std::string & iName,
                         TransformReaderPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// NurbsSurface
NurbsSurfaceReaderPtr
MakeNurbsSurfaceReaderPtr( const std::string & iName,
                           TransformReader & iParent );

//-*****************************************************************************
NurbsSurfaceReaderPtr
MakeNurbsSurfaceReaderPtr( const std::string & iName,
                           TransformReaderPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// Camera
CameraReaderPtr
MakeCameraReaderPtr( const std::string & iName,
                     TransformReader & iParent );

//-*****************************************************************************
CameraReaderPtr
MakeCameraReaderPtr( const std::string & iName,
                     TransformReaderPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// PointPrimitive
PointPrimitiveReaderPtr
MakePointPrimitiveReaderPtr( const std::string & iName,
                             TransformReader & iParent );

//-*****************************************************************************
PointPrimitiveReaderPtr
MakePointPrimitiveReaderPtr( const std::string & iName,
                             TransformReaderPtr iParent );

//-*****************************************************************************
//-*****************************************************************************
// GenericNode
GenericNodeReaderPtr
MakeGenericNodeReaderPtr( const std::string & iName,
                          TransformReader & iParent );

//-*****************************************************************************
GenericNodeReaderPtr
MakeGenericNodeReaderPtr( const std::string & iName,
                          TransformReaderPtr iParent );

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS

} // End namespace TakoAbstract

} // End namespace Alembic

#endif  // _Alembic_TakoAbstract_SharedTypes_h_
