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

// ImageMagick stuff. We get compile-time warnings if we don't define these manually
#define MAGICKCORE_QUANTUM_DEPTH 16
#define MAGICKCORE_HDRI_ENABLE 0
#include <Magick++.h>

#include "Types.h"

class Vector4;

class Artifacts {
public:
    Artifacts( unsigned int imageWidth, unsigned int imageHeight );
    virtual ~Artifacts();
    
    void startNewFrame();
    void resetImages();
    void accumPixelColorMono( unsigned int row, unsigned int col, float value );
    void accumPixelColorRGB( unsigned int row, unsigned int col, float r, float g, float b );
    void setPixelNormal( unsigned int row, unsigned int col, const Vector4 & n );
    void setPixelDepth( unsigned int row, unsigned int col, float depth );
    void accumPixelTime( unsigned int row, unsigned int col, float value );
    void setPixelTime( unsigned int row, unsigned int col, float value );
    void flush();

    typedef std::unique_ptr<Magick::Image> ImagePtr;
    
    std::string output_path;
    ImagePtr image = nullptr;
    ImagePtr normal_image = nullptr;
    ImagePtr depth_image = nullptr;
    ImagePtr time_image = nullptr;
    ImagePtr stddev_image = nullptr;
    std::vector<float3> pixel_color_accum;
    std::vector<float3> pixel_color_sq_accum; // squares of pixel values for variance calculation
    std::vector<unsigned long> pixel_color_num_samples;
    std::vector<float3> pixel_normal;
    std::vector<float> pixel_depth;
    std::vector<double> time_unnormalized_image;
    FILE * intersections_file;

    unsigned int width;
    unsigned int height;
    unsigned int frame_number;

    std::string file_prefix = "";
    
};

#endif /* defined(__FastRender__Artifacts__) */
