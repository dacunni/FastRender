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

class Traceable;

class AssetLoader {
public:
    AssetLoader() {}
    ~AssetLoader() {}
    
    Traceable * load( const std::string & filename );

};


#endif /* defined(__FastRender__AssetLoader__) */
