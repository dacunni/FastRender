
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "GoochShader.h"
#include "RandomNumberGenerator.h"


void GoochShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    const RGBColor warm( 1.0, 1.0, 0.0 ), cool( 0.0, 0.0, 1.0 );
    const float phong_exp = 20.0; // arbitrary, pick to look nice
    // Pick a light direction that is up and to the right of the viewing direction
    const auto upish = Vector4( 0, 1, 0 );
    const auto right = cross( intersection.ray.direction, upish ).normalized();
    const auto up = cross( right, intersection.ray.direction ).normalized();
    const auto light_dir = blend( up, 0.5, right, 0.5 ).normalized();

    float value = 0.5 * (1 + dot( intersection.normal, light_dir ));
    RGBColor color = warm.scaled( value );
    color.accum( cool.scaled( 1.0f - value ) );

    auto reflected = mirror( intersection.ray.direction.negated(), intersection.normal );
    float highlight_scale = powf( std::max( dot( reflected, light_dir ), 0.0f ), phong_exp );
    auto highlight = RGBColor( 1.0, 1.0, 1.0 ).scaled( highlight_scale );

    color.accum( highlight );

    intersection.sample.color = color;
}


