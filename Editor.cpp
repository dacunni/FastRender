#include <stdio.h>
#include <iostream>

#include "Editor.h"
#include "AmbientOcclusionShader.h"
#include "BasicDiffuseSpecularShader.h"
#include "DirectAreaLightShader.h"
#include "DirectEnvironmentMapShader.h"
#include "GoochShader.h"
#include "ImageTracer.h"

Editor::Editor()
{
    editCamera = std::make_shared<SimpleCamera>(editCameraParams.xmin, editCameraParams.xmax,
                                                editCameraParams.ymin, editCameraParams.ymax,
                                                windowWidth, windowHeight);
    updateEditCamera();
}

Editor::~Editor()
{

}

void Editor::init(const std::string & title) 
{
    printf("Initializing editor window\n");

    OpenGLWindow::init(title);

    std::string shaderPath = "shaders/";

    // Solid rendering shader
    std::string defaultVertexShader = shaderPath + "basic.vs";
    std::string defaultFragmentShader = shaderPath + "gooch.fs";
    //std::string defaultFragmentShader = shaderPath + "normals.fs";
    defaultShaderProgram.loadFilesVertexFragment(defaultVertexShader, defaultFragmentShader);

    // Wire mesh shader
    std::string wireMeshEdgeVertexShader = shaderPath + "basic.vs";
    std::string wireMeshEdgeFragmentShader = shaderPath + "wireframe.fs";
    wireMeshEdgeShaderProgram.loadFilesVertexFragment(wireMeshEdgeVertexShader, wireMeshEdgeFragmentShader);

    GL_WARN_IF_ERROR();

    addTimerCallback( update_rate_sec * 1000, [&](){ this->animTimer(); } );
}

void Editor::buildGpuBuffers()
{
    editorScene.buildGpuBuffers(defaultShaderProgram);
}

// Callback

void Editor::viewportReshaped( int w, int h )
{
    glViewport( 0, 0, w, h );
    postRedisplay();
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
        editorScene.draw(*editCamera, wireMeshEdgeShaderProgram);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled
    }
    else {
        // wireframe
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  // Draw polygons as wireframes
        wireMeshEdgeShaderProgram.use();
        GLuint uColorLoc = wireMeshEdgeShaderProgram.uniformLocation("uColor");
        float gray = 0.1;
        glUniform4f(uColorLoc, gray, gray, gray, 1.0);
        editorScene.draw(*editCamera, wireMeshEdgeShaderProgram);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  // Draw polygons filled
        // solid
        defaultShaderProgram.use();
        editorScene.draw(*editCamera, defaultShaderProgram);
    }

    glDisable( GL_DEPTH_TEST );
    swapBuffers();
}

void Editor::keyPressed( unsigned char key, int x, int y )
{
    // options
    if(key == 'W') { drawWireframes = !drawWireframes; }
    // actions
    else if(key == 'R') { renderEditCameraPerspective(); return; }
    else if(key == 'C') { std::cout << editCamera->transform.toJSON() << std::endl; return; }

    updateEditCamera();
    postRedisplay();
}

void Editor::keyReleased( unsigned char key, int x, int y ) 
{

}

void Editor::mouseButton( MouseButton button, MouseButtonState state, int x, int y )
{

}

void Editor::mouseMotionWhileButtonPressed( int x, int y )
{
    int dx = x - mouseLastX;
    int dy = y - mouseLastY;

    // Rotation
    if( mouseLeftDown() ) {
        editCameraParams.yRotation -= editCameraParams.mouseRotationSpeed * (float) dx;
        editCameraParams.xRotation -= editCameraParams.mouseRotationSpeed * (float) dy;
        postRedisplay();
    }
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
    if( keyIsPressed('w') ) { editCameraParams.position += cameraForward() * editCameraParams.translationSpeed * deltaTime; postRedisplay(); }
    if( keyIsPressed('s') ) { editCameraParams.position -= cameraForward() * editCameraParams.translationSpeed * deltaTime; postRedisplay(); }
    if( keyIsPressed('a') ) { editCameraParams.position -= cameraRight() * editCameraParams.translationSpeed * deltaTime; postRedisplay(); }
    if( keyIsPressed('d') ) { editCameraParams.position += cameraRight() * editCameraParams.translationSpeed * deltaTime; postRedisplay(); }
    if( keyIsPressed('r') ) { editCameraParams.position += cameraUp() * editCameraParams.translationSpeed * deltaTime; postRedisplay(); }
    if( keyIsPressed('f') ) { editCameraParams.position -= cameraUp() * editCameraParams.translationSpeed * deltaTime; postRedisplay(); }
    //   Rotation
    if( keyIsPressed('q') ) { editCameraParams.yRotation += editCameraParams.keyboardRotationSpeed * deltaTime; postRedisplay(); }
    if( keyIsPressed('e') ) { editCameraParams.yRotation -= editCameraParams.keyboardRotationSpeed * deltaTime; postRedisplay(); }
    if( keyIsPressed('y') ) { editCameraParams.xRotation += editCameraParams.keyboardRotationSpeed * deltaTime; postRedisplay(); }
    if( keyIsPressed('h') ) { editCameraParams.xRotation -= editCameraParams.keyboardRotationSpeed * deltaTime; postRedisplay(); }
}

void Editor::animTimer()
{
    double elapsedTime = runTimer.elapsed();
    double deltaTime = elapsedTime - animTime;
    animTime = (float) elapsedTime;

    userTimerUpdate( elapsedTime, deltaTime );
    updateEditCamera();

    postRedisplay();
    addTimerCallback( update_rate_sec * 1000, [&](){ this->animTimer(); } );
}

void Editor::updateEditCamera()
{
    editCamera->transform = cameraTransform();
}

void Editor::renderEditCameraPerspective()
{
    printf("Rendering edit camera perspective\n");
    //ImageTracer tracer(windowWidth, windowHeight, 1, 1);
    //ImageTracer tracer(windowWidth, windowHeight, 1, 4);
    ImageTracer tracer(windowWidth, windowHeight, 1, 30);
    //ImageTracer tracer(windowWidth, windowHeight, 1, 100);
    tracer.camera = editCamera;
    tracer.scene = scene.get();
    //tracer.shader = new GoochShader();
    //tracer.shader = new AmbientOcclusionShader();
    //tracer.shader = new DirectAreaLightShader();
    //tracer.shader = new DirectEnvironmentMapShader();
    tracer.shader = new BasicDiffuseSpecularShader();
    tracer.scene->buildLightList();
    tracer.render();
}

