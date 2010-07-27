INCLUDE( CheckCXXSourceCompiles )

PROJECT( GTO )

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

IF( NOT WIN32 )
  FIND_PACKAGE( ZLIB )
ELSE()
  MESSAGE( "On Windows!" )
  FIND_LIBRARY( ZLIB_LIBRARY zlib
     "C:/Program Files (x86)/Microsoft Visual Studio 10.0/VC/lib" 
     "C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/lib" 
     "C:/Program Files (x86)/Microsoft Visual Studio 8.0/VC/lib" 
  )
ENDIF()


SET( CXX_FILES
     FlexLexer.cpp
     Parser.cpp
     Writer.cpp
     Reader.cpp
     RawData.cpp
     Utilities.cpp
     zhacks.cpp )

SET( EXT_H_FILES
     EXTProtocols.h
     Header.h
     Protocols.h
     RawData.h
     Reader.h
     Utilities.h
     Writer.h )

SET( INT_H_FILES
     FlexLexer.h
     Parser.h
     zhacks.h )

IF( ZLIB_FOUND )
    SET( CXX_FILES ${CXX_FILES} zhacks.cpp )
    SET( INT_H_FILES ${INT_H_FILES} zhacks.h )
ENDIF()

SET( SOURCE_FILES ${CXX_FILES} ${EXT_H_FILES} ${INT_H_FILES} )

ADD_LIBRARY( Gto STATIC ${SOURCE_FILES} )

SET_PROPERTY(
    TARGET Gto
    PROPERTY COMPILE_DEFINITIONS PACKAGE_NAME="Gto" PACKAGE_VERSION="3.5.3"
    PACKAGE_STRING="Gto 3.5.3" PACKAGE="gto" VERSION="3.5.3" GTO_SUPPORT_ZIP )

IF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    SET_PROPERTY( TARGET Gto APPEND PROPERTY COMPILE_DEFINITIONS LINUX )
ENDIF()

IF( WIN32 )
    SET_PROPERTY( TARGET Gto APPEND PROPERTY COMPILE_DEFINITIONS WIN32 )
ENDIF()

CHECK_CXX_SOURCE_COMPILES(
"#include <iostream>
int main( int argc, char** argv ) { if ( std::ios_base::cur ) return 0; }"
    HAVE_FULL_IOSTREAMS )

IF( HAVE_FULL_IOSTREAMS )
    SET_PROPERTY( TARGET Gto APPEND PROPERTY COMPILE_DEFINITIONS HAVE_FULL_IOSTREAMS )
ENDIF()

IF( CMAKE_COMPILER_IS_GNUCXX )
    SET_PROPERTY( TARGET Gto APPEND PROPERTY COMPILE_FLAGS "-fPIC" )
ENDIF()

IF( ZLIB_FOUND )
    MESSAGE( STATUS "GTO_SUPPORT_ZIP in da HIZZOUSE!" )
ENDIF()

INSTALL( TARGETS Gto
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/static )

INSTALL( FILES ${EXT_H_FILES}
         DESTINATION include/Gto
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )
