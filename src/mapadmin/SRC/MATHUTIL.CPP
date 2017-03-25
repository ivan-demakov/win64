#include "stdafx.h"

#include <complex>
#include <math.h>
#include "rgnspace.h"
#include "mathutil.h"
#include "cbox.h"
#include "tpdef.h"
#include "mview.h"
#include "ksiutil.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "mercator.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//=====================================================================
using namespace Trig;

CTrig::CTrigTabGenerator TG;
double CTrig::TrigTab[901];
//=====================================================================
using namespace std;
typedef complex<double> dcomplex;
//=====================================================================
CTrig::CTrigTabGenerator::CTrigTabGenerator()
{
  for( int i = 0 ; i <= i900 ; ++i )
    CTrig::TrigTab[i] = ::sin( Deg2Rad( i ));
}
//=====================================================================
double
Deg2Rad( int angle )
{
  return angle * Pi / d1800;
}
//=====================================================================
double
Deg2Rad( double angle )
{
  return angle * Deg;
}
//=====================================================================
int
Rad2Deg( double angle )
{
  return angle * d1800 / Pi;
}
//=====================================================================
double
Trig::sin( int angle )
{
  int negative = angle < 0;
  angle = abs( angle ) % i3600;
  if( angle > i1800 )
  {
    angle -= i1800;
    negative ^= 1;
  }
  double s = CTrig::TrigTab[ angle <= i900 ? angle : i1800 - angle ];
  return negative ? -s : s;
}
//=====================================================================
double
Trig::cos( int angle )
{
  return Trig::sin( angle + i900 );
}
//=====================================================================
int
dist( CPoint const& point1, CPoint const& point2 )
{
  return hipo( point1 - point2 );
}
//=====================================================================
double
hipo( int s1, int s2 )
{
  double x1 = s1, x2 = s2;
  return !s1 ? fabs( x2 ) : !s2 ? fabs( x1 ) : sqrt( x1 * x1 + x2 * x2 );
}
//=====================================================================
int
Trig::atan2( int vCos, int vSin )
{
  if( !vSin )
    return 0;

  if( !vCos )
    return vSin > 0 ? -i900 : i900;

  double s = abs( vSin ) / abs( dcomplex( vCos, vSin ));
  double* p;

  for( int a, l = 0, h = CELEM( CTrig::TrigTab ) ;; *(s>p[0]?&l:&h) = a )
  {
    a = l + h >> 1;
    p = CTrig::TrigTab + a;
    if( a == l || s > p[-1] && s < p[1] )
      return vSin < 0 ? ( vCos < 0 ? a - i1800 : -a ):
                        ( vCos < 0 ? i1800 - a :  a );
  }
}
//=====================================================================
int
Trig::atan2( CSize s )
{
  return atan2( s.cx, -s.cy );
}
//=====================================================================
CPoint
TurnPoint( double x, double y, int angle )
{
  double vcos = cos( angle ), vsin = sin( angle );
  return CPoint( x * vcos + y * vsin + .5,
                -x * vsin + y * vcos + .5 );
}
//=====================================================================
CPoint
TurnPoint( CPoint const& p, double dx, double dy, double dl )
{
  return dl ? CPoint(( p.x * dx + p.y * dy ) / dl + .5,
                     ( p.y * dx - p.x * dy ) / dl + .5 ) : p;
}
//=====================================================================
CPoint
TurnPoint( CPoint const& point, CSize const& center, int angle )
{
  return TurnPoint( point - center, angle ) + center;
}
//=====================================================================
CPoint
TurnPoint( CPoint const& point, CSize const& center, double dx, double dy, double dl )
{
  return TurnPoint( point - center, dx, dy, dl ) + center;
}
//=====================================================================
CRect
TurnRect( CRect const& rect, int angle )
{
  return angle == 0 ?
    rect :
    CRect( TurnPoint( rect.left, rect.top, angle ), rect.Size());
}
//=====================================================================
CPoint*
Rect2Poly( CRect const& rect, double co, double si )
{
  static CPoint buf[5];
  CSize size( rect.Size());
  int xco = size.cx * co;
  int xsi = size.cx * si;
  int yco = size.cy * co;
  int ysi = size.cy * si;
  buf[4] = buf[0] = rect.TopLeft();
  buf[3].x = ( buf[2].x = ( buf[1].x = buf[0].x + xco ) + ysi ) - xco;
  buf[3].y = ( buf[2].y = ( buf[1].y = buf[0].y - xsi ) + yco ) + xsi;
  return buf;
}
//=====================================================================
CPoint*
Rect2Poly( CPoint const& lt, CSize sz, int an )
{
  return Rect2Poly( CRect( lt, sz ), cos( an ), sin( an ));
}
//=====================================================================
int
Intersect( CRect const* r1, CRect const* r2 )
{
  return r1->left < r2->right && r1->top < r2->bottom &&
         r2->left < r1->right && r2->top < r1->bottom;
}
//=====================================================================
int
IsLineCrossPolyline( CPoint const* l, CPoint const* p, int n, CPoint* pCrosPoint )
{
  return IsLineCrossPolyline( l[0], l[1], p, n, pCrosPoint );
}
//=====================================================================
int
IsLineCrossPolyline( CPoint p00, CPoint p01, CPoint const* p, int n, CPoint* pCrosPoint )
{
  if( p00 == p01 )
    return 0;

  int minx = min( p00.x, p01.x );
  int maxx = max( p00.x, p01.x );
  int miny = min( p00.y, p01.y );
  int maxy = max( p00.y, p01.y );

  double dx = p01.x - p00.x;
  double dy = p01.y - p00.y;
  double dl = sqrt( dx * dx + dy * dy );
  int il = dl;

  CPoint p10( *p++ ), p11;
  while( --n > 0 )
  {
    p11 = p10;
    p10 = *p++;

    if( p00 == p10 && p01 == p11 || p00 == p11 && p01 == p10 )
      return 0;

    if( p10 == p11 ||
        minx >= p10.x && minx >= p11.x ||
        maxx <= p10.x && maxx <= p11.x ||
        miny >= p10.y && miny >= p11.y ||
        maxy <= p10.y && maxy <= p11.y )
      continue;

    CPoint p0( TurnPoint( p10 - p00, dx, dy, dl ));
    if( p0.y == 0 && ( p0.x < 0 || p0.x >= il ))
      continue;

    CPoint p1( TurnPoint( p11 - p00, dx, dy, dl ));
    if( p1.y == 0 && ( p1.x < 0 || p1.x >= il ))
      continue;

    if( p0.y < 0 && p1.y < 0 || p0.y > 0 && p1.y > 0 )
      continue;

    int x = .5 + double( p0.x ) - double( p0.y ) * double( p1.x - p0.x ) / double( p1.y - p0.y );
    if( x > 0 && x < il )
    {
      if( pCrosPoint )
        *pCrosPoint = p00 + TurnPoint( CPoint( x, 0 ), dx, -dy, dl );
      return 1;
    }
  }
  return 0;
}
//=====================================================================
int
ImprovePoly( CPoint* p, int n )
{
  CPoint* p0 = p + n;
  CPoint* p1 = p0 - 2;
  CPoint* p2 = p0 - 1;
  for( int i = 0, c = n ; --c >= 0 && n > 2 ; i = ( i + 1 ) % n )
  {
    p0 = p1;
    p1 = p2;
    p2 = p + i;
    if( *p0 == *p1 ||
        IsPointOnLine( *p0, *p1, *p2 ) ||
        IsPointOnLine( *p1, *p0, *p2 ) ||
        IsPointOnLine( *p2, *p0, *p1 ))
    {
      if( p1 < p2 )
        memcpy( p1, p2, (char*)&p[n] - (char*)p2 );
      c = --n;
      p2 = p + i % n;
    }
  }
  return n;
}
//=====================================================================
int
IsLineCrossPolygon( CPoint p00, CPoint p01, CPoint const* p, int n )
{
  if( p00 == p01 )
    return 0;

  int minx = min( p00.x, p01.x );
  int maxx = max( p00.x, p01.x );
  int miny = min( p00.y, p01.y );
  int maxy = max( p00.y, p01.y );

  double dx = p01.x - p00.x;
  double dy = p01.y - p00.y;
  double dl = sqrt( dx * dx + dy * dy );
  int il = dl;

  CPoint p10( p[0] ), p11;
  while( --n >= 0 )
  {
    p11 = p10;
    p10 = p[n];

    if( p00 == p10 && p01 == p11 || p00 == p11 && p01 == p10 )
      return 0;

    if( p10 == p11 ||
        minx >= p10.x && minx >= p11.x ||
        maxx <= p10.x && maxx <= p11.x ||
        miny >= p10.y && miny >= p11.y ||
        maxy <= p10.y && maxy <= p11.y )
      continue;

    CPoint p0( TurnPoint( p10 - p00, dx, dy, dl ));
    if( p0.y == 0 && ( p0.x < 0 || p0.x >= il ))
      continue;

    CPoint p1( TurnPoint( p11 - p00, dx, dy, dl ));
    if( p1.y == 0 && ( p1.x < 0 || p1.x >= il ))
      continue;

    if( p0.y < 0 && p1.y < 0 || p0.y > 0 && p1.y > 0 )
      continue;

    int x = double( p0.x ) - double( p0.y ) * double( p1.x - p0.x ) / double( p1.y - p0.y );

    if( x > 0 && x <= il )
      return 1;
  }
  return 0;
}
//=====================================================================
int
IsPolylineMatchPolyline( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint )
{
  for( int j, i = n0 ; --i >= 0 ; )
  {
    CPoint p( p0[i] );
    for( j = n1 ; --j >= 0 && p1[j] != p ; );
    if( j >= 0 )
    {
      if( pCrosPoint )
        *pCrosPoint = p;
      return 1;
    }
  }
  return 0;
}
//=====================================================================
int
IsPolylineCrossPolyline( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint )
{
  while( --n0 > 0 && !IsLineCrossPolyline( p0++, p1, n1, pCrosPoint ));
  return n0 > 0;
}
//=====================================================================
int
IsPolylineCrossPolygon( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint )
{
  return IsPolylineCrossPolyline( p1, n1, p0, n0, pCrosPoint ) ||
         IsLineCrossPolyline( p1[0], p1[n1-1], p0, n0, pCrosPoint );
}
//=====================================================================
int
IsPolygonCrossPolyline( CPoint const* p1, int n1, CPoint const* p0, int n0, CPoint* pCrosPoint )
{
  return IsPolylineCrossPolygon( p0, n0, p1, n1, pCrosPoint );
}
//=====================================================================
int
IsPolygonCrossPolygon( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint  )
{
  return IsPolylineCrossPolyline( p0, n0, p1, n1, pCrosPoint ) ||
         IsLineCrossPolyline( p0[0], p0[n0-1], p1, n1, pCrosPoint ) ||
         IsLineCrossPolyline( p1[0], p1[n1-1], p0, n0, pCrosPoint ) ||
         CrossPoint( p0[0], p0[n0-1], p1[0], p1[n1-1], pCrosPoint );
}
//=====================================================================
inline double
angle( CPoint p0, CPoint p1, CPoint p2 )
{
  return arg( dcomplex( p2.x - p1.x, p2.y - p1.y ) /
              dcomplex( p1.x - p0.x, p1.y - p0.y ));
}
//=====================================================================
inline float
Det( CSize pd, CSize cd )
{
  return float( pd.cx ) * float( cd.cy ) - float( pd.cy ) * float( cd.cx );
}
//=====================================================================

int
Clockwise( CPoint const* p, int n )
{
  int x, k = -1;

  for( int i = n ; --i >= 0 ; )
    if( k < 0 || p[i].x < x )
      x = p[k=i].x;
  CPoint p0( p[(k-1+n)%n] );
  p0.x += 1;
  CPoint p1( p[k] );
  CPoint p2( p[(k+1)%n] );
  p2.x += 1;
  CSize s0( p1 - p0 );
  CSize s1( p2 - p1 );
  float d = Det( s0, s1 );
  return d == 0 ? 0 : d > 0 ? 1 : -1;
}

/*
int
Clockwise( CPoint const* p, int n )
{
  CPoint p0, p1( p[n-2] ), p2( p[n-1] );
  double s = 0;
  while( --n >= 0 )
  {
    p0 = p1;
    p1 = p2;
    p2 = *p++;
    s += angle( p0, p1, p2 );
  }
  s /= PiPi;
  int r = s + ( s < 0 ? -.1 : .1 );
  return r;
}
*/
//=====================================================================
int
Clockwise( CPoint const* p, int n, int l0, int l1 )
{
  CPoint t[65536];
  memcpy( t, p, ( l0 + 1 ) * sizeof( CPoint ));
  memcpy( t + l0 + 1, p + l1, ( n - l1 ) * sizeof( CPoint ));
  return Clockwise( t, n - ( l1 - l0 - 1 ));
}
//=====================================================================
int
IsPointInTriangle( CPoint c, CPoint const* tgl )
{
  if( c == tgl[0] || c == tgl[1] || c == tgl[2] )
    return 0;

  CPoint p0, p1( tgl[2] );
  int s = 0;
  for( int n = 3 ; --n >= 0 ; )
  {
    p0 = p1;
    p1 = *tgl++;
    if( c.x <  max( p0.x, p1.x ) &&
        c.x >= min( p0.x, p1.x ) &&
        c.y <  max( p0.y, p1.y ))
    {
      CSize d0( p1 - p0 ), d1( c - p0 );
      int d = MulDiv( d1.cx, d0.cy, d0.cx ) - d1.cy;
      if( d == 0 )
        return 0;
      s ^= d > 0;
    }
  }
  return s;
}
//=====================================================================
int
IsPointInPolygon( CPoint c, CPoint const* plg, int n, int bWithBound, int d )
{
  static int const rm[2][2] = {{0,1},{3,2}};
  CPoint p0( plg[n-1] );
  CSize pd( p0 - c );
  int pt = rm[pd.cy<0][pd.cx<0];
  int s = 0;
  while( --n >= 0 )
  {
    if( c == p0 )
      return 2;

    CPoint p1( p0 );
    p0 = *plg++;
    CSize cd( p0 - c );
    int ct = rm[cd.cy<0][cd.cx<0];
    float det;
    switch( ct - pt )
    {
      case -3:
        ++s;
        break;
      case 3:
        --s;
        break;
      case -2:
        det = Det( pd, cd );
        if( det == 0 )
          return 2;
        if( d > 0 && bWithBound )
        {
          det /= hipo( p1 - p0 );
          if( fabs( det ) <= d )
            return 2;
        }
        if( det > 0 )
          ++s;
        break;
      case 2:
        det = Det( pd, cd );
        if( det == 0 )
          return 2;
        if( d > 0 && bWithBound )
        {
          det /= hipo( p1 - p0 );
          if( fabs( det ) <= d )
            return 2;
        }
        if( det < 0 )
          --s;
        break;
    }
    pd = cd;
    pt = ct;
  }
  return s != 0;
}
//=====================================================================
int
IsAnyPointInTriangle( CPoint const* pPnt, int nPnt, CPoint const* tgl )
{
  while( --nPnt >= 0 && !IsPointInTriangle( *pPnt++, tgl ));
  return nPnt >= 0;
}
//=====================================================================
int
IsAnyPointInPolygon( CPoint const* pPnt, int nPnt, CPoint const* pll, int n, int bWithBound )
{
  while( --nPnt >= 0 && !IsPointInPolygon( *pPnt++, pll, n, bWithBound ));
  return nPnt >= 0;
}
//=====================================================================
int
IsAllPointsInPolygon( CPoint const* pPnt, int nPnt, CPoint const* pll, int n, int bWithBound )
{
  while( --nPnt >= 0 && IsPointInPolygon( *pPnt++, pll, n, bWithBound ));
  return nPnt < 0;
}
//=====================================================================
int
IsAllPointsInRect( CPoint const* pPnt, int nPnt, CRect r )
{
  while( --nPnt >= 0 && r.PtInRect( *pPnt++ ));
  return nPnt < 0;
}
//=====================================================================
int
IsAnyPointOnPolygon( CPoint const* pPnt, int nPnt, CPoint const* pll, int n, int d )
{
  while( --nPnt >= 0 && IsPointOnPolygon( *pPnt++, pll, n, d ) < 0 );
  return nPnt >= 0;
}
//=====================================================================
int
IsAnyPointOnLine( CPoint const* pPnt, int nPnt, CPoint p0, CPoint p1, int d )
{
  while( --nPnt >= 0 && IsPointOnLine( pPnt[nPnt], p0, p1, d ) < 0 );
  return nPnt;
}
//=====================================================================
int
IsPolygonOnPolygon( CPoint const* pPg0, int n0, CPoint const* pPg1, int n1, int d )
{
  for( int i = 0 ; i < n0 ; ++i )
  {
    CPoint p0( pPg0[i] );
    CPoint p1( pPg0[(i+1)%n0] );
    for( int i = 0 ; i < n1 ; ++i )
    {
      CPoint q0( pPg1[i] );
      CPoint q1( pPg1[(i+1)%n1] );
      int n = IsPointOnLine( p0, q0, q1, d );
      n += n < 2 && IsPointOnLine( p1, q0, q1, d );
      n += n < 2 && IsPointOnLine( q0, p0, p1, d );
      n += n < 2 && IsPointOnLine( q1, p0, p1, d );
      if( n == 2 )
        return 1;
    }
  }
  return 0;
}
//=====================================================================
int
IsAllPointsOnPolyline( CPoint const* pPnt, int nPnt, CPoint const* pll, int n, int d )
{
  while( --nPnt >= 0 && IsPointOnPolyline( *pPnt++, pll, n, d ) >= 0 );
  return nPnt < 0;
}
//=====================================================================
int
IsPointOnLine( CPoint pt, CPoint p0, CPoint p1, int d )
{
  CSize s( p1 - p0 );
  double ls = hipo( s );
  CPoint t( TurnPoint( pt - p0, s.cx, s.cy, ls ));
  return t.x >= 0 && t.x <= ls && abs( t.y ) <= d;
}
//=====================================================================
int
IsPointOnPolyline( CPoint pt, CPoint const* pll, int n, int d )
{
  for( int i = 0 ; --n > 0 ; ++pll, ++i )
    if( IsPointOnLine( pt, pll[0], pll[1], d ))
      return i;
  return -1;
}
//=====================================================================
int
IsPointInRect( CPoint pt, CRect const& rect, int angle )
{
	return rect.PtInRect( TurnPoint( pt, rect.TopLeft(), -angle ));
}
//=====================================================================
int
DistPointToLine( CPoint pt, CPoint p0, CPoint p1 )
{
  CSize s( p1 - p0 );
  double ls = hipo( s );
  CPoint t( TurnPoint( pt - p0, s.cx, s.cy, ls ));
  return t.x >= 0 && t.x <= ls ? t.y : MAX_INT;
}
//=====================================================================
int
DistPointToLinePoint( CPoint pt, CPoint p0, CPoint p1 )
{
  CSize s( p1 - p0 );
  double ls = hipo( s );
  CPoint t( TurnPoint( pt - p0, s.cx, s.cy, ls ));
  int d0 = t.x >= 0 && t.x <= ls ? t.y : MAX_INT;
	int d1 = min( dist( pt, p0 ), dist( pt, p1 ));
	return min( d0, d1 );
}
//=====================================================================
int
DistPointToPolyline( CPoint pt, CPoint const* pll, int n )
{
	int d = MAX_INT;
  for( int i = 0 ; --n > 0 ; ++pll, ++i )
	{
    int t = DistPointToLine( pt, pll[0], pll[1] );
		if( t == 0 )
      return t;

		if( abs( t ) < d )
			d = t;
	}
  return d;
}
//=====================================================================
int
DistPointToEllipse( CPoint pt, CRect const& rect )
{
  CSize s( pt - rect.CenterPoint());
  int a = abs( rect.Width());
  int b = abs( rect.Height());

  if( a == b )
    return hipo( s ) - a / 2;

  if( a < b )
    a ^= b ^= a ^= b,
    s.cx ^= s.cy ^= s.cx ^= s.cy;

  int df = sqrt(( a + b ) * ( a - b )) / 2;

  return ( hipo( s.cx - df, s.cy ) + hipo( s.cx + df, s.cy ) - a ) / 2;
}
//=====================================================================
int
IsPointInSector( CPoint pt, CRect const& rect, int start, int delta )
{
  if( delta < 0 )
  {
    start += delta;
    delta = -delta;
  }
  if( delta >= 3600 )
    return 1;

  delta %= 3600;
  start = (( start % 3600 ) + 3600 ) % 3600;
  delta += start;

  int a = atan2( pt - rect.CenterPoint());
  if( a >= start && a < delta )
     return 1;

	a += 3600;
  if( a >= start && a < delta )
    return 1;

  return 0;
}
//=====================================================================
int
DistPointToArc( CPoint pt, CRect const& rect, int start, int delta )
{
  return IsPointInSector( pt, rect, start, delta ) ? DistPointToEllipse( pt, rect ) : MAX_INT;
}
//=====================================================================
int
DistPointToSector( CPoint pt, CRect const& rect, int start, int delta )
{
  if( !IsPointInSector( pt, rect, start, delta ))
		return MAX_INT;

	if( delta < 0 )
  {
    start += delta;
    delta = -delta;
  }

  start = (( start % 3600 ) + 3600 ) % 3600;
  delta %= 3600;
  delta += start;
	int W = rect.Width() / 2;
	int H = rect.Height() / 2;
	CPoint p0( rect.CenterPoint());
	CPoint p1( W * cos( start ), H * sin( start ));
	CPoint p2( W * cos( delta ), H * sin( delta ));
	p1 += p0;
	p2 += p0;

	int d1 = DistPointToLine( pt, p0, p1 );
	int d2 = DistPointToLine( pt, p0, p2 );
	int d3 = DistPointToEllipse( pt, rect );

  return min( d3, min( d1, d2 ));
}
//=====================================================================
static CPoint
EllipsePoint( CPoint p0, int a, int b, int ang )
{
	CSize s( 0, 0 );
	switch( ang )
	{
		case    0: s.cx =  a; break;
		case  900: s.cy = -b; break;
		case 1800: s.cx = -a; break;
		case 2700: s.cy =  b; break;
		default:
		{
    	double si = sin( ang ), co = cos( ang ), tg = si / co;
			double at = a * tg;
	    double x = a * b / sqrt( b * b + at * at );
			if( co < 0 )
				x = -x;
	    double y = x * tg;
			s.cx = x;
			s.cy = -y;
		}
	}
	return p0 + s;
}
//=====================================================================
int
DistToChord( CPoint pt, CRect const& rect, int start, int delta )
{
	if( delta < 0 )
  {
    start += delta;
    delta = -delta;
  }

  start = (( start % 3600 ) + 3600 ) % 3600;
  delta %= 3600;
  delta += start;
	int W = rect.Width()  / 2;
	int H = rect.Height() / 2;
	CPoint p0( rect .CenterPoint());

	CPoint p1 = EllipsePoint( p0, W, H, start );
	CPoint p2 = EllipsePoint( p0, W, H, delta );

  return DistPointToLine( pt, p1, p2 );
}
//=====================================================================
int
IsPointOnPolygon( CPoint pt, CPoint const* pll, int n, int d )
{
  int r = IsPointOnPolyline( pt, pll, n, d );
  return r >= 0 ? r : IsPointOnLine( pt, pll[0], pll[n-1], d ) ? n - 1 : -1;
}
//=====================================================================
int
CrossPoint( CPoint const& p00, CPoint const& p01,
            CPoint const& p10, CPoint const& p11,
            CPoint* pCrosPoint, int bOnLinesOnly )
{
/*
  if( p00 == p01 || p10 == p11 ||
      bOnLinesOnly && ( min( p00.x, p01.x ) > max( p10.x, p11.x ) ||
                        max( p00.x, p01.x ) < min( p10.x, p11.x ) ||
                        min( p00.y, p01.y ) > max( p10.y, p11.y ) ||
                        max( p00.y, p01.y ) < min( p10.y, p11.y )))
    return 0;

  CSize s0( p01 - p00 );
  double ll = hipo( s0 );
  CPoint p0( TurnPoint( p10 - p00, s0.cx, s0.cy, ll ));
  CPoint p1( TurnPoint( p11 - p00, s0.cx, s0.cy, ll ));

  if( p0.y == p1.y )
    return 0;

  if( bOnLinesOnly && ( p0.y > 1 && p1.y > 1 || p0.y < -1 && p1.y < -1 ))
    return 0;

  p1 -= p0;
  double dx = double( p0.x ) - double( p0.y ) * double( p1.x ) / double( p1.y );
  if( bOnLinesOnly && ( dx < 0 || dx > ll ))
    return 0;

  if( pCrosPoint )
    *pCrosPoint = p00 + TurnPoint( CPoint( dx, 0 ), s0.cx, -s0.cy, ll );

  return 1;
*/

	double B0 = p00.x - p01.x;
	double A0 = p01.y - p00.y;
	double B1 = p10.x - p11.x;
	double A1 = p11.y - p10.y;
	double z = A0 * B1 - A1 * B0;

	if( z == 0 )
    return 0;

	double C0 = - p00.y * B0 - p00.x * A0;
	double C1 = - p10.y * B1 - p10.x * A1;

	int x = ( B0 * C1 - B1 * C0 ) / z;
	int y = ( C0 * A1 - C1 * A0 ) / z;

  if( pCrosPoint )
	{
		pCrosPoint->x = x;
		pCrosPoint->y = y;
	}

  return
		!bOnLinesOnly ? 1:
		x >= min( p00.x, p01.x ) && x >= min( p10.x, p11.x ) &&
		y >= min( p00.y, p01.y ) && y >= min( p10.y, p11.y ) &&
		x <  max( p00.x, p01.x ) && x <  max( p10.x, p11.x ) &&
		y <  max( p00.y, p01.y ) && y <  max( p10.y, p11.y );
}
//=====================================================================
int
CrossPoint( CPoint const* pp0, CPoint const* pp1,
            CPoint* pCrosPoint, int bOnLinesOnly )
{
  return CrossPoint( pp0[0], pp0[1], pp1[0], pp1[1], pCrosPoint, bOnLinesOnly );
}
//=====================================================================
int
TrueCrossPoint( CPoint const* pp0, CPoint const* pp1, CPoint* pCrosPoint )
{
  return pp0[0] != pp1[0] && pp0[0] != pp1[1] &&
         pp0[1] != pp1[0] && pp0[1] != pp1[1] &&
         CrossPoint( pp0, pp1, pCrosPoint );
}
//=====================================================================
int
IsSelfCrossPolyline( CPoint const* p, int n )
{
  int i = n;
  while( --i > 0 && !IsLineCrossPolyline( p[i-1], p[i], p, n ));
  return i > 0;
}
//=====================================================================
int
IsSelfCrossPolygon( CPoint const* p, int n )
{
  return IsSelfCrossPolyline( p, n ) ||
         IsLineCrossPolyline( p[0], p[n-1], p, n );
}
//=====================================================================
CPoint
StepByLine( CPoint p0, CPoint p1, int l )
{
  int l0 = MerkatorData.Dist( p0, p1 );
  return l0 < l ?
    p1 :
    p0 + CSize( MulDiv( p1.x - p0.x, l, l0 ), MulDiv( p1.y - p0.y, l, l0 ));
}
//=====================================================================
static CPoint
EdgePoint( CPoint p0, CPoint p1, int r )
{
	p1 -= p0;
	return p0 + TurnPoint( CPoint( 0, -r ), p1.x, -p1.y, hipo( p1 ));
}
//=====================================================================
static int
MakeArc( TGstream& dst, CPoint p0, CPoint p1, CPoint p2, int r, int b36 = 1 )
{
  r *= MerkatorData.ScaleFactor( p1 );

  dcomplex v0( p1.x - p0.x, p1.y - p0.y );
  dcomplex v1( p2.x - p1.x, p2.y - p1.y );
  double da = v0 == dcomplex( 0, 0 ) ? PiPi : arg( v1 / v0 );

  if( da == 0 || da < 0 && p0 != p2 )
  {
    int m = r / tan(( Pi + da ) / 2.0 );
    if( m == 0 )
      m = 1;
    p0 = StepByLine( p1, p0, m );
    p2 = StepByLine( p1, p2, m ) - p0;
    dcomplex v1( p2.x, p2.y );
    dcomplex v2( polar( double( r ), arg( v1 ) - ::acos( abs( v1 ) / 2.0 / r )));
    p0 += CSize( v2.real(), v2.imag());
    dst.write( LPCSTR( &p0 ), sizeof p0 );
    return 1;
  }

  double sa = arg( v0 ) - Pi/2;
  if( da < 0  )
    da = -da;

  double const dr = 1 - ::cos( Pi / 36 );
	double d = b36 ? r * dr : pView->m_MaxDelta;
  double h = r - max( 1, d );
  double a = 2 * ::acos( h / r );
  int na = h > r ? 1 : ( int( da / a ) + 1 & ~1 );
  int ea = da != PiPi;
  da /= na;
  na += ea;

  for( int i = na ; --i >= 0 ; sa += da )
  {
    CPoint p( p1.x + r * cos( sa ), p1.y + r * sin( sa ));
    dst.write( LPCSTR( &p ), sizeof p );
  }
  return na;
}
/*
static int
MakeArc( TGstream& dst, CPoint p0, CPoint p1, CPoint p2, int r, int b36 = 1 )
{
  double const dr = 1 - ::cos( Pi / 36 );
	double d = b36 ? r * dr : pView->m_MaxDelta;

  r *= MerkatorData.ScaleFactor( Point( p1.x, p1.y ));

  dcomplex v0( p1.x - p0.x, p1.y - p0.y );
  dcomplex v1( p2.x - p1.x, p2.y - p1.y );
  double sa = arg( v0 ) - Pi/2;
  double da = arg( v1 / v0 );

	if( da == 0 )
    return 0;

 	CPoint tp0[2];
  CPoint tp1[2];
  tp0[0] = EdgePoint( p0, p1, r );
 	tp0[1] = p1 + ( tp0[0] - p0 );
  tp1[0] = EdgePoint( p2, p1, -r );
  tp1[1] = p1 + ( tp1[0] - p2 );

	if( da < 0 && p0 != p2 || d >= r )
  {
		CPoint p;
		CrossPoint( tp0, tp1, &p, 0 );
    dst.write( LPCSTR( &p ), sizeof p );
    return 1;
  }

  int n = 2;
  dst.write( LPCSTR( tp0+1 ), sizeof(CPoint));

  if( da < 0  )
    da = -da;

  double a = 2 * ::acos(( r - d ) / r );

	int na = da / a + .5;
	if( na & 1 )
		++na;

  da /= na;

	while( --na > 0  )
	{
		sa += da;
    CPoint p( p1.x + r * cos( sa ), p1.y + r * sin( sa ));
    dst.write( LPCSTR( &p ), sizeof p );
		++n;
	}

  dst.write( LPCSTR( tp1+1 ), sizeof(CPoint));

	return n;
}
*/
//=====================================================================
int
MakeSpace( TGstream& dst, CPoint* pp, int n, int r, int bLine )
{
  if( n < 1 )
    return 0;

  if( r <= 0 )
  {
    dst.write( LPCSTR( pp ), n * sizeof( CPoint ));
    return n;
  }

  if( n == 1 )
    return MakeArc( dst, *pp, *pp, *pp, r );

  int cw = Clockwise( pp, n );

  if( cw < 0 )
    ReversePoly( pp, n );

  int i, j;

  if( bLine )
  {
    MakeArc( dst, pp[1], pp[0], pp[1], r );
    for( i = 2 ; i < n ; ++i )
      MakeArc( dst, pp[i-2], pp[i-1], pp[i], r );
    MakeArc( dst, pp[n-2], pp[n-1], pp[n-2], r );
    for( i = n ; --i >= 2 ; )
      MakeArc( dst, pp[i], pp[i-1], pp[i-2], r );
  }
  else
  {
    MakeArc( dst, pp[n-1], pp[0], pp[1], r );
    for( i = 2 ; i < n ; ++i )
      MakeArc( dst, pp[i-2], pp[i-1], pp[i], r );
    MakeArc( dst, pp[n-2], pp[n-1], pp[0], r );
  }

  if( cw < 0 )
    ReversePoly( pp, n );

  pp = (CPoint*)dst.str();
  int np = dst.pcount() / sizeof( CPoint ), nl = 0, mx = pp[0].x;

  for( i = np ; --i >= 0 ; )
    if( pp[i].x < mx )
      mx = pp[nl=i].x;

  memcpy( pp + np, pp, nl * sizeof( CPoint ));
  memcpy( pp, pp + nl, np * sizeof( CPoint ));

  CPoint p0;
  pp[np] = pp[0];
  for( i = 0 ; i < np - 2 ; ++i )
    for( j = i + 2 ; j < np - ( i == 0 ) ; ++j )
      if( CrossPoint( pp + i, pp + j, &p0 ))
      {
        pp[i+1] = p0;
        memcpy( pp + i + 2, pp + j + 1, ( np - j ) * sizeof( CPoint ));
        np -= j - i - 1;
        j = i + 2;
      }
  return np;
}
//=====================================================================
static int
RemoveSelfCross( CPoint* pp, int np )
{
  CPoint p0;
  for( int i = 0 ; i < np - 2 ; ++i )
    for( int j = i + 2 ; j < np - 1 ; ++j )
      if( CrossPoint( pp + i, pp + j, &p0 ))
      {
        pp[i+1] = p0;
        memcpy( pp + i + 2, pp + j + 1, ( np - j ) * sizeof( CPoint ));
        np -= j - i - 1;
        j = i + 2;
      }
  return np;
}
//=====================================================================
int
SmartCloneLine( TGstream& dst, CPoint* pp, int n, int r )
{
  if( n < 2 || r == 0 )
    return 0;

  int i;

	CPoint tp0[2];
	CPoint tp1[2];
	if( r > 0 )
	{
	  tp0[0] = EdgePoint( pp[0], pp[1], r );
		tp0[1] = tp0[0] - pp[0] + tp0[0];
	  tp1[0] = EdgePoint( pp[n-1], pp[n-2], -r );
		tp1[1] = tp1[0] - pp[n-1] + tp1[0];
    dst.write((char*)&tp0[0], sizeof tp0[0] );
    for( i = 2 ; i < n ; ++i )
      MakeArc( dst, pp[i-2], pp[i-1], pp[i], r, 0 );
    dst.write((char*)&tp1[0], sizeof tp1[0] );
	}
	else
	{
	  tp0[0] = EdgePoint( pp[n-1], pp[n-2], -r );
    tp0[1] = tp0[0] - pp[n-1] + tp0[0];
	  tp1[0] = EdgePoint( pp[0], pp[1], r );
		tp1[1] = tp1[0] - pp[0] + tp1[0];
    dst.write((char*)&tp0[0], sizeof tp0[0] );
    for( i = n ; --i > 1 ; )
      MakeArc( dst, pp[i], pp[i-1], pp[i-2], -r, 0 );
    dst.write((char*)&tp1[0], sizeof tp1[0] );
	}

  pp = (CPoint*)dst.str();
  int np = dst.pcount() / sizeof( CPoint );

	if( r < 0 )
		ReversePoly( pp, np );

	return RemoveSelfCross( pp, np );

/*
  CPoint p0;

	for( i = 0 ; i < np - 1 ; ++i )
    if( CrossPoint( pp + i, tp0, &p0 ))
    {
      pp[0] = p0;
      memcpy( pp + 1, pp + i + 1, ( np - i - 1 ) * sizeof( CPoint ));
      np -= i;
     }

  for( i = np ; --i > 0 ; )
    if( CrossPoint( pp + i - 1, tp1, &p0 ))
    {
      pp[i] = p0;
      np = i + 1;
     }
  return np;
*/
}
//=====================================================================
double
CalcLength( CPoint* pp, int pn )
{
  double s;
  for( s = 0 ; --pn > 0 ; s += MerkatorData.Dist( pp[0], pp[1] ), ++pp );
  return s;
}
//=====================================================================
void
Polar::Create( double ltg, double ltm, double lts, double lng, double lnm, double lns )
{
  double const d = 60.0;
  double ltsg = ltg >= 0 ? 1 : -1;
  double lnsg = lng >= 0 ? 1 : -1;
  lat = ( ltsg * ( fabs( ltg ) + ( ltm + lts / d ) / d )) * Deg;
  lon = ( lnsg * ( fabs( lng ) + ( lnm + lns / d ) / d )) * Deg;
}
//=====================================================================
Polar::Polar( CDrawObject* pObj ) : lat( 0 ), lon( 0 )
{
  if( pObj->Class() == REPER )
  {
    CCircle* ccl = (CCircle*)pObj->GetActualCont()->GetHead()->GetNext();
    CText* ltg = (CText*)ccl->GetNext()->GetNext();
    CText* ltm = (CText*)ltg->GetNext()->GetNext();
    CText* lts = (CText*)ltm->GetNext()->GetNext();
    CText* lng = (CText*)lts->GetNext()->GetNext()->GetNext();
    CText* lnm = (CText*)lng->GetNext()->GetNext();
    CText* lns = (CText*)lnm->GetNext()->GetNext();
    Create( atof( *ltg ), atof( *ltm ), atof( *lts ),
            atof( *lng ), atof( *lnm ), atof( *lns ));
  }
}
//=====================================================================
int
Polar::operator==( Polar p )
{
  double const sec = 2. * Pi / 360. / 60. / 60.;
  return fabs( p.lat - lat ) < sec &&
         fabs( p.lon - lon ) < sec;
}
//=====================================================================
/*
Point3d
Pol2Dec( Polar p0, double R )
{
  double r = R * ::cos( p0.lat );
  return Point3d( r * ::cos( p0.lon ), r * ::sin( p0.lon ), R * ::sin( p0.lat ));
}
*/
//=====================================================================
/*
Polar
Dec2Pol( Point3d p0 )
{
  double R = sqrt( p0.x * p0.x + p0.y * p0.y + p0.z * p0.z );
  Polar p;
  p.lon = ::atan2( p0.y, p0.x );
  p.lat = ::asin( p0.z / R );
  return p;
}
*/
//=====================================================================
double
Djordan( double* sp, int n, int m )
{
  int const D = 1024;
  ASSERT( m >= n );
  ASSERT( m <= 2*n );
  ASSERT( n <= D );
  int s[D];
  double* ss = sp;
  double Dt = 1.;
  for( int i = 0 ; i < n ; ++i, sp += m )
  {
    int l;
    double mv = 0;
    for( int k = n ; --k >= 0 ; )
      if( fabs( sp[k] ) > fabs( mv ))
        mv = sp[l=k];

    if( mv == 0 )
      return 0;

    Dt *= mv;

    s[i] = l;
    for( int k = m ; --k >= 0 ; sp[k] /= mv );

    double* pp = ss;
    for( int j = 0 ; j < n ; ++j, pp += m )
      if( j != i )
      {
        double dv = pp[l];
        for( int k = m ; --k >= 0 ; pp[k] -= sp[k] * dv );
      }
  }

  int ssz = ( m - n ) * sizeof( double );
  if( ssz )
    for( int i = n ; --i >= 0 ; memcpy( ss + i * m, ss + s[i] * m + n, ssz ));

  return Dt;
}
//=====================================================================
/*
double
dist( Point3d& p0, Point3d& p1 )
{
  double dx = p0.x - p1.x;
  double dy = p0.y - p1.y;
  double dz = p0.z - p1.z;
  double ds = sqrt( dx * dx + dy * dy + dz * dz );

//  double rr = p0.x * p0.x + p0.y * p0.y + p0.z * p0.z;
//  double ds = sqrt( rr ) * ::acos(( p0.x * p1.x + p0.y * p1.y + p0.z * p1.z ) / rr );

  return ds;
}
*/
//=====================================================================
/*
CPoint
Triang( Polar p0, CPoint r0, Polar p1, CPoint r1, Polar p2 )
{
  double R = 1000;

  CSize d( r1 - r0 );
  double rd = hipo( d );
  R *= rd / dist( Pol2Dec( p1, R ), Pol2Dec( p0, R ));

  Point3d dp[] = { Pol2Dec( p0, R ), Pol2Dec( p1, R ), Pol2Dec( p2, R ) };

  double d0 = dist( dp[0], dp[2] );
  double d1 = dist( dp[1], dp[2] );
  double pd = d0 + d1;
  double S = rd >= pd ? 0 : Djordan( &dp[0].x, CELEM( dp ));
  if( S )
  {
    double x = .5 * (( d0 + d1 ) * ( d0 - d1 ) / rd + rd );
    double y = sqrt(( d0 + x ) * ( d0 - x )) * ( S < 0 ? 1. : -1. );
    r0 += TurnPoint( CPoint( x, y ), d.cx, -d.cy, rd );
  }
  else
  if( pd )
  {
    d0 /= pd;
    r0.Offset( d.cx * d0, d.cy * d0 );
  }

  return r0;
}
*/
//=====================================================================
/*
Polar
Triang( Polar p0, CPoint r0, Polar p1, CPoint r1, CPoint r2 )
{
  double R = 1000;

  Polar p2( 0, 0 );
  CSize s0( r2 - r0 );
  CSize s1( r2 - r1 );
  CSize s2( r1 - r0 );
  double l0 = hipo( s0 );
  double l1 = hipo( s1 );
  double l2 = hipo( s2 );
  double ll = l0 + l1;

  R *= l2 / dist( Pol2Dec( p0, R ), Pol2Dec( p1, R ));

  double R2 = R * R;

  if( ll < l2 )
  {
    p2.lat = ( p0.lat * l1 + p1.lat * l0 ) / ll;
    p2.lon = ( p0.lon * l1 + p1.lon * l0 ) / ll;
  }
  else
  {
    Point3d dp0 = Pol2Dec( p0, R );
    Point3d dp1 = Pol2Dec( p1, R );
    Point3d dp2( dp0.y * dp1.z - dp0.z * dp1.y,
                 dp0.z * dp1.x - dp0.x * dp1.z,
                 dp0.x * dp1.y - dp0.y * dp1.x );
    double S = double( s2.cx ) * double( s0.cy ) - double( s2.cy ) * double( s0.cx );
    double b0 = R2 - .5 * l0 * l0;
    double b1 = R2 - .5 * l1 * l1;
    double b2 = - R * S;

    double m[][4] =
    {
      dp0.x, dp0.y, dp0.z, b0,
      dp1.x, dp1.y, dp1.z, b1,
      dp2.x, dp2.y, dp2.z, b2
    };

    double D = Djordan( &m[0][0], 3, 1 );
    if( D )
    {
      Point3d dx( m[0][0], m[0][1], m[0][2] );
      p2 = Dec2Pol( dx );
    }
  }

  return p2;
}
*/
//=====================================================================
static int
FindNext( CPoint* p, int n, int h )
{
  int k;
  for( k = 1 ; k < n ; ++k )
  {
    CSize s( p[k+1] - p[0] );
    if( !s.cx && !s.cy )
      continue;

    double l = hipo( s );
    for( int i = 1 ; i <= k ; ++i )
    {
      CPoint tp( TurnPoint( p[i] - p[0], s.cx, s.cy, l ));
      if( abs( tp.y ) > h )
        return k;
    }
  }
  return k - 1;
}

int
DecimatePoly( CPoint* p, int n, int h )
{
  int nn = n;
  while( n > 2 )
  {
    int k = FindNext( p, n, h );
    memcpy( p + 1, p + k, ( n -= k ) * sizeof( CPoint ));
    nn -= k - 1;
    ++p;
  }
  return nn;
}
//=====================================================================
void
ReversePoly( CPoint* p, int n )
{
  CPoint t;
  for( CPoint* r = p + n ; p + 1 < r ; t = *p, *p++ = *--r, *r = t );
}
//=====================================================================
/*
int
FindPointToPoint( CPoint* p0, int n0, CPoint* p1, int n1, int* pi )
{
int t0 = clock();
  float sx = 0, sy = 0;
  pps* ph = 0;
  pps_mem puf( 1024 );
  int nv = 0;
  float dMax = -1;

  for( CPoint* tp = p1 + n1 ; --tp >= p1 ; sx += tp->x, sy += tp->y );
  CPoint cp( sx / n1, sy / n1 );

  for( int i0 = n0 ; --i0 >= 0 ; )
  {
    CPoint t0( p0[i0] );
    float dx = t0.x - cp.x, dy = t0.y - cp.y;
    float d = dx * dx + dy * dy;

    pps* t = puf.RemoveHead();
    ph = t->Set( i0, 0, d, ph );
    ++nv;
    if( puf.IsEmpty() || i0 == 0 )
    {
      ph = pps::Sort( ph, nv );
      if( i0 )
      {
        pps* tp = ph;
        for( int t = 512 ; tp && --t > 0 ; tp = tp->next );
        if( tp )
        {
          pps* h = tp;
          tp = tp->next;
          h->next = 0;
          dMax = h->dst;
          nv -= puf.AddList( tp );
        }
      }
    }
  }

  int const ts = 4;
  int const bs = 16;
  int const bs2 = 256;
  pps* lh = 0;
  pps_mem buf( bs2 );

  nv = 0;
  int bt[2];

  while( ph )
  {
    int i0 = ph->ni0;
    CPoint t0( p0[i0] );
    ph = ph->next;
    for( int i1 = n1 ; --i1 >= 0 ; )
    {
      CPoint t1( p1[i1] );
      float dx = t0.x - t1.x, dy = t0.y - t1.y;
      float d = dx * dx + dy * dy;
      pps* t = buf.RemoveHead();
      lh = t->Set( i0, i1, d, lh );
      ++nv;
      if( buf.IsEmpty() || !ph || i1 == 0 )
      {
        lh = pps::Sort( lh, nv );
        if( i0 || i1 )
        {
          pps* tp = lh;
          for( int t = bs ; tp && --t > 0 ; tp = tp->next );
          if( tp )
          {
            pps* h = tp;
            tp = tp->next;
            h->next = 0;
            dMax = h->dst;
            nv -= buf.AddList( tp );
          }
        }
      }
    }

    bt[0] = lh->ni0;
    bt[1] = lh->ni1;
    for( int t = ts ; --t >= 0 && lh ; )
    {
      CPoint* tp0 = &p0[pi[0]=lh->ni0];
      CPoint* tp1 = &p1[pi[1]=lh->ni1];
      if( !IsLineCrossPolygon( *tp0, *tp1, p1, n1 ) &&
          !IsLineCrossPolygon( *tp0, *tp1, p0, n0 ))
        return 1;

      pps* h = lh;
      lh = lh->next;
      buf.Add( h );
      --nv;
    }
  }

  pi[0] = bt[0];
  pi[1] = bt[1];
  return 0;
}
*/
/*
int
FindPointToPoint( CPoint* p0, int n0, CPoint* p1, int n1, int* pi )
{
int t0 = clock();
  float sx = 0, sy = 0;
  pps* ph = 0;
  pps_mem puf( 1024 );
  int nv = 0;
  float dMax = -1;

  for( CPoint* tp = p1 + n1 ; --tp >= p1 ; sx += tp->x, sy += tp->y );
  CPoint cp( sx / n1, sy / n1 );

  for( int i0 = n0 ; --i0 >= 0 ; )
  {
    CPoint t0( p0[i0] );
    float dx = t0.x - cp.x, dy = t0.y - cp.y;
    float d = dx * dx + dy * dy;

    pps* t = puf.RemoveHead();
    ph = t->Set( i0, 0, d, ph );
    ++nv;
    if( puf.IsEmpty() || i0 == 0 )
    {
      ph = pps::Sort( ph, nv );
      if( i0 )
      {
        pps* tp = ph;
        for( int t = 512 ; tp && --t > 0 ; tp = tp->next );
        if( tp )
        {
          pps* h = tp;
          tp = tp->next;
          h->next = 0;
          dMax = h->dst;
          nv -= puf.AddList( tp );
        }
      }
    }
  }

  int const bs = 16;
  int const bs2 = 256;
  pps* lh = 0;
  pps_mem buf( bs2 );

  nv = 0;

  while( ph )
  {
    int i0 = ph->ni0;
    CPoint t0( p0[i0] );
    ph = ph->next;
    for( int i1 = n1 ; --i1 >= 0 ; )
    {
      CPoint t1( p1[i1] );
      float dx = t0.x - t1.x, dy = t0.y - t1.y;
      float d = dx * dx + dy * dy;
      pps* t = buf.RemoveHead();
      lh = t->Set( i0, i1, d, lh );
      ++nv;
      if( buf.IsEmpty() || !ph || i1 == 0 )
      {
        lh = pps::Sort( lh, nv );
        if( i0 || i1 )
        {
          pps* tp = lh;
          for( int t = bs ; tp && --t > 0 ; tp = tp->next );
          if( tp )
          {
            pps* h = tp;
            tp = tp->next;
            h->next = 0;
            dMax = h->dst;
            nv -= buf.AddList( tp );
          }
        }
      }
    }
  }

  while( lh )
  {
    CPoint* tp0 = &p0[pi[0]=lh->ni0];
    CPoint* tp1 = &p1[pi[1]=lh->ni1];
    if( !IsLineCrossPolygon( *tp0, *tp1, p1, n1 ) &&
        !IsLineCrossPolygon( *tp0, *tp1, p0, n0 ))
    {
t0 = clock() - t0;
      return 1;
    }

    lh = lh->next;
  }
  return 0;
}
*/

int
FindPointToPoint( CPoint* p0, int n0, CPoint* p1, int n1, int pi[][2], int ni, int bs2, int bs )
{
  pps* lh = 0;
  pps_mem buf( bs2 );
  int nv = 0;
  for( int i0 = n0 ; --i0 >= 0 ; )
  {
    CPoint t0( p0[i0] );
    for( int i1 = n1 ; --i1 >= 0 ; )
    {
      CPoint t1( p1[i1] );
      float dx = t0.x - t1.x, dy = t0.y - t1.y;
      float d = dx * dx + dy * dy;
      pps* t = buf.RemoveHead();
      lh = t->Set( i0, i1, d, lh );
      ++nv;
      if( nv == bs2 || i0 == 0 && i1 == 0 )
      {
        lh = pps::Sort( lh, nv );
        int r = i0 * n1 + i1;
        if( r )
        {
          pps* tp = lh;
          for( int t = bs ; tp && --t > 0 ; tp = tp->next );
          if( tp )
          {
            pps* h = tp;
            tp = tp->next;
            h->next = 0;
            nv -= buf.AddList( tp );
          }
        }
      }
    }
  }

  if( ni > 1 )
  {
    int i;
    for( i = 0 ; i < ni && lh ; lh = lh->next, ++i )
    {
      pi[i][0] = lh->ni0;
      pi[i][1] = lh->ni1;
    }
    return i;
  }

  int ti[2] = { lh->ni0, lh->ni1 };
  while( lh )
  {
    CPoint* tp0 = &p0[pi[0][0]=lh->ni0];
    CPoint* tp1 = &p1[pi[0][1]=lh->ni1];
    if( !IsLineCrossPolygon( *tp0, *tp1, p1, n1 ) &&
        !IsLineCrossPolygon( *tp0, *tp1, p0, n0 ))
      return 1;

    lh = lh->next;
  }

  pi[0][0] = ti[0];
  pi[0][1] = ti[1];
  return 0;
}
//=====================================================================
/*
static int
TestPolygonsIntersection( CPoint* pp0, int n0,
                          CPoint* pp1, int n1,
                          int pn[2][2], CPoint pc[2] )
{ // return 1 ? 2 crosses : 0 ? 0 crosses : -1 ? > 2 crosses
  int cn = 0;
  CPoint t0[2] = { pp0[0] };
  for( int i = n0 ; --i >= 0 ; )
  {
    t0[1] = t0[0];
    t0[0] = pp0[i];
    CPoint t1[2] = { pp1[0] };
    for( int j = n1 ; --j >= 0 ; )
    {
      t1[1] = t1[0];
      t1[0] = pp1[j];
      CPoint cp;
      if( CrossPoint( t0, t1, &cp ))
      {
        for( int k = cn ; --k >= 0 && !Same( cp, pc[k] ); );
        if( k >= 0 )
          continue;

        int i0 = Same( cp, t0[0] ) ? i : Same( cp, t0[1] ) ? i + 1 : -1;
        int i1 = Same( cp, t1[0] ) ? j : Same( cp, t1[1] ) ? j + 1 : -1;
        if( i0 >= 0 && i1 >= 0 )
        {
          CPoint m0( pp0[(i0-1+n0)%n0] ), p0( pp0[(i0+1)%n0] );
          CPoint m1( pp1[(i1-1+n1)%n1] ), p1( pp1[(i1+1)%n1] );
          if(( m0 == p1 ||
               IsPointOnLine( m0, pp1[i1], p1 ) ||
               IsPointOnLine( p1, pp1[i1], m0 )) &&
             ( p0 == m1 ||
               IsPointOnLine( p0, pp1[i1], m1 ) ||
               IsPointOnLine( m1, pp1[i1], p0 )))
          continue;
        }

        if( cn == 2 )
          return -1;

        pc[cn] = cp;
        pn[cn][0] = i;
        pn[cn][1] = j;
        ++cn;
      }
    }
  }
  return cn;
}
*/
//=====================================================================
CPoint*
SubConPolygons( CPoint* p0, int n0, CPoint* p1, int n1, int i0, int i1 )
// p2 = p0 * p1
{
  int bDiff = p0[i0] != p1[i1];
  int k = 0, s = n0 + n1 + bDiff * 2;
  CPoint* p2 = (CPoint*)new CPoint[s+1];
  p2[k++] = CPoint( s, 0 );
  memcpy( p2 + k, p0, ( i0 + bDiff ) * sizeof CPoint );
  k += i0 + bDiff;
  memcpy( p2 + k, p1 + i1, ( n1 - i1 ) * sizeof CPoint );
  k += n1 - i1;
  memcpy( p2 + k, p1, ( i1 + bDiff ) * sizeof CPoint );
  k += i1 + bDiff;
  memcpy( p2 + k, p0 + i0, ( n0 - i0 ) * sizeof CPoint );
  return p2;
}
//=====================================================================
CPoint*
ImprovePolygon( CPoint* pp )
{
  int const d = 3;
  CPoint* sp = pp;
  int n = sp++->x;
  for( int i = 0 ; i < n ; )
  {
    CPoint* p0 = &sp[i==0?n-1:i-1];
    CPoint* p1 = &sp[i];
    CPoint* p2 = &sp[i==n-1?0:i+1];

    if( abs( p0->x - p1->x ) <= d && abs( p0->y - p1->y ) <= d ||
        IsPointOnLine( *p0, *p1, *p2, d ) ||
        IsPointOnLine( *p1, *p0, *p2, d ) ||
        IsPointOnLine( *p2, *p0, *p1, d ))
    {
      pp->x = n -= 1;
      memcpy( p1, p1 + 1, ( n - i ) * sizeof( CPoint ));
//      i = 0;
    }
    else
      ++i;
  }
  return pp;
}
//=====================================================================
CPoint*
SmartConnectPolylines( CPoint* pp0, int n0, CPoint* pp1, int n1 )
{
  int f = 0, d0 = n0 - 1, d1 = n1 - 1, i0, i1;
  for( i0 = 2 ; !f && --i0 >= 0 ; )
    for( i1 = 2 ; !f && --i1 >= 0 ; )
      f = pp0[d0*i0] == pp1[d1*i1];
  if( !f )
    return 0;
  int n = n0 + n1;
  CPoint* p = (CPoint*)new CPoint[n];
  CPoint* dp = p;
  dp++->x = n - 1;
  i0 = i0 * 2 - 1;
  i1 = 1 - i1 * 2;
  for( pp0 += i0 < 0 ? d0 : 0 ; --n0 >= 0 ; *dp++ = *pp0, pp0 += i0 );
  dp -= 1;
  for( pp1 += i1 < 0 ? d1 : 0 ; --n1 >= 0 ; *dp++ = *pp1, pp1 += i1 );
  return p;
}
//=====================================================================
static int
IsIn( CPoint p, CPoint const* plg, int n )
{
  for( int i = 0 ; i < n ; ++i )
    if( p == plg[i] )
      return i;
  return -1;
}
//=====================================================================
int
FindNearesRepers( CPoint dp, RepInf* pRI, int nRep )
{
  int n = 0;
  CObjTreeList* pRl = pView->GetReperList();
  for( CDrawObject* pRep = pRl->SetFirst() ; pRep = pRl->GetNext() ; )
  {
    CPoint* psp = pRep->GetAnyPoints();
    RepInf tt;
    tt.pRp = pRep;
    tt.sPt = *psp;
    tt.pPt = pRep;
    tt.dis = dist( dp, *psp );
    int i;
    for( i = n ; --i >= 0 && pRI[i].dis > tt.dis ; pRI[i+1] = pRI[i] );
    pRI[i+1] = tt;
    n += n < nRep;
  }
  return n;
}
//=====================================================================
int
FindNearesRepers( Polar dp, RepInf* pRI, int nRep )
{
  int n = 0;
  CObjTreeList* pRl = pView->GetReperList();
  double R = 1000;
  for( CDrawObject* pRep = pRl->SetFirst() ; pRep = pRl->GetNext() ; )
  {
    CCircle* crcl = (CCircle*)pRep->GetActualCont()->GetHead()->GetNext();
    CPoint sp( crcl->GetCoord());
    RepInf tt;
    tt.pRp = pRep;
    tt.sPt = sp;
    tt.pPt = pRep;

    Point3d p0 = ::Pol2Dec( dp, R );
    Point3d p1 = ::Pol2Dec( tt.pPt, R );
    double ang = ::acos(( p0.x * p1.x + p0.y * p1.y + p0.z * p1.z ) / R / R );
    tt.dis = R * ang;
    int i;
    for( i = n ; --i >= 0 && pRI[i].dis > tt.dis ; pRI[i+1] = pRI[i] );
    pRI[i+1] = tt;
    n += n < nRep;
  }
  return n;
}
//=====================================================================
CPoint
Pol2Dec( Polar mp )
{
  CPoint p( -1, -1 );

  MerkatorData.InitLocal( mp );

  RepInf rt[N_REP+1];
  if( FindNearesRepers( mp, rt ) == N_REP )
  {
    for( int i = N_REP ; --i >= 0 ; rt[i].dPt = MerkatorData.Pol2Dec( rt[i].pPt ));

    double mvxy[][5] =
    {
      rt[0].dPt.x, rt[0].dPt.y, 1, rt[0].sPt.x, rt[0].sPt.y,
      rt[1].dPt.x, rt[1].dPt.y, 1, rt[1].sPt.x, rt[1].sPt.y,
      rt[2].dPt.x, rt[2].dPt.y, 1, rt[2].sPt.x, rt[2].sPt.y
    };

    if( fabs( Djordan( mvxy[0], 3, 5 )) > 1e-6 )
    {
      CPoint sp( MerkatorData.Pol2Dec( mp ));
      p = CPoint( mvxy[0][0] * sp.x + mvxy[1][0] * sp.y + mvxy[2][0],
                  mvxy[0][1] * sp.x + mvxy[1][1] * sp.y + mvxy[2][1] );
    }
  }

  MerkatorData.DeinitLocal();

  return p;
}
//=====================================================================
Polar
Dec2Pol( CPoint sp )
{
  Polar pp( 0, 0 );
  RepInf rt[N_REP+1];
  if( FindNearesRepers( sp, rt ) != N_REP )
    return pp;

  MerkatorData.InitLocal( rt[0].pPt );

  for( int i = N_REP ; --i >= 0 ; rt[i].dPt = MerkatorData.Pol2Dec( rt[i].pPt ));

  double mvxy[][5] =
  {
    rt[0].sPt.x, rt[0].sPt.y, 1, rt[0].dPt.x, rt[0].dPt.y,
    rt[1].sPt.x, rt[1].sPt.y, 1, rt[1].dPt.x, rt[1].dPt.y,
    rt[2].sPt.x, rt[2].sPt.y, 1, rt[2].dPt.x, rt[2].dPt.y
  };

  if( fabs( Djordan( mvxy[0], 3, 5 )) > 1e-6 )
  {
    CPoint p( mvxy[0][0] * sp.x + mvxy[1][0] * sp.y + mvxy[2][0],
              mvxy[0][1] * sp.x + mvxy[1][1] * sp.y + mvxy[2][1] );
    pp = MerkatorData.Dec2Pol( p );
  }

  MerkatorData.DeinitLocal();

  return pp;
}
//=====================================================================
//=====================================================================
LItemHash::LItemHash( UINT size )
{
  for( m_nSize = CELEM( m_pInTab ) ; m_nSize < size ; m_nSize <<= 1 );
  m_nUsed = 0;
  m_pTab = m_nSize == CELEM( m_pInTab ) ? m_pInTab : new LItem[m_nSize];
  memset( m_pTab, 0, sizeof LItem * m_nSize );
}
//=====================================================================
LItemHash::~LItemHash()
{
  if( m_pTab != m_pInTab )
    delete m_pTab;
}
//=====================================================================
LItemHash1::LItemHash1( CPoint* p1, int n ) : LItemHash( n * 2 )
{
  for( CPoint* p0 = p1 + n - 1 ; --n >= 0 ; p0 = p1++ )
    Add( *p0, *p1 );
}
//=====================================================================
LItemHash::LItem*
LItemHash1::GetFirst()
{
  m_pCur = &m_pTab[m_nSize];
  return 0;
}
//=====================================================================
LItemHash::LItem*
LItemHash1::GetNext()
{
  while( --m_pCur >= m_pTab )
    if( m_pCur->Used())
      return m_pCur;
  return 0;
}
//=====================================================================
int
LItemHash::GetHead( CPoint& p0, CPoint& p1 )
{
  for( LItem* p = &m_pTab[m_nSize] ; --p >= m_pTab ; )
    if( p->Used())
    {
      p0 = p->m_Point0;
      p1 = p->m_Point1;
      return Del( p );
    }
  return 0;
}
//=====================================================================
int
LItemHash::GetNext( CPoint& p0, CPoint& p1 )
{
  int m = m_nSize - 1, n1 = 0, n2 = m_nSize << 1;
  int b = p0.x + p0.y;
  while( n2 > 1 )
  {
    LItem* p = &m_pTab[(b+n1)&m];
    if( p->Free())
      break;
    if( p->m_Point0 == p0 )
    {
      p1 = p->m_Point1;
      return Del( p );
    }
    if(( n1 += n2 ) >= m_nSize )
      n1 = ( n2 >>= 1 ) >> 1;
  }
  return 0;
}
//=====================================================================
int
LItemHash::RemDup()
{
  int r = 0;
  for( LItem* p = &m_pTab[m_nSize] ; --p >= m_pTab ; )
    if( p->Used())
    {
      CPoint p0( p->m_Point0 );
      CPoint p1( p->m_Point1 );
      r |= Rem( p1, p0 ) && Rem( p0, p1 );
    }
  return r;
}
//=====================================================================
int
LItemHash::CalcLines() const
{
  int n = 0;
  for( LItem* p = &m_pTab[m_nSize] ; --p >= m_pTab ; n += p->Used());
  return n;
}
//=====================================================================
void
LItemHash::CopyLines( CPoint* pt ) const
{
  for( LItem* p = &m_pTab[m_nSize] ; --p >= m_pTab ; )
    if( p->Used())
    {
      *pt++ = p->m_Point0;
      *pt++ = p->m_Point1;
    }
}
//=====================================================================
LItemHash::operator ksi_obj() const
{
  ksi_obj ksi_lns = ksi_nil;
  for( LItem* p = &m_pTab[m_nSize] ; --p >= m_pTab ; )
    if( p->Used())
    {
      ksi_lns = ksi_cons( KsiPoint( p->m_Point1 - p->m_Point0 ), ksi_lns );
      ksi_lns = ksi_cons( KsiPoint( p->m_Point0 ), ksi_lns );
    }
  return ksi_lns;
}
//=====================================================================
LItemHash::LItem*
LItemHash::FindPos( CPoint p0 )
{
  int m = m_nSize - 1, n1 = 0, n2 = m_nSize << 1;
  int b = p0.x + p0.y;
  while( n2 > 1 )
  {
    LItem* p = m_pTab + (( b + n1 ) & m );
    if( p->Free())
      return p;
    if(( n1 += n2 ) >= m_nSize )
      n1 = ( n2 >>= 1 ) >> 1;
  }
  return 0; //it is impossible!!!
}
//=====================================================================
LItemHash::LItem*
LItemHash::FindPos( CPoint p0, CPoint p1 )
{
  int m = m_nSize - 1, n1 = 0, n2 = m_nSize << 1;
  int b = p0.x + p0.y;
  while( n2 > 1 )
  {
    LItem* p = m_pTab + (( b + n1 ) & m );
    if( p->Free() || p->Cons( p0, p1 ))
      return p;
    if(( n1 += n2 ) >= m_nSize )
      n1 = ( n2 >>= 1 ) >> 1;
  }
  return 0; //it is impossible!!!
}
//=====================================================================
void
LItemHash::Add( CPoint const& p0, CPoint const& p1 )
{
  if( m_nUsed >= m_nSize >> 1 )
  {
    LItem* pTab = m_pTab;
    int n = m_nSize;
    m_nSize <<= 1;
    m_pTab = new LItem[m_nSize];
    memset( m_pTab, 0, m_nSize * sizeof LItem );
    for( LItem* p = &pTab[n] ; --p >= pTab ; )
      if( p->Used())
        *FindPos( p->m_Point0 ) = *p;
    if( pTab != m_pInTab )
      delete pTab;
  }

  FindPos( p0 )->SetPoints( p0, p1 );
  ++m_nUsed;
}
//=====================================================================
int
LItemHash::Find( CPoint p0 )
{
  LItem* p = FindPos( p0 );
  return p->Used() && p->m_Point0 == p0;
}
//=====================================================================
int
LItemHash::Rem( CPoint const& p0, CPoint const& p1 )
{
  LItem* p = FindPos( p0, p1 );
  return p->Used() && Del( p );
}
//=====================================================================
void
LItemHash2::Add( CPoint const& p0, CPoint const& p1 )
{
  LItemHash::Add( p0, p1 );
  LItemHash::Add( p1, p0 );
}
//=====================================================================
int
LItemHash2::GetHead( CPoint& p0, CPoint& p1 )
{
  return LItemHash::GetHead( p0, p1 ) ? Rem( p1, p0 ) : 0;
}
//=====================================================================
int
LItemHash2::GetNext( CPoint& p0, CPoint& p1 )
{
  return LItemHash::GetNext( p0, p1 ) ? Rem( p1, p0 ) : 0;
}
//=====================================================================
static CPoint
FindPointIn( CPoint p, CPoint* pp1, int n1 )
{
  int sx = 1, mx = sx;
  int sy = 1, my = sy;
  CPoint t( p );
  for( int tr = 16 ; --tr >= 0 ; )
  {
    for( int dx = 0 ; dx != mx ; dx += sx )
    {
      p.x += sx;
      if( IsPointInPolygon( p, pp1, n1, 1, 2 ) == 1 )
        return p;
    }
    for( int dy = 0 ; dy != my ; dy += sy )
    {
      p.y += sy;
      if( IsPointInPolygon( p, pp1, n1, 1, 2 ) == 1 )
        return p;
    }

    mx = -mx + ( sx = -sx );
    my = -my + ( sy = -sy );
  }
  return t;
}
//=====================================================================
void
PrepareForAdding( CPoint* pp0, int n0, CPoint* pp1, int n1 )
{
  for( int i = n0 ; --i >= 0 ; )
  {
    CPoint p( pp0[i] );
    if( IsPointInPolygon( p, pp1, n1, 0 ) != 1 && IsPointOnPolygon( p, pp1, n1, 1 ) >= 0 )
      pp0[i] = FindPointIn( p, pp1, n1 );
  }
}
//=====================================================================
void
SmartClipLine( CPoint p0, CPoint p1, CPoint const* plg, int n, LItemHash* plh,
               int bClipping, int bInside )
{
  if( p0 == p1 )
    return;

  int in0 = IsIn( p0, plg, n );
  int in1 = IsIn( p1, plg, n );
  if( in0 >= 0 && in1 >= 0 )
  {
    int dn = abs( in0 - in1 );
    if( dn == 1 || dn == n - 1 )
    {
      if( bClipping )
        plh->Add( p0, p1 );
      return;
    }
  }

  if( !IsLineCrossPolygon( p0, p1, plg, n ) &&
      ( IsPointInPolygon( p0, plg, n, 0 ) == bInside ||
        IsPointInPolygon( p1, plg, n, 0 ) == bInside ))
  {
    plh->Add( p0, p1 );
    return;
  }

  double dx = p1.x - p0.x;
  double dy = p1.y - p0.y;
  double dl = sqrt( dx * dx + dy * dy );
  dx /= dl;
  dy /= dl;

  double tx = plg[0].x - p0.x;
  double ty = plg[0].y - p0.y;
  FPoint tp1( tx * dx + ty * dy, ty * dx - tx * dy );

  CArray< double, double > tmp;
  tmp.SetSize( 2 );
  tmp[0] = 0;
  tmp[1] = dl;
  for( int i = n ; --i >= 0 ; )
  {
    FPoint tp0( tp1 );
    tx = plg[i].x - p0.x;
    ty = plg[i].y - p0.y;
    tp1.fx = tx * dx + ty * dy;
    tp1.fy = ty * dx - tx * dy;

    if( fabs( tp0.fy ) < 1 && fabs( tp1.fy ) < 1 )
      continue;

    if( tp0.fx <= 0 && tp1.fx <= 0 )
      continue;

    if( tp0.fx >= dl && tp1.fx >= dl )
      continue;

    if( tp0.fy >= 0 && tp1.fy >= 0 )
      continue;

    if( tp0.fy < 0 && tp1.fy < 0 )
      continue;

    double x = tp0.fx - tp0.fy * ( tp1.fx - tp0.fx ) / ( tp1.fy - tp0.fy );
    if( x >= 1 && x <= dl - 1 )
    {
      int k = tmp.GetSize();
      while( --k >= 0 && tmp[k] > x );
      tmp.InsertAt( k + 1, x );
    }
  }

  double x0 = .5 * tmp[1];
  CPoint t( p0.x + x0 * dx, p0.y + x0 * dy );

  int i = IsPointInPolygon( t, plg, n, 0 ) == bInside ? 1 : 2;

  for( n = tmp.GetSize(); i < n ; i += 2 )
  {
    double x0 = tmp[i-1], x1 = tmp[i];
    CPoint t0( p0.x + x0 * dx + .5, p0.y + x0 * dy + .5 );
    CPoint t1( p0.x + x1 * dx + .5, p0.y + x1 * dy + .5 );
    if( t0 != t1 )
      plh->Add( t0, t1 );
  }
}
//=====================================================================
void
SmartClipPlg( CPoint* plg, int pgn, CPoint* rgp, int rgn, LItemHash* plh,
              int bClipping, int bInside )
{
  LItemHash1 lh( plg, pgn );
  lh.RemDup();
  LItemHash::LItem* p = lh.GetFirst();
  while( p = lh.GetNext())
    SmartClipLine( p->m_Point0, p->m_Point1, rgp, rgn, plh, bClipping, bInside );
}
//=====================================================================
CPoint*
SmartSubConPolygons( CPoint* pp0, int n0, CPoint* pp1, int n1, int bAdd )
{

/*
  int n[2];
  if( bAdd ? !IsAnyPointInPolygon( pp1, n1, pp0, n0, 1 ) : IsAllPointsInPolygon( pp1, n1, pp0, n0, 1 ))
  {
    FindPointToPoint( pp0, n0, pp1, n1, &n );
    return SubConPolygons( pp0, n0, pp1, n1, n[0], n[1] );
  }
*/

  if( bAdd )
  {
    PrepareForAdding( pp0, n0, pp1, n1 );
    PrepareForAdding( pp1, n1, pp0, n0 );
  }

  LItemHash2 lh( n0 + n1 );
  SmartClipPlg( pp1, n1, pp0, n0, &lh, 0, !bAdd );
  SmartClipPlg( pp0, n0, pp1, n1, &lh, 0, 0 );

  LsList pgs;
  CPoint p0, p1;
  while( lh.GetHead( p0, p1 ))
  {
    PtList* pplg = new PtList;
    pplg->AddHead( p0 );
    pplg->AddTail( p1 );

    for(;;)
    {
      CPoint np;
      if( lh.GetNext( p0, np ))
        pplg->AddHead( p0 = np );
      else
      if( lh.GetNext( p1, np ))
        pplg->AddTail( p1 = np );
      else
        break;
    }

    if( pplg->GetCount() >= 2 && pplg->GetHead() == pplg->GetTail())
      pplg->RemoveTail();

    if( pplg->GetCount() > 2 )
      pgs.AddTail( pplg );
    else
      delete pplg;
  }

  RgnPool rst;

  int nPlg = pgs.GetCount();

  if( nPlg == 0 || bAdd > 1 && nPlg > 1 )
    return 0;

  while( !pgs.IsEmpty())
  {
    PtList* pplg = pgs.RemoveHead();
    int np = pplg->GetCount();
    pp1 = (CPoint*)new CPoint[np+1];
    pp1[0] = CPoint( np, 0 );
    CPoint* p = &pp1[1];
    for( int i = np ; --i >= 0 ; *p++ = pplg->RemoveHead());
    delete pplg;
    ImprovePolygon( pp1 );
    if( pp1->x >= 3 )
      rst.AddRgn( pp1 );
    else
      delete pp1;
  }

  int nRgn = rst.Size();

  RegionDef* pr = rst.Connect();
  ASSERT( pr );
  return pr ? pr->Close() : 0;
}
//=====================================================================
CPoint*
UnSelfCross( CPoint* p0 )
{
  CPoint cp;
  CPoint* p = p0;
  int n = (*p++).x;
  int lim = n - 2;

  for( int i0 = 0 ; i0 <= lim ; ++i0 )
    for( int l = i0 + 2 ; l < n ; ++l )
      if( CrossPoint( p[i0], p[i0+1], p[l], p[l==n-1?0:l+1], &cp, 1 ))
      {
        CPoint* q = new CPoint[n+3];
        CPoint* t = q;
        t->x = n + 2;
        t->y = 0;
        ++t;
        memcpy( t, p, ( i0 + 1 ) * sizeof CPoint );
        t += i0 + 1;
        *t++ = cp;
        for( int j = l ; j > i0 ; *t++ = p[j--] );
        *t++ = cp;
        memcpy( t, p + l + 1, ( n - l - 1 ) * sizeof CPoint );
        delete p0;
        p0 = q;
        p = p0;
        n = (*p++).x;
        lim = n - 2;
        break;
      }

  return p0;
}
//=====================================================================
CPoint*
SmartSubtractPolygons( CPoint* pp0, int n0, CPoint* pp1, int n1 )
{
  return SmartSubConPolygons( pp0, n0, pp1, n1, 0 );
}
//=====================================================================
CPoint*
SmartConnectPolygons( CPoint* pp0, int n0, CPoint* pp1, int n1 )
{
  return SmartSubConPolygons( pp0, n0, pp1, n1, 1 );
}
//=====================================================================
//=====================================================================
pps*
pps::Sort( pps*& h, int n )
{
  if( n == 0 )
    return 0;

  if( n == 1 )
  {
    pps* a = h;
    h = a->next;
    a->next = 0;
    return a;
  }

  if( n == 2 )
  {
    pps* a = h;
    pps* b = a->next;
    h = b->next;
    if( a->dst < b->dst )
    {
      b->next = 0;
      return a;
    }
    else
    {
      b->next = a;
      a->next = 0;
      return b;
    }
  }

  pps* a = Sort( h, n / 2 );
  pps* b = Sort( h, n - n / 2 );
  pps* r;

  if( a->dst < b->dst )
  {
    r = a;
    a = a->next;
  }
  else
  {
    r = b;
    b = b->next;
  }

  for( pps* t = r ;; )
  {
    if( !a )
    {
      t->next = b;
      break;
    }
    if( !b )
    {
      t->next = a;
      break;
    }
    if( a->dst < b->dst )
    {
      t = t->next = a;
      a = a->next;
    }
    else
    {
      t = t->next = b;
      b = b->next;
    }
  }
  return r;
}
//=====================================================================
pps_mem::pps_mem( int n )
{
  m_head = 0;
  m_buf = n > nSBS ? new pps[n] : m_StaticBuf;
  m_size = n;
  while( --n >= 0 )
  {
    m_buf[n].next = m_head;
    m_head = &m_buf[n];
  }
}

pps_mem::~pps_mem()
{
  if( m_buf != m_StaticBuf )
    delete m_buf;
}

pps*
pps_mem::RemoveHead()
{
  if( !m_head )
    return 0;

  pps* h = m_head;
  m_head = m_head->next;
  return h;
}
void
pps_mem::Add( pps* elm )
{
  elm->next = m_head;
  m_head = elm;
}

int
pps_mem::AddList( pps* elm )
{
  int c = 0;
  while( elm )
  {
    pps* h = elm;
    elm = elm->next;
    h->next = m_head;
    m_head = h;
    ++c;
  }
  return c;
}
//=====================================================================
PointHash::PointHash( int nItem )
{
  for( nSize = 1 ; nSize < nItem ; nSize += nSize );
  m_Tab = (CPointItem**) new char[nSize*sizeof(void*)];
  memset( m_Tab, 0, nSize * sizeof(void*));
  nMask = nSize - 1;
}

PointHash::~PointHash()
{
  CPointItem* q;
  for( int i = nSize ; --i >= 0 ; )
    for( CPointItem* p = m_Tab[i] ; p ; q = p->m_pAlter, delete p, p = q );
  delete m_Tab;
}

int
PointHash::Find( CPoint pt )
{
  CPointItem* p;
  for( p = m_Tab[HashFun(pt)] ; p && p->m_Value != pt ; p = p->m_pAlter );
  return p != 0;
}

void
PointHash::Add( CPoint pt )
{
  CPointItem** pp;
  for(pp = &m_Tab[HashFun(pt)] ; *pp && (*pp)->m_Value != pt ; pp = &(*pp)->m_pAlter );
  if( !*pp )
    *pp = new CPointItem( pt );
}

void
PointHash::Add( PointHash& ph )
{
  for( int i = ph.nSize ; --i >= 0 ; )
    for( CPointItem* p = ph.m_Tab[i] ; p ; p = p->m_pAlter )
      Add( p->m_Value );
}
//=====================================================================
/*
int
SplitPolygon( CPoint* p, int n, int* pi )
{
  int const ts = 8;
  int const bs = 64;
  int const bs2 = 256;
  pps_mem buf( bs2 );
  pps* lh = 0;

  int minD = n / 2;
  int minL = 3;

  int nv = 0;
  float dMax = -1;
  for( int d = minD + 1 ; --d >= minL ; )
  {
    for( int i0 = n - d ; --i0 >= 0 ; )
    {
      CPoint p0( p[i0] );
      int i1 = i0 + d;
      if( i1 >= n )
        i1 -= n;

      CPoint p1( p[i1] );

      float dx = p0.x - p1.x, dy = p0.y - p1.y;
      float ds = dx * dx + dy * dy;

      if( dMax > 0 && ds > dMax )
        continue;

      pps* t = buf.RemoveHead();
      ASSERT( t );
      lh = t->Set( i0, i1, ds, lh );
      ++nv;

      int bLast = d == minL && i0 == 0;
      if( buf.IsEmpty() || bLast )
      {
        lh = pps::Sort( lh, nv );
        for( int t = bLast ? nv : ts ; --t >= 0 && lh ; )
        {
          pi[0] = min( lh->ni0, lh->ni1 );
          pi[1] = max( lh->ni0, lh->ni1 );
          CPoint* p0 = p + pi[0];
          CPoint* p1 = p + pi[1];
          CPoint mp( p0->x + p1->x >> 1, p0->y + p1->y >> 1 );
          if( ::IsPointInPolygon( mp, p, n, 0 ) && !IsLineCrossPolygon( *p0, *p1, p, n ))
            return 1;

          pps* h = lh;
          lh = lh->next;
          buf.Add( h );
          --nv;
        }
        pps* tp = lh;
        for( t = bs - ts  ; tp && --t > 0 ; tp = tp->next );
        if( tp )
        {
          pps* h = tp;
          tp = tp->next;
          h->next = 0;
          dMax = h->dst;
          nv -= buf.AddList( tp );
        }
      }
    }
  }
  return 0;
}
*/
//=====================================================================
static void
FindOnLine( CPoint* pt, int np, CPoint cp, int x0[2] )
{
  int i;
  int* xp = new int[np];
  int cn = 0;

  CPoint p1, p0( pt[0] );
  for( i = np ; --i >= 0 ; )
  {
    p1 = p0;
    p0 = pt[i];
    if( p0.y > cp.y && p1.y > cp.y || p0.y < cp.y && p1.y < cp.y )
      continue;

    int x = p0.x;
    if( p0.y != p1.y )
      x += double( p1.x - p0.x ) / double( p1.y - p0.y ) * double( cp.y - p0.y );
    int tn;
    for( tn = cn++ ; --tn >= 0 && xp[tn] > x ; xp[tn+1] = xp[tn] );
    xp[tn+1] = x;
  }

  cn &= ~1;
  int dx;
  for( i = 0 ; i < cn ; i += 2 )
  {
    int tx = abs( cp.x - ( xp[i] + xp[i+1] ) / 2 );
    if( i == 0 || tx < dx )
    {
      dx = tx;
      x0[0] = xp[i];
      x0[1] = xp[i+1];
    }
  }
  delete xp;
}
//=====================================================================
CPoint
FindInnerPoint( CPoint* pp, int np )
{
  int i;
  CBox box;
  for( i = np ; --i >= 0 ; box |= pp[i] );
  int tx[3][2];
  CPoint cp( box.CenterPoint());
  int dh = box.Height() / 4;
  int y[3];
  y[0] = cp.y;
  FindOnLine( pp, np, cp, tx[0] );
  cp.y = y[1] = y[0] - dh;
  FindOnLine( pp, np, cp, tx[1] );
  cp.y = y[2] = y[0] + dh;
  FindOnLine( pp, np, cp, tx[2] );

  i = tx[0][1] - tx[0][0] > tx[1][1] - tx[1][0] ? 0 : 1;
  i = tx[2][1] - tx[2][0] > tx[i][1] - tx[i][0] ? 2 : i;
  cp.y = y[i];
  cp.x = tx[i][1] + tx[i][0] >> 1;

  return cp;
}
//=====================================================================
int
FindCrossPoints( CRect const& r0, CRect const& r1, CPoint pt[2] )
{
	CPoint c0 = r0.CenterPoint();
	CPoint c1 = r1.CenterPoint();
	double R0 = r0.Width() / 2;
	double R1 = r1.Width() / 2;
	double RR = R0 + R1;
	double d = hipo( c0 - c1 );
	if( d > RR )
		return 0;

	if( d < fabs( R0 - R1 ))
		return 0;

	if( d == RR )
	{
		c1 -= c0;
		pt[0] = c0 + CPoint( MulDiv( c1.x, R0, RR ), MulDiv( c1.y, R0, RR ));
		return 1;
	}

	double a = ( R0 * R0 - R1 * R1 + d * d ) / 2 / d;
	double h = sqrt( R0 *R0 - a * a );
	double x2 = c0.x + a * ( c1.x - c0.x ) / d;
	double y2 = c0.y + a * ( c1.y - c0.y ) / d;
	pt[0].x = x2 + h * ( c1.y - c0.y ) / d;
	pt[1].x = x2 - h * ( c1.y - c0.y ) / d;
	pt[0].y = y2 - h * ( c1.x - c0.x ) / d;
	pt[1].y = y2 + h * ( c1.x - c0.x ) / d;
  return 2;
}
//=====================================================================
