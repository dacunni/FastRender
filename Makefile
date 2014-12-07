
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
	main.o \

fruiOBJ = $(OBJ) \
	ui.o

INC = -I/usr/local/include/ImageMagick-6
CXXFLAGS = -std=c++11
CXXFLAGS += -Wno-deprecated
CXXFLAGS += -O2
CXXFLAGS += -g
LDXXFLAGS = -e _main -lassimp -lMagick++-6.Q16 -lm -lc++ -lc -macosx_version_min 10.9
frLDXXFLAGS = $(LDXXFLAGS)
fruiLDXXFLAGS = $(LDXXFLAGS) -framework GLUT -framework OpenGL

all: fr frui

fr: $(frOBJ)
	ld -o fr $(frOBJ) $(frLDXXFLAGS)

frui: $(fruiOBJ)
	ld -o frui $(fruiOBJ) $(fruiLDXXFLAGS)

.cpp.o: $(HDR)
	g++ -c $< -o $@ $(CXXFLAGS) $(INC)

clean:
	rm -rf $(frOBJ) $(fruiOBJ) fr frui

