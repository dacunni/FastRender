/*
 *  SignedDistanceFunction.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/17/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SIGNEDDISTANCEFUNCTION_H_
#define _SIGNEDDISTANCEFUNCTION_H_

#include <functional>

#include "Ray.h"
#include "Traceable.h"

class SignedDistanceFunction : public Traceable
{
public:
	SignedDistanceFunction() {}
	virtual ~SignedDistanceFunction() {}
	
	virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

    std::function<float(const Vector4&)> valueFunction;

    inline bool walkRay( const Ray & ray,
                         float tInit,
                         float & signedDistance,
                         float & distance,
                         Vector4 & position ) const;

    inline void populateIntersection( const Ray & ray,
                                      float signedDistance,
                                      float distance,
                                      const Vector4 & position,
                                      RayIntersection & intersection ) const;
};




#endif
