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

#include <Alembic/TakoAbstract/SharedTypes.h>
#include <Alembic/TakoAbstract/TransformWriter.h>
#include <Alembic/TakoAbstract/PolyMeshWriter.h>
#include <Alembic/TakoAbstract/SubDWriter.h>
#include <Alembic/TakoAbstract/CameraWriter.h>
#include <Alembic/TakoAbstract/NurbsCurveWriter.h>
#include <Alembic/TakoAbstract/NurbsSurfaceWriter.h>
#include <Alembic/TakoAbstract/PointPrimitiveWriter.h>
#include <Alembic/TakoAbstract/GenericNodeWriter.h>
#include <Alembic/TakoAbstract/TransformReader.h>
#include <Alembic/TakoAbstract/PolyMeshReader.h>
#include <Alembic/TakoAbstract/SubDReader.h>
#include <Alembic/TakoAbstract/CameraReader.h>
#include <Alembic/TakoAbstract/NurbsCurveReader.h>
#include <Alembic/TakoAbstract/NurbsSurfaceReader.h>
#include <Alembic/TakoAbstract/PointPrimitiveReader.h>
#include <Alembic/TakoAbstract/GenericNodeReader.h>

namespace Alembic {
namespace TakoAbstract {
namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
//-*****************************************************************************
// PolyMesh
PolyMeshWriterPtr
MakePolyMeshWriterPtr( const std::string & iName,
                       TransformWriter & iParent )
{
    return iParent.createPolyMeshChild( iName );
}

//-*****************************************************************************
PolyMeshWriterPtr
MakePolyMeshWriterPtr( const std::string & iName,
                       TransformWriterPtr iParent )
{
    return iParent->createPolyMeshChild( iName );
}

//-*****************************************************************************
//-*****************************************************************************
// SubD
SubDWriterPtr
MakeSubDWriterPtr( const std::string & iName,
                   TransformWriter & iParent )
{
    return iParent.createSubDChild( iName );
}

//-*****************************************************************************
SubDWriterPtr
MakeSubDWriterPtr( const std::string & iName,
                   TransformWriterPtr iParent )
{
    return iParent->createSubDChild( iName );
}

//-*****************************************************************************
//-*****************************************************************************
// Transform
TransformWriterPtr
MakeTransformWriterPtr( const std::string & iName,
                        TransformWriter & iParent )
{
    return iParent.createTransformChild( iName );
}

//-*****************************************************************************
TransformWriterPtr
MakeTransformWriterPtr( const std::string & iName,
                        TransformWriterPtr iParent )
{
    return iParent->createTransformChild( iName );
}

//-*****************************************************************************
//-*****************************************************************************
// NurbsCurve
NurbsCurveWriterPtr
MakeNurbsCurveWriterPtr( const std::string & iName,
                         TransformWriter & iParent )
{
    return iParent.createNurbsCurveChild( iName );
}

//-*****************************************************************************
NurbsCurveWriterPtr
MakeNurbsCurveWriterPtr( const std::string & iName,
                         TransformWriterPtr iParent )
{
    return iParent->createNurbsCurveChild( iName );
}

//-*****************************************************************************
//-*****************************************************************************
// NurbsSurface
NurbsSurfaceWriterPtr
MakeNurbsSurfaceWriterPtr( const std::string & iName,
                           TransformWriter & iParent )
{
    return iParent.createNurbsSurfaceChild( iName );
}

//-*****************************************************************************
NurbsSurfaceWriterPtr
MakeNurbsSurfaceWriterPtr( const std::string & iName,
                           TransformWriterPtr iParent )
{
    return iParent->createNurbsSurfaceChild( iName );
}

//-*****************************************************************************
//-*****************************************************************************
// Camera
CameraWriterPtr
MakeCameraWriterPtr( const std::string & iName,
                     TransformWriter & iParent )
{
    return iParent.createCameraChild( iName );
}

//-*****************************************************************************
CameraWriterPtr
MakeCameraWriterPtr( const std::string & iName,
                     TransformWriterPtr iParent )
{
    return iParent->createCameraChild( iName );
}

//-*****************************************************************************
//-*****************************************************************************
// PointPrimitive
PointPrimitiveWriterPtr
MakePointPrimitiveWriterPtr( const std::string & iName,
                             TransformWriter & iParent )
{
    return iParent.createPointPrimitiveChild( iName );
}

//-*****************************************************************************
PointPrimitiveWriterPtr
MakePointPrimitiveWriterPtr( const std::string & iName,
                             TransformWriterPtr iParent )
{
    return iParent->createPointPrimitiveChild( iName );
}

//-*****************************************************************************
//-*****************************************************************************
// GenericNode
GenericNodeWriterPtr
MakeGenericNodeWriterPtr( const std::string & iName,
                          TransformWriter & iParent )
{
    return iParent.createGenericNodeChild( iName );
}

//-*****************************************************************************
GenericNodeWriterPtr
MakeGenericNodeWriterPtr( const std::string & iName,
                          TransformWriterPtr iParent )
{
    return iParent->createGenericNodeChild( iName );
}



//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// READER
//-*****************************************************************************
//-*****************************************************************************


//-*****************************************************************************
//-*****************************************************************************
// PolyMesh
PolyMeshReaderPtr
MakePolyMeshReaderPtr( const std::string & iName,
                       TransformReader & iParent )
{
    return boost::get<PolyMeshReaderPtr>
        ( iParent.getChild( iName ) );
}

//-*****************************************************************************
PolyMeshReaderPtr
MakePolyMeshReaderPtr( const std::string & iName,
                       TransformReaderPtr iParent )
{
    return boost::get<PolyMeshReaderPtr>
        ( iParent->getChild( iName ) );
}

//-*****************************************************************************
//-*****************************************************************************
// SubD
SubDReaderPtr
MakeSubDReaderPtr( const std::string & iName,
                   TransformReader & iParent )
{
    return boost::get<SubDReaderPtr>
        ( iParent.getChild( iName ) );
}

//-*****************************************************************************
SubDReaderPtr
MakeSubDReaderPtr( const std::string & iName,
                   TransformReaderPtr iParent )
{
    return boost::get<SubDReaderPtr>
        ( iParent->getChild( iName ) );
}

//-*****************************************************************************
//-*****************************************************************************
// Transform
TransformReaderPtr
MakeTransformReaderPtr( const std::string & iName,
                        TransformReader & iParent )
{
    return boost::get<TransformReaderPtr>
        ( iParent.getChild( iName ) );
}

//-*****************************************************************************
TransformReaderPtr
MakeTransformReaderPtr( const std::string & iName,
                        TransformReaderPtr iParent )
{
    return boost::get<TransformReaderPtr>
        ( iParent->getChild( iName ) );
}

//-*****************************************************************************
//-*****************************************************************************
// NurbsCurve
NurbsCurveReaderPtr
MakeNurbsCurveReaderPtr( const std::string & iName,
                         TransformReader & iParent )
{
    return boost::get<NurbsCurveReaderPtr>
        ( iParent.getChild( iName ) );
}

//-*****************************************************************************
NurbsCurveReaderPtr
MakeNurbsCurveReaderPtr( const std::string & iName,
                         TransformReaderPtr iParent )
{
    return boost::get<NurbsCurveReaderPtr>
        ( iParent->getChild( iName ) );
}

//-*****************************************************************************
//-*****************************************************************************
// NurbsSurface
NurbsSurfaceReaderPtr
MakeNurbsSurfaceReaderPtr( const std::string & iName,
                           TransformReader & iParent )
{
    return boost::get<NurbsSurfaceReaderPtr>
        ( iParent.getChild( iName ) );
}

//-*****************************************************************************
NurbsSurfaceReaderPtr
MakeNurbsSurfaceReaderPtr( const std::string & iName,
                           TransformReaderPtr iParent )
{
    return boost::get<NurbsSurfaceReaderPtr>
        ( iParent->getChild( iName ) );
}

//-*****************************************************************************
//-*****************************************************************************
// Camera
CameraReaderPtr
MakeCameraReaderPtr( const std::string & iName,
                     TransformReader & iParent )
{
    return boost::get<CameraReaderPtr>
        ( iParent.getChild( iName ) );
}

//-*****************************************************************************
CameraReaderPtr
MakeCameraReaderPtr( const std::string & iName,
                     TransformReaderPtr iParent )
{
    return boost::get<CameraReaderPtr>
        ( iParent->getChild( iName ) );
}

//-*****************************************************************************
//-*****************************************************************************
// PointPrimitive
PointPrimitiveReaderPtr
MakePointPrimitiveReaderPtr( const std::string & iName,
                             TransformReader & iParent )
{
    return boost::get<PointPrimitiveReaderPtr>
        ( iParent.getChild( iName ) );
}

//-*****************************************************************************
PointPrimitiveReaderPtr
MakePointPrimitiveReaderPtr( const std::string & iName,
                             TransformReaderPtr iParent )
{
    return boost::get<PointPrimitiveReaderPtr>
        ( iParent->getChild( iName ) );
}

//-*****************************************************************************
//-*****************************************************************************
// GenericNode
GenericNodeReaderPtr
MakeGenericNodeReaderPtr( const std::string & iName,
                          TransformReader & iParent )
{
    return boost::get<GenericNodeReaderPtr>
        ( iParent.getChild( iName ) );
}

//-*****************************************************************************
GenericNodeReaderPtr
MakeGenericNodeReaderPtr( const std::string & iName,
                          TransformReaderPtr iParent )
{
    return boost::get<GenericNodeReaderPtr>
        ( iParent->getChild( iName ) );
}

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS
} // End namespace TakoAbstract
} // End namespace Alembic
