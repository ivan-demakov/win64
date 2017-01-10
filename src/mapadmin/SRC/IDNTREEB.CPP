//=====================================================================
#include "stdafx.h"

#include <math.h>
#include <string.h>
#include "idntreeb.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
#ifdef _DIRECT_HASH_IDENT_TAB
//=====================================================================
IdentDirectHashTab::IdentDirectHashTab() : 
  m_pTab( 0 ), 
  m_nUsed( 0 ),
  m_nLastUni( 1 ),
  m_nSize( 1024 )
{}
//=====================================================================
IdentDirectHashTab::IdentItem* 
IdentDirectHashTab::FindPos( CTabIdent const& id, int bUsed )
{ 
  int n1 = 0, n2 = m_nLimit;
  int b = ( id.Table() << 8 ) + id.Entry();
  while( n2 > 1 )
  {
    IdentItem* pos = m_pTab + (( b + n1 ) & m_nMask );
    if( pos->m_Id == id || ( bUsed ? pos->Free() : !pos->Used()))
      return pos;
    if(( n1 += n2 ) >= m_nSize )
      n1 = ( n2 >>= 1 ) >> 1;
  }
  return 0; //it is impossible!!!
}
//=====================================================================
int
IdentDirectHashTab::AddObject( CTabIdent const& id, std::streampos pos )
{
  if( !m_pTab || m_nUsed >= m_nLimit )
  {
    int newSize = m_nSize << 1;
    IdentItem* pNew = new IdentItem[newSize];
    if( !pNew )
    {
      cerr << "Alloc " << ( newSize * sizeof IdentItem ) << " for " << m_nUsed << " indexes failed!" << std::endl;
      return 0;
    }
    memset( pNew, 0, newSize * sizeof IdentItem );
    IdentItem* pOld = m_pTab;
    m_pTab = pNew;
    m_nLimit = m_nSize;
    m_nSize = newSize;
    m_nMask = m_nSize - 1;
    m_nUsed = 0;
    if( pOld )
    {
      for( IdentItem* tp = pOld + m_nLimit ; --tp >= pOld ; )
        if( tp->m_Pos > 0 )
        {
          FindPos( tp->m_Id )->Set( tp->m_Id, tp->m_Pos );
          ++m_nUsed;
        }
      delete pOld;
    }
  }

  IdentItem* p = FindPos( id );
  if( !p->Used())
    ++m_nUsed;
  p->Set( id, pos );
  return 1;
}
//=====================================================================
int
IdentDirectHashTab::RemoveObject( CTabIdent const& id )
{
  if( !m_pTab || !m_nUsed )
    return 0;

  IdentItem* p = FindPos( id, 1 );
  if( !p || p->Free())
    return 0;

  if( id.Table() == 0 )
    m_nLastUni = min( id.Entry(), m_nLastUni );

  --m_nUsed;
  p->m_Pos = -1;    
  return 1;
}
//=====================================================================
std::streampos
IdentDirectHashTab::SelectObject( CTabIdent const& id )
{
  if( !m_pTab || !m_nUsed )
    return 0;

  IdentItem* p = FindPos( id, 1 );
  return p && p->Used() ? p->m_Pos : 0;
}
//=====================================================================
EntryType
IdentDirectHashTab::GetUniId()
{
  while( SelectObject( CTabIdent( 0, m_nLastUni )))
    ++m_nLastUni;
  return m_nLastUni++;
}
//=====================================================================
void
IdentDirectHashTab::Clear()
{
  delete m_pTab;
  m_pTab = 0;
  m_nSize >>= 1;
  m_nUsed = 0;
}
//=====================================================================
#endif
//=====================================================================
#ifdef _NEW_IDENT_TREE_
//=====================================================================
IdentTree::IdentTreeItem::IdentTreeItem( CTabIdent const& id, std::streampos pos, IdentTreeItem* pPrnt ) :
  m_Id( id ),
  m_Pos( pos ),
  m_nBlnc( 0 ),
  m_pLeft( 0 ),
  m_pRght( 0 ),
  m_nLastUni( 1 ),
  m_pPrnt( pPrnt )
{}
//=====================================================================
int
IdentTree::BalanceLeft( IdentTreeItem** pRoot )
{
  IdentTreeItem* p = *pRoot;

  if(( p->m_nBlnc += 1 ) < 2 )
    return !p->m_nBlnc;

  IdentTreeItem* p1 = p->m_pRght;
  IdentTreeItem* p2 = p1->m_pLeft;
  int b = p1->m_nBlnc;
  if( b >= 0 )
  {
    ( *pRoot = p1 )->m_pPrnt = p->m_pPrnt;
    if( p->m_pRght = p2 )
      p2->m_pPrnt = p;
    ( p1->m_pLeft = p )->m_pPrnt = p1;
    p->m_nBlnc  = b == 0 ?  1 : 0;
    p1->m_nBlnc = b == 0 ? -1 : 0;
    return b != 0;
  }

  b = p2->m_nBlnc;
  ( *pRoot = p2 )->m_pPrnt = p->m_pPrnt;
  if( p1->m_pLeft = p2->m_pRght )
    p2->m_pRght->m_pPrnt = p1;
  ( p2->m_pRght = p1 )->m_pPrnt = p2;
  if( p->m_pRght = p2->m_pLeft )
    p2->m_pLeft->m_pPrnt = p;
  ( p2->m_pLeft = p )->m_pPrnt = p2;
  p->m_nBlnc  = b ==  1 ? -1 : 0;
  p1->m_nBlnc = b == -1 ?  1 : 0;
  p2->m_nBlnc = 0;
  return 1;
}
//=====================================================================
int
IdentTree::BalanceRght( IdentTreeItem** pRoot )
{
  IdentTreeItem* p = *pRoot;

  if(( p->m_nBlnc -= 1 ) > -2 )
    return !p->m_nBlnc;

  IdentTreeItem* p1 = p->m_pLeft;
  IdentTreeItem* p2 = p1->m_pRght;
  int b = p1->m_nBlnc;
  if( b <= 0 )
  {
    ( *pRoot = p1 )->m_pPrnt = p->m_pPrnt;
    if( p->m_pLeft = p2 )
      p2->m_pPrnt = p;
    ( p1->m_pRght = p )->m_pPrnt = p1;
    p->m_nBlnc  = b == 0 ? -1 : 0;
    p1->m_nBlnc = b == 0 ?  1 : 0;
    return b != 0;  
  }

  b = p2->m_nBlnc;
  ( *pRoot = p2 )->m_pPrnt = p->m_pPrnt;
  if( p1->m_pRght = p2->m_pLeft )
    p2->m_pLeft->m_pPrnt = p1;
  ( p2->m_pLeft = p1 )->m_pPrnt = p2;
  if( p->m_pLeft = p2->m_pRght )
    p2->m_pRght->m_pPrnt = p;
  ( p2->m_pRght = p )->m_pPrnt = p2;
  p->m_nBlnc  = b == -1 ?  1 : 0;
  p1->m_nBlnc = b ==  1 ? -1 : 0;
  p2->m_nBlnc = 0;
  return 1;
}
//=====================================================================
IdentTree::IdentTree() :
  m_pRoot( 0 )
{
  m_Path[0] = 0;
}
//=====================================================================
EntryType
IdentTree::GetUniId()
{
  while( SelectObject( CTabIdent( 0, m_nLastUni )))
    ++m_nLastUni;
  return m_nLastUni++;
}
//=====================================================================
char*
IdentTree::GetPath()
{
  return m_Path;
}
//=====================================================================
void
IdentTree::Clear()
{
  for( IdentTreeItem* p = m_pRoot ; p ;  )
  {
    IdentTreeItem* q;
    while(( q = p->m_pLeft ) || ( q = p->m_pRght ))
      p = q;
    q = p->m_pPrnt;
    *( !q ? &m_pRoot : q->m_pLeft == p ? &q->m_pLeft : &q->m_pRght ) = 0;
    delete p;
    p = q;
  }
}
//=====================================================================
int
IdentTree::AddObject( CTabIdent const& id, std::streampos pos )
{ 
  IdentTreeItem* pPrnt = 0;
  IdentTreeItem* p = m_pRoot;
  while( p && id != p->m_Id )
  {
    pPrnt = p;
    p = id < p->m_Id ? p->m_pLeft : p->m_pRght;
  }
  if( p )
  {
    p->m_Pos = pos;
    return 1;
  }

  IdentTreeItem** pRoot = !pPrnt ? &m_pRoot : id < pPrnt->m_Id ? &pPrnt->m_pLeft : &pPrnt->m_pRght;
  *pRoot = new IdentTreeItem( id, pos, pPrnt );

  if( !*pRoot )
  {
    cerr << "Alloc " << sizeof IdentTreeItem << " for indexes failed!" << endl;
    return 0;
  }

  while( pPrnt )
  {
    pPrnt = ( p = pPrnt )->m_pPrnt;
    pRoot = !pPrnt ? &m_pRoot : pPrnt->m_pLeft == p ? &pPrnt->m_pLeft : &pPrnt->m_pRght;

    if( id < p->m_Id )
    {
      switch( p->m_nBlnc -= 1 )
      {
        case -1 : continue;
        case -2 :
        {
          IdentTreeItem* p1 = p->m_pLeft;
          IdentTreeItem* p2 = p1->m_pRght;
          if( p1->m_nBlnc < 0 )
          {
            ( *pRoot = p1 )->m_pPrnt = pPrnt;
            if( p->m_pLeft = p2 )
              p2->m_pPrnt = p;
            ( p1->m_pRght = p )->m_pPrnt = p1;
            p->m_nBlnc = 0;
          }
          else
          {
            ( *pRoot = p2 )->m_pPrnt = pPrnt;
            if( p1->m_pRght = p2->m_pLeft )
              p2->m_pLeft->m_pPrnt = p1;
            ( p2->m_pLeft = p1 )->m_pPrnt = p2;
            if( p->m_pLeft = p2->m_pRght )
              p2->m_pRght->m_pPrnt = p;
            ( p2->m_pRght = p )->m_pPrnt = p2;
            p->m_nBlnc  = p2->m_nBlnc == -1 ?  1 : 0;
            p1->m_nBlnc = p2->m_nBlnc ==  1 ? -1 : 0;
          }
          (*pRoot)->m_nBlnc = 0;
        }
      }
      break;
    }
    else
    {
      switch( p->m_nBlnc += 1 )
      {
        case 1 : continue;
        case 2 :
        {
          IdentTreeItem* p1 = p->m_pRght;
          IdentTreeItem* p2 = p1->m_pLeft;
          if( p1->m_nBlnc > 0 )
          {
            ( *pRoot = p1 )->m_pPrnt = pPrnt;
            if( p->m_pRght = p2 )
              p2->m_pPrnt = p;
            ( p1->m_pLeft = p )->m_pPrnt = p1;
            p->m_nBlnc = 0;
          }
          else
          {
            ( *pRoot = p2 )->m_pPrnt = pPrnt;
            if( p1->m_pLeft = p2->m_pRght )
              p2->m_pRght->m_pPrnt = p1;
            ( p2->m_pRght = p1 )->m_pPrnt = p2;
            if( p->m_pRght = p2->m_pLeft )
              p2->m_pLeft->m_pPrnt = p;
            ( p2->m_pLeft = p )->m_pPrnt = p2;
            p->m_nBlnc  = p2->m_nBlnc ==  1 ? -1 : 0;
            p1->m_nBlnc = p2->m_nBlnc == -1 ?  1 : 0;
          }
          (*pRoot)->m_nBlnc = 0;
        }
      }
      break;      
    }
  }
  return 1;
} 
//=====================================================================
int
IdentTree::RemoveObject( CTabIdent const& id )
{
  IdentTreeItem* p = m_pRoot;
  while( p && p->m_Id != id )
    p = id < p->m_Id ? p->m_pLeft : p->m_pRght;

  if( !p )
    return 0;

  int dp = 0;
  IdentTreeItem* pRemItem = p;
  IdentTreeItem* pPrnt = p->m_pPrnt;
  IdentTreeItem** pRoot = !pPrnt ? &m_pRoot : id < pPrnt->m_Id ? &pPrnt->m_pLeft : &pPrnt->m_pRght;
  IdentTreeItem* pChnItem = !p->m_pRght ? p->m_pLeft : !p->m_pLeft ? p->m_pRght : p;

  if( pChnItem == p )
  {
    for( p = p->m_pLeft, ++dp ; p->m_pRght ; p = p->m_pRght, ++dp );
    pRemItem->m_Pos = p->m_Pos;
    pRemItem->m_Id = p->m_Id;
    pRemItem = p;
    pPrnt = p->m_pPrnt;
    pRoot = pPrnt->m_pLeft == p ? &pPrnt->m_pLeft : &pPrnt->m_pRght;
    pChnItem = p->m_pLeft;
  }
  if( *pRoot = pChnItem ) 
    pChnItem->m_pPrnt = pPrnt;

  while( pPrnt )
  {
    --dp;
    pPrnt = ( p = pPrnt )->m_pPrnt;
    pRoot = !pPrnt ? &m_pRoot : pPrnt->m_pLeft == p ? &pPrnt->m_pLeft : &pPrnt->m_pRght;
    if( !( !dp || dp < 0 && id < (*pRoot)->m_Id ? BalanceLeft( pRoot ) : BalanceRght( pRoot )))
      break;
  }

  if( id.Table() == 0 )
    m_nLastUni = min( id.Entry(), m_nLastUni );

  delete pRemItem;
  return 1;
} 
//=====================================================================
std::streampos 
IdentTree::SelectObject( CTabIdent const& id )
{
  for( IdentTreeItem* p = m_pRoot ; p && p->m_Id != id ;
       p = id < p->m_Id ? p->m_pLeft : p->m_pRght );
  return p ? p->m_Pos : 0;
}
//=====================================================================
#endif
//=====================================================================
#ifdef _OLD_IDENT_TREE_
//=====================================================================
//=====================================================================
IdnNest::IdnNest( CTabIdent const& id, std::streampos pos, int blnc ) :
  Id( id ),
  Pos( pos ),
  Blnc( blnc ),
  pLeft( 0 ),
  pRight( 0 )
{}
//=====================================================================
IdnNest::~IdnNest()
{
  delete pLeft;
  delete pRight;
}
//=====================================================================
int
IdnNest::AddObject( IdnNest** pRoot, CTabIdent const& id, std::streampos pos )
{
  IdnNest* p = *pRoot;

  if( !p )
  {
    *pRoot = new IdnNest( id, pos, 0 );
    return *pRoot ? 1 : -1;
  }

  if( id < p->Id )
  {
    if( !AddObject( &p->pLeft, id, pos ))
      return 0;

    if(( p->Blnc -= 1 ) > -2 )
      return p->Blnc != 0;

    IdnNest* p1 = p->pLeft;
    if( p1->Blnc < 0 )
    {
      p->pLeft = p1->pRight;
      p1->pRight = p;
      *pRoot = p1;
      p->Blnc = 0;
    }
    else
    {
      IdnNest* p2 = p1->pRight;
      p1->pRight = p2->pLeft;
      p2->pLeft = p1;
      p->pLeft = p2->pRight;
      p2->pRight = p;
      *pRoot = p2;
      p->Blnc  = p2->Blnc == -1 ?  1 : 0;
      p1->Blnc = p2->Blnc ==  1 ? -1 : 0;
    }
    return (*pRoot)->Blnc = 0;
  }

  if( id > p->Id )
  {
    if( !AddObject( &p->pRight, id, pos ))
      return 0;

    if(( p->Blnc += 1 ) < 2 )
      return p->Blnc != 0;

    IdnNest* p1 = p->pRight;
    if( p1->Blnc > 0 )
    {
      p->pRight = p1->pLeft;
      p1->pLeft = p;
      *pRoot = p1;
      p->Blnc = 0;
    }
    else
    {
      IdnNest* p2 = p1->pLeft;
      p1->pLeft = p2->pRight;
      p2->pRight = p1;
      p->pRight = p2->pLeft;
      p2->pLeft = p;
      *pRoot = p2;
      p->Blnc  = p2->Blnc ==  1 ? -1 : 0;
      p1->Blnc = p2->Blnc == -1 ?  1 : 0;
    }
    return (*pRoot)->Blnc = 0;
  }

  // try to add the same ident
  p->Pos = pos;
  return 0;
}
//=====================================================================
//=====================================================================
int
IdnNest::BalanceLeft( IdnNest** pRoot )
{
  IdnNest* p = *pRoot;

  if(( p->Blnc += 1 ) < 2 )
    return !p->Blnc;

  IdnNest* p1 = p->pRight;
  int b = p1->Blnc;
  if( b >= 0 )
  {
    p->pRight = p1->pLeft;
    p1->pLeft = p;
    *pRoot = p1;
    p->Blnc  = b == 0 ?  1 : 0;
    p1->Blnc = b == 0 ? -1 : 0;
    return b;
  }

  IdnNest* p2 = p1->pLeft;
  b = p2->Blnc;
  p1->pLeft = p2->pRight;
  p2->pRight = p1;
  p->pRight = p2->pLeft;
  p2->pLeft = p;
  *pRoot = p2;
  p->Blnc  = b ==  1 ? -1 : 0;
  p1->Blnc = b == -1 ?  1 : 0;
  p2->Blnc = 0;
  return 1;
}
//=====================================================================
int
IdnNest::BalanceRight( IdnNest** pRoot )
{
  IdnNest* p = *pRoot;

  if(( p->Blnc -= 1 ) > -2 )
    return !p->Blnc;

  IdnNest* p1 = p->pLeft;
  int b = p1->Blnc;
  if( b <= 0 )
  {
    p->pLeft = p1->pRight;
    p1->pRight = p;
    *pRoot = p1;
    p->Blnc  = b == 0 ? -1 : 0;
    p1->Blnc = b == 0 ?  1 : 0;
    return b;
  }

  IdnNest* p2 = p1->pRight;
  b = p2->Blnc;
  p1->pRight = p2->pLeft;
  p2->pLeft = p1;
  p->pLeft = p2->pRight;
  p2->pRight = p;
  *pRoot = p2;
  p->Blnc  = b == -1 ?  1 : 0;
  p1->Blnc = b ==  1 ? -1 : 0;
  p2->Blnc = 0;
  return 1;
}
//=====================================================================
int
IdnNest::Remove( IdnNest** pRoot, IdnNest* q )
{
  IdnNest* p = *pRoot;

  if( p->pRight )
    return Remove( &p->pRight, q ) && BalanceRight( pRoot );

  q->Id = p->Id;
  q->Pos = p->Pos;
  *pRoot = p->pLeft;
  p->pLeft = 0;
  delete p;

  return 1;
}
//=====================================================================
int
IdnNest::RemoveObject( IdnNest** pRoot, CTabIdent const& id )
{
  IdnNest* p = *pRoot;

  if( !p )
    return 0;

  if( id < p->Id )
    return RemoveObject( &p->pLeft, id ) && BalanceLeft( pRoot );

  if( id > p->Id )
    return RemoveObject( &p->pRight, id ) && BalanceRight( pRoot );

  if( !p->pRight )
    *pRoot = p->pLeft, p->pLeft = 0;
  else
  if( !p->pLeft )
    *pRoot = p->pRight, p->pRight = 0;
  else
    return Remove( &p->pLeft, p ) && BalanceLeft( pRoot );

  delete p;
  return 1;
}
//=====================================================================
std::streampos 
IdnNest::SelectObject( IdnNest* p, CTabIdent const& id )
{
  while( p && p->Id != id )
    p = id < p->Id ? p->pLeft : p->pRight;
  return p ? p->Pos : 0;
}
//=====================================================================
//=====================================================================
IdentTree::IdentTree() :
  m_nLastUni( 1 ),
  Root( 0 )
{}
//=====================================================================
EntryType
IdentTree::GetUniId()
{
  while( SelectObject( CTabIdent( 0, m_nLastUni )))
    ++m_nLastUni;
  return m_nLastUni++;
}
//=====================================================================
void
IdentTree::Clear()
{
  delete Root;
  Root = 0;
}
//=====================================================================
IdentTree::~IdentTree()
{
  Clear();
}
//=====================================================================
int
IdentTree::RemoveObject( CTabIdent const& id )
{
  if( id.Table() == 0 )
    m_nLastUni = min( id.Entry(), m_nLastUni );

  return IdnNest::RemoveObject( &Root, id );
}
//=====================================================================
int
IdentTree::AddObject( CTabIdent const& id, std::streampos pos )
{
  return IdnNest::AddObject( &Root, id, pos ) >= 0;
}
//=====================================================================
std::streampos
IdentTree::SelectObject( CTabIdent const& id  )
{
  return IdnNest::SelectObject( Root, id );
}
//=====================================================================
#endif
//=====================================================================
#ifdef _LINKED_HASH_IDENT_TAB
//=====================================================================
enum
{ 
  START_SIZE = 8192,
  LINK_SIZE_LIMIT = 8
};
//=====================================================================
IdentLinkedHashTab::IdentLinkedHashTab() :
  m_nSize( START_SIZE ),
  m_nLinks( 0 ),
  m_nUsed( 0 ),
  m_pTab( 0 ),
  m_nLastUni( 1 )
{}
//=====================================================================
IdentLinkedHashTab::IdentItem**
IdentLinkedHashTab::FindPos( CTabIdent const& id )
{
  m_pHead = &m_pTab[(( id.Table() << 8 ) + id.Entry()) & ( m_nSize - 1 )];
  pItem* ppos = m_pHead;
  while( *ppos && (*ppos)->m_Id < id )
    ppos = &(*ppos)->m_pNext;
  
  return ppos;
}
//=====================================================================
int
IdentLinkedHashTab::RemoveObject( CTabIdent const& id )
{
  if( !m_nUsed )
    return 0;

  pItem* ppos = FindPos( id );
  pItem pFnd = *ppos;
  if( !pFnd || pFnd->m_Id != id )
    return 0;
  
  *ppos = pFnd->m_pNext;
  delete pFnd;

  m_nUsed -= 1;
  m_nLinks -= !*m_pHead;

  if( id.Table() == 0 )
    m_nLastUni = min( id.Entry(), m_nLastUni );

  return 1;
}
//=====================================================================
int
IdentLinkedHashTab::AddObject( CTabIdent const& id, std::streampos pos )
{
  int mls = m_nLinks ? m_nUsed / m_nLinks : LINK_SIZE_LIMIT + 1;
  if( mls > LINK_SIZE_LIMIT )
  {
    int oldSize = m_nSize;
    m_nSize <<= 1;
    pItem* pOld = m_pTab;
    m_pTab = new pItem[m_nSize];
    if( !m_pTab )
      return 0;
  
    memset( m_pTab, 0, m_nSize * sizeof pItem );
    if( pOld )
    {
      m_nLinks = 0;
      for( pItem* cp = pOld + oldSize ; --cp >= pOld ; )
        for( pItem pos = *cp ; pos ; )
        {
          pItem nxt = pos->m_pNext;
          pItem* ppos = FindPos( pos->m_Id );
          m_nLinks += !*m_pHead;
          pos->m_pNext = *ppos;
          *ppos = pos;
          pos = nxt;
        }
      delete pOld;
    }
  }

  pItem* ppos = FindPos( id );
  int il = !*m_pHead;

  m_LastReplaced = 0;
  if( *ppos && (*ppos)->m_Id == id )
  {
    m_LastReplaced = (*ppos)->m_Pos;
    (*ppos)->m_Pos = pos;
  }
  else
  {
    pItem pNew = new IdentItem( id, pos, *ppos );
    if( !pNew )
      return 0;

    *ppos = pNew;
    ++m_nUsed;
    m_nLinks += il;
  }

  return 1;
}
//=====================================================================
std::streampos
IdentLinkedHashTab::SelectObject( CTabIdent const& id )
{
  if( !m_nUsed )
    return 0;

  pItem pos = *FindPos( id );
  return pos && pos->m_Id == id ? pos->m_Pos : 0;
}
//=====================================================================
EntryType
IdentLinkedHashTab::GetUniId( int t )
{
  while( SelectObject( CTabIdent( t, m_nLastUni )))
    ++m_nLastUni;
  return m_nLastUni++;
}
//=====================================================================
void
IdentLinkedHashTab::Clear()
{
  if( m_pTab )
  { 
    for( pItem* cp = m_pTab + m_nSize ; --cp >= m_pTab ; )
      while( *cp )
      {
        pItem pTmp = *cp;
        *cp = pTmp->m_pNext;
        delete pTmp;
      }
    delete m_pTab;
    m_pTab = 0;
    m_nUsed = 0;
    m_nLinks = 0;
    m_nSize = START_SIZE;
  }
}
//=====================================================================
#endif
