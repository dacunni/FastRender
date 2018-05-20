cmake_minimum_required(VERSION 2.6)

PROJECT (fastrender)

find_package(ImageMagick COMPONENTS Magick++)
find_package(Assimp REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLU REQUIRED)
find_package(OpenGL REQUIRED)

set(SRC_ROOT src)
set(APP_SRC_ROOT src/app)
set(TEST_SRC_ROOT src/tests)

set(SRCS
    ${SRC_ROOT}/cameras/Camera.cpp
    ${SRC_ROOT}/cameras/SimpleCamera.cpp
    ${SRC_ROOT}/cameras/OmniCamera.cpp
    ${SRC_ROOT}/geometry/Matrix.cpp
    ${SRC_ROOT}/geometry/Transform.cpp
    ${SRC_ROOT}/geometry/Vector.cpp
    ${SRC_ROOT}/shading/AmbientOcclusionShader.cpp
    ${SRC_ROOT}/shading/BasicDiffuseSpecularShader.cpp
    ${SRC_ROOT}/shading/DirectAreaLightShader.cpp
    ${SRC_ROOT}/shading/DirectEnvironmentMapShader.cpp
    ${SRC_ROOT}/shading/DirectPointLightShader.cpp
    ${SRC_ROOT}/shading/GoochShader.cpp
    ${SRC_ROOT}/shading/InspectionShader.cpp
    ${SRC_ROOT}/shading/Shader.cpp
    ${SRC_ROOT}/traceable/Boolean.cpp
    ${SRC_ROOT}/traceable/BoundingVolume.cpp
    ${SRC_ROOT}/traceable/BoundingVolumeHierarchy.cpp
    ${SRC_ROOT}/traceable/CSG.cpp
    ${SRC_ROOT}/traceable/AxisAlignedSlab.cpp
    ${SRC_ROOT}/traceable/Container.cpp
    ${SRC_ROOT}/traceable/SignedDistanceFunction.cpp
    ${SRC_ROOT}/traceable/Sphere.cpp
    ${SRC_ROOT}/traceable/TMOctreeAccelerator.cpp
    ${SRC_ROOT}/traceable/Traceable.cpp
    ${SRC_ROOT}/traceable/TriangleMesh.cpp
    ${SRC_ROOT}/Artifacts.cpp
    ${SRC_ROOT}/AssetLoader.cpp
    ${SRC_ROOT}/Color.cpp
    ${SRC_ROOT}/Config.cpp
    ${SRC_ROOT}/CookTorranceMaterial.cpp
    ${SRC_ROOT}/DistributionSamplers.cpp
    ${SRC_ROOT}/Editor.cpp
    ${SRC_ROOT}/EditorSceneGraph.cpp
    ${SRC_ROOT}/EnvironmentMap.cpp
    ${SRC_ROOT}/FlatContainer.cpp
    ${SRC_ROOT}/GeometryUtils.cpp
    ${SRC_ROOT}/HDRImage.cpp
    ${SRC_ROOT}/Image.cpp
    ${SRC_ROOT}/ImageTracer.cpp
    ${SRC_ROOT}/Lights.cpp
    ${SRC_ROOT}/Logger.cpp
    ${SRC_ROOT}/Material.cpp
    ${SRC_ROOT}/OpenGLUtil.cpp
    ${SRC_ROOT}/Plot2D.cpp
    ${SRC_ROOT}/PreviewWindow.cpp
    ${SRC_ROOT}/Quaternion.cpp
    ${SRC_ROOT}/RGBImage.cpp
    ${SRC_ROOT}/RandomNumberGenerator.cpp
    ${SRC_ROOT}/Ray.cpp
    ${SRC_ROOT}/Scene.cpp
    ${SRC_ROOT}/ShaderProgram.cpp
    ${SRC_ROOT}/SurfaceTexture.cpp
    ${SRC_ROOT}/TestScenes.cpp
    ${SRC_ROOT}/Timer.cpp
    ${SRC_ROOT}/TraceLog.cpp
    ${SRC_ROOT}/TraceableVisitor.cpp
    )

ADD_LIBRARY(FastRender SHARED ${SRCS})

ADD_EXECUTABLE (fr ${APP_SRC_ROOT}/main.cpp)
ADD_EXECUTABLE (fredit ${APP_SRC_ROOT}/editormain.cpp)
ADD_EXECUTABLE (test_materials ${TEST_SRC_ROOT}/test_materials.cpp)
ADD_EXECUTABLE (test_render ${TEST_SRC_ROOT}/test_render.cpp)
ADD_EXECUTABLE (test_ray_trace ${TEST_SRC_ROOT}/test_ray_trace.cpp)
ADD_EXECUTABLE (test_samplers ${TEST_SRC_ROOT}/test_samplers.cpp)
ADD_EXECUTABLE (test_random ${TEST_SRC_ROOT}/test_random.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated -O2")

include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIR})

include_directories(.)
include_directories(src)
include_directories(src/cameras)
include_directories(src/geometry)
include_directories(src/shading)
include_directories(src/traceable)

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

