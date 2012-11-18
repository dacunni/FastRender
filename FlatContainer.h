/*
 *  FlatContainer.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FLAT_CONTAINER_H_
#define _FLAT_CONTAINER_H_

#include <vector>

#include "Container.h"

class FlatContainer : public Container 
{
public:
	FlatContainer();
	virtual ~FlatContainer();
	
	virtual void add( Traceable * traceable );
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
	
	std::vector<Traceable *> objects;
};


#endif
