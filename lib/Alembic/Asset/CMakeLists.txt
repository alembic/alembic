##-*****************************************************************************
##
## Copyright (c) 2009-2010,
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

# Base part of project
SET( ALEMBIC_ASSET_BASE_CXX_FILES
  Base/ISlabCache.cpp
  Base/ITimeSampling.cpp
  Base/Slab.cpp )

SET( ALEMBIC_ASSET_BASE_H_FILES
  Base/Assert.h
  Base/BaseAll.h
  Base/DataType.h
  Base/Exception.h
  Base/BaseAll.h
  Base/ISlabCache.h
  Base/ITimeSampling.h
  Base/ObjectInfo.h
  Base/PropertyInfo.h
  Base/Slab.h
  Base/TimeSamplingInfo.h
  Base/TypedPropertyTraits.h
 )

# Body part of project
SET( ALEMBIC_ASSET_BODY_CXX_FILES
  Body/DataTypeTuple.cpp
  Body/IAssetBody.cpp
  Body/IDataBody.cpp
  Body/IObjectBody.cpp
  Body/IParentBody.cpp
  Body/IPropertyBody.cpp
  Body/ISlabBody.cpp
  Body/ITimeSamplingBody.cpp
  Body/OAssetBody.cpp
  Body/ODataBody.cpp
  Body/OObjectBody.cpp
  Body/OParentBody.cpp
  Body/OPropertyBody.cpp
  Body/OSlabBody.cpp
  Body/OTimeSamplingBody.cpp )

SET( ALEMBIC_ASSET_BODY_H_FILES
  Body/BodyAll.h
  Body/DataTypeTuple.h
  Body/BodyAll.h
  Body/IAssetBody.h
  Body/IContextBody.h
  Body/IDataBody.h
  Body/IObjectBody.h
  Body/IParentBody.h
  Body/IPropertyBody.h
  Body/ISlabBody.h
  Body/ITimeSamplingBody.h
  Body/OAssetBody.h
  Body/OContextBody.h
  Body/ODataBody.h
  Body/OObjectBody.h
  Body/OParentBody.h
  Body/OPropertyBody.h
  Body/OSlabBody.h
  Body/OSlabMapBody.h
  Body/OTimeSamplingBody.h )

SET( ALEMBIC_ASSET_H_FILES
  AssetAll.h
  Foundation.h
  IAsset.h
  IBase.h
  IContext.h
  Init.h
  IObject.h
  IParentObject.h
  IProperty.h
  ISample.h
  OAsset.h
  OBase.h
  OObject.h
  OParentObject.h
  OProperty.h
   )

SET( ALEMBIC_ASSET_CXX_FILES
  IAsset.cpp
  IBase.cpp
  IContext.cpp
  Init.cpp
  IObject.cpp
  IProperty.cpp
  OAsset.cpp
  OBase.cpp
  OObject.cpp
  OProperty.cpp
   )

# C++ files for this project

ADD_LIBRARY( AlembicAsset
  ${ALEMBIC_ASSET_BASE_CXX_FILES}
  ${ALEMBIC_ASSET_BASE_H_FILES}

  ${ALEMBIC_ASSET_BODY_CXX_FILES}
  ${ALEMBIC_ASSET_BODY_H_FILES}

  ${ALEMBIC_ASSET_CXX_FILES}
  ${ALEMBIC_ASSET_H_FILES} 
)

# Libraries and linking stuff

# Installation
INSTALL( TARGETS AlembicAsset
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/static )
#
INSTALL( FILES ${ALEMBIC_ASSET_BASE_H_FILES}
         DESTINATION include/Alembic/Asset/Base
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )

INSTALL( FILES ${ALEMBIC_ASSET_BODY_H_FILES}
         DESTINATION include/Alembic/Asset/Body
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )

INSTALL( FILES ${ALEMBIC_ASSET_H_FILES}
         DESTINATION include/Alembic/Asset
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )

ADD_SUBDIRECTORY( Tests )


