//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
#include "ovr_integration.h"

//-*****************************************************************************
namespace OculusAbcViewer {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
#define BMASK_LEFT   1
#define BMASK_MIDDLE 2
#define BMASK_RIGHT  4

//-*****************************************************************************
static State g_state;

static Timer g_playbackTimer;

//-*****************************************************************************
void overlay();

static ovr_integration::OVRManager ovr_container;

GLFWwindow*
glfw_window(
        GLFWwindow* current_window,
        int width,
        int height,
        bool fullscreen=false);

//-*****************************************************************************
//-*****************************************************************************
// GLUT STUFF
//-*****************************************************************************
//-*****************************************************************************

static Transport *g_transport = NULL;

//-*****************************************************************************
void init_surface( void )
{
    if ( g_transport != NULL )
    {
        delete g_transport;
        g_transport = NULL;
    }
    g_transport = new Transport( g_state.abcFileName, g_state.fps );
}

//-*****************************************************************************
void init(GLFWwindow* window)
{
    {
        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 100.0 };
        GLfloat mat_back_emission[] = {0.0, 0.0, 0.0, 0.0 };
        GLfloat mat_front_emission[] = {1.0f, 0.0, 0.0, 1.0f };

        glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
        glMaterialfv( GL_FRONT, GL_EMISSION, mat_front_emission );
        glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );

        glMaterialfv( GL_BACK, GL_EMISSION, mat_back_emission );
        glMaterialfv( GL_BACK, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_BACK, GL_SHININESS, mat_shininess );

        glColorMaterial(GL_FRONT, GL_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
    }

    glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glDisable(GL_CULL_FACE);

    glShadeModel( GL_SMOOTH );

    // get the window dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    g_state.scene.cam.setSize(width, height);
    g_state.scene.cam.lookAt( V3d( 24, 18, 24 ), V3d( 0.0 ) );
    g_state.bMask = 0;
    g_state.showHelp = false;
    g_state.playback = kStopped;
    g_state.scene.pointSize = 3.0f;
    glPointSize( 3.0f );


    g_state.scene.cam.frame( g_transport->getBounds() );

    std::ostringstream titleStream;
    titleStream << "Archive = "
                << g_transport->getFileName()
                << " | Frame = "
                << g_transport->getCurrentFrame();

    glfwSetWindowTitle(window, titleStream.str().c_str());
}

static void _error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static double start_time = glfwGetTime();
static int frame_count = 0;
static double fps = 0;
static const float TIME_INTERVAL = 1.0f;

void current_fps()
{
    double current_time = glfwGetTime();

    double duration = current_time - start_time;

    if (duration < TIME_INTERVAL)
    {
        frame_count++;
        return;
    }

    fps = frame_count / duration;
    frame_count = 0;
    start_time = glfwGetTime();
}

//-*****************************************************************************
//-*****************************************************************************
static void TickForward()
{
    g_transport->tickForward();

    std::ostringstream titleStream;
    titleStream << "Archive = "
                << g_transport->getFileName()
                << " | Frame = "
                << g_transport->getCurrentFrame();
    glfwSetWindowTitle( g_state.scene.win, titleStream.str().c_str());

    g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );
}

//-*****************************************************************************
static void TickBackward()
{
    g_transport->tickBackward();

    std::ostringstream titleStream;
    titleStream << "Archive = "
                << g_transport->getFileName()
                << " | Frame = "
                << g_transport->getCurrentFrame();
    glfwSetWindowTitle(g_state.scene.win, titleStream.str().c_str());

    g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );
}

//-*****************************************************************************
void turntableIdle()
{
    if ( g_playbackTimer.elapsed() > 1.0/g_state.fps )
    {
        g_playbackTimer.restart();
        g_state.scene.cam.rotate( V2d( 1, 0 ) );
        g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );
    }
}

//-*****************************************************************************
void playFwdIdle()
{
    if ( g_playbackTimer.elapsed() > 1.0/g_state.fps )
    {
        g_playbackTimer.restart();
        TickForward();
    }
}

//-*****************************************************************************
void playBwdIdle()
{
    if ( g_playbackTimer.elapsed() > 1.0/g_state.fps )
    {
        g_playbackTimer.restart();
        TickBackward();
    }
}

//-*****************************************************************************
void display( void )
{


    g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );
    g_transport->draw( g_state.scene );

}

//-*****************************************************************************
void reshape(GLFWwindow* win, int w, int h )
{
    glfwGetFramebufferSize(win, &w, &h);
    g_state.scene.cam.setSize( w, h );
    g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );

    glMatrixMode( GL_MODELVIEW );
    ovr_container.reshape(win, w, h);
}

//-*****************************************************************************
void glPrint( GLfloat x, GLfloat y, const std::string &text )
{
    glRasterPos2f( x, y );
    for ( size_t p = 0; p < text.size(); ++p )
    {
        // @TODO: This is disabled for now.  GLFW doesn't provide an easy text
        // rendering thingy like GLUT did.
        // glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, text[p] );
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
void RenderIt()
{
    const char *templ = "/var/tmp/OculusAbcViewer_camera.XXXXXX";
    char *buffer = new char[strlen( templ ) + 1];
    strcpy( buffer, templ );
#ifndef PLATFORM_WINDOWS
    mkstemp( buffer );
#endif
    std::string cameraFileName = buffer;
#ifdef PLATFORM_WINDOWS
    cameraFileName = "OculusAbcViewer_camera.XXXXXX";
#endif

    float shutterOpenTime = -0.25f; // + ( float )g_transport->getCurrentFrame();
    float shutterCloseTime = 0.25f; //+ ( float )g_transport->getCurrentFrame();
    float openTime = -0.5f + ( float )g_transport->getCurrentFrame();
    float closeTime = 1.0f + openTime;

    std::ofstream camFile( cameraFileName.c_str() );
    camFile << g_state.scene.cam.RIB();
    camFile.close();

    Box3d bnd( g_transport->getBounds() );
    std::string boundStr;
    if ( ! bnd.isEmpty() )
    {
        std::ostringstream boundStream;
        boundStream << "["
                    << bnd.min[0] << " "
                    << bnd.max[0] << " "
                    << bnd.min[1] << " "
                    << bnd.max[1] << " "
                    << bnd.min[2] << " "
                    << bnd.max[2]
                    << "]";
        boundStr = boundStream.str();
    }
    else { boundStr = ""; }

    std::ostringstream cmdStream;
    cmdStream <<
        " --shutteropen=" << shutterOpenTime <<
        " --shutterclose=" << shutterCloseTime <<
        " -C " << cameraFileName <<
        " -P " << g_state.AlembicRiPluginDsoPath <<
        " -a " << g_state.abcFileName <<
        " --sample1=" << openTime <<
        " --sample2=" << closeTime <<
        " -B \"%s\"" << boundStr;
    std::string cmdArgs = cmdStream.str();
    std::string cmd = g_state.RenderScript;
    cmd += " ";
    cmd += cmdArgs;
    system( cmd.c_str() );

    delete[] buffer;
}


// start windowed
bool am_fullscreen = false;


void toggle_fullscreen()
{
    if (!am_fullscreen)
    {
        // get the current Desktop screen resolution and colour depth
        const GLFWvidmode *desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());

        int width = desktop->width;
        int height = desktop->height;
        g_state.scene.win = glfw_window(g_state.scene.win, width, height, true);
    }
    else
    {
        g_state.scene.win = glfw_window(g_state.scene.win, 800, 600);
    }

    am_fullscreen = !am_fullscreen;
    std::cerr << "toggling. now fullscreen: " << am_fullscreen << std::endl;

    if (!g_state.scene.win)
    {
        std::cerr << "glfw window is not valid" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}


enum _idle_function {
    IDLE_NO_OP,
    IDLE_PLAY_FWD_IDLE,
    IDLE_PLAY_BWD_IDLE,
    IDLE_TURNTABLE
};
_idle_function CURRENT_IDLE = IDLE_NO_OP;

void idle_function()
{
    if (IDLE_NO_OP == CURRENT_IDLE)
    {
        return;
    }

    switch (CURRENT_IDLE)
    {
        case IDLE_NO_OP:
            break;
        case IDLE_PLAY_FWD_IDLE:
            {
                playFwdIdle();
                break;
            };
        case IDLE_PLAY_BWD_IDLE:
            {
                playBwdIdle();
                break;
            };
        case IDLE_TURNTABLE:
            {
                turntableIdle();
                break;
            };
    };
}

//-*****************************************************************************
void keyboard(GLFWwindow* win, int key, int scancode, int action, int mods )
{
    // all our actions are on key presses
    if (action != GLFW_PRESS)
    {
        return;
    }

    static bool bf = true;

    switch ( key )
    {
    case '`':
    case '~':
        toggle_fullscreen();
        break;
    case 'f':
    case 'F':
        g_state.scene.cam.frame( g_transport->getBounds() );
        g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );
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
            CURRENT_IDLE = IDLE_NO_OP;
        }
        else
        {
            g_state.playback = kForward;
            CURRENT_IDLE = IDLE_PLAY_FWD_IDLE;
        }
        break;
    case '<':
    case ',':
        if ( g_state.playback == kBackward )
        {
            g_state.playback = kStopped;
            CURRENT_IDLE = IDLE_NO_OP;
        }
        else
        {
            g_state.playback = kBackward;
            CURRENT_IDLE = IDLE_PLAY_BWD_IDLE;
        }
        break;
    case 't':
    case 'T':
        if ( g_state.playback == kTurntable )
        {
            g_state.playback = kStopped;
            CURRENT_IDLE = IDLE_NO_OP;
        }
        else
        {
            g_state.playback = kTurntable;
            CURRENT_IDLE = IDLE_TURNTABLE;
        }
        break;
    case 'r':
    case 'R':
        RenderIt();
        break;
    case 'p':
        g_state.scene.pointSize =
            std::max( g_state.scene.pointSize-0.5f, 1.0f );
        break;
    case 'P':
        g_state.scene.pointSize =
            std::min( g_state.scene.pointSize+0.5f, 10.0f );
        break;
    case 'b':
    case 'B':
        bf = !bf;
        glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, bf ? GL_FALSE : GL_TRUE );
        break;
        break;
    case '\t': // tab
        g_state.showHelp = !g_state.showHelp;
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

//-*****************************************************************************
void mouse(GLFWwindow* win, int button, int action, int mods)
{
    double x, y;
    glfwGetCursorPos(win, &x, &y);

    g_state.last_x = floor(x);
    g_state.last_y = floor(y);

    if ( action == GLFW_PRESS )
    {
        switch( button )
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            g_state.bMask += BMASK_LEFT;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            g_state.bMask += BMASK_MIDDLE;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            g_state.bMask += BMASK_RIGHT;
            break;
        }
    }
    else
    {
        switch( button )
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            g_state.bMask -= BMASK_LEFT;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            g_state.bMask -= BMASK_MIDDLE;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            g_state.bMask -= BMASK_RIGHT;
            break;
        }
    }
    g_state.mods = mods;
}

//-*****************************************************************************
void mouseDrag(GLFWwindow* window, double x, double y )
{
    int dx = floor(x) - g_state.last_x;
    int dy = floor(y) - g_state.last_y;

    g_state.last_x = x;
    g_state.last_y = y;

    if ( g_state.mods & GLFW_MOD_ALT )
    {
        if ( g_state.bMask == BMASK_LEFT + BMASK_MIDDLE ||
             g_state.bMask == BMASK_RIGHT )
        {
            g_state.scene.cam.dolly( V2d( dx, dy ) );
            g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );
            return;
        }
        if ( g_state.bMask ==  BMASK_LEFT )
        {
            g_state.scene.cam.rotate( V2d( dx, dy ) );
            g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );
            return;
        }
        if ( g_state.bMask ==  BMASK_MIDDLE )
        {
            g_state.scene.cam.track( V2d( dx, dy ) );
            g_state.scene.cam.autoSetClippingPlanes( g_transport->getBounds() );
            return;
        }
    }
}

void apply_camera()
{
    g_state.scene.cam.apply();
}



//-*****************************************************************************
char* getOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool optionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int SimpleViewScene( int argc, char *argv[] )
{
    std::string viewerpath( argv[0] );
    std::string RenderScript( viewerpath + "Renderit" );
    std::string abcFileName;
    std::string AlembicRiPluginDsoPath;
    std::string desc( "Simple ABC Viewer:\n"
    "  -h [ --help ]         prints this help message\n"
    "  -f [ --file ] arg     abc file name\n"
    "  --fps arg             frames per second for playback (default=24.0)\n"
    "  --scale arg           Scene units per meter.  If 1 unit is 1cm, this should be 10.0.\n"
    "  -P [ --riPlugin ] arg full path to AlembicRiPlugin.so\n"
    "  --rndrScript arg      full path to Render Script" );
    float fps = 24.0f;
    float scene_scale=1.0f;

    // help
    if ( argc < 2 ||
         optionExists( argv, argv + argc, "-h" ) ||
         optionExists( argv, argv + argc, "--help" )
       ) {
        std::cout << desc << std::endl;
        return 0;
    };

    // file arg
    for(int i = 1; i < argc; i++) {
        if ( string(argv[i]).at(0) != '-' && string(argv[i-1]).at(0) != '-' ) {
            abcFileName = argv[i];
            break;
        };
    };

    // options
    if ( optionExists( argv, argv + argc, "-f" ) )
        abcFileName = string(
                getOption( argv, argv + argc, "-f" )
                );
    else if ( optionExists( argv, argv + argc, "--file" ) )
        abcFileName = string(
                getOption( argv, argv + argc, "--file" )
                );

    if ( optionExists( argv, argv + argc, "-P" ) )
        AlembicRiPluginDsoPath = string(
                getOption( argv, argv + argc, "-P" )
                );
    else if ( optionExists( argv, argv + argc, "--riPlugin" ) )
        AlembicRiPluginDsoPath = string(
                getOption( argv, argv + argc, "--riPlugin" )
                );

    if ( optionExists( argv, argv + argc, "--fps" ) )
        fps = std::atof( string(
                    getOption( argv, argv + argc, "--fps" ) ).c_str()
                );

    if ( optionExists( argv, argv + argc, "--scale" ) )
        scene_scale = std::atof( string(
                    getOption( argv, argv + argc, "--scale" ) ).c_str()
                );

    if ( optionExists( argv, argv + argc, "--rndrScript" ) )
        RenderScript = string(
                getOption( argv, argv + argc, "--rndrScript" )
                );

#ifndef DEBUG
    try
#endif
    {
        // Set up the state.
        g_state.abcFileName = abcFileName;
        g_state.fps = fps;
        g_state.playback = kStopped;
        g_state.AlembicRiPluginDsoPath = AlembicRiPluginDsoPath;
        g_state.RenderScript = RenderScript;
        g_state.scene_scale = scene_scale;

        glfwSetErrorCallback(_error_callback);

        if (!glfwInit())
        {
            std::cerr << "glfw init failed." << std::endl;
            exit(EXIT_FAILURE);
        }

        init_surface();
        g_state.scene.win = glfw_window(nullptr, 800, 600);

        while (!glfwWindowShouldClose(g_state.scene.win))
        {
            idle_function();

            V2d clipping_planes = g_state.scene.cam.clippingPlanes();
            double near_plane = clipping_planes.x;
            double far_plane = clipping_planes.y;

            ovr_container.draw_gl(
                    apply_camera, display, g_state.scene_scale, near_plane, far_plane);

            glfwPollEvents();
            current_fps();
        }

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

static bool glew_initialized = false;

/// Generate a new glfw_window.
GLFWwindow* glfw_window(
        GLFWwindow* current_window, int width, int height, bool fullscreen)
{
    if (current_window)
    {
        glfwDestroyWindow(current_window);
    }

    GLFWwindow* new_window;

    // I'm scoping this block to destroy the monitor after we create new_window.
    {


        auto monitor = glfwGetPrimaryMonitor();
        if (not fullscreen)
        {
            monitor = nullptr;
        }

        new_window = glfwCreateWindow(
                width,
                height,
                "abcviewer",
                monitor,
                NULL);
    }

    glfwMakeContextCurrent(new_window);

    if (not glew_initialized)
    {
        glewExperimental = GL_TRUE;

        GLenum glewStatus = glewInit();
        if (glewStatus != GLEW_OK)
        {
            std::cerr << "Glew init failed." << std::endl;
            exit(EXIT_FAILURE);
        }

        glew_initialized = true;

    }
    // Setup Callbacks
    glfwSetKeyCallback(new_window, keyboard);
    glfwSetWindowSizeCallback(new_window, reshape);
    glfwSetMouseButtonCallback(new_window, mouse);
    glfwSetCursorPosCallback(new_window, mouseDrag);


    // Init local GL stuff
    init(new_window);
    ovr_container.initialize_opengl();

    reshape(new_window, width, height);

    return new_window;
}

} // End namespace OculusAbcViewer
