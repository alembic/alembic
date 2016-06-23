#!/usr/bin/env bash

PREFIX=$1

####	MAYA 2014    ####
MAYA_PREFIX=$PREFIX/maya2014
MAYA_DIR=/servers/Software/Autodesk/Maya/2014_ext_sp1
MAYA_INC_DIR=$MAYA_DIR/include
MAYA_LIB_DIR=$MAYA_DIR/lib

../build_version.sh $MAYA_PREFIX $MAYA_DIR $MAYA_INC_DIR $MAYA_LIB_DIR

####	MAYA 2015    ####
MAYA_PREFIX=$PREFIX/maya2015
MAYA_DIR=/servers/Software/Autodesk/Maya/2015
MAYA_INC_DIR=$MAYA_DIR/include
MAYA_LIB_DIR=$MAYA_DIR/lib

../build_version.sh $MAYA_PREFIX $MAYA_DIR $MAYA_INC_DIR $MAYA_LIB_DIR

####	MAYA 2016    ####
MAYA_PREFIX=$PREFIX/maya2016
MAYA_DIR=/servers/Software/Autodesk/Maya/2016_ext1_sp6
MAYA_INC_DIR=$MAYA_DIR/include
MAYA_LIB_DIR=$MAYA_DIR/lib

../build_version.sh $MAYA_PREFIX $MAYA_DIR $MAYA_INC_DIR $MAYA_LIB_DIR

