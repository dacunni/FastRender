
#include "Lights.h"
#include "RandomNumberGenerator.h"


Vector4 CircleAreaLight::generateSample( RandomNumberGenerator & rng )
{
    float x, z;
    rng.uniformUnitCircle( x, z );
    return mult( transform.fwd, Vector4( x, 0, z ) );
}

Vector4 CircleAreaLight::orientation()
{
    const Vector4 dir( 0, -1, 0, 0 ); // Canonical orientation
    return mult( transform.fwd, dir ).normalized();
}



