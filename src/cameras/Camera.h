#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Ray.h"
#include "Transform.h"
#include "Vector.h"

class RandomNumberGenerator;

class Camera
{
public:
    Camera( int image_width, int image_height );
    Camera( const Camera & c ) = default;

    void jitterRays( bool jitter ) { jitter_rays = jitter; }

    // TODO: Cleanup. This interface is a little hacky, since we don't really have a plane
    //       of focus for some cameras.

    virtual void setFieldOfView( float fovx, float fovy ) = 0;

    virtual void setFocalPlaneDimensions( float xdim, float ydim ) = 0;
    virtual void setFocalPlaneExtents( float xmin, float xmax, float ymin, float ymax ) = 0;
    virtual void getFocalPlaneExtents( float & xmin, float & xmax, float & ymin, float & ymax ) = 0;

    virtual Vector4 vectorThrough( RandomNumberGenerator & rng, int row, int col ) = 0;
    virtual Ray rayThrough( RandomNumberGenerator & rng, int row, int col ) = 0;

    Transform transform;
protected:
    int image_width, image_height;
    bool jitter_rays = true;
};

#endif
