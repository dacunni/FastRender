/*
 *  TestScenes.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 2/22/14
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <sys/stat.h>
#include <set>

#include "Container.h"
#include "AxisAlignedSlab.h"
#include "RandomNumberGenerator.h"
#include "Sphere.h"
#include "Material.h"
#include "AssetLoader.h"
#include "TMOctreeAccelerator.h"
#include "BoundingVolume.h"
#include "EnvironmentMap.h"
#include "FlatContainer.h"
#include "BasicDiffuseSpecularShader.h"
#include "GoochShader.h"
#include "TestScenes.h"
#include "Logger.h"

////////////////////////////////////////////////////////////////////////////
//
// Test Registry
//
std::vector<TestRegistryEntryErased *> testRegistry;

// Prints a list of all tests in the test registry
void printTests() {
    printf("Tests (%u)\n", (unsigned int) testRegistry.size());
    unsigned int index = 0;
    for( auto & test : testRegistry ) {
        printf("%3u : %s\n", index, test->name.c_str());
        index++;
    }
}

// Logs a list of all tests in the test registry
void logTests(Logger & logger) {
    logger.normal() << "Tests (" << testRegistry.size() << ")";
    unsigned int index = 0;
    for( auto & test : testRegistry ) {
        logger.normal()  << index << "  " << test->name;
        index++;
    }
}

void runTest( unsigned int index )
{
    auto & test = testRegistry[index];
    std::cout << "---[ Running Test : " << test->name  << " ]---" << std::endl;
    test->run();
}

// Calls the run() method on all tests in the registry
void runTests() {
    for( auto & test : testRegistry ) {
        std::cout << "---[ Running Test : " << test->name  << " ]---" << std::endl;
        test->run();
    }
}
////////////////////////////////////////////////////////////////////////////
//
// TestScene Class
//
TestScene::TestScene( const std::string & output_path, const std::string & test_name )
  : scene(new Scene()),
    container(new FlatContainer()),
    name(test_name),
    output_dir(output_path),
    tracer(nullptr),
    logger(getLogger())
{
    logger.normal() << "TestScene name: " << name;
}

TestScene::~TestScene()
{

}

void TestScene::setup()
{
    logger.normalf("Creating image tracer %d x %d, %d frames, %d rays per pixel",
                   image_width, image_height, anim_frames, rays_per_pixel);
    logger.normalf("Output directory: %s", output_dir.c_str());

    tracer = new ImageTracer( image_width, image_height,
                              anim_frames, rays_per_pixel );

#if 0
    // Create a subdirectory in the output directory named with the test name, and put standard
    // output files in there
    tracer->artifacts.output_path = output_dir + "/" + name;
    mkdir(tracer->artifacts.output_path.c_str(), 0777);
    tracer->artifacts.file_prefix = "";
#else
    // Prefix each output file with the test name and put them in the output directory
    tracer->artifacts.output_path = output_dir;
    tracer->artifacts.file_prefix = name + "_";
#endif

    tracer->shader = new BasicDiffuseSpecularShader();

    tracer->setCameraTransform( compose(
        // move up a bit
        makeTranslation( 0.0, 0.5, 0.0 ),
        // rotate so we are looking down
        makeRotation( -0.2, Vector4(1, 0, 0) ),
        // back away from the origin
        makeTranslation( 0.0, 0.0, 10.0 )
        ) );
}

void TestScene::buildScene()
{
    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                    +10.0, -1.0, -10.0 );
    container->add( floor );

#if 0
    float size = 1.0;
    float half_size = size / 2.0;
    auto cube = new AxisAlignedSlab( -half_size, 0.0, -half_size,
                                       half_size, size, half_size );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( cube );
#else
    auto sphere = std::make_shared<Sphere>( 0.0, 1.0, 0, 1.0 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( sphere );
#endif

    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();

    tracer->shader = new BasicDiffuseSpecularShader();
}

void TestScene::render()
{
    tracer->scene = scene;
    tracer->scene->buildLightList();
    tracer->render();
}

////////////////////////////////////////////////////////////////////////////

void addSlabGrid( std::shared_ptr<Container> container )
{
    container->add( std::make_shared<AxisAlignedSlab>( -0.1-0.45, -0.1, -1.0,
                                                       0.1-0.45,  0.1, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1+0.45, -0.1, -1.0,
                                                       0.1+0.45,  0.1, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1, -0.1+0.45, -1.0,
                                                       0.1,  0.1+0.45, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1, -0.1-0.45, -1.0,
                                                       0.1,  0.1-0.45, -5.3 ) );

    container->add( std::make_shared<AxisAlignedSlab>( -0.1-0.45, -0.1-0.45, -1.0,
                                                       0.1-0.45,  0.1-0.45, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1-0.45, -0.1+0.45, -1.0,
                                                       0.1-0.45,  0.1+0.45, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1+0.45, -0.1-0.45, -1.0,
                                                       0.1+0.45,  0.1-0.45, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1+0.45, -0.1+0.45, -1.0,
                                                       0.1+0.45,  0.1+0.45, -5.3 ) );
}

void addRandomSpheres( std::shared_ptr<Container> container, RandomNumberGenerator & rng, int numSpheres )
{
	for( int si = 0; si < numSpheres; si++ ) {
        container->add( std::make_shared<Sphere>( Vector4( rng.uniformRange( -1.5, 1.5 ),
                                                           rng.uniformRange( -1.5, 1.5 ),
                                                           rng.uniformRange( -10.0, -3.0 ) ),
                                                  0.15 ) );
    }
}

void addRandomCubes( std::shared_ptr<Container> container, RandomNumberGenerator & rng, int numCubes )
{
	for( int si = 0; si < numCubes; si++ ) {
        float x = rng.uniformRange( -1.5, 1.5 );
        float y = rng.uniformRange( -1.5, 1.5 );
        float z = rng.uniformRange( -10.0, -3.0 );
        container->add( std::make_shared<AxisAlignedSlab>( x - 0.1, y - 0.1, z - 0.1,
                                                           x + 0.1, y + 0.1, z + 0.1 ) );
	}
}

void addOffsetCubes( std::shared_ptr<Container> container )
{
    // Offset cubes for testing AO
    auto cube1 = std::make_shared<AxisAlignedSlab>(  0.1, -0.5, -1.0,
                                                     0.3, -0.3, -1.2 );
    auto cube2 = std::make_shared<AxisAlignedSlab>( -0.1, -0.5, -1.2,
                                                     0.1, -0.3, -1.4 );
    cube1->material = std::make_shared<DiffuseMaterial>( 1.0, 0.0, 0.0 );
    container->add( cube1 );
    container->add( cube2 );
}

void addBunny( std::shared_ptr<Container> container )
{
    AssetLoader loader;
    std::string modelPath = "models";

    // bunnies
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( bunnyPath + "/bun_zipper_res2.ply" );
    //auto mesh = loader.load( bunnyPath + "/bun_zipper_res4.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    //mesh->material = std::make_shared<DiffuseMaterial>( 0.0f, 0.66, 0.42f ); // emerald green
    mesh->material = std::make_shared<MirrorMaterial>();

    printf("Building octree\n");
    auto mesh_octree = new TMOctreeAccelerator( *std::dynamic_pointer_cast<TriangleMesh>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = makeTranslation( Vector4( 0.0, 0.2, -0.5 ) );
    container->add( mesh );
}

void addLitBunny( std::shared_ptr<Container> container ) {
    addBunny( container );
    addSphereLight( container,
                    Vector4( 2.0, 3.8, -4.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 15.0 );
}

void addSphereLight( std::shared_ptr<Container> container,
                     const Vector4 & center, float r,
                     const RGBColor & color,
                     float power )
{
    auto emitter = std::make_shared<Sphere>( center, r );
    emitter->material = std::make_shared<Material>();
    emitter->material->emittance = color;
    emitter->material->emittance.scale( power );
    container->add( emitter );
}

void addGroundPlane( std::shared_ptr<Container> container )
{
    // makeshift ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -5.0, -0.5, +10.0,
                                                     5.0, -0.9, -10.0 );
    floor->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
    container->add( floor );
    
#if 0
    // DEBUG ME
    TriangleMesh * ground = new TriangleMesh();
    makeTriangleMeshGroundPlatform( *ground, 200.0 );
    container->add( ground );
#endif
}

void addTransformedCubes( std::shared_ptr<Container> container )
{
    // Offset cubes for testing AO
    float cube_size = 0.2;
    auto cube1 = std::make_shared<AxisAlignedSlab>( 0.0, 0.0, 0.0,
                                                   cube_size );
    auto cube2 = std::make_shared<AxisAlignedSlab>( 0.0, 0.0, 0.0,
                                                    cube_size );
    auto cube3 = std::make_shared<AxisAlignedSlab>( 0.0, 0.0, 0.0,
                                                    cube_size );

    cube1->material = std::make_shared<DiffuseMaterial>( 1.0, 0.0, 0.0 );
    cube2->material = std::make_shared<DiffuseMaterial>( 0.0, 0.0, 1.0 );
    cube3->material = std::make_shared<DiffuseMaterial>( 0.0, 1.0, 0.0 );

    cube1->transform = std::make_shared<Transform>();
    *cube1->transform = makeTranslation( Vector4( 0.1, 0.5, -1.0 ) );

    cube2->transform = std::make_shared<Transform>();
    *cube2->transform = 
        compose(
            makeTranslation( Vector4( 0.0, 0.15, -1.0 ) ),
            compose( 
                makeRotation( M_PI / 4.0, Vector4( 0.0, 1.0, 0.0 ) ),
                makeRotation( M_PI / 4.0, Vector4( 0.0, 0.0, 1.0 ) )
                )
            );

    cube3->transform = std::make_shared<Transform>();
    *cube3->transform = 
        compose(
            makeTranslation( Vector4( -0.3, 0.0, -1.0 ) ),
            makeScaling( 0.2, 3.0, 2.0 )
            );

    container->add( cube1 );
    container->add( cube2 );
    container->add( cube3 );

    addSphereLight( container,
                    Vector4( 3.0, 3.0, -1.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 200.0 );

    addGroundPlane( container );
}

//////////////////////////////////////////
// Lighting Tests
//////////////////////////////////////////

void addLightingTest1( std::shared_ptr<Container> container )
{
    addGroundPlane( container );
    addSphereLight( container,
                    Vector4( 1.0, 0.8, -3.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 20.0 );
    auto cube1 = std::make_shared<AxisAlignedSlab>(  0.1, -0.5, -1.0,
                                                     0.3, -0.3, -1.2 );
    auto cube2 = std::make_shared<AxisAlignedSlab>( -0.1, -0.5, -1.2,
                                                     0.1, -0.3, -1.4 );
    container->add( cube1 );
    container->add( cube2 );
    auto cube3 = std::make_shared<AxisAlignedSlab>( -1.0, -0.5, -6.0,
                                                     1.0 );
    container->add( cube3 );
}

// cube pyramid
void addLightingTest2( std::shared_ptr<Container> container )
{
    addGroundPlane( container );
    addSphereLight( container,
                    Vector4( 1.0, 4.0, -2.0 ), 1.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 20.0 );

    float size = 0.4;
    Vector4 c( -0.6, -0.5, -2.0 ); // back corner

    container->add( std::make_shared<AxisAlignedSlab>( c.x,        c.y,        c.z,        size ) );
    container->add( std::make_shared<AxisAlignedSlab>( c.x + size, c.y,        c.z,        size ) );
    container->add( std::make_shared<AxisAlignedSlab>( c.x       , c.y + size, c.z,        size ) );
    container->add( std::make_shared<AxisAlignedSlab>( c.x       , c.y       , c.z + size, size ) );

}

// cube pyramid
void addLightingTest3( std::shared_ptr<Container> container )
{
    addGroundPlane( container );
    addSphereLight( container,
                    Vector4( 1.0, 0.8, -0.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 20.0 );

    float size = 0.4;
    Vector4 c( -0.6, -0.5, -2.0 ); // back corner

    // Create a partial pyramid of size pyramid_size blocks in each direction
    //    x
    //    x x
    //    x x x
    int pyramid_size = 5;

    for( int xs = 0; xs <= pyramid_size; xs++ ) {
        for( int ys = 0; ys <= pyramid_size; ys++ ) {
            for( int zs = 0; zs <= pyramid_size; zs++ ) {
                if( xs + ys + zs == pyramid_size ) {
                    container->add( std::make_shared<AxisAlignedSlab>( c.x + (float) xs * size,
                                                                       c.y + (float) ys * size,
                                                                       c.z + (float) zs * size,
                                                                       size ) );
                }
            }
        }
    }

    //container->add( new AxisAlignedSlab( c.x, c.y, c.z + 2.0 * size, size ) );

    //container->add( new AxisAlignedSlab( c.x + size, c.y, c.z, size ) );
    //container->add( new AxisAlignedSlab( c.x, c.y + size, c.z, size ) );
    //container->add( new AxisAlignedSlab( c.x, c.y, c.z + size, size ) );

}


void addLightingTest4( std::shared_ptr<Container> container )
{
    float zoff = -5.0;

    container->add( std::make_shared<Sphere>( 0.0, 0.0, zoff - 2.0, 0.2 ) );
    container->add( std::make_shared<Sphere>( -0.4, 0.0, zoff, 0.2 ) );
    container->add( std::make_shared<Sphere>( 0.4, 0.0, zoff, 0.2 ) );
    container->add( std::make_shared<Sphere>( 0.0, -0.4, zoff, 0.2 ) );
    container->add( std::make_shared<Sphere>( 0.0, 0.4, zoff, 0.2 ) );

    float lightoff = zoff + 0.3;

    addSphereLight( container,
                    Vector4( -0.4, 0.4, lightoff ), 0.1,
                    RGBColor( 0.0, 0.0, 1.0 ), 15.0 );
    addSphereLight( container,
                    Vector4( 0.4, 0.4, lightoff ), 0.1,
                    RGBColor( 0.0, 1.0, 0.0 ), 15.0 );
    addSphereLight( container,
                    Vector4( 0.4, -0.4, lightoff ), 0.1,
                    RGBColor( 1.0, 0.0, 0.0 ), 15.0 );
    addSphereLight( container,
                    Vector4( -0.4, -0.4, lightoff ), 0.1,
                    RGBColor( 1.0, 1.0, 0.0 ), 15.0 );

}

//////////////////////////////////////////
//     Mirror Test
//////////////////////////////////////////
void addMirrors( std::shared_ptr<Container> container )
{
    auto cube1 = std::make_shared<AxisAlignedSlab>( -5.0, -5.0, -5.0,
                                                     5.0, 5.0, -5.2 );
    cube1->material = std::make_shared<MirrorMaterial>();
    container->add( cube1 );
    auto cube2 = std::make_shared<AxisAlignedSlab>( -2.5, -5.0, -5.0,
                                                    -2.7, 5.0, 5.0 );
    cube2->material = std::make_shared<MirrorMaterial>();
    container->add( cube2 );
}


//////////////////////////////////////////
//     Standard Material Test Models
//////////////////////////////////////////
std::shared_ptr<TriangleMesh> loadMaterialTestModel( AssetLoader & loader )
{
    std::string modelBasePath = "models";
    //auto mesh = loader.load( modelBasePath + "/dacunni/material_test1.obj" );
    //auto mesh = loader.load( modelBasePath + "/dacunni/material_test1.stl" );
    auto mesh = loader.load( modelBasePath + "/tf3dm.com/soccerball/untitled.ply" );
    //auto mesh = loader.load( modelBasePath + "/uvsphere.ply" );
    //auto mesh = loader.loadMultiPartMerged( modelBasePath + "/test_objects/mori/testObj.obj" );
#if 0
    auto mesh = loader.loadMultiPartMerged( modelBasePath + "/test_objects/mitsuba/mitsuba.obj" );
#endif
#if 0
    mesh->makeCanonical();
#endif
#if 1
    mesh->accelerator = new TMOctreeAccelerator( *mesh );
    mesh->accelerator->build();
#endif
    return mesh;
}

std::shared_ptr<HDRImageEnvironmentMap> loadStPetersEnvironmentMap()
{
    return std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/stpeters_probe.float", 1500, 1500);
}
std::shared_ptr<HDRImageEnvironmentMap> loadUffiziEnvironmentMap()
{
    return std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/uffizi_probe.float", 1500, 1500);
}
std::shared_ptr<HDRImageEnvironmentMap> loadRNLEnvironmentMap()
{
    return std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/rnl_probe.float", 900, 900);
}
std::shared_ptr<HDRImageEnvironmentMap> loadGraceEnvironmentMap()
{
    return std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/grace_probe.float", 1000, 1000);
}

std::vector<std::string> tokenize(const std::string & s, const char * delim)
{
    std::vector<std::string> tokens;
    std::vector<char> tokbuf;
    std::copy(s.begin(), s.end(), back_inserter(tokbuf));
    tokbuf.push_back('\0');

    char * token = strtok(&tokbuf[0], delim);

    while(token) {
        tokens.push_back(token);
        token = strtok(NULL, delim);
    }

    return std::move(tokens);
}

struct SceneBuildException : std::exception {
    virtual const char * what() const noexcept {
        return msg.c_str();
    }
    std::string msg;
};
struct ParamNotFoundException : SceneBuildException {
    ParamNotFoundException(const std::string & p) {
        msg = "Parameter not found: " + p;
    }
};
struct InvalidArgumentListException : SceneBuildException {
    InvalidArgumentListException() { msg = "Invalid argument list"; }
};
struct UnknownKeywordException : SceneBuildException {
    UnknownKeywordException(const std::string & k) {
        msg = "Unknown keyword: " + k;
    }
};
struct UnknownShaderException : SceneBuildException {
    UnknownShaderException(const std::string & s) {
        msg = "Unknown shader: " + s;
    }
};
struct UnknownMaterialException : SceneBuildException {
    UnknownMaterialException(const std::string & m) {
        msg = "Unknown material: " + m;
    }
};

struct SceneFileElement
{
    std::vector<std::string> tokens;
    std::vector<SceneFileElement> children;

    SceneFileElement & param(const std::string & key) {
        for(auto & child : children) {
            if(child.tokens[0] == key)
                return child;
        }
        //getLogger().error() << "Unable to find param '" << key << "' to element '" << tokens[0] << "'";
        throw ParamNotFoundException(key);
    }

    bool optionalFlag(const std::string & key) {
        try {
            auto keyEl = param(key);
            return true;
        }
        catch (ParamNotFoundException &) {}
        return false;
    }

    std::string & operator[](int i) { return tokens[i]; }

    void print(int indent = 0) {
        std::string keyword("<null>");
        if(tokens.size() > 0) keyword = tokens[0];
        std::cout << std::string(indent, ' ') << "SFE keyword " << keyword << " nchildren " << children.size() << "\n";
        for(auto & child : children) {
            child.print(indent + 4);
        }
    }
};

std::vector<float> getFloats(std::vector<std::string>::iterator first,
                             std::vector<std::string>::iterator last)
{
    std::vector<float> values;
    std::transform(first, last, back_inserter(values), [](std::string & s) { return std::stof(s); } );
    return values;
}

std::vector<float> getFloatArgs(SceneFileElement & element, unsigned int numArgs)
{
    return getFloats(element.tokens.begin() + 1, element.tokens.begin() + 1 + numArgs);
}

std::shared_ptr<Material> makeMaterial(SceneFileElement & element)
{
    auto & name = element[1];
    if(name == "DiffuseUV") {
        return std::make_shared<DiffuseUVMaterial>();
    }
    else if(name == "DiffuseTexture") {
        std::string textureFileName = element.param("texture")[1];
        auto texture = std::make_shared<SurfaceTexture>(textureFileName);
        return std::make_shared<DiffuseTextureMaterial>(texture);
    }
    else if(name == "Diffuse") {
        auto & albedoEl = element.param("albedo");
        std::vector<float> albedoValues = getFloatArgs(albedoEl, 3);
        return std::make_shared<DiffuseMaterial>(albedoValues[0], albedoValues[1], albedoValues[2]);
    }
    else if(name == "Mirror") {
        return std::make_shared<MirrorMaterial>();
    }
    // TODO - all materials
    else {
        throw UnknownMaterialException(name);
    }
}

Transform makeSingleTransform(SceneFileElement & element)
{
    auto & tokens = element.tokens;
    auto & xfType = tokens[0];
    if(xfType == "scale") {
        if(tokens.size() == 2) { // s
            return makeScaling(std::stof(tokens[1]));
        }
        else if(tokens.size() == 4) { // sx sy sz
            std::vector<float> params = getFloats(tokens.begin() + 1, tokens.begin() + 4);
            return makeScaling(params[0], params[1], params[2]);
        }
        else {
            throw InvalidArgumentListException();
        }
    }
    else if(xfType == "translate") {
        if(tokens.size() == 4) { // dx dy dz
            std::vector<float> params = getFloats(tokens.begin() + 1, tokens.begin() + 4);
            return makeTranslation(params[0], params[1], params[2]);
        }
        else {
            throw InvalidArgumentListException();
        }
    }
    else if(xfType == "rotate") {
        if(tokens.size() == 5) { // angle x y z
            std::vector<float> params = getFloats(tokens.begin() + 1, tokens.begin() + 5);
            const Vector4 axis(params[1], params[2], params[3]);
            return makeRotation(params[0], axis);
        }
        else {
            throw InvalidArgumentListException();
        }
    }
    throw UnknownKeywordException(xfType);
}

std::shared_ptr<Transform> makeCompositeTransform(SceneFileElement & element)
{
    auto transform = std::make_shared<Transform>();

    for(auto & child : element.children) {
        *transform = compose(*transform, makeSingleTransform(child));
    }
    transform->print(); // TEMP

    return transform;
}

void buildTraceable(SceneFileElement & element, Traceable & traceable)
{
    // optional: material
    try {
        auto materialEl = element.param("material");
        traceable.material = makeMaterial(materialEl);
    }
    catch (ParamNotFoundException &) {}

    // optional: transform
    try {
        auto transformEl = element.param("transform");
        traceable.transform = makeCompositeTransform(transformEl);
    }
    catch (ParamNotFoundException &) {}
}

void buildSceneElement(SceneFileElement & element, TestScene & testScene, Container & container)
{
    const auto & keyword = element.tokens[0];

    std::set<std::string> rootIgnore{ "outputpath", "testname", "raysperpixel", "animframes", "imagesize" };

    // Ignore keywords consumed by the root element
    if(rootIgnore.find(keyword) != rootIgnore.end()) {
        return;
    }

    if(keyword == "root") {
        std::string outputPath = element.param("outputpath")[1];
        std::string testName = element.param("testname")[1];

        auto sizeEl = element.param("imagesize");
        unsigned int imageWidth = std::stoi(sizeEl[1]);
        unsigned int imageHeight = std::stoi(sizeEl[2]);
        auto animEl = element.param("animframes");
        unsigned int numAnimFrames = std::stoi(animEl[1]);
        auto rppEl = element.param("raysperpixel");
        unsigned int raysPerPixel = std::stoi(rppEl[1]);

        testScene.tracer = new ImageTracer(imageWidth, imageHeight,
                                           numAnimFrames, raysPerPixel);
        testScene.tracer->artifacts.output_path = outputPath;
        testScene.tracer->artifacts.file_prefix = testName + "_";

        for(auto & child : element.children) {
            buildSceneElement(child, testScene, container);
        }
    }
    else if(keyword == "camera") {
        auto transformEl = element.param("transform");
        auto transform = makeCompositeTransform(transformEl);
        testScene.tracer->setCameraTransform(*transform);
        // TODO - handle no transform present
    }
    else if(keyword == "tracer") {
        auto shaderEl = element.param("shader");
        auto shaderType = shaderEl[1];
        if(shaderType == "BasicDiffuseSpecular") {
            testScene.tracer->shader = new BasicDiffuseSpecularShader();
        }
        else if(shaderType == "Gooch") {
            testScene.tracer->shader = new GoochShader();
        }
        else {
            throw UnknownShaderException(shaderType);
        }
    }
    else if(keyword == "axisalignedslab") {
        auto minEl = element.param("min");
        auto maxEl = element.param("max");
        std::vector<float> minCoord = getFloatArgs(minEl, 3);
        std::vector<float> maxCoord = getFloatArgs(maxEl, 3);
        auto obj = std::make_shared<AxisAlignedSlab>(minCoord[0], minCoord[1], minCoord[2],
                                                     maxCoord[0], maxCoord[1], maxCoord[2]);
        buildTraceable(element, *obj);
        obj->print();// TEMP
        container.add(obj);
    }
    else if(keyword == "mesh") {
        AssetLoader loader;
        bool multipart = element.optionalFlag("multipart");
        auto pathEl = element.param("path");
        auto & path = pathEl[1];
        if(multipart) {
            auto mesh = loader.loadMultiPart(path);
            buildTraceable(element, *mesh);
            container.add(mesh);
        }
        else {
            auto mesh = loader.load(path);
            buildTraceable(element, *mesh);
            container.add(mesh);
        }
    }
    else if(keyword == "sphere") {
        auto radiusEl = element.param("radius");
        float radius = std::stof(radiusEl[1]);
        auto centerEl = element.param("center");
        std::vector<float> centerFloats = getFloatArgs(centerEl, 3);
        auto obj = std::make_shared<Sphere>(centerFloats[0], centerFloats[1], centerFloats[2], radius);
        buildTraceable(element, *obj);
        obj->print();// TEMP
        container.add(obj);
    }
    else if(keyword == "circlearealight") {
        auto radiusEl = element.param("radius");
        float radius = std::stof(radiusEl[1]);
        auto powerEl = element.param("power");
        std::vector<float> powerFloats = getFloatArgs(powerEl, 3);
        RGBColor power(powerFloats[0], powerFloats[1], powerFloats[2]);
        auto obj = std::make_shared<CircleAreaLight>(radius, power);
        buildTraceable(element, *obj);
        obj->print(); // TEMP
        container.add(obj);
    }
    else if(keyword == "pointlight") {
        auto positionEl = element.param("position");
        std::vector<float> positionFloats = getFloatArgs(positionEl, 3);
        Vector4 position(positionFloats[0], positionFloats[1], positionFloats[2]);
        auto powerEl = element.param("power");
        std::vector<float> powerFloats = getFloatArgs(powerEl, 3);
        RGBColor bandPower(powerFloats[0], powerFloats[1], powerFloats[2]);
        PointLight light(position, bandPower);
        testScene.scene->addPointLight(light);
    }
    else {
        throw UnknownKeywordException(keyword);
    }
}

bool buildScene(SceneFileElement & root, TestScene & testScene)
{
    auto container = std::make_shared<FlatContainer>();
    testScene.scene = new Scene(); new Scene(); new Scene(); new Scene(); 
    testScene.scene->root = container;

    try {
        buildSceneElement(root, testScene, *container);
    }
    catch(std::exception & e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return false;
    }
    catch(...) {
        std::cout << "Caught generic exception" << std::endl;
        return false;
    }

    return true;
}

bool loadTestSceneFromFile(const std::string & sceneFile, TestScene & testScene)
{
    auto & logger = getLogger();
    
    std::ifstream ifs(sceneFile, std::ios::in);
    if(!ifs.good()) {
        logger.error() << "Unable to open " << sceneFile;
        return false;
    }

    std::vector<int> indentStack{0};
    SceneFileElement rootElement;
    rootElement.tokens.push_back("root");
    std::vector<SceneFileElement*> elementStack;
    SceneFileElement * element = &rootElement;

    for(std::string line; std::getline(ifs, line); ) {
        // Track indentation
        auto indent = line.find_first_not_of(" ");
        if(indent == std::string::npos)
            continue;
        std::cout << "LINE (in " << indent << ") : " << line << "\n";
        if(indent > indentStack.back()) {
            indentStack.push_back(indent);
            elementStack.push_back(element);
            element = &element->children.back();
        }
        else {
            while(indent < indentStack.back() && indentStack.size() > 1) {
                indentStack.pop_back();
                element = elementStack.back();
                elementStack.pop_back();
            }
        }

        // Get tokens
        auto tokens = tokenize(line, " ");
        //std::cout << "TOKENS size " << tokens.size() << "\n";
        if(tokens.size() == 0)
            continue;

        SceneFileElement child;
        child.tokens = tokens;
        element->children.push_back(child);

        //auto & keyword = tokens[0];
        //std::cout << "KEYWORD " << keyword << "\n";

    }

    rootElement.print();

    if(!buildScene(rootElement, testScene)) {
        logger.error() << "Unable to build scene from scene description";
        return false;
    }

    return true;
}
