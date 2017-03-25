#include "stdafx.h"

#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "mdoc.h"
#include "ksi.h"
#include "joke.h"
#include "ksi_type.h"
#include "drawobj.h"
#include "dwin.h"
#include "mview.h"
#include "mathutil.h"
#include "scale.h"
#include "ksiutil.h"
#include "ksikey.h"
#include "util.h"
#include "prndlg.h"
#include "mercator.h"
#include <gdiplus.h>
#include <direct.h>
#include <gdiplus.h>
#include  <io.h>
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
int CDrawContainer::m_bHR;
int CDrawContainer::m_bTHR;
long CDrawContainer::m_bShowAsSelected = 0;
long CDrawContainer::m_bShowBox = 0;
char CDrawContainer::m_bSASMap[];
int CDrawContainer::m_bSAS;
SHAPE_NDX CDrawObject::m_ShapeNdx[CLASS_RANGE];
CDrawObject* CDrawObject::m_pDrawnObject = 0;
int CDrawObject::m_nDrawnClass = 0;
//=====================================================================
CVisiblePrim*
CDrawContainer::AddTail( CVisiblePrim* pPrim )
{
  if( !m_pTail )
    return m_pTail = AddHead( pPrim );

  m_pTail->SetNext( pPrim );
  pPrim->SetNext( 0 );
  ++m_nCount;
  return m_pTail = pPrim;
}
//=====================================================================
CVisiblePrim*
CDrawContainer::AddHead( CVisiblePrim* pPrim )
{
  pPrim->SetNext( m_pHead );
  ++m_nCount;
  return m_pHead = pPrim;
}
//=====================================================================
CVisiblePrim*
CDrawContainer::RemoveHead()
{
  ASSERT( m_pHead );
  CVisiblePrim* pTmp = m_pHead;
  m_pHead = pTmp->GetNext();
  if( !m_pHead )
    m_pTail = m_pHead;
  --m_nCount;
  return pTmp;
}
//=====================================================================
CVisiblePrim*
CDrawContainer::FindIndex( int nIndex )
{
  CVisiblePrim* pPrim;
  for( pPrim = m_pHead ; pPrim && --nIndex >= 0 ; pPrim = pPrim->GetNext());
  return pPrim;
}
//=====================================================================
CVisiblePrim*
CDrawContainer::MakePrim( ksi_obj init_list )
{
//  KsiCall( "tk-box", init_list, 0 );
  ksi_obj type = KSI_CAR( init_list );
  init_list = KSI_CDR( init_list );
  CVisiblePrim* prim = 0;
  switch( PrimKeys->KsiTranslate( type ))
  {
    case KEY_CONTAINER       : prim = new CDrawContainer( init_list );  break;
    case KEY_POINT           : prim = new CPointPrim( init_list );      break;
    case KEY_RECTANGLE       : prim = new CRectangle( init_list );      break;
    case KEY_ELLIPSE         : prim = new CEllipse( init_list );        break;
    case KEY_RING            : prim = new CRing( init_list );           break;
    case KEY_BAR             : prim = new CBar( init_list );            break;
    case KEY_OVAL            : prim = new COval( init_list );           break;
    case KEY_CIRCLE          : prim = new CCircle( init_list );         break;
    case KEY_OUTLINED_BAR    : prim = new COutlinedBar( init_list );    break;
    case KEY_OUTLINED_OVAL   : prim = new COutlinedOval( init_list );   break;
    case KEY_OUTLINED_CIRCLE : prim = new COutlinedCircle( init_list ); break;
    case KEY_POLYLINE        : prim = new CPolyline( init_list, 0 );    break;
    case KEY_FOOTNOTE        : prim = new CFootnote( init_list );       break;
    case KEY_BOUND_POLYLINE  : prim = new CPolyline( init_list, 1 );    break;
    case KEY_LINES           : prim = new CLines( init_list );          break;
    case KEY_TEXT            : prim = new CText( init_list );           break;
    case KEY_TEXT_BY_LINE    : prim = new CTextByLine( init_list );     break;
    case KEY_ARC             : prim = new CArc( init_list );            break;
    case KEY_PIE             : prim = new CPie( init_list );            break;
    case KEY_POLYGON         : prim = new CPolygon( init_list );        break;
    case KEY_OUTLINED_POLYGON: prim = new COutlinedPolygon( init_list );break;
    case KEY_SMART_POLYGON   : prim = new CSmartPolygon( init_list );   break;
    case KEY_ICON            : prim = new CIcon( init_list );           break;
    case KEY_CHORD           : prim = new CChord( init_list );          break;
    case KEY_OUTLINED_CHORD  : prim = new COutlinedChord( init_list );  break;
  }
  return prim;
}
//=====================================================================
CDrawContainer::CDrawContainer( ksi_obj init_obj ) :
//  CVisiblePrim( init_obj ),
  m_pHead( 0 ),
  m_pTail( 0 ),
  m_nCount( 0 )
{
//  KsiCall( "tk-box", init_obj, 0 );
  m_Flags = SF_LOW_VISIBLE | SF_HIG_VISIBLE;
  ASSERT( KSI_PLIST_P( init_obj ));
  for( init_obj = KSI_CAR( init_obj ) ; KSI_PLIST_P( init_obj ) ;
       init_obj = KSI_CDR( init_obj ))
    AddTail( MakePrim( KSI_CAR( init_obj )));
}
//=====================================================================
CDrawContainer::CDrawContainer( CDrawContainer* p ):
  CVisiblePrim( p ),
  m_pHead( 0 ),
  m_pTail( 0 ),
  m_nCount( 0 )
{
  for( CVisiblePrim* pPrim = p->GetHead() ; pPrim ; pPrim = pPrim->GetNext())
    AddTail( pPrim->MakeCopy());
}
//=====================================================================
void
CDrawContainer::GetParams( CDrawContainer* p )
{
  if( p && GetCount() == p->GetCount())
  {
    CVisiblePrim* p1 = GetHead();
    CVisiblePrim* p2 = p->GetHead();
    for( ; p1 ; p1 = p1->GetNext(), p2 = p2->GetNext())
      if( p1->GetType() == p2->GetType())
        p1->GetParams( p2 );
  }
}
//=====================================================================
CDrawContainer::operator ksi_obj()
{
  ksi_obj ksi_lst = ksi_nil;
  for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
    ksi_lst = ksi_cons( *p, ksi_lst );

  return KSI_CONS3( PrimKeys->KsiTranslate( KEY_CONTAINER ),
                    ksi_reverse( ksi_lst ),
                    CVisiblePrim::operator ksi_obj());
}
//=====================================================================
CDrawContainer::~CDrawContainer()
{
  while( m_pHead )
    delete( RemoveHead());
}
//=====================================================================
void
CDrawContainer::SetSelectFlag( unsigned char flags )
{
  SetFlags( flags, SF_STATUS_MASK );
  for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
    p->SetSelectFlag( flags );
}
//=====================================================================
void
CDrawContainer::InflateClipSize( CSize& size )
{
  for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
  {
    int hr = m_bHR || m_bTHR && p->GetType() == KEY_TEXT;
    if(( hr ? p->IsHigVisible() : p->IsLowVisible()) &&
       ( !p->IsSelVisible() || IsSelected() || m_bSAS ))
      p->InflateClipSize( size );
  }
}
//=====================================================================
void
CDrawContainer::Draw( CDC* pDC )
{
  if( m_bHR ? IsHigVisible() : IsLowVisible())
    for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
    {
      int hr = m_bHR || m_bTHR && p->GetType() == KEY_TEXT;
      if(( hr ? p->IsHigVisible() : p->IsLowVisible()) &&
           ( !p->IsSelVisible() || IsSelected() || m_bSAS ))
        p->Draw( pDC );
    }
}
//=====================================================================
void
CDrawContainer::CalcBoundBox( CBox& box )
{
  for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
    if( p->IsAnyVisible())
      p->CalcBoundBox( box );
}
//=====================================================================
int
CDrawContainer::Detected( CSpot const& spot, int bStrict, int* pDist )
{
  int i = m_nCount;
  while( --i >= 0 )
  {
    CVisiblePrim* p = FindIndex( i );
    int hr = m_bHR || m_bTHR && p->GetType() == KEY_TEXT;
    if(( hr ? p->IsHigVisible() : p->IsLowVisible()) &&
       ( !p->IsSelVisible() || IsSelected() || m_bSAS ) &&
       p->Detected( spot, bStrict, pDist ))
      break;
  }
  return i + 1;
}
//=====================================================================
int
CDrawContainer::GetRing( CSpot const& spot, CRect* rct )
{
  for( int i = m_nCount ; --i >= 0 ; )
  {
    CVisiblePrim* p = FindIndex( i );
    int hr = m_bHR || m_bTHR && p->GetType() == KEY_TEXT;
    if(( hr ? p->IsHigVisible() : p->IsLowVisible()) &&
       ( !p->IsSelVisible() || IsSelected() || m_bSAS ) &&
       p->GetRing( spot, rct ))
      return 1;
  }
  return 0;
}
//=====================================================================
void
CDrawContainer::GetAngle( CSpot const& spot, int* pAngle, int* pDist )
{
  for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
  {
    int hr = m_bHR || m_bTHR && p->GetType() == KEY_TEXT;
    if(( hr ? p->IsHigVisible() : p->IsLowVisible()) &&
       ( !p->IsSelVisible() || IsSelected() || m_bSAS ))
    {
      int d = *pDist;
      int a;
      if( p->Detected( spot, 0, &d ) && p->GetAngle( spot, &a ))
      {
        *pDist = d;
        *pAngle = a;
      }
    }
  }
}
//=====================================================================
void
CDrawContainer::MoveOn( CSize const& offset )
{
  for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
    p->MoveOn( offset );
}
//=====================================================================
void
CDrawContainer::SpinOn( CSize const& center, int angle )
{
  for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
    p->SpinOn( center, angle );
}
//=====================================================================
void
CDrawObject::MoveOn( CSize const& offset )
{
  m_pOriginCont->MoveOn( offset );
  if( m_pScaledCont )
    m_pScaledCont->MoveOn( offset );
  m_BoundBox.OffsetRect( offset );
  m_OriginBox.OffsetRect( offset );
}
//=====================================================================
void
CDrawObject::SpinOn( CSize const& center, int angle )
{
  m_pOriginCont->SpinOn( center, angle );
  if( m_pScaledCont )
    m_pScaledCont->SpinOn( center, angle );
  CalcBoundBox();
}
//=====================================================================
void
CDrawContainer::ScaleOn( double cf, CPoint c )
{
  for( CVisiblePrim* p = GetHead() ; p ; p = p->GetNext())
    if( p->IsAnyVisible())
      p->ScaleOn( cf, c );
}
//=====================================================================
void
CDrawObject::ScaleOn( double cf, CPoint c )
{
  int sf = SelectFlag();
  delete m_pScaledCont;
  m_pScaledCont = 0;
  m_pActualCont = m_pOriginCont;
  if( cf != 1. )
  {
    m_pActualCont = m_pScaledCont = new CDrawContainer( m_pOriginCont );
    m_pScaledCont->ScaleOn( cf, c );
  }
  SetSelectFlag( sf );
  CalcBoundBox();
}
//=====================================================================
CBox&
CDrawObject::CalcBoundBox()
{
  m_OriginBox.Reset();
  m_pDrawnObject = this;
  m_pOriginCont->CalcBoundBox( m_OriginBox );

  if( m_pScaledCont )
    m_pScaledCont->CalcBoundBox( m_BoundBox );
  else
    m_BoundBox = m_OriginBox;
  m_pDrawnObject = 0;

  if( m_pLegend )
    m_BoundBox |= m_pLegend->CalcBox( m_OriginBox.CenterPoint(), m_pDiagItem );
  return m_BoundBox;
}
//=====================================================================
void
CDrawObject::GetParams( CDrawContainer* pPrim )
{
  m_pActualCont->GetParams( pPrim );
  CalcBoundBox();
}
//=====================================================================
CDrawObject::CDrawObject( ksi_obj init_list, CIdent id, int node ):
  m_Node( node ),
  m_Ident( id ),
  m_Time( 0 ),
  m_Clock( 0 ),
  m_pDiagItem( 0 ),
  m_pLegend( 0 )
{
  m_pActualCont = m_pOriginCont = new CDrawContainer( KSI_CDR( init_list ));
  m_pScaledCont = 0;
  CalcBoundBox();
}
//=====================================================================
CDrawObject::CDrawObject( CDrawObject* p ):
  m_Node( p->m_Node ),
  m_Ident( p->Ident()),
  m_Time( p->m_Time ),
  m_Clock( p->m_Clock ),
  m_pDiagItem( 0 ),
  m_pLegend( 0 ),
  m_BoundBox( p->m_BoundBox ),
  m_OriginBox( p->m_OriginBox )
{
  m_pOriginCont = new CDrawContainer( p->m_pOriginCont );
  m_pScaledCont = p->m_pScaledCont ? new CDrawContainer( p->m_pScaledCont ) : 0;
  m_pActualCont = m_pScaledCont ? m_pScaledCont : m_pOriginCont;
  m_pActualCont->SetSelectFlag( p->m_pActualCont->SelectFlag());
}
//=====================================================================
CLegend const*
CDrawObject::SetLegend( CLegend const* pLg )
{
        CLegend const* tLg = m_pLegend;
        m_pLegend = pLg;
        return tLg;
}
//=====================================================================
CDrawObject::CDrawObject( CIdent id, int node, long time, long clock, 
                          CDrawContainer* p ):
  m_Node( node ),
  m_Ident( id ),
  m_Time( time ),
  m_Clock( clock ),
  m_pDiagItem( 0 ),
  m_pLegend( 0 )
{
  m_pScaledCont = 0;
  m_pActualCont = m_pOriginCont = p;
}
//=====================================================================
CDrawObject::~CDrawObject()
{
  UnselectSpecial( 1 );
  delete m_pOriginCont;
  delete m_pScaledCont;
}
//=====================================================================
int
CDrawObject::IsHiRes()
{
  int st = Status();

  if( !( st & 0x80 ))
    return 0;

  ksi_obj ksi_img = *m_pOriginCont;
  ksi_obj cl = KSI_MK_SINT( Class());
  int hs = st |  0x80;
  int ls = st & ~0x80;
  ksi_obj ost = KSI_MK_SINT( st );
  ksi_obj hst = KSI_MK_SINT( hs );
  ksi_obj lst = KSI_MK_SINT( ls );
  ksi_obj h_val = KsiCall( "rebuild-object",
                            cl, ost, cl, hst, ksi_nil, ksi_img, 0 );
  ksi_obj l_val = KsiCall( "rebuild-object",
                            cl, ost, cl, lst, ksi_nil, ksi_img, 0 );
  return !KSI_EQUAL_P( h_val, l_val );
}
//=====================================================================
void
CDrawObject::SetSAS()
{
  CDrawContainer::m_bSAS = CDrawContainer::m_bShowAsSelected && ( CDrawContainer::m_bSASMap[Class()>>3] & ( 1 << ( Class() & 7 )));
}
//=====================================================================
int
CDrawObject::PresetHR()
{
  int hr = CDrawContainer::m_bHR;
  m_pDrawnObject = this;
  CDrawContainer::m_bTHR = pView->IsHiResolution();
  CDrawContainer::m_bHR = CDrawContainer::m_bTHR && ( Status() & 0x80 ) != 0;
  return hr;
}
//=====================================================================
void
CDrawObject::RestoreHR( int hr )
{
  m_pDrawnObject = 0;
  CDrawContainer::m_bHR = hr;
}
//=====================================================================
void
CDrawObject::InflateClipSize( CSize& size )
{
  m_pDrawnObject = this;
  m_pActualCont->InflateClipSize( size );
  m_pDrawnObject = 0;
}
//=====================================================================
void
CDrawObject::UnselectSpecial( int bRemove )
{
  delete[] m_pDiagItem;
  m_pDiagItem = 0;
  m_pLegend = 0;
  if( !bRemove )
    CalcBoundBox();
  if( m_pActualCont )
    m_pActualCont->ResetSelectFlag();
}
//=====================================================================
void
CDrawObject::SetSpecSelection( CLegend const* pLegend, DiagItem* pItem )
{
  delete[] m_pDiagItem;
  m_pDiagItem = 0;

  if( m_pLegend = pLegend )
  {
    int n = pLegend->GetSize();
    if( n && pItem )
    {
      m_pDiagItem = new DiagItem[n];
      for( int i = n ; --i >= 0 ; m_pDiagItem[i].Create( &pItem[i] ));
    }
    CalcBoundBox();
  }
}
//=====================================================================
void
CDrawObject::SetSpecSelection( CDrawObject* pSel )
{
  m_pLegend = pSel->m_pLegend;
  m_pDiagItem = 0;
  int n = m_pLegend ? m_pLegend->GetSize(): 0;

  if( pSel->m_pDiagItem && n )
  {
    m_pDiagItem = new DiagItem[n];
    for( int i = n ; --i >= 0 ; m_pDiagItem[i].Create( &pSel->m_pDiagItem[i] ));
    m_BoundBox |= m_pLegend->CalcBox( m_OriginBox.CenterPoint(), m_pDiagItem );
  }
}
//=====================================================================
void
CDrawObject::DrawSpecial( CDC* pDC )
{
  if( m_pLegend && m_pLegend->GetSize() )
    m_pLegend->Draw( pDC, m_OriginBox.CenterPoint(), m_pDiagItem );
}
//=====================================================================
CString const*
CDrawObject::DetectedSpecial( CSpot const& spot )
{
  return m_pLegend && m_pDiagItem ? m_pLegend->Detect( spot, m_pDiagItem  ) : 0;
}
//=====================================================================
int
CDrawObject::IsSublayer()
{
  int t = Class();
  return t > SUBLAYER_CLASS;
}
//=====================================================================
int
CDrawObject::IsSublayerStatic()
{
  int t = Class();
  return t > SUBSTAT_CLASS;
}
//=====================================================================
void
CDrawObject::Draw( CDC* pDC )
{
  if( IsSublayer() && !m_pActualCont->IsSelected() && DrawSubl( pDC ))
  {
    CVisiblePrim* ph = m_pActualCont->RemoveHead();
    int hr = PresetHR();
    m_pActualCont->Draw( pDC );
    RestoreHR( hr );
    m_pActualCont->AddHead( ph );
  }
  else
  {
    CPoint org( m_BoundBox.TopLeft());
    pDC->LPtoDP( &org );
    pDC->SetBrushOrg( org.x & 7, org.y & 7 );
    int hr = PresetHR();
    m_pActualCont->Draw( pDC );
    RestoreHR( hr );
  }

  DrawSpecial( pDC );

  if( m_pActualCont->IsSelected() && CDrawContainer::m_bShowBox )
  {
    CPen pen( PS_DOT, 0, RGB( -1, 0, 0 ));
    CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
    CPen* pOldPen = (CPen*)pDC->SelectObject( &pen );
    pDC->Rectangle( GetBoundBox());
    pDC->SelectObject( pOldPen );
    pDC->SelectObject( pOldBrush );
  }
}
//=====================================================================
int
CDrawObject::Detected( CSpot const& spot, int bStrict, int* pDist )
{
	if( !spot.Inflate( GetBoundBox()).PtInRect( spot ))
    return 0;
/*
  if( !bStrict )
  {
    int np, tp;
    CPoint* pp = GetAnyPoints( &tp, &np );
    if( tp == SH_POINT && spot.PtInRect( *pp ))
      return m_ShapeNdx[Class()].ndx + 1;
  }
*/
  if( !bStrict && DetectedSpecial( spot ))
    return m_pActualCont->m_nCount;

  int hr = PresetHR();
  int r = m_pScaledCont ? m_pScaledCont->Detected( spot, bStrict, pDist ) :
                          m_pActualCont->Detected( spot, bStrict, pDist );
  RestoreHR( hr );

  return r;
}
//=====================================================================
int
CDrawObject::GetRing( CSpot const& spot, CRect* rct )
{
  return spot.Inflate( GetBoundBox()).PtInRect( spot ) ? m_pActualCont->GetRing( spot, rct ) : 0;
}
//=====================================================================
void
CDrawObject::GetAngle( CSpot const& spot, int* pAngle, int* pDist )
{
  if( spot.Inflate( GetBoundBox()).PtInRect( spot ))
  {
    int hr = PresetHR();
    m_pActualCont->GetAngle( spot, pAngle, pDist );
    RestoreHR( hr );
  }
}
//=====================================================================
int
CDrawObject::DrawToPoint( CSpot const& spot, CPoint* pPoint, int mode )
{
  if( !spot.Inflate( GetBoundBox()).PtInRect( spot.CenterPoint()))
    return -1;

  SetSAS();
  int hr = PresetHR();

  CPoint c( spot.CenterPoint());
  int d = spot.GetRadius();
  UINT pd = -1;

  int np, tp;
  CPoint* pp = GetAnyPoints( &tp, &np );

  if( pp )
    switch( mode )
    {
      case 0:
        while( --np >= 0 )
        {
          CPoint tp( *pp++ );
          int td = dist( tp, c );
          if( td <= d && td < pd )
          {
            pPoint[0] = tp;
            pd = td;
          }
        }
        break;
      case 2:
      {
        for( int n = np + ( tp = SH_POLYGON ), i = tp == SH_POLYLINE ; i < n ; ++i )
        {
          int td = dist( pp[i], c );
          if( td <= d && td < pd )
          {
            pPoint[0] = pp[(i-1)%np];
            pPoint[1] = pp[i];
            pPoint[2] = pp[(i+1)%np];
            pd = td;
          }
        }
        break;
      }
      default:
      {
        CPoint p0, p1( tp == SH_POLYGON ? pp[++np-2] : *pp++ );
        while( --np > 0 )
        {
          p0 = p1;
          p1 = *pp++;
          CSize s( p1 - p0 );
          double dl = hipo( s );
          CPoint t( TurnPoint( c - p0, s.cx, s.cy, dl ));
          int td = abs( t.y );
          if( t.x >= 0 && t.x < dl && td <= d && td < pd )
          {
            pd = td;
            pPoint[0] = p0;
            pPoint[1] = p1;
          }
        }
      }
    }

  RestoreHR( hr );

  return pd;
}
//=====================================================================
CVisiblePrim*
CDrawObject::GetItem( int n )
{
  CVisiblePrim* p;
  for( p = n >= 0 ? m_pActualCont->GetHead() : 0 ; p && --n >= 0 ; p = p->GetNext());
  return p;
}
//=====================================================================
CPoint*
CDrawObject::GetPoint( int* pnPnt )
{
  SHAPE_NDX* pSh = &m_ShapeNdx[Class()];
  int tmp;
  CVisiblePrim* pPrim = pSh->shape == SH_POINT ? GetItem( pSh->ndx ) : 0;
  return pPrim ? pPrim->GetPoint( pnPnt ? pnPnt : &tmp ) : 0;
}
//=====================================================================
CPoint*
CDrawObject::GetPolyline( int* pnPnt )
{
  SHAPE_NDX* pSh = &m_ShapeNdx[Class()];
  int tmp;
  CVisiblePrim* pPrim = pSh->shape == SH_POLYLINE ? GetItem( pSh->ndx ) : 0;
  return pPrim ? pPrim->GetPolyline( pnPnt ? pnPnt : &tmp ) : 0;
}
//=====================================================================
CPoint*
CDrawObject::GetPolygon( int* pnPnt )
{
  SHAPE_NDX* pSh = &m_ShapeNdx[Class()];
  int tmp;
  CVisiblePrim* pPrim = pSh->shape == SH_BPOLYLINE ||
                        pSh->shape == SH_POLYGON ? GetItem( pSh->ndx ) : 0;
  return pPrim ? pPrim->GetPolygon( pnPnt ? pnPnt : &tmp ) : 0;
}
//=====================================================================
CPoint*
CDrawObject::GetAnyPoints( int* pTyp, int* pnPnt )
{
  int tmp0, tmp1;
  pTyp  || ( pTyp  = &tmp0 );
  pnPnt || ( pnPnt = &tmp1 );
  CPoint* pPnt;
  *pTyp = ( pPnt = GetPoint   ( pnPnt )) ? SH_POINT    :
          ( pPnt = GetPolyline( pnPnt )) ? SH_POLYLINE :
          ( pPnt = GetPolygon ( pnPnt )) ? SH_POLYGON  :
                                           SH_NOTHING;
  return pPnt;
}
//=====================================================================
void
CDrawObject::SetPoly( CPoint const* points, int pn )
{
  SHAPE_NDX* pSh = &m_ShapeNdx[Class()];
  int n = pSh->ndx;
  
  CVisiblePrim* p;
  for( p = n >= 0 ? m_pOriginCont->GetHead() : 0 ; p && --n >= 0 ; p = p->GetNext());

  CVisiblePrim* pPrim = pSh->shape == SH_POLYLINE  ||
                        pSh->shape == SH_BPOLYLINE ||
                        pSh->shape == SH_POLYGON ? p : 0;
  if( pPrim )
  {
    pPrim->SetPoly( points, pn );
    CalcBoundBox();
  }
}
//=====================================================================
int
CDrawContainer::TranslateToMap( TGstream* pDst, int nShp )
{
  Container par;
  par.length = m_nCount;
  int r = CVisiblePrim::TranslateToMap( pDst, PF_CONTAINER );
  if( r == R_OK )
    r = WriteData( &par.length, sizeof par.length, pDst );

  for( CVisiblePrim* pPrim = m_pHead ; r == R_OK && pPrim ; pPrim = pPrim->GetNext(), --nShp )
    r = pPrim->TranslateToMap( pDst, nShp == 0 ? PF_SHAPE : 0 );
  return r;
}
//=====================================================================
int
CDrawObject::TranslateToMap( TGstream* pDst, int )
{
  SHAPE_NDX* pSh = &m_ShapeNdx[Class()];

  CBaseDataRec* pDrh = pView->GetDataRecHeadPtr();
  streampos begPos = pDst->tellp();
  pDrh->SetMagic( OBJMAGIC );
  pDrh->SetIdent( Ident());
  pDrh->SetNode( m_Node );
  pDrh->SetBox( *((RECT*)&GetOriginBox()));
  pDrh->SetTime( GetTime());
  pDrh->SetClock( GetClock());

  int r = WriteData( pDrh->GetRecPtr(), pDrh->GetRecSize(), pDst );

  if( r == R_OK )
    r = m_pOriginCont->TranslateToMap(pDst, pSh->shape == SH_NOTHING ? -1 : pSh->ndx);

  if( r == R_OK )
  {
    streampos endPos = pDst->tellp();
    pDrh->SetSize( endPos - begPos );
    pDst->seekp( begPos );
    r = WriteData( pDrh->GetRecPtr(), pDrh->GetRecSize(), pDst );
    pDst->seekp( endPos );
  }

  return r == R_OK;
}
//=====================================================================
int
CDrawObject::IsEqual( CDrawObject* pObj )
{
  if( Ident() != pObj->Ident())
    return 0;

  TGstream Dst;
  m_pOriginCont->TranslateToMap( &Dst );
  streamoff l = Dst.tellp();
  pObj->m_pOriginCont->TranslateToMap( &Dst );
  char* p = Dst.str();
  return l == Dst.tellp() - l && !memcmp( p, p + l, l );
}
//=====================================================================
void
CDrawObject::InitShapeNdx( int n )
{
  ksi_obj key[] = { keyFplg, keyBpll, keyPolyline, keyPoint };
  static SHAPE_TYPE SH[] = { SH_POLYGON, SH_BPOLYLINE, SH_POLYLINE, SH_POINT };

  SHAPE_NDX* pSh = &m_ShapeNdx[n];

  pSh->shape = SH_NOTHING;
  pSh->ndx = -1;

  ksi_obj ksi_map = KsiCall( "get-contents-map", KSI_MK_SINT( n ), ksi_zero, 0 );
  if( KSI_TRUE_P( ksi_map ))
  {
    ksi_obj ksi_fmp = ksi_false;
    int k;
    for( k = CELEM( key ) ; KSI_FALSE_P( ksi_fmp ) && --k >= 0 ; ksi_fmp = ksi_memq( key[k], ksi_map ));
    if( KSI_TRUE_P( ksi_fmp ))
    {
      pSh->shape = SH[k];
      pSh->ndx = KSI_LIST_LEN( ksi_map ) - KSI_LIST_LEN( ksi_fmp );
    }
  }
}
//=====================================================================
void
CDrawObject::InitShapeNdx()
{
  for( int i = CLASS_RANGE ; --i >= 0 ; InitShapeNdx( i ));
}
//=====================================================================
int
CDrawObject::IsHostOf( CDrawObject* pObj )
{
  ksi_obj hcl = KSI_MK_SINT( this->Class());
  ksi_obj tcl = KSI_MK_SINT( pObj->Class());
  ksi_obj cls = KsiCall( "get-host-class-list", tcl, 0 );
  return KSI_TRUE_P( ksi_memv( hcl, cls )) &&
         IsHostLink( pObj, IsLinkedTo( pObj ));
}
//=====================================================================
int
CDrawObject::IsHostLink( CDrawObject* pObj, LINK_TYPE lt )
{
  if( lt == LT_NOTHING )
    return 0;

  ksi_obj ksi_list = KsiCall( "get-target-class-list", KSI_MK_SINT( Class()), 0 );
  ksi_obj ksi_mem = ksi_memv( KSI_MK_SINT(pObj->Class()), ksi_list );
  if( KSI_FALSE_P( ksi_mem ))
    return 0;

  SHAPE_TYPE hsht = this->GetShapeType();
  SHAPE_TYPE tsht = pObj->GetShapeType();

  return hsht == SH_POINT && tsht == SH_POLYLINE ?
           lt == LT_BEGIN || lt == LT_END/* 1*/ :
         hsht == SH_POLYLINE && tsht == SH_POLYLINE ?
           lt == LT_BEGIN || lt == LT_END :
         hsht == SH_POLYLINE && tsht == SH_POINT ?
           ( lt == LT_BEGIN || lt == LT_END ) ^
           KSI_FALSE_P( KsiCall( "is-only-insertable?", KSI_MK_SINT( pObj->Class()), 0 )) : 1;
}
//=====================================================================
LINK_TYPE
CDrawObject::IsLinkedTo( CDrawObject* pObj )
{
  if( !Intersect( this->GetOriginBox(), pObj->GetOriginBox()))
    return LT_NOTHING;

  /*static*/ ksi_obj CPL = ksi_eval_str_with_catch( "context-proc-list" );
  /*static*/ ksi_obj FMP = ksi_eval_str_with_catch( "find-match-proc" );
  /*static*/ ksi_obj key_begin = ksi_str02key( "begin" );
  /*static*/ ksi_obj key_end = ksi_str02key( "end" );
  /*static*/ ksi_obj key_undef = ksi_str02key( "undef" );

  int t0 = Class();
  int t1 = pObj->Class();

  ksi_obj cls[] = { KSI_MK_SINT( t0 ), KSI_MK_SINT( t1 ) };
  ksi_obj img0 = *this;
  ksi_obj img1 = *pObj;
  ksi_obj env0 = KsiCall( "unwrap", cls[0], ksi_zero, img0, 0 );
  ksi_obj env1 = KsiCall( "unwrap", cls[1], ksi_zero, img1, 0 );
  ksi_obj env[] = { env0, env1 };

  for( int t = 0 ; t < 2 ; ++t )
  {
    int q = t^1;
    ksi_obj proc = ksi_apply_3_with_catch( FMP, cls[t], cls[q], CPL );
    if( !KSI_PLIST_P( proc ) || KSI_LIST_LEN( proc ) < 3 )
      continue;

    proc = KSI_CAR( KSI_CDR( KSI_CDR( proc )));
    ksi_obj val = ksi_apply_2_with_catch( proc, env[t], env[q] );
    if( !KSI_PAIR_P( val ))
      continue;

    val = t == 0 ? KSI_CAR( val ) : KSI_CDR( val );

    if( KSI_EQ_P( val, key_begin ))
      return LT_BEGIN;

    if( KSI_EQ_P( val, key_end ))
      return LT_END;

    if( KSI_EQ_P( val, key_undef ))
      return LT_UNDEF;
  }

  int nh, sh;
  CPoint* ph = this->GetAnyPoints( &sh, &nh );
  if( !ph )
    return LT_NOTHING;

  int nt, st;
  CPoint* pt = pObj->GetAnyPoints( &st, &nt );
  if( !pt )
    return LT_NOTHING;

  if( sh == SH_POLYLINE && st == SH_POLYLINE )
    return dist( ph[0], pt[0] ) <= 1 ||
           dist( ph[0], pt[nt-1] ) <= 1 ||
           dist( ph[nh-1], pt[0] ) <= 1 ||
           dist( ph[nh-1], pt[nt-1] ) <= 1 ||
           IsAllPointsOnPolyline( ph, nh, pt, nt, 1) ||
           IsAllPointsOnPolyline( pt, nt, ph, nh, 1 ) ? LT_UNDEF : LT_NOTHING;

  int n[] = { nh, nt };
  int s[] = { sh, st };
  CPoint* p[] = { ph, pt };
  int i = sh > st, j = 1 - i;

  if( s[i] != SH_POINT )
    return LT_NOTHING;

  switch( s[j] )
  {
    case SH_POINT:
      return p[i][0] == p[j][0] ? LT_UNDEF : LT_NOTHING;
    case SH_POLYLINE:
      return p[i][0] == p[j][0]      ? LT_BEGIN:
             p[i][0] == p[j][n[j]-1] ? LT_END:
             IsPointOnPolyline( p[i][0], p[j], n[j], 5 ) >= 0 ? LT_UNDEF : LT_NOTHING;
    case SH_BPOLYLINE:
    case SH_POLYGON:
      return IsPointOnPolygon( p[i][0], p[j], n[j], 5 ) >= 0 ? LT_UNDEF : LT_NOTHING;
  }
  return LT_NOTHING;
}
//=====================================================================
static int
SplitNum( int fs, int ss )
{
  if( ss == 0 )
    return 1;
  
  int n;
  for( n = 1 ; fs / n > ss ; ++n );
  return n;
}
//======================================================================
static CString
MakeSplitName( CString const& path, CString const& name, int i, int j )
{
  CString s;
  char bn[_MAX_FNAME], tmp[_MAX_PATH];
  _splitpath( name, tmp, tmp, bn, tmp );
  s.Format( "%s\\%s[%d][%d].jpg", path, bn, i, j );
  s.MakeLower();
  return s;
}
//======================================================================
using namespace Gdiplus;
//======================================================================
int
CDrawObject::IsAccessable()
{
  if( m_BoundBox.IsRectEmpty())
    return 0;

  if( !IsSublayer())
    return 1;

  int bf = 0;
  for( int i = 3 ; !bf && --i >= 0 ; )
  {
    CString path( App.GetSubbmpPath( i ));
    CString fname( *((CText*)m_pOriginCont->m_pHead->GetNext()));
    float t0, t1;
    int n = sscanf( fname, "%f%f", &t0, &t1 );
    bf = !( n == 2 && t0 > 0 && t1 < 0 ) || _access( path + fname, 04 ) == 0;
  }
  return bf;
}
//======================================================================
int
CDrawObject::SplitSubl( int bCreateAll, void* pLoadBmp )
{
  CRectangle* pRectPrim = (CRectangle*)m_pOriginCont->m_pHead;
  CText* pTextPrim = (CText*)pRectPrim->GetNext();

  CString path;

  int br = -1;
  int i, j;
  for( i = 2 ; br < 0 && --i >= 0 ; )
  {
    path = App.GetSubbmpPath( i );
    CString fullPath( path + *pTextPrim );
    br = _access( fullPath, 04 );
  }

  if( br < 0 )
    return 1;

  CString splitPath( path + "split" );
  path += *pTextPrim;
  WCHAR wbuf[_MAX_PATH];

  CWaitCursor wc;

  Bitmap* pBmp = (Bitmap*)pLoadBmp;
  if( !pBmp )
  {
    if( !MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, path, -1, wbuf, _MAX_PATH ))
      return 2;

    if( !( pBmp = Bitmap::FromFile( wbuf, 1 )))
      return 3;
  }

  int SS = App.GetProfileInt( "MAP_SUBLAYER", "SplitSize", 0 );
  int H = pBmp->GetHeight();
  int W = pBmp->GetWidth();
  int nh = SplitNum( H, SS );
  int nw = SplitNum( W, SS );

  if( nw == 1 && nh == 1 )
  {
    if( !pLoadBmp )
      delete pBmp;
    return 0;
  }

  _mkdir( splitPath );

  CLSID bmpClsid;
  UINT num, size;
  GetImageEncodersSize( &num, &size );
  ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*) new BYTE[size];
  GetImageEncoders( num, size, pImageCodecInfo );

  for( i = num ; --i >= 0 && wcscmp( pImageCodecInfo[i].MimeType, L"image/jpeg" ) ; );

  if( i >= 0 )
    bmpClsid = pImageCodecInfo[i].Clsid;

  delete pImageCodecInfo;

  if( i < 0 )
  {
    if( !pLoadBmp )
      delete pBmp;
    return 4;
  }

  int bSuc = 1;
  for( i = 0 ; bSuc && i < nh ; ++i )
    for( j = 0 ; bSuc && j < nw ; ++j )
    {
      CString spn( MakeSplitName( splitPath, path, i, j ));
      if( !bCreateAll && FileExist( spn ))
        continue;
      CString msg;
      msg.Format( "Фрагментация %s %d/%d", path, i * nw + j, nw * nh );
      ReportHint( msg );
      int t = MulDiv( H, i, nh );
      int h = MulDiv( H, i + 1, nh ) - t;
      int l = MulDiv( W, j, nw );
      int w = MulDiv( W, j + 1, nw ) - l;
      Gdiplus::Rect rct( l, t, w, h );
      BitmapData bitmapData;
      bSuc = pBmp->LockBits( &rct, ImageLockModeRead, PixelFormat24bppRGB, &bitmapData ) == Ok;
      if( bSuc )
      {
        Bitmap tmp( bitmapData.Width, bitmapData.Height, bitmapData.Stride, bitmapData.PixelFormat, (BYTE*)bitmapData.Scan0 );
        MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, spn, -1, wbuf, _MAX_PATH );
        tmp.Save( wbuf, &bmpClsid, 0 );
        pBmp->UnlockBits( &bitmapData );
      }
    }

  if( !pLoadBmp )
    delete pBmp;

  ClearHint();
  return bSuc ? 0 : 5;
}
//======================================================================
int
CDrawObject::RemoveSplitSubl()
{
  if( !IsSublayer())
    return 1;

  CRectangle* pRectPrim = (CRectangle*)m_pOriginCont->m_pHead;
  CText* pTextPrim = (CText*)pRectPrim->GetNext();

  CString path;

  int br = -1;
  int i, j;
  for( i = 2 ; br < 0 && --i >= 0 ; )
  {
    path = App.GetSubbmpPath( i );
    CString fullPath( path + *pTextPrim );
    br = _access( fullPath, 04 );
  }

  if( br < 0 )
    return 0;

  CString splitPath( path + "split" );
  path += *pTextPrim;
  WCHAR wbuf[_MAX_PATH];

  CWaitCursor wc;
  if( !MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, path, -1, wbuf, _MAX_PATH ))
    return 0;

  Bitmap* pBmp = 0;

  if( !( pBmp = Bitmap::FromFile( wbuf, 1 )))
    return 0;

  int SS = App.GetProfileInt( "MAP_SUBLAYER", "SplitSize", 0 );
  int H = pBmp->GetHeight();
  int W = pBmp->GetWidth();
  int nh = SplitNum( H, SS );
  int nw = SplitNum( W, SS );

  if( nw > 1 || nh > 1 )
  {
    ReportHint( "Дефрагментация..." );

    for( i = 0 ; i < nh ; ++i )
      for( j = 0 ; j < nw ; ++j )
      {
        CString sn( MakeSplitName( splitPath, path, i, j ));
        if( FileExist( sn ))
          CFile::Remove( sn );
      }
  }

  delete pBmp;
  ClearHint();
  return 1;
}
//======================================================================
int
CDrawObject::DrawSubl( CDC* pDC )
{
  CRectangle* pRectPrim = (CRectangle*)m_pOriginCont->m_pHead;
  CText* pTextPrim = (CText*)pRectPrim->GetNext();
  char const* pStr = *pTextPrim;

  CRect r( *pRectPrim );
  int ang = pRectPrim->GetAngle();
  CString path;

  int br = -1;
  int i, j;
  for( i = 3 ; br < 0 && --i >= 0 ; )
  {
    path = App.GetSubbmpPath( i );
    CString fullPath( path + pStr );
    br = _access( fullPath, 04 );
  }

  if( br < 0 )
    return 0;

  CString splitPath( path + "split" );
  path += pStr;
  WCHAR wbuf[_MAX_PATH];

  CWaitCursor wc;
  if( !MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, path, -1, wbuf, _MAX_PATH ))
    return 0;

  Bitmap* pBmp = Bitmap::FromFile( wbuf, 1 );

  if( !pBmp )
    return 0;

  int SS = App.GetProfileInt( "MAP_SUBLAYER", "SplitSize", 0 );
  int H = pBmp->GetHeight();
  int W = pBmp->GetWidth();

  CRect cr;

  pDC->GetClipBox( &cr );
  pDC->SetStretchBltMode( HALFTONE );

  pDC->LPtoDP( &cr );
  CRect sr( r );
  pDC->LPtoDP( &sr );

  int sdc = SetTM(  pDC );

  int srw = sr.Width();
  int srh = sr.Height();

  int bss = IsSublayerStatic();
        int nh = bss ? 1 : SplitNum( H, SS );
  int nw = bss ? 1 : SplitNum( W, SS );

  int bSuc = 1;
  if( nw > 1 || nh > 1 )
  {
    for( i = nh ; bSuc && --i >= 0 ; )
      for( j = nw ; bSuc && --j >= 0 ; )
        bSuc = FileExist( MakeSplitName( splitPath, path, i, j ));

    if( !bSuc )
      bSuc = !SplitSubl( 0, pBmp );

    CRect dr;
    dr.top = sr.top;
    for( i = 0 ; bSuc && i < nh ; ++i, dr.top = dr.bottom )
    {
      dr.bottom = sr.top + MulDiv( srh, i + 1, nh );
      dr.left = sr.left;
      for( j = 0 ; bSuc && j < nw ; ++j, dr.left = dr.right )
      {
        dr.right = sr.left + MulDiv( srw, j + 1, nw );
        CString spn( MakeSplitName( splitPath, path, i, j ));
        MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, spn, -1, wbuf, _MAX_PATH );

        Bitmap* pBmp = Bitmap::FromFile( wbuf, 1 );
                        if( !pBmp )
                                        continue;

        int H = pBmp->GetHeight();
        int W = pBmp->GetWidth();
                                CBitmap bmp;
        bmp.CreateCompatibleBitmap( pDC, W, H );
          CMemDC memDC( pDC, 1 );
                memDC.SelectObject( &bmp );
        Graphics Grh( memDC.m_hDC );
        Grh.SetPageUnit( UnitPixel );
        bSuc = Grh.DrawImage( pBmp, 0, 0, W, H ) == Ok;
        if( bSuc )
        {
          if( ang )
          {
            CPoint pp[] =
            {
              TurnPoint( dr.TopLeft(), sr.TopLeft(), ang ),
              TurnPoint( CPoint( dr.right, dr.top ), sr.TopLeft(), ang ),
              TurnPoint( CPoint( dr.left, dr.bottom ), sr.TopLeft(), ang ),
              TurnPoint( dr.BottomRight(), sr.TopLeft(), ang )
            };
            CBox box;
            for( int k = 4 ; --k >= 0 ; box |= CPoint( pp[k].x, pp[k].y ));
            if( Intersect( &cr, &box ))
            {
              CBitmap msk;
               pDC->PlgBlt( pp, &memDC, 0, 0, W, H, msk, 0, 0 );
            }
          }
          else
          if( Intersect( &cr, &dr ))
            pDC->StretchBlt( dr.left, dr.top, dr.Width(), dr.Height(), &memDC, 0, 0, W, H, SRCCOPY );
        }
        delete pBmp;
      }
    }
  }
  else
  {
    CBitmap bmp;
    bmp.CreateCompatibleBitmap( pDC, W, H );
    CMemDC memDC( pDC, 1 );
    memDC.SelectObject( &bmp );
    Graphics Grh( memDC.m_hDC );
    Grh.SetPageUnit( UnitPixel );
    bSuc = Grh.DrawImage( pBmp, 0, 0, W, H ) == Ok;
    if( bSuc )
    {
      if( ang )
      {
        CPoint pp[] =
        {
          sr.TopLeft(),
          TurnPoint( CPoint( sr.right, sr.top ),   sr.TopLeft(), ang ),
          TurnPoint( CPoint( sr.left, sr.bottom ), sr.TopLeft(), ang )
        };
        CBitmap msk;
        pDC->PlgBlt( pp, &memDC, 0, 0, W, H, msk, 0, 0 );
      }
      else
        pDC->StretchBlt( sr.left, sr.top, srw, srh, &memDC, 0, 0, W, H, SRCCOPY );
    }
  }

  pDC->RestoreDC( sdc );
  delete pBmp;
  return bSuc;
}
//=====================================================================
CDrawObject*
CDrawObject::Rebuild( ksi_obj ksi_par, int cls, int sts )
{
  ksi_obj ksi_img = *m_pOriginCont;
//  KsiCall( "tk-box", ksi_img, ksi_par, 0 );
  cls >= 0 || ( cls = Class());
  sts >= 0 || ( sts = Status());

  ksi_obj val = KsiCall( "rebuild-object",
                         KSI_MK_SINT( Class()),
                         KSI_MK_SINT( Status()),
                         KSI_MK_SINT( cls ),
                         KSI_MK_SINT( sts ),
                         ksi_par, ksi_img, 0 );
//  KsiCall( "tk-box", val, 0 );
  CDrawObject* pNew = KSI_PLIST_P( val ) ?
    new CDrawObject( val, CIdent( cls, sts, Table(), Entry()), Node()) : 0;
  if( pNew )
    pView->ScaleObject( pNew, pView->GetTrueScale());
  return pNew;
}
//=====================================================================
CDrawObject::operator ksi_obj()
{
//  return *m_pActualCont;
  return *m_pOriginCont;
}
//=====================================================================
CSmartObjPtr::~CSmartObjPtr()
{
  delete mPtr;
}
//=====================================================================
void*
CSmartObjPtr::operator=( CDrawObject* ptr )
{
  delete mPtr;
  return mPtr = ptr;
}
//=====================================================================
int
CDrawObject::operator<( CDrawObject& pObj )
{
  CPoint p0( GetOriginBox().CenterPoint());
  CPoint p1( pObj.GetOriginBox().CenterPoint());
  return p0.y < p1.y || p0.y == p1.y && p0.x < p1.x;
}
//========================================================================
