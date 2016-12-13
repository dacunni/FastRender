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
#include "TraceLog.h"
#include "Container.h"
#include "Material.h"
#include "EnvironmentMap.h"

Scene::Scene()
: root(nullptr),
  env_map(nullptr)
{

}

Scene::~Scene()
{

}

void Scene::updateAnim( float t )
{
    root->updateAnim(t);
}

bool Scene::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    bool hit = false;

    // Asserts
    ray.direction.assertIsUnity();
    ray.direction.assertIsDirection();

    // Logging
#if 0
    for( int i = 0; i < ray.depth; i++ )
        printf(" ");
    printf("dp %d o %f %f %f %f  d %f %f %f %f (%f) ior %f\n",
           (int) ray.depth,
           ray.origin.x, ray.origin.y, ray.origin.z, ray.origin.w,
           ray.direction.x, ray.direction.y, ray.direction.z, ray.direction.w, ray.direction.magnitude(),
           ray.index_of_refraction);
#endif

	if( root && root->intersectTransformed( ray, intersection ) ) {
        // Asserts
        intersection.ray.direction.assertIsUnity();
        intersection.ray.direction.assertIsDirection();
        assert( intersection.distance > 0.0f );
        assert( intersection.distance < FLT_MAX );

        // Fix normals
        if( dot( ray.direction, intersection.normal ) > 0.0f ) {
            intersection.normal.negate(); // normals always face the incoming ray
        }
        intersection.normal.normalize();
        intersection.normal.makeDirection();
        hit =  true;
	}

	if( !hit && env_map ) {
        intersection.sample = env_map->sample( ray );
        intersection.ray = ray;
        intersection.distance = FLT_MAX;
        hit = true;
    }

    logIntersect( ray, intersection, hit );

    return hit;
}

bool Scene::intersectsAny( const Ray & ray, float min_distance ) const
{
    // Asserts
    ray.direction.assertIsUnity();
    ray.direction.assertIsDirection();

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
    printf("Found %u area lights\n", (unsigned int) lights.size());
    // TODO[DAC]: Handle point lights buried within the scene hierarchy
    printf("Found %u point lights\n", (unsigned int) point_lights.size());
}

void Scene::addLightsForTraceable( std::shared_ptr<Traceable> obj )
{
    std::shared_ptr<Container> container;

    // If the object has non-zero emittance, add it to the light list
    if( obj->material && !obj->material->emittance.isZero() ) {
        lights.push_back( obj );
    }

    // If this is a container, recursives check its contents for more lights
    if( obj && (container = std::dynamic_pointer_cast<Container>(obj)) ) {
        int size = container->size();

        for( int i = 0; i < size; i++ ) {
            addLightsForTraceable( container->at( i ) );
        }
    }
}

inline void Scene::logIntersect( const Ray & ray, const RayIntersection & intersection, bool hit ) const
{
    if( trace_log ) {
        TraceLog::Entry entry;
        entry.ray = ray;
        entry.intersection = intersection;
        entry.hit = hit;
        trace_log->entries.push_back( entry );
    }
}
