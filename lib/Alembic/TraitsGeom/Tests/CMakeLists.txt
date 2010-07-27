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

# Create the library
IF ( ${ALEMBIC_HDF5_FOUND} )

#-******************************************************************************
ADD_EXECUTABLE( AlembicTraitsGeom_Test1 meshOut1.cpp )
TARGET_LINK_LIBRARIES( AlembicTraitsGeom_Test1 
                       AlembicTraitsGeom
                       AlembicTraits 
                       AlembicAsset 
                       AlembicMD5Hash
                       AlembicHDF5 AlembicUtil  
                       ${ALEMBIC_HDF5_LIBS}
                       ${ALEMBIC_ILMBASE_LIBS}
                       ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARIES} m )

ADD_EXECUTABLE( AlembicTraitsGeom_Test2 subdOut1.cpp )
TARGET_LINK_LIBRARIES( AlembicTraitsGeom_Test2 
                       AlembicTraitsGeom
                       AlembicTraits 
                       AlembicAsset 
                       AlembicMD5Hash
                       AlembicHDF5 AlembicUtil  
                       ${ALEMBIC_HDF5_LIBS}
                       ${ALEMBIC_ILMBASE_LIBS}
                       ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARIES} m )

ADD_EXECUTABLE( AlembicTraitsGeom_Test3 meshBounds1.cpp )
TARGET_LINK_LIBRARIES( AlembicTraitsGeom_Test3 
                       AlembicTraitsGeom
                       AlembicTraits 
                       AlembicAsset 
                       AlembicMD5Hash
                       AlembicHDF5 AlembicUtil  
                       ${ALEMBIC_HDF5_LIBS}
                       ${ALEMBIC_ILMBASE_LIBS}
                       ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARIES} m )

ADD_EXECUTABLE( AlembicTraitsGeom_Test4 cameraOut1.cpp )
TARGET_LINK_LIBRARIES( AlembicTraitsGeom_Test4 
                       AlembicTraitsGeom
                       AlembicTraits 
                       AlembicAsset 
                       AlembicMD5Hash
                       AlembicHDF5 AlembicUtil  
                       ${ALEMBIC_HDF5_LIBS}
                       ${ALEMBIC_ILMBASE_LIBS}
                       ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARIES} m )

ADD_EXECUTABLE( AlembicTraitsGeom_Test5 partsTest1.cpp  )
TARGET_LINK_LIBRARIES( AlembicTraitsGeom_Test5
                       AlembicTraitsGeom
                       AlembicTraits 
                       AlembicAsset 
                       AlembicMD5Hash
                       AlembicHDF5 AlembicUtil  
                       ${ALEMBIC_HDF5_LIBS}
                       ${ALEMBIC_ILMBASE_LIBS}
                       ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARIES} m )

#-******************************************************************************
#-******************************************************************************
ADD_TEST( AlembicTraitsGeom_TEST1 AlembicTraitsGeom_Test1 )
ADD_TEST( AlembicTraitsGeom_TEST2 AlembicTraitsGeom_Test2 )
ADD_TEST( AlembicTraitsGeom_TEST4 AlembicTraitsGeom_Test4 )
ADD_TEST( AlembicTraitsGeom_TEST5 AlembicTraitsGeom_Test5 )

ENDIF()

