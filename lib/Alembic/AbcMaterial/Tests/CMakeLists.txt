SET( TEST_LIBS
     AlembicAbcMaterial
     AlembicAbcGeom #used by tests but not a dependency of AlembicAbcMaterial
     AlembicAbc
     AlembicAbcCoreHDF5
     AlembicAbcCoreAbstract
     AlembicUtil
     ${ALEMBIC_HDF5_LIBS}
     ${ALEMBIC_ILMBASE_LIBS}
     ${CMAKE_THREAD_LIBS_INIT} ${Boost_THREAD_LIBRARY}
     ${ZLIB_LIBRARIES} ${EXTERNAL_MATH_LIBS} )


ADD_EXECUTABLE( AbcMaterial_WriteMaterialTest
                WriteMaterial.cpp
                )
TARGET_LINK_LIBRARIES( AbcMaterial_WriteMaterialTest ${TEST_LIBS} )
ADD_TEST( AbcMaterial_WriteMaterial_TEST AbcMaterial_WriteMaterialTest )


ADD_EXECUTABLE( AbcMaterial_MaterialFlattenTest
                MaterialFlattenTest.cpp PrintMaterial.cpp
                )
TARGET_LINK_LIBRARIES( AbcMaterial_MaterialFlattenTest ${TEST_LIBS} )
ADD_TEST( AbcMaterial_MaterialFlatten_TEST AbcMaterial_MaterialFlattenTest )



ADD_EXECUTABLE( AbcMaterial_MaterialHasATest
                WriteHasAMaterial.cpp PrintMaterial.cpp
                )
TARGET_LINK_LIBRARIES( AbcMaterial_MaterialHasATest ${TEST_LIBS} )
ADD_TEST( AbcMaterial_Material_TEST_HasA AbcMaterial_MaterialHasATest )




ADD_EXECUTABLE( AbcMaterial_MaterialAssignAndFlattenTest
                MaterialAssignAndFlattenTest.cpp PrintMaterial.cpp
                )
TARGET_LINK_LIBRARIES( AbcMaterial_MaterialAssignAndFlattenTest ${TEST_LIBS} )
ADD_TEST( AbcMaterial_TEST_MaterialAssignAndFlatten AbcMaterial_MaterialAssignAndFlattenTest )


ADD_EXECUTABLE( AbcMaterial_NetworkNodeTest
                NetworkNodeTest.cpp PrintMaterial.cpp
                )
TARGET_LINK_LIBRARIES( AbcMaterial_NetworkNodeTest ${TEST_LIBS} )
ADD_TEST( AbcMaterial_NetworkNodeTest AbcMaterial_NetworkNodeTest )



ADD_EXECUTABLE( AbcMaterial_WriteGeometryWithMaterials
                WriteGeometryWithMaterials.cpp 
                )
TARGET_LINK_LIBRARIES( AbcMaterial_WriteGeometryWithMaterials ${TEST_LIBS} )
ADD_TEST( AbcMaterial_WriteGeometryWithMaterials AbcMaterial_WriteGeometryWithMaterials )

