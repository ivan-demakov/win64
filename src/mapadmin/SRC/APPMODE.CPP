#include "stdafx.h"

#include "mview.h"
#include "appmode.h"
#include "movemode.h"
#ifdef _FULL_PROJECT
  #include "tgmapmole.h"
#else
  #include "mapmole.h"
#endif
#include "ids.h"
#include "util.h"
#include "miscmode.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
void
CMapView::SendCtrMsg()
{
  if( m_bCrtMsg )
    m_Conductor.DeclareEvent( CE_CRT_WND_MSG );
  m_bCrtMsg = 0;
}
//=====================================================================
void
CMapView::DrawMode( CAppMode* pMode )
{
  CViewDC dc( this );
  DrawMode( pMode, AM_ALL, &dc );
}
//=====================================================================
void
CMapView::DrawMode( CAppMode* pMode, UINT nFMask, CDC* pDC )
{
  if( pMode && pMode->m_nMode && ( nFMask == AM_ALL ||( pMode->m_nFlags & nFMask )))
    pMode->Draw( pDC );
}
//=====================================================================
CAppMode*
CMapView::UpdateMode( CAppMode* pMode, int rc, UINT nFMask, CDC* pDC )
{
  CAppMode* pNext = pMode->m_pNext;

  if( rc & AM_RREMOVE )
    RemoveMode( pMode );
  else
    DrawMode( pMode, nFMask, pDC );

  return ( rc & AM_RDONE ) ? 0 : pNext;
}
//=====================================================================
void
CMapView::DelAllModes()
{
  while( m_pMode )
  {
    CAppMode* pCur = m_pMode;
    m_pMode = m_pMode->m_pNext;
    pCur->Term( 1 );
    delete pCur;
  }
}
//=====================================================================
int
CMapView::AddMode( CAppMode* pMode )
{
  if( !pMode->Init( this ))
  {
    delete pMode;
    return 0;
  }

  CAppMode** ppMode;
  for(ppMode = &m_pMode ;
       *ppMode && (*ppMode)->m_nGroup > pMode->m_nGroup ;
       ppMode = &(*ppMode)->m_pNext );

  if( *ppMode && (*ppMode)->m_nGroup == pMode->m_nGroup )
    RemoveMode( *ppMode );

  pMode->m_pNext = *ppMode;
  *ppMode = pMode;

  UINT id = pMode->m_nID;
  if( pMode->Open())
    InvalidateRect( 0, 0 );

  return GetMode( id ) != 0;
}
//=====================================================================
int
CMapView::RemoveMode( CAppMode* pMode )
{
  if( !pMode )
    return 0;

  CAppMode** ppCur;
  for(ppCur = &m_pMode ; *ppCur && *ppCur != pMode ;
       ppCur = &(*ppCur)->m_pNext );

  if( *ppCur )
  {
    *ppCur = (*ppCur)->m_pNext;
    if( pMode->Term())
      InvalidateRect( 0, 0 );
    else
      DrawMode( pMode );
    delete pMode;
  }

  OnSetCursor( this, 0, 0 );

  return *ppCur != 0;
}
//=====================================================================
int
CMapView::RemoveMode( UINT modeId )
{
  return RemoveMode( GetMode( modeId ));
}
//=====================================================================
void
CMapView::DrawModes( CDC* pDC )
{
  for( CAppMode* pCur = m_pMode ; pCur ;
       DrawMode( pCur, AM_ALL, pDC ), pCur = pCur->m_pNext );
}
//=====================================================================
CAppMode*
CMapView::GetMode( UINT nID1, UINT nID2 )
{
  nID2 || ( nID2 = nID1 + 1 );
  CAppMode* pCur;
  for(pCur = m_pMode ;
       pCur && ( pCur->m_nID < nID1 || pCur->m_nID >= nID2 ) ;
       pCur = pCur->m_pNext );
  return pCur;
}
//=====================================================================
CAppMode*
CMapView::GetActiveMode( UINT id )
{
  CAppMode* pMode = GetMode( id );
  return pMode && pMode->m_nMode ? pMode : 0;
}
//=====================================================================
CAppMode*
CMapView::GetActiveMode( UINT* pId )
{
	CAppMode* pMode;
  for(pMode = 0 ; *pId && !pMode ; pMode = GetActiveMode( *pId++ ));
  return pMode;
}
//=====================================================================
CAppMode*
CMapView::GetMode( UINT* pId )
{
	CAppMode* pMode;
  for(pMode = 0 ; *pId && !pMode ; pMode = GetMode( *pId++ ));
  return pMode;
}
//=====================================================================
void
CMapView::OnChar( UINT key_code, UINT count, UINT flags )
{
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;
  m_bCrtMsg = 1;

  CViewDC dc( this );
  for( CAppMode* pCur = m_pMode ; pCur ;
       pCur = UpdateMode( pCur, pCur->OnChar( flags, key_code ), AM_FCHAR, &dc ))
    DrawMode( pCur, AM_FCHAR, &dc );

  SendCtrMsg();
	bLock = 0;
}
//=====================================================================
void
CMapView::OnTimer( UINT nIDEvent )
{
  switch( nIDEvent )
  {
    case IDM_FULINKTIMER:
//      TestFLTopscr( 0 );
      break;
    case IDM_BLINKSELTIMER:
      MakeBlinking();
      break;	
    default:
    {
      CViewDC DC( this );
      for( CAppMode* pCur = m_pMode ; pCur ;
           pCur = UpdateMode( pCur, pCur->OnTimer( nIDEvent ), AM_FTIMER, &DC ))
        DrawMode( pCur, AM_FTIMER, &DC );
    }
  }
}
//=====================================================================
void
CMapView::OnMouseEvent( UINT flags, CPoint loc, UINT nFMask,
                        int (CAppMode::*fun)( UINT&, CPoint& ))
{
  int bAlt = GetKeyState( VK_MENU ) < 0 && !GetActiveMode( IDM_MOVE_MODE );

  CAppMode* pDragMode = GetActiveMode( IDM_DRAG_MODE );

  if( pDragMode )
    if( !bAlt && pDragMode->m_nMode == 2 )
       pDragMode->OnLButtonUp( flags, loc );

  if( flags == -1 )
    bAlt = flags = 0;
  else
    m_Position = DP2LP( loc );

  loc = m_Position;

  if(( nFMask & AM_FBDOWN ))
    SetCapture();
  else
  if( nFMask & AM_FBUP )
    ReleaseCapture();

  CViewDC dc( this );
  for( CAppMode* pCur = m_pMode ; pCur ; )
    if( !bAlt && !pDragMode || pCur->m_nID == IDM_DRAG_MODE )
    {
      DrawMode( pCur, nFMask, &dc );
      pCur = UpdateMode( pCur, (pCur->*fun)( flags, loc ), nFMask, &dc );
    }
    else
      pCur = pCur->m_pNext;

  SendCtrMsg();
}
//=====================================================================
void
CMapView::OnFixMouse( CPoint loc, int bRB )
{
	m_Position = loc;
  CViewDC dc( this );
	CAppMode* pCur;
	UINT flags = 0;

  for( pCur = m_pMode ; pCur ; )
  {
    DrawMode( pCur, AM_FLBDOWN, &dc );
    pCur = UpdateMode( pCur, pCur->OnMouseMove( flags, loc ), AM_FLBDOWN, &dc );
  }
	if(  bRB )
	{
    for( pCur = m_pMode ; pCur ; )
		{
      DrawMode( pCur, AM_FLBDOWN, &dc );
      pCur = UpdateMode( pCur, pCur->OnRButtonDown( flags, loc ), AM_FLBDOWN, &dc );
		}
    for( pCur = m_pMode ; pCur ; )
		{
      DrawMode( pCur, AM_FLBUP, &dc );
      pCur = UpdateMode( pCur, pCur->OnRButtonUp( flags, loc ), AM_FLBUP, &dc );
		}
	}
	else
	{
    for( pCur = m_pMode ; pCur ; )
		{
      DrawMode( pCur, AM_FLBDOWN, &dc );
      pCur = UpdateMode( pCur, pCur->OnLButtonDown( flags, loc ), AM_FLBDOWN, &dc );
		}
    for( pCur = m_pMode ; pCur ; )
		{
      DrawMode( pCur, AM_FLBUP, &dc );
      pCur = UpdateMode( pCur, pCur->OnLButtonUp( flags, loc ), AM_FLBUP, &dc );
		}
	}
}
//=====================================================================
void
CMapView::OnLButtonDown( UINT flags, CPoint loc )
{
  m_bCrtMsg = 1;
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;
  OnMouseEvent( flags, loc, AM_FLBDOWN, CAppMode::OnLButtonDown );
	bLock = 0;
}
//=====================================================================
void
CMapView::OnRButtonDown( UINT flags, CPoint loc )
{
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;
  OnMouseEvent( flags, loc, AM_FRBDOWN, CAppMode::OnRButtonDown );
	bLock = 0;
}
//=====================================================================
void
CMapView::OnMButtonDown( UINT flags, CPoint loc )
{
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;
  OnMouseEvent( flags, loc, AM_FMBDOWN, CAppMode::OnMButtonDown );
	bLock = 0;
}
//=====================================================================
void
CMapView::OnLButtonDblClk( UINT flags, CPoint loc )
{
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;
  m_bCrtMsg = 1;
  m_bDblClick = 1;
  OnMouseEvent( flags, loc, AM_FLBDBLCLK, CAppMode::OnLButtonDblClk );
	bLock = 0;
}
//=====================================================================
void
CMapView::OnRButtonDblClk( UINT flags, CPoint loc )
{
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;
  OnMouseEvent( flags, loc, AM_FRBDBLCLK, CAppMode::OnRButtonDblClk );
	bLock = 0;
}
//=====================================================================
void
CMapView::OnMButtonDblClk( UINT flags, CPoint loc )
{
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;
  OnMouseEvent( flags, loc, AM_FMBDBLCLK, CAppMode::OnMButtonDblClk );
	bLock = 0;
}
//=====================================================================
void
CMapView::OnLButtonUp( UINT flags, CPoint loc )
{
  m_bCrtMsg = !m_bDblClick;
  m_bDblClick = 0;
  OnMouseEvent( flags, loc, AM_FLBUP, CAppMode::OnLButtonUp );
}
//=====================================================================
void
CMapView::OnRButtonUp( UINT flags, CPoint loc )
{
  OnMouseEvent( flags, loc, AM_FRBUP, CAppMode::OnRButtonUp );
}
//=====================================================================
void
CMapView::OnMButtonUp( UINT flags, CPoint loc )
{
  OnMouseEvent( flags, loc, AM_FMBUP, CAppMode::OnMButtonUp );
}
//=====================================================================
void
CMapView::OnMouseMove( UINT flags, CPoint loc )
{
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;
  OnMouseEvent( flags, loc, AM_FMOUSEMOVE, CAppMode::OnMouseMove );
  TestFLTopscr( &loc );
	bLock = 0;
}
//=====================================================================
void
CMapView::OnKeyDown( UINT key_code, UINT count, UINT flags )
{
	static int bLock = 0;
	if( bLock )
		return;
	bLock = 1;

  static UINT ect[][3] =
  {
    VK_DELETE, 0x11, 0x11,
    VK_LEFT,   0x12, 0x12,
    VK_RIGHT,  0x13, 0x13,
    VK_HOME,   0x14, 0x16,
    VK_END,    0x15, 0x17,
    VK_UP,     0x18, 0x18,
    VK_DOWN,   0x19, 0x19
  };

  CWnd::OnKeyDown( key_code, count, flags );

  int bCtrl = GetKeyState( VK_CONTROL ) < 0;
  int bShft = GetKeyState( VK_SHIFT   ) < 0;
  int i;
  for(i = CELEM( ect ) ; --i >= 0 && key_code != ect[i][0] ; );

  if( i >= 0 )
    OnChar( ect[i][bCtrl+1], count, 0 );
  else
  if(( key_code == VK_INSERT || key_code == 'c' || key_code == 'C' ) &&
       bCtrl && !bShft && OpenClipboard())
  {
    CViewDC wndDC( this );
    CMemDC memDC1( &wndDC, 1 );
    CMemDC memDC2( &wndDC, 1 );
    CBitmap bmp;
    bmp.CreateCompatibleBitmap( &wndDC, m_BitmapSize.cx, m_BitmapSize.cy );
    memDC1.SelectObject( &m_Bitmap );
    memDC2.SelectObject( &bmp );
    memDC2.BitBlt( 0, 0, m_BitmapSize.cx, m_BitmapSize.cy, &memDC1, 0, 0, SRCCOPY );
    EmptyClipboard();
    SetClipboardData( CF_BITMAP, HBITMAP( bmp ));
    CloseClipboard();
  }
#if 0
  else
  if( key_code == VK_SPACE )
  {
    CAutoScrollMode* pm = (CAutoScrollMode*)GetMode( IDM_AUTOSCROLL_MODE );
    if( pm )
    {
      pm->SetKeyPressed( 1 );
      SetCapture();
    }
  }
#endif
  OnSetCursor( this, 0, 0 );

  SendCtrMsg();
	bLock = 0;
}
//=====================================================================
void
CMapView::OnKeyUp( UINT key_code, UINT count, UINT flags )
{
  CWnd::OnKeyUp( key_code, count, flags );
  OnSetCursor( this, 0, 0 );
#if 0
  if( key_code == VK_SPACE )
  {
    CAutoScrollMode* pm = (CAutoScrollMode*)GetMode( IDM_AUTOSCROLL_MODE );
    if( pm )
    {
      pm->SetKeyPressed( 0 );
      ReleaseCapture();
    }
  }
#endif
}
//=====================================================================
int
CMapView::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
  static HCURSOR MC = App.LoadCursor( AFX_IDC_MOVE4WAY );
  static HCURSOR AC = App.LoadStandardCursor( IDC_ARROW );
  static HCURSOR GS = App.LoadCursor( IDC_GETSELECT );
  static HCURSOR IS = App.LoadCursor( IDC_INCSCALE );
  static HCURSOR crs[] = { 0, GS, IS, GS };

  CAppMode* pCur;
  for(pCur = m_pMode ; pCur && !pCur->OnSetCursor() ; pCur = pCur->m_pNext );

  int ks = ( GetKeyState( VK_SHIFT   ) < 0 ? 1 : 0 ) |
           ( GetKeyState( VK_CONTROL ) < 0 ? 2 : 0 );

  HCURSOR ci = crs[ ks & ( pCur ? pCur->TestLButtonDown() : 3 )];

  if( !ci && !GetMode( IDM_OBJEDIT_MODE ) && !m_SelectList.IsEmpty() && !m_pSTM )
  {
    CSpot spot( GetSpot());
		int dist = MAX_INT;
    for( POSITION pos = m_SelectList.GetHeadPosition() ; pos && !ci ; )
    {
      CDrawObject* pObj = m_SelectList.GetNext( pos );
      if( !pObj->IsSpecSelected() && pObj->Detected( spot, 0, &dist ))
        ci = MC;
    }
  }

  if( !ci && !pCur )
    ci = AC;

  if( ci )
    SetCursor( ci );

  return 1;
}
//=====================================================================
CAppMode::CAppMode( UINT nGroup, UINT nCursor, UINT nFlags, UINT nID ) :
  m_pNext( 0 ),
  m_nID( nID ),
  m_nCursor( nCursor ),
  m_nGroup( nGroup ),
  m_nMode( 0 ),
  m_nFlags( nFlags )
{}
//=====================================================================
int
CAppMode::OnChar( UINT& flags, UINT& key_code )
{
  return key_code == VK_ESCAPE  && ( m_nFlags & AM_FESCMODE ) ?
     AM_RREMOVE | AM_RDONE : AM_RPROPAGATE;
}
//=====================================================================
int
CAppMode::OnSetCursor()
{
  if( !m_nCursor || !IsMonMode() && !m_nMode )
    return 0;

  SetCursor( App.LoadCursor( m_nCursor ));
  return m_nCursor;
}
//=====================================================================
