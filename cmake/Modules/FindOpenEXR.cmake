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
# NOW, OPENEXR STUFF. EXR IS OPTIONAL, WHERASE ILMBASE IS NOT
#-******************************************************************************
#-******************************************************************************

IF(NOT DEFINED OPENEXR_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
          SET( ALEMBIC_OPENEXR_ROOT NOTFOUND )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( ALEMBIC_OPENEXR_ROOT "/usr/local/openexr-1.6.1/" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( ALEMBIC_OPENEXR_ROOT NOTFOUND )
        ELSE()
          SET( ALEMBIC_OPENEXR_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ELSE()
  SET( ALEMBIC_OPENEXR_ROOT ${OPENEXR_ROOT} )
ENDIF()

IF(NOT $ENV{OPENEXR_ROOT}x STREQUAL "x")
  SET( ALEMBIC_OPENEXR_ROOT $ENV{OPENEXR_ROOT})
ELSE()
  SET( ENV{OPENEXR_ROOT} ${OPENEXR_ROOT} )
ENDIF()


SET(LIBRARY_PATHS 
    ${ALEMBIC_OPENEXR_ROOT}/lib
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

SET(INCLUDE_PATHS 
    ${ALEMBIC_OPENEXR_ROOT}/include/OpenEXR/
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include/OpenEXR/
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    /usr/freeware/include
)

FIND_PATH( ALEMBIC_OPENEXR_INCLUDE_PATH ImfRgba.h
           PATHS
           ${INCLUDE_PATHS}
           DOC "The directory where ImfRgba.h resides" )

FIND_LIBRARY( ALEMBIC_OPENEXR_ILMIMF_LIB IlmImf
              PATHS 
              ${LIBRARY_PATHS}
              DOC "The IlmImf library" )


SET( OPENEXR_FOUND TRUE )

IF ( ${ALEMBIC_OPENEXR_INCLUDE_PATH} STREQUAL "ALEMBIC_OPENEXR_INCLUDE_PATH-NOTFOUND" ) 
  MESSAGE( STATUS "OpenEXR include path not found, disabling" )
  SET( OPENEXR_FOUND FALSE )
ENDIF()

IF ( ${ALEMBIC_OPENEXR_ILMIMF_LIB} STREQUAL "ALEMBIC_OPENEXR_ILMIMF_LIB-NOTFOUND" ) 
  MESSAGE( STATUS "OpenEXR libraries not found, disabling" )
  SET( OPENEXR_FOUND FALSE )
  SET( ALEMBIC_OPENEXR_LIBS NOTFOUND )
ENDIF()

IF (OPENEXR_FOUND)
  MESSAGE( STATUS "OPENEXR INCLUDE PATH: ${ALEMBIC_OPENEXR_INCLUDE_PATH}" )
  SET( ALEMBIC_OPENEXR_LIBS ${ALEMBIC_OPENEXR_ILMIMF_LIB} )
ENDIF()


