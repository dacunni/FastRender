/*
 *  Container.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Container.h"


void Container::add( Traceable * traceable, Material * material )
{
    traceable->material = material;
    add( traceable );
}

