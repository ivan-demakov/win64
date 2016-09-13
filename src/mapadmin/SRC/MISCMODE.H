#ifndef __MISCMODE_H__
#define __MISCMODE_H__

#include "mview.h"
#include "rectmode.h"
#include "appmode.h"
#include "scale.h"
#include "cspot.h"
#include "ids.h"
#include "idg.h"
//=====================================================================
class CBoundingMode : public CAppMode
{
public:
  CBoundingMode( int xBnd, int yBnd ):
    CAppMode( AIDG_BOUNDING, 0, AM_FUNIMODE, 0 ),
    m_Bnd( xBnd, yBnd )
    {}
  int OnLButtonDown( UINT& flags, CPoint& loc ){ return OnMouseEvent( flags, loc ); }
  int OnRButtonDown( UINT& flags, CPoint& loc ){ return OnMouseEvent( flags, loc ); }
  int OnLButtonUp( UINT& flags, CPoint& loc ){ return OnMouseEvent( flags, loc ); }
  int OnRButtonUp( UINT& flags, CPoint& loc ){ return OnMouseEvent( flags, loc ); }
  int OnMouseMove( UINT& flags, CPoint& loc ){ return OnMouseEvent( flags, loc ); }
  int OnMouseEvent( UINT& flags, CPoint& loc );

private:
  CSize m_Bnd;
};
//=====================================================================
class CCrossMode : public CAppMode
{
public:
  CCrossMode() :
    CAppMode( AIDG_CROSSMODE, 0, AM_FUNIMODE | AM_FMOUSEMOVE, IDM_CROSS_MODE ),
    m_CurrPoint( -1, -1 )
  {
    m_nMode = 1;
  }
  int OnMouseMove( UINT& flags, CPoint& loc )
  {
    m_CurrPoint = loc;
    return AM_RPROPAGATE;
  }
  static void Draw( CDC* pDC, CMapView* pView, CPoint pnt );
  void Draw( CDC* pDC ) { Draw( pDC, m_pOwner, m_CurrPoint ); }

private:
  CPoint m_CurrPoint;
};
//=====================================================================
class CReisMode : public CAppMode
{
public:
  CReisMode() :
    CAppMode( AIDG_REISSHINA, 0, AM_FUNIMODE, IDM_REISSHINA_MODE )
  {}
  ~CReisMode() { m_nActive = 0; }
  int  OnLButtonDown( UINT& flags, CPoint& loc ){ return OnButtonDown( flags, loc ); }
  int  OnRButtonDown( UINT& flags, CPoint& loc ){ return OnButtonDown( flags, loc ); }
  int  OnLButtonUp( UINT& flags, CPoint& loc ){ return OnButtonUp( flags, loc ); }
  int  OnRButtonUp( UINT& flags, CPoint& loc ){ return OnButtonUp( flags, loc ); }
  int  OnMouseMove( UINT& flags, CPoint& loc );
  static void SetPrevPoint( CPoint p );
  static void SetActive( int bActive, int bFirstPointDefined = 0 );

private:
  int  OnButtonDown( UINT& flags, CPoint& loc );
  int  OnButtonUp( UINT& flags, CPoint& loc );
  void UpdatePoint( CPoint& loc );

private:
  static CPoint m_PrevPoint;
  static int    m_nActive;
};
//---------------------------------------------------------------------
class CSetReisMode : public CAppMode
{
public:
  CSetReisMode() :
    CAppMode( AIDG_SETREISSHINA, IDC_TURN, AM_FUNIMODE | AM_FMONOPOLY | AM_FALL,
              IDM_SETREISSHINA_MODE )
  {}
  int  Term( int );
  int  Open();
  int  OnLButtonDown( UINT& flags, CPoint& loc );
  int  OnLButtonDblClk( UINT& flags, CPoint& loc );
  int  OnRButtonDown( UINT& flags, CPoint& loc );
  int  OnMouseMove( UINT& flags, CPoint& loc );
  int  OnChar( UINT& flags, UINT& key_code );
  void Draw( CDC* pDC );
  int TestLButtonDown() { return 2; }

private:
  CPoint m_PrevPoint;
  int    m_OldAngle;
  CAppMode* m_pCM;
};
//=====================================================================
class CGridMode : public CAppMode
{
public:
  CGridMode() :
    CAppMode( AIDG_GRID, 0, AM_FUNIMODE, IDM_GRID_MODE ),
    m_PrevPoint( -1, -1 ),
    m_bDoGrid( 1 ),
    m_Scale( 0 )
  {
    m_nMode = 1;
    m_SCF = 1.;
  }
  int OnLButtonDown( UINT& flags, CPoint& loc ){ OnMouseEvent( flags, loc ); return AM_RPROPAGATE; }
  int OnLButtonUp( UINT& flags, CPoint& loc )  { OnMouseEvent( flags, loc ); return AM_RPROPAGATE; }
  int OnRButtonDown( UINT& flags, CPoint& loc ){ OnMouseEvent( flags, loc ); return AM_RPROPAGATE; }
  int OnRButtonUp( UINT& flags, CPoint& loc )  { OnMouseEvent( flags, loc ); return AM_RPROPAGATE; }
  int OnMouseMove( UINT& flags, CPoint& loc )  { return OnMouseEvent( flags, loc ); }
  int OnMouseEvent( UINT& flags, CPoint& loc );
  void Draw( CDC* DC );
  void SetPrintRect( CRect const& r ) { m_PrintRect = r; }
  int  Term( int ) { return 1; }
  int GetDoGrid() { return m_bDoGrid; }

public:
  static int UpdateCoord( int coord, int step );

private:
  CRect  m_PrintRect;
  CPoint m_PrevPoint;
  CSize  m_Xg;
  CSize  m_Yg;
  int    m_Scale;
  int    m_bDoGrid;

public:
  static int m_bDrawGrid;
  static double m_SCF;
};
//=====================================================================
class CCoordGridMode : public CAppMode
{
public:
  CCoordGridMode() :
    CAppMode( AIDG_COORDGRID, 0, AM_FUNIMODE, IDM_COORDGRID_MODE ),
    m_Scale( 0 )
  {
    m_nMode = 1;
  }
  void Draw( CDC* DC );
  void SetPrintRect( CRect const& r ) { m_PrintRect = r; }
  int  Term( int ) { return 1; }

private:
  CRect  m_PrintRect;
  int    m_Scale;
  int    m_Xg;
  int    m_Yg;
};
//=====================================================================
class CScaleBase : public CRectMode, public CAppMode
{
public:
  CScaleBase( UINT nGroup, UINT cursor, UINT nFlags, UINT nID ) :
    CAppMode( nGroup, cursor, nFlags, nID )
  {}
  int GetCurScale() { return m_CurScale; }
  int TestLButtonDown() { return 2; }
  void Draw( CDC* pDC );

protected:
  void SetRB();
  void CalcScale();
  int  SetScale();

protected:
  int  m_CurScale;
};
//=====================================================================
class CScaleMode : public CScaleBase
{
public:
  CScaleMode();
  int OnLButtonDown( UINT& flags, CPoint& loc );
  int OnRButtonDown( UINT& flags, CPoint& loc );
  int OnButtonUp( UINT sFlaf );
  int OnLButtonUp( UINT& flags, CPoint& loc );
  int OnRButtonUp( UINT& flags, CPoint& loc );
  int OnMouseMove( UINT& flags, CPoint& loc );
  int OnChar( UINT& flags, UINT& key_code );
  int TestLButtonDown() { return 0; }
};
//=====================================================================
class CChangeScaleMode : public CScaleBase
{
public:
  CChangeScaleMode( UINT group, UINT cursor, UINT id );
  int OnLButtonDown( UINT& flags, CPoint& loc );
  int OnLButtonUp( UINT& flags, CPoint& loc );
  int OnRButtonDown( UINT& flags, CPoint& loc );
  int OnMouseMove( UINT& flags, CPoint& loc );
};
//=====================================================================
class CIncScaleMode : public CChangeScaleMode
{
public:
  CIncScaleMode::CIncScaleMode();
};
//=====================================================================
class CDecScaleMode : public CChangeScaleMode
{
public:
  CDecScaleMode::CDecScaleMode();
};
//=====================================================================
inline
CScaleMode::CScaleMode() :
  CScaleBase( AIDG_SCALE, 0, AM_FBALL | AM_FMOUSEMOVE | AM_FCHAR,
              IDM_SCALE_MODE )
{}
//=====================================================================
inline
CChangeScaleMode::CChangeScaleMode( UINT group, UINT cursor, UINT id ) :
  CScaleBase( group, cursor,
              AM_FESCMODE |AM_FUNIMODE | AM_FMONOPOLY |
              AM_FBALL | AM_FMOUSEMOVE,
              id )
{}
//=====================================================================
inline
CIncScaleMode::CIncScaleMode() :
  CChangeScaleMode( AIDG_INC_SCALE, IDC_INCSCALE, IDM_INCSCALE_MODE )
{}
//=====================================================================
inline
CDecScaleMode::CDecScaleMode() :
  CChangeScaleMode( AIDG_DEC_SCALE, IDC_DECSCALE, IDM_DECSCALE_MODE )
{}
//=====================================================================
//=====================================================================
class CDragMode : public CAppMode
{
public:
  CDragMode( UINT idg = AIDG_DRAG, UINT cursor = IDC_MYHAND,
             UINT mask = AM_FCHAR, UINT id = IDM_DRAG_MODE ) :
    CAppMode( idg, cursor, mask, id )
  {}
  int OnLButtonDown( UINT& flags, CPoint& loc );
  int OnLButtonUp( UINT& flags, CPoint& loc );
  int OnMouseMove( UINT& flags, CPoint& loc );
  int OnChar( UINT& flags, UINT& key_code );
  int TestLButtonDown() { return 0; }

private:
  CSize  m_Delta;
  CPoint m_OrgPos;
};
//=====================================================================
class CHandMode : public CDragMode
{
public:
  CHandMode() :
    CDragMode( AIDG_HAND, IDC_MYHAND, AM_FUNIMODE | AM_FMONOPOLY, IDM_HAND_MODE )
  {}
  int OnRButtonDown( UINT& flags, CPoint& loc );
  int OnRButtonUp( UINT& flags, CPoint& loc );
  int TestLButtonDown() { return 0; }
};
//=====================================================================
#if 0
class CAutoScrollMode : public CAppMode
{
public:
  CAutoScrollMode() :
    CAppMode( AIDG_AUTOSCROLL, 0, 0, IDM_AUTOSCROLL_MODE ),
    m_bTimerOn( 0 ),
    m_bKeyPressed( 0 ),
    m_pUndoItem( 0 )
  {}
  int  Term( int bDestroy );
  int OnTimer( UINT& nIDEvent );
  int OnMouseMove( UINT& flags, CPoint& loc );
  void SetKeyPressed( int v ) { m_bKeyPressed = v; }

private:
  int  m_bKeyPressed;
  int  m_bTimerOn;
  CSize m_Delta;
  CUndoScaleItem* m_pUndoItem;
};
#endif
//=====================================================================
class CSetCoordGridMode : public CAppMode
{
public:
  CSetCoordGridMode( int bCgm ) :
    CAppMode( AIDG_BLDMODE, IDC_DRAW, AM_FESCMODE | AM_FUNIMODE | AM_FMONOPOLY, IDM_SET_COORD_GRID_MODE ),
    m_bCgm( bCgm )
  {}
  int OnLButtonDown( UINT& flags, CPoint& loc );
  int OnLButtonUp( UINT& flags, CPoint& loc );
  int OnRButtonDown( UINT& flags, CPoint& loc );
  int  Term( int bDestroy );

private:
  int   m_bCgm;
  CPoint m_Pt;
};
//=====================================================================
class CSelectOverviewMode
 : public CRectMode, public CAppMode
{
public:
  CSelectOverviewMode( COverviewDialog* pDlg ) :
    m_pDlg( pDlg ),
    CAppMode( AIDG_STREETPLAN, IDC_PRINTFRAGMENT, 
              AM_FESCMODE | AM_FUNIMODE | AM_FMONOPOLY | AM_FMOUSEMOVE |
              AM_FLBDOWN | AM_FRBDOWN, IDM_SELECT_OVERVIEW )
  {}
  int OnLButtonDown( UINT &flags, CPoint &loc );
  int OnRButtonDown( UINT &flags, CPoint &loc ) { return AM_RREMOVE | AM_RDONE; }
  int OnMouseMove( UINT &flags, CPoint &loc );
  int Open() { m_nMode = 2; return 1; }
  int TestLButtonDown() { return 2; } 
  void Draw( CDC* pDC ) { CRectMode::Draw( pDC ); }

protected:
  COverviewDialog* m_pDlg;
};
//=====================================================================
#endif
