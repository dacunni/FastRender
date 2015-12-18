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
	ArcLightEnvironmentMap()
        : light_dir( 0.0, 1.0, 0.0 ), angular_radius( 0.2 ) {}
	ArcLightEnvironmentMap( const Vector4 & d, float r )
        : light_dir( d.normalized() ), angular_radius( r ) {}
    virtual ~ArcLightEnvironmentMap() {}
	
	virtual RGBRadianceSample sample( const Ray & ray ) const;

    void setLightDirection( const Vector4 & d ) { light_dir = d.normalized(); }
    void setAngularRadius( float r ) { angular_radius = r; }

protected:
    Vector4 light_dir;
    float angular_radius;
};




#endif

