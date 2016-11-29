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
class EnvironmentMap;

class TraceLog
{
    public:
        class Entry
        {
            public:
                Ray ray;
                RayIntersection intersection;
                bool hit;
        };

        std::vector<Entry> entries;
};

class Scene 
{
public:
	Scene();
	~Scene();

    void updateAnim( float t );

	bool intersect( const Ray & ray, RayIntersection & intersection ) const;
	bool intersectsAny( const Ray & ray, float min_distance ) const;

    void addPointLight( const PointLight & light ) { point_lights.push_back(light); }

    void buildLightList();
    // Helper for buildLightList
    void addLightsForTraceable( std::shared_ptr<Traceable> obj );

    void startLogging( TraceLog * log ) { trace_log = log; }
    void stopLogging() { trace_log = nullptr; }
    void logIntersect( const Ray & ray, const RayIntersection & intersection, bool hit ) const;

    std::shared_ptr<Traceable> root;
    std::shared_ptr<EnvironmentMap> env_map;

    // Lights are just traceables that have a non-zero emittance. They are part
    // of the scene, but we keep pointers to them in a light list so we can easily
    // iterate over them for lighting calculations.
    std::vector<std::shared_ptr<Traceable> > lights;
    // Point lights are special because they are infinitesimal and thus can't be
    // found by randomly shooting rays. We keep track of them separately so we
    // can consider them explicitely.
    std::vector<PointLight> point_lights;

    // Logging
    mutable TraceLog * trace_log = nullptr;
};



#endif
