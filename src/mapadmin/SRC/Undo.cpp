#include "stdafx.h"

#include "undo.h"
#include "ksiutil.h"
#include "ksi_type.h"
#include "mview.h"
#include "sltrmode.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//=====================================================================
long CUndo::m_UndoMaxLength = 40;
//=====================================================================
int 
CUndoScaleItem::FINL( CDrawObject* pObj, CObjList* pObjList )
{
  for( POSITION pos = pObjList->GetHeadPosition() ; pos ; )
    if( pObjList->GetNext( pos )->Ident() == pObj->Ident())
      return 1;
  return 0;
}
//=====================================================================
void 
CUndoScaleItem::CPYL( CObjList* pOutList, CObjList const* pInpList )
{
//  ASSERT( pOutList->IsEmpty());
  if( pInpList )
    for( POSITION pos = pInpList->GetHeadPosition() ; pos ; 
         pOutList->AddTail( pInpList->GetNext( pos )->CopyObject()));
}
//=====================================================================
void 
CUndoScaleItem::DELL( CObjList* pObjList )
{
  while( !pObjList->IsEmpty())
    delete pObjList->RemoveHead();
}
//=====================================================================
void
CUndoScaleItem::Set( CMapView* pView )
{
  m_Cntr  = pView->GetCenter();
  m_Scale = pView->m_OldScale;
}
//=====================================================================
int
CUndoScaleItem::Play( CMapView* pView )
{
  if( m_Scale != pView->GetAltScale() || m_Cntr != pView->GetCenter())
  {
    int sc = m_Scale;
    m_Scale = pView->GetAltScale();
    CPoint cn( m_Cntr );
	  m_Cntr = pView->GetCenter();
    pView->SetScale( sc, cn, 0 );
  }
  return 0;
}
//=====================================================================
//=====================================================================
int
CUndoMoveItem::Play( CMapView* pView )
{
  pView->ShowSelected( 0 );
  pView->MoveSelectOn( m_Offset = -m_Offset );
  pView->ShowSelected( 1, 1, 1 );
  return CUndoScaleItem::Play( pView );
}
//=====================================================================
//=====================================================================
int
CUndoSpinItem::Play( CMapView* pView )
{
  pView->ShowSelected( 0 );
  pView->SpinSelectOn( m_Center, m_Angle = -m_Angle );
  pView->ShowSelected( 1, 1, 1 );
  return CUndoScaleItem::Play( pView );
}
//=====================================================================
//=====================================================================
CUndoSelectItem::CUndoSelectItem( CMapView* pView, CObjList const* pPlusList, CObjList const* pMinusList ) :
  CUndoScaleItem( pView )
{
  CPYL( m_pPlusList  = &m_List0, pPlusList );
  CPYL( m_pMinusList = &m_List1, pMinusList );
}
//=====================================================================
void 
CUndoSelectItem::SetList( CObjList const* pList, int bPlus )
{
  CPYL( bPlus ? m_pPlusList : m_pMinusList, pList );
}
//=====================================================================
CUndoSelectItem::~CUndoSelectItem()
{
  DELL( &m_List0 );
  DELL( &m_List1 );
}
//=====================================================================
void 
CUndoSelectItem::SwapLists()
{
  CObjList* pTmp = m_pPlusList;
  m_pPlusList = m_pMinusList;
  m_pMinusList = pTmp;
}
//=====================================================================
int
CUndoSelectItem::Play( CMapView* pView )
{
  SwapLists();

  for( int i = 0 ; i < 2 ; ++i )
  {
    CObjList* pList = i == 0 ? m_pMinusList : m_pPlusList;
    for( POSITION pos = pList->GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = pList->GetNext( pos );
	    CIdent id( pObj->Ident());
		  if( pView->m_pSTM )
  		  pView->m_pSTM->SwapObj( pView->GetObjectPtr( id ));
	  	else
        pView->SelectObject( id, i );
    }
  }

  pView->UpdateWindow();
  return CUndoScaleItem::Play( pView );
}
//=====================================================================
//=====================================================================
int
CUndoCopyItem::Play( CMapView* pView )
{
  POSITION pos;
  if( m_bCopy ^= 1 )
  {
    for( pos = m_pPlusList->GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = m_pPlusList->GetNext( pos );
      CDrawObject* pCop = pObj->CopyObject();
      if( !FINL( pObj, m_pPlusList ))
      {
        pView->m_Conductor.Create( pObj );
        if( pView->m_Conductor.DeclareEvent( CE_OBJ_RESTORE ))
          return 1;
      }
      pView->AddObject( pCop, 1 );
      pView->SelectObject( pCop, pObj->SelectFlag());
    }
    for( pos = m_pMinusList->GetHeadPosition() ; pos ; )
      pView->SelectObject( m_pMinusList->GetNext( pos )->Ident(), 0 );
  }
  else
  {
    for( pos = m_pPlusList->GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = m_pPlusList->GetNext( pos );
      if( !FINL( pObj, m_pMinusList ))
      {
        pView->m_Conductor.Create( pObj );
        if( pView->m_Conductor.DeclareEvent( CE_OBJ_DELETE ))
          return 1;
      }
      pView->RemoveObject( pObj );
    }
    for( pos = m_pMinusList->GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = m_pMinusList->GetNext( pos );
      pView->SelectObject( pObj->Ident(), pObj->SelectFlag());
    }
  }
  pView->UpdateWindow();
  return CUndoScaleItem::Play( pView );
}
//=====================================================================
//=====================================================================
CUndoAddItem::CUndoAddItem( CMapView* pView, CDrawObject* pPlusObj, CDrawObject* pMinusObj ) :
  CUndoSelectItem( pView, 0, 0 )
{
  if( pPlusObj )
    m_List0.AddHead( pPlusObj->CopyObject());
  if( pMinusObj )
    m_List1.AddHead( pMinusObj->CopyObject());
}
//=====================================================================
int
CUndoAddItem::Play( CMapView* pView )
{
  POSITION pos;

  for( pos = m_pPlusList->GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = m_pPlusList->GetNext( pos );
    if( !FINL( pObj, m_pMinusList ))
    {
      pView->m_Conductor.Create( pObj );
      if( pView->m_Conductor.DeclareEvent( CE_OBJ_DELETE ))
        return 1;
    }
    if( pObj->IsSublayer())
    {
      pView->m_bSublayerChanged |= 1;
      pView->m_pSublDlg->RemoveItem( pObj );
    }
    pView->RemoveObject( pObj );
  }

  for( pos = m_pMinusList->GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = m_pMinusList->GetNext( pos );
    if( !FINL( pObj, m_pPlusList ))
    {
      pView->m_Conductor.Create( pObj );
      if( pView->m_Conductor.DeclareEvent( CE_OBJ_RESTORE ))
        return 1;
    }

		CDrawObject* pCop = pObj->CopyObject();
   
    if( pObj->IsSublayer())
		{
      pView->m_bSublayerChanged |= 1;
      pView->m_pSublDlg->AddItem( pCop );
		}
    pView->AddObject( pCop, 1 );
    pView->ScaleObject( pCop, pView->GetTrueScale());
    pView->SelectObject( pCop, pObj->SelectFlag());
	  if( !pView->IsVisibleInScale( pObj ))
		  pView->HideObject( pCop->Ident(), 1 );
	}

  pView->UpdateWindow();
  SwapLists();
  return CUndoScaleItem::Play( pView );
}
//=====================================================================
//=====================================================================
int
CUndoChangeItem::Play( CMapView* pView )
{
  CDrawObject* pObj = pView->GetObjectPtr( m_pPlusList->GetHead()->TabIdent());
  if( !pObj )
    return 1;
  pView->m_Conductor.Create( pObj );
  pView->SaveReverseDirection();
  if( pView->m_Conductor.DeclareEvent( CE_OBJ_PARAM ))
    return 1;
  pView->SaveReverseDirection( pObj );
  pView->SetObjParam( pObj, 1 );
  if( pView->m_pSTM )
    pView->m_pSTM->Reopen();
  pView->UpdateWindow();
  return CUndoScaleItem::Play( pView );
}
//=====================================================================
//=====================================================================
void
CUndo::Purge( CUndoList& list )
{
  while( !list.IsEmpty())
    delete list.RemoveHead();
}
//=====================================================================
void
CUndo::AddItem( CUndoScaleItem* pItem, CMapView* pView )
{
  if( pView->m_UndoLevel > 0 )
  {
    Purge( RedoList );
    UndoList.AddHead( pItem );
    if( UndoList.GetCount() > m_UndoMaxLength )
      delete UndoList.RemoveTail();
  }
  else
    delete pItem;

  if( pItem == pView->m_pUndoItem )
    pView->m_pUndoItem = 0;
}
//=====================================================================
void
CUndo::ExecUndo( CUndoList& srcList, CUndoList& dstList, CMapView* pView )
{
  CUndoScaleItem* pItem = srcList.RemoveHead();
  if( pItem->Play( pView ))
    srcList.AddHead( pItem );
  else
    dstList.AddHead( pItem );

  if( pView->m_pSTM )
    pView->m_pSTM->Reopen();

  pView->ShowSelectedNum();
}
//=====================================================================
