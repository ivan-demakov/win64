#include "stdafx.h"

#include "dwin.h"
#include "mview.h"
#include "mdoc.h"
#include "idg.h"
#include "movemode.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "undo.h"
#include "util.h"
#include "ids.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
int CMoveMode::m_bCopy = 0;
//=====================================================================
int
CMoveMode::CopyMode()
{ 
  return !pView->SublDlgIsActive() && m_bCopy; 
}
//=====================================================================
void
CMoveMode::OnCopyMode()
{ 
  m_bCopy ^= 1; 
}
//=====================================================================
CMoveMode::CMoveMode( CPoint startPoint ) :
  CAppMode( AIDG_MOVMODE, IDC_MOVE, AM_FLBUP | AM_FMOUSEMOVE,
            IDM_MOVE_MODE ),
  m_StartPos( startPoint ),
  m_LastPos( startPoint ),
  m_Scale( 0 )
{}
//=====================================================================
int
CMoveMode::Open()
{
  CObjList& Sl = m_pOwner->m_SelectList;

  for( POSITION pos = Sl.GetHeadPosition() ; pos ; )
    if( !m_pOwner->IsObjectInLegalRegion( Sl.GetNext( pos ), CopyMode() ? "Копирование" : "Перемещение", 1 ))
      return 0;

  m_nMode = 1;
  m_Box = m_pOwner->CalcSelectBox();
  if( CopyMode())
  {
    if( !m_pOwner->TestSelected())
    {
      m_pOwner->RemoveMode( this );
      return 0;
    }
    ASSERT( !m_pOwner->m_pUndoItem );
    m_pOwner->m_pUndoItem = new CUndoCopyItem( m_pOwner, &m_pOwner->m_SelectList );
    m_pOwner->CopySelected();
  }
  else
    m_pOwner->ShowSelected( 0 );

  m_pBlinking = m_pOwner->StartBlinking( 0 );
  OnSetCursor();
  
  return 1;
}
//=====================================================================
CMoveMode::~CMoveMode()
{
  if( m_Scale )
    m_PictBmp.DeleteObject();
}
//=====================================================================
void
CMoveMode::Draw( CDC* pDC )
{ 
  CMemDC memDC( pDC );
  if( !m_Scale )
  { // first time drawing
    CRect box;
    box.IntersectRect( &m_pOwner->m_Clip, &m_Box );
    m_Bmps = m_pOwner->LP2DP( box.Size());
    m_PictOff = box.TopLeft() - m_Box.TopLeft();
    m_PictBmp.CreateCompatibleBitmap( pDC, m_Bmps.cx, m_Bmps.cy );
    memDC.SelectObject( &m_PictBmp );
    memDC.PatBlt( 0, 0, box.Width(), box.Height(), WHITENESS );
    memDC.SetWindowOrg( box.left, box.top );
    m_pOwner->DrawSelect( &memDC );
    m_Scale = m_pOwner->GetAltScale();
  }
  memDC.SelectObject( &m_PictBmp );
  CPoint org = m_pOwner->LP2DP( m_Box.TopLeft() + m_PictOff );
  SetTM( &memDC );
  int saveDC = SetTM( pDC );
  pDC->BitBlt( org.x, org.y, m_Bmps.cx, m_Bmps.cy, &memDC, 0, 0, 0x00990066 );
  pDC->RestoreDC( saveDC );
}
//=====================================================================
int
CMoveMode::OnLButtonUp( UINT &flags, CPoint &loc )
{
  CObjList* pList = &m_pOwner->m_SelectList;

  for( POSITION pos = pList->GetHeadPosition() ; pos ; )
    if( !m_pOwner->IsObjectInLegalRegion( pList->GetNext( pos ), CopyMode() ? "Копирование" : "Перемещение", 2 ))
      return BreakCopy();   

  if( CopyMode())
  {
    if( m_StartPos == m_LastPos )
      return BreakCopy();

    if( pList->GetCount() != 1 || !pList->GetHead()->Table())
    {
      for( POSITION pos = pList->GetHeadPosition() ; pos ; )
      {
        CDrawObject* pObj = pList->GetNext( pos );
        pObj->SetEntry( m_pOwner->GetUniId());
        m_pOwner->AddObject( pObj, 1 );
      }
      m_pOwner->ShowSelected( 1, 0, 1 );
      m_pOwner->m_pUndoItem->SetList( pList, 1 );
			if( !GetProjectSLEnable())
        m_pOwner->m_Undo.AddItem( m_pOwner->m_pUndoItem, m_pOwner );
			else
				m_pOwner->KillUndoItem();
    }
    else
    {
      CDrawObject* pObj = pList->GetHead();
      CDrawObject* pOld = m_pOwner->m_ClipData.RemoveObject( pObj->Ident());
      CConductor* pCnd = m_pOwner->GetConductor();
      pCnd->Create( pObj );
      int r = pCnd->DeclareEvent( CE_OBJ_COPY );
      if( !r && pCnd->GetTable() == pObj->TabIdent())
        pCnd->SetTable( 0, m_pOwner->GetUniId());

      if( !r && m_pOwner->m_MapStore.TestObject( pCnd->GetIdent()) == R_OK )
      {
        ErrBox( "Объект с таким идентификатором уже существует.\n"
                "Обратитесь к администратору проекта." );
        r = 1;
      }

      if( r || m_pOwner->SetObjParam( pObj ) != UE_NOERROR )
      {
        delete pList->RemoveHead();
        m_pOwner->KillUndoItem();
        m_pOwner->InvalidateRect( 0, 0 );
      }
      else
      {
        m_pOwner->m_pUndoItem->SetList( pList, 1 );
				if( !GetProjectSLEnable())
          m_pOwner->m_Undo.AddItem( m_pOwner->m_pUndoItem, m_pOwner );
				else
					m_pOwner->KillUndoItem();
        pCnd->DeclareEvent( CE_OBJ_BUILD_END );
      }
      m_pOwner->m_ClipData.AddObject( pOld );
    }
  }
  else
  {
    m_pOwner->ShowSelected( 1, 1, 1 );
    m_pOwner->StartBlinking( m_pBlinking );
    m_pOwner->m_Undo.AddItem( new CUndoMoveItem( m_pOwner, m_LastPos - m_StartPos ), m_pOwner );
  }

  m_pOwner->ShowSelectedNum();
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CMoveMode::OnMouseMove( UINT& flags, CPoint& loc )
{
  CSize off( loc - m_LastPos );
  CRect b( m_Box + off );
  CSize ds( m_pOwner->GetDocument()->GetSize());
  off.cx -= b.left < 0 ? b.left : b.right  >= ds.cx ? b.right  - ds.cx : 0;
  off.cy -= b.top  < 0 ? b.top  : b.bottom >= ds.cy ? b.bottom - ds.cy : 0;
  m_pOwner->MoveSelectOn( off );
  m_Box += off;
  m_LastPos += off;
  return AM_RPROPAGATE;
}
//=====================================================================
int
CMoveMode::OnChar( UINT& flags, UINT& key_code )
{
  if( key_code != VK_ESCAPE )
    return AM_RDONE;

  if( CopyMode())
    return BreakCopy();

  m_pOwner->MoveSelectOn( m_StartPos - m_LastPos );
  m_pOwner->ShowSelected( 1, 0 );
  
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CMoveMode::BreakCopy()
{
  m_pOwner->ClearSelection( 1, 0 );
  CObjList* pList = m_pOwner->m_pUndoItem->GetList( 0 );
  for( POSITION pos = pList->GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = pList->GetNext( pos );
    m_pOwner->SelectObject( pObj->Ident(), pObj->SelectFlag());
  }
  m_pOwner->KillUndoItem();
  m_nMode = 0;
  
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
