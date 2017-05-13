/*
 *  Image.h
 *  FastRender
 *
 *  Created by David Cunningham on 2/8/17.
 *
 */

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <vector>

#include "Ray.h"

class Image {
    public:
        Image();
        Image( unsigned int w, unsigned int h );
        virtual ~Image();

        std::vector<float> data;
        unsigned int width;
        unsigned int height;
};


#endif

