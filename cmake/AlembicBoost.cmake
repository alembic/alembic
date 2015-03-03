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

# If you know the boost root and there aren't any default versions of boost in
# the default system paths use:
# cmake '-UBoost_*' -DBOOST_ROOT:STRING=<path/to/boost> .

# If you are still having trouble, use explit include paths:
# cmake '-UBoost_*' -DBOOST_INCLUDEDIR:STRING=<path/to/boost_include_dir> .

# If the libraries are in a separate location to the include files, use:
# cmake '-UBoost_*' -DBOOST_INCLUDEDIR:STRING=<path/to/boost_include_dir> -DBOOST_LIBRARYDIR:STRING=<path/to/boost_library_dir> .

# '-UBoost_*' removes any Boost_* entries from the cache so it can be run
# multiple times without breaking the rest of the cached entries

# -------------------------------------------------------------------------------------

# The three ways to search are:

# BOOST_INCLUDEDIR: if it is defined, then search for the boost header files in
# the specific location.

# BOOST_LIBRARYDIR: if it is defined, then search for the boost library files in
# the specific location.

# BOOST_ROOT: Set the boost root to the defined CMake variable BOOST_ROOT, otherwise
# it will use the defaults specified inline below

# For more info read:
# /usr/share/cmake-2.8.0/share/cmake-2.8/Modules/FindBoost.cmake (or equivalent path on your O/S)

#SET(Boost_DEBUG TRUE)

IF(DEFINED BOOST_INCLUDEDIR)
    MESSAGE(STATUS "Using BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}" )
ENDIF()

IF(DEFINED BOOST_LIBRARYDIR)
    MESSAGE(STATUS "Using BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}" )
ENDIF()

# If BOOST_ROOT not set, use predefined paths
IF(NOT DEFINED BOOST_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
	        SET( BOOST_ROOT "/usr/local/include/boost-1_43/boost" )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( BOOST_ROOT "/usr/include/boost-1_42/boost" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( BOOST_ROOT "C:/Program Files (x86)/boost-1_42/boost" )

          # For 64-bit builds use:
          # SET( BOOST_ROOT "C:/Program Files/boost-1_42/boost" )
        ELSE()
          SET( BOOST_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ENDIF()

IF(NOT DEFINED CMAKE_PREFIX_PATH)
    MESSAGE(STATUS "Using BOOST_ROOT: ${BOOST_ROOT}" )
ELSE()
    MESSAGE(STATUS "Using CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}" )
    UNSET(BOOST_ROOT)
ENDIF()

#-******************************************************************************
#-******************************************************************************
# Find the static and multi-threaded version only
#-******************************************************************************
#-******************************************************************************
SET( Boost_USE_STATIC_LIBS TRUE )
SET( Boost_USE_MULTITHREADED TRUE )
SET( Boost_NO_BOOST_CMAKE TRUE ) 

#- Alembic's python bridge boost-python 1.44 (for bug fixes)
SET( Boost_ADDITIONAL_VERSIONS "1.44" "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45.0" )
FIND_PACKAGE( Boost COMPONENTS program_options python REQUIRED thread )


#-******************************************************************************
#-******************************************************************************
# Wrap it all ups
#-******************************************************************************
#-******************************************************************************
IF ( DEFINED Boost_INCLUDE_DIRS )
  SET( BOOST_FOUND TRUE )
ENDIF()

IF ( BOOST_FOUND )
  SET( Boost_FOUND TRUE )
ENDIF()

IF ( DEFINED BOOST_VERSION )
  SET( Boost_VERSION ${BOOST_VERSION} )
ENDIF()

IF ( DEFINED Boost_VERSION )
  SET( BOOST_VERSION ${Boost_VERSION} )
ENDIF()

MESSAGE( STATUS "BOOST_VERSION: ${BOOST_VERSION}" )

IF (Boost_FOUND)
  IF( Boost_VERSION LESS 104200 )
    MESSAGE( STATUS "FOUND INCORRECT BOOST VERSION: ${Boost_LIB_VERSION}")
  ENDIF()

  MESSAGE( STATUS "BOOST FOUND: ${Boost_FOUND}" )
  MESSAGE( STATUS "BOOST INCLUDE DIRS: ${Boost_INCLUDE_DIRS}")
  MESSAGE( STATUS "BOOST LIBRARIES: ${Boost_LIBRARIES}")

  SET(ALEMBIC_BOOST_INCLUDE_PATH ${Boost_INCLUDE_DIRS})
  SET(ALEMBIC_BOOST_LIBRARIES ${Boost_LIBRARIES})


  SET( ALEMBIC_BOOST_FOUND 1 CACHE STRING "Set to 1 if boost is found, 0 otherwise" )

ELSE()
  SET( ALEMBIC_BOOST_FOUND 0 CACHE STRING "Set to 1 if boost is found, 0 otherwise" )
  MESSAGE(SEND_ERROR "Boost not correctly specified")

ENDIF()

MESSAGE( STATUS "ALEMBIC_BOOST_FOUND: ${ALEMBIC_BOOST_FOUND}" )
