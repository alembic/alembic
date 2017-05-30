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

ADD_EXECUTABLE(AlembicUtilOperatorBool_Test OperatorBoolTest.cpp)
TARGET_LINK_LIBRARIES(AlembicUtilOperatorBool_Test Alembic)

ADD_EXECUTABLE(AlembicUtilTokenMap_Test TokenMapTest.cpp)
TARGET_LINK_LIBRARIES(AlembicUtilTokenMap_Test Alembic)

ADD_EXECUTABLE(AlembicUtilDimensions_Test_Jeffs DimensionsTestJeffs.cpp)
TARGET_LINK_LIBRARIES(AlembicUtilDimensions_Test_Jeffs Alembic)

ADD_EXECUTABLE(AlembicUtilNaming_Test NamingTest.cpp)
TARGET_LINK_LIBRARIES(AlembicUtilNaming_Test Alembic)

ADD_TEST(AlembicUtilOperatorBool_TEST AlembicUtilOperatorBool_Test)
ADD_TEST(AlembicUtilTokenMap_TEST AlembicUtilTokenMap_Test)
ADD_TEST(AlembicUtilDimensionsJeffs_TEST AlembicUtilDimensions_Test_Jeffs)
ADD_TEST(AlembicUtilNaming_TEST AlembicUtilNaming_Test)
