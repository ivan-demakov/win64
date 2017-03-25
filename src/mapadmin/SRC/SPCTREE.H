//=====================================================================
// файл "spctree.h"
// описание классов дл€ пространственного индексировани€
// графических данных
//=====================================================================
#ifndef _SPCTREE_H_
#define _SPCTREE_H_
//=====================================================================
#include <stdlib.h>
//#include <fstream.h>
#include "rcode.h"
//#include "ptrect.h"
#include "poslist.h"
#include "ident.h"
//=====================================================================
#define MAGICNEST 0x5453454eL // код начала записи гнезда
#define MAGICPOOL 0x4c4f4f50L // код начала записи списка
#define MAGICEMPT 0x54504D45L // код записи пустой ветви
#define MAXPOOL 32            // максимальный размер неделимого списка
#define NB 3                  // количество ветвей в узле
//=====================================================================
//#define _USE_HEAP_
//=====================================================================
class SpcObj;
class BranchPtr;
//=====================================================================
struct SelectParam : public RECT // габарит 
{
  int           Limit;    // масштаб
  long*         pNodes;   // список узлов
  PositionList* pPosList; // список указателей файла
  SelectParam( RECT b, int l, long* pn, PositionList* pp ) :
    Limit( l ), pNodes( pn ), pPosList( pp )
  {
    top  = b.top;
    left = b.left;
    right = b.right;
    bottom = b.bottom;  
  }
  int IsRectNull() { return left == 0 && top == 0 && right == 0 && bottom == 0; }
};
//=====================================================================
// абстракный базовый класс элементов пространственной структуры
class SpcTree : public RECT // габарит 
{
public:
  SpcTree( RECT b )
  {
    top  = b.top;
    left = b.left;
    right = b.right;
    bottom = b.bottom;  
  }
  SpcTree() { SetBoxNull(); }
  int  IsBoxNull() { return left == 0 && top == 0 && right == 0 && bottom == 0; }
  int  InsideBox( RECT const& b );
  int  IntersectBox( RECT const& b );
  void SetBoxNull() { left = top = right = bottom = 0; }
  void CopyBox( SpcTree* pItem );
  void AddBox( RECT const& b );

public:
  virtual ~SpcTree(){}

  // сортировка 
  virtual int Split( SpcTree** ppRoot ) { return 1; }

  virtual SpcObj* GetHead() { return 0; }

  // выбор ветви
  int SelectSide( SpcObj const* pObj );

  // удаление объекта из структуры
  virtual int RemoveObject( SpcObj const *pObj, SpcTree** ppRoot ) { return 0; }

  // добавление объекта в структуру
  virtual int AddObject( SpcObj *pObj, SpcTree** ppRoot = 0 ) { return 0; }

  // изменение параметров отображени€
  virtual int ReplaceObject( SpcObj const* pObj ) { return 0; }

  // пространственное выделение группы объектов
  virtual int SelectObjects( SelectParam* pSp, int bInside ) { return 0; }

  // сохранение элемента пространственной структуры в файле
  virtual ResultCode Save( ofstream& Ot ) { return R_OK; }

  // загрузка элемента пространственной структуры из файла
  virtual ResultCode Load( ifstream& In ) { return R_OK; }

  // сохранение типа элемента
  static  ResultCode Save( long magic, ofstream& Ot );

  // сохранение полной структуры в заданном файле
  static ResultCode Save( char const* path, BranchPtr* pRoot );

  // загрузка полной структуры из заданного файле
  static ResultCode Load( char const* path, BranchPtr* pRoot );

  // загрузка типа и габарита элемента
  static ResultCode Load( SpcTree** pRoot, ifstream& In );

  static int IsValidNode( int Node, SelectParam* pSp );

#ifdef _USE_HEAP_
  static void* operator new( size_t sz )
    { return HeapAlloc( Heap, HEAP_GENERATE_EXCEPTIONS|HEAP_NO_SERIALIZE, sz ); }
  static void* operator new( size_t sz, LPCSTR, int )
    { return HeapAlloc( Heap, HEAP_GENERATE_EXCEPTIONS|HEAP_NO_SERIALIZE, sz ); }
  static void operator delete( void* p )
    { HeapFree( Heap, HEAP_NO_SERIALIZE, p ); }
  static void operator delete( void* p, LPCSTR, int )
    { HeapFree( Heap, HEAP_NO_SERIALIZE, p ); }
  static HANDLE Heap;
#endif
};
//=====================================================================
// элемент списка пространственных описателей
class SpcObj : public SpcTree
{
public:
  SpcObj( RECT const& b, 
          streampos off,
          int minLimit = 0, 
          int maxLimit = 0, 
          int node = 0, 
          SpcObj* pNxt = 0 ) :
    SpcTree( b ),
    Offset( off ),
    MinLim( minLimit ), 
    MaxLim( maxLimit ),
    Node( node ),
    pNext( pNxt )
  {}

#ifdef _USE_HEAP_
  static void* operator new( size_t sz )
    { return SpcTree::operator new( sz ); }
  static void* operator new( size_t sz, LPCSTR, int )
    { return SpcTree::operator new( sz ); }
  static void operator delete( void* p )
    { SpcTree::operator delete( p ); }
  static void operator delete( void* p, LPCSTR, int )
    { SpcTree::operator delete( p ); }
#endif //_USE_HEAP_

  streampos Offset;  // смещение
  int       MinLim;  // нижн€€  граница отображени€
  int       MaxLim;  // верхн€€ граница отображени€
  int       Node;    // код узла
  SpcObj*   pNext;   // следующий в списке
};
//=====================================================================
// элемент пространственной структуры - список
class Pool : public SpcTree
{
public:
  Pool();
  ~Pool();
  int AddObject( SpcObj* pObj, SpcTree** ppRoot );
  int ReplaceObject( SpcObj const* pObj );
  int RemoveObject( SpcObj const *pObj, SpcTree** ppRoot );
  int SelectObjects( SelectParam* pSp, int bInside );
  int Split( SpcTree** ppRoot );
  SpcObj* GetHead() { return pPool; }

  ResultCode Load( ifstream& In );
  ResultCode Save( ofstream& Ot);

private:
  int PoolSize;  // размер списка
  SpcObj* pPool; // голова списка
};
//=====================================================================
class BranchPtr
{
public:
  BranchPtr() : pBranch( 0 ) {}
  ~BranchPtr() { delete pBranch; }
  int IsBoxNull() { return !pBranch || pBranch->IsBoxNull(); }
  int Split( SpcTree** ppRoot )
  {
    return pBranch ? pBranch->Split( ppRoot ) : 1;
  }
  int AddObject( SpcObj* pObj, int bSort )
  {
    return pBranch || ( pBranch = new Pool ) ? pBranch->AddObject( pObj, bSort ? &pBranch : 0 ): 0;
  }
  int ReplaceObject( SpcObj const* pObj )
  { 
    return pBranch ? pBranch->ReplaceObject( pObj ) : 0; 
  }
  int RemoveObject( SpcObj const *pObj )
  { 
    return pBranch ? pBranch->RemoveObject( pObj, &pBranch ) : 0; 
  }
  int SelectObjects( SelectParam* pSp, int bInside )
  { 
    return pBranch ? pBranch->SelectObjects( pSp, bInside ) : 0; 
  }
  SpcTree** GetTreePtr() 
  { 
    return &pBranch; 
  }
  ResultCode Load( ifstream& In )
  {
    return SpcTree::Load( &pBranch, In );
  }
  ResultCode Save( ofstream& Ot )
  {
    return pBranch ? pBranch->Save( Ot ) : SpcTree::Save( MAGICEMPT, Ot );
  }

#ifdef _USE_HEAP_
  static void* operator new( size_t sz )
    { return SpcTree::operator new( sz ); }
  static void* operator new( size_t sz, LPCSTR, int )
    { return SpcTree::operator new( sz ); }
  static void operator delete( void* p )
    { SpcTree::operator delete( p ); }
  static void operator delete( void* p, LPCSTR, int )
    { SpcTree::operator delete( p ); }
#endif //_USE_HEAP_

public:
  SpcTree* pBranch;
};
//=====================================================================
// элемент пространственной структуры - узел
class Nest : public SpcTree
{
public:
  Nest() {}
  int Init( SpcTree** ppPool );
  int AddObject( SpcObj* pObj, SpcTree** ppRoot );
  int ReplaceObject( SpcObj const* pObj );
  int RemoveObject( SpcObj const* pObj, SpcTree** ppRoot );
  int SelectObjects( SelectParam* pSp, int bInside );
  int NoBranches( SpcTree** ppRoot );

  ResultCode Load( ifstream& In );
  ResultCode Save( ofstream& Ot );

private:
  BranchPtr Br[NB]; // ветви
};
//=====================================================================
// пространственна€ структура
class SpaceTree
{
public:
  SpaceTree();
  ~SpaceTree();
  int   AddObject( RECT const& box, streampos off,
                   int minLimit, int maxLimit, int node, int bSort );
  int   ReplaceObject( RECT const& box, streampos off,
                       int minLimit, int maxLimit, int node );
  int   RemoveObject( RECT const& box, streampos pos );
  int   SelectObjects( RECT const& box, int limit, long* pNodes, PositionList& rPosList );
  void  Clear();
  int   Sort();
  char* GetPath();

  ResultCode Load();
  ResultCode Save();

private:
  char Path[_MAX_PATH];
  BranchPtr Root;  // корневой элемент
#ifdef _USE_HEAP_
  static int nInst;
#endif //_USE_HEAP_
};
//=====================================================================
#endif
