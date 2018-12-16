#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>

#include "HDRImage.h"
#include "RGBImage.h"
#include "Color.h"

HDRImage::HDRImage(unsigned int w, unsigned int h)
    : Image(w, h, 3)
{

}

HDRImage::HDRImage(const std::string & filename, unsigned int w, unsigned int h)
{
    loadDataFromFile(filename, w, h);
}

void HDRImage::toGray(std::vector<float> & grayData) const
{
    grayData.resize(width * height);
    for(auto i = 0; i < width * height; i++) {
        grayData[i] = (data[3 * i + 0] +
                       data[3 * i + 1] +
                       data[3 * i + 2]) / 3.0f;
    }
}

RGBRadianceSample HDRImage::at(unsigned int r, unsigned int c) const
{
    clampRC(r, c);

    RGBColor color(Image::at(r, c, 0),
                   Image::at(r, c, 1),
                   Image::at(r, c, 2));

    float scale_factor = 1.0; // TODO[DAC]: Make the radiometry correct

    RGBRadianceSample s;
    s.color = color.scaled(scale_factor);
    return s;
}

RGBRadianceSample HDRImage::sampleRGB(float u, float v) const
{
    if(width == 0 || height == 0) {
        return RGBRadianceSample();
    }

    float row = std::floor(v * height);
    float col = std::floor(u * width);

#if 1
    // Interpolate pixels
    float rw = v * height - row;
    float cw = u * width - col;
    float omrw = 1.0f - rw;
    float omcw = 1.0f - cw;
    return RGBRadianceSample((omrw * omcw * at(row,   col).color +
                              rw   * omcw * at(row+1, col).color +
                              rw   *   cw * at(row+1, col+1).color +
                              omrw *   cw * at(row,   col+1).color));
#else
    return at(row, col);
#endif
}



