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

#ifndef _SimpleAbcViewer_TransformDrw_h_
#define _SimpleAbcViewer_TransformDrw_h_

#include <SimpleAbcViewer/Foundation.h>
#include <SimpleAbcViewer/BaseTakoDrw.h>
#include <SimpleAbcViewer/InitStateObject.h>
#include <SimpleAbcViewer/CollisionObject.h>

namespace SimpleAbcViewer {

//-*****************************************************************************
class TransformDrw : public BaseTakoDrw
{
public:
    TransformDrw( TransformReaderPtr nodePtr,
                  InitStateObjects &oInitStates,
                  CollisionObjects &oColliders );
    
    virtual ~TransformDrw();

    // Returns whether any of the children read successfully.
    virtual void readInterpolatedFrame( float iFrame,
                                        const M44d &iParentXform );

    // Returns the total bounding box of all the children
    virtual Box3d getBounds() const;

    // Simply calls draw on all the children.
    virtual void draw( const DrawContext &ctx ) const;

protected:
    // Children typedef
    typedef std::vector<SharedDrawable> Children;

    // We have our transform, which is an upcast of the base node ptr.
    TransformReaderPtr m_takoTransformReader;
    
    // We have a list of children.
    // Most of our operations we simple multiplex over the children.
    Children m_children;

    // Matrices
    M44d m_localToWorld;
    M44d m_localToParent;

    // The bounding box calculation can be expensive, so we simply
    // cache the current frame's bounding box.
    Box3d m_childrenBounds;
    Box3d m_bounds;
};

//-*****************************************************************************
typedef boost::shared_ptr<TransformDrw> TransformDrwPtr;

} // End namespace SimpleAbcViewer

#endif
