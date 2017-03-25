#include "stdafx.h"
#include <afxpriv.h>
#include <afxinet.h>

#include "mview.h"
#include "mathutil.h"
#include "mdoc.h"
#include "msutil.h"
#include "ids.h"
#include "util.h"
#include "dlgids.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "subldlg.h"
#include "bldmode.h"
#include "dwin.h"
#include "sltrmode.h"
#include "mercator.h"
#include <gdiplus.h>
#include <direct.h>
#include <io.h>
//======================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=========================================================================
IMPLEMENT_DYNCREATE( CSublDlg, CDialog )
//=========================================================================
BEGIN_MESSAGE_MAP( CSublDlg, CDialog )
  ON_NOTIFY_RANGE( TTN_NEEDTEXTA, IDC_SUD_MOVE_LEFT_BUTTON,   IDC_SUD_MOVE_BOTTOM_BUTTON,  OnNeedText0 )
  ON_NOTIFY_RANGE( TTN_NEEDTEXTA, IDC_SUD_RANGE_UP_BUTTON,    IDC_SUD_RANGE_DN_BUTTON,     OnNeedText1 )
  ON_NOTIFY_RANGE( TTN_NEEDTEXTA, IDC_SUD_VERT_ALIGN_BUTTON,  IDC_SUD_FULL_ALIGN_BUTTON,   OnNeedText2 )
  ON_NOTIFY_RANGE( TTN_NEEDTEXTA, IDC_SUD_ROTATE_LEFT_BUTTON, IDC_SUD_ROTATE_RIGHT_BUTTON, OnNeedText3 )

  ON_CBN_SELENDOK( IDC_SUD_LAYER_COMBO,       OnLayerSelected          )
  ON_CBN_SELCHANGE( IDC_SUD_FILE_LIST,        OnFileSelChange          )
  ON_CBN_EDITCHANGE( IDC_SUD_FILE_LIST,       OnFileEditChange         )
  ON_BN_CLICKED( IDC_SUD_FIND_BMP_BUTTON,     OnFindButtonClicked      )
  ON_BN_CLICKED( IDC_SUD_INSERT_BMP_BUTTON,   OnInsertButtonClicked    )
  ON_BN_CLICKED( IDC_SUD_APPLY_BUTTON,        OnApplyButtonClicked     )
  ON_BN_CLICKED( IDC_SUD_AUTOSIZE_CHECK,      OnAutosizeClicked        )
  ON_BN_CLICKED( IDC_SUD_MOVE_LEFT_BUTTON,    OnLeftButtonClicked      )
  ON_BN_CLICKED( IDC_SUD_MOVE_TOP_BUTTON,     OnUpButtonClicked        )
  ON_BN_CLICKED( IDC_SUD_MOVE_RIGHT_BUTTON,   OnRightButtonClicked     )
  ON_BN_CLICKED( IDC_SUD_MOVE_BOTTOM_BUTTON,  OnDownButtonClicked      )
  ON_BN_CLICKED( IDC_SUD_RANGE_UP_BUTTON,     OnRangeUpButtonClicked   )
  ON_BN_CLICKED( IDC_SUD_RANGE_DN_BUTTON,     OnRangeDnButtonClicked   )
  ON_BN_CLICKED( IDC_SUD_VERT_ALIGN_BUTTON,   OnAlignVertButtonClicked )
  ON_BN_CLICKED( IDC_SUD_HORZ_ALIGN_BUTTON,   OnAlignHorzButtonClicked )
  ON_BN_CLICKED( IDC_SUD_FULL_ALIGN_BUTTON,   OnAlignFullButtonClicked )
  ON_BN_CLICKED( IDC_SUD_LOAD_BUTTON,         OnLoadButtonClicked      )
  ON_BN_CLICKED( IDC_SUD_START_BUTTON,        OnStartButtonClicked     )
  ON_BN_CLICKED( IDC_SUD_END_BUTTON,          OnEndButtonClicked       )
  ON_BN_CLICKED( IDC_SUD_ROTATE_LEFT_BUTTON,  OnLRotateButtonClicked   )
  ON_BN_CLICKED( IDC_SUD_ROTATE_RIGHT_BUTTON, OnRRotateButtonClicked   )

END_MESSAGE_MAP()
//=========================================================================
static CSpinIni S[] =
{
  { IDC_SUD_ORIGIN_X_EDIT,   IDC_SUD_ORIGIN_X_SPIN,   0,           0, 100000, 1000000000 },
  { IDC_SUD_ORIGIN_Y_EDIT,   IDC_SUD_ORIGIN_Y_SPIN,   0,           0, 100000, 1000000000 },
  { IDC_SUD_SIZE_X_EDIT,     IDC_SUD_SIZE_X_SPIN,     0, -1000000000, 100000, 1000000000 },
  { IDC_SUD_SIZE_Y_EDIT,     IDC_SUD_SIZE_Y_SPIN,     0, -1000000000, 100000, 1000000000 },
  { IDC_SUD_STEP_EDIT,       IDC_SUD_STEP_SPIN,       0, -10000000,   100000, 10000000   },
  { IDC_SUD_ANGLE_EDIT,      IDC_SUD_ANGLE_SPIN,      0, -3600, 10, 3600, 1, 10, "%1.1f" },
  { IDC_SUD_ANGLE_STEP_EDIT, IDC_SUD_ANGLE_STEP_SPIN, &CAngleDialog::m_AngleGrid, 1, 50, 1800, 1, 10, "%1.1f" }
};
static CSpinIni D[] =
{
  { IDC_SUD_SIZE_X_EDIT, IDC_SUD_SIZE_X_SPIN, 0, 10, 1, 10000 },
  { IDC_SUD_SIZE_Y_EDIT, IDC_SUD_SIZE_Y_SPIN, 0, 100, 100, 1000000000 }
};
//=========================================================================
CSublDlg::CSublDlg() :
  m_bZoomed( 1 ),
  m_IsActive( 0 ),
  m_nLayerNum( -1 ),
  m_bUsedListInited( 0 ),
  m_Dpi( 200 ),
  m_Scale( 500 ),
  m_Size( 100000, 100000 )
{
  m_Y_HTTP = App.GetProfileString( "General", "YandexMapUrl", "http://static-maps.yandex.ru/1.x/?" );
  m_G_HTTP = App.GetProfileString( "General", "GoogleMapUrl", "http://maps.google.com/staticmap?" );
}
//=========================================================================
static UINT bb[][2] =
{
  IDC_SUD_MOVE_LEFT_BUTTON,    IDB_L_ARROW,
  IDC_SUD_MOVE_TOP_BUTTON,     IDB_U_ARROW,
  IDC_SUD_MOVE_RIGHT_BUTTON,   IDB_R_ARROW,
  IDC_SUD_MOVE_BOTTOM_BUTTON,  IDB_D_ARROW,
  IDC_SUD_RANGE_UP_BUTTON,     IDB_RUP_ARROW,
  IDC_SUD_RANGE_DN_BUTTON,     IDB_RDN_ARROW,
  IDC_SUD_HORZ_ALIGN_BUTTON,   IDB_HORZ_SIZE_ALIGN,
  IDC_SUD_VERT_ALIGN_BUTTON,   IDB_VERT_SIZE_ALIGN,
  IDC_SUD_FULL_ALIGN_BUTTON,   IDB_FULL_SIZE_ALIGN,
  IDC_SUD_ROTATE_LEFT_BUTTON,  IDB_CR_ARROW,
  IDC_SUD_ROTATE_RIGHT_BUTTON, IDB_CW_ARROW
};
//=========================================================================
int
CSublDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  EnableToolTips( 1 );

  for( int i = CELEM( bb ) ; --i >= 0 ; )
  {
    HBITMAP hBmp = (HBITMAP)LoadImage( App.m_hInstance, LPCTSTR( bb[i][1] ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
    if( hBmp )
      ((CButton*)GetDlgItem( bb[i][0] ))->SetBitmap( hBmp );
  }

  CIntEdit* pe[] = { m_Edit, m_Edit+1, m_Edit+2, m_Edit+3, m_Edit+4, m_FEdit, m_FEdit+1 };
  for( int i = CELEM( S ) ; --i >= 0 ; pe[i]->Initialize( S[i], this ));

  ((CButton*)GetDlgItem( IDC_SUD_MOVE_ON_STEP_RADIO ))->SetCheck( 1 );
  ((CButton*)GetDlgItem( IDC_SUD_MOVE_BMP_RADIO ))->SetCheck( 1 );

  CComboBox* pLayerCombo = (CComboBox*)GetDlgItem( IDC_SUD_LAYER_COMBO );

  CObjDef* pObjDef = pView->GetDocument()->GetObjDef();
  for( int i = 0 ; i < SUBLR_RANGE ; ++i )
    pLayerCombo->AddString( pObjDef->GetName( SUBLAYER_CLASS + i + 1 ));

  pLayerCombo->SetCurSel( m_nLayerNum == -1 ? 0 : m_nLayerNum );

  char wild[_MAX_PATH];

  char* tps[] = { "BMP", "EMF", "JPG", "GIF", "PNG", "TIF", "WMF", 0 };
  m_FullArray.RemoveAll();

  for( int i = 2 ; --i >= 0 ; )
  {
    CString tmp( App.GetSubbmpPath( i ));
    m_BmpPath[i] = tmp;
    _mkdir( tmp );
    CString path( m_BmpPath[i] + "*.*" );
    for( char** tp = tps ; *tp ; ++ tp )
    {
      CFileFind ff;
      MakePath( path, wild, *tp, "*" );
      for( int h = ff.FindFile( wild ) ; h ; )
      {
        h = ff.FindNextFile();
        CString fn( ff.GetFileName());
        m_FullArray.Add( fn );
      }
    }
  }

  CComboBox* pScaleCombo = (CComboBox*)GetDlgItem( IDC_SUD_SCALE_COMBO );
  for( int i = 18 ; --i >= 7 ; )
  {
    CString s;
    s.Format( "%d", i );
    pScaleCombo->AddString( s );
  }
  pScaleCombo->SetCurSel( 0 );

  if( !m_bUsedListInited )
  {
    CWaitCursor wc;
    ReportHint( "Обзор подложки..." );

    CSize sz( pView->GetDocument()->GetSize());
    RECT r = { 0, 0, sz.cx, sz.cy };
    long t[SUBLR_RANGE];
    for( int i = SUBLR_RANGE ; --i >= 0 ; t[i] = SUBLAYER_CLASS + i + 1 );
    TGstream buf;
    int cnt = 0;
    if( pView->m_MapStore.SelectByClass( r, t, CELEM( t ), buf, &cnt ) == R_OK )
      for( char* pSrc = buf.str() ; --cnt >= 0 ; )
      {
        CDrawObject* pObj = pView->TranslateObjectFromMap( &pSrc );
        if( pObj )
          m_UsedList.AddHead( pObj );
      }

    m_bUsedListInited = 1;
    ClearHint();
  }

  pView->m_Undo.RemoveAll();
  pView->OnUnselect();
  m_nLayerNum = -1;
  OnLayerSelected();

  ((CComboBox*)GetDlgItem( IDC_SUD_SOURCE_COMBO ))->SetCurSel( 0 );
  CheckDlgButton( IDC_SUD_MAP_RADIO, 1 );

  Zoom( 0, 1 );

  return m_IsActive = 1;
}
//======================================================================
void
CSublDlg::Zoom( int bZoom, int bUseListBox )
{
  static UINT t0[] = { IDC_SUD_SOURCE_COMBO, IDC_SUD_SCALE_COMBO,
                       IDC_SUD_MAP_RADIO, IDC_SUD_SAT_RADIO, IDC_SUD_HYB_RADIO,
                       IDC_SUD_ONE_REPER_CHECK, IDC_SUD_STATIC0, IDC_SUD_STATIC1,
                       IDC_SUD_STATIC3 };
  if( bZoom != m_bZoomed )
  {
    WINDOWPLACEMENT wp;
    GetDlgItem( bUseListBox ? IDC_SUD_FILE_LIST : IDC_SUD_SOURCE_COMBO )->GetWindowPlacement( &wp );
    int w = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
    GetWindowPlacement( &wp );
    wp.rcNormalPosition.right = wp.rcNormalPosition.right + ( bZoom ? w : -w );
    SetWindowPlacement( &wp );
    m_bZoomed = bZoom;
  }
  GetDlgItem( IDC_SUD_FILE_LIST )->ShowWindow( bZoom && bUseListBox );
  for( int i = CELEM( t0 ) ; --i >= 0 ; GetDlgItem( t0[i] )->ShowWindow( bZoom && !bUseListBox ));
}
//=========================================================================
void
CSublDlg::ClearUsedList()
{
  while( !m_UsedList.IsEmpty())
    delete m_UsedList.RemoveHead();
}
//=========================================================================
static CString
CalcFString( CDrawObject* pObj )
{
  CString t( "" );
  if( pObj )
  {
    CText* pSourceText = (CText*)pObj->GetActualCont()->GetHead()->GetNext();
    CText* pNumberText = (CText*)pSourceText->GetNext();
    if( pNumberText )
    {
      t = *pNumberText;
      t += " [";
      t += *pSourceText;
      t += "]";
    }
    else
      t = *pSourceText;
  }
  return t;
}
//=========================================================================
void
CSublDlg::CreateFileList( int n )
{
  int bUsed   = IsDlgButtonChecked( IDC_SUD_FIND_BMP_BUTTON );
  int bUnused = IsDlgButtonChecked( IDC_SUD_INSERT_BMP_BUTTON );
  if( !bUsed && !bUnused )
    return;

  CComboBox* pFileList = (CComboBox*)GetDlgItem( IDC_SUD_FILE_LIST );
  pFileList->ResetContent();

  int nNode = pView->m_Conductor.GetNode();

  if( bUsed )
  {
    CDrawObject* pSel = pView->GetSelectObject( 0 );
    CString sn;
    for( POSITION pos = m_UsedList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = m_UsedList.GetNext( pos );
      if( pObj->Class() - SUBLAYER_CLASS - 1 != m_nLayerNum )
        continue;
      if( nNode && pObj->Node() && pObj->Node() != nNode )
        continue;
      CString en( CalcFString( pObj ));
      if( pSel && pSel->Ident() == pObj->Ident())
        sn = en;
      pFileList->AddString( en );
    }
    if( !sn.IsEmpty())
      n = pFileList->FindString( 0, sn );
  }
  else
    for( int i = m_FullArray.GetSize() ; --i >= 0 ; )
    {
      int bf = 0;
      for( POSITION pos = m_UsedList.GetHeadPosition() ; !bf && pos ; )
      {
        CDrawObject* pObj = m_UsedList.GetNext( pos );
        CText* pSourceText = (CText*)pObj->GetActualCont()->GetHead()->GetNext();
        bf = m_FullArray[i] == *pSourceText;
      }
      if( !bf )
        pFileList->AddString( m_FullArray[i] );
    }
  pFileList->SetCurSel( n );
}
//=========================================================================
void
CSublDlg::OnInsertButtonClicked()
{
  if( IsDlgButtonChecked( IDC_SUD_LOAD_BUTTON ))
    Zoom( 0, 0 );
  CheckDlgButton( IDC_SUD_FIND_BMP_BUTTON, 0 );
  CheckDlgButton( IDC_SUD_LOAD_BUTTON, 0 );
  Zoom( IsDlgButtonChecked( IDC_SUD_INSERT_BMP_BUTTON ), 1 );
  CreateFileList();
}
//=========================================================================
void
CSublDlg::OnFindButtonClicked()
{
  if( IsDlgButtonChecked( IDC_SUD_LOAD_BUTTON ))
    Zoom( 0, 0 );
  CheckDlgButton( IDC_SUD_INSERT_BMP_BUTTON, 0 );
  CheckDlgButton( IDC_SUD_LOAD_BUTTON, 0 );
  Zoom( IsDlgButtonChecked( IDC_SUD_FIND_BMP_BUTTON ), 1 );
  CreateFileList();
}
//=========================================================================
void
CSublDlg::OnLoadButtonClicked()
{
  if( IsDlgButtonChecked( IDC_SUD_INSERT_BMP_BUTTON ) ||
      IsDlgButtonChecked( IDC_SUD_FIND_BMP_BUTTON ))
    Zoom( 0, 1 );
  CheckDlgButton( IDC_SUD_INSERT_BMP_BUTTON, 0 );
  CheckDlgButton( IDC_SUD_FIND_BMP_BUTTON, 0 );
  Zoom( IsDlgButtonChecked( IDC_SUD_LOAD_BUTTON ), 0 );
}
//======================================================================
void
CSublDlg::AddItem( CDrawObject* pObj )
{
  m_UsedList.AddHead( new CDrawObject( pObj ));
  CreateFileList();
}
//=========================================================================
void
CSublDlg::RemoveItem( CDrawObject* pObj )
{
  CComboBox* pFileList = (CComboBox*)GetDlgItem( IDC_SUD_FILE_LIST );
  int i = pFileList->FindString( 0, CalcFString( pObj ));
  for( POSITION pos = m_UsedList.GetHeadPosition() ; pos ; m_UsedList.GetNext( pos ))
    if( m_UsedList.GetAt( pos )->Ident() == pObj->Ident())
    {
      delete m_UsedList.GetAt( pos );
      m_UsedList.RemoveAt( pos );
      CreateFileList( i );
      break;
    }
}
//=========================================================================
void
CSublDlg::SelectItem( CDrawObject* pObj )
{
  CComboBox* pFileList = (CComboBox*)GetDlgItem( IDC_SUD_FILE_LIST );
  int i = pFileList->FindString( 0, CalcFString( pObj ));
  pFileList->SetCurSel( i );
}
//=========================================================================
void
CSublDlg::ResumeLayerVsbl()
{
  int t = SUBLAYER_CLASS + m_nLayerNum + 1;
  CLayer* pSL = pView->GetLayer( t );
  pSL->SetVisible( pView->m_ShowMap[t>>3] & ( 1 << ( t & 7 )));
}
//=========================================================================
void
CSublDlg::OnLayerSelected()
{
  CComboBox* pLayerCombo = (CComboBox*)GetDlgItem( IDC_SUD_LAYER_COMBO );
  int n = pLayerCombo->GetCurSel();
  if( m_nLayerNum == n )
    return;

  ResumeLayerVsbl();
  m_nLayerNum = n;
  int nClass = SUBLAYER_CLASS + m_nLayerNum + 1;
  pView->GetLayer( nClass )->SetVisible( 1 );
  pView->ClearSelection( 0, 0 );

  pView->m_bSublayerChanged = 1;
  pView->Invalidate( 0 );
  CreateFileList();
}
//=========================================================================
int
CSublDlg::CalcSize( int bAskDpi, int bUseSelected )
{
  using namespace Gdiplus;

  CSize t( m_Size );

  if( bAskDpi )
  {
    WCHAR wbuf[_MAX_PATH];
    CString bmpFile;
    if( bUseSelected )
    {
      CDrawObject* pObj = pView->GetSelectObject( 1 );
      if( !pObj )
        return 0;
      CRectangle* pRectPrim = (CRectangle*)pObj->GetActualCont()->GetHead();
      CText* pTextPrim = (CText*)pRectPrim->GetNext();
      bmpFile = *pTextPrim;
    }
    else
    {
      CComboBox* pFileList = (CComboBox*)GetDlgItem( IDC_SUD_FILE_LIST );
      pFileList->GetLBText( pFileList->GetCurSel(), bmpFile );
    }

    CString path;

    int br = -1;
    for( int i = 2 ; br && --i >= 0 ; )
    {
      path = m_BmpPath[i] + bmpFile;
      br = _access( path, 04 );
    }

    if( br )
    {
      CString m( Compose( "Не найден файл подложки ", bmpFile, 0 ));
      ErrBox( m );
      return 0;
    }

    m_Dpi = 200;
    if( MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, path, -1, wbuf, _MAX_PATH ))
    {
      GdiplusStartupInput gdiplusStartupInput;
      ULONG_PTR GdiplusToken;
      GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );
      Bitmap* pBmp = Bitmap::FromFile( wbuf );
      if( pBmp )
      {
        int hr = pBmp->GetHorizontalResolution();
        int vr = pBmp->GetVerticalResolution();
        m_Dpi  = min( hr, vr );

        if( IsDlgButtonChecked( IDC_SUD_AUTOSIZE_CHECK ))
        {
          m_Edit[2].SetValue( m_Dpi );
          m_Scale = m_Edit[3].GetValue();
          if( m_Dpi <= 0 )
          {
            ErrBox( "Некорректное значение DPI" );
            return 0;
          }
          if( m_Scale <= 0 )
          {
            ErrBox( "Некорректное значение масштаба" );
            return 0;
          }
          int m1 = MulDiv( m_Scale, 254, m_Dpi );
          int m2 = SmInUnit * 100;

          t.cx = MulDiv( pBmp->GetWidth(),  m1, m2 );
          t.cy = MulDiv( pBmp->GetHeight(), m1, m2 );
          if( t.cx > 0 && t.cy > 0 )
            m_Size = t;
        }
      }
      GdiplusShutdown( GdiplusToken );
    }
  }

  return t.cx > 0 && t.cy > 0;
}
//=========================================================================
CDrawObject*
CSublDlg::GetCurObj()
{
  CString bmpFile;
  CComboBox* pFileList = (CComboBox*)GetDlgItem( IDC_SUD_FILE_LIST );
  int n = pFileList->GetCurSel();
  if( n < 0 )
    return 0;

  pFileList->GetLBText( n, bmpFile );
  int lp = bmpFile.Find( "[" );
  int rp = bmpFile.Find( "]" );
  if( lp >= 0 && rp > lp + 1 )
    bmpFile = bmpFile.Mid( lp + 1, rp - lp - 1 );

  for( POSITION pos = m_UsedList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = m_UsedList.GetNext( pos );
    CText* pFileText = (CText*)pObj->GetActualCont()->GetHead()->GetNext();
    if( bmpFile == *pFileText )
      return pView->GetObjectPtr( pObj->Ident());
  }
  return 0;
}
//=========================================================================
void
CSublDlg::OnFileSelChange()
{
  if( IsDlgButtonChecked( IDC_SUD_INSERT_BMP_BUTTON ))
    CalcSize( 1 );

  CDrawObject* pObj = GetCurObj();
  RefreshPosSize( pObj );
}
//=========================================================================
void
CSublDlg::OnFileEditChange()
{
  CComboBox* pFileList = (CComboBox*)GetDlgItem( IDC_SUD_FILE_LIST );
  CString t;
  pFileList->GetWindowText( t );

  int i = pFileList->FindString( 0, t );
  if( i >= 0 )
    pFileList->SetCurSel( i );
  pFileList->SetEditSel( t.GetLength(), -1 );
}
//=========================================================================
void
CSublDlg::OnAutosizeClicked()
{
  if( IsDlgButtonChecked( IDC_SUD_AUTOSIZE_CHECK ))
  {
    SetDlgItemText( IDC_SUD_W_STATIC, "DPI" );
    SetDlgItemText( IDC_SUD_H_STATIC, "M 1:" );
    m_Size.cx = m_Edit[2].GetValue();
    m_Size.cy = m_Edit[3].GetValue();
    m_Edit[2].Initialize( D[0], this, 0 );
    m_Edit[3].Initialize( D[1], this, 0 );
    m_Edit[2].SetValue( m_Dpi   );
    m_Edit[3].SetValue( m_Scale );
  }
  else
  {
    SetDlgItemText( IDC_SUD_W_STATIC, "W" );
    SetDlgItemText( IDC_SUD_H_STATIC, "H" );
    m_Dpi   = m_Edit[2].GetValue();
    m_Scale = m_Edit[3].GetValue();
    m_Edit[2].Initialize( S[2], this, 0 );
    m_Edit[3].Initialize( S[3], this, 0 );
    m_Edit[2].SetValue( m_Size.cx );
    m_Edit[3].SetValue( m_Size.cy );
  }
}
//=========================================================================
int
CSublDlg::Activate()
{
  UINT i;
  CPoint p;
  if( pView->m_pSTM && !( pView->m_pSTM->OnRButtonDown( i, p ) & AM_RREMOVE ))
    return 0;
  Create( IDD_SUBLAYER_DEF_DIALOG, pView );
  return m_IsActive;
}
//=========================================================================
void
CSublDlg::OnOK()
{
  OnStartButtonClicked();
}
//=========================================================================
void
CSublDlg::OnCancel()
{
//  OnEndButtonClicked();
}
//=========================================================================
void
CSublDlg::OnEndButtonClicked()
{
  if( !pView->GetBuilding( 1 ))
  {
    m_IsActive = 0;
    m_bZoomed = 1;
    DestroyWindow();
    ResumeLayerVsbl();
    pView->m_bSublayerChanged = 1;
    pView->OnUnselect();
    pView->m_Undo.RemoveAll();
    for( POSITION pos = m_UsedList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = m_UsedList.GetNext( pos );
      if( !pView->GetLayer( pObj->Class())->LayerOn())
        pView->ShowObjectBox( pObj );
    }
  }
}
//=========================================================================
void
CSublDlg::RefreshPosSize( CDrawObject* pObj )
{
  CRectangle z( CRectPrim( 0 , 0, 0, 0, 0 ));
  CRectangle* pRectPrim = pObj ? (CRectangle*)pObj->GetActualCont()->GetHead() : &z;
  CRect r( *pRectPrim );
  double ang = pRectPrim->GetAngle();

  m_Edit[0].SetValue( r.left );
  m_Edit[1].SetValue( r.top  );
  if( IsDlgButtonChecked( IDC_SUD_AUTOSIZE_CHECK ))
    m_Size = r.Size();
  else
  {
    m_Edit[2].SetValue( r.Width());
    m_Edit[3].SetValue( r.Height());
  }
  m_FEdit[0].SetValue( ang );
}
//======================================================================
void
CSublDlg::UpdateBmp( int side )
{
  if( IsDlgButtonChecked( IDC_SUD_ALIGN_RADIO ))
    SetBmp( side );
  else
    MoveBmp( side );
}

//======================================================================
void
CSublDlg::AlignSize( int dir )
{
  CObjList& lst = pView->m_SelectList;
  if( lst.GetCount() < 2 )
  {
    InfBox( "Для выравнивания размеров необходимо выделить\n"
            "не менее 2-х фрагментов подложки" );
    return;
  }

  POSITION pos = lst.GetTailPosition();
  CDrawObject* pObj = lst.GetPrev( pos );
  CRectangle* pRectPrim = (CRectangle*)pObj->GetActualCont()->GetHead();
  CSize size( pRectPrim->GetSize());

  CObjList newList, oldList;

  while( pos )
  {
    CDrawObject* pOld = lst.GetPrev( pos );
    CDrawObject* pNew = new CDrawObject( pOld );
    CRectangle* pRectNew = (CRectangle*)pNew->GetActualCont()->GetHead();
    CSize newSize( pRectNew->GetSize());
    if( dir & 1 )
      newSize.cx = size.cx;
    if( dir & 2 )
      newSize.cy = size.cy;
    pRectNew->SetSize( newSize );
    pNew->CalcBoundBox();
    oldList.AddHead( pOld );
    newList.AddHead( pNew );
  }

  pView->m_Undo.AddItem( new CUndoAddItem( pView, &newList, &oldList ), pView );

  for( pos = newList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = newList.GetNext( pos );
    pView->m_Conductor.Create( pObj );
    pView->m_Conductor.DeclareEvent( CE_OBJ_EDIT );
  }
  pView->SaveToStore( newList, 0, 1 );

  while( !newList.IsEmpty())
  {
    CDrawObject* pObj = newList.RemoveHead();
    AddItem( pObj );
    pView->SelectObject( pObj, 1 );
  }

  pView->m_bSublayerChanged = 1;
}
//======================================================================
void
CSublDlg::SetBmp( int side )
{
  CObjList& lst = pView->m_SelectList;
  if( lst.GetCount() < 2 )
  {
    InfBox( "Для выравнивания необходимо выделить\n"
            "не менее 2-х фрагментов подложки" );
    return;
  }

  int bMoveBnd = IsDlgButtonChecked( IDC_SUD_MOVE_BND_RADIO );
  POSITION pos = lst.GetTailPosition();
  CDrawObject* pObj = lst.GetPrev( pos );
  CRectangle* pRectPrim = (CRectangle*)pObj->GetActualCont()->GetHead();
  CPoint org( pRectPrim->GetCoord());
  CSize size( pRectPrim->GetSize());
  int ang = pRectPrim->GetAngle();

  CObjList newList, oldList;

  int bValid = 1;

  while( pos && bValid )
  {
    CDrawObject* pOld = lst.GetPrev( pos );
    CDrawObject* pNew = new CDrawObject( pOld );
    CRectangle* pRectNew = (CRectangle*)pNew->GetActualCont()->GetHead();
    CSize newSize( pRectNew->GetSize());
    CPoint p0( TurnPoint( pRectNew->GetCoord(), org, -ang )), p1( p0 );
    switch( side )
    {
      case 0:
      {
        if( bMoveBnd )
          newSize.cx += p0.x - org.x;
        p1.x = org.x;
        break;
      }
      case 1:
      {
        if( bMoveBnd )
          newSize.cy += p0.y - org.y;
        p1.y = org.y;
        break;
      }
      case 2:
      {
        if( bMoveBnd )
          newSize.cx = org.x + size.cx - p0.x;
        p1.x = org.x + size.cx - newSize.cx;
        break;
      }
      case 3:
      {
        if( bMoveBnd )
          newSize.cy = org.y + size.cy - p0.y;
        p1.y = org.y + size.cy - newSize.cy;
        break;
      }
    }

    pRectNew->SetCoord( TurnPoint( p1, org, ang ));
    pRectNew->SetSize( newSize );
    pRectNew->SetAngle( ang );

    CDrawObject* pTmp = pNew;
    pNew = pTmp->Rebuild();
    delete pTmp;

    oldList.AddHead( pOld );
    newList.AddHead( pNew );
    bValid = newSize.cx > 0 && newSize.cy > 0;
  }

  if( !bValid )
  {
    ErrBox( "Неположительный размер\n"
            "одного из фрагментов подложки!" );
    while( !newList.IsEmpty())
      delete newList.RemoveHead();
    return;
  }

  pView->m_Undo.AddItem( new CUndoAddItem( pView, &newList, &oldList ), pView );

  for( pos = newList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = newList.GetNext( pos );
    pView->m_Conductor.Create( pObj );
    pView->m_Conductor.DeclareEvent( CE_OBJ_EDIT );
  }
  pView->SaveToStore( newList, 0, 1 );

  while( !newList.IsEmpty())
  {
    CDrawObject* pObj = newList.RemoveHead();
    AddItem( pObj );
    pView->SelectObject( pObj, 1 );
  }

  pView->m_bSublayerChanged = 1;
}
//======================================================================
int
CSublDlg::TestMove( CSize ds )
{
  CSize docSize( pView->GetDocument()->GetSize());
  int n = 0;
  for( POSITION pos = pView->m_SelectList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = pView->m_SelectList.GetNext( pos );
    CRectangle* pRectPrim = (CRectangle*)pObj->GetActualCont()->GetHead();
    CPoint* pts = Rect2Poly( pRectPrim->GetCoord(), pRectPrim->GetSize(), pRectPrim->GetAngle());
    for( CPoint* pp = pts + 4 ; --pp >= pts ; )
    {
      CPoint t( *pp + ds );
      n += t.x < 0 || t.x >= docSize.cx || t.y < 0 || t.y >= docSize.cy;
    }
  }
  return n < 4;
}
//======================================================================
CDrawObject*
CSublDlg::GetActObj()
{
  CDrawObject* pObjCur = GetCurObj();
  CDrawObject* pObjSel = pView->GetSelectObject( !pObjCur );
  return pObjSel ? pObjSel : pObjCur;
}
//======================================================================
void
CSublDlg::SpinBmp( int bLeft )
{
  CDrawObject* pObj = GetActObj();
  if( pObj )
  {
    int a = m_FEdit[1].GetValue();
    CDrawObject* pOld = new CDrawObject( pObj );
    pView->ShowObjectBox( pOld );
    pObj->SpinOn( pObj->GetBoundBox().CenterPoint(), bLeft ? a : -a );
    pView->m_Undo.AddItem( new CUndoAddItem( pView, pObj, pOld ), pView );
    pView->m_Conductor.Create( pObj );
    pView->m_Conductor.DeclareEvent( CE_OBJ_EDIT );
    pView->AddObject( pObj, 0 );
    delete pOld;
    SelectItem( pObj );
  }
}
//======================================================================
void
CSublDlg::MoveBmp( int side )
{
  CDrawObject* pObj = GetActObj();

  int bMoveBnd = IsDlgButtonChecked( IDC_SUD_MOVE_BND_RADIO );
  int bToLimit = IsDlgButtonChecked( IDC_SUD_MOVE_TO_LIMIT_RADIO );

  int bVrt = side & 1;
  int bPos = side & 2;
  int ma = pView->GetModeAngle();
  int step = GetDlgItemInt( IDC_SUD_STEP_EDIT );

  if( !bMoveBnd && !bToLimit )
  {
    int sst = bPos ? step : -step;
    CPoint dd( bVrt ? 0 : sst, bVrt ? sst : 0 );
    CSize ds( TurnPoint( dd, ma ));
    int r = 1;
    if( !pView->m_SelectList.IsEmpty())
    {
      if( r = TestMove( ds ))
      {
        pView->ShowSelected( 0 );
        pView->m_pUndoItem = new CUndoMoveItem( pView, ds );
        pView->MoveSelectOn( ds );
        pView->ShowSelected( 1, 1, 1 );
        pView->m_Undo.AddItem( pView->m_pUndoItem, pView );
      }
    }
    else
    if( pObj )
    {
      pView->m_SelectList.AddHead( pObj );
      int r = TestMove( ds );
      pView->m_SelectList.RemoveHead();
      if( r )
      {
        CDrawObject* pOld = pObj->CopyObject();
        pObj->MoveOn( ds );
        pView->m_Conductor.Create( pObj );
        pView->m_Conductor.DeclareEvent( CE_OBJ_EDIT );
        pView->AddObject( pObj, 0 );
        pView->ShowObjectBox( pOld );
        pView->ShowObjectBox( pObj );
        pView->m_Undo.AddItem( new CUndoAddItem( pView, pObj, pOld ), pView );
        delete pOld;
        SelectItem( pObj );
      }
    }
    if( !r )
      ErrBox( "Подложка полностью выходит за габарит проекта" );

    return;
  }

  if( !pObj )
    return;

  CDrawObject* pOld = pObj->CopyObject();
  CRectangle* pRectPrim = (CRectangle*)pObj->GetActualCont()->GetHead();
  CPoint org( pRectPrim->GetCoord());
  CSize size( pRectPrim->GetSize());
  int angle = pRectPrim->GetAngle();

  if( bToLimit )
  {
    int nClass = SUBLAYER_CLASS + m_nLayerNum + 1;
    int wa = bMoveBnd ? angle : ma;
    CPoint tOrg = TurnPoint( org, -wa );
    CBox tr( CRect( tOrg, size ), tOrg, angle - wa );
    long* pSide = &tr.left + side;
    int mgn;
    int bMgnSet = 0;
    step = 0;

    for( POSITION pos = m_UsedList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pTst = pView->GetObjectPtr( m_UsedList.GetNext( pos )->Ident());
      if( !pTst || pTst == pObj || pTst->Class() != nClass )
        continue;

      CRectangle* pRectPrim = (CRectangle*)pTst->GetActualCont()->GetHead();
      CPoint cOrg = TurnPoint( pRectPrim->GetCoord(), -wa );
      CBox cr( CRect( cOrg, pRectPrim->GetSize()), cOrg, pRectPrim->GetAngle() - wa );
      if( bVrt && cr.left < tr.right && cr.right > tr.left )
      {
        if( bPos && cr.top > tr.bottom && ( !bMgnSet || cr.top < mgn ))
          bMgnSet = mgn = cr.top;
        if( !bPos && cr.bottom < tr.top && ( !bMgnSet || cr.bottom > mgn ))
          bMgnSet = mgn = cr.bottom;
      }
      if( !bVrt && cr.top < tr.bottom && cr.bottom > tr.top )
      {
        if( bPos && cr.left > tr.right && ( !bMgnSet || cr.left < mgn ))
          bMgnSet = mgn = cr.left;
        if( !bPos && cr.right < tr.left && ( !bMgnSet || cr.right > mgn ))
          bMgnSet = mgn = cr.right;
      }
    }
    if( bMgnSet )
      step = abs( mgn - *pSide );
  }

  if( !step )
    return;

  int sst = bPos ? step : -step;
  CSize dsz( bVrt ? 0 : sst, bVrt ? sst : 0 );

  if( !bMoveBnd )
    pRectPrim->SetCoord( org + TurnPoint( dsz, ma ));
  else
  if( side < 2 )
  {
    pRectPrim->SetCoord( org + TurnPoint( dsz, angle ));
    pRectPrim->SetSize( size - dsz );
  }
  else
    pRectPrim->SetSize( size + dsz );

  CDrawObject* pTmp = pObj;
  pObj = pTmp->Rebuild();
  if( pTmp->IsSelected())
  {
    pView->SelectObject( pObj, pTmp->SelectFlag());
    pView->SelectObject( pTmp, 0 );
  }
  pView->m_Conductor.Create( pObj );
  pView->m_Conductor.DeclareEvent( CE_OBJ_EDIT );
  pView->AddObject( pObj, 0 );
  pView->ShowObjectBox( pOld );
  pView->ShowObjectBox( pObj );
  pView->m_Undo.AddItem( new CUndoAddItem( pView, pObj, pOld ), pView );
  delete pOld;
  delete pTmp;
  SelectItem( pObj );

  RefreshPosSize( pObj );
}
//======================================================================
void
CSublDlg::OnApplyButtonClicked()
{
  CDrawObject* pObj;

  ( pObj = IsDlgButtonChecked( IDC_SUD_FIND_BMP_BUTTON ) ? GetCurObj() : 0 ) ||
  ( pObj = pView->GetSelectObject( 1 ));

  if( !pObj )
    return;

  CPoint org( m_Edit[0].GetValue(), m_Edit[1].GetValue());

  int bAutoSize = IsDlgButtonChecked( IDC_SUD_AUTOSIZE_CHECK );
  if( bAutoSize )
  {
    int newDpi = m_Edit[2].GetValue();
    if( newDpi <= 0 )
    {
      ErrBox( "Некорректное значение \"Dpi\"" );
      return;
    }

    if( !CalcSize( 1, 1 ))
      return;

    int oldDpi = m_Edit[2].GetValue();
    m_Edit[2].SetValue( newDpi );

    if( oldDpi != newDpi )
    {
      m_Size.cx = MulDiv( m_Size.cx, oldDpi, newDpi );
      m_Size.cy = MulDiv( m_Size.cy, oldDpi, newDpi );
    }
  }

  CSize size( bAutoSize ? m_Size.cx : m_Edit[2].GetValue(),
              bAutoSize ? m_Size.cy : m_Edit[3].GetValue());
  int ang = m_FEdit[0].GetValue();

  if( size.cx <= 0 || size.cy <= 0 )
  {
    ErrBox( "Недопустимый размер подложки" );
    return;
  }

  CSize docSize( pView->GetDocument()->GetSize());

  if( org.x < 0 || org.x >= docSize.cx ||
      org.y < 0 || org.y >= docSize.cy )
  {
    ErrBox( "Недопустимые координаты подложки" );
    return;
  }

  if( org.x + size.cx >= docSize.cx ||
      org.y + size.cy >= docSize.cy )
  {
    ErrBox( "Подложка выходит за габарит проекта" );
    return;
  }

  CRectangle* pRectPrim = (CRectangle*)pObj->GetActualCont()->GetHead();

  if( org  != pRectPrim->GetCoord() ||
      size != pRectPrim->GetSize()  ||
      ang  != pRectPrim->GetAngle())
  {
    CDrawObject* pOld = pObj->CopyObject();
    pRectPrim->SetCoord( org );
    pRectPrim->SetSize( size );
    pRectPrim->SetAngle( ang );
    pObj->CalcBoundBox();
    pView->m_Conductor.Create( pObj );
    pView->m_Conductor.DeclareEvent( CE_OBJ_EDIT );
    pView->AddObject( pObj, 0 );
    pView->ShowObjectBox( pOld );
    pView->ShowObjectBox( pObj );
    pView->m_Undo.AddItem( new CUndoAddItem( pView, pObj, pOld ), pView );
    delete pOld;
    SelectItem( pObj );
  }
}
//======================================================================
void
CSublDlg::MoveRange( int dr )
{
  int nr = m_nLayerNum + dr;
  if( nr < 0 || nr >= SUBLR_RANGE )
    return;

  if( !pView->m_SelectList.IsEmpty())
  {
    CObjList newList, oldList;
    while( !pView->m_SelectList.IsEmpty())
    {
      CDrawObject* pOld = pView->m_SelectList.RemoveHead();
      CDrawObject* pObj = new CDrawObject( pOld );
      CIdent id( pObj->Class() + dr, 0, pObj->Table(), pObj->Entry());
      pObj->SetIdent( id );
      oldList.AddHead( pOld );
      newList.AddHead( pObj );
    }
    pView->m_Undo.AddItem( new CUndoAddItem( pView, &newList, &oldList ), pView );

    for( POSITION pos = newList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = newList.GetNext( pos );
      pView->m_Conductor.Create( pObj );
      pView->m_Conductor.DeclareEvent( CE_OBJ_EDIT );
    }
    pView->SaveToStore( newList, 0, 1 );

    while( !newList.IsEmpty())
    {
      CDrawObject* pObj = newList.RemoveHead();
      AddItem( pObj );
      pView->SelectObject( pObj, 1 );
    }
  }

  CComboBox* pLayerCombo = (CComboBox*)GetDlgItem( IDC_SUD_LAYER_COMBO );
  pLayerCombo->SetCurSel( pLayerCombo->GetCurSel() + dr );
  OnLayerSelected();
}
//======================================================================
void
CSublDlg::OnNeedText0( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult )
{
  static char* s[] = { "влево", "вверх", "вправо", "вниз" };
  int bMoveSub = IsDlgButtonChecked( IDC_SUD_MOVE_BMP_RADIO );
  int bOnStep  = IsDlgButtonChecked( IDC_SUD_MOVE_ON_STEP_RADIO );
  int bToLevel = IsDlgButtonChecked( IDC_SUD_ALIGN_RADIO );
  sprintf_s(((LPTOOLTIPTEXT)pNotifyStruct)->szText,
             "%s %s %s %s",
             bToLevel ? "Выравнять" : "Подвинуть",
             bMoveSub ? "подложку"  : "границу",
             bToLevel ? "" : bOnStep ? "на шаг" : "до упора",
             s[nID-IDC_SUD_MOVE_LEFT_BUTTON] );
}
//=====================================================================
void
CSublDlg::OnNeedText1( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult )
{
  sprintf_s(((LPTOOLTIPTEXT)pNotifyStruct)->szText,
             "Переместить выделенные в %s слой",
             nID == IDC_SUD_RANGE_UP_BUTTON ? "предыдующий" : "следующий" );
}
//=====================================================================
void
CSublDlg::OnNeedText2( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult )
{
  sprintf_s(((LPTOOLTIPTEXT)pNotifyStruct)->szText,
             "Выравнять размер по %s %s",
             nID == IDC_SUD_VERT_ALIGN_BUTTON ? "вертикали" : "горизонтали",
             nID  > IDC_SUD_HORZ_ALIGN_BUTTON ? "и вертикали" : "" );
}
//=====================================================================
void
CSublDlg::OnNeedText3( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult )
{
  sprintf_s(((LPTOOLTIPTEXT)pNotifyStruct)->szText,
             "Повернуть %s",
             nID == IDC_SUD_ROTATE_LEFT_BUTTON ? "против часовой стрелки" : "по часовой стрелке" );
}
//=====================================================================
int
CSublDlg::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
  NMHDR* pNMHDR = (NMHDR*)lParam;
  int nCode = pNMHDR->code;
  if( nCode != TTN_NEEDTEXTA )
    return CDialog::OnNotify( wParam, lParam, pResult );

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

  int bFnd = 0, i = CELEM( bb );
  while( !bFnd && --i >= 0 )
  {
    CButton* pBtn = (CButton*)GetDlgItem( bb[i][0] );
    bFnd = pBtn && (UINT)pBtn->m_hWnd == wParam;
  }

  return bFnd ? OnCmdMsg( bb[i][0], MAKELONG( nCode, WM_NOTIFY ), &notify, 0 ) : 0;
}
//=====================================================================
using namespace Gdiplus;
inline Gdiplus::Point
MkPoint( CPoint p )
{
  return Gdiplus::Point( p.x, p.y );
}
//=========================================================================
static char Y_KEY[] = "ANpUFEkBAAAAf7jmJwMAHGZHrcKNDsbEqEVjEUtCmufxQMwAAAAAAAAAAAAvVrubVT4btztbduoIgTLAeFILaQ==";
static char G_KEY[] = "MAPS_API_KEY";
//=========================================================================
int
CSublDlg::GetScaleRatio( CString path, Polar c, double ls, int Z, double* pkx, double* pky )
{
  CInternetSession IS;
  CString url, ld;
  int k, r = 0;
  int sx = 0, fx = 0;
  int sy = 0, fy = 0;
  ls /= ( 1 << Z );
  typedef UINT (*MT)[450];
  MT pp;
  Gdiplus::Rect rect( 0, 0, 450, 450 );
  Bitmap* pbmp = 0;
  CStdioFile* IF = 0;
  ofstream OF;
  if( !OF.good())
          goto Fail;

  ld.Format( "c:ff0000ff,f:00ffffff,w:1,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f",
              c.lon - ls, c.lat - ls, c.lon - ls, c.lat + ls,
              c.lon + ls, c.lat + ls, c.lon + ls, c.lat - ls,
              c.lon - ls, c.lat - ls );

  url.Format( "%sll=%1.6f,%1.6f&size=450,450&z=%d&l=map&pl=%s&key=%s",
                          m_Y_HTTP, c.lon, c.lat, Z, ld, Y_KEY );

  IF = IS.OpenURL( url, 1, INTERNET_FLAG_TRANSFER_BINARY );
  if( !IF )
          goto Fail;

  char buf[4096];
  OF.open( path, ios::out|ios::trunc|ios::binary );
  for(;;)
  {
    int n = IF->Read( buf, sizeof buf );
    if( !n )
      break;
    OF.write( buf, n );
  }
  OF.close();
  IF->Close();
  delete IF;

  WCHAR wbuf[_MAX_PATH];
  MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, path, -1, wbuf, _MAX_PATH );

  pbmp = Bitmap::FromFile( wbuf );
  if( !pbmp )
    goto Fail;

  BitmapData bmpData;

  if( pbmp->LockBits( &rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData ) != Ok )
    goto Fail;

  pp = (MT)bmpData.Scan0;

  for( k = 0 ; k < 4 ; ++k )
  {
    static int mx[] = { -1, 1, 0, 0 };
    static int my[] = { 0, 0, -1, 1 };

    int i = 224, j = 224;
    int n, s = 0;
    UINT fv = pp[i][j];
    for( n = 0 ; n < 225 && !s ; ++n, i += mx[k], j += my[k] )
      s = pp[i][j] != fv;

    if( !s )
      goto Fail;

    switch( k )
    {
      case 0: sx = 224 - n; break;
      case 1: fx = 224 + n; break;
      case 2: sy = 224 - n; break;
      case 3: fy = 224 + n; break;
    }
  }

  *pkx = ls * 2 / ( fx - sx - 1 );
  *pky = -ls * 2 / ( fy - sy - 1 );
  r = 1;

  Fail:

  Sleep( 200 );
  pbmp->UnlockBits( &bmpData );
  delete pbmp;

  CFile::Remove( path );
  return r;
}
//=========================================================================
void
CSublDlg::OnStartButtonClicked()
{
  if( !IsDlgButtonChecked( IDC_SUD_LOAD_BUTTON ) &&
      !IsDlgButtonChecked( IDC_SUD_INSERT_BMP_BUTTON ) &&
      !IsDlgButtonChecked( IDC_SUD_FIND_BMP_BUTTON ))
    return;

  if( IsDlgButtonChecked( IDC_SUD_LOAD_BUTTON ))
  {
    CString bmpFile( "tmp" );
    int si = ((CComboBox*)GetDlgItem( IDC_SUD_SOURCE_COMBO ))->GetCurSel();
    int bOnRep = IsDlgButtonChecked( IDC_SUD_ONE_REPER_CHECK);
    CPoint Cntr;
        Polar c;

    if( bOnRep )
    {
      m_pReper = pView->GetSelectObject( 0 );
      if( !m_pReper || m_pReper->Class() != REPER )
      {
        ErrBox( "Необходимо выделить один репер!" );
        return;
      }

      CCircle* crcl = (CCircle*)m_pReper->GetActualCont()->GetHead()->GetNext();
      CPoint dp( crcl->GetCoord());
      CText* glat = (CText*)crcl->GetNext()->GetNext();
      CText* mlat = (CText*)glat->GetNext()->GetNext();
      CText* slat = (CText*)mlat->GetNext()->GetNext();
      CText* glon = (CText*)slat->GetNext()->GetNext()->GetNext();
      CText* mlon = (CText*)glon->GetNext()->GetNext();
      CText* slon = (CText*)mlon->GetNext()->GetNext();
            c = Polar( *glat, *mlat, *slat, *glon, *mlon, *slon );
      Cntr = crcl->GetCoord();
      bmpFile.Format( "%2s%2s%2s-%2s%2s%2s",
                      LPCSTR( *glat ), LPCSTR( *mlat ), LPCSTR( *slat ),
                      LPCSTR( *glon ), LPCSTR( *mlon ), LPCSTR( *slon ));
    }

    bmpFile += si == 0 ? ".png" : ".gif";

    CString path( m_BmpPath[1] + bmpFile );

    ksi_obj ksi_env = ksi_nil;
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "source" ), ksi_str02string( bmpFile ));

    if( bOnRep )
    {
      int bYdx = ((CComboBox*)GetDlgItem( IDC_SUD_SOURCE_COMBO ))->GetCurSel() == 0;
      CSize bs( 450, 450 );

            CComboBox* pSC = (CComboBox*)GetDlgItem( IDC_SUD_SCALE_COMBO );
      int Z = 17 - pSC->GetCurSel();

      double kx, ky, co = ::cos( c.lat ); ;

      c.lat /= Deg;
      c.lon /= Deg;

      ULONG_PTR GdiplusToken;
      GdiplusStartupInput gdiplusStartupInput;
      GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );

      double ls = 128;

      while( ls > 8. && !GetScaleRatio( path, c, ls, Z, &kx, &ky ))
        ls /= 2.;

      GdiplusShutdown( GdiplusToken );

      if( ls <= 8. )
        return;

      double const EL = MerkatorData.GetRmj() * Pi / 180. * 100. / SmInUnit;
      double const ML = MerkatorData.GetRmn() * Pi / 180. * 100. / SmInUnit;

      CSize BS( kx * EL * bs.cx, -ky * ML * bs.cy * co );

      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "x0" ), ksi_int2num( Cntr.x - BS.cx / 2 ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "y0" ), ksi_int2num( Cntr.y - BS.cy / 2 ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "x1" ), ksi_int2num( BS.cx ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "y1" ), ksi_int2num( BS.cy ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "a" ), ksi_zero );
    }

    CIdent id( SUBLAYER_CLASS + m_nLayerNum + 1, 0, 0, pView->GetUniId());
    pView->OnUnselect();
    pView->AddMode( new CBuildMode( IDM_CLASS_BEGIN + id.Class(), id,
                                    pView->GetConductor()->GetNode(),
                                    ksi_env, ksi_nil ));
    pView->SetFocus();
    return;
  }

  CString bmpFile;
  CComboBox* pFileList = (CComboBox*)GetDlgItem( IDC_SUD_FILE_LIST );
  if( pFileList->GetCurSel() < 0 )
    return;

  pFileList->GetLBText( pFileList->GetCurSel(), bmpFile );

  if( IsDlgButtonChecked( IDC_SUD_INSERT_BMP_BUTTON ))
  {
    if( pView->GetBuilding( 1 ))
      return;

    ksi_obj ksi_env = ksi_nil;
    ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "source" ), ksi_str02string( bmpFile ));
    int bAutoSize = IsDlgButtonChecked( IDC_SUD_AUTOSIZE_CHECK );
    int bAutoSet = MerkatorData.TrueMerkator();
    char lnkFile[_MAX_PATH];
    FILE* pCf;
    CSize BS;
    float a0, b0, c0, a1, b1, c1;
    CPoint P00, P01, P10;
    int an0;
    CString wldPath;
    CString bmpPath;
    if( bAutoSet )
    {
      static char* ext[][2] =
      {
        ".wld", 0,
        ".tfw", ".tif",
        ".jgw", ".jpg"
      };

      bAutoSet = 0;
      for( int i = CELEM( ext ) ; !bAutoSet && --i >= 0 ; )
      {
        strncpy_s( lnkFile, bmpFile, sizeof lnkFile );
        char* pExt = strrchr( lnkFile, '.' );
        if( !pExt )
          pExt = lnkFile + strlen( lnkFile );

        if( !ext[i][1] || !strcmp( pExt, ext[i][1] ))
        {
          strcpy_s( pExt, sizeof lnkFile - ( pExt - lnkFile ), ext[i][0] );
          for( int k = 2 ; !bAutoSet && --k >= 0 ; )
          {
            wldPath = m_BmpPath[k] + lnkFile;
            bmpPath = m_BmpPath[k] + bmpFile;
            bAutoSet = FileExist( wldPath ) && FileExist( bmpPath );
          }
        }
      }
    }

    if( bAutoSet )
    {
      using namespace Gdiplus;
      GdiplusStartupInput gdiplusStartupInput;
      ULONG_PTR GdiplusToken;
      GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );

      WCHAR wbuf[_MAX_PATH];
      MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, bmpPath, -1, wbuf, _MAX_PATH );
      Bitmap* pBmp = Bitmap::FromFile( wbuf );

      if( pBmp )
      {
        BS.cx = pBmp->GetWidth();
        BS.cy = pBmp->GetHeight();
        delete pBmp;
      }

      GdiplusShutdown( GdiplusToken );

      if( !pBmp )
      {
        CString m;
        m.Format( "Не удаётся загрузить изображение %s!", bmpFile );
        ErrBox( m );
        return;
      }

      pCf = fopen( wldPath, "rt ");
      if( !pCf )
      {
        CString m;
        m.Format( "Не удаётся открыть файл привязки %s!\n"
                  "Выполнять привязку вручную?" );
        switch( AskBox( m ))
        {
          case IDNO:  return;
          case IDYES: bAutoSet = 0;
        }
      }
    }

    if( bAutoSet )
    {
      int nc = fscanf( pCf, "%f%f%f%f%f%f", &a0, &a1, &b0, &b1, &c0, &c1 );
      if( nc != 6 )
      {
        CString m;
        m.Format( "Недостаточно данных в файле привязки %s!\n"
                  "Выполнять привязку вручную?" );
        switch( AskBox( m ))
        {
           case IDNO:  return;
           case IDYES: bAutoSet = 0;
        }
      }
    }

    if( bAutoSet )
    {
      double D0 = a0 * a0 + b0 * b0;
      double D1 = a0 * b1 - b0 * a1;
      D1 /= D0;
      if( fabs( D1 + 1. ) > 1e-7 )
      {
        CString m;
        m.Format( "Непрямоугольное преобразование в файле привязки %s!\n"
                  "Выполнять привязку вручную?" );
        switch( AskBox( m ))
        {
          case IDNO:  return;
          case IDYES: bAutoSet = 0;
        }
      }
    }

    if( bAutoSet )
    {
      POINT tp = MerkatorData.GetOffset();
      P00 = CPoint( tp.x + c0, tp.y - c1 );
      P01.x = a0 * BS.cx;
      P01.y = b0 * BS.cy;
      P10.x = a1 * BS.cx;
      P10.y = b1 * BS.cy;
      an0 = atan2( P01.x, P01.y );

      m_Size.cx = hipo( P01 );
      m_Size.cy = hipo( P10 );

      CRect r( P00, m_Size );
      CBox box( r, P00, an0 );
      CSize docSize( pView->GetDocument()->GetSize());
      CRect R( 0, 0, docSize.cx, docSize.cy );
      if( !R.PtInRect( r.TopLeft()) || !R.PtInRect( r.BottomRight()))
      {
        switch( AskBox( "Изображение выходит за габарит проекта!\n"
                        "Выполнить привязку вручную?" ))
        {
          case IDNO:  return;
          case IDYES: bAutoSet = 0;
        }
      }
      bAutoSize = bAutoSet;
    }

    if( !bAutoSet && !CalcSize( bAutoSize ))
      return;

    if( bAutoSet )
    {
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "number" ), ksi_str02string( bmpFile ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "x0" ), ksi_int2num( P00.x ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "y0" ), ksi_int2num( P00.y ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "a" ),  ksi_int2num( an0 ));
    }

    if( bAutoSize )
    {
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "x1" ), ksi_int2num( m_Size.cx ));
      ksi_env = ksi_assv_set_x( ksi_env, ksi_str02sym( "y1" ), ksi_int2num( m_Size.cy ));
    }

    CIdent id( SUBLAYER_CLASS + m_nLayerNum + 1, 0, 0, pView->GetUniId());

    pView->OnUnselect();
    pView->AddMode( new CBuildMode( IDM_CLASS_BEGIN + SUBLAYER_CLASS, id,
                                    pView->GetConductor()->GetNode(),
                                    ksi_env, ksi_nil ));
    pView->SetFocus();
  }
  else
  {
    int lp = bmpFile.Find( "[" );
    int rp = bmpFile.Find( "]" );
    if( lp >= 0 && rp > 0 )
      bmpFile = bmpFile.Mid( lp + 1, rp - lp - 1 );

    int nClass = SUBLAYER_CLASS + m_nLayerNum + 1;
    CDrawObject* pObj = 0;
    int bf = 0;
    for( POSITION pos = m_UsedList.GetHeadPosition() ; pos && !bf ; )
    {
      pObj = m_UsedList.GetNext( pos );
      if( pObj->Class() == nClass )
      {
        CText* pNumberText = (CText*)(CText*)pObj->GetActualCont()->GetHead()->GetNext();
        CString nt( *pNumberText );
        bf = !bmpFile.CompareNoCase( nt );
      }
    }
    if( !bf || !pView->FindObject( pObj->TabIdent(), 1, 1 ))
      InfBox( "В выбранном слое объект не найден." );
  }
}
//=========================================================================
using namespace Trig;

int
CSublDlg::OnInsertionEnd( CDrawObject* pObj )
{
  if( !pObj )
    return 0;

  if( !pObj->IsSublayer())
    return 1;

  int bOnReper = IsDlgButtonChecked( IDC_SUD_ONE_REPER_CHECK );

  CComboBox* pFileList = (CComboBox*)GetDlgItem( IDC_SUD_FILE_LIST );
  int i;

  if( IsDlgButtonChecked( IDC_SUD_INSERT_BMP_BUTTON ))
  {
    CWaitCursor wc;
    ReportHint( "Обзор подложки..." );
  
    CSize sz( pView->GetDocument()->GetSize());
    RECT r = { 0, 0, sz.cx, sz.cy };
    long t[SUBLR_RANGE];
    for( int i = SUBLR_RANGE ; --i >= 0 ; t[i] = SUBLAYER_CLASS + i + 1 );
    TGstream buf;
    int cnt = 0;
  
		if( pView->m_MapStore.SelectByClass( r, t, CELEM( t ), buf, &cnt ) == R_OK )
		{
      char const* psrc0 = ((CText*)pObj->GetActualCont()->GetHead()->GetNext())->GetStringPtr()->m_String;
  		for( char* pSrc = buf.str() ; --cnt >= 0 ; )
      {
        CDrawObject* pObe = pView->TranslateObjectFromMap( &pSrc );
        if( pObe )
        {
          char const* psrc1 = ((CText*)pObe->GetActualCont()->GetHead()->GetNext())->GetStringPtr()->m_String;
          if( !_stricmp( psrc0, psrc1 ))
          {
            AddItem( pObe );
            RefreshPosSize( pObe );
            if( AskBox( "Извините, такой фрагмент подложки уже введен!\n\nПоказать?" ) == IDYES )
              pView->FindObject( pObe->TabIdent());
            delete pObe;
            return 0;
          }
          delete pObe;
        }
      }
    }
		
    using namespace Gdiplus;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR GdiplusToken;
    GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );
    int ErrNdx = pObj->SplitSubl( 1 );
    GdiplusShutdown( GdiplusToken );

    if( ErrNdx )
    {
      char* err[] =
      {
        "Не найден путь к файлу",
        "Не удаётся выполнить преобразование имени файла",
        "Не удаётся загрузить файл",
        "Не найдено преобразование формата файла",
        "Не удаётся выделить фрагмент изображения"
      };

      CString m;
      m.Format( "Не удаётся разделить очень большое изображение на фрагменты.\n"
                "Проблема: %s\n"
                "Вывод данного изображения может оказаться невозможным\n"
                "или потребовать слишком продолжительного времени.\n\n"
                "Сохранить объект?", err[ErrNdx-1] );
      if( AskBox( m ) != IDYES )
        return 0;
    }

    AddItem( pObj );
    RefreshPosSize( pObj );
    return 1;
  }

  CBar* pBar = (CBar*)pObj->GetActualCont()->GetHead();
  CPoint org( pBar->GetCoord());
  CSize  sze( pBar->GetSize());

  CText* pSourceText = (CText*)pBar->GetNext();

  CString path( m_BmpPath[1] + *pSourceText );

  int bYdx = ((CComboBox*)GetDlgItem( IDC_SUD_SOURCE_COMBO ))->GetCurSel() == 0;
  CSize bs( 450, 450 );
  int bMap = IsDlgButtonChecked( IDC_SUD_MAP_RADIO );
  int bSat = IsDlgButtonChecked( IDC_SUD_SAT_RADIO );
  int bHyb = IsDlgButtonChecked( IDC_SUD_HYB_RADIO );
  int bRslt = 0;
  ULONG_PTR GdiplusToken;
  GdiplusStartupInput gdiplusStartupInput;
  GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );

  CComboBox* pSC = (CComboBox*)GetDlgItem( IDC_SUD_SCALE_COMBO );
  int Z = 17 - pSC->GetCurSel();
  CMainWin* pmw = (CMainWin*)AfxGetMainWnd();

  CWaitCursor wc;

  if( bOnReper )
  {
    CCircle* crcl = (CCircle*)m_pReper->GetActualCont()->GetHead()->GetNext();
    CText* glat = (CText*)crcl->GetNext()->GetNext();
    CText* mlat = (CText*)glat->GetNext()->GetNext();
    CText* slat = (CText*)mlat->GetNext()->GetNext();
    CText* glon = (CText*)slat->GetNext()->GetNext()->GetNext();
    CText* mlon = (CText*)glon->GetNext()->GetNext();
    CText* slon = (CText*)mlon->GetNext()->GetNext();
    Polar c( *glat, *mlat, *slat, *glon, *mlon, *slon );

    c.lat /= Deg;
    c.lon /= Deg;

    WCHAR wbuf[_MAX_PATH];
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, path, -1, wbuf, _MAX_PATH );

    CLSID bmpClsid;
    UINT num, size;
    GetImageEncodersSize( &num, &size );
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*) new BYTE[size];
    GetImageEncoders( num, size, pImageCodecInfo );

    for( i = num ; --i >= 0 && wcscmp( pImageCodecInfo[i].MimeType, bYdx ? L"image/png" : L"image/gif" ) ; );

    if( i >= 0 )
      bmpClsid = pImageCodecInfo[i].Clsid;

    delete pImageCodecInfo;

    if( i < 0 )
      goto Fail;

    CString url;

    if( bYdx )
      url.Format( "%sll=%1.6f,%1.6f&size=%d,%d&z=%d&l=%s&key=%s", m_Y_HTTP,
                  c.lon, c.lat, bs.cx, bs.cy, Z, bMap ? "map" : bSat ? "sat" : bHyb ? "sat,skl" : "map", Y_KEY );
    else
      url.Format( "%scenter=%1.6f,%1.6f&size=%dx%d&zoom=%d&maptype=%s&key=%s", m_G_HTTP,
                  c.lat, c.lon, bs.cx, bs.cy, Z, bMap ? "roadmap" : bSat ? "satellite" : bHyb ? "hybrid" : "roadmap", G_KEY );

    CInternetSession IS;
    CStdioFile* IF = IS.OpenURL( url, 1, INTERNET_FLAG_TRANSFER_BINARY );
    if( IF )
    {
      ofstream OF( path, ios::out|ios::trunc|ios::binary );
      if( !OF.good())
      {
        ErrBox( "Не удается открыть файл\n для сохранения подложки!" );
        goto Fail;
      }
      char buf[4096];
      for(;;)
      {
        int n = IF->Read( buf, sizeof buf );
        if( !n )
          break;
        OF.write( buf, n );
      }
      OF.close();
      IF->Close();
      delete IF;
    }
    else
    {
      ErrBox( "Не удается загрузить данные!" );
      CFile::Remove( path );
      goto Fail;
    }
  }
  else
  {
    CRect R( org, sze );
    R.NormalizeRect();

    CPoint t( pObj->GetBoundBox().CenterPoint());
    Polar c;
    if( !MerkatorData.Prj2Pol( t, c ))
      c = Dec2Pol( t );

    if( c.lat == 0 && c.lon == 0 )
    {
      ErrBox( "Не найдены базовые реперы!" );
      goto Fail;
    }

    double const d = .0001;
    CPoint aln;
    Polar b( c.lat, c.lon + d );
    CPoint tp1;
    if( MerkatorData.Pol2Prj( b, tp1 ) > 0 )
    {
      aln.x = tp1.x;
      aln.y = tp1.y;
    }
    else
      aln = Pol2Dec( b );

    int a0 = pBar->GetAngle();
    int an = atan2( aln - t );
    int MH = 50;

    double kx, ky, co = ::cos( c.lat );
    double const mc = 60;

    c.lat /= Deg;
    c.lon /= Deg;

    int glat = c.lat;
    int mlat = ( c.lat - glat ) * mc;
    double slat = ( c.lat - glat - mlat / mc ) * mc * mc;
    int glon = c.lon;
    int mlon = ( c.lon - glon ) * mc;
    double slon = ( c.lon - glon - mlon / mc ) * mc * mc;

    CString bmpFile;
    bmpFile.Format( "%02d%02d%1.2f-%02d%02d%1.2f", glat, mlat, slat, glon, mlon, slon );
    bmpFile += bYdx ? ".png" : ".gif";

    pSourceText->GetStringPtr()->SetString( bmpFile );
    path = m_BmpPath[1] + *pSourceText;

    double ls = 128;
    while( ls > 8. && !GetScaleRatio( path, c, ls, Z, &kx, &ky ))
      ls /= 2.;

    if( ls <= 8. )
      goto Fail;

    CBox RR( pObj->GetBoundBox(), t, -an );

    double sf = MerkatorData.ScaleFactor( t );
    double Rmj = MerkatorData.GetRmj();
    double Rmn = MerkatorData.GetRmj();

    double const el = sf * Rmj * Pi / 180. * 100. / SmInUnit;
    double const ml = sf * Rmn * Pi / 180. * 100. / SmInUnit;

    CSize RS( R.Width()  / kx / el, - R.Height()  / ky / ml / co );
    CSize BS( RR.Width() / kx / el, - RR.Height() / ky / ml / co );

    ky *= co;
    kx /= co;

    Polar tl( c.lat - BS.cy * ky / 2, c.lon - BS.cx * kx / 2 );

    Bitmap* pBmp = ::new Bitmap( BS.cx, BS.cy, PixelFormat32bppARGB );
    if( !pBmp )
    {
      ErrBox( "Недостаточно памяти!" );
      goto Fail;
    }

    WCHAR wbuf[_MAX_PATH];
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, path, -1, wbuf, _MAX_PATH );

    CLSID bmpClsid;
    UINT num, size;
    GetImageEncodersSize( &num, &size );
    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*) new BYTE[size];
    GetImageEncoders( num, size, pImageCodecInfo );

    for( i = num ; --i >= 0 && wcscmp( pImageCodecInfo[i].MimeType, bYdx ? L"image/png" : L"image/gif" ) ; );

    if( i >= 0 )
      bmpClsid = pImageCodecInfo[i].Clsid;

    delete pImageCodecInfo;

    if( i < 0 )
    {
      ::delete pBmp;
      goto Fail;
    }

    CWaitCursor wc;
    ReportHint( "Загрузка подложки..." );

    ky /= co;
    kx *= co;
    co = ::cos( tl.lat * Deg );
    ky *= co;
    kx /= co;

    int H = bs.cy - MH;
    int n = ( BS.cy / H + ( BS.cy % H > 0 )) * ( BS.cx / bs.cx + ( BS.cx % bs.cx > 0 ));
    int N = App.GetProfileInt( "General", "SubBmpLimit", 256 );
    if( n > N )
    {
      ::delete pBmp;
      ErrBox( "Слишком большой фрагмент!\n\n"
              "Выберите меньшую область или\n"
              "уровень масштабирования." );
      goto Fail;
    }

    pmw->ShowProgress( 0, n );

    for( int raw = 0 ; raw < BS.cy ; raw += H )
    {
      int rsz = min( bs.cy, BS.cy - raw + MH );
      double clat = tl.lat + ky * ( raw + rsz / 2 );

      ky /= co;
      kx *= co;
      co = ::cos( clat * Deg );
      ky *= co;
      kx /= co;

      for( int col = 0 ; col < BS.cx ; col += bs.cx )
      {
        int csz = min( bs.cx, BS.cx - col );
        double clon = tl.lon + kx * ( col + csz / 2 );

        CString url;

        if( bYdx )
          url.Format( "%sll=%1.6f,%1.6f&size=%d,%d&z=%d&l=%s&key=%s", m_Y_HTTP,
                      clon, clat, csz, rsz, Z, bMap ? "map" : bSat ? "sat" : bHyb ? "sat,skl" : "map",
                      Y_KEY );
        else
          url.Format( "%scenter=%1.6f,%1.6f&size=%dx%d&zoom=%d&maptype=%s&key=%s", m_G_HTTP,
                      clat, clon, csz, rsz, Z, bMap ? "roadmap" : bSat ? "satellite" : bHyb ? "hybrid" : "roadmap",
                      G_KEY );

        CInternetSession IS;
        CStdioFile* IF = IS.OpenURL( url, 1, INTERNET_FLAG_TRANSFER_BINARY );
        if( IF )
        {
          ofstream OF( path, ios::out|ios::trunc|ios::binary );
          if( !OF.good())
          {
            ErrBox( "Не удается открыть временный файл\n для сохранения подложки!" );
            goto Fail;
          }
          char buf[4096];
          for(;;)
          {
            int n = IF->Read( buf, sizeof buf );
            if( !n )
              break;
            OF.write( buf, n );
          }
          IF->Close();
          delete IF;
          IS.Close();
        }
        else
        {
          ErrBox( "Не удается загрузить данные!" );
          CFile::Remove( path );
          goto Fail;
        }

        Bitmap* pbmp = Bitmap::FromFile( wbuf );
        if( !pbmp )
        {
          ErrBox( "Некорректные данные загружены!" );
          ::delete pBmp;
          CFile::Remove( path );
          goto Fail;
        }

        BitmapData bmpData, BmpData;
        Gdiplus::Rect Rect( col, raw, csz, rsz - MH );
        Gdiplus::Rect rect( 0, 0, csz, rsz );

        pBmp->LockBits( &Rect, ImageLockModeWrite, PixelFormat32bppARGB, &BmpData );
        if( pbmp->LockBits( &rect, ImageLockModeRead,  PixelFormat32bppARGB, &bmpData ) != Ok )
        {
          ErrBox( "Превышен лимит на количество скачиваний!\n"
                  "Повторите попытку позже." );
          ::delete pBmp;
          delete pbmp;
          CFile::Remove( path );
          goto Fail;
        }

        BYTE* pbr = (BYTE*)bmpData.Scan0;
        BYTE* pbw = (BYTE*)BmpData.Scan0;

        for( int i = rsz - MH ; --i >= 0 ; )
        {
          memcpy( pbw, pbr, csz * sizeof UINT );
          pbw += BmpData.Stride;
          pbr += bmpData.Stride;
        }

        pbmp->UnlockBits( &bmpData );
        pBmp->UnlockBits( &BmpData );
        delete pbmp;
        Sleep( 200 );
        pmw->OffsetProgress( 1 );
      }
    }

    CDC* pDC = GetDC();
    CMemDC memDC( pDC );

    CBitmap tBmp;
    tBmp.CreateCompatibleBitmap( pDC, RS.cx, RS.cy );
    memDC.SelectObject( &tBmp );
          memDC.FillSolidRect( CRect( CPoint( 0, 0 ), BS ), RGB( 255, 255, 255 ));
    CRect tr( 0, 0, BS.cx, BS.cy );
    int dx = ( RS.cx - BS.cx ) / 2;
    int dy = ( RS.cy - BS.cy ) / 2;
    tr.OffsetRect( dx, dy );
    CPoint tc( tr.CenterPoint());
    an -= a0;
    Gdiplus::Point pp[] =
    {
      MkPoint( TurnPoint( tr.TopLeft(), tc, an )),
      MkPoint( TurnPoint( CPoint( tr.right, tr.top ), tc, an )),
      MkPoint( TurnPoint( CPoint( tr.left, tr.bottom ), tc, an ))
    };

    Graphics Grh( memDC.GetSafeHdc());
        Grh.SetPageUnit( UnitPixel );
          Grh.DrawImage( pBmp, pp, 3 );

    ::delete pBmp;
    Bitmap Sbmp( tBmp, 0 );
    if( Sbmp.Save( wbuf, &bmpClsid, 0 ) != Ok )
    {
      ErrBox( "Не удается сохранить файл подложки!" );
      goto Fail;
    }
  }
  bRslt = 1;

  Fail:

  GdiplusShutdown( GdiplusToken );

  if( bRslt )
  {
    AddItem( pObj );
    RefreshPosSize( pObj );
  }

  pmw->HideProgress();

  return bRslt;
}
//======================================================================
