//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _FluidSimDemo_PrimDrw_h_
#define _FluidSimDemo_PrimDrw_h_

#include <FluidSimDemo/TransformDrw.h>
#include <FluidSimDemo/DrawHelper.h>

namespace FluidSimDemo {

//-*****************************************************************************
enum ShapeType
{
    kSphere,
    kCube,
    kCylinder,
    kCone,
    kPlane
};

//-*****************************************************************************
class PrimDrw : public TransformDrw
{
public:
    PrimDrw( IObjectPtr iObj, CollisionObjects &cobjs, ShapeType iStype );
    
    //-*************************************************************************
    // New functions.
    ShapeType getShapeType() const { return m_shapeType; }
    Box3d getBoundsWorld() const { return m_boundsWorld; }

protected:
    void drawShape( const DrawContext &iCtx ) const;

    void readShape( float frame,
                    const M44d &iParentToWorld )
    {
        m_shapeBounds.min = Abc::V3d( -1, -1, -1 );
        m_shapeBounds.max = Abc::V3d( 1, 1, 1 );

        m_boundsWorld = Imath::transform( m_shapeBounds,
                                          iParentToWorld );
    }
    
    ShapeType m_shapeType;
    mutable DrawHelperPtr m_drawHelper;

    Box3d m_boundsWorld;
};

} // End namespace FluidSimDemo

#endif
