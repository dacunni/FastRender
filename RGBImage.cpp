/*
 *  RGBImage.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 2/8/17.
 *
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>

// ImageMagick stuff. We get compile-time warnings if we don't define these manually
#define MAGICKCORE_QUANTUM_DEPTH 16
#define MAGICKCORE_HDRI_ENABLE 0

#include <Magick++.h>
#include "RGBImage.h"
#include "Color.h"

RGBImage::RGBImage()
    : Image()
{

}

RGBImage::RGBImage( unsigned int w, unsigned int h )
    : Image(w, h, 3)
{

}

RGBImage::RGBImage( const std::string & filename, unsigned int w, unsigned int h )
{
    loadDataFromRawFile(filename, w, h);
}

RGBImage::RGBImage( const std::string & filename )
{
    loadDataFromImageFile(filename);
}

RGBImage::~RGBImage()
{

}

void RGBImage::loadDataFromRawFile( const std::string & filename,
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

void RGBImage::loadDataFromImageFile( const std::string & filename )
{
    Magick::Image imImage;
    imImage.read( filename );

    const unsigned int w = imImage.columns();
    const unsigned int h = imImage.rows();
    const unsigned int numPixels = w * h;
    const unsigned short pixelMax = (1 << MAGICKCORE_QUANTUM_DEPTH) - 1;

    const Magick::PixelPacket * pixelCache = imImage.getPixels(0, 0, w, h);
    data.resize(numPixels * 3);

    // TEMP >>>
    //Magick::Image sanity( Magick::Geometry(w, h), "black" );
    //sanity.magick("png");
    //Magick::PixelPacket * sanityCache = sanity.setPixels(0, 0, w, h);
    // TEMP <<<

    for( unsigned int pi = 0; pi < numPixels; pi++ ) {
        data[pi * 3 + 0] = (float) pixelCache[pi].red / pixelMax;
        data[pi * 3 + 1] = (float) pixelCache[pi].green / pixelMax;
        data[pi * 3 + 2] = (float) pixelCache[pi].blue / pixelMax;
        // TEMP >>>
        //sanityCache[pi].red = data[pi * 3 + 0] * pixelMax;
        //sanityCache[pi].green = data[pi * 3 + 1] * pixelMax;
        //sanityCache[pi].blue = data[pi * 3 + 2] * pixelMax;
        // TEMP <<<
    }
    // TEMP >>>
    //sanity.write("sanity.png");
    // TEMP <<<

    width = w;
    height = h;
}

RGBColor RGBImage::sampleRGB( float u, float v ) const
{
    RGBColor s;

    if( width == 0 || height == 0 ) {
        return s;
    }

    float row = v * height;
    float col = u * width;

    unsigned int ri = std::min( (unsigned int) row, height );
    unsigned int ci = std::min( (unsigned int) col, width );

    unsigned int offset = (ri * width + ci) * 3;

    // TODO[DAC]: Interpolation
    s.r = data[ offset + 0 ];
    s.g = data[ offset + 1 ];
    s.b = data[ offset + 2 ];

    return s;
}



