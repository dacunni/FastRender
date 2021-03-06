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
	FlatContainer() = default;
	virtual ~FlatContainer() = default;
	
	virtual void add( std::shared_ptr<Traceable> traceable );
    virtual std::shared_ptr<Traceable> at( int index ) const { return objects[ index ]; }
    virtual int size() const { return objects.size(); }
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
	virtual bool intersectsAny( const Ray & ray, float min_distance ) const;

    virtual void print( FILE * file = stdout ) const;
    virtual std::string toJSON() const;
    virtual void visit( TraceableVisitor & visitor );
	
	std::vector<std::shared_ptr<Traceable> > objects;
};


#endif
