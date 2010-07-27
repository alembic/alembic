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

#include "Scene.h"
#include "AssetDrw.h"

//-*****************************************************************************
namespace SimpleAbcViewer {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// YO!!!
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void setMaterials( float o, bool negMatrix = false )
{
    if ( negMatrix )
    {
        GLfloat mat_front_diffuse[] = { 0.1 * o, 0.1 * o, 0.9 * o, o };
        GLfloat mat_back_diffuse[] = { 0.9 * o, 0.1 * o, 0.9 * o, o };

        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 100.0 };
        GLfloat light_position[] = { 20.0, 20.0, 20.0, 0.0 };

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
        GLfloat light_position[] = { 20.0, 20.0, 20.0, 0.0 };
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
//-*****************************************************************************
//-*****************************************************************************
// SCENE CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
Scene::Scene( const std::string &abcFileName )
  : m_fileName( abcFileName )
{
    std::cout << "Beginning to open asset: " << abcFileName << std::endl;
    
    boost::timer Timer;

    m_drawable.reset( new AssetDrw( abcFileName ) );
    //if ( !m_drawable->valid() )
    //{
    //    ABC_THROW( "Couldn't open file: " << abcFileName );
    //}

    std::cout << "Opened asset, getting frame range." << std::endl;

    // time to figure out what frames we have in our flat hierarchy
    PropertyPair frameProp;

    std::set < float > frames;

    if ( m_topXformPtr->readProperty( "frameRange", 0.0, frameProp )
         == PROPERTY_STATIC )
    {
        std::vector <float> & fr = boost::get< std::vector<float> >
            ( frameProp.first );
        frames.insert( fr.begin(), fr.end() );
    }
    else if ( m_topXformPtr->readProperty( "frameRangeShape", 0.0, frameProp )
              == PROPERTY_STATIC )
    {
        std::vector <float> & fr = boost::get< std::vector<float> >
            ( frameProp.first );
        frames.insert( fr.begin(), fr.end() );
    }

    // for the static case
    if ( frames.empty() || frames.count( FLT_MAX ) > 0 )
    {
        m_startFrame = m_endFrame = 0;
        frames.insert( FLT_MAX );
    }
    else
    {
        m_startFrame = (*(frames.begin()));
        m_endFrame = (*(frames.rbegin()));
    }

    if ( m_startFrame > m_endFrame )
    {
        ABC_THROW( "Invalid frame range in file: "
                   << m_fileName << ": " << m_startFrame
                   << " to " << m_endFrame );
    }

    std::cout << "Scene: " << m_fileName << " read. " << std::endl
              << "Start frame: " << m_startFrame << std::endl
              << "End frame: " << m_endFrame << std::endl;








    

#if 1
    std::set<float> framesSet;
    m_drawable->getFrames( framesSet );
    if ( framesSet.size() < 1 )
    {
        ABC_THROW( "File has no samples: " << abcFileName );
    }

    m_frames.clear();
    m_frames.insert( m_frames.begin(), framesSet.begin(),
                     framesSet.end() );
#else
    for ( int frame = 88; frame <= 305; ++frame )
    {
        m_frames.push_back( ( float )frame );
    }
#endif
        

    float startFrame = m_frames.front();
    float endFrame = m_frames.back();
    std::cout << "Asset goes from frame: " << startFrame
              << " to " << endFrame << std::endl;

    // Use the first frame.
    if ( !m_drawable->readFrame( startFrame ) )
    {
        ABC_THROW( "Couldn't load frame: " << startFrame
                   << " from file: " << abcFileName );
    }

    std::cout << "Done opening asset. Elapsed time: "
              << Timer.elapsed() << " seconds." << std::endl;

    // Bounds have been formed!
    Box3d bnds = m_drawable->getBounds();
    std::cout << "Bounds: " << bnds.min << " to "
              << bnds.max << std::endl;
}

//-*****************************************************************************
void Scene::setTime( float seconds )
{
    if ( !m_drawable ||
         ( m_frames.size() == 1 &&
           m_frames[0] == FLT_MAX ) )
    {
        return;
    }

    std::vector<float>::iterator lbIter =
        std::lower_bound( m_frames.begin(), m_frames.end(), seconds );

    if ( lbIter == m_frames.end() )
    {
        ABC_THROW( "Couldn't set time to: " << seconds );
    }

    float goodFrame = (*lbIter);

    if ( !m_drawable->readFrame( goodFrame ) )
    {
        ABC_THROW( "Couldn't read frame: " << goodFrame );
    }
}

//-*****************************************************************************
float Scene::startTime() const
{
    return m_frames.front();
}

//-*****************************************************************************
float Scene::endTime() const
{
    return m_frames.back();
}

//-*****************************************************************************
Box3d Scene::bounds() const
{
    if ( !m_drawable )
    {
        Box3d b;
        b.makeEmpty();
        return b;
    }
    else
    {
        return m_drawable->getBounds();
    }
}

//-*****************************************************************************
void Scene::draw( SceneState &s_state ) const
{
    glDrawBuffer( GL_BACK );
    s_state.cam.apply();

    glEnable( GL_LIGHTING );
    setMaterials( 1.0, false );

    
    // Get the matrix
    M44d currentMatrix;
    glGetDoublev( GL_MODELVIEW_MATRIX, ( GLdouble * )&(currentMatrix[0][0]) );
    
    DrawContext dctx;
    dctx.setTransform( currentMatrix );
    
    m_drawable->draw( dctx );
    
    glutSwapBuffers();
}

} // End namespace SimpleAbcViewer
