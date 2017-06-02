/*
 *  SimpleCamera.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SIMPLECAMERA_H_
#define _SIMPLECAMERA_H_

#include "Ray.h"
#include "Transform.h"
#include "Vector.h"

class RandomNumberGenerator;

class SimpleCamera
{
public:
    SimpleCamera( RandomNumberGenerator & rng,
                  float xmin, float xmax, float ymin, float ymax,
                  int image_width, int image_height );

    void setFocalPlaneDimensions( float xmin, float xmax,
                                  float ymin, float ymax );
    void jitterRays( bool jitter ) { jitter_rays = jitter; }

    Vector4 vectorThrough( int row, int col );
    Ray rayThrough( int row, int col );
	
    Transform transform;
protected:
    RandomNumberGenerator & rng;
    float xmin, xmax;
    float ymin, ymax;
    int image_width, image_height;
    bool jitter_rays;
    float x_jitter_range;
    float y_jitter_range;
    float pixel_x_size;
    float pixel_y_size;
};

#endif


