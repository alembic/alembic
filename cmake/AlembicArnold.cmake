##-*****************************************************************************
##
## Copyright (c) 2009-2013,
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


# We shall worry about windowsification later.

#-******************************************************************************
#-******************************************************************************
# FIRST, ARNOLD STUFF
#-******************************************************************************
#-******************************************************************************

# If ARNOLD_ROOT not set, use predefined paths
IF(NOT DEFINED ARNOLD_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
          SET( ALEMBIC_ARNOLD_ROOT NOTFOUND )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( ALEMBIC_ARNOLD_ROOT "/sww/tools/arnold" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( ALEMBIC_ARNOLD_ROOT NOTFOUND )
        ELSE()
          SET( ALEMBIC_ARNOLD_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ELSE()
    # Prefer ARNOLD_ROOT set from the CMakeCache'd variable than default paths
    SET( ALEMBIC_ARNOLD_ROOT ${ARNOLD_ROOT})
ENDIF()

# Prefer ARNOLD_ROOT set from the environment over the CMakeCache'd variable
IF(NOT $ENV{ARNOLD_ROOT}x STREQUAL "x")
  SET( ALEMBIC_ARNOLD_ROOT $ENV{ARNOLD_ROOT})
ENDIF()


FIND_PATH( ALEMBIC_ARNOLD_INCLUDE_PATH ai.h
           PATHS
           "${ALEMBIC_ARNOLD_ROOT}/include"
           DOC "The directory where ai.h resides" )

SET( ALEMBIC_ARNOLD_LIBARNOLD ALEMBIC_ARNOLD_LIBARNOLD-NOTFOUND )
FIND_LIBRARY( ALEMBIC_ARNOLD_LIBARNOLD ai
              PATHS
              "${ALEMBIC_ARNOLD_ROOT}/lib/"
              "${ALEMBIC_ARNOLD_ROOT}/bin/"
              DOC "The ai library" )


IF( ${WINDOWS} )
  SET( ARNOLD_COMPILE_FLAGS "/c /nologo /MT /TP /DWIN32" )
  SET( ARNOLD_LINK_FLAGS "/nologo /dll /LIBPATH:\"%RMANTREE%\lib\" libai.lib" )
ELSEIF( ${DARWIN} )
  SET( ARNOLD_COMPILE_FLAGS "-c" )
  SET( ARNOLD_LINK_FLAGS "-bundle -undefined dynamic_lookup" )
ELSEIF( ${LINUX} )
  SET( ARNOLD_COMPILE_FLAGS "-c -fPIC" )
  SET( ARNOLD_LINK_FLAGS "-shared" )
ENDIF()

IF ( ( ${ALEMBIC_ARNOLD_INCLUDE_PATH} STREQUAL "ALEMBIC_ARNOLD_INCLUDE_PATH-NOTFOUND" ) OR
     ( ${ALEMBIC_ARNOLD_LIBARNOLD} STREQUAL "ALEMBIC_ARNOLD_LIBARNOLD-NOTFOUND" ) )
  MESSAGE( STATUS "Arnold not found" )
  SET( ALEMBIC_ARNOLD_FOUND FALSE )
ELSE()
  MESSAGE( STATUS "ARNOLD INCLUDE PATH: ${ALEMBIC_ARNOLD_INCLUDE_PATH}" )
  MESSAGE( STATUS "libai: ${ALEMBIC_ARNOLD_LIBARNOLD}" )
  SET( ALEMBIC_ARNOLD_FOUND TRUE )
  SET( ALEMBIC_ARNOLD_LIBS ${ALEMBIC_ARNOLD_LIBARNOLD} )
ENDIF()

##-*****************************************************************************
##-*****************************************************************************
# Macro for making arnold plugins
##-*****************************************************************************
##-*****************************************************************************
MACRO(ADD_ARNOLD_CXX_PLUGIN PluginName SourceFile1 )

  IF( NOT ${ALEMBIC_ARNOLD_FOUND} )
    MESSAGE( FATAL_ERROR "Arnold is not found. :(" )
  ENDIF()

  GET_FILENAME_COMPONENT( PluginNameNoDirectory ${PluginName} NAME )
  GET_FILENAME_COMPONENT( PluginNameFullPath ${PluginName} ABSOLUTE )

  SET( TMP_SOURCES ${SourceFile1} ${ARGN} )
  SET( ${PluginName}_SOURCES ${TMP_SOURCES} )

  INCLUDE_DIRECTORIES( ${ALEMBIC_ARNOLD_INCLUDE_PATH} )

  ADD_LIBRARY( ${PluginName} MODULE ${TMP_SOURCES} )

  SET_TARGET_PROPERTIES( ${PluginName}
                         PROPERTIES
                         COMPILE_FLAGS ${ARNOLD_COMPILE_FLAGS}
                         LINK_FLAGS ${ARNOLD_LINK_FLAGS}
                         PREFIX "" )

  TARGET_LINK_LIBRARIES ( ${PluginName} ${ALEMBIC_ARNOLD_LIBARNOLD} )

ENDMACRO(ADD_ARNOLD_CXX_PLUGIN)
