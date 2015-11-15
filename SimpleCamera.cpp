/*
 *  SimpleCamera.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "SimpleCamera.h"
#include "RandomNumberGenerator.h"


SimpleCamera::SimpleCamera( RandomNumberGenerator & rng,
                            float xmin, float xmax, float ymin, float ymax,
                            int image_width, int image_height )
    : rng(rng),
      xmin(xmin),
      xmax(xmax),
      ymin(ymin),
      ymax(ymax),
      image_width(image_width),
      image_height(image_height),
      jitter_rays(true)
{

}

Ray SimpleCamera::rayThrough( int row, int col )
{
    Ray ray;

    ray.origin = mult( transform.fwd, Vector4( 0.0, 0.0, 0.0) );
    ray.direction = mult( transform.fwd, vectorThrough( row, col ) );
    ray.direction.normalize();

    return ray;
}

Vector4 SimpleCamera::vectorThrough( int row, int col )
{
    Vector4 direction;

    float x_jitter = rng.uniformRange( -0.5 * (xmax - xmin) / (float) image_width,
                                       0.5 * (xmax - xmin) / (float) image_width );
    float y_jitter = rng.uniformRange( -0.5 * (ymax - ymin) / (float) image_height,
                                       0.5 * (ymax - ymin) / (float) image_height );

    direction[0] = (float) col / image_width * (xmax - xmin) + xmin;
    direction[1] = (float) (image_height - row - 1) / image_height * (ymax - ymin) + ymin;
    if( jitter_rays ) {
        direction[0] += x_jitter;
        direction[1] += y_jitter;
    }
    direction[2] = -1.0f; // assume film is sitting at z = -1
    direction[3] = 0.0f;
    direction.normalize();

    return direction;
}

