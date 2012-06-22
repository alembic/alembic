SET( TEST_LIBS
     AlembicAbcCollection
     AlembicAbc
     AlembicAbcCoreHDF5
     AlembicAbcCoreAbstract
     AlembicUtil
     ${ALEMBIC_HDF5_LIBS}
     ${ALEMBIC_ILMBASE_LIBS}
     ${CMAKE_THREAD_LIBS_INIT}
     ${ZLIB_LIBRARIES} ${EXTERNAL_MATH_LIBS} )

ADD_EXECUTABLE( AbcCollection_CollectionTest
                CollectionTest.cpp
                )
TARGET_LINK_LIBRARIES( AbcCollection_CollectionTest ${TEST_LIBS} )
ADD_TEST( AbcCollection_Collection_TEST AbcCollection_CollectionTest )
