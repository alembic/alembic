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

ADD_EXECUTABLE(AbcMaterial_WriteMaterialTest
               WriteMaterial.cpp
               )
TARGET_LINK_LIBRARIES(AbcMaterial_WriteMaterialTest Alembic)
ADD_TEST(AbcMaterial_WriteMaterial_TEST AbcMaterial_WriteMaterialTest)

ADD_EXECUTABLE(AbcMaterial_MaterialFlattenTest
               MaterialFlattenTest.cpp PrintMaterial.cpp
               )
TARGET_LINK_LIBRARIES(AbcMaterial_MaterialFlattenTest Alembic)
ADD_TEST(AbcMaterial_MaterialFlatten_TEST AbcMaterial_MaterialFlattenTest)

ADD_EXECUTABLE(AbcMaterial_MaterialHasATest
               WriteHasAMaterial.cpp PrintMaterial.cpp
               )
TARGET_LINK_LIBRARIES(AbcMaterial_MaterialHasATest Alembic)
ADD_TEST( AbcMaterial_Material_TEST_HasA AbcMaterial_MaterialHasATest)

ADD_EXECUTABLE(AbcMaterial_MaterialAssignAndFlattenTest
               MaterialAssignAndFlattenTest.cpp PrintMaterial.cpp
               )
TARGET_LINK_LIBRARIES(AbcMaterial_MaterialAssignAndFlattenTest Alembic)
ADD_TEST(AbcMaterial_TEST_MaterialAssignAndFlatten AbcMaterial_MaterialAssignAndFlattenTest)

ADD_EXECUTABLE(AbcMaterial_NetworkNodeTest
               NetworkNodeTest.cpp PrintMaterial.cpp
               )
TARGET_LINK_LIBRARIES(AbcMaterial_NetworkNodeTest Alembic)
ADD_TEST(AbcMaterial_NetworkNodeTest AbcMaterial_NetworkNodeTest)

ADD_EXECUTABLE(AbcMaterial_WriteGeometryWithMaterials
               WriteGeometryWithMaterials.cpp
               )
TARGET_LINK_LIBRARIES(AbcMaterial_WriteGeometryWithMaterials Alembic)
ADD_TEST(AbcMaterial_WriteGeometryWithMaterials AbcMaterial_WriteGeometryWithMaterials)
