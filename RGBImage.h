/*
 *  RGBImage.h
 *  FastRender
 *
 *  Created by David Cunningham on 2/8/17.
 *
 */

#ifndef _RGBIMAGE_H_
#define _RGBIMAGE_H_

#include <string>

#include "Image.h"

class RGBImage : public Image {
    public:
        RGBImage();
        RGBImage( unsigned int w, unsigned int h );
        RGBImage( const std::string & filename, unsigned int w, unsigned int h );
        RGBImage( const std::string & filename );
        virtual ~RGBImage();

        // [u,v] in [0, 1], u increasing to the right, v increasing down
        virtual RGBColor sampleRGB( float u, float v ) const;

        void loadDataFromRawFile( const std::string & filename,
                                  unsigned int w, unsigned int h );
        void loadDataFromImageFile( const std::string & filename );
};



#endif

