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
#include "CookTorranceMaterial.h"
#include "AssetLoader.h"
#include "TMOctreeAccelerator.h"
#include "BoundingVolume.h"
#include "BoundingVolumeHierarchy.h"
#include "EnvironmentMap.h"
#include "FlatContainer.h"
#include "BasicDiffuseSpecularShader.h"
#include "GoochShader.h"
#include "AmbientOcclusionShader.h"
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
struct FileReadException : SceneBuildException {
    FileReadException(const std::string & name) {
        msg = "Error reading file: " + name;
    }
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
        // We iterate until we find the last match so files using 'include'
        // can override values specified in their includes.
        auto predicate = [&key](const SceneFileElement & child) {
            return child.tokens[0] == key;
        };
        auto it = std::find_if(children.rbegin(), children.rend(), predicate);
        if(it != children.rend()) {
            return *it;
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

std::vector<unsigned int> getUInts(std::vector<std::string>::iterator first,
                                   std::vector<std::string>::iterator last)
{
    std::vector<unsigned int> values;
    std::transform(first, last, back_inserter(values), [](std::string & s) { return std::stoi(s); } );
    return values;
}

std::vector<float> getFloatArgs(SceneFileElement & element, unsigned int numArgs)
{
    return getFloats(element.tokens.begin() + 1, element.tokens.begin() + 1 + numArgs);
}

std::vector<unsigned int> getUIntArgs(SceneFileElement & element, unsigned int numArgs)
{
    return getUInts(element.tokens.begin() + 1, element.tokens.begin() + 1 + numArgs);
}

Vector4 getVectorArgs(SceneFileElement & element)
{
    auto args = getFloatArgs(element, 3);
    return Vector4(args[0], args[1], args[2]);
}

RGBColor getRGBArgs(SceneFileElement & element)
{
    auto args = getFloatArgs(element, 3);
    return RGBColor(args[0], args[1], args[2]);
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
    else if(name == "Refractive") {
        float ior = std::stof(element.param("index")[1]);
        return std::make_shared<RefractiveMaterial>(ior);
    }
    else if(name == "CookTorrance") {
        auto & albedoEl = element.param("albedo");
        std::vector<float> albedoValues = getFloatArgs(albedoEl, 3);
        float roughness = std::stof(element.param("roughness")[1]);
        auto material = std::make_shared<CookTorranceMaterial>(albedoValues[0], albedoValues[1], albedoValues[2],
                                                               roughness);
        try {
            float f0 = std::stof(element.param("f0")[1]);
            material->setFresnelAtNormal(f0);
        }
        catch (ParamNotFoundException &) {}
        return material;
    }
    // TODO - all materials
    else { throw UnknownMaterialException(name); }
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
        else { throw InvalidArgumentListException(); }
    }
    else if(xfType == "translate") {
        if(tokens.size() == 4) { // dx dy dz
            std::vector<float> params = getFloats(tokens.begin() + 1, tokens.begin() + 4);
            return makeTranslation(params[0], params[1], params[2]);
        }
        else { throw InvalidArgumentListException(); }
    }
    else if(xfType == "rotate") {
        if(tokens.size() == 5) { // angle x y z
            std::vector<float> params = getFloats(tokens.begin() + 1, tokens.begin() + 5);
            const Vector4 axis(params[1], params[2], params[3]);
            return makeRotation(params[0], axis);
        }
        else { throw InvalidArgumentListException(); }
    }
    throw UnknownKeywordException(xfType);
}

std::shared_ptr<Transform> makeCompositeTransform(SceneFileElement & element)
{
    auto transform = std::make_shared<Transform>();

    for(auto & child : element.children) {
        *transform = compose(*transform, makeSingleTransform(child));
    }

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

std::map<std::string, std::vector<std::string>> macros;

void buildSceneElement(SceneFileElement & element, TestScene & testScene, Container & container, AssetLoader & loader)
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

        auto imageSize     = getUIntArgs(element.param("imagesize"), 2);
        auto numAnimFrames = getUIntArgs(element.param("animframes"), 1)[0];
        auto raysPerPixel  = getUIntArgs(element.param("raysperpixel"), 1)[0];

        testScene.tracer = new ImageTracer(imageSize[0], imageSize[1],
                                           numAnimFrames, raysPerPixel);
        testScene.tracer->artifacts.output_path = outputPath;
        testScene.tracer->artifacts.file_prefix = testName + "_";

        for(auto & child : element.children) {
            buildSceneElement(child, testScene, container, loader);
        }
    }
    else if(keyword == "camera") {
        // optional: transform
        try {
            auto transform = makeCompositeTransform(element.param("transform"));
            testScene.tracer->setCameraTransform(*transform);
        }
        catch (ParamNotFoundException &) {}
        // optional: dimensions
        try {
            auto dims = getFloatArgs(element.param("dimensions"), 2);
            testScene.tracer->camera->setFocalPlaneDimensions(dims[0], dims[1]);
        }
        catch (ParamNotFoundException &) {}
    }
    else if(keyword == "tracer") {
        auto shaderType = element.param("shader")[1];
        if(shaderType == "BasicDiffuseSpecular") {
            testScene.tracer->shader = new BasicDiffuseSpecularShader();
        }
        else if(shaderType == "Gooch") {
            testScene.tracer->shader = new GoochShader();
        }
        else if(shaderType == "AmbientOcclusion") {
            testScene.tracer->shader = new AmbientOcclusionShader();
        }
        else {
            throw UnknownShaderException(shaderType);
        }
    }
    else if(keyword == "axisalignedslab") {
        auto minCoord = getVectorArgs(element.param("min"));
        auto maxCoord = getVectorArgs(element.param("max"));
        auto obj = std::make_shared<AxisAlignedSlab>(minCoord, maxCoord);
        buildTraceable(element, *obj);
        obj->print();// TEMP
        container.add(obj);
    }
    else if(keyword == "mesh") {
        bool multipart = element.optionalFlag("multipart");
        auto path = element.param("path")[1];
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
        float radius = getFloatArgs(element.param("radius"), 1)[0];
        auto center = getVectorArgs(element.param("center"));
        auto obj = std::make_shared<Sphere>(center, radius);
        buildTraceable(element, *obj);
        obj->print();// TEMP
        container.add(obj);
    }
    else if(keyword == "circlearealight") {
        float radius = getFloatArgs(element.param("radius"), 1)[0];
        auto power = getRGBArgs(element.param("power"));
        auto obj = std::make_shared<CircleAreaLight>(radius, power);
        buildTraceable(element, *obj);
        obj->print(); // TEMP
        container.add(obj);
    }
    else if(keyword == "pointlight") {
        auto position = getVectorArgs(element.param("position"));
        auto power = getRGBArgs(element.param("power"));
        PointLight light(position, power);
        testScene.scene->addPointLight(light);
    }
    else if(keyword == "arclightenvmap") {
        auto direction = getVectorArgs(element.param("direction"));
        direction.makeDirection();
        float radius = getFloatArgs(element.param("radius"), 1)[0];
        float power = getFloatArgs(element.param("power"), 1)[0];
        auto env_map = std::make_shared<ArcLightEnvironmentMap>(direction, radius);
        env_map->setPower(power);
        testScene.scene->env_map = env_map;
    }
    else if(keyword == "hdrenvmap") {
        auto path = element.param("path")[1];
        auto imageSize = getUIntArgs(element.param("imagesize"), 2);
        auto env_map = std::make_shared<HDRImageEnvironmentMap>(path, imageSize[0], imageSize[1]);
        testScene.scene->env_map = env_map;
    }
    else {
        throw UnknownKeywordException(keyword);
    }
}

void buildScene(SceneFileElement & root, TestScene & testScene)
{
    AssetLoader loader;
    auto container = std::make_shared<FlatContainer>();
    testScene.scene = new Scene();

#if 0
    buildSceneElement(root, testScene, *container, loader);

    auto bvh = std::make_shared<BoundingVolumeHierarchy>();
    bvh->build(container);
    testScene.scene->root = bvh;
#else
    testScene.scene->root = container;

    buildSceneElement(root, testScene, *container, loader);
#endif
}

void loadSceneFileElementsFromFile(const std::string & sceneFile, SceneFileElement & rootElement)
{
    std::cout << "Loading scene elements from " << sceneFile << '\n';
    std::ifstream ifs(sceneFile, std::ios::in);
    if(!ifs.good()) { throw FileReadException(sceneFile); }

    std::vector<int> indentStack{0};
    std::vector<SceneFileElement*> elementStack;
    SceneFileElement * element = &rootElement;

    for(std::string line; std::getline(ifs, line); ) {
        // Trim comments
        auto commentStart = line.find("#");
        if(commentStart != std::string::npos) {
            line = line.substr(0, commentStart);
        }
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

        // Replace macros with their expansions
        std::vector<std::string> expandedTokens;
        for(auto ti = tokens.begin(); ti != tokens.end(); ++ti) {
            auto & token = *ti;
            if(token.find("$") == 0) {
                auto key = token.substr(1);
                for(auto & macroToken : macros[key]) {
                    expandedTokens.push_back(macroToken);
                }
            }
            else {
                expandedTokens.push_back(token);
            }
        }
        tokens = std::move(expandedTokens);

        SceneFileElement child;
        child.tokens = tokens;

        if(tokens[0] == "include") {
            auto includeFile = tokens[1];
            auto dirEnd = sceneFile.find_last_of("/");
            std::string prefix;
            if(dirEnd != std::string::npos) {
                prefix = sceneFile.substr(0, dirEnd + 1);
            }
            loadSceneFileElementsFromFile(prefix + includeFile, *element);
        }
        else if(tokens[0] == "macro") {
            auto key = tokens[1];
            std::vector<std::string> args;
            std::copy(tokens.begin() + 2, tokens.end(), std::back_inserter(args));
            macros[key] = std::move(args);
        }
        else {
            element->children.push_back(child);
        }

        //auto & keyword = tokens[0];
        //std::cout << "KEYWORD " << keyword << "\n";
    }
}

bool loadTestSceneFromFile(const std::string & sceneFile, TestScene & testScene)
{
    auto & logger = getLogger();
    SceneFileElement rootElement;
    rootElement.tokens.push_back("root");

    try {
        loadSceneFileElementsFromFile(sceneFile, rootElement);
        rootElement.print();
        std::cout << macros.size() << " macros\n";
        buildScene(rootElement, testScene);
    }
    catch(std::exception & e) {
        std::cout << "Exception: " << e.what() << std::endl;
        logger.error() << "Unable to build scene from scene description";
        return false;
    }
    catch(...) {
        std::cout << "Caught generic exception" << std::endl;
        logger.error() << "Unable to build scene from scene description";
        return false;
    }

    return true;
}
