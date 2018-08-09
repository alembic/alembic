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
    AbcImport.h
    AlembicImportFileTranslator.h
    AlembicNode.h
    CameraHelper.h
    CreateSceneHelper.h
    LocatorHelper.h
    MeshHelper.h
    NodeIteratorVisitorHelper.h
    NurbsCurveHelper.h
    NurbsSurfaceHelper.h
    PointHelper.h
    util.h
    XformHelper.h
)

SET(CXX_FILES
    AbcImport.cpp
    AlembicImportFileTranslator.cpp
    AlembicNode.cpp
    CameraHelper.cpp
    CreateSceneHelper.cpp
    main.cpp
    LocatorHelper.cpp
    MeshHelper.cpp
    NurbsCurveHelper.cpp
    NurbsSurfaceHelper.cpp
    NodeIteratorVisitorHelper.cpp
    PointHelper.cpp
    util.cpp
    XformHelper.cpp
)

SET(SOURCE_FILES ${CXX_FILES} ${H_FILES})

#INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("..")

ADD_MAYA_CXX_PLUGIN(AbcImport ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(AbcImport
    ${MAYA_LIBRARIES}
    Alembic::Alembic
)

set_target_properties(AbcImport PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

INSTALL(TARGETS AbcImport
        DESTINATION ${ALEMBIC_MAYA_PLUGINS_INSTALL_DIR})
