#include "stdafx.h"

#include "mview.h"
#include "miscmode.h"
#include "measmode.h"
#include "util.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
static int
GetLinkPoint( CMapView* pView, CTabIdent const& id, CPoint* pPoint )
{
  CDrawObject* pOb0 = pView->GetObjectPtr( id );
  CDrawObject* pObj = pOb0 ? pOb0 : pView->ExtractMapObj( id );

  int r = 0;
  if( pObj )
  {
    CPoint* pPnt = pObj->GetPoint();
    if( pPnt )
    {
      *pPoint = *pPnt;
      r = 1;
    }
    else
    if( pObj->GetPolygon())
    {
      for( CVisiblePrim* pp = pObj->GetActualCont()->GetHead() ; pp && pp->GetType() != KEY_TEXT ; pp = pp->GetNext());
      if( pp )
      {
        CBox box;
        pp->CalcBoundBox( box );
        *pPoint = box.CenterPoint();
        r = 1;
      }
    }
  }
  if( !pOb0 )
    delete pObj;
  return r;
}
//=====================================================================
CRect
CFuncLink::GetBox() const
{
  CRect r( m_Pt[0], m_Pt[1] );
  r.NormalizeRect();
  return r;
}
//=====================================================================
int
CFuncLink::DefPoints( CMapView* pView )
{
  return GetLinkPoint( pView, m_Id0, &m_Pt[0] ) &&
         GetLinkPoint( pView, m_Id1, &m_Pt[1] );
}
//=====================================================================
void
CFuncLink::Show( CMapView* pView, int bShow )
{
  if( bShow )
  {
    CRect box( pView->LP2DP( GetBox()));
    box.InflateRect( 20, 20 );
    pView->InvalidateRect( &box, 0 );
  }
}
//=====================================================================
int
CMapView::AddFuncLink( CFuncLink& funcLink )
{
  if( !funcLink.DefPoints( this ))
    return 0;

  m_bRedraw = m_bFuLinksShow;

  for( int i = m_FuLinkArray.GetSize() ; --i >= 0 && ( funcLink != m_FuLinkArray[i] ); );
  if( i < 0 )
    i = m_FuLinkArray.Add( funcLink );
  else
  {
    m_FuLinkArray[i].Show( this, m_bFuLinksShow  );
    m_FuLinkArray.SetAt( i, funcLink );
  }

  m_FuLinkArray[i].Show( this, m_bFuLinksShow );

  return 1;
}
//=====================================================================
int
CMapView::RemFuncLink( CFuncLink& funcLink )
{
  if( !funcLink.DefPoints( this ))
    return 0;

  for( int i = m_FuLinkArray.GetSize() ; --i >= 0 && ( funcLink != m_FuLinkArray[i] ); );
  if( i < 0 )
    return 0;

  funcLink = m_FuLinkArray[i];
  m_FuLinkArray.RemoveAt( i );

  m_bRedraw = m_bFuLinksShow;
  funcLink.Show( this, m_bFuLinksShow );

  return 1;
}
//=====================================================================
void
CMapView::RemoveAllFuncLinks()
{
  HideFuncLinks();
  m_FuLinkArray.RemoveAll();
}
//=====================================================================
void
CMapView::HideFuncLinks()
{
  m_bFuLinksShow = 0;
  m_bRedraw = 1;
  for( int i = m_FuLinkArray.GetSize() ; --i >= 0 ; m_FuLinkArray[i].Show( this ));
  UpdateWindow();
}
//=====================================================================
void
CMapView::ShowFuncLinks( int i )
// i >= 0 - Show at i
// i <  0 - Show all
{
  int s = m_bFuLinksShow;
  m_bFuLinksShow = 1;
  int  n = m_FuLinkArray.GetSize();
  if( !n || i >= n )
    return;

  CBox box;
  if( i < 0 )
    for( i = n ; --i >= 0 ; box |= m_FuLinkArray[i].GetBox());
  else
    box = m_FuLinkArray[i].GetBox();

  SetScale( 2 * WindowSize2Scale( box.Size()), box.CenterPoint());
  UpdateWindow();

  if( !s )
  {
    m_bRedraw = 1;
    for( i = n ; --i >= 0 ; m_FuLinkArray[i].Show( this ));
    UpdateWindow();
  }
}
//=====================================================================
void
CMapView::DrawFuncLinks( CDC* pDC )
{
  if( m_bFuLinksShow )
    for( int i = m_FuLinkArray.GetSize() ; --i >= 0 ; m_FuLinkArray[i].Draw( pDC ));
}
//=====================================================================
void
CMapView::RedrawFuncLink( CTabIdent id )
{
  for( int i = m_FuLinkArray.GetSize() ; --i >= 0 ; )
    if( m_FuLinkArray[i].IsMyObject( id ))
    {
      CFuncLink funcLink = m_FuLinkArray[i];
      m_FuLinkArray.RemoveAt( i );
      m_bRedraw = m_bFuLinksShow;
      funcLink.Show( this, m_bFuLinksShow );
      AddFuncLink( funcLink );
    }
}
//=====================================================================
void
CFuncLink::Draw( CDC* pDC )
{
  CRect box( GetBox());
  CSize s( 20, 20 );
  pDC->DPtoLP( &s );
  box.InflateRect( s );
  if( !pDC->RectVisible( &box ))
    return;

  SetGraphicsMode( pDC->GetSafeHdc(), GM_ADVANCED );

  int k = 1;
  CPoint p0( m_Pt[0] ), p1( m_Pt[1] );
  XFORM tf;
  int bRot = pDC->IsPrinting() && pView->GetModeAngle();
  if( bRot )
  {
    GetWorldTransform( pDC->GetSafeHdc(), &tf );
    p0.x = m_Pt[0].x * tf.eM11 + m_Pt[0].y * tf.eM21 + tf.eDx;
    p0.y = m_Pt[0].x * tf.eM12 + m_Pt[0].y * tf.eM22 + tf.eDy;
    p1.x = m_Pt[1].x * tf.eM11 + m_Pt[1].y * tf.eM21 + tf.eDx;
    p1.y = m_Pt[1].x * tf.eM12 + m_Pt[1].y * tf.eM22 + tf.eDy;
  }

  pDC->LPtoDP( &p0 );
  pDC->LPtoDP( &p1 );

  int d = dist( p0, p1 );
  if( d < k * 80 )
    return;

  int sdc = SetTM( pDC );

  CPen pen( PS_SOLID, k * 3, m_Color );
  pDC->SetROP2( R2_COPYPEN );
  pDC->SelectObject( &pen );

  if( bRot )
    SetIdenticalTransform( pDC );

  pDC->MoveTo( p0 );
  pDC->LineTo( p1);

  CRect r( p0, p1 );
  CPoint cp( r.CenterPoint());
  int a = atan2( r.Size());

  if( !m_Subscr.IsEmpty())
  {
    CFont nFont;
    nFont.CreateFont( k * 16, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET,
                      OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                      DRAFT_QUALITY, DEFAULT_PITCH|FF_SCRIPT,
                      "ARIAL" );
    CFont* pOldFont = pDC->SelectObject( &nFont  );
    CSize ext( pDC->GetTextExtent( m_Subscr, m_Subscr.GetLength()));
    int rw = ext.cx + k * 16;
    if( rw + k * 80 <= d )
    {
      int ang = a > 900 ? a - 1800 : a < -900 ? a + 1800 : a;
      double co = cos( ang ), si = sin( ang );
      XFORM tf = { co, -si, si, co, cp.x, cp.y };
      SetWorldTransform( pDC->GetSafeHdc(), &tf );
      int h0 = k * 10;
      int x1 = -rw /2, y1 = -h0, x2 = x1 + rw, y2 = h0, r = h0 * 2;
      pDC->SelectStockObject( WHITE_BRUSH );
      pDC->RoundRect( x1, y1, x2, y2, r, r );
      pDC->SetBkMode( TRANSPARENT );
      pDC->SetTextAlign( TA_CENTER | TA_BASELINE | TA_NOUPDATECP );
      pDC->SetTextColor( 0 );
      pDC->TextOut( 0, k * 4, m_Subscr, m_Subscr.GetLength() );
    }
  }

  if( m_ArrowMask & 3 )
  {
    CPoint ar[] = { CPoint ( 0, 0 ), CPoint ( k * 20, k * -5 ), CPoint ( k * 16, 0 ), CPoint ( k * 20, k * 5 )};
    CBrush br( m_Color );
    pDC->SelectObject( &br );
    pDC->SelectStockObject( NULL_PEN );
    double co = cos( a ), si = sin( a );
    if( m_ArrowMask & 1 )
    {
      XFORM tf = { co, -si, si, co, p0.x, p0.y };
      SetWorldTransform( pDC->GetSafeHdc(), &tf );
      pDC->Polygon( ar, CELEM( ar ));
    }
    if( m_ArrowMask & 2 )
    {
      XFORM tf = { -co, si, -si, -co, p1.x, p1.y };
      SetWorldTransform( pDC->GetSafeHdc(), &tf );
      pDC->Polygon( ar, CELEM( ar ));
    }
  }

  pDC->RestoreDC( sdc );
}
//=====================================================================
int
CFuncLink::IsMyObject( CTabIdent id )
{
  return id == m_Id0 || id == m_Id1;
}
//=====================================================================
CString const*
CFuncLink::Detected( CSpot const &spot )
{
  int k = 6;
  return spot.DetectPolyline( m_Pt, 2, -k, k ) ? &m_Topscr : 0;
}
//=====================================================================
void
CMapView::TestFLTopscr( CPoint* pLoc )
{
  static int const FH = 14;
  static LOGFONT MsgFnt = { FH, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, OUT_TT_PRECIS,
                            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial" };
  static CPoint lastLoc;
  static int bScrShown = 0;
  static int bTimerOn = 0;
  static CRect flRect;

  if( bTimerOn )
  {
    KillTimer( IDM_FULINKTIMER );
    bTimerOn = 0;
  }

  if( pLoc )
  {
    if( bScrShown )
    {
      flRect.InflateRect( 5, 5 );
      InvalidateRect( &flRect, 0 );
      bScrShown = 0;
    }
    SetTimer( IDM_FULINKTIMER, 500, 0 );
    lastLoc = *pLoc;
    bTimerOn = 1;
    return;
  }

  CString const* pScr = 0;
  CSpot spot( DP2LP( lastLoc ), 1 );
  if( m_bFuLinksShow )
    for( int i = m_FuLinkArray.GetSize() ; !pScr && --i >= 0 ; pScr = m_FuLinkArray[i].Detected( spot ));

  if( !pScr )
    for( POSITION pos = m_SelectList.GetTailPosition() ; !pScr && pos ;
         pScr = m_SelectList.GetPrev( pos )->DetectedSpecial( spot ));

	CString ObjStr;
  if( !pScr )
	{
		CDrawObject* pObj = GetDetectedObject( 1 );
		if( pObj )
			if( pObj->IsSpecSelected())
			  pScr = &pObj->GetSpecSelectText();
			else
			if( m_bObjToolTips )
			{
  			ObjStr = m_pObjDef->GetName( pObj->Class());
			  for( CVisiblePrim* p = pObj->GetActualCont()->GetHead() ; p ; p = p->GetNext())
				  if( p->IsAnyVisible() && p->GetText())
					{
						ObjStr += "\n";
						ObjStr += p->GetText();
					}				
  			pScr = &ObjStr;
			}
	}

  if( pScr && !pScr->IsEmpty())
  {
    char const* ps = *pScr;
    CSize S( 0, 10 );
    CClientDC dc( this );
    CFont f;
    f.CreateFontIndirect( &MsgFnt );
    dc.SelectObject( &f );
    char const* p0 = ps;
    for( char const* p1 = p0 ; *p1 ; p0 = p1 + 1 )
    {
      ( p1 = strchr( p0, '\n' )) || ( p1 = strchr( p0, '\0' ));
      CSize ext( dc.GetOutputTextExtent( p0, p1 - p0 ));
      S.cx = max( S.cx, ext.cx );
      S.cy += FH;
    }
    S.cx += 10;
    flRect.SetRect( lastLoc.x - S.cx, lastLoc.y - S.cy, lastLoc.x, lastLoc.y );
    int dx = flRect.left < 0 ? -flRect.left : 0;
    int dy = flRect.top < 0 ? -flRect.top : 0;
    flRect.OffsetRect( dx, dy );

    CBrush brush( RGB( -1, -1, 196 ));
    dc.SelectObject( &brush );
    dc.SelectStockObject( BLACK_PEN );
    dc.RoundRect( &flRect, CPoint( 10, 10 ));

    dc.SetTextColor( 0 );
    dc.SetBkMode( TRANSPARENT );
    dc.SetTextAlign( TA_LEFT | TA_TOP | TA_NOUPDATECP );

    CPoint tp( flRect.left + 5, flRect.top + 5 );

    for( p0 = ps, p1 = p0 ; *p1 ; p0 = p1 + 1 )
    {
      ( p1 = strchr( p0, '\n' )) || ( p1 = strchr( p0, '\0' ));
      dc.TextOut( tp.x, tp.y, p0, p1 - p0 );
      tp.y += FH;
    }
    bScrShown = 1;
  }
}
//=====================================================================
