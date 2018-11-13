//
//  Artifacts.h
//  FastRender
//
//  Created by David Cunningham on 8/23/13.
//
//

#ifndef __FastRender__Artifacts__
#define __FastRender__Artifacts__

#include <memory>
#include <vector>
#include <string>

#include "Types.h"

class Logger;
class Vector4;
class RGBColor;

class Artifacts {
public:
    Artifacts( unsigned int imageWidth, unsigned int imageHeight );
    virtual ~Artifacts();
    
    void startNewFrame();
    void resetImages();
    void resetPixelColor( unsigned int row, unsigned int col );
    void accumPixelColorMono( unsigned int row, unsigned int col, float value );
    void accumPixelColorRGB( unsigned int row, unsigned int col, float r, float g, float b );
    void setAccumPixelColor( unsigned int row, unsigned int col, const RGBColor & color_sum, const RGBColor & color_sq_sum, unsigned int num_samples );
    void setPixelNormal( unsigned int row, unsigned int col, const Vector4 & n );
    void setPixelDepth( unsigned int row, unsigned int col, float depth );
    void accumPixelTime( unsigned int row, unsigned int col, float value );
    void setPixelTime( unsigned int row, unsigned int col, float value );
    void flush();

    float pixelMaxChannelVariance( unsigned int row, unsigned int col );

    std::string output_path;
    std::vector<float3> pixel_color_accum;
    std::vector<float3> pixel_color_sq_accum; // squares of pixel values for variance calculation
    std::vector<unsigned int> pixel_color_num_samples;
    std::vector<float3> pixel_normal;
    std::vector<float> pixel_depth;
    std::vector<double> time_unnormalized_image;
    FILE * intersections_file;

    unsigned int width;
    unsigned int height;
    unsigned int frame_number;

    std::string file_prefix = "";
    
    Logger & logger;
};

#endif /* defined(__FastRender__Artifacts__) */
