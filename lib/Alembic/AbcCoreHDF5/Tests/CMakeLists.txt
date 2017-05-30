##-*****************************************************************************
##
## Copyright (c) 2009-2015,
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

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib ${PROJECT_BINARY_DIR}/lib)

SET(CXX_FILES
    ArchiveTests.cpp
    ArrayPropertyTests.cpp
    ScalarPropertyTests.cpp
    TimeSamplingTests.cpp
)

ADD_EXECUTABLE(AbcCoreHDF5_Test1 test1.cpp)
TARGET_LINK_LIBRARIES(AbcCoreHDF5_Test1 Alembic)

ADD_EXECUTABLE(AbcCoreHDF5_ArchiveTests ArchiveTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreHDF5_ArchiveTests Alembic)

ADD_EXECUTABLE(AbcCoreHDF5_ArrayPropertyTests ArrayPropertyTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreHDF5_ArrayPropertyTests Alembic)

ADD_EXECUTABLE(AbcCoreHDF5_ScalarPropertyTests ScalarPropertyTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreHDF5_ScalarPropertyTests Alembic)

ADD_EXECUTABLE(AbcCoreHDF5_TimeSamplingTests TimeSamplingTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreHDF5_TimeSamplingTests Alembic)

ADD_EXECUTABLE(AbcCoreHDF5_ObjectTests ObjectTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreHDF5_ObjectTests Alembic)

ADD_EXECUTABLE(AbcCoreHDF5_ConstantPropsTest ConstantPropsNumSampsTest.cpp)
TARGET_LINK_LIBRARIES(AbcCoreHDF5_ConstantPropsTest Alembic)

ADD_TEST(AbcCoreHDF5_TEST1 AbcCoreHDF5_Test1)
ADD_TEST(AbcCoreHDF5_ArchiveTESTS AbcCoreHDF5_ArchiveTests)
ADD_TEST(AbcCoreHDF5_ArrayPropertyTESTS AbcCoreHDF5_ArrayPropertyTests)
ADD_TEST(AbcCoreHDF5_ScalarPropertyTESTS AbcCoreHDF5_ScalarPropertyTests)
ADD_TEST(AbcCoreHDF5_TimeSamplingTESTS AbcCoreHDF5_TimeSamplingTests)
ADD_TEST(AbcCoreHDF5_ObjectTESTS AbcCoreHDF5_ObjectTests)
ADD_TEST(AbcCoreHDF5_ConstantPropsTest_TEST AbcCoreHDF5_ConstantPropsTest)
