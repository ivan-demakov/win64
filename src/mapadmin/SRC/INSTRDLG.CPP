#include "stdafx.h"

#include "instrdlg.h"
#include "dlgids.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CInstrDlg* CInstrDlg::pCurDlg = 0;
//=====================================================================
BEGIN_MESSAGE_MAP( CInstrDlg, CDialog )
  ON_WM_TIMER()
END_MESSAGE_MAP()
//=====================================================================
void 
CInstrDlg::OnOK()
{
  *m_pShowFlag = ((CButton*)GetDlgItem( IDC_ISD_CHECK_BOX ))->GetCheck();  
  EndDialog( IDOK );
  pCurDlg = 0;
}
//=====================================================================
void 
CInstrDlg::OnCancel()
{
  *m_pShowFlag = ((CButton*)GetDlgItem( IDC_ISD_CHECK_BOX ))->GetCheck();  
  EndDialog( IDCANCEL );
  pCurDlg = 0;
}
//=====================================================================
void
CInstrDlg::OnTimer( UINT nIDEvent )
{
	if( nIDEvent != IDM_INSTRDLGTIMER )
		return;

	if( pCurDlg )
		pCurDlg->SendMessage( WM_COMMAND, IDCANCEL, 0 );

	KillTimer( IDM_INSTRDLGTIMER );
}
//=====================================================================
int 
CInstrDlg::OnInitDialog()
{
	if( pCurDlg )
		pCurDlg->SendMessage( WM_COMMAND, IDCANCEL, 0 );

  CDialog::OnInitDialog();

  CButton* pText  = ((CButton*)GetDlgItem( IDC_ISD_TEXT_STATIC ));
  CButton* pCheck = ((CButton*)GetDlgItem( IDC_ISD_CHECK_BOX ));
  CButton* pOK    = ((CButton*)GetDlgItem( IDOK ));
  CButton* pCC    = ((CButton*)GetDlgItem( IDCANCEL ));

  int nMaxStr = 0, sr = 0, lp, rp, sp;

  LPCSTR p0 = m_Text;
  CWindowDC DC( this );

  for( LPCSTR p1 = p0 ; *p1 ; p0 = p1 + 1 )
  {
    ( p1 = strchr( p0, '\n' )) || ( p1 = strchr( p0, 0 ));
    CSize ss( DC.GetTextExtent( p0, p1 - p0 ));
    nMaxStr = max( nMaxStr, ss.cx );
    sr += ss.cy;
  }

  WINDOWPLACEMENT WP;

  pCheck->GetWindowPlacement( &WP );
  WP.rcNormalPosition.top    += sr;
  WP.rcNormalPosition.bottom += sr;
  pCheck->SetWindowPlacement( &WP );

  lp = WP.rcNormalPosition.left;
  rp = WP.rcNormalPosition.right;

  pText->GetWindowPlacement( &WP );
  WP.rcNormalPosition.bottom += sr;
  sp = WP.rcNormalPosition.right = WP.rcNormalPosition.left + nMaxStr + 8;
  pText->SetWindowPlacement( &WP );  
  pText->SetWindowText( m_Text );

  GetWindowPlacement( &WP );
  WP.rcNormalPosition.bottom += sr;
  WP.rcNormalPosition.right = WP.rcNormalPosition.left + max( rp - lp, sp ) + 2 * lp;
  SetWindowPlacement( &WP );
  
  pOK->GetWindowPlacement( &WP );
  WP.rcNormalPosition.top    += sr;
  WP.rcNormalPosition.bottom += sr;
  pOK->SetWindowPlacement( &WP );

  pCC->GetWindowPlacement( &WP );
  WP.rcNormalPosition.top    += sr;
  WP.rcNormalPosition.bottom += sr;
  pCC->SetWindowPlacement( &WP );

	if( !m_bCancel )
	{
		pOK->SetWindowText( "OK" );
		pCC->ShowWindow( SW_HIDE );
	}

  pCurDlg = this;
	SetTimer( IDM_INSTRDLGTIMER, 7500, 0 );

  return 1;
}
//=====================================================================
#ifndef _FULL_PROJECT
//=====================================================================
BEGIN_MESSAGE_MAP( CCopyProjectDlg, CDialog )
  ON_BN_CLICKED( IDYES, OnYes )
  ON_BN_CLICKED( IDNO, OnNo )
END_MESSAGE_MAP()
//======================================================================
int CCopyProjectDlg::m_bShowFlag = 1;
int CCopyProjectDlg::m_LastRep = IDYES;
//======================================================================
void 
CCopyProjectDlg::OnYes()
{
  m_bShowFlag = !((CButton*)GetDlgItem( IDC_CTRL ))->GetCheck();
  EndDialog( m_LastRep = IDYES );
}
//=====================================================================
void 
CCopyProjectDlg::OnNo()
{
  m_bShowFlag = !((CButton*)GetDlgItem( IDC_CTRL ))->GetCheck();
  EndDialog( m_LastRep = IDNO );
}
//=====================================================================
#endif
//=====================================================================
