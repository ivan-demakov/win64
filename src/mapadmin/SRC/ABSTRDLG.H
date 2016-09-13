//======================================================================
#ifndef _ABSTRDLG_H_
#define _ABSTRDLG_H_
//======================================================================
#include <afxcmn.h>
#include <limits.h>
#include "objdef.h"
#include "assoc.h"
//======================================================================
class CAutoScrollTreeCtrl : public CTreeCtrl
{
public:
  CAutoScrollTreeCtrl() : 
    m_bDragging( 0 ), 
    m_pImageList( 0 ), 
    m_bTimerOn( 0 ) 
  {}

  int IsDragging() { return m_bDragging; }

protected:
  afx_msg void OnBeginDrag( LPNMHDR pnmhdr, LRESULT *pLResult );
  afx_msg void OnMouseMove( UINT nFlags, CPoint point );
  afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
  afx_msg void OnTimer( UINT nIDEvent );
  afx_msg void OnDestroy();

protected:
  HTREEITEM m_hItemDrag;
  HTREEITEM m_hItemDrop;
  int      m_bDragging;
  CImageList* m_pImageList;

private:
  void doScroll();

private:
  int  m_nScrollDelta;
  int  m_bTimerOn;

  DECLARE_MESSAGE_MAP()
};  
//======================================================================
class CClassTreeCtrl : public CAutoScrollTreeCtrl
{
public:
  int MoveSelected( int bUp );
  HTREEITEM FindTreeNode( int type, HTREEITEM parent = 0 );
  HTREEITEM FindTreeNode( LPCTSTR prfx, int pxln, HTREEITEM parent = 0 );

protected:
  afx_msg void OnLButtonUp( UINT nFlags, CPoint point );

private:
  void TransferItem( HTREEITEM hItem, HTREEITEM hNewParent );
  int  IsChildNodeOf( HTREEITEM hItemChild, HTREEITEM hSuspectedParent );

  DECLARE_MESSAGE_MAP()
};  
//======================================================================
class CSubdialog : public CDialog
{

#define PARDEF( n, v ) ksi_cons( ksi_str02key( n ), v )

public:
  CSubdialog( UINT DlgId, CWnd* pOwner );

private:
  void OnBackClicked();
  void OnCancel() { OnBackClicked(); }
  void OnStopClicked() { EndDialog( IDCANCEL ); }

protected:
  static void AddParam( ksi_obj* pArr );
  static int  GetParam( char const* name, void* pParam );
  static int  IsUserClass();
  static int  IsClass();
  static int  IsAbstractClass();
  static int  GetFreeClass( int bFromBegin );

protected:
  static CObjDef* m_pObjDef;
  static ksi_obj  m_KsiParamList;
  static ksi_obj  m_OldParamList;

  DECLARE_MESSAGE_MAP()
};
//======================================================================
class CAbstractDialog : public CSubdialog
{
  DECLARE_DYNCREATE( CAbstractDialog )
  friend class CClassTreeCtrl;

public:
  CAbstractDialog( CMapView* pView = 0 );

  int  OnInitDialog();
  int  IsActive() { return m_bActive; }
  void DoDataExchange( CDataExchange* pDX );

  void Activate( CMapView* pView );
  void OnOK();
  void OnCancel();
  void OnFore();

  static int IsIcon()         { return m_IsIcon; }
  static int IsPolygon()      { return m_IsPolygon; }
  static int IsPolyline()     { return m_IsPolyline; }
  static int IsText()         { return m_IsText; }
  static int IsFootnote()     { return m_IsFootnote; }
  static int IsNewGraphics()  { return m_bNewGraphics; }
  static int GetClassNumber() { return m_ClassNumber; }
  static int IsClassConcret() { return m_bConcretClass; }  
	static int GetLimitScale( int s );
  static ksi_obj GetParamPath();

private:
  void OnClassTreeSelChanged( NMHDR* pNotifyStruct, LRESULT* result );
  void OnSelectClassUp();
  void OnSelectClassDn();
  void OnGraphicsChange();
  void OnClassChange();
  void OnClassNameEditBegin();
  void OnMinShowAlwaysClicked();
  void OnMaxShowAlwaysClicked();
  void OnRemoveClicked();
  void RemoveClass( int bAsk );
  void OnClassFindClicked();
  void UpdateSelectedClass();
  void OnScaleAlwaysClicked();
  void OnClassClosedClicked();
  int  SaveTreeNode( HTREEITEM node, int order );
  HTREEITEM InsertTreeNode( HTREEITEM parent, int type );

private:
  int m_bClosedObjEdition;
  int m_OldMinShowLimit;
  int m_OldMaxShowLimit;
  int m_OldScaleBegin;
  int m_OldScaleEnd;
  int m_OldClassLaw;
  int m_OldClassLoad;
  int m_OldIsTextInvisible;
  int m_OldIsScaleAlways;
  int m_OldIsMinShowAlways;
  int m_OldIsMaxShowAlways;
  int m_OldClassTable;
  static int m_bConcretClass;
  static int m_IsTextInvisible;
  static int m_MinShowLimit;
  static int m_MaxShowLimit;
  static int m_IsMinShowAlways;
  static int m_IsMaxShowAlways;
  static int m_IsScaleAlways;
  static int m_ScaleBegin;
  static int m_ScaleEnd;
  static int m_IsIcon;
  static int m_IsPolygon;
  static int m_IsPolyline;
  static int m_IsText;
  static int m_IsFootnote;
  static int m_ClassNumber;
  static int m_ParentNumber;
  static int m_Range;
  static int m_ClassLaw; 
  static int m_ClassLoad; 
  static int m_ClassTable;
  static int m_bClassTreeChanged;
  static int m_bClassFind;
  static int m_bNewGraphics;
  static CString m_ClassName;
  static CString m_Hint;

private:
  int           m_bActive;     
  CClassTreeCtrl m_ClassTree;
  CMapView*      m_pView; 

  DECLARE_MESSAGE_MAP()
};
//======================================================================
class CIconListBox : public CListBox
{
public:
  void DrawItem( LPDRAWITEMSTRUCT lpDIS );
};
//======================================================================
class CIconExampleBox : public CListBox
{
public:
  void DrawItem( LPDRAWITEMSTRUCT lpDIS );
};
//======================================================================
class CIconParamDialog : public CSubdialog
{
  enum 
  { 
    ICON_SIZE = 36, 
    ICON_MIN_SIZE = 4, 
    ICON_MAX_SIZE = 64 
  };

  friend class CIconExampleBox;

public:
  CIconParamDialog( CWnd* pOwner );

  int  OnInitDialog();
  void OnOK() { m_Used = 1; UpdateData(); EndDialog( IDOK ); }
  void DoDataExchange( CDataExchange* pDX );
  void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
  void OnIconSelected();
  void OnFore();

  static int GetScaleSingle() { return m_ScaleSingle; }
  static int GetIcomRef()     { return m_Ref; }
  static int IsUsed()         { return m_Used; }
  static int IsIconChanged()  { return m_Used && m_bIconChanged; }
  static void SetUsed( int bUsed ) { m_Used = bUsed; }
  static CString GetBmpName();
  static CString const& GetBmpPath() { return m_BmpPath; }
  static LPCTSTR SectName( int bC );

private:
  void ShowExample();

private:
  static CString m_BmpPath;
  static int m_Used;
  static int m_Size;
  static int m_Ref;
  static int m_ScaleSingle;
  static int m_ScaleExample;
  static int m_bIconChanged;

private:
  CIconExampleBox m_ExampleBox;
  CIconListBox    m_IconListBox;
  CString         m_IconParam;
  int             m_OldSel;
  int             m_OldScale;

  DECLARE_MESSAGE_MAP()
};
//======================================================================
class CColorCombo : public CComboBox
{
public:
  void Init( int* pIndex, int initVal, CIntAssocMap* pMap = 0 );
  void DrawItem( LPDRAWITEMSTRUCT lpDIS );
  void AddNewColor();
  void AddColor( COLORREF clr );
  int  PicColor( COLORREF clr );

public:
  int m_bColor;
};
//======================================================================
class CTextExampleBox : public CListBox
{
  void DrawItem( LPDRAWITEMSTRUCT lpDIS );
};
//======================================================================
class CTextParamDialog : public CSubdialog
{
  enum 
  { 
    MIN_FONT_SIZE = 1, 
    MAX_FONT_SIZE = 40,
    DEF_FONT_SIZE = 420 
  };

  friend class CTextExampleBox;

public:
  CTextParamDialog( CWnd* pOwner );
  int OnInitDialog();
  void OnOK() { m_Used = 1; UpdateData(); EndDialog( IDOK ); }
  void DoDataExchange( CDataExchange* pDX );
  void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
  void OnMonitorColorChange();
  void OnPrinterColorChange();
  static int IsBold()          { return m_IsBold; } 
  static int IsItalic()        { return m_IsItalic; }
  static int IsUnderline()     { return m_IsUnderline; }
  static int IsSelectVisible() { return m_IsSelectVisible; }
  static int GetScale()        { return m_Scale; }
  static int GetFontSize()     { return m_FontSize; }
  static int IsUsed()          { return m_Used; }
  static void SetUsed( int bUsed ) { m_Used = bUsed; }
  static CString const& GetHint() { return m_TextHint;; }
  static COLORREF GetMonitorColor() { return m_MonitorColor; }
  static COLORREF GetPrinterColor() { return m_PrinterColor; }
  static COLORREF GetPrinterBWColor() { return m_PrinterBWColor; }
  static CString const& GetFontFace() { return m_FontFace; }

private:
  void ShowExample();

private:
  static int m_Used;
  static int m_IsBold;
  static int m_IsItalic;
  static int m_IsUnderline;
  static int m_IsSelectVisible;
  static int m_Scale;
  static int m_FontSize;
  static COLORREF m_MonitorColor;
  static COLORREF m_PrinterColor;
  static COLORREF m_PrinterBWColor;
  static int m_MonitorColorIndex;
  static int m_PrinterColorIndex;
  static int m_PrinterBWColorIndex;
  static CString m_FontFace;
  static CString m_TextHint;
  int m_ExampleSize;

private:
  CColorCombo m_MonitorColorCombo;
  CColorCombo m_PrinterColorCombo;
  CColorCombo m_PrinterBWColorCombo;
  CTextExampleBox m_ExampleBox;
  
  DECLARE_MESSAGE_MAP()
};
//======================================================================
#define TEMPLATE_MIN_NUM 10
#define TEMPLATE_MAX_NUM 1000

class CLineExampleBox : public CListBox
{
  void DrawItem( LPDRAWITEMSTRUCT lpDIS );
};
//======================================================================
class CLineStyleList : public CListBox
{
public:
  int  Init( int initVal );
  void DrawItem( LPDRAWITEMSTRUCT lpDIS );
};
//======================================================================
class CLineParamDialog : public CSubdialog
{
  enum { MAX_LINE_WIDTH = 32 };

public:
  CLineParamDialog( CWnd* pOwner );
  int  OnInitDialog();
  void OnOK() { m_Used = 1; UpdateData(); EndDialog( IDOK ); }
  void OnFore();
  void DoDataExchange( CDataExchange* pDX );
  void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
  void OnMonitorColorChange();
  void OnPrinterColorChange();
  void OnLineStyleChange();
  
  static int GetLineStyle() { return m_Style; }
  static int GetLineWidth( int bCalc );
  static int GetScale()     { return m_Scale; }
  static int IsBound()      { return m_Bound; }
  static int IsUsed()       { return m_Used; }
  static void SetUsed( int bUsed ) { m_Used = bUsed; }
  static COLORREF GetMonitorColor() { return m_MonitorColor; }
  static COLORREF GetPrinterColor() { return m_PrinterColor; }
  static COLORREF GetPrinterBWColor() { return m_PrinterBWColor; }

public:
  static CStrAssoc const m_StyleKey[];

private:
  void ShowExample();

private:
  static int m_Used;
  static int m_Style;
  static int m_Scale;
  static int m_Width;
  static int m_Bound;
  static COLORREF m_MonitorColor;
  static COLORREF m_PrinterColor;
  static COLORREF m_PrinterBWColor;
  static int m_MonitorColorIndex;
  static int m_PrinterColorIndex;
  static int m_PrinterBWColorIndex;
  static int m_LineStyleIndex;

private:
  CColorCombo m_MonitorColorCombo;
  CColorCombo m_PrinterColorCombo;
  CColorCombo m_PrinterBWColorCombo;
  CLineStyleList m_LineStyleList;
  CLineExampleBox m_ExampleBox;
  
  DECLARE_MESSAGE_MAP()
};
//======================================================================
class CPolygonStyleList : public CListBox
{
public:
  void Init( int initVal );
  void DrawItem( LPDRAWITEMSTRUCT lpDIS );
};
//======================================================================
class CPolygonParamDialog : public CSubdialog
{
public:
  CPolygonParamDialog( CWnd* pOwner );
  int  OnInitDialog();
  void OnOK() { m_Used = 1; UpdateData(); EndDialog( IDOK ); }
  void OnFore();
  void DoDataExchange( CDataExchange* pDX );
  void OnBoundButtonClick();
  void OnSmartButtonClick();
  void OnHatchButtonClick();
  void OnChangeHatch();
  void OnChangeColored();
  void OnForeMonitorColorChange();
  void OnBackMonitorColorChange();
  void OnForePrinterColorChange();
  void OnBackPrinterColorChange();
  
  static int IsBound()       { return m_Bound; }
  static int IsSmart()       { return m_Smart; }
  static int IsHatched()     { return m_Hatch; }
  static int IsColored()     { return m_bColor; }
  static int IsOpaque()      { return m_Opaque; }
  static int GetHatchStyle() { return m_HatchStyle; }
  static int IsUsed()        { return m_Used; }
  static void SetUsed( int bUsed ) { m_Used = bUsed; }
  static COLORREF GetForeMonitorColor()   { return m_ForeMonitorColor; }
  static COLORREF GetForePrinterColor()   { return m_ForePrinterColor; }
  static COLORREF GetForePrinterBWColor() { return m_ForePrinterBWColor; }
  static COLORREF GetBackMonitorColor()   { return m_BackMonitorColor; }
  static COLORREF GetBackPrinterColor()   { return m_BackPrinterColor; }
  static COLORREF GetBackPrinterBWColor() { return m_BackPrinterBWColor; }

public:
  static CStrAssoc const m_FillKey[];
  static CStrAssoc const m_HatchKey[];

private:
  static int m_Used;
  static int m_bColor;
  static int m_Hatch;
  static int m_Bound;
  static int m_Smart;
  static int m_HatchStyle;
  static int m_Opaque;
  static int m_ForeMonitorColorIndex;
  static int m_ForePrinterColorIndex;
  static int m_ForePrinterBWColorIndex;
  static int m_BackMonitorColorIndex;
  static int m_BackPrinterColorIndex;
  static int m_BackPrinterBWColorIndex;
  static int m_FillStyleListIndex;

  static COLORREF m_ForeMonitorColor;
  static COLORREF m_ForePrinterColor;
  static COLORREF m_ForePrinterBWColor;
  static COLORREF m_BackMonitorColor;
  static COLORREF m_BackPrinterColor;
  static COLORREF m_BackPrinterBWColor;

private:
  CColorCombo m_ForeMonitorColorCombo;
  CColorCombo m_ForePrinterColorCombo;
  CColorCombo m_ForePrinterBWColorCombo;
  CColorCombo m_BackMonitorColorCombo;
  CColorCombo m_BackPrinterColorCombo;
  CColorCombo m_BackPrinterBWColorCombo;
  CPolygonStyleList m_FillStyleList;
  
  DECLARE_MESSAGE_MAP()
};
//======================================================================
#endif
