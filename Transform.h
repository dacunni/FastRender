//
//  Transform.h
//  FastRender
//
//  Created by David Cunningham on 1/13/13.
//
//

#ifndef __FastRender__Transform__
#define __FastRender__Transform__

#include "Vector.h"
#include "Matrix.h"

class Transform
{
public:
    // Constructors
    Transform();
    Transform( const Matrix4x4 & f, const Matrix4x4 & r );

    // Destructors
    virtual ~Transform();
    
    // Returns a new Transform that is the inverse of this Transform
    Transform inverse() const { return Transform( rev, fwd ); }
        
    // Inverts this Transform
    void invert() { Matrix4x4 r = rev; rev = fwd; fwd = r; }
    
    // Members
    Matrix4x4 fwd;      // Forward transformation
    Matrix4x4 rev;      // Reverse transformation
};

// Composes two Transforms into one. Assumes t2 is applied before t1.
Transform compose( const Transform & t1, const Transform & t2 );

// Builds a rotation Transform from an axis and angle
Transform makeRotation( float angle, const Vector4 & axis );

// Builds a translation Transform
Transform makeTranslation( const Vector4 & direction );

// Builds a scaling Transform
Transform makeScaling( float xScale, float yScale, float zScale );

#endif /* defined(__FastRender__Transform__) */
