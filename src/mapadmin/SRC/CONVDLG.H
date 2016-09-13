#ifndef _MAINDLG_H_
#define _MAINDLG_H_
//=====================================================================
#include "ids.h"
#include "ident.h"
#include "tpdef.h"
#include "dlgids.h"
#include "lomedit.h"
#include "ksi.h"
#include "ptrect.h"
//=====================================================================
enum
{
  TEXT_TO_PROJECT,
  PROJECT_TO_PROJECT,
  PROJECT_TO_TEXT,
  PROJECT_TO_MIF,
  MIF_TO_PROJECT
};
//=====================================================================
enum DLG_ITEM_TYPE
{
  NOTHING,
  CHECK,
  EDIT,
  MAP,
  IBOOL,
  IINT,
  IREAL,
  ITIME
};
//=====================================================================
//=====================================================================
//=====================================================================
class MapUpdater;
class CMapView;
class CConvDlg;
//=====================================================================
class CConwThread : public CWinThread
{
  CConvDlg* m_pDlg;

public:
  CConwThread( CConvDlg* pDlg );
  ~CConwThread();
  int  InitInstance();
};

class CCalendarDlg : public CDialog
{
public:
  CCalendarDlg();
  void OnOK();
  SYSTEMTIME m_DateTime;  
};

class CConvDlg : public CDialog
{
  DECLARE_DYNCREATE( CConvDlg )

  friend class CConvCoordDlg;
  friend class CConwThread;

  struct CfgRec
  {
    DLG_ITEM_TYPE type;
    UINT          id;
    char const*   entry;
  };

  struct DatRec
  {
    DLG_ITEM_TYPE type;
    void*         pData;
    char const*   entry;
    UINT          id;
  };

public:
  CConvDlg( CMapView* pView = 0, MapUpdater* pUpdtr = 0 ) : 
    m_pView( pView ),
    m_pUpdtr( pUpdtr ),
    m_Rgn( 0 ),
    pConwThread( 0 ),
    CDialog( IDD_CONVERTOR_DIALOG, 0 ) 
  {}
  int  OnInitDialog();
  int IsConversion() { return m_Conversion; }
  void OnStopClicked();
  void OnConversionComplete();
  void OnSaveClicked();
  void OnRestoreClicked();
  void Activate();
  void OnOK();
  void OnCancel();
  void SetMsgWndVisible( int bVsbl );
  void SetReport( int total, int written, int errors, int bNow = 0 );
  void SetDecomp( int total, int written );
  void Report( UINT = 0 , LONG = 0 );
  int DoSetNode();
  int  GetNode();
  int  GetStop() { return m_Stop; }

private:
  void OnSelectByIdentButtonClicked();
  void OnSelectDateButtonClicked();
  void OnDate1ButtonClicked();
  void OnDate2ButtonClicked();
  void OnSelectClassButtonClicked();
  void OnFindClassButtonClicked();
  void OnNumerateButtonClicked();
	void OnUTM2MButtonClicked();
  void OnConvObjButtonClicked();
	void OnConvCTNClicked();
  void OnEditCheckClicked();
  void OnConvertCoordClicked();
  void OnOutputGeoMetrClicked();
  void OnOutputGeoGradClicked();
  void OnTextToProjectClicked();
  void OnProjectToProjectClicked();
  void OnProjectToTextClicked();
  void OnProjectToMifClicked();
  void OnMifToProjectClicked();
  void OnFindTxtCliced();
  void OnFindLogCliced();
  void OnSetCnvParamClicked();
  void EnDsDlgItems( int opNum );
  void SaveConfig( LPCTSTR path );
  void RestoreConfig( LPCTSTR path );
  void CalcConversion( int bInvers = 0 );
  void SetConversion();
  int  TestProjectSize();
  int  CreateIdentList();
  char* GetMap();

private:
  CConwThread* pConwThread;

  CMapView*   m_pView;
  MapUpdater* m_pUpdtr;
  TCHAR m_TxtPath[_MAX_PATH];
  TCHAR m_LogPath[_MAX_PATH];
  ksi_obj m_ksi_space;
  int m_RgnSize;
  Point* m_Rgn;
  Rect m_Box;
  int volatile m_Stop;
  int m_OpNum;
  int m_Mode;
  int m_Ssd;
  int m_Imp;

  int m_classNumber;
  int m_statusNumber;
  int m_tableNumber;
  int m_entryNumber;
  int m_minIdent;
  int m_maxIdent;
  int m_repType;
  int m_total;
  int m_written;
  int m_errors;
  int m_decomp_rst;
  int m_decomp_wrt;

	ksi_obj m_ParamList;

  char    m_ClassMap[BIT_FULL_RANGE];
  CSize   m_ProjectSize;
  int     m_Conversion;
  int     m_nRep;  
  CString m_SubsPar;
  CString m_KeyPar;

  static double m_Cfs[3][3];
  static int    m_bNumerateFrom;
  static int    m_bOffsetX;
  static int    m_bOffsetY;
  static int    m_bScaleX;
  static int    m_bScaleY;
  static int    m_bRotate;
  static long   m_Angle;
  static double m_OffsetX;
  static double m_OffsetY;
  static double m_CenterX;
  static double m_CenterY;
  static double m_ScaleX;
  static double m_ScaleY;
  static CTime  m_Time1;
  static CTime  m_Time2;
  static CfgRec ControlConfigTab[];
  static DatRec DataConfigTab[];
  
public:
  static double m_UTMP[5];
  
	DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CConvCoordDlg : public CDialog
{
public:
  CConvCoordDlg( CConvDlg* pCnvDlg ) :
    CDialog( IDD_CONVERT_COORD_DIALOG, 0 ),
    m_pCnvDlg( pCnvDlg )
  {}
  int  OnInitDialog();
  int  SaveData( int bInvers );
  void OnOK();
  void OnInvers();
  void OnChangeMode();
  void DoDataExchange( CDataExchange* pDX );

private:
  CConvDlg*  m_pCnvDlg;
  CFloatEdit m_fEdit[1];
  CRealEdit  m_rEdit[6];

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CUTMDialog : public CDialog
{
public:
  CUTMDialog( CConvDlg* pCnvDlg ) :
    CDialog( IDD_UTM_CONVERT_DIALOG, 0 ),
    m_pCnvDlg( pCnvDlg )
  {}
  int  OnInitDialog();
  void OnOK();

private:
  CConvDlg*  m_pCnvDlg;
};
//=====================================================================
class CFindObjectDlg : public CDialog
{
public:
  CFindObjectDlg( CMapView* pView ) : 
    m_pView( pView ),
    CDialog( IDD_FIND_OBJECT_DIALOG, 0 ) 
  {}
  int  OnInitDialog();
  void DoDataExchange( CDataExchange* pDX );
  void OnOK();

private:
  CMapView*  m_pView;
  static int m_Table;
  static int m_Entry;
};
//=====================================================================
#endif