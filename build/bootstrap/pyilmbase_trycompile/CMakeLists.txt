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

# PYILMBASE try-compile CMakeLists.txt

# use explit include paths and library:
# cmake '-UPYILMBASE_*' -DPYILMBASE_INCLUDE_DIR:STRING=<path/to/PYILMBASE_include_dir> 
#  -DALEMBIC_PYILMBASE_PYIMATH_LIB:FILEPATH=<path/to/PYILMBASE_Half_library> .
#
# '-UPYILMBASE_*' removes any PYILMBASE_* entries from the cache so it can be run
# multiple times without breaking the rest of the cached entries

# -------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(PYILMBASE_TRYCOMPILE)

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

IF(DEFINED ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY)
  IF(DEFINED ALEMBIC_PYILMBASE_PYIMATH_LIB )
      MESSAGE(STATUS "Using ALEMBIC_PYILMBASE_PYIMATH_LIB: ${ALEMBIC_PYILMBASE_PYIMATH_LIB}" )
      MESSAGE(STATUS "Using ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY: ${ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY}" )
      MESSAGE(STATUS "Using BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}" )
      MESSAGE(STATUS "Using Boost_THREAD_LIBRARY: ${Boost_THREAD_LIBRARY}" )
      MESSAGE(STATUS "Using BOOST_PYTHON_LIBRARY: ${BOOST_PYTHON_LIBRARY}" )

    # TODO: TAILOR FOR CROSS-COMPILING CASES, see: http://www.cmake.org/cmake/help/cmake-2-8-docs.html
    MESSAGE( STATUS "COMPILING PYILMBASE TRY COMPILE EXECUTABLE IN ${PROJECT_BINARY_DIR}. WITH ${ALEMBIC_PYILMBASE_PYIMATH_LIB}")

    # Added -lm to the front to work around a bug in CMake TRY_RUN that doesn't
    # accept a string variable to LINK_LIBRARIES

    # TODO: fix name of variable PYILMBASE_LIBRARY_DIR
    IF ( NOT WINDOWS )
        SET(TRYCOMPILE_LIBS "-lm ${ALEMBIC_PYILMBASE_PYIMATH_LIB} ${BOOST_PYTHON_LIBRARY}")
    ELSE()
        SET(TRYCOMPILE_LIBS "${ALEMBIC_PYILMBASE_PYIMATH_LIB} ${BOOST_PYTHON_LIBRARY}")
    ENDIF()
    SET(USING_THESE_LIBS "-DLINK_LIBRARIES:INTERNAL=${TRYCOMPILE_LIBS} ${BOOST_PYTHON_LIBRARY}")
    TRY_RUN(PYILMBASE_RUN_RESULT PYILMBASE_COMPILE_RESULT
            ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/pyilmbase_trycompile.cpp 
            CMAKE_FLAGS ${USING_THESE_LIBS}
            COMPILE_DEFINITIONS "-I${ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY} -I${BOOST_INCLUDEDIR}"
            COMPILE_OUTPUT_VARIABLE PYILMBASE_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE PYILMBASE_RUN_OUTPUT
            )

    IF(NOT PYILMBASE_COMPILE_RESULT)
      MESSAGE( FATAL_ERROR "FAILED TO COMPILE A WORKING PYILMBASE TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${PYILMBASE_COMPILE_OUTPUT}")
    ELSE()
      MESSAGE( STATUS "COMPILED A WORKING PYILMBASE TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${PYILMBASE_COMPILE_OUTPUT}")
    ENDIF()

    IF(PYILMBASE_RUN_RESULT STREQUAL "FAILED_TO_RUN")
      MESSAGE( FATAL_ERROR "FAILED TO RUN A WORKING PYILMBASE TRY COMPILE EXECUTABLE.\nERROR LOG:\n${PYILMBASE_RUN_OUTPUT}")
    ELSE()
      MESSAGE( STATUS "SUCCESSFULY RAN A WORKING PYILMBASE TRY COMPILE EXECUTABLE.\nLOG:\n${PYILMBASE_RUN_OUTPUT}")
    ENDIF()

  ELSE()
    MESSAGE(FATAL_ERROR "PYILMBASE not correctly specified")
  ENDIF()
ELSE()
  MESSAGE(FATAL_ERROR "PYILMBASE not correctly specified")
ENDIF()
