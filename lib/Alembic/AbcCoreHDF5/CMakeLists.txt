##-*****************************************************************************
##
## Copyright (c) 2009-2015,
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

LIST(APPEND CXX_FILES
    AbcCoreHDF5/AprImpl.cpp
    AbcCoreHDF5/ApwImpl.cpp
    AbcCoreHDF5/ArImpl.cpp
    AbcCoreHDF5/AwImpl.cpp
    AbcCoreHDF5/CacheImpl.cpp
    AbcCoreHDF5/CprData.cpp
    AbcCoreHDF5/CprImpl.cpp
    AbcCoreHDF5/CpwData.cpp
    AbcCoreHDF5/CpwImpl.cpp
    AbcCoreHDF5/DataTypeRegistry.cpp
    AbcCoreHDF5/HDF5Hierarchy.cpp
    AbcCoreHDF5/HDF5HierarchyReader.cpp
    AbcCoreHDF5/HDF5HierarchyWriter.cpp
    AbcCoreHDF5/HDF5Util.cpp
    AbcCoreHDF5/OrData.cpp
    AbcCoreHDF5/OrImpl.cpp
    AbcCoreHDF5/OwData.cpp
    AbcCoreHDF5/OwImpl.cpp
    AbcCoreHDF5/ReadUtil.cpp
    AbcCoreHDF5/ReadWrite.cpp
    AbcCoreHDF5/SprImpl.cpp
    AbcCoreHDF5/SpwImpl.cpp
    AbcCoreHDF5/StringReadUtil.cpp
    AbcCoreHDF5/StringWriteUtil.cpp
    AbcCoreHDF5/WriteUtil.cpp
)
SET(CXX_FILES "${CXX_FILES}" PARENT_SCOPE)

INSTALL(FILES All.h ReadWrite.h
        DESTINATION include/Alembic/AbcCoreHDF5)

IF (USE_TESTS)
    ADD_SUBDIRECTORY(Tests)
ENDIF()
