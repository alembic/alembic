#!/usr/bin/env bash

PREFIX=$1
if [ -z "$PREFIX" ]; then
    PREFIX="`pwd`/build"
fi
if [ ! -d $PREFIX ]; then
  mkdir $PREFIX
fi
 
cmake .. -DUSE_PYILMBASE=0 -DBOOST_ROOT=/servers/dwarf_server/DEVELOPMENT/DEVEL_LIBS/libs/boost \
        -DOPENEXR_ROOT=/servers/dwarf_server/DEVELOPMENT/LIBRARIES/EXR2.2/ \
        -DILMBASE_ROOT=/servers/dwarf_server/DEVELOPMENT/LIBRARIES/EXR2.2/ \
        -DHDF5_ROOT=/servers/dwarf_server/DEVELOPMENT/DEVEL_LIBS/libs/hdf5 -DUSE_MAYA=1 \
        -DCMAKE_INSTALL_PREFIX=$PREFIX -DALEMBIC_SHARED_LIBS=OFF\
        -DILMBASE_HALF_LIB=Half2Dwarf -DALEMBIC_MAYA_ROOT=/servers/Software/Autodesk/Maya/2014_ext_sp1 \
        -DALEMBIC_MAYA_INC_ROOT=/servers/Software/Autodesk/Maya/2014_ext_sp1/include \
        -DALEMBIC_MAYA_LIB_ROOT=/servers/Software/Autodesk/Maya/2014_ext_sp1/lib

retval=$?

if [ $retval -ne 0 ]; then
    echo "$retval ERRORS DURING CMAKE"
    exit $retval
fi

make -j12 && \
[ -f maya/AbcImport/AbcImport.so ] && \
make install && \
[ -f "$PREFIX/maya/plug-ins/AbcImport.so" ] || exit 1

