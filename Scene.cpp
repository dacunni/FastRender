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
#include "Container.h"
#include "Material.h"
#include "EnvironmentMap.h"

Scene::Scene()
: root(0)
{

}

Scene::~Scene()
{

}

bool Scene::intersect( const Ray & ray, RayIntersection & intersection ) const
{
	if( root != 0 && root->intersectTransformed( ray, intersection ) ) {
        return true;
	}

	if( env_map ) {
        intersection.sample = env_map->sample( ray );
        intersection.ray = ray;
        intersection.distance = FLT_MAX;
        return true;
    }

    return false;
}

bool Scene::intersectsAny( const Ray & ray, float min_distance ) const
{
	if( root != 0 && root->intersectsAnyTransformed( ray, min_distance ) ) {
        return true;
	}

	if( env_map ) {
        return true;
    }

    return false;
}

void Scene::buildLightList()
{
    printf("Finding lights in the scene\n");
    lights.clear();
    addLightsForTraceable( root );    
    printf("Found %u lights\n", (unsigned int ) lights.size());
}

void Scene::addLightsForTraceable( Traceable * obj )
{
    Container * container;

    // If the object has non-zero emittance, add it to the light list
    if( obj->material && !obj->material->emittance.isZero() ) {
        lights.push_back( obj );
    }

    // If this is a container, recursives check its contents for more lights
    if( obj && (container = dynamic_cast<Container *>(obj)) ) {
        int size = container->size();

        for( int i = 0; i < size; i++ ) {
            addLightsForTraceable( container->at( i ) );
        }
    }
}

