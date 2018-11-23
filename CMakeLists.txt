cmake_minimum_required(VERSION 3.1)

PROJECT (fastrender)

find_package(ImageMagick COMPONENTS Magick++)
find_package(Assimp REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLU REQUIRED)
find_package(OpenGL REQUIRED)

set(SRCS
    src/shader/AmbientOcclusionShader.cpp
    src/shader/BasicDiffuseSpecularShader.cpp
    src/shader/DirectLightingShaders.cpp
    src/shader/GoochShader.cpp
    src/shader/InspectionShader.cpp
    src/shader/Shader.cpp
    src/material/Material.cpp
    src/material/CookTorranceMaterial.cpp
    src/traceable/AxisAlignedSlab.cpp
    src/traceable/Boolean.cpp
    src/traceable/CSG.cpp
    src/traceable/Container.cpp
    src/traceable/FlatContainer.cpp
    src/traceable/SignedDistanceFunction.cpp
    src/traceable/Sphere.cpp
    src/traceable/TMOctreeAccelerator.cpp
    src/traceable/Traceable.cpp
    src/traceable/TraceableVisitor.cpp
    src/traceable/TriangleMesh.cpp
    src/editor/Editor.cpp
    src/editor/EditorSceneGraph.cpp
    src/camera/Camera.cpp
    src/camera/OmniCamera.cpp
    src/camera/SimpleCamera.cpp
    src/Artifacts.cpp
    src/AssetLoader.cpp
    src/BoundingVolume.cpp
    src/BoundingVolumeHierarchy.cpp
    src/Color.cpp
    src/Config.cpp
    src/DistributionSamplers.cpp
    src/EnvironmentMap.cpp
    src/Fresnel.cpp
    src/GeometryUtils.cpp
    src/HDRImage.cpp
    src/Image.cpp
    src/ImageTracer.cpp
    src/Lights.cpp
    src/Logger.cpp
    src/Matrix.cpp
    src/Microfacet.cpp
    src/OpenGLUtil.cpp
    src/OpenGLWindow.cpp
    src/Plot2D.cpp
    src/PreviewWindow.cpp
    src/Quaternion.cpp
    src/RGBImage.cpp
    src/RandomNumberGenerator.cpp
    src/Ray.cpp
    src/Scene.cpp
    src/ShaderProgram.cpp
    src/SurfaceTexture.cpp
    src/TestScenes.cpp
    src/Timer.cpp
    src/TraceLog.cpp
    src/Transform.cpp
    src/Vector.cpp
    )

ADD_LIBRARY(FastRender SHARED ${SRCS})

ADD_EXECUTABLE (fr             src/app/main.cpp)
ADD_EXECUTABLE (fredit         src/editor/editormain.cpp)
ADD_EXECUTABLE (test_materials src/app/test_materials.cpp)
ADD_EXECUTABLE (test_render    src/app/test_render.cpp)
ADD_EXECUTABLE (test_ray_trace src/app/test_ray_trace.cpp)
ADD_EXECUTABLE (test_samplers  src/app/test_samplers.cpp)
ADD_EXECUTABLE (test_random    src/app/test_random.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated -O2")

include_directories(.)
include_directories(src)
include_directories(src/editor)
include_directories(src/shader)
include_directories(src/material)
include_directories(src/traceable)
include_directories(src/camera)
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIR})
include_directories(external/stb)

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
ADD_CUSTOM_TARGET(link_textures ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ../textures textures)
ADD_CUSTOM_TARGET(link_test_scenes ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ../test_scenes test_scenes)
ADD_CUSTOM_TARGET(link_models ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ../models models)
ADD_CUSTOM_TARGET(link_light_probes ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ../light_probes light_probes)

add_subdirectory(benchmarks)
add_subdirectory(unittests)

