
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
	EnvironmentMap.h \
	FlatContainer.h \
	GeometryUtils.h \
	GoochShader.h \
	Image.h \
	ImageTracer.h \
	Lights.h \
	Material.h \
	Matrix.h \
	Plot2D.h \
	Quaternion.h \
	RandomNumberGenerator.h \
	Ray.h \
	Scene.h \
	SimpleCamera.h \
	Shader.h \
	Sphere.h \
	TMOctreeAccelerator.h \
	TestScenes.h \
	Timer.h \
	Traceable.h \
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
	EnvironmentMap.o \
	FlatContainer.o \
	GeometryUtils.o \
	GoochShader.o \
	Image.o \
	ImageTracer.o \
    Lights.o \
	Material.o \
	Matrix.o \
	Plot2D.o \
	Quaternion.o \
	RandomNumberGenerator.o \
	Ray.o \
	Scene.o \
	SimpleCamera.o \
	Sphere.o \
	TestScenes.o \
	TMOctreeAccelerator.o \
	Timer.o \
	Traceable.o \
	TraceLog.o \
	Transform.o \
	TriangleMesh.o \
	Vector.o \

frOBJ = $(OBJ) \
	main.o

fruiOBJ = $(OBJ) \
	ui.o

test_randomOBJ = $(OBJ) \
	test_random.o
test_renderOBJ = $(OBJ) \
	test_render.o
test_materialsOBJ = $(OBJ) \
	test_materials.o
test_ray_traceOBJ = $(OBJ) \
	test_ray_trace.o
test_samplersOBJ = $(OBJ) \
	test_samplers.o

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
frLDXXFLAGS = $(LDXXFLAGS)
fruiLDXXFLAGS = $(LDXXFLAGS) -framework GLUT -framework OpenGL
test_randomLDXXFLAGS = $(LDXXFLAGS)
test_samplersLDXXFLAGS = $(LDXXFLAGS)
test_renderLDXXFLAGS = $(LDXXFLAGS)
test_materialsLDXXFLAGS = $(LDXXFLAGS)
test_ray_traceLDXXFLAGS = $(LDXXFLAGS)

#all: fr frui tests
all: fr tests

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
#ld -o fr $(frOBJ_IN_DIR) $(frLDXXFLAGS)

#frui: $(fruiOBJ_IN_DIR)
#	ld -o frui $(fruiOBJ_IN_DIR) $(fruiLDXXFLAGS)

tests: $(test_randomOBJ_IN_DIR) $(test_renderOBJ_IN_DIR) $(test_materialsOBJ_IN_DIR) $(test_ray_traceOBJ_IN_DIR) $(test_samplersOBJ_IN_DIR)
	g++ -o test_random $(test_randomOBJ_IN_DIR) $(test_randomLDXXFLAGS)
	g++ -o test_render $(test_renderOBJ_IN_DIR) $(test_renderLDXXFLAGS)
	g++ -o test_materials $(test_materialsOBJ_IN_DIR) $(test_materialsLDXXFLAGS)
	g++ -o test_ray_trace $(test_ray_traceOBJ_IN_DIR) $(test_ray_traceLDXXFLAGS)
	g++ -o test_samplers $(test_samplersOBJ_IN_DIR) $(test_samplersLDXXFLAGS)

$(OBJDIR)/%.o : %.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INC)

clean:
	rm -rf $(frOBJ_IN_DIR) $(fruiOBJ_IN_DIR) $(testOBJ_IN_DIR) fr frui test_random test_render test_materials test_ray_trace test_samplers

