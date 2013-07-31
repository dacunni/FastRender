//
//  Transform.cpp
//  FastRender
//
//  Created by David Cunningham on 1/13/13.
//
//

#include <cmath>
#include "Transform.h"

//
// Constructors
//

Transform::Transform()
{
    fwd.identity();
    rev.identity();
}

Transform::Transform( const Matrix4x4 & f, const Matrix4x4 & r )
:   fwd(f),
    rev(r)
{

}

// Destructors

Transform::~Transform()
{
    
}

//
// Helpers
//
Transform compose( const Transform & t1, const Transform & t2 )
{
    Transform c;
    mult( t1.fwd, t2.fwd, c.fwd );
    mult( t2.rev, t1.rev, c.rev );
    return c;
}

// Create a rotation Transform from angle and axis
Transform makeRotation( float angle, const Vector4 & axis )
{
    float cos_a = std::cos( angle );
    float sin_a = std::sin( angle );
    Transform t;   
    Vector4 u = axis;
    u.normalize();
    
    // Forward
    t.fwd = Matrix4x4(cos_a + u.x() * u.x() * (1.0f - cos_a), /*HERE*/0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);
    
    // IMPLEMENT ME

    // Reverse
    t.rev.identity();
    
    // IMPLEMENT ME

    return t;
}

// Create a translation Transform
Transform makeTranslation( const Vector4 & d )
{
    return Transform(Matrix4x4(1.0, 0.0, 0.0, d.x(),
                               0.0, 1.0, 0.0, d.y(),
                               0.0, 0.0, 1.0, d.z(),
                               0.0, 0.0, 0.0, 1.0),
                     Matrix4x4(1.0, 0.0, 0.0, -d.x(),
                               0.0, 1.0, 0.0, -d.y(),
                               0.0, 0.0, 1.0, -d.z(),
                               0.0, 0.0, 0.0, 1.0));
    
}


// Create a scaling Transform
Transform makeScaling( float xScale, float yScale, float zScale )
{
    return Transform(Matrix4x4(xScale,  0.0,    0.0,    0.0,
                               0.0,     yScale, 0.0,    0.0,
                               0.0,     0.0,    zScale, 0.0,
                               0.0,     0.0,    0.0,    1.0),
                     Matrix4x4(1.0 / xScale, 0.0,          0.0,          0.0,
                               0.0,          1.0 / yScale, 0.0,          0.0,
                               0.0,          0.0,          1.0 / zScale, 0.0,
                               0.0,          0.0,          0.0,          1.0));
}


