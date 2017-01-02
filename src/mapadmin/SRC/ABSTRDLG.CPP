//======================================================================
#include "stdafx.h"
#ifndef _FULL_PROJECT

#include "ksiutil.h"
#include "assoc.h"
#include "ksi_type.h"
#include "ids.h"
#include "dlgids.h"
#include "dwin.h"
#include "mview.h"
#include "abstrdlg.h"
#include "scale.h"
#include "util.h"
#include "msutil.h"
#include "ksikey.h"
#include "mapmole.h"
#include "mdoc.h"
#include "objdef.h"
#include "linetmpl.h"
#include "joke.h"
#include "mercator.h"
//======================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
BEGIN_MESSAGE_MAP( CAutoScrollTreeCtrl, CTreeCtrl )
  ON_NOTIFY_REFLECT( TVN_BEGINDRAG, OnBeginDrag )
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_DESTROY()
  ON_WM_TIMER()
END_MESSAGE_MAP()
//======================================================================
static int CALLBACK 
CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM )
{
  CObjDef* pObjDef = pView->GetDocument()->GetObjDef();
  int r = pObjDef->GetRange( lParam2 ) - pObjDef->GetRange( lParam1 );
  return r < 0 ? -1 : r > 0 ? 1 : 0;
}
//======================================================================
static int
bBeep()
{
  Beep( 440, 400 );
  return 0;
}
//======================================================================
void
CAutoScrollTreeCtrl::OnDestroy()
{
  CImageList* pImageList = GetImageList( TVSIL_NORMAL );
  pImageList->DeleteImageList();
  delete pImageList;
}
//======================================================================
void
CAutoScrollTreeCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
  UINT flags;
  if( m_bDragging )
  {
    ASSERT( m_pImageList );
    m_pImageList->DragMove( point );
    m_pImageList->DragLeave( this );

    CRect wr;
    GetWindowRect( &wr );
    ScreenToClient( &wr );
    int sbMin, cbMax;
    GetScrollRange( SB_VERT, &sbMin, &cbMax );
    int sp = GetScrollPos( SB_VERT );

    int dy = point.y < wr.top - GetItemHeight() && sp > sbMin ? 1 :
             point.y > wr.bottom && sp < cbMax ? -1 : 0;

    if( dy )
    {
      m_nScrollDelta = dy < 0 ? SB_LINEDOWN : SB_LINEUP;
      if( !m_bTimerOn )
      {
        SetTimer( IDM_TREESCROLLTIMER, 100, 0 );
        m_bTimerOn = 1;
      }
    }
    else
    if( m_bTimerOn )
    {
      KillTimer( IDM_TREESCROLLTIMER );
      m_bTimerOn = 0;
    }

    SelectDropTarget( m_hItemDrop = HitTest( point, &flags ));
    m_pImageList->DragEnter( this, point );
  }
  CTreeCtrl::OnMouseMove( nFlags, point );
}
//======================================================================
void
CAutoScrollTreeCtrl::OnTimer( UINT nIDEvent )
{
  if( nIDEvent == IDM_TREESCROLLTIMER )
    SendMessage( WM_VSCROLL, m_nScrollDelta, 0 );
}
//======================================================================
void
CAutoScrollTreeCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
  if( m_bDragging )
  {
    ASSERT( m_pImageList );
    m_pImageList->DragLeave( this );
    m_pImageList->EndDrag();
    delete m_pImageList;
    m_pImageList = 0;

    ReleaseCapture();
    m_bDragging = 0;
    SelectDropTarget( 0 );
    ShowCursor( 1 );
  }

  if( m_bTimerOn )
  {
    KillTimer( IDM_TREESCROLLTIMER );
    m_bTimerOn = 0;
  }

  CTreeCtrl::OnLButtonUp( nFlags, point );
}
//======================================================================
void
CAutoScrollTreeCtrl::OnBeginDrag( LPNMHDR pnmhdr, LRESULT* pLResult )
{
  CPoint ptAction;
  UINT   nFlags;
  CPoint HotSpot( 0, 0 );

  GetCursorPos( &ptAction );
  ScreenToClient( &ptAction );
  m_bDragging = 1;
  m_hItemDrop = 0;

  ASSERT( !m_pImageList );
  m_pImageList = CreateDragImage( m_hItemDrag = HitTest( ptAction, &nFlags ));
  m_pImageList->DragShowNolock( 1 );
  m_pImageList->SetDragCursorImage( 0, HotSpot );
  ShowCursor( 0 );
  m_pImageList->BeginDrag( 0, HotSpot );
  m_pImageList->DragMove( ptAction );
  m_pImageList->DragEnter( this, ptAction );
  SetCapture();
}
//======================================================================
//======================================================================
BEGIN_MESSAGE_MAP( CClassTreeCtrl, CAutoScrollTreeCtrl )
  ON_WM_LBUTTONUP()
END_MESSAGE_MAP()
//======================================================================
int
CClassTreeCtrl::IsChildNodeOf( HTREEITEM hItemChild, HTREEITEM hSuspectedParent )
{
  while( hItemChild && hItemChild != hSuspectedParent )
     hItemChild = GetParentItem( hItemChild );

  return hItemChild != 0;
}
//======================================================================
void
CClassTreeCtrl::TransferItem( HTREEITEM hItemDrag, HTREEITEM hItemDrop )
{
  if( !hItemDrop || GetItemData( hItemDrag ) < CLASS_RANGE != GetItemData( hItemDrop ) < CLASS_RANGE )
  {
    bBeep();
    return;
  }

  TV_INSERTSTRUCT tvstruct;
  char buf[128];

  tvstruct.item.hItem = hItemDrag;
  tvstruct.item.cchTextMax = sizeof buf - 1;
  tvstruct.item.pszText = buf;
  tvstruct.item.mask = TVIF_PARAM | TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
  GetItem( &tvstruct.item );
  tvstruct.hParent = hItemDrop;
  tvstruct.hInsertAfter = hItemDrop == GetRootItem() ? GetPrevSiblingItem( FindTreeNode( SUBLAYER_CLASS )) : TVI_LAST;
  tvstruct.item.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
  HTREEITEM hNewItem = InsertItem( &tvstruct );

  for( HTREEITEM hFirstChild ; hFirstChild = GetChildItem( hItemDrag ) ;
       TransferItem( hFirstChild, hNewItem ));
  
  DeleteItem( hItemDrag );
}
//======================================================================
int
CClassTreeCtrl::MoveSelected( int bUp )
{
  HTREEITEM dragItem = GetSelectedItem();

  if( !dragItem )
    return bBeep();

  HTREEITEM dropItem = bUp ? GetPrevSiblingItem( dragItem ):
                             GetNextSiblingItem( dragItem );

  if( !dropItem )
    return bBeep();

  int dragClass = GetItemData( dragItem );
  int dropClass = GetItemData( dropItem );
  if( bUp && dragClass < CLASS_RANGE != dropClass < CLASS_RANGE )
    return bBeep();

  if( bUp && !( dropItem = GetPrevSiblingItem( dropItem )))
    dropItem = TVI_FIRST;

  TV_INSERTSTRUCT tvstruct;
  char buf[128];

  tvstruct.item.hItem = dragItem;
  tvstruct.item.cchTextMax = sizeof buf - 1;
  tvstruct.item.pszText = buf;
  tvstruct.item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
  GetItem( &tvstruct.item );
  tvstruct.hParent = GetParentItem( dragItem );
  tvstruct.hInsertAfter = dropItem;
  dropItem = InsertItem( &tvstruct );

  for( HTREEITEM hFirstChild ; hFirstChild = GetChildItem( dragItem ) ;
       TransferItem( hFirstChild, dropItem ));

  DeleteItem( dragItem );
  SelectItem( dropItem );
  SetFocus();
  return 1;
}
//======================================================================
void
CClassTreeCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
  if( m_bDragging )
  {
    int type = GetItemData( m_hItemDrag );
    if( IsUserObject( type ) && m_hItemDrag != m_hItemDrop &&
        !IsChildNodeOf( m_hItemDrop, m_hItemDrag ) &&
        GetParentItem( m_hItemDrag ) != m_hItemDrop )
      TransferItem( m_hItemDrag, m_hItemDrop );
    else
      bBeep();
  }
  CAutoScrollTreeCtrl::OnLButtonUp( nFlags, point );
}
//======================================================================
HTREEITEM
CClassTreeCtrl::FindTreeNode( int type, HTREEITEM node )
{
  if( !node )
    node = GetRootItem();

  if( GetItemData( node ) == type )
    return node;

  HTREEITEM fnde = 0;
  for( node = GetChildItem( node ) ; node && !fnde ; node = GetNextSiblingItem( node ))
    fnde = FindTreeNode( type, node );
  return fnde;
}
//======================================================================
HTREEITEM
CClassTreeCtrl::FindTreeNode( LPCTSTR prfx, int pxln, HTREEITEM node )
{
  if( !node )
    node = GetRootItem();

  if(( pxln ? GetItemText( node ).Left( pxln ) : 
              GetItemText( node )) == prfx )
    return node;    

  HTREEITEM fnde = 0;
  for( node = GetChildItem( node ) ; node && !fnde ;
       node = GetNextSiblingItem( node ))
    fnde = FindTreeNode( prfx, pxln, node );
  return fnde;
}
//======================================================================
//======================================================================
inline void
bound( int& v, int mi, int ma )
{
  v = v < mi ? mi : v > ma ? ma : v;
}
//======================================================================
CObjDef* CSubdialog::m_pObjDef;

int CAbstractDialog::m_IsIcon          = 0;
int CAbstractDialog::m_IsPolygon       = 0;
int CAbstractDialog::m_IsPolyline      = 0;
int CAbstractDialog::m_IsText          = 0;
int CAbstractDialog::m_IsFootnote      = 0;
int CAbstractDialog::m_ClassNumber     = 0;
int CAbstractDialog::m_ParentNumber    = 0;
int CAbstractDialog::m_IsMinShowAlways = 0;
int CAbstractDialog::m_IsMaxShowAlways = 0;
int CAbstractDialog::m_IsScaleAlways   = 0;
int CAbstractDialog::m_ScaleBegin      = 5000;
int CAbstractDialog::m_ScaleEnd        = 5000;
int CAbstractDialog::m_ClassLaw        = 0;
int CAbstractDialog::m_ClassLoad       = 0;
int CAbstractDialog::m_ClassTable      = 0;
int CAbstractDialog::m_bClassFind      = 0;
int CAbstractDialog::m_bNewGraphics    = 0;
int CAbstractDialog::m_bClassTreeChanged;
int CAbstractDialog::m_MinShowLimit    = 0;
int CAbstractDialog::m_MaxShowLimit    = 5000;
int CAbstractDialog::m_Range           = 0;
int CAbstractDialog::m_IsTextInvisible = 0;
int CAbstractDialog::m_bConcretClass   = 0;

int CPolygonParamDialog::m_Used;
int CLineParamDialog::m_Used;
int CIconParamDialog::m_Used;
int CTextParamDialog::m_Used;

CString CAbstractDialog::m_ClassName;
CString CAbstractDialog::m_Hint;
//======================================================================
IMPLEMENT_DYNCREATE( CAbstractDialog, CDialog )
//======================================================================
BEGIN_MESSAGE_MAP( CAbstractDialog, CDialog )
  ON_BN_CLICKED( IDC_AAD_CLASS_CHANGE_BUTTON, OnClassNameEditBegin )
  ON_BN_CLICKED( IDC_AAD_CLASS_UP_BUTTON, OnSelectClassUp )
  ON_BN_CLICKED( IDC_AAD_CLASS_DN_BUTTON, OnSelectClassDn )
  ON_BN_CLICKED( IDC_SDLG_FORE_BUTTON, OnFore )
  ON_BN_CLICKED( IDC_AAD_TEXT_CHECK, OnGraphicsChange )
  ON_BN_CLICKED( IDC_AAD_ICON_RADIO, OnGraphicsChange )
  ON_BN_CLICKED( IDC_AAD_POLYGON_RADIO, OnGraphicsChange )
  ON_BN_CLICKED( IDC_AAD_POLYLINE_RADIO, OnGraphicsChange )
  ON_BN_CLICKED( IDC_AAD_MIN_SHOW_ALWAYS_CHECK, OnMinShowAlwaysClicked )
  ON_BN_CLICKED( IDC_AAD_MAX_SHOW_ALWAYS_CHECK, OnMaxShowAlwaysClicked )
  ON_BN_CLICKED( IDC_AAD_CLASS_FIND_CHECK, OnClassFindClicked )
  ON_BN_CLICKED( IDC_AAD_REMOVE_BUTTON, OnRemoveClicked )
  ON_BN_CLICKED( IDC_AAD_SCALE_ALWAYS_CHECK, OnScaleAlwaysClicked )
  ON_BN_CLICKED( IDC_AAD_CLASS_CLOSED_CHECK, OnClassClosedClicked )
  ON_EN_UPDATE( IDC_AAD_CLASS_NAME_EDIT, OnClassChange )
  ON_NOTIFY( TVN_SELCHANGED, IDC_AAD_CLASS_TREE_CTRL, OnClassTreeSelChanged )
END_MESSAGE_MAP()
//======================================================================
CAbstractDialog::CAbstractDialog( CMapView* pView ) :
  CSubdialog( 0, this ),
  m_pView( pView ),
  m_bActive( 0 )
{}
//======================================================================
void
CAbstractDialog::OnClassClosedClicked()
{
  int bCh = IsDlgButtonChecked( IDC_AAD_CLASS_CLOSED_CHECK );

  if( !m_bClosedObjEdition || IsClass() && !IsAbstractObject( m_ClassNumber ))
  {
    CheckDlgButton( IDC_AAD_CLASS_CLOSED_CHECK, !bCh );
    Beep( 440, 400 );
  }
  else
  {
    m_bConcretClass = bCh;
    if( !IsClass())
    {
      m_KsiParamList = ksi_nil;
      m_ClassNumber = GetFreeClass( m_bConcretClass );
    }
    SetDlgItemInt( IDC_AAD_CLASS_NUMBER_EDIT, m_ClassNumber );
  }
}
//======================================================================
void
CAbstractDialog::OnScaleAlwaysClicked()
{
  int e = !IsDlgButtonChecked( IDC_AAD_SCALE_ALWAYS_CHECK );
  ((CButton*)GetDlgItem( IDC_AAD_SCALE_BEGIN_EDIT ))->EnableWindow( e );
  ((CButton*)GetDlgItem( IDC_AAD_SCALE_END_EDIT ))->EnableWindow( e );
}
//======================================================================
void
CAbstractDialog::OnClassNameEditBegin()
{
  HTREEITEM item = m_ClassTree.GetSelectedItem();
  if( item )
    m_ClassTree.EditLabel( item );
}
//======================================================================
HTREEITEM
CAbstractDialog::InsertTreeNode( HTREEITEM parent, int type )
{
  int bSysObject = !( IsAbstractObject( type ) || IsConcretObject( type ) && m_bClosedObjEdition );

  HTREEITEM p = m_ClassTree.InsertItem( 
    TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM,
    type ? (LPTSTR)m_pObjDef->GetName( type ) : "",
    bSysObject * 2, bSysObject * 2 + 1, 0, 0, 
    type, parent, TVI_LAST );

  int types[CLASS_RANGE];
  for( int i = m_pObjDef->GetDerived( type, types ) ; --i >= 0 ; 
       m_pObjDef->IsClass( types[i] )&& InsertTreeNode( p, types[i] ));

  TVSORTCB SortCB = { parent, CompareFunc, 0 };
  m_ClassTree.SortChildrenCB( &SortCB );

  return p;
}
//======================================================================
int
CAbstractDialog::SaveTreeNode( HTREEITEM node, int newRange )
{
  if( !node )
    return newRange;

  int type = m_ClassTree.GetItemData( node );
  int oldRange = m_pObjDef->GetRange( type );
  if( type && oldRange != newRange )
  {
    m_pObjDef->SetRange( type, newRange );
    m_bClassTreeChanged = 1;
  }
  --newRange;
  for( node = m_ClassTree.GetChildItem( node ) ; node ;
       newRange = SaveTreeNode( node, newRange ),
       node = m_ClassTree.GetNextSiblingItem( node ));
  return newRange;
}
//======================================================================
int
CAbstractDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_Hint = "";
  m_bClassTreeChanged = 0;
  m_pObjDef = m_pView->m_pObjDef;
  m_ClassTree.SubclassDlgItem( IDC_AAD_CLASS_TREE_CTRL, this );

  CImageList* pImageList = new CImageList();
  pImageList->Create( 16, 16, ILC_COLORDDB, 4, 0 );
  CBitmap bitmap;
  bitmap.LoadBitmap( IDB_MAP_OBJECT );
  pImageList->Add( &bitmap, COLORREF( 0 ));

  m_bClosedObjEdition = App.GetProfileInt( "General", "ClosedObjEdition", 0 );
  m_ClassTree.SetImageList( pImageList, TVSIL_NORMAL );
  m_ClassTree.Expand( InsertTreeNode( 0, 0 ), TVE_EXPAND );
  m_ClassTree.SelectItem( m_ClassTree.GetRootItem());

  return 1;
}
//======================================================================
void
CAbstractDialog::OnClassFindClicked()
{
  HTREEITEM item;
  if( !( m_bClassFind ^= 1 ))
    GetDlgItem( IDC_AAD_CLASS_NAME_EDIT )->SetWindowText( m_ClassTree.GetItemText( m_ClassTree.GetSelectedItem()));
  else
  if( item = m_ClassTree.FindTreeNode( m_ClassName, strlen( m_ClassName )))
    m_ClassTree.SelectItem( item );
}
//======================================================================
void
CAbstractDialog::OnClassChange()
{
  if( m_ClassTree.GetEditControl())
    return;

  GetDlgItem( IDC_AAD_CLASS_NAME_EDIT )->GetWindowText( m_ClassName );
  RemoveSpaces( m_ClassName.GetBuffer( 256 ));
  m_ClassName.ReleaseBuffer();

  HTREEITEM item = m_ClassTree.FindTreeNode( m_ClassName, m_bClassFind ? strlen( m_ClassName ) : 0 );
  HTREEITEM prnt = item ? m_ClassTree.GetParentItem( item ) : m_ClassTree.GetSelectedItem();
  m_ClassNumber  = item ? m_ClassTree.GetItemData( item ) : GetFreeClass( 0 );
  m_ParentNumber = prnt ? m_ClassTree.GetItemData( prnt ) : 0;

  SetDlgItemInt( IDC_AAD_CLASS_NUMBER_EDIT, m_ClassNumber );
  GetDlgItem( IDC_AAD_CLASS_NUMBER_EDIT )->EnableWindow( !IsClass());

  if( item )
    if( m_bClassFind && item != m_ClassTree.GetSelectedItem())
      m_ClassTree.SelectItem( item );
    else
      UpdateSelectedClass();
  else
  if( !m_bClassFind )
    GetDlgItem( IDOK )->EnableWindow( strlen( m_ClassName ) != 0 );
  else
    CheckDlgButton( IDC_AAD_CLASS_CLOSED_CHECK, 0 );
}
//======================================================================
void
CAbstractDialog::OnClassTreeSelChanged( NMHDR* pNotifyStruct, LRESULT* result )
{
  HTREEITEM item = m_ClassTree.GetSelectedItem();
  if( !item )
    return;

  HTREEITEM prnt = m_ClassTree.GetParentItem( item );
  
  if( !m_bClassFind )
    GetDlgItem( IDC_AAD_CLASS_NAME_EDIT )->SetWindowText( m_ClassTree.GetItemText( item ));
  m_ClassName    = m_ClassTree.GetItemText( item );
  m_ClassNumber  = m_ClassTree.GetItemData( item );
  m_ParentNumber = prnt ? m_ClassTree.GetItemData( prnt ) : 0;

  UpdateSelectedClass();
}
//======================================================================
void
CAbstractDialog::UpdateSelectedClass()
{
  static CStrAssoc const asa[] =
  {
    CStrAssoc( "icon",             IDC_AAD_ICON_RADIO     ),
    CStrAssoc( "polyline",         IDC_AAD_POLYLINE_RADIO ),
    CStrAssoc( "bound-polyline",   IDC_AAD_POLYLINE_RADIO ),
    CStrAssoc( "polygon",          IDC_AAD_POLYGON_RADIO  ),
    CStrAssoc( "outlined-polygon", IDC_AAD_POLYGON_RADIO  ),
    CStrAssoc( "smart-polygon",    IDC_AAD_POLYGON_RADIO  ),
    CStrAssoc( "text",             IDC_AAD_TEXT_CHECK     ),
    CStrAssoc( "footnote",         IDC_AAD_FOOTNOTE_CHECK )
  };

  static int* isa[] =
  {
    &m_IsIcon,
    &m_IsPolyline,
    &m_IsPolyline,
    &m_IsPolygon,
    &m_IsPolygon,
    &m_IsPolygon,
    &m_IsText,
    &m_IsFootnote
  };

  m_Range        = m_pObjDef->GetRange( m_ClassNumber );
  m_ClassLaw     = 2 - m_pObjDef->GetEdit( m_ClassNumber );
  m_ClassLoad    = 2 - m_pObjDef->GetLoad( m_ClassNumber );
  m_ClassTable   = m_pObjDef->GetTable( m_ClassNumber );
  m_MinShowLimit = m_pObjDef->GetMinLimit( m_ClassNumber );
  m_MaxShowLimit = m_pObjDef->GetMaxLimit( m_ClassNumber );
  m_ScaleBegin   = m_pObjDef->GetMinScale( m_ClassNumber );
  m_ScaleEnd     = m_pObjDef->GetMaxScale( m_ClassNumber );
  m_IsMinShowAlways = !m_MinShowLimit;
  m_IsMaxShowAlways = !m_MaxShowLimit;
  m_IsTextInvisible = (int)m_pObjDef->GetTextInvisible( m_ClassNumber );

  if( m_IsMaxShowAlways )
    m_MaxShowLimit = 5000;
  
  if( m_IsMinShowAlways )
    m_MinShowLimit = 100;

  m_IsScaleAlways = !m_ScaleBegin || !m_ScaleEnd;

  m_OldIsScaleAlways   = m_IsScaleAlways;
  m_OldIsMinShowAlways = m_IsMinShowAlways;
  m_OldIsMaxShowAlways = m_IsMaxShowAlways;
  m_OldMinShowLimit    = m_MinShowLimit;
  m_OldMaxShowLimit    = m_MaxShowLimit;
  m_OldScaleBegin      = m_ScaleBegin;
  m_OldScaleEnd        = m_ScaleEnd;
  m_OldClassLaw        = m_ClassLaw;
  m_OldClassLoad       = m_ClassLoad;
  m_OldClassTable      = m_ClassTable;
  m_OldIsTextInvisible = m_IsTextInvisible;

  if( m_IsScaleAlways )
    m_ScaleBegin = m_ScaleEnd = 5000;

  ksi_obj ksi_cpl = ksi_eval_str( "concret-param-list" );
  ksi_obj ksi_apl = ksi_eval_str( "abstract-param-list" );
  ksi_obj ksi_cln = KSI_MK_SINT(  m_ClassNumber );

  m_bConcretClass = KSI_TRUE_P( ksi_assv_ref( ksi_cpl, ksi_cln ));

  m_KsiParamList = IsClass() ?
    ksi_copy_tree( ksi_assv_ref( m_bConcretClass ? ksi_cpl : ksi_apl, ksi_cln )) :
    ksi_nil;

  if( KSI_FALSE_P( m_KsiParamList ))
    m_KsiParamList = ksi_nil;

  m_Hint = "";
  GetParam( "hint", &m_Hint );

  ksi_obj ksi_cont = ksi_nil;
  GetParam( "contents", &ksi_cont );

  m_bNewGraphics = m_IsIcon = m_IsPolyline = m_IsPolygon = m_IsText = m_IsFootnote = 0;

  for( int i = CELEM( asa) ; --i >= 0 ;
       *isa[i] |= KSI_TRUE_P( ksi_memv( ksi_str02key( asa[i].m_Str ), ksi_cont )));

  UpdateData( 0 );

  int bClosed = IsClass() && !IsUserClass();

  int bNm = !m_ClassName.IsEmpty();
  GetDlgItem( IDC_AAD_REMOVE_BUTTON )->EnableWindow( !bClosed && bNm && IsClass() && ( IsUserClass() || m_bConcretClass ) && !IsAbstractClass());
  GetDlgItem( IDC_AAD_CLASS_CHANGE_BUTTON )->EnableWindow( IsClass() && bNm );
  GetDlgItem( IDOK )->EnableWindow( bNm );
  CheckDlgButton( IDC_AAD_CLASS_CLOSED_CHECK, bClosed );
  SetDlgItemInt( IDC_AAD_CLASS_NUMBER_EDIT, m_ClassNumber );
  GetDlgItem( IDC_AAD_CLASS_NUMBER_EDIT )->EnableWindow( !IsClass());
  GetDlgItem( IDC_SDLG_FORE_BUTTON )->EnableWindow( IsAbstractObject( m_ClassNumber ) || IsConcretObject( m_ClassNumber ) && m_bClosedObjEdition );

  OnMinShowAlwaysClicked();
  OnMaxShowAlwaysClicked();

  CPolygonParamDialog::SetUsed( 0 );
  CLineParamDialog::SetUsed( 0 );
  CIconParamDialog::SetUsed( 0 );
  CTextParamDialog::SetUsed( 0 );
}
//======================================================================
void
CAbstractDialog::DoDataExchange( CDataExchange* pDX )
{
  DDX_Check( pDX, IDC_AAD_ICON_RADIO, m_IsIcon );
  DDX_Check( pDX, IDC_AAD_POLYLINE_RADIO, m_IsPolyline );
  DDX_Check( pDX, IDC_AAD_POLYGON_RADIO, m_IsPolygon );
  DDX_Check( pDX, IDC_AAD_TEXT_CHECK, m_IsText );
  DDX_Check( pDX, IDC_AAD_FOOTNOTE_CHECK, m_IsFootnote );
  DDX_Check( pDX, IDC_AAD_MIN_SHOW_ALWAYS_CHECK, m_IsMinShowAlways );
  DDX_Check( pDX, IDC_AAD_MAX_SHOW_ALWAYS_CHECK, m_IsMaxShowAlways );
  DDX_Check( pDX, IDC_AAD_SCALE_ALWAYS_CHECK, m_IsScaleAlways );
  DDX_Check( pDX, IDC_AAD_TABLE_CHECK, m_ClassTable );
  DDX_Check( pDX, IDC_AAD_START_LAW_CHECK, m_ClassLaw );
  DDX_Check( pDX, IDC_AAD_START_LOAD_CHECK, m_ClassLoad );
  DDX_Check( pDX, IDC_AAD_TEXT_VISIBLE_CHECK, m_IsTextInvisible );
  DDX_Check( pDX, IDC_AAD_CLASS_FIND_CHECK, m_bClassFind );
  DDX_Check( pDX, IDC_AAD_SCALE_ALWAYS_CHECK, m_IsScaleAlways );
  DDX_Text( pDX, IDC_AAD_HINT_EDIT, m_Hint );
  DDX_Text( pDX, IDC_AAD_MIN_SHOW_LIMIT_EDIT, m_MinShowLimit );
  DDX_Text( pDX, IDC_AAD_MAX_SHOW_LIMIT_EDIT, m_MaxShowLimit );
  DDX_Text( pDX, IDC_AAD_SCALE_BEGIN_EDIT, m_ScaleBegin );
  DDX_Text( pDX, IDC_AAD_SCALE_END_EDIT, m_ScaleEnd );
  if( !pDX->m_bSaveAndValidate )
  {
    OnMinShowAlwaysClicked();
    OnMaxShowAlwaysClicked();
    OnScaleAlwaysClicked();
  }
}
//======================================================================
void
CAbstractDialog::OnMinShowAlwaysClicked()
{
  GetDlgItem( IDC_AAD_MIN_SHOW_LIMIT_EDIT )->EnableWindow( !IsDlgButtonChecked( IDC_AAD_MIN_SHOW_ALWAYS_CHECK ));
}
//======================================================================
void
CAbstractDialog::OnMaxShowAlwaysClicked()
{
  GetDlgItem( IDC_AAD_MAX_SHOW_LIMIT_EDIT )->EnableWindow( !IsDlgButtonChecked( IDC_AAD_MAX_SHOW_ALWAYS_CHECK ));
}
//======================================================================
void
CAbstractDialog::OnGraphicsChange()
{
  static int m[] = { IDC_AAD_ICON_RADIO, IDC_AAD_POLYGON_RADIO, IDC_AAD_POLYLINE_RADIO };
  static int* pm[] = { &m_IsIcon, &m_IsPolygon, &m_IsPolyline };
  int pd[] = { m_IsIcon, m_IsPolygon, m_IsPolyline };
  int IsText = m_IsText;
  int bClosed = IsClass() && ! IsUserObject( m_ClassNumber );

  UpdateData( !bClosed );
  if( bClosed )
    return;

  if( IsText == m_IsText )
    for( int i = CELEM( m ) ; --i >= 0 ; )
      if( IsDlgButtonChecked( m[i] ))
        CheckDlgButton( m[i], *pm[i] = pd[i] ^ 1 );
      else
        *pm[i] = 0;

  CButton* pFb = (CButton*)GetDlgItem( IDC_AAD_FOOTNOTE_CHECK );

  int bCh = IsDlgButtonChecked( IDC_AAD_FOOTNOTE_CHECK );
  int bFe = IsDlgButtonChecked( IDC_AAD_ICON_RADIO ) &&
            IsDlgButtonChecked( IDC_AAD_TEXT_CHECK );

  CheckDlgButton( IDC_AAD_FOOTNOTE_CHECK,  bCh && bFe );
  GetDlgItem( IDC_AAD_FOOTNOTE_CHECK )->EnableWindow( bFe );

  int bNm = !m_ClassName.IsEmpty();
  int bGr = bNm && ( m_IsIcon || m_IsPolygon || m_IsPolyline || m_IsText );
  GetDlgItem( IDC_SDLG_FORE_BUTTON )->EnableWindow( bGr && ( IsUserClass() || IsAbstractClass() || m_bConcretClass ));
  GetDlgItem( IDOK )->EnableWindow( 0 );
  m_bNewGraphics = 1;
}
//======================================================================
void
CAbstractDialog::OnRemoveClicked()
{
  RemoveClass( 1 );
}
//======================================================================
void
CAbstractDialog::RemoveClass( int bAsk )
{
  if( !bAsk ||
      AskBox( Compose( "Все объекты данного класса будут удалены!\n\n"
                       "Удалять класс \"", m_ClassName, "\"?", 0 )) == IDYES )
  {
    CWaitCursor wc;
    m_pView->m_MapStore.RemoveAllObjects( m_ClassNumber );
    if( bAsk )
    {
      m_pObjDef->Remove( m_ClassNumber );
      KsiCall( "update-abstract!",
               GetParamPath(), KSI_MK_BOOL( m_bConcretClass ),
               ksi_false, KSI_MK_SINT( m_ClassNumber ), ksi_false, 0 );
      CDrawObject::InitShapeNdx( m_ClassNumber );
      m_pView->GetLayer( m_ClassNumber )->SetVisible( 0 );
      int bClearColorTab = 1;
      m_pView->UpdateColorTab( 0, 0, 0, bClearColorTab );
      m_ClassTree.DeleteItem( m_ClassTree.FindTreeNode( m_ClassNumber ));
      SaveTreeNode( m_ClassTree.GetRootItem(), CLASS_RANGE );
      m_pView->GetDocument()->ModifyObjectMenu();
      m_pObjDef->Save();
    }

    m_pView->ClearSelection( 0, 0 );

    CObjList list;
    for( m_pView->m_ClipData.SelectByClass( m_ClassNumber, list ) ;
         !list.IsEmpty() ; m_pView->ShowObjectBox( list.RemoveHead()));

    m_pView->GetLayer( m_ClassNumber )->RemoveAll( 0 );
    m_pView->m_Clip.SetRectEmpty();
    m_pView->Invalidate( 0 );
    m_pView->m_Undo.RemoveAll();
  }
}
//======================================================================
void 
CAbstractDialog::OnSelectClassUp()
{ 
  if( m_ClassTree.MoveSelected( 1 ))
  {
    SaveTreeNode( m_ClassTree.GetRootItem(), CLASS_RANGE );
    m_pView->m_ClipData.SetLayersRange( m_pObjDef );
  }
}
//======================================================================
void
CAbstractDialog::OnSelectClassDn()
{ 
  if( m_ClassTree.MoveSelected( 0 ))
  {
    SaveTreeNode( m_ClassTree.GetRootItem(), CLASS_RANGE );
    m_pView->m_ClipData.SetLayersRange( m_pObjDef );
  }
}
//======================================================================
void
CAbstractDialog::OnCancel()
{
  if( !m_ClassTree.IsDragging())
    if( m_ClassTree.GetEditControl())
      m_ClassTree.SetFocus();
    else
    {
      DestroyWindow();
      m_bActive = 0;
    }
}
//========================================================================
void
CAbstractDialog::Activate( CMapView* pView )
{
  if( Create( IDD_ABSTRACT_DIALOG, m_pView = pView ))
  {
    m_bActive = 1;
    ShowWindow( SW_SHOW );
  }
}
//========================================================================
ksi_obj
CAbstractDialog::GetParamPath()
{
  return ksi_str02string( App.GetProjectFilePath( m_bConcretClass ? "concret.scm" : "abstract.scm" ));
}
//=========================================================================
int
CAbstractDialog::GetLimitScale( int s )
{
	return m_IsScaleAlways ? s :
	       s < m_ScaleBegin ? m_ScaleBegin :
	       s > m_ScaleEnd   ? m_ScaleEnd   :
	       s;
}
//=========================================================================
void
CAbstractDialog::OnOK()
{
  CWaitCursor wc;

  CEdit* pEdit = m_ClassTree.GetEditControl();
  if( pEdit )
  {
    HTREEITEM item = m_ClassTree.GetSelectedItem();
    CString oldName( m_ClassName );
    CString newName;
    pEdit->GetWindowText( newName );
    RemoveSpaces( newName.GetBuffer( 256 ));
    newName.ReleaseBuffer();
    int cNum = m_pObjDef->GetClass( newName );
    if( !newName.IsEmpty() && !cNum )
    {
      App.WriteProfileString( "MAP_OBJECTS", oldName, 0 );
      m_ClassTree.SetItemText( item, m_ClassName = newName );
      GetDlgItem( IDC_AAD_CLASS_NAME_EDIT )->SetWindowText( m_ClassName );
    }
    m_ClassTree.SetFocus();
  }

  UpdateData();

  int sb = m_ScaleBegin;
  int se = m_ScaleEnd;
  m_ScaleBegin = min( sb, se );
  m_ScaleEnd   = max( sb, se );

  int ClassNumber = m_ClassNumber;
  m_ClassNumber = GetDlgItemInt( IDC_AAD_CLASS_NUMBER_EDIT );
  if( m_ClassNumber != ClassNumber && IsClass())
  {
    ErrBox( "Выбран номер существующего класса!" );
    m_ClassNumber = ClassNumber;
    return;
  }

  int bNewClass = !IsClass();
  int bUserClass = IsUserClass();
  int bAbstract2concret = IsAbstractObject( m_ClassNumber ) && m_bConcretClass;
  int par[] = { m_ClassNumber, m_ParentNumber, bNewClass ? FULL_RANGE : m_Range,
                m_IsMinShowAlways ? 0 : m_MinShowLimit,
                m_IsMaxShowAlways ? 0 : m_MaxShowLimit,
                m_IsScaleAlways   ? 0 : m_ScaleBegin,
                m_IsScaleAlways   ? 0 : m_ScaleEnd,
                2 - m_ClassLaw, 2 - m_ClassLoad,
                m_ClassTable, m_IsTextInvisible };

  m_pObjDef->Add( m_ClassName, par );

  MapStore& MS = m_pView->m_MapStore;

  MS.ClsInf.SetMinScaleLimit( m_ClassNumber, m_IsMinShowAlways ? 0 : m_MinShowLimit );
  MS.ClsInf.SetMaxScaleLimit( m_ClassNumber, m_IsMaxShowAlways ? 0 : m_MaxShowLimit );
  MS.ClsInf.SetParentNum( m_ClassNumber, m_ParentNumber );

  int ind = m_ClassNumber >> 3, msk = 1 << ( m_ClassNumber & 7 );

  if( m_ClassLoad )
  {
    m_pView->m_LoadMap[ind] |= msk;
    m_pView->m_ShowMap[ind] |= msk;
  }
  else
  {
    m_pView->m_LoadMap[ind] &= ~msk;
    m_pView->m_ShowMap[ind] &= ~msk;
  }

  MS.SetLoadMap( m_pView->m_LoadMap );

  pDocument->ModifyObjectMenu();

  ksi_obj ksi_classNum = KSI_MK_SINT( m_ClassNumber );
  ksi_obj ksi_parentNum = KSI_MK_SINT( m_ParentNumber );

  m_pView->m_LocalLaw[m_ClassNumber] = 2 - m_ClassLaw;

  int  bRedraw = 0;
  long bc[CLASS_RANGE];
  int  bParCh[CLASS_RANGE];
  int  ParChTot = 0; 
  memset( bParCh, 0, sizeof bParCh );
  long nbc = m_pObjDef->GetBottom( m_ClassNumber, (int*)bc );
  for( int i = nbc ; --i >= 0 ; )
  {
    int t = bc[i];
    if( m_OldClassLaw != m_ClassLaw )
    {
      m_pObjDef->SetEdit( t, 2 - m_ClassLaw );
      m_pView->m_LocalLaw[bc[i]] = 2 - m_ClassLaw;
    }
    if( m_OldClassLoad != m_ClassLoad )
      m_pObjDef->SetLoad( t, 2 - m_ClassLoad );
    if( m_OldIsMinShowAlways != m_IsMinShowAlways ||
        m_OldIsMaxShowAlways != m_IsMaxShowAlways ||
        !m_IsMinShowAlways && m_OldMinShowLimit != m_MinShowLimit ||
        !m_IsMaxShowAlways && m_OldMaxShowLimit != m_MaxShowLimit )
    {
      ++ParChTot;
      bParCh[t] = 1;
      m_pObjDef->SetMinMaxLimit( t, m_IsMinShowAlways ? 0 : m_MinShowLimit, m_IsMaxShowAlways ? 0 : m_MaxShowLimit );
    }
    if( m_OldIsScaleAlways != m_IsScaleAlways ||
        !m_IsScaleAlways && ( m_OldScaleBegin != m_ScaleBegin || m_OldScaleEnd != m_ScaleEnd ))
    {
      bRedraw = 1;
      m_pObjDef->SetMinMaxScale( t, m_IsScaleAlways ? 0 : m_ScaleBegin, m_IsScaleAlways ? 0 : m_ScaleEnd );
    }
    if( m_OldIsTextInvisible != m_IsTextInvisible )
      m_pObjDef->SetTextInvisible( t, m_IsTextInvisible );
    if( m_OldClassTable != m_ClassTable )
      m_pObjDef->SetTable( t, m_ClassTable );
  }

  m_OldIsScaleAlways   = m_IsScaleAlways;
  m_OldIsMinShowAlways = m_IsMinShowAlways;
  m_OldIsMaxShowAlways = m_IsMaxShowAlways;
  m_OldMinShowLimit    = m_MinShowLimit;
  m_OldMaxShowLimit    = m_MaxShowLimit;
  m_OldScaleBegin      = m_ScaleBegin;
  m_OldScaleEnd        = m_ScaleEnd;
  m_OldClassLaw        = m_ClassLaw;
  m_OldClassLoad       = m_ClassLoad;
  m_OldClassTable      = m_ClassTable;
  m_OldIsTextInvisible = m_IsTextInvisible;

  ksi_obj contents = ksi_nil;
  ksi_obj ksi_oldPar = ksi_copy_tree( m_KsiParamList );

  GetParam( "contents", &contents );
  ksi_obj old_contents = KSI_PLIST_P( contents ) ? ksi_copy_tree( contents ) : contents;

  int bOplg = CPolygonParamDialog::IsUsed() ?
    CPolygonParamDialog::IsBound() :
    KSI_TRUE_P( ksi_memq( keyOplg, contents ));

  int bSplg = CPolygonParamDialog::IsUsed() ?
    CPolygonParamDialog::IsSmart() :     
    KSI_TRUE_P( ksi_memq( keySplg, contents ));

  int bBpll = CLineParamDialog::IsUsed() ?
    CLineParamDialog::IsBound() :
    KSI_TRUE_P( ksi_memq( keyBpll, contents ));

  contents = m_IsIcon     ? KSI_LIST1( keyIcon ) :
             m_IsPolygon  ? KSI_LIST1( bOplg ? keyOplg : bSplg ? keySplg : keyFplg ) :
             m_IsPolyline ? KSI_LIST1( bBpll ? keyBpll : keyPolyline ) :
                            ksi_nil;

  if( m_IsText )
    contents = ksi_cons( keyText, contents );

  if( m_IsFootnote )
    contents = ksi_cons( keyFootnote, contents );

  m_KsiParamList = ksi_assv_set_x( m_KsiParamList, keyHint, ksi_str02string( m_Hint ));

  if( m_IsIcon && CIconParamDialog::IsUsed())
  {
    CBmpDefArray* pBDA = pDocument->GetBmpDefArray( !m_bConcretClass );

    int ref = CIconParamDialog::GetIcomRef();
    if( ref < 0 )
      ref = pBDA->GetFreeId();
    else
    if( bAbstract2concret )
    {
      CBmpDefArray* pBDA = pDocument->GetBmpDefArray( 1 );
	  int i;
      for( i = pBDA->GetSize() ; --i >= 0 && pBDA->GetAt( i )->GetBmpId() != ref ; );
      if( i >= 0 )
      {
        delete pBDA->GetAt( i );
        pBDA->RemoveAt( i );
      }
      char szEntry[64];
      sprintf( szEntry, "%d", ref );
      App.WriteProfileString( CIconParamDialog::SectName( 0 ), szEntry, 0 );
      ref = pBDA->GetFreeId();
    }

    CBmpDef* pBmp = new CBmpDef( ref,
                                 CIconParamDialog::GetBmpPath(),
                                 CIconParamDialog::GetBmpName(),
                                 CIconParamDialog::GetScaleSingle());

	int i;
	for( i = pBDA->GetSize() ; --i >= 0 && !( *pBDA->GetAt( i ) == *pBmp ) ; );
    if( i >= 0 )
    {
      pBmp->SetBmpId( ref = pBDA->GetAt( i )->GetBmpId());
      delete pBDA->GetAt( i );
      pBDA->SetAt( i, pBmp );
    }
    else
      pBDA->Add( pBmp );

    char szEntry[64], szValue[256];
    sprintf( szEntry, "%d", ref );
    sprintf( szValue, "%s %d",
             CIconParamDialog::GetBmpName(),
             CIconParamDialog::GetScaleSingle());
    App.WriteProfileString( CIconParamDialog::SectName( m_bConcretClass ), szEntry, szValue );
    m_KsiParamList = ksi_assv_set_x( m_KsiParamList, keyIconref, KSI_MK_SINT( ref ));
  }

  int bClearColorTab = 1;

  if( m_IsPolygon && CPolygonParamDialog::IsUsed())
  {
    int foreKey = CPolygonParamDialog::IsColored() ?
      m_pView->UpdateColorTab(
        CPolygonParamDialog::GetForeMonitorColor(),
        CPolygonParamDialog::GetForePrinterColor(),
        CPolygonParamDialog::GetForePrinterBWColor(),
        bClearColorTab ) : -1;

    int backKey = CPolygonParamDialog::IsOpaque() ?
      m_pView->UpdateColorTab(
        CPolygonParamDialog::GetBackMonitorColor(),
        CPolygonParamDialog::GetBackPrinterColor(),
        CPolygonParamDialog::GetBackPrinterBWColor(),
        bClearColorTab ) : CPolygonParamDialog::IsHatched() ? -1 : 0;

    ksi_obj ksi_fs = CPolygonParamDialog::IsHatched() ?
      CPolygonParamDialog::m_HatchKey->KsiTranslate( CPolygonParamDialog::GetHatchStyle()) :
      CPolygonParamDialog::m_FillKey->KsiTranslate( UINT( BS_SOLID ));

    ksi_obj pars[] =
    {
      PARDEF( "fill-style", ksi_fs ),
      PARDEF( "fore-color", ksi_int2num( foreKey )),
      PARDEF( "back-color", ksi_int2num( backKey )),
      0
    };
    AddParam( pars );
  }

  int bBound = m_IsPolygon &&
               ( CPolygonParamDialog::IsBound() || CPolygonParamDialog::IsSmart());

  if(( m_IsPolyline || bBound ) && CLineParamDialog::IsUsed())
  {
    int key = m_pView->UpdateColorTab( CLineParamDialog::GetMonitorColor(),
                                       CLineParamDialog::GetPrinterColor(),
                                       CLineParamDialog::GetPrinterBWColor(),
                                       bClearColorTab );

    int lst = CLineParamDialog::GetLineStyle();
    CString s;
    s.Format( "t%d", lst );
    ksi_obj pars[] =
    {
      PARDEF( "polyline-style", lst < TEMPLATE_MIN_NUM ? CLineParamDialog::m_StyleKey->KsiTranslate( CLineParamDialog::GetLineStyle()) : ksi_str02key( s )),
      PARDEF( "polyline-width", ksi_int2num( CLineParamDialog::GetLineWidth( 1 ))),
      PARDEF( "polyline-color", ksi_int2num( key )),
      0
    };
    AddParam( pars );
  }

  if( m_IsText && CTextParamDialog::IsUsed())
  {
    char ts[3];
    char* tp = ts;
    if( CTextParamDialog::IsItalic())
      *tp++ = 'I';
    if( CTextParamDialog::IsUnderline())
      *tp++ = 'U';
    *tp = 0;

    int key = m_pView->UpdateColorTab(
      CTextParamDialog::GetMonitorColor(),
      CTextParamDialog::GetPrinterColor(),
      CTextParamDialog::GetPrinterBWColor(),
      bClearColorTab );

    ksi_obj pars[] =
    {
      PARDEF( "text-hint", ksi_str02string( CTextParamDialog::GetHint())),
      PARDEF( "text-select-visible", CTextParamDialog::IsSelectVisible() ? ksi_true : ksi_false ),
      PARDEF( "font-face", ksi_int2num( pDocument->m_FontTab.Translate( CTextParamDialog::GetFontFace()))),
      PARDEF( "font-weight", ksi_str02string( CTextParamDialog::IsBold() ? "Bold" : "Normal")),
      PARDEF( "font-style", ksi_str02string( ts )),
      PARDEF( "font-color", ksi_int2num( key )),
      PARDEF( "font-size", ksi_int2num( -CTextParamDialog::GetFontSize())),
      0
    };
    AddParam( pars );
  }

  m_KsiParamList = ksi_assv_set_x( m_KsiParamList, keyContents, contents );


  int bIsParentClass = m_pObjDef->GetDerived( m_ClassNumber );
  int bChCont = !bIsParentClass && !bNewClass && 
                 IsAbstractObject( m_ClassNumber ) &&
                 !KSI_EQUAL_P( contents, old_contents );

  int bClassRemoved = 0;
  if( bChCont )
  {
    int r = AskBox( Compose( "Состав объекта класса '", m_ClassName, "' изменен!\n"
                             "Удалить все объекты данного класса?", 0 ), 1 );
    switch( r )
    {
      case IDCANCEL:
        m_KsiParamList = ksi_assv_set_x( m_KsiParamList, keyContents, old_contents );
        UpdateSelectedClass();
        return;
      case IDYES:
        RemoveClass( 0 );
        bClassRemoved = 1;
    }
  }

  if( bAbstract2concret )
  {
    m_bConcretClass = 0;
    KsiCall( "update-abstract!",
             GetParamPath(), KSI_MK_BOOL( m_bConcretClass ),
             ksi_false, KSI_MK_SINT( m_ClassNumber ), ksi_false, 0 );
    m_bConcretClass = 1;
    m_ClassTree.SetItemImage( m_ClassTree.GetSelectedItem(), 2, 3 );
  }

  if(( bUserClass || m_bConcretClass ))
    KsiCall( "update-abstract!",
             GetParamPath(), KSI_MK_BOOL( m_bConcretClass ),
             ksi_true, ksi_classNum, m_KsiParamList, 0 );

  CDrawObject::InitShapeNdx( m_ClassNumber );

  bRedraw |= m_bClassTreeChanged;

  if( !bNewClass && !bIsParentClass &&
      ( bChCont && !bClassRemoved ||
        CIconParamDialog::IsIconChanged() || 
        KSI_PLIST_P( ksi_oldPar ) && !KSI_EQUAL_P( ksi_oldPar, m_KsiParamList )) &&
      AskBox( Compose( "Состав объекта класса '", m_ClassName, "' изменен!\n"
                       "Внести изменения в проект?", 0 )) == IDYES )
  {
    bRedraw = 1;
    m_pView->ClearSelection( 0, 0 );
    m_pView->m_Undo.RemoveAll();

    MS.StartClassScan( m_ClassNumber );

    CMainWin* pmw = (CMainWin*)AfxGetMainWnd();
    pmw->ShowProgress( 0, MS.GetMaxPosScan());

    int total = 0, succ = 0;
    CViewDC DC( m_pView );
    ksi_obj ksi_dc = ksi_make_win_dc( m_pView->GetSafeHwnd(), DC.GetSafeHdc());
    for( ;; )
    {
      TGstream buf;
      if( MS.NextClassScan( buf ) != R_OK )
        break;

      pmw->SetProgress( MS.GetPosScan());

      ++total;
      char* pSrc = buf.str();
      CBaseDataRec* pDrh = m_pView->GetDataRecHeadPtr();
      memcpy( pDrh->GetBriefPtr(), pSrc, pDrh->GetBriefSize());
      pSrc += pDrh->GetBriefSize();
      void* pData = pSrc;
      long vrs;
      MS.GetVersion( &vrs );
      ksi_obj ksi_img = ConvertObject( vrs, &pData, 1 );
      ksi_obj ksi_class  = ksi_int2num( pDrh->GetIdent().Class());
      ksi_obj ksi_status = ksi_int2num( pDrh->GetIdent().Status());

      ksi_defsym( "is-convertion?", ksi_true, ksi_current_env());
      ksi_img = KsiCall( "rebuild-object",
                          ksi_class, ksi_status,
                          ksi_class, ksi_status,
                          ksi_nil, ksi_img, 0 );
      ksi_defsym( "is-convertion?", ksi_false, ksi_current_env());

      if( KSI_PLIST_P( ksi_img ))
      {
        CDrawObject* pObj = new CDrawObject( ksi_img, pDrh->GetIdent(), pDrh->GetNode());
        ++succ;

        CDrawObject* pOld = m_pView->m_ClipData.GetObjectPtr( pDrh->GetIdent());
        if( pOld )
        {
          m_pView->AddObject( pObj, 0 );
          m_pView->ScaleObject( pObj, m_pView->GetTrueScale());
          m_pView->ShowObjectBox( pOld );
          delete pOld;
        }
        else
        {
          CObjList lst;
          lst.AddHead( pObj );
          m_pView->SaveToStore( lst, 0, 0 );
          delete pObj;
        }
      }
    }
    pmw->HideProgress();
    char m[256];
    sprintf( m, "Изменено объектов: %d из %d.", succ, total );
    InfBox( m );
  }

  if( ParChTot )
  {
    CString s;
    int k = ParChTot/10%10 == 1 ? 0 : ParChTot%10;
    char* e = k == 1 ? "" : k > 0 && k < 5 ? "а" : "ов";
    s.Format( "%d класс%s изменили границы отображения!\n"
              "Внести изменения в проект?", ParChTot, e );
    if( AskBox( s ) == IDYES )
    {
      bRedraw = 1;
      MS.StartClassScan( 0 );
      CMainWin* pmw = (CMainWin*)AfxGetMainWnd();
      pmw->ShowProgress( 0, MS.GetMaxPosScan());
      int total = 0;
      for( ;; )
      {
        TGstream buf;
        if( MS.NextClassScan( buf ) != R_OK )
          break;
        long ps = MS.GetPosScan();
        pmw->SetProgress( ps );
        char* pSrc = buf.str();
        CBaseDataRec* pDrh = m_pView->GetDataRecHeadPtr();
        memcpy( pDrh->GetBriefPtr(), pSrc, pDrh->GetBriefSize());
        if( bParCh[pDrh->GetClass()] )
        {
          MS.SpaceIndex.ReplaceObject(
            pDrh->GetBox(), ps - pDrh->GetSize(), 
            m_IsMinShowAlways ? 0 : m_MinShowLimit,
            m_IsMaxShowAlways ? 0 : m_MaxShowLimit,
            pDrh->GetNode());        
          ++total;
        }
      }
      pmw->HideProgress();
      CString s;
      s.Format( "Изменено объектов: %d", total );
      InfBox( s );
    }
  }

  if( bNewClass )
  {
    InsertTreeNode( m_ClassTree.GetSelectedItem(), m_ClassNumber );

    HTREEITEM item = m_ClassTree.FindTreeNode( m_ClassNumber );
    m_ClassTree.SelectItem( item );
    m_pView->GetLayer( m_ClassNumber )->SetVisible( 1 );
  }

  SaveTreeNode( m_ClassTree.GetRootItem(), CLASS_RANGE );
  m_pObjDef->Save();
  pDocument->ModifyObjectMenu();

  if( m_bClassTreeChanged )
  {
    m_ClassTree.SelectItem( m_ClassTree.FindTreeNode( m_ClassNumber ));
    m_Range = m_pObjDef->GetRange( m_ClassNumber );
    m_pView->m_ClipData.SetLayersRange( m_pObjDef );
    m_bClassTreeChanged = 0;
  }

  if( bRedraw )
  {
    m_pView->m_Clip.SetRectEmpty();
    m_pView->Invalidate( 0 );
  }

  UpdateSelectedClass();
}
//======================================================================
void
CAbstractDialog::OnFore()
{
  UpdateData();

  int LastClassNumber = GetDlgItemInt( IDC_AAD_CLASS_NUMBER_EDIT );

  if( LastClassNumber != m_ClassNumber )
  {
    if( LastClassNumber <= 0 || LastClassNumber >= CLASS_RANGE )
    {
      ErrBox( "Вы выбрали недопустимый номер класса!" );
      return;
    }

    m_ClassNumber = LastClassNumber;

    if( IsClass())
    {
      ErrBox( "Вы выбрали номер существующего класса!" );
      return;
    }
  }

  HTREEITEM cit = m_ClassTree.FindTreeNode( m_ClassName, 0 );
  if( cit && cit != m_ClassTree.GetSelectedItem())
  {
    ErrBox( "Класс с таким именем уже существует!" );
    return;
  }

  if( m_ClassNumber < 0 )
    ErrBox( "Список классов переполнен!" );
  else
  {
    UINT R = m_IsIcon     ? CIconParamDialog( this ).DoModal() :
             m_IsPolygon  ? CPolygonParamDialog( this ).DoModal() :
             m_IsPolyline ? CLineParamDialog( this ).DoModal() :
             m_IsText     ? CTextParamDialog( this ).DoModal() : IDRETRY;
    if( R == IDCANCEL )
      OnCancel();
    else
    if( R != IDRETRY )
      OnOK();
  }
}
//======================================================================
ksi_obj CSubdialog::m_KsiParamList = ksi_nil;
//======================================================================
BEGIN_MESSAGE_MAP( CSubdialog, CDialog )
  ON_BN_CLICKED( IDC_SDLG_BACK_BUTTON, OnBackClicked )
  ON_BN_CLICKED( IDC_SDLG_STOP_BUTTON, OnStopClicked )
END_MESSAGE_MAP()
//======================================================================
CSubdialog::CSubdialog( UINT DlgId, CWnd* pOwner ) :
  CDialog( DlgId, pOwner )
{}
//======================================================================
int
CSubdialog::IsUserClass()
{
  return !IsClass() && !CAbstractDialog::IsClassConcret() || IsAbstractObject( CAbstractDialog::GetClassNumber());
}
//======================================================================
int
CSubdialog::GetFreeClass( int bFromBegin )
{
  if( bFromBegin )
  {
    for( int t = 0 ; ++t < CLASS_RANGE && ( m_pObjDef->IsClass( t ) || IsObject( t )) ; );
    return t < CLASS_RANGE ? t : -1;
  }
  else
  {
    for( int t = CLASS_RANGE ; --t >= 0  && ( m_pObjDef->IsClass( t ) || IsObject( t )) ; );
    return t;
  }
}
//======================================================================
int
CSubdialog::IsClass()
{
  return m_pObjDef->IsClass( CAbstractDialog::GetClassNumber());
}
//======================================================================
int
CSubdialog::IsAbstractClass()
{
  return m_pObjDef->GetDerived( CAbstractDialog::GetClassNumber());
}
//======================================================================
void
CSubdialog::AddParam( ksi_obj* pArr )
{
  for( ; *pArr ; ++pArr )
    m_KsiParamList = ksi_assv_set_x( m_KsiParamList, KSI_CAR( *pArr ), KSI_CDR( *pArr ));
}
//========================================================================
int
CSubdialog::GetParam( LPCSTR name, void* pParam )
{
//  KsiCall( "tk-box", m_KsiParamList, 0 )
  long tmp;
  if( !pParam )
    pParam = &tmp;

  ksi_obj ksi_par = ksi_assv_ref( m_KsiParamList, ksi_str02key( name ));
  if( ksi_par == ksi_false )
    return 0;

  if( KSI_LIST_P( ksi_par ))
    *((ksi_obj*)pParam) = ksi_par;
  else
  if( KSI_NUM_P( ksi_par ))
    *((int*)pParam) = ksi_num2int( ksi_par );
  else
  if( KSI_STR_P( ksi_par ))
    *((CString*)pParam) = KSI_STR_PTR( ksi_par );
  else
  if( KSI_KEY_P( ksi_par ))
    *((CString*)pParam) = KSI_KEY_PTR( ksi_par );
  else
  if( ksi_par == ksi_true )
    *((int*)pParam) = 1;
  else
  if( ksi_par == ksi_false )
    *((int*)pParam) = 0;
  else
    return 0;

  return 1;
}
//======================================================================
void
CSubdialog::OnBackClicked()
{
  UpdateData();
  EndDialog( IDRETRY );
}
//======================================================================
//======================================================================
int CIconParamDialog::m_bIconChanged  = 0;
int CIconParamDialog::m_Ref           = 0;
int CIconParamDialog::m_ScaleExample  = 5000;
int CIconParamDialog::m_ScaleSingle   = 5000;
int CIconParamDialog::m_Size          = ICON_MIN_SIZE;

CString CIconParamDialog::m_BmpPath;
//======================================================================
BEGIN_MESSAGE_MAP( CIconParamDialog, CSubdialog )
  ON_WM_HSCROLL()
  ON_LBN_SELCHANGE( IDC_IPD_ICON_LIST, OnIconSelected )
  ON_EN_CHANGE( IDC_IPD_SCALE_EDIT, ShowExample )
  ON_BN_CLICKED( IDC_SDLG_FORE_BUTTON, OnFore )
END_MESSAGE_MAP()
//======================================================================
CIconParamDialog::CIconParamDialog( CWnd* pOwner ) :
  CSubdialog( IDD_ICON_PARAM_DIALOG, pOwner )
{}
//======================================================================
LPCTSTR
CIconParamDialog::SectName( int bC )
{
  return bC ? "CONCRET_BITMAPS" : "MAP_BITMAPS";
}
//======================================================================
CString
CIconParamDialog::GetBmpName()
{
  char iv[_MAX_DRIVE], ip[_MAX_DIR], in[_MAX_FNAME], ie[_MAX_EXT];
  _splitpath( m_BmpPath, iv, ip, in, ie );
  return in;
}
//======================================================================
int
CIconParamDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  m_IconListBox.SubclassDlgItem( IDC_IPD_ICON_LIST, this );
  m_ExampleBox.SubclassDlgItem( IDC_IPD_EXAMPLE_BOX, this );
  m_IconListBox.SetColumnWidth( ICON_SIZE );
  m_IconListBox.SetItemHeight( 0, ICON_SIZE );
  CSliderCtrl* pSCTRL = (CSliderCtrl*)GetDlgItem( IDC_IPD_ICON_SIZE_SLIDER );
  pSCTRL->SetRange( ICON_MIN_SIZE, ICON_MAX_SIZE );

  char wild[_MAX_PATH];

  CString path( App.GetProjectFilePath( "bitmaps\\*.*" ));

  m_OldSel = 0;
  m_Ref = -1;
  GetParam( "icon-ref", &m_Ref );
  char cref[_MAX_FNAME+_MAX_EXT];
  sprintf( cref, "%d", m_Ref );

  m_IconParam = App.GetProfileString( SectName( CAbstractDialog::IsClassConcret()), cref );
  sscanf( m_IconParam, "%s", cref );
  strcat( cref, ".bmp" );

  MakePath( path, wild, "bmp", "*" );

  CFileFind ff;
  for( int h = ff.FindFile( wild ) ; h ; )
  {
    h = ff.FindNextFile();
    CString fn( ff.GetFileName());
    MakePath( path, wild, "", fn );
    int ndx = m_IconListBox.AddString( wild );
    ASSERT( ndx >= 0 );
    if( !_stricmp( cref, fn ))
      m_OldSel = ndx;
  }

  if( !m_IconListBox.GetCount())
  {
    ErrBox( "Библиотека условных знаков пуста!" );
    EndDialog( IDRETRY );
    return 0;
  }

  m_IconListBox.SetCurSel( m_OldSel );
  OnIconSelected();
  m_OldScale = m_ScaleSingle;

  GetDlgItem( IDOK )->EnableWindow( IsClass() && !CAbstractDialog::IsNewGraphics()
                                    || !CAbstractDialog::IsText());
  GetDlgItem( IDC_SDLG_FORE_BUTTON )->EnableWindow( CAbstractDialog::IsText());
  m_bIconChanged = 0;

  return 1;
}
//======================================================================
void
CIconParamDialog::OnIconSelected()
{
  m_Size = ICON_MIN_SIZE + ICON_MAX_SIZE >> 1;
  m_IconListBox.GetText( m_IconListBox.GetCurSel(), m_BmpPath );

  char name[256];
  sscanf( m_IconParam, "%s %d", name, &m_ScaleSingle );

  m_ExampleBox.DeleteString( 0 );
  m_ExampleBox.AddString( m_BmpPath );

  HBITMAP hBmp = (HBITMAP)LoadImage( 0, m_BmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

  if( !hBmp )
    return;

  BITMAP bmps;
  CBitmap::FromHandle( hBmp )->GetBitmap( &bmps );
  int num = CIconParamDialog::m_Size;
  int den = max( bmps.bmWidth, bmps.bmHeight );
  m_ScaleExample = MulDiv( m_ScaleSingle, den, num );
  UpdateData( 0 );
  ShowExample();
}
//======================================================================
void
CIconParamDialog::DoDataExchange( CDataExchange* pDX )
{
  DDX_Text( pDX, IDC_IPD_SCALE_EDIT, m_ScaleExample );
  bound( m_ScaleExample, MaxScale, MinScale );
  DDX_Slider( pDX, IDC_IPD_ICON_SIZE_SLIDER, m_Size );
  
  if( pDX->m_bSaveAndValidate )
    m_bIconChanged = m_OldSel != m_IconListBox.GetCurSel() ||
                     m_OldScale != m_ScaleSingle;
}
//======================================================================
void
CIconParamDialog::OnHScroll( UINT, UINT, CScrollBar* )
{
  ShowExample();
}
//======================================================================
void CIconParamDialog::ShowExample()
{
  UpdateData();
  m_ExampleBox.Invalidate();
}
//======================================================================
void
CIconParamDialog::OnFore()
{
  UpdateData();

  int r = CTextParamDialog( this ).DoModal();
  if( r != IDRETRY )
  {
    m_Used = 1;
    EndDialog( r );
  }
}
//======================================================================
void
CIconExampleBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
  CIconParamDialog* pIpd = (CIconParamDialog*)GetParent();

  CDC* pDC = CDC::FromHandle( lpDIS->hDC );
  pDC->SetStretchBltMode( STRETCH_HALFTONE );
  CDC bmpDC;
  bmpDC.CreateCompatibleDC( 0 );

  char bPath[_MAX_PATH], mPath[_MAX_PATH];

  GetText( lpDIS->itemID, bPath );
  MakePath( bPath, mPath, "msk" );

  HBITMAP hBmp = (HBITMAP)LoadImage( 0, bPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
  HBITMAP hMsk = (HBITMAP)LoadImage( 0, mPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

  if( !hBmp )
    return;

  BITMAP bmps;
  CBitmap::FromHandle( hBmp )->GetBitmap( &bmps );

  int num = CIconParamDialog::m_Size;
  int den = max( bmps.bmWidth, bmps.bmHeight );

  int W = MulDiv( bmps.bmWidth,  num, den );
  int H = MulDiv( bmps.bmHeight, num, den );

  CIconParamDialog::m_ScaleSingle = MulDiv( CIconParamDialog::m_ScaleExample, num, den );

  CRect r;
  GetClientRect( &r );
  CPoint cp = r.CenterPoint() - CSize( W / 2, H / 2 );

  if( hMsk )
  {
    bmpDC.SelectObject( CBitmap::FromHandle( hMsk ));
    pDC->StretchBlt( cp.x, cp.y, W, H,
                     &bmpDC, 0, 0, bmps.bmWidth, bmps.bmHeight,
                     SRCAND );
    DeleteObject( hMsk );
  }
  if( hBmp )
  {
    bmpDC.SelectObject( CBitmap::FromHandle( hBmp ));
    pDC->StretchBlt( cp.x, cp.y, W, H,
                     &bmpDC, 0, 0, bmps.bmWidth, bmps.bmHeight,
                     hMsk ? SRCINVERT : SRCCOPY );
    DeleteObject( hBmp );
  }
}
//======================================================================
//======================================================================
void
CIconListBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
  CDC* pDC = CDC::FromHandle( lpDIS->hDC );
  pDC->SetStretchBltMode( STRETCH_HALFTONE );
  CDC bmpDC;
  bmpDC.CreateCompatibleDC( 0 );

  char bPath[_MAX_PATH], mPath[_MAX_PATH];

  GetText( lpDIS->itemID, bPath );
  MakePath( bPath, mPath, "msk" );

  HBITMAP hBmp = (HBITMAP)LoadImage( 0, bPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
  HBITMAP hMsk = (HBITMAP)LoadImage( 0, mPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

  if( !hBmp )
    return;

  BITMAP bmps;
  CBitmap::FromHandle( hBmp )->GetBitmap( &bmps );

  CRect r( lpDIS->rcItem );
  r.InflateRect( -4, -4 );

  int s = min( r.Width(), r.Height());
  int S = max( bmps.bmWidth, bmps.bmHeight );
  int w = MulDiv( bmps.bmWidth, s, S );
  int h = MulDiv( bmps.bmHeight, s, S );
  CPoint c( r.CenterPoint());
  c.Offset( -w / 2, - h / 2 );

  if( hMsk )
  {
    bmpDC.SelectObject( CBitmap::FromHandle( hMsk ));
    pDC->StretchBlt( c.x, c.y, w, h,
                     &bmpDC, 0, 0, bmps.bmWidth, bmps.bmHeight, SRCAND );
    DeleteObject( hMsk );
  }
  if( hBmp )
  {
    bmpDC.SelectObject( CBitmap::FromHandle( hBmp ));
    pDC->StretchBlt( c.x, c.y, w, h,
                     &bmpDC, 0, 0, bmps.bmWidth, bmps.bmHeight, hMsk ? SRCINVERT : SRCCOPY );
    DeleteObject( hBmp );
  }

  r.InflateRect( 2, 2 );

  if( lpDIS->itemAction & ( ODA_SELECT | ODA_DRAWENTIRE ))
  {
    pDC->SelectStockObject( NULL_BRUSH );
    pDC->SelectStockObject( lpDIS->itemState & ODS_SELECTED ? BLACK_PEN : WHITE_PEN );
    pDC->Rectangle( &r );
  }
}
//======================================================================
//======================================================================
void
CColorCombo::Init( int* pIndex, int initVal, CIntAssocMap* pMap )
{
  if( m_bColor = pMap != 0 )
  {
    SetItemData( AddString( 0 ), -1 );
    int minKey, maxKey;
    pMap->GetMinMaxKey( &minKey, &maxKey );
    while( minKey <= maxKey )
      AddColor( pMap->TranslateKey( minKey++ ));
    initVal = pMap->TranslateKey( initVal );
  }
  else
    for( int i = 0 ; i < 256 ; i += 15 )
      SetItemData( AddString( 0 ), RGB( i, i, i ));

  int s = PicColor( initVal );
  if( pIndex )
    *pIndex = s;
}
//======================================================================
void
CColorCombo::AddNewColor()
{
  if( GetCurSel() == GetCount() - 1 )
  {
    CColorDialog cdlg;
    if( cdlg.DoModal() == IDOK )
      AddColor( cdlg.GetColor());
  }
}
//======================================================================
void
CColorCombo::AddColor( COLORREF clr )
{
  if( clr == -1 )
    return;

  int n = GetCount();

  for( int i = n ; --i >= 0 && GetItemData( i ) != clr ; );
  if( i < 0 )
  {
    InsertString( i = n - 1, 0 );
    SetItemData( i, clr );
  }
  SetCurSel( i );
}
//======================================================================
int
CColorCombo::PicColor( COLORREF clr )
{
  int d = 0xffffff, k = 0;
  if( clr != -1 )
    for( int i = GetCount() ; --i >= 0 ; )
    {
      int t = abs( GetItemData( i ) - clr );
      if( t < d )
      {
        d = t;
        k = i;
      }
    }
  SetCurSel( k );
  return k;
}
//======================================================================
void
CColorCombo::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
  int bNew = m_bColor && lpDIS->itemID == GetCount() - 1;

  CDC* pDC = CDC::FromHandle( lpDIS->hDC );
  CBrush brush( bNew ? RGB( 0xff, 0xff, 0xff ) : lpDIS->itemData );
  pDC->SelectObject( &brush );
  pDC->SelectStockObject( bNew ? NULL_PEN : BLACK_PEN );
  CRect r( lpDIS->rcItem );
  r.InflateRect( -2, -2 );
  pDC->Rectangle( &r );
  r.InflateRect( 2, 2 );

  if( bNew )
    pDC->DrawText( "Новый", &r, DT_CENTER | DT_VCENTER );

  if( lpDIS->itemAction & ( ODA_SELECT | ODA_DRAWENTIRE ))
  {
    pDC->SelectStockObject( NULL_BRUSH );
    pDC->SelectStockObject( lpDIS->itemState & ODS_SELECTED ? BLACK_PEN : WHITE_PEN );
    pDC->Rectangle( &r );
  }
}
//======================================================================
static COLORREF
CalcBWColor( COLORREF clr )
{
  for( int s = 0 ; clr ; s += clr & 0xff, clr >>= 8 );
  int c = s / 3;
  return c | ( c << 8 ) | ( c << 16 );
}
//======================================================================
int CTextParamDialog::m_IsBold = 0;
int CTextParamDialog::m_IsItalic = 0;
int CTextParamDialog::m_IsUnderline = 0;
int CTextParamDialog::m_IsSelectVisible = 0;
int CTextParamDialog::m_Scale = 5000;
int CTextParamDialog::m_FontSize = 0;
COLORREF CTextParamDialog::m_MonitorColor;
COLORREF CTextParamDialog::m_PrinterColor;
COLORREF CTextParamDialog::m_PrinterBWColor;
int CTextParamDialog::m_MonitorColorIndex;
int CTextParamDialog::m_PrinterColorIndex;
int CTextParamDialog::m_PrinterBWColorIndex;
CString CTextParamDialog::m_FontFace;
CString CTextParamDialog::m_TextHint;
//======================================================================
BEGIN_MESSAGE_MAP( CTextParamDialog, CSubdialog )
  ON_WM_HSCROLL()
  ON_EN_CHANGE( IDC_TPD_SCALE_EDIT, ShowExample )
  ON_CBN_SELCHANGE( IDC_TPD_MONITOR_COLOR_COMBO, OnMonitorColorChange )
  ON_CBN_SELCHANGE( IDC_TPD_PRINTER_COLOR_COMBO, OnPrinterColorChange )
  ON_CBN_SELCHANGE( IDC_TPD_FONT_COMBO, ShowExample )
  ON_BN_CLICKED( IDC_TPD_BOLD_CHECK, ShowExample )
  ON_BN_CLICKED( IDC_TPD_ITALIC_CHECK, ShowExample )
  ON_BN_CLICKED( IDC_TPD_UNDERLINE_CHECK, ShowExample )
END_MESSAGE_MAP()
//======================================================================
CTextParamDialog::CTextParamDialog( CWnd* pOwner ) :
  CSubdialog( IDD_TEXT_PARAM_DIALOG, pOwner )
{}
//======================================================================
void
CTextParamDialog::OnMonitorColorChange()
{
  m_MonitorColorCombo.AddNewColor();
  int i = m_MonitorColorCombo.GetCurSel();
  COLORREF clr = m_MonitorColorCombo.GetItemData( i );
  m_PrinterColorCombo.AddColor( clr );
  m_PrinterBWColorCombo.PicColor( CalcBWColor( clr ));
  ShowExample();
}
//======================================================================
void
CTextParamDialog::OnPrinterColorChange()
{
  m_PrinterColorCombo.AddNewColor();
}
//======================================================================
int
CTextParamDialog::OnInitDialog()
{
  m_MonitorColorCombo.SubclassDlgItem( IDC_TPD_MONITOR_COLOR_COMBO, this );
  m_PrinterColorCombo.SubclassDlgItem( IDC_TPD_PRINTER_COLOR_COMBO, this );
  m_PrinterBWColorCombo.SubclassDlgItem( IDC_TPD_PRINTER_BWCOLOR_COMBO, this );

  m_ExampleBox.SubclassDlgItem( IDC_TPD_EXAMPLE_BOX, this );
  m_ExampleBox.SetItemHeight( 0, 40 );
  m_ExampleBox.AddString( "" );

  CSliderCtrl* pSCTRL = (CSliderCtrl*)GetDlgItem( IDC_TPD_FONT_SIZE_SLIDER );
  pSCTRL->SetRange( MIN_FONT_SIZE, MAX_FONT_SIZE );


  m_FontSize = -DEF_FONT_SIZE;
  GetParam( "font-size", &m_FontSize );

  CClientDC dc( &m_ExampleBox );

  int DPI = dc.GetDeviceCaps( LOGPIXELSY );

  m_ExampleSize = MAX_FONT_SIZE / 2;
	int s = abs( m_FontSize ) * SmInUnit / 2.54 * DPI / m_ExampleSize;
  m_Scale = CAbstractDialog::GetLimitScale( s );
	m_ExampleSize = MulDiv( m_ExampleSize, s, m_Scale );

  CString spar;

  m_IsBold = 0;
  if( GetParam( "font-weight", &spar ))
    m_IsBold = spar == "Bold";

  m_IsItalic    = 0;
  m_IsUnderline = 0;
  if( GetParam( "font-style", &spar ))
  {
    m_IsItalic    = strchr( spar, 'I' ) != 0;
    m_IsUnderline = strchr( spar, 'U' ) != 0;
  }

  m_IsSelectVisible = 0;
  m_TextHint = "";
  GetParam( "text-select-visible", &m_IsSelectVisible );
  GetParam( "text-hint", &m_TextHint );

  CComboBox* pFontCombo = (CComboBox*)GetDlgItem( IDC_TPD_FONT_COMBO );

  int ff = 0;
  GetParam( "font-face", &ff );

  CStrAssocMap* pMap = &pDocument->m_FontTab;
  for( int i = 0 ; i < pMap->GetSize() ; ++i )
  {
    pFontCombo->AddString( pMap->GetStr( i ));
    if( pMap->GetKey( i ) == ff )
      pFontCombo->SetCurSel( i );
  }

  ff = 0;
  int bp = GetParam( "font-color", &ff );
  m_MonitorColorCombo.Init( bp ? &m_MonitorColorIndex : 0, ff, &pDocument->m_UserViewColorTab );
  m_PrinterColorCombo.Init( bp ? &m_PrinterColorIndex : 0, ff, &pDocument->m_UserPrintColorTab );
  m_PrinterBWColorCombo.Init( bp ? &m_PrinterBWColorIndex : 0, pDocument->m_UserPrintBWColorTab.TranslateKey( ff ));

  CDialog::OnInitDialog();

  ShowExample();

  GetDlgItem( IDC_SDLG_FORE_BUTTON )->EnableWindow( 0 );

  return 1;
}
//======================================================================
void
CTextParamDialog::DoDataExchange( CDataExchange* pDX )
{
  DDX_Check( pDX, IDC_TPD_BOLD_CHECK, m_IsBold );
  DDX_Check( pDX, IDC_TPD_ITALIC_CHECK, m_IsItalic );
  DDX_Check( pDX, IDC_TPD_UNDERLINE_CHECK, m_IsUnderline );
  DDX_Check( pDX, IDC_TPD_SELECT_VISIBLE_CHECK, m_IsSelectVisible );
  DDX_Text( pDX, IDC_TPD_HINT_EDIT, m_TextHint );
  DDX_Slider( pDX, IDC_TPD_FONT_SIZE_SLIDER, m_ExampleSize );

  DDX_CBIndex( pDX, IDC_TPD_MONITOR_COLOR_COMBO, m_MonitorColorIndex );
  DDX_CBIndex( pDX, IDC_TPD_PRINTER_COLOR_COMBO, m_PrinterColorIndex );
  DDX_CBIndex( pDX, IDC_TPD_PRINTER_BWCOLOR_COMBO, m_PrinterBWColorIndex );
  DDX_CBString( pDX, IDC_TPD_FONT_COMBO, m_FontFace );

  DDX_Text( pDX, IDC_TPD_SCALE_EDIT, m_Scale );

  if( pDX->m_bSaveAndValidate )
  {
    m_MonitorColor = m_MonitorColorCombo.GetItemData( m_MonitorColorIndex );
    m_PrinterColor = m_PrinterColorCombo.GetItemData( m_PrinterColorIndex );
    m_PrinterBWColor = m_PrinterBWColorCombo.GetItemData( m_PrinterBWColorIndex );

    CClientDC dc( this );
    int DPI = dc.GetDeviceCaps( LOGPIXELSY );
    m_FontSize = - ( m_ExampleSize * 2.54  * m_Scale / DPI / SmInUnit );
  }
}
//======================================================================
void
CTextParamDialog::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
  ShowExample();
}
//======================================================================
void
CTextParamDialog::ShowExample()
{
  UpdateData();
  m_ExampleBox.Invalidate();
}
//======================================================================
//======================================================================
void
CTextExampleBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
  CTextParamDialog* pTPD = (CTextParamDialog*)GetParent();

  CDC* pDC = CDC::FromHandle( lpDIS->hDC );

  CFont font;
  font.CreateFont( pTPD->m_ExampleSize, 0, 0, 0,
                   pTPD->IsBold() ? FW_BOLD : FW_DONTCARE,
                   pTPD->IsItalic(),
                   pTPD->IsUnderline(),
                   0,
                   RUSSIAN_CHARSET,
                   OUT_TT_PRECIS,
                   CLIP_DEFAULT_PRECIS,
                   DEFAULT_QUALITY,
                   DEFAULT_PITCH,
                   pTPD->GetFontFace());

  pDC->SelectObject( &font );
  pDC->SetTextColor( pTPD->GetMonitorColor());
  CRect r;
  GetClientRect( &r );
  CPoint cp = r.CenterPoint();
  pDC->SetTextAlign( TA_CENTER | TA_TOP );
  pDC->TextOut( cp.x, cp.y - pTPD->m_ExampleSize / 2, "Образец" );
}
//======================================================================
//======================================================================
int
CLineStyleList::Init( int initVal )
{
  CRect r;
  GetClientRect( &r );
  int s = 0;
  for( int i = 0 ; CLineParamDialog::m_StyleKey[i].m_Str ; ++i )
  {
    int t = CLineParamDialog::m_StyleKey[i].m_Key;
    SetItemData( AddString( 0 ), t );
    if( initVal == t )
      s = i;
  }

  for( int t = TEMPLATE_MIN_NUM ; t < TEMPLATE_MAX_NUM ; ++t )
    if( CLineTmpl::GetTemplate( t ))
    {
      SetItemData( AddString( 0 ), t );
      if( initVal == t )
         s = i;
      ++i;
    }

  SetItemHeight( 0, r.Height() / 6 );
  return s;
}
//======================================================================
static void
DrawTemplate( CDC* pDC, CRect* pRect, CLineTmpl* pTmpl, COLORREF clr )
{
  int const* ptmpl = pTmpl->GetLR();

  int P = *ptmpl++;
  int yMin, yMax;

  pTmpl->GetRange( &yMin, &yMax, 0 );

  CPoint tl( 0, yMin + yMax >> 1 );

  ptmpl = pTmpl->GetLR() + 1;
  int M = pRect->Height(), D = yMax - yMin;

  int dx = MulDiv( P, M, D );

  if( pRect->Width() / dx < 2 )
  {
    D = MulDiv( P, 2 * M, pRect->Width());
    dx = MulDiv( P, M, D );
  }

  int y = pRect->CenterPoint().y;

  for( int nPoints = *ptmpl++, n = nPoints, width = *ptmpl++ ; nPoints ;
           ptmpl += nPoints * 2, n = nPoints = *ptmpl++, width = *ptmpl++ )
  {
    int w = MulDiv( width, M, D );
    CPen pen( PS_SOLID, w, clr );
    CPen* pOldPen = (CPen*)pDC->SelectObject( &pen );

    for( CPoint *cp = (CPoint*)ptmpl ; --n > 0 ; ++cp )
    {
      CPoint p0( cp[0] - tl );
      CPoint p1( cp[1] - tl );
      p0.x = MulDiv( p0.x, M, D ) + pRect->left;
      p1.x = MulDiv( p1.x, M, D ) + pRect->left;
      p0.y = MulDiv( p0.y, M, D ) + y;
      p1.y = MulDiv( p1.y, M, D ) + y;

      for( int lm = pRect->left ; lm < pRect->right ; lm += dx, p0.x += dx, p1.x += dx )
      {
        pDC->MoveTo( p0 );
        pDC->LineTo( p1 );
      }
    }
    pDC->SelectObject( pOldPen );
  }
}
//======================================================================
void
CLineStyleList::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
  CDC* pDC = CDC::FromHandle( lpDIS->hDC );

  CRect r( lpDIS->rcItem );
  int sel = lpDIS->itemState & ODS_SELECTED;

  CPen pen( PS_SOLID, sel ? 2 : 1, RGB( sel ? 0xff : 0, 0, 0 ));
  CPen* pOldPen = (CPen*)pDC->SelectObject( &pen );

  r.InflateRect( -2, -2 );
  pDC->Rectangle( &r );
  r.InflateRect( 2, 2 );

  pDC->SelectObject( pOldPen );

  r.InflateRect( -4, -2 );
  pDC->IntersectClipRect( &r );
  r.InflateRect( -4, -8 );

  int y = r.CenterPoint().y;
  CPoint pnts[] = { CPoint( r.left, y ), CPoint( r.right, y ) } ;

  int stl = lpDIS->itemData;
  if( stl < TEMPLATE_MIN_NUM )
  {
    CPen pen( stl, 0, RGB( 0, 0, 0 ));
    pDC->SelectObject( &pen );
    pDC->MoveTo( pnts[0] );
    pDC->LineTo( pnts[1] );
  }
  else
    DrawTemplate( pDC, &r, CLineTmpl::GetTemplate( stl ), RGB( 0, 0, 0 ));

  pDC->SelectClipRgn( 0 );
}
//======================================================================
void
CLineExampleBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
  CLineParamDialog* pLPD = (CLineParamDialog*)GetParent();
  CDC* pDC = CDC::FromHandle( lpDIS->hDC );

  CRect r;
  GetClientRect( &r );
  r.InflateRect( -4, -2 );
  pDC->IntersectClipRect( &r );
  r.InflateRect( -4, -8 );
  int y = r.CenterPoint().y;
  CPoint pnts[] = { CPoint( r.left, y ), CPoint( r.right, y ) } ;

  int clw = pLPD->GetLineWidth( 0 );
  int stl = pLPD->GetLineStyle();
  if( stl < TEMPLATE_MIN_NUM )
  {
    int s = clw <= 1 ? stl : PS_SOLID;
    CPen pen( s, clw, pLPD->GetMonitorColor());
    pDC->SelectObject( &pen );
    pDC->MoveTo( pnts[0] );
    pDC->LineTo( pnts[1] );
  }
  else
  {
    CLineTmpl* pTempl = CLineTmpl::GetTemplate( stl );
    DrawTemplate( pDC, &r, pTempl, pLPD->GetMonitorColor());
  }
}
//======================================================================
//======================================================================
int CLineParamDialog::m_Style = 0;
int CLineParamDialog::m_Scale = 5000;
int CLineParamDialog::m_Width = 0;
int CLineParamDialog::m_Bound = 0;
COLORREF CLineParamDialog::m_MonitorColor;
COLORREF CLineParamDialog::m_PrinterColor;
COLORREF CLineParamDialog::m_PrinterBWColor;
int CLineParamDialog::m_MonitorColorIndex = 0;
int CLineParamDialog::m_PrinterColorIndex = 0;
int CLineParamDialog::m_PrinterBWColorIndex = 0;
int CLineParamDialog::m_LineStyleIndex = 0;

CStrAssoc const CLineParamDialog::m_StyleKey[] =
{
  CStrAssoc( "solid",        PS_SOLID      ),
  CStrAssoc( "dash",         PS_DASH       ),
  CStrAssoc( "dot",          PS_DOT        ),
  CStrAssoc( "dash-dot",     PS_DASHDOT    ),
  CStrAssoc( "dash-dot-dot", PS_DASHDOTDOT ),
  CStrAssoc( 0,              PS_SOLID      )
};
//======================================================================
BEGIN_MESSAGE_MAP( CLineParamDialog, CSubdialog )
  ON_WM_HSCROLL()
  ON_CBN_SELCHANGE( IDC_LPD_MONITOR_COLOR_COMBO, OnMonitorColorChange )
  ON_CBN_SELCHANGE( IDC_LPD_PRINTER_COLOR_COMBO, OnPrinterColorChange )
  ON_LBN_SELCHANGE( IDC_LPD_LINE_STYLE_LIST,     OnLineStyleChange )
  ON_BN_CLICKED( IDC_SDLG_FORE_BUTTON, OnFore )
END_MESSAGE_MAP()
//======================================================================
CLineParamDialog::CLineParamDialog( CWnd* pOwner ) :
  CSubdialog( IDD_LINE_PARAM_DIALOG, pOwner )
{}
//======================================================================
int
CLineParamDialog::OnInitDialog()
{
  m_MonitorColorCombo.SubclassDlgItem( IDC_LPD_MONITOR_COLOR_COMBO, this );
  m_PrinterColorCombo.SubclassDlgItem( IDC_LPD_PRINTER_COLOR_COMBO, this );
  m_PrinterBWColorCombo.SubclassDlgItem( IDC_LPD_PRINTER_BWCOLOR_COMBO, this );
  m_LineStyleList.SubclassDlgItem( IDC_LPD_LINE_STYLE_LIST, this );
  m_ExampleBox.SubclassDlgItem( IDC_LPD_EXAMPLE_BOX, this );
  m_ExampleBox.SetItemHeight( 0, 40 );
  m_ExampleBox.AddString( "" );

  CSliderCtrl* pSCTRL = (CSliderCtrl*)GetDlgItem( IDC_LPD_LINE_WIDTH_SLIDER );
  pSCTRL->SetRange( 0, MAX_LINE_WIDTH - 1 );

  m_Width = 0;
  GetParam( "polyline-width", &m_Width );

  int ff = 0;
  int bp = GetParam( "polyline-color", &ff );
  m_MonitorColorCombo.Init( bp ? &m_MonitorColorIndex : 0, ff, &pDocument->m_UserViewColorTab );
  m_PrinterColorCombo.Init( bp ? &m_PrinterColorIndex : 0, ff, &pDocument->m_UserPrintColorTab );
  m_PrinterBWColorCombo.Init( bp ? &m_PrinterBWColorIndex : 0, pDocument->m_UserPrintBWColorTab.TranslateKey( ff ));

  CString ls( "solid" );
  bp = GetParam( "polyline-style", &ls );
  m_LineStyleIndex = m_LineStyleList.Init( ls[0] == 't' ? atoi( LPCTSTR( ls ) + 1 ) : m_StyleKey->Translate( ls ));

	CPoint t( pView->GetCenter());
	m_Scale = CAbstractDialog::GetLimitScale( MerkatorData.TrueScale( pView->GetAltScale(), Point( t.x, t.y )));
	
	m_Width = GetLineWidth( -1 );

	if( ls[0] != 't' )
	{
		int l = MAX_LINE_WIDTH/2;
		if( m_Width > l )
		{
			m_Scale = m_Scale * m_Width / l;
			m_Width = l;
		}
	}

  ksi_obj ksi_cont = ksi_nil;
  GetParam( "contents", &ksi_cont );

  if( KSI_PLIST_P( ksi_cont ))
    m_Bound = KSI_TRUE_P( ksi_memv( keyBpll, ksi_cont ));

  CDialog::OnInitDialog();
  ShowExample();

  GetDlgItem( IDC_SDLG_FORE_BUTTON )->EnableWindow( CAbstractDialog::IsText());
  GetDlgItem( IDOK )->EnableWindow( IsClass() && !CAbstractDialog::IsNewGraphics()
                                    || !CAbstractDialog::IsText());
  OnLineStyleChange();

  return 1;
}
//======================================================================
void
CLineParamDialog::OnMonitorColorChange()
{
  m_MonitorColorCombo.AddNewColor();
  int i = m_MonitorColorCombo.GetCurSel();
  COLORREF clr = m_MonitorColorCombo.GetItemData( i );
  m_PrinterColorCombo.AddColor( clr );
  m_PrinterBWColorCombo.PicColor( CalcBWColor( clr ));
  ShowExample();
}
//======================================================================
void
CLineParamDialog::OnPrinterColorChange()
{
  m_PrinterColorCombo.AddNewColor();
}
//======================================================================
int
CLineParamDialog::GetLineWidth( int bCalc )
{
  int w = m_Width;
  if( bCalc  )
	{
    CClientDC dc( 0 );
    int DPI = dc.GetDeviceCaps( LOGPIXELSY );
 		double c = m_Scale * 2.54 / DPI / SmInUnit;
    if( bCalc > 0 )
      w = w * c + .5;
	  else
      w = w / c + .5;
	}
  return w;
}
//======================================================================
void
CLineParamDialog::DoDataExchange( CDataExchange* pDX )
{
  DDX_Check( pDX, IDC_LPD_BOUND_CHECK, m_Bound );
  DDX_Text( pDX, IDC_LPD_SCALE_EDIT, m_Scale );
  DDX_Slider( pDX, IDC_LPD_LINE_WIDTH_SLIDER, m_Width );
  DDX_CBIndex( pDX, IDC_LPD_MONITOR_COLOR_COMBO, m_MonitorColorIndex );
  DDX_CBIndex( pDX, IDC_LPD_PRINTER_COLOR_COMBO, m_PrinterColorIndex );
  DDX_CBIndex( pDX, IDC_LPD_PRINTER_BWCOLOR_COMBO, m_PrinterBWColorIndex );
  DDX_LBIndex( pDX, IDC_LPD_LINE_STYLE_LIST, m_LineStyleIndex );

  if( pDX->m_bSaveAndValidate )
  {
    m_MonitorColor = m_MonitorColorCombo.GetItemData( m_MonitorColorIndex );
    m_PrinterColor = m_PrinterColorCombo.GetItemData( m_PrinterColorIndex );
    m_PrinterBWColor = m_PrinterBWColorCombo.GetItemData( m_PrinterBWColorIndex );
    m_Style = m_LineStyleList.GetItemData( m_LineStyleList.GetCurSel());
  }
}
//======================================================================
void
CLineParamDialog::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
  ShowExample();
}
//======================================================================
void
CLineParamDialog::OnLineStyleChange()
{
  ShowExample();
  static UINT iid[] = { IDC_LPD_LINE_WIDTH_SLIDER, IDC_LPD_SCALE_EDIT,
                        IDC_LPD_WIDTH_STATIC, IDC_LPD_SCALE_STATIC,
                        IDC_LPD_1_STATIC };
  for( int i = CELEM( iid ) ; --i >= 0 ;
       GetDlgItem( iid[i] )->EnableWindow( m_Style == 0 ));
}
//======================================================================
void
CLineParamDialog::ShowExample()
{
  UpdateData();
  m_ExampleBox.Invalidate();
}
//======================================================================
void
CLineParamDialog::OnFore()
{
  UpdateData();
  int r = CTextParamDialog( this ).DoModal();
  if( r != IDRETRY )
  {
    m_Used = 1;
    EndDialog( r );
  }
}
//======================================================================
//======================================================================
void
CPolygonStyleList::Init(  int initVal )
{
  CRect r;
  GetClientRect( &r );
  int s = 0;
  for( int ns = 0 ; CPolygonParamDialog::m_HatchKey[ns].m_Str ; ++ns )
  {
    int v = CPolygonParamDialog::m_HatchKey[ns].m_Key;
    SetItemData( AddString( 0 ), v );
    if( initVal == v )
      s = ns;
  }
  SetItemHeight( 0, r.Height() / ( ns / 2 ));
  SetColumnWidth( r.Width() / 2 );
  SetCurSel( s );
}
//======================================================================
void
CPolygonStyleList::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
  CDC* pDC = CDC::FromHandle( lpDIS->hDC );
  CPolygonParamDialog* pPPD = (CPolygonParamDialog*)GetParent();

  CBrush brush;
  LOGBRUSH lb = { pPPD->IsHatched() ? BS_HATCHED : BS_SOLID,
                  pPPD->IsColored() ? pPPD->GetForeMonitorColor() : RGB( 255, 255, 255 ), 
                  int( lpDIS->itemData )};
  brush.CreateBrushIndirect( &lb );

  pDC->SelectObject( &brush );
  pDC->SelectStockObject( BLACK_PEN );
  pDC->SetBkColor( pPPD->GetBackMonitorColor());
  pDC->SetBkMode( pPPD->IsOpaque() ? OPAQUE : TRANSPARENT );

  CRect r( lpDIS->rcItem );
  r.InflateRect( -4, -4 );
  pDC->Rectangle( r );
  r.InflateRect( 2, 2 );

  if( lpDIS->itemAction & ( ODA_SELECT | ODA_DRAWENTIRE ))
  {
    pDC->SelectStockObject( NULL_BRUSH );
    pDC->SelectStockObject( lpDIS->itemState & ODS_SELECTED ? BLACK_PEN : WHITE_PEN );
    pDC->Rectangle( &r );
  }
}
//======================================================================
//======================================================================
int CPolygonParamDialog::m_Bound;
int CPolygonParamDialog::m_Smart;
int CPolygonParamDialog::m_Hatch;
int CPolygonParamDialog::m_bColor;
int CPolygonParamDialog::m_HatchStyle;
int CPolygonParamDialog::m_Opaque;
int CPolygonParamDialog::m_ForeMonitorColorIndex;
int CPolygonParamDialog::m_ForePrinterColorIndex;
int CPolygonParamDialog::m_ForePrinterBWColorIndex;
int CPolygonParamDialog::m_BackMonitorColorIndex;
int CPolygonParamDialog::m_BackPrinterColorIndex;
int CPolygonParamDialog::m_BackPrinterBWColorIndex;
int CPolygonParamDialog::m_FillStyleListIndex;

COLORREF CPolygonParamDialog::m_ForeMonitorColor;
COLORREF CPolygonParamDialog::m_ForePrinterColor;
COLORREF CPolygonParamDialog::m_ForePrinterBWColor;
COLORREF CPolygonParamDialog::m_BackMonitorColor;
COLORREF CPolygonParamDialog::m_BackPrinterColor;
COLORREF CPolygonParamDialog::m_BackPrinterBWColor;

CStrAssoc const CPolygonParamDialog::m_FillKey[] =
{
  CStrAssoc( "solid", BS_SOLID   ),
  CStrAssoc( 0,       BS_HATCHED )
};

CStrAssoc const CPolygonParamDialog::m_HatchKey[] =
{
  CStrAssoc( "horizontal", HS_HORIZONTAL ),
  CStrAssoc( "vertical",   HS_VERTICAL   ),
  CStrAssoc( "fdiagonal",  HS_FDIAGONAL  ),
  CStrAssoc( "bdiagonal",  HS_BDIAGONAL  ),
  CStrAssoc( "cross",      HS_CROSS      ),
  CStrAssoc( "diagcross",  HS_DIAGCROSS  ),
  CStrAssoc( 0, 0 )
};
//======================================================================
BEGIN_MESSAGE_MAP( CPolygonParamDialog, CSubdialog )
  ON_BN_CLICKED( IDC_PPD_BOUND_CHECK,  OnBoundButtonClick )
  ON_BN_CLICKED( IDC_PPD_SMART_CHECK,  OnSmartButtonClick )
  ON_BN_CLICKED( IDC_PPD_HATCH_CHECK,  OnHatchButtonClick )
  ON_BN_CLICKED( IDC_PPD_OPAQUE_FORE_CHECK, OnChangeColored )
  ON_BN_CLICKED( IDC_PPD_OPAQUE_BACK_CHECK, OnChangeHatch )
  ON_BN_CLICKED( IDC_SDLG_FORE_BUTTON, OnFore )
  ON_CBN_SELCHANGE( IDC_PPD_MONITOR_FORE_COLOR_COMBO, OnForeMonitorColorChange )
  ON_CBN_SELCHANGE( IDC_PPD_PRINTER_FORE_COLOR_COMBO, OnForePrinterColorChange )
  ON_CBN_SELCHANGE( IDC_PPD_MONITOR_BACK_COLOR_COMBO, OnBackMonitorColorChange )
  ON_CBN_SELCHANGE( IDC_PPD_PRINTER_BACK_COLOR_COMBO, OnBackPrinterColorChange )
END_MESSAGE_MAP()
//======================================================================
CPolygonParamDialog::CPolygonParamDialog( CWnd* pOwner ) :
  CSubdialog( IDD_POLYGON_PARAM_DIALOG, pOwner )
{}
//======================================================================
int
CPolygonParamDialog::OnInitDialog()
{
  m_ForeMonitorColorCombo.SubclassDlgItem( IDC_PPD_MONITOR_FORE_COLOR_COMBO, this );
  m_ForePrinterColorCombo.SubclassDlgItem( IDC_PPD_PRINTER_FORE_COLOR_COMBO, this );
  m_ForePrinterBWColorCombo.SubclassDlgItem( IDC_PPD_PRINTER_FORE_BWCOLOR_COMBO, this );

  m_BackMonitorColorCombo.SubclassDlgItem( IDC_PPD_MONITOR_BACK_COLOR_COMBO, this );
  m_BackPrinterColorCombo.SubclassDlgItem( IDC_PPD_PRINTER_BACK_COLOR_COMBO, this );
  m_BackPrinterBWColorCombo.SubclassDlgItem( IDC_PPD_PRINTER_BACK_BWCOLOR_COMBO, this );

  int fc = 0;
  int bp = GetParam( "fore-color", &fc );
  m_ForeMonitorColorCombo.Init( bp ? &m_ForeMonitorColorIndex : 0, fc, &pDocument->m_UserViewColorTab );
  m_ForePrinterColorCombo.Init( bp ? &m_ForePrinterColorIndex : 0, fc, &pDocument->m_UserPrintColorTab );
  m_ForePrinterBWColorCombo.Init( bp ? &m_ForePrinterBWColorIndex : 0, pDocument->m_UserPrintBWColorTab.TranslateKey( fc ));

  int bc = 0;
  bp = GetParam( "back-color", &bc );
  m_Opaque = bc >= 0;

  m_BackMonitorColorCombo.Init( bp ? &m_BackMonitorColorIndex : 0, bc, &pDocument->m_UserViewColorTab );
  m_BackPrinterColorCombo.Init( bp ? &m_BackPrinterColorIndex : 0, bc, &pDocument->m_UserPrintColorTab );
  m_BackPrinterBWColorCombo.Init( bp ? &m_BackPrinterBWColorIndex : 0, pDocument->m_UserPrintBWColorTab.TranslateKey( bc ));

  ksi_obj ksi_cont = ksi_nil;
  GetParam( "contents", &ksi_cont );

  if( KSI_PLIST_P( ksi_cont ))
  {
    m_Bound = ksi_memv( keyOplg, ksi_cont ) != ksi_false;
    m_Smart = ksi_memv( keySplg, ksi_cont ) != ksi_false;
  }

  CString hs( "solid" );
  if( GetParam( "fill-style", &hs ))
  {
    m_Hatch = m_FillKey->Translate( hs );
    m_HatchStyle = m_HatchKey->Translate( hs );
  }

  int cl = 0;
  GetParam( "fore-color", &cl );
  m_bColor = cl >= 0;

  m_FillStyleList.SubclassDlgItem( IDC_PPD_FILL_STYLE_LIST, this );

  CDialog::OnInitDialog();

  m_FillStyleList.Init( m_HatchStyle );

  OnBoundButtonClick();
  OnSmartButtonClick();
  OnHatchButtonClick();
  OnChangeColored();
//  OnChangeHatch();

  return 1;
}
//======================================================================
void
CPolygonParamDialog::OnBoundButtonClick()
{
  UpdateData();
  CheckDlgButton( IDC_PPD_SMART_CHECK, !m_Bound && m_Smart );
  int bCont = m_Bound || m_Smart || CAbstractDialog::IsText();
  int bSave = ( !m_Bound && !m_Smart || GetParam( "polyline-width", 0 )) &&
               ( !CAbstractDialog::IsText() || GetParam( "font-size", 0 ));
  GetDlgItem( IDC_SDLG_FORE_BUTTON )->EnableWindow( bCont );
  GetDlgItem( IDOK )->EnableWindow( bSave );
}
//======================================================================
void
CPolygonParamDialog::OnSmartButtonClick()
{
  UpdateData();
  CheckDlgButton( IDC_PPD_BOUND_CHECK, !m_Smart && m_Bound );
  int bCont = m_Bound || m_Smart || CAbstractDialog::IsText();
  int bSave = ( !m_Bound && !m_Smart || GetParam( "polyline-width", 0 )) &&
               ( !CAbstractDialog::IsText() || GetParam( "font-size", 0 ));
  GetDlgItem( IDC_SDLG_FORE_BUTTON )->EnableWindow( bCont );
  GetDlgItem( IDOK )->EnableWindow( bSave );
}
//======================================================================
void
CPolygonParamDialog::OnHatchButtonClick()
{
  m_Hatch = IsDlgButtonChecked( IDC_PPD_HATCH_CHECK );
  m_FillStyleList. EnableWindow( m_Hatch );
  GetDlgItem( IDC_PPD_FILL_STYLE_LIST_STATIC )->EnableWindow( m_Hatch );
  GetDlgItem( IDC_PPD_FILL_STYLE_LIST )->EnableWindow( m_Hatch );
  GetDlgItem( IDC_PPD_OPAQUE_BACK_CHECK )->EnableWindow( m_Hatch );
  if( !m_Hatch )
    CheckDlgButton( IDC_PPD_OPAQUE_BACK_CHECK, 0 );
  OnChangeHatch();
}
//======================================================================
void
CPolygonParamDialog::OnFore()
{
  UpdateData();
  int r = m_Bound || m_Smart        ? CLineParamDialog( this ).DoModal() :
          CAbstractDialog::IsText() ? CTextParamDialog( this ).DoModal() :
                                      IDOK;
  if( r != IDRETRY )
  {
    m_Used = 1;
    EndDialog( r );
  }
}
//======================================================================
void
CPolygonParamDialog::DoDataExchange( CDataExchange* pDX )
{
  DDX_Check( pDX, IDC_PPD_BOUND_CHECK,  m_Bound );
  DDX_Check( pDX, IDC_PPD_SMART_CHECK,  m_Smart );
  DDX_Check( pDX, IDC_PPD_HATCH_CHECK,  m_Hatch );
  DDX_Check( pDX, IDC_PPD_OPAQUE_FORE_CHECK, m_bColor );
  DDX_Check( pDX, IDC_PPD_OPAQUE_BACK_CHECK, m_Opaque );

  DDX_CBIndex( pDX, IDC_PPD_MONITOR_FORE_COLOR_COMBO,   m_ForeMonitorColorIndex );
  DDX_CBIndex( pDX, IDC_PPD_PRINTER_FORE_COLOR_COMBO,   m_ForePrinterColorIndex );
  DDX_CBIndex( pDX, IDC_PPD_PRINTER_FORE_BWCOLOR_COMBO, m_ForePrinterBWColorIndex );

  DDX_CBIndex( pDX, IDC_PPD_MONITOR_BACK_COLOR_COMBO,   m_BackMonitorColorIndex );
  DDX_CBIndex( pDX, IDC_PPD_PRINTER_BACK_COLOR_COMBO,   m_BackPrinterColorIndex );
  DDX_CBIndex( pDX, IDC_PPD_PRINTER_BACK_BWCOLOR_COMBO, m_BackPrinterBWColorIndex );

  DDX_LBIndex( pDX, IDC_PPD_FILL_STYLE_LIST, m_FillStyleListIndex );

  if( pDX->m_bSaveAndValidate )
  {
    m_ForeMonitorColor = m_ForeMonitorColorCombo.GetItemData( m_ForeMonitorColorIndex );
    m_ForePrinterColor = m_ForePrinterColorCombo.GetItemData( m_ForePrinterColorIndex );
    m_ForePrinterBWColor = m_ForePrinterBWColorCombo.GetItemData( m_ForePrinterBWColorIndex );

    m_BackMonitorColor = m_BackMonitorColorCombo.GetItemData( m_BackMonitorColorIndex );
    m_BackPrinterColor = m_BackPrinterColorCombo.GetItemData( m_BackPrinterColorIndex );
    m_BackPrinterBWColor = m_BackPrinterBWColorCombo.GetItemData( m_BackPrinterBWColorIndex );

    m_HatchStyle = m_FillStyleList.GetCurSel();
  }
}
//======================================================================
void
CPolygonParamDialog::OnChangeHatch()
{
  UINT b[] = 
  {
    IDC_PPD_MONITOR_BACK_COLOR_COMBO,
    IDC_PPD_PRINTER_BACK_COLOR_COMBO,
    IDC_PPD_PRINTER_BACK_BWCOLOR_COMBO,
    IDC_PPD_MONITOR_BACK_COLOR_STATIC,
    IDC_PPD_PRINTER_BACK_COLOR_STATIC,
    IDC_PPD_PRINTER_BACK_BWCOLOR_STATIC,
    IDC_PPD_BACK_COLOR_STATIC
  };

  UpdateData();
  m_FillStyleList.Invalidate();
  int bBack = IsDlgButtonChecked( IDC_PPD_OPAQUE_BACK_CHECK );
  for( int i = CELEM( b ) ; --i >= 0 ; GetDlgItem( b[i] )->EnableWindow( bBack ));
}
//======================================================================
void
CPolygonParamDialog::OnChangeColored()
{
  UINT b[] = 
  {
    IDC_PPD_HATCH_CHECK,
    IDC_PPD_MONITOR_FORE_COLOR_COMBO,
    IDC_PPD_PRINTER_FORE_COLOR_COMBO,
    IDC_PPD_PRINTER_FORE_BWCOLOR_COMBO,
    IDC_PPD_MONITOR_FORE_COLOR_STATIC,
    IDC_PPD_PRINTER_FORE_COLOR_STATIC,
    IDC_PPD_PRINTER_FORE_BWCOLOR_STATIC,
    IDC_PPD_FORE_COLOR_STATIC
  };

  UpdateData();
  int bColored = IsDlgButtonChecked( IDC_PPD_OPAQUE_FORE_CHECK );
  for( int i = CELEM( b ) ; --i >= 0 ; GetDlgItem( b[i] )->EnableWindow( bColored ));
  if( !bColored )
    CheckDlgButton( IDC_PPD_HATCH_CHECK, 0 );
  OnHatchButtonClick();
}
//======================================================================
void
CPolygonParamDialog::OnForeMonitorColorChange()
{
  m_ForeMonitorColorCombo.AddNewColor();
  int i = m_ForeMonitorColorCombo.GetCurSel();
  COLORREF clr = m_ForeMonitorColorCombo.GetItemData( i );
  m_ForePrinterColorCombo.AddColor( clr );
  m_ForePrinterBWColorCombo.PicColor( CalcBWColor( clr ));
  OnChangeHatch();
}
//======================================================================
void
CPolygonParamDialog::OnBackMonitorColorChange()
{
  m_BackMonitorColorCombo.AddNewColor();
  int i = m_BackMonitorColorCombo.GetCurSel();
  COLORREF clr = m_BackMonitorColorCombo.GetItemData( i );
  m_BackPrinterColorCombo.AddColor( clr );
  m_BackPrinterBWColorCombo.PicColor( CalcBWColor( clr ));
  OnChangeHatch();
}
//======================================================================
void
CPolygonParamDialog::OnForePrinterColorChange()
{
  m_ForePrinterColorCombo.AddNewColor();
}
//======================================================================
void
CPolygonParamDialog::OnBackPrinterColorChange()
{
  m_BackPrinterColorCombo.AddNewColor();
}
//======================================================================
#endif //_FULL_PROPJECT
