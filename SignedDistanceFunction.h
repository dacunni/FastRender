/*
 *  SignedDistanceFunction.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/17/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SIGNEDDISTANCEFUNCTION_H_
#define _SIGNEDDISTANCEFUNCTION_H_

#include <functional>

#include "Ray.h"
#include "Traceable.h"

class SignedDistanceFunction : public Traceable
{
    public:
        SignedDistanceFunction() = default;
        virtual ~SignedDistanceFunction() = default;

        virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
        virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

        using ValueFunctionType = std::function<float(const Vector4&)>;

        ValueFunctionType valueFunction;

        inline bool walkRay( const Ray & ray,
                             float tInit,
                             float & signedDistance,
                             float & t,
                             Vector4 & position ) const;

        inline void populateIntersection( const Ray & ray,
                                          float signedDistance,
                                          float t,
                                          const Vector4 & position,
                                          RayIntersection & intersection ) const;
};

// Common SDF shapes
SignedDistanceFunction::ValueFunctionType makeSDFSphere( const Vector4 & center, float radius );
SignedDistanceFunction::ValueFunctionType makeSDFBox( const Vector4 & center, const Vector4 & dims );
SignedDistanceFunction::ValueFunctionType makeSDFTorus( float mainRadius, float tubeRadius );
SignedDistanceFunction::ValueFunctionType makeSDFCylinder( float radius );

// SDF constructive solid geometry operators
SignedDistanceFunction::ValueFunctionType sdfUnion( const SignedDistanceFunction::ValueFunctionType & a,
                                                    const SignedDistanceFunction::ValueFunctionType & b );
SignedDistanceFunction::ValueFunctionType sdfDiff( const SignedDistanceFunction::ValueFunctionType & a,
                                                   const SignedDistanceFunction::ValueFunctionType & b );
SignedDistanceFunction::ValueFunctionType sdfIntersection( const SignedDistanceFunction::ValueFunctionType & a,
                                                           const SignedDistanceFunction::ValueFunctionType & b );

#endif
