#ifndef __BLDMODE_H__
#define __BLDMODE_H__
//=====================================================================
#include "appmode.h"
#include "drawobj.h"
#include "ident.h"
#include "joke.h"
#include "idg.h"
#include "ids.h"
//=====================================================================
class CDrawObject;
//=====================================================================
typedef ksi_obj (*ksi_event_fun)( HWND wnd, int x, int y, unsigned state );
//=====================================================================
class CBuildMode : public CAppMode
{
public:
  CBuildMode( UINT nID,                      // mode ident
              CIdent ident,                  // full object ident
              int nNode,                     // node num.
              ksi_obj paramDef = ksi_nil,    // variable list
              ksi_obj imageDef = ksi_nil );  // parameter list

  int  Term( int bDestroy );
  void Draw( CDC* pDC );
  int  OnLButtonDown( UINT& flags, CPoint& loc );
  int  OnRButtonDown( UINT& flags, CPoint& loc ) { return UpdateMouseEvent( flags, loc, ksi_make_win_right_down ); }
  int  OnLButtonUp( UINT& flags, CPoint& loc )   { return UpdateMouseEvent( flags, loc, ksi_make_win_left_up );    }
  int  OnRButtonUp( UINT& flags, CPoint& loc )   { return UpdateMouseEvent( flags, loc, ksi_make_win_right_up );   }
  int  OnMouseMove( UINT& flags, CPoint& loc )   { return UpdateMouseEvent( flags, loc, ksi_make_win_mouse_move ); }
  int  OnChar( UINT& flags, UINT&key_code );
  int  OnTimer( UINT& nID );
  int  Open();
  int  Init( CMapView* owner );
  int  TestLButtonDown() { return 2; }
  int  IsEdition();
  int  MyIdent( CIdent id ) { return id == m_ObjectIdent; }
  int  IsDragging() { return m_bDragging; }
  void SetDragging( int bd ) { m_bDragging = bd; }
  int  GetSelStatus() { return m_SelStatus; }
  void SetSelStatus( int s ) { m_SelStatus = s; }
  CIdent GetObjectIdent() { return m_ObjectIdent; }

  void* operator new( size_t sz ) { return ksi_malloc_eternal( sz ); }
  void* operator new( size_t sz, LPCSTR, int ) { return ksi_malloc_eternal( sz ); }
  void operator delete( void* p ) { ksi_free( p ); }
  void operator delete( void* p, LPCSTR, int ) { ksi_free( p ); }

  void SetTextCursor( int CursorSize, CPoint CursorPos = CPoint( 0, 0 ), int CursorAngle = 0 );
  void DrawTextCursor();

public:
  static void SetContext( int cb = 0, CDrawObject* pObj = 0, CPoint loc = CPoint( 0, 0 ));
  static int HasContext() { return m_bContextBld; }
  static CIdent GetContextId() { return m_ContextId; }
  static CRect GetClipRect() { return m_ClipRect; }
  static void  SetClipRect( CRect r ) { m_ClipRect = r; }
  static void  ResetClipRect() { m_ClipRect.SetRect( 0, 0, 0, 0 ); }

private:
  int  UpdateMouseEvent( UINT& flags, CPoint& loc, ksi_event_fun fun );
  int  UpdateContainer( ksi_obj val, ksi_obj ksi_dc );

private: //
  CPoint m_CursorPos;
  int    m_CursorAngle;
  int    m_CursorSize;
  int    m_bCursorOn;
  int    m_bCursorShown;

private:
  ksi_obj KSI_Dispatch;
  ksi_obj m_paramDef;
  ksi_obj m_imageDef;
  CIdent  m_ObjectIdent;
  int     m_nNode;
  int     m_SelStatus;
  int     m_bDragging;

private:
  static int     m_bContextBld;
  static ksi_obj m_KsiContext;
  static CPoint  m_LastLoc;
  static CIdent  m_ContextId;
  static CRect   m_ClipRect;
};
//=====================================================================
// Object action mode class
class CObjActionMode : public CAppMode
{
public:
  CObjActionMode( UINT nId ) :
    CAppMode( AIDG_OBJACTIONMODE,
              nId == IDM_OBJEDIT_MODE     ? IDC_EDITOBJ     :
              nId == IDM_CHANGEPARAM_MODE ? IDC_CHANGEPARAM :
              nId == IDM_QUERYPARAM_MODE  ? IDC_QUERYPARAM  : 0,
              AM_FESCMODE | AM_FUNIMODE, nId )

  {
    m_nMode = 1;
  }
  int OnRButtonDown( UINT& flags, CPoint& loc );
  int OnLButtonDown( UINT& flags, CPoint& loc );
  int OnLButtonUp( UINT& flags, CPoint& loc );

private:
  CIdent m_ObjId;
  CPoint m_FirstLoc;
};
//=====================================================================
class CContextMode : public CAppMode
{
public:
  CContextMode() : CAppMode( AIDG_CONTEXTMODE, 0, AM_FRBDOWN, IDM_OBJCONTEXT_MODE ){}
  int OnRButtonDown( UINT& flags, CPoint& loc );
};
//=====================================================================
#endif
