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

void addSlabGrid( std::shared_ptr<Container> container );
void addRandomSpheres( std::shared_ptr<Container> container, RandomNumberGenerator & rng, int numSpheres );
void addRandomCubes( std::shared_ptr<Container> container, RandomNumberGenerator & rng, int numCubes );
void addOffsetCubes( std::shared_ptr<Container> container );
void addBunny( std::shared_ptr<Container> container );
void addLitBunny( std::shared_ptr<Container> container );
void addSphereLight( std::shared_ptr<Container> container,
                     const Vector4 & center, float r,
                     const RGBColor & color,
                     float power );

void addGroundPlane( std::shared_ptr<Container> container );

void addTransformedCubes( std::shared_ptr<Container> container );

void addLightingTest1( std::shared_ptr<Container> container );
void addLightingTest2( std::shared_ptr<Container> container );
void addLightingTest3( std::shared_ptr<Container> container );
void addLightingTest4( std::shared_ptr<Container> container );

void addMirrors( std::shared_ptr<Container> container );

std::shared_ptr<TriangleMesh> loadMaterialTestModel( AssetLoader & loader );

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
    std::shared_ptr<Container> container;
    std::string name;
    int rays_per_pixel;
    int image_width;
    int image_height;
    int anim_frames;
    AssetLoader loader;
    ImageTracer * tracer;
};

#define BEGIN_DERIVED_SCENE(TEST_NAME, PARENT) \
    class TEST_NAME : public PARENT \
    { \
    public: \
        TEST_NAME( const std::string & output_path ) : PARENT( output_path, #TEST_NAME ) {} \
        TEST_NAME( const std::string & output_path, const std::string & test_name ) : PARENT( output_path, test_name ) {} \
        virtual ~TEST_NAME() {} \
        \
        static void run() { \
            TEST_NAME test( output_path ); \
            test.setup(); \
            test.buildScene(); \
            test.render(); \
        }

#define BEGIN_SCENE(TEST_NAME) \
    class TEST_NAME : public TestScene \
    { \
    public: \
        TEST_NAME( const std::string & output_path ) : TestScene( output_path, #TEST_NAME ) {} \
        TEST_NAME( const std::string & output_path, const std::string & test_name ) : TestScene( output_path, test_name ) {} \
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
