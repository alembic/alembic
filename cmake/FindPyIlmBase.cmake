##-*****************************************************************************
##
## Copyright (c) 2009-2011,
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


# We shall worry about windowsification later.

#-******************************************************************************
#-******************************************************************************
# FIRST, PYILMBASE STUFF
#-******************************************************************************
#-******************************************************************************

IF(DEFINED USE_PYALEMBIC AND NOT USE_PYALEMBIC)
    MESSAGE(STATUS "Skipping finding PyIlmBase and PyImath")
ELSE()
    MESSAGE(STATUS "About to start finding PyIlmBase and PyImath")
    IF(NOT DEFINED PYILMBASE_ROOT)
        IF ( ${CMAKE_HOST_UNIX} )
            IF( ${DARWIN} )
              # TODO: set to default install path when shipping out
              SET( ALEMBIC_PYILMBASE_ROOT NOTFOUND )
            ELSE()
              # TODO: set to default install path when shipping out
              SET( ALEMBIC_PYILMBASE_ROOT "/usr/local/pyilmbase/" )
            ENDIF()
        ELSE()
            IF ( ${WINDOWS} )
              # TODO: set to 32-bit or 64-bit path
              SET( ALEMBIC_PYILMBASE_ROOT "C:/Program Files (x86)/pyilmbase/" )
            ELSE()
              SET( ALEMBIC_PYILMBASE_ROOT NOTFOUND )
            ENDIF()
        ENDIF()
    ELSE()
      SET( ALEMBIC_PYILMBASE_ROOT ${PYILMBASE_ROOT} )
    ENDIF()

    SET(LIBRARY_PATHS
        ${ALEMBIC_PYILMBASE_ROOT}/lib
        ${ALEMBIC_PYILMBASE_MODULE_DIRECTORY}
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        /usr/freeware/lib64
    )

    IF( DEFINED PYILMBASE_LIBRARY_DIR )
      SET( LIBRARY_PATHS ${PYILMBASE_LIBRARY_DIR} ${LIBRARY_PATHS} )
    ENDIF()

    SET(INCLUDE_PATHS
        ${ALEMBIC_PYILMBASE_ROOT}/include/OpenEXR/
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/include/OpenEXR/
        /usr/local/include
        /usr/include
        /usr/include/OpenEXR
        /sw/include # Fink
        /opt/local/include # DarwinPorts
        /opt/csw/include # Blastwave
        /opt/include
        /usr/freeware/include
    )

    FIND_PATH( ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY PyImath.h
               PATHS
               ${INCLUDE_PATHS}
               NO_DEFAULT_PATH
               NO_CMAKE_ENVIRONMENT_PATH
               NO_CMAKE_PATH
               NO_SYSTEM_ENVIRONMENT_PATH
               NO_CMAKE_SYSTEM_PATH
               DOC "The directory where PyImath.h resides" )

    IF( NOT DEFINED ALEMBIC_PYILMBASE_PYIMATH_LIB )
      FIND_LIBRARY( ALEMBIC_PYILMBASE_PYIMATH_LIB PyImath
                    PATHS
                    ${LIBRARY_PATHS}
                    NO_DEFAULT_PATH
                    NO_CMAKE_ENVIRONMENT_PATH
                    NO_CMAKE_PATH
                    NO_SYSTEM_ENVIRONMENT_PATH
                    NO_CMAKE_SYSTEM_PATH
                    DOC "The PyImath library" )
    ENDIF()

    IF( NOT DEFINED ALEMBIC_PYILMBASE_PYIMATH_MODULE )
      FIND_LIBRARY( ALEMBIC_PYILMBASE_PYIMATH_MODULE imathmodule.so
                    PATHS
                    ${LIBRARY_PATHS}
                    NO_DEFAULT_PATH
                    NO_CMAKE_ENVIRONMENT_PATH
                    NO_CMAKE_PATH
                    NO_SYSTEM_ENVIRONMENT_PATH
                    NO_CMAKE_SYSTEM_PATH
                    DOC "The PyImath library" )
    ENDIF()

    IF ( ${ALEMBIC_PYILMBASE_PYIMATH_LIB} STREQUAL "ALEMBIC_PYILMBASE_PYIMATH_LIB-NOTFOUND" )
      MESSAGE( FATAL_ERROR "pyilmbase libraries (PyImath) not found, required" )
    ENDIF()

    IF ( ${ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY} STREQUAL "ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY-NOTFOUND" )
      MESSAGE( FATAL_ERROR "pyilmbase header files not found, required: ALEMBIC_PYILMBASE_ROOT: ${ALEMBIC_PYILMBASE_ROOT}" )
    ENDIF()

    MESSAGE( STATUS "PYILMBASE INCLUDE PATH: ${ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY}" )
    MESSAGE( STATUS "PYIMATH LIB: ${ALEMBIC_PYILMBASE_PYIMATH_LIB}" )
    MESSAGE( STATUS "PYIMATH MODULE: ${ALEMBIC_PYILMBASE_PYIMATH_MODULE}" )

    SET( PYILMBASE_FOUND TRUE )
    SET( ALEMBIC_PYILMBASE_LIBS
           ${ALEMBIC_PYILMBASE_PYIMATH_LIB}
       )

ENDIF()
