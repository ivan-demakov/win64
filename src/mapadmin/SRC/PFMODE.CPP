#include "stdafx.h"

#include "mdoc.h"
#include "mview.h"
#include "dwin.h"
#include "util.h"
#include "mathutil.h"
#include "pfmode.h"
#include "prndlg.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "scale.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
int CPrintMapMode::m_bAutoSize;
//=====================================================================
int
CPrintMapMode::Init( CMapView* pOwner )
{
  CWaitCursor wait;
  CPrintInfo printInfo;
  printInfo.m_bPreview = 1;
  printInfo.m_pPD->m_pd.hDevMode  = App.GetDevMode();
  printInfo.m_pPD->m_pd.hDevNames = App.GetDevNames();

  m_pOwner = pOwner;
  pOwner->OnPreparePrinting( &printInfo );
  HDC hDC = printInfo.m_pPD ? printInfo.m_pPD->m_pd.hDC : 0;
  if( !hDC )
    ErrBox( "Принтер недоступен!" );
  else
  {
    CDC dcPrint;
    dcPrint.m_bPrinting = 1;
    dcPrint.Attach( hDC );
    m_pOwner->OnPrepareDC( &dcPrint, &printInfo );
    m_pOwner->m_PrintArea.SetRect( 0, 0, 1, 1 );
    m_pOwner->OnBeginPrinting( &dcPrint, &printInfo );
    m_pOwner->OnEndPrinting( &dcPrint, &printInfo );
    dcPrint.Detach();
  }
  return hDC != 0;
}
//=====================================================================
int
CPrintMapMode::Open()
{
  if( m_bAutoSize = m_pOwner->m_SelectList.GetCount() && CDocSetupPage::m_autoSizeEnb )
  {
    CBox SB( m_pOwner->CalcSelectBox());
    int ex = ::MulDiv( CDocSetupPage::m_extraSpaceVal, m_pOwner->GetPrnScale(), 10 );
    int a = m_pOwner->GetModeAngle();
    if( a )
    {
      CSize cntr( SB.TopLeft());
      SB.Reset();
      for( POSITION pos = m_pOwner->m_SelectList.GetHeadPosition() ; pos ; )
      {
        CDrawObject* pCopy = m_pOwner->m_SelectList.GetNext( pos )->CopyObject();
        pCopy->SpinOn( cntr, -a );
        SB |= pCopy->GetBoundBox();
        delete pCopy;
      }
      SB.InflateRect( ex, ex );
      SetLT( TurnPoint( SB.TopLeft(), cntr, a ));
      SetRB( TopLeft() + SB.Size());
    }
    else
    {
      SB.InflateRect( ex, ex );
      SetLT( SB.TopLeft());
      SetRB( SB.BottomRight());
    }
    ExpandSheet();
  }
  m_nMode = 2;
  return 1;
}
//=====================================================================
int
CPrintMapMode::OnLButtonDown( UINT &flags, CPoint& loc )
{
  if( m_nMode == 2 && !m_bAutoSize )
    SetLT( loc );

  m_nMode = m_nMode == 2 ? 3 : 7;

  return AM_RDONE;
}
//=====================================================================
int
CPrintMapMode::OnLButtonUp( UINT &flags, CPoint& loc )
{
  if( m_nMode == 7 )
  {
    if( TopLeft() == BottomRight())
      return AM_RDONE;

    CRect nr( this );
    nr.NormalizeRect();
    if( CPageSetupPage::m_layopt != LAYOUT_ALBUM && CDocSetupPage::m_autoScaleEnb )
    {
      int sv = CPageSetupPage::m_spaceEnb * CPageSetupPage::m_spaceVal * 10;
      int lv = CPageSetupPage::m_lineEnb * CPageSetupPage::m_lineVal;
      int ex = 2 * ( sv + lv ) + 1;
      int dw = nr.Width();
      int dh = nr.Height();
      if( dw && dh )
      {
        CDocSetupPage::m_scaleVal =
          max( ::MulDiv( dw, SmInUnit * 10, m_pOwner->m_PrinterPageSize.cx - ex ),
               ::MulDiv( dh, SmInUnit * 10, m_pOwner->m_PrinterPageSize.cy - ex - 5 )) / m_pOwner->m_ScaleFactor;
      }
    }
    m_pOwner->PrintMap( nr, TopLeft(), m_bPreview );
    return AM_RDONE | AM_RREMOVE;
  }

  m_nMode = m_nMode == 3 ? 3 : 2;

  return AM_RDONE;
}
//=====================================================================
int
CPrintMapMode::OnRButtonDown( UINT &flags, CPoint& loc )
{
  if( m_nMode == 2 )
    return AM_RREMOVE | AM_RDONE;

  m_nMode = 2;
  m_pOwner->ShowSelectedNum();
  return AM_RDONE;
}
//=====================================================================
int
CPrintMapMode::OnMouseMove( UINT &flags, CPoint &loc )
{
  if( !m_bAutoSize )
  {
    if( m_nMode & 1 )
      SetRB( TurnPoint( loc, TopLeft(), -m_pOwner->GetModeAngle()));
    ExpandSheet();
  }
  return AM_RPROPAGATE;
}
//=====================================================================
static int
Expand( int s, int f, int p )
{
  int n = 1 + abs( f - s ) / p;
  return s + ( s <= f ? n : -n ) * p;
}
//=====================================================================
void
CPrintMapMode::ExpandSheet()
{
  if( CPageSetupPage::m_layopt == LAYOUT_ALBUM )
  {
    right  = Expand( left, right, m_pOwner->m_PrintPageFrst.cx );
    bottom = Expand( top, bottom, m_pOwner->m_PrintPageFrst.cy );
  }
}
//=====================================================================
static void
DrawLines( CDC* pDC, CRect* p, int np, CPoint center, int angle )
{
  CPen pen( PS_SOLID, 4, RGB( 0x80, 0x80, 0x80 ));
  CPen* pOldPen = pDC->SelectObject( &pen );
  for( int i = 0 ; --np >= 0 ; ++p )
  {
    CPoint p0( p->TopLeft()), p1( p->BottomRight());
    pDC->MoveTo( angle ? TurnPoint( p0, center, angle ) : p0 );
    pDC->LineTo( angle ? TurnPoint( p1, center, angle ) : p1 );
    if( !i )
    {
      i = 1;
      pDC->SelectObject( pOldPen )->DeleteObject();
      pen.CreatePen( PS_SOLID, 2, RGB( 0x80, 0x80, 0x80 ));
      pOldPen = pDC->SelectObject( &pen );
    }
  }
  pDC->SelectObject( pOldPen );
}
//=====================================================================
void
CPrintMapMode::Draw( CDC* pDC )
{
  if( m_nMode & 1 )
  {
    pDC->SetBkMode( TRANSPARENT );
    pDC->SetROP2( R2_XORPEN );

    CPoint org( TopLeft());
    pDC->LPtoDP( &org );

    CRect bx[4096], by[4096];
    int ix = 0, iy = 0;

    m_pOwner->m_PrintArea = *this;
    m_pOwner->m_PrintArea.NormalizeRect();
    CRect* pr = &m_pOwner->m_PrintArea;

    if( CPageSetupPage::m_layopt != LAYOUT_ALBUM && CDocSetupPage::m_autoScaleEnb )
    {
      m_pOwner->m_PrintPageNumX = 1;
      m_pOwner->m_PrintPageNumY = 1;

      by[iy++].SetRect( pr->left, pr->top, pr->right, pr->top );
      bx[ix++].SetRect( pr->left, pr->top, pr->left, pr->bottom );
      bx[ix++].SetRect( pr->right, pr->top, pr->right, pr->bottom );
      by[iy++].SetRect( pr->left, pr->bottom, pr->right, pr->bottom );
    }
    else
    { 
      m_pOwner->CalcPrintPageNum();
      m_pOwner->m_PrintPageNumX = min( m_pOwner->m_PrintPageNumX, CELEM( bx ));
      m_pOwner->m_PrintPageNumY = min( m_pOwner->m_PrintPageNumY, CELEM( by ));

      by[iy++].SetRect( pr->left, pr->top, pr->right, pr->top );
      bx[ix++].SetRect( pr->left, pr->top, pr->left, pr->bottom );

      int x = pr->left + m_pOwner->m_PrintPageFrst.cx;
      int y = pr->top  + m_pOwner->m_PrintPageFrst.cy;

      for( int nx = m_pOwner->m_PrintPageNumX ; --nx >= 0 ; x += m_pOwner->m_PrintPageNext.cx )
      {
        x = min( x, pr->right );
        if( x != bx[ix-1].left )
          bx[ix++].SetRect( x, pr->top, x, pr->bottom );
      }

      for( int ny = m_pOwner->m_PrintPageNumY ; --ny >= 0 ; y += m_pOwner->m_PrintPageNext.cy )
      {
        y = min( y, pr->bottom );
        if( y != by[iy-1].top )
          by[iy++].SetRect( pr->left, y, pr->right, y );
      }
    }
    
    pDC->LPtoDP((CPoint*)bx, ix * 2 );
    pDC->LPtoDP((CPoint*)by, iy * 2 );

    int idc = SetTM( pDC );

    int lx = ix--, ly = iy--;

    if( bx[1].left - bx[0].left < 8 || by[1].top - by[0].top < 8 )
    {
      lx = ly = 2;
      bx[1] = bx[ix];
      by[1] = by[iy];
    }

    DrawLines( pDC, bx, lx, org, m_pOwner->GetModeAngle());
    DrawLines( pDC, by, ly, org, m_pOwner->GetModeAngle());

    pDC->RestoreDC( idc );

    char s[128];
    int n = sprintf_s( s, "Страниц печати: %d x %d = %d", iy, ix, ix * iy );
    int a = m_pOwner->GetModeAngle();
    if( a )
    {
      int ma = abs( a );
      sprintf_s( s + n, sizeof s - n, ". Поворот на %s%d.%1d град.", a >= 0 ? "" : "-", ma/10, ma%10 );
    }
    ReportHint( s );
  }
}
//=====================================================================
int
CPrintMapMode::Term( int bDestroy )
{
  m_pOwner->ShowSelectedNum();
  return 0;
}
//=====================================================================
