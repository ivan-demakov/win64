#include "stdafx.h"

#include "drawobj.h"
#include "gpl.h"
#include "filestr.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "graphdoc.h"
#include "movemode.h"
#include "miscmode.h"
#include "mathutil.h"
#include "msutil.h"
#include "mview.h"
#include "mdoc.h"
#include "util.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "joke.h"
#include "dwin.h"
#include "dmc.h"
#include "dlg0.h"
#include "sltrmode.h"
#include "lawdlg.h"
#include "mercator.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
long SmInUnit = 1;
long MaxScale = 100;
long MinScale = 50000;
//=====================================================================
int CMapView::m_bViewInited = 0;
//=====================================================================
int
CMapView::IsHiResolution()
{
  return GetTrueScale() <= m_HiResLimit;
}
//=====================================================================
int
CMapView::GetUniId()
{
  long id;
  return m_MapStore.GetUniId( &id ) == R_OK ? id : -1;
}
//=====================================================================
int
CMapView::RemoveObject( CTabIdent id )
{
  CDrawObject* pObj = GetObjectPtr( id );
  return pObj ? RemoveObject( pObj ) : m_MapStore.RemoveObject( id ) == R_OK;
}
//=====================================================================
int
CMapView::RemoveObject( CDrawObject* pObj )
{
  CTabIdent id( pObj->TabIdent());
  if( pObj->Class() == REPER )
    delete GetReperList()->RemoveObject( id.Entry());

  using namespace Gdiplus;
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR GdiplusToken;
  GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );
  pObj->RemoveSplitSubl();
  GdiplusShutdown( GdiplusToken );  

  int r = m_MapStore.RemoveObject( id ) == R_OK;
  HideObject( pObj->Ident(), 1 );
  RedrawFuncLink( id );
  return r;
}
//=====================================================================
CDrawObject*
CMapView::AddObject( CDrawObject* pObj, int bNew )
{
  TGstream nss;
  SetTimeClock( pObj );

  if( pObj->Class() == REPER )
    delete GetReperList()->AddObject( new CDrawObject( pObj ));

  dword mg = bNew ? INSMAGIC : REPMAGIC;
  nss.write((char*)&mg, sizeof mg );

  if( pObj->IsObjectTemp() || pObj->TranslateToMap( &nss ) && m_MapStore.AddObject( nss ) == R_OK )
  {
    CDrawObject* pOld = m_ClipData.AddObject( pObj );
    ASSERT( !pOld || !bNew ); 
    if( pOld )
    {
      int f = pOld->SelectFlag();
      SelectObject( pOld, 0 );
      SelectObject( pObj, f );
      if( f && pOld == m_pCurSelObj )
        StartBlinking( pObj );
    }
    ShowObjectBox( pOld );
    ShowObjectBox( pObj );
    RedrawFuncLink( pObj->TabIdent());
    pObj = pOld;
  }
  return pObj;
}
//=====================================================================
void
CMapView::SaveToStore( CObjList& ObjList, int bNew, int bAddToClip )
{
  if( ObjList.IsEmpty())
    return;

  long t, c;
  m_MapStore.GetTimeClock( &t, &c );

  dword mg = bNew ? INSMAGIC : REPMAGIC;
  TGstream nss;
  nss.write((char*)&mg, sizeof mg );
  int nObj = 0;

  for( POSITION pos = ObjList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = ObjList.GetNext(pos);
    if( !pObj->IsObjectTemp())
    {
      pObj->SetTimeClock( t, c );
      pObj->TranslateToMap( &nss );
      m_Conductor.RegObjMifCoord( pObj );
      ++nObj;
    }
  }

  int tl = nss.pcount();
  char tm[1024];
  memcpy(tm, nss.str(), tl );

  if( nObj )
    m_MapStore.AddObject( nss );

  if( bAddToClip )
	{
    for( POSITION pos = ObjList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = ObjList.GetNext( pos );
      ShowObjectBox( pObj );
      RedrawFuncLink( pObj->TabIdent());
//			if( IsVisibleInScale( pObj ))
			{
        ScaleObject( pObj, GetTrueScale());
        CDrawObject* pOld = m_ClipData.AddObject( pObj );
        int bOldTemp = pOld && pOld->IsObjectTemp();
        ASSERT( !pOld || !bNew || bOldTemp ); 
        if( pOld && pOld->IsSelected())
          SelectObject( pOld, 0 );
        ShowObjectBox( pOld );
        if( bOldTemp )
          delete pOld;
			}
    }
	}
}
//=====================================================================
void
CMapView::LoadFromStore( CGraphData* pDst,
                         CRect const& r1, CRect const& r2, int scale1,
                         CRect const& r0, int scale0 )
{
  if( r1.IsRectEmpty() && r2.IsRectEmpty())
    return;

  ReportHint( "Загрузка..." );

  for( ResultCode r = R_REPEAT ; r == R_REPEAT ; )
  {
    int nObj = 0;
    TGstream buf;
    r = m_MapStore.SelectObjects( r1, r2, scale1,r0, scale0, buf, 0, &nObj );
    if( r <= R_REPEAT )
      LoadMap( pDst, buf.str(), nObj, scale1 );
  }
}
//=====================================================================
void
CMapView::LoadFromStore( CGraphData* pDst, CRect const& r0,
                         long* pClassArray, int nClass )
{
  if( r0.IsRectEmpty() || !nClass )
    return;

  for( ResultCode r = R_REPEAT ; r == R_REPEAT ; )
  {
    int nObj;
    TGstream buf;
    r = m_MapStore.SelectByClass( r0, pClassArray, nClass, buf, &nObj );
    if( r <= R_REPEAT && nObj )
      LoadMap( pDst ? pDst : &m_ClipData, buf.str(), nObj, 0, 1 );
  }
}
//=====================================================================
int
CMapView::GetShapeNum()
{
  CMainWin* pWnd = (CMainWin*)AfxGetMainWnd();
  return pWnd->GetShapeNum();
}
//=====================================================================
void
CMapView::SetShapeNum( int n )
{
  CMainWin* pWnd = (CMainWin*)AfxGetMainWnd();
  pWnd->SetShapeNum( n );
}
//=====================================================================
void
CMapView::SetDrh()
{
  m_MapStore.GetVersion( &m_Version );
  static GDataRec_0101 Drh_0101;
  static GDataRec_0103 Drh_0103;
  static GDataRec_0107 Drh_0107;
  static GDataRec_0109 Drh_0109;
  static GDataRec_0111 Drh_0111;
  static CBaseDataRecPtr PtrTab[] =
  {
    &Drh_0101, &Drh_0103, &Drh_0103, &Drh_0107, &Drh_0109, &Drh_0111
  };

  int i = min( max( 0, m_Version - 101 >> 1 ), CELEM( PtrTab ) - 1 );
  m_pDRH = PtrTab[i];
}
//=====================================================================
int
CMapView::StartInit()
{
  m_bViewInited = 0;

  static char const gnr[] = "General";
  CWaitCursor wait;
  CMapDoc* pDoc = GetDocument();

  ReportHint( "Создание слоев..." );
  pDoc->CreateLayers( &m_ClipData );

  memset( CDrawContainer::m_bSASMap, 0, sizeof CDrawContainer::m_bSASMap );

  for( int n = 2 ; --n >= 0 ; )
  {
#ifndef _FULL_PROJECT
    CString sm( App.GetProfileString( "General", n ? "SasMap" : "ShowMap" ));
#else
    CString sm( App.GetProfileString( "General", n ? "SasMap" : "ShowMap", "", 1 ));
#endif
    if( sm.IsEmpty())
      continue;
    char buf[1024];
    char* p = strncpy( buf, LPCSTR( sm ), sizeof buf );
    int* pi = n ? (int*)CDrawContainer::m_bSASMap : (int*)m_ShowMap;
    for( p = strtok( p, " " ) ; p ; p = strtok( 0, " " ))
      sscanf( p, "%x", pi++ );
  }

#ifdef _FULL_PROJECT
  pDoc->GetObjDef()->SetModified( 0 );
#endif

  ((CMainWin*)AfxGetMainWnd())->CreateToolBars();

  ReportHint( "Индексация..." );

  if( m_MapStore.Init( App.GetIniPath()) != R_OK )
    return 0;

  MerkatorData.Init( &m_MapStore );

  ClearHint();
  CSize( ds );

  m_MapStore.GetXSize( &ds.cx );
  m_MapStore.GetYSize( &ds.cy );
  m_MapStore.GetUnitDec( &SmInUnit );
  SmInUnit = max( 1, SmInUnit );
  SetDrh();

  MaxScale = App.GetProfileInt( gnr, "MaxScale",   100 );
  MinScale = App.GetProfileInt( gnr, "MinScale", 50000 );

  memset( m_ShowMap, 0x00, sizeof m_ShowMap );
  memset( m_LoadMap, 0x00, sizeof m_LoadMap );
  for( int t = 0 ; t < FULL_RANGE ; ++t )
  {
    if( m_pObjDef->GetLoad( t ) & 1 )
    {
      SetBit( m_LoadMap, t, 1 );
      SetBit( m_ShowMap, t, 1 );
      m_ClipData.GetLayerByClass( t )->SetVisible( 1 );
    }

    m_LocalLaw[t] = m_pObjDef->GetEdit( t );
  }

#ifdef _FULL_PROJECT
  memset( m_LoadMap, -1, sizeof m_LoadMap );
#else
  m_MapStore.SetLoadMap( m_LoadMap );
#endif

  pDoc->SetSize( ds );
  pDoc->ModifyObjectMenu();

  AddMode( new CBoundingMode( ds.cx, ds.cy ));
  m_HiResLimit = App.GetProfileInt( gnr, "HiResLimit", MaxScale );
  m_UndoLevel = App.GetProfileInt( gnr, "UndoLevel", 0 );

#ifndef _FULL_PROJECT
  CString nl( App.GetProfileString( gnr, "Nodes" ));
  if( !nl.IsEmpty())
  {
    static char dlm[] = ",; ";
    int nNodes = 0;
    char* p = nl.GetBuffer( 1 );
    for( p = strtok( p, dlm ) ; p ; p = strtok( 0, dlm ))
      nNodes += atoi( p ) != 0;

    m_pNodes = new long[nNodes + 1];
    p = nl.GetBuffer( 1 );
    for( int i = 0 ; i < nNodes ; ++i )
    {
      int n = atoi( p );
      if( n )
        m_pNodes[i] = n;
      p += 1 + strlen( p );
    }
    m_MapStore.SetValidNodeList( m_pNodes );
  }
#else
  SetValidNodeList( 0, 0 );
#endif  

  ReportHint( "Загрузка обзора..." );
  m_OverviewDialog.Init( 0 );
  SetShapeNum( 0 );

  int sc = App.GetProfileInt( gnr, "StartScale", 2000 );
  int sX = App.GetProfileInt( gnr, "StartX", 0 );
  int sY = App.GetProfileInt( gnr, "StartY", 0 );
  CPoint cn( sX, sY );

#ifdef _FULL_PROJECT
  int s = min( sc, 2000 );
#else
  int s = sc;
#endif

  s = MerkatorData.LogScale( s, cn );
  SetScale( s, cn, 0 );
  ShowScrollbars( 0 );
  SetScale( s, cn, 0 );

#ifdef _FULL_PROJECT
  // allowed regions loading
  int nTbl = 0, pEnt[1024];

  m_nRgn = -1; //GetRegions( &nTbl, pEnt, CELEM( pEnt ));

  if( m_nRgn > 0 )
  {
    ReportHint( "Загрузка регионов..." );

    CTabIdent Tid[4096];
    for( int i = m_nRgn ; --i >= 0 ; Tid[i] = CTabIdent( nTbl, pEnt[i] ));

    for( ResultCode r = R_REPEAT ; r == R_REPEAT ; )
    {
      TGstream buf;
      r =  m_MapStore.FindObjects( Tid, &m_nRgn, buf );
      LoadMap( &m_ClipData, buf.str(), m_nRgn, 0, 1 );
      char* pSrc = buf.str();
      CObjList lst;

      while( --m_nRgn >= 0  )
      {
        CDrawObject* pRgnObj = TranslateObjectFromMap( &pSrc );
        if( pRgnObj && pRgnObj->GetPolygon( 0 ))
          lst.AddHead( pRgnObj );
      }

      m_nRgn = lst.GetCount();

      if( m_nRgn > 0 )
      {
        typedef CPoint* pPoint;
        m_pRgn = new pPoint[m_nRgn];
        while( !lst.IsEmpty())
        {
          CDrawObject* pRgnObj = lst.RemoveHead();
          int np;
          CPoint* pp = pRgnObj->GetPolygon( &np );
          m_pRgn[i] = new CPoint[np+1];
          memcpy( m_pRgn[i], pp, ( np + 1 ) * sizeof( CPoint ));
          delete pRgnObj;
        }
      }
    }
  }
#else
  m_nRgn = -1;
#endif

  ReportHint( "Анализ библиотеки объектов..." );
  TestAllObjects( App.GetProfileInt( gnr, "TestObjects", 0 ));
  ReportHint( "Анализ структуры объектов..." );
  CDrawObject::InitShapeNdx();
  ClearHint();

  EnableWindow( 1 );
  SetFocus();

  m_bViewInited = 1;

#ifndef _FULL_PROJECT
  InitSublayer();
  m_FindObjectDlg.Init(this);
  m_ConvDlg.Init( this, &m_MapStore );
  m_ConvDlg.Create( IDD_CONVERTOR_DIALOG, this );
  m_ShabDlg.Init();
#endif

  m_pSublDlg = new CSublDlg();

  m_OldClip.SetRectEmpty();
  m_Clip.SetRectEmpty();

  return m_bViewInited;
}
//=====================================================================
void
CMapView::ClipObjects()
{
  if( !m_bViewInited )
  {
    ReportHint( "Нет доступа к серверу..." );
    return;
  }

  CMapDoc* pDoc = GetDocument();
  if( !pDoc || !pDoc->IsInited())
    return;

  m_OverviewDialog.MoveRect();

  CRect r0( 0, 0, 0, 0 ), r1( m_Clip ), r2( 0, 0, 0, 0 );
  
  int nsc = m_AltScale / m_ScaleFactor + .5;
  int osc = m_OldScale / m_OldScaleFactor + .5;

//	if( nsc == osc )
	  r0.IntersectRect( &m_Clip, &m_OldClip );

  if( r0.IsRectEmpty())
  {
    m_ClipData.ClipObjects( 0, 1 );
    LoadFromStore( &m_ClipData, r1, r2, nsc, r0, osc );
  }
  else
  {
    m_ClipData.ClipObjects( &r0, 1 );
    if( m_Version <= 103 )
      r0.SetRectEmpty();

    if( nsc != osc )
    {
      LoadFromStore( &m_ClipData, r1, r2, nsc, r0, osc );
      for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
      {
        CDrawObject* pObj = m_SelectList.GetNext( pos );
        if( pObj->IsSpecSelected())
          pObj->CalcBoundBox();
      }
    }
    else
    if( m_Clip.Size() == m_OldClip.Size())
    {
      r2 = r1;
      CSize delta = m_Clip.TopLeft() - m_OldClip.TopLeft();
      if( delta.cx > 0 )
        r1.right = r2.left = r0.right;
      else
        r1.left = r2.right = r0.left;
      if( delta.cy > 0 )
        r1.top = r0.bottom;
      else
        r1.bottom = r0.top;
      LoadFromStore( &m_ClipData, r1, r2, nsc, r0, osc );
    }
    else
    {
      if( m_Clip.left < m_OldClip.left )
        r1.SetRect( m_Clip.left, m_Clip.top, m_OldClip.left, m_OldClip.bottom );
      else
        r1.SetRectEmpty();
      if( m_Clip.top < m_OldClip.top )
        r2.SetRect( m_OldClip.left, m_Clip.top, m_Clip.right, m_OldClip.top );
      else
        r2.SetRectEmpty();
      if( !r1.IsRectEmpty() || !r2.IsRectEmpty())
        LoadFromStore( &m_ClipData, r1, r2, nsc, r0, osc );

      if( m_Clip.right > m_OldClip.right )
        r1.SetRect( m_OldClip.right, m_OldClip.top, m_Clip.right, m_Clip.bottom );
      else
        r1.SetRectEmpty();
      if( m_Clip.bottom > m_OldClip.bottom )
        r2.SetRect( m_Clip.left, m_OldClip.bottom, m_OldClip.right, m_Clip.bottom );
      else
        r2.SetRectEmpty();
      if( !r1.IsRectEmpty() || !r2.IsRectEmpty())
        LoadFromStore( &m_ClipData, r1, r2, nsc, r0, osc );
    }
  }
  AddLoadStaticSublayer( &m_ClipData, nsc, r1 );
  AddLoadStaticSublayer( &m_ClipData, nsc, r2 );
}
//=====================================================================
double
CMapView::CalcObjScaleFactor( int t, int scale, int scaleOn )
{
  if( t > SUBLAYER_CLASS )
    return 1.;

  double cf = scaleOn ? m_ScaleFactor : 1. / m_ScaleFactor;
  
  if( m_ScaleOnLoading )
  {
    int minScale = m_pObjDef->GetMinScale( t );
    int maxScale = m_pObjDef->GetMaxScale( t );
    int sc = scale;
		if( minScale > 0 && scale < minScale )
			sc = minScale;
		if( maxScale > 0 && scale > maxScale )
      sc = maxScale;
    if( scale != sc )
    {
      cf *= scaleOn ? scale : sc;
      cf /= scaleOn ? sc : scale;
    }
  }
  return cf;
}
//=====================================================================
void
CMapView::ScaleObject( CDrawObject* pObj, int scale )
{
  double cf = CalcObjScaleFactor( pObj->Class(), scale, 1 );
  
  CPoint p0( -1, -1 );
  CPoint *p = pObj->GetPoint();
  pObj->ScaleOn( cf, p ? *p : p0 );
}
//=====================================================================
CDrawContainer*
CMapView::TranslateFromMap( void** ppSrc )
{
  enum
  {
    STROKED        = 0x01,
    FILLED         = 0x02,
    ROUND          = 0x04,
    SMART          = 0x08,
    CHORD          = 0x10,
    OUTLINED       = STROKED | FILLED,
    SMART_OUTLINED = OUTLINED | SMART,
    ROUND_STROKED  = ROUND | STROKED,
    ROUND_FILLED   = ROUND | FILLED,
    ROUND_OUTLINED = ROUND | OUTLINED
  };

  CDrawContainer* pCont = 0;
  PrimRecord* pPr = (PrimRecord*)*ppSrc;

  if(( pPr->type & PF_TYPE_MASK ) == PF_CONTAINER )
  {
    int n = pPr->param.container.length;
    *ppSrc = pPr->param.container.prims;
    pCont = new CDrawContainer();
    while( --n >= 0 )
    {
      pPr = (PrimRecord*)*ppSrc;
      int typeKey = pPr->type & PF_TYPE_MASK;
      Drawable* pObjPar  = (Drawable*)&pPr->param;
      PenParam*   pPen   = 0;
      BrushParam* pBrush = 0;
      int subType = (( pPr->type & PF_STROKED ) ? STROKED : 0 ) |
                    (( pPr->type & PF_FILLED  ) ? FILLED  : 0 ) |
                    (( pPr->type & PF_CHORD   ) ? CHORD   : 0 ) |
                    (( pPr->type & PF_SMART   ) ? SMART   : 0 ) |
                    (( pPr->type & PF_ROUND   ) ? ROUND   : 0 );

      int bVisible = pPr->type & PF_VISIBLE;
      CStrokItem SI;
      CFillItem  FI;

      if( bVisible )
        switch( subType & OUTLINED )
        {
          case STROKED :
            pPen    = &pPr->param.stroked.pen;
            pObjPar = &pPr->param.stroked.primParam;
            break;
          case FILLED :
            pBrush  = &pPr->param.filled.brush;
            pObjPar = &pPr->param.filled.primParam;
            break;
          case OUTLINED :
            pPen    = &pPr->param.outlined.pen;
            pBrush  = &pPr->param.outlined.brush;
            pObjPar = &pPr->param.outlined.primParam;
            break;
        }

      if( pPen )
        SI.Create( pPen->color, pPen->style, pPen->style > PS_ALTERNATE ? 0 : pPen->width );

      if( pBrush )
        FI.Create( pBrush->fore, pBrush->back, pBrush->style, pBrush->hatch );

      WORD flags = 0;
      if( pPr->type & PF_LVISIBLE )
        flags |= SF_LOW_VISIBLE;
      if( pPr->type & PF_HVISIBLE )
        flags |= SF_HIG_VISIBLE;
      if( pPr->type & PF_SVISIBLE )
        flags |= SF_SEL_VISIBLE;

      CVisiblePrim* pPrim = 0;

      switch( typeKey )
      {
        case PF_CONTAINER:
          if( pPrim = TranslateFromMap( ppSrc ))
            break;

        default:
          delete pCont;
          return 0;

        case PF_POINT:
          pPrim = new CPointPrim( pPr->param.pnt.x, pPr->param.pnt.y );
          *ppSrc = &pPr->param.pnt + 1;
          break;

        case PF_ICON:
          pPrim = new CIcon( pPr->param.icon.x, pPr->param.icon.y, pPr->param.icon.id );
          *ppSrc  = &pPr->param.icon + 1;
          break;

        case PF_TEXT:
        {
          StringParam* pSP = bVisible ? &pPr->param.text.param.full.string :
                                        &pPr->param.text.param.string;
          CFontItem FI;
          int color = 0;

          if( bVisible )
            if( m_Version < 107 )
            {
              FontParam* pFp = &pPr->param.text.param.full.FontParams.font;
              FI.Create( pFp->number, pFp->height, pFp->weight, pFp->style );
              color = pFp->color;
            }
            else
            {
              FontParam1* pFp = &pPr->param.text.param.full.FontParams.font1;
              FI.Create( pFp->number, pFp->height, pFp->weight, pFp->style );
              color = pFp->color;
            }

          pPrim = new CText( pPr->param.text.org.x, pPr->param.text.org.y,
                             color, FI, pSP->string, pSP->length,
                             pSP->offset.x, pSP->offset.y, pSP->angle );
          *ppSrc = pSP->string + pSP->length;
          break;
        }

        case PF_TEXTBYLINE:
        {
          CFontItem FI;
          int color = 0;
          int length = pPr->param.textbyline.length;
          TxtItem* pItem = pPr->param.textbyline.items;

          if( bVisible )
            if( m_Version < 107 )
            {
              FontParam* pFp = &pPr->param.textbyline.FontParams.font;
              FI.Create( pFp->number, pFp->height, pFp->weight, pFp->style );
              color = pFp->color;
            }
            else
            {
              FontParam1* pFp = &pPr->param.textbyline.FontParams.font1;
              FI.Create( pFp->number, pFp->height, pFp->weight, pFp->style );
              color = pFp->color;
            }

          pPrim = new CTextByLine( color, FI, length, pItem );
          *ppSrc = pItem + length;
          break;
        }

        case PF_FOOTNOTE:
        case PF_POLYPOINT:
        {
          int nPoints = pObjPar->polypoint.length;
          CPoint* pPoints = (CPoint*)pObjPar->polypoint.points;
          if( !( m_Version & 1 ))
            for( int i = 0 ; ++i < nPoints ; pPoints[i] += pPoints[i-1] );
          if( typeKey == PF_FOOTNOTE )
            pPrim = new CFootnote( nPoints, SI, pPoints );
          else
          switch( subType )
          {
            case SMART_OUTLINED : pPrim = new CSmartPolygon( FI, SI, nPoints, pPoints ); break;
            case OUTLINED       : pPrim = new COutlinedPolygon( FI, SI, nPoints, pPoints ); break;
            case STROKED        : pPrim = new CPolyline( nPoints, SI, pPoints, pPr->type & PF_BOUND ); break;
            case FILLED         : pPrim = new CPolygon( FI, nPoints, pPoints ); break;
          }
          *ppSrc = pPoints + nPoints;
          break;
        }

        case PF_MULTILINE:
        {
          int nPoints = pObjPar->polypoint.length;
          CPoint* pPoints = (CPoint*)pObjPar->polypoint.points;
          if( !( m_Version & 1 ))
            for( int i = 0 ; ++i < nPoints ; pPoints[i] += pPoints[i-1], ++i );
          pPrim = new CLines( nPoints, SI, pPoints );
          *ppSrc = pPoints + nPoints;
          break;
        }

        case PF_ELLIPSE :
        {
          RECT* pr = &pObjPar->ellipse;
          CRectPrim RP( pr->left, pr->top, pr->right, pr->bottom );
          switch( subType )
          {
            case ROUND_OUTLINED : pPrim = new COutlinedCircle( RP, SI, FI ); break;
            case ROUND_STROKED  : pPrim = new CRing( RP, SI ); break;
            case ROUND_FILLED   : pPrim = new CCircle( RP, FI ); break;
            case OUTLINED       : pPrim = new COutlinedOval( RP, SI, FI ); break;
            case STROKED        : pPrim = new CEllipse( RP, SI ); break;
            case FILLED         : pPrim = new COval( RP, FI ); break;
          }
          *ppSrc = pr + 1;
          break;
        }

        case PF_RECTANGLE :
        {
          RectParam* pr = &pObjPar->rectangle;
          CRectPrim RP( pr->left, pr->top, pr->right, pr->bottom, pr->angle );
          switch( subType )
          {
            case OUTLINED : pPrim = new COutlinedBar( RP, SI, FI ); break;
            case STROKED  : pPrim = new CRectangle( RP, SI ); break;
            case FILLED   : pPrim = new CBar( RP, FI ); break;
          }
          *ppSrc = pr + 1;
          break;
        }

        case PF_ARC :
        {
          ArcParam* pr = &pObjPar->arc;
          CRectPrim RP( pr->left, pr->top, pr->right, pr->bottom, pr->angle );
          switch( subType )
          {
            case STROKED              : pPrim = new CArc( RP, pr->delta, SI ); break;
            case FILLED               : pPrim = new CPie( RP, pr->delta, FI ); break;
            case FILLED|CHORD         : pPrim = new CChord( RP, pr->delta, FI ); break;
            case FILLED|STROKED|CHORD : pPrim = new COutlinedChord( RP, pr->delta, FI, SI ); break;
          }
          *ppSrc = pr + 1;
          break;
        }
      }

      pPrim->SetFlags( flags );
      pCont->AddTail( pPrim );
    }
  }

  return pCont;
}
//=====================================================================
int
CMapView::LoadMap( CGraphData* pDst, char* pSrc, int nObj, int scale, int bFull )
{
  while( --nObj >= 0 )
  {
    memcpy( m_pDRH->GetBriefPtr(), pSrc, m_pDRH->GetBriefSize());
    char* pNxt = pSrc + m_pDRH->GetDataSize() + m_pDRH->GetBriefSize();

    if( !bFull )
    {
      CIdent id( m_pDRH->GetIdent());

      int t = id.Class();
      int bSkip = !m_pSTM && !IsVisibleInScale( t, scale ) ||
                  pDst->GetObjectPtr( id ) || !m_pObjDef->IsClass( t );

      if( !bSkip && pDst == &m_ClipData )
      {
        POSITION pos;
        for( pos = m_SelectList.GetHeadPosition() ; !bSkip && pos ;
             bSkip = m_SelectList.GetNext( pos )->Ident() == id );

        for( pos = m_LastEditList.GetHeadPosition() ; !bSkip && pos ;
             bSkip = m_LastEditList.GetNext( pos )->Ident() == id );
      }

      if( bSkip  )
      {
        pSrc = pNxt;
        continue;
      }
    }

    CDrawObject* pObj = TranslateObjectFromMap( &pSrc );
    if( !pObj )
      return 0;

    if( !pObj->IsAccessable())
    {
      delete pObj;
      continue;
    }

    ScaleObject( pObj, scale );
    delete pDst->AddObject( pObj );
  }

  return 1;
}
//=====================================================================
CDrawObject*
CMapView::TranslateObjectFromMap( char** ppSrc )
{
  CDrawContainer* pCont = 0;
  int bs = m_pDRH->GetBriefSize();
  memcpy( m_pDRH->GetBriefPtr(), *ppSrc, bs );
  int ds = m_pDRH->GetDataSize();
  if( ds > 0 )
  {
    void* pData = *ppSrc + bs;
    *ppSrc = (char*)pData + ds;
    pCont = TranslateFromMap( &pData );

    if((char*)pData != *ppSrc )
    {
      delete pCont;
      pCont = 0;
    }
  }

  if( !pCont )
  {
#ifdef _FULL_PROJECT
    char m[128];
    sprintf_s( m, "Invalid object: class %d, table %d, object %d",
               m_pDRH->GetClass(), m_pDRH->GetTable(), m_pDRH->GetEntry());
    m_MapStore.TraceError( m );
#endif
    return 0;
  }

	int t = m_pDRH->GetClass();
  if( t <= 0 || t >= SUBSTAT_CLASS )
  {
#ifdef _FULL_PROJECT
    char m[128];
    sprintf_s( m, "Invalid object class: class %d, table %d, object %d",
               m_pDRH->GetClass(), m_pDRH->GetTable(), m_pDRH->GetEntry());
    m_MapStore.TraceError( m );
#endif
    return 0;
  }

  CDrawObject* pObj = new CDrawObject( m_pDRH->GetIdent(), m_pDRH->GetNode(),
                                       m_pDRH->GetTime(), m_pDRH->GetClock(), pCont );

  if( m_Version < 107 )
    pObj->CalcBoundBox();
  else
  {
    pObj->SetBoundBox((CBox*)&m_pDRH->GetBox());
    pObj->SetOriginBox((CBox*)&m_pDRH->GetBox());
  }
  return pObj;
}
//=====================================================================
CDrawObject*
CMapView::ExtractMapObj( CTabIdent id )
{
  TGstream buf;

  ResultCode r = m_MapStore.FindObject( id, buf );
  char* pSrc = buf.str();

  if( r != R_OK || !pSrc )
    return 0;

  CDrawObject* pObj = TranslateObjectFromMap( &pSrc );
  if( pObj )
    ScaleObject( pObj, GetTrueScale());
  return pObj;
}
//=====================================================================
int
CMapView::GetObjMifCoord( CTabIdent id, CString& str )
{
  CSmartObjPtr ptr;
  CDrawObject *pObj = GetObjectPtr( id );
  if( !pObj )
    ptr = pObj = ExtractMapObj( id );

  if( !pObj )
    return 0;
  
	str = CSelectTraceMode::GetMifCoordList( pObj );
  return 1;
}
//=====================================================================
int
CMapView::UpdateSetObjParam( CDrawObject* pObj, int bShow )
{
  if( !pObj )
    return 0;

  m_Conductor.Create( pObj );

  SaveReverseDirection();

#ifdef _FULL_PROJECT
  int bSl = GetProjectSLEnable();
#else
  int bSl = 0;
#endif

  int r = m_Conductor.DeclareEvent( CE_OBJ_PARAM );
  if( !pObj->IsSpecSelected() && ( !m_pSTM || m_pSTM->InTrace( pObj ) < 0 ))
    SelectObject( pObj, 0 );
	
	if( r || bSl )
    return 0;

  SaveReverseDirection( pObj );

  UpdateError er = SetObjParam( pObj, bShow );
  CDrawObject* pNew = GetObjectPtr( m_Conductor.GetIdent());
  int succ = er == UE_NOERROR && pNew;
  if( succ )
    m_Undo.AddItem( new CUndoChangeItem( this, pNew ), this );

  if( m_pSTM )
    m_pSTM->Reopen();

  return succ;
}
//=====================================================================
UpdateError
CMapView::SetObjParam( CDrawObject* pObj, int bShow )
{
  ASSERT( pObj );
  if( !pObj )
    return UE_OBJECT;

  CIdent id( m_Conductor.GetIdent());
  id.SetStatus(( id.Status() & 0x7f ) | ( pObj->Status() & 0x80 ));
  if( id.Class() == 0 )
    id.SetClass( pObj->Class());

  CDrawObject* pOld = pObj;
  pObj = pOld->Rebuild( m_Conductor.CreateParamList(), id.Class(), id.Status());
  ScaleObject( pOld, GetTrueScale());

  if( !pObj )
    return UE_BUILD;

  pObj->SetIdent( id );
  pObj->SetNode( m_Conductor.GetObjectNode());

  int bNew = pObj->TabIdent() != pOld->TabIdent();

  int f = pOld->SelectFlag();
  int bsb = f && pOld == m_pCurSelObj;

  if( bShow )
    SelectObject( pOld, 0 );

  if( bNew && !CMoveMode::CopyMode())
    RemoveObject( pOld );
  else
  if( bShow )
    HideObject( pOld->Ident(), 1 );

  CObjList lst;
  lst.AddHead( pObj );

  if( bShow )
  {
    SaveToStore( lst, bNew, 1 );
    SelectObject( pObj, f );
    if( bsb )
      StartBlinking( pObj );
  }
  else
  {
    SaveToStore( lst, bNew, 0 );
    delete pObj;
  }

  return UE_NOERROR;
}
//=====================================================================
UpdateError
CMapView::CreateInContext( int bShow )
{
  int nob = m_Conductor.GetLinkSize();
  if( nob <= 0 || nob > 2 )
    return UE_CONTEXTSIZE;

  ksi_obj onv[] = { ksi_false, ksi_false };

  while( --nob >= 0 )
  {
    CTabIdent ti( m_Conductor.GetTable( nob ));
    
    CSmartObjPtr ptr;
    CDrawObject *pObj = GetObjectPtr( ti );

    if( !pObj )
      ptr = pObj = ExtractMapObj( ti );

    if( !pObj )
      return UE_OBJECT;

    ksi_obj img = *pObj;
    onv[nob] = KsiCall( "unwrap", ksi_int2num( pObj->Class()),
                                  ksi_int2num( pObj->Status()),
                                  img, 0 );
  }

  CStringArray const& par = m_Conductor.GetParams();
  int np = par.GetSize() & ~1;

  ksi_obj env = ksi_nil;
  for( int i = 0 ; i < np ; i += 2 )
  {
    CString n( par[i] );
    CString v(par[i+1]);
    n.MakeLower();
    env = ksi_assv_set_x( env, ksi_str02sym( n ), ksi_str02string( v ));
  }

  CIdent id( m_Conductor.GetIdent());
  if( id.IsNull())
    id.SetEntry( GetUniId());

  ksi_obj val = KsiCall( "create-in-global-context",
                         KSI_MK_SINT( id.Class()),
                         KSI_MK_SINT( id.Status()),
                         env, onv[0], onv[1], 0 );

  if( !KSI_PAIR_P( val ))
    return UE_BUILD;

  id.SetStatus(( id.Status() & 0x80 ) | ( ksi_num2int( KsiCall( "get-status", 0 )) & ~0x80 ));

  CDrawObject* pObj = new CDrawObject( val, id, m_Conductor.GetNode());
  if( !pObj )
    return UE_CONVERT;

	int n = m_Conductor.GetObjectNode();
	pObj->SetNode( n );
	ResultCode r = R_OK;
	if( n >= 0 )
	{
    TGstream nss;
    dword mg = INSMAGIC;
    nss.write((char*)&mg, sizeof mg );
    SetTimeClock( pObj );
    r = pObj->TranslateToMap( &nss ) ? m_MapStore.AddObject( nss ) : R_BADSTREAM;
	}

  if( bShow )
  {
    ScaleObject( pObj, GetTrueScale());
    delete m_ClipData.AddObject( pObj );
    ShowObjectBox( pObj );
    if( m_pSTM )
      m_pSTM->Reopen();
    ShowSelectedNum();
  }
  else
    delete pObj;

  return r == R_OK ? UE_NOERROR : UE_SERVER;
}
//=====================================================================
int
CMapView::GetObjParam( CDrawObject* pOb, CTabIdent id, CStringArray& par )
{
  CSmartObjPtr ptr;
  CDrawObject *pObj = pOb;
  if( !pObj )
    pObj = GetObjectPtr( id );

  if( !pObj )
    ptr = pObj = ExtractMapObj( id );

  if( !pObj )
    return 0;

  ksi_obj img = *pObj;
  ksi_obj env = KsiCall( "unwrap",
                         KSI_MK_SINT( pObj->Class()),
                         KSI_MK_SINT( pObj->Status()),
                         img, 0 );
  if( !KSI_LIST_P( env ))
    return 0;

  for( par.RemoveAll() ; env != ksi_nil ; env = KSI_CDR( env ))
  {
    ksi_obj pair = KSI_CAR( env );
    par.Add( KSI_SYM_PTR( KSI_CAR( pair )));
    par.Add( ksi_string2str( ksi_object2string( KSI_CDR( pair ))));
  }
  return 1;
}
//=====================================================================
int
CMapView::GetDistance( CTabIdent tid0, CTabIdent tid1 )
{
  CSmartObjPtr ptr;
  CDrawObject* pObj = GetObjectPtr( tid0 );
  if( !pObj )
    ptr = pObj = ExtractMapObj( tid0 );

  if( !pObj )
    return -1;

  CPoint* p = pObj->GetPoint();

  return p ? GetDistance( *p, tid1 ) : -1;
}
//=====================================================================
int
CMapView::GetDistance( CPoint const& pnt, CTabIdent tid1 )
{
  CSmartObjPtr ptr;
  CDrawObject* pObj = GetObjectPtr( tid1 );
  if( !pObj )
    ptr = pObj = ExtractMapObj( tid1 );

  if( !pObj )
    return -1;

  CPoint* p = pObj->GetPoint();

  return p ? MerkatorData.Dist( *p, pnt ) : -1;
}
//=====================================================================
int
CMapView::IsObjectInLegalRegion( CDrawObject* pObj, char const* opName, int mess )
{
  if( m_nRgn < 0 )
    return 1;

  int nPt, tp;
  CPoint* pts = pObj->GetAnyPoints( &tp, &nPt );

  ASSERT( nPt );

  for( ; --nPt >= 0 ; ++pts )
    for( int i = m_nRgn ; --i >= 0 ; )
      if( IsPointInPolygon( *pts, m_pRgn[i] + 1, m_pRgn[i][0].x, 0 ))
        return 1;

  if( mess )
  {
    char m[256];
    sprintf_s( m,
               "Извините! %s %s:\n"
               "объект '%s' находится вне разрешенного региона!",
               opName,
               mess == 1 ? "блокируется" :
               mess == 2 ? "отменяется"  : "",
               m_pObjDef->GetName( pObj->Class()));
    ErrBox( m );
  }
  return 0;
}
//=====================================================================
int
CMapView::GetIncludingObjects( CTabIdent tid, int type, int flags,
                               CTabIdArray& dst )
{
  if( !flags )
    return -1;

  CSmartObjPtr ptr;
  CDrawObject* pHstObj;

  if( !tid.IsEmpty())
  {
    pHstObj = GetObjectPtr( tid );
    if( !pHstObj  )
      ptr = pHstObj = ExtractMapObj( tid );
  }
  else
    pHstObj = GetBuilding( 0 ) ? GetConductor()->GetObject() : GetSelectObject( 0 );

  if( pHstObj )
  {
    int nh, hshp;
    CPoint* pHpnt = pHstObj->GetAnyPoints( &hshp, &nh );
    if( pHpnt )
    {
      int nObj = 0;
      TGstream buf;
      ResultCode r = m_MapStore.SelectByRect( pHstObj->GetBoundBox(), type, buf, &nObj );
      if( r == R_OK && nObj )
      {
        char* pSrc = buf.str();
        for( CDrawObject* pTstObj = 0 ; --nObj >= 0 ; delete pTstObj )
        {
          pTstObj = TranslateObjectFromMap( &pSrc );
          if( !pTstObj || pTstObj->Ident() == pHstObj->Ident())
            continue;

          CTabIdent tid( pTstObj->TabIdent());
          int nt, tshp;
          CPoint* pTpnt = pTstObj->GetAnyPoints( &tshp, &nt );

          if( !pTpnt )
            continue;

          #define CMP( a, b ) ( a * 4 + b )

          switch( CMP( hshp, tshp ))
          {
            case CMP( SH_POINT, SH_POINT ):
              if( *pHpnt == *pTpnt )
                break;
              continue;
            case CMP( SH_POLYLINE, SH_POINT ):
              if(( flags & IF_CROSS ) && IsPointOnPolyline( *pTpnt, pHpnt, nh ) >= 0 )
                break;
              continue;
            case CMP( SH_POLYGON, SH_POINT ):
              if(( flags & IF_CROSS ) && IsPointOnPolygon( *pTpnt, pHpnt, nh ) >= 0 )
                break;
              continue;
            case CMP( SH_POINT, SH_POLYLINE ):
              if(( flags & IF_CROSS ) && IsPointOnPolyline( *pHpnt, pTpnt, nt ) >= 0 )
                break;
              continue;
            case CMP( SH_POLYLINE, SH_POLYLINE ):
              if(( flags & IF_CROSS ) && IsPolylineCrossPolyline( pTpnt, nt, pHpnt, nh ))
                break;
              continue;
            case CMP( SH_POLYGON, SH_POLYLINE ):
              if(( flags & IF_CROSS ) && IsPolylineCrossPolygon( pTpnt, nt, pHpnt, nh ))
                break;
              continue;
            case CMP( SH_POINT, SH_POLYGON ):
              if(( flags & IF_CROSS ) && IsPointOnPolygon( *pHpnt, pTpnt, nt ) >= 0 )
                break;
              if(( flags & IF_INCLUDE ) && IsPointInPolygon( *pHpnt, pTpnt, nt, 0 ))
                break;
              continue;
            case CMP( SH_POLYLINE, SH_POLYGON ):
              if(( flags & IF_CROSS ) && IsPolylineCrossPolygon( pHpnt, nh, pTpnt, nt ))
                break;
              if(( flags & IF_INCLUDE ) && IsAllPointsInPolygon( pHpnt, nh, pTpnt, nt  ))
                break;
              continue;
            case CMP( SH_POLYGON, SH_POLYGON ):
              if(( flags & IF_CROSS ) && IsPolygonCrossPolygon( pHpnt, nh, pTpnt, nt ))
                break;
              if(( flags & IF_INCLUDE ) && IsAllPointsInPolygon( pHpnt, nh, pTpnt, nt  ))
                break;
              continue;
          }
          dst.Add( tid );
        }
      }
    }
  }

  return dst.GetSize();
}
//=====================================================================
void
CMapView::DrawRegion( CPoint* pPoint, int nPoint, int clr, int bFull )
{
	if( nPoint < 3 )
		return;

  CIntAssocMap* pct = &pDocument->m_ViewColorTab;
  static int k = -1;
  if( pct->TranslateKey( k ) < 0 )
  {
    k = pct->FindFreeKey();
    pct->Add( k, clr );
  }

	GetLayer( 0 )->SetVisible( 1 );
  CIdent id( 0, 0, 0 );
  HideObject( id, 1 );
  CDrawContainer* pc = new CDrawContainer;
  COutlinedPolygon* pl = new COutlinedPolygon(
    CFillItem( CForeBackItem( k, -1 ), BS_HATCHED, HS_DIAGCROSS ),
    CStrokItem( 0, PS_SOLID, 100 ), nPoint, pPoint );
  pl->SetFlags( SF_LOW_VISIBLE | SF_HIG_VISIBLE, SF_STATUS_MASK );
  pc->AddTail( pl );
  CDrawObject* pObj = new CDrawObject( id, 0, 0, 0, pc );
  pObj->CalcBoundBox();

  CRect box( pObj->GetBoundBox());
  CPoint cn( box.CenterPoint());
  int s = CalcShowScale( pObj, bFull );
  CRect r( m_Clip );
  if( !r.PtInRect( cn ) || abs( s - m_AltScale ) > 1 )
    SlowMove( cn, s );
	delete m_ClipData.AddObject( pObj );
	ShowObjectBox( pObj );
}
//=====================================================================
char const*
CMapView::MakeSpaceQue( SpaceQueParam& QP )
{
  int bSetNode = QP.mode & SQ_SETNODE;
  int bDrawRgn = QP.mode & SQ_DRAWRGN;
  int bInside  = QP.mode & SQ_INSIDE;
  int bOnside  = QP.mode & SQ_ONSIDE;
  int bSpeak   = !( QP.mode & SQ_SILENT );

  if( !bInside && !bOnside )
  {
    static char const m[] = "Некорректные параметры поиска";
    if( bSpeak )
      ErrBox( m );
    return m;
  }

  CSmartPtr ptr;
  CDrawObject* pObs = GetObjectPtr( QP.id );
  if( !pObs )
  {
    pObs = ExtractMapObj( QP.id );
    if( !bSpeak )
      ptr = pObs;
    else
    if( pObs )
      m_ClipData.AddObject( pObs );
  }

  if( !pObs )
  {
    static char const m[] = "Не найден центральный объект";
    if( bSpeak )
      ErrBox( m );
    return m;
  }

  if( bSpeak )
    SelectObject( pObs, 0 );

  int n, t;
  CPoint const* pg = pObs->GetAnyPoints( &t, &n );

  if( !pg )
  {
    static char const m[] = "Недопустимый центральный объект";
    if( bSpeak )
      ErrBox( m );
    return m;
  }

  CPoint Center( t == SH_POINT ? pg[0] : pObs->GetBoundBox().CenterPoint());

  CWaitCursor wc;
  if( bSpeak )
    ReportHint( "Пожалуйста, подождите..." );

  TGstream ps;

  if( t == SH_POLYGON && IsSelfCrossPolygon( pg, n ))
  {
    static char const m[] = "Самопересекающийся центральный объект";
    if( bSpeak )
      ErrBox( m );
    return m;
  }

  if( t == SH_POLYLINE && IsSelfCrossPolyline( pg, n ))
  {
    static char const m[] = "Самопересекающийся центральный объект";
    if( bSpeak )
      ErrBox( m );
    return m;
  }

  int np = MakeSpace( ps, (CPoint*)pg, n, QP.radius, t == SH_POLYLINE );
  pg = (CPoint const*)ps.str();

  if( np < 3 )
  {
    static char const m[] = "Недопустимый центральный объект";
    if( bSpeak )
      ErrBox( m );
    return m;
  }

  TGstream buf;
  int nId = 0;

#ifdef _FULL_PROJECT
  for( t = CLASS_RANGE ; --t >= 0 ; )
    if( QP.classMap[t>>3] & ( 1 << ( t & 7 )))
    {
      TGstream tmpbuf;
      int nidtmp = 0;
      m_MapStore.ClipObjectsId((Point*)ps.str(), np, t, tmpbuf, &nidtmp );
      if( nidtmp)
      {
        buf.write( tmpbuf.str(), tmpbuf.pcount());
        nId += nidtmp;
      }
    }
#else
  m_MapStore.ClipObjectsId( (POINT*)ps.str(), np, QP.classMap, buf, &nId );
#endif

  if( QP.maxSel )
  {
    if( bSpeak && QP.maxSel > 0 && nId > QP.maxSel )
    {
      CString s;
      s.Format( "В запросе может быть более %d объектов.\n"
                "Их загрузка и выделение может занять\n"
                "продолжительное время.\n\n"
                "Выполнять выделение объектов?", QP.maxSel );
      int r = AskBox( s, 1 );
      QP.maxSel = r == IDYES;
      if( r == IDCANCEL )
        return 0;
    }
    if( QP.maxSel )
      ClearSelection();
  }

  if( bDrawRgn )
		DrawRegion((CPoint*)ps.str(), np, RGB( 176, 176, 176 ));

  ObjectInfoArray& tSel = m_Conductor.GetSelected();
  tSel.RemoveAll();

  CMainWin* pmw = (CMainWin*)AfxGetMainWnd();
  int nObj = 0;
  if( bSpeak )
    pmw->ShowProgress( 0, nId * 2 );

  CTabIdent* pId = (CTabIdent*)buf.str();
  CTabIdent* sId = pId;
  CTabIdent* fId = sId;

  CObjList lst;
  CDrawObject* pObj;

  for( int i = nId ; --i >= 0 ; ++pId )
    if(( pObj = m_ClipData.GetObjectPtr( *pId )) && pObj->Class() < CLASS_RANGE )
    {
      lst.AddHead( pObj );
      if( bSpeak )
        pmw->OffsetProgress( 1 );
    }
    else
      *fId++ = *pId;

  nId = fId - sId;

  if( nId )
  {
    ResultCode r = R_REPEAT;
    while( r == R_REPEAT )
    {
      TGstream bur;
      r = m_MapStore.FindObjects( sId, &nId, bur );
      char* pSrc = bur.str();
      while( --nId >= 0 )
        if(( pObj = TranslateObjectFromMap( &pSrc )) && pObj->Class() < CLASS_RANGE )
        {
          lst.AddHead( pObj );
          if( bSpeak )
            pmw->OffsetProgress( 1 );
        }
      sId = 0;
      nId = 0;
    }
  }
  
  while( !lst.IsEmpty())
  {
    pObj = lst.RemoveHead();
    CDrawObject* pObl = m_ClipData.GetObjectPtr( pObj->Ident());

    if( pObl && pObl != pObj )
    {
      delete pObj;
      pObj = pObl;
    }

    int tp, npp = 1;
    CPoint* pp = pObj->GetAnyPoints( &tp, &npp );
    CPoint cp = pObj->GetBoundBox().CenterPoint();
    if( !pp )
    {
      pp = &cp;
      tp = SH_POINT;
    }
    
    if( bInside && IsAllPointsInPolygon( pp, npp, pg, np ) ||
        bOnside &&
        ( tp == SH_POINT    && IsPointOnPolygon( *pp, pg, np, QP.radius / 100 ) >= 0 ||
          tp == SH_POLYGON  && IsPolygonCrossPolygon( pp, npp, pg, np ) ||
          tp == SH_POLYLINE && IsPolylineCrossPolygon( pp, npp, pg, np )))
    {
      ++nObj;
      int dst = 0x7fffffff;
      while( --npp >= 0 )
      {
        int d = dist( Center, *pp++ );
        dst = min( dst, d );
      }

      ObjectInfo inf( pObj->Class(), pObj->TabIdent(), dst );
      int i = tSel.Add( ObjectInfo());
      for( ; --i >= 0 && tSel[i].Distance > inf.Distance ; tSel[i+1] = tSel[i] );
      tSel[i+1] = inf;

      if( bSetNode )
      {
        pObj->SetNode( QP.node );
        m_MapStore.SetObjectNode( pObj->TabIdent(), QP.node );
      }
      if( !pObl )
        if( QP.maxSel )
          m_ClipData.AddObject( pObj );
        else
          delete pObj;
      if( QP.maxSel )
        SelectObject( pObj, 1 );
    }
    else
    if( !pObl )
      delete pObj;

    if( bSpeak )
      pmw->OffsetProgress( 1 );
  }

  if( bSpeak )
    pmw->HideProgress();

  m_bSpaceSelection = m_SelectList.GetCount() > 0;
  if( bSpeak /*&& tSel.GetSize()*/)
    m_Conductor.DeclareEvent( QP.eventCode );

#ifndef _FULL_PROJECT
  if( nObj )
    m_ObjSelectDialog.Init( this, &tSel );
#endif _FULL_PROJECT

  return 0;
}
//=====================================================================
void
CMapView::MakeSpaceQue( int bSetNode )
{
  CDrawObject* pObs = GetSelectObject( 1 );

  if( !pObs )
    return;

	CSpaceQueDialog dlg( m_pObjDef, this, bSetNode );
  if( DoModalDlg( &dlg, CE_PAR_DLG ) != IDOK )
    return;

  UINT mode = ( CSpaceQueDialog::GetInside() ? SQ_INSIDE  : 0 ) |
              ( CSpaceQueDialog::GetOnside() ? SQ_ONSIDE  : 0 ) |
              ( CSpaceQueDialog::GetRegion() ? SQ_DRAWRGN : 0 ) |
              ( bSetNode ? SQ_SETNODE : 0 );
  int r = bSetNode ? 0 : MulDiv( CSpaceQueDialog::GetRadius(), 100, SmInUnit );

  m_Conductor.m_SpaceQueParam.mode = mode;
  m_Conductor.m_SpaceQueParam.eventCode = CE_SPACE_QUERY;
  m_Conductor.m_SpaceQueParam.maxSel = CSpaceQueDialog::GetSelect() ? 500 : 0;
  m_Conductor.m_SpaceQueParam.node = CSpaceQueDialog::GetNode();
  m_Conductor.m_SpaceQueParam.id = pObs->TabIdent();
  m_Conductor.m_SpaceQueParam.radius = r;
  memcpy( m_Conductor.m_SpaceQueParam.classMap, CSpaceQueDialog::GetClassMap(),
          sizeof m_Conductor.m_SpaceQueParam.classMap );

  OnExtSpaceQue();
}
//=====================================================================
void
CMapView::GetScaleRatio( int t, int* pNum, int* pDen, int prnt )
{
  ASSERT( pNum );
  ASSERT( pDen );

  *pNum = *pDen = 1;

  int minScale = m_pObjDef->GetMinScale( t );
  int maxScale = m_pObjDef->GetMaxScale( t );
  int scale = prnt ? GetPrnScale() : m_AltScale;
  int tscle = scale;

  if( minScale > 0 && tscle < minScale )
    tscle = minScale;
  if( maxScale > 0 && tscle > maxScale )
    tscle = maxScale;

  if( scale != tscle )
  {
    *pDen = tscle;
    *pNum = scale;
  }
}
//=====================================================================
void
CMapView::SetTimeClock( CDrawObject* pObj )
{
  long t;
  long c;
  m_MapStore.GetTimeClock( &t, &c );
  pObj->SetTimeClock( t, c );
}
//=====================================================================
void
CMapView::SetValidNodeList( int nNodes, long* pNodes )
{
#ifdef _FULL_PROJECT
  if( nNodes > 0 && pNodes )
    m_Conductor.SetCurrentNodeList( nNodes, pNodes );

  m_MapStore.SetValidNodeList( m_Conductor.m_nNodes, m_Conductor.m_pCurrentNodeList );
  ReloadAll();
#endif
}
//=====================================================================
void
CMapView::ReloadAll()
{
  ClearSelection( 0, 0 );
  m_ClipData.ClipObjects( 0, 0 );
  m_Clip.SetRectEmpty();
  Invalidate( 0 );
}
//=====================================================================
void
CMapView::FlushMap()
{
#ifdef _FULL_PROJECT
  m_MapStore.Flush();
#endif
}
//=====================================================================
