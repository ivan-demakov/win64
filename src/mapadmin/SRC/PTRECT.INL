#include <math.h>
//=====================================================================
//== Point ============================================================
//=====================================================================
inline
Point::Point( long cx, long cy )
{
  x = cx; y = cy;
}
//=====================================================================
inline
Point::Point( Point const& p )
{
  x = p.x; y = p.y;
}
//=====================================================================
inline void
Point::SetPoint( long cx, long cy )
{
  x = cx; y = cy;
}
//=====================================================================
inline int
Point::operator==( Point const& p ) const
{
  return x == p.x && y == p.y;
}
//=====================================================================
inline Point
Point::operator+( Point const& p ) const
{
  return Point( x + p.x, y + p.y );
}
//=====================================================================
inline Point&
Point::operator+=( Point const& p )
{
  SetPoint( x + p.x, y + p.y );
  return *this;
}
//=====================================================================
inline Point
Point::operator-( Point const& p ) const
{
  return Point( x - p.x, y - p.y );
}
//=====================================================================
inline Point
Point::operator-()
{
  return Point( -x, -y );
}
//=====================================================================
inline Point&
Point::operator-=( Point const& p )
{
  SetPoint( x - p.x, y - p.y );
  return *this;
}
//=====================================================================
inline Point 
Point::Offset( int cx, int cy )
{
  return Point( x + cx, y + cy );
}
//=====================================================================
//== Rect =============================================================
//=====================================================================
inline
Rect::Rect( long x0, long y0, long x1, long y1 )
{
  SetRect( x0, y0, x1, y1 );
}
//=====================================================================
inline
Rect::Rect( Point const& lt, Point const& rb )
{
  SetRect( lt.x, lt.y, rb.x, rb.y );
}
//=====================================================================
inline
Rect::Rect( Rect const& r )
{
  SetRect( r.left, r.top, r.right, r.bottom );
}
//=====================================================================
inline void
Rect::SetRect( Rect const& r )
{
  SetRect( r.left, r.top, r.right, r.bottom );
//  ASSERT( IsRectNormalized());
}
//=====================================================================
inline Point
Rect::LeftTop() const
{
  return Point( left, top );
}
//=====================================================================
inline Point
Rect::LeftBottom() const
{
  return Point( left, bottom );
}
//=====================================================================
inline Point
Rect::RightTop() const
{
  return Point( right, top );
}
//=====================================================================
inline Point
Rect::RightBottom() const
{
  return Point( right, bottom );
}
//=====================================================================
inline void
Rect::SetRect( long x0, long y0, long x1, long y1 )
{
  left = x0; top = y0; right = x1; bottom = y1;
//  ASSERT( IsRectNormalized());
}
//=====================================================================
inline int
Rect::PointInRect( Point const& p ) const
{
  return PointInRect( p.x, p.y );
}
//=====================================================================
inline int
Rect::PointInRect( int x, int y ) const
{
  return x >= left && x < right && y >= top && y < bottom;
}
//=====================================================================
inline int
Rect::IntersectRect( Rect const& r ) const
{
  return left < r.right  && r.left < right &&
         top  < r.bottom && r.top  < bottom;
}
//=====================================================================
inline long
Rect::Width() const
{
  return right - left;
}
//=====================================================================
inline long
Rect::Height() const
{
  return bottom - top;
}
//=====================================================================
#ifndef _CLIENT_APP
inline long
max( long a, long b )
{
  return a > b ? a : b;
}

inline long
min( long a, long b )
{
  return a < b ? a : b;
}
#endif
//=====================================================================
inline Rect
Rect::Normalize() const
{
  return Rect( min( left, right ),
               min( top, bottom ),
               max( left, right ),
               max( top, bottom ));
}
//=====================================================================
inline Rect
Rect::operator|( Rect const& r ) const
{
//  ASSERT( IsRectNormalized());
//  ASSERT( r.IsRectNormalized());
  return IsRectNull()   ? r     : 
         r.IsRectNull() ? *this :
                          Rect( min( left,   r.left ),
                                min( top,    r.top ),
                                max( right,  r.right ),
                                max( bottom, r.bottom ));
}
//=====================================================================
inline int 
Rect::operator==( Rect const& r )
{
//  ASSERT( IsRectNormalized());
//  ASSERT( r.IsRectNormalized());
  return top  == r.top  && 
         left == r.left && 
         right == r.right &&
         bottom == r.bottom; 
}
//=====================================================================
inline int 
Rect::operator!=( Rect const& r )
{
  return !operator==( r ); 
}
//=====================================================================
inline Rect&
Rect::operator|=( Rect const& r )
{
  SetRect( *this | r );
  return *this;
}
//=====================================================================
inline Rect
Rect::operator|( Point const& p ) const
{
  return IsRectNull() ?
    Rect( p.x, p.y, p.x, p.y ) :
    Rect( !p.x ? left   : min( left,   p.x ),
          !p.y ? top    : min( top,    p.y ),
          !p.x ? right  : max( right,  p.x ),
          !p.y ? bottom : max( bottom, p.y ));
}
//=====================================================================
inline Rect&
Rect::operator|=( Point const& p )
{
  SetRect( *this | p );
  return *this;
}
//=====================================================================
inline int
Rect::Inside( Rect const& r ) const
{
  return
    left   >= r.left   &&
    right  <= r. right &&
    top    >= r.top    &&
    bottom <= r.bottom;
}
//=====================================================================
inline int
Rect::IsRectEmpty() const
{
  return left == right || top == bottom;
}
//=====================================================================
inline int
Rect::IsRectNull() const
{
  return !left && !right && !top && !bottom;
}
//=====================================================================
inline void
Rect::Inflate( int d )
{
  if( d )
  {
    left   -= d;
    top    -= d;
    right  += d;
    bottom += d;
  }
}
//=====================================================================
inline void 
Rect::Offset( Point const& p )
{
  left   += p.x;
  top    += p.y;
  right  += p.x;
  bottom += p.y;
}
//=====================================================================
inline int 
Rect::IsRectNormalized() const
{
  return left <= right && top <= bottom;
}
//=====================================================================
