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

Image::Image()
    : width(0),
      height(0)
{

}

Image::Image(unsigned int w, unsigned int h, unsigned int c)
    : width(w),
      height(h),
      channels(c)
{
    data.resize(width * height * channels);
}

void Image::sizeDataToConfig()
{
    data.resize(width * height * channels);
}

void Image::loadDataFromFile(const std::string & filename,
                             unsigned int w, unsigned int h)
{
    // Open file, jumping to the end
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

    if(file.is_open()) {
        std::streampos sizeBytes = file.tellg();
        width = w;
        height = h;
        channels = 3;
        assert(sizeBytes == w * h * 3 * sizeof(float));
        sizeDataToConfig();
        // Read data
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(&data[0]), sizeBytes);
        file.close();
    }
    else {
        printf("Error reading data from %s\n", filename.c_str());
        width = 0;
        height = 0;
        channels = 3;
    }
}

void Image::applyScalingUV(const std::function<float(float,float)> & scale)
{
    for(unsigned int row = 0; row < height; row++) {
        float v = (float) row / height;
        for(unsigned int col = 0; col < width; col++) {
            float u = (float) col / width;
            float s = scale(u, v);
            for(unsigned int ch = 0; ch < channels; ch++) {
                at(row, col, ch) *= s;
            }
        }
    }
}

void Image::maskUV(const std::function<bool(float,float)> & mask)
{
    for(unsigned int row = 0; row < height; row++) {
        float v = (float) row / height;
        for(unsigned int col = 0; col < width; col++) {
            float u = (float) col / width;
            if(!mask(u, v)) {
                for(unsigned int ch = 0; ch < channels; ch++) {
                    at(row, col, ch) = 0.0f;
                }
            }
        }
    }
}

unsigned int Image::rccToIndex(unsigned int row, unsigned int col, unsigned int ch) const
{
    return channels * (row * width + col) + ch;
}

void Image::clampRC(unsigned int & r, unsigned int & c) const
{
    r = std::min(r, height - 1);
    c = std::min(c, width - 1);
}

float & Image::at(unsigned int row, unsigned int col, unsigned int ch)
{
    return data[rccToIndex(row, col, ch)];
}

const float & Image::at(unsigned int row, unsigned int col, unsigned int ch) const
{
    return data[rccToIndex(row, col, ch)];
}

void Image::set(float value)
{
    for(unsigned int row = 0; row < height; row++) {
        for(unsigned int col = 0; col < width; col++) {
            for(unsigned int ch = 0; ch < channels; ch++) {
                at(row, col, ch) = value;
            }
        }
    }
}

void Image::uvToRowCol(float u, float v, float & r, float & c) const
{
    r = std::floor(v * height);
    c = std::floor(u * width);
}

