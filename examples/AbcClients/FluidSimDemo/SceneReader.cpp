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

#include <FluidSimDemo/SceneReader.h>
#include <FluidSimDemo/DrawContext.h>

namespace FluidSimDemo {

//-*****************************************************************************
SceneReader::SceneReader( const std::string &iFileName,
                          const M44d &worldXform )
  : m_fileName( iFileName ),
    m_worldXform( worldXform )
{
    m_archive = Abc::IArchive( iFileName );
    m_topObject = m_archive.getTop();
    
    m_fps = Abc::IDoubleProperty( m_topOject, "framesPerSecond" ).getValue();
    std::cout << "Frames per second: " << m_fps << std::endl;
    m_startFrame = m_fps *
        Abc::IDoubleProperty( m_topObject, "minSampleTime" ).getValue();
    m_endFrame = m_fps *
        Abc::IDoubleProperty( m_topObject, "maxSampleTime" ).getValue();
    std::cout << "Start frame: " << m_startFrame
              << ", end frame: " << m_endFrame << std::endl;
    
    
    std::cout << "Scene: " << m_fileName << " read. " << std::endl;

    // Current frame.
    m_currentFrame = m_startFrame;

    // Create the drawables and collision & init state objects.
    m_sceneXformDrw.reset( new TransformDrw( m_topObject,
                                             m_collisionObjects ) );
    std::cout << "Created Scene Hierarchy. " << std::endl
              << "\tNum Collision Objects: " << m_collisionObjects.size()
              << std::endl;

    // Update to frame.
    updateToFrame( m_startFrame );
}

//-*****************************************************************************
void SceneReader::updateToFrame( float newFrame )
{
    newFrame = std::max( newFrame, m_startFrame );
    newFrame = std::min( newFrame, m_endFrame );

    // std::cout << "\n\n\n********** FRAME: " << newFrame << std::endl;

    m_sceneXformDrw->readInterpolatedFrame( newFrame, m_worldXform );

    m_currentFrame = newFrame;
}

//-*****************************************************************************
void SceneReader::draw() const
{
    glMatrixMode( GL_MODELVIEW );
    
    // Get the matrix
    GLdouble currentMatrix[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, currentMatrix );

    const M44d *mat = reinterpret_cast<const M44d *>(
        ( const GLdouble * )currentMatrix );

    // Create a context.
    DrawContext ctx;
    ctx.setTransform( *mat );
    ctx.setCameraTransform( *mat );

    // Draw.
    m_sceneXformDrw->draw( ctx );
}

} // End namespace FluidSimDemo
