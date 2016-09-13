#ifndef _SABLDLG_H_
#define _SABLDLG_H_

#include "mview.h"
#include "lomedit.h"
#include "rectmode.h"
#include "ids.h"
#include "idg.h"
//=====================================================================
class CSublDlg : public CDialog
{
  DECLARE_DYNCREATE( CSublDlg )

public:
  CSublDlg();
  ~CSublDlg() { ClearUsedList(); }
  int  IsActive() { return m_IsActive; }
  int  Activate();
  int  OnInitDialog();
  int  GetLayerNum() { return m_nLayerNum; }
  int OnInsertionEnd( CDrawObject* pObj );
  void RefreshPosSize( CDrawObject* pObj );
  void OnLayerSelected();
  void AddItem( CDrawObject* pObj );
  void RemoveItem( CDrawObject* pObj );
  void SelectItem( CDrawObject* pObj );
  void OnInsertButtonClicked();
  void OnFindButtonClicked();
  void OnApplyButtonClicked();
  void OnLoadButtonClicked();
  void OnStartButtonClicked();
  void OnEndButtonClicked();
  void CreateFileList( int n = -1 );
  void OnLeftButtonClicked()      { UpdateBmp( 0 ); }
  void OnUpButtonClicked()        { UpdateBmp( 1 ); }
  void OnRightButtonClicked()     { UpdateBmp( 2 ); }
  void OnDownButtonClicked()      { UpdateBmp( 3 ); }
  void OnRangeUpButtonClicked()   { MoveRange( -1 ); }
  void OnRangeDnButtonClicked()   { MoveRange(  1 ); }
  void OnAlignHorzButtonClicked() { AlignSize( 1 ); }
  void OnAlignVertButtonClicked() { AlignSize( 2 ); }
  void OnAlignFullButtonClicked() { AlignSize( 3 ); }
  void OnLRotateButtonClicked()   { SpinBmp( 1 ); }
  void OnRRotateButtonClicked()   { SpinBmp( 0 ); }
  void OnAutosizeClicked();
  void OnFileSelChange();
  void OnFileEditChange();
  void OnCancel();
  void OnOK();
  void TurnItem( int a );

protected:
  afx_msg void OnNeedText0( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult );
  afx_msg void OnNeedText1( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult );
  afx_msg void OnNeedText2( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult );
  afx_msg void OnNeedText3( UINT nID, NMHDR* pNotifyStruct, LRESULT* lResult );
  int OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
  
private:
  void AlignSize( int dir );
  void UpdateBmp( int side );
  void MoveBmp( int side );
  void SetBmp( int side );
  void ClearUsedList();
  void Zoom( int bZoom, int bUseListBox );
  void ResumeLayerVsbl();
  void MoveRange( int dr );
  void SpinBmp( int bLeft );
  int CalcSize( int bAskDpi, int bUseSelected = 0 );
  int TestMove( CSize ds );
  CDrawObject* GetCurObj();
  CDrawObject* GetActObj();
  int GetScaleRatio( CString path, Polar c, double ls, int Z, double* pkx, double* pky );


private:
  int           m_bUsedListInited;
  int           m_bZoomed;
  int           m_Dpi;
  int           m_Scale;
  int           m_IsActive;
  int           m_nLayerNum;
  CSize         m_Size;
  CIntEdit      m_Edit[5];
  CFloatEdit    m_FEdit[2];
  CString       m_BmpPath[2];
  CStringArray  m_FullArray;
  CObjList      m_UsedList;
  CDrawObject*  m_pReper;
  CString       m_Y_HTTP;
  CString       m_G_HTTP;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
#endif