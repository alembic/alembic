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

# C++ files for this project
SET( CXX_FILES
  AprImpl.cpp
  ApwImpl.cpp
  ArImpl.cpp
  AwImpl.cpp
  CacheImpl.cpp
  CprData.cpp
  CprImpl.cpp
  CpwData.cpp
  CpwImpl.cpp
  DataTypeRegistry.cpp
  HDF5Hierarchy.cpp
  HDF5HierarchyReader.cpp
  HDF5HierarchyWriter.cpp
  HDF5Util.cpp
  OrData.cpp
  OrImpl.cpp
  OwData.cpp
  OwImpl.cpp
  ReadUtil.cpp
  ReadWrite.cpp
  SprImpl.cpp
  SpwImpl.cpp
  StringReadUtil.cpp
  StringWriteUtil.cpp
  WriteUtil.cpp
)

SET( H_FILES
  All.h
  AprImpl.h
  ApwImpl.h
  ArImpl.h
  AwImpl.h
  CacheImpl.h
  CprData.h
  CprImpl.h
  CpwData.h
  CpwImpl.h
  DataTypeRegistry.h
  HDF5Hierarchy.h
  HDF5HierarchyReader.h
  HDF5HierarchyWriter.h
  HDF5Util.h
  Foundation.h
  OrData.h
  OrImpl.h
  OwData.h
  OwImpl.h
  ReadUtil.h
  ReadWrite.h
  SimplePrImpl.h
  SimplePwImpl.h
  SprImpl.h
  SpwImpl.h
  StringReadUtil.h
  StringWriteUtil.h
  WriteUtil.h
  WrittenArraySampleMap.h
)

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} )

ADD_LIBRARY( AlembicAbcCoreHDF5 ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES( AlembicAbcCoreHDF5 ${ALEMBIC_HDF5_LIBS} )

INSTALL( TARGETS AlembicAbcCoreHDF5
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/static )

# Only install AbcCoreHDF5.h and ReadArraySampleCache
INSTALL( FILES
         All.h
         ReadWrite.h
         DESTINATION include/Alembic/AbcCoreHDF5
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )

IF( NOT ALEMBIC_NO_TESTS )
	ADD_SUBDIRECTORY( Tests )
ENDIF()

