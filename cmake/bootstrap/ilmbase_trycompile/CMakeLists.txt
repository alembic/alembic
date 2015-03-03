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

# ILMBASE try-compile CMakeLists.txt

# use explit include paths and library:
# cmake '-UILMBASE_*' -DILMBASE_INCLUDE_DIR:STRING=<path/to/ILMBASE_include_dir> 
#  -DALEMBIC_ILMBASE_HALF_LIB:FILEPATH=<path/to/ILMBASE_Half_library> .
#
# '-UILMBASE_*' removes any ILMBASE_* entries from the cache so it can be run
# multiple times without breaking the rest of the cached entries

# -------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ILMBASE_TRYCOMPILE)

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

IF(DEFINED ALEMBIC_ILMBASE_INCLUDE_DIRECTORY)
  IF(DEFINED ALEMBIC_ILMBASE_HALF_LIB )
      MESSAGE(STATUS "Using ALEMBIC_ILMBASE_HALF_LIB: ${ALEMBIC_ILMBASE_HALF_LIB}" )
      MESSAGE(STATUS "Using ALEMBIC_ILMBASE_INCLUDE_DIRECTORY: ${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY}" )

    # TODO: TAILOR FOR CROSS-COMPILING CASES, see: http://www.cmake.org/cmake/help/cmake-2-8-docs.html
    MESSAGE( STATUS "COMPILING ILMBASE TRY COMPILE EXECUTABLE IN ${PROJECT_BINARY_DIR}. WITH ${ALEMBIC_ILMBASE_HALF_LIB}")

    # Added -lm to the front to work around a bug in CMake TRY_RUN that doesn't
    # accept a string variable to LINK_LIBRARIES

    # TODO: fix name of variable ILMBASE_LIBRARY_DIR
    IF ( NOT WINDOWS )
        SET(TRYCOMPILE_LIBS "-lm ${ALEMBIC_ILMBASE_HALF_LIB}")
    ELSE()
        SET(TRYCOMPILE_LIBS "${ALEMBIC_ILMBASE_HALF_LIB}")
    ENDIF()
    SET(USING_THESE_LIBS "-DLINK_LIBRARIES:INTERNAL=${TRYCOMPILE_LIBS}")
    TRY_RUN(ILMBASE_RUN_RESULT ILMBASE_COMPILE_RESULT
            ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/ilmbase_trycompile.cpp 
            CMAKE_FLAGS ${USING_THESE_LIBS}
            COMPILE_DEFINITIONS "-I${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY}"
            COMPILE_OUTPUT_VARIABLE ILMBASE_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE ILMBASE_RUN_OUTPUT
            )

    IF(NOT ILMBASE_COMPILE_RESULT)
      MESSAGE( FATAL_ERROR "FAILED TO COMPILE A WORKING ILMBASE TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${ILMBASE_COMPILE_OUTPUT}")
    ELSE()
      MESSAGE( STATUS "COMPILED A WORKING ILMBASE TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${ILMBASE_COMPILE_OUTPUT}")
    ENDIF()

    IF(ILMBASE_RUN_RESULT STREQUAL "FAILED_TO_RUN")
      MESSAGE( FATAL_ERROR "FAILED TO RUN A WORKING ILMBASE TRY COMPILE EXECUTABLE.\nERROR LOG:\n${ILMBASE_RUN_OUTPUT}")
    ELSE()
      MESSAGE( STATUS "SUCCESSFULY RAN A WORKING ILMBASE TRY COMPILE EXECUTABLE.\nLOG:\n${ILMBASE_RUN_OUTPUT}")
    ENDIF()

  ELSE()
    MESSAGE(FATAL_ERROR "ILMBASE not correctly specified")
  ENDIF()
ELSE()
  MESSAGE(FATAL_ERROR "ILMBASE not correctly specified")
ENDIF()
