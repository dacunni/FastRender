/*
 *  Container.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Container.h"
#include "AxisAlignedSlab.h"


void Container::add( std::shared_ptr<Traceable> traceable,
                     std::shared_ptr<Material> material )
{
    traceable->material = material;
    add( traceable );
}

void Container::updateAnim( float t )
{
    auto size = this->size();
    for( int i = 0; i < size; i++ ) {
        at(i)->updateAnim(t);
    }
}

std::shared_ptr<AxisAlignedSlab> Container::getAxisAlignedBounds() const
{
    auto bounds = std::make_shared<AxisAlignedSlab>( *at(0)->getAxisAlignedBounds() );
    auto size = this->size();
    for( int i = 1; i < size; i++ ) {
        *bounds = merge( *bounds, *at(i)->getAxisAlignedBounds() );
    }

    return bounds;
}
