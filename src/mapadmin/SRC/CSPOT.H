#ifndef __CSPOT_H__
#define __CSPOT_H__
//=====================================================================
class CSpot: public CPoint
{
private:
  int m_Radius;

public:
  CSpot(){}
  CSpot( CPoint p, int r = 0 ) : CPoint( p ), m_Radius( r ) {}

  int GetRadius() const { return m_Radius; }
	CPoint CenterPoint() const { return *this; }
  int DetectRect( CRect const& rect, int angle, int w = 0, int* pDist = 0 ) const;
  int DetectBar( CRect const& rect, int angle, int* pDist = 0 ) const;
  int DetectEllipse( CRect const& rect, int w, int* pDist = 0 ) const;
  int DetectOval( CRect const& rect, int* pDist = 0 ) const;
  int DetectArc( CRect const& rect, int start, int delta, int w, int* pDist = 0 ) const;
  int DetectPie( CRect const& rect, int start, int delta, int* pDist = 0 ) const;
  int DetectChord( CRect const& rect, int start, int delta, int* pDist = 0 ) const;
  int DetectChord( CRect const& rect, int start, int delta, int w, int* pDist = 0 ) const;
  int DetectPolyline( CPoint const* points, int nPoints, int wl, int wh, int* pDist = 0 ) const;
  int DetectLine( CPoint const* line, int wl, int wh, int* pDist = 0 ) const;
  int DetectPolygon( CPoint const* points, int nPoints, int* pDist = 0 ) const;
	int StrictPolyDetect( CPoint const* points, int nPoints, int* pDist ) const;
	CRect Inflate( CRect& r ) const; 
	operator CRect() const;
};
//=====================================================================
#endif