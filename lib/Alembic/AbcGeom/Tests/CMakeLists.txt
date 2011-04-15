##-*****************************************************************************
##
## Copyright (c) 2009-2010, Industrial Light & Magic,
##   a division of Lucasfilm Entertainment Company Ltd.
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


SET( TEST_LIBS
     AlembicAbcGeom
     AlembicAbc
     AlembicAbcCoreHDF5
     AlembicAbcCoreAbstract
     AlembicMD5Hash
     AlembicUtil
     ${ALEMBIC_HDF5_LIBS}
     ${ALEMBIC_ILMBASE_LIBS}
     ${CMAKE_THREAD_LIBS_INIT}
     ${ZLIB_LIBRARIES} ${EXTERNAL_MATH_LIBS} )

#-******************************************************************************
ADD_EXECUTABLE( AbcGeom_PolyMeshTest
		MeshData.h
		MeshData.cpp
		PolyMeshTest.cpp )
TARGET_LINK_LIBRARIES( AbcGeom_PolyMeshTest ${TEST_LIBS} )
ADD_TEST( AbcGeom_PolyMeshTEST AbcGeom_PolyMeshTest )

#-******************************************************************************
ADD_EXECUTABLE( AbcGeom_PointsTest
		PointsTest.cpp )
TARGET_LINK_LIBRARIES( AbcGeom_PointsTest ${TEST_LIBS} )
ADD_TEST( AbcGeom_PointsTEST AbcGeom_PointsTest )

#-******************************************************************************
ADD_EXECUTABLE( AbcGeom_SubDTest
		MeshData.h
		MeshData.cpp
		SubDTest.cpp )
TARGET_LINK_LIBRARIES( AbcGeom_SubDTest ${TEST_LIBS} )
ADD_TEST( AbcGeom_SubDTEST AbcGeom_SubDTest )

##-*****************************************************************************
ADD_EXECUTABLE( AbcGeom_SchemaMatchingTest
		SchemaMatchingTest.cpp )
TARGET_LINK_LIBRARIES( AbcGeom_SchemaMatchingTest ${TEST_LIBS} )
ADD_TEST( AbcGeom_SchemaMatching_TEST AbcGeom_SchemaMatchingTest )

#-******************************************************************************
ADD_EXECUTABLE( AbcGeom_XformTest
		XformTests.cpp )
TARGET_LINK_LIBRARIES( AbcGeom_XformTest  ${TEST_LIBS} )
ADD_TEST( AbcGeom_Xform_TEST  AbcGeom_XformTest  )

#-******************************************************************************
ADD_EXECUTABLE( AbcGeom_XformTest2
		XformTests2.cpp )
TARGET_LINK_LIBRARIES( AbcGeom_XformTest2  ${TEST_LIBS} )
ADD_TEST( AbcGeom_Xform2_TEST  AbcGeom_XformTest2  )

##-*****************************************************************************
ADD_EXECUTABLE( AbcGeom_TransformingMeshTest MeshData.cpp
                TransformingMeshTest.cpp )
TARGET_LINK_LIBRARIES( AbcGeom_TransformingMeshTest ${TEST_LIBS} )
ADD_TEST( AbcGeom_TransformingMeshTest_TEST AbcGeom_TransformingMeshTest )

#-*****************************************************************************
ADD_EXECUTABLE( AbcGeom_CurvesTest
                CurvesData.h
                CurvesData.cpp
                CurvesTest.cpp )
TARGET_LINK_LIBRARIES( AbcGeom_CurvesTest ${TEST_LIBS} )
ADD_TEST( AbcGeom_Curves_TEST AbcGeom_CurvesTest )


