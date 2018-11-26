#ifndef _TEST_SCENES_H_
#define _TEST_SCENES_H_

#include "AssetLoader.h"
#include "ImageTracer.h"

class Container;
class FlatContainer;
class Logger;

std::shared_ptr<TriangleMesh> loadMaterialTestModel( AssetLoader & loader );

std::shared_ptr<HDRImageEnvironmentMap> loadStPetersEnvironmentMap();
std::shared_ptr<HDRImageEnvironmentMap> loadUffiziEnvironmentMap();
std::shared_ptr<HDRImageEnvironmentMap> loadRNLEnvironmentMap();
std::shared_ptr<HDRImageEnvironmentMap> loadGraceEnvironmentMap();

//
// Test Registry
//
// The following classes utilize type erasure to allow us to
// statically register tests at compile time during their
// declaration, either by using REGISTER_TEST(TEST_CLASS) or
// automatically when using the BEGIN_SCENE(TEST_CLASS) or
// BEGIN_DERIVED_SCENE(TEST_CLASS) macros.
//

// Base class used for type erasure in the registry
class TestRegistryEntryErased {
    public:
        TestRegistryEntryErased(const char * _name) : name(_name) {}
        virtual ~TestRegistryEntryErased() = default;
        std::string name = "undefined";
        virtual void run() = 0;
};

// Test registry entry for a class. Stores the name and gives us
// a means to invoke the test's run() method.
template<class TEST_CLASS>
class TestRegistryEntry : public TestRegistryEntryErased {
    public:
        TestRegistryEntry(const char * _name) : TestRegistryEntryErased(_name) {}
        virtual ~TestRegistryEntry() = default;
        virtual void run() { TEST_CLASS::run(); }
};

extern std::vector<TestRegistryEntryErased *> testRegistry;

// This class is used by the REGISTER_TEST() macro to register a test.
// The macro declares a static instance of this class in the global scope,
// ensuring that its constructor is called before we enter main(), so
// all tests are guaranteed to be registered before we first access
// testRegistry.
template<class TEST_CLASS>
class RegisterTest {
    public:
        RegisterTest<TEST_CLASS>(const char * name) {
            testRegistry.push_back(new TestRegistryEntry<TEST_CLASS>(name));
        }
};

// Macro used to register a test
#define REGISTER_TEST(TEST_NAME) \
    class TEST_NAME; \
    static RegisterTest<TEST_NAME> registry_##TEST_NAME(#TEST_NAME);

// Prints a list of all tests in the test registry
void printTests();
void logTests(Logger & logger);

// Calls the run() method on all tests in the registry
void runTest( unsigned int index );
void runTests();


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
    std::string output_dir;
    int rays_per_pixel = 10;
    int image_width = 256;
    int image_height = 256;
    int anim_frames = 1;
    AssetLoader loader;
    ImageTracer * tracer;
    Logger & logger;
};

#define BEGIN_DERIVED_SCENE(TEST_NAME, PARENT) \
    REGISTER_TEST(TEST_NAME); \
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
    REGISTER_TEST(TEST_NAME); \
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
        scene->root = container; \
    }

#define END_SCENE() \
    };

bool loadTestSceneFromFile(const std::string & sceneFile, TestScene & scene);

#endif
