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

IF(DEFINED BOOST_INCLUDEDIR)
    MESSAGE(STATUS "Using BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}" )
ENDIF()

IF(DEFINED Boost_PROGRAM_OPTIONS_LIBRARY)
    MESSAGE(STATUS "Using Boost_PROGRAM_OPTIONS_LIBRARY: ${Boost_PROGRAM_OPTIONS_LIBRARY}" )
ENDIF()


MESSAGE( STATUS "COMPILING BOOST TRY COMPILE EXECUTABLE IN ${PROJECT_BINARY_DIR}.")

# Added -lm to the front to work around a bug in CMake TRY_RUN that doesn't
# accept a string variable to LINK_LIBRARIES
IF ( NOT WINDOWS )
    SET(TRYCOMPILE_LIBS "-lm ${Boost_PROGRAM_OPTIONS_LIBRARY}")
ELSE()
    SET(TRYCOMPILE_LIBS "${Boost_PROGRAM_OPTIONS_LIBRARY}")
ENDIF()
SET(USING_THESE_LIBS "-DLINK_LIBRARIES:INTERNAL=${TRYCOMPILE_LIBS}")

TRY_RUN(BOOST_RUN_RESULT BOOST_COMPILE_RESULT
  ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/boost_trycompile.cpp
  CMAKE_FLAGS ${USING_THESE_LIBS}
  COMPILE_DEFINITIONS "-I${BOOST_INCLUDEDIR}"
  COMPILE_OUTPUT_VARIABLE BOOST_COMPILE_OUTPUT
  RUN_OUTPUT_VARIABLE BOOST_RUN_OUTPUT
  )

IF(NOT BOOST_COMPILE_RESULT)
  MESSAGE( FATAL_ERROR "FAILED TO COMPILE A WORKING BOOST TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${BOOST_COMPILE_OUTPUT}")
ELSE()
  MESSAGE( STATUS "COMPILED A WORKING BOOST TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${BOOST_COMPILE_OUTPUT}")
ENDIF()

IF(BOOST_RUN_RESULT STREQUAL "FAILED_TO_RUN")
  MESSAGE( FATAL_ERROR "FAILED TO RUN A WORKING BOOST TRY COMPILE EXECUTABLE.\nERROR LOG:\n${BOOST_RUN_OUTPUT}")
ELSE()
  MESSAGE( STATUS "SUCCESSFULY RAN A WORKING BOOST TRY COMPILE EXECUTABLE.\nLOG:\n${BOOST_RUN_OUTPUT}")
ENDIF()

