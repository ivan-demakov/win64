#ifndef __MDOC_H__
#define __MDOC_H__
//=====================================================================
#include "ids.h"
#include "bmpdef.h"
#include "objdef.h"
#include "assoc.h"
//=====================================================================
class CGraphData;
class CMapView;
//=====================================================================
typedef int (CObjDef::*ODF)( UINT, int*  ) const;
//=====================================================================
class CMapDoc : public CDocument
{
  DECLARE_DYNCREATE( CMapDoc )

  friend class CAbstractDialog;
  friend class CMapView;
  friend class CSublDlg;
  friend class CSetReisMode;
  friend class CStreetPlanMode;

public:
  CMapDoc();

  void SetTitle( LPCTSTR title );
  void OnCloseDocument();
  void CreateLayers( CGraphData* pGraph );
  void ModifyObjectMenu();
  void SetSize( CSize s ) { m_DocSize = s; }
  LPTSTR SetDocType();
  int CanCloseFrame( CFrameWnd* pFrame );
  int InitColorTab();
  int InitColorTab( char const* SectName,
                    CIntAssocMap& ViewColorTab,
                    CIntAssocMap& PrintColorTab,
                    CIntAssocMap& PrintBWColorTab );

  CMenu*       GetSubMenu( LPCSTR caption );
  CString      GetDocName();
  CSize const& GetSize() const { return m_DocSize; }
  CBmpDef*      GetBmpDef( int id ) { return m_BmpDefArray.GetBmpDef( id ); }
  CBmpDefArray* GetBmpDefArray( int bUser) { return &m_BmpDefArray; }
  CObjDef*      GetObjDef() { return &m_ObjDef; }
  int MakePopupMenu( CMenu* pMenu, int type, UINT id, ODF fun ) const;
  int MakePopupMenu( CMenu* pMenu, int* types, int nTypes, UINT id = 0 ) const;

  char const* num2fontface( int fontnum )  { return m_FontTab.Translate( fontnum ); }
  int IsInited() { return m_bInited; }

  void OnBeginPrinting( int bColor = 0 );
  void OnEndPrinting();
  void OnHelp();
  void SetContextMenu( int bCm ) { m_bContextMenu = bCm; }

  static void DeletePopupMenu( CMenu* pMenu, int lvl );

public:
  CIntAssocMap  m_ViewColorTab;
  CIntAssocMap  m_PrintColorTab;
  CIntAssocMap  m_PrintBWColorTab;
  CIntAssocMap  m_UserViewColorTab;
  CIntAssocMap  m_UserPrintColorTab;
  CIntAssocMap  m_UserPrintBWColorTab;
  CStrAssocMap  m_FontTab;

private:
  int           m_bContextMenu;
  int           m_bInited;
  CSize         m_DocSize;
  CString       m_DocName;
  CObjDef       m_ObjDef;
  CBmpDefArray  m_BmpDefArray;
};
//=====================================================================
extern CMapDoc* pDocument;
//=====================================================================

#endif
