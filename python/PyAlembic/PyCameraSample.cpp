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
void register_camerasample()
{
    // Overloads for CameraSample
    //
    struct SampleOverloads
    {
        typedef AbcG::CameraSample Sample;

        static AbcG::FilmBackXformOp &getOpByIndex( Sample& iSamp,
                                                    size_t iIndex )
        {
            return iSamp[iIndex];
        }

        static dict getScreenWindow( Sample& iSamp )
        {
            double top, bottom, left, right;
            iSamp.getScreenWindow( top, bottom, left, right );

            dict window;
            window["top"] = top;
            window["bottom"] = bottom;
            window["left"] = left;
            window["right"] = right;

            return window;
        }
    };

    // CameraSample
    //
    class_<AbcG::CameraSample>(
          "CameraSample",
          "The CameraSample class is a camera sample reader/writer.",
          init<>() )
        .def( init<double, double, double, double>(
                   ( arg( "top" ), arg( "bottom" ), arg( "left" ), 
                     arg( "right" ) ) ) )
        .def( "__getitem__", 
              SampleOverloads::getOpByIndex, 
              ( arg( "index" ) ),
              return_internal_reference<>() )
        .def( "getScreenWindow", 
              SampleOverloads::getScreenWindow )
        .def( "getFocalLength", 
              &AbcG::CameraSample::getFocalLength,
              "get the camera focal length in millimeters." )
        .def( "setFocalLength", 
              &AbcG::CameraSample::setFocalLength,
              ( arg( "length" ) ),
              "set the horizontal camera film back in centimeters" )
        .def( "getHorizontalAperture", 
              &AbcG::CameraSample::getHorizontalAperture,
              "get the horizontal camera film back in centimeters" )
        .def( "setHorizontalAperture", 
              &AbcG::CameraSample::setHorizontalAperture,
              ( arg( "aperture" ) ),
              "set the horizontal camera film back in centimeters" )
        .def( "getHorizontalFilmOffset", 
              &AbcG::CameraSample::getHorizontalFilmOffset,
              "get the horizontal film back offset in centimeters" )
        .def( "setHorizontalFilmOffset", 
              &AbcG::CameraSample::setHorizontalFilmOffset,
              ( arg( "offset" ) ),
              "set the horizontal film back offset in centimeters" )
        .def( "getVerticalAperture", 
              &AbcG::CameraSample::getVerticalAperture,
              "get the vertical camera film back in centimeters" )
        .def( "setVerticalAperture", 
              &AbcG::CameraSample::setVerticalAperture,
              ( arg( "aperture" ) ),
              "set the vertical camera film back in centimeters" )
        .def( "getVerticalFilmOffset", 
              &AbcG::CameraSample::getVerticalFilmOffset,
              "get the vertical film back offset in centimeters" )
        .def( "setVerticalFilmOffset", 
              &AbcG::CameraSample::setVerticalFilmOffset,
              ( arg( "offset" ) ),
              "set the vertical film back offset in centimeters" )
        .def( "getLensSqueezeRatio", 
              &AbcG::CameraSample::getLensSqueezeRatio,
              "get the amount the camera's lens compresses the image horizontally "
              "(width / height aspect ratio)" )
        .def( "setLensSqueezeRatio", 
              &AbcG::CameraSample::setLensSqueezeRatio,
              ( arg( "ratio" ) ),
              "set the amount the camera's lens compresses the image horizontally "
              "(width / height aspect ratio)" )
        .def( "getOverScanLeft", 
              &AbcG::CameraSample::getOverScanLeft,
              "get over scan fractional percentage for the left part of the "
              "screen window" )
        .def( "setOverScanLeft", 
              &AbcG::CameraSample::setOverScanLeft,
              ( arg( "value" ) ),
              "set over scan fractional percentage for the left part of the "
              "screen window" )
        .def( "getOverScanRight", 
              &AbcG::CameraSample::getOverScanRight,
              "get over scan fractional percentage for the right part of the "
              "screen window" )
        .def( "setOverScanRight", 
              &AbcG::CameraSample::setOverScanRight,
              ( arg( "value" ) ),
              "set over scan fractional percentage for the right part of the "
              "screen window" )
        .def( "getOverScanTop", 
              &AbcG::CameraSample::getOverScanTop,
              "get over scan fractional percentage for the top part of the "
              "screen window" )
        .def( "setOverScanTop", 
              &AbcG::CameraSample::setOverScanTop,
              ( arg( "value" ) ),
              "get over scan fractional percentage for the top part of the "
              "screen window" )
        .def( "getOverScanBottom", 
              &AbcG::CameraSample::getOverScanBottom,
              "get over scan fractional percentage for the bottom part of the "
              "screen window" )
        .def( "setOverScanBottom", 
              &AbcG::CameraSample::setOverScanBottom,
              ( arg( "value" ) ),
              "set over scan fractional percentage for the bottom part of the "
              "screen window" )
        .def( "getFStop", 
              &AbcG::CameraSample::getFStop,
              "set the f-stop (focal length divided by 'effective' lens diameter)" )
        .def( "setFStop", 
              &AbcG::CameraSample::setFStop,
              ( arg( "fstop" ) ),
              "set the f-stop (focal length divided by 'effective' lens diameter)" )
        .def( "getFocusDistance", 
              &AbcG::CameraSample::getFocusDistance,
              "get the distance from the camera to the object being focused on "
              "in centimeters" )
        .def( "setFocusDistance", 
              &AbcG::CameraSample::setFocusDistance,
              ( arg( "distance" ) ),
              "set the distance from the camera to the object being focused on "
              "in centimeters" )
        .def( "getShutterOpen", 
              &AbcG::CameraSample::getShutterOpen,
              "get the frame relative shutter open time in seconds" )
        .def( "setShutterOpen", 
              &AbcG::CameraSample::setShutterOpen,
              ( arg( "shutterOpen" ) ),
              "set the frame relative shutter open time in seconds" )
        .def( "getShutterClose", 
              &AbcG::CameraSample::getShutterClose,
              "get the frame relative shutter close time in seconds" )
        .def( "setShutterClose", 
              &AbcG::CameraSample::setShutterClose,
              ( arg( "shutterClose" ) ),
              "set the frame relative shutter open time in seconds" )
        .def( "getNearClippingPlane", 
              &AbcG::CameraSample::getNearClippingPlane,
              "get the distance from the camera to the near clipping plane in "
              "centimeters" )
        .def( "setNearClippingPlane", 
              &AbcG::CameraSample::setNearClippingPlane,
              ( arg( "nearClippingPlane" ) ),
              "set the distance from the camera to the near clipping plane in "
              "centimeters" )
        .def( "getFarClippingPlane", 
              &AbcG::CameraSample::getFarClippingPlane,
              "get the distance from the camera to the far clipping plane in "
              "centimeters" )
        .def( "setFarClippingPlane", 
              &AbcG::CameraSample::setFarClippingPlane,
              ( arg( "farClippingPlane" ) ),
              "set the distance from the camera to the near clipping plane in "
              "centimeters" )
        .def( "getChildBounds", &AbcG::CameraSample::getChildBounds )
        .def( "setChildBounds", 
              &AbcG::CameraSample::setChildBounds,
              ( arg( "childBounds" ) ) )
        .def( "getCoreValue", 
              &AbcG::CameraSample::getCoreValue,
              ( arg( "index" ) ),
              "helper function for getting one of the 16 core, non film back xform op "
              "related values" )
        .def( "getFieldOfView", 
              &AbcG::CameraSample::getFieldOfView,
              "calculated the field of view in degrees" )
        .def( "addOp", 
              &AbcG::CameraSample::addOp,
              ( arg( "op" ) ),
              "add an op and return the index of the op in its op-stack" )
        .def( "getOp", 
              &AbcG::CameraSample::getOp,
              ( arg( "index" ) ) )
        .def( "getFilmBackMatrix", 
              &AbcG::CameraSample::getFilmBackMatrix,
              "Returns the concatenated 3x3 film back matrix" )
        .def( "getNumOps", &AbcG::CameraSample::getNumOps )
        .def( "getNumOpChannels", &AbcG::CameraSample::getNumOpChannels )
        .def( "reset", &AbcG::CameraSample::reset )
        ;
}
