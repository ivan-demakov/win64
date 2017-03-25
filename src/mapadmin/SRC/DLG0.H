#ifndef __DLG0_H__
#define __DLG0_H__
//=====================================================================
#include "dlgids.h"
#include "lomedit.h"
#include "scale.h"
#include "scale.h"
#include "ids.h"
#include "cbox.h"
#include "mview.h"
//=====================================================================
//=====================================================================
class CRepDialog : public CDialog
{
public:
  CRepDialog() : CDialog( IDD_REPER_DLG ) {}
        int GetRepers() { return m_bRepers; }
        int GetOneError() { return m_bOneError; }
        int GetAllErrors() { return m_bAllErrors; }
        CString const& GetPath() { return m_FilePath; }

private:
  void DoDataExchange( CDataExchange* pDX );
        void OnFilePathButton();

private:
  static int m_bRepers;
  static int m_bOneError;
  static int m_bAllErrors;
        static CString m_FilePath;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CGridDialog : public CDialog
{
public:
  CGridDialog( CWnd *owner ) : CDialog( IDD_SETGRID, owner ) {}

  int OnInitDialog();
  void OnOK();
  static CSize GetXGrid();
  static CSize GetYGrid();

  static long m_nStepX;
  static long m_nStepY;
  static long m_nOffsetX;
  static long m_nOffsetY;

private:
  CFloatEdit edit[2];
};
//=====================================================================
class CGotoDialog : public CDialog
{
public:
  CGotoDialog() : CDialog( IDD_GOTO_DIALOG, 0 )
  {}
  int OnInitDialog();
  void OnCoordButtonClicked();
  void OnGMSButtonClicked();
  void OnHScroll( UINT, UINT, CScrollBar* );
  void OnScaleChange();
  void OnOK();
  void OnCancel();
  void OnLBClicked();
  void OnRBClicked();
	void SetPointScale( CPoint c, int s );
  int GetScale() { return m_Scale; }

  static CPoint GetPoint() { return m_Point; }

private:
  void UpdateCmd( int cmd );
  void ShowDecart();
  void ShowPolar();
  void PolDec();
  void DecPol();

  static int Scale2Pos( int scale );
  static int Pos2Scale( int pos );

private:
  CSliderCtrl*  m_pSCTRL;
  int           m_Scale;

  static int    m_bFixPoint;
  static int    m_scw;
  static int    m_bDecart;
  static CPoint m_Point;
  static Polar  m_Polar;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class COverviewDialog : public CDialog
{
  DECLARE_DYNCREATE( COverviewDialog )

public:
  ~COverviewDialog();

  void Init( CRect* pRect );
  void MoveRect();
  int  Refresh( CRect* pRect = 0 );
  static int HasOverview() { return IsOverview; }

private:
  void OnPaint();
  void OnLButtonDown( UINT nFlags, CPoint point );
  void OnLButtonUp( UINT nFlags, CPoint point );
  void OnMouseMove( UINT nFlags, CPoint point );
  void CalcRect();
  void CorrectRect();
  void Draw();
  int  CreateBmp( CRect* pRect );

private:
  int   m_nMode;
  CRect m_Rect;
  CRect m_Wnd;
  CBox  m_Box;
  CBitmap m_Bmp;
  HBITMAP m_hBmp;

  static CString m_OverviewPath;
  static int IsOverview;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CSetDistDialog : public CDialog
{
public:
  CSetDistDialog( char const* Name, char const* varName ) :
    CDialog( IDD_SETDIST_DIALOG, 0 ),
    m_Name( Name ),
    m_VarName( varName )
  {}
  int OnInitDialog();
  void OnOK();
  static int  GetDist() { return m_Dist; }
  static void SetDist( int d ) { m_Dist = d; }

private:
  static long m_Dist;
  CFloatEdit  m_edit0;
  CIntEdit    m_edit1;
  char const* m_Name;
  char const* m_VarName;
};
//=====================================================================
class CShapeDialog : public CDialog
{
public:
  void Init( CMapView* pView, int init );
  void SetShape( int s );
  void OnCancel(){}
  void OnOK(){}
  int  OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;
  int  GetShape();
};
//=====================================================================
class CAngleDialog : public CDialog
{
public:
  CAngleDialog() : CDialog( IDD_ANGLE_DIALOG, 0 ){}
  int  OnInitDialog();
  void OnOK();

  static long m_AngleGrid;

private:
  CFloatEdit  m_Edit;
};
//=====================================================================
class CCoordGridDialog : public CDialog
{
public:
  CCoordGridDialog( CPoint pt ) :
    CDialog( IDD_COORD_GRID_DIALOG, 0 )
  {
    m_ProjPoint = pt;
  }
  int  OnInitDialog();
  void OnOK();
  static CPoint GetProjPoint() { return m_ProjPoint; }
  static CPoint GetGridPoint() { return m_GridPoint; }
  static CPoint GetGridStep()  { return m_GridStep;  }
  static CPoint GetGridDir()   { return m_GridDir;   }

private:
  CIntEdit m_Edit[6];
  static CPoint m_ProjPoint;
  static CPoint m_GridPoint;
  static CPoint m_GridStep;
  static CPoint m_GridDir;
};
//=====================================================================
class CSplDialog: public CDialog
{
public:
  CSplDialog() :
    CDialog( IDD_SPL_DIALOG, 0 )
  {}
  void DoDataExchange( CDataExchange* pDX );
  void OnScaleButton();
  void OnRgnTypeChange();
  int  OnInitDialog();
  static int IsRectRgn()      { return m_nRgnType == 0; }
  static int IsUnrectRgn()    { return m_nRgnType == 1; }
  static int IsAutoRgn()      { return m_bRgnAuto;      }
  static int IsToClipboard()  { return m_bToClipboard;  }
  static int IsToFile()       { return m_bToFile;       }
  static int IsToMetafile()   { return m_bToMetafile;   }
  static int GetColorEnb()    { return m_bColorEnb;     }
  static int GetSelectEnb()   { return m_bSelectEnb;    }
  static int GetCurScaleEnb() { return m_bCurScaleEnb;  }
  static int GetScale()       { return m_nScale;        }
  static int GetExtraSize()   { return m_nExtraSize;    }

private:
  CIntEdit    m_Edit;
  static int  m_bRgnAuto;
  static int  m_nRgnType;
  static int  m_bToClipboard;
  static int  m_bToFile;
  static int  m_bToMetafile;
  static long m_nExtraSize;
  static int  m_bColorEnb;
  static int  m_bSelectEnb;
  static int  m_bCurScaleEnb;
  static int  m_nScale;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
struct CheckDef
{
  char const* Name;
  int* pValue;
};

class CCheckDialog: public CDialog
{
public:
  CCheckDialog( char const* name, CheckDef* pCheck ):
    CDialog( IDD_CHECKDLG, 0 ),
    m_Name( name ),
    m_pCheck( pCheck ),
    m_pBoxes( 0 )
  {}
  ~CCheckDialog() { delete[] m_pBoxes; }
  int  OnInitDialog();
  void OnOK();

private:
  char const* m_Name;
  CheckDef*   m_pCheck;
  CButton*    m_pBoxes;
};
//=====================================================================
class CObjSelectDialog: public CDialog
{
  DECLARE_DYNCREATE( CObjSelectDialog )

public:
  void Init( CMapView* pView, ObjectInfoArray* pArray );
  void UpdateClass( int n );
  void UpdateObject( int n );
  void OnObjListDblClick( NMHDR*, LRESULT* );
  int  OnInitDialog();
  void OnCancel();
  BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );

private:
  CMapView* m_pView;
  ObjectInfoArray* m_pArray;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
class CCloneExampleBox : public CListBox
{
  public:
    CCloneExampleBox();
    ~CCloneExampleBox();
    void Init( CPoint* pp, int np );
  private:
    void DrawItem( LPDRAWITEMSTRUCT lpDIS );
    void OnLButtonDown( UINT nFlags, CPoint point );

  private:
    CPoint* m_Points[3];
    int     m_nPoint[3];

  public:
    int     m_nDir;

    DECLARE_MESSAGE_MAP()
};
	
class CCloneParamDialog : public CDialog
{
  friend class CCloneExampleBox;

public:
  CCloneParamDialog( CPoint* pp, int np ) :
                CDialog( IDD_CLONE_PARAM_DIALOG, 0 ),
                m_Points( pp ),
                m_nPoint( np )
        {}
  int OnInitDialog();
  void DoDataExchange( CDataExchange* pDX );

  int GetStep();
  int GetRep()  { return m_nRep; }

private:
  static long m_Step;
  static long m_nRep;
  CIntEdit    m_Edit0;
  CFloatEdit  m_Edit1;
  CPoint* m_Points;
  int     m_nPoint;
  CCloneExampleBox m_ExampleBox;

  DECLARE_MESSAGE_MAP()
};
//=====================================================================
#endif
