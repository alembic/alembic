#!/bin/bash
# Build script for Alembic fuzzers (used by OSS-Fuzz)

set -e

# Build directory
BUILD_DIR=${BUILD_DIR:-"build_fuzz"}
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with fuzzing support
cmake .. \
    -DBUILD_FUZZERS=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DALEMBIC_SHARED_LIBS=OFF \
    -DCMAKE_C_COMPILER=${CC:-clang} \
    -DCMAKE_CXX_COMPILER=${CXX:-clang++}

# Build fuzzers
make -j$(nproc) alembic_getarchive_vector_fuzzer

echo "Fuzzers built successfully!"
echo "Executables:"
echo "  $BUILD_DIR/alembic_getarchive_vector_fuzzer"
