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

#include "AssetLoader.h"
#include "ImageTracer.h"

class Container;
class FlatContainer;

void addSlabGrid( Container * container );
void addRandomSpheres( Container * container, RandomNumberGenerator & rng, int numSpheres );
void addRandomCubes( Container * container, RandomNumberGenerator & rng, int numCubes );
void addOffsetCubes( Container * container );
void addBunny( Container * container );
void addLitBunny( Container * container );
void addSphereLight( Container * container,
                     const Vector4 & center, float r,
                     const RGBColor & color,
                     float power );

void addGroundPlane( Container * container );

void addTransformedCubes( Container * container );

void addLightingTest1( Container * container );
void addLightingTest2( Container * container );
void addLightingTest3( Container * container );
void addLightingTest4( Container * container );

void addMirrors( Container * container );

//
// Base class for a test scene. Use the macros below to define
// a subclass and call the static run() method to run it.
//
class TestScene
{
public:
    TestScene( const std::string & output_path, const std::string & test_name );
    virtual ~TestScene();

    virtual void setup();
    virtual void buildScene();
    virtual void render();

    Scene * scene = nullptr;
    FlatContainer * container;
    std::string name;
    int rays_per_pixel;
    int image_width;
    int image_height;
    int anim_frames;
    AssetLoader loader;
    ImageTracer * tracer;
};

#define BEGIN_SCENE(TEST_NAME) \
    class TEST_NAME : public TestScene \
    { \
    public: \
        TEST_NAME( const std::string & output_path ) : TestScene( output_path, #TEST_NAME ) {} \
        virtual ~TEST_NAME() {} \
        \
        static void run() { \
            TEST_NAME test( output_path ); \
            test.setup(); \
            test.buildScene(); \
            test.render(); \
        }

#define SETUP_SCENE(BODY) \
    virtual void setup() { \
        BODY; \
    }

#define BUILD_SCENE(BODY) \
    virtual void buildScene() { \
        BODY; \
    }

#define END_SCENE() \
    };



#endif
