//
//  AssetLoader.h
//  FastRender
//
//  Created by David Cunningham on 8/15/13.
//
//

#ifndef __FastRender__AssetLoader__
#define __FastRender__AssetLoader__

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "TriangleMesh.h"

class Container;
class Traceable;
class Logger;

class AssetFileNotFoundException {};

typedef std::vector<std::shared_ptr<TriangleMesh>> TriangleMeshArray;

class AssetLoader {
public:
    AssetLoader();
    ~AssetLoader() = default;
    
    std::shared_ptr<TriangleMesh> load( const std::string & filename,
                                        bool build_accelerator = true ) throw(AssetFileNotFoundException);
    std::shared_ptr<Container>    loadMultiPart( const std::string & filename,
                                                 bool build_accelerator = true ) throw(AssetFileNotFoundException);
    std::shared_ptr<TriangleMesh> loadMultiPartMerged( const std::string & filename,
                                                       bool build_accelerator = true ) throw(AssetFileNotFoundException);

protected:
    void loadTriangleArray( const std::string & filename,
                            TriangleMeshArray & array ) throw(AssetFileNotFoundException);

    std::map<std::string, std::shared_ptr<TriangleMesh::MeshData>> meshDataCache;

    Logger & logger;
};


#endif /* defined(__FastRender__AssetLoader__) */
