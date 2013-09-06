//-*****************************************************************************
//
// Copyright (c) 2009-2013,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#ifndef _AbcOpenGL_Drawable_h_
#define _AbcOpenGL_Drawable_h_

#include "Foundation.h"
#include "DrawContext.h"

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
//! What can a drawable do?
//! Obviously, it can draw!
//! It can tell you a set of times that it has samples for.
class Drawable : private Alembic::Util::noncopyable
{
public:
    Drawable() {}

    //! Virtual destructor for abstract base class
    //! ...
    virtual ~Drawable() {}

    //! Get the min time.
    //! ...
    virtual chrono_t getMinTime() = 0;

    //! Get the max time.
    //! ...
    virtual chrono_t getMaxTime() = 0;

    //! This function returns whether or not the
    //! drawable is in a valid state. As we read frames into
    //! the drawables, it's possible for some drawables to become invalid.
    virtual bool valid() = 0;

    //! This functions sets the drawable and all its children
    //! to a new time, in seconds.
    virtual void setTime( chrono_t iSeconds ) = 0;

    //! This function gets the bounding box at the
    //! currently set time.
    virtual Box3d getBounds() = 0;

    //! This function is for getting bounds which we don't want multiplied
    //! by the local and ancestor transforms
    virtual Box3d getNonInheritedBounds() { return Box3d(); }

    //! Draw the object (and its children) at the current frame.
    //! ...
    virtual void draw( const DrawContext & iCtx ) = 0;
};

//-*****************************************************************************
typedef Alembic::Util::shared_ptr<Drawable> DrawablePtr;
typedef std::vector<DrawablePtr> DrawablePtrVec;

} // End namespace ABCOPENGL_VERSION_NS

using namespace ABCOPENGL_VERSION_NS;

} // End namespace AbcOpenGL

#endif
