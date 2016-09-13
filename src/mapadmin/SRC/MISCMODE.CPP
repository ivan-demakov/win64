#include "stdafx.h"

#include "prndlg.h"
#include "dwin.h"
#include "util.h"
#include "ksiutil.h"
#include "miscmode.h"
#include "bldmode.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "mdoc.h"
#include "drawobj.h"
#include "dlg0.h"
#include "mathutil.h"
#include "mercator.h"
#include "pfmode.h"
#include "movemode.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//=====================================================================
inline void
DrawLine( CDC* pDC, int x0, int y0, int x1, int y1 )
{
  pDC->MoveTo( x0, y0 );
  pDC->LineTo( x1, y1 );
}
//=====================================================================
inline void
DrawLine( CDC* pDC, CPoint const& p0, CPoint const& p1 )
{
  pDC->MoveTo( p0 );
  pDC->LineTo( p1 );
}
//=====================================================================
inline void
DrawLine( CDC* pDC, CPoint const& p0, CPoint const& p1, int a, CSize off = CSize( 0, 0 ))
{
  pDC->MoveTo(( a ? TurnPoint( p0, a ) : p0 ) + off );
  pDC->LineTo(( a ? TurnPoint( p1, a ) : p1 ) + off );
}
//=== CBoundingMode ===================================================
int
CBoundingMode::OnMouseEvent( UINT& flags, CPoint& loc )
{
  loc.x = loc.x < 0 ? 0 : loc.x > m_Bnd.cx ? m_Bnd.cx : loc.x;
  loc.y = loc.y < 0 ? 0 : loc.y > m_Bnd.cy ? m_Bnd.cy : loc.y;
  return AM_RPROPAGATE;
}
//=====================================================================
//=== CCrossMode ==========================================================
void
CCrossMode::Draw( CDC* pDC, CMapView* pView, CPoint p )
{
  CRect r;
  pView->GetClientRect( &r );
  pDC->DPtoLP( &r );
  if( !r.PtInRect( p ))
    return;

  int a = pView->GetModeAngle();
  CBox box( a ? CBox( r, p, -a ) : r );
  CPoint p0( box.left - p.x, 0 ), p1( box.right - p.x, 0 );
  CPoint q0( 0, box.top - p.y ), q1( 0, box.bottom - p.y );
  CPen pen( PS_SOLID, 0, RGB( 128, 128, 0 ));
  CPen* pOldPen = pDC->SelectObject(& pen );
  int oldMode = pDC->SetROP2( R2_XORPEN );
  DrawLine( pDC, p0, p1, a, p );
  DrawLine( pDC, q0, q1, a, p );
  pDC->SelectObject( pOldPen );
  pDC->SetROP2( oldMode );
}
//=====================================================================
//=== CReisMode =======================================================
// m_nMode bit flags:
// 0x02 -- first point in seria is allready defined
// 0x08 -- point updating allowed
//=====================================================================
CPoint CReisMode::m_PrevPoint;
int    CReisMode::m_nActive = 0;
//=====================================================================
void
CReisMode::SetPrevPoint( CPoint p )
{
  m_PrevPoint = p;
  SetActive( 1, 1 );
}
//=====================================================================
void
CReisMode::SetActive( int bActive, int bFirstPointDefined )
{
  if( bActive )
  {
    m_nActive &= ~0x2;
    m_nActive |= bFirstPointDefined ? 0xA : 0x8;
  }
  else
    m_nActive &= ~0x8;
}
//=====================================================================
int
CReisMode::OnButtonUp( UINT& flags, CPoint& loc )
{
  m_nActive |= 2;
  return AM_RPROPAGATE;
}
//=====================================================================
void
CReisMode::UpdatePoint( CPoint& loc )
{
  if(( m_nActive & 2 ) && ( m_nActive & 8 ))
  {
    CPoint tp( TurnPoint( loc - m_PrevPoint, -m_pOwner->GetCrossAngle()));
    *( abs( tp.x ) > abs( tp.y ) ? &tp.y : &tp.x ) = 0;
    loc = m_PrevPoint + TurnPoint( tp, m_pOwner->GetCrossAngle());
  }
}
//=====================================================================
int
CReisMode::OnButtonDown( UINT& flags, CPoint& loc )
{
  UpdatePoint( loc );
  m_PrevPoint = loc;
  return AM_RPROPAGATE;
}
//=====================================================================
int
CReisMode::OnMouseMove( UINT& flags, CPoint& loc )
{
  UpdatePoint( loc );
  return AM_RPROPAGATE;
}
//=====================================================================
//=== CSetReisMode ====================================================
int
CSetReisMode::Open()
{
  if( m_pCM = m_pOwner->GetMode( IDM_CROSS_MODE ))
    m_pOwner->RemoveMode( m_pCM );

  m_OldAngle = m_pOwner->GetCrossAngle();
  m_pOwner->ShowAngle();
  return m_nMode = 1;
}
//=====================================================================
void
CSetReisMode::Draw( CDC* pDC )
{
  CCrossMode::Draw( pDC, m_pOwner, m_PrevPoint );
}
//=====================================================================
int
CSetReisMode::Term( int bDestroy )
{
  if( m_pCM )
    m_pOwner->AddMode( new CCrossMode );

  if( !bDestroy )
    m_pOwner->ShowSelectedNum();

  return 1;
}
//=====================================================================
int
CSetReisMode::OnRButtonDown( UINT& flags, CPoint& loc )
{
  m_nMode = 0;
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CSetReisMode::OnLButtonDown( UINT& flags, CPoint& loc )
{
  if( flags & MK_SHIFT )
  {
    int angle;
    if( m_pOwner->m_ClipData.GetAngle( m_pOwner->GetSpot(), &angle ))
    {
      angle %= 3600;
      m_pOwner->SetCrossAngle( angle <= -900 ? angle + 1800 :
                               angle >=  900 ? angle - 1800 :
                                               angle );
      m_pOwner->ShowAngle();
    }
    else
      Beep( 440, 500 );
  }
	else
	{
		if( m_nMode & 2 )
		{
      m_pOwner->SetCrossAngle( atan2( m_pOwner->GetPosition() - m_PrevPoint ));
      m_pOwner->ShowAngle();
		}
		else
      m_PrevPoint = m_pOwner->GetPosition();
		m_nMode ^= 2;
	}

  return AM_RDONE;
}
//=====================================================================
int
CSetReisMode::OnLButtonDblClk( UINT& flags, CPoint& loc )
{
  return OnLButtonUp( flags, loc );
}
//=====================================================================
int
CSetReisMode::OnMouseMove( UINT& flags, CPoint& loc )
{
	if( m_nMode & 2 ) 
  {
    m_pOwner->SetCrossAngle( atan2( m_pOwner->GetPosition() - m_PrevPoint ));
    m_pOwner->ShowAngle();
  }
  else
    m_PrevPoint = m_pOwner->GetPosition();
 return AM_RDONE;
}
//=====================================================================
int
CSetReisMode::OnChar( UINT& flags, UINT& key_code )
{
  switch( key_code )
  {
    case VK_ESCAPE :
      m_pOwner->SetCrossAngle( m_OldAngle );
      return AM_RREMOVE | AM_RDONE;
    case '0' :
      m_pOwner->SetCrossAngle( 0 );
      break;
    case 0x12 : // VK_LEFT
      m_pOwner->SetCrossAngle( m_pOwner->GetCrossAngle() + 1 );
      break;
    case 0x13 : // VK_RIGHT
      m_pOwner->SetCrossAngle( m_pOwner->GetCrossAngle() - 1 );
      break;
  }
  m_pOwner->ShowAngle();
  return AM_RDONE;
}
//=====================================================================
//=== CGridMode ==================================================
int CGridMode::m_bDrawGrid = 1;
double CGridMode::m_SCF = 1;
//=====================================================================
int
CGridMode::UpdateCoord( int coord, int step )
{
  return ( coord + ( coord >= 0 ? step : -step ) / 2 ) / step * step;
}
//=====================================================================
int
CGridMode::OnMouseEvent( UINT& flags, CPoint& loc )
{
  if( !m_bDoGrid || ( flags & MK_SHIFT ))
    return AM_RPROPAGATE;

  int a = m_pOwner->GetModeAngle();
  double vsin = sin( a ), vcos = cos( a );

  CSize xg( CGridDialog::GetXGrid());
  CSize yg( CGridDialog::GetYGrid());

  double x = loc.x - xg.cx;
  double y = loc.y - yg.cx;

  int rx = UpdateCoord( x * vcos - y * vsin, xg.cy * CGridMode::m_SCF );
  int ry = UpdateCoord( x * vsin + y * vcos, yg.cy * CGridMode::m_SCF  );

  loc.x =  rx * vcos + ry * vsin + xg.cx;
  loc.y = -rx * vsin + ry * vcos + yg.cx;

  if( loc == m_PrevPoint )
    return AM_RDONE;

  m_PrevPoint = loc;
  return AM_RPROPAGATE;
}
//=====================================================================
void
CGridMode::Draw( CDC* pDC )
{
  if( !m_bDrawGrid )
    return;

  if( !pDC->IsPrinting())
    pDC->GetClipBox( &m_PrintRect );

  if( m_Scale != m_pOwner->GetAltScale())
  {
    m_Xg = CGridDialog::GetXGrid();
    m_Yg = CGridDialog::GetYGrid();

    m_Xg.cy *= m_pOwner->GetScaleFactor();
    m_Yg.cy *= m_pOwner->GetScaleFactor();

    CSize ts( pView->DP2LP( CSize( 20, 20 )));
    ts.cx = ( ts.cx + ts.cx + m_Xg.cy ) / m_Xg.cy / 2;
    ts.cy = ( ts.cy + ts.cy + m_Yg.cy ) / m_Yg.cy / 2;
    m_Xg.cy = max( ts.cx, 1 ) * m_Yg.cy;
    m_Yg.cy = max( ts.cy, 1 ) * m_Yg.cy;
    m_Scale = m_pOwner->GetAltScale();
  }

  if( MerkatorData.TrueMerkator())
  {
    CPoint c( m_Xg.cx, m_Yg.cx );
		Point p0( c.x, c.y );
		Point p1( pView->m_Clip.left, pView->m_Clip.top );
		Point p2( pView->m_Clip.right, pView->m_Clip.bottom );

    m_SCF = MerkatorData.ScaleFactor( p0 );
    double ltf = MerkatorData.ScaleFactor( p1 );
    double brf = MerkatorData.ScaleFactor( p2 );
    int x0 = m_Xg.cy * m_SCF, x1 = m_Xg.cy * ltf, x2 = m_Xg.cy * brf;
    int y0 = m_Yg.cy * m_SCF, y1 = m_Yg.cy * ltf, y2 = m_Yg.cy * brf;
    m_bDoGrid = x0 == x1 && x0 == x2 && y0 == y1 && y0 == y2;
    if( !m_bDoGrid )
      return;
  }

  int a = m_pOwner->GetModeAngle();

  CBox box( pDC->IsPrinting() && a ? CBox( m_PrintRect, m_pOwner->m_PrintOrg, a ) : m_PrintRect );
  CSize cnt( m_Xg.cx, m_Yg.cx );
  box.OffsetRect( -cnt );
  if( a )
    box = CBox( box, CPoint( 0, 0 ), -a );

  CPen pen( PS_SOLID, 0, RGB( 191, 191, !pDC->IsPrinting() || CDocSetupPage::m_colorEnb ? 255 : 191 ));
  CPen* pOldPen = pDC->SelectObject( &pen );
  int oldMode = pDC->SetROP2( R2_COPYPEN );

  for( int x = UpdateCoord( box.left - m_Xg.cy, m_Xg.cy ) ; x < box.right ; x += m_Xg.cy )
    DrawLine( pDC, CPoint( x, box.top ), CPoint( x, box.bottom ), a, cnt );

  for( int y = UpdateCoord( box.top - m_Yg.cy, m_Yg.cy ) ; y < box.bottom ; y += m_Yg.cy )
    DrawLine( pDC, CPoint( box.left, y ), CPoint( box.right, y ), a, cnt );

  pDC->SetROP2( oldMode );
  pDC->SelectObject( pOldPen );
}
//=====================================================================
//==CCoordGridMode=====================================================
static void
CreateModeFont( CFont& nFont, CSize fs, int bVert )
{
  nFont.CreateFont( bVert ? fs.cy : fs.cx, 0,
                    bVert ? 900 : 0, bVert ? 900 : 0,
                    FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    DRAFT_QUALITY, DEFAULT_PITCH|FF_SCRIPT,
                    "ARIAL" );
}
//=====================================================================
void
CCoordGridMode::Draw( CDC* pDC )
{
  if( !pDC->IsPrinting())
  {
    m_pOwner->GetClientRect( &m_PrintRect );
    pDC->DPtoLP( &m_PrintRect );
  }

  CBox box( pDC->IsPrinting() ? CBox( m_PrintRect, m_pOwner->m_PrintOrg, m_pOwner->GetModeAngle() ) : m_PrintRect );

  Polar plt, prb;
  double dgs;
  char fm[16];

  int bMerk = MerkatorData.TrueMerkator() && CMainWin::m_bPolarCoord;

  int hs = 254;
  int vs = 254;
  int hr = pDC->GetDeviceCaps( LOGPIXELSX ) * 10;
  int vr = pDC->GetDeviceCaps( LOGPIXELSY ) * 10;
  int ks = pDC->IsPrinting() ? 12 : 8;
  CSize Fs( MulDiv( 32, vr, vs * ks ), MulDiv( pDC->IsPrinting() ? 32 : 28, hr, hs * ks ));
  pDC->DPtoLP( &Fs );

  if( bMerk )
  {
    MerkatorData.Prj2Pol( Point( box.left, box. top ), plt );
    MerkatorData.Prj2Pol( Point( box.right, box.bottom ), prb );
    double dLat = fabs( plt.lat - prb.lat ) / Deg;
    double dLon = fabs( plt.lon - prb.lon ) / Deg;
    int pov = min( log10( dLat ), log10( dLon ));
    dgs = Deg * pow( 10., pov );
    sprintf( fm, "%%1.%df\xb0", pov >= 0 ? 0 : -pov );
  }
  else
  if( m_Scale != m_pOwner->GetAltScale())
  {
    m_Xg = abs( CMapView::m_XStep ) * 100 / SmInUnit;
    m_Yg = abs( CMapView::m_YStep ) * 100 / SmInUnit;
    CSize ts( pView->DP2LP( CSize( 50, 50 )));
    ts.cx = ( ts.cx + ts.cx + m_Xg ) / m_Xg / 2;
    ts.cy = ( ts.cy + ts.cy + m_Yg ) / m_Yg / 2;
    m_Xg = max( ts.cx, 1 ) * m_Yg;
    m_Yg = max( ts.cy, 1 ) * m_Yg;
    m_Scale = m_pOwner->GetAltScale();
  }

  COLORREF clr = RGB( 0, 0, 128 );
  CPen pen( PS_SOLID, 0, clr );
  CPen* pOldPen = pDC->SelectObject( &pen );
  int oldMode = pDC->SetROP2( R2_COPYPEN );

  pDC->SetBkMode( OPAQUE );
  pDC->SetBkColor( 0xffffff );
  pDC->SetTextColor( clr );

  CPoint cg( CMapView::Project2Grid( box.TopLeft()));
  CPoint eg( CMapView::Project2Grid( box.BottomRight()));
  int rx = cg.x > eg.x;
  int ry = cg.y > eg.y;

  int bShowTxt = !pDC->IsPrinting() || !m_pOwner->GetModeAngle();

  for( int bdl = 2 ; --bdl >= 0 ; )
  {
    CRect r( cg, eg );
    r.NormalizeRect();
    CFont nFont;
    CFont* pOldFont;
    char b[32];

    if( bShowTxt && !bdl )
    {
      CreateModeFont( nFont, Fs, 0 );
      pOldFont = pDC->SelectObject( &nFont  );
    }

    if( bMerk )
    {
      for( double lon = int( plt.lon / dgs ) * dgs ; lon < prb.lon ; lon += dgs )
      {
        Point pp;
        MerkatorData.Pol2Prj( Polar( plt.lat, lon ), pp );
        if( bdl )
          DrawLine( pDC, pp.x, box.top, pp.x, box.bottom );
        else
        if( bShowTxt )
        {
          int bl = sprintf( b, fm, lon / Deg );
          int w = pDC->GetTextExtent( b, bl ).cx;
          pDC->SetTextAlign( TA_CENTER | TA_TOP | TA_NOUPDATECP );
          pDC->TextOut( pp.x, box.top, b, bl );
          pDC->SetTextAlign( TA_CENTER | TA_BOTTOM | TA_NOUPDATECP );
          pDC->TextOut( pp.x, box.bottom, b, bl );
        }
      }
    }
    else
    {
      int l = 0;
      for( r.left = r.left / m_Xg * m_Xg ; r.left <= r.right ; r.left += m_Xg )
      {
        CPoint pp( CMapView::Grid2Project( r.TopLeft()));
        if( bdl )
          DrawLine( pDC, pp.x, box.top, pp.x, box.bottom );
        else
        if( bShowTxt )
        {
          int v = abs( r.left );
          int bl = sprintf( b, "%1.3f", r.left * 1e-5 );
          int w = pDC->GetTextExtent( b, bl ).cx;
          if( l == 0 || ( rx ? pp.x + w <= l : pp.x - w >= l ))
          {
            pDC->SetTextAlign( TA_CENTER | TA_TOP | TA_NOUPDATECP );
            pDC->TextOut( pp.x, box.top, b, bl );
            pDC->SetTextAlign( TA_CENTER | TA_BOTTOM | TA_NOUPDATECP );
            pDC->TextOut( pp.x, box.bottom, b, bl );
            l = rx ? pp.x : pp.x + w;
          }
        }
      }
    }

    if( bShowTxt && !bdl )
    {
      pDC->SelectObject( pOldFont )->DeleteObject();
      CreateModeFont( nFont, Fs, 1 );
      pOldFont = pDC->SelectObject( &nFont  );
    }

    if( bMerk )
    {
      for( double lat = int( prb.lat / dgs ) * dgs ; lat <= plt.lat ; lat += dgs )
      {
        Point pp;
        MerkatorData.Pol2Prj( Polar( lat, prb.lon ), pp );
        if( bdl )
          DrawLine( pDC, box.left, pp.y, box.right, pp.y );
        else
        if( bShowTxt )
        {
          int v = abs( r.top );
          int bl = sprintf( b, fm, lat / Deg );
          pDC->SetTextAlign( TA_CENTER | TA_TOP | TA_NOUPDATECP );
          pDC->TextOut( box.left, pp.y, b, bl );
          pDC->SetTextAlign( TA_CENTER | TA_BOTTOM | TA_NOUPDATECP );
          pDC->TextOut( box.right, pp.y, b, bl );
        }
      }
    }
    else
    {
      int t = 0;
      for( r.top = r.top / m_Yg * m_Yg ; r.top <= r.bottom ; r.top += m_Yg )
      {
        CPoint pp( CMapView::Grid2Project( r.TopLeft()));
        if( bdl )
          DrawLine( pDC, box.left, pp.y, box.right, pp.y );
        else
        if( bShowTxt )
        {
          int bl = sprintf( b, "%1.3f", r.top * 1e-5 );
          int w = pDC->GetTextExtent( b, bl ).cx;
          if( t == 0 || ( ry ? pp.y + w <= t : pp.y - w >= t ))
          {
            pDC->SetTextAlign( TA_CENTER | TA_TOP | TA_NOUPDATECP );
            pDC->TextOut( box.left, pp.y, b, bl );
            pDC->SetTextAlign( TA_CENTER | TA_BOTTOM | TA_NOUPDATECP );
            pDC->TextOut( box.right, pp.y, b, bl );
            t = ry ? pp.y : pp.y + w;
          }
        }
      }
    }
    if( bShowTxt && !bdl )
      pDC->SelectObject( pOldFont )->DeleteObject();
  }

  pDC->SetROP2( oldMode );
  pDC->SelectObject( pOldPen );
}
//=====================================================================
//=== CScaleMode ======================================================
void
CScaleBase::CalcScale()
{
  int Wx = m_pOwner->GetWindowSize().cx;
  int W = abs( Width()) * 2;
  int cscale = m_pOwner->m_AltScale;
  int bInc  = m_nCursor == IDC_INCSCALE;
  m_CurScale = W == 0 ? ( bInc ? cscale / 2 : cscale * 2 ) :
               bInc   ?  min( cscale, ::MulDiv( cscale, W, Wx )) :
                         max( cscale, ::MulDiv( cscale, Wx, W ));
  m_CurScale = min( MinScale, max( MaxScale, m_CurScale ));
}
//=====================================================================
void
CScaleBase::SetRB()
{
  CRectMode::SetRB( m_pOwner->GetPosition());
  int dx = abs( Width());
  int dy = abs( Height());
	if( m_nCursor == IDC_DECSCALE )
	{
    dx = max( dx, m_pOwner->GetWindowSize().cx / m_pOwner->m_DimScaleRatio / 2 );
    dy = max( dy, m_pOwner->GetWindowSize().cy / m_pOwner->m_DimScaleRatio / 2 );
	}
  CSize const& vs = m_pOwner->GetViewportSize();
  int w = ::MulDiv( vs.cx, dy, vs.cy );
  int h = ::MulDiv( vs.cy, dx, vs.cx );
  right  = left + max( w, dx );
  bottom = top  + max( h, dy );

  CalcScale();
}
//=====================================================================
int
CScaleBase::SetScale()
{
  CalcScale();
  if( m_CurScale == m_pOwner->m_AltScale )
    return AM_RDONE;

  if( IsRectEmpty())
  {
    int bInc = m_CurScale < m_pOwner->m_AltScale;
    int num = bInc ? 1 : 2;
    int den = bInc ? 4 : 2;
    CSize ss( m_pOwner->m_Position - m_pOwner->m_Clip.TopLeft());
    ss = ss + ss - m_pOwner->m_WindowSize;
    ss.cx = ::MulDiv( ss.cx, num, den );
    ss.cy = ::MulDiv( ss.cy, num, den );
    m_pOwner->SetScale( m_CurScale, m_pOwner->m_Position - ss, 1 );
  }
  else
    m_pOwner->SetScale( m_CurScale, TopLeft(), 1 );

  m_pOwner->UpdateWindow();
  return AM_RDONE;
}
//=====================================================================
void
CScaleBase::Draw( CDC* pDC )
{
  CRectMode::Draw( pDC, 1 );
}
//=====================================================================
int
CScaleMode::OnLButtonDown( UINT& flags, CPoint& )
{
  if( !( flags & MK_CONTROL ) ||
      ( flags & MK_SHIFT ) ||
      ( flags & MK_LBUTTON ) && ( flags & MK_RBUTTON ))
    return AM_RPROPAGATE;

  SetLT( m_pOwner->GetPosition());
  m_nMode = flags;
  m_nCursor = m_nMode & MK_LBUTTON ? IDC_INCSCALE : IDC_DECSCALE;
  OnSetCursor();
  CalcScale();
  return AM_RDONE;
}
//=====================================================================
int CScaleMode::OnRButtonDown( UINT& flags, CPoint& loc )
{
  return OnLButtonDown( flags, loc );
}
//=====================================================================
int
CScaleMode::OnButtonUp( UINT sFlag )
{
  if( !m_nMode )
    return AM_RPROPAGATE;

  int m = m_nMode;
  m_nMode = 0;
  return m == sFlag ? SetScale() : AM_RPROPAGATE;
}
//=====================================================================
int
CScaleMode::OnLButtonUp( UINT& flags, CPoint& )
{
  return OnButtonUp( flags | MK_LBUTTON );
}
//=====================================================================
int
CScaleMode::OnRButtonUp( UINT& flags, CPoint& )
{
  return OnButtonUp( flags | MK_RBUTTON );
}
//=====================================================================
int
CScaleMode::OnMouseMove( UINT& flags, CPoint& loc )
{
  if( m_nMode == flags )
    SetRB();
  else
    m_nMode = 0;
  return AM_RPROPAGATE;
}
//=====================================================================
int
CScaleMode::OnChar( UINT& flags, UINT& key_code )
{
  UINT ml[] = { IDM_DRAG_MODE, IDM_MOVE_MODE, IDM_CONNECT_MODE, 0 };
  if( !m_pOwner->GetBuilding( 0 ) && !m_pOwner->GetActiveMode( ml ))
    switch( key_code )
    {
      case '-':
      case '+': m_pOwner->ValidateRect( 0 );
                m_pOwner->SetScale( key_code == '+' ?
                  m_pOwner->GetAltScale() / 2 : m_pOwner->GetAltScale() * 2,
                  CPoint( -1, -1 ), 1 );
      return AM_RDONE;
    }
  return AM_RPROPAGATE;
}
//=====================================================================
//=====================================================================
int
CChangeScaleMode::OnLButtonDown( UINT& flags, CPoint& )
{
  m_nMode = 1;
  SetLT( m_pOwner->GetPosition());
  CalcScale();
  return AM_RDONE;
}
//=====================================================================
int
CChangeScaleMode::OnLButtonUp( UINT& flags, CPoint& )
{
  m_nMode = 0;
  return SetScale();
}
//=====================================================================
int
CChangeScaleMode::OnRButtonDown( UINT& flags, CPoint& )
{
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CChangeScaleMode::OnMouseMove( UINT& , CPoint&  )
{
  if( m_nMode )
    SetRB();
  return AM_RDONE;
}
//=====================================================================
//=== CDragMode =======================================================
int
CDragMode::OnChar( UINT& flags, UINT& key_code )
{
  if( !m_nMode || key_code != VK_ESCAPE )
    return AM_RPROPAGATE;

  m_nMode = 0;
  m_pOwner->InvalidateRect( 0 );
  return m_nID == IDM_DRAG_MODE ? AM_RDONE : AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CDragMode::OnLButtonDown( UINT& flags, CPoint& loc )
{
  if( flags & MK_SHIFT )
    return AM_RPROPAGATE;

  ksi_kill_text_cursor();
  m_Delta = ( 0, 0 );
  m_OrgPos = m_pOwner->LP2DP( loc );
  m_nMode = 1 + ( GetKeyState( VK_MENU ) < 0 );
  OnSetCursor();

  if( m_pOwner->m_pCurSelObj )
    m_pOwner->KillTimer( IDM_BLINKSELTIMER );

  return AM_RDONE;
}
//=====================================================================
int
CDragMode::OnLButtonUp( UINT& flags, CPoint& loc )
{
  if( !m_nMode )
    return m_nID == IDM_DRAG_MODE ? AM_RPROPAGATE : AM_RDONE;

  CViewDC dc( m_pOwner );
  dc.DPtoLP( &m_Delta );

  m_pOwner->SetScale( m_pOwner->GetAltScale(), m_pOwner->GetCenter() - m_Delta, 1 );
  m_nMode = 0;
  if( m_pOwner->m_pCurSelObj )
    m_pOwner->SetTimer( IDM_BLINKSELTIMER, 500, 0 );
  return AM_RDONE;
}
//=====================================================================
int
CDragMode::OnMouseMove( UINT& flags, CPoint& loc )
{
  if( !m_nMode )
    return m_nID == IDM_DRAG_MODE ? AM_RPROPAGATE : AM_RDONE;
  else
  if( !( flags & MK_LBUTTON ))
    return OnLButtonUp( flags, loc );

  m_Delta = m_pOwner->LP2DP( loc ) - m_OrgPos;

  CRect r1;
  m_pOwner->GetClientRect( &r1 );
  CRect r2( r1 );
  CRect r0( r1 + m_Delta );
  if( m_Delta.cx > 0 )
    r1.right = r2.left = r0.left;
  else
    r2.right = r1.left = r0.right;
  if( m_Delta.cy > 0 )
    r2.bottom = r0.top;
  else
    r2.top = r0.bottom;

  CViewDC dc( m_pOwner );
  SetTM( &dc );
  CMemDC memDC( &dc, 1 );
  memDC.SelectObject( &m_pOwner->m_Bitmap );
  dc.BitBlt( r0.left, r0.top, r0.Width(), r0.Height(), &memDC, 0, 0, SRCCOPY );
  dc.FillSolidRect( &r1, RGB( -1, -1, -1 ));
  dc.FillSolidRect( &r2, RGB( -1, -1, -1 ));
  return AM_RDONE;
}
//=====================================================================
//=== CHandMode =======================================================
int
CHandMode::OnRButtonDown( UINT& flags, CPoint& loc )
{
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CHandMode::OnRButtonUp( UINT& flags, CPoint& loc )
{
  return AM_RDONE;
}
//=====================================================================
#if 0
//=== CAutoScrollMode =================================================
int
CAutoScrollMode::Term( int bDestroy )
{
  if( m_bTimerOn )
  {
    m_pOwner->KillTimer( IDM_AUTOSCROLLTIMER );
    m_bTimerOn = 0;
  }
  return 0;
}
//=====================================================================
static int
CalcDelta( int p, int w )
{
  int d = w / 16;
  return p < 0 ? min( -p, d ) : p >= w ? max( w - p, -d ) : 0;
}
//=====================================================================
int
CAutoScrollMode::OnMouseMove( UINT& flags, CPoint& loc )
{
  static UINT mds[] = { IDM_INCSCALE_MODE, IDM_DECSCALE_MODE,
                        IDM_DRAG_MODE, IDM_SCALE_MODE, IDM_MOVE_MODE,
                        IDM_SELECT_MODE, IDM_GETSELECT_MODE,
                        IDM_ADDSELECT_MODE, IDM_SUBSELECT_MODE, 0 };
  {
    m_Delta = m_pOwner->LP2DP( loc );
    m_Delta.cx = CalcDelta( m_Delta.cx, m_pOwner->GetViewportSize().cx );
    m_Delta.cy = CalcDelta( m_Delta.cy, m_pOwner->GetViewportSize().cy );
    int bUndo = !m_pOwner->GetBuilding( 0 ) && !m_pOwner->GetActiveMode( mds );

    if(( m_Delta.cx || m_Delta.cy ) && m_bKeyPressed )
    {
      if( !m_bTimerOn )
      {
        m_pOwner->SetTimer( IDM_AUTOSCROLLTIMER, 100, 0 );
        m_bTimerOn = 1;
        m_pUndoItem = new CUndoScaleItem( m_pOwner );
      }
    }
    else
    if( m_bTimerOn || !m_bKeyPressed )
    {
      m_pOwner->KillTimer( IDM_AUTOSCROLLTIMER );
      if( bUndo && m_pUndoItem )
        m_pOwner->m_Undo.AddItem( m_pUndoItem, m_pOwner );
      m_pUndoItem = 0;
      m_bTimerOn = 0;
    }
  }
  return AM_RPROPAGATE;
}
//=====================================================================
int
CAutoScrollMode::OnTimer( UINT& nIDEvent )
{
  if( nIDEvent != IDM_AUTOSCROLLTIMER )
    return AM_RPROPAGATE;

  CSize ds( m_pOwner->DP2LP( m_Delta ));
  m_pOwner->SetScale( m_pOwner->GetTrueScale(), m_pOwner->GetCenter() - ds, 1 );
  m_pOwner->UpdateWindow();

  return AM_RPROPAGATE;
}
//=====================================================================
#endif
//=====================================================================
int
CSetCoordGridMode::Term( int bDestroy )
{
  m_pOwner->ShowSelectedNum();
  return 0;
}
//=====================================================================
int
CSetCoordGridMode::OnRButtonDown( UINT& flags, CPoint& loc )
{
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CSetCoordGridMode::OnLButtonDown( UINT& flags, CPoint& loc )
{
  m_Pt = m_pOwner->GetPosition();
  return AM_RDONE;
}
//=====================================================================
int
CSetCoordGridMode::OnLButtonUp( UINT& flags, CPoint& loc )
{
  if( m_Pt != m_pOwner->GetPosition())
    return AM_RDONE;

  if( m_bCgm )
  {
    CCoordGridDialog dlg( m_Pt );
    if( DoModalDlg( &dlg, CE_PAR_DLG ) == IDOK )
    {
      int xsg = dlg.GetGridDir().x ? -1 : 1;
      int ysg = dlg.GetGridDir().y ? -1 : 1;
      CMapView::m_X0 = dlg.GetGridPoint().x - dlg.GetProjPoint().x * xsg;
      CMapView::m_Y0 = dlg.GetGridPoint().y - dlg.GetProjPoint().y * ysg;
      CMapView::m_XStep = dlg.GetGridStep().x * xsg;
      CMapView::m_YStep = dlg.GetGridStep().y * ysg;
      CCoordGridMode* pGM = (CCoordGridMode*)m_pOwner->GetMode( IDM_COORDGRID_MODE );
      if( pGM )
      {
        m_pOwner->RemoveMode( pGM );
        m_pOwner->AddMode( new CCoordGridMode());
      }
    }
  }
  else
  {
    CGridDialog dlg( m_pOwner );
    if( DoModalDlg( &dlg, CE_PAR_DLG ) == IDOK )
    {
      CPoint loc( m_pOwner->GetPosition());
      CGridDialog::m_nOffsetX = loc.x;
      CGridDialog::m_nOffsetY = loc.y;
      CGridMode* pGM = (CGridMode*)m_pOwner->GetMode( IDM_GRID_MODE );
      if( pGM )
      {
        m_pOwner->RemoveMode( pGM );
        m_pOwner->AddMode( new CGridMode());
      }
    }
  }
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
//=====================================================================
int
CSelectOverviewMode::OnLButtonDown( UINT &flags, CPoint &loc )
{
  if(( m_nMode ^= 1 ) & 1 )
  {
    SetLT( loc );
    return AM_RDONE;
  }
  else
  {
    SetRB( loc );
    m_pDlg->Refresh( this );
    return AM_RREMOVE | AM_RDONE;
  }
}
//=====================================================================
int
CSelectOverviewMode::OnMouseMove( UINT &flags, CPoint &loc )
{
  if( m_nMode & 1 )
    SetRB( loc );
  return AM_RDONE;
}
//=====================================================================

