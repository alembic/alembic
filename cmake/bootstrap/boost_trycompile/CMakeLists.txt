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

# BOOST try-compile CMakeLists.txt

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
# it will use the defaults

# For more info read:
# /usr/share/cmake-2.8.0/share/cmake-2.8/Modules/FindBoost.cmake

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(BOOST_TRYCOMPILE)

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

IF(DEFINED BOOST_INCLUDEDIR)
    MESSAGE(STATUS "Using BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}" )
ENDIF()

IF(DEFINED Boost_THREAD_LIBRARY)
    MESSAGE(STATUS "Using Boost_THREAD_LIBRARY: ${Boost_THREAD_LIBRARY}" )
ENDIF()


MESSAGE( STATUS "COMPILING BOOST TRY COMPILE EXECUTABLE IN ${PROJECT_BINARY_DIR}.")

# Added -lm to the front to work around a bug in CMake TRY_RUN that doesn't
# accept a string variable to LINK_LIBRARIES
IF ( NOT WINDOWS )
    SET(TRYCOMPILE_LIBS "-lm -lpthread ${Boost_THREAD_LIBRARY}")
ELSE()
    SET(TRYCOMPILE_LIBS "${Boost_THREAD_LIBRARY}")
ENDIF()

SET(USING_THESE_LIBS "-DLINK_LIBRARIES:INTERNAL=${TRYCOMPILE_LIBS}")

TRY_RUN(BOOST_RUN_RESULT BOOST_COMPILE_RESULT
  ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/boost_trycompile.cpp
  CMAKE_FLAGS ${USING_THESE_LIBS}
  COMPILE_DEFINITIONS "-I${BOOST_INCLUDEDIR}"
  COMPILE_OUTPUT_VARIABLE BOOST_COMPILE_OUTPUT
  RUN_OUTPUT_VARIABLE BOOST_RUN_OUTPUT
  )

IF(NOT BOOST_COMPILE_RESULT)
  MESSAGE( FATAL_ERROR "FAILED TO COMPILE A WORKING BOOST TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${BOOST_COMPILE_OUTPUT}")
ELSE()
  MESSAGE( STATUS "COMPILED A WORKING BOOST TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${BOOST_COMPILE_OUTPUT}")
ENDIF()

IF(BOOST_RUN_RESULT STREQUAL "FAILED_TO_RUN")
  MESSAGE( FATAL_ERROR "FAILED TO RUN A WORKING BOOST TRY COMPILE EXECUTABLE.\nERROR LOG:\n${BOOST_RUN_OUTPUT}")
ELSE()
  MESSAGE( STATUS "SUCCESSFULY RAN A WORKING BOOST TRY COMPILE EXECUTABLE.\nLOG:\n${BOOST_RUN_OUTPUT}")
ENDIF()

