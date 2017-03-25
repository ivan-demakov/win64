#include "stdafx.h"

#include "graphdoc.h"
#include "ksikey.h"
#include "ksi.h"
#include "ksi_type.h"
#include "mview.h"
#include "mdoc.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "dwin.h"
#include "miscmode.h"
#include "bldmode.h"
#include "sltrmode.h"
#include "conduct.h"
#include "util.h"
#include "mathutil.h"
#include "ksiutil.h"
#include "lawdlg.h"
#include "mercator.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CObjLinkNode* CObjLinkNode::m_pRoot = 0;
//=====================================================================
CObjLinkNode::CObjLinkNode( CDrawObject* pObj, CDrawObject* pHostObj ) :
  m_pObj( pObj ),
  m_bInEdition( 0 ),
  m_pNextNode( m_pRoot ),
  m_HostpObj0( pHostObj ),
  m_HostpObj1( 0 ),
  m_KsiEnv0( ksi_false ),
  m_Changed( FI_UNCHANGED )
{
  ksi_obj ksi_img = *pObj;
  m_KsiEnv1 = KsiCall( "unwrap", KSI_MK_SINT( pObj->Class()), ksi_zero, ksi_img, 0 );
  m_pRoot = this;
}
//=====================================================================
int
CObjLinkNode::HasTemp()
{
  for( CObjLinkNode* p = m_pRoot ; p ; p = p->m_pNextNode )
    if( p->m_pObj->IsObjectTemp() ||
        p->m_HostpObj0 && p->m_HostpObj0->IsObjectTemp() ||
        p->m_HostpObj1 && p->m_HostpObj1->IsObjectTemp())
      return 1;

  return 0;
}
//=====================================================================
int
CObjLinkNode::HasOutNode( int bDel )
{
  int r = 0;
  for( CObjLinkNode* p = m_pRoot ; p ; p = p->m_pNextNode )
  {
    CDrawObject* pObj = p->m_pObj;
    int n = pObj->Node();
    CConductor* pCnd = pView->GetConductor();
    int i = pCnd->m_nNodes;
    while( --i >= 0 && pCnd->m_pCurrentNodeList[i] != n );
    if( i < 0 )
    {
      r |= 1;
      if( bDel )
        pView->HideObject( pObj->Ident(), 1 );
    }
  }
  return r;
}
//=====================================================================
void
CObjLinkNode::Add( CDrawObject* pObj, CDrawObject* pHostObj )
{
  CObjLinkNode* p = new CObjLinkNode( pObj, pHostObj );
  if( p->m_pNextNode )
  {
    m_pRoot = p->m_pNextNode;
    p->m_pNextNode = m_pRoot->m_pNextNode;
    m_pRoot->m_pNextNode = p;
  }

  CObjList tl;
  for( int n = pView->GetTargetsGlobal( pObj, &tl ) ; --n >= 0 ; )
  {
    CDrawObject* pSubObj = tl.RemoveHead();
    for( p = m_pRoot ; p && p->m_pObj != pSubObj ; p = p->m_pNextNode );
    if( !p )
      Add( pSubObj, pObj );
    else
    if( p->m_HostpObj0 != pObj && !p->m_HostpObj1 )
      p->m_HostpObj1 = pObj;
  }
}
//=====================================================================
void
CObjLinkNode::RemoveAll()
{
  while( m_pRoot )
  {
    CObjLinkNode* p = m_pRoot->m_pNextNode;
    delete m_pRoot;
    m_pRoot = p;
  }
}
//=====================================================================
void
CObjLinkNode::AddToList( CObjList& tl, SelectFilter mask )
{
  for( CObjLinkNode* p = m_pRoot ; p ; p = p->m_pNextNode )
    if( p->m_Changed & mask )
      tl.AddHead( p->m_pObj );
}
//=====================================================================
void
CObjLinkNode::Rebuild()
{
  for( CObjLinkNode* p = m_pRoot ; p ; p = p->m_pNextNode )
    if( p->m_Changed & FI_CHANGED )
    {
      p->m_pObj->SetStatus(( p->m_pObj->Status() & 0x7f ) | ( pView->IsHiResolution() ? 0x80 : 0 ));
      p->m_pObj = p->m_pObj->Rebuild( p->m_KsiEnv1 );
    }
}
//=====================================================================
void
CObjLinkNode::EditBy( int CoeditStep, int bHR, CObjLinkNode* pHostNode, ksi_obj ksi_dc )
{
  static char const envName[] = "building-env";
  static ksi_obj CPL = ksi_eval_str_with_catch( "context-proc-list" );
  static ksi_obj FMP = ksi_eval_str_with_catch( "find-match-proc" );
  static ksi_obj GOP = ksi_eval_str_with_catch( "get-obj-prototype" );

  if( m_bInEdition )
    return;

  m_bInEdition = 1;

  if( KSI_LIST_P( m_KsiEnv1 ))
    m_KsiEnv0 = ksi_copy_tree( m_KsiEnv1 );

  ksi_obj env = ksi_eval_str_with_catch( envName );

  if( pHostNode )
  {
    ksi_obj curenv = ksi_current_env();
    ksi_obj host_class = KSI_MK_SINT( pHostNode->m_pObj->Class());
    ksi_obj test_class = KSI_MK_SINT( m_pObj->Class());
    ksi_obj save_env = env;
    ksi_obj proc = ksi_apply_3_with_catch( FMP, host_class, test_class, CPL );
    ksi_obj prot = ksi_apply_1_with_catch( GOP, test_class );
    ksi_obj DRP = ksi_eval_str_with_catch( "draw" );
    ksi_obj cont = ksi_copy_tree( prot );
    if( !( CoeditStep || m_HostpObj1 && pHostNode->m_pObj == m_HostpObj0 ))
    {
      ksi_defsym( envName, m_KsiEnv0, curenv );
      ksi_apply_2_with_catch( DRP, ksi_dc, cont );
    }
    env = ksi_apply_3_with_catch( KSI_CAR( KSI_CDR( proc )), pHostNode->m_KsiEnv0, pHostNode->m_KsiEnv1, m_KsiEnv1 );
    if( KSI_EQUAL_P( m_KsiEnv0, env ))
      env = ksi_false;
    else
    {
      ksi_defsym( envName, m_KsiEnv0, curenv );
      ksi_apply_2_with_catch( DRP, ksi_dc, cont );
      ksi_defsym( envName, env, curenv );
      ksi_apply_2_with_catch( DRP, ksi_dc, cont );
    }
    ksi_defsym( envName, save_env, curenv );
  }

  if( KSI_LIST_P( env ))
    m_KsiEnv1 = ksi_copy_tree( env );

  if( KSI_LIST_P( env ) || ( m_pObj->Status() & 0x80 ) != ( bHR ? 0x80 : 0 ))
    m_Changed = FI_CHANGED;

  for( CObjLinkNode* p = m_pRoot ; p ; p = p->m_pNextNode )
    if( p->m_HostpObj0 == m_pObj || p->m_HostpObj1 == m_pObj )
      p->EditBy( CoeditStep, bHR, this, ksi_dc );

  m_bInEdition = 0;
}
//=====================================================================
void
CMapView::BeginCoedition( CDrawObject* pHostObj, CBuildMode* pMode )
{
  GetHostsGlobal( pHostObj, &m_LastHostList );

  CObjLinkNode::Add( pHostObj, 0 );

  if( CObjLinkNode::HasTemp())
  {
    Beep( 440, 400 );
    ErrBox( "Попытка редактирования временного объекта" );
    CObjLinkNode::RemoveAll();
    RemoveMode( pMode );
    return;
  }
#ifdef _FULL_PROJECT
  if( CObjLinkNode::HasOutNode( 0 ))
  {
    Beep( 440, 400 );
    ErrBox( "Попытка редактирования закрытого объекта другого тех. узла" );
    BreakeCoedition();
    RemoveMode( pMode );
    return;
  }
#endif //_FULL_PROJECT

  CObjLinkNode::AddToList( m_LastEditList, FI_ALL );

  for( POSITION pos = m_LastEditList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = m_LastEditList.GetNext( pos );
    HideObject( pObj->Ident(), 0 );
  }

  m_CoeditStep = 0;
  DrawMode( pMode );
}
//=====================================================================
void
CMapView::BreakeCoedition()
{
  while( !m_LastEditList.IsEmpty())
  {
    CDrawObject* pObj = m_LastEditList.RemoveHead();
    delete m_ClipData.AddObject( pObj );
    ShowObjectBox( pObj );
  }

  CObjLinkNode::RemoveAll();

  delete m_pSuspObj;
  m_pSuspObj = 0;
  m_pDelObj = 0;
  KillUndoItem();

  if( SublDlgIsActive())
    m_pSublDlg->OnInsertionEnd( 0 );

  ksi_draw_diff( ksi_false );
}
//=====================================================================
void
CMapView::ChangeCoedition( ksi_obj ksi_dc )
{
  if( CObjLinkNode::m_pRoot )
    CObjLinkNode::m_pRoot->EditBy( m_CoeditStep, IsHiResolution(), 0, ksi_dc );
  m_CoeditStep = 1;
}
//=====================================================================
static ksi_obj
ksi_mk_txt_env( CDrawObject* pObj, CPoint* pl, int nl, CPoint* pp, int ns, int bFst )
{
  CPoint t( bFst ? pl[++ns] : pl[ns] );
  pl[ns] = *pp;

  CPoint* lp = bFst ? pl : pl + ns;
  int np = bFst ? ns + 1 : nl - ns;

  pObj->SetPoly( lp, np );

  int l = CalcLength( lp, np );
  CString ls;
  ls.Format( "%d.%d", l / 100, l % 100 / 10 );
  CPoint* ps = bFst ? lp  + np - 2 : lp;
  CPoint cp( ps[0].x + ps[1].x >> 1, ps[0].y + ps[1].y >> 1 );
  int a = atan2( ps[0].x - ps[1].x, ps[1].y - ps[0].y );
  int ang = a > 900 ? a - 1800 : a < -900 ? a + 1800 : a;
  struct { char* Name[3]; int Val; }
  m[] = { "tx", "tx1", "tx2", cp.x,
          "ty", "ty1", "ty2", cp.y,
          "ta", "ta1", "ta2", ang };

  ksi_obj env = KSI_LIST1( cons_with_sym( "length", ls ));
  for( int i = CELEM( m ) ; --i >= 0 ; )
    for( int k = CELEM( m[i].Name ) ; --k >= 0 ; )
      env = ksi_cons( cons_with_sym( m[i].Name[k], m[i].Val ), env );

  pl[ns] = t;

  return env;
}
//=====================================================================
void
CMapView::EndCoedition( CDrawObject* pObj )
{
  CDrawObject* pIns = 0;

  ClearSelection( 0, 0 );

  int bCh = 0;
  POSITION pos;

  for( pos = m_LastEditList.GetHeadPosition() ; !bCh && pos ; )
  {
    CDrawObject* pOb1 = m_LastEditList.GetNext( pos );
    CDrawObject* pOb2 = ExtractMapObj( pOb1->TabIdent());
    bCh |= !pOb2 || !pOb1->IsEqualTime( pOb2 );
    delete pOb2;
  }

  m_Conductor.Create( pObj );
  int i = m_Conductor.GetLinkSize();
  int hn = m_LastHostList.GetCount();
  int bHostChng = hn && i != hn;

  while( !bCh && --i >= 0 )
  {
    CTabIdent t( m_Conductor.GetTable( i ));
    CDrawObject* pOb1 = GetObjectPtr( t );
    if( !pOb1 )
      bHostChng = bCh = 1;
    else
    if( pOb1->IsObjectTemp())
      bCh = 2;
    else
    {
      CDrawObject* pOb2 = ExtractMapObj( t );
      bCh |= !pOb2 || ( pOb1->IsEqualTime( pOb2 ) ? 0 : !pOb1->IsEqual( pOb2 ));
      delete pOb2;
    }
  }

  if( bCh )
  {
    Beep( 440, 400 );
    ErrBox( bCh == 2 ? "Попытка построения с временным объектом" :
            "Извините, но с момента начала операции\n"
            "в план были внесены существенные изменения.\n\n"
            "Вы можете продолжить работу после обновления." );
    delete pObj;
    while( !m_LastEditList.IsEmpty())
      delete m_LastEditList.RemoveHead();
    BreakeCoedition();
    if( bCh == 1 )
      ReloadAll();
    return;
  }

  pObj->SetStatus(( pObj->Status() & 0x7f ) | ( IsHiResolution() ? 0x80 : 0 ));

  if( CObjLinkNode::m_pRoot )
  {
    if( m_bNoLinkEdit )
    {
      int n0;
      CPoint* p0 = pObj->GetPolyline( &n0 );
      if( p0 )
      {
        CDrawObject* pOld = CObjLinkNode::m_pRoot->GetNodeObject();
        int n1;
        CPoint* p1 = pOld->GetPolyline( &n1 );
        if( !( p0[0] == p1[0] && p0[n0-1] == p1[n1-1] || p0[0] == p1[n1-1] && p0[n0-1] == p1[0] ))
        {
          Beep( 440, 400 );
          CString m;
          m.Format( "Для линейного объекта '%s'\nизменение конечных точек запрещено!", m_pObjDef->GetName( pObj->Class()));
          ErrBox( m );
          delete pObj;
          BreakeCoedition();
          return;
        }
      }
    }

    CObjList oldList, newList;

    m_LastEditList.RemoveAll();

    CObjLinkNode::AddToList( oldList, FI_UNCHANGED );
    while( !oldList.IsEmpty())
    {
      CDrawObject* pObj = oldList.RemoveHead();
      delete m_ClipData.AddObject( pObj );
      ShowObjectBox( pObj );
    }

    CObjLinkNode::AddToList( oldList, FI_CHANGED );

    if( !oldList.IsEmpty())
    {
      CIdent id = pObj->Ident();

      SaveReverseDirection();

      m_ClipData.AddObject( pObj );
      ShowObjectBox( pObj );

      int bCont = IsObjectInLegalRegion( pObj, "Редактирование", 2 ) &&
                  ( !pObj->Table() || !bHostChng || !m_Conductor.DeclareEvent( CE_OBJ_PARAM ));

      m_ClipData.RemoveObject( pObj );
      ShowObjectBox( pObj );

      if( bCont )
      {
        SaveReverseDirection( pObj );

        if( bHostChng )
        {
          id = m_Conductor.GetIdent();
          id.SetStatus( id.Status() & 0x7f | ( IsHiResolution() ? 0x80 : 0 ));
          m_ObjectClass = id.Class();

          CDrawObject* pOld = pObj;
          pObj = pOld->Rebuild( m_Conductor.CreateParamList(), id.Class(), id.Status());

          if( pObj )
            delete pOld;
        }

        if( m_pUndoItem )
          m_pUndoItem->SetList( &oldList, 0 );

        CObjLinkNode::Rebuild();
        CObjLinkNode::AddToList( newList, FI_CHANGED );

        for( pos = newList.GetHeadPosition() ; pos ; )
        {
          CDrawObject* pOb = newList.GetNext( pos );
          m_Conductor.Create( pOb );
          m_Conductor.DeclareEvent( CE_OBJ_EDIT );
          if( pOb->TabIdent() == pObj->TabIdent() && newList.GetCount() == 1 )
            pIns = pOb;
        }

        delete pObj;
        pObj = 0;

        if( m_pUndoItem )
        {
          m_pUndoItem->SetList( &newList, 1 );
          m_Undo.AddItem( m_pUndoItem, this );
          m_pUndoItem = 0;
        }

        SaveToStore( newList, 0, 1 );

        while( !oldList.IsEmpty())
          delete oldList.RemoveHead();

        if( !m_pSTM )
        {
          CBuildMode* pBM = GetBuilding( 0 );
          SelectObject( GetSelectObject( 0 ), 0 );
          SelectObject( id, pBM->GetSelStatus());
        }
      }
      else
      {
        delete pObj;
        while( !oldList.IsEmpty())
        {
          CDrawObject* pObj = oldList.RemoveHead();
          delete m_ClipData.AddObject( pObj );
          ShowObjectBox( pObj );
        }
      }
    }
    CObjLinkNode::RemoveAll();

    TestCutIn( pIns );
  }
  else
  {
    if( KSI_TRUE_P( ksi_eval_str_with_catch( "&suspend-object?" )))
    {
      ksi_obj ksi_list = KsiCall( "get-brife-host-class-list", KSI_MK_SINT( pObj->Class()), 0 );
      int ClassArr[CLASS_RANGE];
      int nClass = 0;

      while( KSI_PLIST_P( ksi_list ))
      {
        ClassArr[nClass++] = ksi_num2long( KSI_CAR( ksi_list ));
        ksi_list = KSI_CDR( ksi_list );
      }

      m_pSuspObj = pObj;
      CBuildMode::SetContext();
      CMenu popup;
      popup.CreatePopupMenu();
        CMapDoc* pDoc = GetDocument();
            pDoc->SetContextMenu( 1 );
      pDoc->MakePopupMenu( &popup, ClassArr, nClass, IDM_CONTEXT_BEGIN );
      if( popup.GetMenuItemCount())
      {
        CPoint p;
        GetCursorPos( &p );
        popup.TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, p.x, p.y, this );
        GetDocument()->DeletePopupMenu( &popup, 0 );
        popup.DestroyMenu();
      }
      return;
    }

    m_Conductor.Create( pObj );
    CDrawObject* pObj0 = m_ClipData.GetObjectPtr( CBuildMode::GetContextId());
    if( pObj0 )
    {
      for( int i = m_Conductor.GetLinkSize() ;
           --i >= 0 && m_Conductor.GetTable( i ) != pObj0->TabIdent() ; );
      if( i < 0 )
        m_Conductor.AddLink( pObj0->TabIdent());
    }

    SaveReverseDirection();

    m_ClipData.AddObject( pObj );
    ShowObjectBox( pObj );

    int bCancel = !IsObjectInLegalRegion( pObj, "Построение", 2 ) ||
                  m_Conductor.DeclareEvent( CE_OBJ_BUILD );

    m_ClipData.RemoveObject( pObj );
    ShowObjectBox( pObj );

    CIdent id;

    if( !bCancel )
    {
      id = m_Conductor.GetIdent();
      if( !id.Table())
        id.SetEntry( GetUniId());

      if( id.Entry() == -1 )
                        {
        KillUndoItem();
                                RemoveTempObj();
                        }

      if( m_MapStore.TestObject( id ) == R_OK )
      {
        if( id.Entry() == -1 )
          RemoveObject( id );
        else
        {
          ErrBox( "Объект с таким идентификатором уже существует.\n"
                  "Обратитесь к администратору проекта." );
          bCancel = 1;
        }
      }
    }

    if( !bCancel )
    {
      SaveReverseDirection( pObj );

      id.SetStatus( id.Status() & 0x7f | ( IsHiResolution() ? 0x80 : 0 ));
      m_ObjectClass = id.Class();
      pObj->SetTabId( id.TabIdent());
      ScaleObject( pObj, GetTrueScale());
      CDrawObject* pNew = pObj->Rebuild( m_Conductor.CreateParamList(), id.Class(), id.Status());

      if( pNew )
      {
        if( SublDlgIsActive() && !m_pSublDlg->OnInsertionEnd( pNew ))
        {
          delete pNew;
          bCancel = 1;
        }

        if( bCancel )
          KillUndoItem();
        else
        {
          CObjList AddList, RemList;

          AddList.AddHead( pNew );

          if( m_pDelObj )
            RemList.AddHead( m_pDelObj );

          if( m_pUndoItem )
          {
            m_pUndoItem->SetList( &RemList, 0 );
            m_pUndoItem->SetList( &AddList, 1 );
            m_Undo.AddItem( m_pUndoItem, this );
            m_pUndoItem = 0;
          }

          while( !RemList.IsEmpty())
            RemoveObject( RemList.RemoveHead());


                                        SaveToStore( AddList, 1, 1 );

          m_Conductor.Create( pNew );
          m_Conductor.DeclareEvent( CE_OBJ_BUILD_END );

          if( !GetProjectSLEnable())
            TestCutIn( GetObjectPtr( id ));
        }
      }
    }

    if( m_pSuspObj )
    {
      CDrawObject* sObj = m_pSuspObj;
      m_pSuspObj = 0;
      if( !bCancel )
      {
        m_pUndoItem = new CUndoAddItem( this );
        EndCoedition( sObj );
        if( m_bSerialBuilding )
        {
          PostMessage( WM_COMMAND, IDM_CONTEXT_BEGIN + sObj->Class());
          CBuildMode::SetContext( 0, pObj, GetPosition());
        }
      }
    }
    else
    if( m_bSerialBuilding && !CBuildMode::HasContext())
    {
      PostMessage( WM_COMMAND, IDM_CONTEXT_BEGIN + pObj->Class());
      CBuildMode::SetContext( 0, pObj, GetPosition());
    }
    delete pObj;
  }

  m_pDelObj = 0;
}
//=====================================================================
void
CMapView::SaveReverseDirection( CDrawObject* pObj )
{
  static int n;
  static CTabIdent s, f;
  if( !pObj )
  {
    n = m_Conductor.GetLinkSize();
    s = m_Conductor.GetTable( 0 );
    f = m_Conductor.GetTable( 1 );
  }
  else
  if( n >= 2 &&
      m_Conductor.GetLinkSize() == n &&
      m_Conductor.GetTable( 0 ) == f &&
      m_Conductor.GetTable( 1 ) == s )
  {
    int n;
    CPoint* pp = pObj->GetPolyline( &n );
    for( int i = 0, j = n ; i < j ; )
    {
      CPoint t( pp[i] );
      pp[i++] = pp[--j];
      pp[j] = t;
    }
  }
}
//=====================================================================
UpdateError
CMapView::UpdateObject( CTabIdent id, CStringArray& par, int bShow )
{
  CDrawObject* ptr = 0;
  CDrawObject* pObj = GetObjectPtr( id );
  if( !pObj )
    ptr = pObj = ExtractMapObj( id );

  if( !pObj )
    return UE_OBJECT;

  m_Conductor.SetStatus( m_Conductor.GetIdent().Status() & 0x7f | pObj->Status() & 0x80 );

  for( int i = 0, np = par.GetSize() & ~1 ; i < np ; i += 2 )
  {
    par[i].MakeLower();
    m_Conductor.AddParam( par[i], par[i+1] );
  }

  return SetObjParam( pObj, bShow && ptr == 0 );
}
//=====================================================================
int
CMapView::OffsetObject( CTabIdent id, CSize off )
{
  CSmartObjPtr ptr;
  CDrawObject* pObj = GetObjectPtr( id );
  if( !pObj )
    ptr = pObj = ExtractMapObj( id );

  if( !pObj )
    return 0;

        ShowObjectBox( pObj );
        pObj->MoveOn( off );
        ShowObjectBox( pObj );

  return 1;
}
//=====================================================================
int
CMapView::FindObjectCoord( CTabIdent id, CString& dst )
{
  CSmartObjPtr ptr = 0;
  CDrawObject* pObj = GetObjectPtr( id );
  if( !pObj )
    ptr = pObj = ExtractMapObj( id );

  if( !pObj )
    return 0;

  dst = CSelectTraceMode::GetCoordList( pObj );
  return 1;
}
//=====================================================================
int
CMapView::GetObjectCoord( CTabIdent id, int bYcoord )
{
  CSmartObjPtr ptr = 0;
  CDrawObject* pObj = GetObjectPtr( id );
  if( !pObj )
    ptr = pObj = ExtractMapObj( id );

  if( !pObj )
    return -1;

	CPoint* pPoint = pObj->GetAnyPoints();
  if( !pPoint )
    return -1;

  return bYcoord ? pPoint->y : pPoint->x;
}
//=====================================================================
int
CMapView::CalcShowScale( CDrawObject* pObj, int bFull )
{
  int t = pObj->Class();
  int ns = App.GetProfileInt( "General", "FindScaleFactor", 2 );

  CPoint tp( GetCenter());
  int mas = m_pObjDef->GetMaxLimit( t ) / ns;
  int mis = m_pObjDef->GetMinLimit( t );
  int s = MerkatorData.TrueScale( m_AltScale, tp );
  if( mas )
    s = min( s, mas );
  if( mis )
    s = max( s, mis );

  if( bFull )
  {
    int f = MerkatorData.TrueScale( 2 * WindowSize2Scale( pObj->GetBoundBox().Size()), tp );
    if( !mas || f <= mas )
      s = max( f, s );
  }

  return MerkatorData.LogScale( s, tp );
}
//=====================================================================
void
CMapView::SlowMove( CPoint cn, int s )
{
  int fs = s;
  if( m_bSlow )
  {
    m_bFinding = 1;
    CPoint c0( m_Clip.CenterPoint());
    int n = 1;
    if( //s - GetTrueScale() ||
        abs( cn.x - c0.x ) > m_Clip.Width() ||
        abs( cn.y - c0.y ) > m_Clip.Height())
    {
      CRect fllClip( min( c0.x, cn.x ), min( c0.y, cn.y ), max( c0.x, cn.x ), max( c0.y, cn.y ));
      CPoint cf( fllClip.CenterPoint());
      int cw = min( m_Clip.Width(),  fllClip.Width())  / 2;
      int ch = min( m_Clip.Height(), fllClip.Height()) / 2;
      fllClip.InflateRect( cw, ch );
      fs = WindowSize2Scale( fllClip.Size());
      fs = min( MinScale / 2, fs );

      int tw = MulDiv( m_Clip.Width(),  fs, m_AltScale );
      int th = MulDiv( m_Clip.Height(), fs, m_AltScale );

      if( !tw ||!th )
        n = 0;
      else
      {
        CSize ws( fllClip.Size());
        n = max( 1, max ( ws.cx / tw, ws.cy / th ));
        SetScale( fs, c0, 1 );
        UpdateWindow();
      }
    }
      if( n )
      {
      double m = 1./ n, m1 = 0, m2;
      while( --n >= 0 )
      {
        m1 += m;
        m2 = 1. - m1;
        CPoint c( cn.x * m1 + c0.x * m2, cn.y * m1 + c0.y * m2 );
        SetScale( fs, c, 1 );
        UpdateWindow();
      }
    }
    m_bFinding = 0;
  }
  SetScale( s, cn, 1 );
  UpdateWindow();
}
//=====================================================================
int
CMapView::FindObject( CTabIdent id, int bClearSelection, int bShowFull )
{
  int bClick = m_pSTM || m_bSpaceSelection && CSpaceQueDialog::GetSelect();
  if( bClearSelection && !bClick )
    ClearSelection( 0, 1 );
  else
    UnselObj();

  UpdateWindow();

  CDrawObject* pObl = GetObjectPtr( id );
  CDrawObject* pObj = pObl ? pObl : ExtractMapObj( id );

  if( !pObj || pObj->IsSublayerStatic())
  {
#ifndef _FULL_PROJECT
    CString m;
    m.Format( "Объект не найден!\nТаблица %d, Объект %d", id.Table(), id.Entry());
    InfBox( m );
#endif // _FULL_PROJECT
    return 0;
  }

  int np, tp;
  CPoint* pPnt = pObj->GetAnyPoints( &tp, &np );
  CRect box( pObj->GetBoundBox());
  CPoint cn( pPnt && tp == SH_POINT ? *pPnt : box.CenterPoint());
  int s = CalcShowScale( pObj, bShowFull );

  CRect r( m_Clip );
  r.DeflateRect( m_Clip.Width() / 16, m_Clip.Height() / 16 );
  if( !r.PtInRect( cn ) || abs( s - m_AltScale ) > 1 )
    SlowMove( cn, s );

  if( !pObl )
    delete pObj;

  pObj = GetObjectPtr( id );

  if( !pObj )
  {
    pObj = ExtractMapObj( id );
    m_ClipData.AddObject( pObj );
  }

  CShortTraceMode* pSTM = (CShortTraceMode*)GetMode( IDM_SHORTTRACE_MODE );
  if( pSTM )
    return pSTM->ExtSelect( pObj );

  if( bClick )
    StartBlinking( pObj );
  else
    SelectObject( pObj, 1 );

  ShowSelectedNum();

  return 1;
}
//=====================================================================
void
CMapView::OnExtFind()
{
  int m = m_Conductor.GetFindMode();
  if( m >= 0 && m <= 3 )
    FindObject( m_Conductor.GetFindId(), m & 1, m & 2 );
  else
  {
    CObjList lst;
    if( m_pSTM )
      UnselObj();
    else
    if( !m_bSpaceSelection )
      ClearSelection( 0, 1 );

    int s = 0;
    CDrawObject* pSbj = 0;
    for( int i = m_Conductor.GetLinkSize() ; --i >= 0 ; )
    {
      CTabIdent id( m_Conductor.GetTable( i ));
      CDrawObject* pObl = GetObjectPtr( id );
      CDrawObject* pObj = pObl ? pObl : ExtractMapObj( id );
      if( !pObj || pObj->IsSublayer())
        continue;

      if( !pObl )
        m_ClipData.AddObject( pObj );

      if( !lst.Find( pObj ))
      {
        lst.AddHead( pSbj = pObj );
        int ns = CalcShowScale( pObj, 0 );
        s = max( s, ns );
      }
    }

    UpdateWindow();

    if( pSbj )
    {
      int np, tp;
      CPoint* pPnt = pSbj->GetAnyPoints( &tp, &np );
      CRect box( pSbj->GetBoundBox());
      CPoint cn( pPnt && tp == SH_POLYLINE ? *pPnt : box.CenterPoint());

      SlowMove( cn, s );
      CDrawObject* pObj = GetObjectPtr( pSbj->TabIdent());
      if( pObj )
      {
        SelectObject( pObj, 1 );
        if( m_pSTM && m_pSTM->InTrace( pObj ) >= 0 || m_bSpaceSelection && pObj->IsSelected())
          StartBlinking( pObj );
      }
    }
  }
}
//========================================================================
void
CMapView::OnCheckObject( CTabIdent id )
{
  CDrawObject* pObl = GetObjectPtr( id );
  CDrawObject* pObj = pObl ? pObl : ExtractMapObj( id );
  if( !pObj )
    m_Conductor.Create( CTabIdent( 0, 0 ));
  else
  {
    m_Conductor.Create( pObj );
    if( !pObl )
      delete pObj;
  }

  m_Conductor.DeclareEvent( CE_CHECK_RESULT );
}
//========================================================================
int
CMapView::SelectObjectSpecial( CTabIdent id, int ndx, DiagItem* pItem,
                               int bClearSelection, int bShow )
{
  CLegend const* pLegend = GetLegend( ndx );
  if( !pLegend )
                pLegend = GetLegend( 0 );

  int ls = pLegend->GetSize();
        if( pItem && ls == 0 )
    return 0;

  CDrawObject* pObj = GetObjectPtr( id );
  if( !pObj )
  {
    pObj = ExtractMapObj( id );
    if( !pObj )
      return 0;

    m_ClipData.AddObject( pObj );
  }

  pObj->SetSpecSelection( pLegend, pItem );

  if( !m_pSTM && bClearSelection )
    ClearSelection( 0, 1 );

  int bSb = 0;

  if( m_pSTM )
  {
    UnselObj();
    if( m_pSTM->InTrace( pObj ) < 0 )
      SelectObject( pObj, 1, 1 );
    bSb = 1;
  }
  else
  {
    SelectObject( pObj, 1, 1 );
    if( m_SelectList.GetCount() > 1 )
      bSb = 1;
  }

  if( bShow )
  {
    int np, tp;
    CPoint* pPnt = pObj->GetAnyPoints( &tp, &np );
    int s = CalcShowScale( pObj, 0 );
    CPoint cn( pPnt && tp == SH_POLYLINE ? *pPnt : pObj->GetBoundBox().CenterPoint());

    SetScale( s, cn, 1 );
    UpdateWindow();

    if( bSb )
      StartBlinking( pObj );
  }

  ShowSelectedNum();

  return 1;
}
//========================================================================
int
CMapView::TestObjects( CDrawObject* pObj0, CDrawObject* pObj1, int bShow )
{
  int f = IF_NOTHING;
  int n0, sh0;
  CPoint* pPnt0 = pObj0->GetAnyPoints( &sh0, &n0 );
  int n1, sh1;
  CPoint* pPnt1 = pObj1->GetAnyPoints( &sh1, &n1 );

  if( pPnt0 && pPnt1 )
  {
    #define CMP( a, b ) ( a * 4 + b )
    CPoint crsp( -1, -1 );

    switch( CMP( sh0, sh1 ))
    {
      case CMP( SH_POINT, SH_POINT ):
        if( *pPnt0 == *pPnt1 )
          f = IF_CROSS01;
        break;
      case CMP( SH_POLYLINE, SH_POINT ):
        if( IsPointOnPolyline( *pPnt1, pPnt0, n0 ) >= 0 )
          f = IF_CROSS01;
        break;
      case CMP( SH_POLYGON, SH_POINT ):
        if( IsPointOnPolygon( *pPnt1, pPnt0, n0 ) >= 0 )
          f = IF_CROSS01;
        else
        if( IsPointInPolygon( *pPnt1, pPnt0, n0, 0 ))
          f = IF_INCLUDE01;
        break;
      case CMP( SH_POINT, SH_POLYLINE ):
        if( IsPointOnPolyline( *pPnt0, pPnt1, n1 ) >= 0 )
          f = IF_CROSS01;
        break;
      case CMP( SH_POLYLINE, SH_POLYLINE ):
      {
        if( IsPolylineMatchPolyline( pPnt0, n0, pPnt1, n1, &crsp )  ||
            IsPolylineCrossPolyline( pPnt0, n0, pPnt1, n1, &crsp )  ||
            IsPointOnPolyline( crsp = pPnt0[0], pPnt1, n1 )    >= 0 ||
            IsPointOnPolyline( crsp = pPnt0[n0-1], pPnt1, n1 ) >= 0 ||
            IsPointOnPolyline( crsp = pPnt1[0], pPnt0, n0 )    >= 0 ||
            IsPointOnPolyline( crsp = pPnt1[n1-1], pPnt0, n0 ) >= 0 )
          f = IF_CROSS01;
        break;
      }
      case CMP( SH_POLYGON, SH_POLYLINE ):
        if( IsPolylineCrossPolygon( pPnt1, n1, pPnt0, n0 ))
          f = IF_CROSS01;
        else
        if( IsAllPointsInPolygon( pPnt1, n1, pPnt0, n0 ))
          f = IF_INCLUDE01;
        break;
      case CMP( SH_POINT, SH_POLYGON ):
        if( IsPointOnPolygon( *pPnt0, pPnt1, n1 ) >= 0 )
          f = IF_CROSS01;
        else
        if( IsPointInPolygon( *pPnt0, pPnt1, n1, 0 ))
          f = IF_INCLUDE10;
        break;
      case CMP( SH_POLYLINE, SH_POLYGON ):
        if( IsPolylineCrossPolygon( pPnt0, n0, pPnt1, n1 ))
          f = IF_CROSS01;
        else
        if( IsAllPointsInPolygon( pPnt0, n0, pPnt1, n1 ))
          f = IF_INCLUDE10;
        break;
      case CMP( SH_POLYGON, SH_POLYGON ):
        if( IsAllPointsInPolygon( pPnt0, n0, pPnt1, n1 ))
          f = IF_INCLUDE10;
        else
        if( IsAllPointsInPolygon( pPnt1, n1, pPnt0, n0 ))
          f = IF_INCLUDE01;
        else
        if( IsPolygonCrossPolygon( pPnt0, n0, pPnt1, n1 ))
          f = IF_CROSS01;
        break;
    }

    if( f != IF_NOTHING && bShow )
    {
      if( !m_pSTM )
      {
        ClearSelection( 0, 1 );
        SelectObject( pObj0, 1 );
        SelectObject( pObj1, 1 );
        UpdateWindow();
        ShowSelectedNum();
      }

      if( crsp.x < 0 && crsp.y < 0 )
      {
        int ns = App.GetProfileInt( "General", "FindScaleFactor", 2 );
        CBox box( pObj0->GetBoundBox() | pObj1->GetBoundBox());
        int s = WindowSize2Scale( box.Size()) * 2;
        SetScale( s, box.CenterPoint(), 1 );
      }
      else
        SetScale( m_AltScale, crsp, 1 );
    }
  }

  return f;
}
//=====================================================================
int
CMapView::TestObjects( CTabIdent id0, CTabIdent id1, int bShow )
{
  int f = IF_NOTHING;

  if( id0 == id1 )
    return f;

  CDrawObject* pObj0 = GetObjectPtr( id0 );
  if( !pObj0 )
  {
    pObj0 = ExtractMapObj( id0 );
    if( !pObj0 )
      return f;

    m_ClipData.AddObject( pObj0 );
  }

  CDrawObject* pObj1 = GetObjectPtr( id1 );
  if( !pObj1 )
  {
    pObj1 = ExtractMapObj( id1 );
    if( !pObj1 )
      return f;

    m_ClipData.AddObject( pObj1 );
  }

  return TestObjects( pObj0, pObj1, bShow );
}
//=====================================================================
int
CMapView::TestObjects( CTabIdArray const& tb0, CTabIdArray const& tb1, TGstream& drs )
{
  CTabIdArray dst;
  for( int n = 2 ; --n >= 0 ; )
  {
    CTabIdArray const& src = n ? tb1 : tb0;
    for( int i = src.GetSize() ; --i >= 0 ; )
    { 
      CTabIdent id(src[i]);
      if( !GetObjectPtr( id ))
        dst.Add( id );
    }
  }

  int nObj = dst.GetSize();
  if( nObj )
    for( ResultCode r = R_REPEAT ; r == R_REPEAT ; )
    {
      TGstream buf;
      r =  m_MapStore.FindObjects( dst.GetData(), &nObj, buf );
      LoadMap( &m_ClipData, buf.str(), nObj, m_AltScale, 1 );
    }

  nObj = 0;
  for( int i0 = tb0.GetSize() ; --i0 >= 0 ; )
  {
    CDrawObject* pObj0 = GetObjectPtr( tb0[i0] );
    if( !pObj0 )
      continue;
    for( int i1 = tb1.GetSize() ; --i1 >= 0 ; )
    {
      CDrawObject* pObj1 = GetObjectPtr( tb1[i1] );
      if( !pObj1 || pObj1 == pObj0 )
        continue;
      int fc = TestObjects( pObj0, pObj1, 0 );
      if( fc == IF_NOTHING )
        continue;
      drs.write((char*)&i0, sizeof i0 );
      drs.write((char*)&i1, sizeof i1 );
      drs.write((char*)&fc, sizeof fc  );
      ++nObj;
    }
  }

  return nObj;
}
//=====================================================================
void
CMapView::OnJumpToSelect( UINT id )
{
  CDrawObject* pOldSelObj = m_pCurSelObj;
  CDrawObject* pNewSelObj;

  UnselObj();

  if( pOldSelObj && !pOldSelObj->IsSelected())
    return;

  m_pCurSelObj = pOldSelObj;

  switch( id )
  {
    case IDM_SEL_JMP_HOME :
      pNewSelObj = m_SelectList.GetTail();
      break;
    case IDM_SEL_JMP_END  :
      pNewSelObj = m_SelectList.GetHead();
      break;
    case IDM_SEL_JMP_PREV :
      if( m_pCurSelObj && m_pCurSelObj != m_SelectList.GetTail())
        for( POSITION pos = m_SelectList.GetTailPosition() ; pos ; )
        {
          pNewSelObj = m_SelectList.GetPrev( pos );
          if( m_SelectList.GetAt( pos ) == m_pCurSelObj )
            break;
        }
      break;
    case IDM_SEL_JMP_NEXT :
      if( m_pCurSelObj && m_pCurSelObj != m_SelectList.GetHead())
        for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
        {
          pNewSelObj = m_SelectList.GetNext( pos );
          if( m_SelectList.GetAt( pos ) == m_pCurSelObj )
            break;
        }
      break;
  }

  if( pNewSelObj == m_pCurSelObj )
    Beep( 440, 400 );
  else
  {
    SetScale( CalcShowScale( pNewSelObj ));

    CRect b( LP2DP( pNewSelObj->GetBoundBox()));

    CSize ds( b.left < 0 ? b.left :
              b.right > m_ViewportSize.cx ? b.right - m_ViewportSize.cx : 0,
              b.top < 0 ? b.top :
              b.bottom > m_ViewportSize.cy ? b.bottom - m_ViewportSize.cy : 0 );

    if( ds.cx || ds.cy )
    {
      CSize cs( m_ViewportSize.cx / 8, m_ViewportSize.cy / 8 );
      ds.cx += ds.cx < 0 ? -cs.cx : ds.cx > 0 ? cs.cx : 0;
      ds.cy += ds.cy < 0 ? -cs.cy : ds.cy > 0 ? cs.cy : 0;
      m_Undo.AddItem( new CUndoScaleItem( this ), this );
      ds = DP2LP( ds );
      SetScale( m_AltScale, GetCenter() + ds, 1 );
    }

    UpdateWindow();
    StartBlinking( pNewSelObj );
  }
}
//=====================================================================
void
CMapView::OnUpdateJumpToSelect( CCmdUI* pCmdUI )
{
  int bEnb = m_SelectList.GetCount() > 1;
  if( bEnb )
    switch( pCmdUI->m_nID )
    {
      case IDM_SEL_JMP_PREV :
        bEnb &= m_pCurSelObj != 0;
      case IDM_SEL_JMP_HOME :
        bEnb &= m_pCurSelObj != m_SelectList.GetTail();
        break;
      case IDM_SEL_JMP_NEXT :
        bEnb &= m_pCurSelObj != 0;
      case IDM_SEL_JMP_END  :
        bEnb &= m_pCurSelObj != m_SelectList.GetHead();
        break;
    }
  pCmdUI->Enable( bEnb );
}
//=====================================================================
int
CMapView::GetLinkedGlobal( CDrawObject* pObj, int bTgt, CObjList* pList )
{
  static char* pn[] =
  {
    "get-host-class-list",
    "get-target-class-list",
    "get-host-class-list-ex"
  };

  long ClassArr[CLASS_RANGE];
  int nClass = 0;

  ksi_obj ksi_list = KsiCall( pn[bTgt], KSI_MK_SINT( pObj->Class()), 0 );
  while( KSI_PLIST_P( ksi_list ))
  {
    int nType = ksi_num2long( KSI_CAR( ksi_list ));
    ClassArr[nClass++] = nType;
    ksi_list = KSI_CDR( ksi_list );
  }

  if( !nClass )
    return 0;

  CGraphData tmpData;
  LoadFromStore( &tmpData, pObj->GetOriginBox(), ClassArr, nClass );

  CObjList tmpLst[3];

  while( --nClass >= 0 )
  {
    CLayer* pLayer = tmpData.GetLayerByClass( ClassArr[nClass] );
    for( CDrawObject* pTst = pLayer->SetFirst() ; pTst = pLayer->FindNext() ; )
      if( pTst->Ident() != pObj->Ident())
      {
        LINK_TYPE lt = pTst->IsLinkedTo( pObj );
        if( lt !=LT_NOTHING )
          if( bTgt == 1 ? pObj->IsHostLink( pTst, lt ) : pTst->IsHostLink( pObj, lt ))
            tmpLst[lt-1].AddHead( pTst );
      }
  }

  int nObj = 0;

  for( int i = CELEM( tmpLst ) ; --i >= 0 ; )
  {
    nObj += tmpLst[i].GetCount();
    while( !tmpLst[i].IsEmpty())
    {
      CDrawObject* pOb1 = tmpLst[i].RemoveHead();
      CDrawObject* pOb2 = m_ClipData.GetObjectPtr( pOb1->Ident());
      if( !pOb2 )
        m_ClipData.AddObject( pOb2 = tmpData.RemoveObject( pOb1 ));
      if( pList )
        pList->AddHead( pOb2 );
    }
  }

  return nObj;
}
//=====================================================================
int
CMapView::GetHostIds( CDrawObject* pObj, CTabIdList* pList )
{
  long ClassArr[CLASS_RANGE];
  int nClass = 0;

  ksi_obj ksi_list = KsiCall( "get-host-class-list", KSI_MK_SINT( pObj->Class()), 0 );
  while( KSI_PLIST_P( ksi_list ))
  {
    ClassArr[nClass++] = ksi_num2long( KSI_CAR( ksi_list ));
    ksi_list = KSI_CDR( ksi_list );
  }

  if( !nClass )
    return 0;

  CGraphData* pData = &m_ClipData;
  CGraphData tmpData;

  int bGlobal = !m_Clip.PtInRect( pObj->GetOriginBox().TopLeft()) ||
                !m_Clip.PtInRect( pObj->GetOriginBox().BottomRight());
  if( bGlobal )
    LoadFromStore( pData = &tmpData, pObj->GetOriginBox(), ClassArr, nClass );

  CObjList tmpLst[3];

  while( --nClass >= 0 )
  {
    CLayer* pLayer = pData->GetLayerByClass( ClassArr[nClass] );
    for( CDrawObject* pTst = pLayer->SetFirst() ; pTst = pLayer->FindNext() ; )
      if( pTst->Ident() != pObj->Ident())
      {
        LINK_TYPE lt = pTst->IsLinkedTo( pObj );
        if( pTst->IsHostLink( pObj, lt ))
          tmpLst[lt-1].AddHead( pTst );
      }
  }

  int n = 0;
  for( int i = CELEM( tmpLst ) ; --i >= 0 ; )
  {
    n += tmpLst[i].GetCount();
    while( !tmpLst[i].IsEmpty())
    {
      CDrawObject* pObj = tmpLst[i].RemoveHead();
      if( pList )
        pList->AddHead( pObj->TabIdent());
    }
  }
  return n;
}
//=====================================================================
inline  int
PTPL( SHAPE_TYPE t )
{
  return t == SH_POINT || t == SH_POLYLINE;
}
//=====================================================================
inline  int
PTPL( CDrawObject* pObj )
{
  return PTPL( pObj->GetShapeType());
}
//=====================================================================
int
CMapView::IsCopyingAllowable()
{ // returns class of unlinked object or 0
  for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj0 = m_SelectList.GetNext( pos );
    ksi_obj tcl = KSI_MK_SINT( pObj0->Class());
    ksi_obj cls = KsiCall( "get-host-class-list", tcl, 0 );
    ksi_obj hcls = ksi_nil;
    while( KSI_PLIST_P( cls ))
    {
      ksi_obj hcl = KSI_CAR( cls );
      cls = KSI_CDR( cls );
      ksi_obj tls = KsiCall( "get-target-class-list", hcl, 0 );
      if( KSI_FALSE_P( ksi_memv( tcl, tls )))
        continue;
      int t = ksi_num2int( hcl );
      if( PTPL( CDrawObject::GetShapeType( t )))
        hcls = ksi_cons( hcl, hcls );
    }

    if( KSI_NULL_P( hcls ))
      continue;

    CObjList hstl;
    for( int n = GetHostsGlobal( pObj0, &hstl ) ; --n >= 0 ; )
    {
      CDrawObject* pObj = hstl.RemoveHead();
      if( KSI_TRUE_P( ksi_memq( KSI_MK_SINT( pObj->Class()), hcls )) &&
         !pObj->IsSelected())
        return pObj0->Class();
    }
  }

  return 0;
}
//=====================================================================
int
CMapView::IsEditionAllowable( int bDel )
{ // returns class of unlinked object or 0
  CObjList lst;
  for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = m_SelectList.GetNext( pos );

    if( PTPL( pObj ))
      for( int n = GetTargetsGlobal( pObj, &lst ) ; --n >= 0 ; )
      {
        CDrawObject* pTst = lst.RemoveHead();
        if( !pTst->IsSelected())
        {
          LINK_TYPE t = pObj->IsLinkedTo( pTst );
          if( t && pObj->IsHostLink( pTst, t ))
            return pTst->Class();
        }
      }
    if( !bDel )
      for( int n = GetHostsGlobal( pObj, &lst ) ; --n >= 0 ; )
      {
        CDrawObject* pTst = lst.RemoveHead();
        if( !pTst->IsSelected() && PTPL( pTst ))
        {
          LINK_TYPE t = pTst->IsLinkedTo( pObj );
          if( t == LT_BEGIN || t == LT_END )
            return pTst->Class();
          if( t == LT_UNDEF && pTst->IsHostLink( pObj, t ))
            return pTst->Class();
        }
      }
  }
  return 0;
}
//=====================================================================
CDrawObject*
CMapView::GetSelectObject( int bMessage )
{
  if( m_SelectList.GetCount() != 1 && bMessage )
    ErrBox( "Для выполнения выбранной Вами операции\n"
            "необходимо выделить один объект" );
  return m_SelectList.GetCount() == 1 ? m_SelectList.GetHead() : 0;
}
//=====================================================================
static char proh[] = "Операция запрещена!";
//=====================================================================
CBuildMode*
CMapView::GetBuilding( int bMess )
{
  CBuildMode* pBM = (CBuildMode*)GetMode( IDM_CLASS_BEGIN, IDM_CLASS_END + SUBLR_RANGE );
  if( pBM && bMess )
  {
    CString m( pBM->IsEdition() ? "Редактирование" : "Построение" );
    m += " не завершено!\nДля отмены нажмите Esc.";
    ErrBox( m );
  }
  return pBM;
}
//=========================================================================
void
CMapView::OnUpdateNoEditCommand( CCmdUI* pCmdUI )
{
  pCmdUI->Enable( !GetBuilding( 0 ));
}
//=========================================================================
#ifdef _FULL_PROJECT
//=========================================================================
void
CMapView::OnSpecialCommand( UINT cmd )
{
  if( SublDlgIsActive())
    return;

  if( m_pSTM && !m_pSTM->AskForClose( ST_MODE( 0 )))
    return;

  UINT f = 0;
  CPoint p;
  if( m_pSTM )
    if( m_pSTM->OnRButtonDown( f, p ) & AM_RREMOVE )
      RemoveMode( m_pSTM );
    else
      return;

  CDrawObject* pObj = GetSelectObject( 0 );
  if( pObj )
  {
    m_Conductor.Create( pObj );
    m_Conductor.SetSelTable( pObj->TabIdent() );
  }
  else
    m_Conductor.RemoveAll();

    if( m_Conductor.DeclareEvent( CE_SPECIAL_COMMAND + cmd - IDM_SPECIAL_BEGIN ))
    ErrBox( proh );

  m_Conductor.GetUserRights( 1 );
  m_bRedraw = 1;
  InvalidateRect( 0 );
}
//=========================================================================
void
CMapView::OnDocCommand( UINT cmd )
{
  if( cmd == IDM_VIEWDOCUMENT )
    if( m_pSTM )
      m_Conductor.Create( CSelectTraceMode::GetTraceIdent());
    else
    {
      m_Conductor.SetSelTable( CTabIdent(0,0) );
      CDrawObject* pObj = GetSelectObject( 1 );
      if( pObj )
      {
        m_Conductor.Create( pObj );
        m_Conductor.SetSelTable( pObj->TabIdent() );
      }
      else
        return;
    }

  if( cmd == IDM_CALCCANAL && m_pSTM )
    if( m_pSTM->AskForClose( ST_MODE( 0 )))
      RemoveMode( m_pSTM );
    else
      return;

  if( m_Conductor.DeclareEvent( CE_DOC_COMMAND + cmd - IDM_DOCCTL_BEGIN ))
    ErrBox( proh );
}
//=========================================================================
void
CMapView::OnFindCommand( UINT cmd )
{
  CDrawObject* pObj = GetSelectObject( 0 );
  if( pObj )
  {
    m_Conductor.Create( pObj );
    m_Conductor.SetSelTable( pObj->TabIdent() );
  }
  else
    m_Conductor.RemoveAll();

  if( m_Conductor.DeclareEvent( CE_FIND_COMMAND + cmd - IDM_FNDCTL_BEGIN ))
    ErrBox( proh );
}
//=====================================================================
#endif
//=========================================================================
int
CMapView::RemoveTempObj( int nCode )
{
  if( m_bCutIn )
  {
    ASSERT( m_pCutObj0 );
    ASSERT( m_pCutIns );

    if( m_pCutObj0 )
    {
      CTabIdent id( m_pCutObj0->Table(), -1 );
      CDrawObject* pTmpObj = GetObjectPtr( id );
      ASSERT( pTmpObj );
      if( pTmpObj )
        HideObject( pTmpObj->Ident(), 1 );
      delete AddObject( m_pCutObj0, 0 );
      ShowObjectBox(  m_pCutObj0 );
    }
    if( m_pCutIns )
      RemoveObject( m_pCutIns );
    m_pCutObj0 = m_pCutIns = 0;
    m_bCutIn = 0;
  }
  else
  if( m_bCutOut )
  {
    ASSERT( m_pCutObj0 );
    ASSERT( m_pCutObj1 );
    ASSERT( m_pCutIns );

    if( m_pCutObj0 )
      delete AddObject( m_pCutObj0, 0 );

    if( m_pCutObj1 )
      delete AddObject( m_pCutObj1, 1 );

    if( m_pCutIns )
      delete AddObject( m_pCutIns,  1 );

    m_pCutObj0 = m_pCutObj1 = m_pCutIns = 0;
    m_bCutOut = 0;
  }

  CTabIdent id( m_Conductor.GetTable().Table(), -1 );
  CDrawObject* pObj = GetObjectPtr( id );
  if( !pObj )
    return 0;

  HideObject( pObj->Ident(), 1 );
  RedrawFuncLink( id );

  return 1;
}
//=====================================================================
int
CMapView::UpdateTemporaryObject( int ObjCls, int TabID, int ObjID )
{
  CDrawObject* pObj = GetObjectPtr( CTabIdent( TabID, -1 ));
  int r = pObj != 0;
  if( r )
  {
    CIdent id( pObj->Ident());
    HideObject( id, 0 );

    id.SetTable( TabID );
    id.SetEntry( abs( ObjID ));
    id.SetClass( ObjCls );

    int f = pObj->SelectFlag();
    SelectObject( pObj, 0 );
    CDrawObject* pOld = pObj;
    pObj = pOld->Rebuild( m_Conductor.CreateParamList(), id.Class(), id.Status());
    delete pOld;
    pObj->SetIdent( id );
    CObjList lst;
    lst.AddHead( pObj );
    SaveToStore( lst, !m_pCutObj1 || !m_pCutObj0, 0 );
    SelectObject( pObj, f );
    m_ClipData.AddObject( pObj );
  }

  delete m_pCutObj0;
  delete m_pCutObj1;
  if( m_bCutOut )
    delete m_pCutIns;
  m_pCutIns = m_pCutObj1 = m_pCutObj0 = 0;
  m_bCutIn = m_bCutOut = 0;

  TestCutIn( pObj );

  return r;
}
//=========================================================================
void
CMapView::OnRemCutInCmd()
{
  static char em[] = "Удаление выделенного объекта в качестве врезки некорректно!";
  CDrawObject* pIns = GetSelectObject( 1 );
  if( !pIns )
    return;

  CObjList lst;

  GetTargetsGlobal( pIns, &lst );
  for( POSITION pos = lst.GetHeadPosition() ; pos ; lst.GetNext( pos ))
    if( lst.GetAt( pos )->GetShapeType() != SH_POLYLINE )
      lst.RemoveAt( pos );

  if( lst.GetCount() != 2 )
  {
    ErrBox( em );
    return;
  }

  CDrawObject* pTgt[2] = { lst.GetHead(), lst.GetTail() };
  CDrawObject* pHst[2] = { 0, 0 };

  for( int i = 2 ; --i >= 0 ; )
  {
    lst.RemoveAll();
    GetHostsGlobal( pTgt[i], &lst );
    for( POSITION pos = lst.GetHeadPosition() ; pos ; lst.GetNext( pos ))
      if( lst.GetAt( pos )->GetShapeType() != SH_POINT )
        lst.RemoveAt( pos );

    if( lst.GetCount() != 2 )
    {
      ErrBox( em );
      return;
    }

    pHst[i] = lst.GetHead()->Ident() == pIns->Ident() ? lst.GetTail() : lst.GetHead();
  }

  int bSlClient = GetProjectSLEnable();

  if( bSlClient && AskBox( "Муфта будет удалена.\nПродолжить?" ) != IDYES )
    return;

  CObjList AddList, RemList;

  CUndoAddItem* pItem = 0;

  if( !bSlClient )
  {
    RemList.AddHead( pTgt[0] );
    RemList.AddHead( pTgt[1] );
    pItem = new CUndoAddItem( this, 0, &RemList );
  }

  int n0;
  CPoint* p0 = pTgt[0]->GetPolyline( &n0 );
  int n1;
  CPoint* p1 = pTgt[1]->GetPolyline( &n1 );
  int nl = n0 + n1 - 1;
  CPoint* pl = new CPoint[nl];
  if( p0[n0-1] == p1[0] )
  {
    memcpy( pl, p0, n0 * sizeof CPoint );
    memcpy( pl + n0 - 1, p1, n1 * sizeof CPoint );
  }
  else
  if( p1[n1-1] == p0[0] )
  {
    memcpy( pl, p1, n1 * sizeof CPoint );
    memcpy( pl + n1 - 1, p0, n0 * sizeof CPoint );

    CDrawObject* pTmp;
    pTmp = pTgt[1];
    pTgt[1] = pTgt[0];
    pTgt[0] = pTmp;
    pTmp = pHst[1];
    pHst[1] = pHst[0];
    pHst[0] = pTmp;
  }
  else
  if( p0[n0-1] == p1[n1-1] )
  {
    memcpy( pl, p0, n0 * sizeof CPoint );
    memcpy( pl + n0 - 1, p1, n1 * sizeof CPoint );
    ReversePoly( pl + n0 - 1, n1 );
  }
  else
  if( p0[0] == p1[0] )
  {
    memcpy( pl, p0, n0 * sizeof CPoint );
    ReversePoly( pl, n0 );
    memcpy( pl + n0 - 1, p1, n1 * sizeof CPoint );
  }

  if( bSlClient )
  {
    m_pCutIns  = pIns->CopyObject();;
    m_pCutObj0 = pTgt[0]->CopyObject();
    m_pCutObj1 = pTgt[1]->CopyObject();

    m_Conductor.RemoveAll();
    m_Conductor.AddLink( pHst[0]->TabIdent());
    m_Conductor.AddLink( pTgt[0]->TabIdent());
    m_Conductor.AddLink( pIns->TabIdent());
    m_Conductor.AddLink( pTgt[1]->TabIdent());
    m_Conductor.AddLink( pHst[1]->TabIdent());
    m_Conductor.SetClass( pTgt[0]->Class());
    m_Conductor.DeclareEvent( CE_OBJ_CUT_OUT );
    CIdent id( m_Conductor.GetIdent());
    id.SetStatus( pTgt[0]->Status() & 0x7f | ( IsHiResolution() ? 0x80 : 0 ));
    RemoveObject( pTgt[1] );
    RemoveObject( pIns );
    m_ClipData.RemoveObject( pTgt[0] );
    pTgt[0]->SetIdent( id );
    pTgt[0]->SetPoly( pl, nl );
    m_ClipData.AddObject( pTgt[0] );
    m_bCutOut = 1;
  }
  else
  {
    m_Conductor.Create( pTgt[1] );

    if( m_Conductor.DeclareEvent( CE_OBJ_DELETE ))
    {
      delete pItem;
      delete pl;
      return;
    }

    CDrawObject* pNew = pTgt[0]->CopyObject();
    pNew->SetPoly( pl, nl );

    m_Conductor.Create( pNew );
    if( m_Conductor.DeclareEvent( CE_OBJ_PARAM  ))
    {
      delete pItem;
      delete pl;
      delete pNew;
      return;
    }

    SetObjParam( pNew, 1 );
    delete pNew;
    pNew = GetObjectPtr( m_Conductor.GetTable());
    ASSERT( pNew );
    AddList.AddHead( pNew );
    pItem->SetList( &AddList, 1 );
    m_Undo.AddItem( pItem, this );
    RemoveObject( pTgt[1] );
  }

  delete pl;
}
//=========================================================================
void
CMapView::MakeCutIn( CDrawObject* pIns, CDrawObject* pCut, CObjList& lst )
{
  int bCancel = 0;
  int bSlClient = GetProjectSLEnable();

  int nl, ns;
  CPoint* pp = pIns->GetPoint();
  CPoint* pl = pCut->GetPolyline( &nl );
  ns = IsPointOnPolyline( *pp, pl, nl, 50 );
  CDrawObject* pObp[2] = { 0, 0 };

  CPoint mp( *pp );
  for( int i = 2 ; !bCancel && --i >= 0 ; )
  {
    CDrawObject* pObj = pCut->CopyObject();
    ksi_obj env = ksi_mk_txt_env( pObj, pl, nl, &mp, ns, i );
    CDrawObject* pTmp = pObj;
    pObj = pTmp->Rebuild( env );
    delete pTmp;

    m_Conductor.Create( pObj );
    bCancel = !bSlClient && m_Conductor.DeclareEvent( i ? CE_OBJ_PARAM : CE_OBJ_COPY );

    if( !bCancel )
    {
      CIdent id( m_Conductor.GetIdent());
      if( !bSlClient && i == 0 && pCut->TabIdent() == id.TabIdent())
      {
        id.SetTable( 0 );
        id.SetEntry( /*bSlClient ? 0 :*/ GetUniId());
      }
      id.SetStatus( id.Status() & 0x7f | ( IsHiResolution() ? 0x80 : 0 ));
      pObj->SetIdent( id );
      pObp[i] = pObj->Rebuild( m_Conductor.CreateParamList(), id.Class(), id.Status());
    }

    delete pObj;
  }

  if( bSlClient )
  {
    m_Conductor.RemoveAll();
    CPoint* p0 = pObp[1]->GetAnyPoints();
    CPoint* p1 = lst.GetHead()->GetPoint();
    if( *p0 != *p1 )
      lst.AddHead( lst.RemoveTail());
    m_Conductor.AddLink( lst.GetHead()->TabIdent());
    m_Conductor.AddLink( pObp[1]->TabIdent());
    m_Conductor.AddLink( pIns->TabIdent());
    m_Conductor.AddLink( pObp[0]->TabIdent());
    m_Conductor.AddLink( lst.GetTail()->TabIdent());
    m_Conductor.SetClass( pCut->Class());
    m_Conductor.DeclareEvent( CE_OBJ_CUT_IN );
    CIdent id( m_Conductor.GetIdent());
    if( id.Entry() == -1 )
      RemoveTempObj();
    id.SetStatus( pCut->Status() & 0x7f | ( IsHiResolution() ? 0x80 : 0 ));
    pObp[0]->SetIdent( id );

    m_pCutIns = pIns;
    m_pCutObj0 = pCut;
    AddObject( pObp[0], 1 );
    AddObject( pObp[1], 0 );
    m_bCutIn = 1;
  }
  else
  if( bCancel)
  {
    delete pObp[0];
    delete pObp[1];
  }
  else
  {
    CObjList AddList, RemList;
    RemList.AddHead( pCut );
    AddList.AddHead( pObp[0] );
    AddList.AddHead( pObp[1] );
    m_Undo.AddItem( new CUndoAddItem( this, &AddList, &RemList ), this );
    delete AddObject( pObp[0], 1 );
    delete AddObject( pObp[1], 0 );
  }
}
//=========================================================================
void
CMapView::TestCutIn( CDrawObject* pIns )
{
  CObjList lst;
  CDrawObject* pObj0 = 0;
  if( TestCutIn( pIns, pObj0, &lst, 0 ) &&
                  ( GetProjectSLEnable() || AskBox( "Сделать врезку?" ) == IDYES ))
  {
    ClearSelection();
    SelectObject( pIns, 1 );
    PostMessage( WM_COMMAND, IDM_CUT_IN_CMD );
  }
}
//=========================================================================
int
CMapView::TestCutIn( CDrawObject* pIns, CDrawObject*& pObj0, CObjList* pLst, int bMsg )
{
  if( !pIns )
    return 0;

  long ClassArr[CLASS_RANGE];
  int nClass = 0;

  ksi_obj ksi_list = KsiCall( "get-insertable-class-list", KSI_MK_SINT( pIns->Class()), 0 );
  while( KSI_PLIST_P( ksi_list ))
  {
    ClassArr[nClass++] = ksi_num2long( KSI_CAR( ksi_list ));
    ksi_list = KSI_CDR( ksi_list );
  }

  CPoint* pp = pIns->GetPoint();

  if( !nClass || !pp )
  {
    if( bMsg )
      ErrBox( "Для выделенного объекта врезка невозможна" );
    return 0;
  }

  CPoint* pl;
  int nl;

  CObjList TgtList;
  while( --nClass >= 0 )
  {
    CLayer* pLayer = m_ClipData.GetLayerByClass( ClassArr[nClass] );
    for( CDrawObject* pTst = pLayer->SetFirst() ; pTst = pLayer->FindNext() ; )
    {
      pl = pTst->GetPolyline( &nl );
      if( !pl )
        continue;
      int ns = IsPointOnPolyline( *pp, pl, nl, 10 );
      if( ns >= 0 && *pp != pl[0] && *pp != pl[nl-1] )
        TgtList.AddHead( pTst );
    }
  }

  if( TgtList.GetCount() == 0 || pObj0 && !TgtList.Find( pObj0 ))
  {
    if( bMsg )
      ErrBox( "Некорректное место для врезки!" );
    return 0;
  }

  if( !pObj0 && TgtList.GetCount() >= 2 )
  {
    if( bMsg )
      ErrBox( "Неоднозначное место для врезки!" );
    return 0;
  }

  pObj0 || ( pObj0 = TgtList.GetHead()); // line object

  GetHostsGlobal( pObj0, pLst );
  if( pLst->GetCount() != 2 )
  {
    if( bMsg )
      ErrBox( "Несвязанный линейный объект!" );
    return 0;
  }

  return 1;
}
//=========================================================================
void
CMapView::OnCutInCmd()
{
  int n = m_SelectList.GetCount();

  if( n == 0 || n > 2 )
  {
    ErrBox( "Для выполнения врезки необходимо выделить один или два объекта!" );
    return;
  }

  CDrawObject* pIns = m_SelectList.GetHead();
  if( !pIns->GetPoint() && n == 2 )
    pIns = m_SelectList.GetTail();

  if( !pIns->GetPoint())
  {
    ErrBox( "Для выполнения врезки необходимо выделить один точечный объект!" );
    return;
  }

  CDrawObject* pObj0 = 0;
  if( n == 2 )
  {
    pObj0 = pIns == m_SelectList.GetTail() ? m_SelectList.GetHead() : m_SelectList.GetTail();
    if( !pObj0->GetPolyline())
    {
      ErrBox( "Для выполнения врезки второй выделенный объект должен быть линенйным!" );
      return;
    }
  }

  CObjList lst;
  if( TestCutIn( pIns, pObj0, &lst, 1 ))
    MakeCutIn( pIns, pObj0, lst );
}
//=========================================================================
void
CMapView::KillUndoItem()
{
  if( !m_pUndoItem )
    return;

  CObjList* pList;
  if( pList = m_pUndoItem->GetList( 1 ))
    while( !pList->IsEmpty())
      delete pList->RemoveHead();
  if( pList = m_pUndoItem->GetList( 0 ))
    while( !pList->IsEmpty())
      delete pList->RemoveHead();
  delete m_pUndoItem;
  m_pUndoItem = 0;
}
//=========================================================================
int
CMapView::CreatePointObject()
{
  int t = m_Conductor.GetClass();
  if( !m_pObjDef->IsClass( t ))
    return 0;

  ksi_obj ksi_param = ksi_nil;
  static char* ipn[] = { "x0", "y0", "tx", "ty", "ta", 0 };
  int n = m_Conductor.GetParams().GetSize();
  for( int i = 0 ; i < n ; i += 2 )
  {
    CString nm = m_Conductor.GetParams()[i];
    CString vl = m_Conductor.GetParams()[i+1];
    char** pn;
    for( pn = ipn ; *pn && nm.CompareNoCase( *pn ) ; ++pn );
    if( *pn )
      ksi_param = ksi_cons( cons_with_sym( nm, atoi( vl )), ksi_param );
    else
      ksi_param = ksi_cons( cons_with_sym( nm, vl ), ksi_param );
  }

  ksi_obj ksi_val = KsiCall( "create-object", ksi_int2num( t ), ksi_zero, ksi_param, 0 );
  if( !KSI_PLIST_P( ksi_val ))
    return 0;

  CIdent id( m_Conductor.GetIdent());
  CDrawObject* pObj = new CDrawObject( ksi_val, id, 0 );
  TGstream str;
  int bNew = m_MapStore.FindObject( pObj->TabIdent(), str ) != R_OK;
  delete AddObject( pObj, bNew );
  return 1;
}
//=========================================================================
int
CMapView::CreateLineObject()
{
  if( m_Conductor.GetLinkSize() != 2 )
    return 0;

  CTabIdent id0 = m_Conductor.GetTable( 0 );
  CTabIdent id1 = m_Conductor.GetTable( 1 );

  if( id0 == id1 )
    return 0;

  CSmartObjPtr ptr0;
  CDrawObject *pObj0 = GetObjectPtr( id0 );
  if( !pObj0 )
    ptr0 = pObj0 = ExtractMapObj( id0 );

  CPoint* p0 = pObj0->GetPoint();

  if( !pObj0 || !p0 )
    return 0;

  CSmartObjPtr ptr1;
  CDrawObject *pObj1 = GetObjectPtr( id1 );
  if( !pObj1 )
    ptr1 = pObj1 = ExtractMapObj( id1 );

  CPoint* p1 = pObj1->GetPoint();

  if( !pObj1 || !p1 )
    return 0;

  CIdent id( m_Conductor.GetIdent());
  ksi_obj ksi_vars = ksi_nil;

  for( int i = m_Conductor.GetParamSize() ; --i >= 0 ; )
  {
    ksi_obj ksi_par = cons_with_sym( m_Conductor.GetParamName( i ), m_Conductor.GetParamValue( i ));
    ksi_vars = ksi_cons( ksi_par, ksi_vars );
  }

  CPoint pts[] = { *p0, *p1 };
  ksi_vars = ksi_cons( cons_with_sym( "apl", pts, 2 ), ksi_vars );

  CPoint cp( p0->x + p1->x >> 1, p0->y + p1->y >> 1 );
  int a = atan2( p0->x - p1->x, p1->y - p0->y );
  int ang = a > 900 ? a - 1800 : a < -900 ? a + 1800 : a;
  struct { char* Name; int Val; }
  m[] = { "tx", cp.x, "ty", cp.y, "ta", ang };

  for( int i = CELEM( m ) ; --i >= 0 ; )
    ksi_vars = ksi_cons( cons_with_sym( m[i].Name, m[i].Val ), ksi_vars );

  ksi_obj val =  KsiCall( "create-object", KSI_MK_SINT( id.Class()), id.Status(), ksi_vars, 0 );

  CDrawObject* pObj = new CDrawObject( val, id, GetConductor()->GetNode());
  if( !pObj )
    return 0;

  TGstream str;
  int bNew = m_MapStore.FindObject( id.TabIdent(), str ) != R_OK;

  delete AddObject( pObj, bNew );
  return 1;
}
//=========================================================================
