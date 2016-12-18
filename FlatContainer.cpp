/*
 *  FlatContainer.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <sstream>

#include "FlatContainer.h"
#include "Ray.h"

FlatContainer::FlatContainer()
{
	
}

FlatContainer::~FlatContainer()
{
    /*
	for( std::vector<Traceable*>::const_iterator object = objects.begin(); object != objects.end(); ++object ) {
		if( *object ) {
			delete *object;
		}
	}
    */
}

void FlatContainer::add( std::shared_ptr<Traceable> traceable )
{
	objects.push_back( traceable );
}

bool FlatContainer::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    RayIntersection temp_isect = intersection;
    bool found_one = false;
	
	// Iterate over all of the traceable objects in the container, checking for the closest hit
	for( std::vector<std::shared_ptr<Traceable>>::const_iterator object = objects.begin(); object != objects.end(); ++object ) {
		// See if we hit each object
		if( (*object)->intersectTransformed( ray, temp_isect ) ) {
			if( found_one ) {
				// If we already have an intersection, see if this one is closer
				if( temp_isect.distance < intersection.distance ) {
					intersection = temp_isect;
				}
			}
			else {
				// Simply record the first hit
                intersection = temp_isect;
                found_one = true;
			}
		}
	}
	
	return found_one;
}

bool FlatContainer::intersectsAny( const Ray & ray, float min_distance ) const
{
    // Iterate over all of the traceable objects in the container, checking for the closest hit
	for( std::vector<std::shared_ptr<Traceable>>::const_iterator object = objects.begin(); object != objects.end(); ++object ) {
		// See if we hit each object
		if( (*object)->intersectsAnyTransformed( ray, min_distance ) ) {
            return true;
		}
	}
	
	return false;
}

std::string FlatContainer::toJSON() const
{
    std::stringstream ss;

    ss << "{\"type\":\"FlatContainer\","
        << "\"objects\":" << "[";

    for( auto oi = objects.begin(); oi != objects.end(); ++oi ) {
        if( oi != objects.begin() ) {
            ss << ",";
        }
        ss << (*oi)->toJSON();
    }

    ss << "]"
        << "}";

    return ss.str();
}

