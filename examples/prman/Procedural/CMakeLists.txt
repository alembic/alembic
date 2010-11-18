
PROJECT( ALEMBIC_PRMAN_PROCEDURAL_MODULE )


IF( ${ALEMBIC_PRMAN_FOUND} )

SET( H_FILES 
  ArbAttrUtil.h
  PathUtil.h
  ProcArgs.h
  SampleUtil.h
  SubDTags.h
  WriteGeo.h
  )

SET( CXX_FILES
  ArbAttrUtil.cpp
  PathUtil.cpp
  ProcArgs.cpp
  ProcMain.cpp
  SampleUtil.cpp
  SubDTags.cpp
  WriteGeo.cpp
)

SET( CORE_LIBS 
  AlembicAbcGeom
  AlembicAbc
  AlembicAbcCoreHDF5
  AlembicAbcCoreAbstract
  AlembicMD5Hash
  AlembicUtil )

ADD_PRMAN_CXX_PLUGIN( Procedural ProcMain.cpp ${H_FILES} ${CXX_FILES} )
TARGET_LINK_LIBRARIES( Procedural
  ${CORE_LIBS}
  ${ALEMBIC_HDF5_LIBS}
  ${ALEMBIC_ILMBASE_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${ZLIB_LIBRARIES}
)




ENDIF()
