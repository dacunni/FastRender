//
//  PreviewWindow.cpp
//  FastRender
//
//  Created by David Cunningham on 6/24/17.
//
//

#include "Artifacts.h"
#include "PreviewWindow.h"

static PreviewWindow * self = nullptr;

PreviewWindow::PreviewWindow( Artifacts & a )
: artifacts(a)
{

}

PreviewWindow::~PreviewWindow()
{

}

void PreviewWindow::init() 
{
    printf("Initializing preview window\n");

    // FIXME: HACKHACK: Workaround for no user data pointer in GLUT. Assumes
    //        a single preview window.
    self = this;

    int argc = 1;
    const char *argv[] = { "" };

    window_width = artifacts.width;
    window_height = artifacts.height;

    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode( GLUT_DOUBLE              // Double buffered
                         | GLUT_RGBA | GLUT_DEPTH
                         | GLUT_3_2_CORE_PROFILE  // Core profile context
                         );
    glutInitWindowSize( window_width, window_height );
    glutInitWindowPosition( 0, 0 );
    glutCreateWindow("FastRender");
    
    // Create shaders
    // TODO: Clean up shader compilation
    GLint status;
    img_vertex_shader = glCreateShader( GL_VERTEX_SHADER );
    img_fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );;
    img_shader_program = glCreateProgram();

    // Vertex Shader
    const char * vs = R"glsl(
        #version 410
        
        layout (location=0) in vec4 position;
        layout (location=1) in vec2 uv;

        out vec2 vUV;
        
        void main()
        {
            gl_Position = position;
            vUV = uv;
        }
    )glsl";
    const char * vs_srcs[] = { vs };
    glShaderSource( img_vertex_shader, 1, vs_srcs, NULL );
    glCompileShader( img_vertex_shader );
    glGetShaderiv( img_vertex_shader, GL_COMPILE_STATUS, &status );
    printf( "Vertex Shader Compile status: %s\n", status ? "OK" : "ERROR" );
    if( !status ) {
        GLint len = 0;
        glGetShaderiv( img_vertex_shader, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetShaderInfoLog( img_vertex_shader, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteShader( img_vertex_shader );
        exit(EXIT_FAILURE); // FIXME - return error
    }

    // Fragment Shader
    const char * fs = R"glsl(
        #version 410
        
        in vec2 vUV;
        out vec4 color;
        
        uniform sampler2D pixelAccum;
        uniform usampler2D pixelCount;
         
        void main()
        {
            // Divide pixel accumulation buffer by sample count to get pixel color
            color = texture( pixelAccum, vUV ) / texture( pixelCount, vUV ).r;
            color.a = 1.0;
        }
    )glsl";
    const char * fs_srcs[] = { fs };
    glShaderSource( img_fragment_shader, 1, fs_srcs, NULL );
    glCompileShader( img_fragment_shader );
    glGetShaderiv( img_fragment_shader, GL_COMPILE_STATUS, &status );
    printf( "Fragment Shader Compile status: %s\n", status ? "OK" : "ERROR" );
    if( !status ) {
        GLint len = 0;
        glGetShaderiv( img_fragment_shader, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetShaderInfoLog( img_fragment_shader, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteShader( img_fragment_shader );
        exit(EXIT_FAILURE); // FIXME - return error
    }

    glAttachShader( img_shader_program, img_vertex_shader );
    glAttachShader( img_shader_program, img_fragment_shader );
    glLinkProgram( img_shader_program );
    glGetProgramiv( img_shader_program, GL_LINK_STATUS, &status ); 
    printf( "Link status: %s\n", status ? "OK" : "ERROR" );
    if( !status ) {
        GLint len = 0;
        glGetProgramiv( img_shader_program, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetProgramInfoLog( img_shader_program, len, &len, &log[0] );
        printf( "Linker Error Message:\n%s", (char *) &log[0] );
        glDeleteProgram( img_shader_program );
        exit(EXIT_FAILURE); // FIXME - return error
    }
    glUseProgram( img_shader_program );

    float quad[20] = {
        // x, y, z, u, v
        -1.0, -1.0, 1.0, 0.0, 1.0,
         1.0, -1.0, 1.0, 1.0, 1.0,
        -1.0,  1.0, 1.0, 0.0, 0.0,
         1.0,  1.0, 1.0, 1.0, 0.0
    };

    glGenVertexArrays( 1, &img_vao );
    glBindVertexArray( img_vao );
    glGenBuffers( 1, &img_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, img_vbo );

    auto position_loc = glGetAttribLocation( img_shader_program, "position" );
    auto uv_loc = glGetAttribLocation( img_shader_program, "uv" );
    glBufferData( GL_ARRAY_BUFFER, sizeof(quad), &quad[0], GL_STATIC_DRAW );
    glVertexAttribPointer( position_loc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL );
    glEnableVertexAttribArray( position_loc );
    glVertexAttribPointer( uv_loc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)) );
    glEnableVertexAttribArray( uv_loc );

    GL_WARN_IF_ERROR();

    // TODO: Create texture object for image
    glGenTextures( 1, &pixelAccumTex );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, pixelAccumTex );
    GLint pixelAccumLoc = glGetUniformLocation(img_shader_program, "pixelAccum");
    glUniform1i(pixelAccumLoc, 0);
    GL_WARN_IF_ERROR();

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, artifacts.width, artifacts.height,
                  0, GL_RGB, GL_FLOAT, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glGenTextures( 1, &pixelCountTex );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, pixelCountTex );
    GLint pixelCountLoc = glGetUniformLocation(img_shader_program, "pixelCount");
    glUniform1i(pixelCountLoc, 1);
    GL_WARN_IF_ERROR();

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, artifacts.width, artifacts.height,
                  0, GL_RED, GL_UNSIGNED_INT, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    GL_WARN_IF_ERROR();

    glutReshapeFunc( sViewportReshaped );
    glutDisplayFunc( sRepaintViewport );
    glutKeyboardFunc( sKeyPressed );
    glutMouseFunc( sMouseButton );
    glutMotionFunc( sMouseMotionWhileButtonPressed );

    glutTimerFunc( update_rate_sec * 1000, sAnimTimer, 0 );
    glutMainLoop();
}

// Static functions to use as GLUT callbacks. These delegate to their
// corresponding instance member functions in PreviewWindow.
void PreviewWindow::sViewportReshaped( int w, int h ) { self->viewportReshaped( w, h ); }
void PreviewWindow::sRepaintViewport() { self->repaintViewport(); }
void PreviewWindow::sKeyPressed( unsigned char key, int x, int y ) { self->keyPressed( key, x, y ); }
void PreviewWindow::sMouseButton( int button, int state, int x, int y ) { self->mouseButton( button, state, x, y ); }
void PreviewWindow::sMouseMotionWhileButtonPressed( int x, int y ) { self->mouseMotionWhileButtonPressed( x, y ); }
void PreviewWindow::sAnimTimer( int value ) { self->animTimer( value ); }

// Callback

void PreviewWindow::viewportReshaped( int w, int h )
{
    window_width = w;
    window_height = h;
    glViewport( 0, 0, w, h );
}

void PreviewWindow::repaintViewport()
{
    // Update texture with pixel accumulation buffer
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, pixelAccumTex );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, artifacts.width, artifacts.height,
                  0, GL_RGB, GL_FLOAT,
                  &artifacts.pixel_color_accum[0] );
    GL_WARN_IF_ERROR();

    // Update texture with pixel sample count. The divide is done
    // in the shader to get the pixel's rendered value.
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, pixelCountTex );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_R32UI, artifacts.width, artifacts.height,
                  0, GL_RED_INTEGER, GL_UNSIGNED_INT,
                  &artifacts.pixel_color_num_samples[0] );
    GL_WARN_IF_ERROR();

    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //glEnable( GL_DEPTH_TEST );
    
    glBindVertexArray( img_vao );
    glBindBuffer( GL_ARRAY_BUFFER, img_vbo );
    glUseProgram( img_shader_program );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    GL_WARN_IF_ERROR();

    //glDisable( GL_DEPTH_TEST );
    glutSwapBuffers();
}

void PreviewWindow::keyPressed( unsigned char key, int x, int y )
{

}

void PreviewWindow::mouseButton( int button, int state, int x, int y )
{

}

void PreviewWindow::mouseMotionWhileButtonPressed( int x, int y )
{

}

void PreviewWindow::animTimer( int value )
{
    glutPostRedisplay();
    glutTimerFunc( update_rate_sec * 1000, sAnimTimer, 0 );
}


