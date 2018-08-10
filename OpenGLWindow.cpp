#include <stdio.h>
#include <iostream>
#include <mutex>
#include <map>
#include <typeinfo>

#include "OpenGLWindow.h"
#include "AmbientOcclusionShader.h"
#include "BasicDiffuseSpecularShader.h"
#include "DirectAreaLightShader.h"
#include "DirectEnvironmentMapShader.h"
#include "GoochShader.h"
#include "ImageTracer.h"

// Some GLUT implementations don't provide a user data pointer
// that we could use to pass the 'this' pointer to our static
// callbacks to map them to the object we need to call our
// method callbacks on. To work around this, we keep a mapping
// from window ID to self pointer and use it in the callbacks.
static std::map<int, OpenGLWindow *> windowIdToSelfPtr;

static OpenGLWindow * self()
{
    int id = glutGetWindow();
    auto me = windowIdToSelfPtr[id];
    return me;
}

// ------------------------------------------------------------------

OpenGLWindow::OpenGLWindow()
{
    std::fill(mouseButtonState, &mouseButtonState[4], MouseButtonUp);
}

OpenGLWindow::~OpenGLWindow()
{

}

void OpenGLWindow::init( const std::string & title ) 
{
    // Dummy params for glutInit()
    int argc = 1;
    const char *argv[] = { "" };

    glutInit( &argc, const_cast<char **>(argv) );
    glutInitDisplayMode(GLUT_DOUBLE              // Double buffered
                        | GLUT_RGBA | GLUT_DEPTH
#ifdef __APPLE__
                        | GLUT_3_2_CORE_PROFILE  // Core profile context
#endif
                       );
#ifdef FREEGLUT
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextVersion(4, 1);
    //glutInitContextFlags(GLUT_DEBUG);
#endif
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    windowId = glutCreateWindow(title.c_str());
    windowIdToSelfPtr[windowId] = this;

    GL_WARN_IF_ERROR();

    std::string glVersion   = (const char *) glGetString(GL_VERSION);
    std::string glslVersion = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);

    std::cout << "OpenGL version: " << glVersion << std::endl;
    std::cout << "GLSL version:   " << glVersion << std::endl;

    glutIgnoreKeyRepeat(1);

    glutReshapeFunc(sViewportReshaped);
    glutDisplayFunc(sRepaintViewport);
    glutKeyboardFunc(sKeyPressed);
    glutKeyboardUpFunc(sKeyReleased);
    glutMouseFunc(sMouseButton);
    glutMotionFunc(sMouseMotionWhileButtonPressed);
}

void OpenGLWindow::start()
{
    runTimer.start();
    glutMainLoop();
}

void OpenGLWindow::swapBuffers()
{
    glutSwapBuffers();
}

void OpenGLWindow::postRedisplay()
{
    glutPostRedisplay();
}

// Static functions to use as GLUT callbacks. These delegate to their
// corresponding instance member functions in OpenGLWindow.
void OpenGLWindow::sViewportReshaped( int w, int h )                   { self()->internalViewportReshaped( w, h ); }
void OpenGLWindow::sRepaintViewport()                                  { self()->internalRepaintViewport(); }
void OpenGLWindow::sKeyPressed( unsigned char key, int x, int y )      { self()->internalKeyPressed( key, x, y ); }
void OpenGLWindow::sKeyReleased( unsigned char key, int x, int y )     { self()->internalKeyReleased(key, x, y); }
void OpenGLWindow::sMouseButton( int button, int state, int x, int y ) { self()->internalMouseButton( button, state, x, y ); }
void OpenGLWindow::sMouseMotionWhileButtonPressed( int x, int y )      { self()->internalMouseMotionWhileButtonPressed( x, y ); }

// Callback

void OpenGLWindow::viewportReshaped( int w, int h )
{
    glViewport( 0, 0, w, h );
    glutPostRedisplay();
}

void OpenGLWindow::internalViewportReshaped( int w, int h )
{
    windowWidth = w;
    windowHeight = h;
    viewportReshaped(w, h);
}

void OpenGLWindow::repaintViewport()
{
    glClearColor( 0.5, 0.0, 0.0, 1.0 );
    glClear(GL_COLOR_BUFFER_BIT);
    swapBuffers();
}

void OpenGLWindow::internalRepaintViewport()
{
    repaintViewport();
}

void OpenGLWindow::internalKeyPressed( unsigned char key, int x, int y )
{
    keyPressed(key, x, y);
    keyState[key] = 1;
}

void OpenGLWindow::internalKeyReleased( unsigned char key, int x, int y ) 
{
    keyReleased(key, x, y);
    keyState[key] = 0;
}

void OpenGLWindow::internalMouseButton( int button, int state, int x, int y )
{
    MouseButton stdButton = glutToStdMouseButton(button);
    MouseButtonState stdState = glutToStdMouseButtonState(state);
    mouseButtonState[stdButton] = stdState;
    mouseButton(stdButton, stdState, x, y);
    mouseLastX = x;
    mouseLastY = y;
}

void OpenGLWindow::internalMouseMotionWhileButtonPressed( int x, int y )
{
    mouseMotionWhileButtonPressed(x, y);
    mouseLastX = x;
    mouseLastY = y;
}

// Lookup table for mapping GLUT callback key to the registered
// callback function. When a callback is registered, we register
// a unique key with GLUT so we can retrieve the callback function
// via this LUT.
static std::map<int, std::function<void()>> timerCallbackLUT;
static std::mutex timerCallbackLUTMutex;

void OpenGLWindow::sAnimTimer( int value )
{
    std::function<void()> callback;
    {   // scope-locked region
        std::lock_guard<std::mutex> guard(timerCallbackLUTMutex);
        auto it = timerCallbackLUT.find(value);
        if(it == timerCallbackLUT.end()) {
            printf("Error looking up callback from key %d\n", value);
            return;
        }
        callback = it->second;
        timerCallbackLUT.erase(it);
    }
    callback();
}

void OpenGLWindow::addTimerCallback( unsigned int milliseconds,
                                     std::function<void()> callback )
{
    std::lock_guard<std::mutex> guard(timerCallbackLUTMutex);
    static int nextKey = 0;
    timerCallbackLUT[nextKey] = callback;
    glutTimerFunc( milliseconds, sAnimTimer, nextKey );
    ++nextKey;
}

// State queries

bool OpenGLWindow::mouseLeftDown()                 { return mouseButtonState[MouseLeftButton] == MouseButtonDown; }
bool OpenGLWindow::mouseMiddleDown()               { return mouseButtonState[MouseMiddleButton] == MouseButtonDown; }
bool OpenGLWindow::mouseRightDown()                { return mouseButtonState[MouseRightButton] == MouseButtonDown; }
bool OpenGLWindow::keyIsPressed(unsigned char key) { return !!keyState[key]; }

OpenGLWindow::MouseButton OpenGLWindow::glutToStdMouseButton( int button )
{
    switch(button) {
        case GLUT_LEFT_BUTTON:   return MouseLeftButton;
        case GLUT_MIDDLE_BUTTON: return MouseMiddleButton;
        case GLUT_RIGHT_BUTTON:  return MouseRightButton;
        default:                 return MouseLeftButton;
    }
}

OpenGLWindow::MouseButtonState OpenGLWindow::glutToStdMouseButtonState( int state )
{
    switch(state) {
        case GLUT_UP:   return MouseButtonUp;
        case GLUT_DOWN: return MouseButtonDown;
        default: return MouseButtonUp;
    }
}


