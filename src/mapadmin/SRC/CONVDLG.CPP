#include "stdafx.h"

#include <afxdtctl.h>
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "convdlg.h"
#include "lawdlg.h"
#include "mview.h"
#include "mdoc.h"
#include "msutil.h"
#include "ksi_type.h"
#include "joke.h"
#include "miscmode.h"
#include "instrdlg.h"
#include "util.h"
#include "dwin.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
static char stdcnfpath[] = "convertor.cfg";
//=====================================================================
double CConvDlg::m_Cfs[3][3];
int    CConvDlg::m_bNumerateFrom = 0;
int    CConvDlg::m_bOffsetX = 0;
int    CConvDlg::m_bOffsetY = 0;
int    CConvDlg::m_bScaleX  = 0;
int    CConvDlg::m_bScaleY  = 0;
int    CConvDlg::m_bRotate  = 0;
long   CConvDlg::m_Angle    = 0;
double CConvDlg::m_OffsetX  = 0;
double CConvDlg::m_OffsetY  = 0;
double CConvDlg::m_ScaleX   = 1;
double CConvDlg::m_ScaleY   = 1;
double CConvDlg::m_CenterX  = 0;
double CConvDlg::m_CenterY  = 0;
CTime  CConvDlg::m_Time1    = 0;
CTime  CConvDlg::m_Time2    = 0;
double CConvDlg::m_UTMP[] = { 0, 0, 0, 1, 0 };
//=====================================================================
CConvDlg::CfgRec CConvDlg::ControlConfigTab[] =
{
  CHECK, IDC_CVD_TEXT_TO_PROJECT_RADIO,       "TEXT_TO_PROJECT_RADIO",
  CHECK, IDC_CVD_PROJECT_TO_PROJECT_RADIO,    "PROJECT_TO_PROJECT_RADIO",
  CHECK, IDC_CVD_PROJECT_TO_TEXT_RADIO,       "PROJECT_TO_TEXT_RADIO",
  CHECK, IDC_CVD_PROJECT_TO_MIF_RADIO,        "PROJECT_TO_MIF_RADIO",
  CHECK, IDC_CVD_MIF_TO_PROJECT_RADIO,        "MIF_TO_PROJECT_RADIO",
  CHECK, IDC_CVD_ASSAMBLY_BY_CLASS_RADIO,     "ASSAMBLY_BY_CLASS_RADIO",
  CHECK, IDC_CVD_ASSAMBLY_BY_TABLE_RADIO,     "ASSAMBLY_BY_TABLE_RADIO",
  CHECK, IDC_CVD_ASSAMBLY_ALL_RADIO,          "ASSAMBLY_ALL_RADIO",
  CHECK, IDC_CVD_CREATE_NEW_CHECK,            "CREATE_NEW_CHECK",
  CHECK, IDC_CVD_SQUIZZE_CHECK,               "SQUIZZE_CHECK",
  CHECK, IDC_CVD_CONVERT_COORD_CHECK,         "CONVERT_COORD_CHECK",
  CHECK, IDC_CVD_CONVERT_CLASSES_CHECK,       "CONVERT_CLASSES_CHECK",
  CHECK, IDC_CVD_CONVERT_TABLES_CHECK,        "CONVERT_TABLES_CHECK",
  CHECK, IDC_CVD_CONVERT_NODES_CHECK,         "CONVERT_NODES_CHECK",
  CHECK, IDC_CVD_REPLACE_TABLE_RADIO,         "REPLACE_TABLE_RADIO",
  CHECK, IDC_CVD_IGNORE_TABLE_RADIO,          "IGNORE_TABLE_RADIO",
  CHECK, IDC_CVD_CREATE_NEW_TABLE_RADIO,      "CREATE_NEW_TABLE_RADIO",  
  CHECK, IDC_CVD_REMOVE_CONVERTED_CHECK,      "REMOVE_CONVERTED_CHECK",
  CHECK, IDC_CVD_OUTPUT_GEO_METR_CHECK,       "OUTPUT_GEO_METR_CHECK",
  CHECK, IDC_CVD_OUTPUT_GEO_GRAD_CHECK,       "OUTPUT_GEO_GRAD_CHECK",
  CHECK, IDC_CVD_REMOVE_DUPLICATES_CHECK,     "REMOVE_DUPICATES_CHECK",
  CHECK, IDC_CVD_CONVERT_OBJECTS_CHECK,       "CONVERT_OBJECTS_CHECK",
  CHECK, IDC_CVD_USE_REGION_CHECK,            "USE_REGION_CHECK",
  CHECK, IDC_CVD_CONVERT_DELETED_CHECK,       "CONVERT_DELETED_CHECK",
  CHECK, IDC_CVD_INCLUDE_GRAPH_CLASSES_CHECK, "INCLUDE_GRAPH_CLASSES_CHECK",
  CHECK, IDC_CVD_CREATE_IN_CONTEXT_CHECK,     "CREATE_IN_CONTEXT_CHECK",
  CHECK, IDC_CVD_IMPROVE_POLY_CHECK,          "IMPROVE_POLY_CHECK",
  CHECK, IDC_CVD_CREATE_SUBSRIPTS_CHECK,      "CREATE_SUBSRIPTS_CHECK",
  CHECK, IDC_CVD_SEPARATE_POLY_CHECK,         "SEPARATE_POLY_CHECK",
  CHECK, IDC_CVD_MOVE_TO_PROJECT_CHECK,       "MOVE_TO_PROJECT_CHECK",
  CHECK, IDC_CVD_SET_NODE_CHECK,              "SET_NODE_CHECK",
  EDIT,  IDC_CVD_NODE_EDIT,                   "NODE_EDIT",
  EDIT,  IDC_CVD_TXT_PATH_EDIT,               "TXT_PATH_EDIT",
  EDIT,  IDC_CVD_LOG_PATH_EDIT,               "LOG_PATH_EDIT",
  EDIT,  IDC_CVD_CLASS_NUMBER_EDIT,           "CLASS_EDIT",
  EDIT,  IDC_CVD_TABLE_EDIT,                  "TABLE_EDIT",
  EDIT,  IDC_CVD_ENTRY_NUMBER_EDIT,           "ENTRY_EDIT",
  EDIT,  IDC_CVD_STATUS_NUMBER_EDIT,          "STATUS_EDIT",
  EDIT,  IDC_CVD_CONTEXT_PARAM_EDIT,          "CONTEXT_PARAM_EDIT",
  EDIT,  IDC_CVD_DECOMPOSE_PARAM_EDIT,        "DECOMPOSE_PARAM_EDIT",
  EDIT,  IDC_CVD_IMPROVE_POLY_PARAM_EDIT,     "IMPROVE_POLY_PARAM_EDIT",
  EDIT,  IDC_CVD_SUBS_PARAM_EDIT,             "SUBS_PARAM_EDIT",
  CHECK, IDC_CVD_FILTER_ID_CHECK,             "FILTER_ID_CHECK",
  EDIT,  IDC_CVD_MIN_ID_EDIT,                 "MIN_ID_EDIT",
  EDIT,  IDC_CVD_MAX_ID_EDIT,                 "MAX_ID_EDIT",
  MAP,   0,                                   "CLASS_MAP",
  CHECK, IDC_CVD_SELECT_BY_DATE_CHECK,        "SELECT_BY_DATE_CHECK",
  EDIT,  IDC_CVD_SELECT_BY_DATE_EDIT1,        "SELECT_BY_DATE_EDIT1",
  EDIT,  IDC_CVD_SELECT_BY_DATE_EDIT2,        "SELECT_BY_DATE_EDIT2",
  CHECK, IDC_CVD_SELECT_BY_KEY_CHECK,         "SELECT_BY_KEY_CHECK",
  EDIT,  IDC_CVD_KEY_FIELD_EDIT,              "KEY_FIELD_EDIT",
  EDIT,  IDC_CVD_KEY_STRING_EDIT,             "KEY_STRING_EDIT",
  EDIT,  IDC_CVD_KEY_STRING_EDIT,             "KEY_STRING_EDIT",
	CHECK, IDC_CVD_UTM2M_CHECK,                 "UTM2M_CHECK",
	CHECK, IDC_CVD_SELECT_BY_IDENT_CHECK,       "SELECT_BY_IDENT_CHECK",

  NOTHING
};
//=========================================================================
//=====================================================================
CConvDlg::DatRec CConvDlg::DataConfigTab[]  =
{
	ITIME,  &m_Time1,         "TIME_1",         0,
  ITIME,  &m_Time2,         "TIME_2",         0,
  IINT,   &m_bNumerateFrom, "NUM_FROM_BTN",   0,
  IBOOL,  &m_bOffsetX,      "OFFSET_X_CHECK", 0,
  IBOOL,  &m_bOffsetY,      "OFFSET_Y_CHECK", 0,
  IBOOL,  &m_bScaleX,       "SCALE_X_CHECK",  0,
  IBOOL,  &m_bScaleY,       "SCALE_Y_CHECK",  0,
  IBOOL,  &m_bRotate,       "ROTATE_CHECK",   0,
  IINT,   &m_Angle,         "ANGLE_EDIT",     0,
  IREAL,  &m_OffsetX,       "OFFSET_X_EDIT",  0,
  IREAL,  &m_OffsetY,       "OFFSET_Y_EDIT",  0,
  IREAL,  &m_CenterX,       "CENTER_X_EDIT",  0,
  IREAL,  &m_CenterY,       "CENTER_Y_EDIT",  0,
  IREAL,  &m_ScaleX,        "SCALE_X_EDIT",   0,
  IREAL,  &m_ScaleY,        "SCALE_Y_EDIT",   0,

  IREAL,  &m_UTMP[0],       "UTM_X0",         0,
  IREAL,  &m_UTMP[1],       "UTM_Y0",         0,
  IREAL,  &m_UTMP[2],       "UTM_L0",         0,
  IREAL,  &m_UTMP[3],       "UTM_K0",         0,
  IREAL,  &m_UTMP[4],       "UTM_E0",         0,
	
	IREAL,  &m_Cfs[0][0],     "CF00",           IDC_CVD_AX_EDIT,
	IREAL,  &m_Cfs[0][1],     "CF01",           IDC_CVD_BX_EDIT,
	IREAL,  &m_Cfs[0][2],     "CF02",           IDC_CVD_CX_EDIT,
	IREAL,  &m_Cfs[1][0],     "CF10",           IDC_CVD_AY_EDIT,
	IREAL,  &m_Cfs[1][1],     "CF11",           IDC_CVD_BY_EDIT,
	IREAL,  &m_Cfs[1][2],     "CF12",           IDC_CVD_CY_EDIT,
  NOTHING
};
//=========================================================================
IMPLEMENT_DYNCREATE( CConvDlg, CDialog )
//=====================================================================
BEGIN_MESSAGE_MAP( CConvDlg, CDialog )
  ON_BN_CLICKED( IDC_CVD_SET_CNV_PARAM_BUTTON,        OnSetCnvParamClicked )
  ON_BN_CLICKED( IDC_CVD_NUMERATE_BUTTON,             OnNumerateButtonClicked )
  ON_BN_CLICKED( IDC_CVD_SELECT_CLASS_BUTTON,         OnSelectClassButtonClicked )
  ON_BN_CLICKED( IDC_CVD_FIND_GRAPH_CLASSES_BUTTON,   OnFindClassButtonClicked )
  ON_BN_CLICKED( IDC_CVD_TEXT_TO_PROJECT_RADIO,       OnTextToProjectClicked )
  ON_BN_CLICKED( IDC_CVD_PROJECT_TO_PROJECT_RADIO,    OnProjectToProjectClicked )
  ON_BN_CLICKED( IDC_CVD_PROJECT_TO_TEXT_RADIO,       OnProjectToTextClicked )
  ON_BN_CLICKED( IDC_CVD_PROJECT_TO_MIF_RADIO,        OnProjectToMifClicked )
  ON_BN_CLICKED( IDC_CVD_MIF_TO_PROJECT_RADIO,        OnMifToProjectClicked )
  ON_BN_CLICKED( IDC_CVD_FIND_TXT_BUTTON,             OnFindTxtCliced )
  ON_BN_CLICKED( IDC_CVD_FIND_LOG_BUTTON,             OnFindLogCliced )
  ON_BN_CLICKED( IDC_CVD_CONVERT_COORD_CHECK,         OnConvertCoordClicked )
  ON_BN_CLICKED( IDC_CVD_OUTPUT_GEO_METR_CHECK,       OnOutputGeoMetrClicked )
  ON_BN_CLICKED( IDC_CVD_OUTPUT_GEO_GRAD_CHECK,       OnOutputGeoGradClicked )
  ON_BN_CLICKED( IDC_CVD_INCLUDE_GRAPH_CLASSES_CHECK, OnEditCheckClicked )
  ON_BN_CLICKED( IDC_CVD_CREATE_IN_CONTEXT_CHECK,     OnEditCheckClicked )
  ON_BN_CLICKED( IDC_CVD_IMPROVE_POLY_CHECK,          OnEditCheckClicked )
  ON_BN_CLICKED( IDC_CVD_CREATE_SUBSRIPTS_CHECK,      OnEditCheckClicked )
  ON_BN_CLICKED( IDC_CVD_SET_NODE_CHECK,              OnEditCheckClicked )
  ON_BN_CLICKED( IDC_CVD_FILTER_ID_CHECK,             OnEditCheckClicked )
  ON_BN_CLICKED( IDC_CVD_SEPARATE_POLY_CHECK,         OnEditCheckClicked )
  ON_BN_CLICKED( IDC_CVD_SELECT_BY_KEY_CHECK,         OnEditCheckClicked )
  ON_BN_CLICKED( IDC_CVD_STOP_BUTTON,                 OnStopClicked )
  ON_BN_CLICKED( IDC_CVD_SAVE_BUTTON,                 OnSaveClicked )
  ON_BN_CLICKED( IDC_CVD_RESTORE_BUTTON,              OnRestoreClicked )
  ON_BN_CLICKED( IDC_CVD_SELECT_BY_DATE_CHECK,        OnSelectDateButtonClicked )
  ON_BN_CLICKED( IDC_CVD_SELECT_DATE_BUTTON1,         OnDate1ButtonClicked )
  ON_BN_CLICKED( IDC_CVD_SELECT_DATE_BUTTON2,         OnDate2ButtonClicked )
	ON_BN_CLICKED( IDC_CVD_UTM2M_CHECK,                 OnUTM2MButtonClicked )
	ON_BN_CLICKED( IDC_CVD_CONVERT_OBJECTS_CHECK,       OnConvObjButtonClicked )

	ON_BN_CLICKED( IDC_CVD_CONVERT_CLASSES_CHECK,       OnConvCTNClicked )
	ON_BN_CLICKED( IDC_CVD_CONVERT_TABLES_CHECK,        OnConvCTNClicked )
	ON_BN_CLICKED( IDC_CVD_CONVERT_NODES_CHECK,         OnConvCTNClicked )

  ON_MESSAGE( IDM_CONVERTOR_REPORT, Report )
END_MESSAGE_MAP()
//=====================================================================
static int
IsValidPar( ksi_obj ksi_par )
{
	ksi_obj ksi_id;
	if( KSI_PLIST_P( ksi_par ))
	{
		ksi_id = KSI_CAR( ksi_par );
		ksi_par  = KSI_CDR( ksi_par );
	}
	else
	if( KSI_PAIR_P( ksi_par ))
	{
		ksi_id = ksi_par;
		ksi_par  = ksi_nil;
	}
	else
		return 0;

	if( !KSI_EXACT_P( KSI_CAR( ksi_id )) || !KSI_EXACT_P( KSI_CDR( ksi_id )))
		return 0;

	for( ksi_obj p = ksi_par ; !KSI_NULL_P( p ) ;  p = KSI_CDR( p ))
	{
		ksi_obj ksi_pai = KSI_CAR( p );
  	if( !KSI_PAIR_P( ksi_pai ))
  		return 0;

		ksi_obj ksi_h = KSI_CAR( ksi_pai );
		ksi_obj ksi_t = KSI_CDR( ksi_pai );

		if( !KSI_KEY_P( ksi_h ) && !KSI_SYM_P( ksi_h ))
  		return 0;
 
		if( !KSI_EINT_P( ksi_t ) && !KSI_STR_P( ksi_t ))
		  return 0;
	}

	return 1;
}
//=====================================================================
int
CConvDlg::CreateIdentList()
{
	m_ParamList = ksi_nil;

	static char szFilter[] = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";

	CFileDialog* fdlg = new CFileDialog( 1, ".txt", 0, OFN_HIDEREADONLY, szFilter );

  int r = fdlg->DoModal();
  CString fName( fdlg->GetPathName());
  delete fdlg;

  if( r != IDOK )
		return 0;

  if( !FileExist( fName ))
  {
    CString m;
    m.Format( "Не найден файл %s", fName );
    ErrBox( m );
    return 0;
  }

  ksi_obj in_port = ksi_open_file( ksi_ptr02string( fName ), ksi_ptr02string( "rt" ));


	for( int tt = 0 ;;  )
  {
    ksi_obj ksi_par = ksi_read( in_port );

    if( KSI_TRUE_P( ksi_eof_object_p( ksi_par )))
      break;

		if( !IsValidPar( ksi_par ))
		{
			ksi_obj ksi_str = ksi_object2string( ksi_par );
			CString m( "Некорректные параметры объекта\n" );
			m += KSI_STR_PTR( ksi_str );
			ErrBox( m );
			return 0;
		}
		

		if( !KSI_PLIST_P( ksi_par ))
			ksi_par = KSI_LIST1( ksi_par );
		m_ParamList = ksi_cons( ksi_par, m_ParamList );
		++tt;
  }
  
  ksi_close_port( in_port );

	return tt;
}
//=====================================================================
int
CConvDlg::DoSetNode()
{
  return IsDlgButtonChecked( IDC_CVD_SET_NODE_CHECK );
}
//=====================================================================
int
CConvDlg::GetNode()
{
  CString s;
  GetDlgItemText( IDC_CVD_NODE_EDIT, s );
  return atoi( s );
}
//=====================================================================
int
CConvDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  RestoreConfig( stdcnfpath );
  OnNumerateButtonClicked();
  OnNumerateButtonClicked();
  GetDlgItem( IDC_CVD_STOP_BUTTON )->EnableWindow( 0 );
  OnConvertCoordClicked();
  SetMsgWndVisible( 0 );
  m_Conversion = 0;

  return 1;
}
//=====================================================================
void 
CConvDlg::Activate()
{
  m_pUpdtr->GetXSize( &m_ProjectSize.cx );
  m_pUpdtr->GetYSize( &m_ProjectSize.cy );
  SetDlgItemInt( IDC_CVD_SIZE_X_EDIT, m_ProjectSize.cx );
  SetDlgItemInt( IDC_CVD_SIZE_Y_EDIT, m_ProjectSize.cy );
  SetMsgWndVisible( 0 );
}
//=====================================================================
void
CConvDlg::OnCancel()
{
  SaveConfig( stdcnfpath );
  ShowWindow( SW_HIDE );
  m_pView->SetFocus();
}
//=====================================================================
void 
CConvDlg::OnConvCTNClicked()
{
  if( IsDlgButtonChecked( IDC_CVD_CONVERT_CLASSES_CHECK ) ||
		  IsDlgButtonChecked( IDC_CVD_CONVERT_TABLES_CHECK ) ||
		  IsDlgButtonChecked( IDC_CVD_CONVERT_NODES_CHECK ))
	CheckDlgButton( IDC_CVD_CONVERT_OBJECTS_CHECK, 0 );
}
//=====================================================================
void 
CConvDlg::OnConvObjButtonClicked()
{
  if( IsDlgButtonChecked( IDC_CVD_CONVERT_OBJECTS_CHECK ))
	{
		CheckDlgButton( IDC_CVD_CONVERT_CLASSES_CHECK, 0 );
		CheckDlgButton( IDC_CVD_CONVERT_TABLES_CHECK, 0 );
		CheckDlgButton( IDC_CVD_CONVERT_NODES_CHECK, 0 );
	}
}
//=====================================================================
void
CConvDlg::OnNumerateButtonClicked()
{
  m_bNumerateFrom = !m_bNumerateFrom;
  GetDlgItem( IDC_CVD_NUMERATE_BUTTON )->SetWindowText( 
    m_bNumerateFrom ? "Первый номер" : "Поле номера" );
}
//=====================================================================
void
CConvDlg::OnConversionComplete()
{
  m_pView->SetKsiHiResolution();

  delete m_Rgn;
  m_Rgn = 0;
  pConwThread = 0;

  GetDlgItem( IDOK )->EnableWindow( 1 );
  GetDlgItem( IDCANCEL )->EnableWindow( 1 );
  GetDlgItem( IDC_CVD_STOP_BUTTON )->EnableWindow( 0 );

  if( m_pUpdtr->GetModified())
    m_pView->ReloadAll();

  Beep( 440, 500 );
  m_Conversion = 0;
}
//=====================================================================
void
CConvDlg::OnFindClassButtonClicked()
{
  CSelectClassesDialog( m_ClassMap, m_pView->m_pObjDef, m_pView ).DoModal();
}
//=====================================================================
void
CConvDlg::OnSelectClassButtonClicked()
{
  CSelectClassDlg dlg( "Выбор класса", m_pView->m_pObjDef );
  dlg.DoModal();
  SetDlgItemInt( IDC_CVD_CLASS_NUMBER_EDIT, dlg.GetClass());
}
//=====================================================================
void
CConvDlg::OnFindTxtCliced()
{
  CFileDialog* fdlg = 
    new CFileDialog( 1, ".txt", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     m_OpNum == PROJECT_TO_MIF || m_OpNum == MIF_TO_PROJECT ?
                     "Mif files (*.mif)|*.mif||" : "Text files (*.txt)|*.txt|All files (*.*)|*.*||",
                     this );
  int r = fdlg->DoModal();
  CString fName( fdlg->GetPathName());
  delete fdlg;

  if( r == IDOK )
    GetDlgItem( IDC_CVD_TXT_PATH_EDIT )->SetWindowText( fName );
}
//=====================================================================
void
CConvDlg::OnFindLogCliced()
{
  CFileDialog* fdlg =
    new CFileDialog( 1, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "Text Files (*.txt)|*.txt||", this );
  int r = fdlg->DoModal();
  CString fName( fdlg->GetPathName());
  delete fdlg;
  if( r == IDOK )
    GetDlgItem( IDC_CVD_LOG_PATH_EDIT )->SetWindowText( fName );
}
//=====================================================================
void 
CConvDlg::OnConvertCoordClicked()
{
  static UINT t[] = { IDC_CVD_AX_EDIT, IDC_CVD_AY_EDIT,
                      IDC_CVD_BX_EDIT, IDC_CVD_BY_EDIT,
                      IDC_CVD_CX_EDIT, IDC_CVD_CY_EDIT };
  int ch = IsDlgButtonChecked( IDC_CVD_CONVERT_COORD_CHECK );
  if( m_OpNum == PROJECT_TO_PROJECT )
    GetDlgItem( IDC_CVD_CREATE_NEW_CHECK )->EnableWindow( !ch );
  if( ch && m_OpNum == PROJECT_TO_MIF  )
	{
    CheckDlgButton( IDC_CVD_OUTPUT_GEO_METR_CHECK, 0 );
    CheckDlgButton( IDC_CVD_OUTPUT_GEO_GRAD_CHECK, 0 );
	}

//  if( ch && m_OpNum == TEXT_TO_PROJECT )
//    CheckDlgButton( IDC_CVD_UTM2M_CHECK, 0 );
  
  for( int i = CELEM( t ) ; --i >= 0 ; GetDlgItem( t[i] )->EnableWindow( ch ));
}
//=====================================================================
void 
CConvDlg::OnUTM2MButtonClicked()
{
  if( IsDlgButtonChecked( IDC_CVD_UTM2M_CHECK ))
    if( CUTMDialog( this ).DoModal())
		{
//      CheckDlgButton( IDC_CVD_CONVERT_COORD_CHECK, 0 );
//			OnConvertCoordClicked();
		}
		else
      CheckDlgButton( IDC_CVD_UTM2M_CHECK, 0 );
}
//=====================================================================
void 
CConvDlg::OnOutputGeoMetrClicked()
{
  if( m_OpNum == PROJECT_TO_MIF && IsDlgButtonChecked( IDC_CVD_OUTPUT_GEO_METR_CHECK ))
	{
    CheckDlgButton( IDC_CVD_OUTPUT_GEO_GRAD_CHECK, 0 );
    CheckDlgButton( IDC_CVD_CONVERT_COORD_CHECK, 0 );
    OnConvertCoordClicked();
	}
}
//=====================================================================
void 
CConvDlg::OnOutputGeoGradClicked()
{
  if( m_OpNum == PROJECT_TO_MIF && IsDlgButtonChecked( IDC_CVD_OUTPUT_GEO_GRAD_CHECK ))
	{
    CheckDlgButton( IDC_CVD_OUTPUT_GEO_METR_CHECK, 0 );
    CheckDlgButton( IDC_CVD_CONVERT_COORD_CHECK, 0 );
    OnConvertCoordClicked();
	}
}
//=====================================================================
void 
CConvDlg::CalcConversion( int bInvers )
{
  int a = m_bRotate ? m_Angle : 0;
  double cx = m_bRotate ? m_CenterX : 0;
  double cy = m_bRotate ? m_CenterY : 0;
  double si = sin( a );
  double co = cos( a );
  double dc = 1 - co;
  double kx = m_bScaleX ? m_ScaleX : 1;
  double ky = m_bScaleY ? m_ScaleY : 1;
  double dx = m_bOffsetX ? m_OffsetX : 0;
  double dy = m_bOffsetY ? m_OffsetY : 0;
  if( bInvers )
  {
    m_Cfs[0][0] = co / kx;
    m_Cfs[0][1] = -si / ky;
    m_Cfs[0][2] = -co * dx / kx + si * dy / ky + cx * dc + cy * si;
    m_Cfs[1][0] = si / kx;
    m_Cfs[1][1] = co / ky;
    m_Cfs[1][2] = -si * dx / kx - co * dy / ky + cy * dc - cx * si;
  }
  else
  {
    m_Cfs[0][0] = co * kx;
    m_Cfs[0][1] = si * kx;
    m_Cfs[0][2] = kx * ( cx * dc - cy * si ) + dx;
    m_Cfs[1][0] = -si * ky;
    m_Cfs[1][1] = co * ky;
    m_Cfs[1][2] = ky * ( cx * si + cy * dc ) + dy;
  }

  static UINT e[][3] = { IDC_CVD_AX_EDIT, IDC_CVD_BX_EDIT, IDC_CVD_CX_EDIT,
                         IDC_CVD_AY_EDIT, IDC_CVD_BY_EDIT, IDC_CVD_CY_EDIT };

  for( int i = 0 ; i < 2 ; ++i )
    for( int j = 0 ; j < 3 ; ++j )
    {
      CString t;
      t.Format( j < 2 ? "%1.10f" : "%1.2f", m_Cfs[i][j] );
      GetDlgItem( e[i][j] )->SetWindowText( t );
    }
}
//=====================================================================
void
CConvDlg::SetConversion()
{
  static UINT e[][3] = { IDC_CVD_AX_EDIT, IDC_CVD_BX_EDIT, IDC_CVD_CX_EDIT,
                         IDC_CVD_AY_EDIT, IDC_CVD_BY_EDIT, IDC_CVD_CY_EDIT };

  for( int i = 0 ; i < 2 ; ++i )
    for( int j = 0 ; j < 3 ; ++j )
    {
      CString t;
			GetDlgItem( e[i][j] )->GetWindowText( t );
      m_Cfs[i][j] = atof( t );
    }
}
//=====================================================================
void 
CConvDlg::OnSetCnvParamClicked()
{
  CConvCoordDlg( this ).DoModal();
  OnConvertCoordClicked();
}
//=====================================================================
void
CConvDlg::OnEditCheckClicked()
{
  static UINT t[][3] =
  {
    IDC_CVD_CONTEXT_PARAM_EDIT,      IDC_CVD_CREATE_IN_CONTEXT_CHECK, 0,
    IDC_CVD_IMPROVE_POLY_PARAM_EDIT, IDC_CVD_IMPROVE_POLY_CHECK,      0,
    IDC_CVD_SUBS_PARAM_EDIT,         IDC_CVD_CREATE_SUBSRIPTS_CHECK,  0,
    IDC_CVD_NODE_EDIT,               IDC_CVD_SET_NODE_CHECK,          0,
    IDC_CVD_MIN_ID_EDIT,             IDC_CVD_FILTER_ID_CHECK,         0,
    IDC_CVD_MAX_ID_EDIT,             IDC_CVD_FILTER_ID_CHECK,         0,
    IDC_CVD_TABLE_EDIT,              IDC_CVD_FILTER_ID_CHECK,         IDC_CVD_MIF_TO_PROJECT_RADIO,
    IDC_CVD_DECOMPOSE_PARAM_EDIT,    IDC_CVD_SEPARATE_POLY_CHECK,     0,
    IDC_CVD_KEY_FIELD_EDIT,          IDC_CVD_SELECT_BY_KEY_CHECK,     0,
    IDC_CVD_KEY_STRING_EDIT,         IDC_CVD_SELECT_BY_KEY_CHECK,     0
  };

  for( int i = CELEM( t ) ; --i >= 0 ;
       GetDlgItem( t[i][0] )->EnableWindow(
         GetDlgItem( t[i][1] )->IsWindowEnabled() && IsDlgButtonChecked( t[i][1] ) ||
         t[i][2] && IsDlgButtonChecked( t[i][2] )));
}
//=====================================================================
void
CConvDlg::OnTextToProjectClicked()
{
  EnDsDlgItems( TEXT_TO_PROJECT );
}
//=====================================================================
void
CConvDlg::OnProjectToProjectClicked()
{
  EnDsDlgItems( PROJECT_TO_PROJECT );
}
//=====================================================================
void
CConvDlg::OnProjectToTextClicked()
{
  EnDsDlgItems( PROJECT_TO_TEXT );
}
//=====================================================================
void
CConvDlg::OnProjectToMifClicked()
{
  EnDsDlgItems( PROJECT_TO_MIF );
}
//=====================================================================
void
CConvDlg::OnMifToProjectClicked()
{
  EnDsDlgItems( MIF_TO_PROJECT );
}
//=====================================================================
void
CConvDlg::EnDsDlgItems( int opNum )
{
  static UINT edm[][6] =
  {
// IDC_MIF_TO_PROJECT_RADIO     .....................
// IDC_PROJECT_TO_MIF_RADIO     ..................  :
// IDC_PROJECT_TO_TEXT_RADIO    ...............  :  :
// IDC_PROJECT_TO_PROJECT_RADIO ............  :  :  :
// IDC_TEXT_TO_PROJECT_RADIO    .........  :  :  :  :
//                                      :  :  :  :  :
    IDC_CVD_TXT_PATH_EDIT,              1, 1, 1, 1, 1,
    IDC_CVD_FIND_TXT_BUTTON,            1, 1, 1, 1, 1,
    IDC_CVD_LOG_PATH_EDIT,              1, 1, 0, 0, 1,
    IDC_CVD_FIND_LOG_BUTTON,            1, 1, 0, 0, 1,
    IDC_CVD_SIZE_X_EDIT,                1, 1, 0, 0, 1,
    IDC_CVD_SIZE_Y_EDIT,                1, 1, 0, 0, 1,
    IDC_CVD_CREATE_NEW_CHECK,           1, 1, 0, 0, 1,
    IDC_CVD_SQUIZZE_CHECK,              0, 1, 0, 0, 0,
    IDC_CVD_CONVERT_CLASSES_CHECK,      1, 0, 0, 0, 0,
    IDC_CVD_CONVERT_TABLES_CHECK,       1, 0, 0, 0, 0,
    IDC_CVD_CONVERT_NODES_CHECK,        1, 0, 0, 0, 0,
    IDC_CVD_REPLACE_TABLE_RADIO,        1, 0, 0, 0, 1,
    IDC_CVD_IGNORE_TABLE_RADIO,         1, 0, 0, 0, 1,
    IDC_CVD_CREATE_NEW_TABLE_RADIO,     1, 0, 0, 0, 1,
    IDC_CVD_IMPROVE_POLY_CHECK,         1, 0, 0, 0, 1,
    IDC_CVD_REMOVE_CONVERTED_CHECK,     0, 0, 1, 0, 0,
    IDC_CVD_OUTPUT_GEO_METR_CHECK,      0, 0, 1, 1, 0,
    IDC_CVD_OUTPUT_GEO_GRAD_CHECK,      0, 0, 1, 1, 0,
    IDC_CVD_REMOVE_DUPLICATES_CHECK,    1, 0, 0, 0, 1,
    IDC_CVD_CONVERT_OBJECTS_CHECK,      1, 1, 0, 0, 0,
    IDC_CVD_USE_REGION_CHECK,           0, 1, 1, 1, 0,
    IDC_CVD_CONVERT_DELETED_CHECK,      0, 0, 1, 1, 0,
    IDC_CVD_INCLUDE_GRAPH_CLASSES_CHECK,1, 1, 1, 1, 0,
    IDC_CVD_FIND_GRAPH_CLASSES_BUTTON,  1, 1, 1, 1, 0,
    IDC_CVD_SELECT_CLASS_BUTTON,        0, 0, 0, 0, 1,
    IDC_CVD_CREATE_IN_CONTEXT_CHECK,    1, 0, 0, 0, 1,
    IDC_CVD_CONVERT_COORD_CHECK,        1, 1, 1, 1, 1,
    IDC_CVD_CLASS_NUMBER_EDIT,          0, 0, 0, 0, 1,
    IDC_CVD_STATUS_NUMBER_EDIT,         0, 0, 0, 0, 1,
    IDC_CVD_TABLE_EDIT,                 0, 1, 1, 1, 1,
    IDC_CVD_ENTRY_NUMBER_EDIT,          0, 0, 0, 0, 1,
    IDC_CVD_CREATE_SUBSRIPTS_CHECK,     0, 0, 0, 0, 1,
    IDC_CVD_SUBS_PARAM_EDIT,            0, 0, 0, 0, 1,
    IDC_CVD_SEPARATE_POLY_CHECK,        0, 0, 0, 0, 1,
    IDC_CVD_DECOMPOSE_PARAM_EDIT,       0, 0, 0, 0, 1,
    IDC_CVD_ASSAMBLY_BY_CLASS_RADIO,    0, 0, 1, 1, 0,
    IDC_CVD_ASSAMBLY_BY_TABLE_RADIO,    0, 0, 1, 1, 0,
    IDC_CVD_ASSAMBLY_ALL_RADIO,         0, 0, 1, 1, 0,
    IDC_CVD_MOVE_TO_PROJECT_CHECK,      0, 1, 1, 0, 0,
    IDC_CVD_SET_NODE_CHECK,             1, 1, 0, 0, 1,
    IDC_CVD_NODE_EDIT,                  1, 1, 0, 0, 1,
    IDC_CVD_FILTER_ID_CHECK,            0, 1, 1, 1, 0,
    IDC_CVD_MIN_ID_EDIT,                0, 1, 1, 1, 0,
    IDC_CVD_MAX_ID_EDIT,                0, 1, 1, 1, 0,
    IDC_CVD_SELECT_BY_DATE_CHECK,       0, 1, 1, 1, 0,
    IDC_CVD_SELECT_BY_IDENT_CHECK,      1, 1, 1, 1, 0,
    IDC_CVD_SELECT_BY_KEY_CHECK,        0, 0, 0, 0, 1,
		IDC_CVD_UTM2M_CHECK,                1, 0, 0, 0, 0
  };

  m_OpNum = opNum;

  for( int i = CELEM( edm ) ; --i >= 0 ;
       GetDlgItem( edm[i][0] )->EnableWindow( edm[i][opNum+1] ));

  OnConvObjButtonClicked();
  OnSelectDateButtonClicked();
  OnEditCheckClicked();

  if( m_OpNum == PROJECT_TO_PROJECT )
    GetDlgItem( IDC_CVD_CREATE_NEW_CHECK )->EnableWindow( !IsDlgButtonChecked( IDC_CVD_CONVERT_COORD_CHECK ) ||
                                                          !GetDlgItem( IDC_CVD_CONVERT_COORD_CHECK )->IsWindowEnabled());
}
//=====================================================================
void
CConvDlg::OnSelectDateButtonClicked()
{ 
  UINT id1[] = { IDC_CVD_SELECT_DATE_BUTTON1,
                 IDC_CVD_SELECT_DATE_BUTTON2, 0 };
  int bEnb = GetDlgItem( IDC_CVD_SELECT_BY_DATE_CHECK )->IsWindowEnabled() &&
             IsDlgButtonChecked( IDC_CVD_SELECT_BY_DATE_CHECK );
  for( UINT* p1 = id1 ; *p1 ; GetDlgItem( *p1++ )->EnableWindow( bEnb ));
}
//=====================================================================
CCalendarDlg::CCalendarDlg() : 
  CDialog( IDD_CALENDAR_DIALOG, 0 )
{}
//=====================================================================
void
CCalendarDlg::OnOK()
{
  ((CMonthCalCtrl*)GetDlgItem( IDC_CTRL ))->GetCurSel( &m_DateTime );
  EndDialog( 1 );
}
//=====================================================================
void
CConvDlg::OnDate1ButtonClicked()
{
  CCalendarDlg dlg;
  if( dlg.DoModal() == IDOK )
  {
    CString st;
    st.Format( "%02d.%02d.%04d", dlg.m_DateTime.wDay, dlg.m_DateTime.wMonth, dlg.m_DateTime.wYear );
    GetDlgItem( IDC_CVD_SELECT_BY_DATE_EDIT1 )->SetWindowText( st );
    dlg.m_DateTime.wHour = 0;
    dlg.m_DateTime.wMinute = 0;
    dlg.m_DateTime.wSecond = 0;
    dlg.m_DateTime.wMilliseconds = 0;
    m_Time1 = CTime( dlg.m_DateTime );
  }
}
//=====================================================================
void
CConvDlg::OnDate2ButtonClicked()
{
  CCalendarDlg dlg;
  if( dlg.DoModal() == IDOK )
  {
    CString st;
    st.Format( "%02d.%02d.%04d", dlg.m_DateTime.wDay, dlg.m_DateTime.wMonth, dlg.m_DateTime.wYear );
    GetDlgItem( IDC_CVD_SELECT_BY_DATE_EDIT2 )->SetWindowText( st );
    dlg.m_DateTime.wHour = 23;
    dlg.m_DateTime.wMinute = 59;
    dlg.m_DateTime.wSecond = 59;
    dlg.m_DateTime.wMilliseconds = 999;
    m_Time2 = CTime( dlg.m_DateTime );
  }
}
//=====================================================================
void
CConvDlg::SetMsgWndVisible( int bVsbl )
{
  UINT id1[] = { IDC_CVD_TOTAL_STATIC, IDC_CVD_WRITTEN_STATIC, 
                 IDC_CVD_ERRORS_STATIC, IDC_CVD_TOTAL_EDIT, 
                 IDC_CVD_WRITTEN_EDIT, IDC_CVD_ERRORS_EDIT, 0 };

  for( UINT* p1 = id1 ; *p1 ;
       GetDlgItem( *p1++ )->ShowWindow( bVsbl ? SW_SHOW : SW_HIDE ));
}
//=====================================================================
void
CConvDlg::OnStopClicked()
{
  m_Stop = 1;
  m_Stop = AskBox( "Прекратить конвертацию?" ) == IDYES ? 2 : 0;
}
//=====================================================================
void
CConvDlg::SetReport( int total, int written, int errors, int bNow )
{ 
  m_total   = total;
  m_written = written;
  m_errors  = errors;
  if( bNow )
    m_nRep = 0;

  if( !m_nRep )
  {
    m_nRep = 1;
    m_repType = 0;
    PostMessage( IDM_CONVERTOR_REPORT );
  }
}
//=====================================================================
void
CConvDlg::SetDecomp( int rest, int wrtn )
{
  m_decomp_rst = rest;
  m_decomp_wrt = wrtn;
  
  if( !m_decomp_rst && !m_decomp_wrt )
    m_nRep = 0;

  if( !m_nRep )
  {
    m_nRep = 1;
    m_repType = 1;
    PostMessage( IDM_CONVERTOR_REPORT );
  }
}
//=====================================================================
void
CConvDlg::Report( UINT, LONG )
{
  static struct rd { UINT sid; UINT eid; int* pVal; } sa[] = 
  {
    IDC_CVD_TOTAL_STATIC,   IDC_CVD_TOTAL_EDIT,   &m_total,
    IDC_CVD_WRITTEN_STATIC, IDC_CVD_WRITTEN_EDIT, &m_written,
    IDC_CVD_ERRORS_STATIC,  IDC_CVD_ERRORS_EDIT,  &m_errors
  };

  if( m_repType == 0 )
  {
    for( rd* pr = sa + CELEM( sa ) ; --pr >= sa ; )
    {
      int bv = *pr->pVal != 0;
      if( bv )
        SetDlgItemInt( pr->eid, *pr->pVal );
      if( GetDlgItem( pr->sid )->IsWindowVisible() ^ bv )
      {
        GetDlgItem( pr->sid )->ShowWindow( bv );
        GetDlgItem( pr->eid )->ShowWindow( bv );
      }
    }
  }
  else
  {
    CWnd* pWnd = GetDlgItem( IDC_CVD_DECOMPOSE_TEXT );

    if( !m_decomp_rst && !m_decomp_wrt )
      pWnd->ShowWindow( 0 );
    else
    {
      CString s;
      s.Format( "Декомпозиция:    %d    %d", m_decomp_wrt, m_decomp_rst );
      pWnd->SetWindowText( s );
      pWnd->ShowWindow( 1 );
    }  
  }
   m_nRep = 0;
}
//=====================================================================
char* 
CConvDlg::GetMap()
{
  return IsDlgButtonChecked( IDC_CVD_INCLUDE_GRAPH_CLASSES_CHECK ) ?
    m_ClassMap : 0;
}
//=====================================================================
void
CConvDlg::OnOK()
{
  SetMsgWndVisible( 0 );

  static UINT mids[][2] =
  {
    IDC_CVD_CREATE_NEW_CHECK,        CM_REMOVE_ALL,
    IDC_CVD_SQUIZZE_CHECK,           CM_SQUIZZE, 
    IDC_CVD_CONVERT_CLASSES_CHECK,   CM_CLASS_CONVERT,
    IDC_CVD_CONVERT_TABLES_CHECK,    CM_TABLE_CONVERT,
    IDC_CVD_CONVERT_NODES_CHECK,     CM_NODE_CONVERT,
    IDC_CVD_REPLACE_TABLE_RADIO,     CM_CHANGE,
    IDC_CVD_IGNORE_TABLE_RADIO,      CM_IGNORE,
    IDC_CVD_CREATE_NEW_TABLE_RADIO,  CM_CREATE_NEW,
    IDC_CVD_IMPROVE_POLY_CHECK,      CM_IMPROVE,
    IDC_CVD_REMOVE_CONVERTED_CHECK,  CM_REMOVE,
    IDC_CVD_OUTPUT_GEO_METR_CHECK,   CM_OUTPUT_GEO_METR,
    IDC_CVD_OUTPUT_GEO_GRAD_CHECK,   CM_OUTPUT_GEO_GRAD,
    IDC_CVD_CONVERT_OBJECTS_CHECK,   CM_OBJECTS_CONVERT,
    IDC_CVD_USE_REGION_CHECK,        CM_SPACE_SELECT,
    IDC_CVD_CONVERT_DELETED_CHECK,   CM_RESTORE,
    IDC_CVD_CREATE_IN_CONTEXT_CHECK, CM_AUTOBUILD,
    IDC_CVD_CONVERT_COORD_CHECK,     CM_COORD_CONVERT,
		IDC_CVD_UTM2M_CHECK,             CM_UTM_CONVERT,
    IDC_CVD_SEPARATE_POLY_CHECK,     CM_SEPARATE_POLY,
    IDC_CVD_ASSAMBLY_BY_CLASS_RADIO, CM_ASSAMBLY_BY_CLASS,
    IDC_CVD_ASSAMBLY_BY_TABLE_RADIO, CM_ASSAMBLY_BY_TABLE,
    IDC_CVD_ASSAMBLY_ALL_RADIO,      CM_ASSAMBLY_ALL,
    IDC_CVD_MOVE_TO_PROJECT_CHECK,   CM_MOVE,
    IDC_CVD_SET_NODE_CHECK,          CM_SET_NODE,
    IDC_CVD_REMOVE_DUPLICATES_CHECK, CM_REMOVE_DUPLICATES,
    IDC_CVD_FILTER_ID_CHECK,         CM_FLTER_ID_DIAP,
    IDC_CVD_SELECT_BY_IDENT_CHECK,   CM_FLTER_ID_LIST,
    IDC_CVD_SELECT_BY_KEY_CHECK,     CM_KEY_SELECT,
    IDC_CVD_SELECT_BY_DATE_CHECK,    CM_TIME_SELECT
  };

  m_Mode = m_bNumerateFrom ? CM_NUMERATE_FROM : 0;

  for( int i = CELEM( mids ) ; --i >= 0 ; )
    if( GetDlgItem( mids[i][0] )->IsWindowEnabled() &&
        IsDlgButtonChecked( mids[i][0] ))
      m_Mode |= mids[i][1];

  if( GetDlgItem( IDC_CVD_TXT_PATH_EDIT )->IsWindowEnabled() &&
      ( m_OpNum != PROJECT_TO_PROJECT || ( m_Mode & CM_MOVE )) &&
      !GetDlgItemText( IDC_CVD_TXT_PATH_EDIT, m_TxtPath, sizeof m_TxtPath ))
  {
    ErrBox( "Не определен текстовый файл!" );
    return;
  }

  if( GetDlgItem( IDC_CVD_LOG_PATH_EDIT )->IsWindowEnabled() &&
      !GetDlgItemText( IDC_CVD_LOG_PATH_EDIT, m_LogPath, sizeof m_LogPath ))
  {
    ErrBox( "Не определен файл ошибок!" );
    return;
  }

  if( m_Mode & CM_TIME_SELECT &&
     ( m_OpNum == PROJECT_TO_PROJECT || 
       m_OpNum == PROJECT_TO_TEXT || 
       m_OpNum == PROJECT_TO_MIF ))
  {
    long vers;
    m_pUpdtr->GetVersion( &vers );
    char const* pm =
      vers < 103 ? "Данный проект не поддерживает\nдату создания объекта!" :
      m_Time1 == 0 ? "Не задано начало периода!" :
      m_Time2 == 0 ? "Не задан конец периода!" :
      m_Time1 > m_Time2 ? "Некорректный период!" : 0;
    if( pm )
    {
      ErrBox( pm );
      return;
    }
  }

  TCHAR b[256];
  m_classNumber  = GetDlgItemInt( IDC_CVD_CLASS_NUMBER_EDIT );
  m_statusNumber = GetDlgItemInt( IDC_CVD_STATUS_NUMBER_EDIT );
  m_tableNumber  = GetDlgItemInt( IDC_CVD_TABLE_EDIT );
  m_entryNumber  = GetDlgItemInt( IDC_CVD_ENTRY_NUMBER_EDIT );
   
  m_minIdent     = GetDlgItemInt( IDC_CVD_MIN_ID_EDIT );
  m_maxIdent     = GetDlgItemInt( IDC_CVD_MAX_ID_EDIT ) + 1;
  if( m_OpNum == MIF_TO_PROJECT )
  {
    int bk = m_Mode & CM_KEY_SELECT;
    char const* pm =
      bk && !GetDlgItemText( IDC_CVD_KEY_FIELD_EDIT, b, sizeof b ) ?
        "Не определено поле ключа!" :
      bk && !atoi( b ) ?
        "Некорректное поле ключа!" :
      bk && !GetDlgItemText( IDC_CVD_KEY_STRING_EDIT, m_KeyPar ) ?
        "Не определен ключ!" :
      !GetDlgItemText( IDC_CVD_CLASS_NUMBER_EDIT, b, sizeof b ) ?
        "Не определен номер класса!" :
      m_tableNumber && !GetDlgItemText( IDC_CVD_ENTRY_NUMBER_EDIT, b, sizeof b ) ?
        "Не определено начало нумерации!" :
      m_classNumber <= 0 || m_classNumber >= FULL_RANGE ?
        "Недопустимый номер класса!" :
      m_tableNumber < 0 ?
        "Недопустимый номер таблицы!" :
      m_tableNumber && m_entryNumber <= 0 ?
        "Недопустимое начало нумерации!" : 0;
    if( pm )
    {
      ErrBox( pm );
      return;
    }
  }

  m_RgnSize = 0;
  m_Rgn = 0;

  m_Box.SetRect();
  if( m_Mode & CM_SPACE_SELECT )
  {
    CString path( App.GetProjectFilePath( "region.txt" ));
    if( !FileExist( path ))
    {
      ErrBox( "Не найден файл описания региона!" );
      return;
    }

    for( int step = 2 ; --step >= 0 ; )
    {
      m_Rgn = step ? 0 : new Point[m_RgnSize+1];
      Point* pp = m_Rgn;

      std::ifstream rin( path, ios::in|ios::nocreate );
      if( rin.bad())
      {
        ErrBox( "Не удается открыть файл описания региона!" );
        return;
      }

      for(;;)
      {
        char ttl[256];
        int nr, rs;
        rin >> ttl;
        
        if( rin.eof())
          break;
        
        rin >> nr >> rs;

        if( rin.eof() || stricmp( ttl, "Region" ) || nr != 1 || rs < 3 )
        {
          ErrBox( "Некорректный регион!" );
          return;
        }
        
        if( step )
          m_RgnSize += rs + 1;
        else
          *pp++ = Point( rs, 0 );

        
        while( --rs >= 0 )
        {
          Point p;
          if( rin.eof())
          {
            ErrBox( "Некорректный регион!" );
            return;
          }
          double x, y;
          rin >> x >> y;
          p.x = x * 100;
          p.y = y * 100;
          if( !step )
          {
            *pp++ = p;
            m_Box |= p;
          }
        }
      }
      if( !step )
        *pp = Point( 0 , 0 );
      else
        rin.close();
    }
  }

  int dr = m_OpNum == PROJECT_TO_TEXT || m_OpNum == PROJECT_TO_MIF ? IDNO :
           CCopyProjectDlg::DoShow() ? CCopyProjectDlg().DoModal() : 
           CCopyProjectDlg::LastRep();
  int bReservCopy = 0;

  if( dr == IDCANCEL )
    return;

  if( dr == IDYES )
  {
    CWaitCursor wc;
    ReportHint( "Копирование..." );
    bReservCopy = m_pUpdtr->Backup() == R_OK;
    ClearHint();
    if( !bReservCopy && AskBox( "Не удалось создать резерную копию!\n\nПродолжить?" ) != IDYES )
      return;
  }

  if( !bReservCopy && ( m_Mode & CM_REMOVE_ALL ))
    switch( AskBox( "Вы действительно хотите\nперезаписать существующий проект?", 1 ))
    {
      case IDCANCEL:
        return;
      case IDYES: 
        m_pView->ClearSelection( 0, 0 );
        pView->m_ClipData.ClipObjects( 0, 0 );
        m_pUpdtr->RemoveAll();
    }

  long vers = 0;
  m_pUpdtr->GetVersion( &vers );

  if( m_Mode & CM_IMPROVE )
  {
    m_Imp = GetDlgItemInt( IDC_CVD_IMPROVE_POLY_PARAM_EDIT );
    if( !m_Imp )
      m_Imp = 50;
    SetDlgItemInt( IDC_CVD_IMPROVE_POLY_PARAM_EDIT, m_Imp );
    ksi_defsym( "max-delta", ksi_int2num( m_Imp ), ksi_current_env());
  }

  if( m_Mode & CM_AUTOBUILD )
  {
    m_Ssd = GetDlgItemInt( IDC_CVD_CONTEXT_PARAM_EDIT );
    if( !m_Ssd )
      m_Ssd = 100;
    SetDlgItemInt( IDC_CVD_CONTEXT_PARAM_EDIT, m_Ssd );
    int dsd = m_Ssd * 100;
    ksi_defsym( "auto-build-param", ksi_int2num( dsd ), ksi_current_env());
  }

  m_pView->ClearSelection( 0, 0 );
  m_pView->m_Undo.RemoveAll();


  if( GetDlgItem( IDC_CVD_CREATE_SUBSRIPTS_CHECK )->IsWindowEnabled() &&
      IsDlgButtonChecked( IDC_CVD_CREATE_SUBSRIPTS_CHECK ))
    GetDlgItemText( IDC_CVD_SUBS_PARAM_EDIT, m_SubsPar );
  else
    m_SubsPar = "";
  
  SetKsiHiResolutionX( 1 );
  TestProjectSize();
	SetConversion();

  SetReport( 0, 0, 0, 1 );

  if( m_Mode & CM_FLTER_ID_LIST )
	{
		int ni = CreateIdentList();
		CString m;
		m.Format( "Идентификаторов в списке: %d.", ni );
		if( ni )
			InfBox( m );
		else
		if( AskBox( m + "\nПродолжить?" ) != IDYES )
			return;
	}

  m_Stop = 0;
  GetDlgItem( IDOK )->EnableWindow( 0 );
  GetDlgItem( IDCANCEL )->EnableWindow( 0 );
  GetDlgItem( IDC_CVD_STOP_BUTTON )->EnableWindow( !( m_Mode & CM_SQUIZZE ));

  pConwThread = new CConwThread( this );
  pConwThread->CreateThread();
  m_Conversion = 1;
}
//=====================================================================
int
CConvDlg::TestProjectSize()
{
  CSize ns( GetDlgItemInt( IDC_CVD_SIZE_X_EDIT ),
            GetDlgItemInt( IDC_CVD_SIZE_Y_EDIT ));
  
  CMapDoc* pDoc = m_pView->GetDocument();

  if( ns != pDoc->GetSize() &&
      AskBox( "Размер проекта изменен!\nПрименить новый размер?" ) == IDYES )
  {
    pDoc->SetSize( m_ProjectSize = ns );
    m_pView->AddMode( new CBoundingMode( ns.cx, ns.cy ));
    m_pUpdtr->SetSize( ns );
  }
  SetDlgItemInt( IDC_CVD_SIZE_X_EDIT, m_ProjectSize.cx );
  SetDlgItemInt( IDC_CVD_SIZE_Y_EDIT, m_ProjectSize.cy );
  return 1;
}
//=====================================================================
void
CConvDlg::OnSaveClicked()
{
  CFileDialog* fdlg = 
    new CFileDialog( 1, ".cfg", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "Config files (*.cfg)|*.cfg||", this );

  int r = fdlg->DoModal();
  CString fName( fdlg->GetPathName());
  delete fdlg;

  if( r == IDOK )
    SaveConfig( fName );
}
//=====================================================================
void
CConvDlg::OnRestoreClicked()
{
  CFileDialog* fdlg = 
    new CFileDialog( 1, ".cfg", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "Config files (*.cfg)|*.cfg||", this );
  int r = fdlg->DoModal();
  CString fName( fdlg->GetPathName());
  delete fdlg;

  if( r == IDOK )
    RestoreConfig( fName );
}
//=====================================================================
void
CConvDlg::SaveConfig( LPCTSTR path )
{
  for( CfgRec* pcf = ControlConfigTab ; pcf->type != NOTHING ; ++pcf )
  {
    CString tmp, t;
    switch( pcf->type )
    {
      case CHECK :
        tmp = IsDlgButtonChecked( pcf->id ) ? "1" : "0";
        break;
      case EDIT :
        GetDlgItemText( pcf->id, tmp );
        break;
      case MAP :
        tmp = "";
        for( int i = 1 ; i < CLASS_RANGE ; ++i )
          if( m_ClassMap[i>>3] & ( 1 << ( i & 7 )))
          {
            t.Format( "%d;", i );
            tmp += t;
          }
        break;
    }
    ::WritePrivateProfileString( "config", pcf->entry, tmp, path );
  }
  ::WritePrivateProfileString( "config", "NUMERATE_MODE", m_bNumerateFrom ? "1" : "0", path );

  for( DatRec* pdr = DataConfigTab ; pdr->type != NOTHING ; ++pdr )
  {
    CString tmp;
    switch( pdr->type )
    {
      case IBOOL :
        tmp = *((int*)pdr->pData ) ? "1" : "0";
        break;
      case IINT :
      case ITIME :
        tmp.Format( "%d", *((int*)pdr->pData ));
        break;
      case IREAL :
				if( pdr->id )
					GetDlgItemText( pdr->id, tmp );
				else
          tmp.Format( "%1.10f", *((double*)pdr->pData ));
        break;
    }
    ::WritePrivateProfileString( "config", pdr->entry, tmp, path );
  }
}
//=====================================================================
void
CConvDlg::RestoreConfig( LPCTSTR path )
{
  memset( m_ClassMap, 0, sizeof m_ClassMap );

  char tmp[1024];
  int n = 0, c = 0;
  int ch;

  for( CfgRec* pcf = ControlConfigTab ; pcf->type != NOTHING ; ++pcf, ++n )
  {
    ::GetPrivateProfileString( "config", pcf->entry, "", tmp, sizeof tmp, path );
    switch( pcf->type )
    {
      case CHECK :
        ch = tmp[0] == '1';
        CheckDlgButton( pcf->id, ch );
        if( n < 5 && ch )
					c = n;
        break;
      case EDIT :
        SetDlgItemText( pcf->id, tmp );
        break;
      case MAP :
        for( char* p = tmp ; p && *p ; )
        {
          int t = atoi( p );
          m_ClassMap[t>>3] |= ( 1 << ( t & 7 ));
          p = strchr( p, ';' );
          p += p != 0;
        }
        break;
    }
  }

  ::GetPrivateProfileString( "config", "NUMERATE_MODE", "1", tmp, sizeof tmp, path );
  m_bNumerateFrom = atoi( tmp );

  for( DatRec* pdr = DataConfigTab ; pdr->type != NOTHING ; ++pdr )
  {
    ::GetPrivateProfileString( "config", pdr->entry, "", tmp, sizeof tmp, path );
		double fv;
    switch( pdr->type )
    {
      case IBOOL :
        *((int*)pdr->pData ) = tmp[0] == '1';
        break;
      case ITIME :
      case IINT :
        *((int*)pdr->pData ) = atoi( tmp );
        break;
      case IREAL :
				fv = *((double*)pdr->pData ) = atof( tmp );
				if( pdr->id )
          SetDlgItemText( pdr->id, tmp );
        break;
    }
  }
  
	static UINT AC[] = { IDC_CVD_ASSAMBLY_BY_CLASS_RADIO,
                       IDC_CVD_ASSAMBLY_BY_TABLE_RADIO,
                       IDC_CVD_ASSAMBLY_ALL_RADIO };

  for( int i = CELEM( AC ) ; --i >= 0 && !IsDlgButtonChecked( AC[i] ) ; );
  if( i < 0 )
    CheckDlgButton( PROJECT_TO_PROJECT, 1 );
  
  static UINT RC[] = { IDC_CVD_TEXT_TO_PROJECT_RADIO, 
                       IDC_CVD_PROJECT_TO_PROJECT_RADIO,
                       IDC_CVD_PROJECT_TO_TEXT_RADIO,   
                       IDC_CVD_PROJECT_TO_MIF_RADIO,    
                       IDC_CVD_MIF_TO_PROJECT_RADIO };    

  for( i = CELEM( AC ) ; --i >= 0 && !IsDlgButtonChecked( AC[i] ) ; );
  if( i < 0 )
    CheckDlgButton( IDC_CVD_ASSAMBLY_ALL_RADIO, 1 );

  EnDsDlgItems( c );
  OnNumerateButtonClicked();
  OnNumerateButtonClicked();
}
//=====================================================================
//=====================================================================
BEGIN_MESSAGE_MAP( CConvCoordDlg, CDialog )
  ON_BN_CLICKED( IDC_CCD_SCALE_X_CHECK,  OnChangeMode )
  ON_BN_CLICKED( IDC_CCD_SCALE_Y_CHECK,  OnChangeMode )
  ON_BN_CLICKED( IDC_CCD_OFFSET_X_CHECK, OnChangeMode )
  ON_BN_CLICKED( IDC_CCD_OFFSET_Y_CHECK, OnChangeMode )
  ON_BN_CLICKED( IDC_CCD_ROTATE_CHECK,   OnChangeMode )
  ON_BN_CLICKED( IDC_CCD_INVERSE_BTN,    OnInvers     )
END_MESSAGE_MAP()
//=====================================================================
int
CConvCoordDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  int const mi = 0x80000000;
  int const ma = 0x7fffffff;

  static CSpinIni FS[] =
  {
    { IDC_CCD_ANGLE_EDIT, IDC_CCD_ANGLE_SPIN, &CConvDlg::m_Angle, -1800, 10, 1800, 1, 10, "%1.1f" }
  };
  static CRealSpinIni RS[] =
  {
    { IDC_CCD_SCALE_X_EDIT,  IDC_CCD_SCALE_X_SPIN,  &CConvDlg::m_ScaleX,  -1e10, 1e2, 1e10, "%1.4f" },
    { IDC_CCD_SCALE_Y_EDIT,  IDC_CCD_SCALE_Y_SPIN,  &CConvDlg::m_ScaleY,  -1e10, 1e2, 1e10, "%1.4f" },
    { IDC_CCD_OFFSET_X_EDIT, IDC_CCD_OFFSET_X_SPIN, &CConvDlg::m_OffsetX, -1e16, 1e2, 1e16, "%1.0f" },
    { IDC_CCD_OFFSET_Y_EDIT, IDC_CCD_OFFSET_Y_SPIN, &CConvDlg::m_OffsetY, -1e16, 1e2, 1e16, "%1.0f" },
    { IDC_CCD_CENTER_X_EDIT, IDC_CCD_CENTER_X_SPIN, &CConvDlg::m_CenterX, -1e16, 1e2, 1e16, "%1.0f" },
    { IDC_CCD_CENTER_Y_EDIT, IDC_CCD_CENTER_Y_SPIN, &CConvDlg::m_CenterY, -1e16, 1e2, 1e16, "%1.0f" }
  };

  int i;

  for( i = CELEM( RS ) ; --i >= 0 ; m_rEdit[i].Initialize( RS[i], this ));
  for( i = CELEM( FS ) ; --i >= 0 ; m_fEdit[i].Initialize( FS[i], this ));
  OnChangeMode();
  return 1;
}
//=====================================================================
void 
CConvCoordDlg::DoDataExchange( CDataExchange* pDX )
{
  DDX_Check( pDX, IDC_CCD_OFFSET_X_CHECK, CConvDlg::m_bOffsetX );
  DDX_Check( pDX, IDC_CCD_OFFSET_Y_CHECK, CConvDlg::m_bOffsetY );
  DDX_Check( pDX, IDC_CCD_SCALE_X_CHECK,  CConvDlg::m_bScaleX );
  DDX_Check( pDX, IDC_CCD_SCALE_Y_CHECK,  CConvDlg::m_bScaleY );
  DDX_Check( pDX, IDC_CCD_ROTATE_CHECK,   CConvDlg::m_bRotate );
}
//=====================================================================
int
CConvCoordDlg::SaveData( int bInvers )
{
  int i;
  for( i = CELEM( m_fEdit ) ; --i >= 0 ; m_fEdit[i].SetValue());
  for( i = CELEM( m_rEdit ) ; --i >= 0 ; m_rEdit[i].SetValue());

  int sx = IsDlgButtonChecked( IDC_CCD_SCALE_X_CHECK );
  int sy = IsDlgButtonChecked( IDC_CCD_SCALE_Y_CHECK );

  if( sx && CConvDlg::m_ScaleX == 0 || sy && CConvDlg::m_ScaleY == 0 )
  {
    CString m( "Вырожденное преобразование по оси " );
    ErrBox( m + ( sx ? "X" : "Y" ));
    return 0;
  }  
	
	UpdateData( 1 );
  m_pCnvDlg->CalcConversion( bInvers );
  return 1;
}
//=====================================================================
void 
CConvCoordDlg::OnOK()
{
  if( SaveData( 0 ))
    EndDialog( IDOK );
}
//=====================================================================
void
CConvCoordDlg::OnInvers()
{
  if( SaveData( 1 ))
    EndDialog( IDOK );
}
//=====================================================================
void 
CConvCoordDlg::OnChangeMode()
{
  static UINT m[][2] =
  {
    IDC_CCD_SCALE_X_CHECK,  IDC_CCD_SCALE_X_EDIT,
    IDC_CCD_SCALE_Y_CHECK,  IDC_CCD_SCALE_Y_EDIT,
    IDC_CCD_OFFSET_X_CHECK, IDC_CCD_OFFSET_X_EDIT,
    IDC_CCD_OFFSET_Y_CHECK, IDC_CCD_OFFSET_Y_EDIT,
    IDC_CCD_ROTATE_CHECK,   IDC_CCD_CENTER_X_EDIT,
    IDC_CCD_ROTATE_CHECK,   IDC_CCD_CENTER_Y_EDIT,
    IDC_CCD_ROTATE_CHECK,   IDC_CCD_ANGLE_EDIT
  };
  for( int i = CELEM( m ) ; --i >= 0 ; )
    GetDlgItem( m[i][1] )->EnableWindow( IsDlgButtonChecked( m[i][0] ));
}
//=====================================================================
//=====================================================================
int CFindObjectDlg::m_Table = 0;
int CFindObjectDlg::m_Entry = 0;
//=====================================================================
void 
CFindObjectDlg::DoDataExchange( CDataExchange* pDX )
{
  DDX_Text( pDX, IDC_FOD_TABLE_EDIT, m_Table );
  DDX_Text( pDX, IDC_FOD_ENTRY_EDIT, m_Entry );
}
//=====================================================================
void 
CFindObjectDlg::OnOK()
{
  UpdateData( 1 );
  m_pView->FindObject( CTabIdent( m_Table, m_Entry ), 1, 1 );
}
//=====================================================================
int  
CFindObjectDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  UpdateData( 0 );
  return 1;
}
//=====================================================================
static struct { UINT id; double* pVal; } utmd[] = 
{
	IDC_UTM_X0_EDIT, &CConvDlg::m_UTMP[0],
	IDC_UTM_Y0_EDIT, &CConvDlg::m_UTMP[1],
	IDC_UTM_L0_EDIT, &CConvDlg::m_UTMP[2],
	IDC_UTM_K0_EDIT, &CConvDlg::m_UTMP[3],
	IDC_UTM_E0_EDIT, &CConvDlg::m_UTMP[4]
};

int
CUTMDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
  char const* f[] = { "%1.2f", "%1.2f", "%1.2f", "%1.4f", "%1.10f" }; 
	for( int i = CELEM( utmd ) ; --i >= 0 ; )
	{
		CString s;
		s.Format( f[i], *utmd[i].pVal );
		SetDlgItemText( utmd[i].id, s );
	}
  return 1;
}
//=====================================================================
void
CUTMDialog::OnOK()
{
	for( int i = CELEM( utmd ) ; --i >= 0 ; )
	{
		CString s;
		GetDlgItemText( utmd[i].id, s );
		*utmd[i].pVal = atof( s );
	}
	EndDialog( IDOK );
}
//=====================================================================
//=====================================================================
CConwThread::CConwThread( CConvDlg* pDlg ) :
  m_pDlg( pDlg ), 
  CWinThread()
{}
//=====================================================================
CConwThread::~CConwThread()
{}
//=====================================================================
int
CConwThread::InitInstance()
{
  int t = m_pDlg->m_pUpdtr->SetModified( 0 );
  switch( m_pDlg->m_OpNum )
  {
    case TEXT_TO_PROJECT:
      m_pDlg->m_pUpdtr->TXT2GPL( m_pDlg, m_pDlg->m_TxtPath, m_pDlg->m_LogPath,
                                 m_pDlg->m_ParamList,
                                 m_pDlg->m_Mode, m_pDlg->GetNode(), 
                                 m_pDlg->GetMap(), m_pDlg->m_Cfs, m_pDlg->m_UTMP );
      break;
    case PROJECT_TO_PROJECT:
      if( m_pDlg->m_Mode & CM_REMOVE_ALL )
        break;
      m_pDlg->m_pUpdtr->GPL2GPL( m_pDlg, m_pDlg->m_TxtPath, m_pDlg->m_LogPath,
                                 m_pDlg->m_ParamList,
                                 m_pDlg->m_Mode, m_pDlg->m_tableNumber, 
                                 m_pDlg->m_minIdent, m_pDlg->m_maxIdent,
                                 m_pDlg->m_Time1, m_pDlg->m_Time2,
                                 m_pDlg->GetNode(), m_pDlg->GetMap(),
                                 m_pDlg->m_Cfs, m_pDlg->m_Rgn, 
                                 !m_pDlg->m_Rgn ? 0 : &m_pDlg->m_Box );
      m_pDlg->m_pView->SetDrh();
      m_pDlg->m_pView->ClearSelection( 0, 0 );
      m_pDlg->m_pView->m_Undo.RemoveAll();
      m_pDlg->m_pView->m_Clip.SetRectEmpty();
      m_pDlg->m_pView->Invalidate( 0 );
      break;
    case PROJECT_TO_TEXT:
      m_pDlg->m_pUpdtr->GPL2TXT( m_pDlg, m_pDlg->m_TxtPath,
                                 m_pDlg->m_ParamList,
                                 m_pDlg->m_Mode, 
                                 m_pDlg->m_tableNumber,
                                 m_pDlg->m_minIdent, m_pDlg->m_maxIdent,
                                 m_pDlg->m_Time1, m_pDlg->m_Time2,
                                 m_pDlg->GetMap(), m_pDlg->m_Cfs,
                                 m_pDlg->m_Rgn,
                                 !m_pDlg->m_Rgn ? 0 : &m_pDlg->m_Box );
      break;
    case PROJECT_TO_MIF:
      m_pDlg->m_pUpdtr->GPL2MIF( m_pDlg, m_pDlg->m_TxtPath,
                                 m_pDlg->m_ParamList,
                                 m_pDlg->m_Mode,
                                 m_pDlg->m_tableNumber, 
                                 m_pDlg->m_minIdent, m_pDlg->m_maxIdent,
                                 m_pDlg->m_Time1, m_pDlg->m_Time2,
                                 m_pDlg->GetMap(), m_pDlg->m_Cfs,
                                 m_pDlg->m_Rgn,
                                 !m_pDlg->m_Rgn ? 0 : &m_pDlg->m_Box );
      break;
    case MIF_TO_PROJECT:
      m_pDlg->m_pUpdtr->MIF2GPL( m_pDlg, m_pDlg->m_TxtPath, m_pDlg->m_LogPath,
                                 m_pDlg->m_Mode, m_pDlg->GetNode(),
                                 m_pDlg->m_classNumber, m_pDlg->m_statusNumber,
                                 m_pDlg->m_tableNumber, m_pDlg->m_entryNumber,
                                 m_pDlg->m_SubsPar,
                                 m_pDlg->GetDlgItemInt(IDC_CVD_KEY_FIELD_EDIT ),
                                 m_pDlg->m_KeyPar, m_pDlg->m_Cfs, 
                                 m_pDlg->GetDlgItemInt( IDC_CVD_DECOMPOSE_PARAM_EDIT ));
      break;
  }

  if( t )
    m_pDlg->m_pUpdtr->SetModified( t );
    
  m_pDlg->OnConversionComplete();
  return 0;
}
//=====================================================================
