#include <stdio.h>
#include <iostream>

#include "Editor.h"
#include "AmbientOcclusionShader.h"
#include "BasicDiffuseSpecularShader.h"
#include "DirectAreaLightShader.h"
#include "DirectEnvironmentMapShader.h"
#include "GoochShader.h"
#include "ImageTracer.h"

static Editor * self = nullptr;

Editor::Editor()
    :  editCamera(editCameraParams.xmin, editCameraParams.xmax,
                  editCameraParams.ymin, editCameraParams.ymax,
                  windowWidth, windowHeight)
{
    // FIXME: HACKHACK: Workaround for no user data pointer in GLUT. Assumes
    //        a single editor window.
    self = this;

    updateEditCamera();
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
    std::string defaultFragmentShader = shaderPath + "gooch.fs";
    //std::string defaultFragmentShader = shaderPath + "normals.fs";
    defaultShaderProgram.loadFilesVertexFragment(defaultVertexShader, defaultFragmentShader);

    std::string wireMeshEdgeVertexShader = shaderPath + "basic.vs";
    std::string wireMeshEdgeFragmentShader = shaderPath + "wireframe.fs";
    wireMeshEdgeShaderProgram.loadFilesVertexFragment(wireMeshEdgeVertexShader, wireMeshEdgeFragmentShader);

    GL_WARN_IF_ERROR();

    glutIgnoreKeyRepeat(1);

    glutReshapeFunc(sViewportReshaped);
    glutDisplayFunc(sRepaintViewport);
    glutKeyboardFunc(sKeyPressed);
    glutKeyboardUpFunc(sKeyReleased);
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
    runTimer.start();
    glutMainLoop();
}

// Static functions to use as GLUT callbacks. These delegate to their
// corresponding instance member functions in Editor.
void Editor::sViewportReshaped( int w, int h ) { self->viewportReshaped( w, h ); }
void Editor::sRepaintViewport() { self->repaintViewport(); }
void Editor::sKeyPressed( unsigned char key, int x, int y ) { self->keyPressed( key, x, y ); }
void Editor::sKeyReleased( unsigned char key, int x, int y ) { self->keyReleased(key, x, y); }
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
    glClearColor( 0.6, 0.6, 0.8, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    if(drawWireframes) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
        wireMeshEdgeShaderProgram.use();
        GLuint uColorLoc = wireMeshEdgeShaderProgram.uniformLocation("uColor");
        float gray = 0.8;
        glUniform4f(uColorLoc, gray, gray, gray, 1.0);
        editorScene.draw(editCamera, wireMeshEdgeShaderProgram);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled
    }
    else {
        // wireframe
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
        wireMeshEdgeShaderProgram.use();
        GLuint uColorLoc = wireMeshEdgeShaderProgram.uniformLocation("uColor");
        float gray = 0.1;
        glUniform4f(uColorLoc, gray, gray, gray, 1.0);
        editorScene.draw(editCamera, wireMeshEdgeShaderProgram);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled
        // solid
        defaultShaderProgram.use();
        editorScene.draw(editCamera, defaultShaderProgram);
    }

    glDisable( GL_DEPTH_TEST );
    glutSwapBuffers();
}

void Editor::keyPressed( unsigned char key, int x, int y )
{
    keyState[key] = 1;

    // options
    if(key == 'W') { drawWireframes = !drawWireframes; }
    // actions
    else if(key == 'R') { renderEditCameraPerspective(); return; }
    else if(key == 'C') { std::cout << editCamera.transform.toJSON() << std::endl; return; }

    updateEditCamera();
    glutPostRedisplay();
}

void Editor::keyReleased( unsigned char key, int x, int y ) 
{
    keyState[key] = 0;
}

void Editor::mouseButton( int button, int state, int x, int y )
{
    mouseButtonState[button] = state;
    mouseLastX = x;
    mouseLastY = y;
}

void Editor::mouseMotionWhileButtonPressed( int x, int y )
{
    int dx = x - mouseLastX;
    int dy = y - mouseLastY;

    // Rotation
    if( mouseButtonState[GLUT_LEFT_BUTTON] == GLUT_DOWN ) {
        editCameraParams.yRotation -= editCameraParams.mouseRotationSpeed * (float) dx;
        editCameraParams.xRotation -= editCameraParams.mouseRotationSpeed * (float) dy;
        glutPostRedisplay();
    }

    mouseLastX = x;
    mouseLastY = y;
}

Transform Editor::cameraTranslation()
{
    const auto & cameraPosition = editCameraParams.position;
    return makeTranslation( cameraPosition.x, cameraPosition.y, cameraPosition.z );
}

Transform Editor::cameraRotation()
{
    return compose( makeRotation( editCameraParams.yRotation, Vector4( 0, 1, 0 ) ),
                    makeRotation( editCameraParams.xRotation, Vector4( 1, 0, 0 ) ) );
}

Transform Editor::cameraTransform()
{
    return compose( cameraTranslation(), cameraRotation() );
}

Vector4 Editor::cameraForward()
{
    return mult( cameraRotation().fwd, Vector4(0, 0, -1) );
}

Vector4 Editor::cameraRight()
{
    return mult( cameraRotation().fwd, Vector4(1, 0, 0) );
}

Vector4 Editor::cameraUp()
{
    return mult( cameraRotation().fwd, Vector4(0, 1, 0) );
}

void Editor::userTimerUpdate( double timeNow, double deltaTime )
{
    // Camera controls
    //   Translation
    if( keyState['w'] ) { editCameraParams.position += cameraForward() * editCameraParams.translationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['s'] ) { editCameraParams.position -= cameraForward() * editCameraParams.translationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['a'] ) { editCameraParams.position -= cameraRight() * editCameraParams.translationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['d'] ) { editCameraParams.position += cameraRight() * editCameraParams.translationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['r'] ) { editCameraParams.position += cameraUp() * editCameraParams.translationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['f'] ) { editCameraParams.position -= cameraUp() * editCameraParams.translationSpeed * deltaTime; glutPostRedisplay(); }
    //   Rotation
    if( keyState['q'] ) { editCameraParams.yRotation += editCameraParams.keyboardRotationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['e'] ) { editCameraParams.yRotation -= editCameraParams.keyboardRotationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['y'] ) { editCameraParams.xRotation += editCameraParams.keyboardRotationSpeed * deltaTime; glutPostRedisplay(); }
    if( keyState['h'] ) { editCameraParams.xRotation -= editCameraParams.keyboardRotationSpeed * deltaTime; glutPostRedisplay(); }
}

void Editor::animTimer( int value )
{
    double elapsedTime = runTimer.elapsed();
    double deltaTime = elapsedTime - animTime;
    animTime = (float) elapsedTime;

    userTimerUpdate( elapsedTime, deltaTime );
    updateEditCamera();

    glutPostRedisplay();
    glutTimerFunc( update_rate_sec * 1000, sAnimTimer, 0 );
}

void Editor::updateEditCamera()
{
    editCamera.transform = cameraTransform();
}

void Editor::renderEditCameraPerspective()
{
    printf("Rendering edit camera perspective\n");
    //ImageTracer tracer(windowWidth, windowHeight, 1, 1);
    //ImageTracer tracer(windowWidth, windowHeight, 1, 4);
    ImageTracer tracer(windowWidth, windowHeight, 1, 30);
    //ImageTracer tracer(windowWidth, windowHeight, 1, 100);
    tracer.camera = &editCamera;
    tracer.scene = scene.get();
    //tracer.shader = new GoochShader();
    //tracer.shader = new AmbientOcclusionShader();
    //tracer.shader = new DirectAreaLightShader();
    //tracer.shader = new DirectEnvironmentMapShader();
    tracer.shader = new BasicDiffuseSpecularShader();
    tracer.scene->buildLightList();
    tracer.render();
}

