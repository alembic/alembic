##-*****************************************************************************
##
## Copyright (c) 2009-2010, Industrial Light & Magic,
##   a division of Lucasfilm Entertainment Company Ltd.
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

PROJECT( ALEMBIC_TAKO_SPI_EXPORT_MAYA_PLUGIN )

SET( H_FILES
  AlembicTakoExport.h
  AttributesWriter.h
  Foundation.h
  HDFWriteJob.h
  MayaCameraWriter.h
  MayaLightWriter.h
  MayaLocatorWriter.h
  MayaMeshWriter.h
  MayaNurbsCurveWriter.h
  MayaNurbsSurfaceWriter.h
  MayaPointPrimitiveWriter.h
  MayaTransformWriter.h
  MayaUtility.h
)
SET( CXX_FILES
  AlembicTakoExport.cpp
  AttributesWriter.cpp
  HDFWriteJob.cpp
  MayaCameraWriter.cpp
  MayaLightWriter.cpp
  MayaLocatorWriter.cpp
  MayaMeshWriter.cpp
  MayaNurbsCurveWriter.cpp
  MayaNurbsSurfaceWriter.cpp
  MayaPointPrimitiveWriter.cpp
  MayaTransformWriter.cpp
  MayaUtility.cpp
)

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} )

SET( CORE_LIBS
  AlembicTakoSPI
  AlembicTakoAbstract
  AlembicHDF5
  AlembicMD5Hash
  AlembicUtil )

INCLUDE_DIRECTORIES( ${ALEMBIC_HDF5_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ".." )

ADD_MAYA_CXX_PLUGIN( AlembicTakoSPIExport ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES( AlembicTakoSPIExport
                         ${CORE_LIBS}
                         ${ALEMBIC_HDF5_LIBS}
			 ${ALEMBIC_ILMBASE_LIBS}
			 ${CMAKE_THREAD_LIBS_INIT}
			 ${ZLIB_LIBRARIES} m )

# Worry about automatic installation later
#GET_TARGET_PROPERTY( ModuleFileName AlembicExport LOCATION )
#GET_FILENAME_COMPONENT( ModuleFileNoDirectory ${ModuleFileName} NAME )
#SET( INST_PLUGIN "${CMAKE_INSTALL_PREFIX}/maya/modules/AlembicExport/plug-ins/${ModuleFileNoDirectory}" )
#SET( INST_MOD_TXT "${CMAKE_INSTALL_PREFIX}/maya/modules/AlembicExport.txt" )

#SET( ALEMBIC_EXPORT_MAYA_MODULE_ROOT "${CMAKE_INSTALL_PREFIX}/maya/modules/AlembicExport" )
#SET( ALEMBIC_EXPORT_MAYA_MODULE_VERSION ${PROJECT_VERSION} )


INSTALL( TARGETS AlembicTakoSPIExport
         DESTINATION maya/plug-ins )

#  INSTALL( FILES "AlembicExportOptions.mel"
#           DESTINATION "${ALEMBIC_EXPORT_MAYA_MODULE_ROOT}/scripts/" )
#  INSTALL( CODE "FILE( WRITE ${INST_MOD_TXT} \"+ AlembicExport ${ALEMBIC_EXPORT_MAYA_MODULE_VERSION} ${ALEMBIC_EXPORT_MAYA_MODULE_ROOT}\" )" )
#  INSTALL( CODE "FILE( MAKE_DIRECTORY ${ALEMBIC_EXPORT_MAYA_MODULE_ROOT}/icons )" )
#  INSTALL( CODE "FILE( MAKE_DIRECTORY ${ALEMBIC_EXPORT_MAYA_MODULE_ROOT}/presets )" )

