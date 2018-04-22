/*
 *  CSG.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/19/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _CSG_H_
#define _CSG_H_

#include "Ray.h"
#include "Traceable.h"

class CSGAnd : public Traceable
{
public:
	CSGAnd( std::shared_ptr<Traceable> o1, std::shared_ptr<Traceable> o2 ) : obj1(o1), obj2(o2) {}
	virtual ~CSGAnd() {}
	
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

    std::shared_ptr<Traceable> obj1;
    std::shared_ptr<Traceable> obj2;
};




#endif
