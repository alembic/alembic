##-*****************************************************************************
##
## Copyright (c) 2009-2015,
##  Sony Pictures Imageworks, Inc. and
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
## Industrial Light & Magic nor the names of their contributors may be used
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

SET(CXX_FILES 
    AbcReader.cpp
    ParseReader.cpp
    Parser.cpp
    Reader.cpp)

SET(PUBLIC_H_FILES 
    AbcReader.h
    All.h
    Export.h
    Foundation.h
    Parser.h
    Reader.h)

SET(PRIVATE_H_FILES
    ParseReader.h)

SET(H_FILES ${PUBLIC_H_FILES} ${PRIVATE_H_FILES})

SET(SOURCE_FILES ${CXX_FILES} ${H_FILES})

ADD_LIBRARY(AbcWFObjConvert ${LIB_TYPE} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(AbcWFObjConvert Alembic)

IF(ALEMBIC_SHARED_LIBS)
  SET_TARGET_PROPERTIES(AbcWFObjConvert 
                        PROPERTIES DEFINE_SYMBOL
                        ABC_WFOBJ_CONVERT_EXPORTS)
ENDIF()

INSTALL(TARGETS AbcWFObjConvert
        LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
        RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR}
        ARCHIVE DESTINATION ${ARCHIVE_INSTALL_DIR})

INSTALL(FILES ${PUBLIC_H_FILES}
        DESTINATION include/AbcClients/WFObjConvert
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

IF (USE_TESTS AND USE_HDF5)
    ADD_SUBDIRECTORY(Tests)
ELSE()
    MESSAGE(STATUS 
        "Skipping WFObjConvert tests: requires USE_TESTS and USE_HDF5")
ENDIF()
