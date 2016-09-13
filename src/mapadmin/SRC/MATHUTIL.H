#ifndef __MATHUTIL_H__
#define __MATHUTIL_H__
//=====================================================================
#include "TGstream.h"
#include "polar.h"
#include "ksi.h"
#include "MathConst.h"
//=====================================================================
class CDrawObject;
//=====================================================================
class PointHash
{
  class CPointItem
  {
  public:
    CPoint      m_Value;
    CPointItem* m_pAlter;

    CPointItem( CPoint p ) : 
      m_pAlter( 0 ),
      m_Value( p )
    {}
  };

public:
  PointHash( int nItem );
  ~PointHash();

  void Add( CPoint p );
  void Add( PointHash& ph );
  int  Find( CPoint p );
  int  HashFun( CPoint p );

private:
  int nSize;
  int nMask;
  CPointItem** m_Tab;
};

inline int
PointHash::HashFun( CPoint p )
{
  return p.x & nMask;
}
//=====================================================================
class LItemHash
{
public:
  struct LItem
  {
    CPoint m_Point0;
    CPoint m_Point1;
    void SetPoints( CPoint p0, CPoint p1 ) { m_Point0 = p0; m_Point1 = p1; }
    int Free() { return m_Point0.x == 0; }
    int Used() { return m_Point0.x > 0; }
    int Cons( CPoint p0, CPoint p1 ) { return m_Point0 == p0 && m_Point1 == p1; }
  };

protected:
  LItem* m_pTab;
  int    m_nSize;
  int    m_nUsed;

private:
  LItem m_pInTab[1024];

public:
  LItemHash( UINT size );
  ~LItemHash();

  void Add( CPoint const& p0 ) { Add( p0, p0 ); }
  virtual void Add( CPoint const& p0, CPoint const& p1 );
  int Rem( CPoint const& p0, CPoint const& p1 );
  int RemDup();
  virtual int GetHead( CPoint& p0, CPoint& p1 );
  virtual int GetNext( CPoint& p0, CPoint& p1 );
  operator ksi_obj() const;
  int CalcLines() const;
  void CopyLines( CPoint* p ) const;
  int Del( LItem* p ) { p->m_Point0.x = -1; return 1; }
  int Find( CPoint p0 );

private:
  LItem* FindPos( CPoint p0, CPoint p1 );
  LItem* FindPos( CPoint p0 );
};
//=====================================================================
class LItemHash1 : public LItemHash
{
public:
  LItemHash1( CPoint* p1, int n );
  LItem* GetFirst();
  LItem* GetNext();
private:
  LItem* m_pCur;
};
//=====================================================================
class LItemHash2 : public LItemHash
{
public:
  LItemHash2( UINT size ) : LItemHash( size * 2 ) {}
  void Add( CPoint const& p0, CPoint const& p1 );    
  int GetHead( CPoint& p0, CPoint& p1 );
  int GetNext( CPoint& p0, CPoint& p1 );
};
//=====================================================================
struct LnDef
{
  CPoint m_Point0; 
  CPoint m_Point1; 
  LnDef(){}
  LnDef( CPoint p0, CPoint p1 ) : m_Point0( p0 ), m_Point1( p1 ) {}
  int operator==( LnDef& ld )
  {
    return ld.m_Point0 == m_Point1 && ld.m_Point1 == m_Point0 ||
           ld.m_Point0 == m_Point0 && ld.m_Point1 == m_Point1;
  }
};
//=====================================================================
struct RepInf
{
  CDrawObject* pRp;
  Polar  pPt;
  CPoint sPt;
  CPoint dPt;
  double dis;
};
//=====================================================================
#define N_REP 3
//=====================================================================
#define PtList CList< CPoint, CPoint& >
#define LsList CList< PtList*, PtList* >
//=====================================================================
class CTrig
{
public:
  class CTrigTabGenerator
  {
  public:
    CTrigTabGenerator();
  };

  friend class CTrigTabGenerator;
  friend int atan2( int vCos, int vSin );
  friend int atan2( CSize s );
  friend double sin( int angle );
  friend double cos( int angle );

public:
  CTrig(){}

private:
  static double TrigTab[];
  static CTrigTabGenerator TG;
};

int Intersect( CRect const* r1, CRect const* r2 );
int dist( CPoint const& point1, CPoint const& point2 );
double hipo( int s1, int s2 );

double Deg2Rad( int angle );
double Deg2Rad( double angle );
int Rad2Deg( double angle );

CPoint  TurnPoint( double x, double y, int angle );
CPoint  TurnPoint( CPoint const& point, double dx, double dy, double dl );
CPoint  TurnPoint( CPoint const& point, CSize const& center, int angle );
CPoint  TurnPoint( CPoint const& point, CSize const& center, double dx, double dy, double dl );
CPoint* Rect2Poly( CRect const& rect, double vcos, double vsin );
CPoint* Rect2Poly( CPoint const& lt, CSize sz, int an );
CRect   TurnRect( CRect const& rect, int angle );
int CrossPoint( CPoint const& p00, CPoint const& p01,
                 CPoint const& p10, CPoint const& p11, 
                 CPoint* pCrosPoint = 0, int bOnLinesOnly = 1 );
int CrossPoint( CPoint const* pp0, CPoint const* pp1,
                 CPoint* pCrosPoint = 0, int bOnLinesOnly = 1 );
int TrueCrossPoint( CPoint const* pp0, CPoint const* pp1,
                     CPoint* pCrosPoint = 0 );
int FindNearesRepers( CPoint dp, RepInf* pRI, int nRep = N_REP );
int FindNearesRepers( Polar  dp, RepInf* pRI, int nRep = N_REP );
//Polar Dec2Pol( Point3d p0 );
//double  dist( Point3d& p0, Point3d& p1 );
double  angle( CPoint p0, CPoint p1, CPoint p2 );
//CPoint  Triang( Polar p0, CPoint r0, Polar p1, CPoint r1, Polar p2 );
//Polar   Triang( Polar p0, CPoint r0, Polar p1, CPoint r1, CPoint p2 );
int IsSelfCrossPolyline( CPoint const* p, int n );
int IsSelfCrossPolygon( CPoint const* p, int n );
int IsLineCrossPolyline( CPoint const* l, CPoint const* p, int n, CPoint* pCrosPoint = 0 );
int IsLineCrossPolyline( CPoint p0, CPoint p1, CPoint const* p, int n, CPoint* pCrosPoint = 0 );
int IsLineCrossPolygon( CPoint p0, CPoint p1, CPoint const* p, int n );
int IsPolylineCrossPolyline( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint = 0 );
int IsPolylineMatchPolyline( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint = 0 );
int IsPolylineCrossPolygon( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint = 0 );
int IsPolygonCrossPolyline( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint = 0 );
int IsPolygonCrossPolygon( CPoint const* p0, int n0, CPoint const* p1, int n1, CPoint* pCrosPoint = 0 );
int IsPointInPolygon( CPoint pnt, CPoint const* pll, int n, int bWithBound, int d = 0 );
int IsPointInTriangle( CPoint pnt, CPoint const* tgl );
int IsAnyPointInTriangle( CPoint const* pPnt, int nPnt, CPoint const* tgl );
int IsAnyPointInPolygon( CPoint const* pPnt, int nPnt, CPoint const* pll, int n, int bWithBound = 0 );
int IsAllPointsInPolygon( CPoint const* pPnt, int nPnt, CPoint const* pll, int n, int bWithBound = 0 );
int IsAllPointsInRect( CPoint const* pPnt, int nPnt, CRect r );
int IsAllPointsOnPolyline( CPoint const* pPnt, int nPnt, CPoint const* pll, int n, int d = 1 );
int IsAnyPointOnPolygon( CPoint const* pPnt, int nPnt, CPoint const* pll, int n, int d = 1 );
int IsPolygonOnPolygon( CPoint const* pPg0, int n0, CPoint const* pPg1, int n1, int d = 1 );
int IsPointOnLine( CPoint pt, CPoint p0, CPoint p1, int d = 1 );
int DistPointToLine( CPoint pt, CPoint p0, CPoint p1 );
int DistPointToLinePoint( CPoint pt, CPoint p0, CPoint p1 );
int IsPointOnPolyline( CPoint pt, CPoint const* pll, int n, int d = 1 );
int DistPointToPolyline( CPoint pt, CPoint const* pll, int n );
int DistPointToEllipse( CPoint pt, CRect const& rect );
int IsPointInSector( CPoint pt, CRect const& rect, int start, int delta );
int DistPointToArc( CPoint pt, CRect const& rect, int start, int delta );
int DistPointToSector( CPoint pt, CRect const& rect, int start, int delta );
int IsPointInRect( CPoint pt, CRect const& rect, int angle );
int DistToChord( CPoint pt, CRect const& rect, int start, int delta );

int IsPointOnPolygon( CPoint pt, CPoint const* pll, int n, int d = 1 );
int Clockwise( CPoint const* p, int n );
int Clockwise( CPoint const* p, int n, int i0, int i1 );
void PrepareForAdding( CPoint* pp0, int n0, CPoint* pp1, int n1 );

double CalcLength( CPoint* pp, int pn );
int MakeSpace( TGstream& dst, CPoint* pp, int n, int r, int bLine );
int SmartCloneLine( TGstream& dst, CPoint* pp, int n, int r );
int DecimatePoly( CPoint* p, int n, int h );
void ReversePoly( CPoint* pp, int n );
CPoint* SubConPolygons( CPoint* p0, int n0, CPoint* p1, int n1 );
CPoint* SubConPolygons( CPoint* p0, int n0, CPoint* p1, int n1, int i0, int i1 );
CPoint* SmartSubtractPolygons( CPoint* p0, int n0, CPoint* p1, int n1 );
CPoint* SmartConnectPolygons( CPoint* p0, int n0, CPoint* p1, int n1 );
CPoint* SmartConnectPolylines( CPoint* p0, int n0, CPoint* p1, int n1 );
CPoint* SmartSubConPolygons( CPoint* pp0, int n0, CPoint* pp1, int n1, int bAdd );
CPoint* UnSelfCross( CPoint* p );
void SmartClipLine( CPoint p0, CPoint p1, CPoint const* plg, int n, LItemHash* plh, int bClipping, int bInside );
void SmartClipPlg( CPoint* plg, int pgn, CPoint* rgp, int rgn, LItemHash* plh,
                  int bClipping, int bInside );
int SplitPolygon( CPoint* p, int n, int* pi );
int ImprovePoly( CPoint* p, int n );
CPoint* ImprovePolygon( CPoint* pp );
int FindPointToPoint( CPoint* p0, int n0, CPoint* p1, int n1, int pi[][2], int ni = 1, int bs2 = 1024, int bs = 32 );
CPoint Pol2Dec( Polar mp );
Polar  Dec2Pol( CPoint mp );
double Djordan( double* sp, int n, int m );
CPoint StepByLine( CPoint p0, CPoint p1, int l );
CPoint FindInnerPoint( CPoint* pp, int np );
int    FindCrossPoints( CRect const& r0, CRect const& r1, CPoint pt[2] );
//=====================================================================
extern double const Pi;
extern double const Deg;
extern int const i900;
extern int const i1800;
extern int const i3600;
//=====================================================================
inline int 
Intersect( CRect const& r1, CRect const& r2 )
{
  return Intersect( &r1, &r2 );
}
//=====================================================================
inline int
atan2( CSize s )
{
  return atan2( s.cx, -s.cy );
}
//=====================================================================
inline double
hipo( CSize p )
{
  return hipo( p.cx, p.cy );
}
//=====================================================================
inline CPoint  
TurnPoint( CPoint const& point, int angle )
{
  return TurnPoint( point.x, point.y, angle );
}
//=====================================================================
inline CPoint  
TurnPoint( int x, int y, CSize const& center, int angle )
{
  return TurnPoint( CPoint( x, y ), center, angle );
}
//=====================================================================
inline CPoint*
Rect2Poly( CRect const& rect, int angle )
{
  return Rect2Poly( rect, cos( angle ), sin( angle ));
}
//=====================================================================
struct pps
{
  int ni0, ni1;
  float dst;
  pps* next;
  pps() {}
  pps( int i0, int i1, float ds, pps* nxt = 0 )
  {
    Set( i0, i1, ds, nxt );
  }

  pps* Set( int i0, int i1, float ds, pps* nxt )
  {
    ni0 = i0;
    ni1 = i1;
    dst = ds;
    next = nxt;
    return this;
  }
  static pps* Sort( pps*& h, int n );
};

class pps_mem
{
  enum { nSBS = 1024 };
public:
  pps_mem( int size );
  ~pps_mem();
  void Add( pps* elm );
  int  AddList( pps* elm );
  int  IsEmpty() { return m_head == 0; }
  pps* RemoveHead();

private:
  pps  m_StaticBuf[nSBS];
  pps* m_buf;
  pps* m_head;
  int  m_size;
};
//=====================================================================
extern double const Min;
//=====================================================================
#endif