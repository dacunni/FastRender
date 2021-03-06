/*
 *  Sphere.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Ray.h"
#include "Traceable.h"

class Sphere : public Traceable
{
public:
	Sphere() {}
	Sphere( const Vector4 & c, float r ) : center(c), radius(r) {}
	Sphere( float x, float y, float z, float r ) : center(x, y, z), radius(r) {}
	virtual ~Sphere() = default;

    bool intersectHelper( const Ray & ray, float & dist1, float & dist2 ) const;
	
	virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

    virtual std::string toString() const;
    virtual std::string toJSON() const;
    virtual void visit( TraceableVisitor & visitor );

	Vector4 center;
	float radius = 0.0f;
    
    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class    
};




#endif
