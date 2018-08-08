#ifndef _SHADER_H_
#define _SHADER_H_

class RandomNumberGenerator;
class RayIntersection;
class Scene;
class PointLight;
class AreaLight;
class RGBColor;
class EnvironmentMap;
class Vector4;

class Shader 
{
    public:
        Shader() {}
        virtual ~Shader() {}

        // Shade an intersection point
        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection ) = 0;

        bool inShadow( const Scene & scene,
                       const RayIntersection & intersection,
                       const Vector4 & toLight // Relative position of light (unnormalized)
                     );
    
        RGBColor samplePointLight( const Scene & scene,
                                   const RayIntersection & intersection,
                                   const PointLight & light );
        RGBColor samplePointLights( const Scene & scene,
                                    const RayIntersection & intersection );

        RGBColor sampleAreaLight( const Scene & scene,
                                  const RayIntersection & intersection,
                                  RandomNumberGenerator & rng,
                                  const AreaLight & light,
                                  Vector4 & direction,
                                  bool & hit );
        RGBColor sampleAreaLights( const Scene & scene,
                                   const RayIntersection & intersection,
                                   RandomNumberGenerator & rng );

        RGBColor sampleEnvironmentMap( const Scene & scene,
                                       const RayIntersection & intersection,
                                       RandomNumberGenerator & rng );

        RGBColor reflectedRadiance( const RayIntersection & intersection,
                                    const RGBColor & Li,
                                    const Vector4 & lightDirection );
};

// Shader that does nothing
class NullShader : public Shader
{
    public:
        NullShader() = default;
        virtual ~NullShader() = default;

        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );
};


#endif
