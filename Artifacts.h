//
//  Artifacts.h
//  FastRender
//
//  Created by David Cunningham on 8/23/13.
//
//

#ifndef __FastRender__Artifacts__
#define __FastRender__Artifacts__

#include <vector>

// ImageMagick stuff. We get compile-time warnings if we don't define these manually
#define MAGICKCORE_QUANTUM_DEPTH 16
#define MAGICKCORE_HDRI_ENABLE 0
#include <Magick++.h>

class Vector4;

class Artifacts {
public:
    Artifacts( unsigned int imageWidth, unsigned int imageHeight );
    virtual ~Artifacts();
    
    void startNewFrame();
    void setPixelColorMono( unsigned int row, unsigned int col, float value );
    void setPixelColorRGB( unsigned int row, unsigned int col, float r, float g, float b );
    void setPixelNormal( unsigned int row, unsigned int col, const Vector4 & n );
    void setPixelDepth( unsigned int row, unsigned int col, float depth );
    void setPixelTime( unsigned int row, unsigned int col, float value );
    void flush();
    
    std::string output_path;
    Magick::Image * image = nullptr;
    Magick::Image * normal_image = nullptr;
    Magick::Image * depth_image = nullptr;
    Magick::Image * time_image = nullptr;
    std::vector<double> time_unnormalized_image;
    FILE * intersections_file;

    unsigned int width;
    unsigned int height;
    unsigned int frame_number;
    
};

#endif /* defined(__FastRender__Artifacts__) */
