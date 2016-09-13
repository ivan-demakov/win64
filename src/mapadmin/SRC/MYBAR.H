#ifndef _MYBAR_H_
#define _MYBAR_H_

#include <afxcmn.h>
//=====================================================================
struct BarBtnInf
{
  CString BmpFileName;
  CString ToolTipText;
  UINT    CommandID;
  void Create( BarBtnInf const& bi )
  {
    Create( bi.BmpFileName, bi.ToolTipText, bi.CommandID );
  }
  void Create( LPCTSTR nm, LPCTSTR tt, UINT id )
  {
    BmpFileName = nm;
    ToolTipText = tt;
    CommandID   = id;
  }
};

class CMyBar : public CToolBar
{
  enum
  {
    MAX_BAR_SIZE = 128,
    BUTTON_X_SIZE = 16,
    BUTTON_Y_SIZE = 16
  };

public:
  CMyBar( CWnd* pWnd, LPCTSTR sectName, UINT baseID, UINT tbid );
  CMyBar( CMyBar* pBar );
  ~CMyBar();
  int GetBarSize() { return m_BarSize; }
  int GetID( int i ) { return i < m_BarSize ? m_pBarInf[i].CommandID : 0; }
  CString GetToolTip( int i ) { return i < m_BarSize ? m_pBarInf[i].ToolTipText : ""; }
  CString GetName( int i ) { return i < m_BarSize ? m_pBarInf[i].BmpFileName : ""; }
  UINT GetBaseId()  { return m_BaseId; }
  int IsSeparator( int i );
  int RemoveButton( int n );
  int InsertButton( int n, CString const& BFName, CString const& TTText, UINT CmdID );
  int InsertSeparator( int n );
  int  GetPosition( UINT id );

private:
  int        m_BarSize;
  UINT       m_BaseId;
  CBitmap    m_BarBmp;
  BarBtnInf* m_pBarInf;

protected:
  afx_msg void OnNeedTextA( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult );
  int OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
   
  DECLARE_MESSAGE_MAP()
};
//=====================================================================
#endif //_MYBAR_H_
