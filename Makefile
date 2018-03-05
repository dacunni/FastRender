
HDR = $(wildcard *.h) $(wildcard *.hpp)

OBJ = \
	AmbientOcclusionShader.o \
	Artifacts.o \
	AssetLoader.o \
	AxisAlignedSlab.o \
	BasicDiffuseSpecularShader.o \
	Boolean.o \
	BoundingVolume.o \
	BoundingVolumeHierarchy.o \
    Color.o \
	Container.o \
    CookTorranceMaterial.o \
	CSG.o \
    DirectAreaLightShader.o \
    DirectEnvironmentMapShader.o \
    DirectPointLightShader.o \
	DistributionSamplers.o \
    Editor.o \
    EditorSceneGraph.o \
	EnvironmentMap.o \
	FlatContainer.o \
	GeometryUtils.o \
	GoochShader.o \
    HDRImage.o \
	Image.o \
	ImageTracer.o \
	InspectionShader.o \
    Lights.o \
	Material.o \
	Matrix.o \
    OpenGLUtil.o \
	Plot2D.o \
	PreviewWindow.o \
	Quaternion.o \
	RandomNumberGenerator.o \
	Ray.o \
    RGBImage.o \
	Scene.o \
    Shader.o \
    ShaderProgram.o \
	SignedDistanceFunction.o \
	SimpleCamera.o \
	Sphere.o \
    SurfaceTexture.o \
	TestScenes.o \
	TMOctreeAccelerator.o \
	Timer.o \
	Traceable.o \
	TraceableVisitor.o \
	TraceLog.o \
	Transform.o \
	TriangleMesh.o \
	Vector.o

FAST_RENDER_LIB_OBJ = $(OBJ)

frOBJ = $(OBJ) \
	main.o

freditOBJ = $(OBJ) \
	editormain.o

test_randomOBJ = $(OBJ) test_random.o
test_renderOBJ = $(OBJ) test_render.o
#test_materialsOBJ = $(OBJ) test_materials.o
test_materialsOBJ = test_materials.o
test_ray_traceOBJ = $(OBJ) test_ray_trace.o
test_samplersOBJ = $(OBJ) test_samplers.o

UNAME_S := $(shell uname -s)

#
# Compiler flags
#
INC = -I.
INC += -I/usr/local/include
INC += -I/usr/include/ImageMagick
INC += -I/usr/local/include/ImageMagick-6
CXXFLAGS = -std=c++11
CXXFLAGS += -Wno-deprecated
CXXFLAGS += -O2
#CXXFLAGS += -O3
#CXXFLAGS += -ffast-math
# Uncomment to enable symbols
CXXFLAGS += -g
# Uncomment to disable asserts
#CXXFLAGS += -DNDEBUG

ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -mmacosx-version-min=10.10
    # Silence "gl.h and gl3.h are both included.  Compiler will not invoke errors if using removed OpenGL functionality."
    CXXFLAGS += -D__gl_h_ -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
endif
#CXXFLAGS += -v

#
# Linker flags
#
LDXXFLAGS += -L/usr/local/lib
#LDXXFLAGS += -e _main -lassimp -lm -lc
LDXXFLAGS += -framework GLUT -framework OpenGL
ifeq ($(UNAME_S),Linux)
    # FIXME: HACKHACK
    LDXXFLAGS += -L/usr/lib/gcc/x86_64-amazon-linux/4.8.3/
endif
LDXXFLAGS += -lassimp -lm -lc
ifeq ($(UNAME_S),Darwin)
    #LDXXFLAGS += -lc++
    #LDXXFLAGS += -mmacosx-version-min=10.10
    LDXXFLAGS += -lMagick++-6.Q16
else
    LDXXFLAGS += -lMagick++
endif
# Linux
#LDXXFLAGS += -lstdc++
#LDXXFLAGS += -lgcc_s
#LDXXFLAGS += -v
frLDXXFLAGS = $(LDXXFLAGS) -framework GLUT -framework OpenGL
freditLDXXFLAGS = $(LDXXFLAGS) -framework GLUT -framework OpenGL
test_randomLDXXFLAGS = $(LDXXFLAGS)
test_samplersLDXXFLAGS = $(LDXXFLAGS)
test_renderLDXXFLAGS = $(LDXXFLAGS)
test_materialsLDXXFLAGS = $(LDXXFLAGS) -L. -lFastRender
test_ray_traceLDXXFLAGS = $(LDXXFLAGS)

#all: fr tests
#all: fr fredit tests libFastRender.so benchmarks unittests
all: libFastRender.so tests
#all: tests
#all: fr fredit tests python_bindings

# Stash object files away in a separate directory so we don't have 
# to look at them
OBJDIR = objs

FAST_RENDER_LIB_OBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(FAST_RENDER_LIB_OBJ))

frOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(frOBJ))
$(frOBJ_IN_DIR): | $(OBJDIR)

freditOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(freditOBJ))
$(freditOBJ_IN_DIR): | $(OBJDIR)

test_randomOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(test_randomOBJ))
$(test_randomOBJ_IN_DIR): | $(OBJDIR)

test_renderOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(test_renderOBJ))
$(test_renderOBJ_IN_DIR): | $(OBJDIR)

test_materialsOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(test_materialsOBJ))
$(test_materialsOBJ_IN_DIR): | $(OBJDIR)

test_ray_traceOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(test_ray_traceOBJ))
$(test_ray_traceOBJ_IN_DIR): | $(OBJDIR)

test_samplersOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(test_samplersOBJ))
$(test_samplersOBJ_IN_DIR): | $(OBJDIR)

testOBJ_IN_DIR = \
	$(test_randomOBJ_IN_DIR) \
	$(test_renderOBJ_IN_DIR) \
	$(test_materialsOBJ_IN_DIR) \
	$(test_ray_traceOBJ_IN_DIR) \
	$(test_samplersOBJ_IN_DIR)

$(OBJDIR):
	mkdir $(OBJDIR)

libFastRender.so: $(FAST_RENDER_LIB_OBJ_IN_DIR)
	g++ -shared -undefined dynamic_lookup -o libFastRender.so $(FAST_RENDER_LIB_OBJ_IN_DIR)

fr: $(frOBJ_IN_DIR)
	g++ -o fr $(frOBJ_IN_DIR) $(frLDXXFLAGS)

fredit: $(freditOBJ_IN_DIR)
	g++ -o fredit $(freditOBJ_IN_DIR) $(freditLDXXFLAGS)

#
# Tests
#
test_random: $(test_randomOBJ_IN_DIR)
	g++ -o $@ $(test_randomOBJ_IN_DIR) $(test_randomLDXXFLAGS)
test_ray_trace: $(test_ray_traceOBJ_IN_DIR)
	g++ -o $@ $(test_ray_traceOBJ_IN_DIR) $(test_ray_traceLDXXFLAGS)
test_samplers: $(test_samplersOBJ_IN_DIR)
	g++ -o $@ $(test_samplersOBJ_IN_DIR) $(test_samplersLDXXFLAGS)

test_materials: $(test_materialsOBJ_IN_DIR)
	g++ -o $@ $(test_materialsOBJ_IN_DIR) $(test_materialsLDXXFLAGS)

test_render: $(test_renderOBJ_IN_DIR)
	g++ -o $@ $(test_renderOBJ_IN_DIR) $(test_renderLDXXFLAGS)

TESTS = test_random test_ray_trace test_samplers test_materials test_render

tests: $(TESTS)

.PHONY: benchmarks unittests

BENCHMARK_SRC = $(wildcard benchmarks/*.cpp)
BENCHMARKS = $(BENCHMARK_SRC:.cpp=)
benchmarks: $(BENCHMARKS)

BENCHMARK_LDXXFLAGS = $(LDXXFLAGS) -L. -L/usr/local/lib -lFastRender -lbenchmark

benchmarks/vector: benchmarks/vector.cpp $(HDR) libFastRender.so
	g++ -o $@ $< $(CXXFLAGS) $(INC) $(BENCHMARK_LDXXFLAGS)
benchmarks/transform: benchmarks/transform.cpp $(HDR) libFastRender.so
	g++ -o $@ $< $(CXXFLAGS) $(INC) $(BENCHMARK_LDXXFLAGS)
benchmarks/intersect: benchmarks/intersect.cpp $(HDR) libFastRender.so
	g++ -o $@ $< $(CXXFLAGS) $(INC) $(BENCHMARK_LDXXFLAGS)
benchmarks/random: benchmarks/random.cpp $(HDR) libFastRender.so
	g++ -o $@ $< $(CXXFLAGS) $(INC) $(BENCHMARK_LDXXFLAGS)

UNITTEST_SRC = $(wildcard unittests/*.cpp)
UNITTESTS = $(UNITTEST_SRC:.cpp=)
unittests: $(UNITTESTS)

UNITTEST_LDXXFLAGS = $(LDXXFLAGS) -L. -L/usr/local/lib -lFastRender -lgtest

unittests/vector: unittests/vector.cpp $(HDR) libFastRender.so
	g++ -o $@ $< $(CXXFLAGS) $(INC) $(UNITTEST_LDXXFLAGS)
unittests/transform: unittests/transform.cpp $(HDR) libFastRender.so
	g++ -o $@ $< $(CXXFLAGS) $(INC) $(UNITTEST_LDXXFLAGS)
unittests/raysphere: unittests/raysphere.cpp $(HDR) libFastRender.so
	g++ -o $@ $< $(CXXFLAGS) $(INC) $(UNITTEST_LDXXFLAGS)


#
# Python Bindings
#
PYTHON_BINDING_OBJ = FastRender_wrap.o $(OBJ)
PYTHON_BINDING_OBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(PYTHON_BINDING_OBJ))
FastRender_wrap.cpp: FastRender.i $(HDR)
	swig -python -c++ -o FastRender_wrap.cpp $(INC) FastRender.i 
$(OBJDIR)/FastRender_wrap.o: FastRender_wrap.cpp
	g++ -o $(OBJDIR)/FastRender_wrap.o -c FastRender_wrap.cpp $(CXXFLAGS) $(INC) `python-config --includes`
_FastRender.so: $(PYTHON_BINDING_OBJ_IN_DIR)
	g++ -dynamiclib -o _FastRender.so $(PYTHON_BINDING_OBJ_IN_DIR) `python-config --ldflags --libs` $(LDXXFLAGS)
python_bindings: _FastRender.so

MATERIAL_SCENES = $(wildcard test_scenes/materials/*.scene)
$(OBJDIR)/test_materials.o: $(MATERIAL_SCENES)

RENDER_SCENES = $(wildcard test_scenes/render/*.scene)
$(OBJDIR)/test_render.o: $(RENDER_SCENES)

$(OBJDIR)/%.o : %.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INC)


clean:
	rm -f $(frOBJ_IN_DIR) $(freditOBJ_IN_DIR) $(testOBJ_IN_DIR) fr fredit $(TESTS)
	rm -f $(UNITTESTS)
	rm -f $(BENCHMARKS)
	rm -f _FastRender.so FastRender_wrap.cpp $(OBJDIR)/FastRender_wrap.o FastRender.py

