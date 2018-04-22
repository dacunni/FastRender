/*
 *  Container.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <memory>
#include "Traceable.h"

class Material;

class Container : public Traceable 
{
public:
	
	virtual void add( std::shared_ptr<Traceable> traceable ) = 0;
	void add( std::shared_ptr<Traceable> traceable, std::shared_ptr<Material> material );
		
    virtual std::shared_ptr<Traceable> at( int index ) const = 0;
    virtual int size() const = 0;

    virtual std::shared_ptr<AxisAlignedSlab> getAxisAlignedBounds() const;

    virtual void updateAnim( float t );
};

#endif


