
#include <iostream>
#include "CSG.h"


bool CSGAnd::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    RayIntersection isect1 = intersection;
    isect1.min_distance = 0.0001;
    RayIntersection isect2 = intersection;
    isect2.min_distance = 0.0001;

    unsigned int obj1_hits = 0;
    unsigned int obj2_hits = 0;

    // Count intersections from here to infinity, so we know whether
    // we're inside or outside each object
    RayIntersection tmp_isect = intersection;
    tmp_isect.min_distance = 0.0001;
    Ray tmp_ray = ray;
    while( obj1->intersectTransformed( tmp_ray, tmp_isect ) ) {
        obj1_hits++;
        tmp_ray.origin = tmp_isect.position;
    }
    tmp_isect = intersection;
    tmp_isect.min_distance = 0.0001;
    tmp_ray = ray;
    while( obj2->intersectTransformed( tmp_ray, tmp_isect ) ) {
        obj2_hits++;
        tmp_ray.origin = tmp_isect.position;
    }

    bool in_obj1 = obj1_hits % 2 == 1;
    bool in_obj2 = obj2_hits % 2 == 1;
    bool start_in = in_obj1 && in_obj2;

    //  A surface is at the point of transition between
    //  outside and inside. So we should walk the intersections
    //  always looking at the next closest and switching our
    //  in/out state for that object. Once we see an overall
    //  switch of the in/out state of the union from our
    //  original state, we're at a surface.

    bool in_state = start_in;
    RayIntersection best_isect;
    tmp_ray = ray;

    while( 1 ) {
        // Find next closest intersection
        bool hit1 = obj1->intersectTransformed( tmp_ray, isect1 ); 
        bool hit2 = obj2->intersectTransformed( tmp_ray, isect2 ) ;

        // Object 1 is closest
        if( (hit1 && !hit2)
            || (hit1 && hit2
                && isect1.distance < isect2.distance) ) {
            in_obj1 = !in_obj1;
            best_isect = isect1;
        }
        // Object 2 is closest
        else if( (hit2 && !hit1)
                 || (hit1 && hit2
                     && isect1.distance >= isect2.distance) ) {
            in_obj2 = !in_obj2;
            best_isect = isect2;
        }
        // No hits, we're done
        else {
            return false;
        }

        in_state = in_obj1 && in_obj2;

        if( in_state != start_in ) {
            // We hit a surface
            intersection = best_isect;
            intersection.material = material;
            return true;
        }

        tmp_ray.origin = best_isect.position;
    }
    
    return false;
}

