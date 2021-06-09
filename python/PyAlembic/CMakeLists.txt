##-*****************************************************************************
##
## Copyright (c) 2009-2016,
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

SET(Boost_USE_STATIC_LIBS ${USE_STATIC_BOOST})

# disables linking to -mt variants on osx
IF (APPLE)
    SET(Boost_USE_MULTITHREADED OFF)
ENDIF()

# require 1.53+ since we need std::shared_ptr supported in the wrapper
FIND_PACKAGE(Boost 1.53.0 COMPONENTS "python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}")

# find via . instead, i.e. python2.7
IF (NOT Boost_FOUND)
    FIND_PACKAGE(Boost 1.53.0 COMPONENTS "python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
ENDIF()

# hit a weird case where boost python released seperately from the other boost modules in 1.70 could
# not find it unless it was optional, so fall back on this
IF (NOT Boost_FOUND)
    FIND_PACKAGE(Boost 1.53.0 OPTIONAL_COMPONENTS "python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}")
ENDIF()

IF (Boost_FOUND)
    IF (Boost_VERSION_MACRO LESS 105300)
        MESSAGE(FATAL_ERROR "Boost VERSION IS TOO OLD.")
    ENDIF()
    MESSAGE(STATUS "BOOST INCLUDE DIRS: ${Boost_INCLUDE_DIRS}")
    MESSAGE(STATUS "BOOST LIBRARIES: ${Boost_LIBRARIES}")
ELSE()
    MESSAGE(FATAL_ERROR "Boost not found.")
ENDIF()

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY})

# lots of warnings coming from unused local typedefs inside boost python
IF (NOT WINDOWS)
    ADD_DEFINITIONS(-Wno-unused-local-typedefs)
ENDIF()

SET(CPP_FILES
    PyAbcCoreLayer.cpp
    PyAbcGeomTypes.cpp
    PyAbcTypes.cpp
    PyArchiveBounds.cpp
    PyArchiveInfo.cpp
    PyCameraSample.cpp
    PyCoreAbstractTypes.cpp
    PyFilmBackXformOp.cpp
    PyIArchive.cpp
    PyIArrayProperty.cpp
    PyICamera.cpp
    PyICollections.cpp
    PyICompoundProperty.cpp
    PyICurves.cpp
    PyIFaceSet.cpp
    PyIGeomBase.cpp
    PyIGeomParam.cpp
    PyIGeomParamPOD.cpp
    PyIGeomParamVertexPoint.cpp
    PyIGeomParamBox.cpp
    PyIGeomParamMatrixQuadColorNormal.cpp
    PyILight.cpp
    PyIMaterial.cpp
    PyINuPatch.cpp
    PyIObject.cpp
    PyIPoints.cpp
    PyIPolyMesh.cpp
    PyISampleSelector.cpp
    PyIScalarProperty.cpp
    PyISubD.cpp
    PyITypedArrayProperty.cpp
    PyITypedScalarProperty.cpp
    PyIXform.cpp
    PyMaterialFlatten.cpp
    PyMaterialAssignment.cpp
    PyOArchive.cpp
    PyOArrayProperty.cpp
    PyOCamera.cpp
    PyOCollections.cpp
    PyOCompoundProperty.cpp
    PyOCurves.cpp
    PyOFaceSet.cpp
    PyOGeomParam.cpp
    PyOGeomParamPOD.cpp
    PyOGeomParamVertexPoint.cpp
    PyOGeomParamBox.cpp
    PyOGeomParamMatrixQuadColorNormal.cpp
    PyOLight.cpp
    PyOMaterial.cpp
    PyONuPatch.cpp
    PyOObject.cpp
    PyOPoints.cpp
    PyOPolyMesh.cpp
    PyOScalarProperty.cpp
    PyOSubD.cpp
    PyOTypedArrayProperty.cpp
    PyOTypedScalarProperty.cpp
    PyOXform.cpp
    PyTypedArraySampleConverter.cpp
    PyTypedPropertyTraits.cpp
    PyUtilTypes.cpp
    PyVisibility.cpp
    PyXformOp.cpp
    PyXformSample.cpp
    main.cpp
)

IF (1900 VERSION_EQUAL MSVC_VERSION)
    IF ((19.00.24213.1 VERSION_LESS_EQUAL CMAKE_CXX_COMPILER_VERSION) AND
        (19.00.24234.1 VERSION_GREATER_EQUAL CMAKE_CXX_COMPILER_VERSION))
        list(APPEND CPP_FILES "msvc14fixes.cpp")
    ENDIF ()
ENDIF ()

INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_DIR}" "${CMAKE_SOURCE_DIR}/lib"
    "${CMAKE_SOURCE_DIR}/python/PyAlembic")

ADD_LIBRARY(alembic SHARED ${CPP_FILES})

if ("${PYTHON_LIBRARIES}" STREQUAL "")
    SET(PYTHON_LIBRARIES "${Python${PYTHON_VERSION_MAJOR}_LIBRARIES}")
endif()

TARGET_LINK_LIBRARIES(alembic
    LINK_PUBLIC
    Alembic::Alembic
    ${PYTHON_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ALEMBIC_PYILMBASE_PYIMATH_LIB}
)

SET( ALEMBIC_PYTHON_INSTALL_DIR
    lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
    CACHE PATH
    "Alembic's python bindings install directory"
)

INSTALL (TARGETS alembic
    DESTINATION ${ALEMBIC_PYTHON_INSTALL_DIR}
)

# Not sure about the best way to find the Imath python bindings for Mac
IF (USE_TESTS AND NOT APPLE)
    ADD_SUBDIRECTORY(Tests)
ENDIF()

# For the python libs Mac looks for .so, .dylib didnt work
SET (EXT ".so")
IF (WIN32)
    SET (EXT ".pyd")
ENDIF ()

# dont add any wacky lib prefix, as python doesnt like this
set_target_properties(alembic PROPERTIES PREFIX "" SUFFIX ${EXT})

set_target_properties(alembic PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
