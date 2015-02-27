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

# Declare the Alembic Project
PROJECT(ALEMBIC)

# This is the top level of the Alembic project. Bins and then libs!
# Also, this top level should include any "global" build configurations
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

MESSAGE(STATUS "CMAKE SYSTEM NAME: ${CMAKE_SYSTEM_NAME}" )

IF (RUN_FROM_MK)
    SET ( CMAKE_FIRST_RUN CACHE STRING "CMake initialised from mk script" )
    MARK_AS_ADVANCED ( CMAKE_FIRST_RUN )
ENDIF()

IF (NOT DEFINED QUIET)
    SET(CMAKE_VERBOSE_MAKEFILE TRUE)
ENDIF()

# Declare the version
SET(PROJECT_VERSION_MAJOR "1")
SET(PROJECT_VERSION_MINOR "5")
SET(PROJECT_VERSION_PATCH "8")
SET(PROJECT_VERSION
     ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )
SET(VERSION ${PROJECT_VERSION})

# Define some build options (set with -D<variable-name>=<value>)
OPTION(USE_HDF5 "Include HDF5 stuff" ON)
OPTION(USE_PRMAN "Include Prman stuff" OFF)
OPTION(USE_ARNOLD "Include Arnold stuff" OFF)
OPTION(USE_MAYA "Include Maya stuff" OFF)
OPTION(USE_PYALEMBIC "Include PyAlembic stuff" OFF)
OPTION(USE_EXAMPLES "Include examples" OFF)
OPTION(USE_OPENGL "Include AbcOpenGL lib" OFF)
OPTION(USE_DOCS "Include docs" OFF)
OPTION(USE_STATIC_BOOST "Build with static Boost libs" ON)
OPTION(USE_STATIC_HDF5 "Build with static HDF5 libs" OFF)
OPTION(ALEMBIC_NO_TESTS "Skip Alembic tests" OFF)
OPTION(ALEMBIC_NO_BOOTSTRAP "" OFF) # can we lose this one?

# Cmake system specific flags
SET(WINDOWS FALSE)
IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    SET(WINDOWS TRUE)
ENDIF()

SET(DARWIN FALSE)
IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
    SET(DARWIN TRUE)
    IF (NOT ${CMAKE_SYSTEM_VERSION} VERSION_LESS "13.0.0")
        ADD_DEFINITIONS( "-std=c++11" )
    ENDIF()
ENDIF()

SET(LINUX FALSE)
IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    SET(LINUX TRUE)
ENDIF()

# Set some debug vs opt flags
IF ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    ADD_DEFINITIONS(-DDEBUG=1 -UNDEBUG)
    IF (NOT WINDOWS)
        ADD_DEFINITIONS(-Wall -Werror -Wextra -Wno-unused-parameter)
    ENDIF()
ELSEIF ("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    ADD_DEFINITIONS(-DNDEBUG=1 -UDEBUG)
    IF (NOT WINDOWS)
        ADD_DEFINITIONS(-O3)
    ENDIF()
ENDIF()

IF (NOT WINDOWS)
    SET(EXTERNAL_MATH_LIBS "-lm")
ELSE()
    SET(EXTERNAL_MATH_LIBS "")
ENDIF()

IF (WINDOWS)
    ADD_DEFINITIONS (-DOPENEXR_DLL -DHALF_EXPORTS)
    ADD_DEFINITIONS(-DPLATFORM_WINDOWS -DPLATFORM=WINDOWS)
ELSEIF (DARWIN)
    #ADD_DEFINITIONS(-DPLATFORM_DARWIN -DPLATFORM=DARWIN "-arch x86_64")
    ADD_DEFINITIONS(-DPLATFORM_DARWIN -DPLATFORM=DARWIN)
ELSE()
    ADD_DEFINITIONS(-DPLATFORM_LINUX -DPLATFORM=LINUX)
ENDIF()

IF (MSVC AND DEFINED BUILD_STATIC_LIBS AND BUILD_STATIC_LIBS)
	MESSAGE(STATUS "Replacing /MD with /MT for compiles" )
	FOREACH (MODE "_DEBUG" "_MINSIZEREL" "_RELEASE" "_RELWITHDEBINFO")
		STRING(REPLACE "/MD" "/MT" TMP "${CMAKE_C_FLAGS${MODE}}")
		SET(CMAKE_C_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
		STRING(REPLACE "/MD" "/MT" TMP "${CMAKE_CXX_FLAGS${MODE}}")
		SET(CMAKE_CXX_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
	ENDFOREACH(MODE)
ENDIF()

#-******************************************************************************
# INSTALLATION STUFF
#-******************************************************************************

IF (DEFINED ENV{ALEMBIC_INSTALL_PREFIX})
    SET( CMAKE_INSTALL_PREFIX
        $ENV{ALEMBIC_INSTALL_PREFIX}/alembic-${VERSION})
ELSE()
    SET(ORIGINAL_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    SET(CMAKE_INSTALL_PREFIX
        ${ORIGINAL_INSTALL_PREFIX}/alembic-${VERSION})
ENDIF()

# Tell me what my install location would be
MESSAGE(STATUS "The install dir is ${CMAKE_INSTALL_PREFIX}")

# check in the source directory
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/build)

#-******************************************************************************
# TESTS
#-******************************************************************************

# Globally enable testing.
#INCLUDE(CTest)
ENABLE_TESTING()
IF (BUILD_TESTING)
    SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
    MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

IF(UNIX AND NOT WINDOWS)
    FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
    IF(CMAKE_UNAME)
        EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
        SET(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
"processor type (i386 and x86_64)")
        IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
            ADD_DEFINITIONS(-fPIC)
        ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    ENDIF(CMAKE_UNAME)
ENDIF(UNIX AND NOT WINDOWS)

IF (DARWIN)
  ADD_DEFINITIONS(-fPIC)
ENDIF()

# Set up the include directories
# IF( WINDOWS )
#  INCLUDE_DIRECTORIES( "C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/include" )
# ENDIF()

#-******************************************************************************
# DEPENDENCIES
#-******************************************************************************

# TODO: 
# - add build/bootstrap try_compile stuff for each option
# - add HDF5 switch in lib/Alembic cmakelist.txt file
# - replace all ALEMBIC_HDF5_INCLUDE_PATH with HDF5_INCLUDE_DIRS
# - replace all ALEMBIC_HDF5_LIBS with HDF5_LIBRARIES

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

# Boost
INCLUDE("./build/AlembicBoost.cmake")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# IlmBase
INCLUDE("./build/AlembicIlmBase.cmake")
INCLUDE_DIRECTORIES(${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY})

# OpenGL
IF (USE_OPENGL)
    FIND_PACKAGE(OpenGL)
    FIND_PACKAGE(GLUT)
    IF (WINDOWS)
        # Do a windows OpenGL thing, whatever that may be
        SET( ALEMBIC_GL_LIBS glew32s ${GLUT_LIBRARY} ${OPENGL_LIBRARIES} )
        INCLUDE_DIRECTORIES ( ${GLUT_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES ( ${Glew_INCLUDE_DIR} )
        LINK_DIRECTORIES ( ${Glew_LIBRARY_DIR} )
    ELSEIF(DARWIN)
        # Do a darwin OpenGL thing.
        SET(ALEMBIC_GL_LIBS ${GLUT_LIBRARY} "-framework OpenGL")
    ELSE()
        SET(ALEMBIC_GL_LIBS GLEW ${GLUT_LIBRARY} ${OPENGL_LIBRARIES})
    ENDIF()
    MESSAGE(STATUS "OpenGL Libraries: ${ALEMBIC_GL_LIBS}")
ELSE()
    MESSAGE(STATUS "Skipping OpenGL stuff")
ENDIF()

# HDF5
IF (USE_HDF5)
    INCLUDE("./build/AlembicHDF5.cmake")
    INCLUDE_DIRECTORIES(${ALEMBIC_HDF5_INCLUDE_PATH})
ELSE()
    MESSAGE(STATUS "Skipping HDF5 stuff")
ENDIF()

#-******************************************************************************
# BUILD LIBRARIES
#-******************************************************************************

# Alembic, AbcOpenGL
ADD_SUBDIRECTORY(lib)
INCLUDE_DIRECTORIES("${ALEMBIC_SOURCE_DIR}/lib")

# PyAlembic, PyAbcOpenGL
IF (USE_PYALEMBIC)
    INCLUDE("./build/AlembicPyIlmBase.cmake")
    INCLUDE("./build/AlembicPython.cmake")
    INCLUDE_DIRECTORIES(${ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY})
    ADD_SUBDIRECTORY(python)
ELSE()
    MESSAGE(STATUS "Skipping PyAlembic")
ENDIF()

#-******************************************************************************
# BUILD BINARIES
#-******************************************************************************

# Prman
IF (USE_PRMAN)
    INCLUDE("./build/AlembicPRMan.cmake")
    IF (ALEMBIC_PRMAN_FOUND)
        MESSAGE(STATUS "Building PRMan plugins")
        ADD_SUBDIRECTORY(prman)
    ELSE()
        MESSAGE(STATUS "For some reason, ALEMBIC_PRMAN_FOUND is False?")
    ENDIF()
ELSE()
    MESSAGE(STATUS "Skipping Prman plugins")
ENDIF()

# Arnold
IF (USE_ARNOLD)
    INCLUDE("./build/AlembicArnold.cmake")
    IF (ALEMBIC_ARNOLD_FOUND)
        MESSAGE(STATUS "Building Arnold plugins")
        ADD_SUBDIRECTORY( arnold )
    ELSE()
        MESSAGE(STATUS "For some reason, ALEMBIC_ARNOLD_FOUND is False?")
    ENDIF()
ELSE()
    MESSAGE(STATUS "Skipping Arnold plugins")
ENDIF()

# Maya
IF (USE_MAYA)
    INCLUDE("./build/AlembicMaya.cmake")
    IF (MAYA_FOUND)
        MESSAGE(STATUS "Building Maya plugins")
        ADD_SUBDIRECTORY(maya)
    ELSE()
        MESSAGE(STATUS "For some reason, MAYA_FOUND is False?")
    ENDIF()
ELSE()
    MESSAGE(STATUS "Skipping Maya plugins")
ENDIF()

# Examples (unsupported)
IF (USE_EXAMPLES)
    ADD_SUBDIRECTORY(examples)
ELSE()
    MESSAGE(STATUS "Skipping Alembic examples")
ENDIF()

# Python docs (run `make docs` from build root)
IF (USE_PYALEMBIC AND USE_DOCS)
    ADD_SUBDIRECTORY(doc)
ELSE()
    MESSAGE(STATUS "Skipping PyAlembic docs")
ENDIF()
