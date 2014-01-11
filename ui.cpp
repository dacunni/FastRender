#include <iostream>
#include <stdio.h>
#include <stdlib.h>

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
#include "Timer.h"
#include "BoundingVolume.h"
#include "TMOctreeAccelerator.h"
#endif
#include "AssetLoader.h"
#include "TriangleMesh.h"

int window_width = 768;
int window_height = 512;

TriangleMesh * mesh = nullptr;

#define GL_WARN_IF_ERROR() warnIfError( __FUNCTION__, __LINE__ )

GLenum warnIfError( const char * func, int line ) 
{
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {
        printf("GL Error: %s:%d %s\n", func, line, gluErrorString(err));
    }
}

void viewportReshaped( int width, int height ) 
{
    //printf("reshape: %d x %d\n", width, height);
    window_width = width;
    window_height = height;

    glViewport( 0, 0, window_width, window_height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 4.0, 1000.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void drawMesh( TriangleMesh & mesh ) 
{
    for( int tri = 0; tri < mesh.triangles.size(); tri++ ) {
        TriangleMesh::IndexTriangle vert_indices = mesh.triangles[tri];

        for( int vi = 0; vi < 3; vi++ ) {
            glVertex3f( mesh.vertices[ vert_indices.vi[vi] ].x,
                        mesh.vertices[ vert_indices.vi[vi] ].y,
                        mesh.vertices[ vert_indices.vi[vi] ].z );

        }
    }
}

bool data_uploaded = false;
GLuint vbo = 0;
GLuint ibo = 0;

void repaintViewport( void ) 
{
    GLenum err = GL_NO_ERROR;
    //printf("repaint\n");
    glClearColor( 0.1, 0.1, 0.1, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glColor4f( 0.25, 0.25, 0.25, 1.0 );
    glBegin( GL_LINES );
    glVertex3f( -1.0, -1.0, -0.1 );
    glVertex3f(  1.0,  1.0, -0.1 );
    glVertex3f( -1.0,  1.0, -0.1 );
    glVertex3f(  1.0, -1.0, -0.1 );
    glEnd();

    if( mesh ) {
        if( !data_uploaded ) {
            printf("uploading vertices\n");

            // Upload vertex positions
            glGenBuffers( 1, &vbo );
            glBindBuffer( GL_ARRAY_BUFFER, vbo );
            glBufferData( GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(float) * 4, &mesh->vertices[0].x, GL_STATIC_DRAW );
            GL_WARN_IF_ERROR();
            // Upload vertex indices
            glGenBuffers( 1, &ibo );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh->triangles.size() * sizeof(unsigned int) * 3, &mesh->triangles[0], GL_STATIC_DRAW );
            GL_WARN_IF_ERROR();
            
            printf("done uploading\n");
            data_uploaded = true;
        }

        if( data_uploaded ) {
            // Bind vertex data
            glBindBuffer( GL_ARRAY_BUFFER, vbo );
            glEnableClientState( GL_VERTEX_ARRAY );
            glVertexPointer( 4, GL_FLOAT, 0, NULL );
            // Bind vertex indices
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

            glEnable( GL_DEPTH_TEST );

            // draw polygon outlines
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            glColor4f( 1.0, 1.0, 1.0, 1.0 );
            glDrawElements( GL_TRIANGLES, mesh->triangles.size() * 3, GL_UNSIGNED_INT, NULL );
            GL_WARN_IF_ERROR();

            // prevent z-fighting
            glPolygonOffset( 1, 1 );
            glEnable( GL_POLYGON_OFFSET_FILL );

            // draw filled polygons
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            glColor4f( 0.2, 0.2, 0.4, 1.0 );
            glDrawElements( GL_TRIANGLES, mesh->triangles.size() * 3, GL_UNSIGNED_INT, NULL );
            GL_WARN_IF_ERROR();

            glDisable( GL_DEPTH_TEST );
        }
    }

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


    AssetLoader loader;
    std::string modelPath = "models";

    // bunnies
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    mesh = loader.load( bunnyPath + "/bun_zipper_res4.ply" );
    //mesh = loader.load( bunnyPath + "/bun_zipper_res3.ply" );
    //mesh = loader.load( bunnyPath + "/bun_zipper_res2.ply" );
    //mesh = loader.load( bunnyPath + "/bun_zipper.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return EXIT_FAILURE;
    }

    glutMainLoop();
    return EXIT_SUCCESS;
}

