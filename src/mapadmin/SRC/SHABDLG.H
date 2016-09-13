#ifndef _SHABDLG_H_
#define _SHABDLG_H_

#include <strstrea.h>
#include <afxtempl.h>
#include "ids.h"
#include "dlgids.h"
#include "lomedit.h"
//=====================================================================
#define MAX_LINE_WIDTH 32
#define MIN_TMPL_INDEX 10
#define MAX_TMPL_PERIOD 320
//=====================================================================
enum SD_MODE
{
  NO_MODE,
  INSERT_POINT_MODE,
  INSERT_POLY_MODE,
  INSERT_ARC_MODE,
  INSERT_SPLINE_MODE,
  MOVE_POLY_MODE,
  DELETE_POLY_MODE,
  MOVE_POINT_MODE,
  DELETE_POINT_MODE
};
//=====================================================================
class CShabDlg;
//=====================================================================
struct SD_POINT
{
  CPoint    point;
  SD_POINT* pNextPoint;
  SD_POINT( CPoint pnt, SD_POINT* nxt = 0 ) : point( pnt ), pNextPoint( nxt ) {}
  void Change( CPoint pnt ) { point = pnt; }
  int Dist( CPoint pnt ) { return abs( pnt.x - point.x ) + abs( pnt.y - point.y ); }
};
//=====================================================================
struct SD_POLY
{
  int width;
  int length;
  SD_POINT* pFirstPoint;
  SD_POLY*  pNextPoly;
  SD_POLY( int wd = 0, SD_POLY* nxt = 0 ) :
    pFirstPoint( 0 ),
    pNextPoly( nxt ),
    width( max( wd, 1 )),
    length( 0 )
  {}
  ~SD_POLY();

  void CopyPoints( SD_POLY* pPoly );
  void KillPoints();
  SD_POLY* CopyPoly();
  void RemoveDuplicates();
  void Offset( int dx, int dy );
  CRect CalcBoundBox( int bSel = 0 );
};
//=====================================================================
class CExampleBox : public CWnd
{
  DECLARE_DYNCREATE( CExampleBox )

public:
  void OnPaint();
  void OnLButtonDown( UINT nFlags, CPoint point );
  void OnLButtonUp( UINT nFlags, CPoint point );
  void OnMouseMove( UINT nFlags, CPoint point );
  void OnRButtonDown( UINT nFlags, CPoint point );
  void SetWndCursor( UINT id );
  int  OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );

private:
  CShabDlg* GetParent() { return (CShabDlg*)CWnd::GetParent(); }

private:
  HCURSOR m_hWndCursor;

  DECLARE_MESSAGE_MAP()
};
//======================================================================
class UndoItem
{
public:
  virtual ~UndoItem() {}
  virtual void Play( CShabDlg* pDlg ) = 0;  
public:  
  UndoItem* m_pNextItem;
};
//=====================================================================
class MovePointItem : public UndoItem
{
public:
  MovePointItem( SD_POINT* pPoint, SD_POLY* pCurPoly ) :
    m_pCurPoly( pCurPoly ),
    m_pPoint( pPoint ),
    m_Pnt( pPoint->point )
  {}
  void Play( CShabDlg* pDlg );
private:
  CPoint    m_Pnt;
  SD_POINT* m_pPoint;
  SD_POLY*  m_pCurPoly;
};
//=====================================================================
class InsertPointItem : public UndoItem
{
public:
  InsertPointItem( int bInsert, SD_POINT* pPoint, SD_POINT** ppPrvPoint, SD_POLY* pCurPoly ) :
    m_bInsert( bInsert ),
    m_pCurPoly( pCurPoly ),
    m_ppPrvPoint( ppPrvPoint ),
    m_pPoint( pPoint )
  {}
  ~InsertPointItem();
  void Play( CShabDlg* pDlg );  
private:
  int        m_bInsert;
  SD_POINT** m_ppPrvPoint;
  SD_POINT*  m_pPoint;
  SD_POLY*   m_pCurPoly;
};
//=====================================================================
class InsertPolyItem : public UndoItem
{
public:
  InsertPolyItem( int bInsert, SD_POLY** ppPrvPoly, SD_POLY* pPoly ) :
    m_bInsert( bInsert ),
    m_ppPrvPoly( ppPrvPoly ),
    m_pPoly( pPoly )
  {}
  ~InsertPolyItem(); 
  void Play( CShabDlg* pDlg );  
private:
  int       m_bInsert;
  SD_POLY*  m_pPoly;
  SD_POLY** m_ppPrvPoly;
};
//======================================================================
class MovePolyItem : public UndoItem
{
public:
  MovePolyItem( CSize delta, SD_POLY* pCurPoly ) :
    m_pCurPoly( pCurPoly ),
    m_Delta( delta )
  {}
  void Play( CShabDlg* pDlg );
private:
  CSize    m_Delta;
  SD_POLY* m_pCurPoly;
};
//=====================================================================
class CShabDlg : public CDialog
{
  DECLARE_DYNCREATE( CShabDlg )

  friend class CExampleBox;
  friend class InsertPointItem;
  friend class MovePointItem;
  friend class InsertPolyItem;
  friend class MovePolyItem;

  struct Template
  {
    Template() : m_pFstPoly( 0 ) {}
    Template( SD_POLY* pPoly, int ndx, int xs, int ys, int pr, int sf ) :
      m_pFstPoly( pPoly->CopyPoly()),
      m_nIndex( ndx ),
      m_xS( xs ),
      m_yS( ys ),
      m_Period( pr ),
      m_nScale( sf )
    {}
    ~Template();
    int m_nIndex;
    int m_xS;
    int m_yS;
    int m_Period;
    int m_nScale;
    SD_POLY* m_pFstPoly;
  };

  class CTmplListBox : public CListBox
  {
  public:
    void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
    int  CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct );
  private:
    CShabDlg* GetParent() { return (CShabDlg*)CWnd::GetParent(); }
  };

public:
  CShabDlg();
  ~CShabDlg();

  int  OnInitDialog();
  void Activate( CWnd* pView );
  void OnOK();
  void OnCancel();
  void RemoveAll();

  void AddUndo( UndoItem* pItem );
  void RemoveUndo( UndoItem** ppItem );
  void Play( UndoItem** ppSrcHead, UndoItem** ppDstHead );
  void OnUndoButtonClicked() { Play( &m_UndoHead, &m_RedoHead ); }
  void OnRedoButtonClicked() { Play( &m_RedoHead, &m_UndoHead ); }
  void OnStyleChange();

  void OnResetButtonClicked();
  void OnClearButtonClicked();
  void OnRemoveTmplButtonClicked();
  void OnEditButtonClicked();
  void OnSaveButtonClicked();
  void OnModeButtonClicked();
  void OnGridClicked();

  void OnLButtonDown( CPoint point );
  void OnLButtonUp( CPoint point );
  void OnMouseMove( CPoint point );
  void OnSelect( CPoint point );

  void AddPoint( CPoint pnt );
  void ChangePoint( CPoint pnt );
  void StopMode();

  void DrawPoly( SD_POLY* pPoly = 0, int bFull = 1 );
  void DrawAll();
  void ShowCoords( CPoint pnt );
  void UpdateCoord( CPoint& pnt );
  void SelectPoly( SD_POLY* pPoly );
  void ApplyNewPeriod();
  void ApplyNewWidth();
  void MakeTmpPoly( char const* PrcName );

  int  FindPoly( CPoint pnt );
  int  FindPoint( CPoint pnt );
  int  GetNewIndex();
  int  GetOX() { return m_ExRect.Height() / 2; }
  int IsActive() { return m_bActive; }
  Template* GetTmpl( int nIndex );

  SD_MODE GetMode() { return m_nMode; }

private:
  int         m_bActive; 
  CString      m_DlgCap;
  CExampleBox* m_pExample;
  CTmplListBox m_List;
  CRect        m_ExRect;
  CPoint       m_TracePoint;
  CPoint       m_StartPoint;
  SD_MODE      m_nMode;
  SD_POLY      m_TmpPoly;
  SD_POLY*     m_pFstPoly;
  SD_POLY*     m_pCurPoly;
  SD_POLY**    m_ppPrvPoly;
  SD_POINT*    m_pCurPoint;
  SD_POINT**   m_ppPrvPoint;
  int          mb_Changing;
  int          m_nIndex;
  int          m_bMod;
  long         m_Scale;
  long         m_Width;
  long         m_Period;
  long         m_GridStep;
  CIntEdit     m_ScaleEdit;
  CIntEdit     m_WidthEdit;
  CIntEdit     m_PeriodEdit;
  CIntEdit     m_GridStepEdit;

  UndoItem*    m_UndoHead;
  UndoItem*    m_RedoHead;

  CArray< Template*, Template* > m_TmplLib;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
#endif