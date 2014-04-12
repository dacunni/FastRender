
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
	Sphere.o \
    TestScenes.o \
    TMOctreeAccelerator.o \
	Timer.o \
	Traceable.o \
	Transform.o \
	TriangleMesh.o \
	Vector.o \
	main.o \

INC = -I/usr/local/include/ImageMagick-6
CXXFLAGS = -std=c++11
CXXFLAGS += -O2
CXXFLAGS += -g
LDXXFLAGS = -e _main -lassimp -lMagick++-6.Q16 -lm -lc++ -lc -macosx_version_min 10.9

fr: $(OBJ)
	ld -o fr $(OBJ) $(LDXXFLAGS)

.cpp.o: $(HDR)
	g++ -c $< -o $@ $(CXXFLAGS) $(INC)

clean:
	rm -rf $(OBJ) fr

