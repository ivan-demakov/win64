#include "stdafx.h"

#include "scale.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
  #include "prjlib.h"
#endif
#include "mdoc.h"
#include "mview.h"
#include "mathutil.h"
#include "mercator.h"
#include "dlg0.h"
#include "dwin.h"
#include "scale.h"
#include "util.h"
#include "ksi_type.h"
#include "mercator.h"
#include <fstream>
#include <gdiplus.h>
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//=====================================================================
//=====================================================================
long CGridDialog::m_nStepX;
long CGridDialog::m_nStepY;
long CGridDialog::m_nOffsetX;
long CGridDialog::m_nOffsetY;
//=====================================================================
CSize
CGridDialog::GetXGrid()
{
  return CSize( m_nOffsetX, max( 10, m_nStepX / SmInUnit ));
}
//=====================================================================
CSize
CGridDialog::GetYGrid()
{
  return CSize( m_nOffsetY, max( 10, m_nStepY / SmInUnit ));
}
//=====================================================================
int
CGridDialog::OnInitDialog()
{
  static CSpinIni S[] =
  {
    { IDC_SCGD_XS_EDIT, IDC_SCGD_XS_SPIN, &m_nStepX, 10 * SmInUnit, 100 * SmInUnit, 1000000 * SmInUnit, 1, 100, "%1.2f" },
    { IDC_SCGD_YS_EDIT, IDC_SCGD_YS_SPIN, &m_nStepY, 10 * SmInUnit, 100 * SmInUnit, 1000000 * SmInUnit, 1, 100, "%1.2f" }
  };

  CDialog::OnInitDialog();
  for( int i = CELEM( edit ) ; --i >= 0 ; edit[i].Initialize( S[i], this ));
  return 1;
}
//=====================================================================
void
CGridDialog::OnOK()
{
  for( int i = CELEM( edit ) ; --i >= 0 ; edit[i].SetValue());
  EndDialog( 1 );
}
//=====================================================================
//=====================================================================
//=====================================================================
CPoint CGotoDialog::m_Point( 0, 0 );
Polar  CGotoDialog::m_Polar( 0, 0 );
int    CGotoDialog::m_scw;
int    CGotoDialog::m_bDecart = 1;
int    CGotoDialog::m_bFixPoint = 0;
//=====================================================================
BEGIN_MESSAGE_MAP( CGotoDialog, CDialog )
  ON_WM_HSCROLL()
  ON_EN_CHANGE( IDC_GTD_SCALE_EDIT, OnScaleChange )
  ON_BN_CLICKED( IDC_GTD_DECART_RADIO, OnCoordButtonClicked )
  ON_BN_CLICKED( IDC_GTD_POLAR_RADIO,  OnCoordButtonClicked )
  ON_BN_CLICKED( IDC_GTD_GMS_CHECK,    OnGMSButtonClicked   )
  ON_BN_CLICKED( IDC_GTD_LBTN,         OnLBClicked )
  ON_BN_CLICKED( IDC_GTD_RBTN,         OnRBClicked )
END_MESSAGE_MAP()
//======================================================================
void
CGotoDialog::OnScaleChange()
{
  m_pSCTRL = (CSliderCtrl*)GetDlgItem( IDC_GTD_SCALE_SLIDER );
  m_pSCTRL->SetPos( Scale2Pos( GetDlgItemInt( IDC_GTD_SCALE_EDIT )));
}
//=====================================================================
void
CGotoDialog::OnHScroll( UINT, UINT, CScrollBar* )
{
  m_pSCTRL = (CSliderCtrl*)GetDlgItem( IDC_GTD_SCALE_SLIDER );
  SetDlgItemInt( IDC_GTD_SCALE_EDIT, Pos2Scale( m_pSCTRL->GetPos()));
}
//======================================================================
int
CGotoDialog::Scale2Pos( int scale )
{
  return m_scw * ( log( MinScale ) - log( scale )) / ( log( MinScale ) - log( MaxScale ));
}
//======================================================================
int
CGotoDialog::Pos2Scale( int pos )
{
  return exp( log( MinScale ) - pos * ( log( MinScale ) - log( MaxScale )) / m_scw );
}
//======================================================================
int
CGotoDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  static sct[] = { MaxScale, MinScale, 500, 1500, 4000, 15000, 50000 };

  m_pSCTRL = (CSliderCtrl*)GetDlgItem( IDC_GTD_SCALE_SLIDER );

  CRect r;
  m_pSCTRL->GetChannelRect( &r );
  m_scw = r.Width();
  m_pSCTRL->SetRange( 0, m_scw );
  m_pSCTRL->SetPageSize( m_scw / 10 );

  for( int i = CELEM( sct ) ; --i >= 0 ; )
  {
    int sc = sct[i];
    if( sc >= MaxScale && sc <= MinScale )
      m_pSCTRL->SetTic( Scale2Pos( sc ));
  }

  m_pSCTRL->SetPos( m_Scale );
  SetDlgItemInt( IDC_GTD_SCALE_EDIT, m_Scale );
  
	Point tp( m_Point.x, m_Point.y );
  if( MerkatorData.Prj2Pol( tp, m_Polar ) != 1 )
    m_Polar = Dec2Pol( m_Point );
 	if( m_Polar.lat == 0 && m_Polar.lon == 0 )
	{
	  GetDlgItem( IDC_GTD_POLAR_RADIO )->EnableWindow( 0 ); 
	  GetDlgItem( IDC_GTD_GMS_CHECK )->EnableWindow( 0 ); 
	  m_bDecart = 1;
	}

  if( m_bDecart )
  {
    CheckDlgButton( IDC_GTD_DECART_RADIO, 1 );
    ShowDecart();
  }
  else
  {
    CheckDlgButton( IDC_GTD_POLAR_RADIO, 1 );
    ShowPolar();
  }

  return 1;
}
//=====================================================================
void
CGotoDialog::OnGMSButtonClicked()
{
  if( !m_bDecart )
    ShowPolar();
}
//=====================================================================
void
CGotoDialog::ShowDecart()
{
  GetDlgItem( IDC_GTD_STATIC0 )->SetWindowText( "X(м)" );
  GetDlgItem( IDC_GTD_STATIC1 )->SetWindowText( "Y(м)" );
  char b[32];
	sprintf( b, "%1.2f", m_Point.x / 100. * SmInUnit );
  SetDlgItemText( IDC_GTD_XS_EDIT, b );
	sprintf( b, "%1.2f", m_Point.y / 100. * SmInUnit );
  SetDlgItemText( IDC_GTD_YS_EDIT, b );
}
//=====================================================================
static void
G2GMS( double val, char* buf )
{
  double const r = 60.;
  val /= Deg;
  int g = floor( val );
  int m = floor(( val - g ) * r );
  double s = ( val - g - m / r ) * r * r;
  sprintf( buf, "%d\xb0%02d\'%02.4f\"", g, m, s );
}
//=====================================================================
static double
GMS2G( char* buf )
{
  double const r = 60.;
  double g = atof( buf );
  if( buf = strchr( buf, '\xb0' ))
  {
    g += atof( buf + 1 ) / r;
    if( buf = strchr( buf, '\'' ))
      g += atof( buf + 1 ) / r / r;
  }
  return g;
}
//=====================================================================
void
CGotoDialog::ShowPolar()
{
  GetDlgItem( IDC_GTD_STATIC0 )->SetWindowText( "Д(гр)" );
  GetDlgItem( IDC_GTD_STATIC1 )->SetWindowText( "Ш(гр)" );
  char t[32];
  if( IsDlgButtonChecked( IDC_GTD_GMS_CHECK ))
  {
    G2GMS( m_Polar.lon, t );
    SetDlgItemText( IDC_GTD_XS_EDIT, t );
    G2GMS( m_Polar.lat, t );
    SetDlgItemText( IDC_GTD_YS_EDIT, t );
  }
  else
  {
    sprintf( t, "%1.6f", m_Polar.lon / Deg );
    SetDlgItemText( IDC_GTD_XS_EDIT, t );
    sprintf( t, "%1.6f", m_Polar.lat / Deg );
    SetDlgItemText( IDC_GTD_YS_EDIT, t );
  }
}
//=====================================================================
void
CGotoDialog::PolDec()
{
  char tx[32];
  char ty[32];
  GetDlgItemText( IDC_GTD_XS_EDIT, tx, sizeof tx - 1 );
  GetDlgItemText( IDC_GTD_YS_EDIT, ty, sizeof ty - 1 );

	int bGMS = IsDlgButtonChecked( IDC_GTD_GMS_CHECK );
	m_Polar.lon = ( bGMS ? GMS2G( tx ) : atof( tx )) * Deg;
  m_Polar.lat = ( bGMS ? GMS2G( ty ) : atof( ty )) * Deg;
  if( !MerkatorData.Pol2Prj( m_Polar, *((Point*)&m_Point )))
    m_Point = Pol2Dec( m_Polar );
	m_Point = CMapView::Project2Grid( m_Point );
}
//=====================================================================
void
CGotoDialog::DecPol()
{
  char tx[32];
  char ty[32];
  GetDlgItemText( IDC_GTD_XS_EDIT, tx, sizeof tx - 1 );
  GetDlgItemText( IDC_GTD_YS_EDIT, ty, sizeof ty - 1 );

	m_Point.x = atof( tx ) * 1e2 / SmInUnit;
  m_Point.y = atof( ty ) * 1e2 / SmInUnit;
	CPoint t = CMapView::Grid2Project( m_Point );
	Point tp( t.x, t.y );
  if( !MerkatorData.Prj2Pol( tp, m_Polar ))
    m_Polar = Dec2Pol( t );
}
//=====================================================================
void
CGotoDialog::OnCoordButtonClicked()
{
  int bOldDec = m_bDecart;
  m_bDecart = IsDlgButtonChecked( IDC_GTD_DECART_RADIO );
  if( m_bDecart == bOldDec )
    return;

  if( m_bDecart )
  {
		PolDec();
    ShowDecart();
  }
  else
  {
		DecPol();
    ShowPolar();
  }
}
//======================================================================
void
CGotoDialog::UpdateCmd( int cmd )
{
  if( m_bDecart )
		DecPol();
	else
		PolDec();

  m_Scale = GetDlgItemInt( IDC_GTD_SCALE_EDIT );
  CSize s( pView->GetDocument()->GetSize());
	CPoint p( CMapView::Grid2Project( m_Point ));

  if( p.x < 0 || p.y < 0 || p.x >= s.cx && p.y >= s.cy )
    ErrBox( "Попытка выхода за габарит проекта" );
	else
		switch( cmd )
		{
			case 0: pView->GoTo( p, m_Scale ); break;
			case 1: pView->OnFixMouse( p, 0 ); break;
			case 2: pView->OnFixMouse( p, 1 ); break;
	}
}
//=====================================================================
void
CGotoDialog::OnOK()
{
	UpdateCmd( 0 );
}
//=====================================================================
void
CGotoDialog::OnLBClicked()
{
	UpdateCmd( 1 );
}
//=====================================================================
void
CGotoDialog::OnRBClicked()
{
	UpdateCmd( 2 );
}
//=====================================================================
void
CGotoDialog::OnCancel()
{
	ShowWindow( SW_HIDE );
}
//=====================================================================
IMPLEMENT_DYNCREATE( COverviewDialog, CDialog )
//=====================================================================
BEGIN_MESSAGE_MAP( COverviewDialog, CDialog )
  ON_WM_MOVE()
  ON_WM_CLOSE()
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
//=====================================================================
CString COverviewDialog::m_OverviewPath;
int    COverviewDialog::IsOverview;
//=====================================================================
int
COverviewDialog::CreateBmp( CRect* pRect )
{
  CWaitCursor wc;
  ReportHint( "Формирование обзора..." );

  CViewDC dc( pView );
  GetClientRect( &m_Wnd );
  CRect cr( m_Wnd );

  CRect r1, r2( 0, 0, 0, 0 );

  if( pRect )
    r1 = *pRect;
  else
  {
    CSize ds( pView->GetDocument()->GetSize());
    r1.SetRect( 0, 0, ds.cx, ds.cy );
  }

  int scale = pView->WindowSize2Scale( r1.Size());
	CPoint t( r1.CenterPoint());
	Point tp( t.x, t.y );
  scale /= MerkatorData.ScaleFactor( tp );

  CGraphData gd;
  pView->GetDocument()->CreateLayers( &gd );
  pView->m_ScaleOnLoading = 0;
  pView->LoadFromStore( &gd, r1, r2, scale, r2, scale );

  for( int i = FULL_RANGE ; --i >= CLASS_RANGE ; gd.GetLayerByClass( i )->RemoveAll( 0 ));

  pView->m_ScaleOnLoading = 1;

//  m_Box = pRect ? *pRect : gd.CalcBoundBox();
  m_Box = pRect ? *pRect : CRect( CPoint( 0, 0 ), pView->GetDocument()->GetSize());

  if( m_Box.IsRectNull())
    m_Box = r1;

  if( MulDiv( m_Box.Width(), m_Wnd.Height(), m_Wnd.Width()) < m_Box.Height())
    m_Wnd.right = cr.left + MulDiv( cr.Height(), m_Box.Width(), m_Box.Height());
  else
    m_Wnd.bottom = cr.top + MulDiv( cr.Width(), m_Box.Height(), m_Box.Width());

  CMemDC memDC( &dc );
  pView->OnPrepareDC( &memDC );

  m_Bmp.DeleteObject();
  m_Bmp.CreateCompatibleBitmap( &dc, m_Wnd.Width(), m_Wnd.Height());
  m_hBmp = HBITMAP( m_Bmp );

  memDC.SelectObject( &m_Bmp );
  memDC.SetViewportOrg( m_Wnd.TopLeft());
  memDC.SetViewportExt( m_Wnd.Size());
  memDC.SetWindowOrg( m_Box.TopLeft());
  memDC.SetWindowExt( m_Box.Size());
  memDC.FillSolidRect( &m_Box, RGB( -1, -1, -1 ));
  gd.Draw( &memDC, OBJLAYER, 0, 0 );

  int saved = 1;
  using namespace Gdiplus;
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, 0 );
  {
    Bitmap bmp( m_Bmp, 0 );
    WCHAR wbuf[_MAX_PATH];
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, m_OverviewPath, -1, wbuf, _MAX_PATH );

    CLSID bmpClsid;
    UINT  num, size;
    GetImageEncodersSize( &num, &size );
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*) new char[size];
    GetImageEncoders( num, size, pImageCodecInfo );
    for( int i = num ; --i >= 0 && wcscmp( pImageCodecInfo[i].MimeType, L"image/bmp" ) ; );
    if( i >= 0 )
      bmpClsid = pImageCodecInfo[i].Clsid;
    delete pImageCodecInfo;

    if( i < 0 || bmp.Save( wbuf, &bmpClsid, 0 ) != Ok )
    {
      ErrBox( Compose( "Не удается сохранить обзор\nв файле ", m_OverviewPath, 0 ));
      saved = 0;
      pView->ShowSelectedNum();
    }
    else
    {
      std::ofstream os( m_OverviewPath, ios::out | ios::app | ios::binary );
      os.write((char*)&m_Box, sizeof m_Box );
      os.close();
    }
  }
  GdiplusShutdown( gdiplusToken );
  return saved;
}
//=====================================================================
int
COverviewDialog::Refresh( CRect* pRect )
{
  CWaitCursor wc;
  DeleteObject( m_hBmp );
  DestroyWindow();
  if( FileExist( m_OverviewPath ))
    CFile::Remove( m_OverviewPath );
  Init( pRect );
  return 1;
}
//=====================================================================
void
COverviewDialog::Init( CRect* pRect )
{
  if( IsOverview )
  {
    IsOverview = 0;
    DeleteObject( m_hBmp );
  }

  if( IsWindow( m_hWnd ))
    DestroyWindow();

  Create( IDD_OVERVIEW, pView );

  CString prjPath( App.GetProfileString( "General", "project", "" ));
  char bn[_MAX_FNAME], tmp[_MAX_PATH];
  _splitpath( prjPath, tmp, tmp, bn, tmp );
  _makepath( tmp, "", "", bn, "bmp" );

  m_OverviewPath = App.GetProjectFilePath( tmp );

  m_nMode = 0;

  IsOverview = 0;

  if( FileExist( m_OverviewPath ))
  {
    m_hBmp = (HBITMAP)LoadImage( 0, m_OverviewPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
    if( m_hBmp )
    {
      BITMAP bm;
      CBitmap::FromHandle( m_hBmp )->GetBitmap( &bm );
      m_Wnd.SetRect( 0, 0, bm.bmWidth, bm.bmHeight );
      CFile in( m_OverviewPath, CFile::modeRead|CFile::typeBinary );
      in.Seek( 0 - sizeof m_Box, CFile::end );
      in.Read((char*)&m_Box, sizeof m_Box );
      in.Close();
      IsOverview = 1;
    }
  }

#if defined( _DEBUG ) || !defined( _FULL_PROJECT )
  if( !IsOverview )
  {
    CreateBmp( pRect );
    IsOverview = 1;
    pView->ShowSelectedNum();
  }
#endif

  WINDOWPLACEMENT wp;
  GetWindowPlacement( &wp );

  CRect cr( m_Wnd );
  GetClientRect( &cr );

  wp.rcNormalPosition.right -= wp.rcNormalPosition.left;
  wp.rcNormalPosition.left = 0;
  wp.rcNormalPosition.bottom -= wp.rcNormalPosition.top;
  wp.rcNormalPosition.top = 0;
  wp.rcNormalPosition.right  += m_Wnd.Width()  - cr.Width();
  wp.rcNormalPosition.bottom += m_Wnd.Height() - cr.Height();
  wp.showCmd = SW_HIDE;
  SetWindowPlacement( &wp );
}
//=====================================================================
COverviewDialog::~COverviewDialog()
{
  DeleteObject( m_hBmp );
}
//=====================================================================
void
COverviewDialog::MoveRect()
{
  Draw();
  CalcRect();
}
//=====================================================================
void
COverviewDialog::CalcRect()
{
  CRect vr( pView->GetScrollPosition(), pView->GetWindowSize());
  vr -= m_Box.TopLeft();
  CPoint c( vr.CenterPoint());
  int m = m_Wnd.Width(), d = m_Box.Width();
  m_Rect.SetRect( MulDiv( c.x, m, d ),
                  MulDiv( c.y, m, d ),
                  MulDiv( vr.right,  m, d ),
                  MulDiv( vr.bottom, m, d ));
  Draw();
}
//=====================================================================
void
COverviewDialog::OnPaint()
{
  CWnd::OnPaint();
  CalcRect();
  m_nMode = 0;
}
//=====================================================================
void
COverviewDialog::Draw()
{
  CRect rect( m_Rect );
  rect.TopLeft() -= m_Rect.Size();
  rect.NormalizeRect();
  rect.SetRect( max( rect.left, m_Wnd.left ),
                max( rect.top, m_Wnd.top ),
                min( rect.right, m_Wnd.right ),
                min( rect.bottom, m_Wnd.bottom ));
  if( rect.IsRectEmpty())
    rect.InflateRect( 1, 1 );
  CPen pen( PS_SOLID, 2, RGB( 0xff, 0x00, 0x00 ));
  CClientDC dc( this );
  CMemDC memDC( &dc );
  memDC.SelectObject( m_hBmp );
  dc.BitBlt( 0, 0, m_Wnd.Width(), m_Wnd.Height(), &memDC, 0, 0, SRCCOPY );
  dc.SelectStockObject( NULL_BRUSH );
  dc.SelectObject( &pen );
  dc.SetROP2( R2_COPYPEN );
  dc.Rectangle( rect );
}
//=====================================================================
void
COverviewDialog::OnLButtonDown( UINT nFlags, CPoint pnt )
{
  Draw();
  m_nMode = 1;
  m_Rect.TopLeft() = m_Rect.BottomRight() = pnt;
  SetCapture();
}
//=====================================================================
void
COverviewDialog::CorrectRect()
{
  CSize vs( pView->GetViewportSize());

  int dx = abs( m_Rect.Width());
  int dy = abs( m_Rect.Height());
  m_Rect.right  = m_Rect.left + max( MulDiv( vs.cx, dy, vs.cy ), dx );
  m_Rect.bottom = m_Rect.top  + max( MulDiv( vs.cy, dx, vs.cx ), dy );

  CSize ws( pView->Scale2WindowSize( MinScale ));

  int nw = MulDiv( ws.cx, m_Wnd.Width(), 2 * m_Box.Width());
  int nh = MulDiv( ws.cy, m_Wnd.Width(), 2 * m_Box.Width());

  m_Rect.right  = m_Rect.left + min( m_Rect.Width(),  nw );
  m_Rect.bottom = m_Rect.top  + min( m_Rect.Height(), nh );

  Draw();
}
//=====================================================================
void
COverviewDialog::OnMouseMove( UINT nFlags, CPoint pnt )
{
  if( m_nMode )
    if( nFlags & MK_LBUTTON )
    {
      Draw();
      m_Rect.BottomRight() = pnt;
      CorrectRect();
    }
    else
      m_nMode = 0;
}
//=====================================================================
void
COverviewDialog::OnLButtonUp( UINT nFlags, CPoint pnt )
{
  m_Rect.TopLeft() -= m_Rect.Size();
  m_Rect.NormalizeRect();
  m_Rect.SetRect( max( m_Rect.left,   m_Wnd.left   ),
                  max( m_Rect.top,    m_Wnd.top    ),
                  min( m_Rect.right,  m_Wnd.right  ),
                  min( m_Rect.bottom, m_Wnd.bottom ));

  CPoint c( m_Rect.CenterPoint());

  CPoint cntr( m_Box.left + MulDiv( c.x - m_Wnd.left, m_Box.Width(), m_Wnd.Width()),
               m_Box.top  + MulDiv( c.y - m_Wnd.top,  m_Box.Height(), m_Wnd.Height()));

  if( IsWindowVisible())
  {
    int scale = m_Rect.IsRectEmpty() ?
      pView->GetAltScale() :
      pView->WindowSize2Scale( CSize( MulDiv( m_Box.Width(),  m_Rect.Width(),  m_Wnd.Width()),
                                      MulDiv( m_Box.Height(), m_Rect.Height(), m_Wnd.Height())));
        pView->SetScale( scale, cntr, 1 );
  }

  m_nMode = 0;
  ReleaseCapture();
}
//=====================================================================
long CSetDistDialog::m_Dist = 100;
//=====================================================================
int
CSetDistDialog::OnInitDialog()
{
  static CSpinIni S[] =
  {{ IDC_SDD_DIST_EDIT, IDC_SDD_DIST_SPIN, &m_Dist, 0, 100, 1000000, 1, 100, "%1.2f" },
   { IDC_SDD_DIST_EDIT, IDC_SDD_DIST_SPIN, &m_Dist, 0, 1, 100 }};

  CDialog::OnInitDialog();
  SetWindowText( m_Name );

  if( m_VarName )
  {
    ksi_obj ksi_val = ksi_eval_str_with_catch( m_VarName );
    m_Dist = KSI_NUM_P( ksi_val ) ? ksi_num2int( ksi_val ) : 0;
    m_Dist *= SmInUnit;
    m_edit0.Initialize( S[0], this );
  }
  else
    m_edit1.Initialize( S[1], this );

  return 1;
}
//=====================================================================
void
CSetDistDialog::OnOK()
{
  if( m_VarName )
  {
    m_edit0.SetValue();
    m_Dist /= SmInUnit;
    ksi_defsym( m_VarName, ksi_int2num( m_Dist ), ksi_current_env());
  }
  else
    m_edit1.SetValue();

  EndDialog( 1 );
}
//=====================================================================
//=====================================================================
void
CShapeDialog::Init( CMapView* pView, int init )
{
  static int inited = 0;

  int s = 0;
  if( inited )
  {
    s = ((CComboBox*)GetDlgItem( IDC_CTRL ))->GetCurSel();
    DestroyWindow();
  }

  inited = init;

  if( !init )
    return;

  Create( IDD_SHAPE_DIALOG, pView );
  CComboBox* pCtrl = (CComboBox*)GetDlgItem( IDC_CTRL );
  pCtrl->SetCurSel( s );

  CRect mwr, dwr;
  pView->GetWindowRect( mwr );
  GetWindowRect( dwr );
  MoveWindow( mwr.right - dwr.Width(),
              mwr.top - dwr.Height(),
              dwr.Width(),
              dwr.Height());

  EnableToolTips();
  ShowWindow( SW_SHOW );
}
//=====================================================================
int
CShapeDialog::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
  pTI->uFlags = TTF_IDISHWND;
  pTI->uId = (UINT)GetDlgItem( IDC_CTRL )->GetSafeHwnd();
  strcpy( pTI->lpszText = new char[16], "Форма линии" );
  return 1;
}
//=====================================================================
int
CShapeDialog::GetShape()
{
  return ((CComboBox*)GetDlgItem( IDC_CTRL ))->GetCurSel();
}
//=====================================================================
void
CShapeDialog::SetShape( int n )
{
 ((CComboBox*)GetDlgItem( IDC_CTRL ))->SetCurSel(( n + 10 ) % 10 );
}
//=====================================================================
//=====================================================================
long CAngleDialog::m_AngleGrid = 50;
//=====================================================================
int
CAngleDialog::OnInitDialog()
{
  static CSpinIni S =
  { IDC_GTD_XS_EDIT, IDC_GTD_XS_SPIN, &m_AngleGrid, 1, 50, 1800, 1, 10, "%1.1f" };

  CDialog::OnInitDialog();

  m_Edit.Initialize( S, this );
  return 1;
}
//=====================================================================
void
CAngleDialog::OnOK()
{
  m_Edit.SetValue();
  if( m_AngleGrid )
    EndDialog( 1 );
  else
    ErrBox( "Шаг угла поворота должен отличаться от нуля!" );
}
//=====================================================================
//=====================================================================
CPoint CCoordGridDialog::m_ProjPoint( 0, 0 );
CPoint CCoordGridDialog::m_GridPoint( 0, 0 );
CPoint CCoordGridDialog::m_GridStep( 500, 500 );
CPoint CCoordGridDialog::m_GridDir( 0, 0 );
//=====================================================================
int
CCoordGridDialog::OnInitDialog()
{
  static CSpinIni S[] =
  {
    { IDC_CGD_XC_EDIT, IDC_CGD_XC_SPIN, &m_ProjPoint.x, 0x80000000, 100, 0x7fffffff },
    { IDC_CGD_YC_EDIT, IDC_CGD_YC_SPIN, &m_ProjPoint.y, 0x80000000, 100, 0x7fffffff },
    { IDC_CGD_XP_EDIT, IDC_CGD_XP_SPIN, &m_GridPoint.x, 0x80000000, 100, 0x7fffffff },
    { IDC_CGD_YP_EDIT, IDC_CGD_YP_SPIN, &m_GridPoint.y, 0x80000000, 100, 0x7fffffff },
    { IDC_CGD_XS_EDIT, IDC_CGD_XS_SPIN, &m_GridStep.x,  100, 100, 10000 },
    { IDC_CGD_YS_EDIT, IDC_CGD_YS_SPIN, &m_GridStep.y,  100, 100, 10000 }
  };

  int xsg = CMapView::m_XStep > 0 ? 1 : -1;
  int ysg = CMapView::m_YStep > 0 ? 1 : -1;
  m_ProjPoint.x /= 100;
  m_ProjPoint.y /= 100;
  m_GridPoint.x = CMapView::m_X0 + m_ProjPoint.x * xsg;
  m_GridPoint.y = CMapView::m_Y0 + m_ProjPoint.y * ysg;
  m_GridStep.x = abs( CMapView::m_XStep );
  m_GridStep.y = abs( CMapView::m_YStep );

  CDialog::OnInitDialog();

  for( int i = CELEM( S ); --i >= 0 ; m_Edit[i].Initialize( S[i], this ));

  CheckRadioButton( IDC_CGD_XL_RADIO, IDC_CGD_XR_RADIO, xsg < 0 ? IDC_CGD_XL_RADIO : IDC_CGD_XR_RADIO );
  CheckRadioButton( IDC_CGD_YU_RADIO, IDC_CGD_YD_RADIO, ysg < 0 ? IDC_CGD_YU_RADIO : IDC_CGD_YD_RADIO );
  return 1;
}
//=====================================================================
void
CCoordGridDialog::OnOK()
{
  for( int i = 6 ; --i >= 0 ; m_Edit[i].SetValue());
  m_GridDir.x = IsDlgButtonChecked( IDC_CGD_XL_RADIO );
  m_GridDir.y = IsDlgButtonChecked( IDC_CGD_YU_RADIO );
  EndDialog( 1 );
}
//=====================================================================
//=====================================================================
int
CCheckDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  int n, i;

  SetWindowText( m_Name );
  for( n = 0 ; m_pCheck[n].Name ; ++n );
  m_pBoxes = new CButton[n];

  CDC* pDC = GetWindowDC();

  CSize ex( pDC->GetTextExtent( m_Name, strlen( m_Name )));
  int w = ex.cx + 16;
  CPoint org( 8, 8 );
  UINT id = 1000;
  CButton* pButton;

  for( i = 0 ; i < n ; ++i, org.y += 32 )
  {
    pButton = m_pBoxes + i;
    char const* pName = m_pCheck[i].Name;
    CSize ex( pDC->GetTextExtent( pName, strlen( pName )));
    CRect rect( org.x, org.y, org.x + ex.cx + 32, org.y + 24 );
    pButton->Create( pName, BS_AUTOCHECKBOX, rect, this, id++ );
    pButton->SetFont( GetFont());
    pButton->ShowWindow( SW_SHOW );
    pButton->SetCheck( *m_pCheck[i].pValue != 0 );
    w = max( w, rect.right );
  }

  WINDOWPLACEMENT wp;
  pButton = (CButton*)GetDlgItem( IDOK );
  pButton->GetWindowPlacement( &wp );
  int h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.top = org.y;
  wp.rcNormalPosition.bottom = org.y + h;
  pButton->SetWindowPlacement( &wp );
  pButton = (CButton*)GetDlgItem( IDCANCEL );
  pButton->GetWindowPlacement( &wp );
  wp.rcNormalPosition.top = org.y;
  wp.rcNormalPosition.bottom = org.y + h;
  pButton->SetWindowPlacement( &wp );

  GetWindowPlacement( &wp );
  wp.rcNormalPosition.right  = max( wp.rcNormalPosition.right, wp.rcNormalPosition.left + w );
  wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + org.y + h + 40;
  SetWindowPlacement( &wp );

  return 1;
}
//=====================================================================
void
CCheckDialog::OnOK()
{
  for( int i = 0 ; m_pCheck[i].Name ; ++i )
    *m_pCheck[i].pValue = m_pBoxes[i].GetCheck();
  EndDialog( 1 );
}
//=====================================================================
//=====================================================================
BEGIN_MESSAGE_MAP( CSplDialog, CDialog )
  ON_BN_CLICKED( IDC_SPL_CURSCALE_ENB, OnScaleButton )
  ON_BN_CLICKED( IDC_SPL_RECT_RGN,   OnRgnTypeChange )
  ON_BN_CLICKED( IDC_SPL_UNRECT_RGN, OnRgnTypeChange )
  ON_BN_CLICKED( IDC_SPL_AUTO_RGN,   OnRgnTypeChange )
END_MESSAGE_MAP()
//=====================================================================
int  CSplDialog::m_bRgnAuto     = 0;
int  CSplDialog::m_nRgnType     = 0;
int  CSplDialog::m_bToClipboard = 0;
int  CSplDialog::m_bToFile      = 1;
int  CSplDialog::m_bToMetafile  = 0;
int  CSplDialog::m_bColorEnb    = 1;
int  CSplDialog::m_bSelectEnb   = 0;
int  CSplDialog::m_bCurScaleEnb = 1;
long CSplDialog::m_nExtraSize   = 100;
int  CSplDialog::m_nScale       = 2000;
//=====================================================================
void
CSplDialog::OnScaleButton()
{
  GetDlgItem( IDC_SPL_SCALE_EDIT )->EnableWindow( !IsDlgButtonChecked( IDC_SPL_CURSCALE_ENB ));
}
//=====================================================================
void
CSplDialog::OnRgnTypeChange()
{
  GetDlgItem( IDC_SPL_EXTRA_SIZE_EDIT )->EnableWindow( IsDlgButtonChecked( IDC_SPL_AUTO_RGN ));
}
//=====================================================================
void
CSplDialog::DoDataExchange( CDataExchange* pDX )
{
  DDX_Check( pDX, IDC_SPL_TO_CLIPBOARD,    m_bToClipboard );
  DDX_Check( pDX, IDC_SPL_TO_FILE,         m_bToFile      );
  DDX_Check( pDX, IDC_SPL_TO_METAFILE,     m_bToMetafile  );
  DDX_Check( pDX, IDC_SPL_COLOR_ENB,       m_bColorEnb    );
  DDX_Check( pDX, IDC_SPL_SELECT_ENB,      m_bSelectEnb   );
  DDX_Check( pDX, IDC_SPL_CURSCALE_ENB,    m_bCurScaleEnb );
  DDX_Text ( pDX, IDC_SPL_SCALE_EDIT,      m_nScale       );
  DDX_Radio( pDX, IDC_SPL_RECT_RGN,        m_nRgnType     );
  DDX_Check( pDX, IDC_SPL_AUTO_RGN,        m_bRgnAuto     );
  DDX_Text ( pDX, IDC_SPL_EXTRA_SIZE_EDIT, m_nExtraSize   );
}
//=====================================================================
int
CSplDialog::OnInitDialog()
{
  static CSpinIni S =
  {
    IDC_SPL_EXTRA_SIZE_EDIT, IDC_SPL_EXTRA_SIZE_SPIN, &m_nExtraSize, 10, 10, 1000
  };

  CDialog::OnInitDialog();
  m_Edit.Initialize( S, this );
  OnScaleButton();
  OnRgnTypeChange();
  return 1;
}
//=====================================================================
//=====================================================================
IMPLEMENT_DYNCREATE( CObjSelectDialog, CDialog )
//=====================================================================
BEGIN_MESSAGE_MAP( CObjSelectDialog, CDialog )
  ON_NOTIFY( NM_DBLCLK, IDC_OSL_OBJECT_LIST, OnObjListDblClick )
END_MESSAGE_MAP()
//=====================================================================
BOOL
CObjSelectDialog::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
  int r = CWnd::OnNotify( wParam, lParam, pResult );
  NMHDR* ph = (NMHDR*)lParam;
  if( ph->code == LVN_ITEMCHANGED )
  {
    NMLVODSTATECHANGE* pk = (NMLVODSTATECHANGE*)ph;
    if( pk->uNewState == 3 && pk->uOldState != pk->uNewState )
      if( wParam == IDC_OSL_CLASS_LIST )
        UpdateClass( pk->iFrom );
      else
      if( IsDlgButtonChecked( IDC_OSL_SHOW_OBJECT_CHECK ))
        UpdateObject( pk->iFrom );
  }
  return r;
}
//=====================================================================
void
CObjSelectDialog::Init( CMapView* pView, ObjectInfoArray* pArray )
{
  if( IsWindow( m_hWnd ))
    DestroyWindow();

  m_pArray = pArray;
  Create( IDD_SELECT_OBJ_DIALOG, m_pView = pView );
}
//=====================================================================
void
CObjSelectDialog::OnCancel()
{
  DestroyWindow();
}
//=====================================================================
int
CObjSelectDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  UINT S = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB ;
  COLORREF Y = RGB( 255, 255, 191 );

  CRect r;
  CListCtrl* pBox0 = (CListCtrl*)GetDlgItem( IDC_OSL_CLASS_LIST );
  CListCtrl* pBox1 = (CListCtrl*)GetDlgItem( IDC_OSL_OBJECT_LIST );
  pBox0->SetExtendedStyle( S );
  pBox1->SetExtendedStyle( S );
  pBox0->SetBkColor( Y );
  pBox0->SetTextBkColor( Y );

  pBox0->GetClientRect( &r );
  pBox0->InsertColumn( 0, "Класс", LVCFMT_LEFT, r.Width() - 80, 0 );
  pBox0->InsertColumn( 1, "Найдено", LVCFMT_LEFT, 90, 1 );
  pBox1->InsertColumn( 0, "№ п/п", LVCFMT_LEFT, 45, 0 );

  int i, n = 0, s[CLASS_RANGE];
  memset( s, 0, sizeof s );
  for( i = m_pArray->GetSize() ; --i >= 0 ; s[m_pArray->GetAt( i ).ObjClass] += 1 );
  for( i = CELEM( s ) ; --i > 0 ; )
    if( s[i] )
    {
      char m[16];
      sprintf( m, "%d", s[i] );
      LVITEM i0 = { LVIF_TEXT|LVIF_PARAM, n, 0, 0, 0, LPSTR( m_pView->m_pObjDef->GetName( i )), 0, 0, i };
      pBox0->InsertItem( &i0 );
      pBox0->SetItemText( n++, 1, m );
    }

  pBox0->SetSelectionMark( 0 );
  return 1;
}
//=====================================================================
void
CObjSelectDialog::UpdateClass( int n )
{
  CListCtrl* pBox = (CListCtrl*)GetDlgItem( IDC_OSL_CLASS_LIST );

  int t = pBox->GetItemData( n );
  n = 0;

  m_pView->StartBlinking( 0 );
  pBox = (CListCtrl*)GetDlgItem( IDC_OSL_OBJECT_LIST );
  pBox->DeleteAllItems();
  CRect r;
  pBox->GetClientRect( &r );
  pBox->DeleteColumn( 1 );
  pBox->InsertColumn( 1, LPSTR( m_pView->m_pObjDef->GetName( t )), LVCFMT_LEFT, r.Width() - 50, 1 );

  for( int i = m_pArray->GetSize() ; --i >= 0 ; )
    if( m_pArray->GetAt( i ).ObjClass == t )
    {
      CTabIdent id( m_pArray->GetAt( i ).ObjIdent );
      CSmartObjPtr ptr;
      CDrawObject* pObj = m_pView->GetObjectPtr( id );
      if( !pObj )
        ptr = pObj = m_pView->ExtractMapObj( id );
      if( pObj )
      {
        CString S;
        for( CVisiblePrim* pPrim = pObj->GetActualCont()->GetHead() ; pPrim ; pPrim = pPrim->GetNext())
          if( pPrim->IsAnyVisible() && pPrim->GetType() == KEY_TEXT )
          {
            S += ((CText*)pPrim )->GetStringPtr()->m_String;
            S += " | ";
          }
        S = S.Left( S.GetLength() - 3 );
        char m[16];
        sprintf( m, "%d", n + 1 );
        LVITEM i0 = { LVIF_TEXT|LVIF_PARAM, n, 0, 0, 0, m, 0, 0, i };
        pBox->InsertItem( &i0 );
        pBox->SetItemText( n, 1, S );
        ++n;
      }
    }
}
//=====================================================================
void
CObjSelectDialog::UpdateObject( int n )
{
  CListCtrl* pBox = (CListCtrl*)GetDlgItem( IDC_OSL_OBJECT_LIST );
  if( !m_pView->FindObject( m_pArray->GetAt( pBox->GetItemData( n )).ObjIdent, 1, 1 ))
    ErrBox( "Объект не найден!" );
}
//=====================================================================
void
CObjSelectDialog::OnObjListDblClick()
{
  CListCtrl* pBox = (CListCtrl*)GetDlgItem( IDC_OSL_OBJECT_LIST );
  int n = pBox->GetSelectionMark();
  if( n >= 0 )
    UpdateObject( n );
}
//=====================================================================
//=====================================================================
int CRepDialog::m_bRepers = 1;
int CRepDialog::m_bOneError;
int CRepDialog::m_bAllErrors;
CString CRepDialog::m_FilePath;
//=====================================================================
BEGIN_MESSAGE_MAP( CRepDialog, CDialog )
  ON_BN_CLICKED( IDC_RPD_FILE_PATH_BUTTON, OnFilePathButton )
END_MESSAGE_MAP()
//=====================================================================
void
CRepDialog::DoDataExchange( CDataExchange* pDX )
{
  DDX_Check( pDX, IDC_RPD_REPERS_CHECK,     m_bRepers    );
  DDX_Check( pDX, IDC_RPD_ONE_ERROR_CHECK,  m_bOneError  );
  DDX_Check( pDX, IDC_RPD_ALL_ERRORS_CHECK, m_bAllErrors );
  DDX_Text ( pDX, IDC_RPD_FILE_PATH_EDIT,   m_FilePath   );
}
//=====================================================================
void
CRepDialog::OnFilePathButton()
{
  CFileDialog* fdlg =
    new CFileDialog( 1, ".txt", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*||", this );

  int r = fdlg->DoModal();
  m_FilePath = fdlg->GetPathName();
  delete fdlg;
  if( r == IDOK )
    GetDlgItem( IDC_RPD_FILE_PATH_EDIT )->SetWindowText( m_FilePath );
}
//=====================================================================
long CCloneParamDialog::m_Step = 50;
long CCloneParamDialog::m_nRep = 1;
int  CCloneParamDialog::m_nDir = 1;
//=====================================================================
BEGIN_MESSAGE_MAP( CCloneParamDialog, CDialog )
  ON_BN_CLICKED( IDC_CPD_RADIO_0, OnDirBtn )
  ON_BN_CLICKED( IDC_CPD_RADIO_1, OnDirBtn )
END_MESSAGE_MAP()
//=====================================================================
int
CCloneParamDialog::OnInitDialog()
{
  static CSpinIni S[] =
  {
    { IDC_CPD_REP_EDIT,  IDC_CPD_REP_SPIN,  &m_nRep, 1, 1, 99 },
    { IDC_CPD_STEP_EDIT, IDC_CPD_STEP_SPIN, &m_Step, 10 * SmInUnit, 10 * SmInUnit, 1000 * SmInUnit, 1, 100, "%1.2f" }
  };

  CDialog::OnInitDialog();

	int i;
  for( i = CELEM( S ) ; --i >= 0 ; )
		i ? m_Edit1.Initialize( S[i], this ) : m_Edit0.Initialize( S[i], this );

  static UINT bb[][2] =
  {
    IDC_CPD_RADIO_1, IDB_GREEN,
    IDC_CPD_RADIO_0, IDB_BLUE
  };

  for( i = CELEM( bb ) ; --i >= 0 ; )
  {
    HBITMAP hBmp = (HBITMAP)LoadImage( App.m_hInstance, LPCTSTR( bb[i][1] ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
    if( hBmp )
      ((CButton*) GetDlgItem( bb[i][0] ))->SetBitmap( hBmp );
  }

	m_ExampleBox.SubclassDlgItem( IDC_CPD_EXAMPLE_BOX, this );
	m_ExampleBox.Init( m_Points, m_nPoint );
	m_ExampleBox.AddString( "" );
  return 1;
}
//=====================================================================
void
CCloneParamDialog::OnDirBtn()
{
	m_nDir = IsDlgButtonChecked( IDC_CPD_RADIO_1 );
}
//=====================================================================
void
CCloneParamDialog::DoDataExchange( CDataExchange* pDX )
{
  DDX_Text( pDX, IDC_CPD_REP_EDIT,   m_nRep );
  DDX_Radio( pDX, IDC_CPD_RADIO_0,   m_nDir  );

	if( pDX->m_bSaveAndValidate )
	  m_Edit1.SetValue();  
}
//=====================================================================
CCloneParamDialog::CExampleBox::CExampleBox()
{
	for( int k = 3 ; --k >= 0 ; m_Points[k]  = 0 );
}
//=====================================================================
CCloneParamDialog::CExampleBox::~CExampleBox() 
{ 
	for( int k = 3 ; --k >= 0 ; delete m_Points[k] );
}
//=====================================================================
void
CCloneParamDialog::CExampleBox::Init( CPoint* pp, int np )
{ 
  CRect r;
  GetClientRect( &r );
  r.InflateRect( -4, -4 );
	CPoint c0( r.CenterPoint());

	CBox b( pp, np );
	CPoint c1( b.CenterPoint());

	double kx = (double)r.Width() / (double)b.Width();
	double ky = (double)r.Height() / (double)b.Height();
	double kk = min( kx, ky ) * .8 ;
  
	m_Points[0] = new CPoint[m_nPoint[0]=np];
	memcpy( m_Points[0], pp, np * sizeof( CPoint ));
	int s = 10 / kk;

	for( int i = 3 ; --i > 0 ; )
	{
		TGstream dst;
		int nn = SmartCloneLine( dst, m_Points[0], m_nPoint[0], i > 1 ? -s : s );
		if( nn )
		{
			m_Points[i] = new CPoint[m_nPoint[i]=nn];
			memcpy( m_Points[i], dst.str(), nn * sizeof( CPoint ));
		}
	}

	for( int k = 3 ; --k >= 0 ; )
	{
	  CPoint* pt = m_Points[k];
	  for( int i = m_nPoint[k] ; --i >= 0 ; ++pt )
		{
		  *pt -= c1;
		  pt->x *= kk;
		  pt->y *= kk;
		  *pt += c0;
		}
	}
}
//=====================================================================
void
CCloneParamDialog::CExampleBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
	static COLORREF clr[] ={ RGB( 255, 0, 0 ), RGB( 0, 255, 0 ), RGB( 0, 0, 255 ) };

  CDC* pDC = CDC::FromHandle( lpDIS->hDC );

	for( int k = 3 ; --k >= 0 ; )
	{
  	CPen pen( PS_SOLID, 3, clr[k] );
  	pDC->SelectObject( &pen );
	  pDC->Polyline( m_Points[k], m_nPoint[k] );
	}
}
//=====================================================================
