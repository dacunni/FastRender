#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <stdio.h>
#include <math.h>
#include <functional>
#include "Color.h"
#include "Ray.h"
#include "SurfaceTexture.h"


// TEMP - switch for testing with cosine-weighted sampling instead
//        of hemisphere sampling
#define USE_COSINE_SAMPLING

class RandomNumberGenerator;

template<typename ValueType>
using MaterialParam = std::function<ValueType(const RayIntersection &)>;
using MaterialParamRGB = MaterialParam<RGBColor>;
using MaterialParamScalar = MaterialParam<float>;

MaterialParamRGB makeConstantParamRGB(const RGBColor & c);
MaterialParamRGB makeConstantParamRGB(float r, float g, float b);

// The result of sampling a PDF. Useful in importance sampling BxDFs
//   Distributions that are characterized by a Dirac delta (zero
//   everywhere but at one point), are represented by a special value.
class DistributionSample
{
    public:
        DistributionSample() = default;
        DistributionSample( Vector4 d, float s )
            : direction(d), pdf_sample(s),
              //use_material_index_of_refraction(false)
              new_index_of_refraction(1.0f)
            {}
        ~DistributionSample() = default;

        constexpr static const float DIRAC_PDF_SAMPLE = 0.0f;

        Vector4 direction;
        float pdf_sample;

        // Indicates whether the sample ray should take on the index
        // of refraction of the material
        //bool use_material_index_of_refraction;
        // FIXME - IOR handling is ugly and not robust. Need to know
        //         when we're changing IORs by entering or leaving
        //         an object's interior, and handle intersecting objects
        //         in a sane manner.
        float new_index_of_refraction;
};

// TODO: Implement BxDF in all materials
// TODO: Use BxDF in shaders

class Material 
{
    public:
        Material();
        virtual ~Material() = default;

        std::string name() const { return typeid(*this).name(); }

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const;
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;
        DistributionSample sampleHemisphereUniform( RandomNumberGenerator & rng,
                                                    const RayIntersection & intersection ) const;
        DistributionSample sampleCosineLobe( RandomNumberGenerator & rng,
                                             const RayIntersection & intersection ) const;

        // FIXME - stopgap
        virtual bool isEmitter() { return emittance.r > 0.0 || emittance.g > 0.0 || emittance.b > 0.0; }

        RGBColor diffuse( const RayIntersection & isect ) { return albedo(isect); }

        virtual float specularity() { return 0.0f; }

        void setAlbedo(MaterialParamRGB param) { albedo = param; }

        MaterialParamRGB albedo;

        RGBColor emittance;

        float index_of_refraction = 1.0f;

        bool perfect_reflector = false;
        bool perfect_refractor = false;
};

extern std::shared_ptr<Material> DEFAULT_MATERIAL;

class DiffuseMaterial : public Material 
{
    public:
        DiffuseMaterial() : Material() {
            setAlbedo(makeConstantParamRGB(1, 1, 1));
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        DiffuseMaterial( float r, float g, float b ) : Material() { 
            setAlbedo(makeConstantParamRGB(r, g, b));
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        virtual ~DiffuseMaterial() = default;

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const;
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;
};

class DiffuseEmitterMaterial : public DiffuseMaterial
{
    public:
        DiffuseEmitterMaterial( const RGBColor & e ) : DiffuseMaterial() { emittance = e; }
        virtual ~DiffuseEmitterMaterial() = default;
};

class DiffuseCheckerBoardMaterial : public Material 
{
    public:
        DiffuseCheckerBoardMaterial();
        DiffuseCheckerBoardMaterial(float r, float g, float b);
        DiffuseCheckerBoardMaterial(float r1, float g1, float b1,
                                    float r2, float g2, float b2);
        virtual ~DiffuseCheckerBoardMaterial() = default;
        
        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const;
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;

        float gridSize = 0.2;
};

// Special material to visualize the UV map
class DiffuseUVMaterial : public DiffuseMaterial 
{
    public:
        DiffuseUVMaterial();
        virtual ~DiffuseUVMaterial() = default;
};

class DiffuseTextureMaterial : public DiffuseMaterial
{
    public:
        DiffuseTextureMaterial(std::shared_ptr<SurfaceTexture> & tex);
        virtual ~DiffuseTextureMaterial() = default;
        
        std::shared_ptr<SurfaceTexture> texture;
};

class MirrorMaterial : public Material 
{
    public:
        MirrorMaterial() : Material() { 
            setAlbedo(makeConstantParamRGB(0, 0, 0));
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
            perfect_reflector = true;
        }

        virtual ~MirrorMaterial() = default;

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const { return 1.0f; }
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;
};

class RefractiveMaterial : public Material 
{
    public:
        RefractiveMaterial() : Material() { 
            setAlbedo(makeConstantParamRGB(0, 0, 0));
            emittance.setRGB(0.0f, 0.0f, 0.0f);
            perfect_refractor = true;
        }
        RefractiveMaterial( float index ) : RefractiveMaterial() {
            index_of_refraction = index;
        }

        virtual ~RefractiveMaterial() = default;

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const { return 1.0f; }
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;
};

#endif
