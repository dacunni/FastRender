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
	ArcLightEnvironmentMap() {}
    virtual ~ArcLightEnvironmentMap() {}
	
	virtual RGBRadianceSample sample( const Ray & ray ) const;
};




#endif

