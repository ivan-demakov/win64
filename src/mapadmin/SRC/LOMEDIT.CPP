#include "stdafx.h"
#include "lomedit.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
//=====================================================================
BEGIN_MESSAGE_MAP( CIntEdit, CSpinButtonCtrl )
  ON_WM_VSCROLL_REFLECT()
END_MESSAGE_MAP()
//=====================================================================
void
CIntEdit::Initialize( CSpinIni& InitStr, CDialog* pDlg, int bCreate )
{
  CEdit* pEdit = (CEdit*)pDlg->GetDlgItem( m_EditId = InitStr.EditId );

  WINDOWPLACEMENT WP;
  pEdit->GetWindowPlacement( &WP );
  CRect spinRect( WP.rcNormalPosition.right  + 4,
                  WP.rcNormalPosition.top    - 2,
                  WP.rcNormalPosition.right  + 12,
                  WP.rcNormalPosition.bottom + 2 );
  if( bCreate )
    Create( UDS_ARROWKEYS | WS_CHILD | WS_VISIBLE,
            spinRect, pDlg, InitStr.SpinId );
  SetBuddy( pEdit );
  m_nStep = InitStr.Step;
  SetRange32( InitStr.MinValue / m_nStep, InitStr.MaxValue / m_nStep );
  m_pValue = InitStr.InitValue;
  m_nLastPos = m_pValue ? *m_pValue : 0;
  pDlg->SetDlgItemText( InitStr.EditId, "0" );
  ChangeValue( m_nLastPos );
  SetPos( m_nLastPos / m_nStep );
}
//=====================================================================
int
CIntEdit::CorrectValue( int nVal )
{
  int min, max;
  GetRange32( min, max );
  min *= m_nStep;
  max *= m_nStep;
  return nVal > max ? max : nVal < min ? min : nVal;
}
//=====================================================================
void
CIntEdit::ChangeValue( int nDelta )
{
  GetParent()->SetDlgItemInt( m_EditId,
                              CorrectValue( GetValue() + nDelta ));
}
//=====================================================================
void
CIntEdit::SetValue()
{
  if( m_pValue )
    *m_pValue = GetValue();
}
//=====================================================================
void
CIntEdit::SetValue( int v )
{
  ChangeValue( v - GetValue());
}
//=====================================================================
int
CIntEdit::GetValue()
{
  int bOk;
  return GetParent()->GetDlgItemInt( m_EditId, &bOk );
}
//=====================================================================
void 
CIntEdit::VScroll( UINT nSBCode, UINT nPos )
{
  ChangeValue( nPos * m_nStep - m_nLastPos );
  m_nLastPos = GetValue();
  SetPos( m_nLastPos / m_nStep );
  CEdit* pEdit = (CEdit*)(GetParent())->GetDlgItem( m_EditId );
  pEdit->SetSel( 0, -1 );
  pEdit->SetFocus();
}
//=====================================================================
//=====================================================================
BEGIN_MESSAGE_MAP( CFloatEdit, CIntEdit )
END_MESSAGE_MAP()
//=====================================================================
void
CFloatEdit::Initialize( CSpinIni& InitStr, CDialog* pDlg, int bCreate  )
{
  m_mFactor = InitStr.mFactor;
  m_dFactor = InitStr.dFactor;
  m_Format  = InitStr.format;
  CIntEdit::Initialize( InitStr, pDlg, bCreate );
}
//=====================================================================
void
CFloatEdit::ChangeValue( int nDelta )
{
  char buf[80];
  sprintf_s( buf, m_Format,
             float( CorrectValue( GetValue() + nDelta )) * m_mFactor / m_dFactor );
  GetParent()->SetDlgItemText( m_EditId, buf );
}
//=====================================================================
int
CFloatEdit::GetValue()
{
  char buf[80];
  float fVal = 0.0;
  GetParent()->GetDlgItemText( m_EditId, buf, sizeof buf );
  sscanf( buf, "%f", &fVal );
  return ( fVal * m_dFactor ) / m_mFactor + ( fVal >= 0 ? .5 : -.5 );
}
//=====================================================================
//=====================================================================
BEGIN_MESSAGE_MAP( CRealEdit, CSpinButtonCtrl )
  ON_WM_VSCROLL_REFLECT()
END_MESSAGE_MAP()
//=====================================================================
void
CRealEdit::Initialize( CRealSpinIni& InitStr, CDialog* pDlg, int bCreate )
{
  CEdit* pEdit = (CEdit*)pDlg->GetDlgItem( m_EditId = InitStr.EditId );

  WINDOWPLACEMENT WP;
  pEdit->GetWindowPlacement( &WP );
  CRect spinRect( WP.rcNormalPosition.right  + 4,
                  WP.rcNormalPosition.top    - 2,
                  WP.rcNormalPosition.right  + 12,
                  WP.rcNormalPosition.bottom + 2 );
  if( bCreate )
    Create( UDS_ARROWKEYS | WS_CHILD | WS_VISIBLE,
            spinRect, pDlg, InitStr.SpinId );
  SetBuddy( pEdit );
  m_Step   = InitStr.Step;
  m_Min    = InitStr.MinValue;
  m_Max    = InitStr.MaxValue;
  m_pValue = InitStr.InitValue;
  m_Format = InitStr.format;
  SetRange32( InitStr.MinValue / m_Step, InitStr.MaxValue / m_Step );
  m_nLastPos = 0;
  pDlg->SetDlgItemText( InitStr.EditId, "0" );
  ChangeValue( *m_pValue );
}
//=====================================================================
double
CRealEdit::CorrectValue( double val )
{
  return val > m_Max ? m_Max : val < m_Min ? m_Min : val;
}
//=====================================================================
void
CRealEdit::SetValue()
{
  if( m_pValue )
    *m_pValue = GetValue();
}
//=====================================================================
void
CRealEdit::SetValue( double v )
{
  ChangeValue( v - GetValue());
}
//=====================================================================
double
CRealEdit::GetValue()
{
  char buf[80];
  GetParent()->GetDlgItemText( m_EditId, buf, sizeof buf );
  return atof( buf );
}
//=====================================================================
void
CRealEdit::ChangeValue( double delta )
{
  double v = CorrectValue( GetValue() + delta );
  char buf[80];
  sprintf_s( buf, m_Format, v );
  GetParent()->SetDlgItemText( m_EditId, buf );
}
//=====================================================================
void 
CRealEdit::VScroll( UINT nSBCode, UINT nPos )
{
  ChangeValue(( int( nPos ) - m_nLastPos ) * m_Step );
  m_nLastPos = nPos;
  CEdit* pEdit = (CEdit*)(GetParent())->GetDlgItem( m_EditId );
  pEdit->SetSel( 0, -1 );
  pEdit->SetFocus();
}
//=====================================================================
