#include "stdafx.h"

#include "graphdoc.h"
#include "dwin.h"
#include "mview.h"
#include "miscmode.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
  #include "tgmapcliv1.h"
#endif
#include "mdoc.h"
#include "prndlg.h"
#include "dlg0.h"
#include "bldmode.h"
#include "ksi_type.h"
#include "undo.h"
#include "util.h"
#include "pfmode.h"
#include "mercator.h"
//=====================================================================
IMPLEMENT_DYNCREATE( CMainWin, CFrameWnd )
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
long CMainWin::m_bToolTips = 0;
long CMainWin::m_bPolarCoord = 0;
//=====================================================================
UINT CMainWin::indicators[] =
{
  IDM_STS0, IDM_STS1, IDM_STS2, IDM_STS3
};
//=====================================================================
BEGIN_MESSAGE_MAP( CMainWin, CFrameWnd )
  ON_WM_NCRBUTTONDOWN()
  ON_COMMAND_RANGE( IDW_TOOLBAR_BEG, IDW_TOOLBAR_END-1, OnToolbarSelect )
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_SIZING()
  ON_WM_MOVE()
  ON_WM_CLOSE()
  ON_UPDATE_COMMAND_UI_RANGE( IDM_STS1, IDM_STS3, OnUpdateSts )
  ON_COMMAND( IDM_DOCPRINTSETUP, OnDocPrintSetup )
  ON_COMMAND( IDM_POLARCOORD_MODE, OnPolarCoord )
  ON_UPDATE_COMMAND_UI( IDM_POLARCOORD_MODE , OnUpdatePolarCoord )

END_MESSAGE_MAP()
//=====================================================================
CMainWin::CMainWin()
{
  memset( m_pToolBar, 0, sizeof m_pToolBar );
}
//=====================================================================
CMainWin::~CMainWin()
{
  for( int i = CELEM( m_pToolBar ) ; --i >= 0 ; delete m_pToolBar[i] );
}
//=====================================================================
ToolBarPos const
CMainWin::TBS[] =
{
  { IDB_MAP_OBJECTS,  "Объекты",            "OBJECTBAR", IDM_OBJCTL_BEGIN, AFX_IDW_DOCKBAR_TOP,  0               },
  { IDB_MAP_VIEW,     "Документы",          "DOCBAR",    IDM_DOCCTL_BEGIN, RIGHT_OF,             IDB_MAP_OBJECTS },
  { IDB_MAP_FIND,     "Поиск",              "FINDBAR",   IDM_FNDCTL_BEGIN, RIGHT_OF,             IDB_MAP_VIEW    },
  { 0,                "Форма полилинии",    0,           IDD_SHAPE_DIALOG, RIGHT_OF,             IDB_MAP_FIND    },
  { IDB_MAP_MODE,     "Сервис-карта",       0,           0,                AFX_IDW_DOCKBAR_LEFT, 0               },
  { IDB_MAP_MODE_EXT, "Сервис-объекты",     0,           0,                RIGHT_OF,             IDB_MAP_MODE    },
  { IDB_MAP_MODE_EXT1,"Сервис-объекты",     0,           0,                RIGHT_OF,             IDB_MAP_MODE    },
  { IDB_MAP_PRINT,    "Печать",             0,           0,                BOTTOM_OF,            IDB_MAP_MODE    },
  { IDB_MAP_SEL_JMP,  "Просмотр выделения", 0,           0,                BOTTOM_OF,            IDB_MAP_MODE_EXT }
};
//=====================================================================
void
CMainWin::SetToolTips( int bSet )
{
  m_bToolTips = bSet;
  for( int i = CELEM( m_pToolBar ) ; --i >= 0 ; )
  {
    CControlBar* pBar = m_pToolBar[i];
    if( pBar )
    {
      UINT style = pBar->GetBarStyle();
      if( bSet )
        style |= CBRS_TOOLTIPS;
      else
        style &= ~CBRS_TOOLTIPS;
      pBar->SetBarStyle( style );
    }
  }
}
//=====================================================================
CControlBar*
CMainWin::CreateToolBar( int i )
{
  ToolBarPos const* ptbd = TBS + i;
  CControlBar* pCtrlBar = 0;
  UINT tbid = IDW_TOOLBAR_BEG + i;

  if( ptbd->bitMapID == ( GetProjectSLEnable() ? IDB_MAP_MODE_EXT : IDB_MAP_MODE_EXT1 ))
    return 0;

  if( ptbd->bitMapID )
  {
    CToolBar* pBar = 0;

    if( !ptbd->pSect )
    {
      pCtrlBar = pBar = new CToolBar;
      if( !pBar->Create( this,
                         WS_CHILD | WS_VISIBLE | CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
                         tbid ) ||
          !pBar->LoadToolBar( ptbd->bitMapID ))
      {
        ASSERT( 0 );
        delete pBar;
        return 0;
      }
    }
    else
      pCtrlBar = pBar = new CMyBar( this, ptbd->pSect, ptbd->baseID, tbid );
  }
  else
  {
    CDialogBar* pBar = 0;
    pCtrlBar = pBar = new CDialogBar;
    if( !pBar->Create( this, ptbd->baseID, CBRS_GRIPPER, tbid ))
    {
      ASSERT( 0 );
      delete pBar;
      return 0;
    }
  }
  pCtrlBar->SetWindowText( ptbd->pName );

  m_pToolBar[i] = pCtrlBar;
  if( !pCtrlBar )
    return 0;

  pCtrlBar->EnableDocking( CBRS_ALIGN_ANY );

  if( ptbd->neighbourID )
  {
    static int scc[][2] = { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 1 } };
    CControlBar* pNeighbour = GetBarPtr( ptbd->neighbourID );
    if( !pNeighbour )
      return pCtrlBar;

    CRect r;
    pNeighbour->GetWindowRect( &r );
    r.OffsetRect( r.Width()  * scc[ptbd->place][0],
                  r.Height() * scc[ptbd->place][1] );
    r.OffsetRect( -2, -2 );
    UINT dw = pNeighbour->GetBarStyle();
    static UINT st[][2] =
    {
      CBRS_ALIGN_LEFT   , AFX_IDW_DOCKBAR_LEFT,
      CBRS_ALIGN_TOP    , AFX_IDW_DOCKBAR_TOP,
      CBRS_ALIGN_RIGHT  , AFX_IDW_DOCKBAR_RIGHT,
      CBRS_ALIGN_BOTTOM , AFX_IDW_DOCKBAR_BOTTOM
    };
    UINT s = 0;
    for( int i = CELEM( st ) ; !s && --i >= 0 ; )
      if( dw & st[i][0] )
        s = st[i][1];
    DockControlBar( pCtrlBar, s, &r );
    if( !ptbd->bitMapID )
    {
      UINT s = 0;
      pCtrlBar->GetWindowRect( &r );
      CRect wr;
      GetWindowRect( &wr );
      r.left += wr.Width();
      DockControlBar( pCtrlBar, s, &r );
    }
  }
  else
    DockControlBar( pCtrlBar, ptbd->place, 0 );

  ShowControlBar( pCtrlBar, 1, 0 );
	
	return pCtrlBar;
}
//=====================================================================
int
CMainWin::CreateToolBars()
{
	int i;
  for( i = CELEM( m_pToolBar ) ; --i >= 0 ; delete m_pToolBar[i] )
    m_pToolBar[i] && m_pToolBar[i]->DestroyWindow();

  ToolBarPos const* ptbd = TBS;
  int nBars = min( CELEM( TBS ), CELEM( m_pToolBar ));

  EnableDocking( CBRS_ALIGN_ANY );

  for( i = 0 ; i < nBars ; ++i, ++ptbd )
		CreateToolBar( i );

  for( i = nBars ; --i >= 0 ; )
    if((--ptbd)->pSect )
    {
      CMyBar* pb = (CMyBar*)GetBarPtr( TBS[i].bitMapID );
      if( !pb || pb->GetBarSize())
        continue;
      ShowControlBar( pb, 0, 0 );
      m_pToolBar[i] = 0;
      delete pb;
    }

  SetToolTips( m_bToolTips );
  return 0;
}
//=====================================================================
int
CMainWin::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if( !m_Vstatus.Create( this ) ||
      !m_Vstatus.SetIndicators( indicators, CELEM( indicators )))
  {
    ASSERT( 0 );
    return -1;
  }

  m_Vstatus.SetFont((CFont*)m_Vstatus.GetDC()->SelectStockObject( SYSTEM_FONT ));
  SetFocus();

  CFrameWnd::OnCreate( lpCreateStruct );
  ShowWindow( SW_SHOWMAXIMIZED );

  return 1;
}
//=====================================================================
void
CMainWin::ShowProgress( int nLower, int nUpper )
{
  int width;
  UINT idr, style;
  m_Vstatus.GetPaneInfo( 0, idr, style, width );
  CRect sbr;
  m_Vstatus.GetWindowRect( &sbr );

  CRect prr( 4, 2, width, sbr.Height() - 2 );
  if( !IsWindow( m_Progress.m_hWnd ))
    m_Progress.Create( WS_CHILD, prr, &m_Vstatus, 100 );
  m_Progress.SetRange32( nLower, nUpper );
  m_Progress.SetPos( nLower );
  m_Progress.ShowWindow( SW_SHOW );
}
//=====================================================================
void
CMainWin::SetProgress( int nPos )
{
  m_Progress.SetPos( nPos );
}
//=====================================================================
void
CMainWin::OffsetProgress( int nPos )
{
  m_Progress.OffsetPos( nPos );
}
//=====================================================================
void
CMainWin::HideProgress()
{
  if( IsWindow( m_Progress.m_hWnd ))
    m_Progress.DestroyWindow();
}
//=====================================================================
char*
CMainWin::indicatorExmp[] =
{
  "",
  "X=-99999999.99м",
  "Y=-99999999.99м",
  " 9:99999999999 "
};
//=====================================================================
void
CMainWin::OnSize( UINT type, int cx, int cy )
{
  if( GetProjectSLEnable())
    pView->GetConductor()->DeclareEvent( CE_CRT_WND_MSG );

  CFrameWnd::OnSize( type, cx, cy );
  CDC* pDC = GetDC();
  pDC->SelectStockObject( SYSTEM_FONT );

  int rs = type == SIZE_RESTORED || type == SIZE_MAXIMIZED;
  if( rs )
  {
    CRect r;
    GetClientRect( &r );
    for( int pane = CELEM( indicatorExmp ) ; --pane >= 0 ; )
    {
      CSize ext( pDC->GetTextExtent( indicatorExmp[pane], strlen( indicatorExmp[pane] )));
      int width;
      UINT idr, style;
      m_Vstatus.GetPaneInfo( pane, idr, style, width );
      m_Vstatus.SetPaneInfo( pane, idr, style,
                             pane ? ext.cx : r.right > 16 ? r.right - 6 : 0 );
      r.right -= ext.cx + 6;
    }
  }
}
//=====================================================================
void
CMainWin::DirectUpdateSts()
{
  m_Vstatus.OnUpdateCmdUI( this, 1 );
}
//=====================================================================
void
CMainWin::OnUpdateSts( CCmdUI* pCmdUI )
{
  CString s;
  if( pView )
  {
    if( pCmdUI->m_nID == IDM_STS3 )
    {
      static UINT smId[] = { IDM_SCALE_MODE, IDM_INCSCALE_MODE, IDM_DECSCALE_MODE, 0 };
      CScaleBase* psb = (CScaleBase*)pView->GetActiveMode( smId );
      int sc = psb ? psb->GetCurScale() : pView->GetAltScale();
			CPoint t( psb ? psb->TopLeft() : pView->GetPosition());
      sc = MerkatorData.TrueScale( sc, Point( t.x, t.y ));
      s.Format( " 1:%d ", sc );
    }
    else
    {
      int by = pCmdUI->m_nID == IDM_STS2;
      if( m_bPolarCoord )
      {
        Polar pc;
  			CPoint t( pView->GetPosition());
        MerkatorData.Prj2Pol( Point( t.x, t.y ), pc );
        s.Format( " %c=%1.5f\xb0 ", by ? 'Ш' : 'Д', ( by ? pc.lat : pc.lon ) / Deg );
      }
      else
      {
        CPoint t( pView->Project2Grid( pView->GetPosition()));
				double v = ( by ? t.y : t.x ) * 1e-2 * SmInUnit;
        s.Format( SmInUnit >= 100 ? " %c=%1.0fм " : " %c=%1.2fм ", by ? 'Y' : 'X', v );
      }
    }
    m_Vstatus.SetPaneText( m_Vstatus.CommandToIndex( pCmdUI->m_nID ), s );
  }
}
//=====================================================================
void
CMainWin::PutHint( char const* text, UINT pane )
{
  if( IsWindow( m_Vstatus.m_hWnd ))
    m_Vstatus.SetPaneText( pane, text ? text : "" );
}
//=====================================================================
void
CMainWin::OnDocPrintSetup()
{
  CPrintMapMode* pm = (CPrintMapMode*)pView->GetMode( IDM_PRINT_MAP_MODE );
  int pv;
  if( pm )
  {
    pv = pm->IsPreview();
    pView->RemoveMode( pm );
  }
  CPrintSetupSheet dlg( this );
        DoModalDlg((CDialog*)&dlg, CE_PAR_DLG );
  if( pm )
    pView->AddMode( new CPrintMapMode( pv ));
}
//=====================================================================
void
CMainWin::OnNcRButtonDown( UINT nFlags, CPoint point )
{
  if( GetProjectSLEnable())
    pView->GetConductor()->DeclareEvent( CE_CRT_WND_MSG );

  CMenu pop;
  pop.CreatePopupMenu();

  for( int i = 0 ; i < CELEM( TBS ) ; ++i )
  {
    UINT id = IDW_TOOLBAR_BEG + i;
    CControlBar* pBar = m_pToolBar[i];
    if( !pBar )
      continue;
    pop.InsertMenu( -1, MF_BYPOSITION | MF_STRING, id, TBS[i].pName );
    pop.CheckMenuItem( id, MF_BYCOMMAND | ( pBar->IsWindowVisible() ? MF_CHECKED : MF_UNCHECKED ));
  }
  CPoint p;
  GetCursorPos( &p );
  pop.TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, p.x, p.y, this );
}
//=====================================================================
void
CMainWin::OnToolbarSelect( UINT nId )
{
  CControlBar* pBar = m_pToolBar[nId - IDW_TOOLBAR_BEG];
  ShowControlBar( pBar, !pBar->IsWindowVisible(), 0 );
}
//=====================================================================
int
CMainWin::GetBarNum( UINT bitMapID )
{
	int i;
  for(i = CELEM( TBS ) ; --i >= 0 && TBS[i].bitMapID != bitMapID ; );
  return i;
}
//=====================================================================
CControlBar*
CMainWin::GetBarPtr( UINT bitMapID )
{
  int i = GetBarNum( bitMapID );
  return i >= 0 ? m_pToolBar[i] : 0;
}
//=====================================================================
int
CMainWin::GetShapeNum()
{
  CDialogBar* pBar = (CDialogBar*)GetBarPtr( 0 );
  CComboBox* pBox = (CComboBox*)pBar->GetDlgItem( IDC_CTRL );
  return pBox->GetCurSel();
}
//=====================================================================
void
CMainWin::SetShapeNum( int n )
{
  CDialogBar* pBar = (CDialogBar*)GetBarPtr( 0 );
  CComboBox* pBox = (CComboBox*)pBar->GetDlgItem( IDC_CTRL );
  pBox->SetCurSel( max( 0, n ));
}
//=====================================================================
void
CMainWin::OnSizing( UINT nSide, LPRECT lpRect )
{
  if( GetProjectSLEnable())
    pView->GetConductor()->DeclareEvent( CE_CRT_WND_MSG );

  int const MIN_WIDTH  = 1000;
  int const MIN_HEIGHT = 750;

  if( lpRect->right - lpRect->left < MIN_WIDTH )
    switch( nSide )
    {
      case WMSZ_LEFT :
      case WMSZ_TOPLEFT :
      case WMSZ_BOTTOMLEFT : lpRect->left = lpRect->right - MIN_WIDTH;
           break;

      case WMSZ_RIGHT :
      case WMSZ_TOPRIGHT :
      case WMSZ_BOTTOMRIGHT : lpRect->right = lpRect->left + MIN_WIDTH;
           break;
    }

  if( lpRect->bottom - lpRect->top < MIN_HEIGHT )
    switch( nSide )
    {
      case WMSZ_TOP :
      case WMSZ_TOPLEFT :
      case WMSZ_TOPRIGHT : lpRect->top = lpRect->bottom - MIN_HEIGHT;
           break;

      case WMSZ_BOTTOM :
      case WMSZ_BOTTOMLEFT :
      case WMSZ_BOTTOMRIGHT : lpRect->bottom = lpRect->top + MIN_HEIGHT;
           break;
    }

  CWnd::OnSizing( nSide, lpRect );
}
//=====================================================================
void
ReportHint( char const* rep )
{
  ((CMainWin*)App.GetMainWnd())->PutHint( rep );
}
//=====================================================================
void
ReportHint( char const* rep, int par )
{
  CString s;
  s.Format( "%s %d", rep, par );
  ((CMainWin*)App.GetMainWnd())->PutHint( s );
}
//=====================================================================
void
CMainWin::OnPolarCoord()
{
  m_bPolarCoord ^= 1;
}
//=====================================================================
void
CMainWin::OnUpdatePolarCoord( CCmdUI* pCmdUI )
{
  int tm = MerkatorData.TrueMerkator();
  pCmdUI->Enable( tm );
  pCmdUI->SetCheck( m_bPolarCoord &= tm );
}
//=====================================================================
void
CMainWin::OnClose( void )
{
#ifndef _FULL_PROJECT
  CFrameWnd::OnClose();
#else
  int rt = GetProjectSLEnable() && CTGMapCliV1::FireCloseFrameWnd();
  if( !rt )
    CFrameWnd::OnClose();
#endif
}
//=====================================================================
