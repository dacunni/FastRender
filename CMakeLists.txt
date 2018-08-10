cmake_minimum_required(VERSION 2.6)

PROJECT (fastrender)

find_package(ImageMagick COMPONENTS Magick++)
find_package(Assimp REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLU REQUIRED)
find_package(OpenGL REQUIRED)

set(SRCS
    AmbientOcclusionShader.cpp
    Artifacts.cpp
    AssetLoader.cpp
    AxisAlignedSlab.cpp
    BasicDiffuseSpecularShader.cpp
    Boolean.cpp
    BoundingVolume.cpp
    BoundingVolumeHierarchy.cpp
    Camera.cpp
    CSG.cpp
    Color.cpp
    Config.cpp
    CookTorranceMaterial.cpp
    Container.cpp
    DirectAreaLightShader.cpp
    DirectEnvironmentMapShader.cpp
    DirectPointLightShader.cpp
    DistributionSamplers.cpp
    Editor.cpp
    EditorSceneGraph.cpp
    EnvironmentMap.cpp
    FlatContainer.cpp
    Fresnel.cpp
    GeometryUtils.cpp
    GoochShader.cpp
    HDRImage.cpp
    Image.cpp
    ImageTracer.cpp
    InspectionShader.cpp
    Lights.cpp
    Logger.cpp
    Material.cpp
    Matrix.cpp
    Microfacet.cpp
    OmniCamera.cpp
    OpenGLUtil.cpp
    OpenGLWindow.cpp
    Plot2D.cpp
    PreviewWindow.cpp
    Quaternion.cpp
    RGBImage.cpp
    RandomNumberGenerator.cpp
    Ray.cpp
    Scene.cpp
    Shader.cpp
    ShaderProgram.cpp
    SignedDistanceFunction.cpp
    SimpleCamera.cpp
    Sphere.cpp
    SurfaceTexture.cpp
    TMOctreeAccelerator.cpp
    TestScenes.cpp
    Timer.cpp
    TraceLog.cpp
    Traceable.cpp
    TraceableVisitor.cpp
    Transform.cpp
    TriangleMesh.cpp
    Vector.cpp
    )

ADD_LIBRARY(FastRender SHARED ${SRCS})

ADD_EXECUTABLE (fr main.cpp)
ADD_EXECUTABLE (fredit editormain.cpp)
ADD_EXECUTABLE (test_materials test_materials.cpp)
ADD_EXECUTABLE (test_render test_render.cpp)
ADD_EXECUTABLE (test_ray_trace test_ray_trace.cpp)
ADD_EXECUTABLE (test_samplers test_samplers.cpp)
ADD_EXECUTABLE (test_random test_random.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated -O2")

include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIR})

link_directories(/usr/local/lib)

set(LIBS
    -L/usr/local/lib
    ${ASSIMP_LIBRARIES}
    ${GLUT_LIBRARY}
    ${GLU_LIBRARY}
    ${OpenGL_LIBRARY}
    ${ImageMagick_LIBRARIES}
    m
    c)

TARGET_LINK_LIBRARIES (FastRender ${LIBS})

TARGET_LINK_LIBRARIES (fr FastRender ${LIBS})
TARGET_LINK_LIBRARIES (fredit FastRender ${LIBS})
TARGET_LINK_LIBRARIES (test_materials FastRender ${LIBS})
TARGET_LINK_LIBRARIES (test_render FastRender ${LIBS})
TARGET_LINK_LIBRARIES (test_ray_trace FastRender ${LIBS})
TARGET_LINK_LIBRARIES (test_samplers FastRender ${LIBS})
TARGET_LINK_LIBRARIES (test_random FastRender ${LIBS})

ADD_CUSTOM_TARGET(link_shaders ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ../shaders shaders)
ADD_CUSTOM_TARGET(link_test_scenes ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ../test_scenes test_scenes)
ADD_CUSTOM_TARGET(link_models ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ../models models)
ADD_CUSTOM_TARGET(link_light_probes ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ../light_probes light_probes)

