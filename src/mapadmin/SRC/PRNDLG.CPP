#include "stdafx.h"

#include "lomedit.h"
#include "prndlg.h"
#include "dlgids.h"
#include "mview.h"
#include "mdoc.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "pfmode.h"
#include "scale.h"
#include "ids.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
IMPLEMENT_DYNAMIC( CPageSetupPage,   CPropertyPage )
IMPLEMENT_DYNAMIC( CDocSetupPage,    CPropertyPage )
IMPLEMENT_DYNAMIC( CPrintSetupSheet, CPropertySheet )
//=====================================================================
CRect CPageSetupPage::m_spc;

long CPageSetupPage::m_layopt;
long CPageSetupPage::m_overlapVal;
long CPageSetupPage::m_spaceEnb;
long CPageSetupPage::m_lineEnb;
long CPageSetupPage::m_overlapEnb;
long CPageSetupPage::m_spaceVal;
long CPageSetupPage::m_lineVal;
long CPageSetupPage::m_numerationEnb;
long CPageSetupPage::m_startNumber;
long CPageSetupPage::m_subscriptEnb;

long CDocSetupPage::m_colorEnb;
long CDocSetupPage::m_selectionEnb;
long CDocSetupPage::m_selectOnlyEnb;
long CDocSetupPage::m_curscaleEnb;
long CDocSetupPage::m_scaleVal;
long CDocSetupPage::m_autoSizeEnb;
long CDocSetupPage::m_autoScaleEnb;
long CDocSetupPage::m_extraSpaceVal;

CString CPageSetupPage::m_subscriptText;
//=====================================================================
inistr CDocSetupPage::m_IniStr[] =
{
  IDC_DSD_CURSCALE_ENB,    &m_curscaleEnb,
  IDC_DSD_COLOR_ENB,       &m_colorEnb,
  IDC_DSD_AUTO_SIZE_ENB,   &m_autoSizeEnb,
  IDC_DSD_AUTO_SCALE_ENB,  &m_autoScaleEnb,
  IDC_DSD_SELECT_ENB,      &m_selectionEnb,
  IDC_DSD_SELECT_ONLY_ENB, &m_selectOnlyEnb
};
//=====================================================================
inistr CPageSetupPage::m_IniStr[] =
{
  IDC_PSD_OFR_SPACE_ENB,  &m_spaceEnb,
  IDC_PSD_OFR_LINE_ENB,   &m_lineEnb,
  IDC_PSD_OVERLAP_ENB,    &m_overlapEnb,
  IDC_PSD_NUMERATION_ENB, &m_numerationEnb,
  IDC_PSD_SUBSCRIPT_ENB,  &m_subscriptEnb,
};
//=====================================================================
BEGIN_MESSAGE_MAP( CPageSetupPage, CPropertyPage )
  ON_COMMAND( IDC_PSD_FRM_LYT_ALBUM, OnAlbSelected )
  ON_COMMAND( IDC_PSD_FRM_LYT_GLUE,  OnMapSelected )
  ON_COMMAND( IDC_PSD_NUMERATION_ENB,  OnCheckSelected )
  ON_COMMAND( IDC_PSD_SUBSCRIPT_ENB,  OnCheckSelected )
  ON_COMMAND( IDC_PSD_OFR_SPACE_ENB,  OnCheckSelected )
  ON_COMMAND( IDC_PSD_OFR_LINE_ENB,  OnCheckSelected )
  ON_COMMAND( IDC_PSD_OVERLAP_ENB,  OnCheckSelected )
END_MESSAGE_MAP()
//=====================================================================
static UINT AlbProp[] =
{
  IDC_PSD_SPC_TOP_EDIT,
  IDC_PSD_SPC_LEFT_EDIT,
  IDC_PSD_SPC_BOTTOM_EDIT,
  IDC_PSD_SPC_RIGHT_EDIT,
  IDC_PSD_NUMERATION_ENB,
  IDC_PSD_START_NUM_EDIT,
  IDC_PSD_SUBSCRIPT_ENB,
  IDC_PSD_SUBSCRIPT_EDIT,
  0
};

static UINT MapProp[] =
{
  IDC_PSD_OFR_SPACE_ENB,
  IDC_PSD_OFR_SPACE_EDIT,
  IDC_PSD_OFR_LINE_ENB,
  IDC_PSD_OFR_LINE_EDIT,
  IDC_PSD_OVERLAP_ENB,
  IDC_PSD_OVERLAP_EDIT,
  0
};

static UINT PageProp[] =
{
  IDC_PSD_NUMERATION_ENB, IDC_PSD_START_NUM_EDIT,
  IDC_PSD_SUBSCRIPT_ENB, IDC_PSD_SUBSCRIPT_EDIT,
  IDC_PSD_OFR_SPACE_ENB, IDC_PSD_OFR_SPACE_EDIT,
  IDC_PSD_OFR_LINE_ENB, IDC_PSD_OFR_LINE_EDIT,
  IDC_PSD_OVERLAP_ENB, IDC_PSD_OVERLAP_EDIT,
  0
};
//=====================================================================
void
CPageSetupPage::OnCheckSelected()
{
  for( UINT* p = PageProp ; *p ; p += 2 )
    GetDlgItem( p[1] )->EnableWindow(
                  GetDlgItem( p[0] )->IsWindowEnabled() && IsDlgButtonChecked( p[0] ));
  if( IsWindow( m_pDsp->GetSafeHwnd()))
    m_pDsp->OnCheckSelected();
}
//=====================================================================
void
CPageSetupPage::RED( UINT* el, UINT* dl )
{
  for( ; *el ; GetDlgItem( *el++ )->EnableWindow( 1 ));
  for( ; *dl ; GetDlgItem( *dl++ )->EnableWindow( 0 ));
  OnCheckSelected();
}
//=====================================================================
void
CPageSetupPage::OnAlbSelected()
{
  m_layopt = 0;
  RED( AlbProp, MapProp );
}
//=====================================================================
void
CPageSetupPage::OnMapSelected()
{
  m_layopt = 1;
  RED( MapProp, AlbProp );
}
//=====================================================================
CPageSetupPage::CPageSetupPage() :
  CPropertyPage( IDD_PAGESETUP ),
  m_bInited( 0 )
{}
//=====================================================================
int
CPageSetupPage::OnInitDialog()
{
  static CSpinIni S[] =
  {
    { IDC_PSD_OFR_SPACE_EDIT,  IDC_PSD_OFR_SPACE_SPIN,  &m_spaceVal,    0, 1, MAX_DOCUMENT_FRAME_SPACE },
    { IDC_PSD_SPC_TOP_EDIT,    IDC_PSD_SPC_TOP_SPIN,    &m_spc.top,     0, 1, MAX_SPC_TOP    },
    { IDC_PSD_SPC_BOTTOM_EDIT, IDC_PSD_SPC_BOTTOM_SPIN, &m_spc.bottom,  0, 1, MAX_SPC_BOTTOM },
    { IDC_PSD_SPC_LEFT_EDIT,   IDC_PSD_SPC_LEFT_SPIN,   &m_spc.left,    0, 1, MAX_SPC_LEFT   },
    { IDC_PSD_SPC_RIGHT_EDIT,  IDC_PSD_SPC_RIGHT_SPIN,  &m_spc.right,   0, 1, MAX_SPC_RIGHT  },
    { IDC_PSD_OVERLAP_EDIT,    IDC_PSD_OVERLAP_SPIN,    &m_overlapVal,  0, 1, MAX_DOCUMENT_OVERLAP },
    { IDC_PSD_START_NUM_EDIT,  IDC_PSD_START_NUM_SPIN,  &m_startNumber, 1, 1, 999 }
  };

  for( int n = CELEM( S ) ; --n >= 0 ; iedit[n].Initialize( S[n], this ));

  static CSpinIni Sf =
    { IDC_PSD_OFR_LINE_EDIT,  IDC_PSD_OFR_LINE_SPIN,  &m_lineVal, 0, 1, MAX_DOCUMENT_FRAME_LINE, 1, 10, "%2.1f" };

  fedit[0].Initialize( Sf, this );

  CheckRadioButton( IDC_PSD_FRM_LYT_ALBUM, IDC_PSD_FRM_LYT_GLUE, m_layopt + IDC_PSD_FRM_LYT_ALBUM );

  m_bInited = CPropertyPage::OnInitDialog();
  if( m_layopt )
    OnMapSelected();
  else
    OnAlbSelected();

  return m_bInited;
}
//=====================================================================
void
CPageSetupPage::DoDataExchange( CDataExchange* pDX )
{
  DDX_Text( pDX, IDC_PSD_SUBSCRIPT_EDIT, m_subscriptText );

  for( inistr* pi = m_IniStr + CELEM( m_IniStr ) ; --pi >= m_IniStr ; )
    if( pDX->m_bSaveAndValidate )
      *pi->pv = IsDlgButtonChecked( pi->id );
    else
      CheckDlgButton( pi->id, *pi->pv );
}
//=====================================================================
void
CPageSetupPage::OnOK()
{
  if( m_bInited )
  {
    for( int i = CELEM( iedit ) ; --i >= 0 ; iedit[i].SetValue());
    fedit[0].SetValue();
    UpdateData( 1 );
  }
}
//=====================================================================
BEGIN_MESSAGE_MAP( CDocSetupPage, CPropertyPage )
  ON_COMMAND( IDC_DSD_AUTO_SIZE_ENB,  OnCheckSelected )
  ON_COMMAND( IDC_DSD_AUTO_SCALE_ENB, OnCheckSelected )
  ON_COMMAND( IDC_DSD_CURSCALE_ENB,   OnCheckSelected )
END_MESSAGE_MAP()
//=====================================================================
CDocSetupPage::CDocSetupPage() :
  CPropertyPage( IDD_DOCSETUP ),
  m_bInited( 0 )
{}
//=====================================================================
void
CDocSetupPage::OnCheckSelected()
{
  GetDlgItem( IDC_DSD_AUTO_SCALE_ENB )->EnableWindow( CPageSetupPage::m_layopt );
  int asc = CPageSetupPage::m_layopt && IsDlgButtonChecked( IDC_DSD_AUTO_SCALE_ENB );
  GetDlgItem( IDC_DSD_CURSCALE_ENB )->EnableWindow( !asc );
  GetDlgItem( IDC_DSD_EXTRA_SPACE_EDIT )->EnableWindow( IsDlgButtonChecked( IDC_DSD_AUTO_SIZE_ENB ));
  GetDlgItem( IDC_DSD_SCALE_EDIT )->EnableWindow( !asc && !IsDlgButtonChecked( IDC_DSD_CURSCALE_ENB ));
}
//=====================================================================
void
CDocSetupPage::DoDataExchange( CDataExchange* pDX )
{
  for( inistr* pi = m_IniStr + CELEM( m_IniStr ) ; --pi >= m_IniStr ; )
    if( pDX->m_bSaveAndValidate )
      *pi->pv = IsDlgButtonChecked( pi->id );
    else
      CheckDlgButton( pi->id, *pi->pv );
}
//=====================================================================
int
CDocSetupPage::OnInitDialog()
{
  static CSpinIni Si[] =
  {
    { IDC_DSD_EXTRA_SPACE_EDIT, IDC_DSD_EXTRA_SPACE_SPIN, &m_extraSpaceVal,  0,   1, MAX_DOCUMENT_FRAME_SPACE },
    { IDC_DSD_SCALE_EDIT,       IDC_DSD_SCALE_SPIN,       &m_scaleVal,     100, 100, 300000 }
  };

  for( int i = CELEM( Si ) ; --i >= 0 ; iedit[i].Initialize( Si[i], this ));

  m_bInited = CPropertyPage::OnInitDialog();
        OnCheckSelected();

  return m_bInited ;
}
//=====================================================================
void
CDocSetupPage::OnOK()
{
  if( m_bInited )
  {
    for( int i = CELEM( iedit ) ; --i >= 0 ; iedit[i].SetValue());
    UpdateData( 1 );
  }
}
//=====================================================================
BEGIN_MESSAGE_MAP( CPrintSetupSheet, CPropertySheet )
  ON_COMMAND( IDOK, OnOK )
END_MESSAGE_MAP()
//=====================================================================
int CPrintSetupSheet::m_curIndex = 0;
//=====================================================================
CPrintSetupSheet::CPrintSetupSheet( CWnd* pWndParent ) :
  CPropertySheet( "Параметры печати", pWndParent )
{
  AddPage( &m_pageSetupPage );
  AddPage( &m_docSetupPage );
  m_pageSetupPage.SetDsp( &m_docSetupPage );
  SetActivePage( m_curIndex );
}
//=====================================================================
void
CPrintSetupSheet::OnOK()
{
  m_pageSetupPage.OnOK();
  m_docSetupPage.OnOK();
  m_curIndex = GetActiveIndex();
  EndDialog( IDOK );
}
//=====================================================================
