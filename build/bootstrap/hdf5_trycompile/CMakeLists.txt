# HDF5 try-compile CMakeLists.txt

# use explit include paths and library:
# cmake '-UHDF5_*' -DHDF5_C_INCLUDE_DIR:STRING=<path/to/HDF5_include_dir> 
#  -DHDF5_hdf5_gl_LIBRARY:STRING=<path/to/HDF5_hdf5_hl_library> .
#
# '-UHDF5_*' removes any HDF5_* entries from the cache so it can be run
# multiple times without breaking the rest of the cached entries

# -------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(HDF5_TRYCOMPILE)

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

IF(DEFINED HDF5_C_INCLUDE_DIR)
  IF(DEFINED HDF5_hdf5_hl_LIBRARY)
      MESSAGE(STATUS "Using HDF5_hdf5_hl_LIBRARY: ${HDF5_hdf5_hl_LIBRARY}" )
      MESSAGE(STATUS "Using HDF5_C_INCLUDE_DIR: ${HDF5_C_INCLUDE_DIR}" )

    # TODO: TAILOR FOR CROSS-COMPILING CASES, see: http://www.cmake.org/cmake/help/cmake-2-8-docs.html
    MESSAGE( STATUS "COMPILING HDF5 TRY COMPILE EXECUTABLE IN ${PROJECT_BINARY_DIR}. WITH ${HDF5_hdf5_hl_LIBRARY}")

    # Added -lm to the front to work around a bug in CMake TRY_RUN that doesn't
    # accept a string variable to LINK_LIBRARIES
    SET(TRYCOMPILE_LIBS "-lm ${HDF5_hdf5_hl_LIBRARY}")
    SET(USING_THESE_LIBS "-DLINK_LIBRARIES:INTERNAL=${TRYCOMPILE_LIBS}")
    TRY_RUN(HDF5_RUN_RESULT HDF5_COMPILE_RESULT
            ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/hdf5_trycompile.cpp 
            CMAKE_FLAGS ${USING_THESE_LIBS}
            COMPILE_DEFINITIONS "-I${HDF5_C_INCLUDE_DIR}"
            COMPILE_OUTPUT_VARIABLE HDF5_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE HDF5_RUN_OUTPUT
            )

    IF(NOT HDF5_COMPILE_RESULT)
      MESSAGE( FATAL_ERROR "FAILED TO COMPILE A WORKING HDF5 TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${HDF5_COMPILE_OUTPUT}")
    ELSE()
      MESSAGE( STATUS "COMPILED A WORKING HDF5 TRY COMPILE EXECUTABLE.\nCOMPILE LOG:\n${HDF5_COMPILE_OUTPUT}")
    ENDIF()

    IF(HDF5_RUN_RESULT STREQUAL "FAILED_TO_RUN")
      MESSAGE( FATAL_ERROR "FAILED TO RUN A WORKING HDF5 TRY COMPILE EXECUTABLE.\nERROR LOG:\n${HDF5_RUN_OUTPUT}")
    ELSE()
      MESSAGE( STATUS "SUCCESSFULY RAN A WORKING HDF5 TRY COMPILE EXECUTABLE.\nLOG:\n${HDF5_RUN_OUTPUT}")
    ENDIF()

  ELSE()
    MESSAGE(FATAL_ERROR "HDF5 not correctly specified")
  ENDIF()
ELSE()
  MESSAGE(FATAL_ERROR "HDF5 not correctly specified")
ENDIF()
