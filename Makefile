
OBJ = \
    AmbientOcclusionShader.o \
    Artifacts.o \
	AssetLoader.o \
	AxisAlignedSlab.o \
	BoundingVolume.o \
	Container.o \
	FlatContainer.o \
	Matrix.o \
	RandomNumberGenerator.o \
	Ray.o \
	Scene.o \
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

.cpp.o:
	g++ -c $< -o $@ $(CXXFLAGS) $(INC)

clean:
	rm -rf $(OBJ) fr frui



