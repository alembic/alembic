##-*****************************************************************************
##
## Copyright (c) 2009-2015,
##  Sony Pictures Imageworks Inc. and
##  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
##
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## *       Redistributions of source code must retain the above copyright
## notice, this list of conditions and the following disclaimer.
## *       Redistributions in binary form must reproduce the above
## copyright notice, this list of conditions and the following disclaimer
## in the documentation and/or other materials provided with the
## distribution.
## *       Neither the name of Industrial Light & Magic nor the names of
## its contributors may be used to endorse or promote products derived
## from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##-*****************************************************************************

LIST(APPEND CXX_FILES
    AbcGeom/ArchiveBounds.cpp
    AbcGeom/GeometryScope.cpp
    AbcGeom/FilmBackXformOp.cpp
    AbcGeom/CameraSample.cpp
    AbcGeom/ICamera.cpp
    AbcGeom/OCamera.cpp
    AbcGeom/Basis.cpp
    AbcGeom/ICurves.cpp
    AbcGeom/OCurves.cpp
    AbcGeom/OFaceSet.cpp
    AbcGeom/IFaceSet.cpp
    AbcGeom/OLight.cpp
    AbcGeom/ILight.cpp
    AbcGeom/ONuPatch.cpp
    AbcGeom/INuPatch.cpp
    AbcGeom/OPoints.cpp
    AbcGeom/IPoints.cpp
    AbcGeom/OPolyMesh.cpp
    AbcGeom/IPolyMesh.cpp
    AbcGeom/OSubD.cpp
    AbcGeom/ISubD.cpp
    AbcGeom/Visibility.cpp
    AbcGeom/XformOp.cpp
    AbcGeom/XformSample.cpp
    AbcGeom/IXform.cpp
    AbcGeom/OXform.cpp
)
SET(CXX_FILES "${CXX_FILES}" PARENT_SCOPE)

INSTALL(FILES
    All.h
    Foundation.h
    ArchiveBounds.h
    IGeomBase.h
    OGeomBase.h
    GeometryScope.h
    SchemaInfoDeclarations.h
    OLight.h
    ILight.h
    FilmBackXformOp.h
    CameraSample.h
    ICamera.h
    OCamera.h
    Basis.h
    CurveType.h
    ICurves.h
    OCurves.h
    FaceSetExclusivity.h
    OFaceSet.h
    IFaceSet.h
    ONuPatch.h
    INuPatch.h
    OGeomParam.h
    IGeomParam.h
    OPoints.h
    IPoints.h
    OPolyMesh.h
    IPolyMesh.h
    OSubD.h
    ISubD.h
    Visibility.h
    XformOp.h
    XformSample.h
    IXform.h
    OXform.h
    DESTINATION include/Alembic/AbcGeom
)

IF (USE_TESTS)
    ADD_SUBDIRECTORY(Tests)
ENDIF()
