#ifndef _SIMPLECAMERA_H_
#define _SIMPLECAMERA_H_

#include "Camera.h"

class SimpleCamera : public Camera
{
public:
    SimpleCamera( float xdim, float ydim,
                  int image_width, int image_height );
    SimpleCamera( float xmin, float xmax, float ymin, float ymax,
                  int image_width, int image_height );
    SimpleCamera( const SimpleCamera & c );

    virtual void setFieldOfView( float fovx, float fovy );

    virtual void setFocalPlaneDimensions( float xdim, float ydim );
    virtual void setFocalPlaneExtents( float xmin, float xmax, float ymin, float ymax );
    virtual void getFocalPlaneExtents( float & xmin, float & xmax, float & ymin, float & ymax );

    virtual Vector4 vectorThrough( RandomNumberGenerator & rng, int row, int col );
    virtual Ray rayThrough( RandomNumberGenerator & rng, int row, int col );
	
protected:
    float xmin, xmax;
    float ymin, ymax;
    float x_jitter_range;
    float y_jitter_range;
    float pixel_x_size;
    float pixel_y_size;
};

#endif
