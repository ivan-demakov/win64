#include "stdafx.h"

#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif

#include "util.h"
#include "msutil.h"
#include "mybar.h"
#include "ids.h"
#include "afxpriv.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
BEGIN_MESSAGE_MAP( CMyBar, CToolBar )
  ON_NOTIFY_RANGE( TTN_NEEDTEXTA, IDM_DOCCTL_BEGIN, IDM_DOCCTL_END, OnNeedTextA )
  ON_NOTIFY_RANGE( TTN_NEEDTEXTA, IDM_FNDCTL_BEGIN, IDM_FNDCTL_END, OnNeedTextA )
  ON_NOTIFY_RANGE( TTN_NEEDTEXTA, IDM_OBJCTL_BEGIN, IDM_OBJCTL_END, OnNeedTextA )
END_MESSAGE_MAP()
//=====================================================================
void
CMyBar::OnNeedTextA( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult )
{
  for( int i = m_BarSize ; --i >= 0 && m_pBarInf[i].CommandID != nID ; );
  CString t( i < 0 ? "?" : m_pBarInf[i].ToolTipText );

  LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct;
  _tcscpy( lpTTT->szText, t.Left( 79 ));
}
//=====================================================================
int
CMyBar::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
  NMHDR* pNMHDR = (NMHDR*)lParam;
  int nCode = pNMHDR->code;
  if( nCode != TTN_NEEDTEXTA )
    return CToolBar::OnNotify( wParam, lParam, pResult );

  HWND hWndCtrl = pNMHDR->hwndFrom;
  ASSERT( hWndCtrl );
  ASSERT( ::IsWindow( hWndCtrl ));

  UINT nID = wParam;

  ASSERT( pNMHDR->idFrom == nID );

  if( AfxGetThreadState()->m_hLockoutNotifyWindow == GetSafeHwnd() ||
      ReflectLastMsg( hWndCtrl, pResult ))
    return 1;

  AFX_NOTIFY notify;
  notify.pResult = pResult;
  notify.pNMHDR = pNMHDR;
  return OnCmdMsg( nID, MAKELONG( nCode, WM_NOTIFY ), &notify, 0 );
}
//=====================================================================
CMyBar::CMyBar( CWnd* pWnd, LPCTSTR sectName, UINT baseID, UINT tbid ) :
  m_BaseId( baseID ),
  m_BarSize( 0 ),
  m_pBarInf( 0 )
{
  Create( pWnd, 
          WS_CHILD | WS_VISIBLE | CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 
          tbid );
  
  char buf[16384];
  UINT i = 0, x = 0;

  CString path( App.GetProjectFilePath( "icons\\*.*" ));

  int n = App.GetProfileKeys( sectName, buf, sizeof buf );

  m_pBarInf = new BarBtnInf[n];

  CDC barDC, icoDC;
  icoDC.CreateCompatibleDC( 0 );
  barDC.CreateCompatibleDC( 0 );

  m_BarBmp.CreateCompatibleBitmap( GetDC(), n * BUTTON_X_SIZE, BUTTON_Y_SIZE );
  barDC.SelectObject( &m_BarBmp );

  int bSep = 1; 
  for( char* pb = buf ; *pb ; pb += strlen( pb ) + 1 )
  {
    UCHAR* p = (UCHAR*)"";
    UCHAR const d[] = "?";
    UINT id = ID_SEPARATOR;
    CString t;

    if( stricmp( pb, "Separator" ))
    {
      t = App.GetProfileString( sectName, pb );
      char const* buf = t.GetBuffer( 1024 ); 
      int oid = atoi( buf );

      for( p = (UCHAR*)buf ; isdigit( *p ) || isspace( *p ) ; ++p );

      char bPath[_MAX_PATH];
      MakePath( path, bPath, "bmp", pb );

      HBITMAP hBmp = (HBITMAP)LoadImage( 0, bPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_LOADMAP3DCOLORS );

      if( m_BarSize < MAX_BAR_SIZE && hBmp && oid < CLASS_RANGE )
      {
        id = baseID + oid;
        icoDC.SelectObject( hBmp );
        barDC.BitBlt( x, 0, x + BUTTON_X_SIZE, BUTTON_Y_SIZE, &icoDC, 0, 0, SRCCOPY );
        x += BUTTON_X_SIZE;
      }
    }
    if( !bSep || id != ID_SEPARATOR )
    {
      m_pBarInf[m_BarSize++].Create( pb, LPCTSTR( *p ? p : d ), id );
      bSep = id == ID_SEPARATOR;
    }
  }

  if( m_BarSize )
  {
    SetBitmap( HBITMAP( m_BarBmp ));
    UINT b[MAX_BAR_SIZE];
    for( int i = m_BarSize ; --i >= 0 ; b[i] = m_pBarInf[i].CommandID );
    SetButtons( b, m_BarSize );
  }
}
//=====================================================================
CMyBar::CMyBar( CMyBar* pBar ) :
  m_BarSize( pBar->m_BarSize ),
  m_BaseId( pBar->GetBaseId())
{
  CreateEx( pBar->GetParent(), 
            TBSTYLE_FLAT, 
            WS_CHILD | WS_VISIBLE | CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 
            CRect( 0, 0, 0, 0 ), 
            0 );
  m_pBarInf = new BarBtnInf[m_BarSize];
  for( int i = m_BarSize ; --i >= 0 ; m_pBarInf[i] = pBar->m_pBarInf[i] );
}
//=====================================================================
CMyBar::~CMyBar()
{
  DeleteObject( &m_BarBmp );
  delete[] m_pBarInf;
}
//=====================================================================
int
CMyBar::RemoveButton( int n )
{
  if( n < 0 || n >= m_BarSize )
    return 0;

  BarBtnInf* bi = new BarBtnInf[--m_BarSize];
  for( int i = m_BarSize ; --i >= 0 ;  bi[i].Create( m_pBarInf[i+(i>=n)] ));
  delete[] m_pBarInf;
  m_pBarInf = bi;
  return 1;
}
//=====================================================================
int
CMyBar::InsertButton( int n, CString const& BFName, CString const& TTText, UINT CmdID )
{
  if( n < 0 || n > m_BarSize )
    return 0;

  BarBtnInf* bi = new BarBtnInf[++m_BarSize];
  for( int i = m_BarSize - 1 ; --i >= 0 ; bi[i+(i>=n)].Create( m_pBarInf[i] ));
  bi[n].Create( BFName, TTText, CmdID );
  delete[] m_pBarInf;
  m_pBarInf = bi;
  return 1;
}
//=====================================================================
int
CMyBar::InsertSeparator( int n )
{
  return InsertButton( n, "", "", ID_SEPARATOR );
}
//=====================================================================
int
CMyBar::IsSeparator( int i )
{
  return i < m_BarSize && m_pBarInf[i].CommandID == ID_SEPARATOR;
}
//=====================================================================
int
CMyBar::GetPosition( UINT id )
{
  for( int i = m_BarSize ; --i >= 0 && m_pBarInf[i].CommandID != id ; );
  return i;
}
//=====================================================================
