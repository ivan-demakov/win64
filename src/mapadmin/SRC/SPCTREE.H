//=====================================================================
// файл "spctree.h"
// описание классов дл€ пространственного индексировани€
// графических данных
//=====================================================================
#ifndef _SPCTREE_H_
#define _SPCTREE_H_
//=====================================================================
#include <stdlib.h>
#include <fstream>
#include "rcode.h"
#include "ptrect.h"
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
struct SelectParam
{
  Rect          Box;      // габарит 
  int           Limit;    // масштаб
  long*         pNodes;   // список узлов
  PositionList* pPosList; // список указателей файла
  SelectParam( Rect b, int l, long* pn, PositionList* pp ) :
    Box( b ), Limit( l ), pNodes( pn ), pPosList( pp )
  {}
};
//=====================================================================
// абстракный базовай класс элементов пространственной структуры
class SpcTree : public Rect
{
public:
  virtual ~SpcTree(){}

  // сортировка 
  virtual int Split( SpcTree** ppRoot ) { return 1; }

  virtual SpcObj* GetHead() { return 0; }

  // выбор ветви
  int SelectSide( SpcObj const* pObj );

  // удаление объекта из структуры
  virtual int RemoveObject( SpcObj const *pObj, SpcTree** ppRoot ) = 0;

  // добавление объекта в структуру
  virtual int AddObject( SpcObj *pObj, SpcTree** ppRoot = 0 ) = 0;

  // изменение параметров отображени€
  virtual int ReplaceObject( SpcObj const* pObj ) = 0;

  // пространственное выделение группы объектов
  virtual int SelectObjects( SelectParam* pSp, int bInside ) = 0;

  // сохранение элемента пространственной структуры в файле
  virtual ResultCode Save( std::ofstream& Ot ) = 0;

  // загрузка элемента пространственной структуры из файла
  virtual ResultCode Load( std::ifstream& In ) = 0;

  // сохранение типа элемента
  static  ResultCode Save( long magic, std::ofstream& Ot );

  // сохранение полной структуры в заданном файле
  static ResultCode Save( char const* path, BranchPtr* pRoot );

  // загрузка полной структуры из заданного файле
  static ResultCode Load( char const* path, BranchPtr* pRoot );

  // загрузка типа и габарита элемента
  static ResultCode Load( SpcTree** pRoot, std::ifstream& In );

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
class SpcObj : public Rect
{
public:
  SpcObj( Rect const& box, std::streampos off,
          int minLimit = 0, 
          int maxLimit = 0, 
          int node = 0, 
          SpcObj* pNxt = 0 ) :
    Rect( box ),
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

  std::streampos Offset;  // смещение
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

  ResultCode Load(std::ifstream& In );
  ResultCode Save(std::ofstream& Ot);

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
  operator Rect()  { return pBranch ? *pBranch : Rect(); } 
  int IsRectNull() { return !pBranch || pBranch->IsRectNull(); }
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
  ResultCode Load(std::ifstream& In )
  {
    return SpcTree::Load( &pBranch, In );
  }
  ResultCode Save(std::ofstream& Ot )
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

private:
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

  ResultCode Load(std::ifstream& In );
  ResultCode Save(std::ofstream& Ot );

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
  int   AddObject( Rect const& box, std::streampos off,
                   int minLimit, int maxLimit, int node, int bSort );
  int   ReplaceObject( Rect const& box, std::streampos off,
                       int minLimit, int maxLimit, int node );
  int   RemoveObject( Rect const& box, std::streampos pos );
  int   SelectObjects( Rect const& box, int limit, long* pNodes, PositionList& rPosList );
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
