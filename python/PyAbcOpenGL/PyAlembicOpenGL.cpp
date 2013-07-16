//-*****************************************************************************
//
// Copyright (c) 2013,
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
#include <AbcOpenGL/All.h>

using namespace boost::python;

//-*****************************************************************************
void register_opengl()
{

    // setMaterials
    //
    def("setMaterials", 
        &AbcOpenGL::setMaterials,
        ( arg( "o" ) = 1.0, arg( "negMatrix" ) = false ) )
    ;

    // GLCamera overloads
    //
    void ( AbcOpenGL::GLCamera::*setSizeWidthHeight )( int w, int h )
        = &AbcOpenGL::GLCamera::setSize;

    // GLCamera
    //
    class_<AbcOpenGL::GLCamera>(
          "GLCamera",
          "Wraps AbcOpenGL GLCamera class",
          init<>() )
        .def( "setSize",
              setSizeWidthHeight,
              ( arg( "w" ), arg( "h" ) ) )
        .def( "apply",
              &AbcOpenGL::GLCamera::apply )
        .def( "frame",
              &AbcOpenGL::GLCamera::frame,
              ( arg( "bounds" ) ) )
        .def( "lookAt",
              &AbcOpenGL::GLCamera::lookAt,
              ( arg( "eye" ), arg( "at" ) ) )
        .def( "track",
              &AbcOpenGL::GLCamera::track,
              ( arg( "point" ) ) )
        .def( "dolly",
              &AbcOpenGL::GLCamera::dolly,
              ( arg( "point" ), arg( "speed" ) = 5.0 ) )
        .def( "rotate",
              &AbcOpenGL::GLCamera::rotate,
              ( arg( "point" ), arg( "speed" ) = 400.0 ) )
        .def( "width",
              &AbcOpenGL::GLCamera::width )
        .def( "height",
              &AbcOpenGL::GLCamera::height )
        .def( "centerOfInterest",
              &AbcOpenGL::GLCamera::centerOfInterest )
        .def( "setCenterOfInterest",
              &AbcOpenGL::GLCamera::setCenterOfInterest,
              ( arg( "center" ) ) )
        .def( "fovy",
              &AbcOpenGL::GLCamera::fovy )
        .def( "setFovy",
              &AbcOpenGL::GLCamera::setFovy,
              ( arg( "fvy" ) ) )
        .def( "transform",
              &AbcOpenGL::GLCamera::transform )
        .def( "translation",
              &AbcOpenGL::GLCamera::translation,
              return_value_policy<copy_const_reference>() )
        .def( "setTranslation",
              &AbcOpenGL::GLCamera::setTranslation,
              ( arg( "trans" ) ) )
        .def( "rotation",
              &AbcOpenGL::GLCamera::rotation,
              return_value_policy<copy_const_reference>() )
        .def( "setRotation",
              &AbcOpenGL::GLCamera::setRotation,
              ( arg( "rot" ) ) )
        .def( "scale",
              &AbcOpenGL::GLCamera::scale,
              return_value_policy<copy_const_reference>() )
        .def( "setScale",
              &AbcOpenGL::GLCamera::setScale,
              ( arg( "scale" ) ) )
        .def( "autoSetClippingPlanes",
              &AbcOpenGL::GLCamera::autoSetClippingPlanes,
              ( arg( "bounds" ) ) )
        .def( "clippingPlanes",
              &AbcOpenGL::GLCamera::clippingPlanes )
        .def( "setClippingPlanes",
              &AbcOpenGL::GLCamera::setClippingPlanes,
              ( arg( "near" ), arg( "far" ) ) )
        ;

    // SceneWrapper
    //
    class_<AbcOpenGL::SceneWrapper>(
          "SceneWrapper",
          "Wraps AbcOpenGL Scene class",
          no_init )
        .def( init<const std::string, bool> (
              ( arg( "fileName" ), arg( "verbose" ) = false ),
              "doc"))
        .def( "draw",
              &AbcOpenGL::SceneWrapper::draw )
        .def( "getMinTime",
              &AbcOpenGL::SceneWrapper::getMinTime )
        .def( "getMaxTime",
              &AbcOpenGL::SceneWrapper::getMaxTime )
        .def( "getCurrentTime",
              &AbcOpenGL::SceneWrapper::getCurrentTime )
        .def( "isConstant",
              &AbcOpenGL::SceneWrapper::isConstant )
        .def( "setTime",
              &AbcOpenGL::SceneWrapper::setTime,
              ( arg( "time" ) ) )
        .def( "playForward",
              &AbcOpenGL::SceneWrapper::playForward,
              ( arg( "fps" ) ) )
        .def( "bounds",
              &AbcOpenGL::SceneWrapper::bounds )
        .def( "frame",
              &AbcOpenGL::SceneWrapper::frame,
              ( arg( "bounds" ) ) )
        ;
}
