#include "stdafx.h"

#include "ksi_type.h"
#include "joke.h"
#include "drawprim.h"
#include "drawobj.h"
#include "graphdoc.h"

#include "appmode.h"
#include "miscmode.h"
#include "selmode.h"
#include "bldmode.h"
#include "mdoc.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
  #include "subldlg.h"
#else
  #include "tgmapmole.h"
#endif
#include "mview.h"
#include "dwin.h"
#include "linetmpl.h"
#include "util.h"
#include "ksiutil.h"
#include "mathutil.h"
#include "measmode.h"
#include "sltrmode.h"
#include "rstrmode.h"
#include "prndlg.h"
#include "mercator.h"
#include  <io.h>
//=====================================================================
IMPLEMENT_DYNCREATE( CMapView, CScrollView )
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CMapView*  pView = 0;
CConductor CMapView::m_Conductor;
long       CMapView::m_DimScaleRatio = 10;
long       CMapView::m_Sensibility = 5;
long       CMapView::m_MaxDelta = 25;
long       CMapView::m_Deviation = 0;
long       CMapView::m_bSlow = 1;
long       CMapView::m_bTextShadow = 0;
long       CMapView::m_bScrollBars = 0;
long       CMapView::m_X0 = 0;
long       CMapView::m_Y0 = 0;
long       CMapView::m_XStep = 100000;
long       CMapView::m_YStep = 100000;
long       CMapView::m_bUseGrid = 0;

CConvDlg        CMapView::m_ConvDlg;
CShabDlg        CMapView::m_ShabDlg;
CAbstractDialog CMapView::m_AbstractDlg;
CFindObjectDlg  CMapView::m_FindObjectDlg;
//=========================================================================
//=====================================================================
BEGIN_MESSAGE_MAP( CMapView, CScrollView )
//=====================================================================
  ON_WM_SYSCHAR()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_RBUTTONDBLCLK()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONUP()
  ON_WM_MBUTTONDBLCLK()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEWHEEL()
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_PAINT()
  ON_WM_SETCURSOR()
  ON_WM_VSCROLL()
  ON_WM_HSCROLL()
  ON_WM_TIMER()
  ON_WM_SIZE()


  ON_COMMAND( IDM_CLONE_LINES, OnCloneLines )
  ON_COMMAND( IDM_BACKUP_PROJECT, OnBackupProject )

  ON_COMMAND_RANGE( IDM_SEL_JMP_HOME, IDM_SEL_JMP_END, OnJumpToSelect )
  ON_UPDATE_COMMAND_UI_RANGE( IDM_SEL_JMP_HOME, IDM_SEL_JMP_END, OnUpdateJumpToSelect )

  ON_COMMAND_RANGE( IDM_OBJEDIT_MODE, IDM_QUERYPARAM_MODE, OnObjectActionMode  )
  ON_COMMAND_RANGE( IDM_FIND_DIST_MODE, IDM_SELECT_POLY_MODE, OnMeasureMode )
  ON_COMMAND_RANGE( IDM_LOAD_TRACK_CMD, IDM_LOAD_POINTS_CMD, OnLoadTrackPointsCmd )

  ON_COMMAND( IDM_GRID_MODE, OnGrid )
  ON_UPDATE_COMMAND_UI( IDM_GRID_MODE, OnUpdateGrid )
  ON_COMMAND( IDM_COORDGRID_MODE, OnCoordGrid )
  ON_UPDATE_COMMAND_UI( IDM_COORDGRID_MODE, OnUpdateCoordGrid )
  ON_COMMAND( IDM_DRAW_GRID, OnDrawGrid )
  ON_UPDATE_COMMAND_UI( IDM_DRAW_GRID, OnUpdateDrawGrid )
  ON_COMMAND( IDM_SHOW_BOX, OnShowBox )
  ON_UPDATE_COMMAND_UI( IDM_SHOW_BOX, OnUpdateShowBox )

  ON_COMMAND( IDM_REISSHINA_MODE, OnReisshina )
  ON_COMMAND( IDM_SETREISSHINA_MODE, OnSetReisshina )
  ON_COMMAND( IDM_CROSS_MODE, OnCross )

  ON_COMMAND( IDM_COPY_MODE, OnCopyMode )
  ON_UPDATE_COMMAND_UI( IDM_COPY_MODE, OnUpdateCopyMode )

  ON_COMMAND( IDM_HAND_MODE, OnHandMode )
  ON_COMMAND( IDM_GOTO, OnGoTo )

  ON_COMMAND( IDM_INCSCALE_MODE, OnIncScaleMode )
  ON_COMMAND( IDM_DECSCALE_MODE, OnDecScaleMode )

  ON_COMMAND_RANGE( IDM_MAGNIT_MODE1, IDM_MAGNIT_MODE8, OnMagnitMode )
  ON_UPDATE_COMMAND_UI_RANGE( IDM_MAGNIT_MODE1, IDM_MAGNIT_MODE8, OnUpdateMagnitMode )

  ON_COMMAND( IDM_GETSELECT_MODE, OnGetSelectMode )
  ON_COMMAND( IDM_ADDSELECT_MODE, OnAddSelectMode )
  ON_COMMAND( IDM_SUBSELECT_MODE, OnSubSelectMode )
  ON_COMMAND( IDM_STREETPLAN_MODE, OnStreetPlanMode )

  ON_COMMAND( IDM_UNSELECT, OnUnselect )
  ON_COMMAND( IDM_SAVE_FULL_SCREEN, OnSaveFullScreen )

  ON_COMMAND( IDM_CONNECT_MODE, OnConnectMode )
  ON_COMMAND( IDM_CUT_IN_CMD, OnCutInCmd )
  ON_COMMAND( IDM_REM_CUT_IN_CMD, OnRemCutInCmd )

  ON_COMMAND_RANGE( IDM_DISTANCE, IDM_IMPROVE_PARAM, OnDistance )
  ON_COMMAND( IDM_ANGLESTEP, OnAngleStep )
  ON_COMMAND( IDM_GRIDSTEP,  OnGridStep  )
  ON_COMMAND( IDM_EDITOR,    OnEditor    )
  ON_COMMAND( IDM_LAYERDLG,  OnLayerDlg  )
  ON_COMMAND( IDM_SASDLG,    OnSASDlg    )

  ON_COMMAND_RANGE( IDM_OVERVIEW_MODE, IDM_REFRESH_OVERVIEW, OnOverview )
  ON_UPDATE_COMMAND_UI( IDM_OVERVIEW_MODE, OnUpdateOverview )

  ON_COMMAND( ID_FILE_PRINT, OnFilePrint )
  ON_COMMAND_RANGE( IDM_PRINT_MAP_MODE, IDM_PREVIEW_MAP_MODE, OnPrintMap )
  ON_UPDATE_COMMAND_UI_RANGE( IDM_PRINT_MAP_MODE, IDM_PREVIEW_MAP_MODE, OnUpdatePrintCommand )

  ON_COMMAND_RANGE( IDM_OBJCTL_BEGIN,  IDM_OBJCTL_END-1,  OnObjCommand )
  ON_COMMAND_RANGE( IDM_CONTEXT_BEGIN, IDM_CONTEXT_END-1, OnObjContextCommand )

  ON_UPDATE_COMMAND_UI_RANGE( IDM_OBJCTL_BEGIN, IDM_OBJCTL_END-1, OnUpdateObjCommandUI )
  ON_UPDATE_COMMAND_UI_RANGE( IDM_APPCOMMAND_BEGIN, IDM_APPCOMMAND_END-1, OnUpdateAppCommandUI )

  ON_COMMAND( IDM_CHANGE_CLASS_MODE, OnChangeClassMode )

  ON_COMMAND_RANGE( IDM_VIEWTRACE_MODE, IDM_CALCTRACE_MODE, OnSelectTraceMode )
  ON_COMMAND_RANGE( IDM_TRACE_CLOSE, IDM_TRACE_CLOSE, OnTraceClose )

  ON_COMMAND_RANGE( IDM_SHORTTRACE_OPEN, IDM_SHORTTRACE_CANCEL, OnShortTrace )

  ON_COMMAND( IDM_SUBL_ADMINISTATOR, OnSublayerAdm )
  ON_COMMAND( IDM_IMPROVE_POLYLINE, OnImprovePolyline )
  ON_COMMAND( IDM_TEST_REPER, OnTestReper )
  ON_COMMAND( IDM_TEST_ALL_REPERS, OnTestAllRepers )

  ON_COMMAND( IDM_SPACE_QUERY,    OnMakeSpaceQue   )
  ON_COMMAND( IDM_SET_NODE_QUERY, OnMakeSetNodeQue )

  ON_COMMAND( IDM_UNDO, OnUndo )
  ON_COMMAND( IDM_REDO, OnRedo )
  ON_COMMAND( IDM_RESTORE_MODE, OnRestoreMode)
  ON_UPDATE_COMMAND_UI( IDM_UNDO, OnUpdateUndo )
  ON_UPDATE_COMMAND_UI( IDM_REDO, OnUpdateRedo )

  ON_COMMAND( IDM_SERIAL_BUILDING, OnSerialBuildingCmd )
  ON_UPDATE_COMMAND_UI( IDM_SERIAL_BUILDING, OnUpdateSerialBuildingCmd )

  ON_COMMAND( IDM_TRANSFOCATOR, OnTransfocatorCmd )
  ON_UPDATE_COMMAND_UI( IDM_TRANSFOCATOR, OnUpdateTransfocatorCmd )
  ON_COMMAND( IDM_SHOW_SCROLLBARS, OnScrollbarsCmd )
  ON_UPDATE_COMMAND_UI( IDM_SHOW_SCROLLBARS, OnUpdateScrollbarsCmd )
  ON_COMMAND( IDM_SHOW_SCROLLBARS, OnScrollbarsCmd )
  ON_UPDATE_COMMAND_UI( IDM_SHOW_SCROLLBARS, OnUpdateScrollbarsCmd )
  ON_COMMAND( IDM_TOOL_TIPS_CMD, OnToolTipsCmd )
  ON_UPDATE_COMMAND_UI( IDM_TOOL_TIPS_CMD, OnUpdateToolTipsCmd )
  ON_COMMAND( IDM_USE_GRID_CMD, OnUseGridCmd )
  ON_UPDATE_COMMAND_UI( IDM_USE_GRID_CMD, OnUpdateUseGridCmd )
  ON_COMMAND( IDM_SHOW_AS_SELECTED, OnShowAsSeletedCmd )
  ON_UPDATE_COMMAND_UI( IDM_SHOW_AS_SELECTED, OnUpdateShowAsSeletedCmd )
  ON_COMMAND( IDM_TEXT_SHADOW, OnTextShadowCmd )
  ON_UPDATE_COMMAND_UI( IDM_TEXT_SHADOW, OnUpdateTextShadowCmd )

  ON_COMMAND_RANGE( IDM_SELECT_LINKED, IDM_SELECT_All_LINKED, OnSelectLinkedCmd )
  ON_UPDATE_COMMAND_UI_RANGE( IDM_SELECT_LINKED, IDM_SELECT_All_LINKED, OnUpdateSelectLinkedCmd )

#ifdef _FULL_PROJECT
  ON_COMMAND( IDM_EXTFINDOBJECT, OnExtFind )
  ON_COMMAND( IDM_EXTSETSCALE, OnExtSetScale )
  ON_COMMAND( IDM_EXTSPACEQUE, OnExtSpaceQue )

  ON_COMMAND_RANGE( IDM_SPECIAL_BEGIN, IDM_SPECIAL_END, OnSpecialCommand )
  ON_COMMAND_RANGE( IDM_DOCCTL_BEGIN, IDM_DOCCTL_END,   OnDocCommand     )
  ON_COMMAND_RANGE( IDM_FNDCTL_BEGIN, IDM_FNDCTL_END,   OnFindCommand    )
  ON_UPDATE_COMMAND_UI_RANGE( IDM_SPECIAL_BEGIN, IDM_SPECIAL_END, OnUpdateNoEditCommand )
  ON_UPDATE_COMMAND_UI_RANGE( IDM_DOCCTL_BEGIN,  IDM_DOCCTL_END,  OnUpdateNoEditCommand )
  ON_UPDATE_COMMAND_UI_RANGE( IDM_FNDCTL_BEGIN,  IDM_FNDCTL_END,  OnUpdateNoEditCommand )
  ON_UPDATE_COMMAND_UI( IDM_RESTORE_MODE, OnUpdateRestoreMode )
#else
  ON_COMMAND( IDM_LOADER,              OnLoader          )
  ON_COMMAND( IDM_CONVERTOR,           OnConvertor       )
  ON_COMMAND( IDM_FINDOBJECT,          OnFindObject      )
  ON_COMMAND( IDM_OBJECT_ADMINISTATOR, OnObjectAdm       )
  ON_COMMAND( IDM_LINE_TEMPL_EDITOR,   OnLineTemplEditor )
  ON_COMMAND( IDM_BAR_ADMINISTATOR,    OnToolbarAdm      )
  ON_COMMAND( IDM_SET_COORD_GRID_MODE, OnSetCoordGrid    )
  ON_COMMAND( IDM_LINE_ATTACHMENT,     OnLineAttachment  )
  ON_COMMAND_RANGE( IDM_MAKE_STATISTICS, IDM_MAKE_RGN_STATISTICS, OnMakeStatistics  )
  ON_COMMAND( IDM_DEBUG1, OnDebug1 )
  ON_COMMAND( IDM_DEBUG2, OnDebug2 )
  ON_COMMAND( IDM_DEBUG3, OnDebug3 )
  ON_COMMAND( IDM_DEBUG4, OnDebug4 )
#endif

  ON_COMMAND_RANGE( IDM_USER_RIGHTS_OK, IDM_USER_RIGHTS_OK_EXT, OnUserRightsOk )
  ON_COMMAND( IDM_USER_RIGHTS_CANCEL, OnUserRightsCancel )

END_MESSAGE_MAP()
//=====================================================================
static char szGrDefSection[]   = "GridParameters";
static char szPrDefSection[]   = "PrintParameters";
static char szBarSection[]     = "BarParameters";
static char szGeneralSection[] = "General";
static char szGridSection[]    = "Grid";
static CProfileVar pVars[] =
{
  &CMainWin::m_bToolTips,   szGeneralSection, "ToolTips",   1, 1, 0, 1,
  &CMainWin::m_bPolarCoord, szGeneralSection, "PolarCoord", 0, 1, 0, 1,

  &CSelectTraceMode::m_bNoInstructions, szGeneralSection, "STMNoInstructions", 0, 1, 0, 1,
  &CRestoreMode::m_bNoInstructions,     szGeneralSection, "RSMNoInstructions", 0, 1, 0, 1,
  &CDrawContainer::m_bShowAsSelected,   szGeneralSection, "ShowAsSelected",    1, 1, 0, 1,

  &CUndo::m_UndoMaxLength,     szGeneralSection, "UndoMaxLength", 40, 0, 0, 100,

  &CMapView::m_bUseGrid,       szGrDefSection,   "UseGrid",  1, 1, 0, 1,
  &CMapView::m_X0,             szGridSection,    "X0", 0, 0, 0x80000000, 0x7fffffff,
  &CMapView::m_Y0,             szGridSection,    "Y0", 0, 0, 0x80000000, 0x7fffffff,
  &CMapView::m_XStep,          szGridSection,    "DX", 50000, 0, -5000000, 5000000,
  &CMapView::m_YStep,          szGridSection,    "DY", 50000, 0, -5000000, 5000000,
  &CMapView::m_bTextShadow,    szGeneralSection, "TexrShadow",     0, 1, 0, 1,
  &CMapView::m_bSlow,          szGeneralSection, "SlidingZoom",    0, 1, 0, 1,
  &CMapView::m_bScrollBars,    szGeneralSection, "ScrollBars",     1, 1, 0, 1,
  &CMapView::m_Sensibility,    szGeneralSection, "Sensibility",   10, 0, 5, 100,
  &CMapView::m_MaxDelta,       szGeneralSection, "MaxDelta",      10, 0, 5, 25,

  &CMapView::m_DimScaleRatio,  szGeneralSection, "DimScaleRatio", 10, 0, 10, 100,

  &CPageSetupPage::m_layopt,        szPrDefSection, "Layout"          ,  0, 1, 0, 1,
  &CPageSetupPage::m_spc.top,       szPrDefSection, "TopSpace"        , 20, 0, 0, MAX_SPC_TOP,
  &CPageSetupPage::m_spc.bottom,    szPrDefSection, "BottomSpace"     , 30, 0, 0, MAX_SPC_BOTTOM,
  &CPageSetupPage::m_spc.left,      szPrDefSection, "LeftSpace"       , 20, 0, 0, MAX_SPC_LEFT,
  &CPageSetupPage::m_spc.right,     szPrDefSection, "RightSpace"      , 20, 0, 0, MAX_SPC_RIGHT,
  &CPageSetupPage::m_overlapVal,    szPrDefSection, "DocOverlap"      ,  2, 0, 0, MAX_DOCUMENT_OVERLAP,
  &CPageSetupPage::m_overlapEnb,    szPrDefSection, "DocOverlapEnb"   ,  0, 1, 0, 1,
  &CPageSetupPage::m_numerationEnb, szPrDefSection, "DocNumEnabled"   ,  0, 1, 0, 1,
  &CPageSetupPage::m_spaceEnb,      szPrDefSection, "DocSpaceEnabled" ,  0, 1, 0, 1,
  &CPageSetupPage::m_lineEnb,       szPrDefSection, "DocLineEnabled"  ,  0, 1, 0, 1,
  &CPageSetupPage::m_spaceVal,      szPrDefSection, "DocSpace"        ,  0, 0, 0, MAX_DOCUMENT_FRAME_SPACE,
  &CPageSetupPage::m_lineVal,       szPrDefSection, "DocLine"         ,  5, 0, 0, MAX_DOCUMENT_FRAME_LINE,
  &CPageSetupPage::m_startNumber,   szPrDefSection, "DocStartPageNum" ,  1, 0, 1, 9999,

  &CDocSetupPage::m_colorEnb,      szPrDefSection, "DocColorEnabled"     , 0, 1, 0, 1,
  &CDocSetupPage::m_autoSizeEnb,   szPrDefSection, "DocAutoSizeEnabled"  , 0, 1, 0, 1,
  &CDocSetupPage::m_autoScaleEnb,  szPrDefSection, "DocAutoScaleEnabled" , 0, 1, 0, 1,
  &CDocSetupPage::m_selectionEnb,  szPrDefSection, "DocSelectionEnabled" , 0, 1, 0, 1,
  &CDocSetupPage::m_selectOnlyEnb, szPrDefSection, "DocSelectOblyEnabled", 0, 1, 0, 1,
  &CDocSetupPage::m_curscaleEnb,   szPrDefSection, "DocCurScaleEnabled"  , 0, 1, 0, 1,
  &CDocSetupPage::m_scaleVal,      szPrDefSection, "DocScale"        , 5000, 0, 100, 300000,
  &CDocSetupPage::m_extraSpaceVal, szPrDefSection, "DocExtraSpace"   , 0, 0, 0, 100,

  &CGridDialog::m_nStepX,   szGrDefSection, "GridStepX"  , 100, 0, 10, 1000000,
  &CGridDialog::m_nStepY,   szGrDefSection, "GridStepY"  , 100, 0, 10, 1000000,
  &CGridDialog::m_nOffsetX, szGrDefSection, "GridOffsetX",  0, 0, 0x80000000, 0x7fffffff,
  &CGridDialog::m_nOffsetY, szGrDefSection, "GridOffsetY",  0, 0, 0x80000000, 0x7fffffff,
  0
};
//=====================================================================
CMapView::CMapView():
#ifndef _FULL_PROJECT
  m_pNodes( 0 ),
#endif
  m_pGotoDialog( 0 ),
  m_bReperListCreated( 0 ),
  m_bObjToolTips( 0 ),
  m_bCutIn( 0 ),
  m_bCutOut( 0 ),
  m_pCutIns( 0 ),
  m_pCutObj0( 0 ),
  m_pCutObj1( 0 ),
  m_bScaleOnSize( 1 ),
  m_AltScale( 0 ),
  m_Position( 0, 0 ),
  m_pMode( 0 ),
  m_BitmapSize( 0, 0 ),
  m_CrossAngle( 0 ),
  m_OldScale( -1 ),
  m_bSerialBuilding( 0 ),
  m_ScaleOnLoading( 1 ),
  m_pSTM( 0 ),
  m_pURMode( 0 ),
  m_nRgn( 0 ),
  m_pRgn( 0 ),
  m_bSelectLinked( 0 ),
  m_bSelectAllLinked( 0 ),
  m_bSublayerChanged( 0 ),
  m_bRedraw( 1 ),
  m_pSuspObj( 0 ),
  m_pUndoItem( 0 ),
  m_pCurSelObj( 0 ),
  m_pSublDlg( 0 ),
  m_bSpaceSelection( 0 ),
  m_pDelObj( 0 ),
  m_bDrawSel( 1 ),
  m_bFuLinksShow( 0 ),
  m_bFinding( 0 ),
  m_ScaleFactor( 1 ),
  m_nURMode( UR_NOMODE )
{
  pView = this;
}
//=====================================================================
void
CMapView::Clear( int bClose )
{
  CPoint c( m_Clip.CenterPoint());
  int s = GetTrueScale();
  static char gnr[] = "General";
  App.WriteProfileInt( gnr, "StartScale", s );
  App.WriteProfileInt( gnr, "StartX", c.x );
  App.WriteProfileInt( gnr, "StartY", c.y );
  if( !bClose )
    pView->ClearSelection( 0, 0 );
  DelAllModes();
  while( --m_nRgn >= 0 )
    delete[] m_pRgn[m_nRgn];
  delete m_pRgn;
  m_pRgn = 0;
  delete m_pUndoItem;
  m_pUndoItem = 0;
  m_Undo.RemoveAll();
#ifndef _FULL_PROJECT
  m_pSublDlg->DestroyWindow();
  delete m_pSublDlg;
  m_pSublDlg = 0;
  m_FindObjectDlg.DestroyWindow();
  delete m_pNodes;
#endif
  delete m_pURMode;
  m_pURMode = 0;

  m_ReperList.RemoveAll( 0, 0 );
  m_bReperListCreated = 0;

  for( CProfileVar* pv = pVars ; pv->var; ++pv )
    App.WriteProfileInt( pv->sect, pv->entry, *pv->var );

  for( int n = 2 ; --n >= 0 ; )
  {
    CString sm;
    int* pi = n ? (int*)CDrawContainer::m_bSASMap : (int*)m_ShowMap;
    for( int i = BIT_FULL_RANGE / 4 + 1 ; --i >= 0 ; )
    {
      CString ds;
      ds.Format( "%08x", *pi++ );
      sm += ds + ' ';
    }
    App.WriteProfileString( szGeneralSection,  n ? "SasMap" : "ShowMap", sm );
  }
}
//=====================================================================
CMapView::~CMapView()
{
  m_Conductor.FreeConnection();
  CStrokItem::DeletePen();
  CFillItem::DeleteBrush();
  CFontItem::DeleteFont();
  Clear( 1 );
  pView = 0;
  delete m_pGotoDialog;
}
//=====================================================================
void
CMapView::OnInitialUpdate()
{
  CScrollView::OnInitialUpdate();
  EnableWindow( 0 );

  m_bMultiSelection = App.GetProfileInt( "General", "MultiSelection", 0 );
  m_bObjToolTips    = App.GetProfileInt( "General", "ObjToolTips", 0 );
  SetScale( 1 );
  AddMode( new CContextMode );
  AddMode( new CSelectMode );
  AddMode( new CScaleMode );
  AddMode( new CDragMode );

  for( CProfileVar* pv = pVars ; pv->var; ++pv )
  {
    int val = App.GetProfileInt( pv->sect, pv->entry, pv->def );
    *pv->var = pv->bVal      ? val != 0 :
               val < pv->min ? pv->min  :
               val > pv->max ? pv->max  : val;
  }

#if 0
  AddMode( new CAutoScrollMode );
#endif

#ifndef _FULL_PROJECT
  m_FindObjectDlg.Create( IDD_FIND_OBJECT_DIALOG, this );
#endif
  m_pObjDef = GetDocument()->GetObjDef();
  ksi_defsym( "is-convertion?", ksi_false, ksi_current_env());

  CLegend DefLegend;
  AddLegend( &DefLegend );
}
//=====================================================================
void
CMapView::OnPrepareDC( CDC* pDC, CPrintInfo* pInfo )
{
  CScrollView::OnPrepareDC( pDC, pInfo );
  pDC->SetMapMode( MM_ANISOTROPIC );
  if( pDC->IsPrinting())
  {
    pDC->SetWindowExt( m_PrintPageSize );
    pDC->SetViewportExt( pDC->GetDeviceCaps( HORZRES ),
                         pDC->GetDeviceCaps( VERTRES ));
    pInfo->m_bContinuePrinting = 1;
    SetGraphicsMode( pDC->GetSafeHdc(), GM_ADVANCED );
  }
  else
  {
    pDC->SetWindowExt( m_WindowSize );
    pDC->SetViewportExt( m_ViewportSize );
  }
}
//=====================================================================
static void*
SaveBmp( CDC* pDC, CBitmap* pBmp, BITMAPINFO& bi )
{
  BITMAP bm;
  pBmp->GetBitmap( &bm );
  char* bu = new char[bm.bmWidthBytes*bm.bmHeight];
  memset( &bi, 0, sizeof BITMAPINFO );
  bi.bmiHeader.biSize = sizeof BITMAPINFO;
  bi.bmiHeader.biWidth  = bm.bmWidth;
  bi.bmiHeader.biHeight = bm.bmHeight;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = bm.bmBitsPixel;
  bi.bmiHeader.biCompression = BI_RGB;
  GetDIBits( pDC->GetSafeHdc(), HBITMAP( *pBmp ), 0, bm.bmHeight,
             bu, &bi, DIB_RGB_COLORS );
  return bu;
}
//=====================================================================
static int
Mix( int v1, int v2, int n, int d )
{
  return v1 + MulDiv( v2 - v1, n, d );
}
//=====================================================================
void
CMapView::OnDraw( CDC* pDC )
{
  static COLORREF WCLR( RGB( -1, -1, -1 ));
  static void* obu = 0;
  static void* nbu = 0;
  static BITMAPINFO obi;
  static BITMAPINFO nbi;

  CWaitCursor wc;

  int bDecScale = m_OldScale > m_AltScale;
  int bDrag = 0;

  CRect clip;
  pDC->GetClipBox( &clip );

  while( !m_BoxList.IsEmpty())
    clip |= m_BoxList.RemoveHead();

  pDC->LPtoDP( &clip );
  clip.InflateRect( 8, 8 );
  pDC->DPtoLP( &clip );
  pDC->SelectClipRgn( 0 );
  pDC->IntersectClipRect( &clip );

  CRect full;
  GetClientRect( &full );
  CRect f( DP2LP( full ));

  if( m_Clip != f )
  {
    m_OldClip = m_Clip;
    clip = m_Clip = f;
    m_OldScaleFactor = m_ScaleFactor;

    CPoint t( m_Clip.CenterPoint());
    m_ScaleFactor = MerkatorData.ScaleFactor( t );

    ClipObjects();
    m_bSublayerChanged = !( m_OldClip.TopLeft() == m_Clip.TopLeft() &&
                            m_OldClip.Width()  > m_Clip.Width() &&
                            m_OldClip.Height() > m_Clip.Height());
    bDrag = m_OldClip.Size() == m_Clip.Size();
  }

  m_bRedraw |= m_bSublayerChanged;

  if( m_BitmapSize != full.Size())
  {
    m_Bitmap.DeleteObject();
    m_SubBmp.DeleteObject();
    m_BitmapSize = full.Size();
    if( !m_Bitmap.CreateCompatibleBitmap( pDC, m_BitmapSize.cx, m_BitmapSize.cy ) ||
        !m_SubBmp.CreateCompatibleBitmap( pDC, m_BitmapSize.cx, m_BitmapSize.cy ))
      return;
    m_bSublayerChanged = m_bRedraw = 1;
  }

  CMemDC memDC( pDC );
  OnPrepareDC( &memDC );
  memDC.IntersectClipRect( &clip );
  memDC.SelectObject( &m_Bitmap );

  CDrawContainer::SetHR( IsHiResolution());

  int bSlow = m_bSlow && !bDrag && m_OldScale > 0 && ( m_OldScale != m_AltScale || m_bFinding );

  if( bSlow && !obu )
    obu = SaveBmp( pDC, &m_Bitmap, obi );

  if( m_bRedraw )
  {
    using namespace Gdiplus;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR GdiplusToken;
    GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );

    CMemDC bmpDC( pDC );
    OnPrepareDC( &bmpDC );
    bmpDC.IntersectClipRect( &clip );
    bmpDC.SelectObject( &m_SubBmp );

    if( bDrag )
    {
      CRect r1( &m_Clip ), r2( &m_Clip );

      if( m_Clip.left > m_OldClip.left )
        r1.right = r2.left = m_OldClip.right;
      else
        r2.right = r1.left = m_OldClip.left;
      if( m_Clip.top > m_OldClip.top )
        r1.top = m_OldClip.bottom;
      else
        r1.bottom = m_OldClip.top;

      int w = m_OldClip.Width(), h = m_OldClip.Height();
      memDC.BitBlt( m_OldClip.left, m_OldClip.top, w, h, &memDC, m_Clip.left, m_Clip.top, SRCCOPY );
      bmpDC.BitBlt( m_OldClip.left, m_OldClip.top, w, h, &bmpDC, m_Clip.left, m_Clip.top, SRCCOPY );

      for( int i = 2 ; --i >= 0 ; )
      {
        CRect pr( i ? r1 : r2 );
        memDC.SelectClipRgn( 0 );
        memDC.IntersectClipRect( &pr );
        if( m_bSublayerChanged )
        {
          bmpDC.SelectClipRgn( 0 );
          bmpDC.IntersectClipRect( &pr );
          bmpDC.FillSolidRect( &pr, WCLR );
          m_ClipData.Draw( &bmpDC, SUBLAYER );
        }
        memDC.BitBlt( pr.left, pr.top, pr.Width(), pr.Height(), &bmpDC, pr.left, pr.top, SRCCOPY );
        m_ClipData.Draw( &memDC, OBJLAYER );

        if( m_bDrawSel )
          DrawSelect( &memDC );
        DrawFuncLinks( &memDC );
      }
    }
    else
    {
      if( m_bSublayerChanged )
      {
        bmpDC.FillSolidRect( &clip, WCLR );
        m_ClipData.Draw( &bmpDC, SUBLAYER );
      }
      memDC.BitBlt( clip.left, clip.top, clip.Width(), clip.Height(),
                    &bmpDC, clip.left, clip.top, SRCCOPY );
      m_ClipData.Draw( &memDC, OBJLAYER );
      if( m_bDrawSel )
        DrawSelect( &memDC );
      DrawFuncLinks( &memDC );
    }

    GdiplusShutdown( GdiplusToken );

    m_bSublayerChanged = 0;
    m_bRedraw = 0;
  }

  if( bSlow )
  {
    CBitmap tbmp;
    tbmp.CreateCompatibleBitmap( pDC, full.Width(), full.Height());
    CMemDC tmpDC( pDC );
    OnPrepareDC( &tmpDC );
    tmpDC.SelectObject( &tbmp );
    tmpDC.SetStretchBltMode( COLORONCOLOR );

    nbu = SaveBmp( pDC, &m_Bitmap, nbi );

    int NewW = m_Clip.Width();
    int NewH = m_Clip.Height();
    int OldW = m_OldClip.Width();
    int OldH = m_OldClip.Height();
    int dl = m_Clip.left   - m_OldClip.left;
    int dt = m_Clip.top    - m_OldClip.top;
    int dr = m_Clip.right  - m_OldClip.right;
    int db = m_Clip.bottom - m_OldClip.bottom;
    int nw = MulDiv( NewW, NewW, OldW );
    int nh = MulDiv( NewH, NewH, OldH );
    int nl = m_Clip.left + MulDiv( dl, NewW, OldW );
    int nt = m_Clip.top  + MulDiv( dt, NewH, OldH );
    CSize ds( max( abs( dl ), abs( dr )), max( abs( dt ), abs( db )));
    ds = LP2DP( ds );
    int md = max( ds.cx, ds.cy ) >> 4;
    int n = min( 64, md );
    CRect or, nr;
    CRect* pr1 = bDecScale ? &or : &nr;
    CRect* pr2 = bDecScale ? &nr : &or;
    void*  pb1 = bDecScale ? obu : nbu;
    void*  pb2 = bDecScale ? nbu : obu;
    BITMAPINFO* pi1 = bDecScale ? &obi : &nbi;
    BITMAPINFO* pi2 = bDecScale ? &nbi : &obi;

    for( int i = n, j = 1 ; --i > 0 ; ++j )
    {
      nr.left   = Mix( m_Clip.left, nl, i, n );
      nr.top    = Mix( m_Clip.top,  nt, i, n );
      nr.right  = Mix( nw, NewW, j, n ) + nr.left;
      nr.bottom = Mix( nh, NewH, j, n ) + nr.top;

      or.left   = Mix( m_Clip.left, m_OldClip.left, j, n );
      or.top    = Mix( m_Clip.top,  m_OldClip.top,  j, n );
      or.right  = Mix( NewW, OldW, j, n ) + or.left;
      or.bottom = Mix( NewH, OldH, j, n ) + or.top;

      CRect r1( pr1 );
      int w1 = r1.Width(), h1 = r1.Height();
      int w0 = pi1->bmiHeader.biWidth, h0 = pi1->bmiHeader.biHeight;
      CRect r0( 0, 0, w0, h0 );

      if( r1.left < clip.left )
      {
        r0.left += MulDiv( w0, clip.left - r1.left, w1 );
        r1.left = clip.left;
      }
      if( r1.top < clip.top )
      {
        r0.bottom -= MulDiv( h0, clip.top - r1.top, h1 );
        r1.top = clip.top;
      }
      if( r1.right > clip.right )
      {
        r0.right += MulDiv( w0, clip.right - r1.right, w1 );
        r1.right = clip.right;
      }
      if( r1.bottom > clip.bottom )
      {
        r0.top -= MulDiv( h0, clip.bottom - r1.bottom, h1 );
        r1.bottom = clip.bottom;
      }

      tmpDC.FillSolidRect( clip, WCLR );

      for( int k = 4 ; --k >= 0 ; )
      {
        CRect tr1( r1 ), tr0( r0 );
        switch( k )
        {
          case 0: tr0.top -= MulDiv( r0.Height(), pr2->top - r1.bottom, r1.Height());
                  tr0.right += MulDiv( w0, pr2->right - r1.right, w1 );
                  tr1.right = pr2->right;
                  tr1.bottom = pr2->top;
                  break;
          case 1: tr0.bottom -= MulDiv( r0.Height(), pr2->bottom - r1.top, r1.Height());
                  tr0.left += MulDiv( w0, pr2->left - r1.left, w1 );
                  tr1.left = pr2->left;
                  tr1.top = pr2->bottom;
                  break;
          case 2: tr0.bottom -= MulDiv( h0, pr2->top - r1.top, h1 );
                  tr0.right += MulDiv( r0.Width(), pr2->left - r1.right, r1.Width());
                  tr1.right = pr2->left;
                  tr1.top = pr2->top;
                  break;
          case 3: tr0.top -= MulDiv( h0, pr2->bottom - r1.bottom, h1 );
                  tr0.left += MulDiv( r0.Width(), pr2->right - r1.left, r1.Width());
                  tr1.left = pr2->right;
                  tr1.bottom = pr2->bottom;
                  break;
        }
        StretchDIBits( tmpDC.GetSafeHdc(),
                       tr1.left, tr1.top, tr1.Width(), tr1.Height(),
                       tr0.left, tr0.top, tr0.Width(), tr0.Height(),
                       pb1, pi1, DIB_RGB_COLORS, SRCCOPY );
      }
      StretchDIBits( tmpDC.GetSafeHdc(),
                     pr2->left, pr2->top, pr2->Width(), pr2->Height(),
                     0, 0, pi2->bmiHeader.biWidth, pi2->bmiHeader.biHeight,
                     pb2, pi2, DIB_RGB_COLORS, SRCCOPY );

      pDC->BitBlt( clip.left, clip.top, clip.Width(), clip.Height(),
                   &tmpDC, clip.left, clip.top, SRCCOPY );
    }

    delete obu;
    obu = nbu;
    obi = nbi;
  }

  if( !m_bFinding )
  {
    delete obu;
    obu = 0;
  }

  pDC->BitBlt( clip.left, clip.top, clip.Width(), clip.Height(),
               &memDC, clip.left, clip.top, SRCCOPY );

  if( !CMeasureMode::Restore( this ) && !GetBuilding( 0 ))
    ShowSelectedNum();

  m_OldScale = m_AltScale;
  DrawModes( pDC );
}
//=====================================================================
int
CMapView::CanUndo( int beep )
{
  int yes = m_UndoLevel &&
            m_Undo.IsUndo() &&
            !GetBuilding( 0 ) &&
            !GetMode( IDM_CONNECT_MODE );
  if( beep && !yes )
    Beep( 440, 120 );
  return yes;
}
//=====================================================================
int
CMapView::CanRedo( int beep )
{
  int yes = m_UndoLevel &&
            m_Undo.IsRedo() &&
            !GetBuilding( 0 ) &&
            !GetMode( IDM_CONNECT_MODE );
  if( beep && !yes )
    Beep( 440, 120 );
  return yes;
}
//=====================================================================
void
CMapView::OnUndo()
{
  if( CanUndo( 1 ))
    m_Undo.Undo( this );
}
//=====================================================================
void
CMapView::OnRedo()
{
  if( CanRedo( 1 ))
    m_Undo.Redo( this );
}
//=====================================================================
void
CMapView::OnUpdateUndo( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( CanUndo( 0 ));
}
//=====================================================================
void
CMapView::OnUpdateRedo( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( CanRedo( 0 ));
}
//=====================================================================
void
CMapView::OnSysChar( UINT key_code, UINT count, UINT flags )
{
  switch( key_code )
  {
    case VK_BACK:
      if( ::GetKeyState( VK_SHIFT ) < 0 )
        OnRedo();
      else
        OnUndo();
      break;
    case 's' : case 'S' :
      SetShapeNum( GetShapeNum() + 1 );
      break;
    case 'a' : case 'A' :
      SetShapeNum( GetShapeNum() - 1 );
      break;
    case '0' : case '1' : case '2' : case '3' : case '4' :
    case '5' : case '6' : case '7' : case '8' : case '9' :
      SetShapeNum( key_code - '0' );
      break;
    case 'p' : case 'P' : case 'з' : case 'З' :
      DrawToPoint( 0 );
      break;
    case 'l' : case 'L' : case 'д' : case 'Д' :
      DrawToPoint( 1 );
      break;
    case 'v' : case 'V' : case 'м' : case 'М' :
      DrawToPoint( 2 );
      break;
    case 'x' : case 'X' : case 'ч' : case 'Ч' :
      DrawToPoint( 3 );
      break;
    case 'o' : case 'O' : case 'щ' : case 'Щ' :
      DrawToPoint( 4 );
      break;
    case 'q' : case 'Q' : case 'й' : case 'Й' :
      DrawToPoint( 5 );
      break;
    case 'r' : case 'R' :
      ReloadAll();
      break;
  }
}
//=====================================================================
static int
PurgeList( CObjList& lst )
{
  while( !lst.IsEmpty())
    delete lst.RemoveHead();
  return -1;
}
//=========================================================================
int
CMapView::TestAllObjects( int bFull )
{
  int ec = 0, ex = 0, i = 0;
  CObjList lst;
  for( int t = 0 ; t < CLASS_RANGE ; ++t )
  {
    ksi_obj kt = KSI_MK_SINT( t );
    if( KSI_FALSE_P( KsiCall( "defined-object?", kt, 0 )))
      continue;

    ksi_obj val = KsiCall( "create-default-object", kt, 0 );

    int e = !KSI_LIST_P( val );
    ec += e;
    if( e )
    {
      CString s;
      s.Format( "Ошибка построения объекта '%s'(#%d)\n"
                "Продолжать тестирование?", m_pObjDef->GetName( t ), t );

      switch( AskBox( s, 1 ))
      {
        case IDYES:     continue;
        case IDCANCEL : return PurgeList( lst );
      }
      break;
    }

    CDrawObject* pObj = new CDrawObject( val, CIdent( t, 0, 0, i++ ), 0 );
    if( bFull )
      lst.AddHead( pObj );
    else
      delete pObj;
  }

  if( !bFull )
    return ec;

  ksi_obj ctx_proc = ksi_eval_str( "make-building-context" );
  int e, bCnt = 1;
  for( POSITION pos0 = lst.GetHeadPosition() ; bCnt && pos0 ; lst.GetNext( pos0 ))
  {
    CDrawObject* pObj0 = lst.GetAt( pos0 );
    ksi_obj img0 = *pObj0;
    ksi_obj typ0 = KSI_MK_SINT( pObj0->Class());
    for( POSITION pos1 = pos0 ; bCnt && pos1 ; lst.GetNext( pos1 ))
    {
      ksi_obj l0, l1;
      CDrawObject* pObj1 = lst.GetAt( pos1 );
      ksi_obj img1 = *pObj1;
      ksi_obj typ1 = KSI_MK_SINT( pObj1->Class());
      l0 = ksi_apply_with_catch( ctx_proc, KSI_LIST6( typ0, ksi_zero, img0, ksi_zero, ksi_zero, typ1));
      l1 = ksi_apply_with_catch( ctx_proc, KSI_LIST6( typ1, ksi_zero, img1, ksi_zero, ksi_zero, typ0));
      e = KSI_EXN_P( l0 ) || KSI_EXN_P( l1 );
      ex += e;
      if( e )
      {
        CString s;
        s.Format( "Ошибка генерации контекста для объектов\n"
                  "'%s' (#%d) -> '%s' (#%d)\n"
                  "Продолжать тестирование?",
                  m_pObjDef->GetName( pObj0->Class()), pObj0->Class(),
                  m_pObjDef->GetName( pObj1->Class()), pObj1->Class());
        switch( AskBox( s, 1 ))
        {
          case IDYES: continue;
          case IDCANCEL : return PurgeList( lst );
        }
        bCnt = 0;
      }
    }
  }

  while( !lst.IsEmpty())
    delete lst.RemoveHead();

  int ac = ec + ex;
  if( ac )
  {
    CString c, x;
    if( ec )
      c.Format( "Ошибок построения зафиксировано %d\n", ec );
    if( ex )
      x.Format( "Ошибок контекстного построения зафиксировано %d\n", ex );
    InfBox( c + x );
  }

  return ac;
}
//=====================================================================
void
CMapView::InitSublayer()
{
  static char const SubSectName[] = "MAP_SUBLAYER";
  char buf[65536];

  App.GetProfileKeys( SubSectName, buf, sizeof buf );
  CObjList lst;

  CSize ps( GetDocument()->GetSize());
  int d;

  int n = 0;
  for( char* pb = buf ; *pb ; pb += strlen( pb ) + 1 )
  {
    CString b( App.GetProfileString( SubSectName, pb ));
    int l, t, w, h, a = 0;
    if( sscanf( b, "%d%d%d%d%d", &l, &t, &w, &h, &a ) >= 4 )
    {
      CString title( pb );
      int np = title.Find( '.' );
      if( np > 0 )
        title = title.Left( np );
      ksi_obj ksi_env = ksi_nil;
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "number" ),  ksi_str02string( title ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "source" ), ksi_str02string( pb ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "x0" ), ksi_int2num( l ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "y0" ), ksi_int2num( t ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "x1" ), ksi_int2num( w ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "y1" ), ksi_int2num( h ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "a" ),  ksi_int2num( a ));

      CIdent id( SUBLAYER_CLASS + 1, 0, 0, GetUniId());
      ksi_obj ksi_sub = KsiCall( "create-object",
                                 KSI_MK_SINT( SUBLAYER_CLASS ),
                                 ksi_zero, ksi_env, 0 );
      ASSERT( KSI_LIST_P( ksi_sub ));
      if( KSI_LIST_P( ksi_sub ))
      {
        CDrawObject* pObj = new CDrawObject( ksi_sub, id, 0 );
        lst.AddHead( pObj );
        App.WriteProfileString( SubSectName, pb, 0 );
        d = ps.cx - pObj->GetBoundBox().right;
        if( d < 0 )
          pObj->MoveOn( CSize( d, 0 ));
        d = pObj->GetBoundBox().left;
        if( d < 0 )
          pObj->MoveOn( CSize( -d, 0 ));
        d = ps.cy - pObj->GetBoundBox().bottom;
        if( d < 0 )
          pObj->MoveOn( CSize( 0, d ));
        d = pObj->GetBoundBox().top;
        if( d < 0 )
          pObj->MoveOn( CSize( 0, -d ));
      }
    }
  }

  SaveToStore( lst, 1, 0 );
  while( !lst.IsEmpty())
    delete lst.RemoveHead();
}
//=====================================================================
void
CMapView::OnSaveFullScreen()
{
  if( OpenClipboard())
  {
    CViewDC dc( this );
    CMemDC memDC1( &dc, 1 );
    CMemDC memDC2( &dc, 1 );
    CBitmap bmp;
    bmp.CreateCompatibleBitmap( &dc, m_BitmapSize.cx, m_BitmapSize.cy );
    memDC1.SelectObject( &m_Bitmap );
    memDC2.SelectObject( &bmp );
    memDC2.BitBlt( 0, 0, m_BitmapSize.cx, m_BitmapSize.cy, &memDC1, 0, 0, SRCCOPY );
    EmptyClipboard();
    SetClipboardData( CF_BITMAP, HBITMAP( bmp ));
    CloseClipboard();
  }
}
//=====================================================================
void
CMapView::OnLoadTrackPointsCmd( UINT nID )
{
  #define TRACK_CLASS 98
  #define POINT_CLASS 77
  CFileDialog* fdlg = new CFileDialog( 1, "txt", 0, OFN_FILEMUSTEXIST, "All files (*.*)|*.*||", this );

  int r = DoModalDlg( fdlg, CE_PAR_DLG );

  if( r != IDOK )
  {
    delete fdlg;
    return;
  }

  ifstream data;
  data.open( fdlg->GetPathName(), ios::in, SH_DENYWR );
  delete fdlg;

  CArray <Polar,Polar&> PolPoints;

  while( !data.eof())
  {
    double lat, lon;
    data >> lat >> lon;
    if( lat == 0 || lon == 0 )
      break;
    if( fabs( lat ) >= 90. || fabs( lon ) >= 180. )
      break;
    Polar pr( lat, 0, 0, lon, 0, 0 );
    PolPoints.Add( pr );
  }
  data.close();

  int n = PolPoints.GetSize();
  if( !n )
  {
    ErrBox( "Координаты отсутствуют!" );
    return;
  }

  ksi_obj ksi_pts = ksi_nil;
  CSize docSize( GetDocument()->GetSize());
  int i;
  for( i = 0 ; i < n ; ++i )
  {
    CPoint pt;
    if( !MerkatorData.Pol2Prj( PolPoints[i], pt ))
    {
      CPoint t( Pol2Dec( PolPoints[i] ));
      pt.x = t.x;
      pt.y = t.y;
    }

    if( pt.x == -1 && pt.y == -1 )
    {
      ErrBox( "Недостаточно реперов!" );
      return;
    }
    if( pt.x < 0 || pt.y < 0 || pt.x >= docSize.cx || pt.y >= docSize.cy )
    {
      ErrBox( "Координаты вне проекта!" );
      return;
    }

    ksi_pts = ksi_cons( KsiPoint( CPoint( pt.x, pt.y )), ksi_pts );
  }

  CIdent lastId;
  if( nID == IDM_LOAD_TRACK_CMD )
  {
    ksi_obj ksi_env = ksi_nil;
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "apl" ), ksi_reverse( ksi_pts ));
    ksi_obj val =  KsiCall( "create-object", KSI_MK_SINT( TRACK_CLASS ), ksi_zero, ksi_env, 0 );
    CDrawObject* pObj = new CDrawObject( val, CIdent( TRACK_CLASS, 0, 0, GetUniId()), GetConductor()->GetNode());
    AddObject( pObj, 1 );
    lastId = pObj->Ident();
  }
  else
  while( KSI_PLIST_P( ksi_pts ))
  {
    ksi_obj ksi_pt = KSI_CAR( ksi_pts );
    ksi_pts = KSI_CDR( ksi_pts );
    ksi_obj ksi_env = ksi_nil;
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "x0" ), KSI_CAR( ksi_pt ));
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "y0" ), KSI_CDR( ksi_pt ));
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "tx" ), ksi_add( KSI_CAR( ksi_pt ), KSI_MK_SINT( 100 )));
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "ty" ), KSI_CDR( ksi_pt ));
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "ta" ), ksi_zero );
    Polar pl( PolPoints[--i] );
    CString t;
    t.Format( "(%1.6f\xb0,%1.6f\xb0)", pl.lat*180/Pi, pl.lon*180/Pi );
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "number" ), ksi_str02string( t ));
    ksi_obj val =  KsiCall( "create-object", KSI_MK_SINT( POINT_CLASS  ), ksi_zero, ksi_env, 0 );
    CDrawObject* pObj = new CDrawObject( val, CIdent( POINT_CLASS, 0, 0, GetUniId()), GetConductor()->GetNode());
    AddObject( pObj, 1 );
    lastId = pObj->Ident();
  }
  FindObject( lastId, 1, 1 );
}
//=====================================================================
//=====================================================================
static UINT
Name2Num( char const* name )
{
  UINT ss = 0;
  int ns = -4;
  while( *name )
  {
    ss = ( ss << 2 ) + ( *name++ - '0' );
    ns += 1;
  }
  return ( ss << 4 ) | ns;
}
//=====================================================================
static void
Name2PathName( char const* name, char* path, int l )
{
  int const d = 4;
  for( int len = strlen( name ) ; len > d ; len -= d )
  {
    strncpy_s( path, l, name, d );
    l -= d;
    path += d;
    name += d;
    *path++ = '\\';
    l -= 1;
  }
  strcpy_s( path, l, name );
}
//=====================================================================
int
CMapView::TestSubRect( CGraphData* pData, int lvl, int tlvl, CRect& tile, CRect& rect, CString const& path, CString name )
{
  CRect R( tile );
  int D = int( Pi * MerkatorData.GetRmj());
  int X = D * 2;
  R.OffsetRect( -D, -D );
  if( R.left < 0 )
    R.OffsetRect( X, 0 );

  CPoint tp( MerkatorData.GetOffset());
  CSize s( tp.x, tp.y );
  R.OffsetRect( s );
  if( tlvl > 1 && !Intersect( &R, &rect ))
    return 1;

  if( lvl > tlvl )
  {
    int dx = tile.Width()  / 2, mx = tile.left + dx;
    int dy = tile.Height() / 2, my = tile.top  + dy;
    CRect t0( tile.left, my, mx, tile.bottom );
    CRect t1( tile.left, tile.top, mx, my );
    CRect t2( mx, tile.top, tile.right, my );
    CRect t3( mx, my, tile.right, tile.bottom );
    if( TestSubRect( pData, lvl, tlvl + 1, t0, rect, path, name + "0" ) &
        TestSubRect( pData, lvl, tlvl + 1, t1, rect, path, name + "1" ) &
        TestSubRect( pData, lvl, tlvl + 1, t2, rect, path, name + "2" ) &
        TestSubRect( pData, lvl, tlvl + 1, t3, rect, path, name + "3" ))
      return 1;
  }

  int t = SUBSTAT_CLASS + 1 + tlvl - MIN_TILE_LEVEL;
  CIdent id( t, 0, t, Name2Num( name ));
  if( pData->GetObjectPtr( id ))
    return 1;

  CString fullName( name + ".jpg" );

  if( _access( path + fullName, 4 ) < 0 )
  {
    char pnBuf[1024];
    Name2PathName( name, pnBuf, sizeof pnBuf );
    fullName = pnBuf;
    fullName += ".jpg";
    CString pathName = path + fullName;
    if( _access( pathName, 4 ) < 0 )
      return 0;
  }

  // Creating the object
  CStrokItem si;
  CFontItem fi;
  CRectangle* prp = new CRectangle( CRectPrim( R ), si );
  CText* ptp = new CText( 0, 0, 0, fi, fullName, fullName.GetLength(), 0, 0, 0 );
  prp->SetFlags( SF_ANY_VISIBLE );
  CDrawContainer* pCont = new CDrawContainer;
  pCont->AddTail( prp );
  pCont->AddTail( ptp );
  CDrawObject* pObj = new CDrawObject( id, 0, 0, 0, pCont );
  pObj->CalcBoundBox();
  pData->AddObject( pObj );
  return 1;
}
//=====================================================================
void
CMapView::AddLoadStaticSublayer( CGraphData* pData, int sc, CRect rect )
{
  if( rect.IsRectEmpty())
    return;

  CString path( App.GetSubbmpPath( 2 ));
  if( path.IsEmpty())
    return;

  int lv = max( MIN_TILE_LEVEL, MerkatorData.Scale2Level( sc ));

  int D = int( Pi * MerkatorData.GetRmj());
  int X = D * 2;
  CRect MainTile( 0, 0, X, X );
  TestSubRect( pData, lv, 1, MainTile, rect, path, "0" );
}
//=====================================================================
void
CMapView::OnBackupProject()
{
#ifndef _FULL_PROJECT
  if( m_MapStore.Backup() == R_OK )
    InfBox( "Копия проекта успешно создана" );
  else
    ErrBox( "Не удалось создать копию проекта" );
#endif // _FULL_PROJECT
}
//=====================================================================
