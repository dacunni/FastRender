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

Image::Image( unsigned int w, unsigned int h, unsigned int c )
    : width(w),
      height(h),
      channels(c)
{
    data.resize(width * height * channels);
}

Image::~Image()
{

}

void Image::sizeDataToConfig()
{
    data.resize(width * height * channels);
}


