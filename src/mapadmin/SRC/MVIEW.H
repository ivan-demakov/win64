#ifndef __MVIEW_H__
#define __MVIEW_H__
//=====================================================================
#include <gdiplus.h>
#include "appmode.h"
#include "tgstream.h"
#ifndef _FULL_PROJECT
  #include "convdlg.h"
  #include "abstrdlg.h"
  #include "shabdlg.h"
  #include "mapupdtr.h"
#else
  #include "mapstore.h"
#endif
#include "graphdoc.h"
#include "cspot.h"
#include "cbox.h"
#include "tpdef.h"
#include "drawobj.h"
#include "conduct.h"
#include "assoc.h"
#include "undo.h"
#include "ids.h"
#include "dlg0.h"
#include "ksiutil.h"
#include "subldlg.h"
#include "objdef.h"
#include "fulink.h"
#include "util.h"
//=====================================================================
int const REPER = 97;
//=====================================================================
struct PrintPageInfo
{
  int    row;
  int    col;
  CPoint portOrg;
  CRect  area;
  int    crmt[4];
};
//=====================================================================
class  CIdent;
class  CMapDoc;
class  CPrim;
class  CAppMode;
//=====================================================================
enum
{
  UR_NOMODE    = 0,
  UR_BLDMODE   = 1,
  UR_DELETE    = 2,
  UR_MOVEMODE  = 3,
  UR_SPINMODE  = 4,
  UR_SPINLEFT  = 5,
  UR_SPINRIGHT = 6,
  UR_SUBLAYER  = 7
};
//=====================================================================
enum
{
  SQ_SETNODE = 0x01,
  SQ_INSIDE  = 0x02,
  SQ_ONSIDE  = 0x04,
  SQ_DRAWRGN = 0x08,
  SQ_SILENT  = 0x10
};
//=====================================================================
enum
{
  IF_NOTHING  = 0,
  IF_INCLUDE  = 1,
  IF_CROSS    = 2,

  IF_INCLUDE01 = 1,
  IF_INCLUDE10 = 2,
  IF_CROSS01   = 3,
  IF_CROSS10   = 3,
};
//=====================================================================
enum UpdateError
{
  UE_NOERROR,
  UE_OBJECT,
  UE_PARAM,
  UE_BUILD,
  UE_CONVERT,
  UE_CONTEXTSIZE,
  UE_SERVER
};
//=====================================================================
enum SelectFilter
{
  FI_CHANGED = 1,
  FI_UNCHANGED = 2,
  FI_ALL = 3
};
//=====================================================================
class CObjLinkNode
{
public:
  CObjLinkNode( CDrawObject* pObj, CDrawObject* pHostObj );
	CDrawObject* GetNodeObject() { return m_pObj; }
  void EditBy( int CoeditStep, int bHR, CObjLinkNode* pHostNode, ksi_obj ksi_dc );
  static void Add( CDrawObject* pObj, CDrawObject* pHostObj );
  static void RemoveAll();
  static void Rebuild();
  static void AddToList( CObjList& tl, SelectFilter mask );
  static int  HasTemp();
  static int  HasOutNode( int bDel );

private:
  void* operator new( size_t sz ) { return ksi_malloc_eternal( sz ); }
  void* operator new( size_t sz, LPCSTR, int ) { return ksi_malloc_eternal( sz ); }
  void operator delete( void* p ) { ksi_free( p ); }
  void operator delete( void* p, LPCSTR, int ) { ksi_free( p ); }

private:
  CDrawObject*  m_pObj;
  int           m_bInEdition;
  int           m_Changed;
  ksi_obj       m_KsiEnv0;
  ksi_obj       m_KsiEnv1;
  CDrawObject*  m_HostpObj0;
  CDrawObject*  m_HostpObj1;
  CObjLinkNode* m_pNextNode;
public:
  static CObjLinkNode* m_pRoot;
};
//=====================================================================
typedef CList< CRect, CRect&> CBoxList;
//=====================================================================
class CMapView : public CScrollView
{
  DECLARE_DYNCREATE( CMapView )

  friend class CMagnitMode;
  friend class CGotoDialog;
  friend ksi_obj ksi_get_all_of_class( ksi_obj ksi_type, ksi_obj ksi_class_list );
  friend class CGraphData;
  friend class CConvDlg;
  friend class CMapDoc;
  friend class CUndo;
  friend class CObjActionMode;
  friend class CMeasureMode;
  friend class CShortTraceMode;
  friend class CConnectMode;
  friend class CBuildMode;
  friend class CDragMode;
  friend class CGridMode;
  friend class CCoordGridMode;
  friend class CMoveMode;
  friend class CSpinMode;
  friend class CPrintMapMode;
  friend class CSelectMode;
  friend class CChangeSelectMode;
  friend class CGetSelectMode;
  friend class CContextMode;
  friend class CSelectTraceMode;
  friend class CObjActionBase;
  friend class CStreetPlanMode;
  friend class CSetReisMode;
  friend class CScaleBase;
  friend class CScaleMode;
  friend class CAutoScrollMode;
  friend class CRestoreMode;
  friend class CUndoSelectItem;
  friend class CUndoAddItem;
  friend class CUndoChangeItem;
  friend class CUndoMoveItem;
  friend class CUndoCopyItem;
  friend class CUndoScaleItem;
  friend class COverviewDialog;
  friend class CShowLayerDialog;
  friend class CAbstractDialog;
  friend class CSublDlg;
  friend class CChangeClassMode;
  friend class CObjSelectDialog;
  friend class CConvCoordDlg;
  friend class CConwThread;

public:
  CMapView();
  ~CMapView();

	void RemoveExtObjects();
  void ResetCrtMsg() { m_bCrtMsg = 0; }
  void SendCtrMsg();
  void FlushMap();
  void SetTimeClock( CDrawObject* pObj );
  void SetPosition( CPoint p );
  void DrawToPoint( int bToLine );
  void SetKsiHiResolution() { SetKsiHiResolutionX( IsHiResolution()); }
  void HideObject( CIdent const& id, int bDelete = 0 );
  void ShowSelectedNum();
  void ShowAngle();
  void ResumeObject( CDrawObject* pObj, int bDeleted, char* pBuf );
  void ShowBox( CRect box );
  void ShowObjectBox( CDrawObject* pObj );
  void ShowSelected( int bShow, int bSave = 0, int bDeclare = 0 );
  void AddSelectRect( CRect const& rect );
  void SubSelectRect( CRect const& rect );
  void MoveSelectOn( CSize off );
  int  SpinSelectOn( CSize center, int angle );
  void SetScale( int scale, CPoint center = CPoint( -1, -1 ), int bMakeUndo = 0 );
  void PrintMap( const CRect &fragment, CPoint cntr, int bPreview );
  void OnFilePrint();
  void InitSublayer();
  void OnSaveFullScreen();
  int  AddMode( CAppMode* pMode );
  int  RemoveMode( CAppMode* pMode );
  int  RemoveMode( UINT modeId );
  void DelAllModes();
  void DrawModes( CDC* pDC );
  void DrawSelect( CDC* pDC, CRect* pRect = 0 );
  void DrawMode( CAppMode* pMode );
  void DrawMode( CAppMode* pMode, UINT nFMask, CDC* pDC );
  void OnPrepareDC( CDC* pDC, CPrintInfo* pInfo = 0 );
  void SetCrossAngle( int a ) { m_CrossAngle = a; }
  void BeginGlobalSearch( CSpot const &spot ) { m_ClipData.BeginGlobalSearch( spot ); }
  CAppMode* UpdateMode( CAppMode* pMode, int rc, UINT nFMask, CDC* pDC );
  CAppMode* GetMode( UINT nID1, UINT nID2 = 0 );
  CAppMode* GetMode( UINT* pId );
  CAppMode* GetActiveMode( UINT* pId );
  CAppMode* GetActiveMode( UINT id );
  CBuildMode* GetBuilding( int bMess = 1 );
  int CreateLineObject();
	int TestPointObject( CPoint p ) { return m_ClipData.TestPointObject( p ); }
  int CreatePointObject();
  int  TestCutIn( CDrawObject* pIns, CDrawObject*& pObj0, CObjList* pLst, int bMsg );
  void TestCutIn( CDrawObject* pIns );
  void MakeCutIn( CDrawObject* pIns, CDrawObject* pObj0, CObjList& lst );
  int AddLegend( CLegend* pLegend ) { return m_LegendCollection.AddLegend( pLegend ); }
  CLegend* GetLegend( int ndx ) { return m_LegendCollection.GetLegend( ndx ); }
  int AddLegendItem( int nLegend, COLORREF Color, LPCSTR Prefix, LPCSTR Postfix );
  CDrawObject* NextGlobalSearch() { return m_ClipData.NextGlobalSearch(); }
  CDrawObject* GetDetectedObject( int bPreSel = 1, int bStrict = 0 );

  int  UpdateTemporaryObject( int ObjCls, int TabID, int ObjID );
  int  GetObjMifCoord( CTabIdent id, CString& str );

  int  SublDlgIsActive() { return m_pSublDlg && m_pSublDlg->IsActive(); }
  int  StartInit();
  int  TestAllObjects( int bFull = 0 );
  void SortShowSelect();
  void CopySelected();
  int  TestSelected();
	int  OffsetObject( CTabIdent id, CSize off );
  int  RemoveObject( CTabIdent id );
  int  RemoveObject( CDrawObject* pObj );
  int  RemoveTempObj( int nCode = 0 );
  int  GetUniId();
  int  GetCrossAngle() { return m_CrossAngle; }
  int  GetModeAngle()  { return GetMode( IDM_SETREISSHINA_MODE ) ||
                                GetMode( IDM_REISSHINA_MODE ) ? m_CrossAngle : 0; }
  double GetScaleFactor() { return m_ScaleFactor; }
  double GetOldScaleFactor() { return m_OldScaleFactor; }
  int  GetAltScale()   { return m_AltScale; }
  int  GetOldScale()   { return m_OldScale; }
  int  GetTrueScale();
  int  IsHiResolution();
  int  GetPrnScale();
  int  GetDsbl( int nType );
	int  IsVisibleInScale( int nType, int nScale );
	int  IsVisibleInScale( int nType ) { return IsVisibleInScale( nType, GetTrueScale()); }
	int  IsVisibleInScale( CDrawObject* pObj ) { return IsVisibleInScale( pObj->Class()); }

  void Clear( int bClose );
  void         ClearSelection( int bDelete = 0, int bMakeUndo = 1 );
  CPoint       GetScrollPosition(){ return m_Clip.TopLeft(); }
  CPoint       GetCenter() { return m_Clip.CenterPoint(); }
  CSize        GetClipSize() { return m_Clip.Size(); }
  CPoint       GetOldCenter() { return m_OldClip.CenterPoint(); }
  CDrawObject* AddObject( CDrawObject* pObj, int bNew );
  CLayer*      GetLayer( int num ) { return m_ClipData.GetLayerByClass( num ); }
  CDrawObject* GetObjectPtr( CIdent const& id ) { return m_ClipData.GetObjectPtr( id ); }
  CDrawObject* GetObjectPtr( CTabIdent id ) { return m_ClipData.GetObjectPtr( id ); }
  CDrawObject* GetSelectObject( int bMessage );
  CDrawObject* ExtractMapObj( CTabIdent id );
  CDrawObject* SelectObject( CIdent const& id, int flags, int bShow = 1 );
  CDrawObject* SelectObject( CDrawObject* pObj, int flags, int bShow = 1 );
  CDrawObject* TranslateObjectFromMap( char** ppSrc );
  CDrawContainer* TranslateFromMap( void** ppSrc );

  CMapDoc*      GetDocument()       { return (CMapDoc*)CScrollView::GetDocument(); }
  CPoint const& GetPosition()       { return m_Position; }
  CSize  const& GetWindowSize()     { return m_WindowSize; }
  CSize  const& GetViewportSize()   { return m_ViewportSize; }
  CSpot         GetSpot( CPoint const& p, int r = -1 );
  CSpot         GetSpot()           { return GetSpot( m_Position ); }
  CBox          CalcSelectBox();

  int WaitingEdit() { return m_nURMode != UR_NOMODE; }
  void QueryUserRights( int nMode, CAppMode* pMode = 0 );
  void OnUserRightsOk( UINT cmd );
  void OnUserRightsCancel();

 	void OnBackupProject();
	void SetDrh();
  void OnExtFind();
  void OnCheckObject( CTabIdent id );
  void OnExtSetScale();
  void OnExtSpaceQue();
  void OnSetCoordGrid();
  void OnMeasureMode( UINT Id );
  void OnSelectTraceMode( UINT Id );
  void OnTraceClose( UINT Id );
  void OnShortTrace( UINT Id );
  void BeginCoedition( CDrawObject* pHostObj, CBuildMode* pMode );
  void EndCoedition( CDrawObject* pHostObj );
  void BreakeCoedition();
  void ChangeCoedition( ksi_obj ksi_dc );
  int  IsCopyingAllowable();
  int  IsEditionAllowable( int bDel );

	void GoTo( CPoint c, int s );
  void SlowMove( CPoint cn, int s );
  int  FindObject( CTabIdent id, int bClearSelection = 1, int bShowFull = 1 );
  int  FindObjectCoord( CTabIdent id, CString& dst );
  int  TestObjects( CTabIdent id0, CTabIdent id1, int bShow );
  int  TestObjects( CDrawObject* pOb0, CDrawObject* pOb1, int bShow );
  int  TestObjects( CTabIdArray const& tb0, CTabIdArray const& tb1, TGstream& dst );
  int  SelectObjectSpecial( CTabIdent id, int bFlash, COLORREF clr, int bClearSelection = 0, int bShow = 0 );
  int  SelectObjectSpecial( int nTab, int nEnt, int bFlash, COLORREF clr, int bClearSelection = 0, int bShow = 0 )
    { return SelectObjectSpecial( CTabIdent( nTab, nEnt ), bFlash, clr, bClearSelection, bShow ); }
  int  SelectObjectSpecial( CTabIdent id, int ndx, DiagItem* pItem = 0, int bClearSelection = 0, int bShow = 0 );
  int  SelectObjectSpecial( int nTab, int nEnt,  int ndx, DiagItem* pItem = 0, int bClearSelection = 0, int bShow = 0 )
    { return SelectObjectSpecial( CTabIdent( nTab, nEnt ), ndx, pItem, bClearSelection, bShow ); }
  int  GetObjParam( CDrawObject* pObj, CTabIdent id, CStringArray& par );
  int  UpdateSetObjParam( CDrawObject* pObj, int bShow = 1 );
  char const* MakeSpaceQue( SpaceQueParam& QP );
  UpdateError CreateInContext( int bShow = 0 );
  UpdateError SetObjParam( CDrawObject* pObj, int bShow = 1 );
  UpdateError UpdateObject( CTabIdent id, CStringArray& par, int bShow = 0 );
  int GetDistance( CTabIdent tid0, CTabIdent tid1 );
  int GetDistance( CPoint const& pnt, CTabIdent tid1 );
  int GetIncludingObjects( CTabIdent tid, int type, int flags, CTabIdArray& dst );
  int IsObjectInLegalRegion( CDrawObject* pObj, char const* opName = 0, int mess = 0 );
  int GetHostIds( CDrawObject* pObj, CTabIdList* pList );
  int GetTargetsGlobal( CDrawObject* pObj, CObjList* pList )
    { return GetLinkedGlobal( pObj, 1, pList ); }
  int GetHostsGlobal( CDrawObject* pObj, CObjList* pList )
    { return GetLinkedGlobal( pObj, 0, pList ); }
  long GetVersion() { return m_Version; }

  void GetScaleRatio( int t, int* pNum, int* pDen, int prnt = 0 );

  int  GetShapeNum();
  void SetShapeNum( int n );
  double CalcObjScaleFactor( int t, int scale, int scaleOn );
  void ScaleObject( CDrawObject* pObj, int scale );
  void SetValidNodeList( int nNodes, long* pNodes );
  void ReloadAll();
  void OnInitialUpdate();
  void SetVersion( int n ) { m_Version = n; }

  CSize  Scale2WindowSize( int scale );
  int    WindowSize2Scale( CSize ws );
  CSize  LP2DP( CSize const& s );
  CSize  DP2LP( CSize const& s );
  CPoint LP2DP( CPoint const& p );
  CPoint DP2LP( CPoint const& p );
  CRect  LP2DP( CRect const& r );
  CRect  DP2LP( CRect const& r );

  CBaseDataRecPtr GetDataRecHeadPtr() { return m_pDRH; }
  CObjTreeList* GetReperList();

  int  AddFuncLink( CFuncLink& funcLink );
  int  RemFuncLink( CFuncLink& funcLink );
  int  RemFuncLink( CTabIdent id0, CTabIdent id1 ) { return RemFuncLink( CFuncLink( id0, id1 )); }
  void RemoveAllFuncLinks();
  void ShowFuncLinks( int LinkNum );
  void HideFuncLinks();

  static CConductor* GetConductor() { return &m_Conductor; }
  static CPoint Grid2Project( CPoint p );
  static CPoint Project2Grid( CPoint p );
//  static void   Project2Grid( CPoint p, double r[] );

private:
	void OnCloneLines();
  void AddLoadStaticSublayer( CGraphData* pData, int sc, CRect r );
	int TestSubRect( CGraphData* pData, int lvl, int tlvl, CRect& tile, CRect& rect, CString const& path, CString name );
  void TestFLTopscr( CPoint* pLoc );
  void RedrawFuncLink( CTabIdent id );
  void DrawFuncLinks( CDC* pDC );
  void OnJumpToSelect( UINT id );
  void OnUpdateJumpToSelect( CCmdUI* pCmdUI );
  void SaveReverseDirection( CDrawObject* pObj = 0 );
  void MakeSpaceQue( int bSetNode );
  void OnMakeSpaceQue()   { MakeSpaceQue( 0 ); }
  void OnMakeSetNodeQue() { MakeSpaceQue( 1 ); }
  void OnOverview( UINT );
  void OnUpdateOverview( CCmdUI* pCmdUI );
  void OnUpdateNoEditCommand( CCmdUI* pCmdUI );
  void StartBuilding();
  void ClipObjects();
  void SetPageSize( CSize size );
  void PrintBorder( int side, PrintPageInfo* ppi, CRect const& r, CDC* pDC );
  void OnScaleInc();
  void OnScaleDec();
  void OnSize( UINT nType, int cx, int cy );
  void OnDrawGrid();
  void OnUpdateDrawGrid( CCmdUI* pCmdUI );
  void OnShowBox();
  void OnUpdateShowBox( CCmdUI* pCmdUI );
  void OnGridStep();
  void OnDistance( UINT id );
  void OnAngleStep();
  void OnUnselect();
  void OnPrintMap( UINT nID );
  void OnUpdatePrintCommand( CCmdUI* pCmdUI );
  void OnSysChar( UINT key_code, UINT count, UINT flags );
  void OnDraw( CDC* DC );
  void OnBeginPrinting( CDC* pDC, CPrintInfo* pInfo );
  void OnEndPrinting( CDC* pDC, CPrintInfo* pInfo );
  void CalcPrintPageNum();
  void OnPrint( CDC* pDC, CPrintInfo* pInfo );
  void OnMouseEvent( UINT flags, CPoint loc, UINT nFMask, int (CAppMode::*f)( UINT&, CPoint& ));
  void OnFixMouse( CPoint loc, int bRB );
  void OnLButtonDown( UINT flags, CPoint loc );
  void OnLButtonUp( UINT flags, CPoint loc );
  void OnLButtonDblClk( UINT flags, CPoint loc );
  void OnRButtonDown( UINT flags, CPoint loc );
  void OnRButtonUp( UINT flags, CPoint loc );
  void OnRButtonDblClk( UINT flags, CPoint loc );
  void OnMButtonDown( UINT flags, CPoint loc );
  void OnMButtonUp( UINT flags, CPoint loc );
  void OnMButtonDblClk( UINT flags, CPoint loc );
  void OnMouseMove( UINT flags, CPoint loc );
  void OnChar( UINT key_code, UINT count, UINT flags );
  void OnKeyDown( UINT key_code, UINT count, UINT flags );
  void OnKeyUp( UINT key_code, UINT count, UINT flags );
  void OnUpdateObjCommandUI( CCmdUI* pCmdUI );
  void OnUpdateAppCommandUI( CCmdUI* pCmdUI );
  void OnTimer( UINT nIDEvent );
  void OnReisshina();
  void OnSetReisshina();
  void OnCross();
  void OnCopyMode();
  void OnUpdateCopyMode( CCmdUI* pCmdUI );
  void OnHandMode();
  void OnGoTo();
	void OnMagnitMode( UINT cmd );
  void OnIncScaleMode();
  void OnDecScaleMode();
  void OnGetSelectMode();
  void OnAddSelectMode();
  void OnSubSelectMode();
  void OnStreetPlanMode();
  void OnScroll( UINT nBar, UINT nSBCode );
  void OnVScroll( UINT nSBCode, UINT, CScrollBar* );
  void OnHScroll( UINT nSBCode, UINT, CScrollBar* );
  void OnGrid();
  void OnUpdateGrid( CCmdUI* pCmdUI );
  void OnCoordGrid();
  void OnUpdateCoordGrid( CCmdUI* pCmdUI );
  void OnConnectMode();
  void OnCutInCmd();
  void OnRemCutInCmd();
  void OnRestoreMode();
#ifdef _FULL_PROJECT
  void OnUpdateRestoreMode( CCmdUI* pCmdUI );
#endif
  void OnObjectActionMode( UINT nID );
  void OnObjCommand( UINT nID );
  void OnObjContextCommand( UINT nID );
  void OnEditor();
  void OnLoader();
  void OnLayerDlg();
  void OnSASDlg();
  void LoadFromStore( CGraphData* pDst, CRect const& r1, CRect const& r2, int limit1, CRect const& r0, int limit0 );
  void LoadFromStore( CGraphData* pDst, CRect const& r, long* pClassArray, int nClass );
  void SaveToStore( CObjList& ObjList, int bNew, int bAddToClip );
  void OnSerialBuildingCmd();
  void OnUpdateSerialBuildingCmd( CCmdUI* pCmdUI );
  void OnTransfocatorCmd();
  void OnUpdateTransfocatorCmd( CCmdUI* pCmdUI );
  void ShowScrollbars( int bDraw );
  void OnScrollbarsCmd();
  void OnUpdateScrollbarsCmd( CCmdUI* pCmdUI );
  void OnToolTipsCmd();
  void OnUpdateToolTipsCmd( CCmdUI* pCmdUI );
  void OnUseGridCmd();
  void OnUpdateUseGridCmd( CCmdUI* pCmdUI );
  void OnShowAsSeletedCmd();
  void OnUpdateShowAsSeletedCmd( CCmdUI* pCmdUI );
  void OnSelectLinkedCmd( UINT cmd );
  void OnUpdateSelectLinkedCmd( CCmdUI* pCmdUI );
  void OnSublayerAdm();
  void OnImprovePolyline();
  void OnTestReper();
  void OnTestAllRepers();
  int  Test1Reper( CDrawObject* pRep, std::ofstream* pFile, int nMode );
  void UnselObj();
  void OnRegionSave();
  void OnChangeClassMode();
  void OnLoadTrackPointsCmd( UINT nID );

#ifndef _FULL_PROJECT
  void OnObjectAdm();
  void OnLineTemplEditor();
  void OnToolbarAdm();
  void OnMakeStatistics( UINT id );
  void OnConvertor();
  void OnFindObject();
  void AttachmentLines( LPCTSTR fName, int* lct, int* wct, int pct, int bct,
                        int ptn, int pen, int btn, int ben );
  void OnLineAttachment();
#endif

  int  CanUndo( int beep );
  int  CanRedo( int beep );
  void OnUndo();
  void OnRedo();
  void OnUpdateUndo( CCmdUI* pCmdUI );
  void OnUpdateRedo( CCmdUI* pCmdUI );
  void KillUndoItem();

#ifndef _FULL_PROJECT
  void OnDebug1();
  void OnDebug2();
  void OnDebug3();
  void OnDebug4();
#endif

  int OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
  int CalcShowScale( CDrawObject* pObj, int bFull  = 0 );

#ifndef _FULL_PROJECT
  int UpdateColorTab( COLORREF MonitorColor,
                      COLORREF PrinterColor,
                      COLORREF PrinterBWColor,
                      int& bClear );
#endif
  int MakeSelectList( CRect const& r, CObjList& list );
  int OnAppCommand( UINT nID );
  int OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
  int OnPreparePrinting( CPrintInfo* pInfo );
  int GetLinkedGlobal( CDrawObject* pObj, int bTgt, CObjList* pList );
  int LoadMap( CGraphData* pData, char* pSrc, int nObj, int scale, int bFull = 0 );
  CDrawObject* StartBlinking( CDrawObject* pObj );
  void MakeBlinking();

#ifdef _FULL_PROJECT
private:
  void OnFindCommand( UINT cmd );
  void OnDocCommand( UINT cmd );
  void OnSpecialCommand( UINT cmd );
#endif

private:
  int               m_bObjToolTips;
  int               m_bNoLinkEdit;
  int               m_bDblClick;
  int               m_bCrtMsg;
  int               m_bPreview;
  int               m_bFinding;
  int               m_bScaleOnSize;
  int               m_bSpaceSelection;
  int               m_bRedraw;
  int               m_bSublayerChanged;
  int               m_bSelectLinked;
  int               m_bSelectAllLinked;
  int               m_bSerialBuilding;
  int               m_bMultiSelection;
  int               m_CrossAngle;
  double            m_ScaleFactor;
  double            m_OldScaleFactor;
  int               m_AltScale;
  int               m_PrnScale;
  int               m_OldScale;
  int               m_CoeditStep;
  UINT              m_ObjectClass;
  CSize             m_BitmapSize;
  CBitmap           m_Bitmap;
  CBitmap           m_SubBmp;

  int               m_BlinkStat;

  CRect             m_Clip;
  CRect             m_OldClip;
  CGraphData        m_ClipData;
  CObjTreeList      m_ReperList;
	int               m_bReperListCreated;

#ifndef _FULL_PROJECT
  MapUpdater        m_MapStore;
#else
  MapStore          m_MapStore;
#endif
  PrintPageInfo*    m_pPrintPages;
  CSize             m_PrinterPageSize;
  CSize             m_PrintPageSize;
  CSize             m_PrintPageSngl;
  CSize             m_PrintPageFrst;
  CSize             m_PrintPageNext;
  CSize             m_PrintPageLast;
  int               m_PrintPageNumX;
  int               m_PrintPageNumY;
  int               m_PrintPageNum;
  CRect             m_PrintArea;
  CAppMode*         m_pMode;
  CSize             m_WindowSize;
  CSize             m_ViewportSize;
  int               m_bDrawSel;
  CPoint            m_Position;
  CPoint            m_PrintOrg;
  CObjList          m_SelectList;
  CObjList          m_LastEditList;
  CObjList          m_LastHostList;
  int               m_HostClass;
  char              m_LocalLaw[FULL_RANGE];
  char              m_LoadMap[BIT_FULL_RANGE];
  char              m_ShowMap[BIT_FULL_RANGE];
  long              m_Version;
  CUndo             m_Undo;
  CUndoScaleItem*   m_pUndoItem;
  int               m_HiResLimit;
  int               m_UndoLevel;
  COverviewDialog   m_OverviewDialog;
	CGotoDialog*      m_pGotoDialog;
  CObjSelectDialog  m_ObjSelectDialog;
  CSelectTraceMode* m_pSTM;
  int               m_nRgn;
  CPoint**          m_pRgn;
  CObjDef*          m_pObjDef;
  CDrawObject*      m_pCurSelObj;
  CDrawObject*      m_pSuspObj;
  CDrawObject*      m_pDelObj;
  
	CDrawObject*      m_pCutIns;
  CDrawObject*      m_pCutObj0;
  CDrawObject*      m_pCutObj1;
  int               m_bCutIn;
  int               m_bCutOut;

  CBoxList          m_BoxList;
  CBaseDataRecPtr   m_pDRH;
  CLinkArray        m_FuLinkArray;
  int               m_bFuLinksShow;
  CLegendCollection m_LegendCollection;

  int        m_nURMode;
  CAppMode*  m_pURMode;
  CTabIdList m_URIdLst;

  static CConductor m_Conductor;

public:
  int             m_ScaleOnLoading;
  CSublDlg*       m_pSublDlg;
#ifndef _FULL_PROJECT
  CConvDlg        m_ConvDlg;
  CShabDlg        m_ShabDlg;
  CAbstractDialog m_AbstractDlg;
  CFindObjectDlg  m_FindObjectDlg;
  long*           m_pNodes;
#endif
  static long m_DimScaleRatio;
  static long m_Deviation;
  static long m_Sensibility;
  static long m_MaxDelta;
  static long m_bSlow;
  static long m_bScrollBars;
  static int m_bViewInited;
  static int IsViewInited() { return m_bViewInited; }
  static long m_X0;
  static long m_Y0;
  static long m_XStep;
  static long m_YStep;
  static long m_bUseGrid;
	static CLegend const* m_pLg;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CViewDC: public CClientDC
{
public:
  CViewDC( CMapView* pView ):
    CClientDC( pView )
  {
    pView->OnPrepareDC( this );
  }
};
//=====================================================================
extern CMapView* pView;
//=====================================================================
#endif
