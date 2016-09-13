#ifndef __TOOLBARDLG_H__
#define __TOOLBARDLG_H__
//======================================================================
#include "ids.h"
#include "dlgids.h"
#include "mybar.h"
#include "lawdlg.h"
//======================================================================
#define MAX_BUTTONS 1024
//======================================================================
class CMyButton : public CButton
{
  friend class CToolbarDlg;
public:
  CMyButton() {}
  CMyButton( CMyButton* pBtn );
  CMyButton( CWnd* pWnd, CRect const& r, HBITMAP hBmp, UINT id );
  void OnLButtonDown( UINT, CPoint loc );
  void OnLButtonUp( UINT, CPoint loc );
  void OnMouseMove( UINT, CPoint loc );

private:
  UINT   m_Id;
  CPoint m_Coord;
  int   m_Catched;
  
  DECLARE_MESSAGE_MAP()
};
//======================================================================
class CToolbarDlg : public CDialog
{
  enum 
  { 
    BUTTON_X_SIZE = 24, 
    BUTTON_Y_SIZE = 22 
  };

public:
  CToolbarDlg() : 
    CDialog( IDD_TOOLBAR_DIALOG, 0 ), 
    m_nButtons( 0 ),
    m_nExButtons( 0 ),
    m_pBar( 0 ),
    m_BarID( 0 ),
    m_Changed( 0 )
  {}
  ~CToolbarDlg();

  int  OnInitDialog();
  void OnBarSelect( UINT newId );
  void OnObjBarSelect() { OnBarSelect( IDB_MAP_OBJECTS ); }
  void OnDocBarSelect() { OnBarSelect( IDB_MAP_VIEW ); }
  void OnFndBarSelect() { OnBarSelect( IDB_MAP_FIND ); }
  void OnButtonDown( CMyButton* pButton );
  void OnButtonDrop( CMyButton* pButton );
  void OnSelectClicked();
  void OnLftClicked();
  void OnRhtClicked();
  void OnOK();
  void OnCancel();

private:
  void ShowExample( int nStartShow );
  int  SaveModified();

private:
  int        m_nButtons;
  int        m_nExButtons;
  int        m_nStartShow;
  CMyButton* m_Buttons[MAX_BUTTONS];
  CMyButton* m_ExButtons[MAX_BUTTONS];
  CString    m_Names[MAX_BUTTONS];
  UINT       m_BarID;
  int       m_Changed;
  CMyBar*    m_pBar;

  DECLARE_MESSAGE_MAP()
};
//======================================================================
#endif
