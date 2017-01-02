//=====================================================================
// файл "poslist.h"
//=====================================================================
#ifndef _POSLIST_H_
#define _POSLIST_H_
//=====================================================================
#include <iostream>
#include "t.h"


//=====================================================================
//#define _TREE_POSLIST_
#define _HASH_POSLIST_
//=====================================================================
#ifdef _TREE_POSLIST_
//=====================================================================
class PositionList
{
  struct CPosTreeItem
  {
    CPosTreeItem( streampos pos, CPosTreeItem* pPrnt ) :
      m_Pos( pos ),
      m_nBlnc( 0 ),
      m_pLeft( 0 ),
      m_pRght( 0 ),
      m_pPrnt( pPrnt )
    {}

    streampos m_Pos;
    int m_nBlnc;
    CPosTreeItem* m_pLeft;
    CPosTreeItem* m_pRght;
    CPosTreeItem* m_pPrnt;
  };

public:
  PositionList() : m_pRoot( 0 ), m_pCurr( 0 ) {}
  ~PositionList(){ RemoveAll(); }
  void RemoveAll();
  int Insert( streampos pos );
  int Remove( streampos pos );
  int Find( streampos pos );
  void SetFirst();
  void SetNext();
  streampos GetCurr();
  streampos GetNext();

private:
  static int BalanceLeft( CPosTreeItem** pRoot );
  static int BalanceRght( CPosTreeItem** pRoot );

private:
  CPosTreeItem* m_pRoot;
  CPosTreeItem* m_pCurr;
};
//=====================================================================
#endif
//=====================================================================
#ifdef _HASH_POSLIST_
//=====================================================================
class PositionList
{
#define MIN_SIZE 1024

public:
  PositionList();
  ~PositionList();
  void RemoveAll();
  int Insert( std::streampos pos );
  int Remove( std::streampos pos );
  int Find( std::streampos pos );
  void SetFirst();
  void SetNext();
  int  Empty() { return !m_nUsed; }
  std::streampos GetCurr();
  std::streampos GetNext();

private:
  std::streampos* FindPos( std::streampos pos );

private:
  std::streampos*  m_pTab;
  std::streampos*  m_pSrt;
  int         m_nCurr;
  int         m_nSize;
  int         m_nLimit;
  int         m_m0;
  int         m_nUsed;
};
//=====================================================================
#endif
//=====================================================================
//#define _LIST_FREELIST_   // список (размер-позиция)
//#define _SLIST_FREELIST_  // упорядоченный список (размер-позиция)
//#define _TLIST_FREELIST_  // упорядоченный список (размер-список позиций)
                            // с отдельным хранением последнего элемента 
//#define _ALIST_FREELIST_  // упорядоченный список (размер-массив позиций)
                            // с отдельным хранением последнего элемента 
//=====================================================================
#ifdef _LIST_FREELIST_
//=====================================================================
class FreeList // список удаленных объектов в файле
{
public:
  struct Elem // элемент списка
  {
    Elem( streampos pos, int size, Elem* pTail ) :
      Pos( pos ), Size( size ), pNext( pTail ) {}

    streampos Pos;
    int       Size;
    Elem*     pNext;
  };

public:
  FreeList() : pHead( 0 ), pCurr( 0 ) {}
  ~FreeList() { RemoveAll(); }
  void RemoveAll();
  int  Insert( streampos pos, int size );
  streampos FindFree( int size, streampos defPos );
  streampos GetStart();
  streampos GetNext();

private:
  Elem* pHead;
  Elem* pCurr;
};
//=====================================================================
#endif
//=====================================================================
#ifdef _TLIST_FREELIST_
//=====================================================================
class FreeList // список удаленных объектов в файле
{
public:

  struct PosItem // элемент списка позиций
  {
    streampos m_Pos;
    PosItem*  m_pNext;
    PosItem( streampos pos, PosItem* pNext ) : 
      m_Pos( pos ), m_pNext( pNext ) {}
  };

  struct SizeItem // элемент списка размеров
  {
    int       m_Size;
    SizeItem* m_pNext;
    PosItem*  m_pPos;
    SizeItem ( int size, PosItem* pPos, SizeItem* pNext ) :
      m_Size( size ), m_pPos( pPos ), m_pNext( pNext ) {}
  };

public:
  FreeList() : m_pHead( 0 ), m_pCurPos( 0 ), m_LastSize( 0 ) {}
  ~FreeList() { RemoveAll(); }
  void RemoveAll();
  int  Insert( streampos pos, int size );
  streampos FindFree( int size, streampos defPos );
  streampos GetStart();
  streampos GetNext();

private:
  void InsertLast();

private:
  SizeItem* m_pHead;
  SizeItem* m_pCurSize;
  PosItem*  m_pCurPos;
  int       m_LastSize;
  streampos m_LastPos;
};
//=====================================================================
#endif
//=====================================================================
#ifdef _SLIST_FREELIST_
//=====================================================================
class FreeList // список удаленных объектов в файле
{
  struct Elem // элемент списка
  {
    streampos Pos;
    int       Size;
    Elem*     pNext;
    Elem( streampos pos, int size, Elem* pTail ) :
      Pos( pos ), Size( size ), pNext( pTail ) {}
  };
  
  static Elem* Merge( Elem* h0, Elem* h1 );
  static Elem* Sort( Elem** ph, int l );

public:
  FreeList() : bSorted( 0 ), nLength( 0 ), pHead( 0 ), pCurr( 0 ) {}
  ~FreeList() { RemoveAll(); }
  void Sort();
  void RemoveAll();
  int  Insert( streampos pos, int size );
  streampos FindFree( int size, streampos defPos );
  streampos GetStart();
  streampos GetNext();

private:
  int   bSorted;
  int   nLength;
  Elem* pHead;
  Elem* pCurr;
};
//=====================================================================
#endif
//=====================================================================
#ifdef _ALIST_FREELIST_
//=====================================================================
class FreeList // список удаленных объектов в файле
{
#define MINSIZE 4
  struct SizeItem // элемент списка размеров
  {
    int        m_nSpace;
    SizeItem*  m_pNext;
    int        m_nSize;
    int        m_nUsed;
    streampos* m_pPos;
    SizeItem ( int space, SizeItem* pNext );
  };

public:
  FreeList() : m_pHead( 0 ), m_LastSpace( 0 ) {}
  ~FreeList() { RemoveAll(); }
  void RemoveAll();
  int  Insert( streampos pos, int space );
  streampos FindFree( int space, streampos defPos );
  streampos GetStart();
  streampos GetNext();

private:
  int InsertLast();

private:
  SizeItem* m_pHead;
  SizeItem* m_pCurSize;
  int       m_nCurPos;
  int       m_LastSpace;
  streampos m_LastPos;
};
//=====================================================================
#endif
//=====================================================================
#endif
//=====================================================================
