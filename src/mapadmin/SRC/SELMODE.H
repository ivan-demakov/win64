#ifndef __SELMODE_H__
#define __SELMODE_H__
//=====================================================================
#include "rectmode.h"
#include "ids.h"
//=====================================================================
class CSelectMode : public CRectMode, public CAppMode
{
public:
  CSelectMode() :
    CAppMode( AIDG_SELECT, 0,
              AM_FUNIMODE | AM_FLBDOWN | AM_FLBUP | AM_FRBDOWN |
              AM_FRBUP | AM_FLBDBLCLK | AM_FMOUSEMOVE | AM_FCHAR, IDM_SELECT_MODE )
  {}
  int OnLButtonDown( UINT &flags, CPoint &loc )
    { return OnButtonDown( flags, loc, 1 ); }
  int OnRButtonDown( UINT &flags, CPoint &loc )
    { return OnButtonDown( flags, loc, 0 ); }
  int OnLButtonUp( UINT &flags, CPoint &loc )
    { return OnButtonUp( flags, loc, 1 ); }
  int OnRButtonUp( UINT &flags, CPoint &loc )
    { return OnButtonUp( flags, loc, 0 ); }
  int OnLButtonDblClk( UINT &flags, CPoint &loc );
  int OnMouseMove( UINT &flags, CPoint &loc );
  int OnChar( UINT& flags, UINT& key_code );
  int TestLButtonDown() { return 0; }
  void Draw( CDC* pDC );
private:
  int OnButtonDown( UINT &flags, CPoint &loc, int bLeft );
  int OnButtonUp( UINT &flags, CPoint &loc, int bLeft );
  int TestEdition( int bLeft, int bDel = 0 );
};
//=====================================================================
class CChangeSelectMode : public CRectMode, public CAppMode
{
public:
  CChangeSelectMode( UINT group, UINT cursor, UINT id ) :
    CAppMode( group, cursor,
              AM_FESCMODE | AM_FUNIMODE | AM_FMONOPOLY |
              AM_FLBDOWN | AM_FLBUP | AM_FMOUSEMOVE, id )
  {}
  int  TestLButtonDown() { return 2; }
  int  OnLButtonDown( UINT &flags, CPoint &loc );
  int  OnRButtonDown( UINT &flags, CPoint &loc );
  int  OnLButtonUp( UINT &flags, CPoint &loc );
  int  OnMouseMove( UINT &flags, CPoint &loc );
  void Draw( CDC* pDC );
};
//=====================================================================
class CGetSelectMode : public CChangeSelectMode
{
public:
  CGetSelectMode() :
    CChangeSelectMode( AIDG_GET_SELECT, IDC_GETSELECT, IDM_GETSELECT_MODE )
  {}
  int OnLButtonDblClk( UINT &flags, CPoint &loc );
};
//=====================================================================
class CAddSelectMode : public CChangeSelectMode
{
public:
  CAddSelectMode() :
    CChangeSelectMode( AIDG_ADD_SELECT, IDC_ADDSELECT, IDM_ADDSELECT_MODE )
  {}
};
//=====================================================================
class CSubSelectMode : public CChangeSelectMode
{
public:
  CSubSelectMode() :
    CChangeSelectMode( AIDG_SUB_SELECT, IDC_SUBSELECT, IDM_SUBSELECT_MODE )
  {}
};
//=====================================================================
class CChangeClassMode : public CAppMode
{
public:
  CChangeClassMode() :
    CAppMode( AIDG_OBJACTIONMODE, IDC_CHANGE_CLASS,
              AM_FUNIMODE | AM_FESCMODE | AM_FLBDOWN | AM_FRBDOWN,
              IDM_CHANGE_CLASS_MODE )
  {}
  int OnLButtonDown( UINT &flags, CPoint &loc );
  int OnRButtonDown( UINT &flags, CPoint &loc );
  int Init( CMapView* pOwner );

private:
  int m_nClass;
};
//=====================================================================
class CMagnitMode : public CAppMode
{
public:
  CMagnitMode() :
    CAppMode( AIDG_MAGNITMODE, 0, AM_FMOUSEMOVE | AM_FLBDOWN | AM_FRBDOWN, IDM_MAGNIT_MODE1 ),
    m_nSelectMode( 0 ),
    m_pPoint( 0 )
  {
    m_nMode = 1;
  }
  int OnLButtonDown( UINT &flags, CPoint &loc )
    { return OnButtonDown( flags, loc ); }
  int OnLButtonUp( UINT &flags, CPoint &loc )
    { return OnButtonDown( flags, loc ); }
  int OnRButtonDown( UINT &flags, CPoint &loc )
    { return OnButtonDown( flags, loc ); }
  int OnRButtonUp( UINT &flags, CPoint &loc )
    { return OnButtonDown( flags, loc ); }
  int OnMouseMove( UINT &flags, CPoint &loc );
  void Draw( CDC* pDC );
  void Select( CPoint pt );
  void Select( CPoint* pPt, int n );
  void Select( CRect const& r );
  void SetSelMode( int msk ) { m_nSelectMode ^= msk; }
  int  GetSelMode() { return m_nSelectMode; }

private:
  int OnButtonDown( UINT &flags, CPoint &loc );

  int m_nSelectMode;

  CPoint* svl[3];
  int     snl[3];
  int     dl[3];
  int     m_nLine;
  int     m_nRect;

  CPoint* m_pPoint;
  CPoint  m_CurPoint;
  CPoint  m_CurPos;
  int     m_CurDist;

  CRect svr[4];
  int   dr[4];
};
//=====================================================================
#endif
