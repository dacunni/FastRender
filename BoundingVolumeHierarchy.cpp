
#include <algorithm>
#include <list>
#include <iostream>
#include <cfloat>

#include "BoundingVolumeHierarchy.h"
#include "AxisAlignedSlab.h"
#include "FlatContainer.h"


bool BoundingVolumeHierarchy::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    if( root ) {
        return root->intersect( ray, intersection );
    }

    return false;
}

bool BoundingVolumeHierarchy::intersectsAny( const Ray & ray, float min_distance ) const
{
    if( root ) {
        return root->intersectsAny( ray, min_distance );
    }

    return false;
}


void BoundingVolumeHierarchy::build( std::shared_ptr<Container> container )
{
    if( container->size() < 1 )
        return;
    
    std::list<std::shared_ptr<BoundingVolume> > objects;

    std::cout << "container.size = " << container->size() << std::endl; // TEMP

    // Create a BoundingVolume node for each object
    const int num_objects = container->size();
    for( int i = 0; i < num_objects; i++ ) {
        objects.push_back( std::make_shared<BoundingVolume>( container->at(i) ) );
    }

    std::cout << "objects.size = " << objects.size() << std::endl; // TEMP

    auto compare = []( std::shared_ptr<BoundingVolume> a, std::shared_ptr<BoundingVolume> b ) {
                           return std::static_pointer_cast<AxisAlignedSlab>(a->bound)->volume() 
                              < std::static_pointer_cast<AxisAlignedSlab>(b->bound)->volume();
                       };
    objects.sort( compare );

    while(1) {

        if( objects.size() == 1 ) {
            root = objects.front();
            return;
        }
#if 0

        std::list<BoundingVolume*>::iterator p1, p2;
        bool found = false;

        float best_volume = FLT_MAX;
        std::cout << "Initial volume = " << best_volume << std::endl; // TEMP

        // Check all pairings of bounding volumes and find a pair that has a minimum combined volume
        for( auto iter1 = objects.begin(); iter1 != objects.end(); ++iter1 ) {
            AxisAlignedSlab * bound1 = static_cast<AxisAlignedSlab*>((*iter1)->bound);
            //std::cout << " bound1 = " << bound1->volume() << std::endl; // TEMP
            for( auto iter2 = objects.begin(); iter2 != objects.end(); ++iter2 ) {
                AxisAlignedSlab * bound2 = static_cast<AxisAlignedSlab*>((*iter2)->bound);
                //std::cout << "   bound2 = " << bound2->volume() << std::endl; // TEMP
                AxisAlignedSlab combined = merge( *bound1, *bound2 );
                float volume = combined.volume();
                //std::cout << "      combined = " << volume << std::endl; // TEMP

                if( (!found || volume < best_volume) && iter1 != iter2 ) {
                    std::cout << "Found better volume = " << volume << ", was = " << best_volume << std::endl; // TEMP
                    p1 = iter1;
                    p2 = iter2;
                    best_volume = volume;
                    found = true;
                }
            }
        }

        std::cout << "BEST volume = " << best_volume << std::endl; // TEMP

        // Replace the best partner with a node containing both nodes
        FlatContainer * pair = new FlatContainer();
        pair->add( *p1 );
        pair->add( *p2 );

        objects.erase( p1 );
        objects.erase( p2 );

        objects.push_front( new BoundingVolume( pair ) );

        std::cout << "objects.size = " << objects.size() << std::endl; // TEMP

#else
        auto o = objects.front();
        objects.pop_front();

        std::list<std::shared_ptr<BoundingVolume> >::iterator partner, iter;

        // Greedily take the node with the smallest bounding volume and combine it with
        // whichever node minimizes their combined bounding volume.
        auto o_bound = std::static_pointer_cast<AxisAlignedSlab>(o->bound);
        float best_volume = o_bound->volume();
        //std::cout << "Initial volume = " << best_volume << std::endl; // TEMP
        partner = objects.end();
        for( iter = objects.begin(); iter != objects.end(); ++iter ) {
            auto o2_bound = std::static_pointer_cast<AxisAlignedSlab>((*iter)->bound);
            //std::cout << "  volume = " << o2_bound->volume(); // TEMP
            AxisAlignedSlab combined = merge( *o_bound, *o2_bound );
            float volume = combined.volume();
            //std::cout << " , combined = " << volume << std::endl; // TEMP
            if( partner == objects.end() || volume < best_volume ) {
                //std::cout << "Found better volume = " << volume << ", was = " << best_volume << std::endl; // TEMP
                partner = iter;
                best_volume = volume;
            }
        }

        //std::cout << "BEST volume = " << best_volume << std::endl; // TEMP

        // Replace the best partner with a node containing both nodes
        auto pair = std::make_shared<FlatContainer>();
        pair->add( o );
        pair->add( *partner );

        objects.erase( partner );

        objects.push_front( std::make_shared<BoundingVolume>( pair ) );

        std::cout << "objects.size = " << objects.size() << std::endl; // TEMP

        // Resort them
        objects.sort( compare );

#endif
    }
}


void BoundingVolumeHierarchy::print( FILE * file ) const
{
    // TODO
}



