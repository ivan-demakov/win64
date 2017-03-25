//=====================================================================
// פאיכ "idntreeb.h"
//=====================================================================
#ifndef _IDNTREE_H_
#define _IDNTREE_H_
//=====================================================================
#include <stdlib.h>
#include <fstream>
#include "t.h"
#include "rcode.h"
#include "filestr.h"
#include "ident.h"
//=====================================================================
#define MAGICNEST 0x5453454eL
//=====================================================================
//#define _OLD_IDENT_TREE_
//#define _NEW_IDENT_TREE_
#define _DIRECT_HASH_IDENT_TAB
#define _LINKED_HASH_IDENT_TAB
//=====================================================================
#ifdef _NEW_IDENT_TREE_
class IdentTree
{
  class IdentTreeItem
  {
  public:
    IdentTreeItem( CTabIdent const& id = 0, streampos pos = 0, IdentTreeItem* pPrnt = 0 );

    CTabIdent      m_Id;
    streampos      m_Pos;
    char           m_nBlnc;
    IdentTreeItem* m_pLeft;
    IdentTreeItem* m_pRght;
    IdentTreeItem* m_pPrnt;
  };

public:
  IdentTree();
  ~IdentTree() { Clear(); }
  int RemoveObject( CTabIdent const& id );
  int AddObject( CTabIdent const& id, streampos pos );
  streampos SelectObject( CTabIdent const& id );
  EntryType GetUniId(); 
  void Clear();
  char* GetPath();

private:
  static int BalanceLeft( IdentTreeItem** pRoot );
  static int BalanceRght( IdentTreeItem** pRoot );

private:
  char m_Path[_MAX_PATH];
  IdentTreeItem* m_pRoot;
  EntryType m_nLastUni; 
};
//=====================================================================
#endif
//=====================================================================
#ifdef _OLD_IDENT_TREE_
//=====================================================================
class IdnNest
{
public:
  IdnNest( CTabIdent const& id, streampos pos, int blnc );
  IdnNest(){}
  ~IdnNest();

  static int AddObject( IdnNest** pRoot, CTabIdent const& id, streampos pos );
  static int RemoveObject( IdnNest** pRoot, CTabIdent const& id );
  static streampos SelectObject( IdnNest*p, CTabIdent const& id );

#ifdef _DEBUG
  void Show( int cp, int dp );
#endif

private:
  static int BalanceLeft( IdnNest** pRoot );
  static int BalanceRight( IdnNest** pRoot );
  static int Remove( IdnNest** pRoot, IdnNest* q );

private:
  CTabIdent Id;
  streampos Pos;
  char      Blnc;
  IdnNest*  pLeft;
  IdnNest*  pRight;
};
//=====================================================================
class IdentTree
{
public:
  IdentTree();
  ~IdentTree();
  int RemoveObject( CTabIdent const& id );
  int AddObject( CTabIdent const& id, streampos pos );
  streampos SelectObject( CTabIdent const& id );
  EntryType GetUniId();
  void Clear();
  char* GetPath();

#ifdef _DEBUG
  void Show();
#endif

private:
  IdnNest* Root;
  EntryType m_nLastUni; 
};
//=====================================================================
#endif
//=====================================================================
#ifdef _DIRECT_HASH_IDENT_TAB
//=====================================================================
using namespace std;

class IdentDirectHashTab
{
  friend class MapUpdater;

public:
  class IdentItem
  {
  public:
    void Set( CTabIdent const& id, streampos pos ) { m_Id = id; m_Pos = pos; }
    int Free() { return m_Pos == (streampos)0; }
    int Used() { return m_Pos > 0; }
    CTabIdent m_Id;
    streampos m_Pos;
  };

public:
  IdentDirectHashTab();
  ~IdentDirectHashTab() { Clear(); }
  int RemoveObject( CTabIdent const& id );
  int AddObject( CTabIdent const& id, streampos pos );
  streampos SelectObject( CTabIdent const& id );
  EntryType GetUniId(); 
  void Clear();

  int  GetSize() { return m_nSize; }

private:
  IdentItem* FindPos( CTabIdent const& id, int bUsed = 0 );

private:
  IdentItem* m_pTab;
  int        m_nSize;
  int        m_nUsed;
  int        m_nLimit;
  int        m_nMask;
  EntryType  m_nLastUni; 
};
//=====================================================================
#endif
//=====================================================================
#ifdef _LINKED_HASH_IDENT_TAB
//=====================================================================
class IdentLinkedHashTab
{
public:
  class IdentItem
  {
  public:
    CTabIdent  m_Id;
    streampos  m_Pos;
    IdentItem* m_pNext;

    IdentItem( CTabIdent const& id, streampos pos, IdentItem* next ):
      m_Id( id ), m_Pos( pos ), m_pNext( next )
      {}

  };
  typedef IdentItem* pItem;

public:
  IdentLinkedHashTab();
  ~IdentLinkedHashTab() { Clear(); }
  int RemoveObject( CTabIdent const& id );
  int AddObject( CTabIdent const& id, streampos pos );
  streampos LastReplaced() { return m_LastReplaced; }
  streampos SelectObject( CTabIdent const& id );
  EntryType GetUniId( int t ); 
  void Clear();

private:
  IdentItem** FindPos( CTabIdent const& id );

private:
  pItem*    m_pTab;
  pItem*    m_pHead;
  int       m_nSize;
  int       m_nUsed;
  int       m_nLinks; 
  EntryType m_nLastUni; 
  streampos m_LastReplaced;
};
//=====================================================================
#endif
//=====================================================================
#endif
