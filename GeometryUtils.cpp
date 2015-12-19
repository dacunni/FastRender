/*
 *  GeometryUtils.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 12/18/2015.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include <math.h>

#include "Vector.h"
#include "GeometryUtils.h"


// Calculate the area of a spherical cap given by the half angle
// subtended by the sphere about its center axis
//
//      http://mathworld.wolfram.com/SphericalCap.html
//
// TODO[DAC]: This seems to work for angles > PI / 2, but need to check to be sure
//
float sphericalCapSurfaceArea( float half_angle, float radius )
{
    float cap_height = radius * ( 1.0f - cos( half_angle ) );
    float contact_angle = M_PI * 0.5f - half_angle;
    float base_radius = sin( half_angle );
    float cap_area = M_PI * ( sq( base_radius ) + sq( cap_height ) );

    // printf( "cap_height    = %f\n", cap_height );
    // printf( "contact_angle = %f\n", contact_angle );
    // printf( "base_radius   = %f\n", base_radius );
    // printf( "cap_area      = %f\n", cap_area  );

    return cap_area;
}

