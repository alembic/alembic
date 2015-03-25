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

ADD_SUBDIRECTORY(Util)
ADD_SUBDIRECTORY(AbcCoreAbstract)
ADD_SUBDIRECTORY(AbcCoreOgawa)
IF (USE_HDF5)
    ADD_SUBDIRECTORY(AbcCoreHDF5)
ENDIF()
ADD_SUBDIRECTORY(Abc)
ADD_SUBDIRECTORY(AbcCoreFactory)
ADD_SUBDIRECTORY(AbcGeom)
ADD_SUBDIRECTORY(AbcCollection)
ADD_SUBDIRECTORY(AbcMaterial)
ADD_SUBDIRECTORY(Ogawa)

# http://www.cmake.org/Wiki/CMake/Tutorials/Object_Library
IF (USE_HDF5)
    SET(TARGET_OBJECTS
        $<TARGET_OBJECTS:AlembicAbcMaterial>
        $<TARGET_OBJECTS:AlembicAbcGeom>
        $<TARGET_OBJECTS:AlembicAbcCollection>
        $<TARGET_OBJECTS:AlembicAbcCoreFactory>
        $<TARGET_OBJECTS:AlembicAbc>
        $<TARGET_OBJECTS:AlembicAbcCoreHDF5>
        $<TARGET_OBJECTS:AlembicAbcCoreOgawa>
        $<TARGET_OBJECTS:AlembicAbcCoreAbstract>
        $<TARGET_OBJECTS:AlembicOgawa>
        $<TARGET_OBJECTS:AlembicUtil>
    )
ELSE()
    SET(TARGET_OBJECTS
        $<TARGET_OBJECTS:AlembicAbcMaterial>
        $<TARGET_OBJECTS:AlembicAbcGeom>
        $<TARGET_OBJECTS:AlembicAbcCollection>
        $<TARGET_OBJECTS:AlembicAbcCoreFactory>
        $<TARGET_OBJECTS:AlembicAbc>
        $<TARGET_OBJECTS:AlembicAbcCoreOgawa>
        $<TARGET_OBJECTS:AlembicAbcCoreAbstract>
        $<TARGET_OBJECTS:AlembicOgawa>
        $<TARGET_OBJECTS:AlembicUtil>
    )
ENDIF()
ADD_LIBRARY(Alembic ${LIB_TYPE} ${TARGET_OBJECTS})

# fixes undefined symbols for x86_64
IF (ALEMBIC_SHARED_LIBS AND DARWIN)
    TARGET_LINK_LIBRARIES(${CORE_LIBS})
ENDIF()

INSTALL(TARGETS Alembic
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
