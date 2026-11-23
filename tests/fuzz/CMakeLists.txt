SET(FUZZING_ENGINE "$ENV{LIB_FUZZING_ENGINE}" CACHE STRING
    "Linker flags or library for the fuzzing engine")
SEPARATE_ARGUMENTS(FUZZING_ENGINE_FLAGS NATIVE_COMMAND "${FUZZING_ENGINE}")

ADD_EXECUTABLE(alembic_dump_info_fuzzer alembic_dump_info_fuzzer.cc)
TARGET_LINK_LIBRARIES(alembic_dump_info_fuzzer
    PRIVATE Alembic::Alembic ${FUZZING_ENGINE_FLAGS})
