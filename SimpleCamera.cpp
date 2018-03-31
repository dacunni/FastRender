#include "SimpleCamera.h"
#include "RandomNumberGenerator.h"


SimpleCamera::SimpleCamera( RandomNumberGenerator & rng,
                            float xmin, float xmax, float ymin, float ymax,
                            int image_width, int image_height )
    : rng(&rng),
      image_width(image_width),
      image_height(image_height),
      jitter_rays(true)
{
    setFocalPlaneDimensions( xmin, xmax, ymin, ymax );
}

SimpleCamera::SimpleCamera( const SimpleCamera & c ) 
: rng(c.rng), image_width(c.image_width), image_height(c.image_height),
    jitter_rays(c.jitter_rays)
{
    setFocalPlaneDimensions(c.xmin, c.xmax, c.ymin, c.ymax );
}

void SimpleCamera::setFocalPlaneDimensions( float xmin, float xmax,
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

void SimpleCamera::getFocalPlaneDimensions( float & xmin, float & xmax,
                                            float & ymin, float & ymax )
{
    xmin = this->xmin;
    xmax = this->xmax;
    ymin = this->ymin;
    ymax = this->ymax;
}

Ray SimpleCamera::rayThrough( int row, int col )
{
    Ray ray;

    ray.origin = mult( transform.fwd, Vector4( 0.0, 0.0, 0.0) );
    ray.direction = mult( transform.fwd, vectorThrough( row, col ) );
    ray.direction.assertIsUnity();

    return ray;
}

Vector4 SimpleCamera::vectorThrough( int row, int col )
{
    Vector4 direction;

    direction.x = (float) col * pixel_x_size + xmin;
    direction.y = (float) (image_height - row - 1) * pixel_y_size + ymin;
    if( jitter_rays ) {
        float x_jitter = rng->uniformRange( -0.5f * x_jitter_range, 0.5f * x_jitter_range );
        float y_jitter = rng->uniformRange( -0.5f * y_jitter_range, 0.5f * y_jitter_range );
        direction.x += x_jitter;
        direction.y += y_jitter;
    }
    direction.z = -1.0f; // assume film is sitting at z = -1
    direction.makeDirection();
    direction.normalize();

    return direction;
}

