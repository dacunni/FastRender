#include <cmath>
#include "OmniCamera.h"
#include "RandomNumberGenerator.h"

OmniCamera::OmniCamera( float fovx, float fovy,
                                int image_width, int image_height )
    : Camera(image_width, image_height)
{
    setFieldOfView(fovx, fovy);
}

OmniCamera::OmniCamera( const OmniCamera & c ) 
    : Camera(c)
{
    setFieldOfView(c.fovx, c.fovy);
}

void OmniCamera::setFieldOfView( float fovx, float fovy )
{
    this->fovx = fovx;
    this->fovy = fovy;

    float xdim = fovx;
    float ydim = fovy;
    setFocalPlaneDimensions(xdim, ydim);
}

void OmniCamera::setFocalPlaneDimensions( float xdim, float ydim )
{
    float halfx = 0.5f * xdim;
    float halfy = 0.5f * ydim;
    setFocalPlaneExtents(-halfx, halfx, -halfy, halfy);
}

void OmniCamera::setFocalPlaneExtents( float xmin, float xmax,
                                           float ymin, float ymax )
{
    this->az_min = xmin;
    this->az_max = xmax;
    this->el_min = ymin;
    this->el_max = ymax;
}

void OmniCamera::getFocalPlaneExtents( float & xmin, float & xmax,
                                           float & ymin, float & ymax )
{
    xmin = this->az_min;
    xmax = this->az_max;
    ymin = this->el_min;
    ymax = this->el_max;
}

Ray OmniCamera::rayThrough( RandomNumberGenerator & rng, int row, int col )
{
    Ray ray;

    ray.origin = mult(transform.fwd, Vector4(0.0, 0.0, 0.0));
    //ray.direction = mult(transform.fwd, vectorThrough(rng, row, col));
    ray.direction = vectorThrough(rng, row, col);
    ray.direction.assertIsUnity();

    return ray;
}

Vector4 OmniCamera::vectorThrough( RandomNumberGenerator & rng, int row, int col )
{
    Vector4 direction;

    float daz_dcol = (az_max - az_min) / float(image_width);
    float del_drow = (el_max - el_min) / float(image_height);

    // az increases as column decreases
    // el increases as row decreases
    float az = daz_dcol * float(image_width - col - 1) + az_min;
    float el = del_drow * float(image_height - row - 1) + el_min;

    az += M_PI; // make az,el = 0,0 point down -z axis

    direction.x = std::sin(az) * std::cos(el);
    direction.y = std::sin(el);
    direction.z = std::cos(az) * std::cos(el);

#if 0 // TODO - jitter pixels
    if( jitter_rays ) {
        float x_jitter = rng.uniformRange(-0.5f * x_jitter_range, 0.5f * x_jitter_range);
        float y_jitter = rng.uniformRange(-0.5f * y_jitter_range, 0.5f * y_jitter_range);
        direction.x += x_jitter;
        direction.y += y_jitter;
    }
#endif

    direction.makeDirection();
    direction.normalize();

    return direction;
}

