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
};

class DiffuseMaterial : public Material 
{
    public:
        DiffuseMaterial( float r, float g, float b ) : Material() { 
            diffuse.setRGB( r, g, b ); 
            specular.setRGB( 0.0f, 0.0f, 0.0f ); 
            emittance.setRGB( 0.0f, 0.0f, 0.0f );
        }
        ~DiffuseMaterial() {}
};

#endif
