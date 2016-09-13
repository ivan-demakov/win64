#include "stdafx.h"

#include "dwin.h"
#include "mview.h"
#include "drawobj.h"
#include "cnctmode.h"
#include "sltrmode.h"
#include "conduct.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "util.h"
#include "joke.h"
#include "undo.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
static char const* m[] =
{
  "Выделите первый объект",
  "Выделите второй объект",
  "Нажмите \"+\" для сложения, \"-\" для вычитания выделенных объектов"
};
//=====================================================================
int
CConnectMode::Init( CMapView* pOwner )
{
  m_pOwner = pOwner;
  return !m_pOwner->m_pSTM;
}
//=====================================================================
int
CConnectMode::Term( int bDestroy )
{
  if( !bDestroy )
  {
    ClearHint();
    m_pOwner->ClearSelection();
    if( m_pOwner->m_pSTM )
      m_pOwner->m_pSTM->Reopen();
  }
  return 0;
}
//=====================================================================
int
CConnectMode::Open()
{
  m_pOwner->ClearSelection();
  ReportHint( m[0] );
  m_nMode = 1;
  return 1;
}
//=====================================================================
void
CConnectMode::Draw( CDC* pDC )
{
  ReportHint( m[m_nMode-1] );
}
//=====================================================================
int
CConnectMode::OnLButtonDown( UINT &flags, CPoint &loc )
{
  if( !( flags & MK_SHIFT ))
    return AM_RPROPAGATE;

  CDrawObject* pObj;
  ( pObj = m_pOwner->GetDetectedObject( 0 )) ||
  ( pObj = m_pOwner->GetDetectedObject( 1 ));

  if( pObj )
  {
    CObjList tl;
    tl.AddHead( pObj );
    if( pObj->IsSelected())
    {
      m_pOwner->SelectObject( pObj, 0 );
      m_pOwner->m_Undo.AddItem( new CUndoSelectItem( m_pOwner, 0, &tl ), m_pOwner );
      --m_nMode;
    }
    else
    if( m_nMode < 3 && ( m_pOwner->m_LocalLaw[pObj->Class()] & 1 ))
    {
      m_pOwner->SelectObject( pObj, 1 );
      m_pOwner->m_Undo.AddItem( new CUndoSelectItem( m_pOwner, &tl, 0 ), m_pOwner );
      ++m_nMode;
    }
  }
  return AM_RDONE;
}
//=====================================================================
int
CConnectMode::OnRButtonDown( UINT &flags, CPoint &loc )
{
  return AM_RDONE | AM_RREMOVE;
}
//=====================================================================
int
CConnectMode::OnChar( UINT& flags, UINT& key_code )
{
  if( m_nMode == 3 && ( key_code == '+' || key_code == '-' ))
  {
    CDrawObject* pObj0 = m_pOwner->m_SelectList.GetTail();
    CDrawObject* pObj1 = m_pOwner->m_SelectList.GetHead();
    if( pObj0->IsObjectTemp() || pObj1->IsObjectTemp())
    {
      Beep( 440, 400 );
      ErrBox( key_code == '+' ?
        "Попытка сложения с временным объектом" :
        "Попытка вычитания с временным объектом" );
      return AM_RDONE | AM_RREMOVE;
    }
  }

  switch( key_code )
  {
    case VK_ESCAPE :
      return AM_RDONE | AM_RREMOVE;
    case '+' :
      return m_nMode != 3 ? AM_RDONE : CreateUnion();
    case '-' :
      return m_nMode != 3 ? AM_RDONE : CreateSubtraction();
    case VK_BACK :
      if( m_nMode > 1 )
      {
        m_pOwner->SelectObject( m_pOwner->m_SelectList.GetHead(), 0 );
        --m_nMode;
      }
      return AM_RDONE;
  }
  return AM_RPROPAGATE;
}
//=====================================================================
int
CConnectMode::CreateUnion()
{
  static char err1[] = "Сложение объектов различных\nклассов запрещено!";
  static char err2[] = "Сложение выбранных объектов некорректно!";
  static char err3[] = "Вы не обладаете правами для изменения выбранных объектов!";

  CDrawObject* pObj0 = m_pOwner->m_SelectList.GetTail();
  CDrawObject* pObj1 = m_pOwner->m_SelectList.GetHead();

  if( pObj0->Class() != pObj1->Class())
  {
    ErrBox( err1 );
    return AM_RDONE;
  }

  int typ0, nPnt0;
  CPoint* pPnt0 = pObj0->GetAnyPoints( &typ0, &nPnt0 );
  int typ1, nPnt1;
  CPoint* pPnt1 = pObj1->GetAnyPoints( &typ1, &nPnt1 );

  if( typ0 != typ1 || typ0 != SH_POLYGON && typ0 != SH_POLYLINE )
  {
    ErrBox( err2 );
    return AM_RDONE;
  }

  CObjList l0, l1;
  m_pOwner->GetLinkedGlobal( pObj0, 2, &l0 );
  m_pOwner->GetLinkedGlobal( pObj1, 2, &l1 );

  if( l0.Find( pObj1 ) || l1.Find( pObj0 ))
  {
    ErrBox( err2 );
    return AM_RDONE;
  }

  while( !l0.IsEmpty())
    if( l1.Find( l0.RemoveHead()))
    {
      ErrBox( err2 );
      return AM_RDONE;
    }

  CConductor* pCnd = m_pOwner->GetConductor();
  pCnd->Create( pObj0 );
  int r0 = pCnd->GetUserRights();
  pCnd->Create( pObj1 );
  int r1 = pCnd->GetUserRights();
  if( r0 < UR_L_EDIT || r1 < UR_L_EDIT )
  {
    ErrBox( err3 );
    return AM_RDONE;
  }

  char const on[] = "Сложение";
  if( !m_pOwner->IsObjectInLegalRegion( pObj0, on, 1 ) ||
      !m_pOwner->IsObjectInLegalRegion( pObj1, on, 1 ))
    return AM_RDONE;

  CPoint* pNewPnt = 0;
  {
    CWaitCursor wc;
    ReportHint( "Пожалуйста, подождите..." );

    pNewPnt = typ0 == SH_POLYGON ? SmartConnectPolygons( pPnt0, nPnt0, pPnt1, nPnt1 ) :
                                   SmartConnectPolylines( pPnt0, nPnt0, pPnt1, nPnt1 );
    ClearHint();
  }
  if( !pNewPnt )
  {
    ErrBox( err2 );
    return AM_RDONE;
  }

  int bNoTabObj = !pObj0->Table() && !pObj1->Table();

  if( !bNoTabObj )
  {
    m_pOwner->m_Conductor.Create( pObj0 );
    m_pOwner->m_Conductor.Create( pObj0->TabIdent(), pObj1->TabIdent());
  }

  if( bNoTabObj || !m_pOwner->m_Conductor.DeclareEvent( CE_OBJ_JOIN ))
  {
    CDrawObject* pTmp = new CDrawObject( pObj0 );
    pTmp->SetPoly( pNewPnt + 1, pNewPnt->x );
    CDrawObject* pObj = pTmp->Rebuild( ksi_nil, pTmp->Class(), pTmp->Status());
    delete pTmp;
    delete pNewPnt;
    pObj->SetIdent( m_pOwner->m_Conductor.GetIdent());
    pObj->SetTabId( m_pOwner->m_Conductor.GetTable());
    CUndoAddItem* pItem = 0;
    if( !GetProjectSLEnable())
      pItem = new CUndoAddItem( m_pOwner, 0, &m_pOwner->m_SelectList );
    m_pOwner->RemoveObject( pObj1 );
    m_pOwner->RemoveObject( pObj0 );
    m_pOwner->AddObject( pObj, 1 );
    m_pOwner->SelectObject( pObj, 1 );
    if( pItem )
    {
      pItem->SetList( &m_pOwner->m_SelectList, 1 );
      m_pOwner->m_Undo.AddItem( pItem, m_pOwner );
    }
    m_nMode = 2;
  }
  else
  {
    delete pNewPnt;
    return AM_RDONE | AM_RREMOVE;
  }

  return AM_RDONE;
}
//=====================================================================
int
CConnectMode::CreateSubtraction()
{
  static char err0[] = "Вычитание выбранных объектов некорректно!";
  static char err1[] = "Вы не обладаете правами для изменения выбранного объекта!";
  CDrawObject* pObj0 = m_pOwner->m_SelectList.GetHead();
  CDrawObject* pObj1 = m_pOwner->m_SelectList.GetTail();

  int nPnt0;
  CPoint* pPnt0 = pObj0->GetPolygon( &nPnt0 );
  int nPnt1;
  CPoint* pPnt1 = pObj1->GetPolygon( &nPnt1 );

  if( !pPnt0 || !pPnt1 )
  {
    ErrBox( err0 );
    return AM_RDONE;
  }

  CConductor* pCnd = m_pOwner->GetConductor();
  pCnd->Create( pObj0 );
  int r0 = pCnd->GetUserRights();
  if( r0 < UR_L_EDIT )
  {
    ErrBox( err1 );
    return AM_RDONE;
  }

  char const on[] = "Вычитание";
  if( !m_pOwner->IsObjectInLegalRegion( pObj0, on, 1 ) ||
      !m_pOwner->IsObjectInLegalRegion( pObj1, on, 1 ))
    return AM_RDONE;

  CWaitCursor wc;
  ReportHint( "Пожалуйста, подождите..." );

  CPoint* pNewPnt = SmartSubtractPolygons( pPnt0, nPnt0, pPnt1, nPnt1 );
  if( !pNewPnt )
  {
    ErrBox( err0 );
    return AM_RDONE;
  }

  CDrawObject* pTmp = new CDrawObject( pObj0 );
  pTmp->SetPoly( pNewPnt + 1, pNewPnt->x );
  CDrawObject* pObj = pTmp->Rebuild( ksi_nil, pTmp->Class(), pTmp->Status());
  delete pTmp;
  delete pNewPnt;

  m_pOwner->SelectObject( pObj1, 0 );

  CUndoAddItem* pItem = 0;
  if( !GetProjectSLEnable())
    pItem = new CUndoAddItem( m_pOwner, 0, &m_pOwner->m_SelectList );

  m_pOwner->SelectObject( pObj0, 0 );
  m_pOwner->AddObject( pObj, 0 );
  m_pOwner->SelectObject( pObj, 1 );

  if( pItem )
  {
    pItem->SetList( &m_pOwner->m_SelectList, 1 );
    m_pOwner->m_Undo.AddItem( pItem, m_pOwner );
  }
  delete pObj0;
  m_nMode = 2;

  return AM_RDONE;
}
//=====================================================================
