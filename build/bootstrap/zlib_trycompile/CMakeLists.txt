# ZLIB try-compile CMakeLists.txt

# If you know the zlib root and there aren't any default versions of zlib in
# the default system paths use:
# cmake '-UZLIB_*' -DZLIB_ROOT:STRING=<path/to/zlib> .

# If you are still having trouble, use explit include paths:
# cmake '-UZLIB_*' -DZLIB_INCLUDEDIR:STRING=<path/to/zlib_include_dir> .

# If the libraries are in a separate location to the include files, use:
# cmake '-UZLIB_*' -DZLIB_INCLUDEDIR:STRING=<path/to/zlib_include_dir> -DZLIB_LIBRARYDIR:STRING=<path/to/zlib_library_dir> .

# '-UZLIB_*' removes any ZLIB_* entries from the cache so it can be run
# multiple times without breaking the rest of the cached entries

# -------------------------------------------------------------------------------------

# The three ways to search are:

# ZLIB_INCLUDEDIR: if it is defined, then search for the zlib header files in
# the specific location. 

# ZLIB_LIBRARYDIR: if it is defined, then search for the zlib library files in
# the specific location. 

# ZLIB_ROOT: Set the boost root to the defined CMake variable ZLIB_ROOT, otherwise
# it will use the defaults

# For more info read:
# /usr/share/cmake-2.8.0/share/cmake-2.8/Modules/FindZLIB.cmake

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ZLIB_TRYCOMPILE)

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

IF(DEFINED ZLIB_INCLUDEDIR)
    MESSAGE(STATUS "Using ZLIB_INCLUDEDIR: ${ZLIB_INCLUDEDIR}" )
ENDIF()
 
IF(DEFINED ZLIB_LIBRARYDIR)
    MESSAGE(STATUS "Using ZLIB_LIBRARYDIR: ${ZLIB_LIBRARYDIR}" )
ENDIF()

# If ZLIB_ROOT not set, use predefined paths
IF(NOT DEFINED ZLIB_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
	        SET( ZLIB_ROOT "/usr/local/z" )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( ZLIB_ROOT "/usr/local/z" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( ZLIB_ROOT "C:/Program Files (x86)/z" )

          # For 64-bit builds use:
          # SET( ZLIB_ROOT "C:/Program Files/z" )
        ELSE()
          SET( ZLIB_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ENDIF()

IF(NOT DEFINED CMAKE_PREFIX_PATH)
    MESSAGE(STATUS "Using ZLIB_ROOT: ${ZLIB_ROOT}" )
ELSE()
    MESSAGE(STATUS "Using CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}" )
    UNSET(ZLIB_ROOT)
ENDIF()

FIND_PACKAGE( ZLIB REQUIRED)

IF (ZLIB_FOUND)
  MESSAGE( STATUS "ZLIB FOUND: ${ZLIB_FOUND}" )
  MESSAGE( STATUS "ZLIB INCLUDE DIRS: ${ZLIB_INCLUDE_DIRS}")
  MESSAGE( STATUS "ZLIB LIBRARIES: ${ZLIB_LIBRARIES}")

  INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
  ADD_EXECUTABLE(zlib_trycompile zlib_trycompile.cpp)  
  TARGET_LINK_LIBRARIES(zlib_trycompile ${ZLIB_LIBRARIES})
ELSE()
  MESSAGE(SEND_ERROR "ZLIB not correctly specified")
ENDIF()


