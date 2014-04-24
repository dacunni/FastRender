//
//  AxisAlignedSlab.cpp
//  FastRender
//
//  Created by David Cunningham on 3/8/13.
//
//

#include <algorithm>
#include <utility>
#include <math.h>
#include "Ray.h"
#include "AxisAlignedSlab.h"

unsigned long AxisAlignedSlab::intersection_test_count = 0;

AxisAlignedSlab::AxisAlignedSlab()
:   xmin( 0.0 ),
    ymin( 0.0 ),
    zmin( 0.0 ),
    xmax( 0.0 ),
    ymax( 0.0 ),
    zmax( 0.0 )
{
    correctMinMax();
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
    correctMinMax();
}

void AxisAlignedSlab::correctMinMax( void )
{
    if( xmin > xmax ) {
        std::swap( xmin, xmax );
    }
    if( ymin > ymax ) {
        std::swap( ymin, ymax );
    }
    if( zmin > zmax ) {
        std::swap( zmin, zmax );
    }
}

static const Vector4 boxNormals[6] = {
    Vector4( -1.0, 0.0, 0.0 ),
    Vector4( 1.0, 0.0, 0.0 ),
    Vector4( 0.0, -1.0, 0.0 ),
    Vector4( 0.0, 1.0, 0.0 ),
    Vector4( 0.0, 0.0, -1.0 ),
    Vector4( 0.0, 0.0, 1.0 )
};

const int MINUS_X_NORMAL_INDEX = 0;
const int PLUS_X_NORMAL_INDEX  = 1;
const int MINUS_Y_NORMAL_INDEX = 2;
const int PLUS_Y_NORMAL_INDEX  = 3;
const int MINUS_Z_NORMAL_INDEX = 4;
const int PLUS_Z_NORMAL_INDEX  = 5;

const int X_NORMAL_BASE_INDEX = 0;
const int Y_NORMAL_BASE_INDEX = 2;
const int Z_NORMAL_BASE_INDEX = 4;

const int LOOK_POSITIVE_NORMAL_OFFSET = 0;
const int LOOK_NEGATIVE_NORMAL_OFFSET = 1;

//
// Reference: http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-box-intersection/
//
// Note: This method assumes that min/max values are ordered correctly. Call correctMinMax() to ensure this
//       prior to calling intersect().
bool AxisAlignedSlab::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    float xn, xf, yn, yf, zn, zf;       // near and far planes for the box
    int nin, nif, nix, niy, niz;        // indices into normal table (near plane, far plane, x, y, z)
    
    intersection_test_count++;
    
    // Determine which sides of the box to label "near" or "far" depending on the
    // ray direction
    if( ray.direction.x >= 0.0f ) {
        xn = xmin;
        xf = xmax;
        nix = X_NORMAL_BASE_INDEX + LOOK_POSITIVE_NORMAL_OFFSET;
    }
    else {
        xn = xmax;
        xf = xmin;
        nix = X_NORMAL_BASE_INDEX + LOOK_NEGATIVE_NORMAL_OFFSET;
    }
    
    if( ray.direction.y >= 0.0f ) {
        yn = ymin;
        yf = ymax;
        niy = Y_NORMAL_BASE_INDEX + LOOK_POSITIVE_NORMAL_OFFSET;
    }
    else {
        yn = ymax;
        yf = ymin;
        niy = Y_NORMAL_BASE_INDEX + LOOK_NEGATIVE_NORMAL_OFFSET;
    }

    if( ray.direction.z >= 0.0f ) {
        zn = zmin;
        zf = zmax;
        niz = Z_NORMAL_BASE_INDEX + LOOK_POSITIVE_NORMAL_OFFSET;
    }
    else {
        zn = zmax;
        zf = zmin;
        niz = Z_NORMAL_BASE_INDEX + LOOK_NEGATIVE_NORMAL_OFFSET;
    }
    
    // Find t values for intersection into each of the planes that
    // define the sides of the axis-aligned slab
    float tnx = (xn - ray.origin.x) / ray.direction.x;
    float tfx = (xf - ray.origin.x) / ray.direction.x;
    float tny = (yn - ray.origin.y) / ray.direction.y;
    float tfy = (yf - ray.origin.y) / ray.direction.y;
    float tnz = (zn - ray.origin.z) / ray.direction.z;
    float tfz = (zf - ray.origin.z) / ray.direction.z;
    
    // Reject if farthest y is closer than closest x, or
    // if farthest x is closer than closest y
    if( tfy < tnx || tfx < tny )
        return false;

    // Find first potential intersection point as the farthest of the t0s, ignoring z
    float tn = tnx;
    nin = nix;
    if( tny > tn ) {
        tn = tny;
        nin = niy;
    }
    // Find second potential intersection point as the closest of the t1s, ignoring z
    float tf = tfx;
    nif = nix;
    if( tfy < tf ) {
        tf = tfy;
        nif = niy;
    }

    // Reject if farthest z is closer than closest x,y, or
    // if farthest x,y is closer than closest z
    if( tfz < tn || tnz > tf )
        return false;
    
    // Update closest and farthest intersection with z
    if( tnz > tn ) {
        tn = tnz;
        nin = niz;
    }
    if( tfz < tf ) {            // FIXME - is this the right or is it < ?
        tf = tfz;
        nif = niz;
    }

#if 0    // TEMP - debugging
    ray.origin.print();
    ray.direction.print();
    printf("tnx=%f tfx=%f tny=%f tfy=%f tnz=%f tfz=%f | ", tnx, tfx, tny, tfy, tnz, tfz );
    printf("tn=%f tf=%f\n", tn, tf);
#endif
        
    if( tn > intersection.min_distance ) {
        intersection.normal = boxNormals[ nin ];
        intersection.distance = tn;
        intersection.ray = ray;
		scale( ray.direction, intersection.distance, intersection.position );
		add( intersection.position, ray.origin, intersection.position );
        intersection.material = material;
        return true;
    }
    else if( tf > intersection.min_distance ) {
        intersection.normal = boxNormals[ nif ];
        intersection.distance = tf;
        intersection.ray = ray;
		scale( ray.direction, intersection.distance, intersection.position );
		add( intersection.position, ray.origin, intersection.position );
        intersection.material = material;
        return true;
    }
    else {
        return false;
    }
}

void AxisAlignedSlab::print( FILE * file ) const
{
    printf( "AASlab: x: %f, %f y: %f, %f z: %f, %f\n", xmin, xmax, ymin, ymax, zmin, zmax );
}


