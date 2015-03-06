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

# ZLIB try-compile CMakeLists.txt

# If you know the zlib root and there aren't any default versions of zlib in
# the default system paths use:
# cmake '-UZLIB_*' -DZLIB_ROOT:STRING=<path/to/zlib> .

# If you are still having trouble, use explit include paths:
# cmake '-UZLIB_*' -DZLIB_INCLUDEDIR:STRING=<path/to/zlib_include_dir> .

# If the libraries are in a separate location to the include files, use:
# cmake '-UZLIB_*' -DZLIB_INCLUDEDIR:STRING=<path/to/zlib_include_dir> -DZLIB_LIBRARYDIR:STRING=<path/to/zlib_library_dir> .

# '-UZLIB_*' removes any ZLIB_* entries from the cache so it can be run
# multiple times without breaking the rest of the cached entries

# -------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ZLIB_TRYCOMPILE)

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

SET( ZLIB_FOUND TRUE )

IF(DEFINED ZLIB_INCLUDE_DIR)
    MESSAGE(STATUS "Using ZLIB_INCLUDE_DIR: ${ZLIB_INCLUDE_DIR}" )
    SET( ZLIB_FOUND FALSE )
ENDIF()
 
IF(DEFINED ZLIB_LIBRARY)
    MESSAGE(STATUS "Using ZLIB_LIBRARY: ${ZLIB_LIBRARY}" )
ELSE()
    SET( ZLIB_FOUND FALSE )
ENDIF()

IF( NOT ZLIB_FOUND )
    FIND_PACKAGE(ZLIB REQUIRED)  
ENDIF()    

IF (ZLIB_FOUND)
  MESSAGE( STATUS "ZLIB FOUND: ${ZLIB_FOUND}" )
  MESSAGE( STATUS "ZLIB INCLUDE DIRS: ${ZLIB_INCLUDE_DIRS}")
  MESSAGE( STATUS "ZLIB LIBRARIES: ${ZLIB_LIBRARIES}")

  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})

  # TODO: TAILOR FOR CROSS-COMPILING CASES, see: http://www.cmake.org/cmake/help/cmake-2-8-docs.html
  MESSAGE( STATUS "COMPILING ZLIB TRY COMPILE EXECUTABLE IN ${PROJECT_BINARY_DIR}. WITH ${ZLIB_LIBRARIES}")

  SET(USING_THESE_LIBS "-DLINK_LIBRARIES:STRING=${ZLIB_LIBRARIES}")
  TRY_RUN(ZLIB_RUN_RESULT ZLIB_COMPILE_RESULT
          ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/zlib_trycompile.cpp 
          COMPILE_DEFINITIONS "-I${ZLIB_INCLUDE_DIRS}"
          CMAKE_FLAGS ${USING_THESE_LIBS}
          COMPILE_OUTPUT_VARIABLE ZLIB_COMPILE_OUTPUT
          RUN_OUTPUT_VARIABLE ZLIB_RUN_OUTPUT
          )

  IF(NOT ZLIB_COMPILE_RESULT)
    MESSAGE( FATAL_ERROR "FAILED TO COMPILE A WORKING ZLIB TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${ZLIB_COMPILE_OUTPUT}")
  ELSE()
    MESSAGE( STATUS "COMPILED A WORKING ZLIB TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${ZLIB_COMPILE_OUTPUT}")
  ENDIF()

  IF(ZLIB_RUN_RESULT STREQUAL "FAILED_TO_RUN")
    MESSAGE( FATAL_ERROR "FAILED TO RUN A WORKING ZLIB TRY COMPILE EXECUTABLE.\nERROR LOG:\n${ZLIB_RUN_OUTPUT}")
  ELSE()
    MESSAGE( STATUS "SUCCESSFULY RAN A WORKING ZLIB TRY COMPILE EXECUTABLE.\nLOG:\n${ZLIB_RUN_OUTPUT}")
  ENDIF()
   
ELSE()
  MESSAGE(SEND_ERROR "ZLIB not correctly specified")
ENDIF()

