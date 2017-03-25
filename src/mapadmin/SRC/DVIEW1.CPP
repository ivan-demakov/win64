#include "stdafx.h"

#include "ksi.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "drawobj.h"
#include "graphdoc.h"
#include "joke.h"
#include "mview.h"
#include "mdoc.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "dlg0.h"
#include "bldmode.h"
#include "miscmode.h"
#include "selmode.h"
#include "movemode.h"
#include "pfmode.h"
#include "prndlg.h"
#include "dwin.h"
#include "winuser.h"
#include "util.h"
#include "sltrmode.h"
#include "splmode.h"
#include "measmode.h"
#include "conduct.h"
#include "lawdlg.h"
#include "cnctmode.h"
#include "rstrmode.h"
#include "linetmpl.h"
#include "mathutil.h"
#ifndef _FULL_PROJECT
  #include "abstrdlg.h"
  #include "shabdlg.h"
  #include "subldlg.h"
  #include "bardlg.h"
  #include "convdlg.h"
  #include "msutil.h"
#endif
#include "instrdlg.h"
#include "mercator.h"
//========================================================================
#ifdef _DEBUG
#include <io.h>
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//========================================================================
int
CMapView::GetPrnScale()
{
  return
    CPageSetupPage::m_layopt != LAYOUT_ALBUM && CDocSetupPage::m_autoScaleEnb ?
      CDocSetupPage::m_scaleVal :
    CDocSetupPage::m_curscaleEnb ?
      GetTrueScale() :
      CDocSetupPage::m_scaleVal;
}
//========================================================================
void
CMapView::OnPrintMap( UINT nID )
{
  int bPreview = nID == IDM_PREVIEW_MAP_MODE;
  CPrintMapMode* pPMM = (CPrintMapMode*)GetMode( IDM_PRINT_MAP_MODE );
  if( pPMM && pPMM->IsPreview() == bPreview )
    RemoveMode( pPMM );
  else
    AddMode( new CPrintMapMode( bPreview ));
}
//========================================================================
void
CMapView::OnUpdatePrintCommand( CCmdUI* pCmdUI )
{
  UINT const ids[] = { IDM_PRINT_MAP_MODE, IDM_PREVIEW_MAP_MODE };
  CPrintMapMode* pm = (CPrintMapMode*)GetMode( IDM_PRINT_MAP_MODE );
  pCmdUI->SetCheck( pm && ids[pm->IsPreview()] == pCmdUI->m_nID );
}
//========================================================================
void
CMapView::OnDrawGrid()
{
  CGridMode::m_bDrawGrid ^= 1;
  InvalidateRect( 0, 0 );
}
//========================================================================
void
CMapView::OnUpdateDrawGrid( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( CGridMode::m_bDrawGrid );
  pCmdUI->Enable( GetMode( IDM_GRID_MODE ) != 0 );
}
//========================================================================
void
CMapView::OnShowBox()
{
  CDrawContainer::m_bShowBox ^= 1;
  for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ;
       ShowObjectBox( m_SelectList.GetNext( pos )));
}
//========================================================================
void
CMapView::OnUpdateShowBox( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( CDrawContainer::m_bShowBox );
}
//========================================================================
void
CMapView::OnAngleStep()
{
  CAngleDialog dlg;
        DoModalDlg( &dlg, CE_PAR_DLG );
}
//========================================================================
void
CMapView::OnDistance( UINT id )
{
  switch( id )
  {
    case IDM_DISTANCE :
    {
      CSetDistDialog dlg( "Уклонение", "trace-dist" );
      if( DoModalDlg( &dlg, CE_PAR_DLG ) == IDOK )
        m_Deviation = CSetDistDialog::GetDist();
      break;
    }
    case IDM_SENSIBILITY :
    {
      CSetDistDialog dlg( "Чувствительность", 0 );
      dlg.SetDist( m_Sensibility );
      if( DoModalDlg( &dlg, CE_PAR_DLG ) == IDOK )
        m_Sensibility = dlg.GetDist();
      break;
    }
    case IDM_IMPROVE_PARAM :
    {
      CSetDistDialog dlg( "Прореживание", 0 );
      dlg.SetDist( m_MaxDelta );
      if( DoModalDlg( &dlg, CE_PAR_DLG ) == IDOK )
      {
        int t = dlg.GetDist();
        if( t <= 0 )
          ErrBox( "Недопустимый параметр прореживания!" );
        else
        {
          m_MaxDelta = t;
          ksi_defsym( "max-delta", ksi_int2num( m_MaxDelta ), ksi_current_env());
        }
      }
      break;
    }
  }
}
//========================================================================
//#define _FLOAT_CALC_
//========================================================================
CSize
CMapView::Scale2WindowSize( int scale )
{
  CClientDC dc( 0 );
#ifdef _FLOAT_CALC_
  double hl = dc.GetDeviceCaps( LOGPIXELSX );
  double vl = dc.GetDeviceCaps( LOGPIXELSY );
  double s = double( scale ) / double( SmInUnit ) * 2.54;
  return CSize( double( m_ViewportSize.cx ) * s / hl, double( m_ViewportSize.cy ) * s / vl );
#else
  int hl = dc.GetDeviceCaps( LOGPIXELSX );
  int vl = dc.GetDeviceCaps( LOGPIXELSY );
  return CSize( MulDiv( m_ViewportSize.cx * 254, scale, hl * SmInUnit * 100 ),
                MulDiv( m_ViewportSize.cy * 254, scale, vl * SmInUnit * 100  ));
#endif
}
//========================================================================
int
CMapView::WindowSize2Scale( CSize ws )
{
  CClientDC dc( 0 );
#ifdef _FLOAT_CALC_
  double hl = dc.GetDeviceCaps( LOGPIXELSX );
  double vl = dc.GetDeviceCaps( LOGPIXELSY );
  double s = double( SmInUnit ) / 2.54;
  double sx = double( ws.cx ) * s / double( m_ViewportSize.cx ) * hl;
  double sy = double( ws.cy ) * s / double( m_ViewportSize.cy ) * vl;
  return max( sx, sy );
#else
  int hl = dc.GetDeviceCaps( LOGPIXELSX );
  int vl = dc.GetDeviceCaps( LOGPIXELSY );
  int sx = MulDiv( hl * SmInUnit * 100, ws.cx, 254 * m_ViewportSize.cx );
  int sy = MulDiv( vl * SmInUnit * 100, ws.cy, 254 * m_ViewportSize.cy );
  return max( sx, sy );
#endif
}
//========================================================================
CSize
CMapView::LP2DP( CSize const& s )
{
#ifdef _FLOAT_CALC_
  double cx = double( s.cx ) * double( m_ViewportSize.cx ) / double( m_WindowSize.cx );
  double cy = double( s.cy ) * double( m_ViewportSize.cy ) / double( m_WindowSize.cy );
#else
  int cx = MulDiv( s.cx, m_ViewportSize.cx, m_WindowSize.cx );
  int cy = MulDiv( s.cy, m_ViewportSize.cy, m_WindowSize.cy );
#endif
  return CSize( cx, cy );
}
//========================================================================
CSize
CMapView::DP2LP( CSize const& s )
{
#ifdef _FLOAT_CALC_
  double cx = double( s.cx ) * m_WindowSize.cx / m_ViewportSize.cx;
  double cy = double( s.cy ) * m_WindowSize.cy / m_ViewportSize.cy;
#else
  int cx = MulDiv( s.cx, m_WindowSize.cx, m_ViewportSize.cx );
  int cy = MulDiv( s.cy, m_WindowSize.cy, m_ViewportSize.cy );
#endif
  return CSize( cx, cy );
}
//========================================================================
CPoint
CMapView::LP2DP( CPoint const& p )
{
  return LP2DP( p - m_Clip.TopLeft());
}
//========================================================================
CPoint
CMapView::DP2LP( CPoint const& p )
{
  CSize LT( GetScrollPos( SB_HORZ ), GetScrollPos( SB_VERT ));
  CPoint TL( DP2LP( LT ));
  return TL + DP2LP( CSize( p ));
//  return m_Clip.TopLeft() + DP2LP( CSize( p ));
}
//========================================================================
CRect
CMapView::LP2DP( CRect const& r )
{
  return CRect( LP2DP( r.TopLeft()), LP2DP( r.BottomRight()));
}
//========================================================================
CRect
CMapView::DP2LP( CRect const& r )
{
  return CRect( DP2LP( r.TopLeft()), DP2LP( r.BottomRight()));
}
//========================================================================
void
CMapView::SetScale( int scale, CPoint center, int bMakeUndo )
{
  if( center.x < 0 || center.y < 0 )
    center = GetCenter();

  int rs = scale;

  int sc;
  CBuildMode* pBM = GetBuilding( 0 );
  if( pBM )
  {
    int t = pBM->m_nID - IDM_CLASS_BEGIN;
    sc = m_pObjDef->GetMinLimit( t );
    if( sc )
      rs = max( rs, MerkatorData.LogScale( sc, center));
    sc = m_pObjDef->GetMaxLimit( t );
    if( sc )
      rs = min( rs, MerkatorData.LogScale( sc, center));

    pBM->SetTextCursor( 0 );
  }

  int mas = MerkatorData.LogScale( MaxScale, center );
  int mis = MerkatorData.LogScale( MinScale, center );
  rs = max( mas, min( mis, rs ));
  int ss = rs;

  for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
  {
    int t = m_SelectList.GetNext( pos )->Class();
    if( sc = m_pObjDef->GetMinLimit( t ))
      ss = max( ss, MerkatorData.LogScale( sc, center ));
    if( sc = m_pObjDef->GetMaxLimit( t ))
      ss = min( ss, MerkatorData.LogScale( sc, center ));
  }

  static long ih = 0;
  if( !m_pSTM && !ih && !m_SelectList.IsEmpty() && !m_bFinding && rs != scale )
  {
    char b[256];
    sprintf_s( b, "На плане выделены объекты,\n"
                  "не отображаемые в масштабе менее, чем 1 : %d.\n\n"
                  "Ограничить масштаб?", MerkatorData.TrueScale( ss, center ));
    CInstrDlg dlg( &ih, b );
    if( DoModalDlg( &dlg, CE_ASK_DLG ) == IDOK )
      rs = ss;
  }

  scale = rs;

  int bDrag = scale == m_AltScale && bMakeUndo;

  if( bMakeUndo && ! m_pUndoItem )
    m_Undo.AddItem( new CUndoScaleItem( this ), this );

  CRect r;
  GetClientRect( &r );
  m_ViewportSize = r.Size();
  CSize docSize( GetDocument()->GetSize());

  m_OldScale = m_AltScale;

  if( !bDrag )
  {
    m_WindowSize = Scale2WindowSize( scale );

    int minXSize = max( MulDiv( docSize.cx, m_ViewportSize.cx, 0x7fffffff ), m_ViewportSize.cx );
    int minYSize = max( MulDiv( docSize.cy, m_ViewportSize.cy, 0x7fffffff ), m_ViewportSize.cy );
    if( m_WindowSize.cx > docSize.cx && m_WindowSize.cy > docSize.cy )
                {
      int nwcy = MulDiv( m_WindowSize.cy, docSize.cx, m_WindowSize.cx );
      if( nwcy >= docSize.cy )
      {
        m_WindowSize.cy = nwcy;
        m_WindowSize.cx = docSize.cx;
      }
      else
      {
        m_WindowSize.cx = MulDiv( m_WindowSize.cx, docSize.cy, m_WindowSize.cy );
        m_WindowSize.cy = docSize.cy;
      }
    }
    if( m_WindowSize.cx < minXSize )
    {
      m_WindowSize.cy = MulDiv( m_WindowSize.cy, minXSize, m_WindowSize.cx );
      m_WindowSize.cx = minXSize;
    }
    if( m_WindowSize.cy < minYSize )
    {
      m_WindowSize.cx = MulDiv( m_WindowSize.cx, minYSize, m_WindowSize.cy );
      m_WindowSize.cy = minYSize;
    }

    CSize s( MulDiv( docSize.cx, m_ViewportSize.cx, m_WindowSize.cx ),
             MulDiv( docSize.cy, m_ViewportSize.cy, m_WindowSize.cy ));

    m_nMapMode = MM_TEXT;
    m_totalDev = m_totalLog = s;

    SetRedraw( 0 );
    ShowScrollbars( 0 );
    m_AltScale = WindowSize2Scale( m_WindowSize );
    SetKsiHiResolution();
  }
  CPoint vp( center );
  vp.Offset( -m_WindowSize.cx / 2, -m_WindowSize.cy / 2 );

  if( vp.x < 0 || vp.x + m_WindowSize.cx > docSize.cx &&
      ( vp.x = docSize.cx - m_WindowSize.cx ) < 0 )
      vp.x = 0;
  if( vp.y < 0 || vp.y + m_WindowSize.cy > docSize.cy &&
      ( vp.y = docSize.cy - m_WindowSize.cy ) < 0 )
      vp.y = 0;
  int t, mx = m_totalDev.cx - 1, my = m_totalDev.cy - 1;
  GetScrollRange( SB_HORZ, &t, &mx );
  GetScrollRange( SB_VERT, &t, &my );
  vp.x = MulDiv( vp.x, m_totalDev.cx, docSize.cx );
  vp.y = MulDiv( vp.y, m_totalDev.cy, docSize.cy );
  SetScrollPos( SB_HORZ, min( vp.x, mx - m_ViewportSize.cx ));
  SetScrollPos( SB_VERT, min( vp.y, my - m_ViewportSize.cy ));
  SetRedraw( 1 );
  Invalidate( 0 );
  StartBlinking( m_pCurSelObj );

  CMainWin* pmw = (CMainWin*)AfxGetMainWnd();
  if( pmw )
    pmw->DirectUpdateSts();
}
//========================================================================
void
CMapView::OnSize( UINT nType, int cx, int cy )
{
  if( !m_bScaleOnSize )
    return;

  m_ViewportSize.cx = cx;
  m_ViewportSize.cy = cy;
  if( m_AltScale )
    m_WindowSize = Scale2WindowSize( m_AltScale );
}
//========================================================================
void
CMapView::ShowScrollbars( int bDraw )
{
  m_bScaleOnSize = 0;
  if( m_bScrollBars )
    UpdateBars();
  else
  {
    SetScrollRange( SB_HORZ, 0, m_totalDev.cx - 1, 0 );
    SetScrollRange( SB_VERT, 0, m_totalDev.cy - 1, 0 );
    ShowScrollBar( SB_BOTH, 0 );
  }
  m_bScaleOnSize = 1;

  if( bDraw  )
  {
    int s = m_AltScale;
    CPoint cn( GetCenter());
    m_AltScale = 0;
    SetScale( s, cn, 0 );
  }
}
//========================================================================
void
CMapView::OnScrollbarsCmd()
{
  m_bScrollBars ^= 1;
  ShowScrollbars( 1 );
}
//========================================================================
void
CMapView::OnUpdateScrollbarsCmd( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( m_bScrollBars );
}
//========================================================================
int
CMapView::OnAppCommand( UINT nID )
{
  CAppMode* pCur = GetMode( nID );
  if( pCur && pCur->IsUniMode())
    RemoveMode( pCur );
  return pCur != 0;
}
//========================================================================
void
CMapView::OnHandMode()
{
  if( !OnAppCommand( IDM_HAND_MODE ))
    AddMode( new CHandMode );
}
//========================================================================
void
CMapView::OnGoTo()
{
  int s = GetTrueScale();

  if( !m_pGotoDialog )
  {
    m_pGotoDialog = new CGotoDialog();
    m_pGotoDialog->Create( IDD_GOTO_DIALOG, this );
  }

  m_pGotoDialog->SetPointScale( Project2Grid( GetCenter()), s );
  m_pGotoDialog->ShowWindow( m_pGotoDialog->IsWindowVisible() ? SW_HIDE : SW_SHOW );
}
//========================================================================
void
CMapView::GoTo( CPoint c, int s )
{
  SetScale( MerkatorData.LogScale( s, c ), c, 1 );
  UpdateWindow();
  SetPosition( c );
}
//========================================================================
void
CMapView::OnIncScaleMode()
{
  if( !OnAppCommand( IDM_INCSCALE_MODE ))
    AddMode( new CIncScaleMode );
}
//========================================================================
void
CMapView::OnMagnitMode( UINT cmd )
{
	int msk = 1 << ( cmd - IDM_MAGNIT_MODE1 );
	CMagnitMode* pMode = (CMagnitMode*)GetMode( IDM_MAGNIT_MODE1 );
	if( !pMode )
		AddMode( pMode = new CMagnitMode());

	pMode->SetSelMode( msk );
  if( !pMode->GetSelMode())
    RemoveMode( pMode );
}
//========================================================================
void
CMapView::OnUpdateMagnitMode( CCmdUI* pCmdUI )
{
	int bCheck = 0;
	CMagnitMode* pMode = (CMagnitMode*)GetMode( IDM_MAGNIT_MODE1 );
	if( pMode )
	{
  	int msk = 1 << ( pCmdUI->m_nID - IDM_MAGNIT_MODE1 );
		bCheck = ( pMode->GetSelMode() & msk ) != 0;
	}
	pCmdUI->SetCheck( bCheck );
}
//========================================================================
void
CMapView::OnDecScaleMode()
{
  if( !OnAppCommand( IDM_DECSCALE_MODE ))
    AddMode( new CDecScaleMode );
}
//========================================================================
void
CMapView::OnGetSelectMode()
{
  if( !OnAppCommand( IDM_GETSELECT_MODE ))
    AddMode( new CGetSelectMode );
}
//========================================================================
void
CMapView::OnAddSelectMode()
{
  if( !OnAppCommand( IDM_ADDSELECT_MODE ))
    AddMode( new CAddSelectMode );
}
//========================================================================
void
CMapView::OnSubSelectMode()
{
  if( !OnAppCommand( IDM_SUBSELECT_MODE ))
    AddMode( new CSubSelectMode );
}
//========================================================================
void
CMapView::InitQueryPolygon()
{
  static UINT mm[] =
  {
    IDM_LINE_MEASURE_MODE,
    IDM_SQUARE_MEASURE_MODE,
    IDM_SELECT_POLY_MODE,
		IDM_SELECT_POLYGON_MODE,
    0
  };
  RemoveMode( GetMode( mm ));
	AddMode( new CMeasureMode( IDM_SELECT_POLYGON_MODE ));
}
//========================================================================
void
CMapView::OnStreetPlanMode()
{
  CSplDialog dlg;
  if( !OnAppCommand( IDM_STREETPLAN_MODE ) &&
      !OnAppCommand( IDM_UNRECTPLAN_MODE ) &&
      DoModalDlg( &dlg, CE_PAR_DLG ) == IDOK )
  {
    static UINT mm[] =
    {
      IDM_LINE_MEASURE_MODE,
      IDM_SQUARE_MEASURE_MODE,
      IDM_SELECT_POLY_MODE,
      0
    };
    RemoveMode( GetMode( mm ));

    if( CSplDialog::IsUnrectRgn() && !CSplDialog::IsAutoRgn())
      AddMode( new CMeasureMode( IDM_UNRECTPLAN_MODE ));
    else
      AddMode( new CStreetPlanMode());
  }
}
//========================================================================
void
CMapView::OnUnselect()
{
  ClearSelection();
}
//========================================================================
void
CMapView::OnCopyMode()
{
  CMoveMode::OnCopyMode();
  if( CMoveMode::CopyMode())
    RemoveMode( IDM_CONNECT_MODE );
}
//========================================================================
void
CMapView::OnUpdateCopyMode( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( m_Conductor.GetUserRights() >= UR_L_EDIT );
  pCmdUI->SetCheck( CMoveMode::CopyMode());
}
//========================================================================
void
CMapView::OnReisshina()
{
  if( !OnAppCommand( IDM_REISSHINA_MODE ))
    AddMode( new CReisMode );

  if( CGridMode::m_bDrawGrid )
    InvalidateRect( 0, 0 );
}
//========================================================================
void
CMapView::OnSetReisshina()
{
  if( !OnAppCommand( IDM_SETREISSHINA_MODE ))
    AddMode( new CSetReisMode );
}
//========================================================================
void
CMapView::OnCross()
{
  if( !OnAppCommand( IDM_CROSS_MODE ))
    AddMode( new CCrossMode );
}
//========================================================================
void
CMapView::OnSerialBuildingCmd()
{
  m_bSerialBuilding ^= 1;
}
//========================================================================
void
CMapView::OnUpdateSerialBuildingCmd( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( m_bSerialBuilding );
}
//========================================================================
void
CMapView::OnTransfocatorCmd()
{
  m_bSlow ^= 1;
}
//========================================================================
void
CMapView::OnUpdateTransfocatorCmd( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( m_bSlow );
}
//========================================================================
void
CMapView::OnToolTipsCmd()
{
  ((CMainWin*)AfxGetMainWnd())->SetToolTips( !CMainWin::m_bToolTips );
}
//========================================================================
void
CMapView::OnUpdateToolTipsCmd( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( CMainWin::m_bToolTips );
}
//========================================================================
//========================================================================
void
CMapView::OnTextShadowCmd()
{
  m_bTextShadow ^= 1;
	m_bRedraw = 1;
  InvalidateRect( 0, 0 );
}
//========================================================================
void
CMapView::OnUpdateTextShadowCmd( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( m_bTextShadow );
}
//========================================================================
void
CMapView::OnUseGridCmd()
{
  m_bUseGrid = !m_bUseGrid;
  if( !m_bUseGrid )
    RemoveMode( IDM_COORDGRID_MODE );
}
//========================================================================
void
CMapView::OnUpdateUseGridCmd( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( m_bUseGrid );
}
//========================================================================
void
CMapView::OnShowAsSeletedCmd()
{
  CDrawContainer::m_bShowAsSelected ^= 1;
  m_bRedraw = 1;
  Invalidate( 0 );
}
//========================================================================
void
CMapView::OnUpdateShowAsSeletedCmd( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( CDrawContainer::m_bShowAsSelected );
}
//========================================================================
void
CMapView::OnSelectLinkedCmd( UINT cmd )
{
  if( cmd == IDM_SELECT_LINKED )
  {
    m_bSelectLinked ^= 1;
    m_bSelectAllLinked = 0;
  }
  else
  {
    m_bSelectAllLinked ^= 1;
    m_bSelectLinked = 0;
  }
}
//========================================================================
void
CMapView::OnUpdateSelectLinkedCmd( CCmdUI* pCmdUI )
{
  if( pCmdUI->m_nID == IDM_SELECT_LINKED )
    pCmdUI->SetCheck( m_bSelectLinked );
  else
    pCmdUI->SetCheck( m_bSelectAllLinked );
}
//========================================================================
void
CMapView::OnScroll( UINT nBar, UINT nSBCode )
{
  CPoint sp( GetDeviceScrollPosition());
  SCROLLINFO inf;
  GetScrollInfo( nBar, &inf, SIF_TRACKPOS );
  CSize sh( 0, 0 );
  long* ps = nBar == SB_HORZ ? &sp.x : &sp.y;
  long* pp = nBar == SB_HORZ ? &sh.cx : &sh.cy;
  int   vs = nBar == SB_HORZ ? m_ViewportSize.cx : m_ViewportSize.cy;

  switch( nSBCode )
  {
    case SB_PAGEUP     : *pp = +vs; break;
    case SB_PAGEDOWN   : *pp = -vs; break;
    case SB_LINEUP     : *pp = +vs / 16; break;
    case SB_LINEDOWN   : *pp = -vs / 16; break;
    case SB_THUMBTRACK : *pp = *ps - inf.nTrackPos; break;
    default            : return;
  }

  sh = DP2LP( sh );
  SetScale( m_AltScale, GetCenter() - sh, 1 );
}
//=====================================================================
void
CMapView::OnVScroll( UINT nSBCode, UINT, CScrollBar* )
{
  OnScroll( SB_VERT, nSBCode );
}
//=====================================================================
void
CMapView::OnHScroll( UINT nSBCode, UINT, CScrollBar* )
{
  OnScroll( SB_HORZ, nSBCode );
}
//=====================================================================
void
CMapView::OnGridStep()
{
  ReportHint( "Щелкните левой кнопкой в точке привязки сетки" );
  AddMode( new CSetCoordGridMode( 0 ));
}
//========================================================================
void
CMapView::OnGrid()
{
  if( !OnAppCommand( IDM_GRID_MODE ))
    AddMode( new CGridMode());
}
//========================================================================
void
CMapView::OnUpdateGrid( CCmdUI* pCmdUI )
{
  CGridMode* pGM = (CGridMode*)GetMode( IDM_GRID_MODE );
  pCmdUI->SetCheck( pGM != 0 && pGM->GetDoGrid());
}
//========================================================================
int
CMapView::OnMouseWheel( UINT flags, short zDelta, CPoint loc )
{
  if( GetKeyState( VK_CONTROL ) < 0 )
  {
    int den = zDelta < 0 ? 5 : 4;
    int num = zDelta < 0 ? 4 : 5;
    int ns = MulDiv( m_AltScale, num, den );
    ns = min( MinScale, max( MaxScale, ns ));
    if( ns != m_AltScale )
    {
      CSize ss( m_Position - m_Clip.TopLeft());
      ss = ss + ss - m_WindowSize;
      den += den;
      ss.cx = MulDiv( ss.cx, num, den );
      ss.cy = MulDiv( ss.cy, num, den );
      int t = m_bSlow;
      m_bSlow = 0;
      SetScale( ns, m_Position - ss, 1 );
      UpdateWindow();
      m_bSlow = t;
    }
  }
  else
    OnScroll( GetKeyState( VK_SHIFT ) < 0 ? SB_HORZ : SB_VERT, zDelta < 0 ? SB_LINEDOWN : SB_LINEUP );
  return 1;
}
//========================================================================
void
CMapView::OnCoordGrid()
{
  if( !OnAppCommand( IDM_COORDGRID_MODE ))
    AddMode( new CCoordGridMode );
}
//========================================================================
void
CMapView::OnUpdateCoordGrid( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( m_bUseGrid );
  pCmdUI->SetCheck( GetMode( IDM_COORDGRID_MODE ) != 0 );
}
//========================================================================
void
CMapView::OnConnectMode()
{
  if( !OnAppCommand( IDM_CONNECT_MODE ))
    AddMode( new CConnectMode );
}
//========================================================================
void
CMapView::OnRestoreMode()
{
  CDrawObject* pObj = GetSelectObject( 1 );
  if( pObj && !OnAppCommand( IDM_RESTORE_MODE ))
  {
    if( pObj->Table())
      AddMode( new CRestoreMode( pObj->TabIdent()));
    else
      InfBox( "История объекта не подлежит восстановлению!" );
  }
}
//========================================================================
void
CMapView::OnExtSetScale()
{
  CPoint c( m_Conductor.GetPosition());
  SetScale( m_Conductor.GetScale() * MerkatorData.ScaleFactor( c ), c, 0 );
//  SetForegroundWindow();
}
//========================================================================
void
CMapView::OnExtSpaceQue()
{
  MakeSpaceQue( m_Conductor.m_SpaceQueParam );
//  SetForegroundWindow();
  ShowSelectedNum();
}
//========================================================================
#ifdef _FULL_PROJECT
void
CMapView::OnUpdateRestoreMode( CCmdUI* pCmdUI )
{
  pCmdUI->Enable(0);
}
#endif
//========================================================================
void
CMapView::OnMeasureMode( UINT Id )
{
  if( !OnAppCommand( Id ))
   switch( Id )
   {
     case IDM_LINE_MEASURE_MODE : AddMode( new CLinkMeasureMode );   break;
     case IDM_FIND_DIST_MODE :    AddMode( new CFindDistanceMode );  break;
     default :                    AddMode( new CMeasureMode( Id  )); break;
   }
}
//========================================================================
void
CMapView::OnObjectActionMode( UINT nID )
{
  int bEdit = nID == IDM_OBJEDIT_MODE;
  if( !OnAppCommand( nID ) &&
      ( !bEdit || !GetBuilding( 0 ) && m_Conductor.GetUserRights() >= UR_L_EDIT ))
  {
    if( bEdit )
      m_Conductor.RemoveAll();
    AddMode( new CObjActionMode( nID ));
  }
}
//========================================================================
void
CMapView::QueryUserRights( int nMode, CAppMode* pMode )
{
  m_URIdLst.RemoveAll();
  delete m_pURMode;
  m_pURMode = pMode;
  m_nURMode = nMode;

  if( nMode == UR_BLDMODE )
    m_Conductor.Create(((CBuildMode*)pMode)->GetObjectIdent());
        else
        if( nMode == UR_SUBLAYER )
    m_Conductor.SetClass( SUBLAYER_CLASS + 1, 0 );
  else
  {
    for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ;
         m_URIdLst.AddHead( m_SelectList.GetNext( pos )->TabIdent()));
    m_Conductor.Create( m_URIdLst.GetHead());
  }

  m_Conductor.GetUserRights( 2 );
  ReportHint( "Запрос прав на выпонение операции..." );
}
//========================================================================
void
CMapView::OnUserRightsOk( UINT cmd )
{
  if( m_nURMode == UR_NOMODE )
  {
    ASSERT( 0 );
    return;
  }

  if( m_nURMode == UR_BLDMODE )
  {
    ASSERT( m_pURMode );
    if( !m_pURMode )
      return;

    CBuildMode* pBM = (CBuildMode*)m_pURMode;
    CIdent id( m_Conductor.GetIdent());
    if( pBM->MyIdent( id ))
    {
      if( id.Entry() == 0 )
        AddMode( pBM );
      else
      {
        CAppMode* pSTM = m_pSTM;
        if( !AddMode( pBM  ) && pSTM )
          return;

        ClearSelection( 0, !m_pSTM );

        CDrawObject* pObj = GetObjectPtr( id );
        if( !pObj )
        {
          pObj = ExtractMapObj( id );
          m_ClipData.AddObject( pObj );
        }
        m_bNoLinkEdit = cmd == IDM_USER_RIGHTS_OK_EXT;
        BeginCoedition( pObj, pBM );
      }
    }
    else
    {
      ErrBox( "Ошибка при запросе прав" );
      delete m_pURMode;
    }
    m_pURMode = 0;
    m_nURMode = UR_NOMODE;
    return;
  }

  if( !m_URIdLst.IsEmpty())
  {
    if( m_URIdLst.GetHead() != m_Conductor.GetTable())
    {
      ErrBox( "Ошибка при запросе прав" );
      delete m_pURMode;
      m_pURMode = 0;
      m_nURMode = UR_NOMODE;
      m_URIdLst.RemoveAll();
      return;
    }
   else
   {
     m_URIdLst.RemoveHead();
     if( !m_URIdLst.IsEmpty())
     {
       m_Conductor.Create( m_URIdLst.GetHead());
       m_Conductor.GetUserRights( 2 );
       return;
      }
    }
  }

  switch( m_nURMode )
  {
          case UR_DELETE:
      m_Undo.AddItem( new CUndoAddItem( this, 0, &m_SelectList ), this );
      ClearSelection( 1, 0 );
      break;
          case UR_SUBLAYER:
      Invalidate( 0 );
                        m_pSublDlg->Activate();
      m_pSublDlg->ShowWindow( SW_SHOW );
      break;
    case UR_MOVEMODE :
    case UR_SPINMODE :
      ASSERT( m_pURMode );
      if( m_pURMode )
        AddMode( m_pURMode );
      break;
    case UR_SPINLEFT :
    case UR_SPINRIGHT :
    {
      int a = CAngleDialog::m_AngleGrid;
      if( m_nURMode != UR_SPINLEFT )
        a = - a;

      for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
        if( !IsObjectInLegalRegion( m_SelectList.GetNext( pos ), "Поворот", 1 ))
          return;

      CWaitCursor wc;
      ReportHint( "Пожалуйста, подождите" );
      ShowSelected( 0 );
      CBox b( CalcSelectBox());
      ShowBox( b );
      CPoint c( b.CenterPoint());
      CBox s( b, c, a );

      CDrawObject* pBlinking = StartBlinking( 0 );
      if( pBlinking )
        pBlinking = pBlinking->CopyObject();

      if( SpinSelectOn( c, a )  && !GetProjectSLEnable())
        m_Undo.AddItem( new CUndoSpinItem( this, c, a ), this );
      ShowSelected( 1, 1, 1 );

      if( pBlinking )
        for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
        {
          CDrawObject* pObj = m_SelectList.GetNext( pos );
          if( pObj->Ident() != pBlinking->Ident())
            continue;
          StartBlinking( pObj );
          delete pBlinking;
        }

      ShowSelectedNum();
    }
  }

  m_URIdLst.RemoveAll();
  m_nURMode = UR_NOMODE;
  m_pURMode = 0;
}
//========================================================================
void
CMapView::OnUserRightsCancel()
{
  if( m_nURMode == UR_NOMODE )
  {
    ASSERT( 0 );
    return;
  }

  CString m;
  if( m_nURMode == UR_BLDMODE )
  {
    CBuildMode* pBM = (CBuildMode*)m_pURMode;
    CIdent id( pBM->GetObjectIdent());
    int bBld = id.Entry() == 0;
    int nClass = id.Class();
    m.Format( "Вы не можете выполнить %s объекта '%s', "
              "так как у Вас отсутствуют права на выполнение данной операции.",
              bBld ? "построение" : "редактирование",
              m_pObjDef->GetName( nClass ));
  }
  else
  {
    m.Format( "Вы не можете выполнить %s выделнных объектов, "
              "так как у Вас отсутствуют права на выполнение данной операции.",
              m_nURMode != UR_MOVEMODE ? "вращение" :
              CMoveMode::CopyMode()    ? "копирование" : "перемещение" );
  }

  ErrBox( m );
  m_URIdLst.RemoveAll();
  m_nURMode = UR_NOMODE;
  delete m_pURMode;
  m_pURMode = 0;
}
//========================================================================
void
CMapView::StartBuilding()
{
  ClearSelection( 0, !m_pSTM );

  ksi_obj ksi_param = ksi_nil;
  if( m_pSuspObj )
  {
                int np;
    CPoint* pPnt = m_pSuspObj->GetPolyline( &np );
                CPoint pt = pPnt ? pPnt[np-1] : m_Position;
                ksi_obj ksi_img = *m_pSuspObj;
                ksi_obj ksi_class = KSI_MK_SINT( m_pSuspObj->Class());
                ksi_obj ksi_status = KSI_MK_SINT( m_pSuspObj->Status());

                ksi_param = KsiCall( "make-building-context", ksi_class, ksi_status, ksi_img,
                         ksi_int2num( pt.x ), ksi_int2num( pt.y ),
                         KSI_MK_SINT( m_ObjectClass ), 0 );

    if( !KSI_PLIST_P( ksi_param ))
      ksi_param = KsiCall( "make-polyline-context", ksi_class, ksi_status, ksi_img, 0 );
        }
  CBuildMode* pBM = new CBuildMode( IDM_CLASS_BEGIN + m_ObjectClass,
                                    CIdent( m_ObjectClass, 0, 0, 0 ),
                                    0, ksi_param, ksi_nil );
  RemoveMode( IDM_OBJEDIT_MODE );
  QueryUserRights( UR_BLDMODE, pBM );
}
//=====================================================================
void
CMapView::OnObjContextCommand( UINT nID )
{
  m_ObjectClass = nID - UINT( IDM_CONTEXT_BEGIN );
  StartBuilding();
}
//========================================================================
void
CMapView::OnObjCommand( UINT nID )
{
  if( !GetBuilding( 1 ))
  {
    m_ObjectClass = nID - IDM_OBJCTL_BEGIN;
    CMenu popup;
    popup.CreatePopupMenu();
    CMapDoc* pDoc = GetDocument();
    pDoc->SetContextMenu( 0 );
    pDoc->MakePopupMenu( &popup, m_ObjectClass, IDM_OBJCTL_BEGIN, &CObjDef::GetDerived );
    if( popup.GetMenuItemCount())
    {
      CPoint p;
      GetCursorPos( &p );
      popup.TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, p.x, p.y, this );
      GetDocument()->DeletePopupMenu( &popup, 0 );
      popup.DestroyMenu();
    }
    else
    {
      CBuildMode::SetContext();
      StartBuilding();
    }
  }
}
//========================================================================
void
CMapView::OnOverview( UINT id )
{
  switch( id )
  {
    case IDM_OVERVIEW_MODE :
    {
      m_OverviewDialog.ShowWindow( m_OverviewDialog.IsWindowVisible() ? SW_HIDE : SW_SHOW );
      break;
    }
    case IDM_REFRESH_OVERVIEW :
      m_OverviewDialog.Refresh( 0 );
      break;
    case IDM_SELECT_OVERVIEW :
    {
      AddMode( new CSelectOverviewMode( &m_OverviewDialog ));
      break;
    }
  }
}
//========================================================================
void
CMapView::OnUpdateOverview( CCmdUI* pCmdUI )
{
  pCmdUI->SetCheck( IsWindow( m_OverviewDialog.GetSafeHwnd()) &&
                    m_OverviewDialog.IsWindowVisible());
  pCmdUI->Enable( COverviewDialog::HasOverview());
}
//========================================================================
void
CMapView::OnSelectTraceMode( UINT Id )
{
  OnShortTrace( IDM_SHORTTRACE_CANCEL );
  ST_MODE m = ST_MODE( Id - IDM_VIEWTRACE_MODE + 1 );
  if( !m_pSTM || m_pSTM->AskForClose( m ))
    AddMode( new CSelectTraceMode( m ));
}
//========================================================================
void
CMapView::OnTraceClose( UINT Id )
{
  if( m_pSTM )
  {
    m_pSTM->Close();
    RemoveMode( m_pSTM );
  }
}
//========================================================================
void
CMapView::OnShortTrace( UINT Id )
{
        OnTraceClose( 0 );

        CShortTraceMode* pSTM = (CShortTraceMode*)GetMode( IDM_SHORTTRACE_MODE );

        if( pSTM && Id == IDM_SHORTTRACE_BKSPACE )
        {
                OnChar( VK_BACK, 0, 0 );
                return;
        }

        if( pSTM && Id == IDM_SHORTTRACE_CLOSE )
                pSTM->DeclareEndSelection();

        if( pSTM )
                RemoveMode( pSTM );

        if( Id == IDM_SHORTTRACE_OPEN )
                AddMode( new CShortTraceMode());
}
//========================================================================
void
CMapView::OnEditor()
{
  CEditLawDialog dlg( m_LocalLaw, m_pObjDef, this );
  DoModalDlg( &dlg, CE_PAR_DLG );
}
//========================================================================
void
CMapView::OnLayerDlg()
{
  CLoadLawDialog dlg( "Управление отображением", m_ShowMap,
                       m_pObjDef, this, IDD_SELEC_CLASSES_DIALOG, 1 );
  if( DoModalDlg( &dlg, CE_PAR_DLG ) != IDOK )
    return;

  for( int t = FULL_RANGE ; --t >= 0 ; )
  {
    int bSub = t > SUBLAYER_CLASS;
    CLayer* pLayer = m_ClipData.GetLayerByClass( t );
    int bVold = pLayer->LayerOn();
    int bVsbl = m_ShowMap[t>>3] & ( 1 << ( t & 7 ));
    if( !bSub || !bVold || !SublDlgIsActive() || t - SUBLAYER_CLASS - 1 != m_pSublDlg->GetLayerNum())
    {
      pLayer->SetVisible( bVsbl );
      m_bSublayerChanged |= bVold != bVsbl && bSub;
    }
  }
  m_bRedraw = 1;
  StartBlinking( 0 );
  Invalidate( 0 );
}
//========================================================================
void
CMapView::OnSASDlg()
{
  CLoadLawDialog dlg( "Показывать как выделенные", CDrawContainer::m_bSASMap,
                       m_pObjDef, this, IDD_SELEC_CLASSES_DIALOG, 0 );
  if( DoModalDlg( &dlg, CE_PAR_DLG ) == IDOK && CDrawContainer::m_bShowAsSelected )
  {
    m_bRedraw = 1;
    m_bSublayerChanged = 1;
    Invalidate( 0 );
  }
}
//========================================================================
#ifndef _FULL_PROJECT
int
CMapView::UpdateColorTab( COLORREF MonitorColor,
                          COLORREF PrinterColor,
                          COLORREF PrinterBWColor,
                          int& bClear )
{
  static ksi_obj clr_key[] =
  {
    ksi_str02key( "fore-color" ),
    ksi_str02key( "back-color" ),
    ksi_str02key( "font-color" ),
    ksi_str02key( "polyline-color" )
  };

  int bConcret = CAbstractDialog::IsClassConcret();
  char const sectName[] = "UserColorTab";
  char szKey[16], szCol[48];
  CMapDoc* pDoc = GetDocument();
  CIntAssocMap* pMCT = &pDoc->m_UserViewColorTab;
  CIntAssocMap* pPCT = &pDoc->m_UserPrintColorTab;
  CIntAssocMap* pBWT = &pDoc->m_UserPrintBWColorTab;

  int minKey, maxKey;
  pMCT->GetMinMaxKey( &minKey, &maxKey );

  int key = -1;

  ksi_obj par_lst = ksi_eval_str( bConcret ? "concret-param-list" : "abstract-param-list" );

  for( int k = minKey ; k <= maxKey ; ++k )
  {
    long c0 = pMCT->TranslateKey( k );
    if( c0 < 0 )
      continue;
    long c1 = pPCT->TranslateKey( k );
    long c2 = pBWT->TranslateKey( k );

    if( c0 == MonitorColor && c1 == PrinterColor && c2 == PrinterBWColor )
      key = k;

    if( bClear && !bConcret )
    {
      ksi_obj ksi_key = ksi_int2num( k );
      ksi_obj lst;
      for( lst = par_lst ; KSI_PLIST_P( lst ) ; lst = KSI_CDR( lst ))
      {
        ksi_obj par = KSI_CAR( lst );
        int i = CELEM(clr_key);
        while( --i >= 0 )
          if( KSI_EQ_P( ksi_key, ksi_assv_ref( par, clr_key[i] )))
            break;
        if( i >= 0 )
          break;
      }
      if( bClear && key < 0 && KSI_NULL_P( lst ))
      {
        sprintf_s( szKey, "%d", k );
        App.WriteProfileString( sectName, szKey, 0 );
      }
    }
  }

  if( key < 0 )
  {
    key = pMCT->FindFreeKey();

    sprintf_s( szKey, "%d", key );
    sprintf_s( szCol, "0x%06lx 0x%06lx 0x%06lx", MonitorColor, PrinterColor, PrinterBWColor );
    App.WriteProfileString( sectName, szKey, szCol );

    pMCT->Add( key, MonitorColor );
    pPCT->Add( key, PrinterColor );
    pBWT->Add( key, PrinterBWColor );
  }

  bClear = 0;

  return key;
}
#endif
//========================================================================
void
CMapView::OnImprovePolyline()
{
  if( GetBuilding( 1 ))
    return;

  CDrawObject* pObj = GetSelectObject( 1 );

  if( !pObj )
    return;

  int np, tp;
  CPoint* pPnt = pObj->GetAnyPoints( &tp, &np );

  if( !pPnt || np <= 3 )
  {
    ErrBox( "Выделенный объект не подлежит прореживанию!" );
    return;
  }

  ksi_obj ksi_pl = KsiPoints( pPnt, np );

  CWaitCursor wc;
  ksi_pl = KsiCall( "improve-polyline", ksi_pl, KSI_MK_BOOL( tp == SH_POLYGON ), ksi_true, ksi_true, 0 );

  int ln = KSI_LIST_LEN( ksi_pl );

  if( ln == np )
    return;

  if( tp == SH_POLYGON && ln < 3 )
  {
    ErrBox( "Прореживание приводит к вырождению объекта!" );
    return;
  }

  pPnt = new CPoint[ln];
  for( int i = 0 ; i < ln ; ksi_pl = KSI_CDR( ksi_pl ))
    pPnt[i++] = CppPoint( KSI_CAR( ksi_pl ));

  pObj->SetPoly( pPnt, ln );
  delete pPnt;
  CDrawObject* pOld = pObj;
  pObj = pOld->Rebuild();

  m_Undo.AddItem( new CUndoAddItem( this, pObj, pOld ), this );

  SelectObject( pOld, 0 );
  delete AddObject( pObj, 0 );
  SelectObject( pObj, 1 );
}
//========================================================================
void
CMapView::OnSublayerAdm()
{
  if( SublDlgIsActive())
    m_pSublDlg->OnEndButtonClicked();
  else
                QueryUserRights( UR_SUBLAYER );
}
//========================================================================
void
CMapView::OnChangeClassMode()
{
  AddMode( new CChangeClassMode );
}
//========================================================================
#define N_REP 3
//========================================================================
static CString
Reper2Str( CDrawObject* pRpr )
{
  CCircle* crcl = (CCircle*)pRpr->GetActualCont()->GetHead()->GetNext();
  CPoint cp( crcl->GetCoord());
  CText* glat = (CText*)crcl->GetNext()->GetNext();
  CText* slat = (CText*)glat->GetNext()->GetNext();
  CText* mlat = (CText*)slat->GetNext()->GetNext();
  CText* glon = (CText*)mlat->GetNext()->GetNext()->GetNext();
  CText* slon = (CText*)glon->GetNext()->GetNext();
  CText* mlon = (CText*)slon->GetNext()->GetNext();
  CString rpt;
  rpt.Format( "(%s\xb0%s'%s\", %s\xb0%s'%s)",
              LPCSTR( *glat ), LPCSTR( *slat ), LPCSTR( *mlat ),
              LPCSTR( *glon ), LPCSTR( *slon ), LPCSTR( *mlon ));
  return rpt;
}
//========================================================================
enum REP_CMP
{
  NO_ERR = 0,
  EQ_DEC = 1,
  EQ_POL = 2,
  ER_POS = 3,
  ER_DIF = 4
};
//========================================================================
static REP_CMP
Test2Repers( CDrawObject* pRep0, CDrawObject* pRep1 )
{
  if( pRep0->Entry() == pRep1->Entry())
    return NO_ERR;

  CPoint d0( *pRep0->GetAnyPoints());
  CPoint d1( *pRep1->GetAnyPoints());
  if( d0 == d1 )
    return EQ_DEC;

  Polar pl0( pRep0 );
  Polar pl1( pRep1 );
  if( pl0 == pl1 )
    return EQ_POL;

  if( pl0.lon < pl1.lon && d0.x > d1.x ||
      pl0.lon > pl1.lon && d0.x < d1.x ||
      pl0.lat < pl1.lat && d0.y < d1.y ||
      pl0.lat > pl1.lat && d0.y > d1.y )
    return ER_POS;

  Point3d p0 = ::Pol2Dec( pl0, 1 );
  Point3d p1 = ::Pol2Dec( pl1, 1 );

  double nSec = ::acos(( p0.x * p1.x + p0.y * p1.y + p0.z * p1.z )) / Deg * 3600;
  if( nSec < 1. )
    return ER_DIF;

  return NO_ERR;
}
//========================================================================
static CString
RepRep( CDrawObject* pRpr )
{
        CString r;
  CPoint* p = pRpr->GetAnyPoints();
  r.Format( "X=%-8d Y=%-8d %s", p->x, p->y, Reper2Str( pRpr ));
  return r;
}
//========================================================================
int
CMapView::Test1Reper( CDrawObject* pRep, ofstream* pFile, int nMode )
{
  static char* et[] =
  {
    "",
    "Наложение",
    "Дублирование",
    "Перекос",
    "Неразличимость"
  };
        CWaitCursor wc;

  int s = 0;
  for( CDrawObject* pTst ; pTst = m_ReperList.GetNext() ; )
  {
    REP_CMP e = Test2Repers( pRep, pTst );
    s += e != NO_ERR;

                if( e != NO_ERR )
    {
      if( pFile )
                        {
        CString rpt;
        rpt.Format( "%-48s %-48s %s\n", RepRep( pRep ), RepRep( pTst ), et[e] );
        *pFile << rpt;
                                if( nMode == 2 )
                                        return s;
                        }
      else
      {
        CString r0( RepRep( pRep ));
        CString r1( RepRep( pTst ));
        CString rpt;
        rpt.Format( "%s\n\n%s\n%s\n\nПоказать реперы?", et[e], r0, r1 );
        int r = AskBox( rpt, 1 );

        if( r == IDCANCEL )
          return -s;

        if( r == IDYES )
        {
          ClearSelection( 0, 0 );
          CBox box( pRep->GetBoundBox() | pTst->GetBoundBox());
          int s = 2 * WindowSize2Scale( box.Size());
          SetScale( s, box.CenterPoint());
          UpdateWindow();
          SelectObject( pRep->Ident(), 1 );
          SelectObject( pTst->Ident(), 1 );
        }
      }
    }
  }
  return s;
}
//========================================================================
void
CMapView::OnTestReper()
{
  CDrawObject* pRep = GetSelectObject( 0 );
  if( !pRep || pRep->Class() != REPER )
    ErrBox( "Необходимо выделить один репер" );
  else
  {
    GetReperList();
    m_ReperList.SetFirst();
    int s = Test1Reper( pRep, 0, 4 );
    CString m;
    if( s > 0 )
      m.Format( "Проверка завершена.\nОшибок вывлено %d.", s );
    else
    if( s < 0 )
      m.Format( "Проверка прервана.\nОшибок вывлено %d.", -s );
    else
      m = "Проверка завершена.\nНекорректных реперов не выявлено";
    InfBox( m );
  }
}
//========================================================================
void
CMapView::OnTestAllRepers()
{
  CRepDialog dlg;
        if( dlg.DoModal() != IDOK )
                return;

  CString pfn( dlg.GetPath());
        int bRepes = dlg.GetRepers();
        int bOneErr = dlg.GetOneError();
        int bAllErr = dlg.GetAllErrors();
        int mode = bRepes + bOneErr * 2 + bAllErr * 4;

  int bWriteFile = !pfn.IsEmpty();

  ofstream pFile;

  if( bWriteFile )
  {
    pFile.open( pfn, ios::out | ios::trunc, SH_DENYNO );
    if( !pFile.good())
    {
      ErrBox( "Невозможно открыть файл\n"
              "для сохранения протокола" );
      return;
    }
  }

  GetReperList();
  CDrawObject* pRep = m_ReperList.SetFirst();
  int s = 0, e = 0;
  while(( pRep = m_ReperList.GetNext()) && e >= 0 )
  {
    m_ReperList.SavePosition();
    if( mode == 1 )
    {
      if( bWriteFile )
        pFile << RepRep( pRep ) << endl;
      else
      {
        CString rpt;
        rpt.Format( "%s\n\nПоказать репер?", RepRep( pRep ));
        int r = AskBox( rpt, 1 );
        if( r == IDCANCEL )
          break;

        if( r == IDYES )
        {
          ClearSelection( 0, 0 );
          CBox box( pRep->GetBoundBox());
          int s = 2 * WindowSize2Scale( box.Size());
          SetScale( s, box.CenterPoint());
          UpdateWindow();
          if( !GetObjectPtr( pRep->Ident()))
            m_ClipData.AddObject( pRep );
          SelectObject( pRep->Ident(), 1 );
        }
      }
    }
    else
    {
      e = Test1Reper( pRep, bWriteFile ? &pFile : 0, mode );
      s += abs( e );
    }
    m_ReperList.RestPosition();
  }
  ClearSelection( 0, 0 );

  if( bWriteFile )
    pFile.close();

  if( mode != 1 )
  {
    CString m;
    if( s == 0 )
      m  = "Проверка завершена.\nНекорректных реперов не выявлено";
    else
    if( e < 0 )
      m.Format( "Проверка прервана.\nОшибок вывлено %d.", s );
    else
      m.Format( "Проверка завершена.\nОшибок вывлено %d.", s );
    InfBox( m );
  }
}
//========================================================================
CObjTreeList*
CMapView::GetReperList()
{
  CWaitCursor wc;
  if( !m_bReperListCreated )
  {
    CWaitCursor wc;
    ReportHint( "Загрузка реперов..." );
    long ClassArr[] = { REPER };
    CSize ds( GetDocument()->GetSize());
    CRect rct( 0, 0, ds.cx, ds.cy );
    for( ResultCode r = R_REPEAT ; r == R_REPEAT ; )
    {
      int nCnt;
      TGstream buf;
      r = m_MapStore.SelectByClass( rct, ClassArr, 1, buf, &nCnt );
      if( r <= R_REPEAT )
      {
        char* pSrc = buf.str();
        while( --nCnt >= 0 )
        {
          memcpy( m_pDRH->GetBriefPtr(), pSrc, m_pDRH->GetBriefSize());
          char* pNxt = pSrc + m_pDRH->GetDataSize() + m_pDRH->GetBriefSize();
          CDrawObject* pObj = TranslateObjectFromMap( &pSrc );
          if( pObj )
            m_ReperList.AddObject( pObj );
        }
      }
    }
    ClearHint();
    m_bReperListCreated = 1;
  }
  return &m_ReperList;
}
//========================================================================
void
CMapView::OnCloneLines()
{
  CDrawObject* pObj = GetSelectObject( 1 );
  if( !pObj )
    return;

  int np;
  CPoint* pp = pObj->GetPolyline( &np );
  if( !pp )
  {
    ErrBox( "Необходимо выделить линейный объект!" );
    return;
  }

  CCloneParamDialog dlg( pp, np );
  if( dlg.DoModal() != IDOK )
    return;

  int n = dlg.GetRep();
  int r0 = dlg.GetStep();

  ClearSelection();

  CWaitCursor wc;

  CObjList lst;
  int nl = 0, r = 0;
  while( --n >= 0 )
  {
    TGstream dst;
    r += r0;
    int nn = SmartCloneLine( dst, pp, np, r );
    if( nn )
    {
      CDrawObject* pTmp = pObj->CopyObject();
      long ev;
      m_MapStore.GetUniId( &ev );
      pTmp->SetEntry( ev );
      pTmp->SetPoly((CPoint*)dst.str(), nn );
      CDrawObject* pNew = pTmp->Rebuild();
      delete pTmp;
      AddObject( pNew, 1 );
      lst.AddHead( pNew );
      ++nl;
      ShowObjectBox( pNew );
    }
    if( nl )
      m_Undo.AddItem( new CUndoAddItem( this, &lst, 0 ), this );
  }
}
//========================================================================
#ifndef _FULL_PROJECT
//========================================================================
void
CMapView::OnMakeStatistics( UINT id )
{
  CPoint* pRgn = 0;
  int rgnSize = 0;

  if( id == IDM_MAKE_RGN_STATISTICS )
  {
    CString path( App.GetProjectFilePath( "region.txt" ));
    ifstream rin( path, ios::in );

    if( rin.bad())
    {
      ErrBox( "Не найден файл описания региона!" );
      return;
    }

    char ttl[256];
    int nr;

    rin >> ttl >> nr >> rgnSize;

    if( rin.eof() || _stricmp( ttl, "Region" ) || nr != 1 || rgnSize < 3 )
    {
      ErrBox( "Некорректный регион!" );
      return;
    }

    pRgn = new CPoint[rgnSize];
    for( int i = rgnSize ; --i >= 0 ; )
    {
      double x, y;
      rin >> x >> y;
      pRgn[i].x = x * 100;
      pRgn[i].y = y * 100;
      if( rin.eof())
      {
        ErrBox( "Некорректный регион!" );
        return;
      }
    }
  }

  CFileDialog* fdlg = new CFileDialog( 1, "txt", 0, OFN_OVERWRITEPROMPT,
                                       "All files (*.*)|*.*||", this );

  int r = DoModalDlg( fdlg, CE_PAR_DLG );
  CString fName( fdlg->GetPathName());

  delete fdlg;

  if( r != IDOK ||
      FileExist( fName ) &&
      AskBox( Compose( "Файл \"", fName, "\" уже существует.\nПереписать?", 0 )) != IDYES )
    return;

  CWaitCursor wc;
  long t[FULL_RANGE];
  int  c[FULL_RANGE];

  ReportHint( "Пожалуйста, подождите..." );
  m_MapStore.AcountAllObjects( t, pRgn, rgnSize );

  for( int i = CELEM( c ) ; --i >= 0 ; c[i] = i );

  int l, ml = 18;

  for( int i = 1 ; i < FULL_RANGE ; ++i )
  {
    int t0 = t[i], t1 = c[i];
    int nl = strlen( m_pObjDef->GetName( t1 ));
    if( t[i] && ml < nl )
      ml = nl;
    int j;
    for( j = i ; --j > 0 && strcmp( m_pObjDef->GetName( c[j] ), m_pObjDef->GetName( t1 )) < 0 ;
         t[j+1] = t[j], c[j+1] = c[j] );
    t[j+1] = t0;
    c[j+1] = t1;
  }

  ofstream os( fName, ios::out | ios::trunc, SH_DENYWR );

  CTime tm = CTime::GetCurrentTime();
  char line[256];
  char * pl = line + ml + 8;
  *--pl = 0;
  *--pl = '\n';
  while( --pl >= line )
    *pl = '=';

  os << line
     << "Проект - " << GetDocument()->GetDocName() << endl
     << "Дата   - " << tm.Format( "%d.%m.%y" ) << endl
     << "Время  - " << tm.Format( "%H:%M:%S" ) << endl
     << line;
  int i = CELEM( t );
  while( --i >= 0 )
  {
    LPCTSTR n = m_pObjDef->GetName( c[i] );
    if( t[i] && ( !m_pObjDef->GetDerived( c[i] ) || n[0] == '#' ) || !c[i]  )
    {
      char d[16];
      sprintf_s( d, "%d", t[i] );
      os << n;
      for( l = ml + 6 - strlen( n ) - strlen( d ) ; --l >= 0 ; os << '.' );
      os << d << endl;
    }
  }
  os << line;
  os.close();

  char m[256];
  sprintf_s( m, "Отчет записан в файл\n%s", (LPCSTR)fName );
  InfBox( m );

  ShowSelectedNum();
}
//========================================================================
void
CMapView::OnLoader()
{
  CLoadLawDialog dlg( "Управление загрузкой", m_LoadMap,
                      m_pObjDef, this, IDD_SELEC_CLASSES_DIALOG, 1 );
  if( DoModalDlg( &dlg, CE_PAR_DLG ) == IDOK )
  {
    m_MapStore.SetLoadMap( m_LoadMap );
    ReloadAll();
  }
}
//========================================================================
void
CMapView::OnConvertor()
{
  int wsb = m_ConvDlg.IsWindowVisible();
  if( !wsb )
  {
    RemoveMode( IDM_REISSHINA_MODE );
    m_ConvDlg.Activate();
  }
  m_ConvDlg.ShowWindow( wsb  ? SW_HIDE : SW_SHOW );
}
//========================================================================
void
CMapView::OnFindObject()
{
  int wsb = m_FindObjectDlg.IsWindowVisible();
  m_FindObjectDlg.ShowWindow( wsb ? SW_HIDE : SW_SHOW );
}
//========================================================================
void
CMapView::OnObjectAdm()
{
  if( !m_AbstractDlg.IsActive())
    m_AbstractDlg.Activate( this );
  else
    m_AbstractDlg.OnCancel();
}
//========================================================================
void
CMapView::OnLineTemplEditor()
{
  if( !m_ShabDlg.IsActive())
    m_ShabDlg.Activate( this );
  else
    m_ShabDlg.OnCancel();
}
//========================================================================
void
CMapView::OnToolbarAdm()
{
  CToolbarDlg dlg;
  DoModalDlg( &dlg, CE_PAR_DLG );
}
//========================================================================
//========================================================================
void
CMapView::AttachmentLines( LPCTSTR fName, int* lct, int* wct, int pct, int bct,
                           int ptn, int pen, int btn, int ben )
{
  CWaitCursor wc;

  bool bCommNum = ptn == btn && pen == ben;

  ofstream os( fName, ios::out | ios::trunc, SH_DENYNO );

  int sol = m_ScaleOnLoading;
  m_ScaleOnLoading = 0;
  int n = 0, g = 0;
  CViewDC DC( this );
  ksi_obj ksi_dc = ksi_make_win_dc( GetSafeHwnd(), DC.GetSafeHdc());

  ClearSelection( 0, 0 );
  m_Undo.RemoveAll();

  m_MapStore.StartClassScan( 0 );

  CMainWin* pmw = (CMainWin*)AfxGetMainWnd();
  pmw->ShowProgress( 0, m_MapStore.GetMaxPosScan());

  TGstream buf;
  for( CDrawObject* pPllObj = 0 ;; delete pPllObj, pPllObj = 0 )
  {
    buf.seekp( 0 );
    if( m_MapStore.NextClassScan( buf ) != R_OK )
      break;

    pmw->SetProgress( m_MapStore.GetPosScan());

    char* pSrc = buf.str();

    memcpy( m_pDRH->GetBriefPtr(), pSrc, m_pDRH->GetBriefSize());

    CIdent pi( m_pDRH->GetIdent());
    int* pc;
    for( pc = lct ; *pc != 0 && *pc != pi.Class() ; ++pc );
    if( !*pc )
      continue;

    ++g;

    ksi_obj ksi_hst = KsiCall( "get-host-class-list", KSI_MK_SINT( pi.Class()), 0 );
    long ClassArr[FULL_RANGE], nClass = 0;

    for( ; ClassArr[nClass] = wct[nClass] ; ++nClass );

    if( !nClass )
      continue;

    pPllObj = TranslateObjectFromMap( &pSrc );
    if( !pPllObj )
      continue;

    int nPnt;
    CPoint* pPnt = pPllObj->GetPolyline( &nPnt );
    if( !pPnt )
      continue;

    CPoint* cp[] = { &pPnt[0], &pPnt[nPnt-1] };

    CRect r0( *cp[0], *cp[0] );
    CRect r1( *cp[1], *cp[1] );
    int const R = 160;
    r0.InflateRect( R, R );
    r1.InflateRect( R, R );
    CRect rc0( r0.left, r0.top, r0.right, r0.bottom );
    CRect rc1( r1.left, r1.top, r1.right, r1.bottom );

    int nCnt0 = 0, nCnt1 = 0;

    buf.seekp( 0 );
    pSrc = buf.str();

    m_MapStore.SelectByClass( rc0, ClassArr, nClass, buf, &nCnt0 );
    m_MapStore.SelectByClass( rc1, ClassArr, nClass, buf, &nCnt1 );

    CPoint prt[2];
    int f = 0, q = 0, md0 = R, md1 = R, nCnt = nCnt0 + nCnt1;
    for( CDrawObject* pPntObj = 0 ; f != 3 && --nCnt >= 0 ; delete pPntObj )
    {
      pPntObj = TranslateObjectFromMap( &pSrc );
      if( !pPntObj )
        continue;

      CPoint* pt = pPntObj->GetPoint();
      if( !pt )
        continue;

      if( pt[0] == *cp[0] )
        f |= 1;
      else
      if( pt[0] == *cp[1] )
        f |= 2;
      else
      {
        int d0 = hipo( *cp[0] - pt[0] );
        int d1 = hipo( *cp[1] - pt[0] );
        if( d0 < d1 && d0 < md0 )
        {
          prt[0] = pt[0];
          md0 = d0;
          q |= 1;
        }
        if( d1 < d0 && d1 < md1 )
        {
          prt[1] = pt[0];
          md1 = d1;
          q |= 2;
        }
      }
    }

    if( f == 3 )
      continue;

    SetKsiHiResolutionX( pPllObj->Status() & 0x80 );

    int t = 0, w = 0;

    for( int i = 2, msk = 2 ; --i >= 0 ; msk >>= 1 )
    {
      if( f & msk )
        continue;

      if( q & msk )
      {
        *cp[i] = prt[i];
        t = 1;
        continue;
      }

      int const BR = 100;
      int const BB = 6; //Строение

      CRect rc( cp[i][0].x - BR, cp[i][0].y - BR, cp[i][0].x + BR, cp[i][0].y + BR );

      buf.seekp( 0, ios::beg );

      ResultCode r = R_REPEAT;
      int tCnt;
      for( tCnt = 0 ; !tCnt && r == R_REPEAT ;
           r = m_MapStore.SelectByRect( rc, BB, buf, &tCnt ));

      int tpc = tCnt ? bct : pct;
      int tab = tCnt ? btn : ptn;
      int ent = bCommNum || tCnt ? ben++ : pen++;

      ksi_obj ksi_env = ksi_nil;
      int tx = cp[i][0].x + R;
      int ty = cp[i][0].y - R;
      ksi_obj ksi_pt = KsiPoint( *cp[i] );
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "x0" ), KSI_CAR( ksi_pt ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "y0" ), KSI_CDR( ksi_pt ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "a" ), ksi_zero );
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "tx" ), ksi_int2num( tx ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "ty" ), ksi_int2num( ty ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "ta" ), ksi_zero );
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "number" ), ksi_number2string( ksi_int2num( ent ), KSI_MK_SINT( 10 )));

      ksi_obj val =  KsiCall( "create-object",
                              KSI_MK_SINT( tpc ), ksi_zero,
                              ksi_env, 0 );

      ASSERT( KSI_PAIR_P( val ));
      if( !KSI_PAIR_P( val ))
        continue;

      TGstream tmp;
      dword mg = INSMAGIC;
      tmp.write((char*)&mg, sizeof mg );

      r = WriteData( 0, m_pDRH->GetRecPtr(), m_pDRH->GetRecSize(), &tmp );
      if( r != R_OK )
        continue;

      void* pData = tmp.str() + tmp.tellp();

      r = ConvertObject( m_Version, val, tmp );
      if( r != R_OK )
        continue;

      int nSize = tmp.tellp();

      CBox box( CalcBoundBox( m_Version, &pData ));

      m_pDRH->SetMagic( OBJMAGIC );
      m_pDRH->SetSize( nSize );
      m_pDRH->SetBox( box );
      m_pDRH->SetIdent( tpc, 0, tab, ent );
      long t;
      long c;
      m_MapStore.GetTimeClock( &t, &c );
      m_pDRH->SetTime( t );
      m_pDRH->SetClock( c );
      r = WriteData( 0, m_pDRH->GetRecPtr(), m_pDRH->GetRecSize(), &tmp );
      if( r != R_OK )
        continue;

      tmp.seekp( nSize );
      m_MapStore.AddObject( tmp );

      w = 1;

      os <<  tpc << ";"
         <<  tab << ";"
         <<  ent << endl;
    }

    n += t || w;

    if( t == 0 )
      continue;

    pPllObj->CalcBoundBox();
    TGstream tmp;
    dword mg = REPMAGIC;
    tmp.write((char*)&mg, sizeof mg );
    SetTimeClock( pPllObj );
    pPllObj->TranslateToMap( &tmp );
    m_MapStore.AddObject( tmp );
  }

  pmw->HideProgress();
  SetKsiHiResolution();

  os.close();
  char m[1024];
  sprintf_s( m, "Обработано %d из %d линейных объектов\n\n"
                "Параметры добавленных объектов записаны в файл\n%s", n, g, fName );
  InfBox( m );
  ClearHint();
  m_Undo.RemoveAll();

  ReloadAll();

  m_ScaleOnLoading = sol;
}
//========================================================================
void
CMapView::OnLineAttachment()
{
  CFileDialog* fdlg = new CFileDialog( 1, "txt", 0, OFN_OVERWRITEPROMPT,
                                       "All files (*.*)|*.*||", this );

  int drt = DoModalDlg( fdlg, CE_PAR_DLG );
  CString fName( fdlg->GetPathName());

  delete fdlg;

  if( drt == IDCANCEL ||
      FileExist( fName ) &&
      AskBox( Compose( "Файл \"", fName, "\" уже существует.\nПереписать?", 0 )) != IDYES )
    return;

  char LCMask[CLASS_RANGE/8];
  memset( LCMask, 0, sizeof LCMask );

  CLoadLawDialog dlg1( "Линейные объекты", LCMask, m_pObjDef, this, IDD_SELEC_CLASSES_DIALOG, 0 );
  if( DoModalDlg( &dlg1, CE_PAR_DLG ) != IDOK )
    return;

  char WCMask[CLASS_RANGE/8];
  memset( WCMask, 0, sizeof WCMask );

  CLoadLawDialog dlg2( "Точечные объекты", WCMask, m_pObjDef, this, IDD_SELEC_CLASSES_DIALOG, 0 );
  if( DoModalDlg( &dlg2, CE_PAR_DLG ) != IDOK )
    return;

  char PCMask[CLASS_RANGE/8];
  memset( PCMask, 0, sizeof PCMask );

  CSelectClassTabDlg dlg3( "Объект-заглушка", m_pObjDef, 1 );
  if( DoModalDlg( &dlg3, CE_PAR_DLG ) != IDOK )
    return;

  int ptn = dlg3.GetTabNum();
  int pen = dlg3.GetTabStart();

  CSelectClassTabDlg dlg4( "Объект-заглушка на строениии", m_pObjDef, 1 );
  if( DoModalDlg( &dlg4, CE_PAR_DLG ) != IDOK )
    return;

  int btn = dlg4.GetTabNum();
  int ben = dlg4.GetTabStart();

  int lct[CLASS_RANGE], lctn = 0;
  int wct[CLASS_RANGE], wctn = 0;
  int pct = dlg3.GetClass();
  int bct = dlg4.GetClass();

  for( int t = CLASS_RANGE  ; --t > 0 ; )
    if( !m_pObjDef->GetDerived( t ))
    {
      int n = t >> 3;
      int m = 1 << ( t & 7 );
      if( GetBit( LCMask, t ))
        lct[lctn++] = t;
      if( GetBit( WCMask, t ))
        wct[wctn++] = t;
  }
  lct[lctn] = 0;
  wct[wctn] = 0;

  AttachmentLines( fName, lct, wct, pct, bct,
                   ptn, max( pen, 1 ),
                   btn, max( ben, 1 ));
}
//========================================================================
//========================================================================
void
CMapView::OnDebug1()
{
}
//========================================================================
void
CMapView::OnDebug2()
{
}
//========================================================================
void
CMapView::OnDebug3()
{
}
//========================================================================
void
CMapView::OnDebug4()
{
}
//========================================================================
#endif // _FULL_PROJECT
