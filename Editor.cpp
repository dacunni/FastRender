#include <stdio.h>

#include "Editor.h"
#include "AmbientOcclusionShader.h"
#include "BasicDiffuseSpecularShader.h"
#include "ImageTracer.h"

static Editor * self = nullptr;

Editor::Editor()
    :  editCamera(rng, editCameraParams.xmin, editCameraParams.xmax,
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
    //std::string defaultFragmentShader = shaderPath + "basic.fs";
    std::string defaultFragmentShader = shaderPath + "normals.fs";

    defaultShaderProgram.loadFilesVertexFragment(defaultVertexShader, defaultFragmentShader);

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
    printf("Building GPU buffers\n");
    editorScene.buildGpuBuffers(defaultShaderProgram);
    printf("DONE Building GPU buffers\n");
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
    glEnable( GL_DEPTH_TEST );

    if(drawWireframes)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes

    double elapsedTime = runTimer.elapsed();
    defaultShaderProgram.use();
    editorScene.draw(editCamera, defaultShaderProgram);

    if(drawWireframes)
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled

    glDisable( GL_DEPTH_TEST );
    glutSwapBuffers();
}

void Editor::keyPressed( unsigned char key, int x, int y )
{
    // rotation
         if(key == 'q') { editCameraParams.az -= editCameraParams.azStep; }
    else if(key == 'e') { editCameraParams.az += editCameraParams.azStep; }
    else if(key == 'y') { editCameraParams.el -= editCameraParams.elStep; }
    else if(key == 'h') { editCameraParams.el += editCameraParams.elStep; }
    // translation
    else if(key == 'a') { editCameraParams.position.x -= editCameraParams.positionStep.x; }
    else if(key == 'd') { editCameraParams.position.x += editCameraParams.positionStep.x; }
    else if(key == 's') { editCameraParams.position.z += editCameraParams.positionStep.z; }
    else if(key == 'w') { editCameraParams.position.z -= editCameraParams.positionStep.z; }
    else if(key == 'r') { editCameraParams.position.y += editCameraParams.positionStep.y; }
    else if(key == 'f') { editCameraParams.position.y -= editCameraParams.positionStep.y; }
    // options
    else if(key == 'W') { drawWireframes = !drawWireframes; }
    // actions
    else if(key == 'R') { renderEditCameraPerspective(); return; }

    updateEditCamera();
    glutPostRedisplay();
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

Transform Editor::cameraRotation()
{
    return compose( makeRotation( editCameraParams.az, Vector4(0, 1, 0) ),
                    makeRotation( editCameraParams.el, Vector4(1, 0, 0) ) );
}

void Editor::updateEditCamera()
{
    Transform rotation = cameraRotation();
    Transform translation = makeTranslation( editCameraParams.position );
    editCamera.transform = compose( rotation, translation );
}

void Editor::renderEditCameraPerspective()
{
    printf("Rendering edit camera perspective\n");
    ImageTracer tracer(windowWidth, windowHeight, 1, 4);
    tracer.camera = editCamera;
    tracer.scene = scene.get();
    //tracer.shader = new AmbientOcclusionShader();
    tracer.shader = new BasicDiffuseSpecularShader();
    tracer.scene->buildLightList();
    tracer.render();
}

