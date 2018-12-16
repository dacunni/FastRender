#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <vector>
#include <functional>

#include "Ray.h"

class Image {
    public:
        Image();
        Image(unsigned int w, unsigned int h, unsigned int c);
        virtual ~Image() = default;

        void sizeDataToConfig();

        void loadDataFromFile(const std::string & filename,
                               unsigned int w, unsigned int h);

        // Apply a scaling factor at uv (params: u,v)
        void applyScalingUV(const std::function<float(float,float)> & scale);

        // Clear values using the mask (params: u,v)
        void maskUV(const std::function<bool(float,float)> & mask);

        float & at(unsigned int row, unsigned int col, unsigned int ch);
        const float & at(unsigned int row, unsigned int col, unsigned int ch) const;

        void set(float value);
        void uvToRowCol(float u, float v, float & r, float & c) const;

        unsigned int rccToIndex(unsigned int row, unsigned int col, unsigned int ch) const;
        void clampRC(unsigned int & r, unsigned int & c) const;

        std::vector<float> data;
        unsigned int width;
        unsigned int height;
        unsigned int channels;
};


#endif

