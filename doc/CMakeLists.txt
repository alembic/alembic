##-*****************************************************************************
##
## Copyright (c) 2009-2012,
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

IF(DEFINED USE_PYALEMBIC AND NOT USE_PYALEMBIC)

    MESSAGE(STATUS "Skipping finding Sphinx")

ELSEIF(DEFINED USE_PYALEMBIC AND USE_PYALEMBIC)

    FIND_PACKAGE( Sphinx REQUIRED )

    # set default Sphinx theme
    IF( NOT DEFINED SPHINX_THEME )
        SET( SPHINX_THEME default )
    ENDIF()

    # Sphinx themes location
    IF( NOT DEFINED SPHINX_THEME_DIR )
        SET( SPHINX_THEME_DIR )
    ENDIF()

    # configured documentation tools and intermediate build results
    SET(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_build")

    # location of Sphinx source files
    SET(SPHINX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

    # Sphinx cache with pickled ReST documents
    SET(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_doctrees")

    # HTML output directory
    SET(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

    # copy the conf.py file and parse @ vars
    CONFIGURE_FILE(
        "${CMAKE_CURRENT_SOURCE_DIR}/source/conf.py.in"
        "${BINARY_BUILD_DIR}/conf.py"
        @ONLY)

    # attemp to get boost python in LD_LIBRARY_PATH
    # http://www.cmake.org/Bug/view.php?id=5145
    SET(ENV{LD_LIBRARY_PATH} ${Boost_LIBRARY_DIRS})
    MESSAGE(STATUS "LD_LIBRARY_PATH: " $ENV{LD_LIBRARY_PATH})

    # runs the Sphinx command
    ADD_CUSTOM_TARGET(docs
        ${SPHINX_EXECUTABLE}
        -b html
        -c "${BINARY_BUILD_DIR}"
        -d "${SPHINX_CACHE_DIR}"
        "${SPHINX_SOURCE_DIR}"
        "${SPHINX_HTML_DIR}"
        COMMENT "Building Alembic Python bindings documentation")

    ADD_DEPENDENCIES(docs alembicmodule)

ENDIF()
