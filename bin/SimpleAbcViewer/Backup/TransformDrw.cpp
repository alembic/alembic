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

#include "TransformDrw.h"

namespace SimpleAbcViewer {

//-*****************************************************************************
TransformDrw::TransformDrw( Atk::TransformReaderPtr xform )
  : ParentDrw()
{
    // Fill in our children.
    setTakoTransformReader( xform );
    
    // Set transform to identity.
    m_currentFrameTransform.makeIdentity();
    
    // Set bounds to empty.
    m_transformedCurrentFrameBounds.makeEmpty();
}

//-*****************************************************************************
TransformDrw::~TransformDrw()
{
    // Nothing
}

//-*****************************************************************************
bool TransformDrw::readFrame( float i_frame )
{
    // First do parent load.
    if ( !ParentDrw::readFrame( i_frame ) )
    {
        // Durn
        m_currentFrameTransform.makeIdentity();
        m_transformedCurrentFrameBounds.makeEmpty();
        return false;
    }

    // Okay, now that we have parent loaded, and we've got
    // a tako transform reader...
    assert( m_takoTransformReader );

    // Get the transformation matrix.
    m_currentFrameTransform = m_takoTransformReader->getMatrix();

    // Get the current bounds and transform them.
    if ( m_currentFrameBounds.isEmpty() )
    {
        m_transformedCurrentFrameBounds.makeEmpty();
    }
    else
    {
        m_transformedCurrentFrameBounds =
            Imath::transform( m_currentFrameBounds,
                              m_currentFrameTransform );
    }

    return true;
}

//-*****************************************************************************
Box3d TransformDrw::getBounds() const
{
    return m_transformedCurrentFrameBounds;
}

//-*****************************************************************************
void TransformDrw::draw( const DrawContext & i_ctx ) const
{
    // See if we're valid.
    if ( !valid() || m_transformedCurrentFrameBounds.isEmpty() )
    {
        return;
    }

    // Get the matrix
    // GLdouble currentMatrix[16];
    // glGetDoublev( GL_MODELVIEW_MATRIX, currentMatrix );
    
    // Basically, we want to load our matrix into the thingy.
    // We don't use the OpenGL transform stack because we have deep
    // deep hierarchy that exhausts the stack depth quickly.
    // M44d concatXform = i_ctx.transform() * m_currentFrameTransform;
    M44d concatXform = m_currentFrameTransform * i_ctx.transform();

    // Load this xform.
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( ( const GLdouble * )&( concatXform[0][0] ) );
    // M44d cftTrans = m_currentFrameTransform;
    // glMultMatrixd( ( const GLdouble * )&( cftTrans[0][0] ) );

    // Draw children.
    DrawContext childCtx( i_ctx );
    childCtx.setTransform( concatXform );
    ParentDrw::draw( childCtx );

    // Reset matrix?
    // Probably not necessary
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( ( const GLdouble * )&( i_ctx.transform()[0][0] ) );
    //glLoadMatrixd( currentMatrix );
}

} // End namespace SimpleAbcViewer
