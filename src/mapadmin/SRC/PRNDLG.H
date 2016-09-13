#ifndef __PRNDLG_H__
#define __PRNDLG_H__
//=====================================================================
#include "lomedit.h"
//=====================================================================
#define MAX_SPC_TOP    50
#define MAX_SPC_BOTTOM 50
#define MAX_SPC_LEFT   50
#define MAX_SPC_RIGHT  50

#define MAX_DOCUMENT_FRAME_SPACE 50
#define MAX_DOCUMENT_FRAME_LINE  20
#define MAX_DOCUMENT_OVERLAP     10
//=====================================================================
enum { LAYOUT_ALBUM, LAYOUT_GLUE };
//=====================================================================
struct inistr
{
  UINT  id;
  long* pv;
};
//=====================================================================
class CDocSetupPage;
//=====================================================================
class CPageSetupPage : public CPropertyPage
{
  DECLARE_DYNAMIC( CPageSetupPage )
public:
  CPageSetupPage();

  static long m_layopt;
  static long m_overlapVal;
  static long m_spaceEnb;
  static long m_spaceVal;
  static long m_lineEnb;
  static long m_overlapEnb;
  static long m_lineVal;
  static long m_numerationEnb;
  static long m_startNumber;
  static long m_subscriptEnb;
  static CString m_subscriptText;
  static CRect   m_spc;

  int OnInitDialog ();

protected:
  CDocSetupPage* m_pDsp;
  CIntEdit   iedit[7];
  CFloatEdit fedit[1];
  WORD m_bInited;
  static inistr m_IniStr[];
  void OnAlbSelected();
  void OnMapSelected();
  void RED( UINT* el, UINT* dl );

public:
  void OnCheckSelected();
  void SetDsp( CDocSetupPage* pDsp ) { m_pDsp = pDsp; }

public:
  void OnOK();
  void DoDataExchange( CDataExchange* pDX );
  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CDocSetupPage : public CPropertyPage
{
  DECLARE_DYNAMIC( CDocSetupPage )
public:
  CDocSetupPage();

  static long m_colorEnb;
  static long m_selectionEnb;
  static long m_selectOnlyEnb;
  static long m_gridEnb;
  static long m_gridVal;
  static long m_curscaleEnb;
  static long m_scaleVal;
  static long m_autoSizeEnb;
  static long m_autoScaleEnb;
  static long m_extraSpaceVal;

  int OnInitDialog();

protected:
  CIntEdit   iedit[2];
  WORD m_bInited;
  static inistr m_IniStr[];

public:
  void OnCheckSelected();

public:
  void OnOK();
  void DoDataExchange( CDataExchange* pDX );
  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CPrintSetupSheet : public CPropertySheet
{
public:
  DECLARE_DYNAMIC( CPrintSetupSheet )
  CPrintSetupSheet( CWnd *pWndParent );

protected:
  CPageSetupPage m_pageSetupPage;
  CDocSetupPage  m_docSetupPage;
  void OnOK();

  static int m_curIndex;
  DECLARE_MESSAGE_MAP()
};
//=====================================================================
#endif
