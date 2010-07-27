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

#ifndef _SimpleAbcViewer_ParentDrw_h_
#define _SimpleAbcViewer_ParentDrw_h_

#include "Foundation.h"
#include "Drawable.h"

namespace SimpleAbcViewer {

//-*****************************************************************************
//! \brief The parent drw is a base class for AssetDrw and TransformDrw
//! which allows the "looping over children" parts to be encapsulated
class ParentDrw : public Drawable
{
protected:
    ParentDrw();
    
public:
    virtual ~ParentDrw();

    // Returns whether any of the children are valid.
    virtual bool valid() const;

    // Returns whether any of the children read successfully.
    virtual bool readFrame( float i_frame );

    // Returns the total bounding box of all the children
    virtual Box3d getBounds() const;

    // Simply calls draw on all the children.
    virtual void draw( const DrawContext &ctx ) const;

protected:
    //! \brief Derived classes should call this function to
    //! set the Tako::TransformReader and to create children.
    void setTakoTransformReader( Atk::TransformReaderPtr i_xform );
    
    // Usage of containers and other "conceptual" classes
    // are always typedefed, rather than declared inline in the code.
    // This ensures that it is easy to change their type later on, if the
    // implementation calls for it.
    typedef std::vector<SharedDrawable> Children;

    // We have our transform, which is NULL by default.
    // Derived classes should set this using the setTakoTransformReader
    // call.
    Atk::TransformReaderPtr m_takoTransformReader;
    
    // We have a list of children.
    // Most of our operations we simple multiplex over the children.
    Children m_children;

    // This is the return status of the most recent "readFrame" call.
    bool m_readStatus;

    // The bounding box calculation can be expensive, so we simply
    // cache the current frame's bounding box.
    Box3d m_currentFrameBounds;
};

} // End namespace SimpleAbcViewer

#endif
