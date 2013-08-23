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

	Traceable * root;
};



#endif
