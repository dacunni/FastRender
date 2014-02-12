#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

//#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>

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

GLuint vertex_shader = 0;
GLuint fragment_shader = 0;
GLuint shader_program = 0;

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
    GL_WARN_IF_ERROR();
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
GLuint vao = 0;
GLuint vbo = 0;
GLuint ibo = 0;

void repaintViewport( void ) 
{
    //printf("repaint\n");
    GL_WARN_IF_ERROR();
    glClearColor( 0.3, 0.3, 0.5, 1.0 );
    GL_WARN_IF_ERROR();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    GL_WARN_IF_ERROR();

    if( mesh ) {

        if( shader_program != 0 ) {
            glUseProgram( shader_program );
            GL_WARN_IF_ERROR();
        }

        if( !data_uploaded ) {
            printf("uploading vertices\n");
            // Create vertex array object to hold everything
            glGenVertexArrays( 1, &vao );
            glBindVertexArray( vao );
            // Upload vertex positions
            glGenBuffers( 1, &vbo );
            glBindBuffer( GL_ARRAY_BUFFER, vbo );
            glBufferData( GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(float) * 4, &mesh->vertices[0].x, GL_STATIC_DRAW );
            glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, 0 );
            glEnableVertexAttribArray( 0 );
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
            glBindVertexArray( vao );
            GL_WARN_IF_ERROR();
            //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            glDrawArrays( GL_TRIANGLES, 0, mesh->triangles.size() * 3 );
            GL_WARN_IF_ERROR();
        }
    }

    glutSwapBuffers();
}

const char * shaderTypeAsString( GLuint type )
{
    switch( type ) {
        case GL_VERTEX_SHADER:
            return "VERTEX SHADER";
        case GL_FRAGMENT_SHADER:
            return "FRAGMENT SHADER";
        default:
            return "UNKNOWN SHADER";
    }
}

GLuint loadShader( GLuint type, const std::string & filename )
{
    const char * type_string = shaderTypeAsString( type );
    int status = 0;

    std::ifstream ifs( filename );
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string src = ss.str();
    // FIXME - add error handling

    printf( ">>>> %s >>>>\n%s<<<< %s <<<<\n", type_string, src.c_str(), type_string );
    GLuint shader = glCreateShader( type );    

    // Make an array of pointers for GL
    const char * srcs[] = { src.c_str() };
    glShaderSource( shader, 1, srcs, NULL );

    // Compile shader
    glCompileShader( shader );
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );

    printf( "Shader Compile status: %d\n", status );

    if( !status ) {
        GLint len = 0;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetShaderInfoLog( shader, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteShader( shader );
        return 0;
    }

    // FIXME - add error handling

    return shader;
}

void createShaders( void ) 
{
    GLuint status = 0;
    GLint program_status = 0;

    vertex_shader = loadShader( GL_VERTEX_SHADER, "basic.vs" );
    fragment_shader = loadShader( GL_FRAGMENT_SHADER, "basic.fs" );

    shader_program = glCreateProgram();
    if( vertex_shader != 0 )
        glAttachShader( shader_program, vertex_shader );
    if( fragment_shader != 0 ) 
        glAttachShader( shader_program, fragment_shader );
    glBindAttribLocation( shader_program, 0, "position" );
    glLinkProgram( shader_program );

    glGetProgramiv( shader_program, GL_LINK_STATUS, &program_status ); 
    printf( "Link status: %d\n", program_status );
}

int main (int argc, char * const argv[]) 
{
    printf("FastRender UI\n");
    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE );
    glutInitWindowSize( window_width, window_height );
    glutInitWindowPosition( 0, 0 );
    glutCreateWindow("FastRender UI");

    printf( "Renderer: %s\n", glGetString( GL_RENDERER ) );
    printf( "GL Version: %s\n", glGetString( GL_VERSION ) );
    printf( "GLSL Version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

    createShaders();

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

    GL_WARN_IF_ERROR();
    glutMainLoop();
    return EXIT_SUCCESS;
}

