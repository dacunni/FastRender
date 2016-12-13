
#include "TraceLog.h"


/////////////////////////////////////////////////////////////////////////
// Comma-Separated Values

void TraceLog::Entry::writeCSV( std::ostream & os )
{
    // Aliases for compact code below
    const auto & o = ray.origin;
    const auto & d = ray.direction;
    const auto & p = intersection.position;
    const auto & n = intersection.normal;

    // Ray
    os << o.x << "," << o.y << "," << o.z << "," << o.w << ","
       << d.x << "," << d.y << "," << d.z << "," << d.w << ","
       // Hit test
       << hit << ",";

    // Intersection
    if( hit ) {
        os << p.x << "," << p.y << "," << p.z << "," << p.w << ","
           << n.x << "," << n.y << "," << n.z << "," << n.w;
    }
    else {
        // Dummy values for no hit
        Vector4 p( 0, 0, 0, 0 );
        Vector4 n( 0, 0, 0, 0 );
        os << p.x << "," << p.y << "," << p.z << "," << p.w << ","
           << n.x << "," << n.y << "," << n.z << "," << n.w;
    }

    os << std::endl;
}

void TraceLog::writeCSV( std::ostream & os )
{
    unsigned int index = 0;
    for( auto entry : entries ) {
        os << index++ << ",";
        entry.writeCSV(os);
    }
}

void TraceLog::writeCSV( const std::string & filename )
{
    std::ofstream os(filename, std::ofstream::out | std::ofstream::trunc);
    writeCSV(os); 
}

/////////////////////////////////////////////////////////////////////////
// Raw dump of structures

void TraceLog::Entry::writeRaw( std::ostream & os )
{
    os.write( (char*) &ray, sizeof(ray) );
    os.write( (char*) &hit, sizeof(hit) );
    os.write( (char*) &intersection, sizeof(intersection) );
}

void TraceLog::writeRaw( std::ostream & os )
{
    unsigned int index = 0;
    for( auto entry : entries ) {
        os.write((char*) &index, sizeof(index));
        entry.writeRaw(os);
    }
}

void TraceLog::writeRaw( const std::string & filename )
{
    std::ofstream os(filename, std::ofstream::out | std::ofstream::trunc);
    writeRaw(os); 
}


