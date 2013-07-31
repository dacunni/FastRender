//
//  AxisAlignedSlab.cpp
//  FastRender
//
//  Created by David Cunningham on 3/8/13.
//
//

#include <algorithm>
#include <utility>
#include "Ray.h"
#include "AxisAlignedSlab.h"

AxisAlignedSlab::AxisAlignedSlab()
:   xmin( 0.0 ),
    ymin( 0.0 ),
    zmin( 0.0 ),
    xmax( 0.0 ),
    ymax( 0.0 ),
    zmax( 0.0 )
{
    
}

AxisAlignedSlab::AxisAlignedSlab( float xmin_default, float ymin_default, float zmin_default,
                                  float xmax_default, float ymax_default, float zmax_default )
:   xmin( xmin_default ),
    ymin( ymin_default ),
    zmin( zmin_default ),
    xmax( xmax_default ),
    ymax( ymax_default ),
    zmax( zmax_default )
{
    
}

//
// Reference: http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-box-intersection/
//
bool AxisAlignedSlab::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    // Find t values for intersection into each of the planes that
    // define the sides of the axis-aligned slab
    float t0x = (xmin - ray.origin.x()) / ray.direction.x();
    float t1x = (xmax - ray.origin.x()) / ray.direction.x();
    float t0y = (ymin - ray.origin.y()) / ray.direction.y();
    float t1y = (ymax - ray.origin.y()) / ray.direction.y();
    float t0z = (zmin - ray.origin.z()) / ray.direction.z();
    float t1z = (zmax - ray.origin.z()) / ray.direction.z();
    
    // Swap t values if necessary to make t0{x,y,z} < t1{x,y,z}
    if( t1x < t0x )
        std::swap( t0x, t1x );
    if( t1y < t0y )
        std::swap( t0y, t1y );
    if( t1z < t0z )
        std::swap( t0z, t1z );

    // Reject if farthest y is closer than closest x, or
    // if farthest x is closer than closest y
    if( t1y < t0x || t1x < t0y )
        return false;

    // Find first potential intersection point as the farthest of the t0s, ignoring z
    float t0 = std::max( t0x, t0y );
    // Find second potential intersection point as the closest of the t1s, ignoring z
    float t1 = std::min( t1x, t1y );
    
    // Reject if farthest z is closer than closest x,y, or
    // if farthest x,y is closer than closest z
    if( t1z < t0 || t0z > t1 )
        return false;
    
    // Update closest and farthest intersection with z
    t0 = std::max( t0, t0z );
    t1 = std::max( t1, t1z );

    
    //TODO - handle divide by zero above
    
    //intersection.normal =      // TODO
    intersection.distance = t0;
    return true; // TEMP
}



