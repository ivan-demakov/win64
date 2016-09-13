#ifndef __INSTRDLG_H__
#define __INSTRDLG_H__
//=====================================================================
#include "ids.h"
//=====================================================================
class CInstrDlg :  public CDialog
{
public:
  CInstrDlg( long* pShowFlag, LPCSTR text, int bCancel = 1 ) : 
    CDialog( IDD_INSTRUCT_DIALOG, 0 ),
    m_pShowFlag( pShowFlag ),
		m_bCancel( bCancel ),
    m_Text( text )
  {}

  int OnInitDialog();
  void OnOK();
	void OnCancel();
  void OnTimer( UINT nIDEvent );

private:
  long*  m_pShowFlag;
	int    m_bCancel;
  LPCSTR m_Text;

	static CInstrDlg* pCurDlg;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
#ifndef _FULL_PROJECT
class CCopyProjectDlg :  public CDialog
{
public:
  CCopyProjectDlg() : CDialog( IDD_COPY_PROJECT_DIALOG, 0 ) {}
  void OnYes();
  void OnNo();
  static int DoShow() { return m_bShowFlag; }
  static int LastRep() { return m_LastRep; }

private:
  static int m_bShowFlag;
  static int  m_LastRep;

  DECLARE_MESSAGE_MAP()
};
#endif
//=====================================================================
#endif