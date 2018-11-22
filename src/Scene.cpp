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
#include "Logger.h"

Scene::Scene()
: root(nullptr),
  env_map(nullptr),
  logger(getLogger())
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

#if 0
    if( !hit && env_map ) {
        hit = env_map->intersect( ray, intersection );
    }
#endif

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

bool Scene::intersectEnvMap( const Ray & ray, RayIntersection & intersection ) const
{
    return env_map && env_map->intersect( ray, intersection );
}

void Scene::buildLightList()
{
    lights.clear();
    addLightsForTraceable( root );    
    // TODO[DAC]: Handle point lights buried within the scene hierarchy
    //printf("Found lights : %u area %u point\n", (unsigned int) lights.size(), (unsigned int) point_lights.size());
    logger.normalf("Found lights : %u area %u point", (unsigned int) lights.size(), (unsigned int) point_lights.size());
}

void Scene::addLightsForTraceable( std::shared_ptr<Traceable> obj )
{
    std::shared_ptr<Container> container;

    // If the object has non-zero emittance, add it to the light list
    if( obj->material
        // FIXME: Just supporting area lights for now
        && std::dynamic_pointer_cast<AreaLight>(obj)
        && obj->material->isEmitter() ) {
        lights.push_back( obj );
        area_lights.push_back( std::dynamic_pointer_cast<AreaLight>(obj) );
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
