#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <vector>

#include "Ray.h"

class Image {
    public:
        Image();
        Image( unsigned int w, unsigned int h, unsigned int c );
        virtual ~Image() = default;

        void sizeDataToConfig();

        std::vector<float> data;
        unsigned int width;
        unsigned int height;
        unsigned int channels;
};


#endif

