//
//  BoundingSphere.h
//  FastRender
//
//  Created by David Cunningham on 3/8/13.
//
//

#ifndef __FastRender__BoundingSphere__
#define __FastRender__BoundingSphere__

#include <iostream>
#include "BoundingVolume.h"
#include "Sphere.h"

class BoundingSphere : public BoundingVolume, public Sphere
{
public:
    BoundingSphere() {}
    virtual ~BoundingSphere() {}
    

    
};

#endif /* defined(__FastRender__BoundingSphere__) */
