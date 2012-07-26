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
#include <PyOSchemaObject.h>

using namespace boost::python;

void register_xformop()
{

    // XformOp
    //
    class_<AbcG::XformOp>(
          "XformOp",
          "This class holds the data about a particular transform operation, but does "
          "not hold the actual data to calculate a 4x4 matrix.  It holds the type of "
          "operation (Translate, Rotate, Scale, Matrix), a hint about the type which "
          "can be interpreted by packages like Maya, and what particular parts of the "
          "operations can change over time.",
         init<const AbcG::XformOperationType, const AbcU::uint8_t>(
             ( arg( "type" ), arg( "hint" ) = 0 ),
             "Create an XformOp with default settings based on the type of operation "
             "and a hint about the type for applications like Maya." ) )
        .def( init<>("Create an empty XformOp" ) )
        .def( "getType",
              &AbcG::XformOp::getType,
              "Get the type of transform operation. (Translate, Rotate, Scale, "
              "Matrix)")
        .def( "setType",
              &AbcG::XformOp::setType,
              ( arg( "itype" ) ),
              "Set the type of transform operation. (Translate, Rotate, Scale, "
              "Matrix) Setting the type resets the hint, and sets all the channels "
              "to static.")
        .def( "getHint",
              &AbcG::XformOp::getHint,
              "Get the MatrixHint, RotateHint, TranslateHint, or ScaleHint to help "
              "disambiguate certain options that may have the same type.")
        .def( "setHint",
              &AbcG::XformOp::setHint,
              ( arg( "hint" ) ),
              "Set the hint, if it is an illegal value for the type, then the hint "
              "is set to the default, 0.")
        .def( "isXAnimated",
              &AbcG::XformOp::isXAnimated,
              "Returns whether the x component (index 0) is animated."
              "Only meaningful on read.")
        .def( "isYAnimated",
              &AbcG::XformOp::isYAnimated,
              "Returns whether the y component (index 1) is animated."
              "Only meaningful on read.")
        .def( "isZAnimated",
              &AbcG::XformOp::isZAnimated,
              "Returns whether the z component (index 2) is animated."
              "Only meaningful on read.")
        .def( "isAngleAnimated",
              &AbcG::XformOp::isAngleAnimated,
              "Returns whether the angle component (index 3) is animated. "
              "Since Scale and Translate do not have an angle component, "
              "false is returned for those types."
              "Only meaningful on read.")
        .def( "isChannelAnimated",
              &AbcG::XformOp::isChannelAnimated,
              ( arg( "index" ) ),
              "Returns whether a particular channel is animated. "
              "Scale and Translate only have 3 channels, Rotate has 4, and "
              "Matrix has 16.  Indices greater than the number of channels will "
              "return false."
              "Only meaningful on read.")
        .def( "getNumChannels",
              &AbcG::XformOp::getNumChannels,
              "Get the number of components that this operation has based on the type. "
              "Translate and Scale have 3, Rotate has 4 and Matrix has 16, and "
              "single-axis rotate ops (rotateX, rotateY, rotateZ) have 1.")
        .def( "getDefaultChannelValue",
              &AbcG::XformOp::getDefaultChannelValue,
              ( arg( "index" ) ),
              "For every channel, there's a default value.  Typically, for each op "
              "type, it's the same across channels. But matrix ops have different "
              "defaults to allow the identity matrix to be defaulted (most channels "
              "there are 0.0, the determinant channels are 1.0).")
        .def( "getChannelValue", 
              &AbcG::XformOp::getChannelValue,
              ( arg( "index" ) ) )
        .def( "setChannelValue",
              &AbcG::XformOp::setChannelValue,
              ( arg( "index" ), arg( "value" ) ),
              "Set a single channel; will throw if iIndex is greater than"
              "numchannels - 1.")
        .def( "setVector", 
              &AbcG::XformOp::setVector,
              ( arg( "vector" ) ) )
        .def( "setTranslate", 
              &AbcG::XformOp::setTranslate,
              ( arg( "trans" ) ) )
        .def( "setScale", 
              &AbcG::XformOp::setScale,
              ( arg( "scale" ) ) )
        .def( "setAxis", 
              &AbcG::XformOp::setAxis,
              ( arg( "axis" ) ) )
        .def( "setAngle", 
              &AbcG::XformOp::setAngle,
              ( arg( "degrees" ) ) )
        .def( "setMatrix", &AbcG::XformOp::setMatrix,
              ( arg( "matrix" ) ) )
        .def( "getVector", &AbcG::XformOp::getVector )
        .def( "getTranslate", &AbcG::XformOp::getTranslate )
        .def( "getScale", &AbcG::XformOp::getScale )
        .def( "getAxis", &AbcG::XformOp::getAxis )
        .def( "getAngle", &AbcG::XformOp::getAngle )
        .def( "getMatrix", &AbcG::XformOp::getMatrix )
        .def( "getXRotation", &AbcG::XformOp::getXRotation )
        .def( "getYRotation", &AbcG::XformOp::getYRotation )
        .def( "getZRotation", &AbcG::XformOp::getZRotation )
        .def( "isTranslateOp", &AbcG::XformOp::isTranslateOp )
        .def( "isScaleOp", &AbcG::XformOp::isScaleOp )
        .def( "isRotateOp", &AbcG::XformOp::isRotateOp )
        .def( "isMatrixOp", &AbcG::XformOp::isMatrixOp )
        .def( "isRotateXOp", &AbcG::XformOp::isRotateXOp )
        .def( "isRotateYOp", &AbcG::XformOp::isRotateYOp )
        .def( "isRotateZOp", &AbcG::XformOp::isRotateZOp )
        .def( "getOpEncoding", &AbcG::XformOp::getOpEncoding )
        ;
}
