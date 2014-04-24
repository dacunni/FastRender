#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include <GLUT/glut.h>      // GLUT + OpenGL
#include <OpenGL/gl3.h>     // Core OpenGL 3.x+

#include "AssetLoader.h"
#include "TriangleMesh.h"
#include "OpenGLUtil.h"
#include "GPUMesh.h"
#include "GPUPointCloud.h"
#include "RandomNumberGenerator.h"
#include "BoundingVolume.h"

int window_width = 350;
int window_height = 350;

TriangleMesh * mesh = nullptr;
GPUMesh gpu_mesh;
GPUPointCloud gpu_point_cloud;

GLuint mesh_shader_program = 0;
GLuint point_cloud_shader_program = 0;

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

void buildPointCloud( void ) 
{
    // Make some random points
    std::vector<Vector4> points;
    RandomNumberGenerator rng;
    rng.seedCurrentTime();
    int num_points = 100000;
    Vector4 p;
    Vector4 dir( 1.0, 1.0, 0 );
    dir.normalize();
    for( int i = 0; i < num_points; i++ ) {
        rng.uniformSurfaceUnitSphere( p );     
        //rng.uniformSurfaceUnitHalfSphere( dir, p );     
        points.push_back( p );
    }
    gpu_point_cloud.setPointSize( 1 );
    gpu_point_cloud.upload( points );
}

void repaintViewport( void ) 
{
    //printf("repaint\n");
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    if( mesh ) {
        if( mesh_shader_program != 0 ) {
            glUseProgram( mesh_shader_program );
        }

        if( !gpu_mesh.uploaded() ) {
            gpu_mesh.upload( *mesh );
        }

        if( gpu_mesh.uploaded() ) {
            //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
            //glFrontFace( GL_CCW );
            //glEnable(GL_CULL_FACE);
            gpu_mesh.bind();
            gpu_mesh.draw();
        }
    }

    if( point_cloud_shader_program != 0 ) {
        glUseProgram( point_cloud_shader_program );
    }

    if( !gpu_point_cloud.uploaded() ) {
        buildPointCloud();
    }

    if( gpu_point_cloud.uploaded() ) {
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
        gpu_point_cloud.bind();
        //gpu_point_cloud.draw();
    }

    glDisable( GL_DEPTH_TEST );
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

GLuint createShaders( const char * vs, const char * fs ) 
{
    GLuint status = 0;
    GLint program_status = 0;
    GLuint shader_program = 0;

    GLuint vertex_shader = loadShader( GL_VERTEX_SHADER, vs );
    GLuint fragment_shader = loadShader( GL_FRAGMENT_SHADER, fs );

    shader_program = glCreateProgram();
    if( vertex_shader != 0 )
        glAttachShader( shader_program, vertex_shader );
    if( fragment_shader != 0 ) 
        glAttachShader( shader_program, fragment_shader );
    glBindAttribLocation( shader_program, 0, "position" );
    glBindAttribLocation( shader_program, 1, "normal" );
    glLinkProgram( shader_program );

    glGetProgramiv( shader_program, GL_LINK_STATUS, &program_status ); 
    printf( "Link status: %d\n", program_status );

    if( !program_status ) {
        GLint len = 0;
        glGetProgramiv( shader_program, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetProgramInfoLog( shader_program, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteProgram( shader_program );
        return 0; 
    }
    // FIXME - add error handling

    return shader_program;
}

int main (int argc, char * const argv[]) 
{
    printf("FastRender UI\n");
    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode( GLUT_DOUBLE              // Double buffered
                         | GLUT_RGBA | GLUT_DEPTH
                         | GLUT_3_2_CORE_PROFILE  // Core profile context
                         );
    glutInitWindowSize( window_width, window_height );
    glutInitWindowPosition( 0, 0 );
    glutCreateWindow("FastRender UI");

    printf( "Renderer: %s\n", glGetString( GL_RENDERER ) );
    printf( "GL Version: %s\n", glGetString( GL_VERSION ) );
    printf( "GLSL Version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

    mesh_shader_program = createShaders( "basic.vs", "basic.fs" );
    point_cloud_shader_program = createShaders( "points.vs", "points.fs" );

    glutReshapeFunc( viewportReshaped );
    glutDisplayFunc( repaintViewport );

    AssetLoader loader;
    std::string modelPath = "models";

    // dragon
    std::string dragonPath = modelPath + "/stanford/dragon/reconstruction";
    //mesh = loader.load( dragonPath + "/dragon_vrip_res4.ply" );
    //mesh = loader.load( dragonPath + "/dragon_vrip_res3.ply" );
    //mesh = loader.load( dragonPath + "/dragon_vrip_res2.ply" );
    //mesh = loader.load( dragonPath + "/dragon_vrip.ply" );

    // bunnies
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    //mesh = loader.load( bunnyPath + "/bun_zipper_res4.ply" );
    //mesh = loader.load( bunnyPath + "/bun_zipper_res3.ply" );
    //mesh = loader.load( bunnyPath + "/bun_zipper_res2.ply" );
    mesh = loader.load( bunnyPath + "/bun_zipper.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return EXIT_FAILURE;
    }

    BoundingVolume * meshBB = new BoundingVolume();
    meshBB->buildAxisAligned( mesh );
    meshBB->print();
    delete meshBB;

    GL_WARN_IF_ERROR();
    glutMainLoop();
    return EXIT_SUCCESS;
}

