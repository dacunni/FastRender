/*
 *  HDRImage.h
 *  FastRender
 *
 *  Created by David Cunningham on 2/8/17.
 *
 */

#ifndef _HDRIMAGE_H_
#define _HDRIMAGE_H_

#include <string>

#include "Image.h"

class HDRImage : public Image {
    public:
        HDRImage( unsigned int w, unsigned int h );
        HDRImage( const std::string & filename, unsigned int w, unsigned int h );
        virtual ~HDRImage();

        // [u,v] in [0, 1], u increasing to the right, v increasing down
        virtual RGBRadianceSample sampleRGB( float u, float v ) const;

        // TODO: Clean this up and make a grayscale image type
        void toGray( std::vector<float> & grayData ) const;

        // Clear values using the mask
        void maskUV( const std::function<bool(float,float)> & mask );

        void loadDataFromFile( const std::string & filename,
                               unsigned int w, unsigned int h);
};



#endif

