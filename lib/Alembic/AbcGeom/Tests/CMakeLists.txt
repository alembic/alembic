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

ADD_EXECUTABLE(AbcGeom_PolyMeshTest
               MeshData.h
               MeshData.cpp
               PolyMeshTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_PolyMeshTest ${CORE_LIBS})
ADD_TEST(AbcGeom_PolyMesh_TEST AbcGeom_PolyMeshTest)

ADD_EXECUTABLE(AbcGeom_HelperLibTest
                HelperLibTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_HelperLibTest ${CORE_LIBS})
ADD_TEST(AbcGeom_HelperLib_TEST AbcGeom_HelperLibTest)

ADD_EXECUTABLE(AbcGeom_PointsTest
               PointsTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_PointsTest ${CORE_LIBS})
ADD_TEST(AbcGeom_Points_TEST AbcGeom_PointsTest)

ADD_EXECUTABLE(AbcGeom_NurbsTest
               NurbsData.h
               NurbsData.cpp
               NurbsTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_NurbsTest ${CORE_LIBS})
ADD_TEST(AbcGeom_Nurbs_TEST AbcGeom_NurbsTest)

ADD_EXECUTABLE(AbcGeom_SubDTest
               MeshData.h
               MeshData.cpp
               SubDTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_SubDTest ${CORE_LIBS})
ADD_TEST(AbcGeom_SubD_TEST AbcGeom_SubDTest)

ADD_EXECUTABLE(AbcGeom_SubDFaceSetTest
               MeshData.h
               MeshData.cpp
               SubDFaceSetTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_SubDFaceSetTest ${CORE_LIBS})
ADD_TEST(AbcGeom_SubDFaceSet_TEST AbcGeom_SubDFaceSetTest)

ADD_EXECUTABLE(AbcGeom_SchemaMatchingTest
               SchemaMatchingTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_SchemaMatchingTest ${CORE_LIBS})
ADD_TEST(AbcGeom_SchemaMatching_TEST AbcGeom_SchemaMatchingTest)

ADD_EXECUTABLE(AbcGeom_XformTest
               XformTests.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_XformTest  ${CORE_LIBS})
ADD_TEST(AbcGeom_Xform_TEST  AbcGeom_XformTest)

ADD_EXECUTABLE(AbcGeom_XformTest2
               XformTests2.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_XformTest2  ${CORE_LIBS})
ADD_TEST(AbcGeom_Xform2_TEST  AbcGeom_XformTest2)

ADD_EXECUTABLE(AbcGeom_CurvesTest
               CurvesData.h
               CurvesData.cpp
               CurvesTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_CurvesTest ${CORE_LIBS})
ADD_TEST(AbcGeom_Curves_TEST AbcGeom_CurvesTest)

ADD_EXECUTABLE(AbcGeom_GeomBaseTest
               CurvesData.h
               CurvesData.cpp
               GeomBaseTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_GeomBaseTest ${CORE_LIBS})
ADD_TEST(AbcGeom_GeomBase_TEST AbcGeom_GeomBaseTest)

ADD_EXECUTABLE(AbcGeom_TransformingMeshTest
               MeshData.h
               MeshData.cpp
               TransformingMeshTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_TransformingMeshTest ${CORE_LIBS})
ADD_TEST(AbcGeom_TransformingMesh_TEST AbcGeom_TransformingMeshTest)

ADD_EXECUTABLE(AbcGeom_CompileTest
               CompileTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_CompileTest ${CORE_LIBS})
ADD_TEST(AbcGeom_CompileTest_TEST AbcGeom_CompileTest)

ADD_EXECUTABLE(AbcGeom_LightTest
               LightTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_LightTest ${CORE_LIBS})
ADD_TEST(AbcGeom_LightTest_TEST AbcGeom_LightTest)

ADD_EXECUTABLE(AbcGeom_CameraTest
               CameraTest.cpp)
TARGET_LINK_LIBRARIES(AbcGeom_CameraTest ${CORE_LIBS})
ADD_TEST(AbcGeom_Points_TEST AbcGeom_CameraTest)

ADD_EXECUTABLE(playground PlayGround.cpp)
TARGET_LINK_LIBRARIES(playground ${CORE_LIBS})
