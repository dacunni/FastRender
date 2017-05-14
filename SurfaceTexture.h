/*
 *  SurfaceTexture.h
 *  FastRender
 *
 *  Created by David Cunningham on 2/8/17.
 *
 */

#ifndef _SURFACE_TEXTURE_H_
#define _SURFACE_TEXTURE_H_

#include <string>
#include <vector>

#include "RGBImage.h"
#include "Color.h"

class SurfaceTexture {
    public:
        SurfaceTexture();
        SurfaceTexture( const std::string & filename );
        virtual ~SurfaceTexture();

        RGBColor sample( float u, float v );
        // TODO: Sampling method (nearest, linear, cubic, ...)

        RGBImage image;
};


#endif

