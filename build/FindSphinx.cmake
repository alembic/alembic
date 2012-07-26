#-*****************************************************************************
##
## Copyright (c) 2012,
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

IF(DEFINED USE_PYALEMBIC)
    IF(NOT USE_PYALEMBIC)
        MESSAGE(STATUS "Skipping Sphinx docs")
    ELSE()
        FIND_FILE(ALEMBIC_PYILMBASE_PYIMATH_MODULE
          NAMES imathmodule.so
          PATHS ${ALEMBIC_PYILMBASE_PYIMATH_MODULE_DIRECTORY}
        )
        FIND_PROGRAM(SPHINX_EXECUTABLE 
          NAMES sphinx-build
          HINTS
          $ENV{SPHINX_DIR}
          PATH_SUFFIXES bin\r\n  DOC "Sphinx documentation generator"
        )
        INCLUDE(FindPackageHandleStandardArgs)
        FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sphinx DEFAULT_MSG\r\n  SPHINX_EXECUTABLE
        )
        MARK_AS_ADVANCED(
          SPHINX_EXECUTABLE
        )
        MESSAGE(STATUS "Found Sphinx: ${SPHINX_EXECUTABLE}")
        MESSAGE(STATUS "Found imath module: ${ALEMBIC_PYILMBASE_PYIMATH_MODULE}")
    ENDIF()
ELSE()
    MESSAGE(STATUS "USE_PYALEMBIC undefined")
ENDIF()
