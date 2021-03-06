//
//  Transform.h
//  FastRender
//
//  Created by David Cunningham on 1/13/13.
//
//

#ifndef __FastRender__Transform__
#define __FastRender__Transform__

#include <functional>

#include "Vector.h"
#include "Matrix.h"

class Transform
{
public:
    // Constructors
    Transform();
    Transform( const Matrix4x4 & f, const Matrix4x4 & r );

    // Destructors
    virtual ~Transform() = default;

    virtual void updateAnim( float t ) {}
    
    // Returns a new Transform that is the inverse of this Transform
    Transform inverse() const { return Transform( rev, fwd ); }
        
    // Inverts this Transform
    void invert() { Matrix4x4 r = rev; rev = fwd; fwd = r; }

    void print();
    std::string toString() const;
    std::string toJSON() const;
    
    // Members
    Matrix4x4 fwd;      // Forward transformation
    Matrix4x4 rev;      // Reverse transformation
};

class TimeVaryingTransform : public Transform
{
public:
    TimeVaryingTransform( const std::function<Transform(float)> & cb )
        { update_cb = cb; }
    virtual ~TimeVaryingTransform() = default;

    virtual void updateAnim( float t ) { Transform xform = update_cb(t); fwd = xform.fwd; rev = xform.rev; };
    std::function<Transform(float)> update_cb;
};

// Composes multiple Transforms into one. Last transform is applied first.
Transform compose( const Transform & t1 );
Transform compose( const Transform & t1, const Transform & t2 );

// Variadic expansion of compose() to N transforms
template<typename... Args>
Transform compose( const Transform & t1, Args... args ) {
    return compose( t1, compose( args... ) );
}

// Builds a rotation Transform from an axis and angle
Transform makeRotation( float angle, const Vector4 & axis );

// Builds a translation Transform
Transform makeTranslation( const Vector4 & direction );
Transform makeTranslation( float x, float y, float z );

// Builds a scaling Transform
Transform makeScaling( float xScale, float yScale, float zScale );
Transform makeScaling( float scale );

#endif /* defined(__FastRender__Transform__) */
