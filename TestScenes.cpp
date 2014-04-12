/*
 *  TestScenes.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 2/22/14
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Container.h"
#include "AxisAlignedSlab.h"
#include "RandomNumberGenerator.h"
#include "Sphere.h"
#include "Material.h"

void addSlabGrid( Container * container )
{
    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1, -1.0,
                                         0.1-0.45,  0.1, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1, -1.0,
                                         0.1+0.45,  0.1, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1, -0.1+0.45, -1.0,
                                         0.1,  0.1+0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1, -0.1-0.45, -1.0,
                                         0.1,  0.1-0.45, -5.3 ) );

    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1-0.45, -1.0,
                                         0.1-0.45,  0.1-0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1+0.45, -1.0,
                                         0.1-0.45,  0.1+0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1-0.45, -1.0,
                                         0.1+0.45,  0.1-0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1+0.45, -1.0,
                                         0.1+0.45,  0.1+0.45, -5.3 ) );
}

void addRandomSpheres( Container * container, RandomNumberGenerator & rng, int numSpheres )
{
	for( int si = 0; si < numSpheres; si++ ) {
		container->add( new Sphere( Vector4( rng.uniformRange( -1.5, 1.5 ),
                                            rng.uniformRange( -1.5, 1.5 ),
                                            rng.uniformRange( -10.0, -3.0 ) ),
                                   0.15 ) );
	}
}

void addRandomCubes( Container * container, RandomNumberGenerator & rng, int numCubes )
{
	for( int si = 0; si < numCubes; si++ ) {
        float x = rng.uniformRange( -1.5, 1.5 );
        float y = rng.uniformRange( -1.5, 1.5 );
        float z = rng.uniformRange( -10.0, -3.0 );
        container->add( new AxisAlignedSlab( x - 0.1, y - 0.1, z - 0.1,
                                             x + 0.1, y + 0.1, z + 0.1 ) );
	}
}

void addOffsetCubes( Container * container )
{
    // Offset cubes for testing AO
    AxisAlignedSlab * cube1 = new AxisAlignedSlab(  0.1, -0.5, -1.0,
                                                    0.3, -0.3, -1.2 );
    AxisAlignedSlab * cube2 = new AxisAlignedSlab( -0.1, -0.5, -1.2,
                                                    0.1, -0.3, -1.4 );
    cube1->material = new DiffuseMaterial( 1.0, 0.0, 0.0 );
    container->add( cube1 );
    container->add( cube2 );
}


