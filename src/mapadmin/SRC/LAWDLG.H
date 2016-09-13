//======================================================================
#ifndef _EDITLAWDLG_H_
#define _EDITLAWDLG_H_
//======================================================================
#include "objdef.h"
#include "lomedit.h"
#include "mview.h"
//======================================================================
#define CIntList CList< int, int&>
//======================================================================
class CBaseLawDialog : public CDialog
{
public:
  CBaseLawDialog( char const* title, char* pLawTab,
                  CObjDef const* pObjDef, CWnd* pOwner,
                  UINT dlgId, int bEdit, int bAddSub = 0 ) :
    CDialog( dlgId, pOwner ),
    m_pHead( title ),
    m_pLawTab( pLawTab ),
    m_pObjDef( pObjDef ),
    m_bExpanded( 0 ),
    m_bEdit( bEdit ),
    m_bAddSub( bAddSub )
  {}
  int OnInitDialog();

protected:
  void OnClassTreeClick( NMHDR* pNotifyStruct, LRESULT* result );
  void OnClassTreeChar( NMHDR* pNotifyStruct, LRESULT* result );
  void OnPaint();
  void OnOK();
  HTREEITEM InsertTreeNode( HTREEITEM parent, int type, int bSub );
  HTREEITEM FindTreeNode( HTREEITEM node, int type );
  virtual void ExpandCheck( HTREEITEM item, int bCheck ) = 0;

protected:
  int            m_bEdit;
  int            m_bAddSub;
  int            m_bExpanded;
  char const*    m_pHead; 
  char*          m_pLawTab;
  CObjDef const* m_pObjDef;
  CTreeCtrl*     m_pClassTree;

private:
  static CIntList m_ExList;
  static int      m_FstVsblType;
  static int      m_SelType;

  DECLARE_MESSAGE_MAP()
};
//======================================================================
class CEditLawDialog : public CBaseLawDialog
{
public:
  CEditLawDialog( char* pLawTab, CObjDef const* pObjDef, CWnd* pOwner ) :
    CBaseLawDialog( "Управление редактированием", pLawTab, pObjDef, pOwner, IDD_SELEC_CLASSES_DIALOG, 1 )
  {}
private:
  void ExpandCheck( HTREEITEM item, int bCheck );
  void OnSelectAll()  { ExpandCheck( m_pClassTree->GetRootItem(), 1 ); }
  void OnUnselectAll(){ ExpandCheck( m_pClassTree->GetRootItem(), 0 ); }

  DECLARE_MESSAGE_MAP()
};
//======================================================================
class CLoadLawDialog : public CBaseLawDialog
{
public:
  CLoadLawDialog( char* title, char* pLawTab, CObjDef const* pObjDef,
                  CWnd* pOwner, UINT dlgId, int bAddSub ) :
    CBaseLawDialog( title, pLawTab, pObjDef, pOwner, dlgId, 0, bAddSub )
  {
    memcpy( m_TmpTab, pLawTab, sizeof m_TmpTab );
  }
  void OnOK();

protected:
  void ExpandCheck( HTREEITEM item, int bCheck );
  void OnSelectAll();
  void OnUnselectAll();

protected:
  char m_TmpTab[BIT_FULL_RANGE];

  DECLARE_MESSAGE_MAP()
};
//======================================================================
class CSelectClassesDialog : public CLoadLawDialog
{
public:
  CSelectClassesDialog( char* pSelTab, CObjDef const* pObjDef, CWnd* pOwner ) :
    CLoadLawDialog( "Включать классы", pSelTab, pObjDef, pOwner, IDD_SELEC_CLASSES_DIALOG, 1 )
  {}
};
//======================================================================
class CSpaceQueDialog : public CLoadLawDialog
{
public:
  CSpaceQueDialog( CObjDef const* pObjDef, CWnd* pOwner, int bSetNode ) :
    m_bSetNode( bSetNode ),
      CLoadLawDialog( bSetNode ? "Установка номера узла" : "Пространственный запрос", 
                      m_ClassMap, pObjDef, pOwner, 
                      IDD_SPACE_QUE_DIALOG, 0 )
  {}
  int OnInitDialog();
  void OnOK();
  static int GetNode()   { return m_NodeNum; }
  static int GetRadius() { return m_Radius;  }
  static int GetSelect() { return m_bSelect; }
  static int GetRegion() { return m_bRegion; }
  static int GetInside() { return m_bInside; }
  static int GetOnside() { return m_bOnside; }
  static char* GetClassMap() { return m_ClassMap; }

private:
  int     m_bSetNode;
  CIntEdit m_Edit;

private:
  static long m_NodeNum;
  static long m_Radius;
  static int m_bSelect;
  static int m_bRegion;
  static int m_bInside;
  static int m_bOnside;
  static char m_ClassMap[BIT_FULL_RANGE];
};
//======================================================================
class CSelectClassDlg : public CBaseLawDialog
{
public:
  CSelectClassDlg( char const* title, CObjDef const* pObjDef, int bFinalClass = 0, UINT ids = IDD_SELEC_CLASSES_DIALOG ) :
    m_bFinalClass( bFinalClass ),
    m_nClassNum( 0 ),
    CBaseLawDialog( title, 0, pObjDef, 0, ids, 0, 1 )
  {}
  int OnInitDialog();
  int GetClass() { return m_nClassNum; }
  void ExpandCheck( HTREEITEM hItem, int bCheck );
  void ClearCheck( HTREEITEM hItem );
  void OnOK();

private:
  int m_nClassNum;
  int m_bFinalClass;
};
//======================================================================
class CSelectClassTabDlg : public CSelectClassDlg
{
public:
  CSelectClassTabDlg( char const* title, CObjDef const* pObjDef, int bFinalClass = 0 ) :
    CSelectClassDlg( title, pObjDef, bFinalClass, IDD_SELEC_CLAS_TAB_DLG )
  {}
  int OnInitDialog();
  static int GetTabNum()   { return m_nTabNum; }
  static int GetTabStart() { return m_nTabStart; }
  void DoDataExchange( CDataExchange* pDX );

private:
  static int m_nTabNum;
  static int m_nTabStart;
};
//======================================================================
#endif
