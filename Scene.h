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

class Scene 
{
public:
	Scene();
	~Scene();

	bool intersect( const Ray & ray, RayIntersection & intersection ) const;
	bool intersectsAny( const Ray & ray, float min_distance ) const;

    void buildLightList();
    // Helper for buildLightList
    void addLightsForTraceable( Traceable * obj );

	Traceable * root;

    // Lights are just traceables that have a non-zero emittance. They are part
    // of the scene, but we keep pointers to them in a light list so we can easily
    // iterate over them for lighting calculations.
    std::vector<Traceable *> lights;
};



#endif
