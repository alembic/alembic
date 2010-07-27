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

#include "Viewer.h"

//-*****************************************************************************
namespace SimpleFsdViewer {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
#define BMASK_LEFT   1
#define BMASK_MIDDLE 2
#define BMASK_RIGHT  4

//-*****************************************************************************
static State g_state;
static boost::timer g_playbackTimer;
#define SECONDS_PER_FRAME 0.046667

//-*****************************************************************************
void overlay();

//-*****************************************************************************
//-*****************************************************************************
// GLUT STUFF
//-*****************************************************************************
//-*****************************************************************************

static Scenes *g_tester = NULL;

//-*****************************************************************************
void init_surface( void )
{
    if ( g_tester != NULL )
    {
        delete g_tester;
        g_tester = NULL;
    }
    g_tester = new Scenes( g_state.abcFileName, g_state.fps );
}

//-*****************************************************************************
void init( void )
{
    {
        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 100.0 };
        GLfloat light_position[] = { 20.0, 20.0, 20.0, 0.0 };
        GLfloat mat_front_emission[] = {0.0, 0.0, 0.0, 0.0 };
        GLfloat mat_back_emission[] = {1.0f, 0.0, 0.0, 1.0f };

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

    // glLightfv( GL_LIGHT0, GL_POSITION, light_position );
    glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glDisable(GL_CULL_FACE);

    glShadeModel( GL_SMOOTH );

    g_state.scene.cam.setSize( 800, 600 );
    g_state.scene.cam.lookAt( V3d( 24, 18, 24 ), V3d( 0.0 ) );
    g_state.bMask = 0;
    g_state.showHelp = false;
    g_state.playback = kStopped;

    init_surface();

    g_state.scene.cam.frame( g_tester->bounds() );

    glutSetWindowTitle( ( boost::format( "Asset = %s | Frame = %d" )
                          % g_tester->fileName()
                          % g_tester->currentFrame() ).str().c_str() );
}

//-*****************************************************************************
//-*****************************************************************************
static void TickForward()
{
    g_tester->tickForward();
    glutSetWindowTitle( ( boost::format( "Asset = %s | Frame = %d" )
                          % g_tester->fileName()
                          % g_tester->currentFrame() ).str().c_str() );
    g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );
    glutPostRedisplay();
}

//-*****************************************************************************
static void TickBackward()
{
    g_tester->tickBackward();
    glutSetWindowTitle( ( boost::format( "Asset = %s | Frame = %d" )
                          % g_tester->fileName()
                          % g_tester->currentFrame() ).str().c_str() );
    g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );
    glutPostRedisplay();
}

//-*****************************************************************************
void turntableIdle()
{
    if ( g_playbackTimer.elapsed() > SECONDS_PER_FRAME )
    {
        g_playbackTimer.restart();
        g_state.scene.cam.rotate( V2d( 1, 0 ) );
        g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );
        glutPostRedisplay();
    }
}

//-*****************************************************************************
void playFwdIdle()
{
    if ( g_playbackTimer.elapsed() > SECONDS_PER_FRAME )
    {
        g_playbackTimer.restart();
        TickForward();
    }
}

//-*****************************************************************************
void playBwdIdle()
{
    if ( g_playbackTimer.elapsed() > SECONDS_PER_FRAME )
    {
        g_playbackTimer.restart();
        TickBackward();
    }
}

//-*****************************************************************************
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // glPushMatrix();

    g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );
    g_tester->draw( g_state.scene );

    // glPopMatrix();
    glFlush();
}

//-*****************************************************************************
void reshape( int w, int h )
{
    g_state.scene.cam.setSize( w, h );
    g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );

    glMatrixMode( GL_MODELVIEW );
}

//-*****************************************************************************
void glPrint( GLfloat x, GLfloat y, const std::string &text )
{
    glRasterPos2f( x, y );
    for ( int p = 0; p < text.size(); ++p )
    {
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, text[p] );
    }
}

//-*****************************************************************************
void overlay()
{
    if ( !g_state.showHelp )
    {
        return;
    }

    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );

    glColor4f( .1f, .9f, .1f, 1.0f );
    glPrint( -5, 5, "Good" );

    glColor4f( .9f, .1f, .1f, 1.0f );
    glPrint( -5, 4.5, "Inverse Normals" );

    glColor4f( .1f, .1f, .9f, 1.0f );
    glPrint( -5, 4, "Negative Transform" );

    glColor4f( .9f, .1f, .9f, 1.0f );
    glPrint( -5, 3.5, "Negative Transform + Inverse Normals" );

    glEnable( GL_LIGHTING );
    glEnable( GL_DEPTH_TEST );
}

//-*****************************************************************************
void keyboard( unsigned char key, int x, int y )
{
    // std::cout << "Key hit: " << ( int )key << std::endl;

    static bool bf = true;
    static bool xray = false;

    switch ( key )
    {
        //case 'u':
        //case 'U':
        //init_surface();
        //g_state.scene.rebuild = true;
        //glutPostRedisplay();
        //break;
    case '`':
    case '~':
        glutFullScreen();
        break;
    case 'f':
    case 'F':
        g_state.scene.cam.frame( g_tester->bounds() );
        g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );
        glutPostRedisplay();
        break;
    case 'c':
        TickBackward();
        break;
    case 'v':
        TickForward();
        break;

    case '>':
    case '.':
        if ( g_state.playback == kForward )
        {
            g_state.playback = kStopped;
            glutIdleFunc( NULL );
        }
        else
        {
            g_state.playback = kForward;
            glutIdleFunc( playFwdIdle );
        }
        break;
    case '<':
    case ',':
        if ( g_state.playback == kBackward )
        {
            g_state.playback = kStopped;
            glutIdleFunc( NULL );
        }
        else
        {
            g_state.playback = kBackward;
            glutIdleFunc( playBwdIdle );
        }
        break;
    case '\t': // tab
        g_state.showHelp = !g_state.showHelp;
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

//-*****************************************************************************
void mouse( int button, int state, int x, int y )
{
    g_state.last_x = x;
    g_state.last_y = y;
    if ( state == GLUT_DOWN )
    {
        switch( button )
        {
        case GLUT_LEFT_BUTTON:
            g_state.bMask += BMASK_LEFT;
            break;
        case GLUT_MIDDLE_BUTTON:
            g_state.bMask += BMASK_MIDDLE;
            break;
        case GLUT_RIGHT_BUTTON:
            g_state.bMask += BMASK_RIGHT;
            break;
        }
    }
    else
    {
        switch( button )
        {
        case GLUT_LEFT_BUTTON:
            g_state.bMask -= BMASK_LEFT;
            break;
        case GLUT_MIDDLE_BUTTON:
            g_state.bMask -= BMASK_MIDDLE;
            break;
        case GLUT_RIGHT_BUTTON:
            g_state.bMask -= BMASK_RIGHT;
            break;
        }
    }
    g_state.mods = glutGetModifiers();
}

//-*****************************************************************************
void mouseDrag( int x, int y )
{
    int dx = x - g_state.last_x;
    int dy = y - g_state.last_y;

    g_state.last_x = x;
    g_state.last_y = y;

    if ( g_state.mods && GLUT_ACTIVE_ALT )
    {
        if ( g_state.bMask == BMASK_LEFT + BMASK_MIDDLE ||
             g_state.bMask == BMASK_RIGHT )
        {
            g_state.scene.cam.dolly( V2d( dx, dy ) );
            g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );
            glutPostRedisplay();
            return;
        }
        if ( g_state.bMask ==  BMASK_LEFT )
        {
            g_state.scene.cam.rotate( V2d( dx, dy ) );
            g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );
            glutPostRedisplay();
            return;
        }
        if ( g_state.bMask ==  BMASK_MIDDLE )
        {
            g_state.scene.cam.track( V2d( dx, dy ) );
            g_state.scene.cam.autoSetClippingPlanes( g_tester->bounds() );
            glutPostRedisplay();
            return;
        }
    }
}

//-*****************************************************************************
namespace po = boost::program_options;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int SimpleViewScene( int argc, char *argv[] )
{
    std::string viewerpath( argv[0] );
    std::string abcFileName = "";
    float fps = 1.0f;

    po::options_description desc( "Simple FluidSimDemo Viewer" );
    desc.add_options()

        ( "help,h", "prints this help message" )

        ( "file,f",
          po::value<std::string>( &abcFileName ),
          "abc file name" )

        ( "fps",
          po::value<float>( &fps ),
          "frames per second for playback (default=1.0)" )

        ;

    po::positional_options_description pod;
    pod.add( "file", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( argc, argv ).
               options( desc ).positional( pod ).run(), vm );
    po::notify( vm );

    //-*************************************************************************
    if ( vm.count( "help" ) || argc < 2 )
    {
        std::cout << desc << std::endl;
        return -1;
    }

#ifndef DEBUG
    try
#endif
    {
        // Set up the state.
        g_state.abcFileName = abcFileName;
        g_state.fps = fps;
        g_state.playback = kStopped;

        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
        glutInitWindowSize( 800, 600 );
        glutInitWindowPosition( 100, 100 );

        char *fakeArgv[] = { "nothing" };
        int fakeArgc = 1;

        glutInit( &fakeArgc, fakeArgv );
        glutCreateWindow( abcFileName.c_str() );

        // Initialize GL.
        Alembic::GLUtil::InitGL();

        // Init local GL stuff
        init();

        // Setup Callbacks
        glutDisplayFunc( display );
        glutKeyboardFunc( keyboard );
        glutReshapeFunc( reshape );
        glutMouseFunc( mouse );
        glutMotionFunc( mouseDrag );

        glutMainLoop();
        return 0;
    }
#ifndef DEBUG
    catch ( std::exception &exc )
    {
        std::cerr << "EXCEPTION: " << exc.what() << std::endl;
        exit( -1 );
    }
    catch ( ... )
    {
        std::cerr << "UNKNOWN EXCEPTION" << std::endl;
        exit( -1 );
    }
#endif

    return 0;
}

} // End namespace SimpleFsdViewer
