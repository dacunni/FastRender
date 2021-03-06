//
//  PreviewWindow.cpp
//  FastRender
//
//  Created by David Cunningham on 6/24/17.
//
//

#include "Artifacts.h"
#include "PreviewWindow.h"

PreviewWindow::PreviewWindow( Artifacts & a )
: artifacts(a)
{

}

void PreviewWindow::init(const std::string & title) 
{
    printf("Initializing preview window\n");

    windowWidth = artifacts.width;
    windowHeight = artifacts.height;

    OpenGLWindow::init(title);
    
    // Create shaders
    std::string vertexShader = R"glsl(
        #version 410
        layout (location=0) in vec4 position;
        layout (location=1) in vec2 uv;
        out vec2 vUV;
        
        void main() {
            gl_Position = position;
            vUV = uv;
        }
    )glsl";
    std::string fragmentShader = R"glsl(
        #version 410
        uniform sampler2D pixelAccum;
        uniform usampler2D pixelCount;
        uniform bool divideByCount;
        uniform bool grayInRed;
        uniform float gain;
        uniform float bias;
        uniform bool flagUnnatural;
        in vec2 vUV;
        out vec4 color;

        // TODO - make controls for these
        const bool showCalibrationOverlay = false;
        const bool doGammaCorrection = false;
         
        void main() {
            if( grayInRed ) {
                float gray = texture( pixelAccum, vUV ).r;
                color.rgb = vec3( gray, gray, gray );
            }
            else {
                color = texture( pixelAccum, vUV );
            }
    
            if( divideByCount ) {
                // Divide pixel accumulation buffer by sample count to get pixel color
                color = color / texture( pixelCount, vUV ).r;
            }

            if( showCalibrationOverlay ) {
                if( vUV.t > 0.95 ) {
                    // gradient
                    color.rgb = vec3( vUV.s );
                }
                else if( vUV.t > 0.9 ) {
                    // gamma check
                    if( vUV.t > 0.915 && vUV.t < 0.935 ) {
                        // middle gray
                        color.rgb = vec3( 0.5 );
                    }
                    else {
                        // checkerboard with average value of middle gray
                        bvec2 odd = bvec2( ivec2( gl_FragCoord.xy ) % 2 );
                        color.rgb = vec3( odd.x ^^ odd.y );
                    }
                }
            }

            // Image adjustments
            color.rgb = (color.rgb + bias) * gain;

            // Gamma correction
            if( doGammaCorrection ) {
                color.rgb = pow( color.rgb, vec3(1 / 2.4) );
            }

            if( flagUnnatural ) {
                if( any(lessThan(color.rgb, vec3(0.0))) ) {
                    color.rgb = vec3(1.0, 0.0, 1.0);
                }
                if( any(isnan(color.rgb)) ) {
                    color.rgb = vec3(1.0, 1.0, 0.0);
                }
                if( any(isinf(color.rgb)) ) {
                    color.rgb = vec3(0.0, 1.0, 1.0);
                }
            }

            color.a = 1.0;
        }
    )glsl";

    imgShaderProgram.loadSourceVertexFragment(vertexShader, fragmentShader);

    imgShaderProgram.use();

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

    auto position_loc = imgShaderProgram.attribLocation("position");
    auto uv_loc       = imgShaderProgram.attribLocation("uv");
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
    GLint pixelAccumLoc = glGetUniformLocation(imgShaderProgram.id, "pixelAccum");
    glUniform1i(pixelAccumLoc, 0);
    GL_WARN_IF_ERROR();

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, artifacts.width, artifacts.height,
                  0, GL_RGB, GL_FLOAT, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glGenTextures( 1, &pixelCountTex );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, pixelCountTex );
    GLint pixelCountLoc = glGetUniformLocation(imgShaderProgram.id, "pixelCount");
    glUniform1i(pixelCountLoc, 1);
    GL_WARN_IF_ERROR();

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, artifacts.width, artifacts.height,
                  0, GL_RED, GL_UNSIGNED_INT, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    GL_WARN_IF_ERROR();

    addTimerCallback( update_rate_sec * 1000, [&](){ this->animTimer(); } );
}

// Callback

void PreviewWindow::viewportReshaped( int w, int h )
{
    glViewport( 0, 0, w, h );
}

void PreviewWindow::repaintViewport()
{
    // Update texture with pixel accumulation buffer
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, pixelAccumTex );
    switch( activeImage ) {
        case FramebufferImage:
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, artifacts.width, artifacts.height,
                          0, GL_RGB, GL_FLOAT, &artifacts.pixel_color_accum[0] );
            break;
        case NormalsImage:
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, artifacts.width, artifacts.height,
                          0, GL_RGB, GL_FLOAT, &artifacts.pixel_normal[0] );
            break;
        case DepthImage:
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, artifacts.width, artifacts.height,
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
        glUniform1i( glGetUniformLocation( imgShaderProgram.id, "divideByCount" ), 1 );
    }
    else {
        glUniform1i( glGetUniformLocation( imgShaderProgram.id, "divideByCount" ), 0 );
    }
    GL_WARN_IF_ERROR();
    if( activeImage == DepthImage ) {
        glUniform1i( glGetUniformLocation( imgShaderProgram.id, "grayInRed" ), 1 );
    }
    else {
        glUniform1i( glGetUniformLocation( imgShaderProgram.id, "grayInRed" ), 0 );
    }
    GL_WARN_IF_ERROR();

    glUniform1f( glGetUniformLocation( imgShaderProgram.id, "gain" ), gain );
    glUniform1f( glGetUniformLocation( imgShaderProgram.id, "bias" ), bias );
    glUniform1i( glGetUniformLocation( imgShaderProgram.id, "flagUnnatural" ), (int) flagUnnaturalValues );

    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //glEnable( GL_DEPTH_TEST );
    
    glBindVertexArray( img_vao );
    glBindBuffer( GL_ARRAY_BUFFER, img_vbo );
    imgShaderProgram.use();
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    GL_WARN_IF_ERROR();

    //glDisable( GL_DEPTH_TEST );
    swapBuffers();
}

void PreviewWindow::keyPressed( unsigned char key, int x, int y )
{
    switch(key) {
        // Cycle through images
        case ' ':
            activeImage = (ImageArtifact) ((int) (activeImage + 1) % NumImageArtifacts);
            postRedisplay();
            break;
        // Multiplicative image adjustment
        case 'q':
            gain /= gainAdjustMultiplier;
            postRedisplay();
            break;
        case 'w':
            gain *= gainAdjustMultiplier;
            postRedisplay();
            break;
        // Additive image adjustment
        case 'a':
            bias -= biasAdjustIncrement;
            postRedisplay();
            break;
        case 's':
            bias += biasAdjustIncrement;
            postRedisplay();
            break;
        // Flag values that are impossible (negatives, ...)
        case 'f':
            flagUnnaturalValues = !flagUnnaturalValues;
            postRedisplay();
            break;
    }
}

void PreviewWindow::printValuesAt( int win_x, int win_y )
{
    int img_x = int(float(win_x) / float(windowWidth) * float(artifacts.width));
    int img_y = int(float(win_y) / float(windowHeight) * float(artifacts.height));
    int index = img_y * artifacts.width + img_x;

    float3 color_accum = artifacts.pixel_color_accum[index];
    float color_num_samples = artifacts.pixel_color_num_samples[index];
    float3 color(color_accum.r / color_num_samples,
                 color_accum.g / color_num_samples,
                 color_accum.b / color_num_samples);
    float3 normal = artifacts.pixel_normal[index];
    float depth = artifacts.pixel_depth[index];
    printf("win %3d %3d img %4d %4d : ", win_x, win_y, img_x, img_y);
    printf("color %f %f %f  ", color.r, color.g, color.b);
    printf("normal %f %f %f  ", normal.x, normal.y, normal.z);
    printf("depth %f\n", depth);
}

void PreviewWindow::mouseButton( MouseButton button, MouseButtonState state, int x, int y )
{
    if(button == MouseLeftButton && state == MouseButtonDown) {
        printValuesAt( x, y );
    }
}

void PreviewWindow::mouseMotionWhileButtonPressed( int x, int y )
{
    if(mouseLeftDown()) {
        printValuesAt( x, y );
    }
}

void PreviewWindow::animTimer()
{
    postRedisplay();
    addTimerCallback( update_rate_sec * 1000, [&](){ this->animTimer(); } );
}


