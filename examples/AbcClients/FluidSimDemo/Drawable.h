//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _FluidSimDemo_Drawable_h_
#define _FluidSimDemo_Drawable_h_

#include <FluidSimDemo/Foundation.h>
#include <FluidSimDemo/DrawContext.h>

namespace FluidSimDemo {

//-*****************************************************************************
// What can a drawable do?
// Obviously, it can draw!
// It can tell you a set of times that it has samples for.
class Drawable : private boost::noncopyable
{
public:
    Drawable() {}

    //! \brief Virtual destructor for abstract base class
    //! ...
    virtual ~Drawable() {}

    virtual std::string getName() const = 0;

    //! \brief This functions reads in a frame at a given time.
    //! \return Returns whether or not the object has data at the
    //! frame specified. If not, the object will be in an "empty" state.
    virtual void readInterpolatedFrame( float iFrame,
                                        const M44d &iParentXform ) = 0;

    //! \brief This function gets the bounding box at the frame that
    //! has been read.
    virtual Box3d getBounds() const = 0;

    //! \brief Draw the object (and its children) at the current frame.
    virtual void draw( const DrawContext & iCtx ) const = 0;
};

typedef boost::shared_ptr<Drawable> DrawablePtr;

} // End namespace FluidSimDemo

#endif
