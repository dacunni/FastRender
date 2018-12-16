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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "RGBImage.h"
#include "Color.h"

RGBImage::RGBImage()
    : Image()
{

}

RGBImage::RGBImage(unsigned int w, unsigned int h)
    : Image(w, h, 3)
{

}

RGBImage::RGBImage(const std::string & filename, unsigned int w, unsigned int h)
{
    loadDataFromRawFile(filename, w, h);
}

RGBImage::RGBImage(const std::string & filename)
{
    loadDataFromImageFile(filename);
}

RGBImage::~RGBImage()
{

}

void RGBImage::loadDataFromRawFile(const std::string & filename,
                                    unsigned int w, unsigned int h)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open())
    {
        std::streampos size = file.tellg();
        assert(size == w * h * 3 * sizeof(float));
        data.resize(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(&data[0]), size);
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

void RGBImage::loadDataFromImageFile(const std::string & filename)
{
    int w = 0, h = 0, numComponents = 3;

#if 1
    unsigned char * stbiData = stbi_load(filename.c_str(), &w, &h, &numComponents, 3);
    const unsigned int numElements = w * h * 3;
    data.resize(numElements);

    for(unsigned int pi = 0; pi < numElements; pi++) {
        data[pi] = (float) stbiData[pi] / 255.0f;
    }
#elif 1
    // TODO[DAC]: Compare the correctness of stbi_loadf(), since it does gamma correction
    //            for LDR images.
    float * stbiData = stbi_loadf(filename.c_str(), &w, &h, &numComponents, 3);
    const unsigned int numElements = w * h * 3;
    data.resize(numElements);
    std::copy(stbiData, stbiData + numElements, data.begin());
#endif
    stbi_image_free(stbiData);

    width = w;
    height = h;
}

RGBColor RGBImage::sampleRGB(float u, float v) const
{
    RGBColor s;

    if(width == 0 || height == 0) {
        return s;
    }

    float row = v * height;
    float col = u * width;

    unsigned int ri = std::min((unsigned int) row, height);
    unsigned int ci = std::min((unsigned int) col, width);

    unsigned int offset = (ri * width + ci) * 3;

    // TODO[DAC]: Interpolation
    s.r = data[ offset + 0 ];
    s.g = data[ offset + 1 ];
    s.b = data[ offset + 2 ];

    return s;
}



