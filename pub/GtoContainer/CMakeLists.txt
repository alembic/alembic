PROJECT( GTOCONTAINER )

CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

SET( CXX_FILES
    Component.cpp
    ObjectVector.cpp
    PropertyContainer.cpp
    Property.cpp
    Reader.cpp
    StdProperties.cpp
    Writer.cpp )

SET( EXT_H_FILES
    Component.h
    Exception.h
    Foundation.h
    ObjectVector.h
    PropertyContainer.h
    Property.h
    Protocol.h
    Reader.h
    StdProperties.h
    TypedProperty.h
    Utilities.h
    Writer.h )

SET( SOURCE_FILES ${CXX_FILES} ${EXT_H_FILES} )

ADD_LIBRARY( GtoContainer STATIC ${SOURCE_FILES} )

SET_PROPERTY(
    TARGET GtoContainer
    PROPERTY COMPILE_DEFINITIONS PACKAGE_NAME="Gto" PACKAGE_VERSION="3.5.3"
    PACKAGE_STRING="Gto 3.5.3" PACKAGE="gto" VERSION="3.5.3" )

IF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    SET_PROPERTY( TARGET GtoContainer APPEND PROPERTY COMPILE_DEFINITIONS LINUX )
ENDIF()

IF( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    SET_PROPERTY( TARGET GtoContainer APPEND PROPERTY COMPILE_DEFINITIONS WIN32 )
ENDIF()

CHECK_CXX_SOURCE_COMPILES(
"#include <iostream>
int main( int argc, char** argv ) { if ( std::ios_base::cur ) return 0; }"
    HAVE_FULL_IOSTREAMS )

IF( HAVE_FULL_IOSTREAMS )
    SET_PROPERTY( TARGET GtoContainer APPEND PROPERTY COMPILE_DEFINITIONS HAVE_FULL_IOSTREAMS )
ENDIF()

IF( CMAKE_COMPILER_IS_GNUCXX )
    SET_PROPERTY( TARGET GtoContainer APPEND PROPERTY COMPILE_FLAGS "-fPIC" )
ENDIF()

INSTALL( TARGETS GtoContainer
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/static )

INSTALL( FILES ${EXT_H_FILES}
         DESTINATION include/GtoContainer
         PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )
