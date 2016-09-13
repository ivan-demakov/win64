#include "stdafx.h"

#include <math.h>
#include "ksi_type.h"
#include "cspot.h"
#include "drawitem.h"
#include "mathutil.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CSpot::operator CRect() const
{
	return CRect ( x - m_Radius, y - m_Radius, x + m_Radius, y + m_Radius );
}
//=====================================================================
CRect
CSpot::Inflate( CRect& r ) const
{
	return CRect( r.left - m_Radius, r.top - m_Radius, r.right + m_Radius, r.bottom + m_Radius );
}
//=====================================================================
int
CSpot::DetectRect( CRect const& rect, int angle, int w, int* pDist ) const
{
  w /= 2;
	return DetectPolyline( Rect2Poly( rect, angle ), 5, w, w, pDist );
}
//=====================================================================
int
CSpot::DetectEllipse( CRect const& rect, int w, int* pDist ) const
{
	int d = abs( DistPointToEllipse( *this, rect ));

	if( d >= m_Radius + w )
		return 0;

	if( !pDist )
		return 1;
		
	if( d > *pDist )
		return 0;

	*pDist = d;
	return 1;
}
//=====================================================================
int
CSpot::DetectOval( CRect const& rect, int* pDist ) const
{
	int d = DistPointToEllipse( *this, rect );

	if( d > m_Radius )	
		return 0;

	if( !pDist )
  	return 1;
	
	if( d > *pDist )
		return 0;

	*pDist = max( 0, d );	
	return 1;
}
//=====================================================================
int
CSpot::DetectArc( CRect const& rect, int start, int delta, int w, int* pDist ) const
{
	int d = abs( DistPointToArc( *this, rect, start, delta ));

	if( d >= m_Radius + w )
		return 0;

	if( !pDist )
		return 1;
		
	if( d > *pDist )
		return 0;

	*pDist = d;
	return 1;
}
//=====================================================================
int
CSpot::DetectPie( CRect const& rect, int start, int delta, int* pDist ) const
{
  if( !IsPointInSector( *this, rect, start, delta ))
		return 0;

	int d = DistPointToEllipse( *this, rect );

	if( d > m_Radius )
		return 0;

	if( pDist )
  	*pDist = 0;
	
	return 1;
}
//=====================================================================
int
CSpot::DetectChord( CRect const& rect, int start, int delta, int* pDist ) const
{
	int d1 = DistToChord( *this, rect, start, delta );
	int d2 = DistPointToEllipse( *this, rect );

	if( -d1 > m_Radius || d2 > m_Radius )
		return 0;

	if( pDist )
  	*pDist = 0;
	
	return 1;
}
//=====================================================================
int
CSpot::DetectChord( CRect const& rect, int start, int delta, int w, int* pDist ) const
{
	int d1 = DistToChord( *this, rect, start, delta );
	int d2 = IsPointInSector( *this, rect, start, delta ) ? DistPointToEllipse( *this, rect ) : MAX_INT;
	d1 = min( abs( d1 ), abs( d2 ));

	if( d1 > m_Radius )
		return 0;

	if( pDist )
  	*pDist = d1;
	
	return 1;
}
//=====================================================================
int
CSpot::DetectBar( CRect const& rect, int angle, int* pDist ) const
{
	CPoint pt( TurnPoint( CenterPoint() - rect.TopLeft(), -angle ));
  CRect R( 0, 0, rect.Width(), rect.Height());
	R.NormalizeRect();
	
	if( !R.PtInRect( pt ))
		return DetectRect( rect, angle, 0, pDist );
	
	if( pDist )
  	*pDist = 0;

	return 1;
}
//=====================================================================
int
CSpot::DetectPolyline( CPoint const* points, int nPoints, int wl, int wh, int* pDist ) const
{
  if( !nPoints )
    return 0;

	int d = MAX_INT;
	int n = 0;
  for( int i = 0 ; --nPoints > 0 && d ; ++i, ++points )
	{
    int t = abs( DistPointToLinePoint( *this, points[0], points[1] ));
		if( t < d )
		{
			d = t;
			n = i;
		}
	}
	
  if( d == MAX_INT || abs( d ) >= m_Radius + ( d > 0 ?  wh : -wl ))
		return 0;

	if( !pDist )
		return n + 1;
		
	if( abs( d ) > *pDist )
		return 0;

	*pDist = abs( d );
	return n + 1;
}
//=====================================================================
int
CSpot::DetectLine( CPoint const* points, int wl, int wh, int* pDist ) const
{
	return DetectPolyline( points, 2, wl, wh, pDist );
}
//=====================================================================
int
CSpot::DetectPolygon( CPoint const* points, int nPoints, int* pDist ) const
{
	if( !points )
		return 0;

  if( !IsPointInPolygon( *this, points, nPoints, 1 ))
		return DetectPolyline( points, nPoints, 0, 0, pDist );
	
	if( pDist )
		*pDist = 0;
  return 1;
}
//=====================================================================
int
CSpot::StrictPolyDetect( CPoint const* points, int nPoints, int* pDist ) const
{
	int d = MAX_INT;
  while( --nPoints >= 0 && d )
	{
		int t = abs( dist( *this, *points++ ));
		d = min( t, d );
	}

  if( d > m_Radius || pDist && d > *pDist )
		return 0;

	if( pDist )
		*pDist = d;

	return 1;    
}
//=====================================================================
