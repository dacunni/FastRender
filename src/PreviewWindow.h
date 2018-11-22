//
//  PreviewWindow.h
//  FastRender
//
//  Created by David Cunningham on 6/24/17.
//
//

#ifndef __PREVIEWWINDOW__
#define __PREVIEWWINDOW__

#include <memory>
#include <vector>

#include "OpenGLWindow.h"
#include "ShaderProgram.h"

class Artifacts;

class PreviewWindow : public OpenGLWindow {
public:
    PreviewWindow() = delete;
    PreviewWindow( Artifacts & a );
    virtual ~PreviewWindow() = default;
    
    virtual void init( const std::string & title ) override;

protected:
    // Callbacks
    virtual void viewportReshaped( int w, int h ) override;
    virtual void repaintViewport() override;
    virtual void keyPressed( unsigned char key, int x, int y ) override;
    virtual void mouseButton( MouseButton button, MouseButtonState state, int x, int y ) override;
    virtual void mouseMotionWhileButtonPressed( int x, int y ) override;

    // Timer callbacks
    void animTimer();

private:
    void printValuesAt( int win_x, int win_y );

    enum ImageArtifact {
        FramebufferImage = 0, NormalsImage, DepthImage,
        NumImageArtifacts
    };
    ImageArtifact activeImage = FramebufferImage;
    Artifacts & artifacts;

    ShaderProgram imgShaderProgram;

    GLuint img_vao       = 0;
    GLuint img_vbo       = 0;
    GLuint pixelAccumTex = 0;
    GLuint pixelCountTex = 0;

    float update_rate_sec = 1.0f / 20.0f; // 20 FPS
    
    // Image adjustments
    float gain                 = 1.0f;
    float gainAdjustMultiplier = 1.15;
    float bias                 = 0.0f;
    float biasAdjustIncrement  = 0.05;
    bool flagUnnaturalValues   = false;
};

#endif
