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

# If MAYA_ROOT not set, use predefined paths
IF(NOT DEFINED MAYA_ROOT)
  IF ( ${CMAKE_HOST_UNIX} )
    IF( ${DARWIN} )
      # TODO: set to default install path when shipping out
      # SET( ALEMBIC_MAYA_ROOT NOTFOUND )
      SET( ALEMBIC_MAYA_ROOT "/Applications/Autodesk/maya2012" )
      SET( ALEMBIC_MAYA_INC_ROOT "/Applications/Autodesk/maya2012/include" )
      SET( ALEMBIC_MAYA_LIB_ROOT "/Applications/Autodesk/maya2012/Maya.app/Contents/MacOS" )
    ELSE()
      SET( ALEMBIC_MAYA_ROOT "/usr/autodesk/maya2012-x64" )
    ENDIF()
  ELSE()
    IF ( ${WINDOWS} )
      SET( ALEMBIC_MAYA_ROOT "C:/Program Files/Autodesk/Maya2012" )
    ELSE()
      SET( ALEMBIC_MAYA_ROOT NOTFOUND )
    ENDIF()
  ENDIF()
ELSE()
  # Prefer MAYA_ROOT set from the CMakeCache'd variable than default paths
  MESSAGE( STATUS "Using MAYA_ROOT ${MAYA_ROOT}" )
  SET( ALEMBIC_MAYA_ROOT ${MAYA_ROOT})
ENDIF()

# Prefer MAYA_ROOT set from the environment over the CMakeCache'd variable
IF(NOT $ENV{MAYA_ROOT}x STREQUAL "x")
  SET( ALEMBIC_MAYA_ROOT $ENV{MAYA_ROOT})
ENDIF()

IF( NOT DEFINED ALEMBIC_MAYA_INC_ROOT )
  SET( ALEMBIC_MAYA_INC_ROOT "${ALEMBIC_MAYA_ROOT}/include" )
ENDIF()

IF( NOT DEFINED ALEMBIC_MAYA_LIB_ROOT )
  IF ( ${DARWIN} )
    SET( ALEMBIC_MAYA_LIB_ROOT "${ALEMBIC_MAYA_ROOT}/Maya.app/Contents/MacOS" )
  ELSE()
    SET( ALEMBIC_MAYA_LIB_ROOT "${ALEMBIC_MAYA_ROOT}/lib" )
  ENDIF()
ENDIF()

MESSAGE( STATUS "Maya lib root: ${ALEMBIC_MAYA_LIB_ROOT}" )

# Just start with forcing it to ILM's location
SET( MAYA_INCLUDE_PATH MAYA_INCLUDE_PATH-NOTFOUND )
FIND_PATH( MAYA_INCLUDE_PATH maya/MTypes.h
  PATHS
  "${ALEMBIC_MAYA_INC_ROOT}"
  "${ALEMBIC_MAYA_ROOT}/include"
  "${ALEMBIC_MAYA_ROOT}/devkit/include"
  DOC "The directory where MTypes.h resides" )

SET( MAYA_FOUNDATION_LIBRARY MAYA_FOUNDATION_LIBRARY-NOTFOUND )
FIND_LIBRARY( MAYA_FOUNDATION_LIBRARY Foundation
  PATHS
  "${ALEMBIC_MAYA_LIB_ROOT}"
  "${ALEMBIC_MAYA_ROOT}/lib"
  DOC "The directory where Foundation.lib resides"
  )

SET( MAYA_OPENMAYA_LIBRARY MAYA_OPENMAYA_LIBRARY-NOTFOUND )
FIND_LIBRARY( MAYA_OPENMAYA_LIBRARY OpenMaya
  PATHS
  "${ALEMBIC_MAYA_LIB_ROOT}"
  "${ALEMBIC_MAYA_ROOT}/lib"
  DOC "The directory where OpenMaya.lib resides" )

SET( MAYA_OPENMAYAANIM_LIBRARY MAYA_OPENMAYAANIM_LIBRARY-NOTFOUND )
FIND_LIBRARY( MAYA_OPENMAYAANIM_LIBRARY OpenMayaAnim
  PATHS
  "${ALEMBIC_MAYA_LIB_ROOT}"
  "${ALEMBIC_MAYA_ROOT}/lib"
  DOC "The directory where OpenMayaAnim.lib resides" )

SET( MAYA_OPENMAYAFX_LIBRARY MAYA_OPENMAYAFX_LIBRARY-NOTFOUND )
FIND_LIBRARY( MAYA_OPENMAYAFX_LIBRARY OpenMayaFX
  PATHS
  "${ALEMBIC_MAYA_LIB_ROOT}"
  "${ALEMBIC_MAYA_ROOT}/lib"
  DOC "The directory where OpenMayaFX.lib resides" )

SET( MAYA_OPENMAYARENDER_LIBRARY MAYA_OPENMAYARENDER_LIBRARY-NOTFOUND )
FIND_LIBRARY( MAYA_OPENMAYARENDER_LIBRARY OpenMayaRender
  PATHS
  "${ALEMBIC_MAYA_LIB_ROOT}"
  "${ALEMBIC_MAYA_ROOT}/lib"
  DOC "The directory where OpenMayaRender.lib resides" )

SET( MAYA_OPENMAYAUI_LIBRARY MAYA_OPENMAYAUI_LIBRARY-NOTFOUND )
FIND_LIBRARY( MAYA_OPENMAYAUI_LIBRARY OpenMayaUI
  PATHS
  "${ALEMBIC_MAYA_LIB_ROOT}"
  "${ALEMBIC_MAYA_ROOT}/lib"
  DOC "The directory where OpenMayaUI.lib resides" )

SET( MAYA_LIBRARIES
  ${MAYA_FOUNDATION_LIBRARY}
  ${MAYA_OPENMAYA_LIBRARY}
  ${MAYA_OPENMAYAANIM_LIBRARY}
  ${MAYA_OPENMAYAFX_LIBRARY}
  ${MAYA_OPENMAYARENDER_LIBRARY}
  ${MAYA_OPENMAYAUI_LIBRARY} )

IF ( NOT WINDOWS )
  IF ( NOT DARWIN )
    SET ( MAYA_EXTENSION ".so" )
    SET( MAYA_COMPILE_FLAGS
      "-m64 -g -pthread -pipe -D_BOOL -DLINUX -DLINUX_64 -DREQUIRE_IOSTREAM -fPIC -Wno-deprecated -fno-gnu-keywords" )

    SET( MAYA_LINK_FLAGS
      "-shared -m64 -g -pthread -pipe -D_BOOL -DLINUX -DLINUX_64 -DREQUIRE_IOSTREAM -fPIC -Wno-deprecated -fno-gnu-keywords -Wl,-Bsymbolic" )
  ELSE()
    #SET( MAYA_COMPILE_FLAGS
    #  "-DAW_NEW_IOSTREAMS -DCC_GNU_ -DOSMac_ -DOSMacOSX_ -DBits32_ \
    #  -DOSMac_MachO_ -DREQUIRE_IOSTREAM -fno-gnu-keywords -fpascal-strings \
    #-arch i386 -D_LANGUAGE_C_PLUS_PLUS" ) \
    SET ( MAYA_EXTENSION ".bundle" )
    SET( MAYA_COMPILE_FLAGS
      "-DAW_NEW_IOSTREAMS -DCC_GNU_ -DOSMac_ -DOSMacOSX_ -DOSMac_MachO_ -DREQUIRE_IOSTREAM -fno-gnu-keywords -D_LANGUAGE_C_PLUS_PLUS" )

    SET( MAYA_LINK_FLAGS
      #"-dynamic -g -fPIC "
      #"-shared -g -fPIC "
      "-fno-gnu-keywords -framework System  -framework SystemConfiguration -framework CoreServices -framework Carbon -framework Cocoa -framework ApplicationServices -framework Quicktime -framework IOKit -bundle -fPIC -L${ALEMBIC_MAYA_LIB_ROOT} -Wl,-executable_path,${ALEMBIC_MAYA_LIB_ROOT}" )
  ENDIF()
ELSE()
  SET( MAYA_EXTENSION ".mll" )
  SET( MAYA_COMPILE_FLAGS "/MD /D \"NT_PLUGIN\" /D \"REQUIRE_IOSTREAM\" /D \"_BOOL\"" )
  SET( MAYA_LINK_FLAGS " /export:initializePlugin /export:uninitializePlugin " )
ENDIF()

#-******************************************************************************
#-******************************************************************************
# Wrap it all ups
#-******************************************************************************
#-******************************************************************************
IF( MAYA_INCLUDE_PATH )
  #  SET( MAYA_FOUND 1 CACHE STRING "Set to 1 if Maya is found, 0 otherwise" )
  SET( MAYA_FOUND 1 )
  MESSAGE( STATUS "Found Maya!" )
ELSE( MAYA_INCLUDE_PATH )
  SET( MAYA_FOUND 0 CACHE STRING "Set to 1 if Maya is found, 0 otherwise" )
  MESSAGE( STATUS "Could not find Maya." )
ENDIF( MAYA_INCLUDE_PATH )

#MARK_AS_ADVANCED( MAYA_FOUND )

#-******************************************************************************
#-******************************************************************************
# Macros for making maya plugins
#-******************************************************************************
#-******************************************************************************
MACRO(ADD_MAYA_CXX_PLUGIN PluginName SourceFile1 )

  IF( NOT ${MAYA_FOUND} )
    MESSAGE( FATAL_ERROR "Could not find Maya. " )
  ENDIF()

  # Get various filename permutations
  GET_FILENAME_COMPONENT( PluginNameNoDirectory ${PluginName} NAME )
  GET_FILENAME_COMPONENT( PluginNameFullPath ${PluginName} ABSOLUTE )

  # Set a variable for sources
  SET( TMP_SOURCES ${SourceFile1} ${ARGN} )
  SET( ${PluginName}_SOURCES ${TMP_SOURCES} )

  # Include the maya dirs
  INCLUDE_DIRECTORIES( ${MAYA_INCLUDE_PATH} )

  # Add the target
  ADD_LIBRARY( ${PluginName} MODULE ${TMP_SOURCES} )

  # Compile and linker flags
  SET_TARGET_PROPERTIES( ${PluginName}
    PROPERTIES
    COMPILE_FLAGS ${MAYA_COMPILE_FLAGS}
    LINK_FLAGS ${MAYA_LINK_FLAGS}
    PREFIX ""
    SUFFIX ${MAYA_EXTENSION} )

  # Link the target
  TARGET_LINK_LIBRARIES( ${PluginName} ${MAYA_LIBRARIES} )

ENDMACRO(ADD_MAYA_CXX_PLUGIN)

