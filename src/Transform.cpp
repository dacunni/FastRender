//
//  Transform.cpp
//  FastRender
//
//  Created by David Cunningham on 1/13/13.
//
//

#include <cmath>
#include <sstream>
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

// Printing

void Transform::print()
{
    printf("Transform:\n");
    printf("fwd:\n");
    fwd.print();
    printf("rev:\n");
    rev.print();
}

std::string Transform::toString() const
{
    std::stringstream ss;
    ss << "trans"
       << " fwd: " << fwd.toString()
       << " rev: " << fwd.toString();
    return ss.str();
}

std::string Transform::toJSON() const
{
    std::stringstream ss;

    ss << "{"
        "\"fwd\":" << fwd.toJSON() << ","
        "\"rev\":" << rev.toJSON()
        << "}";

    return ss.str();
}

//
// Helpers
//
Transform compose( const Transform & t1 )
{
    return t1;
}

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
    float ca = std::cos( angle );
    float omca = 1.0f - ca;
    float sa = std::sin( angle );
    Transform t;   
    Vector4 u = axis;
    u.normalize();
    
    t.fwd = Matrix4x4( ca + u.x * u.x * omca, u.x * u.y * omca - u.z * sa, u.x * u.z * omca + u.y * sa, 0.0,
                       u.y * u.x * omca + u.z * sa, ca + u.y * u.y * omca, u.y * u.z * omca - u.x * sa, 0.0,
                       u.z * u.x * omca - u.y * sa, u.z * u.y * omca + u.x * sa, ca + u.z * u.z * omca, 0.0,
                       0.0, 0.0, 0.0, 1.0);
    inverse( t.fwd, t.rev );

    return t;
}

// Create a translation Transform
Transform makeTranslation( const Vector4 & d )
{
    return Transform(Matrix4x4(1.0, 0.0, 0.0, d.x,
                               0.0, 1.0, 0.0, d.y,
                               0.0, 0.0, 1.0, d.z,
                               0.0, 0.0, 0.0, 1.0),
                     Matrix4x4(1.0, 0.0, 0.0, -d.x,
                               0.0, 1.0, 0.0, -d.y,
                               0.0, 0.0, 1.0, -d.z,
                               0.0, 0.0, 0.0, 1.0));
    
}

Transform makeTranslation( float x, float y, float z )
{
    return Transform(Matrix4x4(1.0, 0.0, 0.0, x,
                               0.0, 1.0, 0.0, y,
                               0.0, 0.0, 1.0, z,
                               0.0, 0.0, 0.0, 1.0),
                     Matrix4x4(1.0, 0.0, 0.0, -x,
                               0.0, 1.0, 0.0, -y,
                               0.0, 0.0, 1.0, -z,
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

Transform makeScaling( float scale )
{
    return makeScaling( scale, scale, scale );
}

