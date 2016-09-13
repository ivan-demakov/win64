#ifndef _PTRECT_H_
#define _PTRECT_H_
//=====================================================================
#include "tptrect.h"
//=====================================================================
class Point : public TagPoint
{
public:
  Point( long cx = 0, long cy = 0 );
  Point( Point const& p );

  int operator==( Point const& p ) const;
  void SetPoint( long cx = 0, long cy = 0 );
  Point operator+( Point const& p ) const;
  Point& operator+=( Point const& p );
  Point operator-( Point const& p ) const;
  Point operator-();
  Point& operator-=( Point const& p );
  Point Offset( int cx, int cy );
};
//=====================================================================
class Rect : public TagRect
{
public:
  Rect( long l = 0, long t = 0, long r = 0, long b = 0 );
  Rect( Point const& lt, Point const& rb );
  Rect( Rect const& r );
  Rect( Rect const* r );

  Rect Normalize() const;

  Rect operator|( Rect const& r ) const;
  Rect& operator|=( Rect const& r );

  Rect operator|( Point const& p ) const;
  Rect& operator|=( Point const& p );

  Point LeftTop() const;
  Point LeftBottom() const;
  Point RightBottom() const;
  Point RightTop() const;
  void SetRect( long l = 0, long t = 0, long r = 0, long b = 0 );
  void SetRect( Rect const& r );
  void AddRect( Rect const& r, int angle = 0 );
  void Offset( Point const& p );

  long Width() const;
  long Height() const;
  void Inflate( int d );
  int PointInRect( Point const& p ) const;
  int PointInRect( int x, int y ) const;
  int IntersectRect( Rect const& r ) const;
  int Inside( Rect const& r ) const;
  int IsRectEmpty() const;
  int IsRectNull() const;
  int IsRectNormalized() const;
  int operator==( Rect const& r );
  int operator!=( Rect const& r );
};
//=====================================================================
#include "ptrect.inl"
//=====================================================================
#endif