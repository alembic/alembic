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


# We shall worry about windowsification later.

#-******************************************************************************
#-******************************************************************************
# FIRST, PRMAN STUFF
#-******************************************************************************
#-******************************************************************************

# If PRMAN_ROOT not set, use predefined paths
IF(NOT DEFINED PRMAN_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
          SET( ALEMBIC_PRMAN_ROOT NOTFOUND )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( ALEMBIC_PRMAN_ROOT "/sww/tools/pixar/rman15" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( ALEMBIC_PRMAN_ROOT NOTFOUND )
        ELSE()
          SET( ALEMBIC_PRMAN_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ELSE()
    # Prefer PRMAN_ROOT set from the CMakeCache'd variable than default paths
    SET( ALEMBIC_PRMAN_ROOT ${PRMAN_ROOT})
ENDIF()

# Prefer PRMAN_ROOT set from the environment over the CMakeCache'd variable
IF(NOT $ENV{PRMAN_ROOT}x STREQUAL "x")
  SET( ALEMBIC_PRMAN_ROOT $ENV{PRMAN_ROOT})
ENDIF()


FIND_PATH( ALEMBIC_PRMAN_INCLUDE_PATH ri.h
           PATHS
           "${ALEMBIC_PRMAN_ROOT}/include"
           DOC "The directory where ri.h resides" )

SET( ALEMBIC_PRMAN_LIBPRMAN ALEMBIC_PRMAN_LIBPRMAN-NOTFOUND )
FIND_LIBRARY( ALEMBIC_PRMAN_LIBPRMAN prman
              PATHS
              "${ALEMBIC_PRMAN_ROOT}/lib/"
              DOC "The prman library" )


IF( ${WINDOWS} )
  SET( PRMAN_COMPILE_FLAGS "/c /nologo /MT /TP /DWIN32" )
  SET( PRMAN_LINK_FLAGS "/nologo /dll /LIBPATH:\"%RMANTREE%\\lib\" libprman.lib" )
ELSEIF( ${DARWIN} )
  SET( PRMAN_COMPILE_FLAGS "-c" )
  SET( PRMAN_LINK_FLAGS "-bundle -undefined dynamic_lookup" )
ELSEIF( ${LINUX} )
  SET( PRMAN_COMPILE_FLAGS "-c -fPIC" )
  SET( PRMAN_LINK_FLAGS "-shared" )
ENDIF()

IF ( ( ${ALEMBIC_PRMAN_INCLUDE_PATH} STREQUAL "ALEMBIC_PRMAN_INCLUDE_PATH-NOTFOUND" ) OR
     ( ${ALEMBIC_PRMAN_LIBPRMAN} STREQUAL "ALEMBIC_PRMAN_LIBPRMAN-NOTFOUND" ) )
  MESSAGE( STATUS "PRMan not found" )
  SET( ALEMBIC_PRMAN_FOUND FALSE )
ELSE()
  MESSAGE( STATUS "PRMAN INCLUDE PATH: ${ALEMBIC_PRMAN_INCLUDE_PATH}" )
  MESSAGE( STATUS "libprman: ${ALEMBIC_PRMAN_LIBPRMAN}" )
  SET( ALEMBIC_PRMAN_FOUND TRUE )
  SET( ALEMBIC_PRMAN_LIBS ${ALEMBIC_PRMAN_LIBPRMAN} )
ENDIF()

##-*****************************************************************************
##-*****************************************************************************
# Macro for making prman plugins
##-*****************************************************************************
##-*****************************************************************************
MACRO(ADD_PRMAN_CXX_PLUGIN PluginName SourceFile1 )

  IF( NOT ${ALEMBIC_PRMAN_FOUND} )
    MESSAGE( FATAL_ERROR "PRMan is not found. :(" )
  ENDIF()

  GET_FILENAME_COMPONENT( PluginNameNoDirectory ${PluginName} NAME )
  GET_FILENAME_COMPONENT( PluginNameFullPath ${PluginName} ABSOLUTE )

  SET( TMP_SOURCES ${SourceFile1} ${ARGN} )
  SET( ${PluginName}_SOURCES ${TMP_SOURCES} )

  INCLUDE_DIRECTORIES( ${ALEMBIC_PRMAN_INCLUDE_PATH} )

  ADD_LIBRARY( ${PluginName} MODULE ${TMP_SOURCES} )

  SET_TARGET_PROPERTIES( ${PluginName}
                         PROPERTIES
                         COMPILE_FLAGS ${PRMAN_COMPILE_FLAGS}
                         LINK_FLAGS ${PRMAN_LINK_FLAGS}
                         PREFIX "" )

  TARGET_LINK_LIBRARIES ( ${PluginName} ${ALEMBIC_PRMAN_LIBPRMAN} )
#  TARGET_LINK_LIBRARIES( ${PluginName}
#                         AlembicTraitsGeom AlembicTraits 
#                         AlembicAsset MD5Hash AlembicHDF5
#                         ${ALEMBIC_HDF5_LIBS} AlembicUtil AlembicExc
#                         ${Boost_REGEX_LIBRARY} ${Boost_FILESYSTEM_LIBRARY}
#                        ${Boost_SYSTEM_LIBRARY}
#                         -lpthread -lz -lm )

ENDMACRO(ADD_PRMAN_CXX_PLUGIN)
