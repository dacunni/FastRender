/*
 *  Image.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 2/8/17.
 *
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>

#include "Image.h"
#include "Color.h"

HDRImage::HDRImage( const std::string & filename, unsigned int w, unsigned int h)
    : width(0),
      height(0)
{
    loadDataFromFile(filename, w, h);
}

void HDRImage::loadDataFromFile( const std::string & filename,
                                 unsigned int w, unsigned int h)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if( file.is_open() )
    {
        std::streampos size = file.tellg();
        assert( size == w * h * 3 * sizeof(float) );
        data.resize( size );
        file.seekg( 0, std::ios::beg );
        file.read( reinterpret_cast<char*>(&data[0]), size );
        file.close();
        width = w;
        height = h;
    }
    else {
        printf("Error reading data from %s\n", filename.c_str());
        width = 0;
        height = 0;
    }
}

void HDRImage::toGray( std::vector<float> & grayData ) const
{
    grayData.resize(width * height);
    for( auto i = 0; i < width * height; i++ ) {
        grayData[i] = (data[3 * i + 0] +
                       data[3 * i + 1] +
                       data[3 * i + 2]) / 3.0f;
    }
}

void HDRImage::maskUV( const std::function<bool(float,float)> & mask )
{
    for( unsigned int row = 0; row < height; row++ ) {
        for( unsigned int col = 0; col < width; col++ ) {
            float v = (float) row / height;
            float u = (float) col / width;
            if( !mask(u, v) ) {
                data[3 * (row * width + col) + 0] = 0.0f;
                data[3 * (row * width + col) + 1] = 0.0f;
                data[3 * (row * width + col) + 2] = 0.0f;
            }
        }
    }
}

RGBRadianceSample HDRImage::sampleRGB( float u, float v ) const
{
    RGBRadianceSample s;
    s.mask = RGB_BITS;

    if( width == 0 || height == 0 ) {
        return s;
    }

    float row = v * height;
    float col = u * width;

    unsigned int ri = std::min( (unsigned int) row, height );
    unsigned int ci = std::min( (unsigned int) col, width );

    unsigned int offset = (ri * width + ci) * 3;

    //printf("ri = %u ci = %u\n", ri, ci); // TEMP
    
    // TODO[DAC]: Make the radiometry correct
    float scale_factor = 1.0;

    // TODO[DAC]: Interpolation
    s.color.r = data[ offset + 0 ] * scale_factor;
    s.color.g = data[ offset + 1 ] * scale_factor;
    s.color.b = data[ offset + 2 ] * scale_factor;

    return s;
}



