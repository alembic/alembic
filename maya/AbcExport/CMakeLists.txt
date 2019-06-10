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
## *       Neither the name of Sony Pictures Imageworks, nor
## Industrial Light & Magic, nor the names of their contributors may be used
## to endorse or promote products derived from this software without specific
## prior written permission.
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

SET(H_FILES
    AbcExport.h
    AbcWriteJob.h
    AttributesWriter.h
    Foundation.h
    MayaCameraWriter.h
    MayaFaceSetWriter.h
    MayaMeshWriter.h
    MayaLocatorWriter.h
    MayaNurbsCurveWriter.h
    MayaNurbsSurfaceWriter.h
    MayaPointPrimitiveWriter.h
    MayaTransformWriter.h
    MayaUtility.h
)

SET(CXX_FILES
    AbcExport.cpp
    AbcWriteJob.cpp
    AttributesWriter.cpp
    MayaCameraWriter.cpp
    MayaFaceSetWriter.cpp
    MayaMeshWriter.cpp
    MayaLocatorWriter.cpp
    MayaNurbsCurveWriter.cpp
    MayaNurbsSurfaceWriter.cpp
    MayaPointPrimitiveWriter.cpp
    MayaTransformWriter.cpp
    MayaUtility.cpp
)

SET(SOURCE_FILES ${CXX_FILES} ${H_FILES})

#INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("..")

ADD_MAYA_CXX_PLUGIN(AbcExport ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(AbcExport
    ${MAYA_LIBRARIES}
    Alembic::Alembic
)

set_target_properties(AbcExport PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

INSTALL(TARGETS AbcExport
        DESTINATION ${ALEMBIC_MAYA_PLUGINS_INSTALL_DIR})
