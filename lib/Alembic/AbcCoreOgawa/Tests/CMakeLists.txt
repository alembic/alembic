##-*****************************************************************************
##
## Copyright (c) 2013-2015,
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
    HashesTests.cpp
    ScalarPropertyTests.cpp
    TimeSamplingTests.cpp
)

ADD_EXECUTABLE(AbcCoreOgawa_ArchiveTests ArchiveTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreOgawa_ArchiveTests Alembic)

ADD_EXECUTABLE(AbcCoreOgawa_ArrayPropertyTests ArrayPropertyTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreOgawa_ArrayPropertyTests Alembic)

ADD_EXECUTABLE(AbcCoreOgawa_HashesTests HashesTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreOgawa_HashesTests Alembic)

ADD_EXECUTABLE(AbcCoreOgawa_ScalarPropertyTests ScalarPropertyTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreOgawa_ScalarPropertyTests Alembic)

ADD_EXECUTABLE(AbcCoreOgawa_TimeSamplingTests TimeSamplingTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreOgawa_TimeSamplingTests Alembic)

ADD_EXECUTABLE(AbcCoreOgawa_ObjectTests ObjectTests.cpp)
TARGET_LINK_LIBRARIES(AbcCoreOgawa_ObjectTests Alembic)

ADD_EXECUTABLE(AbcCoreOgawa_ConstantPropsTest ConstantPropsNumSampsTest.cpp)
TARGET_LINK_LIBRARIES(AbcCoreOgawa_ConstantPropsTest Alembic)

ADD_TEST(AbcCoreOgawa_ArchiveTESTS AbcCoreOgawa_ArchiveTests)
ADD_TEST(AbcCoreOgawa_ArrayPropertyTESTS AbcCoreOgawa_ArrayPropertyTests)
ADD_TEST(AbcCoreOgawa_HashesTESTS AbcCoreOgawa_HashesTests)
ADD_TEST(AbcCoreOgawa_ScalarPropertyTESTS AbcCoreOgawa_ScalarPropertyTests)
ADD_TEST(AbcCoreOgawa_TimeSamplingTESTS AbcCoreOgawa_TimeSamplingTests)
ADD_TEST(AbcCoreOgawa_ObjectTESTS AbcCoreOgawa_ObjectTests)
ADD_TEST(AbcCoreOgawa_ConstantPropsTest_TEST AbcCoreOgawa_ConstantPropsTest)
