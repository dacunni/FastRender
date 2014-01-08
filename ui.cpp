#include <iostream>
#include <stdio.h>

#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#if 0
#include "Artifacts.h"
#include "Matrix.h"
#include "Ray.h"
#include "Sphere.h"
#include "RandomNumberGenerator.h"
#include "Scene.h"
#include "FlatContainer.h"
#include "AxisAlignedSlab.h"
#include "TriangleMesh.h"
#include "Timer.h"
#include "AssetLoader.h"
#include "BoundingVolume.h"
#include "TMOctreeAccelerator.h"
#endif

int window_width = 768;
int window_height = 512;

void viewportReshaped( int width, int height ) 
{
    printf("reshape: %d x %d\n", width, height);

    window_width = width;
    window_height = height;

    glViewport( 0, 0, window_width, window_height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 0.1, 1000.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

}

void repaintViewport( void ) 
{
    printf("repaint\n");
    glClearColor( 0.1, 0.1, 0.1, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glColor4f( 0.0, 0.0, 1.0, 1.0 );
    glBegin( GL_LINES );
    glVertex3f( -1.0, -1.0, -0.1 );
    glVertex3f(  1.0,  1.0, -0.1 );
    glVertex3f( -1.0,  1.0, -0.1 );
    glVertex3f(  1.0, -1.0, -0.1 );
    glEnd();

    glutSwapBuffers();
}

int main (int argc, char * const argv[]) 
{
    printf("FastRender UI\n");
    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize( window_width, window_height );
    glutInitWindowPosition( 0, 0 );
    glutCreateWindow("FastRender UI");

    glutReshapeFunc( viewportReshaped );
    glutDisplayFunc( repaintViewport );




    glutMainLoop();
    return 0;
}

