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

#include "Traceable.h"

class Container : public Traceable 
{
public:
	
	virtual void add( Traceable * traceable ) = 0;
		
    virtual Traceable * at( int index ) = 0;
    virtual int size() = 0;
};

#endif


