/*
 *  Traceable.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <typeinfo>
#include <cfloat>

#include "Material.h"
#include "Traceable.h"
#include "TraceableVisitor.h"
#include "Ray.h"
#include "AxisAlignedSlab.h"

Traceable::Traceable()
    : material(DEFAULT_MATERIAL)
{

}

Traceable::~Traceable()
{

}

void Traceable::updateAnim( float t )
{
    if( transform ) {
        transform->updateAnim(t);
    }
}

bool Traceable::intersectsAny( const Ray & ray, float min_distance ) const
{
    RayIntersection intersection;
    intersection.min_distance = min_distance;
    return intersect( ray, intersection );
}

bool Traceable::intersectTransformed( const Ray & ray, RayIntersection & intersection ) const
{
    if( transform ) {
        Ray tray = ray;
        tray.origin = mult( transform->rev, ray.origin );
        // Fix direction W in case other code is sloppy
        Vector4 dir = ray.direction;
        dir.normalize();
        dir.makeDirection();
        tray.direction = mult( transform->rev, dir );
        tray.direction.normalize();
        tray.direction.makeDirection();
        bool hit = intersect( tray, intersection );
        if( hit ) {
            intersection.position = mult( transform->fwd, intersection.position );
            intersection.distance = subtract( intersection.position, ray.origin ).magnitude();

            // FIXME - The min_distance checks in each Traceable's intersect() function
            //         do not account for transforms.

            assert( intersection.distance > 0.0f );
            //assert( intersection.distance > intersection.min_distance );
            assert( intersection.distance < FLT_MAX );

            // Set normal W to be 0, since vector math elsewhere may be sloppy about this
            intersection.normal.w = 0;
            intersection.normal.assertIsDirection();
            intersection.normal.assertIsUnity();

            intersection.normal = mult( transform->fwd, intersection.normal );
            intersection.normal.normalize();
            intersection.normal.assertIsDirection();

            // Record the untransformed ray in the intersection
            intersection.ray = ray;
        }
        return hit;
    }
    else {
        return intersect( ray, intersection );
    }
}

bool Traceable::intersectsAnyTransformed( const Ray & ray, float min_distance ) const
{
    // FIXME - how should we adjust min_distance?
    if( transform ) {
        Ray tray = ray;
        tray.origin = mult( transform->rev, ray.origin );
        // Fix direction W in case other code is sloppy
        Vector4 dir = ray.direction;
        dir.normalize();
        dir.makeDirection();
        tray.direction = mult( transform->rev, dir );
        tray.direction.normalize();
        tray.direction.makeDirection();
        return intersectsAny( tray, min_distance );
    }
    else {
        return intersectsAny( ray, min_distance );
    }
}

std::string Traceable::className() const
{
    // FIXME - can we demangle this name?
    return typeid(*this).name();
}

void Traceable::print( FILE * file ) const
{
    fprintf( file, "Traceable (%s) has_xform: %s\n",
             className().c_str(),
             transform ? "yes" : "no" );
}

std::string Traceable::toJSON() const
{
    return "{\"type\":none}";
}

void Traceable::visit( TraceableVisitor & visitor )
{
    visitor.handle(*this);
}

std::shared_ptr<AxisAlignedSlab> Traceable::getTransformedAxisAlignedBounds() const
{
    auto objB = getAxisAlignedBounds();
    auto xfB = std::make_shared<AxisAlignedSlab>();

    if(!transform) {
        return objB;
    }

    float x[2] = { objB->xmin, objB->xmax };
    float y[2] = { objB->ymin, objB->ymax };
    float z[2] = { objB->zmin, objB->zmax };

    Vector4 minCorner = mult(transform->fwd, Vector4(x[0], y[0], z[0], 1.0f));
    Vector4 maxCorner = mult(transform->fwd, Vector4(x[0], y[0], z[0], 1.0f));

    for( int xi = 0; xi < 2; xi++ ) {
        for( int yi = 0; yi < 2; yi++ ) {
            for( int zi = 0; zi < 2; zi++ ) {
                Vector4 xfPoint = mult(transform->fwd, Vector4(x[xi], y[yi], z[zi], 1.0f));
                minCorner.x = std::min(minCorner.x, xfPoint.x);
                minCorner.y = std::min(minCorner.y, xfPoint.y);
                minCorner.z = std::min(minCorner.z, xfPoint.z);
                maxCorner.x = std::max(maxCorner.x, xfPoint.x);
                maxCorner.y = std::max(maxCorner.y, xfPoint.y);
                maxCorner.z = std::max(maxCorner.z, xfPoint.z);
            }
        }
    }

    minCorner = perspective_scale(minCorner);
    maxCorner = perspective_scale(maxCorner);

    xfB->xmin = minCorner.x;
    xfB->ymin = minCorner.y;
    xfB->zmin = minCorner.z;
    xfB->xmax = maxCorner.x;
    xfB->ymax = maxCorner.y;
    xfB->zmax = maxCorner.z;

    return xfB;
}

