#ifndef _TRACEABLE_H_
#define _TRACEABLE_H_

#include <stdio.h>
#include <memory>
#include <string>

#include "Transform.h"

class Material;
class Ray; 
class RayIntersection;
class AxisAlignedSlab;
class TraceableVisitor;

class Traceable {
public:
	Traceable();
    virtual ~Traceable();

    virtual void updateAnim( float t );
	
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const = 0;
	virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
	virtual bool intersectTransformed( const Ray & ray, RayIntersection & intersection ) const;
	virtual bool intersectsAnyTransformed( const Ray & ray, float min_distance ) const;

    // Children should implement this to create an axis-aligned bounding box
    // Returns null if not implemented
    virtual std::shared_ptr<AxisAlignedSlab> getAxisAlignedBounds() const { return std::shared_ptr<AxisAlignedSlab>(); }

    // Return axis aligned bounds of transformed object
    // Returns bounds around transformed bounding volume by default. Children may reimplement for a tigher fit.
    virtual std::shared_ptr<AxisAlignedSlab> getTransformedAxisAlignedBounds() const;

    virtual bool isAreaLight() const { return false; }

    std::string className() const;
    virtual void print( FILE * file = stdout ) const;
    virtual std::string toJSON() const;
    virtual void visit( TraceableVisitor & visitor );

    // FIXME - we probably don't want all traceables to get a material, or we'll just end up wasting memory
    std::shared_ptr<Material> material;

    // Transform to apply to the object relative to any containing objects, or the scene if at the root
    std::shared_ptr<Transform> transform;
};


#endif

