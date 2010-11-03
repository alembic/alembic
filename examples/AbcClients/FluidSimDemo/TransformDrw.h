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

#ifndef _FluidSimDemo_TransformDrw_h_
#define _FluidSimDemo_TransformDrw_h_

#include <FluidSimDemo/Foundation.h>
#include <FluidSimDemo/Drawable.h>

namespace FluidSimDemo {

//-*****************************************************************************
class CollisionObject;
typedef boost::shared_ptr<CollisionObject> CollisionObjectPtr;
typedef std::vector<CollisionObjectPtr> CollisionObjects;

//-*****************************************************************************
class TransformDrw : public Drawable
{
public:
    void init( Abc::ObjectReaderPtr iParentPtr,
               CollisionObjects &oColliders );

    template <class OBJECT>
    TransformDrw( OBJECT& iParentObj,
                  CollisionObjects &oColliders )
    {
        init( GetObjectReaderPtr( iParentObj ),
              oColliders );
    }

    virtual std::string getName() const
    {
        if ( m_ )
        {
            return m_parent
            return m_parent->name();
        }
        else
        {
            return "blah";
        }
    }

    // Returns whether any of the children read successfully.
    virtual void readInterpolatedFrame( float iFrame,
                                        const M44d &iParentXform );

    // Returns the total bounding box of all the children
    virtual Box3d getBounds() const;

    // Simply calls draw on all the children.
    virtual void draw( const DrawContext &ctx ) const;

protected:
    virtual void drawShape( const DrawContext &ctx ) const {}

    virtual void readShape( float frame,
                            const M44d &iParentXform )
    {
        m_shapeBounds.makeEmpty();
    }
    
    // Children typedef
    typedef std::vector<DrawablePtr> Children;

    // Our object.
    Abc::IObject m_object;

    // Our xform trait.
    Abc::ISimpleTransformTrait m_xformTrait;
    
    // We have a list of children.
    // Most of our operations we simple multiplex over the children.
    Children m_children;

    // Matrices
    M44d m_localToWorld;
    M44d m_localToParent;

    // The bounding box calculation can be expensive, so we simply
    // cache the current frame's bounding box.
    Box3d m_shapeBounds;
    Box3d m_childrenBounds;
    Box3d m_bounds;
};

//-*****************************************************************************
typedef boost::shared_ptr<TransformDrw> TransformDrwPtr;

} // End namespace FluidSimDemo

#endif
