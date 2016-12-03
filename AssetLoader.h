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

class Container;
class Traceable;
class TriangleMesh;

class AssetFileNotFoundException {};

class AssetLoader {
public:
    AssetLoader() {}
    ~AssetLoader() {}
    
    std::shared_ptr<TriangleMesh> load( const std::string & filename,
                                        bool build_accelerator = true ) throw(AssetFileNotFoundException);
    std::shared_ptr<Container>    loadMultiPart( const std::string & filename,
                                                 bool build_accelerator = true ) throw(AssetFileNotFoundException);
    std::shared_ptr<TriangleMesh> loadMultiPartMerged( const std::string & filename,
                                                       bool build_accelerator = true ) throw(AssetFileNotFoundException);

};


#endif /* defined(__FastRender__AssetLoader__) */
