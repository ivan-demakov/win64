//======================================================================
#include "stdafx.h"

#include <afxcmn.h>

#include "dlgids.h"
#include "lawdlg.h"
#include "util.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "graphdoc.h"
//======================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
BEGIN_MESSAGE_MAP( CBaseLawDialog, CDialog )
  ON_NOTIFY( NM_CLICK, IDC_AAD_CLASS_TREE_CTRL, OnClassTreeClick )
  ON_NOTIFY( TVN_KEYDOWN, IDC_AAD_CLASS_TREE_CTRL, OnClassTreeChar )
  ON_WM_PAINT()
END_MESSAGE_MAP()
//======================================================================
CIntList CBaseLawDialog::m_ExList;
int      CBaseLawDialog::m_FstVsblType = 0;
int      CBaseLawDialog::m_SelType = 0;
//======================================================================
HTREEITEM
CBaseLawDialog::InsertTreeNode( HTREEITEM parent, int type, int bSub )
{
  if( type >= SUBLAYER_CLASS && !bSub )
    return 0;

  HTREEITEM hItem = m_pClassTree->InsertItem( 
    TVIF_TEXT | TVIF_PARAM,
    (LPTSTR)m_pObjDef->GetName( type ),
    0, 0, 0, 0, type, parent, TVI_LAST );

  int types[FULL_RANGE];
  int n = m_pObjDef->GetDerived( type, types );

  for( int k = 1 ; k < n ; ++k )
	{
    int t = types[k];
		int r = m_pObjDef->GetMenuRange( t );
		for( int l = k ; --l >= 0 && m_pObjDef->GetMenuRange( types[l] ) > r ; types[l+1] = types[l] );
		types[l+1] = t;
	}

  for( int i = 0 ; i < n ; ++i )
  {
    type = types[i];
    if( m_pObjDef->IsClass( type ) && m_pObjDef->GetLoad( type ) && ( !m_bEdit || m_pObjDef->GetEdit( type )))
      InsertTreeNode( hItem, type, bSub );
  }
  return hItem;
}
//======================================================================
HTREEITEM
CBaseLawDialog::FindTreeNode( HTREEITEM node, int type )
{
  for( HTREEITEM fnode = 0 ; !fnode && node ; node = m_pClassTree->GetNextSiblingItem( node ))
    fnode = m_pClassTree->GetItemData( node ) == type ? 
      node : FindTreeNode( m_pClassTree->GetChildItem( node ), type );
  return fnode;
}
//======================================================================
void 
CBaseLawDialog::OnOK()
{
  TVITEM item;
  item.hItem = m_pClassTree->GetFirstVisibleItem();
  item.mask  = TVIF_PARAM ;
  m_pClassTree->GetItem( &item );
  m_FstVsblType = item.lParam;
  item.hItem = m_pClassTree->GetSelectedItem();
  m_pClassTree->GetItem( &item );
  m_SelType = item.lParam;
  EndDialog( 1 );
}
//======================================================================
void
CBaseLawDialog::OnPaint()
{
  if( !m_bExpanded )
  {
    HTREEITEM item;
    for( POSITION pos = m_ExList.GetHeadPosition() ; pos ; )
    {
      item = FindTreeNode( m_pClassTree->GetRootItem(), m_ExList.GetNext( pos ));
      if( item )
        m_pClassTree->Expand( item, TVE_EXPAND );
    }
    item = FindTreeNode( 0, m_FstVsblType );
    m_pClassTree->SelectSetFirstVisible( item );
    item = FindTreeNode( 0, m_SelType );
    m_pClassTree->Select( item, TVGN_CARET );
    m_pClassTree->SetFocus();
    m_bExpanded = 1;
  }
  
  HTREEITEM hItem = m_pClassTree->GetRootItem();
  ExpandCheck( hItem, -1 );
  if( m_bAddSub )
  {
    if( m_pObjDef->IsClass( SUBLAYER_CLASS ))
      ExpandCheck( hItem = m_pClassTree->GetNextSiblingItem( hItem ), -1 );
    if( m_pObjDef->IsClass( SUBSTAT_CLASS ))
      ExpandCheck( hItem = m_pClassTree->GetNextSiblingItem( hItem ), -1 );
  }
  CDialog::OnPaint();
}
//======================================================================
int
CBaseLawDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  SetWindowText( m_pHead );
  m_pClassTree = (CTreeCtrl*)GetDlgItem( IDC_AAD_CLASS_TREE_CTRL );
  InsertTreeNode( 0, 0, 0 );
  if( m_bAddSub )
  {
    if( m_pObjDef->IsClass( SUBLAYER_CLASS ))
      InsertTreeNode( 0, SUBLAYER_CLASS, 1 );
    if( m_pObjDef->IsClass( SUBSTAT_CLASS ))
      InsertTreeNode( 0, SUBSTAT_CLASS,  1 );
  }
  return 1;
}
//======================================================================
void
CBaseLawDialog::OnClassTreeClick( NMHDR* pNotifyStruct, LRESULT* result )
{
  UINT f;
  CRect wr;
  CWnd::FromHandle( pNotifyStruct->hwndFrom )->GetWindowRect( &wr );
  CPoint pt;
  GetCursorPos( &pt );

  HTREEITEM hItem = m_pClassTree->HitTest( pt - wr.TopLeft(), &f );
  if( *result = hItem && ( f & TVHT_ONITEM ))
    ExpandCheck( hItem, !m_pClassTree->GetCheck( hItem ));

  if( f & TVHT_ONITEMBUTTON )
  {
    TVITEM item;
    item.hItem = hItem;
    item.mask  = TVIF_PARAM ;
    m_pClassTree->GetItem( &item );
    int type = item.lParam;
    POSITION pos = m_ExList.Find( type );
    if( pos )
      m_ExList.RemoveAt( pos );
    else
      m_ExList.AddTail( m_SelType = type );
  }

  m_pClassTree->Select( hItem, TVGN_CARET );
  m_pClassTree->SetFocus();
}
//======================================================================
void
CBaseLawDialog::OnClassTreeChar( NMHDR* pNotifyStruct, LRESULT* result )
{
  HTREEITEM hItem = m_pClassTree->GetSelectedItem();

  if( !hItem )
    return;

  LV_KEYDOWN* pk = (LV_KEYDOWN*)pNotifyStruct;

  switch( pk->wVKey )
  {
    case ' ' :
    {
      int ch = m_pClassTree->GetCheck( hItem );
      ExpandCheck( hItem, !ch );
      m_pClassTree->SetCheck( hItem, ch );
      break;
    }
    case VK_LEFT :
    case VK_RIGHT :
    {
      TVITEM item;
      item.hItem = hItem;
      item.mask  = TVIF_PARAM ;
      m_pClassTree->GetItem( &item );
      int type = item.lParam;
      POSITION pos = m_ExList.Find( type );
      if( pos )
        m_ExList.RemoveAt( pos );
      else
        m_ExList.AddTail( m_SelType = type );
      break;
    }
  }
}
//======================================================================
//======================================================================
BEGIN_MESSAGE_MAP( CEditLawDialog, CBaseLawDialog )
  ON_BN_CLICKED( IDC_SCD_SELECT_ALL,   OnSelectAll   )
  ON_BN_CLICKED( IDC_SCD_UNSELECT_ALL, OnUnselectAll )
END_MESSAGE_MAP()
//======================================================================
void
CEditLawDialog::ExpandCheck( HTREEITEM hItem, int bCheck )
{
  TVITEM item;
  item.hItem = hItem;
  item.mask  = TVIF_PARAM ;
  m_pClassTree->GetItem( &item );
  int type = item.lParam;
  int ch = bCheck >= 0 ? bCheck : ( m_pLawTab[type] & 1 );

  m_pLawTab[type] = ch;
  m_pClassTree->SetCheck( hItem, ch );

  for( hItem = m_pClassTree->GetChildItem( hItem ) ; hItem ;
       hItem = m_pClassTree->GetNextSiblingItem( hItem ))
    ExpandCheck( hItem, bCheck );
}
//======================================================================
//======================================================================
BEGIN_MESSAGE_MAP( CLoadLawDialog, CBaseLawDialog )
  ON_BN_CLICKED( IDC_SCD_SELECT_ALL,   OnSelectAll   )
  ON_BN_CLICKED( IDC_SCD_UNSELECT_ALL, OnUnselectAll )
END_MESSAGE_MAP()
//======================================================================
void 
CLoadLawDialog::OnSelectAll()
{
  for( HTREEITEM hItem = m_pClassTree->GetRootItem() ; hItem ;
       ExpandCheck( hItem, 1 ), hItem = m_pClassTree->GetNextSiblingItem( hItem ));
  memset( m_TmpTab, 0xff, sizeof m_TmpTab );
}
//======================================================================
void 
CLoadLawDialog::OnUnselectAll()
{
  for( HTREEITEM hItem = m_pClassTree->GetRootItem() ; hItem ;
       ExpandCheck( hItem, 0 ), hItem = m_pClassTree->GetNextSiblingItem( hItem ));
  memset( m_TmpTab, 0x00, sizeof m_TmpTab );
}
//======================================================================
void
CLoadLawDialog::OnOK()
{
  memcpy( m_pLawTab, m_TmpTab, sizeof m_TmpTab );
  CBaseLawDialog::OnOK();
}
//======================================================================
void
CLoadLawDialog::ExpandCheck( HTREEITEM hItem, int bCheck )
{
  TVITEM item;
  item.hItem = hItem;
  item.mask  = TVIF_PARAM ;
  m_pClassTree->GetItem( &item );
  int type = item.lParam;
  int off = type >> 3;
  int msk = 1 << ( type & 7 );
  int ch = bCheck >= 0 ? bCheck : ( m_TmpTab[off] & msk );
  if( ch )
    m_TmpTab[off] |= msk;
  else
    m_TmpTab[off] &= ~msk;
  m_pClassTree->SetCheck( hItem, ch );

  for( hItem = m_pClassTree->GetChildItem( hItem ) ; hItem ;
       hItem = m_pClassTree->GetNextSiblingItem( hItem ))
    ExpandCheck( hItem, bCheck );
}
//======================================================================
//======================================================================
long CSpaceQueDialog::m_Radius  = 0;
long CSpaceQueDialog::m_NodeNum = 0;
int CSpaceQueDialog::m_bSelect = 0;
int CSpaceQueDialog::m_bRegion = 0;
int CSpaceQueDialog::m_bInside = 0;
int CSpaceQueDialog::m_bOnside = 0;
char CSpaceQueDialog::m_ClassMap[];
//======================================================================
int
CSpaceQueDialog::OnInitDialog()
{
  static CSpinIni S[] =
  {{ IDC_SPD_RADIUS_EDIT, IDC_SPD_RADIUS_SPIN, &m_Radius,  0, 100, 100000 },
   { IDC_SPD_RADIUS_EDIT, IDC_SPD_RADIUS_SPIN, &m_NodeNum, 0,   1, 100000 }};

  if( !CBaseLawDialog::OnInitDialog())
    return 0;

  m_Edit.Initialize( S[m_bSetNode], this );
  
  CheckDlgButton( IDC_SPD_SELECT_CHECK,  m_bSelect );
  CheckDlgButton( IDC_SPD_REGION_CHECK,  m_bRegion );
  CheckDlgButton( IDC_SPD_INCLUDE_RADIO, m_bInside && !m_bOnside );
  CheckDlgButton( IDC_SPD_CROSS_RADIO,   m_bOnside && !m_bInside );
  CheckDlgButton( IDC_SPD_ALL_RADIO,     m_bOnside && m_bInside || !m_bOnside && !m_bInside );
  GetDlgItem( IDC_SPD_STATIC1 )->SetWindowText( m_bSetNode ? "    № узла" : "Радиус (м)" );

  return 1;
}
//======================================================================
void
CSpaceQueDialog::OnOK()
{
  m_Edit.SetValue();
  m_bSelect = IsDlgButtonChecked( IDC_SPD_SELECT_CHECK );
  m_bRegion = IsDlgButtonChecked( IDC_SPD_REGION_CHECK );
  m_bInside = IsDlgButtonChecked( IDC_SPD_INCLUDE_RADIO ) || IsDlgButtonChecked( IDC_SPD_ALL_RADIO );
  m_bOnside = IsDlgButtonChecked( IDC_SPD_CROSS_RADIO )   || IsDlgButtonChecked( IDC_SPD_ALL_RADIO );

  memcpy( m_ClassMap, m_TmpTab, sizeof m_TmpTab );
  CBaseLawDialog::OnOK();
}
//=====================================================================
//======================================================================
void
CSelectClassDlg::ClearCheck( HTREEITEM hItem )
{
  m_pClassTree->SetCheck( hItem, 0 );
  for( hItem = m_pClassTree->GetChildItem( hItem ) ; hItem ;
       hItem = m_pClassTree->GetNextSiblingItem( hItem ))
    ClearCheck( hItem );
}
//======================================================================
void
CSelectClassDlg::ExpandCheck( HTREEITEM hItem, int bCheck )
{
  ClearCheck( 0 );
  TVITEM item;
  item.hItem = hItem;
  item.mask  = TVIF_PARAM ;
  m_pClassTree->GetItem( &item );
  m_nClassNum = 0;
  if( !m_bFinalClass || !m_pClassTree->GetChildItem( hItem ))
  {
    m_pClassTree->SetCheck( hItem, 1 );
    m_nClassNum = item.lParam;
  }
}
//======================================================================
int
CSelectClassDlg::OnInitDialog()
{
  CBaseLawDialog::OnInitDialog();
  GetDlgItem( IDC_SCD_SELECT_ALL )->ShowWindow( SW_HIDE );
  GetDlgItem( IDC_SCD_UNSELECT_ALL )->ShowWindow( SW_HIDE );
  return 1;
}
//======================================================================
void 
CSelectClassDlg::OnOK()
{
  if( m_nClassNum )
    EndDialog( 1 );
  else
  if( AskBox( "Целевой класс не выбран!\n"
              "Закрыть диалог?", 0 ) == IDYES )
    EndDialog( 0 );
  UpdateData();
}
//======================================================================
int CSelectClassTabDlg::m_nTabNum = 0;
int CSelectClassTabDlg::m_nTabStart = 0;
//======================================================================
int
CSelectClassTabDlg::OnInitDialog()
{
  UpdateData( 0 );
  return CBaseLawDialog::OnInitDialog();
}
//======================================================================
void
CSelectClassTabDlg::DoDataExchange( CDataExchange* pDX )
{
  DDX_Text( pDX, IDC_SCD_TABLE_EDIT, m_nTabNum );
  DDX_Text( pDX, IDC_SCD_ENTRY_EDIT, m_nTabStart );
}
//======================================================================
