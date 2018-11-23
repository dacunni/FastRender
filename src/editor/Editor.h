#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <memory>

#include "OpenGLWindow.h"
#include "EditorSceneGraph.h"
#include "ShaderProgram.h"
#include "SimpleCamera.h"
#include "RandomNumberGenerator.h"

class Editor : public OpenGLWindow {
public:
    Editor();
    virtual ~Editor();
    
    virtual void init( const std::string & title ) override;

    void buildGpuBuffers();

    std::shared_ptr<Scene> scene;
    EditorSceneGraph editorScene;

protected:
    // Callbacks
    virtual void viewportReshaped( int w, int h ) override;
    virtual void repaintViewport() override;
    virtual void keyPressed( unsigned char key, int x, int y ) override;
    virtual void keyReleased( unsigned char key, int x, int y ) override;
    virtual void mouseButton( MouseButton button, MouseButtonState state, int x, int y ) override;
    virtual void mouseMotionWhileButtonPressed( int x, int y ) override;

    // Timer callbacks
    void animTimer();
    void userTimerUpdate( double timeNow, double deltaTime );

    void renderEditCameraPerspective();

private:
    Transform cameraTranslation();
    Transform cameraRotation();
    Transform cameraTransform();
    Vector4 cameraForward();
    Vector4 cameraRight();
    Vector4 cameraUp();
    void updateEditCamera();

    ShaderProgram defaultShaderProgram;
    ShaderProgram wireMeshEdgeShaderProgram;

    float update_rate_sec = 1.0f / 20.0f; // 20 FPS
    double animTime = 0.0;

    // Editor camera
    struct EditCameraParams {
        // Transform
        Vector4 position = Vector4( 0.0, 0.0, 25.0 );
        float xRotation = 0.0;
        float yRotation = 0.0;
        // Control speed
        float translationSpeed = 5.0;
        float keyboardRotationSpeed = 0.25;
        float mouseRotationSpeed = 0.001;
        // Camera Parameters
        float xmin = -0.15;
        float xmax = +0.15;
        float ymin = -0.15;
        float ymax = +0.15;
    } editCameraParams;

    std::shared_ptr<Camera> editCamera;

    RandomNumberGenerator rng;

    bool drawWireframes = false;
};

#endif

