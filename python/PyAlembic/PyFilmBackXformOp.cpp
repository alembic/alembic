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
void register_filmbackxformop()
{

    // FilmBackXformOp
    //
    class_<AbcG::FilmBackXformOp>(
          "FilmBackXformOp",
          "This class holds the data about a particular transform operation, but does "
          "not hold the actual data to calculate a 3x3 matrix.  It holds the type of "
          "operation (Translate, Scale, Matrix), a hint about the type which "
          "can be interpreted by packages like Maya, and what particular parts of the "
          "operations can change over time.",
         init<const AbcG::FilmBackXformOperationType, const std::string&>(
             ( arg( "type" ), arg( "hint" ) = 0 ),
             "Create an FilmBackXformOp with default settings based on the type of operation "
             "and a hint about the type for applications like Maya." ) )
        .def( init<>("Create an empty FilmBackXformOp" ) )
        .def( "getType",
              &AbcG::FilmBackXformOp::getType,
              "Get the type of transform operation. (Translate, Scale, "
              "Matrix)")
        .def( "getHint",
              &AbcG::FilmBackXformOp::getHint,
              "Get the Hint to help "
              "disambiguate certain options that may have the same type.")
        .def( "getTypeHint",
              &AbcG::FilmBackXformOp::getHint,
              "Get the type and hint, where the first character is the type"
              "(t for translate, s for scale, m for matrix) and the rest of the"
              "returned string is the optional hint value." )
        .def( "getNumChannels",
              &AbcG::FilmBackXformOp::getNumChannels,
              "Get the number of components that this operation has based on the type. "
              "Translate and Scale have 2 and Matrix has 9." )
        .def( "getChannelValue", 
              &AbcG::FilmBackXformOp::getChannelValue,
              ( arg( "index" ) ),
              "Get a single channel, 0 will be returned if iIndex is greater than"
              "numChannels - 1" )
        .def( "setChannelValue",
              &AbcG::FilmBackXformOp::setChannelValue,
              ( arg( "index" ), arg( "value" ) ),
              "Set a single channel; nothing will be set if iIndex is greater than"
              "numchannels - 1.")
        .def( "setTranslate", 
              &AbcG::FilmBackXformOp::setTranslate,
              ( arg( "trans" ) ) )
        .def( "setScale", 
              &AbcG::FilmBackXformOp::setScale,
              ( arg( "scale" ) ) )
        .def( "setMatrix", &AbcG::FilmBackXformOp::setMatrix,
              ( arg( "matrix" ) ) )
        .def( "getTranslate", &AbcG::FilmBackXformOp::getTranslate )
        .def( "getScale", &AbcG::FilmBackXformOp::getScale )
        .def( "getMatrix", &AbcG::FilmBackXformOp::getMatrix )
        .def( "isTranslateOp", &AbcG::FilmBackXformOp::isTranslateOp )
        .def( "isScaleOp", &AbcG::FilmBackXformOp::isScaleOp )
        .def( "isMatrixOp", &AbcG::FilmBackXformOp::isMatrixOp )
        ;
}
