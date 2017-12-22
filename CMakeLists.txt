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
    CSG.cpp
    Container.cpp
    DirectAreaLightShader.cpp
    DirectPointLightShader.cpp
    DistributionSamplers.cpp
    Editor.cpp
    EditorSceneGraph.cpp
    EnvironmentMap.cpp
    FlatContainer.cpp
    GeometryUtils.cpp
    GoochShader.cpp
    HDRImage.cpp
    Image.cpp
    ImageTracer.cpp
    InspectionShader.cpp
    Lights.cpp
    Material.cpp
    Matrix.cpp
    OpenGLUtil.cpp
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

ADD_LIBRARY(FastRender ${SRCS})
ADD_EXECUTABLE (fr main.cpp)
ADD_EXECUTABLE (test_materials test_materials.cpp)
ADD_EXECUTABLE (test_render test_render.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated")

include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIR})

link_directories(/usr/local/lib)

set(LIBS
    -L/usr/local/lib
    FastRender
    ${ASSIMP_LIBRARIES}
    ${GLUT_LIBRARY}
    ${GLU_LIBRARY}
    ${OpenGL_LIBRARY}
    ${ImageMagick_LIBRARIES}
    m
    c)

TARGET_LINK_LIBRARIES (fr ${LIBS})
TARGET_LINK_LIBRARIES (test_materials ${LIBS})
TARGET_LINK_LIBRARIES (test_render ${LIBS})

