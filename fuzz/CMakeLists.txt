# Fuzzing targets for Alembic
# These are built by OSS-Fuzz, not part of the main Alembic build

if(BUILD_FUZZERS)
    # Find Fuzzing engine
    find_package(Fuzzing REQUIRED)
    
    # Add include directories
    include_directories(
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/lib/Alembic/AbcCoreFactory
        ${CMAKE_SOURCE_DIR}/lib/Alembic/AbcCoreOgawa
        ${CMAKE_SOURCE_DIR}/lib/Alembic/AbcGeom
        ${CMAKE_SOURCE_DIR}/lib/Alembic/AbcMaterial
    )
    
    # Fuzzer for getArchive(vector<string>)
    add_executable(alembic_getarchive_vector_fuzzer
        alembic_getarchive_vector_fuzzer.cc
    )
    
    target_link_libraries(alembic_getarchive_vector_fuzzer
        Alembic::Alembic
        ${FuzzingEngine}
    )
    
    set_target_properties(alembic_getarchive_vector_fuzzer PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
    )
    
    # Add compiler flags for fuzzing
    target_compile_options(alembic_getarchive_vector_fuzzer PRIVATE
        ${FUZZER_COMPILE_OPTIONS}
    )
    
endif()
