
#include <algorithm>
#include <list>
#include <iostream>
#include <cfloat>

#include "BoundingVolumeHierarchy.h"
#include "AxisAlignedSlab.h"
#include "FlatContainer.h"
#include "TraceableVisitor.h"


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
    //buildBottomUp( container );
    buildTopDown( container );
}

void BoundingVolumeHierarchy::buildBottomUp( std::shared_ptr<Container> container )
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
        auto o = objects.front();
        objects.pop_front();

        std::list<std::shared_ptr<BoundingVolume> >::iterator partner, iter;

        // Greedily take the node with the smallest bounding volume and combine it with
        // whichever node minimizes their combined bounding volume.
        auto o1_bound = std::static_pointer_cast<AxisAlignedSlab>(o->bound);
        float best_volume = o1_bound->volume();

        //std::cout << "Initial volume = " << best_volume << std::endl; // TEMP

        partner = objects.end();
        for( iter = objects.begin(); iter != objects.end(); ++iter ) {
            auto o2_bound = std::static_pointer_cast<AxisAlignedSlab>((*iter)->bound);
            //std::cout << "  volume = " << o2_bound->volume(); // TEMP
            AxisAlignedSlab combined = merge( *o1_bound, *o2_bound );
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

        objects.sort( compare ); // Resort them

        std::cout << "objects.size = " << objects.size() << " best_volume = " << best_volume << std::endl; // TEMP
    }
}

#define COMPARE_FUNC(DIR) \
    []( const std::shared_ptr<BoundingVolume> & a, const std::shared_ptr<BoundingVolume> & b ) { \
                       return std::static_pointer_cast<AxisAlignedSlab>(a->bound)->DIR##mid() \
                             < std::static_pointer_cast<AxisAlignedSlab>(b->bound)->DIR##mid(); }

std::shared_ptr<BoundingVolume> BoundingVolumeHierarchy::buildNodeTopDown( std::list<std::shared_ptr<BoundingVolume> > & objects )
{
    //std::cout << "buildNodeTopDown size = " << objects.size() << std::endl;
    assert(objects.size() > 0);

    if(objects.size() == 1) {
        return objects.front();
    }

    AxisAlignedSlab bounds;
    for( auto & bv : objects ) {
        bounds = merge(bounds, *std::static_pointer_cast<AxisAlignedSlab>(bv->bound));
    }
    
    //std::cout << "Bound Dims: " << bounds.xdim() << " x " << bounds.ydim() << " x " << bounds.zdim() << std::endl;

    auto comparex = COMPARE_FUNC(x);
    auto comparey = COMPARE_FUNC(y);
    auto comparez = COMPARE_FUNC(z);

    float xd = bounds.xdim(), yd = bounds.ydim(), zd = bounds.zdim();

    if(xd > yd && xd > zd) { /* printf("splitting on X\n"); */ objects.sort( comparex ); }
    else if(yd > zd)       { /* printf("splitting on Y\n"); */ objects.sort( comparey ); }
    else                   { /* printf("splitting on Z\n"); */ objects.sort( comparez ); }

    std::list<std::shared_ptr<BoundingVolume> > objects2;

    auto it = objects.begin();
    std::advance(it, objects.size() / 2);
    objects2.splice(objects2.end(), objects, it, objects.end());

    //std::cout << "split " << objects.size() << ", " << objects2.size() << std::endl; // TEMP

    auto left = buildNodeTopDown(objects);
    auto right = buildNodeTopDown(objects2);

    auto pair = std::make_shared<FlatContainer>();
    pair->add( left );
    pair->add( right );
    return std::make_shared<BoundingVolume>(pair);
}

void BoundingVolumeHierarchy::buildTopDown( std::shared_ptr<Container> container )
{
    if( container->size() < 1 )
        return;

    std::list<std::shared_ptr<BoundingVolume> > objects;

    // Create a BoundingVolume node for each object
    const int num_objects = container->size();
    for( int i = 0; i < num_objects; i++ ) {
        objects.push_back( std::make_shared<BoundingVolume>( container->at(i) ) );
    }

    root = buildNodeTopDown( objects );
}

void BoundingVolumeHierarchy::print( FILE * file ) const
{
    fprintf(file, "BVH:\n");
    if(root) {
        root->print(file);
    }
}

void BoundingVolumeHierarchy::visit( TraceableVisitor & visitor )
{
    visitor.handle(*this);
}

