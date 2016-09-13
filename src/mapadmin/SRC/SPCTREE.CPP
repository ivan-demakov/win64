//=====================================================================
// файл "spctree.cpp"
// реализация пространственного индексирования
//=====================================================================
#include "stdafx.h"

#include "spctree.h"
//=====================================================================
#ifdef _CLIENT_APP
  #ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char BASED_CODE THIS_FILE[] = __FILE__;
  #endif
#endif
//=====================================================================	
#ifdef _USE_HEAP_
HANDLE SpcTree::Heap;
#endif //_USE_HEAP_
//=====================================================================	
#ifdef _DEBUG
  static int nStackSz; // глубина стека
  static int nMaxStSz; // максимальная глубина стека
#endif
//=====================================================================	
int
SpcTree::IsValidNode( int Node, SelectParam* pSp )
{
  if( !Node || !pSp->pNodes || !*pSp->pNodes )
    return 1;

  for( long* p = pSp->pNodes ; *p && *p != Node ; ++p );
  return *p != 0;
}
//=====================================================================	
int
SpcTree::SelectSide( SpcObj const* pObj )
{
  int w = Width() >> 1, h = Height() >> 1, m = w > h ? left + w : top + h;
  return w > h ? pObj->right  < m ? 0 : pObj->left > m ? 1 : 2 :
                 pObj->bottom < m ? 0 : pObj->top  > m ? 1 : 2;
}
//=====================================================================
ResultCode
SpcTree::Load( char const* path, BranchPtr* pRoot )
{
  ifstream In( path, ios::in|ios::nocreate|ios::binary, filebuf::sh_none );
  ResultCode r = pRoot->Load( In );
  In.close();
  return r;
}
//=====================================================================
ResultCode
SpcTree::Save( char const* path, BranchPtr* pRoot )
{
  ofstream Ot( path, ios::out|ios::trunc|ios::binary, filebuf::sh_none );
  ResultCode r = pRoot->Save( Ot );
  Ot.close();
  return r;
}
//=====================================================================
ResultCode
SpcTree::Load( SpcTree** pRoot, ifstream& In )
{
  *pRoot = 0;

  if( !In.good())
    return R_BADSTREAM;

  long magic;
  In.read((char*)&magic, sizeof magic );
  switch( magic )
  {
    case MAGICPOOL : *pRoot = new Pool; break;
    case MAGICNEST : *pRoot = new Nest; break;
    case MAGICEMPT : return R_OK;      
    default        : return R_BADFILESTRUCT;
  }
  return *pRoot ? (*pRoot)->Load( In ) : R_OUTOFMEMORY;
}
//=====================================================================
ResultCode
SpcTree::Save( long magic, ofstream& Ot )
{
  Ot.write((char*)&magic, sizeof magic );
  return Ot.good() ? R_OK : R_BADSTREAM;
}
//=====================================================================
//=====================================================================
int
Nest::Init( SpcTree** ppPool )
{
  SetRect( **ppPool );
  for( SpcObj* pObj  = (*ppPool)->GetHead() ; pObj ; )
  {
    SpcObj* pNxt = pObj->pNext;
    Br[SelectSide( pObj )].AddObject( pObj, 0 );
    pObj = pNxt;
  }
  
  int r = 1;
  if( NoBranches( ppPool ))
    SetRect();
  else
	{
#ifdef _DEBUG
		nStackSz += 1;
#endif
    for( BranchPtr* pb = Br + NB ; --pb >= Br ; r &= pb->Split( pb->GetTreePtr()));
#ifdef _DEBUG
		if( !IsRectNull())
			nMaxStSz = max( nStackSz, nMaxStSz );
		nStackSz -= 1;
#endif
	}

  return r;
}
//=====================================================================
int
Nest::AddObject( SpcObj* pObj, SpcTree** )
{
 *this |= *pObj;
  return Br[SelectSide(pObj)].AddObject( pObj, 1 );
}
//=====================================================================
int
Nest::NoBranches( SpcTree** ppRoot )
{
  BranchPtr* sb;
  int n = 0;
  for( BranchPtr* pb = Br + NB ; --pb >= Br ; )
    if( !pb->IsRectNull())
      sb = pb, ++n;
  if( n > 1 )
    return 0;

  SpcTree** pFn = sb->GetTreePtr();
  *ppRoot = *pFn;
  *pFn = 0;
  return 1;
}
//=====================================================================
int
Nest::RemoveObject( SpcObj const* pObj, SpcTree** ppRoot )
{
  if( !IntersectRect( *pObj ))
    return 0;

  for( BranchPtr* pb = Br + NB ; --pb >= Br && !pb->RemoveObject( pObj ) ; );
  if( pb < Br )
    return 0;

  if( NoBranches( ppRoot ))
    delete this;
  else
  {
    SetRect();
    for( BranchPtr* pb = Br + NB ; --pb >= Br ; *this |= *pb );
  }  
  return 1;
}
//=====================================================================
int
Nest::ReplaceObject( SpcObj const* pObj )
{
  if( !IntersectRect( *pObj ))
    return 0;

  for( BranchPtr* pb = Br + NB ; --pb >= Br && !pb->ReplaceObject( pObj ) ; );
  return pb >= Br;
}
//=====================================================================
int
Nest::SelectObjects( SelectParam* pSp, int bInside )
{
  int n = 0;

#ifdef _DEBUG
	nStackSz += 1;
	nMaxStSz = max( nStackSz, nMaxStSz );
#endif

  if( bInside || ( bInside = Inside( pSp->Box )) || IntersectRect( pSp->Box ))
    for( BranchPtr* pb = Br + NB ; --pb >= Br ; )
    {
      int m = pb->SelectObjects( pSp, bInside );
      if( m < 0 )
        return m;
      n += m;
    }
  
#ifdef _DEBUG
  nStackSz -= 1;
#endif
  
	return n;
}
//=====================================================================
ResultCode
Nest::Save( ofstream& Ot )
{
  ResultCode r = SpcTree::Save( MAGICNEST, Ot );
  Ot.write((char*)&left, sizeof Rect );
  for( BranchPtr* pb = Br + NB ; --pb >= Br && r == R_OK ; r = pb->Save( Ot ));
  return r;
}
//=====================================================================
ResultCode
Nest::Load( ifstream& In )
{
#ifdef _DEBUG
	nStackSz += 1;
	nMaxStSz = max( nStackSz, nMaxStSz );
#endif

  In.read((char*)&left, sizeof Rect );
  ResultCode r = In.good() ? R_OK : R_BADSTREAM;
  for( BranchPtr* pb = Br + NB ; --pb >= Br && r == R_OK ; r = pb->Load( In ));
	
#ifdef _DEBUG
	nStackSz -= 1;
#endif
  
	return r;
}
//=====================================================================
//=====================================================================
Pool::Pool() :
  pPool( 0 ),
  PoolSize( 0 ) 
{}
//=====================================================================
Pool::~Pool()
{
  while( pPool )
  {
    SpcObj* p = pPool;
    pPool = p->pNext;
    delete p;
  }
}
//=====================================================================
int
Pool::ReplaceObject( SpcObj const* pObj )
{
  if( !IntersectRect( *pObj ))
    return 0;

  for( SpcObj* pOb = pPool ; pOb && pOb->Offset != pObj->Offset ; pOb = pOb->pNext );
  if( !pOb )
    return 0;

  pOb->MinLim = pObj->MinLim;
  pOb->MaxLim = pObj->MaxLim;
  pOb->Node   = pObj->Node;
  return 1;
}
//=====================================================================
int
Pool::RemoveObject( SpcObj const* pObj, SpcTree** ppRoot )
{
  if( !IntersectRect( *pObj ))
    return 0;

  for( SpcObj** ppObj = &pPool ; *ppObj && (*ppObj)->Offset != pObj->Offset ;
       ppObj = &(*ppObj)->pNext );

  if( !*ppObj )
    return 0;

  SpcObj* pDel = *ppObj;
  *ppObj = pDel->pNext;
  delete pDel;
  if( --PoolSize )
  {
    SetRect();
    for( pObj = pPool ; pObj ; *this |= *pObj, pObj = pObj->pNext );
  }
  else
  {
    *ppRoot = 0;
    delete this;
  }
  return 1;
}
//=====================================================================
int
Pool::Split( SpcTree** ppRoot )
{
  if( PoolSize > MAXPOOL )
  {
    Nest* pNest = new Nest;
    if( !pNest )
      return 0;

    if( !pNest->Init( ppRoot ))
      return 0;
    
    if( pNest->IsRectNull())
      delete pNest;
    else
      *ppRoot = pNest;

    pPool = 0;
    delete this;
  }
  return 1;
}
//=====================================================================
int
Pool::AddObject( SpcObj* pObj, SpcTree** ppRoot )
{
  *this |= *pObj;
  pObj->pNext = pPool;
  pPool = pObj;
  ++PoolSize;
  return ppRoot ? Split( ppRoot ) : 1;
}
//=====================================================================
int
Pool::SelectObjects( SelectParam* pSp, int bInside )
{
  int count = 0;
  if( bInside || ( bInside = Inside( pSp->Box )) || IntersectRect( pSp->Box ))
    for( SpcObj* pObj = pPool ; pObj ; pObj = pObj->pNext )
      if( IsValidNode( pObj->Node, pSp ) &&
          ( !pSp->Limit || ( !pObj->MinLim || pSp->Limit >= pObj->MinLim ) &&
                           ( !pObj->MaxLim || pSp->Limit <= pObj->MaxLim )) &&
          ( bInside || pObj->IntersectRect( pSp->Box )))
      {
        int n = pSp->pPosList->Insert( pObj->Offset );
        if( n < 0 )
          return n;
        count += n;
      }
  return count;
}
//=====================================================================
ResultCode
Pool::Load( ifstream& In )
{
  In.read((char*)&PoolSize, sizeof PoolSize );
  if( PoolSize )
    In.read((char*)&left, sizeof Rect );

  for( int n = PoolSize ; --n >= 0 ; )
  {
    if( In.eof())
      return R_EOFSTREAM;
    Rect rct;
    streampos off;
    int minLimit, maxLimit, node;
    In.read((char*)&rct.left, sizeof rct );
    In.read((char*)&off, sizeof off );
    In.read((char*)&minLimit, sizeof minLimit );
    In.read((char*)&maxLimit, sizeof maxLimit );
    In.read((char*)&node, sizeof node );
    pPool = new SpcObj( rct, off, minLimit, maxLimit, node, pPool );
    if( !pPool )
      return R_OUTOFMEMORY;
  }
  return In.good() ? R_OK : R_BADSTREAM;
}
//=====================================================================
ResultCode
Pool::Save( ofstream& Ot )
{
  SpcTree::Save( MAGICPOOL, Ot );
  Ot.write((char*)&PoolSize, sizeof( long ));
  if( PoolSize )
    Ot.write((char*)&left, sizeof Rect );

  for( SpcObj* pObj = pPool ; pObj ; pObj = pObj->pNext )
  {
    Ot.write((char*)&pObj->left,   sizeof Rect );
    Ot.write((char*)&pObj->Offset, sizeof( streampos ));
    Ot.write((char*)&pObj->MinLim, sizeof( int ));
    Ot.write((char*)&pObj->MaxLim, sizeof( int ));
    Ot.write((char*)&pObj->Node,   sizeof( int ));
  }
  return Ot.good() ? R_OK : R_BADSTREAM;
}
//=====================================================================
#ifdef _USE_HEAP_
int SpaceTree::nInst = 0;
#endif //_USE_HEAP_
//=====================================================================
SpaceTree::SpaceTree()
{
  *Path = 0;
#ifdef _USE_HEAP_
  if( nInst == 0 )
  {
    SpcTree::Heap = HeapCreate( HEAP_GENERATE_EXCEPTIONS|HEAP_NO_SERIALIZE, 32768, 0 );
    if( !SpcTree::Heap )
      cerr << endl << "HeapCreate failed!" << endl;
  }
  ++nInst;
#endif //_USE_HEAP_
}
//=====================================================================
SpaceTree::~SpaceTree()
{
  Clear();
#ifdef _USE_HEAP_
  if( --nInst == 0 )
    HeapDestroy( SpcTree::Heap );
#endif //_USE_HEAP_
}
//=====================================================================
char*
SpaceTree::GetPath()
{
  return Path;
}
//=====================================================================
int
SpaceTree::RemoveObject( Rect const& box, streampos off )
{
  SpcObj obj( box, off );
  return Root.RemoveObject( &obj );
}
//=====================================================================
int
SpaceTree::AddObject( Rect const& box, streampos off, 
                      int minLimit, int maxLimit, int Node, int bSort )
{
  SpcObj* pObj = new SpcObj( box, off, minLimit, maxLimit, Node );
  return pObj ? Root.AddObject( pObj, bSort ) : 0;
}
//=====================================================================
int
SpaceTree::ReplaceObject( Rect const& box, streampos off, 
                          int minLimit, int maxLimit, int Node )
{
  SpcObj obj( box, off, minLimit, maxLimit, Node );
  return Root.ReplaceObject( &obj );
}
//=====================================================================
int
SpaceTree::SelectObjects( Rect const& box, int limit, long* pNodes, PositionList& rPosList )
{
  SelectParam sp( box, limit, pNodes, &rPosList );
#ifdef _DEBUG
	nStackSz = nMaxStSz = 0;
#endif
  return box.IsRectNull() ? 0 : Root.SelectObjects( &sp, 0 );
}
//=====================================================================
ResultCode
SpaceTree::Save()
{
  return SpcTree::Save( Path, &Root );
}
//=====================================================================
ResultCode
SpaceTree::Load()
{
#ifdef _DEBUG
	nStackSz = nMaxStSz = 0;
#endif
  return SpcTree::Load( Path, &Root );
}
//=====================================================================
void 
SpaceTree::Clear()
{
  SpcTree** pTree = Root.GetTreePtr();
  delete *pTree;
  *pTree = 0;
}
//=====================================================================
int
SpaceTree::Sort()
{
#ifdef _DEBUG
	nStackSz = nMaxStSz = 0;
#endif
  return Root.Split( Root.GetTreePtr());
}
//=====================================================================
