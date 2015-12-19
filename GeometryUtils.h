/*
 *  GeometryUtils.h
 *  FastRender
 *
 *  Created by David Cunningham on 12/18/2015
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _GEOMETRY_UTILS_H_
#define _GEOMETRY_UTILS_H_


// Calculate the area of a spherical cap given by the half angle
// subtended by the sphere about its center axis
float sphericalCapSurfaceArea( float half_angle, float radius = 1.0f );

#endif
