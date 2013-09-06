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

#include "Scene.h"
#include "GLCamera.h"
#include "IObjectDrw.h"
#include "MeshDrwHelper.h"

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {
    
// global object index for GL picking
std::vector<std::string> OBJECT_MAP;

//-*****************************************************************************
void setMaterials( float o, bool negMatrix = false )
{
    if ( negMatrix )
    {
        GLfloat mat_front_diffuse[] = { 0.1f * o, 0.1f * o, 0.9f * o, o };
        GLfloat mat_back_diffuse[] = { 0.9f * o, 0.1f * o, 0.9f * o, o };

        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 100.0 };

        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_front_diffuse );
        glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );

        glMaterialfv( GL_BACK, GL_DIFFUSE, mat_back_diffuse );
        glMaterialfv( GL_BACK, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_BACK, GL_SHININESS, mat_shininess );
    }
    else
    {

        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 100.0 };
        GLfloat mat_front_emission[] = {0.0, 0.0, 0.0, 0.0 };
        GLfloat mat_back_emission[] = {o, 0.0, 0.0, o };

        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        glMaterialfv( GL_FRONT, GL_EMISSION, mat_front_emission );
        glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );

        glMaterialfv( GL_BACK, GL_EMISSION, mat_back_emission );
        glMaterialfv( GL_BACK, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_BACK, GL_SHININESS, mat_shininess );

        glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
    }
}

//-*****************************************************************************
// SCENE CLASS
//-*****************************************************************************
Scene::Scene( const std::string &fileName, bool verbose )
  : m_fileName( fileName )
  , m_minTime( ( chrono_t )FLT_MAX )
  , m_maxTime( ( chrono_t )-FLT_MAX )
{
    Timer playbackTimer;

    Alembic::AbcCoreFactory::IFactory factory;
    m_archive = factory.getArchive( fileName );

    m_topObject = IObject( m_archive, kTop );

    if ( verbose )
        std::cout << "Opened archive and top object, creating drawables."
                  << std::endl;

    m_drawable.reset( new IObjectDrw( m_topObject, false ) );
    ABCA_ASSERT( m_drawable->valid(),
                 "Invalid drawable for archive: " << fileName );

    if ( verbose )
        std::cout << "Created drawables, getting time range." << std::endl;
    
    m_minTime = m_drawable->getMinTime();
    m_maxTime = m_drawable->getMaxTime();

    if ( m_minTime <= m_maxTime ) {
        if ( verbose )
            std::cout << "\nMin Time: " << m_minTime << " seconds " << std::endl
                      << "Max Time: " << m_maxTime << " seconds " << std::endl
                      << "\nLoading min time." << std::endl;
        m_drawable->setTime( m_minTime );
    }
    else {
        if ( verbose )
            std::cout << "\nConstant Time." << std::endl
                      << "\nLoading constant sample." << std::endl;
        m_minTime = m_maxTime = 0.0;
        m_drawable->setTime( 0.0 );
    }

    ABCA_ASSERT( m_drawable->valid(),
                 "Invalid drawable after reading start time" );

    if ( verbose )
        std::cout << "Done opening archive. Elapsed CPU time: "
                  << ((float)playbackTimer.elapsed()) / CLOCKS_PER_SEC << " seconds." << std::endl;

    // Bounds have been formed!
    m_bounds = m_drawable->getBounds();
    
    if ( verbose )
        std::cout << "Bounds at min time: " << m_bounds.min << " to "
                  << m_bounds.max << std::endl;
}

//-*****************************************************************************
void Scene::setTime( chrono_t iSeconds )
{
    ABCA_ASSERT( m_archive && m_topObject &&
                 m_drawable && m_drawable->valid(),
                 "Invalid Scene: " << m_fileName );

    if ( m_minTime <= m_maxTime )
    {
        m_drawable->setTime( iSeconds );
        ABCA_ASSERT( m_drawable->valid(),
                     "Invalid drawable after setting time to: "
                     << iSeconds );
    }

    m_bounds = m_drawable->getBounds();
}

//-*****************************************************************************
int Scene::processHits( GLint hits, GLuint buffer[] )
{
    GLint i;
    GLuint j, names, *ptr, minZ, *ptrNames, index;

    ptr = (GLuint *) buffer;
    minZ = 0xffffffff;
    for ( i=0; i<hits; i++ ) {
        names = *ptr;
        ptr++;
        if ( *ptr < minZ ) {
            index = names;
            minZ = *ptr;
            ptrNames = ptr+2;
        }
        ptr += names+2;
    }

    std::cout << "names " << names << std::endl;

    ptr = ptrNames;
    for (j = 0; j < index; j++,ptr++) {
        std::cout << *ptr ;
    }

    return index;
}

//-*****************************************************************************
std::string Scene::selection( int x, int y, GLCamera &camera, SceneState &s_state )
{
    const int MaxSize = 512;
    GLuint buffer[MaxSize];
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MaxSize, buffer);

    // enter select mode
    glRenderMode(GL_SELECT);
    glInitNames();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix((GLdouble)x,
                  (GLdouble)(viewport[3] - y),
                  5.0, 5.0, viewport);

    GLfloat ratio = (GLfloat)camera.width() / camera.height();
    gluPerspective(camera.fovy(), ratio, 0.1f, 1000);

    // draw the scene
    draw(s_state, true);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // finally release the rendering context again
    int hits = glRenderMode(GL_RENDER);

    if ( hits == 0 ) {
        return "";
    } else {
        return OBJECT_MAP[ buffer[3]-1 ];
    }

}

//-*****************************************************************************
void Scene::drawBounds( SceneState &s_state, const int mode )
{
    drawBoundingBox( m_drawable->getBounds(), mode );
}

//-*****************************************************************************
void Scene::draw( SceneState &s_state, bool visibleOnly, bool boundsOnly )
{
    ABCA_ASSERT( m_archive && m_topObject &&
                 m_drawable && m_drawable->valid(),
                 "Invalid Scene: " << m_fileName );

    glEnable( GL_LIGHTING );
    
    // clear the object index for GL picking
    OBJECT_MAP.clear();

    // Get the matrix
    M44d currentMatrix;
    glGetDoublev( GL_MODELVIEW_MATRIX, ( GLdouble * )&(currentMatrix[0][0]) );

    DrawContext dctx;
    dctx.setWorldToCamera( currentMatrix );
    dctx.setPointSize( s_state.pointSize );
    dctx.setVisibleOnly( visibleOnly );
    dctx.setBoundsOnly( boundsOnly );

    m_drawable->draw( dctx );

}

} // End namespace ABCOPENGL_VERSION_NS
} // End namespace AbcOpenGL
