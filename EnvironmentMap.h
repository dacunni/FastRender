/*
 *  EnvironmentMap.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ENVIRONMENT_MAP_H_
#define _ENVIRONMENT_MAP_H_

#include <stdio.h>
#include <memory>
#include <math.h>

#include "Transform.h"
#include "Ray.h"

class Material;
class Ray; 
class RayIntersection;

class EnvironmentMap {
public:
	EnvironmentMap() {}
    virtual ~EnvironmentMap() {}
	
	virtual RGBRadianceSample sample( const Ray & ray ) const = 0;
};

// Test pattern environment map
//
// Displays reference angles on the sphere at infinity
class TestPatternEnvironmentMap : public EnvironmentMap {
public:
	TestPatternEnvironmentMap() {}
    virtual ~TestPatternEnvironmentMap() {}
	
	virtual RGBRadianceSample sample( const Ray & ray ) const;
};

class ArcLightEnvironmentMap : public EnvironmentMap {
public:
	ArcLightEnvironmentMap();
	ArcLightEnvironmentMap( const Vector4 & d, float r );
    virtual ~ArcLightEnvironmentMap() {}
	
	virtual RGBRadianceSample sample( const Ray & ray ) const;

    void setLightDirection( const Vector4 & d ) { light_dir = d.normalized(); }
    void setAngularRadius( float r ) { angular_radius = r; recalculateRadiance(); }
    void setPower( float p ) { power = p; recalculateRadiance(); }

protected:
    void recalculateRadiance();

    // Direction of center axis of spherical cap light
    Vector4 light_dir = Vector4( 0.0, 1.0, 0.0 );
    // Angle between center axis and outer edge of cap
    float angular_radius = M_PI / 4.0f;
    // Power in Watts across a unit spherical cap
    float power = 20.0f;

    // Cache of radiance per sample. Should be cached when parameters change
    // by calling recalculateRadiance()
    // TODO[DAC]: Colored light!
    float radiance_per_sample = 0.0f;
};




#endif
