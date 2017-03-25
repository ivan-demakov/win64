#include "stdafx.h"

#include <string.h>
#include <math.h>
#include "ksi.h"
#include "ksi_type.h"
#include "ksikey.h"
#include "joke.h"
#include "drawprim.h"
#include "drawobj.h"
#include "dwin.h"
#include "mview.h"
#include "mdoc.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "linetmpl.h"
#include "scale.h"
#include "bmpdef.h"
#include "ksiutil.h"
#include "msutil.h"
#include "util.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CStrAssoc const CVisiblePrim::PrimKeys[] =
{
  CStrAssoc( "point",              KEY_POINT ),
  CStrAssoc( "container",          KEY_CONTAINER ),
  CStrAssoc( "rectangle",          KEY_RECTANGLE ),
  CStrAssoc( "ellipse",            KEY_ELLIPSE ),
  CStrAssoc( "ring",               KEY_RING ),
  CStrAssoc( "bar",                KEY_BAR ),
  CStrAssoc( "oval",               KEY_OVAL ),
  CStrAssoc( "circle",             KEY_CIRCLE ),
  CStrAssoc( "outlined-bar",       KEY_OUTLINED_BAR ),
  CStrAssoc( "outlined-oval",      KEY_OUTLINED_OVAL ),
  CStrAssoc( "outlined-circle",    KEY_OUTLINED_CIRCLE ),
  CStrAssoc( "polyline",           KEY_POLYLINE ),
  CStrAssoc( "footnote",           KEY_FOOTNOTE ),
  CStrAssoc( "bound-polyline",     KEY_BOUND_POLYLINE ),
  CStrAssoc( "polygon",            KEY_POLYGON ),
  CStrAssoc( "outlined-polygon",   KEY_OUTLINED_POLYGON ),
  CStrAssoc( "smart-polygon"   ,   KEY_SMART_POLYGON ),
  CStrAssoc( "lines",              KEY_LINES ),
  CStrAssoc( "text",               KEY_TEXT ),
  CStrAssoc( "text-by-line",       KEY_TEXT_BY_LINE ),
  CStrAssoc( "arc",                KEY_ARC ),
  CStrAssoc( "pie",                KEY_PIE ),
  CStrAssoc( "chord",              KEY_CHORD ),
  CStrAssoc( "outlined-chord",     KEY_OUTLINED_CHORD ),
  CStrAssoc( "icon",               KEY_ICON ),
  CStrAssoc( 0,                    0 )
};
//=====================================================================
CVisiblePrim::CVisiblePrim( ksi_obj init_list )
{
  m_Flags =
    ( KSI_TRUE_P( ksi_assv_ref( init_list, keyLVis )) ? SF_LOW_VISIBLE : 0 ) |
    ( KSI_TRUE_P( ksi_assv_ref( init_list, keyHVis )) ? SF_HIG_VISIBLE : 0 ) |
    ( KSI_TRUE_P( ksi_assv_ref( init_list, keySVis )) ? SF_SEL_VISIBLE : 0 );
}
//=====================================================================
CVisiblePrim::operator ksi_obj()
{
  ksi_obj ksi_prim = ksi_nil;
	if( m_Flags & SF_LOW_VISIBLE )
		ksi_prim = ksi_cons( ksi_cons( keyLVis, ksi_true ), ksi_prim );
	if( m_Flags & SF_HIG_VISIBLE )
		ksi_prim = ksi_cons( ksi_cons( keyHVis, ksi_true ), ksi_prim );
	if( m_Flags & SF_SEL_VISIBLE )
		ksi_prim = ksi_cons( ksi_cons( keySVis, ksi_true ), ksi_prim );
  return ksi_prim;
}
//=====================================================================
CPointPrim::operator ksi_obj()
{
  return KSI_CONS3( PrimKeys->KsiTranslate( KEY_POINT ),
                    m_Coord,
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
void
CPointPrim::CalcBoundBox( CBox& box )
{
  box |= m_Coord;
}
//=====================================================================
void
CPointPrim::MoveOn( CSize const& s )
{
  m_Coord += s;
}
//=====================================================================
void
CPointPrim::SpinOn( CSize const& center, int angle )
{
  m_Coord = TurnPoint( m_Coord, center, angle );
}
//=====================================================================
//=====================================================================
CRectPrim::operator ksi_obj()
{
  return KSI_CONS3( m_Coord,
                    m_Size,
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
CRectPrim::operator CRect() const
{
  return CRect( m_Coord, m_Size );
}
//=====================================================================
void
CRectPrim::Rect2Ellipse()
{
  m_Coord -= m_Size;
  m_Size  += m_Size;
}
//=====================================================================
void
CRectPrim::Ellipse2Rect()
{
  m_Size.cx >>= 1;
  m_Size.cy >>= 1;
  m_Coord += m_Size;
}
//=====================================================================
CPoint*
CRectPrim::Ellipse2Point()
{
  static CPoint p;
  p.x = m_Coord.x + m_Size.cx / 2;
  p.y = m_Coord.y + m_Size.cy / 2;
  return &p;
}
//=====================================================================
void
CRectPrim::Ellipse2Ring()
{
  Ellipse2Rect();
  int acx = abs( m_Size.cx ), acy = abs( m_Size.cy );
  if( acx > acy )
    m_Size.cy = m_Size.cy > 0 ? acx : -acx;
  else
    m_Size.cx = m_Size.cx > 0 ? acy : -acy;
  Rect2Ellipse();
}
//=====================================================================
void
CRectPrim::CalcBoundBox( CBox& box )
{
  box |= CBox( *this, m_Coord, m_Size.m_Angle, 1 );
}
//=====================================================================
void
CRectPrim::MoveOn( CSize const& s )
{
  m_Coord += s;
}
//=====================================================================
void
CRectPrim::SpinOn( CSize const& center, int angle )
{
  m_Coord = TurnPoint( m_Coord, center, angle );
  m_Size.Spin( angle );
}
//=====================================================================
void
CRectPrim::ScaleOn( double cf, CPoint c )
{
  if( c.x >= 0 && c.y >= 0 )
	{
		if( m_Size.m_Angle )
		{
		  CPoint c0( m_Coord - c + TurnPoint( m_Size.cx / 2., m_Size.cy / 2., m_Size.m_Angle ));
      c0.x *= cf;
      c0.y *= cf;
      m_Size.ScaleOn( cf );
		  m_Coord = c + c0 + TurnPoint( -m_Size.cx / 2., -m_Size.cy / 2., m_Size.m_Angle );
		}
		else
		{
		  CPoint c0( m_Coord - c + CSize( m_Size.cx / 2, m_Size.cy / 2 ));
      c0.x *= cf;
      c0.y *= cf;
      m_Size.ScaleOn( cf );
		  m_Coord = c + c0 + CSize( -m_Size.cx / 2, -m_Size.cy / 2 );
		}
	}
	else
    m_Size.ScaleOn( cf );
}
//=====================================================================
int
CRectPrim::GetAngle( CSpot const& spot, int* pAngle )
{
  *pAngle = m_Size.m_Angle;
  return 1;
}
//=====================================================================
//=====================================================================
CStrokRectPrim::operator ksi_obj()
{
  ksi_obj s( m_Strok );
  return KSI_CONS3( KSI_CAR( s ),
                    KSI_CDR( s ),
                    CRectPrim::operator ksi_obj());
}
//=====================================================================
void
CStrokRectPrim::CalcBoundBox( CBox& box )
{
  box |= CBox( *this, m_Coord, m_Size.m_Angle, m_Strok.m_Width );
}
//=====================================================================
int
CStrokRectPrim::Detected( CSpot const &spot, int bStrict, int* pDist )
{
  return bStrict ? spot.StrictPolyDetect( Rect2Poly( *this, m_Size.m_Angle ), 5, pDist ) :
                   spot.DetectRect( *this, m_Size.m_Angle, m_Strok.m_Width / 2, pDist );
}
//=====================================================================
void
CStrokRectPrim::ScaleOn( double cf, CPoint c )
{
  CRectPrim::ScaleOn( cf, c );
  m_Strok.ScaleOn( cf );
}
//=====================================================================
//=====================================================================
CFillRectPrim::operator ksi_obj()
{
  ksi_obj f = m_Fill;
  return KSI_CONS3( KSI_CAR( f ),
                    KSI_CDR( f ),
                    CRectPrim::operator ksi_obj());
}
//=====================================================================
int
CFillRectPrim::Detected( CSpot const &spot, int bStrict, int* pDist )
{
  return bStrict ? 
    spot.StrictPolyDetect( Rect2Poly( *this, m_Size.m_Angle ), 5, pDist ) : 
    spot.DetectBar( *this, m_Size.m_Angle, pDist );
}
//=====================================================================
//=====================================================================
COutlinedRectPrim::operator ksi_obj()
{
  ksi_obj f = m_Fill;
  return KSI_CONS3( KSI_CAR( f ),
                    KSI_CDR( f ),
                    CStrokRectPrim::operator ksi_obj());
}
//=====================================================================
int
COutlinedRectPrim::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	return bStrict ? 
    spot.StrictPolyDetect( Rect2Poly( *this, m_Size.m_Angle ), 5, pDist ) :
    spot.DetectBar( *this, m_Size.m_Angle, pDist ) ||  
		spot.DetectRect( *this, m_Size.m_Angle, m_Strok.m_Width / 2, pDist );
}
//=====================================================================
//=====================================================================
CRectangle::operator ksi_obj()
{
  return ksi_cons( PrimKeys->KsiTranslate( KEY_RECTANGLE ),
                   CStrokRectPrim::operator ksi_obj());
}
//=====================================================================
void
CRectangle::Draw( CDC* pDC )
{
  CPen* pOldPen = pDC->SelectObject( m_Strok.CreatePen( m_Flags ));
  CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
  int oldBkMode = pDC->SetBkMode( TRANSPARENT );
  CPoint* rect = Rect2Poly( *this, m_Size.m_Angle );
  pDC->Polyline( rect, 5 );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
}
//=====================================================================
//=====================================================================
void
CEllipse::SpinOn( CSize const& center, int angle )
{
  CSize halfSize( m_Size.cx >> 1, m_Size.cy >> 1 );
  m_Coord = TurnPoint( m_Coord + halfSize, center, angle ) - halfSize;
}
//=====================================================================
CEllipse::operator ksi_obj()
{
  Ellipse2Rect();
  ksi_obj ret = ksi_cons( PrimKeys->KsiTranslate( KEY_ELLIPSE ),
                          CStrokRectPrim::operator ksi_obj());
  Rect2Ellipse();
  return ret;
}
//=====================================================================
void
CEllipse::Draw( CDC* pDC )
{
  CPen* pOldPen = pDC->SelectObject( m_Strok.CreatePen( m_Flags ));
  CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
  int oldBkMode = pDC->SetBkMode( TRANSPARENT );
  pDC->Ellipse( CRect( *this ));
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
}
//=====================================================================
int
CEllipse::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	return spot.DetectEllipse( *this, m_Strok.m_Width / 2, pDist );
}
//=====================================================================
CRing::operator ksi_obj()
{
  Ellipse2Rect();
  ksi_obj ret = ksi_cons( PrimKeys->KsiTranslate( KEY_RING ),
                          CStrokRectPrim::operator ksi_obj());
  Rect2Ellipse();
  return ret;
}
//=====================================================================
CBar::operator ksi_obj()
{
  return ksi_cons( PrimKeys->KsiTranslate( KEY_BAR ),
                   CFillRectPrim::operator ksi_obj());
}
//=====================================================================
void
CBar::Draw( CDC* pDC )
{
  CPoint* poly = Rect2Poly( *this, m_Size.m_Angle );
  int bSel = IsSelected();
  int bSpc = bSel && !CDrawObject::GetDrawnObject()->IsSpecSelected();
  CPen pen( PS_SOLID, 0, GetSpecColor( m_Fill.m_ForeNum, m_Flags ));
  CPen* pOldPen = bSel ? pDC->SelectObject( &pen ) : (CPen*)pDC->SelectStockObject( NULL_PEN );
  CBrush* pOldBrush = bSpc ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
  int oldBkMode = bSel ? pDC->SetBkMode( TRANSPARENT ) : pDC->SetBkMode( m_Fill.GetBkMode());
  COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
  pDC->Polygon( poly, 5 );
  pDC->SetBkColor( oldBkCOlor );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
}
//=====================================================================
//=====================================================================
COval::COval( ksi_obj init_list ):
  CFillRectPrim( init_list )
{
  Rect2Ellipse();
  m_Size.Angle0();
}
//=====================================================================
void
COval::SpinOn( CSize const& center, int angle )
{
  CSize halfSize( m_Size.cx >> 1, m_Size.cy >> 1 );
  m_Coord = TurnPoint( m_Coord + halfSize, center, angle ) - halfSize;
}
//=====================================================================
COval::operator ksi_obj()
{
  Ellipse2Rect();
  ksi_obj ret = ksi_cons( PrimKeys->KsiTranslate( KEY_OVAL ),
                          CFillRectPrim::operator ksi_obj());
  Rect2Ellipse();
  return ret;
}
//=====================================================================
void
COval::Draw( CDC* pDC )
{
  int bSel = IsSelected();
  int bSpc = bSel && !CDrawObject::GetDrawnObject()->IsSpecSelected();
  CPen pen( PS_SOLID, 0, GetSpecColor( m_Fill.m_ForeNum, m_Flags ));
  CPen* pOldPen = bSel ? pDC->SelectObject( &pen ) : (CPen*)pDC->SelectStockObject( NULL_PEN );
  CBrush* pOldBrush = bSpc ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
  int oldBkMode = bSel ? pDC->SetBkMode( TRANSPARENT ) : pDC->SetBkMode( m_Fill.GetBkMode());
  COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
  pDC->Ellipse( CRect( *this ));
  pDC->SetBkColor( oldBkCOlor );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
}
//=====================================================================
int
COval::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	return spot.DetectOval( *this, pDist );
}
//=====================================================================
//=====================================================================
CCircle::operator ksi_obj()
{
  Ellipse2Rect();
  ksi_obj ret = ksi_cons( PrimKeys->KsiTranslate( KEY_CIRCLE ),
                          CFillRectPrim::operator ksi_obj());
  Rect2Ellipse();
  return ret;
}
//=====================================================================
//=====================================================================
COutlinedBar::operator ksi_obj()
{
  return ksi_cons( PrimKeys->KsiTranslate( KEY_OUTLINED_BAR ),
                   COutlinedRectPrim::operator ksi_obj());
}
//=====================================================================
void
COutlinedBar::Draw( CDC* pDC )
{
  CPoint* poly = Rect2Poly( *this, m_Size.m_Angle );
  int bSel = IsSelected();
  int bSpc = bSel && !CDrawObject::GetDrawnObject()->IsSpecSelected();
  CPen* pOldPen = pDC->SelectObject( m_Strok.CreatePen( m_Flags ));
  CBrush* pOldBrush = bSpc ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
  int oldBkMode = bSel ? pDC->SetBkMode( TRANSPARENT ) : pDC->SetBkMode( m_Fill.GetBkMode());
  COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
  pDC->Polygon( poly, 5 );
  pDC->SetBkColor( oldBkCOlor );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
}
//=====================================================================
int
COutlinedBar::Detected( CSpot const &spot, int bStrict, int* pDist )
{
  return bStrict ? spot.StrictPolyDetect( Rect2Poly( *this, m_Size.m_Angle ), 5, pDist ) :
                   spot.DetectBar( *this, m_Size.m_Angle, pDist ) ||
                   spot.DetectRect( *this, m_Size.m_Angle, m_Strok.m_Width / 2, pDist );
}
//=====================================================================
//=====================================================================
void
COutlinedOval::SpinOn( CSize const& center, int angle )
{
  CSize halfSize( m_Size.cx >> 1, m_Size.cy >> 1 );
  m_Coord = TurnPoint( m_Coord + halfSize, center, angle ) - halfSize;
}
//=====================================================================
COutlinedOval::operator ksi_obj()
{
  Ellipse2Rect();
  ksi_obj ret = ksi_cons( PrimKeys->KsiTranslate( KEY_OUTLINED_OVAL ),
                          COutlinedRectPrim::operator ksi_obj());
  Rect2Ellipse();
  return ret;
}
//=====================================================================
void
COutlinedOval::Draw( CDC* pDC )
{

  int bSel = IsSelected();
  int bSpc = bSel && !CDrawObject::GetDrawnObject()->IsSpecSelected();
  CPen* pOldPen = pDC->SelectObject( m_Strok.CreatePen( m_Flags ));
  CBrush* pOldBrush = bSel ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
  int oldBkMode = bSpc ? pDC->SetBkMode( TRANSPARENT ) : pDC->SetBkMode( m_Fill.GetBkMode());
  COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
  pDC->Ellipse( CRect( *this ));
  pDC->SetBkColor( oldBkCOlor );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
 }
//=====================================================================
int
COutlinedOval::Detected( CSpot const &spot, int bStrict, int* pDist )
{
  return spot.DetectOval( *this, pDist ) ||
         spot.DetectEllipse( *this, m_Strok.m_Width / 2, pDist );
}
//=====================================================================
//=====================================================================
COutlinedCircle::operator ksi_obj()
{
  Ellipse2Rect();
  ksi_obj ret = ksi_cons( PrimKeys->KsiTranslate( KEY_OUTLINED_CIRCLE ),
                          COutlinedRectPrim::operator ksi_obj());
  Rect2Ellipse();
  return ret;
}
//=====================================================================
//=====================================================================
CPolyline::operator ksi_obj()
{
  ksi_obj s( m_Strok );
  return KSI_CONS5( PrimKeys->KsiTranslate( m_Poly.m_PointNum == m_Poly.ExtPointNum() ?
                                            KEY_POLYLINE : KEY_BOUND_POLYLINE ),
                    KSI_CDR( s ),
                    KSI_CAR( s ),
                    m_Poly, 
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
CFootnote::operator ksi_obj()
{
  ksi_obj s( m_Strok );
  return KSI_CONS5( PrimKeys->KsiTranslate( KEY_FOOTNOTE ),
                    KSI_CDR( s ),
                    KSI_CAR( s ),
                    m_Poly, 
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
void
CPolyline::ScaleOn( double cf, CPoint c )
{
  m_Strok.ScaleOn( cf );
  m_Poly.ScaleOn( cf, c );
}
//=====================================================================
void
CPolyline::InflateClipSize( CSize& size )
{
  CLineTmpl* pTempl = CLineTmpl::GetTemplate( m_Strok.TemlateStyle());
  if( pTempl )
    pTempl->InflateClipSize( size );
}
//=====================================================================
void
CPolyline::Draw( CDC* pDC )
{
  if( !m_Poly.m_PointNum )
    return;

  CLineTmpl* pTempl = CLineTmpl::GetTemplate( m_Strok.TemlateStyle());
  if( pTempl )
  {
    int mi, ma;
    pTempl->GetRange( &mi, &ma );
    int D = ma - mi;
    CSize d( D, 0 );
    pDC->LPtoDP( &d );
    if( !pDC->IsPrinting() && d.cx == 0 )
    {
      pTempl = 0;
      m_Strok.m_Width = D;
    }
  }

  int bBlackDashed = !( m_Flags & SF_STATUS_MASK ) && 
                     m_Strok.m_Width > 0 && m_Strok.m_Style > PS_SOLID && m_Strok.m_Style <= PS_DASHDOTDOT;
  DWORD nStyle = m_Strok.m_Style;
  if( bBlackDashed )
    m_Strok.m_Style = PS_SOLID;

  CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
  int oldBkMode = pDC->SetBkMode( TRANSPARENT );
  CPen* pOldPen = pDC->SelectObject( m_Strok.CreatePen( m_Flags ));
  if( pTempl && !( pView->GetMode( IDM_OBJEDIT_MODE ) && IsSelected()))
    pTempl->Draw( pDC, m_Poly.m_pPoints, m_Poly.ExtPointNum());
  else
    pDC->Polyline( m_Poly.m_pPoints, m_Poly.ExtPointNum());
  pDC->SelectObject( pOldPen );

  if( bBlackDashed )
  {
    CPen pen( nStyle, 0, RGB( 0, 0, 0 ));
    CPen* pOldPen = pDC->SelectObject( &pen );
    pDC->Polyline( m_Poly.m_pPoints, m_Poly.ExtPointNum());
    pDC->SelectObject( pOldPen );
    m_Strok.m_Style = nStyle;
  }

  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldBrush );
  
  if( CDrawObject::GetDrawnObject()->GetAnyPoints() == m_Poly.m_pPoints )
    m_Poly.DrawVerts( pDC, SelectFlag());
}
//=====================================================================
void
CFootnote::Draw( CDC* pDC )
{
  if( !pDocument->GetObjDef()->GetTextInvisible( CDrawObject::GetDrawnClass()))
    CPolyline::Draw( pDC );
}
//=====================================================================
int
CPolyline::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	if( spot.StrictPolyDetect( m_Poly.m_pPoints, m_Poly.ExtPointNum(), pDist ))
    return 1;
  
	if( bStrict )
    return 0;

  int wl = 0, wh = 0;
  CLineTmpl* pTempl = CLineTmpl::GetTemplate( m_Strok.TemlateStyle());

  if( pTempl )
    pTempl->GetRange( &wl, &wh );

  wh += m_Strok.m_Width + 1 >> 1;
  wl -= m_Strok.m_Width >> 1;

	return spot.DetectPolyline( m_Poly.m_pPoints, m_Poly.ExtPointNum(), wl, wh, pDist );
}
//=====================================================================
int
CPolyline::GetAngle( CSpot const &spot, int* pAngle )
{
  return m_Poly.GetAngle( spot, pAngle );
}
//=====================================================================
void
CPolyline::CalcBoundBox( CBox& box )
{
  box |= CBox( m_Poly.m_pPoints, m_Poly.m_PointNum,
               m_Strok.TemlateStyle(), m_Strok.m_Width );
}
//=====================================================================
void
CPolyline::MoveOn( CSize const& s )
{
  m_Poly.MoveOn( s );
}
//=====================================================================
void
CPolyline::SpinOn( CSize const& center, int angle )
{
  m_Poly.SpinOn( center, angle );
}
//=====================================================================
//=====================================================================
CPolygon::operator ksi_obj()
{
  ksi_obj f = m_Fill;
  return KSI_CONS5( PrimKeys->KsiTranslate( KEY_POLYGON ),
                    KSI_CAR( f ),
                    KSI_CDR( f ),
                    m_Poly, 
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
void
CPolygon::ScaleOn( double cf, CPoint c )
{
  m_Poly.ScaleOn( cf, c );
}
//=====================================================================
void
CPolygon::Draw( CDC* pDC )
{
  if( !m_Poly.m_PointNum )
    return;

  int bSel = IsSelected();
  int bSpc = m_Fill.m_ForeNum == -1 || bSel && !CDrawObject::GetDrawnObject()->IsSpecSelected();
  CPen pen( PS_SOLID, 0, GetSpecColor( m_Fill.m_ForeNum, m_Flags ));
  CPen* pOldPen = bSel ? pDC->SelectObject( &pen ) : (CPen*)pDC->SelectStockObject( NULL_PEN );
  CBrush* pOldBrush = bSpc ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
  COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
  int oldBkMode = bSel ? pDC->SetBkMode( TRANSPARENT ) : pDC->SetBkMode( m_Fill.GetBkMode());
  pDC->Polygon( m_Poly.m_pPoints, m_Poly.m_PointNum );
  if( CDrawObject::GetDrawnObject()->GetAnyPoints() == m_Poly.m_pPoints )
    m_Poly.DrawVerts( pDC, SelectFlag());
  pDC->SetBkColor( oldBkCOlor );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
 }
//=====================================================================
int
CPolygon::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	return bStrict ? 
    spot.StrictPolyDetect( m_Poly.m_pPoints, m_Poly.ExtPointNum(), pDist ) : 
    spot.DetectPolygon( m_Poly.m_pPoints, m_Poly.ExtPointNum(), pDist );
}
//=====================================================================
int
CPolygon::GetAngle( CSpot const &spot, int* pAngle )
{
  return m_Poly.GetAngle( spot, pAngle );
}
//=====================================================================
void
CPolygon::CalcBoundBox( CBox& box )
{
  box |= CBox( m_Poly.m_pPoints, m_Poly.m_PointNum );
}
//=====================================================================
void
CPolygon::MoveOn( CSize const& s )
{
  m_Poly.MoveOn( s );
}
//=====================================================================
void
CPolygon::SpinOn( CSize const& center, int angle )
{
  m_Poly.SpinOn( center, angle );
}
//=====================================================================
//=====================================================================
COutlinedPolygon::operator ksi_obj()
{
  ksi_obj s = m_Strok;
  ksi_obj f = m_Fill;
  return KSI_CONS7( PrimKeys->KsiTranslate( KEY_OUTLINED_POLYGON ),
                    KSI_CDR( s ),
                    KSI_CAR( s ),
                    KSI_CDR( f ),
                    KSI_CAR( f ),
                    m_Poly, 
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
CSmartPolygon::operator ksi_obj()
{
  ksi_obj s = m_Strok;
  ksi_obj f = m_Fill;
  return KSI_CONS7( PrimKeys->KsiTranslate( KEY_SMART_POLYGON ),
                    KSI_CDR( s ),
                    KSI_CAR( s ),
                    KSI_CDR( f ),
                    KSI_CAR( f ),
                    m_Poly, 
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
void
CSmartPolygon::CreateSmartPoly()
{
  LItemHash1 lh( m_Poly.m_pPoints, m_Poly.m_PointNum );
  if( lh.RemDup())
  {
    m_SmartPoly.m_PointNum = lh.CalcLines() * 2;
    m_SmartPoly.m_pPoints = new CPoint[m_SmartPoly.m_PointNum];
    lh.CopyLines( m_SmartPoly.m_pPoints );
  }
}
//=====================================================================
void
CSmartPolygon::MoveOn( CSize const& s )
{
  COutlinedPolygon::MoveOn( s );
  m_SmartPoly.MoveOn( s );

}
//=====================================================================
void
CSmartPolygon::SpinOn( CSize const& center, int angle )
{
  COutlinedPolygon::SpinOn( center, angle );
  m_SmartPoly.SpinOn( center, angle );
}
//=====================================================================
void
CSmartPolygon::Draw( CDC* pDC )
{
  if( !m_Poly.m_PointNum )
    return;

  if( IsSelected() && !CDrawObject::GetDrawnObject()->IsSpecSelected())
    CPolyline::Draw( pDC );
  else
  if( !m_SmartPoly.m_pPoints )
    COutlinedPolygon::Draw( pDC );
  else
  {
    CBrush* pOldBrush = m_Fill.m_ForeNum == -1 ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
    CPen* pOldPen = (CPen*)pDC->SelectStockObject( NULL_PEN );
    COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
    int oldBkMode = pDC->SetBkMode( m_Fill.GetBkMode());
    pDC->Polygon( m_Poly.m_pPoints, m_Poly.m_PointNum );
    pDC->SetBkMode( oldBkMode );
    pDC->SelectObject( pOldPen );
    pDC->SelectObject( pOldBrush );
    pDC->SetBkColor( oldBkCOlor );
    CLineTmpl* pTempl = CLineTmpl::GetTemplate( m_Strok.TemlateStyle());
    if( !pDC->IsPrinting() && pTempl )
    {
      int mi, ma;
      pTempl->GetRange( &mi, &ma );
      CSize d( ma - mi, 0 );
      pDC->LPtoDP( &d );
      if( d.cx < 1 )
        pTempl = 0;
    }
    pOldPen = pDC->SelectObject( m_Strok.CreatePen( m_Flags ));
    oldBkMode = pDC->SetBkMode( TRANSPARENT );

    CPoint* p = m_SmartPoly.m_pPoints;
    for( int i = m_SmartPoly.m_PointNum / 2 ; --i >= 0 ; p += 2 )
      if( pTempl )
        pTempl->Draw( pDC, p, 2 );
      else
      {
        pDC->MoveTo( p[0] );
        pDC->LineTo( p[1] );
      }

    pDC->SetBkMode( oldBkMode );
    pDC->SelectObject( pOldPen );
  }
}
//=====================================================================
void
COutlinedPolygon::Draw( CDC* pDC )
{
  if( !m_Poly.m_PointNum )
    return;

  if( m_Fill.m_ForeNum >= 0 && ( !IsSelected() || CDrawObject::GetDrawnObject()->IsSpecSelected()))
  {
    CBrush* pOldBrush = pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
    CPen* pOldPen = (CPen*)pDC->SelectStockObject( NULL_PEN );
    int oldBkMode = pDC->SetBkMode( m_Fill.GetBkMode());
    COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
    pDC->Polygon( m_Poly.m_pPoints, m_Poly.m_PointNum );
    pDC->SetBkColor( oldBkCOlor );
    pDC->SetBkMode( oldBkMode );
    pDC->SelectObject( pOldPen );
    pDC->SelectObject( pOldBrush );
  }

  CPolyline::Draw( pDC );
}
//=====================================================================
int
COutlinedPolygon::Detected( CSpot const &spot, int bStrict, int* pDist )
{
  return bStrict ? 
    spot.StrictPolyDetect( m_Poly.m_pPoints, m_Poly.ExtPointNum(), pDist) :
    spot.DetectPolygon( m_Poly.m_pPoints, m_Poly.ExtPointNum(), pDist ) ||
	  spot.DetectPolyline( m_Poly.m_pPoints, m_Poly.ExtPointNum(), m_Strok.m_Width / 2, m_Strok.m_Width / 2, pDist );
}
//=====================================================================
//=====================================================================
void
CLines::Draw( CDC* pDC )
{
  if( !m_Poly.m_PointNum )
    return;

  CLineTmpl* pTempl = CLineTmpl::GetTemplate( m_Strok.TemlateStyle());
  if( pTempl )
  {
    int mi, ma;
    pTempl->GetRange( &mi, &ma );
    CSize d( ma - mi, 0 );
    pDC->LPtoDP( &d );
    if( !pDC->IsPrinting() && d.cx < 1 )
      pTempl = 0;
  }

  CPen* pOldPen = pDC->SelectObject( m_Strok.CreatePen( m_Flags ));
  int oldBkMode = pDC->SetBkMode( TRANSPARENT );

  CPoint* p = m_Poly.m_pPoints;
  for( int i = m_Poly.m_PointNum / 2 ; --i >= 0 ; p += 2 )
    if( pTempl )
      pTempl->Draw( pDC, p, 2 );
    else
    {
      pDC->MoveTo( p[0] );
      pDC->LineTo( p[1] );
    }

  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
}
//=====================================================================
int
CLines::Detected( CSpot const &spot, int bStrict, int* pDist )
{
  if( !m_Poly.m_PointNum )
    return 0;

  int wl = 0, wh = 0;
  CLineTmpl* pTempl = CLineTmpl::GetTemplate( m_Strok.TemlateStyle());

  if( pTempl )
    pTempl->GetRange( &wl, &wh );

  wh += m_Strok.m_Width + 1 >> 1;
  wl -= m_Strok.m_Width >> 1;

  int d = MAX_INT;
	int r = 0;

	for( int i = 0 ; i < m_Poly.m_PointNum && d ; i += 2 )
		r |= spot.DetectLine( &m_Poly.m_pPoints[i], wl, wh, &d );

	if( !pDist )
  	return r;  
		
	if( *pDist <= d )
  	return 0;  

	*pDist = d;
 	return r;  
}
//=====================================================================
using namespace Trig;
int
CLines::GetAngle( CSpot const &spot, int* pAngle )
{
  CPoint const* p = m_Poly.m_pPoints;
  for( int i = m_Poly.m_PointNum / 2 ; --i >= 0 ; p += 2 )
    if( spot.DetectLine( p, 0, 1 ))
    {
      *pAngle = atan2(((CRect*)p)->Size());
      return 1;
    }
  return 0;
}
//=====================================================================
CLines::operator ksi_obj()
{
  ksi_obj s( m_Strok );
  return KSI_CONS5( PrimKeys->KsiTranslate( KEY_LINES ),
                    KSI_CDR( s ),
                    KSI_CAR( s ),
                    m_Poly, 
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
CLines::CLines( ksi_obj init_list ) :
  CPolyline( init_list )
{}
//=====================================================================
//=====================================================================
CText::operator ksi_obj()
{
  return KSI_CONS6( PrimKeys->KsiTranslate( KEY_TEXT ),
                    m_String,
                    m_Font,
                    m_Color,
                    m_Coord, 
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
CText::CText( CText const* p ):
  CVisiblePrim( p ),
  m_Coord( p->m_Coord ),
  m_Color( p->m_Color ),
  m_Font( p->m_Font ),
  m_String( p->m_String ),
  m_Ascent( p->m_Ascent ),
  m_Descent( p->m_Descent ),
  m_nLines( p->m_nLines )
{
  m_pLineRect = new CRect[m_nLines];
  memcpy( m_pLineRect, p->m_pLineRect, m_nLines * sizeof( m_pLineRect[0] ));
}
//=====================================================================
void
CText::Draw( CDC* pDC )
{
  if( pDocument->GetObjDef()->GetTextInvisible( CDrawObject::GetDrawnClass()))
    return;

	if( !m_String.m_String[0] )
    return;

	CRect t( m_pLineRect[0] );
	int h = t.Height();
	int s = pView->GetClipSize().cy;

	if( h > s )
    return;	
	
	pDC->LPtoDP( &t );
	if( t.Height() < 4 )
    return;

  COLORREF clr = CDrawObject::GetDrawnObject()->IsSpecSelected() ?
                 GetColor( 0 ) :
                 GetSpecColor( m_Color.m_ColorNum, m_Flags );

  CFont* pFont = m_Font.CreateFont( m_String.m_Angle );
  if( pFont )
  {
    COLORREF otc = pDC->SetTextColor( clr );
    int oldBkMode = pDC->SetBkMode( TRANSPARENT );
    CFont* pOldFont = pDC->SelectObject( pFont );

    int i = 0;
    pDC->SetTextAlign( TA_LEFT | TA_BASELINE | TA_NOUPDATECP );
    for( char* p0 = m_String.m_String, *p1 = p0 ; *p1 ; p0 = p1 + 1, ++i )
    {
      ( p1 = strchr( p0, '\n' )) || ( p1 = strchr( p0, '\0' ));
			int tl = p1 - p0;
			if( pView->DrawTextShadow())
			{
        CSize ds = pView->DP2LP( CSize( 1, 1 ));
			  pDC->SetTextColor( 0xffffff );
				for( int sx = -1 ; sx <= 1 ; sx += 2 ) 
				  for( int sy = -1 ; sy <= 1 ; sy += 2 ) 
			      pDC->TextOut( m_pLineRect[i].left+ds.cx*sx, m_pLineRect[i].top+ds.cy*sy, p0, tl );
			}
			pDC->SetTextColor( clr );      
			pDC->TextOut( m_pLineRect[i].left, m_pLineRect[i].top, p0, tl );
    }
    pDC->SelectObject( pOldFont );
    pDC->SetBkMode( oldBkMode );
    pDC->SetTextColor( otc );
  }
}
//=====================================================================
int
CText::Detected( CSpot const& spot, int bStrict, int* pDist )
{
  if( bStrict && !IsAnyVisible() ||
      pDocument->GetObjDef()->GetTextInvisible( CDrawObject::GetDrawnClass()))
    return 0;

  for( int i = m_nLines ; --i >= 0 ; )
	{
		CRect b( m_pLineRect[i] );
    CPoint c( b.TopLeft());
		CPoint t( TurnPoint( spot, c, -m_String.m_Angle ));
		b.OffsetRect( 0, -m_Ascent );
    CBox box( b, c, 0 );
		if( CSpot( t, spot.GetRadius()).DetectBar( box, 0, pDist ))
			return 1;
	}
	
	return 0;	
}
//=====================================================================
int
CText::GetAngle( CSpot const& spot, int* pAngle )
{
  *pAngle = m_String.m_Angle;
  return 1;
}
//=====================================================================
void
CText::InitPar()
{
  m_Ascent  = 0;
  m_Descent = 0;
  m_nLines = 0;
  char* p0 = m_String.m_String;
  char* p1;
  for( p1 = p0 ; *p1 ; p0 = p1 + 1, ++m_nLines )
    ( p1 = strchr( p0, '\n' )) || ( p1 = strchr( p0, '\0' ));

  delete m_pLineRect;
  m_pLineRect = m_nLines ? new CRect[m_nLines] : 0;
  if( !m_pLineRect )
    return;

  int height = m_Font.m_Height;
  int h = abs( height ), dy = MulDiv( h, 17, 16 );
  int M = 4096;
  int a = m_String.m_Angle;

  if( h < M )
    M = h;
  else
    m_Font.m_Height = height > 0 ? M : -M;

  CFont* pFont = m_Font.CreateFont( a );
  if( !pFont )
    return;

  m_Font.m_Height = height;

  CClientDC dc( 0 );
  dc.SelectObject( pFont );

  TEXTMETRIC tm;
  if( dc.GetOutputTextMetrics( &tm ))
  {
    m_Ascent  = MulDiv( tm.tmAscent,  h, M );
    m_Descent = MulDiv( tm.tmDescent, h, M );

    CSize dd( dy * sin( a ), dy * cos( a ));
    CPoint tp( m_Coord + TurnPoint( m_String.m_Offset, a ));

    int i = 0;
    for( p0 = m_String.m_String, p1 = p0 ; *p1 ; p0 = p1 + 1, ++i )
    {
      ( p1 = strchr( p0, '\n' )) || ( p1 = strchr( p0, '\0' ));
      CSize ext( dc.GetOutputTextExtent( p0, p1 - p0 ));
      CSize dex( dc.GetOutputTextExtent( " ", 1 ));
      CRect b( 0, 0, MulDiv( ext.cx + dex.cx, h, M ), MulDiv( ext.cy, h, M ));
      b.OffsetRect( tp );
      m_pLineRect[i] = b;
      tp += dd;
    }
  }
}
//=====================================================================
void
CText::CalcBoundBox( CBox& box )
{
  for( int i = m_nLines ; --i >= 0 ; )
  {
    CRect b( m_pLineRect[i] );
    CPoint c( b.TopLeft());
		b.OffsetRect( 0, -m_Ascent );
    box |= CBox( b, c, m_String.m_Angle );
  }
}
//=====================================================================
void
CText::MoveOn( CSize const& s )
{
  m_Coord += s;
  InitPar();
}
//=====================================================================
void
CText::SpinOn( CSize const& center, int angle )
{
  m_Coord = TurnPoint( m_Coord, center, angle );
  m_String.m_Angle += angle;
  InitPar();
}
//=====================================================================
void
CText::ScaleOn( double cf, CPoint c )
{
  m_Font.ScaleOn( cf );
  m_String.ScaleOn( cf );
	m_Coord.ScaleOn( cf, c );
  InitPar();
}
//=====================================================================
//=====================================================================
void
CArcPrim::SpinOn( CSize const& center, int angle )
{
  CSize halfSize( m_Size.cx >> 1, m_Size.cy >> 1 );
  m_Coord = TurnPoint( m_Coord + halfSize, center, angle ) - halfSize;
  m_Size.Spin( angle );
}
//=====================================================================
void
CArcPrim::ScaleOn( double cf, CPoint c )
{
  m_Size.ScaleOn( cf );
  m_Coord.ScaleOn( cf, c );
}
//=====================================================================
void
COutlinedChord::ScaleOn( double cf, CPoint c )
{
	CArcPrim::ScaleOn( cf, c );
  m_Strok.ScaleOn( cf );
}
//=====================================================================
CArcPrim::operator ksi_obj()
{
  Ellipse2Rect();
  ksi_obj ret = ksi_cons( m_Delta.operator ksi_obj(),
                          CRectPrim::operator ksi_obj());
  Rect2Ellipse();
  return ret;
}
//=====================================================================
//=====================================================================
CArc::operator ksi_obj()
{
  ksi_obj s = m_Strok;
  return KSI_CONS4( PrimKeys->KsiTranslate( KEY_ARC ),
                    KSI_CAR( s ),
                    KSI_CDR( s ),
                    CArcPrim::operator ksi_obj());
}
//=====================================================================
void
CArc::Draw( CDC* pDC )
{
  CPen* pOldPen = pDC->SelectObject( m_Strok.CreatePen( m_Flags ));
  int oldBkMode = pDC->SetBkMode( TRANSPARENT );
  CPoint start = Center(), finish = start;
  int a = m_Size.m_Angle, d = m_Delta.m_Delta;
  if( d < 0 )
    a += d, d = -d;
  d += a;
  start.Offset( 1024 * cos( a ), -1024 * sin( a ));
  finish.Offset( 1024 * cos( d ), -1024 * sin( d ));
  CRect rect( *this );
  pDC->Arc( &rect, start, finish );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
}
//=====================================================================
int
CArc::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	return spot.DetectArc( *this, m_Size.m_Angle, m_Delta.m_Delta, m_Strok.m_Width / 2, pDist );
}
//=====================================================================
void
CArc::CalcBoundBox( CBox& box )
{
  box |= CBox( m_Coord, CSize( m_Size.cx, m_Size.cy ),
               m_Size.m_Angle, m_Delta.m_Delta, m_Strok.m_Width );
}
//=====================================================================
//=====================================================================
CPie::operator ksi_obj()
{
  ksi_obj f = m_Fill;
  return KSI_CONS4( PrimKeys->KsiTranslate( KEY_PIE ),
                    KSI_CAR( f ),
                    KSI_CDR( f ),
                    CArcPrim::operator ksi_obj());
}
//=====================================================================
void
CPie::Draw( CDC* pDC )
{
  CPoint start = Center(), finish = start;
  int a = m_Size.m_Angle, d = m_Delta.m_Delta;

  if( d < 0 )
    a += d, d = -d;
  d += a;

  start.Offset( 1024 * cos( a ), -1024 * sin( a ));
  finish.Offset( 1024 * cos( d ), -1024 * sin( d ));

  int bSel = IsSelected();
  int bSpc = bSel && !CDrawObject::GetDrawnObject()->IsSpecSelected();
  CPen pen( PS_SOLID, 0, GetSpecColor( m_Fill.m_ForeNum, m_Flags ));
  CPen* pOldPen = bSpc ? pDC->SelectObject( &pen ) : (CPen*)pDC->SelectStockObject( NULL_PEN );
  CBrush* pOldBrush = bSel ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
  int oldBkMode = bSel ? pDC->SetBkMode( TRANSPARENT ) : pDC->SetBkMode( m_Fill.GetBkMode());
  COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
	pDC->Pie( CRect( *this ), start, finish );
  pDC->SetBkColor( oldBkCOlor );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
}
//=====================================================================
int
CPie::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	return spot.DetectPie( *this, m_Size.m_Angle, m_Delta.m_Delta, pDist );
}
//=====================================================================
void
CPie::CalcBoundBox( CBox& box )
{
  box |= CBox( m_Coord, CSize( m_Size.cx, m_Size.cy ),
               m_Size.m_Angle, m_Delta.m_Delta, 0, 1 );
}
//=====================================================================
//=====================================================================
CChord::operator ksi_obj()
{
  ksi_obj f = m_Fill;
  return KSI_CONS4( PrimKeys->KsiTranslate( KEY_CHORD ),
                    KSI_CAR( f ),
                    KSI_CDR( f ),
                    CArcPrim::operator ksi_obj());
}
//=====================================================================
COutlinedChord::operator ksi_obj()
{
  ksi_obj f = m_Fill;
  return KSI_CONS5( PrimKeys->KsiTranslate( KEY_CHORD ),
		                m_Strok,
                    KSI_CAR( f ),
                    KSI_CDR( f ),
                    CArcPrim::operator ksi_obj());
}
//=====================================================================
void
CChord::Draw( CDC* pDC )
{
  CPoint start = Center(), finish = start;
  int a = m_Size.m_Angle, d = m_Delta.m_Delta;

  if( d < 0 )
    a += d, d = -d;
  d += a;

  start.Offset( 1024 * cos( a ), -1024 * sin( a ));
  finish.Offset( 1024 * cos( d ), -1024 * sin( d ));

  int bSel = IsSelected();
  int bSpc = bSel && !CDrawObject::GetDrawnObject()->IsSpecSelected();
  CPen pen( PS_SOLID, 0, GetSpecColor( m_Fill.m_ForeNum, m_Flags ));
  CPen* pOldPen = bSpc ? pDC->SelectObject( &pen ) : (CPen*)pDC->SelectStockObject( NULL_PEN );
  CBrush* pOldBrush = bSel ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
  int oldBkMode = bSel ? pDC->SetBkMode( TRANSPARENT ) : pDC->SetBkMode( m_Fill.GetBkMode());
  COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
  pDC->Chord( CRect( *this ), start, finish );
  pDC->SetBkColor( oldBkCOlor );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
}
//=====================================================================
void
COutlinedChord::Draw( CDC* pDC )
{
  CPoint start = Center(), finish = start;
  int a = m_Size.m_Angle, d = m_Delta.m_Delta;

  if( d < 0 )
    a += d, d = -d;
  d += a;

  start.Offset( 1024 * cos( a ), -1024 * sin( a ));
  finish.Offset( 1024 * cos( d ), -1024 * sin( d ));

  int bSel = IsSelected();
  int bSpc = bSel && !CDrawObject::GetDrawnObject()->IsSpecSelected();
  CPen pen( PS_SOLID, bSpc ? 0 : m_Strok.m_Width, GetSpecColor( m_Strok.m_ColorNum, m_Flags ));
  CPen* pOldPen = pDC->SelectObject( &pen );
  CBrush* pOldBrush = bSel ? (CBrush*)pDC->SelectStockObject( NULL_BRUSH ) : pDC->SelectObject( m_Fill.CreateBrush( m_Flags ));
  int oldBkMode = bSel ? pDC->SetBkMode( TRANSPARENT ) : pDC->SetBkMode( m_Fill.GetBkMode());
  COLORREF oldBkCOlor = pDC->SetBkColor( GetColor( m_Fill.m_BackNum ));
  pDC->Chord( CRect( *this ), start, finish );
  pDC->SetBkColor( oldBkCOlor );
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldPen );
  pDC->SelectObject( pOldBrush );
}
//=====================================================================
int
CChord::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	return spot.DetectChord( *this, m_Size.m_Angle, m_Delta.m_Delta, pDist );
}
//=====================================================================
int
COutlinedChord::Detected( CSpot const &spot, int bStrict, int* pDist )
{
	return spot.DetectChord( *this, m_Size.m_Angle, m_Delta.m_Delta, pDist ) ||
		     spot.DetectChord( *this, m_Size.m_Angle, m_Delta.m_Delta, m_Strok.m_Width / 2, pDist );
}
//=====================================================================
void
CChord::CalcBoundBox( CBox& box )
{
  box |= CBox( m_Coord, CSize( m_Size.cx, m_Size.cy ),
               m_Size.m_Angle, m_Delta.m_Delta, 0 );
}
//=====================================================================
void
COutlinedChord::CalcBoundBox( CBox& box )
{
	CBox t( m_Coord, CSize( m_Size.cx, m_Size.cy ), m_Size.m_Angle, m_Delta.m_Delta, 0 );
	int d = m_Strok.m_Width / 2;
	t.InflateRect( d, d );
  box |= t;
}
//=====================================================================
//=====================================================================
CIcon::operator ksi_obj()
{
  return KSI_CONS4( PrimKeys->KsiTranslate( KEY_ICON ),
                    m_Coord,
                    m_IconRef, 
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
void
CIcon::MoveOn( CSize const& s )
{
  m_Coord += s;
}
//=====================================================================
void
CIcon::SpinOn( CSize const& center, int angle )
{
  m_Coord = TurnPoint( m_Coord, center, angle );
}
//=====================================================================
void
CIcon::ScaleOn( double cf, CPoint c )
{
  m_Cf *= cf; 
}
//=====================================================================
void
CIcon::CalcBoundBox( CBox& box )
{
  box |= CBox( m_IconRef.m_Reference, m_Coord, m_Cf );
}
//=====================================================================
int
CIcon::Detected( CSpot const& spot, int bStrict, int* pDist )
{
	return spot.Inflate( CBox( m_IconRef.m_Reference, m_Coord, m_Cf )).PtInRect( spot.CenterPoint());
}
//=====================================================================
void
CIcon::GetParams( CVisiblePrim* pPrim )
{
  m_IconRef = ((CIcon*)pPrim)->m_IconRef;
}
//=====================================================================
void
CIcon::Draw( CDC* pDC )
{
  CBmpDef* pBD = pDocument->GetBmpDef( m_IconRef.m_Reference );
	if( pBD )
  {
    CBox box( m_IconRef.m_Reference, m_Coord, m_Cf );
    CRect scb;

    if( !pDC->IsPrinting())
    {
      pDC->GetClipBox( &scb );
      CRect ncb( scb );
      ncb |= box;
      pDC->SelectClipRgn( 0 );
      pDC->IntersectClipRect( &ncb );
    }

    pBD->Draw( pDC, m_Coord, pView->GetAltScale() / m_Cf );
    if( IsSelected())
    {
      CSize w( 0, 2 );
      pDC->DPtoLP( &w );
      CPen pen( PS_SOLID, w.cy, GetSpecColor( 0, m_Flags ));
      CPen* pOldPen = pDC->SelectObject( &pen );
      CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
      pDC->Rectangle( &box );
      pDC->SelectObject( pOldBrush );
      pDC->SelectObject( pOldPen );
    }

    if( !pDC->IsPrinting())
      pDC->IntersectClipRect( &scb );
  }
}
//=====================================================================
//=====================================================================
void
CStrokRectPrim::GetParams( CVisiblePrim* pPrim )
{
  m_Strok = ((CStrokRectPrim*)pPrim)->m_Strok;
}
//=====================================================================
void
CFillRectPrim::GetParams( CVisiblePrim* pPrim )
{
  m_Fill = ((CFillRectPrim*)pPrim)->m_Fill;
}
//=====================================================================
void
COutlinedRectPrim::GetParams( CVisiblePrim* pPrim )
{
  CStrokRectPrim::GetParams( pPrim );
  m_Fill = ((COutlinedRectPrim*)pPrim)->m_Fill;
}
//=====================================================================
void
CPolyline::GetParams( CVisiblePrim* pPrim )
{
  m_Strok = ((CPolyline*)pPrim)->m_Strok;
}
//=====================================================================
void
CPolygon::GetParams( CVisiblePrim* pPrim )
{
  m_Fill = ((CPolygon*)pPrim)->m_Fill;
}
//=====================================================================
void
COutlinedPolygon::GetParams( CVisiblePrim* pPrim )
{
  m_Strok = ((COutlinedPolygon*)pPrim)->m_Strok;
  m_Fill = ((COutlinedPolygon*)pPrim)->m_Fill;
}
//=====================================================================
void
CText::GetParams( CVisiblePrim* pPrim )
{
  m_Color = ((CText*)pPrim)->m_Color;
  m_Font  = ((CText*)pPrim)->m_Font;
  m_Flags = ((CText*)pPrim)->m_Flags;
}
//=====================================================================
void
CArc::GetParams( CVisiblePrim* pPrim )
{
  m_Strok = ((CArc*)pPrim)->m_Strok;
}
//=====================================================================
void
CPie::GetParams( CVisiblePrim* pPrim )
{
  m_Fill = ((CPie*)pPrim)->m_Fill;
}
//=====================================================================
void
CChord::GetParams( CVisiblePrim* pPrim )
{
  m_Fill = ((CChord*)pPrim)->m_Fill;
}
//=====================================================================
void
COutlinedChord::GetParams( CVisiblePrim* pPrim )
{
	CChord::GetParams( pPrim );
  m_Strok = ((COutlinedChord*)pPrim)->m_Strok;
}
//=====================================================================
int
CVisiblePrim::TranslateToMap( TGstream* pDst, int type )
{
  PrimRecord rec;
  int bCont = type == PF_CONTAINER;
  rec.type = (  bCont || IsLowVisible() ? PF_LVISIBLE : 0 ) |
             (  bCont || IsHigVisible() ? PF_HVISIBLE : 0 ) |
             ( !bCont && IsSelVisible() ? PF_SVISIBLE : 0 ) | type;
  return WriteData( &rec.type, sizeof rec.type, pDst );
}
//=====================================================================
int
CPointPrim::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  int r = CVisiblePrim::TranslateToMap( pDst, PF_POINT | ShapeBit );
  return r == R_OK ? WriteData( &m_Coord, sizeof m_Coord, pDst ) : r;
}
//=====================================================================
int
CIcon::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  Icon par;
  par.x  = m_Coord.x;
  par.y  = m_Coord.y;
  par.id = m_IconRef.m_Reference;
  int r = CVisiblePrim::TranslateToMap( pDst, PF_ICON | ShapeBit);
  return r == R_OK ? WriteData( &par, sizeof par, pDst ) : r;
}
//=====================================================================
int
CText::TranslateToMap( TGstream* pDst, int )
{
  int bVisible = IsAnyVisible();
  Text par;
  StringParam* pSP = bVisible ? &par.param.full.string : &par.param.string;

  par.org.x     = m_Coord.x;
  par.org.y     = m_Coord.y;
  pSP->offset.x = m_String.m_Offset.cx;
  pSP->offset.y = m_String.m_Offset.cy;
  pSP->angle    = m_String.m_Angle;
  pSP->length   = strlen( m_String.m_String );

  if( bVisible )
    if( pView->GetVersion() < 107 )
    {
      FontParam* fp = &par.param.full.FontParams.font;
      fp->number = m_Font.m_FontNum;
      fp->height = m_Font.m_Height;
      fp->weight = m_Font.m_Weight;
      fp->style  = m_Font.m_Style;
      fp->color  = m_Color.m_ColorNum;
    }
    else
    {
      FontParam1* fp = &par.param.full.FontParams.font1;
      fp->number = m_Font.m_FontNum;
      fp->height = m_Font.m_Height;
      fp->weight = m_Font.m_Weight;
      fp->style  = m_Font.m_Style;
      fp->color  = m_Color.m_ColorNum;
    }

  int r = CVisiblePrim::TranslateToMap( pDst, PF_TEXT );
  if( r != R_OK )
    return r;

  r = WriteData( &par, sizeof POINT, pDst );
  if( r != R_OK )
    return r;

  r = bVisible ? WriteData( &par.param.full,   sizeof par.param.full,   pDst ):
                 WriteData( &par.param.string, sizeof par.param.string, pDst );

  return r == R_OK ?  WriteData( m_String.m_String, pSP->length, pDst ) : r;
}
//=====================================================================
int
CPolyline::TranslateToMap( TGstream* pDst, int type )
{
  ( type & ~PF_SHAPE ) || ( type = PF_POLYPOINT );
  int r = CVisiblePrim::TranslateToMap( pDst, type | PF_STROKED | ( m_Poly.m_bBound ? PF_BOUND : 0 ));
  if( r == R_OK && IsAnyVisible())
    r = m_Strok.TranslateToMap( pDst );
  return r == R_OK ? m_Poly.TranslateToMap( pDst ) : r;
}
//=====================================================================
int
CFootnote::TranslateToMap( TGstream* pDst, int )
{
  return CPolyline::TranslateToMap( pDst, PF_FOOTNOTE );
}
//=====================================================================
int
CLines::TranslateToMap( TGstream* pDst, int )
{
  return CPolyline::TranslateToMap( pDst, PF_MULTILINE );
}
//=====================================================================
int
CPolygon::TranslateToMap( TGstream* pDst, int )
{
  int r = CVisiblePrim::TranslateToMap( pDst, PF_POLYPOINT | PF_FILLED );
  if( r == R_OK && IsAnyVisible())
    r = m_Fill.TranslateToMap( pDst );
  return r == R_OK ? m_Poly.TranslateToMap( pDst ) : r;
}
//=====================================================================
int
COutlinedPolygon::TranslateToMap( TGstream* pDst, int type )
{
  int r = CVisiblePrim::TranslateToMap( pDst, type | PF_POLYPOINT | PF_FILLED | PF_BOUND | PF_STROKED );
  if( r == R_OK && IsAnyVisible())
    r = m_Strok.TranslateToMap( pDst );
  if( r == R_OK && IsAnyVisible())
    r = m_Fill.TranslateToMap( pDst );
  return r == R_OK ? m_Poly.TranslateToMap( pDst ) : r;
}
//=====================================================================
int
CSmartPolygon::TranslateToMap( TGstream* pDst, int )
{
  return COutlinedPolygon::TranslateToMap( pDst, PF_SMART );
}
//=====================================================================
int
CRectPrim::TranslateToMap( TGstream* pDst, int type  )
{
  if(( type & PF_TYPE_MASK ) == PF_RECTANGLE )
  {
    RectParam par;
    par.left   = m_Coord.x;
    par.top    = m_Coord.y;
    par.right  = m_Coord.x + m_Size.cx;
    par.bottom = m_Coord.y + m_Size.cy;
    par.angle  = m_Size.m_Angle;
    return WriteData( &par, sizeof par, pDst );
  }
  else
  {
    RECT par;
    par.left   = m_Coord.x + m_Size.cx / 2;
    par.top    = m_Coord.y + m_Size.cy / 2;
    par.right  = m_Coord.x + m_Size.cx;
    par.bottom = m_Coord.y + m_Size.cy;
    return WriteData( &par, sizeof par, pDst );
  }
}
//=====================================================================
int
CStrokRectPrim::TranslateToMap( TGstream* pDst, int type )
{
  int r = CVisiblePrim::TranslateToMap( pDst, type | PF_STROKED );
  if( r == R_OK && IsAnyVisible())
    r = m_Strok.TranslateToMap( pDst );
  return r == R_OK ? CRectPrim::TranslateToMap( pDst, type ) : r;
}
//=====================================================================
int
CFillRectPrim::TranslateToMap( TGstream* pDst, int type )
{
  int r = CVisiblePrim::TranslateToMap( pDst, type | PF_FILLED );
  if( r == R_OK && IsAnyVisible())
    r = m_Fill.TranslateToMap( pDst );
  return r == R_OK ? CRectPrim::TranslateToMap( pDst, type ) : r;
}
//=====================================================================
int
COutlinedRectPrim::TranslateToMap( TGstream* pDst, int type )
{
  int r = CVisiblePrim::TranslateToMap( pDst, type | PF_STROKED | PF_FILLED );
  if( r == R_OK && IsAnyVisible())
    r = m_Strok.TranslateToMap( pDst );
  if( r == R_OK && IsAnyVisible())
    r = m_Fill.TranslateToMap( pDst );
  return r == R_OK ? CRectPrim::TranslateToMap( pDst, type ) : r;
}
//=====================================================================
int
CRectangle::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return CStrokRectPrim::TranslateToMap( pDst, PF_RECTANGLE | ShapeBit );
}
//=====================================================================
int
CEllipse::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return CStrokRectPrim::TranslateToMap( pDst, PF_ELLIPSE | ShapeBit );
}
//=====================================================================
int
CRing::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return CStrokRectPrim::TranslateToMap( pDst, PF_ELLIPSE | PF_ROUND | ShapeBit );
}
//=====================================================================
int
CBar::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return CFillRectPrim::TranslateToMap( pDst, PF_RECTANGLE | ShapeBit );
}
//=====================================================================
int
COval::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return CFillRectPrim::TranslateToMap( pDst, PF_ELLIPSE | ShapeBit );
}
//=====================================================================
int
CCircle::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return CFillRectPrim::TranslateToMap( pDst, PF_ELLIPSE | PF_ROUND | ShapeBit );
}
//=====================================================================
int
COutlinedBar::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return COutlinedRectPrim::TranslateToMap( pDst, PF_RECTANGLE | ShapeBit );
}
//=====================================================================
int
COutlinedOval::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return COutlinedRectPrim::TranslateToMap( pDst, PF_ELLIPSE | ShapeBit );
}
//=====================================================================
int
COutlinedCircle::TranslateToMap( TGstream* pDst, int ShapeBit )
{
  return COutlinedRectPrim::TranslateToMap( pDst, PF_ELLIPSE | PF_ROUND | ShapeBit );
}
//=====================================================================
int
CArcPrim::TranslateToMap( TGstream* pDst, int )
{
  ArcParam par;
  par.left   = m_Coord.x + m_Size.cx / 2;
  par.top    = m_Coord.y + m_Size.cy / 2;
  par.right  = m_Coord.x + m_Size.cx;
  par.bottom = m_Coord.y + m_Size.cy;
  par.angle  = m_Size.m_Angle;
  par.delta  = m_Delta.m_Delta;
  return WriteData( &par, sizeof par, pDst );
}
//=====================================================================
int
CArc::TranslateToMap( TGstream* pDst, int )
{
  int r = CVisiblePrim::TranslateToMap( pDst, PF_ARC | PF_STROKED );
  if( r == R_OK && IsAnyVisible())
    r = m_Strok.TranslateToMap( pDst );
  return r == R_OK ? CArcPrim::TranslateToMap( pDst ) : r;
}
//=====================================================================
int
CPie::TranslateToMap( TGstream* pDst, int type )
{
  int r = CVisiblePrim::TranslateToMap( pDst, PF_ARC | PF_FILLED );
  if( r == R_OK && IsAnyVisible())
    r = m_Fill.TranslateToMap( pDst );
  return r == R_OK ? CArcPrim::TranslateToMap( pDst ) : r;
}
//=====================================================================
int
CChord::TranslateToMap( TGstream* pDst, int type )
{
  int r = CVisiblePrim::TranslateToMap( pDst, PF_ARC | PF_CHORD | PF_FILLED );
  if( r == R_OK && IsAnyVisible())
    r = m_Fill.TranslateToMap( pDst );
  return r == R_OK ? CArcPrim::TranslateToMap( pDst ) : r;
}
//=====================================================================
int
COutlinedChord::TranslateToMap( TGstream* pDst, int type )
{
  int r = CVisiblePrim::TranslateToMap( pDst, PF_ARC | PF_CHORD | PF_FILLED | PF_STROKED );
  if( r == R_OK && IsAnyVisible())
    r = m_Fill.TranslateToMap( pDst ) || m_Strok.TranslateToMap( pDst );
  return r == R_OK ? CArcPrim::TranslateToMap( pDst ) : r;
}
//=====================================================================
CTextByLine::~CTextByLine()
{
  delete m_pItems;
}
//=====================================================================
CTextByLine::CTextByLine( ksi_obj init_list ) :
  CVisiblePrim( init_list ),
  m_Color( init_list ),
  m_Font( init_list ),
  m_Length( 0 ),
  m_pItems( 0 )
{
  ksi_obj ksi_ims = ksi_assv_ref( init_list, keyTxtItem );
  if( KSI_PLIST_P( ksi_ims ))
  {
    m_Length = KSI_LIST_LEN( ksi_ims );
    m_pItems = new TblItem[m_Length];
    for( TblItem* pi = m_pItems ; KSI_PLIST_P( ksi_ims ) ; ++pi )
    {
      ksi_obj ksi_itm = KSI_CAR( ksi_ims );
      pi->m_Org = CppPoint( KSI_VEC_REF( ksi_itm, 0 ));
      pi->m_Angle = ksi_num2int( KSI_VEC_REF( ksi_itm, 1 ));
      pi->m_Letter[0] = ksi_num2int( ksi_char2integer( KSI_VEC_REF( ksi_itm, 2 )));
      pi->m_Letter[1] = 0;
      ksi_ims = KSI_CDR( ksi_ims );
    }
  }
}
//=====================================================================
CTextByLine::CTextByLine( CTextByLine const* p ) :
  CVisiblePrim( p ),
  m_Font( p->m_Font ),
  m_Color( p->m_Color ),
  m_Length( p->m_Length )
{
  m_pItems = m_Length ? new TblItem[m_Length] : 0;
  if( m_Length )
    memcpy( m_pItems, p->m_pItems, m_Length * sizeof( TblItem ));
}
//=====================================================================
CTextByLine::CTextByLine( int color, CFontItem& FI, int length, TxtItem* pItems ) :
  m_Color( color ),
  m_Font( FI ),
  m_Length( length )
{
  m_pItems = m_Length ? new TblItem[m_Length] : 0;
  int n = m_Length;
  for( TblItem* pItem = m_pItems ; --n >= 0 ; ++pItem, ++pItems )
  {
    pItem->m_Org.x = pItems->org.x;
    pItem->m_Org.y = pItems->org.y;
    pItem->m_Angle = pItems->angle;
    pItem->m_Letter[0] = pItems->letter[0];
    pItem->m_Letter[1] = 0;    
  }
}
//=====================================================================
CTextByLine::operator ksi_obj()
{
  ksi_obj itl = ksi_nil;
  TblItem* pi = m_pItems;
  for( int i = m_Length ; --i >= 0 ; ++pi )
  {
    ksi_obj vec = ksi_make_vector( ksi_three, ksi_false );
    KSI_VEC_REF( vec, 0 ) = ksi_cons( ksi_int2num( pi->m_Org.x ),
                                      ksi_int2num( pi->m_Org.y ));
    KSI_VEC_REF( vec, 1 ) = ksi_int2num( pi->m_Angle );
    KSI_VEC_REF( vec, 2 ) = KSI_MK_CHAR( pi->m_Letter[0] );
    itl = ksi_cons( vec, itl );
  }
  return KSI_CONS5( PrimKeys->KsiTranslate( KEY_TEXT_BY_LINE ),
                    m_Font,
                    m_Color,
                    ksi_cons( keyTxtItem, ksi_reverse( itl )),
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
void
CTextByLine::MoveOn( CSize const& s )
{
  for( TblItem* pi = m_pItems + m_Length ; --pi >= m_pItems ; pi->m_Org += s );
}
//=====================================================================
void
CTextByLine::SpinOn( CSize const& center, int angle )
{
  for( TblItem* pi = m_pItems + m_Length ; --pi >= m_pItems ; )
  {
    pi->m_Org = TurnPoint( pi->m_Org, center, angle );
    pi->m_Angle += angle;
  }
}
//=====================================================================
void
CTextByLine::ScaleOn( double cf, CPoint c )
{
  m_Font.ScaleOn( cf );

  if( c.x > 0 && c.y > 0 )
    for( TblItem* pi = m_pItems + m_Length ; --pi >= m_pItems ; )
    {
      pi->m_Org.x = c.x + cf * ( pi->m_Org.x - c.x ) + .5;
      pi->m_Org.y = c.y + cf * ( pi->m_Org.y - c.y ) + .5;
    }
}
//=====================================================================
int
CTextByLine::Detected( CSpot const& spot, int bStrict, int* pDist )
{
	int r = abs( m_Font.m_Height ) / 2;
	int R = r + spot.GetRadius();
  CPoint tp( r, -r * 3 / 4 );

  for( TblItem* pi = m_pItems + m_Length ; --pi >= m_pItems ; )
  {
    CPoint c( pi->m_Org + TurnPoint( tp, pi->m_Angle ));
    if( dist( spot.CenterPoint(), c ) < r )
		{
			if( pDist )
				*pDist = 0;
      return 1;
		}
	}
	return 0;
}
//=====================================================================
int
CTextByLine::TranslateToMap( TGstream* pDst, int t )
{
  int r = CVisiblePrim::TranslateToMap( pDst, PF_TEXTBYLINE );
  if( r != R_OK )
    return r;

  TextByLine par;

  if( pView->GetVersion() < 107 )
  {
    FontParam* fp = &par.FontParams.font;
    fp->number = m_Font.m_FontNum;
    fp->height = m_Font.m_Height;
    fp->weight = m_Font.m_Weight;
    fp->style  = m_Font.m_Style;
    fp->color  = m_Color.m_ColorNum;
  }
  else
  {
    FontParam1* fp = &par.FontParams.font1;
    fp->number = m_Font.m_FontNum;
    fp->height = m_Font.m_Height;
    fp->weight = m_Font.m_Weight;
    fp->style  = m_Font.m_Style;
    fp->color  = m_Color.m_ColorNum;
  }

  int n = par.length = m_Length;

  r = WriteData( &par, sizeof par, pDst );

  for( TblItem* pItem = m_pItems ; r == R_OK && --n >= 0 ; ++pItem )
  {
    TxtItem Item;
    Item.org.x = pItem->m_Org.x;
    Item.org.y = pItem->m_Org.y;
    Item.angle = pItem->m_Angle;
    Item.letter[0] = pItem->m_Letter[0];
    Item.letter[1] = 0;
    r = WriteData( &Item, sizeof Item, pDst );
  }

  return r;
}
//=====================================================================
void
CTextByLine::GetParams( CVisiblePrim* pPrim )
{
  CTextByLine* pt = (CTextByLine*)pPrim;
  m_Color = pt->m_Color;
  m_Font  = pt->m_Font;
  m_Flags = pt->m_Flags;
}
//=====================================================================
void
CTextByLine::Draw( CDC* pDC )
{
  if( pDocument->GetObjDef()->GetTextInvisible( CDrawObject::GetDrawnClass()))
    return;

  if( !pDC->IsPrinting() && !pView->GetMode( IDM_STREETPLAN_MODE ))
  {
    double t = double( abs( m_Font.m_Height )) * SmInUnit / pView->GetAltScale();
    if( t <= .05 )
      return;
  }

  COLORREF clr =
    CDrawObject::GetDrawnObject()->IsSpecSelected() ?
      GetColor( 0 ) :
      GetSpecColor( m_Color.m_ColorNum, m_Flags );

  COLORREF otc = pDC->SetTextColor( clr );
  int oldBkMode = pDC->SetBkMode( TRANSPARENT );
  pDC->SetTextAlign( TA_LEFT | TA_BASELINE | TA_NOUPDATECP );

  for( TblItem* pi = m_pItems + m_Length ; --pi >= m_pItems ; )
  {
    CFont* pFont = m_Font.CreateFont( pi->m_Angle );
    if( pFont )
    {
      CFont* pOldFont = pDC->SelectObject( pFont );
      pDC->TextOut( pi->m_Org.x, pi->m_Org.y, pi->m_Letter, 1 );
      pDC->SelectObject( pOldFont );
    }
  }
  pDC->SetBkMode( oldBkMode );
  pDC->SetTextColor( otc );
}
//=====================================================================
void
CTextByLine::CalcBoundBox( CBox& box )
{
  for( TblItem* pi = m_pItems + m_Length ; --pi >= m_pItems ; )
    box |= CBox( pi->m_Org, CSize( 0, 0 ), pi->m_Angle, pi->m_Letter, 1,
                 m_Font.m_FontNum, m_Font.m_Height,
                 m_Font.m_Weight, m_Font.m_Style );
}
//=====================================================================
