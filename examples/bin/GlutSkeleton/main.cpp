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

#include <Alembic/GLUtil/All.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

//-*****************************************************************************
// GLUT display callback.  This is called whenever the display window needs
// to be refreshed
void display()
{
    glDrawBuffer( GL_BACK );    
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    
    // Put user draw code here....
    
    glutSwapBuffers();    
}

//-*****************************************************************************
// GLUT keyboard handler.  This is called whenever a key is hit.
void keyboardFunc(unsigned char key, int x, int y)
{
    switch( key )
    {
    case 27:   // ESC
        exit( 0 );
        break;
    }
}

//-*****************************************************************************
// GLUT mouse button handler.  This is called whenever a mouse button is hit.
void mouseFunc( int button, int state, int x, int y )
{
    // Nothing
}

//-*****************************************************************************
// GLUT mouse motion handler.  This is called whenever the mouse is moved
// while a mouse button is being hit.
void mouseDragFunc( int x, int y )
{
    // Nothing
}

//-*****************************************************************************
int main( int argc, char **argv )
{
    // Do basic glut setup stuff
    glutInitDisplayMode( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 640, 480 );
    glutInit( &argc, argv ); // parses some special glut args (see manpage)
    glutCreateWindow( "glut_skeleton" );
  
    // Initialize GL.
    Alembic::GLUtil::InitGL();

    // Set up callbacks
    glutDisplayFunc( display );    
    glutKeyboardFunc( keyboardFunc );
    glutMouseFunc( mouseFunc );
    glutMotionFunc( mouseDragFunc );
    
    glutMainLoop(); // NOTE: never returns
	
    return 0;
}
