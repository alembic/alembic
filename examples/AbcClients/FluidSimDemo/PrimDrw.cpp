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

#include <FluidSimDemo/PrimDrw.h>

namespace FluidSimDemo {

//-*****************************************************************************
PrimDrw::PrimDrw( IObjectPtr iObj,
                  CollisionObjects &cobjs, ShapeType iStype )
  : TransformDrw( MakeParent( iObj ),  cobjs ),
    m_shapeType( iStype )
{
    m_shapeBounds = Box3d( V3d( -1.0, -1.0, -1.0 ), V3d( 1.0, 1.0, 1.0 ) );
}

//-*****************************************************************************
void PrimDrw::drawShape( const DrawContext & iCtx ) const
{
    if ( !m_drawHelper )
    {
        m_drawHelper = MakeDrawHelperPtr();
    }

#if 1
    // Get the matrix
    GLdouble currentMatrix[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, currentMatrix );
    
    // Basically, we want to load our matrix into the thingy.
    // We don't use the OpenGL transform stack because we have deep
    // deep hierarchy that exhausts the stack depth quickly.
    M44d concatXform = m_localToWorld * iCtx.cameraTransform();

    // Load this xform.
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( ( const GLdouble * )&( concatXform[0][0] ) );
#endif

    // Draw shape.
    switch ( m_shapeType )
    {
    case kSphere: m_drawHelper->drawSphere(); break;
    case kCube: m_drawHelper->drawCube(); break;
    case kCylinder: m_drawHelper->drawCylinder(); break;
    case kCone: m_drawHelper->drawCone(); break;
    default:
    case kPlane: break;
    };

#if 0
    // Reset matrix?
    // Probably not necessary
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( currentMatrix );
#endif
}

} // End namespace FluidSimDemo

