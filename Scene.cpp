/*
 *  Scene.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Scene.h"
#include "Ray.h"
#include "Traceable.h"

Scene::Scene()
: root(0)
{

}

Scene::~Scene()
{

}

bool Scene::intersect( const Ray & ray, RayIntersection & intersection ) const
{
	if( root != 0 ) {
		return root->intersect( ray, intersection );
	}
	else {
		return false;
	}
}

bool Scene::intersectsAny( const Ray & ray, float min_distance ) const
{
	if( root != 0 ) {
		return root->intersectsAny( ray, min_distance );
	}
	else {
		return false;
	}    
}
