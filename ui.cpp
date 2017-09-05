#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "FastRender.h"
#include "OpenGLUtil.h"

int window_width = 350;
int window_height = 350;

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

void repaintViewport( void ) 
{
    //printf("repaint\n");
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    // TODO

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
    GLint program_status = 0;

    GLuint vertex_shader = loadShader( GL_VERTEX_SHADER, vs );
    GLuint fragment_shader = loadShader( GL_FRAGMENT_SHADER, fs );

    GLuint shader_program = glCreateProgram();
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
    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE );
    glutInitWindowSize( window_width, window_height );
    glutInitWindowPosition( 0, 0 );
    glutCreateWindow("FastRender UI");

    printf( "Renderer: %s\n", glGetString( GL_RENDERER ) );
    printf( "GL Version: %s\n", glGetString( GL_VERSION ) );
    printf( "GLSL Version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

    //mesh_shader_program = createShaders( "basic.vs", "basic.fs" );
    //point_cloud_shader_program = createShaders( "points.vs", "points.fs" );

    glutReshapeFunc( viewportReshaped );
    glutDisplayFunc( repaintViewport );

    // TODO

    GL_WARN_IF_ERROR();
    glutMainLoop();
    return EXIT_SUCCESS;
}

