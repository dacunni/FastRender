/*
 *  Scene.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>

class Traceable;
class Ray;
class RayIntersection;
class EnvironmentMap;

// TEMP >>> move this to its own file
#include "Color.h"
#include "Vector.h"

class PointLight
{
public:
    PointLight() {}
    PointLight( const Vector4 & pos, const RGBColor & bp ) : position(pos), band_power(bp) {}
    ~PointLight() {}

    Vector4  position;
    RGBColor band_power;
};
// TEMP <<<

class Scene 
{
public:
	Scene();
	~Scene();

	bool intersect( const Ray & ray, RayIntersection & intersection ) const;
	bool intersectsAny( const Ray & ray, float min_distance ) const;

    void addPointLight( const PointLight & light ) { point_lights.push_back(light); }

    void buildLightList();
    // Helper for buildLightList
    void addLightsForTraceable( Traceable * obj );

	Traceable * root;
    EnvironmentMap * env_map;

    // Lights are just traceables that have a non-zero emittance. They are part
    // of the scene, but we keep pointers to them in a light list so we can easily
    // iterate over them for lighting calculations.
    std::vector<Traceable *> lights;
    // Point lights are special because they are infinitesimal and thus can't be
    // found by randomly shooting rays. We keep track of them separately so we
    // can consider them explicitely.
    std::vector<PointLight> point_lights;
};



#endif
