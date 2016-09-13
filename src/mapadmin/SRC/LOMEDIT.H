#ifndef __LOMEDIT_H__
#define __LOMEDIT_H__
//=====================================================================
#include "afxcmn.h"
//=====================================================================
struct CSpinIni
{
  UINT EditId, SpinId;
  long *InitValue, MinValue, Step, MaxValue;
  long mFactor, dFactor;
  char* format;
};
//=====================================================================
class CIntEdit : public CSpinButtonCtrl
{
public:
  int CorrectValue( int val );
  void SetValue();
  void SetValue( int v );
  virtual void Initialize( CSpinIni& InitStr, CDialog* pDlg, int bCreate = 1 );
  virtual void ChangeValue( int nDelta = 0 );
  virtual int GetValue();

protected:
  UINT   m_EditId;
  int    m_nStep;
  int    m_nLastPos;
  long*  m_pValue;
  void VScroll( UINT nSBCode, UINT nPos );

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CFloatEdit : public CIntEdit
{
public:
  void Initialize( CSpinIni& InitStr, CDialog* pDlg, int bCreate = 1 );
  void ChangeValue( int nDelta = 0 );
  int GetValue();

protected:
  UINT m_mFactor, m_dFactor;
  char* m_Format;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
struct CRealSpinIni
{
  UINT EditId, SpinId;
  double *InitValue, MinValue, Step, MaxValue;
  char* format;
};
//=====================================================================
class CRealEdit : public CSpinButtonCtrl
{
public:
  double CorrectValue( double val );
  void SetValue();
  void SetValue( double v );
  void Initialize( CRealSpinIni& InitStr, CDialog* pDlg, int bCreate = 1 );
  void ChangeValue( double delta = 0 );
  double GetValue();

private:
  UINT    m_EditId;
  int     m_nLastPos;
  double  m_Min;
  double  m_Max;
  double  m_Step;
  double* m_pValue;
  char*   m_Format;

private:
  void VScroll( UINT nSBCode, UINT nPos );

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
#endif
