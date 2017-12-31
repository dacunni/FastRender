/*
 *  Material.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/12/14
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <stdio.h>
#include <math.h>
#include "Color.h"
#include "Ray.h"
#include "SurfaceTexture.h"

class RandomNumberGenerator;

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
        Material() {}
        virtual ~Material() {}

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

        virtual RGBColor diffuse( const RayIntersection & isect ) { return diffuseColor; }
        virtual RGBColor specular( const RayIntersection & isect ) { return specularColor; }

        virtual float specularity() { return 0.0f; }

        // Reflectances
        // FIXME - not realistic. need a more generic, physically-based reflection model. gotta start somewhere, though
        RGBColor diffuseColor;
        RGBColor specularColor;
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
            diffuseColor.setRGB( 1.0f, 1.0f, 1.0f );
            specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        DiffuseMaterial( float r, float g, float b ) : Material() { 
            diffuseColor.setRGB( r, g, b ); 
            specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        virtual ~DiffuseMaterial() {}

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const;
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;
};

class DiffuseEmitterMaterial : public DiffuseMaterial
{
    public:
        DiffuseEmitterMaterial( const RGBColor & e ) : DiffuseMaterial() { emittance = e; }
        virtual ~DiffuseEmitterMaterial() {}
};

class DiffuseCheckerBoardMaterial : public Material 
{
    public:
        DiffuseCheckerBoardMaterial() : Material() {
            diffuseColor.setRGB( 1.0f, 1.0f, 1.0f );
            specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        DiffuseCheckerBoardMaterial( float r, float g, float b ) : Material() { 
            diffuseColor.setRGB( r, g, b ); 
            specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        virtual ~DiffuseCheckerBoardMaterial() {}
        
        virtual RGBColor diffuse( const RayIntersection & isect );

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const;
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;

        float gridSize = 0.2;
};

// Special material to visualize the UV map
class DiffuseUVMaterial : public DiffuseMaterial 
{
    public:
        DiffuseUVMaterial() : DiffuseMaterial() {}
        virtual ~DiffuseUVMaterial() {}
        
        virtual RGBColor diffuse( const RayIntersection & isect );
};

class DiffuseTextureMaterial : public DiffuseMaterial
{
    public:
        DiffuseTextureMaterial( std::shared_ptr<SurfaceTexture> & tex ) : DiffuseMaterial(), texture(tex) {}
        virtual ~DiffuseTextureMaterial() {}
        
        virtual RGBColor diffuse( const RayIntersection & isect );

        std::shared_ptr<SurfaceTexture> texture;
};

class MirrorMaterial : public Material 
{
    public:
        MirrorMaterial() : Material() { 
            init( 1.0f, 1.0f, 1.0f ); 
        }
        MirrorMaterial( float r, float g, float b ) : Material() { 
            init( r, g, b );
        }

        void init( float r, float g, float b )
        {
            diffuseColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            specularColor.setRGB( r, g, b ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
            perfect_reflector = true;
        }

        virtual ~MirrorMaterial() {}

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const { return 1.0f; }
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;
};

class RefractiveMaterial : public Material 
{
    public:
        RefractiveMaterial() : Material() { 
            init( 1.0f, 1.0f, 1.0f ); 
        }
        RefractiveMaterial( float index ) : Material() { 
            init( 1.0f, 1.0f, 1.0f ); 
            index_of_refraction = index;
        }
        RefractiveMaterial( float r, float g, float b ) : Material() { 
            init( r, g, b );
        }

        void init( float r, float g, float b )
        {
            diffuseColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            specularColor.setRGB( r, g, b ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
            perfect_refractor = true;
        }

        virtual ~RefractiveMaterial() {}

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const { return 1.0f; }
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;
};

class CookTorranceMaterial : public Material
{
    public:
        CookTorranceMaterial() : Material() {
            diffuseColor.setRGB( 1.0f, 1.0f, 1.0f );
            specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        CookTorranceMaterial( float r, float g, float b ) : Material() { 
            diffuseColor.setRGB( r, g, b ); 
            specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        CookTorranceMaterial( float r, float g, float b, float roughness ) : Material() { 
            diffuseColor.setRGB( r, g, b ); 
            specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
            this->roughness = roughness;
        }
        virtual ~CookTorranceMaterial() {}

        virtual float specularity() { return specular; }

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const;
        // TODO
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;

        float roughness = 0.1; // [0, 1]
        float specular = 0.2; // fraction of light reflected by specular reflection
};


#endif
