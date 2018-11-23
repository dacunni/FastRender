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
#include "Lights.h"
#include "Ray.h"

class Traceable;
class TraceLog;
class EnvironmentMap;
class Logger;

class Scene 
{
public:
	Scene();
	~Scene();

    // Update the scene state for a given point in time
    void updateAnim( float t );

    // Ray-scene intersection
    bool intersect( const Ray & ray, RayIntersection & intersection ) const;
    bool intersectsAny( const Ray & ray, float min_distance ) const;
    bool intersectEnvMap( const Ray & ray, RayIntersection & intersection ) const;

    // Lights
    void addPointLight( const PointLight & light ) { point_lights.push_back(light); }
    void buildLightList();
    // Helper for buildLightList
    void addLightsForTraceable( std::shared_ptr<Traceable> obj );

    // Logging
    void startLogging( TraceLog * log ) { trace_log = log; }
    void stopLogging() { trace_log = nullptr; }
    void logIntersect( const Ray & ray, const RayIntersection & intersection, bool hit ) const;

    // Members

    // The root of the scene graph
    std::shared_ptr<Traceable> root;

    // At most one environment map may be bound to the scene
    std::shared_ptr<EnvironmentMap> env_map;

    // Lights are just traceables that have a non-zero emittance. They are part
    // of the scene, but we keep pointers to them in a light list so we can easily
    // iterate over them for lighting calculations.
    std::vector<std::shared_ptr<Traceable> > lights;
    std::vector<std::shared_ptr<AreaLight> > area_lights;
    // Point lights are special because they are infinitesimal and thus can't be
    // found by randomly shooting rays. We keep track of them separately so we
    // can consider them explicitely.
    std::vector<PointLight> point_lights;

    // Logging
    mutable TraceLog * trace_log = nullptr;
    Logger & logger;
};



#endif
