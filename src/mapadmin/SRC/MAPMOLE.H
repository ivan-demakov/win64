#ifndef __DAPP_H__
#define __DAPP_H__
//=====================================================================
#include "ids.h"
#include "ident.h"
#include "ksi.h"
//=====================================================================
class CMapDoc;
class CConductor;
//=====================================================================
enum LawLevel
{
  LL_NOTHING = 0,
  LL_PARAMS  = 1,
  LL_EDITION = 2
};
//=====================================================================
class CMapmoleApp : public CWinApp
{
public:
  CMapmoleApp();

  void WinHelp( DWORD dwData, UINT nCmd = HELP_CONTEXT );
  void OnFileNew();

  int InitInstance();
  int GetProfileKeys( char const* pSection, char* buf, int size );
  LPCSTR GetIniPath() { return m_ModulePath; }
  LPCSTR GetProjectFilePath( LPCSTR name );
  CConductor* GetConductor();
  CString    GetSubbmpPath( int bLocal );

  LRESULT ProcessWndProcException( CException* e, const MSG* pMsg );
  CDocument* OpenDocumentFile( LPCSTR DocFileName );
  HGLOBAL GetDevMode()  { return m_hDevMode; }
  HGLOBAL GetDevNames() { return m_hDevNames; }
  CMapDoc* GetDocument();

private:
  char m_ModulePath[_MAX_PATH];
  char m_FilePath[_MAX_PATH];

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
extern CMapmoleApp App;
//=====================================================================
#endif
