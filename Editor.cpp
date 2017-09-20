#include <stdio.h>

#include "RandomNumberGenerator.h" // TEMP
#include "SimpleCamera.h"
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

void Editor::init() 
{
    // Dummy params for glutInit()
    int argc = 1;
    const char *argv[] = { "" };

    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode(GLUT_DOUBLE              // Double buffered
                        | GLUT_RGBA | GLUT_DEPTH
                        | GLUT_3_2_CORE_PROFILE  // Core profile context
                       );
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("FastRender Editor");
    
    std::string shaderPath = "shaders/";
    std::string defaultVertexShader = shaderPath + "basic.vs";
    std::string defaultFragmentShader = shaderPath + "basic.fs";

    defaultShaderProgram.loadFilesVertexFragment(defaultVertexShader, defaultFragmentShader);

#if 0
    float quad[20] = {
        // x, y, z, u, v
        //-1.0, -1.0, 1.0, 0.0, 1.0,
        // 1.0, -1.0, 1.0, 1.0, 1.0,
        //-1.0,  1.0, 1.0, 0.0, 0.0,
        // 1.0,  1.0, 1.0, 1.0, 0.0
        -0.5, -0.5, 0.5, 0.0, 1.0,
         0.5, -0.5, 0.5, 1.0, 1.0,
        -0.5,  0.5, 0.5, 0.0, 0.0,
         0.5,  0.5, 0.5, 1.0, 0.0
    };

    glGenVertexArrays( 1, &img_vao );
    glBindVertexArray( img_vao );
    glGenBuffers( 1, &img_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, img_vbo );

    auto position_loc = defaultShaderProgram.attribLocation("position");
    auto uv_loc = defaultShaderProgram.attribLocation("uv");
    glBufferData( GL_ARRAY_BUFFER, sizeof(quad), &quad[0], GL_STATIC_DRAW );
    glVertexAttribPointer( position_loc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL );
    glEnableVertexAttribArray( position_loc );
    GL_WARN_IF_ERROR();
    glVertexAttribPointer( uv_loc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)) );
    glEnableVertexAttribArray( uv_loc );
    GL_WARN_IF_ERROR();
#endif

    GL_WARN_IF_ERROR();

    glutReshapeFunc(sViewportReshaped);
    glutDisplayFunc(sRepaintViewport);
    glutKeyboardFunc(sKeyPressed);
    glutMouseFunc(sMouseButton);
    glutMotionFunc(sMouseMotionWhileButtonPressed);

    glutTimerFunc(update_rate_sec * 1000, sAnimTimer, 0);
}

void Editor::buildGpuBuffers()
{
    editorScene.buildGpuBuffers(defaultShaderProgram);
}

void Editor::start()
{
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

    //bool drawWireframes = false;
    bool drawWireframes = true;

    if(drawWireframes)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes

#if 0
    glBindVertexArray( img_vao );
    glBindBuffer( GL_ARRAY_BUFFER, img_vbo );
    defaultShaderProgram.use();
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    GL_WARN_IF_ERROR();
#endif

    // FIXME Sample transform. This should be loaded from the scene, but
    //       we currently store it in ImageTracer. So we should either
    //       move it to the Scene or pass the tracer around here.
    RandomNumberGenerator rng; // TEMP Part of this hacky mess
    SimpleCamera camera(rng, -0.15, 0.15, -0.15, 0.15, windowWidth, windowHeight);
    Transform rotation = compose( makeRotation( M_PI / 4, Vector4(0, 1, 0) ),
                                  makeRotation( -0.2, Vector4(1, 0, 0) ) );
    Transform translation = makeTranslation( 2.0, 2.0, 25.0 );
    camera.transform = compose( rotation, translation );

    defaultShaderProgram.use();
    editorScene.draw(camera, defaultShaderProgram);

    if(drawWireframes)
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled

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


