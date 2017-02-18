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

class HDRImage {
public:
	HDRImage( const std::string & filename, unsigned int w, unsigned int h);
    virtual ~HDRImage() {}
	
    // [u,v] in [0, 1], u increasing to the right, v increasing down
	virtual RGBRadianceSample sampleRGB( float u, float v ) const;

    // TODO: Clean this up and make a grayscale image type
    void toGray( std::vector<float> & grayData ) const;

    // Clear values using the mask
    void maskUV( const std::function<bool(float,float)> & mask );

//protected:
    void loadDataFromFile( const std::string & filename,
                           unsigned int w, unsigned int h);

    std::vector<float> data;
    unsigned int width;
    unsigned int height;
};



#endif

