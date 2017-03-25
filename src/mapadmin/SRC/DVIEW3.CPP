#include "stdafx.h"

#include "sltrmode.h"
#include "graphdoc.h"
#include "mview.h"
#include "mdoc.h"
#include "dlg0.h"
#include "movemode.h"
#include "miscmode.h"
#include "mathutil.h"
#include "bldmode.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "dwin.h"
#include "util.h"
#include "ksi.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "mercator.h"
#include <complex>
//========================================================================
using namespace std;
typedef complex<double> dcomplex;
//========================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//========================================================================
int
CMapView::GetTrueScale()
{
  return m_AltScale / m_ScaleFactor + .5;
}
//========================================================================
int
CMapView::IsVisibleInScale( int nType, int nScale )
{
  if( !nScale )
    return 1;

  int maxLim = m_pObjDef->GetMaxLimit( nType );
        int minLim = m_pObjDef->GetMinLimit( nType );
        return ( !maxLim || nScale <= maxLim ) && ( !minLim || nScale >= minLim );
}
//========================================================================
CSpot
CMapView::GetSpot( CPoint const& p, int r )
{
  r >= 0 || ( r = m_Sensibility );
  CSize d( r, r );
  d = DP2LP( d );
  return CSpot( p, max( d.cx, d.cy ));
}
//========================================================================
void
CMapView::ShowBox( CRect box )
{
  box = LP2DP( box );
  box.InflateRect( 5, 5 );
  m_bRedraw = 1;
  InvalidateRect( &box, 0 );
  box = DP2LP( box );
  m_BoxList.AddHead( box );
}
//========================================================================
void
CMapView::MoveSelectOn( CSize off )
{
  if( off.cx || off.cy )
    for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ;
         m_SelectList.GetNext( pos )->MoveOn( off ));
}
//========================================================================
int
CMapView::SpinSelectOn( CSize center, int angle )
{
  if( !angle )
    return 0;

  POSITION pos;
  CObjList SpinList;

  for( pos = m_SelectList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = m_SelectList.GetNext( pos );
    CDrawObject* pCop = pObj->CopyObject();
    pCop->SpinOn( center, angle );
    CDrawObject* pSpn = pCop->Rebuild();
    delete pCop;
    ScaleObject( pSpn, GetTrueScale());
    pSpn->SetSelectFlag( pObj->SelectFlag());
    SpinList.AddHead( pSpn );
  }

  CPoint dp( 0, 0 );
  CSize ds( GetDocument()->GetSize());
  CRect dr( dp, ds );

  int n = 0;
  for( pos = SpinList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = SpinList.GetNext( pos );
    if( !IsObjectInLegalRegion( pObj, "Поворот", 2 ))
    {
      while( !SpinList.IsEmpty())
        delete SpinList.RemoveHead();
      return 0;
    }

    CBox b( pObj->GetOriginBox());
    n += !( dr.PtInRect( b.TopLeft()) && dr.PtInRect( b.BottomRight()));
  }

  if( n )
  {
    char m[256];
    sprintf_s( m, "%s выходит\nза габарит проекта",
               n == 1 ? "Объект" : "Несколько объектов" );
    ErrBox( m );
    while( !SpinList.IsEmpty())
      delete SpinList.RemoveHead();
    return 0;
  }

  m_SelectList.RemoveAll();
  m_SelectList.AddHead( &SpinList );
  while( !SpinList.IsEmpty())
    delete m_ClipData.AddObject( SpinList.RemoveHead());

  return 1;
}
//========================================================================
int
CMapView::TestSelected()
{
  int nTabs = 0;
  for( POSITION pos = m_SelectList.GetHeadPosition() ; pos && nTabs < 2 ;
       nTabs += !m_SelectList.GetNext( pos )->TabIdent().IsNull());
  int r = !nTabs || nTabs == 1 && nTabs == m_SelectList.GetCount();
  if( !r )
    ErrBox( "Совместное копирование объектов учета запрещено!" );
  return r;
}
//========================================================================
void
CMapView::CopySelected()
{
  m_bDrawSel = 0;
  for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; m_SelectList.GetNext( pos ))
  {
    CDrawObject* pObj = m_SelectList.GetAt( pos );
    CDrawObject* pCop = pObj->CopyObject();
    m_SelectList.SetAt( pos, pCop );
    pObj->ResetSelectFlag();
    ShowObjectBox( pObj );
  }
  UpdateWindow();
  m_bDrawSel = 1;
}
//========================================================================
void
CMapView::ShowSelected( int bShow, int bSave, int bDclr )
{
  POSITION pos;
  if( bShow && bSave )
    SaveToStore( m_SelectList, 0, 0 );

  int bti = m_SelectList.GetCount() == 1;
  if( bDclr )
  {
    for( pos = m_SelectList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = m_SelectList.GetNext( pos );
      m_Conductor.Create( pObj );
      m_Conductor.DeclareEvent( CE_OBJ_EDIT );
      if( bti )
        TestCutIn( pObj );
    }
  }

  m_bDrawSel = bShow;
  for( pos = m_SelectList.GetHeadPosition() ; pos ;
       ShowObjectBox( m_SelectList.GetNext( pos )));
  ShowSelectedNum();
//  m_bDrawSel = 1;
}
//=====================================================================
void
CMapView::ResumeObject( CDrawObject* pObj, int bDeleted )
{
  CString t( "" );
  for( CVisiblePrim* p = pObj->GetActualCont()->GetHead() ; p ; p = p->GetNext())
    if( p->IsAnyVisible() && p->GetText())
    {
      t += t.IsEmpty() ? "(" : " ";
      t += p->GetText();
    }
  t += t.IsEmpty() ? "" : ")";
  char buf[1024];
  int l = sprintf_s( buf, "%s%s: %d-%d-%d-%d, Узел %d",
                     m_pObjDef->GetName( pObj->Class()), LPCSTR( t ),
                     pObj->Class(), pObj->Status(),
                     pObj->Table(), pObj->Entry(), pObj->Node());

  if( m_Version >= 103 )
  {
    CTime t( pObj->GetTime());
    CString st( t.Format( "%d.%m.%y %H:%M" ));
    if( st.IsEmpty())
      st = "?";
    l += sprintf_s( buf + l, sizeof buf - l, ", %s %s", bDeleted ? "Удален" : "Введен", LPCSTR( st ));
    if( SublDlgIsActive())
      m_pSublDlg->RefreshPosSize( pObj );
  }
#ifdef _DEBUG
  int np;
  if( pObj->GetAnyPoints( 0, &np ))
    sprintf_s( buf + l, sizeof buf - l, ", (%d)", np );
#endif
  ReportHint( buf );
}
//========================================================================
void
CMapView::ShowSelectedNum()
{
  if( GetMode( IDM_CONNECT_MODE ))
    return;

  char s[1024] = "";

  if( m_pSTM )
    sprintf_s( s, "%s [%d]",
               *m_pSTM->GetTraceName() ? m_pSTM->GetTraceName() : "Трасса кабеля (?) ",
               m_pSTM->GetTraceSize());
  else
  {
    int n = m_SelectList.GetCount();
    if( n == 1 )
      ResumeObject( m_SelectList.GetHead(), 0 );
    else
      sprintf_s( s, n ? "Выделено: %d" : "", n );
  }
  if( s[0] )
    ReportHint( s );
}
//========================================================================
int
CMapView::MakeSelectList( CRect const& r, CObjList& list )
{
  #define REPER_CLASS 97

  CRect sr( r );
  sr.NormalizeRect();

  if( sr.IsRectEmpty())
  {
    CDrawObject* pObj = m_ClipData.SelectBySpot( GetSpot( sr.TopLeft()), 0 );
    if( pObj )
      list.AddHead( pObj );
  }
  else
    m_ClipData.SelectByRect( sr, list );

  int bSa = SublDlgIsActive();
  int sc = bSa ? m_pSublDlg->GetLayerNum() + SUBLAYER_CLASS + 1 : 0;

  for( POSITION pos = list.GetTailPosition() ; pos ; )
  {
    POSITION tps = pos;
    list.GetPrev( pos );
    CDrawObject* pObj = list.GetAt( tps );
    int t = pObj->Class();
//#ifndef _DEBUG
    if( bSa ? t != sc && t != REPER_CLASS : pObj->IsSublayer())
      list.RemoveAt( tps );
//#endif
  }

  if( sr.IsRectEmpty())
    while( list.GetCount() > 1 )
      list.RemoveTail();

  if( m_bSelectLinked && list.GetCount() == 1 )
  {
    CObjList lst;
    CDrawObject* pObj = list.GetHead();
    for( int n = GetTargetsGlobal( pObj, &lst ) + GetHostsGlobal( pObj, &lst ) ;
         --n >= 0 ; list.AddHead( lst.RemoveHead()));
  }
  else
  if( m_bSelectAllLinked && list.GetCount() == 1 )
  {
    CObjList lst0, lst1;
    lst1.AddHead( list.GetHead());
    while( !lst1.IsEmpty())
    {
      while( !lst1.IsEmpty())
      {
        CDrawObject* pObj = lst1.RemoveHead();
        GetTargetsGlobal( pObj, &lst0 );
        GetHostsGlobal( pObj, &lst0 );
      }
      while( !lst0.IsEmpty())
      {
        CDrawObject* pObj = lst0.RemoveHead();
        if( list.Find( pObj ))
          continue;
        list.AddHead( pObj );
        lst1.AddHead( pObj );
      }
    }
  }
  return !list.IsEmpty();
}
//========================================================================
CLegend const* CMapView::m_pLg = 0;
//========================================================================
CDrawObject*
CMapView::StartBlinking( CDrawObject* pObj )
{
  if( m_pCurSelObj == pObj )
    return 0;

  CDrawObject* pOld;
  if( pOld = m_pCurSelObj )
  {
    if( m_pLg )
      m_pLg = m_pCurSelObj->SetLegend( m_pLg );
    KillTimer( IDM_BLINKSELTIMER );
    m_pCurSelObj->SetSelectFlag( m_BlinkStat );
    ShowObjectBox( m_pCurSelObj );
  }

  if( m_pCurSelObj = pObj )
  {
    m_BlinkStat = m_pCurSelObj->SelectFlag();
    pObj->SetSelectFlag( 1 );
    SetTimer( IDM_BLINKSELTIMER, 500, 0 );
  }
  return pOld;
}
//========================================================================
void
CMapView::MakeBlinking()
{
  CViewDC scrDC( this );
  using namespace Gdiplus;
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR GdiplusToken;
  GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );

  if( m_pCurSelObj->IsSpecSelected())
  {
    m_pLg = m_pCurSelObj->SetLegend( 0 );
    m_pCurSelObj->SetSelectFlag( 0 );
  }
  else
  if( m_pLg )
  {
    m_pLg = m_pCurSelObj->SetLegend( m_pLg );
    m_pCurSelObj->SetSelectFlag( 1 );
  }
  else
    m_pCurSelObj->SetSelectFlag( m_pCurSelObj->SelectFlag() ^ SF_STATUS_MASK );

  m_pCurSelObj->Draw( &scrDC );

  GdiplusShutdown( GdiplusToken );
}
//========================================================================
void
CMapView::UnselObj()
{
  StartBlinking( 0 );
  for( POSITION pos = m_SelectList.GetTailPosition() ; pos ; )
  {
    CDrawObject* pObj = m_SelectList.GetPrev( pos );
    if( m_pSTM && CSelectTraceMode::InTrace( pObj ) < 0 )
      SelectObject( pObj, 0 );
  }
}
//========================================================================
void
CMapView::ClearSelection( int bDelete, int bMakeUndo )
{
  if( GetActiveMode( IDM_SHORTTRACE_MODE ) || GetActiveMode( IDM_CONNECT_MODE ))
    return;

  StartBlinking( 0 );
  m_bSpaceSelection = 0;

  if( m_SelectList.IsEmpty())
    return;

  if( !bDelete && bMakeUndo )
    m_Undo.AddItem( new CUndoSelectItem( this, 0, &m_SelectList ), this );

  int bCm = GetMode( IDM_MOVE_MODE ) && CMoveMode::CopyMode();

  while( !m_SelectList.IsEmpty())
  {
    CDrawObject* pObj = m_SelectList.RemoveHead();
    ShowObjectBox( pObj );
    pObj->UnselectSpecial();
    m_bSublayerChanged |= pObj->IsSublayer();

    CRect box( pObj->GetBoundBox());
    if( bDelete )
      if( bCm )
        delete pObj;
      else
        RemoveObject( pObj );
    ShowBox( box );
  }

  if( SublDlgIsActive() && m_SelectList.IsEmpty())
    m_pSublDlg->SelectItem( 0 );

  ShowSelectedNum();
}
//========================================================================
CDrawObject*
CMapView::SelectObject( CIdent const& id, int flags, int bShow )
{
  return SelectObject( m_ClipData.GetObjectPtr( id ), flags, bShow );
}
//========================================================================
//#define SORT_IN_NATIVE_LIST
//========================================================================
#ifdef SORT_IN_NATIVE_LIST
static void
SortObjList( CObjList& Dst, CObjList& Src, int n )
{
        switch( n )
        {
                case 1 :
                        Dst.AddHead( Src.RemoveHead());
                        break;
                case 2 :
                {
                        Dst.AddHead( Src.RemoveHead());
                        if( *Dst.GetHead() < *Src.GetHead())
                        Dst.AddTail( Src.RemoveHead());
                        else
                        Dst.AddHead( Src.RemoveHead());
                        break;
                }
                default :
                {
                        CObjList Tmp0, Tmp1;
                        SortObjList( Tmp0, Src, n / 2 );
                        SortObjList( Tmp1, Src, n - n / 2 );
      for(;;)
                        {
                                if( Tmp0.IsEmpty())
                                {
                                        Dst.AddTail( &Tmp1 );
                                        break;
                                }
                                if( Tmp1.IsEmpty())
                                {
                                        Dst.AddTail( &Tmp0 );
                                        break;
                                }
                    Dst.AddTail( *Tmp0.GetHead() < *Tmp1.GetHead() ? Tmp0.RemoveHead() : Tmp1.RemoveHead());
                        }
                }
        }
}
//========================================================================
#else
struct OLI
{
        CDrawObject* pObj;
        OLI* pNext;
        void Create( CDrawObject* p, OLI* n )
        {
                pObj = p;
                pNext = n;
        }
};
//========================================================================
static OLI*
SortOLI( OLI*& p, int n )
{
        switch( n )
        {
                case 0:
                  return 0;
                case 1:
                {
                  OLI* p0 = p;
                  p = p0->pNext;
                  p0->pNext = 0;
                  return p0;
                }
                case 2:
                {
                  OLI* p0 = p;
                  OLI* p1 = p0->pNext;
                  p = p1->pNext;
      if( *p1->pObj < *p0->pObj )
                        {
                          p1->pNext = p0;
                          p0->pNext = 0;
                          return p1;
                        }
                  else
                        {
                  p1->pNext = 0;
                          return p0;
                        }
                }
                default:
                {
        OLI* p0 = SortOLI( p, n / 2 );
            OLI* p1 = SortOLI( p, n - n / 2 );
            OLI hd;
            OLI* tl = &hd;
            for(;;)
                        {
              if( !p0 )
                                {
                            tl->pNext = p1;
                            break;
                                }
              if( !p1 )
                                {
                            tl->pNext = p0;
                            break;
                                }
            if( *p0->pObj < *p1->pObj )
                                {
                            tl = tl->pNext = p0;
                            p0 = p0->pNext;
                                }
                    else
                                {
                            tl = tl->pNext = p1;
                            p1 = p1->pNext;
                                }
                        }
        return hd.pNext;
                }
        }
}
#endif
//========================================================================
void
CMapView::SortShowSelect()
{
        int n = m_SelectList.GetCount();
        if( n == 0 )
                return;

        POSITION pos;

  CDrawObject* pObj = m_SelectList.GetTail();

#ifdef SORT_IN_NATIVE_LIST
  CObjList Tmp;
        SortObjList( Tmp, m_SelectList, n );
        m_SelectList.AddTail( &Tmp );
#else
  OLI* pHead = new OLI[n];
        OLI* p = pHead + n;
        OLI* x = 0;
        for( pos = m_SelectList.GetHeadPosition() ; pos ; )
        {
                (--p)->Create( m_SelectList.GetNext( pos ), x );
                x = p;
        }
  m_SelectList.RemoveAll();
        for( p = SortOLI( p, n ) ; p ; p = p->pNext )
                m_SelectList.AddTail( p->pObj );
        delete pHead;
#endif

        for( pos = m_SelectList.GetHeadPosition() ; pos ; ShowObjectBox( m_SelectList.GetNext( pos )));
  SetScale( m_AltScale, pObj->GetBoundBox().CenterPoint(), 1 );
}
//========================================================================
CDrawObject*
CMapView::SelectObject( CDrawObject* pObj, int flags, int bShow )
{
  if( bShow )
    StartBlinking( 0 );
  if( pObj )
  {
    if( bShow )
      ShowObjectBox( pObj );

    POSITION pos = m_SelectList.Find( pObj );
    if( pos )
    {
      if( !flags )
      {
        pObj->UnselectSpecial();
        m_SelectList.RemoveAt( pos );
      }
      else
        pObj->SetSelectFlag( flags );
    }
    else
    if( flags )
    {
      pObj->SetSelectFlag( flags );
      m_SelectList.AddTail( pObj );
    }
    if( bShow )
      ShowObjectBox( pObj );
  }

  if( SublDlgIsActive() && pObj && pObj->IsSublayer())
    if( m_SelectList.IsEmpty())
      m_pSublDlg->SelectItem( 0 );
    else
      m_pSublDlg->SelectItem( pObj );

  return pObj;
}
//========================================================================
void
CMapView::AddSelectRect( CRect const& r )
{
  CObjList list;
  if( MakeSelectList( r, list ))
  {
    CObjList lst;
    for( POSITION pos = list.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = list.GetNext( pos );
      if( !pObj->IsExtObject() && ( m_bMultiSelection || !m_SelectList.GetCount()) && SelectObject( pObj, 1 ))
        lst.AddHead( pObj );
    }
    ShowSelectedNum();
    if( m_pUndoItem )
      m_pUndoItem->SetList( &lst, 1 );
  }
}
//========================================================================
void
CMapView::SubSelectRect( CRect const& r )
{
  CObjList list;
  if( MakeSelectList( r, list ))
  {
    CObjList lst;
    for( POSITION pos = list.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = list.GetNext( pos );
      if( pObj->IsSelected() && SelectObject( pObj, 0 ))
        lst.AddHead( pObj );
    }
    ShowSelectedNum();
    if( m_pUndoItem && !lst.IsEmpty())
      m_pUndoItem->SetList( &lst, 0 );
  }
}
//==========================================================================
CBox
CMapView::CalcSelectBox()
{
  CBox box;
  for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ;
       box |= m_SelectList.GetNext( pos )->GetBoundBox());
  return box;
}
//========================================================================
void
CMapView::ShowObjectBox( CDrawObject* pObj )
{
  if( pObj )
  {
    m_bSublayerChanged |= pObj->IsSublayer();
    ShowBox( pObj->GetBoundBox());
  }
}
//========================================================================
void
CMapView::HideObject( CIdent const& id, int bDelete )
{
  if( bDelete )
    SelectObject( id, 0 );
  CDrawObject* pObj = m_ClipData.RemoveObject( id );
  if( pObj )
  {
    ShowObjectBox( pObj );
    if( bDelete )
    {
      if( pObj->IsSublayer() && SublDlgIsActive())
        m_pSublDlg->RemoveItem( pObj );
      delete pObj;
    }
  }
}
//========================================================================
CDrawObject*
CMapView::GetDetectedObject( int bPreSel, int bStrict )
{
  int dist = MAX_INT;
  CDrawObject* pObj = 0;
  CSpot spot = GetSpot();
  if( bPreSel )
    for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObt = m_SelectList.GetNext( pos );
      if( pObt->Detected( spot, bStrict, &dist ))
        pObj = pObt;
    }

  if( !pObj )
    pObj = m_ClipData.SelectBySpot( spot, bStrict );

  return pObj;
}
//========================================================================
void
CMapView::ShowAngle()
{
  int a = GetCrossAngle();
  char sg = ' ';
  if( a < 0 )
    a = -a, sg = '-';
  char s[40];
  sprintf_s( s, " Поворот на %c%d.%1d град.", sg, a/10, a%10 );
  ReportHint( s );
}
//=====================================================================
void
CMapView::DrawToPoint( int mode )
{
  CPoint fp = m_Position;
  if( mode > 3 )
  {
    UINT f = 0;
    CMagnitMode mm;
		mm.SetSelMode( mode - 4 );
    mm.Init( this );
    mm.OnMouseMove( f, fp );
  }
  else
  {
    CSpot spot( GetSpot( m_Position, -1 ));

    UINT fd0 = -1, fd1 = -1;
    CPoint p0, tp[3], fp0[3], fp1[3];

    CObjList lst;
    m_ClipData.SelectBySpot( spot, lst );

    while( !lst.IsEmpty())
    {
      CDrawObject* pObj = lst.RemoveHead();
      int fd = pObj->DrawToPoint( spot, tp, mode );
      if( fd < 0 )
        continue;

      if( fd < fd0 )
      {
        if( fd0 < fd1 )
        {
          fd1 = fd0;
          memcpy( fp1, fp0, sizeof tp );
        }
        fd0 = fd;
        memcpy( fp0, tp, sizeof tp );
      }
      else
      if( fd < fd1 )
      {
        fd1 = fd;
        memcpy( fp1, tp, sizeof tp );
      }
    }

    if( fd0 == -1 || mode == 3 && fd1 == -1 )
      return;

    CPoint c( spot.CenterPoint());

    switch( mode )
    {
      case 0:
        fp = fp0[0];
        break;
      case 1:
      {
        double dx = fp0[1].x - fp0[0].x;
        double dy = fp0[1].y - fp0[0].y;
        double dl = hipo( dx, dy );
        CPoint p0( TurnPoint( c - fp0[0], dx, dy, dl ));
        p0.y = p0.y > 0 ? m_Deviation : -m_Deviation;
        fp = fp0[0] + TurnPoint( p0, dx, -dy, dl );
        break;
      }
      case 2:
        p0 = fp1[0] = fp0[1];
        fp1[1] = fp0[2];
      case 3:
        if( mode != 2 && !CrossPoint( fp0, fp1, &p0 ) ||
            dist( p0, m_Position ) > spot.GetRadius())
          return;
        if( m_Deviation )
        {
          double dx0 = fp0[1].x - fp0[0].x;
          double dy0 = fp0[1].y - fp0[0].y;
          double dl0 = hipo( dx0, dy0 );
          double dx1 = fp1[1].x - fp1[0].x;
          double dy1 = fp1[1].y - fp1[0].y;
          double dl1 = hipo( dx1, dy1 );
          int y0 = TurnPoint( c - fp0[0], dx0, dy0, dl0 ).y;
          int y1 = TurnPoint( c - fp1[0], dx1, dy1, dl1 ).y;
          if( mode == 2 && ( y0 > 0 ) != ( y1 > 0 ))
          {
             double a = arg( dcomplex( fp1[1].x - fp1[0].x, fp1[1].y - fp1[0].y ) /
                             dcomplex( fp0[1].x - fp0[0].x, fp0[1].y - fp0[0].y ));
             y0 = y1 = a <= 0 ? 1 : -1;
          }

          y0 = y0 > 0 ? m_Deviation : -m_Deviation;
          y1 = y1 > 0 ? m_Deviation : -m_Deviation;
          CSize s0( -y0 * dy0 / dl0, y0 * dx0 / dl0 );
          CSize s1( -y1 * dy1 / dl1, y1 * dx1 / dl1 );
          fp0[0] += s0;
          fp0[1] += s0;
          fp1[0] += s1;
          fp1[1] += s1;
        }
        if( !CrossPoint( fp0, fp1, &fp, 0 ))
          return;

        break;
    }
  }

  if( m_Clip.PtInRect( fp ))
  {
    static UINT ct[][3] =
    {
      WM_MOUSEMOVE, WM_RBUTTONDOWN, WM_RBUTTONUP,
      WM_MOUSEMOVE, WM_LBUTTONDOWN, WM_LBUTTONUP
    };
    SetPosition( m_Position = fp );
    fp = LP2DP( fp );
    LPARAM LP = MAKELPARAM( fp.x, fp.y );
    UINT* pc = GetKeyState( VK_SHIFT ) < 0 ? ct[0] : ct[1];
    for( int i = 0 ; i < 3 ; SendMessage( pc[i++], -1, LP ));
  }
}
//=====================================================================
void
CMapView::SetPosition( CPoint fp )
{
  CSize dp( LP2DP( fp - m_Clip.TopLeft()));
  CSize ds( dp.cx < 0 ? dp.cx :
            dp.cx > m_ViewportSize.cx ? dp.cx - m_ViewportSize.cx : 0,
            dp.cy < 0 ? dp.cy :
            dp.cy > m_ViewportSize.cy ? dp.cy - m_ViewportSize.cy : 0 );

  if( ds.cx || ds.cy )
  {
    CSize cs( m_ViewportSize.cx / 8, m_ViewportSize.cy / 8 );
    ds.cx += ds.cx < 0 ? -cs.cx : ds.cx > 0 ? cs.cx : 0;
    ds.cy += ds.cy < 0 ? -cs.cy : ds.cy > 0 ? cs.cy : 0;

    ds = DP2LP( ds );
    SetScale( m_AltScale, GetCenter() + ds, 0 );
    UpdateWindow();
  }

  fp = LP2DP( fp );
  ClientToScreen( &fp );
  SetCursorPos( fp.x, fp.y );
}
//=====================================================================
void
CMapView::OnSetCoordGrid()
{
  ReportHint( "Щелкните левой кнопкой в реперной точке" );
  AddMode( new CSetCoordGridMode( 1 ));
}
//=====================================================================
CPoint
CMapView::Grid2Project( CPoint p )
{
  double x = p.x;
  double y = p.y;
  if( m_bUseGrid )
  {
    p.x -= m_X0 * 1e2 / SmInUnit;
    p.y -= m_Y0 * 1e2 / SmInUnit;
    p.x = m_XStep > 0 ? p.x : -p.x;
    p.y = m_YStep > 0 ? p.y : -p.y;
  }
  return p;
}
//=====================================================================
CPoint
CMapView::Project2Grid( CPoint p )
{
  if( m_bUseGrid )
  {
    p.x = m_XStep > 0 ? p.x : -p.x;
    p.y = m_YStep > 0 ? p.y : -p.y;
    p.x += m_X0 * 1e2 / SmInUnit;
    p.y += m_Y0 * 1e2 / SmInUnit;
  }
  return p;
}
//=====================================================================
void
CMapView::DrawSelect( CDC* pDC, CRect* pRect )
{
  for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = m_SelectList.GetNext( pos );
    if( !pRect || Intersect( pRect, &pObj->GetBoundBox()))
      pObj->Draw( pDC );
  }
}
//=====================================================================
void
CMapView::RemoveExtObjects()
{
  CObjList lst;
  for( int i = CLASS_RANGE ; --i >= 0 ; )
  {
    CLayer* pl = m_ClipData.GetLayerByClass( i );
    if( pl )
    {
      for( CDrawObject* pObj = pl->GetLast() ; pObj ; pObj = pl->FindPrev())
        if( pObj->IsExtObject())
          lst.AddHead( pObj );
    }
  }
  while( !lst.IsEmpty())
  {
    CDrawObject* pObj = lst.RemoveHead();
    m_ClipData.RemoveObject( pObj );
    ShowObjectBox( pObj );
    delete pObj;
  }
}
//=====================================================================
