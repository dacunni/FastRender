//
//  Octree.h
//  FastRender
//
//  Created by David Cunningham on 8/10/13.
//
//

#ifndef __FastRender__Octree__
#define __FastRender__Octree__

#include "AxisAlignedSlab.h"

class Traceable;
class Container;

class Octree {
public:
    class OctreeNode {
    public:
        Traceable * object = nullptr;
        OctreeNode * xleft = nullptr;
        OctreeNode * xright = nullptr;
        OctreeNode * yleft = nullptr;
        OctreeNode * yright = nullptr;
        OctreeNode * zleft = nullptr;
        OctreeNode * zright = nullptr;
    };
    
    void build( Container * container );
    
    OctreeNode * root = nullptr;
    AxisAlignedSlab bounds;
};






#endif /* defined(__FastRender__Octree__) */
