/*
 *  Boolean.h
 *  FastRender
 *
 *  Created by David Cunningham on 11/15/15.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _BOOLEAN_H_
#define _BOOLEAN_H_

#include "Ray.h"
#include "Traceable.h"

class Boolean : public Traceable
{
public:
	Boolean() {}
	~Boolean() {}
	
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class    
};

class BooleanDifference : public Boolean
{
public:
	BooleanDifference( Traceable * l, Traceable * r ) : left(l), right(r) {}
	~BooleanDifference() {}
	
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class    
    Traceable * left, * right;
};



#endif
