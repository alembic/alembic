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
ADD_SUBDIRECTORY(AbcCoreLayer)
ADD_SUBDIRECTORY(AbcGeom)
ADD_SUBDIRECTORY(AbcCollection)
ADD_SUBDIRECTORY(AbcMaterial)
ADD_SUBDIRECTORY(Ogawa)

ADD_LIBRARY(Alembic ${LIB_TYPE} ${CXX_FILES})

TARGET_INCLUDE_DIRECTORIES(Alembic
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
    ${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY}
    ${Boost_INCLUDE_DIRS}
    PRIVATE
    ${HDF5_INCLUDE_DIRS}
    )

IF (ALEMBIC_SHARED_LIBS)
  SET_TARGET_PROPERTIES(Alembic PROPERTIES DEFINE_SYMBOL ALEMBIC_EXPORTS
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    VERSION ${PROJECT_VERSION})
ENDIF()


TARGET_LINK_LIBRARIES(Alembic
    LINK_PUBLIC
    ${ALEMBIC_ILMBASE_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTERNAL_MATH_LIBS}
    LINK_PRIVATE
    ${HDF5_LIBRARIES}
    ${ZLIB_LIBRARY}
  )

# link in atomic if we are using tr1 and the compiler is new enough
# to want  __atomic_compare_exchange_n
IF ( ${ALEMBIC_LIB_USES_TR1} AND CMAKE_COMPILER_IS_GNUCXX AND
    CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8 )
    TARGET_LINK_LIBRARIES( Alembic atomic )
ENDIF()

SET( ALEMBIC_LIB_INSTALL_DIR lib CACHE PATH "Where to install the Alembic libs")
INSTALL(TARGETS Alembic
        EXPORT AlembicTargets
        LIBRARY DESTINATION ${ALEMBIC_LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${ALEMBIC_LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${ALEMBIC_LIB_INSTALL_DIR})

#-******************************************************************************
# PACKAGE EXPORTS
#-******************************************************************************

CONFIGURE_FILE(AlembicConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/AlembicConfig.cmake"
    @ONLY
    )

INCLUDE(CMakePackageConfigHelpers)
WRITE_BASIC_PACKAGE_VERSION_FILE(
    "${CMAKE_CURRENT_BINARY_DIR}/AlembicConfigVersion.cmake"
    VERSION
    ${PROJECT_VERSION}
    COMPATIBILITY
    AnyNewerVersion
    )

EXPORT(TARGETS
    Alembic
    FILE
    "${CMAKE_CURRENT_BINARY_DIR}/AlembicTargets.cmake"
    NAMESPACE
    Alembic::
    )

SET(ConfigPackageLocation lib/cmake/Alembic CACHE PATH
    "Where to install the Alembic's cmake files")

INSTALL(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/AlembicConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/AlembicConfigVersion.cmake"
    DESTINATION
    ${ConfigPackageLocation}
    )

INSTALL(EXPORT
    AlembicTargets
    FILE
    AlembicTargets.cmake
    NAMESPACE
    Alembic::
    DESTINATION
    ${ConfigPackageLocation}
    )
