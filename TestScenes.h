/*
 *  TestScenes.h
 *  FastRender
 *
 *  Some utility functions for creating test scenes. Will hopefully eventually be subsumed by some kind
 *  of scripting or scene graph format. 
 *
 *  Created by David Cunningham on 2/22/14
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TEST_SCENES_H_
#define _TEST_SCENES_H_

void addSlabGrid( Container * container );
void addRandomSpheres( Container * container, RandomNumberGenerator & rng, int numSpheres );
void addRandomCubes( Container * container, RandomNumberGenerator & rng, int numCubes );
void addOffsetCubes( Container * container );



#endif
