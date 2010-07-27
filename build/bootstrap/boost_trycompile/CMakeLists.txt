# BOOST try-compile CMakeLists.txt

# If you know the boost root and there aren't any default versions of boost in
# the default system paths use:
# cmake '-UBoost_*' -DBOOST_ROOT:STRING=<path/to/boost> .

# If you are still having trouble, use explit include paths:
# cmake '-UBoost_*' -DBOOST_INCLUDEDIR:STRING=<path/to/boost_include_dir> .

# If the libraries are in a separate location to the include files, use:
# cmake '-UBoost_*' -DBOOST_INCLUDEDIR:STRING=<path/to/boost_include_dir> -DBOOST_LIBRARYDIR:STRING=<path/to/boost_library_dir> .

# '-UBoost_*' removes any Boost_* entries from the cache so it can be run
# multiple times without breaking the rest of the cached entries

# -------------------------------------------------------------------------------------

# The three ways to search are:

# BOOST_INCLUDEDIR: if it is defined, then search for the boost header files in
# the specific location. 

# BOOST_LIBRARYDIR: if it is defined, then search for the boost library files in
# the specific location. 

# BOOST_ROOT: Set the boost root to the defined CMake variable BOOST_ROOT, otherwise
# it will use the defaults

# For more info read:
# /usr/share/cmake-2.8.0/share/cmake-2.8/Modules/FindBoost.cmake

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(BOOST_TRYCOMPILE)

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

SET(Boost_DEBUG TRUE)

IF(DEFINED BOOST_INCLUDEDIR)
    MESSAGE(STATUS "Using BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}" )
ENDIF()
 
IF(DEFINED BOOST_LIBRARYDIR)
    MESSAGE(STATUS "Using BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}" )
ENDIF()

# If BOOST_ROOT not set, use predefined paths
IF(NOT DEFINED BOOST_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
	        SET( BOOST_ROOT "/usr/local/include/boost-1_43/boost" )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( BOOST_ROOT "/usr/include/boost-1_42/boost" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( BOOST_ROOT "C:/Program Files (x86)/boost-1_42/boost" )

          # For 64-bit builds use:
          # SET( BOOST_ROOT "C:/Program Files/boost-1_42/boost" )
        ELSE()
          SET( BOOST_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ENDIF()

IF(NOT DEFINED CMAKE_PREFIX_PATH)
    MESSAGE(STATUS "Using BOOST_ROOT: ${BOOST_ROOT}" )
ELSE()
    MESSAGE(STATUS "Using CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}" )
    UNSET(BOOST_ROOT)
ENDIF()

SET( Boost_USE_STATIC_LIBS TRUE )
SET( Boost_USE_MULTITHREADED TRUE )

IF ( ${CMAKE_HOST_UNIX} )
  IF ( ${DARWIN} )
    SET( Boost_ADDITIONAL_VERSIONS "1.43" "1.43.0" )
    FIND_PACKAGE( Boost 1.43.0 COMPONENTS iostreams program_options filesystem system regex python REQUIRED)
  ELSE()
    SET( Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.43" "1.43.0" )
    FIND_PACKAGE( Boost 1.42.0 COMPONENTS iostreams program_options filesystem system regex python REQUIRED)
  ENDIF()
ELSE()
  IF ( ${CMAKE_HOST_WIN32} )
    SET( Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.43" "1.43.0" )
    FIND_PACKAGE( Boost 1.42.0 COMPONENTS iostreams program_options filesystem system regex python REQUIRED)
  ELSE()
    MESSAGE( ERROR "BOOST: This platform is not supported yet" )
  ENDIF()
ENDIF()

IF (Boost_FOUND)
  IF( Boost_VERSION LESS 104200 )
    MESSAGE(FATAL_ERROR "FOUND INCORRECT BOOST VERSION: ${Boost_LIB_VERSION}")
  ENDIF()

  MESSAGE( STATUS "BOOST FOUND: ${Boost_FOUND}" )
  MESSAGE( STATUS "BOOST INCLUDE DIRS: ${Boost_INCLUDE_DIRS}")
  MESSAGE( STATUS "BOOST LIBRARIES: ${Boost_LIBRARIES}")

  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  ADD_EXECUTABLE(boost_trycompile boost_trycompile.cpp)  
  TARGET_LINK_LIBRARIES(boost_trycompile ${Boost_LIBRARIES})
ELSE()
  MESSAGE(SEND_ERROR "Boost not correctly specified")
ENDIF()


