#ifndef _SHADER_H_
#define _SHADER_H_

class RandomNumberGenerator;
class RayIntersection;
class Scene;
class PointLight;
class AreaLight;
class RGBColor;
class EnvironmentMap;

class Shader 
{
    public:
        Shader() {}
        virtual ~Shader() {}

        // Shade an intersection point
        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection ) = 0;
    
        RGBColor samplePointLight( const Scene & scene,
                                   const RayIntersection & intersection,
                                   const PointLight & light );
        RGBColor samplePointLights( const Scene & scene,
                                    const RayIntersection & intersection );

        RGBColor sampleAreaLight( const Scene & scene,
                                  const RayIntersection & intersection,
                                  RandomNumberGenerator & rng,
                                  const AreaLight & light );
        RGBColor sampleAreaLights( const Scene & scene,
                                   const RayIntersection & intersection,
                                   RandomNumberGenerator & rng );

        RGBColor sampleEnvironmentMap( const Scene & scene,
                                       const RayIntersection & intersection,
                                       RandomNumberGenerator & rng );
    
};


#endif
