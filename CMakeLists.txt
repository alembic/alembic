##-*****************************************************************************
##
## Copyright (c) 2009-2010, Industrial Light & Magic,
##   a division of Lucasfilm Entertainment Company Ltd.
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

# This is the top level of the Alembic project. Bins and then libs!
# Also, this top level should include any "global" build configurations
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

# Declare an Alembic Project
PROJECT( ALEMBIC )

IF (RUN_FROM_MK)
  SET ( CMAKE_FIRST_RUN CACHE STRING "CMake initialised from mk script" )
  MARK_AS_ADVANCED ( CMAKE_FIRST_RUN )
ENDIF()

IF (NOT DEFINED QUIET)
  SET(CMAKE_VERBOSE_MAKEFILE TRUE)
ENDIF()

# Declare the version
SET( PROJECT_VERSION_MAJOR 0 )
SET( PROJECT_VERSION_MINOR 9 )
SET( PROJECT_VERSION_PATCH 2 )
SET( PROJECT_VERSION
     ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )
SET( VERSION ${PROJECT_VERSION} )

MESSAGE(STATUS "CMAKE SYSTEM NAME = ${CMAKE_SYSTEM_NAME}" )

# Windows flag
SET( WINDOWS FALSE )
IF( "${CMAKE_SYSTEM_NAME}" MATCHES "Windows" )
  SET( WINDOWS TRUE )
ENDIF()

SET( DARWIN FALSE )
IF( "${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" )
  SET( DARWIN TRUE )
ENDIF()

SET( LINUX FALSE )
IF( "${CMAKE_SYSTEM_NAME}" MATCHES "Linux" )
  SET( LINUX TRUE )
ENDIF()

# Set some debug vs opt flags
IF ( "${CMAKE_BUILD_TYPE}" MATCHES "Debug" )
  ADD_DEFINITIONS( -DDEBUG=1 -UNDEBUG )
ELSEIF ( "${CMAKE_BUILD_TYPE}" MATCHES "Release" )
  ADD_DEFINITIONS( -DNDEBUG=1 -UDEBUG )
  IF ( NOT WINDOWS )
    ADD_DEFINITIONS( -O3 )
  ENDIF()
ENDIF()

IF ( NOT WINDOWS )
  SET(EXTERNAL_MATH_LIBS "-lm")
ELSE()
  SET(EXTERNAL_MATH_LIBS "")
ENDIF()

IF ( NOT WINDOWS )
  SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wreturn-type -Werror" )
ENDIF()

IF( ${WINDOWS} )
  ADD_DEFINITIONS( -DPLATFORM_WINDOWS -DPLATFORM=WINDOWS )
ELSEIF( ${DARWIN} )
  #ADD_DEFINITIONS( -DPLATFORM_DARWIN -DPLATFORM=DARWIN "-arch x86_64" )
  ADD_DEFINITIONS( -DPLATFORM_DARWIN -DPLATFORM=DARWIN )
ELSE()
  ADD_DEFINITIONS( -DPLATFORM_LINUX -DPLATFORM=LINUX )
ENDIF()

if (MSVC AND DEFINED BUILD_STATIC_LIBS AND BUILD_STATIC_LIBS)
	MESSAGE(STATUS "Replacing /MD with /MT for compiles" )
	foreach (MODE "_DEBUG" "_MINSIZEREL" "_RELEASE" "_RELWITHDEBINFO")
		string(REPLACE "/MD" "/MT" TMP "${CMAKE_C_FLAGS${MODE}}")
		set(CMAKE_C_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
		string(REPLACE "/MD" "/MT" TMP "${CMAKE_CXX_FLAGS${MODE}}")
		set(CMAKE_CXX_FLAGS${MODE} "${TMP}" CACHE STRING "" FORCE)
	endforeach (MODE)
endif ()

#-******************************************************************************
#-******************************************************************************
# INSTALLATION STUFF
#-******************************************************************************
#-******************************************************************************

SET( ORIGINAL_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
SET( CMAKE_INSTALL_PREFIX
     ${ORIGINAL_INSTALL_PREFIX}/alembic-${VERSION} )

# Tell me what my install location would be
MESSAGE(STATUS "The install dir is ${CMAKE_INSTALL_PREFIX}" )

# check in the source directory
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/build )

#-******************************************************************************
#-******************************************************************************
# TESTING STUFF
#-******************************************************************************
#-******************************************************************************
# Globally enable testing.
#INCLUDE(CTest)
ENABLE_TESTING()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

IF(UNIX AND NOT WIN32)
  FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
  IF(CMAKE_UNAME)
    EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
    SET(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
"processor type (i386 and x86_64)")
    IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      ADD_DEFINITIONS(-fPIC)
    ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  ENDIF(CMAKE_UNAME)
ENDIF(UNIX AND NOT WIN32)

IF ( ${DARWIN} )
  ADD_DEFINITIONS( -fPIC )
ENDIF()

IF(BOOTSTRAP_MODE)

  ADD_SUBDIRECTORY(build/bootstrap)

ELSE()

# Set up the include directories
IF( WINDOWS )
  INCLUDE_DIRECTORIES( "C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/include" )
ENDIF()

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

#-******************************************************************************
#-******************************************************************************
# OPEN GL STUFF
#-******************************************************************************
#-******************************************************************************
FIND_PACKAGE( OpenGL REQUIRED )
FIND_PACKAGE( GLUT REQUIRED )

IF ( ${WINDOWS} )
  # Do a windows OpenGL thing, whatever that may be
  SET( ALEMBIC_GL_LIBS GLEW ${GLUT_LIBRARY} ${OPENGL_LIBRARIES} )
ELSEIF( ${DARWIN} )
  # Do a darwin OpenGL thing.
  SET( ALEMBIC_GL_LIBS ${GLUT_LIBRARY} "-framework OpenGL" )
ELSE()
  SET( ALEMBIC_GL_LIBS GLEW ${GLUT_LIBRARY} ${OPENGL_LIBRARIES} )
ENDIF()

MESSAGE( STATUS "OpenGL Libraries: ${ALEMBIC_GL_LIBS}" )

#-******************************************************************************
#-******************************************************************************
# BOOST STUFF
#-******************************************************************************
#-******************************************************************************

# Include boost
INCLUDE( "./build/AlembicBoost.cmake" )
IF ( NOT ALEMBIC_BOOST_FOUND )
  IF( Boost_FOUND AND (DEFINED Boost_VERSION) AND (NOT ${Boost_VERSION} LESS 104200 ) )
    SET( ALEMBIC_BOOST_FOUND 1 )
  ENDIF()
ENDIF()

IF ( NOT ALEMBIC_BOOST_FOUND )
  IF( Boost_FOUND )
    MESSAGE( ERROR "Boost version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION} found. Require 1.42.0 or greater.")
  ELSE()
    MESSAGE( ERROR "Boost not found." )
  ENDIF()
  MESSAGE( FALTAL_ERROR "Please see comments in ${ALEMBIC_SOURCE_DIR}/build/AlembicBoost.cmake for possible solutions.")
ENDIF()

INCLUDE_DIRECTORIES( ${ALEMBIC_BOOST_INCLUDE_PATH} )

#-******************************************************************************
#-******************************************************************************
# ILM BASE
#-******************************************************************************
#-******************************************************************************

INCLUDE( "./build/AlembicIlmBase.cmake" )
INCLUDE_DIRECTORIES( ${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY} )

#-******************************************************************************
#-******************************************************************************
# OPEN EXR
#-******************************************************************************
#-******************************************************************************

# Automatically include ILM Base Stuff.
#INCLUDE( "./build/AlembicOpenEXR.cmake" )
#IF ( ALEMBIC_OPENEXR_FOUND )
#  INCLUDE_DIRECTORIES( ${ALEMBIC_OPENEXR_INCLUDE_PATH} )
#ENDIF()



# Include Python stuff
INCLUDE("./build/AlembicPython.cmake")

# Include HDF5 stuff
INCLUDE("./build/AlembicHDF5.cmake")
if ( ALEMBIC_HDF5_FOUND )
  INCLUDE_DIRECTORIES( ${ALEMBIC_HDF5_INCLUDE_PATH} )
ENDIF()

# Recurse into libs, then bins.
ADD_SUBDIRECTORY( lib )
INCLUDE_DIRECTORIES( "${ALEMBIC_SOURCE_DIR}/lib" )

# Include Prman stuff
IF(DEFINED USE_PRMAN AND NOT USE_PRMAN)
    MESSAGE(STATUS "Skipping Prman plugins")
ELSE()
    MESSAGE( STATUS "About to include AlembicPRMan.cmake" )
    INCLUDE( "./build/AlembicPRMan.cmake" )
    IF( ALEMBIC_PRMAN_FOUND )
        MESSAGE(STATUS "Building PRMan plugins" )
        ADD_SUBDIRECTORY( prman )
    ELSE()
        MESSAGE(STATUS "For some reason, ALEMBIC_PRMAN_FOUND is False?")
    ENDIF()
ENDIF()

ADD_SUBDIRECTORY( python EXCLUDE_FROM_ALL )

# Include Maya stuff
IF(DEFINED USE_MAYA AND NOT USE_MAYA)
    MESSAGE(STATUS "Skipping Maya plugins")
ELSE()
    MESSAGE( STATUS "About to include AlembicMaya.cmake" )
    INCLUDE( "./build/AlembicMaya.cmake" )
    IF( MAYA_FOUND )
        MESSAGE(STATUS "Building Maya plugins" )
        ADD_SUBDIRECTORY( maya )
    ELSE()
        MESSAGE(STATUS "For some reason, MAYA_FOUND is False?")
    ENDIF()
ENDIF()

# example code not supported
ADD_SUBDIRECTORY( examples EXCLUDE_FROM_ALL )

ENDIF()



