#ifndef _OMNICAMERA_H_
#define _OMNICAMERA_H_

#include "Camera.h"

class OmniCamera : public Camera
{
public:
    OmniCamera( float fovx, float fovy,
                int image_width, int image_height );
    OmniCamera( const OmniCamera & c );
    virtual ~OmniCamera() = default;

    virtual void setFieldOfView( float fovx, float fovy );

    virtual void setFocalPlaneDimensions( float xdim, float ydim );
    virtual void setFocalPlaneExtents( float xmin, float xmax, float ymin, float ymax );
    virtual void getFocalPlaneExtents( float & xmin, float & xmax, float & ymin, float & ymax );

    virtual Vector4 vectorThrough( RandomNumberGenerator & rng, int row, int col );
    virtual Ray rayThrough( RandomNumberGenerator & rng, int row, int col );

protected:
    float fovx;
    float fovy;

    float el_min, el_max; // elevation
    float az_min, az_max; // azimuth
};


#endif


