// file "DRAWPRIM.H"
// store classes for 'drawable primitives' - definition
// Autors: A.Tarnovski

#ifndef DRAWPRIM_H
#define DRAWPRIM_H

#include "cbox.h"
#include "cspot.h"
#include "ident.h"
#include "drawitem.h"
#include "mathutil.h"
#include "selmode.h"

class CDrawObject;
class CMapView;
class CMapDoc;
//=====================================================================
enum // primitive status flags
{
  SF_STATUS_MASK = 0x0f,
  SF_SEL_VISIBLE = 0x20,
  SF_LOW_VISIBLE = 0x40,
  SF_HIG_VISIBLE = 0x80,
  SF_ANY_VISIBLE = SF_LOW_VISIBLE | SF_HIG_VISIBLE
};
//=====================================================================
enum
{
  KEY_NOTHING,
  KEY_CONTAINER,
  KEY_POINT,
  KEY_RECTANGLE,
  KEY_BAR,
  KEY_OUTLINED_BAR,
  KEY_ELLIPSE,
  KEY_RING,
  KEY_OVAL,
  KEY_CIRCLE,
  KEY_OUTLINED_OVAL,
  KEY_OUTLINED_CIRCLE,
  KEY_POLYLINE,
  KEY_BOUND_POLYLINE,
  KEY_FOOTNOTE,
  KEY_POLYGON,
  KEY_OUTLINED_POLYGON,
  KEY_SMART_POLYGON,
  KEY_TEXT,
  KEY_TEXT_BY_LINE,
  KEY_LINES,
  KEY_ARC,
  KEY_PIE,
  KEY_CHORD,
  KEY_OUTLINED_CHORD,
  KEY_ICON
};
//=====================================================================
// abstract base class for all drawable primitives and container
class CVisiblePrim : public CItem
{
protected:
  unsigned int m_Flags;
  CVisiblePrim* m_pNext;
  static const CStrAssoc PrimKeys[];

public:
  CVisiblePrim( ksi_obj init_list );
  CVisiblePrim( ): m_Flags( 0 ),  m_pNext( 0 ){}
  CVisiblePrim( CVisiblePrim const* p ): m_Flags( p->m_Flags ), m_pNext( 0 ){}
  virtual ~CVisiblePrim() {}

  virtual operator ksi_obj() = 0;

  virtual void Draw( CDC* pDC ){}
  virtual void InflateClipSize( CSize& size ){}
  virtual void SetPoly( CPoint const* points, int pn ) {}
  virtual void MoveOn( int cx, int cy ) { MoveOn( CSize( cx, cy )); }
  virtual void MoveOn( CSize const& s ) = 0;
  virtual void SpinOn( CSize const& center, int angle ) = 0;
  virtual void ScaleOn( double cf, CPoint c ) = 0;
  virtual void GetParams( CVisiblePrim* pPrim ){}
  virtual void CalcBoundBox( CBox& box ) = 0;
  virtual void SetSelectFlag( unsigned char flags ) { SetFlags( flags, SF_STATUS_MASK ); }
  virtual int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 ) { return 0; }
	virtual int GetRing( CSpot const& spot, CRect* rect ){ return 0; }
  virtual int GetAngle( CSpot const& spot, int* pAngle ) { return 0; }		
  virtual int GetType() { return KEY_NOTHING; }
  virtual int TranslateToMap( TGstream* pDst, int t = 0 ) = 0;
  virtual CPoint* GetPoint( int* pnPnt )    { return 0; }
  virtual CPoint* GetPolyline( int* pnPnt ) { return 0; }
  virtual CPoint* GetPolygon( int* pnPnt )  { return 0; }
  virtual char const* GetText()             { return 0; }
  virtual CVisiblePrim* MakeCopy() = 0;

  void SetFlags( unsigned int flags, unsigned int mask = 0 ) { m_Flags &= ~mask; m_Flags |= flags; }
  void ResetFlags( unsigned int flags ){ m_Flags &= ~flags; }
  void ResetSelectFlag() { SetSelectFlag( 0 ); }
  void ResetAllFlags(){ m_Flags = 0; }
  void SetNext( CVisiblePrim* p ) { m_pNext = p; }
  int  TestFlags( unsigned int flags, unsigned int mask = 0 ) const { return ( m_Flags & ( mask ? mask : flags )) == flags; }
  int  TestAnyFlag( unsigned int mask ) const { return m_Flags & mask; }
  int  SelectFlag() const { return TestAnyFlag( SF_STATUS_MASK ); }
  int  IsSelected() const { return SelectFlag() != 0; }
  int  IsAnyVisible() const { return TestAnyFlag( SF_ANY_VISIBLE ); }
  int  IsHigVisible() const { return TestFlags( SF_HIG_VISIBLE ); }
  int  IsLowVisible() const { return TestFlags( SF_LOW_VISIBLE ); }
  int  IsSelVisible() const { return TestFlags( SF_SEL_VISIBLE ); }
  void ReverseFlags( int msk ) { m_Flags ^= msk; }
  CVisiblePrim* GetNext() { return m_pNext; }
};
//=====================================================================
class CPointPrim: public CVisiblePrim
{
protected:
  CCoordItem m_Coord;

public:
  CPointPrim( ksi_obj init_list ):
    CVisiblePrim( init_list ),
    m_Coord( init_list )
    {}
  CPointPrim( CPoint const& p ): m_Coord( p ){}
  CPointPrim( int x, int y ): m_Coord( x, y ){}
  CPointPrim( CPointPrim const* p ):
    CVisiblePrim( p ),
    m_Coord( p->m_Coord )
  {}
  CVisiblePrim* MakeCopy(){ return new CPointPrim( this ); }
  operator ksi_obj();
  CPoint* GetPoint( int* pnPnt ) { *pnPnt = 1; return &m_Coord; }
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void CalcBoundBox( CBox& box );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void ScaleOn( double cf, CPoint c ){}
};
//=====================================================================
class CRectPrim: public CVisiblePrim
{
protected:
  CCoordItem m_Coord;
  CSizeItem  m_Size;

protected:
  void Rect2Ellipse();
  void Ellipse2Rect();
  void Ellipse2Ring();
  CPoint* Ellipse2Point();

public:
  CRectPrim( ksi_obj init_list ):
    CVisiblePrim( init_list ),
    m_Coord( init_list ),
    m_Size( init_list )
    {}
  CRectPrim( CPoint const& p, CSize const& s, int a = 0 ):
    m_Coord( p ), m_Size( s, a ){}
  CRectPrim( CRect const& r, int a = 0 ):
    m_Coord( r.left, r.top ),
    m_Size( r.right - r.left, r.bottom - r.top, a ){}
  CRectPrim( int l, int t, int r, int b, int a = 0 ):
    m_Coord( l, t ),
    m_Size( r - l, b - t, a ){}
  CRectPrim( CRectPrim const* p ):
    CVisiblePrim( p ),
    m_Coord( p->m_Coord ),
    m_Size( p->m_Size )
  {}
  CVisiblePrim* MakeCopy(){ return new CRectPrim( this ); }
  operator CRect() const;
  operator ksi_obj();
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void ScaleOn( double cf, CPoint c );
  int GetAngle( CSpot const& spot, int* pAngle );
  CPoint* GetPoint( int* pnPnt ) { *pnPnt = 1; return &m_Coord; }
  CPoint GetCoord() { return m_Coord; }
  CSize  GetSize()  { return m_Size; }
  int GetAngle()    { return m_Size.m_Angle; }
  void SetCoord( CPoint p ) { m_Coord.x = p.x; m_Coord.y = p.y; }
  void SetSize( CSize s )   { m_Size.cx = s.cx; m_Size.cy = s.cy; }
  void SetAngle( int a )    { m_Size.m_Angle = a; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class CStrokRectPrim: public CRectPrim
{
protected:
  CStrokItem m_Strok;

public:
  CStrokRectPrim( ksi_obj init_list ):
    CRectPrim( init_list ),
    m_Strok( init_list )
    {}
  CStrokRectPrim( CRectPrim const& rp, CStrokItem const& si ):
    CRectPrim( rp ), m_Strok( si ){}
  CStrokRectPrim( CStrokRectPrim const* p ):
    CRectPrim( p ),
    m_Strok( p->m_Strok )
  {}

  operator ksi_obj();
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_RECTANGLE; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void ScaleOn( double cf, CPoint c );
  void GetParams( CVisiblePrim* pPrim );
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class CFillRectPrim: public CRectPrim
{
protected:
  CFillItem m_Fill;

public:
  CFillRectPrim( ksi_obj init_list ):
    CRectPrim( init_list ),
    m_Fill( init_list )
    {}
  CFillRectPrim( CRectPrim const& rp, CFillItem const& fi ):
    CRectPrim( rp ), m_Fill( fi ){}
  CFillRectPrim( CFillRectPrim const* p ):
    CRectPrim( p ),
    m_Fill( p->m_Fill )
  {}
  operator ksi_obj();
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_BAR; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void GetParams( CVisiblePrim* pPrim );
};
//=====================================================================
class COutlinedRectPrim: public CStrokRectPrim
{
protected:
  CFillItem m_Fill;

public:
  COutlinedRectPrim( ksi_obj init_list ):
    CStrokRectPrim( init_list ),
    m_Fill( init_list )
    {}
  COutlinedRectPrim( CRectPrim const& rp, CStrokItem const& si,
                     CFillItem const& fi ):
    CStrokRectPrim( rp, si ), m_Fill( fi ){}
  COutlinedRectPrim( COutlinedRectPrim const* p ):
    CStrokRectPrim( p ),
    m_Fill( p->m_Fill )
  {}
  operator ksi_obj();
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_OUTLINED_BAR; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void GetParams( CVisiblePrim* pPrim );
};
//=====================================================================
class CRectangle: public CStrokRectPrim
{
public:
  CRectangle( ksi_obj init_list ):
    CStrokRectPrim( init_list )
    {}
  CRectangle( CRectPrim const& rp, CStrokItem const& si ):
    CStrokRectPrim( rp, si )
    {}
  CRectangle( CRectangle const* p ):
    CStrokRectPrim( p )
    {}
  CVisiblePrim* MakeCopy(){ return new CRectangle( this ); }
  void Draw( CDC* pDC );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class CEllipse: public CStrokRectPrim
{
public:
  CEllipse( ksi_obj init_list ):
    CStrokRectPrim( init_list )
    {
      Rect2Ellipse();
      m_Size.Angle0();
    }
  CEllipse( CRectPrim const& rp, CStrokItem const& si ):
    CStrokRectPrim( rp, si )
    { Rect2Ellipse(); }
  CEllipse( CEllipse const* p ):
    CStrokRectPrim( p )
    {}
  CVisiblePrim* MakeCopy(){ return new CEllipse( this ); }
  void SpinOn( CSize const& center, int angle );
  void Draw( CDC* pDC );
  CPoint* GetPoint( int* pnPnt ) { *pnPnt = 1; return Ellipse2Point(); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class CRing: public CEllipse
{
public:
  CRing( ksi_obj init_list ):
    CEllipse( init_list )
    { Ellipse2Ring(); }
  CRing( CRectPrim const& rp, CStrokItem const& si ):
    CEllipse( rp, si )
    { Ellipse2Ring(); }
  CVisiblePrim* MakeCopy(){ return new CRing( this ); }
  CRing( CRing const* p ):
    CEllipse( p )
  {}
	int GetRing( CSpot const& spot, CRect* rect ){ *rect = *this; return 1; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class CBar: public CFillRectPrim
{
public:
  CBar( ksi_obj init_list ):
    CFillRectPrim( init_list )
    {}
  CBar( CRectPrim const& rp, CFillItem const& fi ):
    CFillRectPrim( rp, fi )
    {}
  CBar( CBar const* p ):
    CFillRectPrim( p )
    {}
  CVisiblePrim* MakeCopy(){ return new CBar( this ); }
  void Draw( CDC* pDC );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class COval: public CFillRectPrim
{
public:
  COval( ksi_obj init_list );
  COval( CRectPrim const& rp, CFillItem const& fi ):
    CFillRectPrim( rp, fi )
    { Rect2Ellipse(); }
  COval( COval const* p ):
    CFillRectPrim( p )
    {}
  CVisiblePrim* MakeCopy(){ return new COval( this ); }
  void SpinOn( CSize const& center, int angle );
  void Draw( CDC* pDC );
  CPoint* GetPoint( int* pnPnt ) { *pnPnt = 1; return Ellipse2Point(); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class CCircle: public COval
{
public:
  CCircle( ksi_obj init_list ):
    COval( init_list )
    { Ellipse2Ring(); }
  CCircle( CRectPrim const& rp, CFillItem const& fi ):
    COval( rp, fi )
    { Ellipse2Ring(); }
  CCircle( CCircle const* p ):
    COval( p )
    {}
  CVisiblePrim* MakeCopy(){ return new CCircle( this ); }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class COutlinedBar: public COutlinedRectPrim
{
public:
  COutlinedBar( ksi_obj init_list ):
    COutlinedRectPrim( init_list )
    {}
  COutlinedBar( CRectPrim const& rp, CStrokItem const& si,
                CFillItem const& fi ):
    COutlinedRectPrim( rp, si, fi )
    {}
  COutlinedBar( COutlinedBar const* p ):
    COutlinedRectPrim( p )
    {}
  CVisiblePrim* MakeCopy(){ return new COutlinedBar( this ); }
  void Draw( CDC* pDC );
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class COutlinedOval: public COutlinedRectPrim
{
public:
  COutlinedOval( ksi_obj init_list ):
    COutlinedRectPrim( init_list )
    {
      Rect2Ellipse();
      m_Size.Angle0();
    }
  COutlinedOval( CRectPrim const& rp, CStrokItem const& si,
                 CFillItem const& fi ):
    COutlinedRectPrim( rp, si, fi )
    {
      Rect2Ellipse();
    }
  COutlinedOval( COutlinedOval const* p ):
    COutlinedRectPrim( p )
    {}
  CVisiblePrim* MakeCopy(){ return new COutlinedOval( this ); }
  CPoint* GetPoint( int* pnPnt ) { *pnPnt = 1; return Ellipse2Point(); }
  void SpinOn( CSize const& center, int angle );
  void Draw( CDC* pDC );
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class COutlinedCircle: public COutlinedOval
{
public:
  COutlinedCircle( ksi_obj init_list ):
    COutlinedOval( init_list )
    {
      Ellipse2Ring();
    }
  COutlinedCircle( CRectPrim const& rp, CStrokItem const& si,
                   CFillItem const& fi ):
    COutlinedOval( rp, si, fi )
    { Ellipse2Ring(); }
  COutlinedCircle( COutlinedCircle const* p ):
    COutlinedOval( p )
    {}
  CVisiblePrim* MakeCopy(){ return new COutlinedCircle( this ); }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class CPolyline: public CVisiblePrim
{
protected:
  CStrokItem m_Strok;
  CPolyItem  m_Poly;

public:
  CPolyline( ksi_obj init_list, int bBound = 0 ):
    CVisiblePrim( init_list ),
    m_Strok( init_list ),
    m_Poly( init_list, bBound )
    {}
  CPolyline( int pn, CStrokItem const& si, CPoint const* points, int bBound = 0 ):
    m_Strok( si ), m_Poly( pn, points, bBound )
    {}
  CPolyline( CPolyline* p ):
    CVisiblePrim( p ),
    m_Strok( p->m_Strok ),
    m_Poly( p->m_Poly )
    {}

  CVisiblePrim* MakeCopy(){ return new CPolyline( this ); }

  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetAngle( CSpot const& spot, int* pAngle );
  int GetType() { return KEY_POLYLINE; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  CPoint* GetPolyline( int* pnPnt ) { *pnPnt = m_Poly.m_PointNum; return m_Poly.m_bBound ? 0 : m_Poly.m_pPoints; }
  CPoint* GetPolygon( int* pnPnt )  { *pnPnt = m_Poly.m_PointNum; return m_Poly.m_bBound ? m_Poly.m_pPoints : 0; }
  void SetPoly( CPoint const* points, int pn ) { m_Poly.SetPoints( pn, points, m_Poly.IsBound()); }
  void Draw( CDC* pDC );
  void InflateClipSize( CSize& size );
  operator ksi_obj();
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void ScaleOn( double cf, CPoint c );
  operator CPolyItem*() { return &m_Poly; }
  void GetParams( CVisiblePrim* pPrim );
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class CFootnote: public CPolyline
{
public:
  CFootnote( ksi_obj init_list ):
    CPolyline( init_list )
    {}
  CFootnote( int pn, CStrokItem const& si, CPoint const* points ):
    CPolyline( pn, si, points )
    {}
  CFootnote( CFootnote* p ):
    CPolyline((CPolyline*) p )
    {}
  CVisiblePrim* MakeCopy(){ return new CFootnote( this ); }

  void Draw( CDC* pDC );
  CPoint* GetPolyline( int* pnPnt ) { return 0; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class CLines: public CPolyline
{
public:
  CLines( ksi_obj init_list );
  CLines( int pn, CStrokItem const& si, CPoint const* points ):
    CPolyline( pn, si, points )
    {}
  CLines( CLines* p ):
    CPolyline( p )
    {}
  CVisiblePrim* MakeCopy(){ return new CLines( this ); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetAngle( CSpot const& spot, int* pAngle );
  CPoint* GetPolyline( int* pnPnt ) { return 0; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void Draw( CDC* pDC );
  operator ksi_obj();
  void CalcBoundBox( CBox& box ) { CPolyline::CalcBoundBox( box ); }
};
//=====================================================================
class CPolygon: public CVisiblePrim
{
protected:
  CFillItem m_Fill;
  CPolyItem m_Poly;

public:
  CPolygon( ksi_obj init_list ):
    CVisiblePrim( init_list ),
    m_Poly( init_list, 1 ),
    m_Fill( init_list )
    {}
  CPolygon( CFillItem const& fi, int pn = 0, CPoint const* points = 0 ):
    m_Fill( fi ),
    m_Poly( pn, points, 1 )
    {}
  CPolygon( CPolygon* p ):
    CVisiblePrim( p ),
    m_Fill( p->m_Fill ),
    m_Poly( p->m_Poly )
    {}
  CVisiblePrim* MakeCopy(){ return new CPolygon( this ); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetAngle( CSpot const& spot, int* pAngle );
  int GetType() { return KEY_POLYGON; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void SetPoly( CPoint const* points, int pn ) { m_Poly.SetPoints( pn, points, 0 ); }
  CPoint* GetPolygon( int* pnPnt )  { *pnPnt = m_Poly.m_PointNum; return m_Poly.m_pPoints; }
  void Draw( CDC* pDC );
  operator ksi_obj();
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void ScaleOn( double cf, CPoint c );
  operator CPolyItem*() { return &m_Poly; }
  void GetParams( CVisiblePrim* pPrim );
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class COutlinedPolygon : public CPolyline
{
protected:
  CFillItem m_Fill;

public:
  COutlinedPolygon( ksi_obj init_list ):
    CPolyline( init_list, 1 ),
    m_Fill( init_list )
    {}
  COutlinedPolygon( CFillItem const& fi, CStrokItem const& si, int pn = 0, CPoint const* points = 0 ):
    m_Fill( fi ),
    CPolyline( pn, si, points, 1 )
    {}
  COutlinedPolygon( COutlinedPolygon* p ):
    CPolyline( p ),
    m_Fill( p->m_Fill )
    {}
  CVisiblePrim* MakeCopy(){ return new COutlinedPolygon( this ); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_OUTLINED_POLYGON; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  CPoint* GetPolygon( int* pnPnt )  { *pnPnt = m_Poly.m_PointNum; return m_Poly.m_pPoints; }
  void Draw( CDC* pDC );
  operator ksi_obj();
  void GetParams( CVisiblePrim* pPrim );
};
//=====================================================================
class CSmartPolygon : public COutlinedPolygon
{
protected:
  CPolyItem m_SmartPoly;

protected:
  void CreateSmartPoly();

public:
  CSmartPolygon( ksi_obj init_list ):
    COutlinedPolygon( init_list )
    { CreateSmartPoly(); }
  CSmartPolygon( CFillItem const& fi, CStrokItem const& si, int pn = 0, CPoint const* points = 0 ) :
    COutlinedPolygon( fi, si, pn, points )
    { CreateSmartPoly(); }
  CSmartPolygon( CSmartPolygon* p ):
    COutlinedPolygon( p ),
    m_SmartPoly( p->m_SmartPoly )
    {}
  CVisiblePrim* MakeCopy(){ return new CSmartPolygon( this ); }
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void Draw( CDC* pDC );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  operator ksi_obj();
};
//=====================================================================
class CText: public CVisiblePrim
{
protected:
  CCoordItem  m_Coord;
  CColorItem  m_Color;
  CFontItem   m_Font;
  CStringItem m_String;
  int         m_Ascent;
  int         m_Descent;
  int         m_nLines;
  CRect*      m_pLineRect;

public:
  ~CText() { delete m_pLineRect; }
  CText( ksi_obj init_list ):
    CVisiblePrim( init_list ),
    m_Coord( init_list ),
    m_Color( init_list ),
    m_Font( init_list ),
    m_String( init_list ),
    m_pLineRect( 0 )
    { InitPar(); }
  CText( CCoordItem  const& cdi,
         CColorItem  const& cli,
         CFontItem   const& fnt,
         CStringItem const& str ):
    m_Coord( cdi ),
    m_Color( cli ),
    m_Font( fnt ),
    m_String( str ),
    m_pLineRect( 0 )
    { InitPar(); }
  CText( int x, int y,
         COLORREF color,
         CFontItem const& fi,
         const char* str, int slen, int cx, int cy, int ang ):
    m_Coord( x, y ),
    m_Color( color ),
    m_Font( fi ),
    m_String( str, slen, cx, cy, ang ),
    m_pLineRect( 0 )
    { InitPar(); }
  CText( CText const* p );
  CVisiblePrim* MakeCopy(){ return new CText( this ); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_TEXT; }
  void Draw( CDC* pDC );
  operator ksi_obj();
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void ScaleOn( double cf, CPoint c );
  void GetParams( CVisiblePrim* pPrim );
  void CalcBoundBox( CBox& box );
  int GetAngle( CSpot const& spot, int* pAngle );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void SetCoord( CPoint p ) { m_Coord.x = p.x; m_Coord.y = p.y; }
  CPoint GetCoord() { return m_Coord; }
  operator char const*() { return m_String.m_String; }
  CStringItem* GetStringPtr() { return &m_String; }
  char const* GetText() { return m_String.m_String; }

private:
  void InitPar();
};
//=====================================================================
class CArcPrim: public CRectPrim
{
protected:
  CDeltaItem m_Delta;
public:
  CArcPrim( ksi_obj init_list ):
    CRectPrim( init_list ),
    m_Delta( init_list )
    {
      Rect2Ellipse();
    }
  CArcPrim( CRectPrim const& rp, int delta ):
    CRectPrim( rp ), m_Delta( delta )
    {
      Rect2Ellipse();
    }
  CArcPrim( CArcPrim const* p ):
    CRectPrim( p ),
    m_Delta( p->m_Delta )
    {}
  void SpinOn( CSize const& center, int angle );
  operator ksi_obj();

protected:
  int CenterX() const { return m_Coord.x+(m_Size.cx>>1); }
  int CenterY() const { return m_Coord.y+(m_Size.cy>>1); }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  CPoint Center() const { return CPoint( CenterX(), CenterY()); }
  CPoint* GetPoint( int* pnPnt ) { *pnPnt = 1; return Ellipse2Point(); }
  void ScaleOn( double cf, CPoint c );
  void CalcBndBox( int bw );
};
//=====================================================================
class CArc: public CArcPrim
{
protected:
  CStrokItem m_Strok;
public:
  CArc( ksi_obj init_list ):
    CArcPrim( init_list ),
    m_Strok( init_list )
    {}
  CArc( CRectPrim const& rp, int delta, CStrokItem const& si ):
    CArcPrim( rp, delta ), m_Strok( si )
    {}
  CArc( CArc const *p ):
    CArcPrim( p ),
    m_Strok( p->m_Strok )
    {}
  CVisiblePrim* MakeCopy(){ return new CArc( this ); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_ARC; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void Draw( CDC* pDC );
  void GetParams( CVisiblePrim* pPrim );
  operator ksi_obj();
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class CPie: public CArcPrim
{
protected:
  CFillItem  m_Fill;
public:
  CPie( ksi_obj init_list ):
    CArcPrim( init_list ),
    m_Fill( init_list )
    {}
  CPie( CRectPrim const& rp, int delta, CFillItem const& fi ):
    CArcPrim( rp, delta ), m_Fill( fi )
    {}
  CPie( CPie const *p ):
    CArcPrim( p ),
    m_Fill( p->m_Fill )
    {}
  CVisiblePrim* MakeCopy(){ return new CPie( this ); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_PIE; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void Draw( CDC* pDC );
  void GetParams( CVisiblePrim* pPrim );
  operator ksi_obj();
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class CChord: public CArcPrim
{
protected:
  CFillItem  m_Fill;
public:
  CChord( ksi_obj init_list ):
    CArcPrim( init_list ),
    m_Fill( init_list )
    {}
  CChord( CRectPrim const& rp, int delta, CFillItem const& fi ):
    CArcPrim( rp, delta ), m_Fill( fi )
    {}
  CChord( CChord const* p ):
    CArcPrim( p ),
    m_Fill( p->m_Fill )
    {}
  CVisiblePrim* MakeCopy(){ return new CChord( this ); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_CHORD; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void Draw( CDC* pDC );
  void GetParams( CVisiblePrim* pPrim );
  operator ksi_obj();
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class COutlinedChord: public CChord
{
protected:
  CStrokItem m_Strok;
public:
  COutlinedChord( ksi_obj init_list ):
    CChord( init_list ),
    m_Strok( init_list )
    {}
  COutlinedChord( CRectPrim const& rp, int delta, CFillItem const& fi, CStrokItem const& si ):
    CChord( rp, delta, fi ), m_Strok( si )
    {}
  COutlinedChord( COutlinedChord const* p ):
    CChord( p ),
    m_Strok( p->m_Strok )
    {}
  CVisiblePrim* MakeCopy(){ return new COutlinedChord( this ); }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int GetType() { return KEY_OUTLINED_CHORD; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void ScaleOn( double cf, CPoint c );
  void Draw( CDC* pDC );
  void GetParams( CVisiblePrim* pPrim );
  operator ksi_obj();
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class CIcon: public CVisiblePrim
{
protected:
  CCoordItem m_Coord;
  CRefItem   m_IconRef;
  double m_Cf;

public:
  CIcon( ksi_obj init_list ):
    CVisiblePrim( init_list ),
    m_Coord( init_list ),
    m_IconRef( init_list ),
    m_Cf( 1 )
    {}
  CIcon( CPoint const& p, int id ):
    m_Coord( p ),
    m_IconRef( id ),
    m_Cf( 1 )
  {}
  CIcon( int x, int y, int id ):
    m_Coord( x, y ),
    m_IconRef( id ),
    m_Cf( 1 )
  {}
  CIcon( CIcon const* p ):
    CVisiblePrim( p ),
    m_Coord( p->m_Coord ),
    m_IconRef( p->m_IconRef ),
    m_Cf( p->m_Cf )
  {}
  CVisiblePrim* MakeCopy(){ return new CIcon( this ); }
  operator ksi_obj();
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void ScaleOn( double cf, CPoint c );
  CPoint* GetPoint( int* pnPnt ) { *pnPnt = 1; return &m_Coord; }
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void GetParams( CVisiblePrim* pPrim );
  void Draw( CDC* pDC );
  void CalcBoundBox( CBox& box );
};
//=====================================================================
class CTextByLine: public CVisiblePrim
{
#pragma pack( push, 1 )
public:
  struct TblItem
  {
    CPoint m_Org;
    short  m_Angle;
    char   m_Letter[2];
  };
#pragma pack( pop )

protected:
  CFontItem  m_Font;
  CColorItem m_Color;
  int        m_Length;
  TblItem*   m_pItems;

public:
  ~CTextByLine();
  CTextByLine( ksi_obj init_list );
  CTextByLine( int color, CFontItem& FI, int length, TxtItem* pItem );
  CTextByLine( CTextByLine const* p);
  CVisiblePrim* MakeCopy(){ return new CTextByLine( this ); }
  int GetType() { return KEY_TEXT; }
  operator ksi_obj();
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void ScaleOn( double cf, CPoint c );
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
  int TranslateToMap( TGstream* pDst, int t = 0 );
  void GetParams( CVisiblePrim* pPrim );
  void Draw( CDC* pDC );
  void CalcBoundBox( CBox& box );
};
//=====================================================================
#endif // DRAWPRIM_H
