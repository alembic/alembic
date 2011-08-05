##-*****************************************************************************
##
## Copyright (c) 2009-2011,
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


# If HDF5_ROOT not set, use predefined paths
IF(NOT DEFINED HDF5_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
          SET( HDF5_ROOT "/usr/local/hdf5-1.8.5-patch1" )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( HDF5_ROOT "/usr/local/hdf5-1.8.5-patch1" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( HDF5_ROOT "C:/Program Files (x86)/hdf5-1.8.5-patch1" )
        ELSE()
          SET( HDF5_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ENDIF()

#IF( NOT DEFINED HDF5_USE_STATIC_LIBRARIES )
#  SET( HDF5_USE_STATIC_LIBRARIES TRUE )
#ENDIF()



# Prefer HDF5_ROOT set from the environment over the CMakeCache'd variable
IF(NOT $ENV{HDF5_ROOT}x STREQUAL "x")
  SET( HDF5_ROOT $ENV{HDF5_ROOT})
ELSE()
  SET( ENV{HDF5_ROOT} ${HDF5_ROOT} )
ENDIF()

IF( DEFINED ENV{HDF5_INCLUDE_DIR} )
  SET( HDF5_INCLUDE_DIR $ENV{HDF5_INCLUDE_DIR} )
  SET( ALEMBIC_HDF5_INCLUDE_PATH ${HDF5_INCLUDE_DIR} )
  MESSAGE( STATUS "I AM TOTALLY SETTING HDF5 INCLUDE DIR FROM ENVIRONMENT: ${HDF5_INCLUDE_DIR}" )
ELSE()
  MESSAGE( STATUS "NOT SETTING HDF5_INCLUDE_DIR FROM ENVIRONMENT" )
ENDIF()

IF( CMAKE_MINOR_VERSION GREATER 7 AND CMAKE_PATCH_VERSION GREATER 4 OR CMAKE_MAJOR_VERSION GREATER 2 )
  FIND_PACKAGE( HDF5 COMPONENTS C HL REQUIRED )
ELSE()
  FIND_PACKAGE( HDF5 )
ENDIF()

IF( HDF5_FOUND )
  IF ( NOT DEFINED ${ALEMBIC_HDF5_INCLUDE_PATH} )
    SET( ALEMBIC_HDF5_INCLUDE_PATH ${HDF5_INCLUDE_DIR} )
  ENDIF()
  SET( ALEMBIC_HDF5_LIB ${HDF5_C_LIBRARIES} )
  SET( ALEMBIC_HDF5_HL_LIB ${HDF5_CXX_LIBRARIES} )
  SET( ALEMBIC_HDF5_LIBS ${HDF5_LIBRARIES} )
  SET( ALEMBIC_HDF5_FOUND TRUE )

  MESSAGE(STATUS "HDF5 INCLUDE PATH: ${ALEMBIC_HDF5_INCLUDE_PATH}" )
  MESSAGE(STATUS "HDF5 LIBRARIES: ${ALEMBIC_HDF5_LIBS}" )

  MESSAGE(STATUS "Found HDF5 Library!" )

ELSE()
  IF( DEFINED ALEMBIC_HDF5_CONFIGURED )
    MESSAGE(STATUS "" )
    MESSAGE(STATUS "CMake could not find HDF5; using values from the bootstrap script")
    MESSAGE(STATUS "")
    SET( ALEMBIC_HDF5_INCLUDE_PATH ${HDF5_C_INCLUDE_DIR} )
    SET( ALEMBIC_HDF5_LIB ${HDF5_hdf5_LIBRARY} )
    SET( ALEMBIC_HDF5_HL_LIB ${HDF5_hdf5_hl_LIBRARY} )
    SET( ALEMBIC_HDF5_LIBS ${ALEMBIC_HDF5_HL_LIB} ${ALEMBIC_HDF5_LIB} pthread z m )
    SET( HDF5_FOUND TRUE )
    SET( ALEMBIC_HDF5_FOUND TRUE )
    MESSAGE(STATUS "HDF5 INCLUDE PATH: ${ALEMBIC_HDF5_INCLUDE_PATH}" )
    MESSAGE(STATUS "HDF5 LIBRARIES: ${ALEMBIC_HDF5_LIBS}" )
    MESSAGE(STATUS "Found HDF5 Library!" )
  ELSE()
    SET( ALEMBIC_HDF5_FOUND FALSE )
    SET( ALEMBIC_HDF5_LIBS NOTFOUND )
    MESSAGE(STATUS "Disabling HDF5 Library!" )
  ENDIF()
ENDIF( )

