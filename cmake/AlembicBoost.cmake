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

# If you know the boost root and there aren't any default versions of boost in
# the default system paths use:
# cmake '-UBoost_*' -DBOOST_ROOT:STRING=<path/to/boost> .
#
# If you are still having trouble, use explit include paths:
# cmake '-UBoost_*' -DBOOST_INCLUDEDIR:STRING=<path/to/boost_include_dir> .
#
# If the libraries are in a separate location to the include files, use:
# cmake '-UBoost_*' -DBOOST_INCLUDEDIR:STRING=<path/to/boost_include_dir> -DBOOST_LIBRARYDIR:STRING=<path/to/boost_library_dir> .
#
# '-UBoost_*' removes any Boost_* entries from the cache so it can be run
# multiple times without breaking the rest of the cached entries
#
# For more info:
# cmake --help-module FindBoost

#SET(Boost_DEBUG TRUE)

#-******************************************************************************
# FindBoost uses these as hints about search locations
#-******************************************************************************

IF (DEFINED BOOST_ROOT)
    MESSAGE(STATUS "Using BOOST_ROOT: ${BOOST_ROOT}")
ELSE()
    IF (DEFINED BOOST_INCLUDEDIR)
        MESSAGE(STATUS "Using BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}")
    ENDIF()
    IF (DEFINED BOOST_LIBRARYDIR)
        MESSAGE(STATUS "Using BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}")
    ENDIF()
ENDIF()

#-******************************************************************************
# Find static and multi-threaded versions only (1.42.0+)
#-******************************************************************************

SET(Boost_USE_STATIC_LIBS ${USE_STATIC_BOOST})
SET(Boost_NO_BOOST_CMAKE ON) 

# disables linking to -mt variants on osx
IF (USE_PYALEMBIC AND APPLE)
    SET(Boost_USE_MULTITHREADED OFF)
ENDIF()

IF (USE_PYALEMBIC)
    FIND_PACKAGE(Boost 1.42.0 COMPONENTS program_options python)
ELSE()
    FIND_PACKAGE(Boost 1.42.0 COMPONENTS program_options)
ENDIF()

#-******************************************************************************
# Wrap it all up
#-******************************************************************************

IF (Boost_FOUND)
    IF (Boost_VERSION LESS 104200)
        MESSAGE(FATAL_ERROR "Boost VERSION IS TOO OLD.")
    ENDIF()
    MESSAGE(STATUS "BOOST INCLUDE DIRS: ${Boost_INCLUDE_DIRS}")
    MESSAGE(STATUS "BOOST LIBRARIES: ${Boost_LIBRARIES}")
ELSE()
    MESSAGE(FATAL_ERROR "Boost not found.")
ENDIF()
