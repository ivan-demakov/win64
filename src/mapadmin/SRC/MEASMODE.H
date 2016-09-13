#ifndef __MEGMODE_H__
#define __MEGMODE_H__
//=====================================================================
#include "appmode.h"
#include "cspot.h"
#include "ids.h"
//=====================================================================
enum
{
  MAX_POINT_NUM = 65535,
  ER = 4
};

class CMeasureMode : public CAppMode
{

public:
  CMeasureMode( UINT nID ) :
    CAppMode( AIDG_MEAGURE, IDC_DRAW,
              AM_FCHAR | AM_FMOUSEMOVE | AM_FLBDOWN | AM_FRBDOWN | AM_FMONOPOLY | AM_FUNIMODE,
              nID ),
    m_bSelect( nID == IDM_SELECT_POLY_MODE || 
               nID == IDM_UNRECTPLAN_MODE ),    
    m_bSquare( m_bSelect || 
               nID == IDM_SQUARE_MEASURE_MODE ),
    m_bDrawFinal( 0 ),
		m_bPolylineFound( 0 ),
    m_pPoly( m_Polyline ),
    m_nPointNum( 0 ),
    m_ClipRect( 0, 0, 0, 0 )
  {
    m_nMode = 1;
  }

  int  OnLButtonDown( UINT &flags, CPoint &loc );
  int  OnMouseMove( UINT &flags, CPoint &loc );
  int  OnRButtonDown( UINT &flags, CPoint &loc );
  int  OnChar( UINT& flags, UINT& key_code );
  int  Term( int bDestroy );
  void Draw( CDC* pDC );
  void SaveRegion();
	void PutHint( int bDraw );
  void ResetClipRect() { m_ClipRect.SetRectEmpty(); }
  void CalcClipRect( CPoint p0, CPoint p1 );
  int  OnSetCursor();
  static int  Restore( CMapView* pView );
  static void DrawSegments( CDC* pDC, CPoint* lp, int n );

protected:
  void   ClearAll();
  void   DrawLoop( CDC* pDC, CPoint* pp, int pn );
  virtual void   CalcSum();
	int    SaveSelectPoly();
  double CalcSquare();

protected:
  int     m_bSelect;
  int     m_bSquare;
  int     m_bDrawFinal;
  int     m_bPolylineFound;
  CPoint  m_CurPoint;
  CPoint  m_TmpPoly[MAX_POINT_NUM+1];
  CIdent  m_ExtId;
  int     m_nPointNum;
  CPoint* m_pPoly;
  CPoint  m_Polyline[MAX_POINT_NUM+1];
  CRect   m_ClipRect;
	double  m_Length;
	double  m_Square;
};
//=====================================================================
class CLinkMeasureMode : public CMeasureMode
{
public:
	CLinkMeasureMode() : CMeasureMode( IDM_LINE_MEASURE_MODE )			
	{
	} 

  int  OnLButtonDown( UINT &flags, CPoint &loc );
  int  OnMouseMove( UINT &flags, CPoint &loc );
  int  OnRButtonDown( UINT &flags, CPoint &loc );
  void CalcSum();
};
//=====================================================================
#ifdef _TabIdentHashTab_
class TabIdentHashTab
{
public:
  class IdentItem
  {
  public:
    CTabIdent  m_Id;
    IdentItem* m_pNext;

    IdentItem( CTabIdent const& id, IdentItem* next ):
      m_Id( id ), m_pNext( next )
    {}

  };
  typedef IdentItem* pItem;

public:
  TabIdentHashTab();
  ~TabIdentHashTab() { Clear(); }
  int Remove( CTabIdent const& id );
  int Add( CTabIdent const& id );
  int Find( CTabIdent const& id );
	int GetCount() { return m_nUsed; }
  void Clear();

private:
  IdentItem** FindPos( CTabIdent const& id );

private:
  pItem*    m_pTab;
  pItem*    m_pHead;
  int       m_nSize;
  int       m_nUsed;
  int       m_nLinks; 
};
#endif // _TabIdentHashTab_
//=====================================================================
class CFindDistanceMode : public CAppMode
{
public:
	CFindDistanceMode() : 
    CAppMode( AIDG_OBJACTIONMODE, IDC_SELECTTRACE,
              AM_FUNIMODE | AM_FLBDOWN | AM_FRBDOWN | AM_FESCMODE,
              IDM_FIND_DIST_MODE ),
		m_nPointNum( 0 ),
		m_pSolution( 0 ),
		m_nSolution( 0 )
  {}

  void Draw( CDC* pDC );
  int Open();
  int Term( int bDestroy );
  int OnLButtonDown( UINT &flags, CPoint &loc );
  int OnRButtonDown( UINT &flags, CPoint &loc );

private:
#ifdef _TabIdentHashTab_
	int FindPoint( CDrawObject* pObj0, TabIdentHashTab& IdList );
#else
	int FindPoint( CDrawObject* pObj0, CTabIdList& IdList );
#endif // _TabIdentHashTab_
  
private:
	CTabIdent m_Id0;
	CTabIdent m_Id1;
  int       m_nPointNum;
  CPoint    m_Polyline[MAX_POINT_NUM+1];
	CPoint*   m_pSolution;
	int       m_nSolution;
	int       m_SolutionSize;
	int       m_Dist;
};
//=====================================================================
#endif