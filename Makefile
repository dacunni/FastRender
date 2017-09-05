
HDR = \
	AmbientOcclusionShader.h \
	Artifacts.h \
	AssetLoader.h \
	AxisAlignedSlab.h \
	BasicDiffuseSpecularShader.h \
	Boolean.h \
	BoundingVolume.h \
	BoundingVolumeHierarchy.h \
	Color.h \
	Container.h \
	CSG.h \
	DistributionSamplers.h \
    EditorSceneGraph.h \
	EnvironmentMap.h \
	FlatContainer.h \
	GeometryUtils.h \
	GoochShader.h \
    HDRImage.h \
	Image.h \
	ImageTracer.h \
	InspectionShader.h \
	Lights.h \
	Material.h \
	Matrix.h \
    OpenGLUtil.h \
	Plot2D.h \
	PreviewWindow.h \
	Quaternion.h \
	RandomNumberGenerator.h \
	Ray.h \
    RGBImage.h \
	Scene.h \
	SignedDistanceFunction.h \
	SimpleCamera.h \
	Shader.h \
	Sphere.h \
    SurfaceTexture.h \
	TMOctreeAccelerator.h \
	TestScenes.h \
	Timer.h \
	Traceable.h \
	TraceableVisitor.h \
	TraceLog.h \
	Transform.h \
	TriangleMesh.h \
	Vector.h

OBJ = \
	AmbientOcclusionShader.o \
	Artifacts.o \
	AssetLoader.o \
	AxisAlignedSlab.o \
	BasicDiffuseSpecularShader.o \
	Boolean.o \
	BoundingVolume.o \
	BoundingVolumeHierarchy.o \
	Container.o \
	CSG.o \
	DistributionSamplers.o \
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
	Vector.o \

frOBJ = $(OBJ) \
	main.o

fruiOBJ = $(OBJ) \
	ui.o

test_randomOBJ = $(OBJ) test_random.o
test_renderOBJ = $(OBJ) test_render.o
test_materialsOBJ = $(OBJ) test_materials.o
test_ray_traceOBJ = $(OBJ) test_ray_trace.o
test_samplersOBJ = $(OBJ) test_samplers.o

UNAME_S := $(shell uname -s)

#
# Compiler flags
#
INC = -I/usr/local/include
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
CXXFLAGS += -DNDEBUG

ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -mmacosx-version-min=10.10
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
fruiLDXXFLAGS = $(LDXXFLAGS) -framework GLUT -framework OpenGL
test_randomLDXXFLAGS = $(LDXXFLAGS)
test_samplersLDXXFLAGS = $(LDXXFLAGS)
test_renderLDXXFLAGS = $(LDXXFLAGS)
test_materialsLDXXFLAGS = $(LDXXFLAGS)
test_ray_traceLDXXFLAGS = $(LDXXFLAGS)

all: fr frui tests
#all: fr tests python_bindings
#all: fr tests

# Stash object files away in a separate directory so we don't have 
# to look at them
OBJDIR = objs

frOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(frOBJ))
$(frOBJ_IN_DIR): | $(OBJDIR)

fruiOBJ_IN_DIR = $(addprefix $(OBJDIR)/, $(fruiOBJ))
$(fruiOBJ_IN_DIR): | $(OBJDIR)

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

fr: $(frOBJ_IN_DIR)
	g++ -o fr $(frOBJ_IN_DIR) $(frLDXXFLAGS)

frui: $(fruiOBJ_IN_DIR)
	g++ -o frui $(fruiOBJ_IN_DIR) $(fruiLDXXFLAGS)

#
# Tests
#
test_random: $(test_randomOBJ_IN_DIR)
	g++ -o $@ $(test_randomOBJ_IN_DIR) $(test_randomLDXXFLAGS)
test_ray_trace: $(test_ray_traceOBJ_IN_DIR)
	g++ -o $@ $(test_ray_traceOBJ_IN_DIR) $(test_ray_traceLDXXFLAGS)
test_samplers: $(test_samplersOBJ_IN_DIR)
	g++ -o $@ $(test_samplersOBJ_IN_DIR) $(test_samplersLDXXFLAGS)

MATERIAL_SCENES = $(wildcard test_scenes/materials/*.scene)
# FIXME: scene dependency not working
test_materials.o: $(MATERIAL_SCENES)
test_materials: $(test_materialsOBJ_IN_DIR)
	g++ -o $@ $(test_materialsOBJ_IN_DIR) $(test_materialsLDXXFLAGS)

RENDER_SCENES = $(wildcard test_scenes/render/*.scene)
# FIXME: scene dependency not working
test_render.o: $(RENDER_SCENES)
test_render: $(test_renderOBJ_IN_DIR)
	g++ -o $@ $(test_renderOBJ_IN_DIR) $(test_renderLDXXFLAGS)

TESTS = test_random test_ray_trace test_samplers test_materials test_render

tests: $(TESTS)

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


$(OBJDIR)/%.o : %.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INC)

clean:
	rm -rf $(frOBJ_IN_DIR) $(fruiOBJ_IN_DIR) $(testOBJ_IN_DIR) fr frui $(TESTS)
	rm -rf _FastRender.so FastRender_wrap.cpp $(OBJDIR)/FastRender_wrap.o FastRender.py

