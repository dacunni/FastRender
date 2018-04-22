//
//  Plot2D.hpp
//  FastRender
//
//  Created by David Cunningham on 11/14/15.
//
//
#ifndef __FastRender__Plot2D__hpp
#define __FastRender__Plot2D__hpp

template<typename VecType>
void Plot2D::addPoint( const VecType & v )
{
    addPoint( v.x, v.y );
}

template<typename VecType>
void Plot2D::drawLine( const VecType & u, const VecType & v )
{
    drawLine( u.x, u.y, v.x, v.y );
}

template<typename VecType>
void drawCircle( const VecType & v, float radius )
{
    drawCircle( v.x, v.y, radius );
}

#endif /* defined(__FastRender__Plot2D__hpp) */
