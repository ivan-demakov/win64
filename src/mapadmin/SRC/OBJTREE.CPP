//=====================================================================
#include "stdafx.h"

#include "objtree.h"
#include "mview.h"
#include "mdoc.h"
#include "util.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "mercator.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
void
CObjHashTab::SavePosition()
{ 
  if( m_pTab )
    m_pTab->m_pSafe = m_pTab->m_pCur;
}
//=====================================================================
void
CObjHashTab::RestPosition()
{ 
  if( m_pTab )
    m_pTab->m_pCur = m_pTab->m_pSafe;
}
//=====================================================================
pObject*
CObjHashTab::FindPos( UINT id, int bFree )
{
  int s = m_pTab->m_nSize, n1 = 0, n2 = s << 1, msk = s - 1;
  pObject* pb = m_pTab->m_pBeg;
  while( n2 > 1 )
  {
    pObject* pos = pb + (( id + n1 ) & msk );
    pObject pObj = *pos;
    if( !pObj || ( IsObjPtr( pObj ) ? pObj->Entry() == id : bFree ))
      return pos;
    if(( n1 += n2 ) >= s )
      n1 = ( n2 >>= 1 ) >> 1;
  }
//  ASSERT( 0 );
  return 0;
}
//=====================================================================
CDrawObject*
CObjHashTab::Find( CTabIdent id )
{
  pObject pObj = m_pTab ? *FindPos( id.Entry()) : 0;
  return pObj && pObj->Table() == id.Table() ? pObj : 0;
}
//=====================================================================
CDrawObject*
CObjHashTab::AddObject( CDrawObject* pObj )
{
  ASSERT( pObj );

  if( !m_pTab || m_pTab->m_nUsed >= ( m_pTab->m_nSize >> 1 ))
  {
    int size = !m_pTab ? 16 : m_pTab->m_nSize << 1;
    TabDef* pOld = m_pTab;
    m_pTab = (TabDef*) new char[sizeof TabDef + size * sizeof pObject];
    if( !m_pTab )
    {
      ErrBox( "Ошибка выделения памяти!\nПриложение будет закрыто." );
      AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_APP_EXIT, 0 );
      return 0;
    }
    memset( m_pTab->m_pBeg, 0, size * sizeof pObject );
    m_pTab->m_pEnd = m_pTab->m_pBeg + size;
    m_pTab->m_nSize = size;
    m_pTab->m_nUsed = 0;
    if( pOld )
    {
      m_pTab->m_nUsed = pOld->m_nUsed;
      for( pObject* p = pOld->m_pEnd ; --p >= pOld->m_pBeg ; )
        if( IsObjPtr( *p ))
          *FindPos((*p)->Entry()) = *p;
      delete pOld;
    }
  }

  pObject* p = FindPos( pObj->Entry(), 1 );
  CDrawObject* pExc = *p;
  *p = pObj;
  
  if( IsObjPtr( pExc ))
    return pExc;

  ++m_pTab->m_nUsed;
  return 0;
}
//=====================================================================
CDrawObject*
CObjHashTab::RemoveObject( EntryType id )
{
  if( !m_pTab || !m_pTab->m_nUsed )
    return 0;

  pObject* p = FindPos( id );
  CDrawObject* pExc = *p;
  if( pExc )
    *p = (pObject)-1;

  if( !IsObjPtr( pExc ))
    return 0;

  --m_pTab->m_nUsed;
  return pExc;
}
//=====================================================================
CDrawObject*
CObjHashTab::GetFirst()
{
  SetFirst();
  return GetNext();
}
//=====================================================================
CDrawObject*
CObjHashTab::SetFirst()
{
  if( m_pTab )
    m_pTab->m_pCur = m_pTab->m_pBeg;
  return 0;
}
//=====================================================================
CDrawObject*
CObjHashTab::GetLast()
{
  SetLast();
  return GetPrev();
}
//=====================================================================
CDrawObject*
CObjHashTab::SetLast()
{
  if( m_pTab )
    m_pTab->m_pCur = m_pTab->m_pEnd;
  return 0;
}
//=====================================================================
CDrawObject*
CObjHashTab::GetNext()
{
  if( m_pTab )
    while( m_pTab->m_pCur < m_pTab->m_pEnd )
    {
      CDrawObject* pObj = *m_pTab->m_pCur++;
      if( IsObjPtr( pObj ))
        return pObj;
    }
  return 0;
}
//=====================================================================
CDrawObject*
CObjHashTab::GetPrev()
{
  if( m_pTab )
    while( m_pTab->m_pCur > m_pTab->m_pBeg )
    {
      CDrawObject* pObj = *--m_pTab->m_pCur;
      if( IsObjPtr( pObj ))
        return pObj;
    }
  return 0;
}
//=====================================================================
void
CObjHashTab::RemoveAll( CRect const* pClipRect, int bSaveSlct )
{
	if( !m_pTab )
    return;

  if( pClipRect || bSaveSlct )
  {
		double osf = pView->GetOldScaleFactor();
		double nsf = pView->GetScaleFactor();
    int nsc = pView->GetAltScale();
    int osc = pView->GetOldScale();
		nsc = nsc / nsf + .5;
		osc = osc / osf + .5;

    int bDifScale = nsc != osc;
    int bVldScale = 1;
    SetLast();
    CDrawObject* pObj = GetPrev();
    if( bDifScale && pObj )
    {
      CObjDef* pObjDef = pView->GetDocument()->GetObjDef();
      int t = pObj->Class();
      int mis = pObjDef->GetMinLimit( t );
      int mas = pObjDef->GetMaxLimit( t );
      bVldScale = ( !mis || nsc >= mis ) && ( !mas || nsc <= mas );
    }
    
    TabDef* pOld = m_pTab;
    m_pTab = 0;

    for( pObject* p = pOld->m_pEnd ; --p >= pOld->m_pBeg ; )
      if( IsObjPtr( pObj = *p ))
      {
        if( !( bSaveSlct && ( pObj->IsSelected() || pObj->IsObjectTemp() || pObj->IsExtObject()) ||
               bVldScale && ( !pClipRect || Intersect( &pObj->GetOriginBox(), pClipRect ))))
          delete pObj;
        else
        {
          if( bDifScale )
            pView->ScaleObject( pObj, nsc );
          AddObject( pObj );
        }
      }
    delete pOld;
  }
  else
  {
    for( pObject* p = m_pTab->m_pEnd ; --p >= m_pTab->m_pBeg ; )
      if( IsObjPtr( *p ))
        delete *p;
    delete m_pTab;
    m_pTab = 0;
  }
}
//=====================================================================
