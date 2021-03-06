#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <algorithm>

#include "RandomNumberGenerator.h"
#include "Transform.h"
#include "Vector.h"
#include "GeometryUtils.h"

using std::max;

RandomNumberGenerator::RandomNumberGenerator()
  : engine(device()),
    distribution(0.0f, 1.0f)
{
}

void RandomNumberGenerator::uniformUnitCircle( float & x, float & y )
{
    float r = sqrtf( uniform01() );
    float theta = uniformRange( 0.0, 2.0 * M_PI );
    x = r * cosf( theta );
    y = r * sinf( theta );
}

void RandomNumberGenerator::uniformCircle( float radius, float & x, float & y )
{
    float r = radius * sqrtf( uniform01() );
    float theta = uniformRange( 0.0, 2.0 * M_PI );
    x = r * cosf( theta );
    y = r * sinf( theta );
}

// Generate a random 3D point on the surface of the unit sphere by means of the rejection method
void RandomNumberGenerator::uniformSurfaceUnitSphere( float & x, float & y, float & z )
{
    float u1 = uniform01();
    float u2 = uniform01();
    
    z = 1.0f - 2.0f * u1;
    float r = sqrtf( max( 0.0f, 1.0f - z * z) );
    float phi = 2.0f * M_PI * u2;
    x = r * cosf( phi );
    y = r * sinf( phi );
}

void RandomNumberGenerator::uniformSurfaceUnitSphere( Vector4 & v )
{
    uniformSurfaceUnitSphere( v.x, v.y, v.z );
    v.w = 1.0;
}

void RandomNumberGenerator::uniformSurfaceUnitHalfSphere( const Vector4 & half_space, Vector4 & v )
{
    do {
        uniformSurfaceUnitSphere( v );
    } while( dot( v, half_space ) < 0.0 );
}

void RandomNumberGenerator::uniformConeDirection( const Vector4 & dir, float theta, Vector4 & v )
{
    // Find random points on the unit sphere within a code centered at the origin facing along +Z
    float cos_theta = cosf( theta );
    v.z = uniformRange( cos_theta, 1.0f );
    float phi = uniformRange( 0.0f, 2.0 * M_PI );
    float s = sqrtf( 1.0f - sq(v.z) );
    v.x = s * cosf( phi );
    v.y = s * sinf( phi );

    // Rotate the point such that the center of the randomization is along the input vector 
    // FIXME - This certainly going to be be slow when called a lot.
    //       - Make just the rotation we need, not the inverse
    //       - Make a batch random number picker so we can only take the hit once
    // FIXME - this doesn't seem to work
    // FIXME - use the normalized direction vector
    static const Vector4 up( 0.0f, 0.0f, 1.0f );
    Transform xform = makeRotation( acos(dot(up, dir)), cross(up, dir) ); 
    v = mult( xform.fwd, v );
    v.makeDirection();
}

void RandomNumberGenerator::cosineUnitHalfSphere( float & x, float & y, float & z )
{
    const float u1 = uniform01(), u2 = uniform01();
    const float r = std::sqrt(u1);
    const float az = 2.0 * M_PI * u2;
    x = r * std::cos(az);
    y = r * std::sin(az);
    z = std::sqrt(1.0f - u1);
}

void RandomNumberGenerator::cosineUnitHalfSphere( Vector4 & v )
{
    cosineUnitHalfSphere( v.x, v.y, v.z );
    v.w = 1.0f;
}

void RandomNumberGenerator::beckmanNDF( float roughness, float & x, float & y, float & z )
{
    if(roughness <= 0.0f) { x = y = 0.0f; z = 1.0f; }
    const float u1 = uniform01(), u2 = uniform01();
    const float phi = 2.0 * M_PI * u2;
    const float theta = atanf(sqrtf(-roughness * roughness * std::log(u1)));
    const float sin_theta = std::sin(theta);
    x = sin_theta * std::cos(phi);
    y = sin_theta * std::sin(phi);
    z = std::cos(theta);
}

void RandomNumberGenerator::beckmanNDF( float roughness, Vector4 & v )
{
    beckmanNDF( roughness, v.x, v.y, v.z );
    v.w = 1.0f;
}

// Generate a random 3D point within the unit cube
void RandomNumberGenerator::uniformVolumeUnitCube( float & x, float & y, float & z )
{
    x = uniformRange( -1.0f, 1.0f );
    y = uniformRange( -1.0f, 1.0f );
    z = uniformRange( -1.0f, 1.0f );
}

void RandomNumberGenerator::uniformVolumeUnitCube( Vector4 & v )
{
    uniformVolumeUnitCube( v.x, v.y, v.z );
}

// Generate a random 3D point within the unit sphere by means of the rejection method
void RandomNumberGenerator::uniformVolumeUnitSphere( float & x, float & y, float & z )
{
    do {
        uniformVolumeUnitCube( x, y, z );
    } while( x*x + y*y + z*z > 1.0 );
}

void RandomNumberGenerator::uniformUnitTriangle2D( float & x, float & y )
{
    // Let the area below y(x) = x, x in [0, 1] be the triangle we wish to sample
    // Make a pdf for x by normalizing y(x) to give p(x) = 2x
    // Pick a random x using the inverse of the cdf c(x) = x^2
    // inv cdf c'(u) = sqrt(u), u in [0, 1]
    x = sqrtf( uniformRange( 0.0, 1.0 ) );
    y = uniformRange( 0.0, x );
}

void RandomNumberGenerator::uniformTriangle3D( BarycentricCoordinate & bary )
{
    // Pick a random point in a 2D triangle, and map it to barycentric
    // coordinates in a 3D triangle
    float x, y;
    uniformUnitTriangle2D( x, y );

    bary.u = 0.5 * (1.0 - x);
    bary.v = 0.5 * (1.0 - y);
    bary.w = 1.0 - bary.u - bary.v;
}

void RandomNumberGenerator::uniformTriangle3D( const Vector4 & v1,
                                               const Vector4 & v2,
                                               const Vector4 & v3,
                                               Vector4 & r )
{
    // Pick a random point in a 2D triangle, and map it to barycentric
    // coordinates in a 3D triangle
    BarycentricCoordinate bary;
    uniformTriangle3D( bary );

    // Find the triangle coordinate for our random barycentric coordinate
    r = add( add( scale( v1, bary.u ),
                  scale( v2, bary.v ) ),
             scale( v3, bary.w ) );
}


