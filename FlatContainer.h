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
    virtual Traceable * at( int index ) { return objects[ index ]; }
    virtual int size() { return objects.size(); }
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
	virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
	
	std::vector<Traceable *> objects;
};


#endif
