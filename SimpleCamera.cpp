#include <cmath>
#include "SimpleCamera.h"
#include "RandomNumberGenerator.h"

SimpleCamera::SimpleCamera( float xdim, float ydim,
                            int image_width, int image_height )
    : Camera(image_width, image_height)
{
    setFocalPlaneDimensions(xdim, ydim);
}

SimpleCamera::SimpleCamera( float xmin, float xmax, float ymin, float ymax,
                            int image_width, int image_height )
    : Camera(image_width, image_height)
{
    setFocalPlaneExtents(xmin, xmax, ymin, ymax);
}

SimpleCamera::SimpleCamera( const SimpleCamera & c ) 
    : Camera(c)
{
    setFocalPlaneExtents(c.xmin, c.xmax, c.ymin, c.ymax);
}

void SimpleCamera::setFieldOfView( float fovx, float fovy )
{
    float xdim = 2.0f * std::tan(std::abs(fovx) * 0.5f);
    float ydim = 2.0f * std::tan(std::abs(fovy) * 0.5f);
    setFocalPlaneDimensions(xdim, ydim);
}

void SimpleCamera::setFocalPlaneDimensions( float xdim, float ydim )
{
    float halfx = 0.5f * xdim;
    float halfy = 0.5f * ydim;
    setFocalPlaneExtents(-halfx, halfx, -halfy, halfy);
}

void SimpleCamera::setFocalPlaneExtents( float xmin, float xmax,
                                         float ymin, float ymax )
{
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;
    pixel_x_size = (xmax - xmin) / (float) image_width;
    pixel_y_size = (ymax - ymin) / (float) image_height;
    x_jitter_range = 0.5f * pixel_x_size;
    y_jitter_range = 0.5f * pixel_y_size;
}

void SimpleCamera::getFocalPlaneExtents( float & xmin, float & xmax,
                                         float & ymin, float & ymax )
{
    xmin = this->xmin;
    xmax = this->xmax;
    ymin = this->ymin;
    ymax = this->ymax;
}

Ray SimpleCamera::rayThrough( RandomNumberGenerator & rng, int row, int col )
{
    Ray ray;

    ray.origin = mult(transform.fwd, Vector4(0.0, 0.0, 0.0));
    ray.direction = mult(transform.fwd, vectorThrough(rng, row, col));
    ray.direction.assertIsUnity();

    return ray;
}

Vector4 SimpleCamera::vectorThrough( RandomNumberGenerator & rng, int row, int col )
{
    Vector4 direction;

    direction.x = (float) col * pixel_x_size + xmin;
    direction.y = (float) (image_height - row - 1) * pixel_y_size + ymin;
    if( jitter_rays ) {
        float x_jitter = rng.uniformRange(-0.5f * x_jitter_range, 0.5f * x_jitter_range);
        float y_jitter = rng.uniformRange(-0.5f * y_jitter_range, 0.5f * y_jitter_range);
        direction.x += x_jitter;
        direction.y += y_jitter;
    }
    direction.z = -1.0f; // assume film is sitting at z = -1
    direction.makeDirection();
    direction.normalize();

    return direction;
}

