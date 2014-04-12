/*
 *  FlatContainer.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "FlatContainer.h"
#include "Ray.h"

FlatContainer::FlatContainer()
{
	
}

FlatContainer::~FlatContainer()
{
	for( std::vector<Traceable*>::const_iterator object = objects.begin(); object != objects.end(); ++object ) {
		if( *object ) {
			delete *object;
		}
	}
}

void FlatContainer::add( Traceable * traceable )
{
	objects.push_back( traceable );
}

bool FlatContainer::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    RayIntersection temp_isect;
    bool found_one = false;
	
    temp_isect.best_hint = intersection.best_hint;
    
	// Iterate over all of the traceable objects in the container, checking for the closest hit
	for( std::vector<Traceable*>::const_iterator object = objects.begin(); object != objects.end(); ++object ) {
		// See if we hit each object
		if( (*object)->intersect( ray, temp_isect ) ) {
			if( found_one ) {
				// If we already have an intersection, see if this one is closer
				if( temp_isect.distance < intersection.distance ) {
                    temp_isect.best_hint = intersection.distance;
					intersection = temp_isect;
				}
			}
			else {
				// Simply record the first hit
				if( temp_isect.distance < intersection.best_hint ) {
                    temp_isect.best_hint = temp_isect.distance;
                    intersection = temp_isect;
                    found_one = true;
                }
			}
		}
	}
	
	return found_one;
}

bool FlatContainer::intersectsAny( const Ray & ray, float min_distance ) const
{
    // Iterate over all of the traceable objects in the container, checking for the closest hit
	for( std::vector<Traceable*>::const_iterator object = objects.begin(); object != objects.end(); ++object ) {
		// See if we hit each object
		if( (*object)->intersectsAny( ray, min_distance ) ) {
            return true;
		}
	}
	
	return false;
}

