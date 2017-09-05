#include <stdio.h>

#include "Editor.h"

static Editor * self = nullptr;

Editor::Editor()
{
    // FIXME: HACKHACK: Workaround for no user data pointer in GLUT. Assumes
    //        a single editor window.
    self = this;

}

Editor::~Editor()
{

}

void Editor::start() 
{

    int argc = 1;
    const char *argv[] = { "" };

    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode( GLUT_DOUBLE              // Double buffered
                         | GLUT_RGBA | GLUT_DEPTH
                         | GLUT_3_2_CORE_PROFILE  // Core profile context
                         );
    glutInitWindowSize( windowWidth, windowHeight );
    glutInitWindowPosition( 0, 0 );
    glutCreateWindow("FastRender Editor");
    
    std::string shaderPath = "shaders/";
    std::string defaultVertexShader = shaderPath + "basic.vs";
    std::string defaultFragmentShader = shaderPath + "basic.fs";

    defaultShaderProgram.loadFilesVertexFragment(defaultVertexShader, defaultFragmentShader);

    // TODO
#if 0

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

    // Create texture object for image
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
#endif

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
// corresponding instance member functions in Editor.
void Editor::sViewportReshaped( int w, int h ) { self->viewportReshaped( w, h ); }
void Editor::sRepaintViewport() { self->repaintViewport(); }
void Editor::sKeyPressed( unsigned char key, int x, int y ) { self->keyPressed( key, x, y ); }
void Editor::sMouseButton( int button, int state, int x, int y ) { self->mouseButton( button, state, x, y ); }
void Editor::sMouseMotionWhileButtonPressed( int x, int y ) { self->mouseMotionWhileButtonPressed( x, y ); }
void Editor::sAnimTimer( int value ) { self->animTimer( value ); }

// Callback

void Editor::viewportReshaped( int w, int h )
{
    windowWidth = w;
    windowHeight = h;
    glViewport( 0, 0, w, h );
    glutPostRedisplay();
}

void Editor::repaintViewport()
{
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

#if 0
    // Update texture with pixel accumulation buffer
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, pixelAccumTex );
    switch( activeImage ) {
        case FramebufferImage:
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, artifacts.width, artifacts.height,
                          0, GL_RGB, GL_FLOAT, &artifacts.pixel_color_accum[0] );
            break;
        case NormalsImage:
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, artifacts.width, artifacts.height,
                          0, GL_RGB, GL_FLOAT, &artifacts.pixel_normal[0] );
            break;
        case DepthImage:
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, artifacts.width, artifacts.height,
                          0, GL_RED, GL_FLOAT, &artifacts.pixel_depth[0] );
            break;
        default: ;
    }
    GL_WARN_IF_ERROR();

    // Update texture with pixel sample count. The divide is done
    // in the shader to get the pixel's rendered value.
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, pixelCountTex );
    if( activeImage == FramebufferImage ) {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_R32UI, artifacts.width, artifacts.height,
                      0, GL_RED_INTEGER, GL_UNSIGNED_INT,
                      &artifacts.pixel_color_num_samples[0] );
        glUniform1i( glGetUniformLocation( img_shader_program, "divideByCount" ), 1 );
    }
    else {
        glUniform1i( glGetUniformLocation( img_shader_program, "divideByCount" ), 0 );
    }
    GL_WARN_IF_ERROR();
    if( activeImage == DepthImage ) {
        glUniform1i( glGetUniformLocation( img_shader_program, "grayInRed" ), 1 );
    }
    else {
        glUniform1i( glGetUniformLocation( img_shader_program, "grayInRed" ), 0 );
    }
    GL_WARN_IF_ERROR();

    //glEnable( GL_DEPTH_TEST );
    
    glBindVertexArray( img_vao );
    glBindBuffer( GL_ARRAY_BUFFER, img_vbo );
    glUseProgram( img_shader_program );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    GL_WARN_IF_ERROR();

    //glDisable( GL_DEPTH_TEST );
#endif
    glutSwapBuffers();
}

void Editor::keyPressed( unsigned char key, int x, int y )
{

}

void Editor::mouseButton( int button, int state, int x, int y )
{

}

void Editor::mouseMotionWhileButtonPressed( int x, int y )
{

}

void Editor::animTimer( int value )
{
    glutPostRedisplay();
    glutTimerFunc( update_rate_sec * 1000, sAnimTimer, 0 );
}


