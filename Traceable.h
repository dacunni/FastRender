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

class Ray; 
class RayIntersection;

class Traceable {
public:
	Traceable() {}
	virtual ~Traceable() {}
	
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const = 0;
};



#endif

