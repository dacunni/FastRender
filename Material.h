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

class Material 
{
    public:
        Material() {}
        virtual ~Material() {}

        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection );

        // FIXME - stopgap
        virtual bool isEmitter() { return emittance.r > 0.0 || emittance.g > 0.0 || emittance.b > 0.; }

        virtual RGBColor diffuse( RayIntersection & isect ) { return diffuseColor; }
        virtual RGBColor specular( RayIntersection & isect ) { return specularColor; }

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
        ~DiffuseMaterial() {}

        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection );
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
        ~DiffuseCheckerBoardMaterial() {}
        
        virtual RGBColor diffuse( RayIntersection & isect );

        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection );

        float gridSize = 0.2;
};

class MirrorMaterial : public Material 
{
    public:
        MirrorMaterial()
            : Material()
        { 
            init( 1.0f, 1.0f, 1.0f ); 
        }

        MirrorMaterial( float r, float g, float b )
            : Material()
        { 
            init( r, g, b );
        }

        void init( float r, float g, float b )
        {
            diffuseColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            specularColor.setRGB( r, g, b ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
            perfect_reflector = true;
        }

        ~MirrorMaterial() {}

        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection );
};

class RefractiveMaterial : public Material 
{
    public:
        RefractiveMaterial()
            : Material()
        { 
            init( 1.0f, 1.0f, 1.0f ); 
        }

        RefractiveMaterial( float index )
            : Material()
        { 
            init( 1.0f, 1.0f, 1.0f ); 
            index_of_refraction = index;
        }

        RefractiveMaterial( float r, float g, float b )
            : Material()
        { 
            init( r, g, b );
        }

        void init( float r, float g, float b )
        {
            diffuseColor.setRGB( 0.0f, 0.0f, 0.0f ); 
            specularColor.setRGB( r, g, b ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
            perfect_refractor = true;
        }

        ~RefractiveMaterial() {}

        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection );
};


#endif
