#ifndef __OPENGLWINDOW_H__
#define __OPENGLWINDOW_H__

#include <functional>

#include "OpenGLUtil.h"
#include "Timer.h"

class OpenGLWindow {
public:
    OpenGLWindow();
    virtual ~OpenGLWindow();
    
    virtual void init( const std::string & title );
    void start();

protected:
    enum MouseButton { MouseLeftButton, MouseMiddleButton, MouseRightButton };
    enum MouseButtonState { MouseButtonUp, MouseButtonDown };

    virtual void swapBuffers();
    virtual void postRedisplay();

    // Callbacks
    virtual void viewportReshaped( int w, int h );
    virtual void repaintViewport();
    virtual void keyPressed( unsigned char key, int x, int y ) {}
    virtual void keyReleased( unsigned char key, int x, int y ) {}
    virtual void mouseButton( MouseButton button, MouseButtonState state, int x, int y ) {}
    virtual void mouseMotionWhileButtonPressed( int x, int y ) {}

    void addTimerCallback( unsigned int milliseconds,
                           std::function<void()> callback );

    // State queries
    bool mouseLeftDown();
    bool mouseMiddleDown();
    bool mouseRightDown();
    bool keyIsPressed(unsigned char key);

protected:
    unsigned int windowWidth = 512;
    unsigned int windowHeight = 512;
    int mouseLastX = -1;
    int mouseLastY = -1;
    WallClockTimer runTimer;

private:
    // Internal callbacks. These call the virtual equivalents after doing
    // any necessary bookkeeping.
    void internalViewportReshaped( int w, int h );
    void internalRepaintViewport();
    void internalKeyPressed( unsigned char key, int x, int y );
    void internalKeyReleased( unsigned char key, int x, int y );
    void internalMouseButton( int button, int state, int x, int y );
    void internalMouseMotionWhileButtonPressed( int x, int y );

    static void sViewportReshaped( int w, int h );
    static void sRepaintViewport();
    static void sKeyPressed( unsigned char key, int x, int y );
    static void sKeyReleased( unsigned char key, int x, int y );
    static void sMouseButton( int button, int state, int x, int y );
    static void sMouseMotionWhileButtonPressed( int x, int y );
    static void sAnimTimer( int value );

    MouseButton glutToStdMouseButton( int button );
    MouseButtonState glutToStdMouseButtonState( int state );

    int windowId = -1;

    MouseButtonState mouseButtonState[5];
    unsigned char keyState[256] = {0};
};

#endif

