#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>

#include "HDRImage.h"
#include "Color.h"

HDRImage::HDRImage( unsigned int w, unsigned int h )
    : Image(w, h, 3)
{

}

HDRImage::HDRImage( const std::string & filename, unsigned int w, unsigned int h)
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
        channels = 3;
    }
    else {
        printf("Error reading data from %s\n", filename.c_str());
        width = 0;
        height = 0;
        channels = 3;
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

void HDRImage::set( float value )
{
    for( unsigned int row = 0; row < height; row++ ) {
        for( unsigned int col = 0; col < width; col++ ) {
            data[3 * (row * width + col) + 0] = value;
            data[3 * (row * width + col) + 1] = value;
            data[3 * (row * width + col) + 2] = value;
        }
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

void HDRImage::applyScalingUV( const std::function<float(float,float)> & scale )
{
    for( unsigned int row = 0; row < height; row++ ) {
        for( unsigned int col = 0; col < width; col++ ) {
            float v = (float) row / height;
            float u = (float) col / width;
            float s = scale(u, v);
            data[3 * (row * width + col) + 0] *= s;
            data[3 * (row * width + col) + 1] *= s;
            data[3 * (row * width + col) + 2] *= s;
        }
    }
}

RGBRadianceSample HDRImage::at( unsigned int r, unsigned int c ) const
{
    unsigned int ri = std::min( (unsigned int) r, height - 1 );
    unsigned int ci = std::min( (unsigned int) c, width - 1 );

    unsigned int offset = (ri * width + ci) * 3;

    // TODO[DAC]: Make the radiometry correct
    float scale_factor = 1.0;

    RGBRadianceSample s;
    s.color.r = data[ offset + 0 ] * scale_factor;
    s.color.g = data[ offset + 1 ] * scale_factor;
    s.color.b = data[ offset + 2 ] * scale_factor;
    return s;
}

RGBRadianceSample HDRImage::sampleRGB( float u, float v ) const
{
    RGBRadianceSample s;

    if( width == 0 || height == 0 ) {
        return s;
    }

    float row = std::floor(v * height);
    float col = std::floor(u * width);

#if 1
    // Interpolate pixels
    float rw = v * height - row;
    float cw = u * width - col;
    float omrw = 1.0f - rw;
    float omcw = 1.0f - cw;
    s = RGBRadianceSample((omrw * omcw * at(row,   col).color +
                           rw   * omcw * at(row+1, col).color +
                           rw   *   cw * at(row+1, col+1).color +
                           omrw *   cw * at(row,   col+1).color));
    return s;
#else
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
#endif
}



