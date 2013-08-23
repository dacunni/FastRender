//
//  Artifacts.h
//  FastRender
//
//  Created by David Cunningham on 8/23/13.
//
//

#ifndef __FastRender__Artifacts__
#define __FastRender__Artifacts__

#include <Magick++.h>

class Vector4;

class Artifacts {
public:
    Artifacts( unsigned int imageWidth, unsigned int imageHeight );
    virtual ~Artifacts();
    
    void setPixelColorMono( unsigned int row, unsigned int col, float value );
    void setPixelNormal( unsigned int row, unsigned int col, const Vector4 & n );
    void setPixelDepth( unsigned int row, unsigned int col, float depth );
    void flush();
    
    std::string output_path;
    Magick::Image * image = nullptr;
    Magick::Image * normal_image = nullptr;
    Magick::Image * depth_image = nullptr;
    FILE * intersections_file;
    
};

#endif /* defined(__FastRender__Artifacts__) */
