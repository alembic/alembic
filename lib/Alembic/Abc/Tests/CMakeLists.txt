##-*****************************************************************************
##
## Copyright (c) 2009-2013,
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


SET( TEST_LIBS
     AlembicAbcCoreFactory
     AlembicAbc
     AlembicAbcCoreHDF5
     AlembicAbcCoreOgawa
     AlembicAbcCoreAbstract
     AlembicOgawa
     AlembicUtil
     ${ALEMBIC_HDF5_LIBS}
     ${ALEMBIC_ILMBASE_LIBS}
     ${CMAKE_THREAD_LIBS_INIT} ${Boost_THREAD_LIBRARY}
     ${ZLIB_LIBRARIES} ${EXTERNAL_MATH_LIBS} )


#-******************************************************************************

ADD_EXECUTABLE( Abc_Test1 test1.cpp )
TARGET_LINK_LIBRARIES( Abc_Test1 ${TEST_LIBS} )

ADD_EXECUTABLE( Abc_ObjectsAndProperties_Test2 ObjectsAndPropertiesTest.cpp )
TARGET_LINK_LIBRARIES( Abc_ObjectsAndProperties_Test2 ${TEST_LIBS} )

ADD_TEST( Abc_TEST1 Abc_Test1 )
ADD_TEST( Abc_Objects_and_Properties_TEST2 Abc_ObjectsAndProperties_Test2 )

#-******************************************************************************

ADD_EXECUTABLE( Abc_CompileTests CompileTest.cpp )
TARGET_LINK_LIBRARIES( Abc_CompileTests ${TEST_LIBS} )
ADD_TEST( Abc_Compile_TEST Abc_CompileTests )

#-******************************************************************************

ADD_EXECUTABLE( Abc_ObjectTests ObjectTests.cpp )
TARGET_LINK_LIBRARIES( Abc_ObjectTests ${TEST_LIBS} )
ADD_TEST( Abc_Object_TEST Abc_ObjectTests )

#-******************************************************************************

ADD_EXECUTABLE( Abc_PropertyTests PropertyTests.cpp )
TARGET_LINK_LIBRARIES( Abc_PropertyTests ${TEST_LIBS} )
ADD_TEST( Abc_Property_TEST Abc_PropertyTests )

#-******************************************************************************

ADD_EXECUTABLE( Abc_UniformPropertyTest UniformPropertyTest.cpp )
TARGET_LINK_LIBRARIES( Abc_UniformPropertyTest ${TEST_LIBS} )
ADD_TEST( Abc_Uniform_Property_TEST Abc_UniformPropertyTest )

#-******************************************************************************

ADD_EXECUTABLE( Abc_IdentityPropertyTest IdentityPropertyTest.cpp )
TARGET_LINK_LIBRARIES( Abc_IdentityPropertyTest ${TEST_LIBS} )
ADD_TEST( Abc_Identity_Property_TEST Abc_IdentityPropertyTest )

#-******************************************************************************

ADD_EXECUTABLE( Abc_ArrayPropertyTest ArrayPropertyTest.cpp )
TARGET_LINK_LIBRARIES( Abc_ArrayPropertyTest ${TEST_LIBS} )
ADD_TEST( Abc_Array_Property_TEST Abc_ArrayPropertyTest )

#-******************************************************************************

ADD_EXECUTABLE( Abc_InstanceTest InstanceTest.cpp )
TARGET_LINK_LIBRARIES( Abc_InstanceTest ${TEST_LIBS} )
ADD_TEST( Abc_Instance_TEST Abc_InstanceTest )

#-******************************************************************************

ADD_EXECUTABLE( Abc_ArchiveTest ArchiveTest.cpp )
TARGET_LINK_LIBRARIES( Abc_ArchiveTest ${TEST_LIBS} )
ADD_TEST( Abc_Archive_TEST Abc_ArchiveTest )

#-******************************************************************************

ADD_EXECUTABLE( Abc_CyclicPropertyTest CyclicPropertyTest.cpp )
TARGET_LINK_LIBRARIES( Abc_CyclicPropertyTest ${TEST_LIBS} )
ADD_TEST( Abc_Cyclic_Property_TEST Abc_CyclicPropertyTest )

#-******************************************************************************

ADD_EXECUTABLE( Abc_ParentingTest ParentingTest.cpp )
TARGET_LINK_LIBRARIES( Abc_ParentingTest ${TEST_LIBS} )
ADD_TEST( Abc_Parenting_TEST Abc_ParentingTest )

#-******************************************************************************

ADD_EXECUTABLE( Abc_OctessenceBug17 OctessenceBug17.cpp )
TARGET_LINK_LIBRARIES( Abc_OctessenceBug17 ${TEST_LIBS} )
ADD_TEST( Abc_OctessenceBug17_TEST Abc_OctessenceBug17 )

#-******************************************************************************

ADD_EXECUTABLE( Abc_OctessenceBug16 OctessenceBug16.cpp )
TARGET_LINK_LIBRARIES( Abc_OctessenceBug16 ${TEST_LIBS} )
ADD_TEST( Abc_OctessenceBug16_TEST Abc_OctessenceBug16 )

#-******************************************************************************

ADD_EXECUTABLE( Abc_CacheControlTest CacheControlTest.cpp )
TARGET_LINK_LIBRARIES( Abc_CacheControlTest ${TEST_LIBS} )
ADD_TEST( Abc_CacheControl_TEST Abc_CacheControlTest )

#-******************************************************************************

ADD_EXECUTABLE( Abc_RedundantDataPathsTest RedundantDataTest.cpp )
TARGET_LINK_LIBRARIES( Abc_RedundantDataPathsTest ${TEST_LIBS} )
ADD_TEST( Abc_RedundantDataPaths_TEST Abc_RedundantDataPathsTest )
