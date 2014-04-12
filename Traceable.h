/*
 *  Traceable.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TRACEABLE_H_
#define _TRACEABLE_H_

#include <stdio.h>
#include "Transform.h"

class Ray; 
class RayIntersection;
class AxisAlignedSlab;

class Traceable {
public:
	Traceable() {}
	virtual ~Traceable() {}
	
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const = 0;
	virtual bool intersectsAny( const Ray & ray, float min_distance ) const;

    // Children should implement this to create an axis-aligned bounding box
    // Returns null if not implemented
    virtual AxisAlignedSlab * getAxisAlignedBounds() const { return nullptr; }

    virtual void print( FILE * file = stdout ) const;
};



#endif

