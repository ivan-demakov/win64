//=====================================================================
// פאיכ "objtree.h"
//=====================================================================
#ifndef _OBJTREE_H_
#define _OBJTREE_H_
//=====================================================================
#include "drawobj.h"
//=====================================================================
typedef CDrawObject* pObject;

class CObjHashTab
{
  struct TabDef
  {
    int m_nSize;
    int m_nUsed;
    pObject *m_pCur;
    pObject *m_pEnd;
    pObject *m_pSafe;
    pObject m_pBeg[0];
  };

public:
  CObjHashTab(): m_pTab( 0 ) {}  
  ~CObjHashTab(){ RemoveAll( 0 ); }
  int IsEmpty() { return !m_pTab || !m_pTab->m_nUsed; }
  int GetSize() { return m_pTab ? m_pTab->m_nUsed : 0; }
  void RemoveAll( CRect const* pClipRect, int bSaveSel = 0 );
  CDrawObject* AddObject( CDrawObject* pObj );
  CDrawObject* RemoveObject( EntryType id );
  CDrawObject* SetFirst();
  CDrawObject* GetFirst();
  CDrawObject* SetLast();
  CDrawObject* GetLast();
  CDrawObject* GetNext();
  CDrawObject* GetPrev();
  CDrawObject* Find( CTabIdent id );
  void SavePosition();
  void RestPosition();

private:
  pObject* FindPos( UINT id, int bFree = 0 );
  static int IsObjPtr( pObject p ) { return p && p != (pObject)-1; }

private:
  TabDef* m_pTab;
};
typedef CObjHashTab CObjTreeList;
//=====================================================================
#endif
