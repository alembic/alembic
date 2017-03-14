##-*****************************************************************************
##
## Copyright (c) 2009-2016,
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

PROJECT(Alembic)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

MESSAGE(STATUS "CMAKE SYSTEM NAME: ${CMAKE_SYSTEM_NAME}")

IF (RUN_FROM_MK)
    SET(CMAKE_FIRST_RUN CACHE STRING "CMake initialised from mk script")
    MARK_AS_ADVANCED (CMAKE_FIRST_RUN)
ENDIF()

IF (NOT DEFINED QUIET)
    SET(CMAKE_VERBOSE_MAKEFILE TRUE)
ENDIF()

# Declare the project version
SET(PROJECT_VERSION_MAJOR "1")
SET(PROJECT_VERSION_MINOR "7")
SET(PROJECT_VERSION_PATCH "1")
SET(PROJECT_VERSION
     ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
SET(VERSION ${PROJECT_VERSION})

#-******************************************************************************
# OPTIONS (set with -D<option>=<value>)
#-******************************************************************************

OPTION(USE_ARNOLD "Include Arnold stuff" OFF)
OPTION(USE_BINARIES "Include binaries" ON)
OPTION(USE_EXAMPLES "Include examples" OFF)
OPTION(USE_HDF5 "Include HDF5 stuff" OFF)
OPTION(USE_MAYA "Include Maya stuff" OFF)
OPTION(USE_PRMAN "Include PRMan stuff" OFF)
OPTION(USE_PYALEMBIC "Include PyAlembic stuff" OFF)
OPTION(USE_STATIC_BOOST "Build with static Boost libs" OFF)
OPTION(USE_STATIC_HDF5 "Build with static HDF5 libs" OFF)
OPTION(USE_TESTS "Include Alembic tests" ON)
OPTION(ALEMBIC_ILMBASE_LINK_STATIC "IlmBase is a static library" OFF)
OPTION(ALEMBIC_SHARED_LIBS "Build shared libraries" ON)
OPTION(ALEMBIC_LIB_USES_BOOST
    "Main Alembic library uses boost for things like shared_ptr" OFF)
OPTION(ALEMBIC_LIB_USES_TR1
    "Main Alembic library uses std::tr1 for things like shared_ptr" OFF)

# Set static/dynamic build options
SET(LIB_TYPE STATIC)
SET(RUNTIME_INSTALL_DIR lib)
SET(LIBRARY_INSTALL_DIR lib)
SET(ARCHIVE_INSTALL_DIR lib)
IF (ALEMBIC_SHARED_LIBS)
    SET(LIB_TYPE SHARED)
    SET(ARCHIVE_INSTALL_DIR lib)
    IF (WIN32)
        ADD_DEFINITIONS(-DALEMBIC_DLL)
    ENDIF()
ENDIF(ALEMBIC_SHARED_LIBS)

# Need to test this on multiple platforms, it is an easy way to enable
# memory checks like valgrind by doing:
# ctest -D ExperimentalMemCheck
# include (CTest)

# Cmake system specific flags
SET(WINDOWS FALSE)
IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    SET(WINDOWS TRUE)
ENDIF()

SET(DARWIN FALSE)
IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
    SET(DARWIN TRUE)
    # suppress rpath warning
    IF (POLICY CMP0042)
        CMAKE_POLICY(SET CMP0042 OLD)
    ENDIF()
ENDIF()

SET(LINUX FALSE)
IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    SET(LINUX TRUE)
ENDIF()

IF (NOT ${ALEMBIC_LIB_USES_TR1} AND NOT ${ALEMBIC_LIB_USES_BOOST})
    # TODO figure out how to do this for Windows
    INCLUDE(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    IF (COMPILER_SUPPORTS_CXX11)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    ELSEIF (COMPILER_SUPPORTS_CXX0X)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    ENDIF()
ENDIF()

# Set visibility for GNU compilers
IF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
ENDIF()

# Set some debug vs opt flags
IF ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    ADD_DEFINITIONS(-DDEBUG=1 -UNDEBUG)
    IF (NOT WINDOWS)
        ADD_DEFINITIONS(-Wall -Werror -Wextra -Wno-unused-parameter)
    ENDIF()
ELSEIF ("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    ADD_DEFINITIONS(-DNDEBUG=1 -UDEBUG)
    IF (NOT WINDOWS)
        ADD_DEFINITIONS(-O3)
    ENDIF()
ENDIF()

IF (NOT ${WINDOWS})
    SET(EXTERNAL_MATH_LIBS "-lm")
ELSE()
    SET(EXTERNAL_MATH_LIBS "")
ENDIF()

IF (${WINDOWS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    ADD_DEFINITIONS(-DPLATFORM_WINDOWS -DPLATFORM=WINDOWS)
    IF (NOT ALEMBIC_ILMBASE_LINK_STATIC)
        ADD_DEFINITIONS(-DOPENEXR_DLL)
    ENDIF()
ELSEIF (${DARWIN})
    #ADD_DEFINITIONS(-DPLATFORM_DARWIN -DPLATFORM=DARWIN "-arch x86_64")
    ADD_DEFINITIONS(-DPLATFORM_DARWIN -DPLATFORM=DARWIN)
ELSE()
    ADD_DEFINITIONS(-DPLATFORM_LINUX -DPLATFORM=LINUX)
ENDIF()

#-******************************************************************************
# INSTALLATION
#-******************************************************************************

IF (DEFINED ENV{ALEMBIC_INSTALL_PREFIX})
    SET( CMAKE_INSTALL_PREFIX
        $ENV{ALEMBIC_INSTALL_PREFIX}/alembic-${VERSION})
ENDIF()

# Tell me what my install location would be
MESSAGE(STATUS "The install dir is ${CMAKE_INSTALL_PREFIX}")

# check in the source directory
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

#-******************************************************************************
# TESTS
#-******************************************************************************

# Globally enable testing
ENABLE_TESTING()
IF (BUILD_TESTING)
    SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
    MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

IF (UNIX AND NOT WINDOWS)
    FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
    IF (CMAKE_UNAME)
        EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
        SET(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
"processor type (i386 and x86_64)")
        IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
            ADD_DEFINITIONS(-fPIC)
        ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    ENDIF(CMAKE_UNAME)
ENDIF(UNIX AND NOT WINDOWS)

IF (DARWIN)
    ADD_DEFINITIONS(-fPIC)
ENDIF()

#-******************************************************************************
# DEPENDENCIES
#-******************************************************************************

FIND_PACKAGE(Threads REQUIRED)

# We only need boost if including PyAlembic, or using it in the lib
IF (ALEMBIC_LIB_USES_BOOST OR USE_PYALEMBIC)
    INCLUDE("./cmake/AlembicBoost.cmake")
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF()

# IlmBase
INCLUDE("./cmake/AlembicIlmBase.cmake")
INCLUDE_DIRECTORIES(${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY})

# HDF5
IF (USE_HDF5)
    FIND_PACKAGE(ZLIB REQUIRED)
    SET(ALEMBIC_WITH_HDF5 "1")
    INCLUDE("./cmake/AlembicHDF5.cmake")
    INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DH5_USE_18_API")
ENDIF()

#-******************************************************************************
# BUILD LIBRARIES
#-******************************************************************************

IF (USE_HDF5)
    SET(CORE_LIBS
        Alembic
        ${HDF5_LIBRARIES}
        ${ALEMBIC_ILMBASE_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        ${ZLIB_LIBRARIES}
        ${EXTERNAL_MATH_LIBS}
    )
ELSE()
    SET(CORE_LIBS
        Alembic
        ${ALEMBIC_ILMBASE_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        ${EXTERNAL_MATH_LIBS}
    )
ENDIF()

# Alembic
ADD_SUBDIRECTORY(lib)
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/lib" "${PROJECT_BINARY_DIR}/lib")

# PyAlembic
IF (USE_PYALEMBIC)
    INCLUDE("./cmake/AlembicPython.cmake")
    INCLUDE("./cmake/AlembicPyIlmBase.cmake")
    IF (PYTHONLIBS_FOUND)
        MESSAGE(STATUS "ALEMBIC_PYTHON_INCLUDE_DIRS: ${ALEMBIC_PYTHON_INCLUDE_DIRS}")
        MESSAGE(STATUS "ALEMBIC_PYTHON_LIBRARY: ${ALEMBIC_PYTHON_LIBRARY}")
        INCLUDE_DIRECTORIES(${ALEMBIC_PYTHON_INCLUDE_DIRS} ${ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY})
        ADD_SUBDIRECTORY(python)
    ELSE()
        MESSAGE(WARNING "Python not found! Skipping PyAlembic")
    ENDIF()
ENDIF()

#-******************************************************************************
# BUILD BINARIES
#-******************************************************************************

# PRMan
IF (USE_PRMAN)
    INCLUDE("./cmake/AlembicPRMan.cmake")
    IF (${ALEMBIC_PRMAN_FOUND})
        MESSAGE(STATUS "Building PRMan plugins")
        ADD_SUBDIRECTORY(prman)
    ELSE()
        MESSAGE(STATUS "PRMan not found")
    ENDIF()
ENDIF()

# Arnold
IF (USE_ARNOLD)
    INCLUDE("./cmake/AlembicArnold.cmake")
    IF (${ALEMBIC_ARNOLD_FOUND})
        MESSAGE(STATUS "Building Arnold plugins")
        ADD_SUBDIRECTORY( arnold )
    ELSE()
        MESSAGE(STATUS "Arnold not found")
    ENDIF()
ENDIF()

# Maya
IF (USE_MAYA)
    INCLUDE("./cmake/AlembicMaya.cmake")
    IF (MAYA_FOUND)
        MESSAGE(STATUS "Building Maya plugins")
        ADD_SUBDIRECTORY(maya)
    ELSE()
        MESSAGE(STATUS "Maya not found")
    ENDIF()
ENDIF()

# Binaries (abcls, abctree, etc)
IF (USE_BINARIES)
    ADD_SUBDIRECTORY(bin)
ENDIF()

# Examples
IF (USE_EXAMPLES)
    ADD_SUBDIRECTORY(examples)
ENDIF()

#-******************************************************************************

SET(_config_msg "\n   * Alembic Configuration          ===")
MACRO(info_cfg_option
    _setting)
    SET(_msg "   * ${_setting}")
    STRING(LENGTH "${_msg}" _len)
    WHILE("36" GREATER "${_len}")
        SET(_msg "${_msg} ")
        MATH(EXPR _len "${_len} + 1")
    ENDWHILE()
    SET(_config_msg "${_config_msg}\n${_msg}${${_setting}}")
ENDMACRO()

info_cfg_option(USE_ARNOLD)
info_cfg_option(USE_BINARIES)
info_cfg_option(USE_EXAMPLES)
info_cfg_option(USE_HDF5)
info_cfg_option(USE_MAYA)
info_cfg_option(USE_PRMAN)
info_cfg_option(USE_PYALEMBIC)
info_cfg_option(USE_STATIC_BOOST)
info_cfg_option(USE_STATIC_HDF5)
info_cfg_option(USE_TESTS)
info_cfg_option(ALEMBIC_ILMBASE_LINK_STATIC)
info_cfg_option(ALEMBIC_SHARED_LIBS)
info_cfg_option(ALEMBIC_LIB_USES_BOOST)
info_cfg_option(ALEMBIC_LIB_USES_TR1)
MESSAGE("${_config_msg}")
