#ifndef __UNDO_H__
#define __UNDO_H__
//=====================================================================
#include "ident.h"
#include "drawobj.h"
#include "tpdef.h"
//=====================================================================
class CUndoScaleItem
{
public:
  CUndoScaleItem() {}
  CUndoScaleItem( CMapView* pView ) { Set( pView ); }
  virtual ~CUndoScaleItem() {}
  void Set( CMapView* pView );
  virtual int Play( CMapView* pView );
  virtual void SetList( CObjList const* pList, int bPlus ) {}
  virtual CObjList* GetList( int bPlus ) { return 0; }

  static void CPYL( CObjList* pOutList, CObjList const* pInpList );
  static void DELL( CObjList* pObjList );
  static int FINL( CDrawObject* pObj, CObjList* pObjList );

protected:
  CPoint m_Cntr;
  int    m_Scale;
};
//=====================================================================
class CUndoMoveItem : public CUndoScaleItem
{
public:
  CUndoMoveItem( CMapView* pView, CSize off ) :
    CUndoScaleItem( pView ),
    m_Offset( off )
  {}
  int Play( CMapView* pView );

private:
  CSize m_Offset;
};
//=====================================================================
class CUndoSpinItem : public CUndoScaleItem
{
public:
  CUndoSpinItem( CMapView* pView, CPoint center, int angle ) :
    CUndoScaleItem( pView ),
    m_Center( center ),
    m_Angle( angle )
  {}
  int Play( CMapView* pView );

private:
  int    m_Angle;
  CPoint m_Center;
};
//=====================================================================
class CUndoSelectItem : public CUndoScaleItem
{
public:
  CUndoSelectItem( CMapView* pView, CObjList const* pPlusList = 0, CObjList const* pMinusList = 0 );
  ~CUndoSelectItem();
  void SetList( CObjList const* pList, int bPlus );
  CObjList* GetList( int bPlus ) { return bPlus ? m_pPlusList : m_pMinusList; }
  void SwapLists();
  int Play( CMapView* pView );

protected:
  CObjList  m_List0;
  CObjList  m_List1;
  CObjList* m_pPlusList;
  CObjList* m_pMinusList;
};
//=====================================================================
//=====================================================================
class CUndoCopyItem : public CUndoSelectItem
{
public:
  CUndoCopyItem( CMapView* pView, CObjList const* pSrcList ) :
    CUndoSelectItem( pView, 0, pSrcList ),
    m_bCopy( 1 )
  {}
  int Play( CMapView* pView );

private:
  int m_bCopy;
};
//=====================================================================
class CUndoAddItem : public CUndoSelectItem
{
public:
  CUndoAddItem( CMapView* pView, CObjList const* pPlusList = 0, CObjList const* pMinusList = 0 ) :
    CUndoSelectItem( pView, pPlusList, pMinusList )
  {}
  CUndoAddItem( CMapView* pView, CDrawObject* pPlusObj, CDrawObject* pMinusObj );
  int Play( CMapView* pView );
};
//=====================================================================
//=====================================================================
class CUndoChangeItem : public CUndoAddItem
{
public:
  CUndoChangeItem( CMapView* pView, CDrawObject* pObj ) :
    CUndoAddItem( pView, pObj, 0 )
  {}
  int Play( CMapView* pView );
};
//=====================================================================
//=====================================================================
class CUndo
{
public:
  ~CUndo() { RemoveAll(); }
  void RemoveAll() { Purge( UndoList ); Purge( RedoList ); }
  void AddItem( CUndoScaleItem* pItem, CMapView* pView );
  void Undo( CMapView* pView ) { ExecUndo( UndoList, RedoList, pView ); }
  void Redo( CMapView* pView ) { ExecUndo( RedoList, UndoList, pView ); }
  int  IsRedo() { return !RedoList.IsEmpty(); }
  int  IsUndo() { return !UndoList.IsEmpty(); }

public:
  static long m_UndoMaxLength;

private:
  void ExecUndo( CUndoList& srcList, CUndoList& dstList, CMapView* pView );

private:
  static void Purge( CUndoList& list );

private:
  CUndoList UndoList;
  CUndoList RedoList;
};
//=====================================================================
#endif
