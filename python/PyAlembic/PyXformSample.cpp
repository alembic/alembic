//-*****************************************************************************
//
// Copyright (c) 2012,
//  Sony Pictures Imageworks Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#include <Foundation.h>

using namespace boost::python;

//-*****************************************************************************
void register_xformsample()
{
    // Overloads for XformSample
    //
    size_t ( AbcG::XformSample::*addTranslateScaleOp )
        ( AbcG::XformOp, const Imath::V3d& )
            = &AbcG::XformSample::addOp;
    size_t ( AbcG::XformSample::*addRotateOp )
        ( AbcG::XformOp, const Imath::V3d&, double )
            = &AbcG::XformSample::addOp;
    size_t ( AbcG::XformSample::*addMatrixOp )
        ( AbcG::XformOp, const Imath::M44d& )
            = &AbcG::XformSample::addOp;
    size_t ( AbcG::XformSample::*addRotateAxisOp )
        ( AbcG::XformOp, double )
            = &AbcG::XformSample::addOp;
    size_t ( AbcG::XformSample::*addPresetOp )
        ( const AbcG::XformOp& )
            = &AbcG::XformSample::addOp;

    // Overload for __getitem__
    struct Overloads
    {
        typedef AbcG::XformSample Sample;

        static AbcG::XformOp &getOpByIndex( Sample& iSamp, size_t iIndex )
        {
            return iSamp[iIndex];
        }
    };

    // XformSample
    //
    class_<AbcG::XformSample>(
          "XformSample",
         init<>( "Creates an empty XformSample class object." ) )
        .def( "__getitem__",
              Overloads::getOpByIndex,
              ( arg( "index" ) ),
              return_internal_reference<>() )
        .def( "addOp",
              addTranslateScaleOp,
              ( arg( "transOrScaleOp" ), arg( "value" ) ),
              "Add translate or scale op. Returns the index of the op in its "
              "op-stack." )
        .def( "addOp",
              addRotateOp,
              ( arg( "rotateOp" ), arg( "axis" ), arg( "degrees" ) ),
              "Add rotate op. Returns the index of the op in its op-stack." )
        .def( "addOp",
              addMatrixOp,
              ( arg( "matrixOp" ), arg( "matrix" ) ),
              "Add matrix op. Returns the index of the op in its op-stack." )
        .def( "addOp",
              addRotateAxisOp,
              ( arg( "axisRotateOp" ), arg( "degrees" ) ),
              "Add rotateX, rotateY or rotateZ op." )
        .def( "addOp",
              addPresetOp,
              ( arg( "op" ) ),
              "Add an op with values already set on the op." )
        .def( "getOp",
              &AbcG::XformSample::getOp,
              ( arg( "index" ) ) )
        .def( "getNumOps",
              &AbcG::XformSample::getNumOps )
        .def( "getNumOpChannels",
              &AbcG::XformSample::getNumOpChannels )
        .def( "setInheritsXforms",
              &AbcG::XformSample::setInheritsXforms,
              ( arg( "inherits" ) ) )
        .def( "getInheritsXforms",
              &AbcG::XformSample::getInheritsXforms )
        .def( "setTranslation",
              &AbcG::XformSample::setTranslation,
              ( arg( "trans" ) ) )
        .def( "getTranslation",
              &AbcG::XformSample::getTranslation )
        .def( "setRotation",
              &AbcG::XformSample::setRotation,
              ( arg( "axis" ), arg( "degrees" ) ) )
        .def( "getAxis",
              &AbcG::XformSample::getAxis )
        .def( "getAngle",
              &AbcG::XformSample::getAngle )
        .def( "setXRotation",
              &AbcG::XformSample::setXRotation,
              ( arg( "degrees" ) ) )
        .def( "getXRotation",
              &AbcG::XformSample::getXRotation )
        .def( "setYRotation",
              &AbcG::XformSample::setYRotation,
              ( arg( "degrees" ) ) )
        .def( "getYRotation",
              &AbcG::XformSample::getYRotation )
        .def( "setZRotation",
              &AbcG::XformSample::setZRotation,
              ( arg( "degrees" ) ) )
        .def( "getZRotation",
              &AbcG::XformSample::getZRotation )
        .def( "setScale",
              &AbcG::XformSample::setScale,
              ( arg( "scale" ) ) )
        .def( "getScale",
              &AbcG::XformSample::getScale )
        .def( "setMatrix",
              &AbcG::XformSample::setMatrix,
              ( arg( "matrix" ) ) )
        .def( "getMatrix",
              &AbcG::XformSample::getMatrix )
        .def( "isTopologyEqual",
              &AbcG::XformSample::isTopologyEqual,
              ( arg( "sample" ) ),
              "Tests whether this sample has the same topology as 'sample'" )
        .def( "getIsTopologyFrozen",
              &AbcG::XformSample::getIsTopologyFrozen,
              "Has this Sample been used in a call to OXformSchema::set()" )
        .def( "reset", &AbcG::XformSample::reset )
        ;
}
