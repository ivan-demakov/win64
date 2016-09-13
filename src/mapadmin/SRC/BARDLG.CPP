#include "stdafx.h"

#include "dwin.h"
#include "mdoc.h"
#include "bardlg.h"
#include "msutil.h"
#include "util.h"
#include "TGstream.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
//======================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
BEGIN_MESSAGE_MAP( CToolbarDlg, CDialog )
  ON_BN_CLICKED( IDC_TBD_OBJ_BAR_RADIO, OnObjBarSelect )
  ON_BN_CLICKED( IDC_TBD_DOC_BAR_RADIO, OnDocBarSelect )
  ON_BN_CLICKED( IDC_TBD_FND_BAR_RADIO, OnFndBarSelect )
  ON_BN_CLICKED( IDC_TBD_LFT_BUTTON,    OnLftClicked )
  ON_BN_CLICKED( IDC_TBD_RHT_BUTTON,    OnRhtClicked )
  ON_BN_CLICKED( IDC_TBD_SEL_BUTTON,    OnSelectClicked )
END_MESSAGE_MAP()
//======================================================================
CToolbarDlg::~CToolbarDlg()
{
  delete m_pBar;

  while( --m_nButtons >= 0 )
    delete m_Buttons[m_nButtons];

  while( --m_nExButtons >= 0 )
    delete m_ExButtons[m_nExButtons];
}
//======================================================================
int
CToolbarDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  static UINT bb[][2] =
  {
    IDC_TBD_LFT_BUTTON, IDB_L_ARROW,
    IDC_TBD_RHT_BUTTON, IDB_R_ARROW
  };

  for( int i = CELEM( bb ) ; --i >= 0 ; )
  {
    HBITMAP hBmp = (HBITMAP)LoadImage( App.m_hInstance, LPCTSTR( bb[i][1] ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
    if( !hBmp )
      continue;
    CButton* pBtn = (CButton*) GetDlgItem( bb[i][0] );
    pBtn->SetBitmap( hBmp );
    pBtn->ShowWindow( SW_HIDE );
  }

  WINDOWPLACEMENT WP;
  GetDlgItem( IDC_TBD_EXAMPLE_STATIC )->GetWindowPlacement( &WP );
  WP.rcNormalPosition.bottom =  WP.rcNormalPosition.top + 30;
  GetDlgItem( IDC_TBD_EXAMPLE_STATIC )->SetWindowPlacement( &WP );

  GetDlgItem( IDC_TBD_BMP_STATIC )->GetWindowPlacement( &WP );
  CRect r( WP.rcNormalPosition );

  int raws = r.Height() / BUTTON_Y_SIZE;
  int clms = r.Width()  / BUTTON_X_SIZE;
  int mbn = min( raws * clms, MAX_BUTTONS );

  CString wild( App.GetProjectFilePath( "icons\\*.bmp" ));
  CFileFind ff;
  for( int h = ff.FindFile( wild ) ; m_nButtons < mbn && h ; )
  {
    h = ff.FindNextFile();
    HBITMAP hBmp = (HBITMAP)LoadImage( 0, ff.GetFilePath(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_LOADMAP3DCOLORS );

    if( !hBmp )
      continue;

    m_Names[m_nButtons] = ff.GetFileTitle();
    CRect br( 0, 0, BUTTON_X_SIZE, BUTTON_Y_SIZE );
    br.OffsetRect( r.left + m_nButtons % clms * BUTTON_X_SIZE, r.top  + m_nButtons / clms * BUTTON_Y_SIZE );
    m_Buttons[m_nButtons++] = new CMyButton( this, br, hBmp, m_nButtons );
  }

  if( !m_nButtons )
  {
    ErrBox( "Библиотека иконок пуста!" );
    EndDialog( 0 );
    return 0;
  }

  GetDlgItem( IDC_TBD_SEL_BUTTON )->EnableWindow( 0 );
  GetDlgItem( IDC_TBD_ID_EDIT )->EnableWindow( 0 );
  SetDlgItemInt( IDC_TBD_ID_EDIT, 0 );

  return 1;
}
//======================================================================
void
CToolbarDlg::OnBarSelect( UINT newId )
{
  if( newId == m_BarID )
    return;

  if( SaveModified() == IDCANCEL )
  {
    CheckDlgButton( IDC_TBD_OBJ_BAR_RADIO, m_BarID == IDB_MAP_OBJECTS );
    CheckDlgButton( IDC_TBD_DOC_BAR_RADIO, m_BarID == IDB_MAP_VIEW );
    CheckDlgButton( IDC_TBD_FND_BAR_RADIO, m_BarID == IDB_MAP_FIND );
    return;
  }

  CMainWin* pWnd = (CMainWin*)AfxGetMainWnd();

  m_BarID = newId;

  GetDlgItem( IDC_TBD_SEL_BUTTON )->EnableWindow( m_BarID == IDB_MAP_OBJECTS );
  GetDlgItem( IDC_TBD_ID_EDIT )->EnableWindow( m_BarID != IDB_MAP_OBJECTS );

  delete m_pBar;
  CMyBar* pBar = (CMyBar*)pWnd->GetBarPtr( m_BarID );
  if( !pBar )
	{
		int i = pWnd->GetBarNum( m_BarID );
		pBar = (CMyBar*)pWnd->CreateToolBar( i );
	}
  if( !pBar )
		ErrBox( "Панель инструментов не найдена" );
  else
  {
    m_pBar = new CMyBar( pBar );
    ShowExample( m_nStartShow = 0 );
    m_Changed = 0;
  }
}
//======================================================================
void
CToolbarDlg::ShowExample( int n )
{
  if( !m_pBar )
    return;

  for( int i = m_nExButtons ; --i >= 0 ; delete m_ExButtons[i] );

  CWnd* pStatic = GetDlgItem( IDC_TBD_EXAMPLE_STATIC );
  WINDOWPLACEMENT WP;
  pStatic->GetWindowPlacement( &WP );
  int x = WP.rcNormalPosition.left + 4;
  int y = WP.rcNormalPosition.top + 4;
  CRect br( x, y, x + BUTTON_X_SIZE, y + BUTTON_Y_SIZE );
  m_nExButtons = 0;
  for( i = n ; i < m_pBar->GetBarSize() && br.right < WP.rcNormalPosition.right ; ++i )
    if( m_pBar->GetID( i ) == ID_SEPARATOR )
    {
      m_ExButtons[m_nExButtons++] = 0;
      br.OffsetRect( 8, 0 );
    }
    else
    {
      for( int k = m_nButtons ; --k >= 0 && m_Names[k].CompareNoCase( m_pBar->GetName( i )) ; );
      ASSERT( k >= 0 );
      m_ExButtons[m_nExButtons++] = new CMyButton( this, br, m_Buttons[k]->GetBitmap(), m_pBar->GetID( i ));
      br.OffsetRect( BUTTON_X_SIZE, 0 );
    }

  GetDlgItem( IDC_TBD_RHT_BUTTON )->ShowWindow( i < m_pBar->GetBarSize() ? SW_SHOW : SW_HIDE );
  GetDlgItem( IDC_TBD_LFT_BUTTON )->ShowWindow( n > 0 ? SW_SHOW : SW_HIDE );
}
//======================================================================
void
CToolbarDlg::OnLftClicked()
{
  --m_nStartShow;
  ShowExample( m_nStartShow );
}
//======================================================================
void
CToolbarDlg::OnRhtClicked()
{
  ++m_nStartShow;
  ShowExample( m_nStartShow );
}
//======================================================================
void
CToolbarDlg::OnButtonDown( CMyButton* pButton )
{
  if( !m_pBar )
    return;

  for( int i = m_nButtons ; --i >= 0 && m_Buttons[i]->GetBitmap() != pButton->GetBitmap() ; );
  ASSERT( i >= 0 );
  CString const& name = m_Names[i];
  for( i = m_pBar->GetBarSize() ; --i >= 0 && m_pBar->GetName( i ).CompareNoCase( name ) ; );
  if( i >= 0 )
  {
    SetDlgItemInt( IDC_TBD_ID_EDIT, m_pBar->GetID( i ) - m_pBar->GetBaseId());
    SetDlgItemText( IDC_TBD_TT_EDIT, m_pBar->GetToolTip( i ));
  }
}
//======================================================================
void
CToolbarDlg::OnButtonDrop( CMyButton* pButton )
{
  CWnd* pStatic = GetDlgItem( IDC_TBD_EXAMPLE_STATIC );
  WINDOWPLACEMENT WP;
  pStatic->GetWindowPlacement( &WP );
  CRect r1( WP.rcNormalPosition );
  pButton->GetWindowPlacement( &WP );
  CRect r2( WP.rcNormalPosition );

  int ins = CRect().IntersectRect( &r1, &r2 );

  if( !m_pBar || !ins && pButton->m_Id < MAX_BUTTONS )
  {
    delete pButton;
    return;
  }

  CString tt;
  GetDlgItemText( IDC_TBD_TT_EDIT, tt );

  if( ins && tt.IsEmpty())
  {
    InfBox( "Введите текст в окно \"Подсказка\"" );
    delete pButton;
    return;
  }

  int id = m_pBar->GetBaseId() + GetDlgItemInt( IDC_TBD_ID_EDIT );

  int pos = m_pBar->GetPosition( id );

  if( ins && pButton->m_Id < MAX_BUTTONS && pos >= 0 )
  {
    ErrBox( "Кнопка с таким идентификатором уже есть!" );
    delete pButton;
    return;
  }

  int lsp = pos > 0 && m_pBar->GetID( pos - 1 ) == ID_SEPARATOR;

  m_pBar->RemoveButton( pos );

  if( ins )
  {
    int k = -1, d = 4000;

    for( int n = m_nExButtons ; --n >= 0 ; )
    {
      CMyButton* pb = m_ExButtons[n];
      if( !pb || pb == pButton )
        continue;
      int t = r2.left - pb->m_Coord.x;
      if( t > 0 && t < d )
        d = t, k = n;
    }

    k = k < 0 ? m_nStartShow : m_pBar->GetPosition( m_ExButtons[k]->m_Id ) + 1;

    d -= BUTTON_X_SIZE * 3 / 2;

    lsp &= k + 1 == pos;

    if( d > 0 )
      if( lsp )
        ++k;
      else
        m_pBar->InsertSeparator( k++ );
    else
    if( lsp )
      m_pBar->RemoveButton( k );

    for( int i = m_nButtons ; --i >= 0 && m_Buttons[i]->GetBitmap() != pButton->GetBitmap() ; );
    m_pBar->InsertButton( k, m_Names[i], tt, id );
  }

  if( pos < 0 )
    delete pButton;

  m_Changed = 1;
  ShowExample( m_nStartShow );
}
//======================================================================
int
CToolbarDlg::SaveModified()
{
  if( !m_Changed )
    return IDOK;

  CMainWin* pWnd = (CMainWin*)AfxGetMainWnd();
  char m[128];
  sprintf( m, "Сохранить изменения панели \"%s\"?", pWnd->TBS[pWnd->GetBarNum( m_BarID )].pName );
  int r = AskBox( m );
  if( r == IDYES )
    OnOK();
  return r;
}
//======================================================================
void
CToolbarDlg::OnCancel()
{
  if( SaveModified() != IDCANCEL )
    EndDialog( 0 );
}
//======================================================================
void
CToolbarDlg::OnOK()
{
  if( !m_pBar )
    return;

  CMainWin* pWnd = (CMainWin*)AfxGetMainWnd();

  LPCTSTR pSect = pWnd->TBS[pWnd->GetBarNum( m_BarID )].pSect;

  TGstream str;

  for( int i = 0 ; i < m_pBar->GetBarSize() ; ++i )
  {
    CString nm( m_pBar->GetName( i ));
    CString tt = "";
    INT id = m_pBar->GetID( i );
    if( id == ID_SEPARATOR )
      nm = "SEPARATOR=";
    else
      tt.Format( "=%d %s", id - m_pBar->GetBaseId(), m_pBar->GetToolTip( i ));

    str << nm <<  tt << '\0';
  }
  str << '\0';

  WritePrivateProfileSection( pSect, str.str(), App.GetIniPath());
  pWnd->CreateToolBars();
  m_Changed = 0;
}
//======================================================================
void
CToolbarDlg::OnSelectClicked()
{
  CObjDef* pObjDef = pDocument->GetObjDef();
  CSelectClassDlg dlg( "Выбор класса", pObjDef );
  dlg.DoModal();
  UINT id = dlg.GetClass();
  SetDlgItemInt( IDC_TBD_ID_EDIT, id );
  SetDlgItemText( IDC_TBD_TT_EDIT, pObjDef->GetName( id ));
}
//======================================================================
BEGIN_MESSAGE_MAP( CMyButton, CButton )
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
//======================================================================
CMyButton::CMyButton( CMyButton* pBtn ) :
  m_Catched( 0 ),
  m_Id( pBtn->m_Id )
{
  WINDOWPLACEMENT WP;
  pBtn->GetWindowPlacement( &WP );
  Create( 0, pBtn->GetStyle(), WP.rcNormalPosition, pBtn->GetParent(), pBtn->m_Id );
  m_Coord.x = WP.rcNormalPosition.left;
  m_Coord.y = WP.rcNormalPosition.top;
  SetBitmap( pBtn->GetBitmap());
}
//======================================================================
CMyButton::CMyButton( CWnd* pWnd, CRect const& r, HBITMAP hBmp, UINT id ) :
  m_Catched( 0 ),
  m_Id( id )
{
  Create( 0, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_CENTER|BS_BITMAP, r, pWnd, id );
  m_Coord = r.TopLeft();
  SetBitmap( hBmp );
}
//======================================================================
void
CMyButton::OnLButtonDown( UINT, CPoint loc )
{
  ((CToolbarDlg*)GetParent())->OnButtonDown( this );
  CMyButton* pb = m_Id < MAX_BUTTONS ? new CMyButton( this ) : this;
  pb->m_Coord = loc;
  pb->m_Catched = 1;
  pb->SetCapture();
}
//======================================================================
void
CMyButton::OnLButtonUp( UINT, CPoint loc )
{
  ((CToolbarDlg*)GetParent())->OnButtonDrop( this );
  ReleaseCapture();
}
//======================================================================
void
CMyButton::OnMouseMove( UINT, CPoint loc )
{
  if( !m_Catched )
    return;

  WINDOWPLACEMENT WP;
  GetWindowPlacement( &WP );
  CRect r( WP.rcNormalPosition );
  r.OffsetRect( loc - m_Coord );
  MoveWindow( &r, 1 );
}
//======================================================================
