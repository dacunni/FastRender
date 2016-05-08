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
#include "Color.h"

class Material 
{
    public:
        Material() {}
        virtual ~Material() {}

        // Reflectances
        // FIXME - not realistic. need a more generic, physically-based reflection model. gotta start somewhere, though
        RGBColor diffuse;
        RGBColor specular;
        RGBColor emittance;

        float index_of_refraction = 1.0f;

        bool perfect_reflector = false;
        bool perfect_refractor = false;
};

class DiffuseMaterial : public Material 
{
    public:
        DiffuseMaterial( float r, float g, float b ) : Material() { 
            diffuse.setRGB( r, g, b ); 
            specular.setRGB( 0.0f, 0.0f, 0.0f ); 
            //specular.setRGB( 0.5f, 0.5f, 0.5f );  // TEMP
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        ~DiffuseMaterial() {}
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
            diffuse.setRGB( 0.0f, 0.0f, 0.0f ); 
            specular.setRGB( r, g, b ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
            perfect_reflector = true;
        }

        ~MirrorMaterial() {}
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
            diffuse.setRGB( 0.0f, 0.0f, 0.0f ); 
            specular.setRGB( r, g, b ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
            perfect_refractor = true;
        }

        ~RefractiveMaterial() {}
};


#endif
