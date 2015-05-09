
HDR = \
	AmbientOcclusionShader.h \
	Artifacts.h \
	AssetLoader.h \
	AxisAlignedSlab.h \
	BasicDiffuseSpecularShader.h \
	BoundingVolume.h \
	Color.h \
	Container.h \
	FlatContainer.h \
	Material.h \
	Matrix.h \
    Plot2D.h \
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
	Transform.h \
	TriangleMesh.h \
	Vector.h

OBJ = \
	AmbientOcclusionShader.o \
	Artifacts.o \
	AssetLoader.o \
	AxisAlignedSlab.o \
	BasicDiffuseSpecularShader.o \
	BoundingVolume.o \
	Container.o \
	FlatContainer.o \
	Matrix.o \
    Plot2D.o \
	RandomNumberGenerator.o \
	Ray.o \
	Scene.o \
	SimpleCamera.o \
	Sphere.o \
	TestScenes.o \
	TMOctreeAccelerator.o \
	Timer.o \
	Traceable.o \
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

INC = -I/usr/local/include/ImageMagick-6
CXXFLAGS = -std=c++11
CXXFLAGS += -Wno-deprecated
CXXFLAGS += -O2
CXXFLAGS += -g
LDXXFLAGS = -e _main -lassimp -lMagick++-6.Q16 -lm -lc++ -lc -macosx_version_min 10.9
frLDXXFLAGS = $(LDXXFLAGS)
fruiLDXXFLAGS = $(LDXXFLAGS) -framework GLUT -framework OpenGL
test_randomLDXXFLAGS = $(LDXXFLAGS)
test_renderLDXXFLAGS = $(LDXXFLAGS)

all: fr frui tests

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

$(OBJDIR):
	mkdir $(OBJDIR)

fr: $(frOBJ_IN_DIR)
	ld -o fr $(frOBJ_IN_DIR) $(frLDXXFLAGS)

frui: $(fruiOBJ_IN_DIR)
	ld -o frui $(fruiOBJ_IN_DIR) $(fruiLDXXFLAGS)

tests: $(test_randomOBJ_IN_DIR) $(test_renderOBJ_IN_DIR)
	ld -o test_random $(test_randomOBJ_IN_DIR) $(test_randomLDXXFLAGS)
	ld -o test_render $(test_renderOBJ_IN_DIR) $(test_renderLDXXFLAGS)

$(OBJDIR)/%.o : %.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INC)

clean:
	rm -rf $(frOBJ_IN_DIR) $(fruiOBJ_IN_DIR) fr frui

