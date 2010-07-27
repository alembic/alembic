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

SET( CXX_FILES 
     Attribute.cpp
     AttributedObject.cpp
     BaseObject.cpp
     Dataset.cpp
     Dataspace.cpp
     Datatype.cpp
     Echo.cpp
     File.cpp
     Group.cpp
     HDF5HelloWorld.cpp
     Init.cpp
     PropertyList.cpp )

SET( H_FILES
     AlembicHDF5.h 
     Assert.h
     Attribute.h
     AttributedObject.h
     BaseObject.h
     BufferedObject.h
     Dataset.h
     Dataspace.h
     Datatype.h
     Echo.h
     Exception.h
     File.h
     Foundation.h
     Group.h
     HDF5HelloWorld.h
     Init.h
     ParentObject.h
     PropertyList.h )

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} )

# Create the library
IF ( ${ALEMBIC_HDF5_FOUND} )
#INCLUDE_DIRECTORIES( ${ALEMBIC_HDF5_INCLUDE_PATH} )

ADD_LIBRARY( AlembicHDF5 ${SOURCE_FILES} )

# Recurse into test subdirectory
ADD_SUBDIRECTORY( Tests )

#-******************************************************************************
#-******************************************************************************
# INSTALLATION
#-******************************************************************************
#-******************************************************************************

INSTALL( TARGETS AlembicHDF5
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/static )

INSTALL( FILES ${H_FILES}
         DESTINATION include/AlembicHDF5
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )

ENDIF()

