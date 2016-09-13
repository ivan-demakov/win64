#ifndef __DWIN_H__
#define __DWIN_H__
//=====================================================================
#include "mybar.h"
#include "ksi.h"
#include "ids.h"
//=====================================================================
enum { LEFT_OF, TOP_OF, RIGHT_OF, BOTTOM_OF };
//=====================================================================
struct CProfileVar
{
  long* var;
  TCHAR* sect;
  TCHAR* entry;
  int def;
  int bVal;
  int min, max;
};
//=====================================================================
struct ToolBarPos
{
  UINT bitMapID;
  char const* pName;
  char const* pSect;
  UINT baseID;
  int  place;
  UINT neighbourID;
};
//=====================================================================
class CMainWin : public CFrameWnd
{
private:
  DECLARE_DYNCREATE( CMainWin )

  static UINT  indicators[];
  static char* indicatorExmp[];
  static long  m_bToolTips;
  static long  m_bPolarCoord;

public:
  CMainWin();
  ~CMainWin();

  CControlBar* GetBarPtr( UINT bitMapID );
  int  CreateToolBars();
  CControlBar* CreateToolBar( int i );
  int  GetBarNum( UINT bitMapID );
  void DirectUpdateSts();
  int  GetShapeNum();
  void SetShapeNum( int n );
  void SetToolTips( int bSet );

private:
  CStatusBar    m_Vstatus;
  CProgressCtrl m_Progress;
  CControlBar*  m_pToolBar[IDW_TOOLBAR_END-IDW_TOOLBAR_BEG];
  int  OnCreate( LPCREATESTRUCT lpCreateStruct );
  void OnSize( UINT type, int cx, int cy );
  void OnUpdateSts( CCmdUI* pCmdUI );
  void OnDocPrintSetup();
  void OnNcRButtonDown( UINT nFlags, CPoint point );
  void OnToolbarSelect( UINT nId );
  void OnSizing( UINT nSide, LPRECT lpRect );
  void OnPolarCoord();
  void OnUpdatePolarCoord( CCmdUI* pCmdUI );
  void OnClose();

public:
  static ToolBarPos const TBS[];

public:
  void PutHint( char const* hint = 0, UINT pane = 0 );
  void ClearHint( UINT pane = 0 ){ PutHint( "", pane ); }
  void ShowProgress( int nLower, int nUpper );
  void OffsetProgress( int nPos );
  void SetProgress( int nPos );
  void HideProgress();

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
void ReportHint( char const* repot );
//=====================================================================
inline void
ClearHint()
{
  ReportHint( "" );
}
//=====================================================================
#endif
