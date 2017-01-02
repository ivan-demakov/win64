#include "stdafx.h"

#include <fstream>
#include <limits.h>

#include <afxcmn.h>
#include <math.h>

#include "shabdlg.h"
#include "linetmpl.h"
#include "mathutil.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "resource.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "util.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=========================================================================
void
SD_POLY::KillPoints()
{
  for( SD_POINT* pp ; pp = pFirstPoint ; pFirstPoint = pp->pNextPoint, delete pp );
  pFirstPoint = 0;
  length = 0;
}
//=========================================================================
SD_POLY::~SD_POLY()
{
  KillPoints();
}
//=========================================================================
CShabDlg::Template::~Template()
{
  for( SD_POLY* pp ; pp = m_pFstPoly ; m_pFstPoly = pp->pNextPoly, delete pp );
}
//=========================================================================
void
CShabDlg::OnSelect( CPoint point )
{
  if( m_nMode == NO_MODE )
    FindPoly( point );
  if( m_pCurPoly )
    SetDlgItemInt( IDC_LTD_WIDTH_EDIT, m_pCurPoly->width );
}
//=========================================================================
void
CShabDlg::MakeTmpPoly( char const* PrcName )
{
  static char const varName[] = "max-delta";
  ksi_obj ksi_val = ksi_eval_str_with_catch( varName );

  int delta = IsDlgButtonChecked( IDC_LTD_GRID_CHECK ) ?
    GetDlgItemInt( IDC_LTD_GRID_STEP_EDIT ) : 1;

  ksi_defsym( varName, ksi_int2num( delta ), ksi_current_env());

  ksi_obj ksi_lst = ksi_nil;

  for( SD_POINT* pp = m_pCurPoly->pFirstPoint ; pp ; pp = pp->pNextPoint )
    ksi_lst = ksi_cons( KsiPoint( pp->point ), ksi_lst );

  m_TmpPoly.KillPoints();

  ksi_lst = KsiCall( PrcName, ksi_reverse( ksi_lst ), 0 );

  m_TmpPoly.length = KSI_LIST_LEN( ksi_lst );

  for( SD_POINT** ppp = &m_TmpPoly.pFirstPoint ; KSI_PLIST_P( ksi_lst ) ;
       ksi_lst = KSI_CDR( ksi_lst ))
  {
    *ppp = new SD_POINT( CppPoint( KSI_CAR( ksi_lst )), 0 );
    ppp = &(*ppp)->pNextPoint;
  }

  ksi_defsym( varName, ksi_val, ksi_current_env());
}
//=========================================================================
void
CShabDlg::OnLButtonDown( CPoint point )
{
  CPoint p( point );
  UpdateCoord( p );

  switch( m_nMode )
  {
    case INSERT_ARC_MODE :
      if( !m_pCurPoly )
      {
        m_pCurPoly = new SD_POLY( GetDlgItemInt( IDC_LTD_WIDTH_EDIT ), m_pFstPoly );
        m_ppPrvPoint = &m_pCurPoly->pFirstPoint;
      }
      if( m_pCurPoly->length < 3 )
        do
        {
          *m_ppPrvPoint = m_pCurPoint = new SD_POINT( p, *m_ppPrvPoint );
          m_ppPrvPoint = &m_pCurPoint->pNextPoint;
        }
        while( ++m_pCurPoly->length < 2 );
      else
        StopMode();
      break;
    case INSERT_SPLINE_MODE :
      if( !m_pCurPoly )
      {
        m_pCurPoly = new SD_POLY( GetDlgItemInt( IDC_LTD_WIDTH_EDIT ), m_pFstPoly );
        m_ppPrvPoint = &m_pCurPoly->pFirstPoint;
      }
      do
      {
        *m_ppPrvPoint = m_pCurPoint = new SD_POINT( p, *m_ppPrvPoint );
        m_ppPrvPoint = &m_pCurPoint->pNextPoint;
      }
      while( ++m_pCurPoly->length < 2 );
      break;
    case INSERT_POLY_MODE :
      if( !m_pCurPoly )
      {
        m_pCurPoly = m_pFstPoly =
          new SD_POLY( GetDlgItemInt( IDC_LTD_WIDTH_EDIT ), m_pFstPoly );
        m_ppPrvPoly  = &m_pFstPoly;
        m_ppPrvPoint = &m_pCurPoly->pFirstPoint;
        mb_Changing = 1;
      }
      AddPoint( p );
      break;
    case MOVE_POLY_MODE :
      if( FindPoly( point ))
        m_StartPoint = m_TracePoint = p;
      break;
    case INSERT_POINT_MODE :
      if( !FindPoly( point ))
        break;
      AddPoint( p );
    case MOVE_POINT_MODE :
      if( FindPoint( point ))
      {
        mb_Changing ^= 1;
        AddUndo( new MovePointItem( m_pCurPoint, m_pCurPoly ));
      }
      break;
    case DELETE_POINT_MODE :
      if( !FindPoint( point ))
        break;
      if( m_pCurPoly->length > 2 )
      {
        AddUndo( new InsertPointItem( 0, m_pCurPoint, m_ppPrvPoint, m_pCurPoly ));
        DrawPoly();
        m_pCurPoint = *m_ppPrvPoint = m_pCurPoint->pNextPoint;
        DrawPoly();
        m_pCurPoly->length -= 1;
        break;
      }
    case DELETE_POLY_MODE :
    {
      SD_POLY* pcp = m_pCurPoly;
      if( FindPoly( point ) && m_pCurPoly == pcp )
      {
        *m_ppPrvPoly = m_pCurPoly->pNextPoly;
        m_pCurPoly->pNextPoly = 0;
        AddUndo( new InsertPolyItem( 0, m_ppPrvPoly, m_pCurPoly ));
        SelectPoly( 0 );
      }
      break;
    }
  }
}
//=========================================================================
void
CShabDlg::OnMouseMove( CPoint p )
{
  UpdateCoord( p );
  ShowCoords( p );

  switch( m_nMode )
  {
    case INSERT_ARC_MODE :
      if( m_pCurPoly )
      {
        DrawPoly();
        m_pCurPoint->Change( p );
        MakeTmpPoly( "make-arced" );
        DrawPoly();
      }
      break;
    case INSERT_SPLINE_MODE :
      if( m_pCurPoly )
      {
        DrawPoly();
        m_pCurPoint->Change( p );
        MakeTmpPoly( "make-splined-hard" );
        DrawPoly();
      }
    case INSERT_POLY_MODE :
      ChangePoint( p );
      break;
    case MOVE_POLY_MODE :
      if( m_pCurPoly )
      {
        DrawPoly();
        CSize d( p - m_TracePoint );
        for( SD_POINT* pp = m_pCurPoly->pFirstPoint ; pp ; pp = pp->pNextPoint )
          pp->point.Offset( d );
        DrawPoly();
      }
      m_TracePoint = p;
      break;
    case INSERT_POINT_MODE :
    case MOVE_POINT_MODE :
      if( m_pCurPoint )
        ChangePoint( p );
      break;
  }
}
//=========================================================================
void
CShabDlg::OnLButtonUp( CPoint p )
{
  UpdateCoord( p );
  switch( m_nMode )
  {
    case MOVE_POLY_MODE :
      if( m_pCurPoly )
        AddUndo( new MovePolyItem( m_TracePoint - m_StartPoint, m_pCurPoly ));
      SelectPoly( 0 );
      break;
    case MOVE_POINT_MODE :
    case INSERT_POINT_MODE :
      mb_Changing ^= 1;
      break;
  }
}
//=========================================================================
void
SD_POLY::Offset( int dx, int dy )
{
  for( SD_POLY* pPoly = this ; pPoly ; pPoly = pPoly->pNextPoly )
    for( SD_POINT* pPoint = pPoly->pFirstPoint ; pPoint ; pPoint = pPoint->pNextPoint )
      pPoint->point.Offset( dx, dy );
}
//=========================================================================
void
SD_POLY::CopyPoints( SD_POLY* pPoly )
{
  length = pPoly->length;
  SD_POINT** ppCurPoint = &pFirstPoint;
  for( SD_POINT* pPoint = pPoly->pFirstPoint ; pPoint ; pPoint = pPoint->pNextPoint )
  {
    *ppCurPoint = new SD_POINT( pPoint->point, 0 );
    ppCurPoint = &( *ppCurPoint )->pNextPoint;
  }
}
//=========================================================================
SD_POLY*
SD_POLY::CopyPoly()
{
  SD_POLY* pFstPoly = 0;
  SD_POLY** ppCurPoly = &pFstPoly;

  for( SD_POLY* pPoly = this ; pPoly ; pPoly = pPoly->pNextPoly )
    if( pPoly->length > 1 )
    {
      *ppCurPoly = new SD_POLY( pPoly->width, 0 );
      ( *ppCurPoly )->CopyPoints( pPoly );
      ppCurPoly = &( *ppCurPoly )->pNextPoly;
    }
  return pFstPoly;
}
//=========================================================================
void
SD_POLY::RemoveDuplicates()
{
  SD_POINT* rm;
  for( SD_POLY* pPoly = this ; pPoly ; pPoly = pPoly->pNextPoly )
    for( SD_POINT* pp = pPoly->pFirstPoint ; pp && ( rm = pp->pNextPoint ) ; pp = pp->pNextPoint )
      if( pp->point == rm->point )
      {
        pp->pNextPoint = rm->pNextPoint;
        delete rm;
        --pPoly->length;
      }
}
//=========================================================================
CRect
SD_POLY::CalcBoundBox( int bSel )
{
  CRect r( INT_MAX, INT_MAX, INT_MIN, INT_MIN );

  for( SD_POLY* pPoly = this ; pPoly ; pPoly = pPoly->pNextPoly )
  {
    for( SD_POINT* pp = pPoly->pFirstPoint ; pp ; pp = pp->pNextPoint )
      r.SetRect( min( pp->point.x, r.left ),
                 min( pp->point.y, r.top ),
                 max( pp->point.x, r.right ),
                 max( pp->point.y, r.bottom ));
    int h = bSel ? max( pPoly->width / 4, 2 ) : pPoly->width + 1 >> 1;
    int w = bSel ? h : 0;
    r.InflateRect( w, h );
  }
  return r;
}
//=========================================================================
IMPLEMENT_DYNCREATE( CShabDlg, CDialog )
//=========================================================================
BEGIN_MESSAGE_MAP( CShabDlg, CDialog )
  ON_BN_CLICKED( IDC_LTD_INSERT_POINT_BUTTON, OnModeButtonClicked )
  ON_BN_CLICKED( IDC_LTD_INSERT_POLY_BUTTON,  OnModeButtonClicked )
  ON_BN_CLICKED( IDC_LTD_INSERT_ARC_BUTTON,   OnModeButtonClicked )
  ON_BN_CLICKED( IDC_LTD_INSERT_SPLINE_BUTTON,OnModeButtonClicked )
  ON_BN_CLICKED( IDC_LTD_MOVE_POINT_BUTTON,   OnModeButtonClicked )
  ON_BN_CLICKED( IDC_LTD_MOVE_POLY_BUTTON,    OnModeButtonClicked )
  ON_BN_CLICKED( IDC_LTD_DELETE_POINT_BUTTON, OnModeButtonClicked )
  ON_BN_CLICKED( IDC_LTD_DELETE_POLY_BUTTON,  OnModeButtonClicked )
  ON_BN_CLICKED( IDC_LTD_SAVE_BUTTON,         OnSaveButtonClicked )
  ON_BN_CLICKED( IDC_LTD_EDIT_BUTTON,         OnEditButtonClicked )
  ON_BN_CLICKED( IDC_LTD_CLEAR_BUTTON,        OnClearButtonClicked )
  ON_BN_CLICKED( IDC_LTD_RESET_BUTTON,        OnResetButtonClicked )
  ON_LBN_SELCHANGE( IDC_LTD_TMPL_LIST,        OnStyleChange )
  ON_BN_CLICKED( IDC_LTD_UNDO_BUTTON,         OnUndoButtonClicked )
  ON_BN_CLICKED( IDC_LTD_REDO_BUTTON,         OnRedoButtonClicked )
  ON_BN_CLICKED( IDC_LTD_REMOVE_TMPL_BUTTON,  OnRemoveTmplButtonClicked )
  ON_BN_CLICKED( IDC_LTD_GRID_CHECK,          OnGridClicked )
  ON_EN_CHANGE( IDC_LTD_GRID_STEP_EDIT,       OnGridClicked )
  ON_EN_CHANGE( IDC_LTD_WIDTH_EDIT,           ApplyNewWidth )
  ON_EN_CHANGE( IDC_LTD_PERIOD_EDIT,          ApplyNewPeriod )
END_MESSAGE_MAP()
//=========================================================================
CShabDlg::~CShabDlg()
{
  for( SD_POLY* pp ; pp = m_pFstPoly ; m_pFstPoly = pp->pNextPoly, delete pp );
  for( int i = m_TmplLib.GetSize() ; --i >= 0 ; delete m_TmplLib.GetAt( i ));
  RemoveUndo( &m_UndoHead );
  RemoveUndo( &m_RedoHead );
}
//=========================================================================
CShabDlg::CShabDlg() :
  m_bActive( 0 ),
  m_pExample( 0 ),
  m_nMode( NO_MODE ),
  m_pCurPoly( 0 ),
  m_pFstPoly( 0 ),
  m_UndoHead( 0 ),
  m_RedoHead( 0 ),
  m_Scale( 1000 ),
  m_Width( 0 ),
  m_Period( 100 ),
  m_GridStep( 10 )
{
  std::ifstream is( App.GetProjectFilePath( CLineTmpl::TmplFileName ), std::ios::binary );

  try
  {
    if( is.fail())
      throw( 0 );

    TCHAR b[32] = "";

    is.read( b, 1 + strlen( CLineTmpl::m_Ttl0 ));
    if( is.bad() || is.eof())
       throw( 1 );

    int v0 = !strcmp( b, CLineTmpl::m_Ttl0 );
    int v1 = !strcmp( b, CLineTmpl::m_Ttl1 );

    if( !v0 && !v1 )
       throw( 1 );

    int n;
    is.read( LPTSTR( &n ), sizeof n );
    if( is.bad() || is.eof())
      throw( 2 );

    m_TmplLib.SetSize( n );

    CPoint cp( m_ExRect.CenterPoint());

    int itl = 1 + strlen( CLineTmpl::m_Th );

    for( int i = 0 ; i < n ; ++i )
    {
      is.read( b, itl );
      if( is.bad() || is.eof() || strcmp( b, CLineTmpl::m_Th ))
        throw( 2 );

      Template* pTmpl = m_TmplLib[i] = new Template;
      is.read( LPTSTR( &pTmpl->m_nIndex ), sizeof( int ));
      is.read( LPTSTR( &pTmpl->m_nScale ), sizeof( int ));
      is.read( LPTSTR( &pTmpl->m_Period ), sizeof( int ));
      is.read( LPTSTR( &pTmpl->m_xS ), sizeof( int ));
      is.read( LPTSTR( &pTmpl->m_yS ), sizeof( int ));

      if( v0 )
        pTmpl->m_nScale *= 40;

      if( is.bad() || is.eof())
        throw( 2 );

      for( ;; )
      {
        int len, wid;
        is.read( LPTSTR( &len ), sizeof( int ));
        if( !len )
          break;

        is.read( LPTSTR( &wid ), sizeof( int ));
        if( is.bad() || is.eof())
          throw( 2 );

        pTmpl->m_pFstPoly = new SD_POLY( wid, pTmpl->m_pFstPoly );
        pTmpl->m_pFstPoly->length = len;
        while( --len >= 0 )
        {
          CPoint pnt;
          is.read( LPTSTR( &pnt ), sizeof( CPoint ));
          if( is.bad() || is.eof())
            throw( 2 );
           pTmpl->m_pFstPoly->pFirstPoint = new SD_POINT( pnt, pTmpl->m_pFstPoly->pFirstPoint );
        }
      }
    }
  }

  catch( int ec )
  {
    TCHAR* msg[] =
    {
      0,
     "Некорректное начало файла шаблонов!",
     "Некорректная структура файла шаблонов!"
    };

    TCHAR* pm = msg[ec];
    if( pm )
      ErrBox( pm );
  }

  is.close();
}
//=========================================================================
void
CShabDlg::RemoveAll()
{
  for( SD_POLY* pp ; pp = m_pFstPoly ; m_pFstPoly = pp->pNextPoly, delete pp );
  m_pCurPoly = m_pFstPoly = 0;
  m_TmpPoly.KillPoints();
  RemoveUndo( &m_UndoHead );
  RemoveUndo( &m_RedoHead );
}
//=========================================================================

void
CShabDlg::Activate( CWnd* pView )
{
  if( Create( IDD_LINE_TEMPL_DIALOG, pView ))
  {
    m_bActive = 1;
    ShowWindow( SW_SHOW );
  }
}
//=========================================================================
int
CShabDlg::OnInitDialog()
{
  CWaitCursor wc;
  CDialog::OnInitDialog();

  static UINT bb[][2] =
  {
    IDC_LTD_REMOVE_TMPL_BUTTON, IDB_DELETE,
    IDC_LTD_SAVE_BUTTON,        IDB_L_ARROW,
    IDC_LTD_EDIT_BUTTON,        IDB_R_ARROW,
    IDC_LTD_UNDO_BUTTON,        IDB_CR_ARROW,
    IDC_LTD_REDO_BUTTON,        IDB_CW_ARROW
  };

  int i;
  for( i = CELEM( bb ) ; --i >= 0 ; )
  {
    HBITMAP hBmp = (HBITMAP)LoadImage( App.m_hInstance, LPCTSTR( bb[i][1] ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
    if( hBmp )
      ((CButton*) GetDlgItem( bb[i][0] ))->SetBitmap( hBmp );
  }

  GetWindowText( m_DlgCap );
  m_List.SubclassDlgItem( IDC_LTD_TMPL_LIST, this );
  CRect r;
  m_List.GetClientRect( &r );
  m_List.SetItemHeight( 0, r.Height() / 12 );

  WINDOWPLACEMENT wp;
  GetDlgItem( IDC_LTD_VIEW_STATIC )->GetWindowPlacement( &wp );

  m_pExample = new CExampleBox;
  m_pExample->Create( 0, 0, WS_CHILD | WS_VISIBLE | WS_BORDER  | WS_OVERLAPPED,
                      wp.rcNormalPosition, this, IDC_LTD_VIEW_STATIC1 );
  m_pExample->GetClientRect( &m_ExRect );

  static CSpinIni
    S0 = { IDC_LTD_SCALE_EDIT, IDC_LTD_SCALE_SPIN, &m_Scale, 100, 100, 1000000 },
    S1 = { IDC_LTD_WIDTH_EDIT, IDC_LTD_WIDTH_SPIN, &m_Width, 0, 1, MAX_LINE_WIDTH },
    S2 = { IDC_LTD_PERIOD_EDIT, IDC_LTD_PERIOD_SPIN, &m_Period, 2, 1, MAX_TMPL_PERIOD },
    S3 = { IDC_LTD_GRID_STEP_EDIT, IDC_LTD_GRID_STEP_SPIN, &m_GridStep, 4, 1, 64 };

  m_ScaleEdit.Initialize( S0, this );
  m_WidthEdit.Initialize( S1, this );
  m_PeriodEdit.Initialize( S2, this );
  m_GridStepEdit.Initialize( S3, this );

  for( i = m_TmplLib.GetSize() ; --i >= 0 ; )
  {
    Template* pTmpl = m_TmplLib.GetAt( i );
    m_List.SetItemData( m_List.AddString( 0 ), pTmpl->m_nIndex );
  }

  StopMode();
  m_nIndex = -1;
  m_bMod = 0;

  RemoveUndo( &m_UndoHead );
  RemoveUndo( &m_RedoHead );

  return 1;
}
//=========================================================================
void
CShabDlg::OnStyleChange()
{
  SetDlgItemInt( IDC_LTD_TMPLNUM_EDIT, m_nIndex = m_List.GetItemData( m_List.GetCurSel()));
}
//=========================================================================
int
CShabDlg::GetNewIndex()
{
	int n;
  for(n = MIN_TMPL_INDEX ; GetTmpl( n ) ; ++n );
  return n;
}
//=========================================================================
void
CShabDlg::OnClearButtonClicked()
{
  if( AskBox( "Вы действительно хотите\nочистить окно редактора?" ) == IDYES )
  {
    RemoveAll();
    DrawAll();
  }
}
//=========================================================================
void
CShabDlg::OnResetButtonClicked()
{
  m_nIndex = -1;
  m_List.SetCurSel( LB_ERR );
  SetDlgItemText( IDC_LTD_TMPLNUM_EDIT, "" );
  StopMode();
}
//=========================================================================
void
CShabDlg::OnRemoveTmplButtonClicked()
{
  int nSel = m_List.GetCurSel();
  if( nSel >= 0 &&
      AskBox( "Вы действительно хотите удалить\nвыделенный шаблон?" ) == IDYES )
  {
    int ndx = m_List.GetItemData( nSel );
    m_List.DeleteString ( nSel );
    Template* pTmpl = GetTmpl( ndx );
    m_TmplLib.RemoveAt( nSel );
    delete pTmpl;
    m_bMod = 1;
  }
}
//=========================================================================
void
CShabDlg::OnEditButtonClicked()
{
  if( !m_List.GetCount())
    return;

  int nSel = m_List.GetCurSel();
  if( nSel == LB_ERR )
    return;

  m_nIndex = m_List.GetItemData( nSel );
  SetDlgItemInt( IDC_LTD_TMPLNUM_EDIT, m_nIndex );

  Template* pTmpl = GetTmpl( m_nIndex );
  delete m_pFstPoly;
  m_pFstPoly = pTmpl->m_pFstPoly ->CopyPoly();
  CPoint cp( m_ExRect.CenterPoint());
  m_pFstPoly->Offset( pTmpl->m_xS + cp.x, pTmpl->m_yS + cp.y );
  SetDlgItemInt( IDC_LTD_SCALE_EDIT, pTmpl->m_nScale );
  SetDlgItemInt( IDC_LTD_PERIOD_EDIT, pTmpl->m_Period );
  StopMode();
  RemoveUndo( &m_UndoHead );
  RemoveUndo( &m_RedoHead );
  DrawAll();
}
//=========================================================================
void
CShabDlg::OnSaveButtonClicked()
{
  if( !m_pFstPoly )
    return;

  int sf = GetDlgItemInt( IDC_LTD_SCALE_EDIT );
  if( sf <= 0 )
  {
    ErrBox( "Не выбран или некорректный масштаб!" );
    return;
  }

  m_pFstPoly->RemoveDuplicates();
  CRect prc( m_pFstPoly->CalcBoundBox());

  int pr = GetDlgItemInt( IDC_LTD_PERIOD_EDIT );

  if( !pr )
  {
    ErrBox( "Шаблон имеет нулевой период!" );
    return;
  }

  if( m_nIndex < 0 )
    m_nIndex = GetNewIndex();
  else
  {
	  int i;
    for(i = m_List.GetCount() ; --i >= 0 && m_List.GetItemData( i ) != m_nIndex ; );
    if( i >= 0 )
    {
      m_List.DeleteString( i );
      Template* t = m_TmplLib.GetAt( i );
      m_TmplLib.RemoveAt( i );
      delete t;
    }
  }

  int i = m_List.AddString( 0 );
  m_List.SetItemData( i, m_nIndex );

  CPoint cp( m_ExRect.CenterPoint());
  int xs = prc.left;
  int ys = cp.y;
  m_pFstPoly->Offset( -xs, -ys );
  m_TmplLib.InsertAt( i, new Template( m_pFstPoly, m_nIndex, xs - cp.x, 0, pr, sf ));
  m_pFstPoly->Offset( prc.left, cp.y );
  m_List.SetCurSel( i );

  m_nIndex = -1;
  StopMode();
  delete m_pFstPoly;
  m_pFstPoly = 0;
  m_bMod = 1;
  DrawAll();
  RemoveUndo( &m_UndoHead );
  RemoveUndo( &m_RedoHead );
  OnStyleChange();
}
//=========================================================================
static UINT md[][3] =
{
  IDC_LTD_INSERT_POINT_BUTTON, INSERT_POINT_MODE, IDC_LTD_INSERT_CURSOR,
  IDC_LTD_INSERT_POLY_BUTTON,  INSERT_POLY_MODE,  IDC_LTD_INSERT_CURSOR,
  IDC_LTD_INSERT_ARC_BUTTON,   INSERT_ARC_MODE,   IDC_LTD_INSERT_CURSOR,
  IDC_LTD_INSERT_SPLINE_BUTTON,INSERT_SPLINE_MODE,IDC_LTD_INSERT_CURSOR,
  IDC_LTD_MOVE_POLY_BUTTON,    MOVE_POLY_MODE,    IDC_LTD_MOVE_CURSOR,
  IDC_LTD_MOVE_POINT_BUTTON,   MOVE_POINT_MODE,   IDC_LTD_MOVE_CURSOR,
  IDC_LTD_DELETE_POLY_BUTTON,  DELETE_POLY_MODE,  IDC_LTD_DELETE_CURSOR,
  IDC_LTD_DELETE_POINT_BUTTON, DELETE_POINT_MODE, IDC_LTD_DELETE_CURSOR
};
//=========================================================================
void
CShabDlg::SelectPoly( SD_POLY* pSel )
{
  mb_Changing = 0;
  if( m_pCurPoly = pSel )
    SetDlgItemInt( IDC_LTD_WIDTH_EDIT, m_pCurPoly->width );
  DrawAll();
}
//=========================================================================
void
CShabDlg::OnCancel()
{
  if( m_pCurPoly )
    SelectPoly( 0 );

  if( m_nMode != NO_MODE )
    StopMode();
  else
  {
    int r = m_bMod ? AskBox( "Библиотека шаблонов изменена!\nСохранить изменения?", 1 ) : IDNO;
    switch( r )
    {
      case IDCANCEL : return;
      case IDYES    : OnOK();
    }

    delete m_pExample;
    RemoveAll();
    DestroyWindow();
    m_bActive = 0;
  }
}
//=========================================================================
void
CShabDlg::OnOK()
{
  if( m_nMode != NO_MODE )
    return;

  if( m_bMod )
  {
    m_bMod = 0;
	std::ofstream os( App.GetProjectFilePath( CLineTmpl::TmplFileName ), std::ios::binary | std::ios::trunc );

    int n = m_TmplLib.GetSize();

    os.write( CLineTmpl::m_Ttl1, 1 + strlen( CLineTmpl::m_Ttl1 ));
    os.write( LPTSTR( &n ), sizeof n );

    for( int i = 0 ; i < n ; ++i )
    {
      Template* pTmpl = m_TmplLib[i];

      os.write( CLineTmpl::m_Th, 1 + strlen( CLineTmpl::m_Th ));
      os.write( LPTSTR( &pTmpl->m_nIndex ), sizeof( int ));
      os.write( LPTSTR( &pTmpl->m_nScale ), sizeof( int ));
      os.write( LPTSTR( &pTmpl->m_Period ), sizeof( int ));
      os.write( LPTSTR( &pTmpl->m_xS ), sizeof( int ));
      os.write( LPTSTR( &pTmpl->m_yS ), sizeof( int ));

      for( SD_POLY* pPoly = pTmpl->m_pFstPoly ; pPoly ; pPoly = pPoly->pNextPoly )
      {
        os.write( LPTSTR( &pPoly->length ), sizeof( int ));
        os.write( LPTSTR( &pPoly->width  ), sizeof( int ));
        for( SD_POINT* pPoint = pPoly->pFirstPoint ; pPoint ; pPoint = pPoint->pNextPoint )
          os.write( LPTSTR( pPoint ), sizeof( CPoint ));
      }
      int z = 0;
      os.write( LPTSTR( &z ), sizeof( int ));
    }
    os.close();
  }
  CLineTmpl::DeleteTemplates();
  CLineTmpl::CreateTemplates();
}
//=========================================================================
void
CShabDlg::OnModeButtonClicked()
{
	int i;
  for( i = CELEM( md ) ; --i >= 0 && !IsDlgButtonChecked( md[i][0] ) ; );
  if( i < 0 )
    return;
  m_nMode = SD_MODE( md[i][1] );
  m_pExample->SetWndCursor( md[i][2] );
  SelectPoly( 0 );
}
//=========================================================================
void
CShabDlg::StopMode()
{
  if( m_pCurPoly && m_TmpPoly.length > 0 )
  {
    m_pFstPoly = m_pCurPoly->pNextPoly;
    delete m_pCurPoly;
    m_pCurPoly = 0;
    m_pFstPoly = new SD_POLY( GetDlgItemInt( IDC_LTD_WIDTH_EDIT ), m_pFstPoly );
    m_ppPrvPoly = &m_pFstPoly;
    m_pFstPoly->CopyPoints( &m_TmpPoly );
    m_TmpPoly.KillPoints();
    AddUndo( new InsertPolyItem( 1, m_ppPrvPoly, m_pFstPoly ));
  }

  if( m_nMode != INSERT_POLY_MODE &&
      m_nMode != INSERT_SPLINE_MODE &&
      m_nMode != INSERT_ARC_MODE ||
      !m_pCurPoly ||
      m_pCurPoly->length < 2 )
  {
    for( int i = CELEM( md ) ; --i >= 0 ; CheckDlgButton( md[i][0], 0 ));
    m_nMode = NO_MODE;
    m_pExample->SetWndCursor( 0 );
  }
  SelectPoly( 0 );
}
//=========================================================================
static int
PointDetected( CPoint p0, CPoint detector, int w )
{
  return dist( detector, p0 ) <= w;
}
//=========================================================================
static int
LineDetected( CPoint p0, CPoint p1, CPoint detector, int w )
{
  if( PointDetected( p0, detector, w ) || PointDetected( p1, detector, w ))
    return 1;

  CPoint p( detector - p0 );
  CSize s( p1 - p0 );
  double dl = hipo( s.cx, s.cy );

  if( dl )
    return 0;

  p = TurnPoint( p, s.cx, s.cy, dl );
  return p.x >= 0 && p.x < dl && p.y >= -w  && p.y < w;
}
//=========================================================================
int
CShabDlg::FindPoly( CPoint pnt )
{
  for( int s = 2 ; --s >= 0 ; )
    for( m_ppPrvPoly = &m_pFstPoly ; *m_ppPrvPoly ; m_ppPrvPoly = &(*m_ppPrvPoly)->pNextPoly )
      if( !s || *m_ppPrvPoly == m_pCurPoly )
      {
        int w = max( (*m_ppPrvPoly)->width / 2, 3 );
        for( m_ppPrvPoint = &(*m_ppPrvPoly)->pFirstPoint ;
             *m_ppPrvPoint && (*m_ppPrvPoint)->pNextPoint ;
             m_ppPrvPoint = &(*m_ppPrvPoint)->pNextPoint )
          if( LineDetected((*m_ppPrvPoint)->point,
                           (*m_ppPrvPoint)->pNextPoint->point, pnt, w ))
          {
            m_ppPrvPoint = &(*m_ppPrvPoint)->pNextPoint;
            SelectPoly( *m_ppPrvPoly );
            return 1;
          }
      }

  m_pCurPoint  = 0;
  return 0;
}
//=========================================================================
int
CShabDlg::FindPoint( CPoint pnt )
{
  if( !FindPoly( pnt ))
    return 0;

  int w = max( m_pCurPoly->width / 2, 3 );
  SD_POINT** pp;
  for( pp = &m_pCurPoly->pFirstPoint ;
       *pp && !PointDetected((*pp)->point, pnt, w ) ;
       pp = &(*pp)->pNextPoint );

  return ( m_pCurPoint = pp ? *( m_ppPrvPoint = pp ) : 0 ) != 0;
}
//=========================================================================
void
CShabDlg::AddPoint( CPoint pnt )
{
  ASSERT( m_ppPrvPoint );

  DrawPoly();

  SD_POINT** pp;

  do
  {
    *m_ppPrvPoint = m_pCurPoint = new SD_POINT( pnt, *m_ppPrvPoint );
    pp = m_ppPrvPoint;
    m_ppPrvPoint = &m_pCurPoint->pNextPoint;
  }
  while( ++m_pCurPoly->length < 2 );

  DrawPoly();

  if( m_pCurPoly->length == 2 )
    AddUndo( new InsertPolyItem( 1, m_ppPrvPoly, m_pCurPoly ));
  else
    AddUndo( new InsertPointItem( 1, m_pCurPoint, pp, m_pCurPoly ));
}
//=========================================================================
void
CShabDlg::ChangePoint( CPoint pnt )
{
  if( !m_pCurPoly || !m_pCurPoly->length || !mb_Changing )
    return;

  DrawPoly( 0, 0 );
  m_pCurPoint->Change( pnt );
  DrawPoly( 0, 0 );
}
//=========================================================================
void
CShabDlg::OnGridClicked()
{
  DrawAll();
}
//=========================================================================
void
CShabDlg::ApplyNewWidth()
{
  if( !m_pCurPoly )
    return;

  DrawPoly();
  m_pCurPoly->width =
  m_TmpPoly.width = min( GetDlgItemInt( IDC_LTD_WIDTH_EDIT ), MAX_LINE_WIDTH );
  DrawPoly();
}
//=========================================================================
void
CShabDlg::ApplyNewPeriod()
{
  m_PeriodEdit.SetValue();
  DrawAll();
}
//=========================================================================
void
CShabDlg::UpdateCoord( CPoint& pnt )
{
  int gs = GetDlgItemInt( IDC_LTD_GRID_STEP_EDIT );
  if( gs > 1 && IsDlgButtonChecked( IDC_LTD_GRID_CHECK ))
  {
    CPoint cp( m_ExRect.CenterPoint());
    pnt -= cp;
    pnt.x = cp.x + ( pnt.x + ( pnt.x > 0 ? gs : -gs ) / 2 ) / gs * gs;
    pnt.y = cp.y + ( pnt.y + ( pnt.y > 0 ? gs : -gs ) / 2 ) / gs * gs;
  }
}
//=========================================================================
void
CShabDlg::ShowCoords( CPoint p )
{
  char b[128];
  p -= m_ExRect.CenterPoint();
  sprintf( b, "X = %3d", p.x );
  SetDlgItemText( IDC_LTD_XVAL_STATIC, b );
  sprintf( b, "Y = %3d", -p.y );
  SetDlgItemText( IDC_LTD_YVAL_STATIC, b );
}
//=========================================================================
void
CShabDlg::DrawPoly( SD_POLY* pPoly, int bFull )
{
  if( !( pPoly ||
      ( pPoly = &m_TmpPoly ) && pPoly->length > 0 ||
      ( pPoly = m_pCurPoly )))
    return;

  int bSel = pPoly == m_pCurPoly || pPoly == &m_TmpPoly;
  bFull |= pPoly == &m_TmpPoly;

  CDC* pDC = m_pExample->GetDC();

  pDC->SetWindowExt( m_ExRect.BottomRight() - m_ExRect.TopLeft());
  pDC->SetViewportExt( m_ExRect.BottomRight() - m_ExRect.TopLeft());

  LOGBRUSH lb = { BS_SOLID, RGB( bSel ? 255 : 0, 0, 0 ), 0 };
  CPen pen( PS_GEOMETRIC|PS_ENDCAP_ROUND|PS_JOIN_ROUND, pPoly->width, &lb );

  CPen* pOldPen = pDC->SelectObject( &pen );
  int oldMode = pDC->SetROP2( bSel ? R2_NOTXORPEN : R2_COPYPEN );
  CPoint b[4096];
  CPoint* pb = b;
  int n = 0, p = 0;
  for( SD_POINT* pPnt = pPoly->pFirstPoint ;  n < CELEM( b ) && pPnt ; pPnt = pPnt->pNextPoint )
  {
    if( pPnt == m_pCurPoint )
      p = n;
    b[n++] = pPnt->point;
  }

  if( !bFull )
  {
    int bf = p == 0, bl = p == n - 1;
    int s = bf ? p : p - 1;
    int f = bl ? p : p + 1;
    pb += s;
    n = f - s + 1;
  }

  if( bSel )
  {
    int w = max( pPoly->width / 4, 2 );
    CBrush brush( RGB( 255, 0, 0 ));
    pDC->SelectObject( brush );
    pDC->MoveTo( *pb );
    for( CPoint* pPnt = pb ; --n >= 0 ; ++pPnt )
    {
      pDC->Ellipse( pPnt->x - w, pPnt->y - w, pPnt->x + w + 1, pPnt->y + w + 1 );
      n && pDC->LineTo( pPnt[1] );
    }
  }
  else
    pDC->Polyline( pb, n );

  pDC->SetROP2( oldMode );
  pDC->SelectObject( pOldPen );

  ReleaseDC( pDC );
}
//=========================================================================
void
CShabDlg::DrawAll()
{
  CDC* pDC = m_pExample->GetDC();

  pDC->SelectStockObject( WHITE_BRUSH );
  pDC->SelectStockObject( NULL_PEN );
  pDC->Rectangle( &m_ExRect );
  CPen pen( PS_SOLID, 0, RGB( 192, 192, 192 ));
  pDC->SelectObject( &pen );
  CPoint cp( m_ExRect.CenterPoint());

  pDC->MoveTo( m_ExRect.left  + 2, cp.y );
  pDC->LineTo( m_ExRect.right - 3, cp.y );
  cp.x -= m_Period / 2;
  pDC->MoveTo( cp.x, m_ExRect.top    + 2 );
  pDC->LineTo( cp.x, m_ExRect.bottom - 3 );
  cp.x += m_Period;
  pDC->MoveTo( cp.x, m_ExRect.top    + 2 );
  pDC->LineTo( cp.x, m_ExRect.bottom - 3 );

  int gs = GetDlgItemInt( IDC_LTD_GRID_STEP_EDIT );
  if( gs > 1 && IsDlgButtonChecked( IDC_LTD_GRID_CHECK ))
  {
    pDC->SelectStockObject( NULL_PEN );
    pDC->SelectStockObject( BLACK_BRUSH );
    for( int x = m_ExRect.left ; x < m_ExRect.right ; x += gs )
      for( int y = m_ExRect.top ; y < m_ExRect.bottom ; y += gs )
      {
        CPoint p( x, y );
        UpdateCoord( p );
        pDC->Rectangle( p.x - 1, p.y - 1, p.x + 1, p.y + 1 );
      }
  }

  for( SD_POLY* pPoly = m_pFstPoly ; pPoly ; pPoly = pPoly->pNextPoly )
    if( pPoly != m_pCurPoly )
      DrawPoly( pPoly );

  if( m_TmpPoly.length > 0 )
    DrawPoly( &m_TmpPoly );
  else
  if( m_pCurPoly )
    DrawPoly( m_pCurPoly );

  ReleaseDC( pDC );
}
//=========================================================================
CShabDlg::Template*
CShabDlg::GetTmpl( int nIndex )
{
	int i;
  for(i = m_TmplLib.GetSize() ; --i >= 0 && m_TmplLib[i]->m_nIndex != nIndex ; );
  return i >= 0 ? m_TmplLib[i] : 0;
}
//=========================================================================
//=========================================================================
IMPLEMENT_DYNCREATE( CExampleBox, CWnd )
//=========================================================================
BEGIN_MESSAGE_MAP( CExampleBox, CWnd )
  ON_WM_PAINT()
  ON_WM_SETCURSOR()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()
//=========================================================================
void
CExampleBox::OnPaint()
{
  CWnd::OnPaint();
  GetParent()->DrawAll();
}
//=========================================================================
int
CExampleBox::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
  return m_hWndCursor ? SetCursor( m_hWndCursor ) :
                        CWnd::OnSetCursor( pWnd, nHitTest, message ) != 0;
}
//=========================================================================
void
CExampleBox::OnLButtonDown( UINT nFlags, CPoint point )
{
  if( nFlags & MK_SHIFT )
    GetParent()->OnSelect( point );
  else
    GetParent()->OnLButtonDown( point );
}
//=========================================================================
void
CExampleBox::OnMouseMove( UINT nFlags, CPoint p )
{
  GetParent()->OnMouseMove( p );
}
//=========================================================================
void
CExampleBox::OnLButtonUp( UINT nFlags, CPoint p )
{
  GetParent()->OnLButtonUp( p );
}
//=========================================================================
void
CExampleBox::OnRButtonDown( UINT nFlags, CPoint p )
{
  GetParent()->StopMode();
}
//=========================================================================
void
CExampleBox::SetWndCursor( UINT id )
{
  m_hWndCursor = id ? App.LoadCursor( id ) : 0;
}
//=========================================================================
//=========================================================================
void
CShabDlg::CTmplListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
  CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );

  CRect r( lpDrawItemStruct->rcItem );
  CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( WHITE_BRUSH );
  CPen* pOldPen = (CPen*)pDC->SelectStockObject( NULL_PEN );
  pDC->Rectangle( &r );

  int sel = lpDrawItemStruct->itemState & ODS_SELECTED;

  CPen pen( PS_SOLID, sel ? 2 : 1, RGB( sel ? 0xff : 0, 0, 0 ));
  pDC->SelectObject( &pen );

  r.InflateRect( -2, -2 );
  pDC->Rectangle( &r );
  r.InflateRect( 2, 2 );

  r.InflateRect( -4, -2 );
  pDC->IntersectClipRect( &r );
  r.InflateRect( -4, -8 );

  CShabDlg* pDlg = GetParent();
  Template* pTmpl = pDlg->GetTmpl( lpDrawItemStruct->itemData );

  CRect prc( pTmpl->m_pFstPoly->CalcBoundBox());

  int M = r.Height(), D = prc.Height();

  int dx = MulDiv( pTmpl->m_Period, M, D );

  if( r.Width() / dx < 2 )
  {
    D = MulDiv( pTmpl->m_Period, 2 * M, r.Width());
    dx = MulDiv( pTmpl->m_Period, M, D );
  }

  int h = MulDiv( prc.Height(), M, D );
  r.InflateRect( 0, ( h - r.Height()) / 2 );

  for( SD_POLY* pPoly = pTmpl->m_pFstPoly ; pPoly ; pPoly = pPoly->pNextPoly )
  {
    int w = MulDiv( pPoly->width, M, D );
    LOGBRUSH lb = { BS_SOLID, RGB( 0, 0, 0 ), 0 };
    CPen pen( PS_GEOMETRIC|PS_ENDCAP_ROUND|PS_JOIN_ROUND, w, &lb );
    pDC->SelectObject( &pen );

    for( SD_POINT* pPoint = pPoly->pFirstPoint ; pPoint && pPoint->pNextPoint ;
         pPoint = pPoint->pNextPoint )
    {
      CPoint p0( pPoint->point - prc.TopLeft());
      CPoint p1( pPoint->pNextPoint->point - prc.TopLeft());
      p0.x = MulDiv( p0.x, M, D ) + r.left;
      p1.x = MulDiv( p1.x, M, D ) + r.left;
      p0.y = MulDiv( p0.y, M, D ) + r.top;
      p1.y = MulDiv( p1.y, M, D ) + r.top;

      for( int lm = r.left ; lm < r.right ; lm += dx, p0.x += dx, p1.x += dx )
      {
        pDC->MoveTo( p0 );
        pDC->LineTo( p1 );
      }
    }
    pDC->SelectObject( pOldPen );
    pDC->SelectObject( pOldBrush );
  }
  pDC->SelectClipRgn( 0 );
}
//=========================================================================
int
CShabDlg::CTmplListBox::CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct )
{
  int d = lpCompareItemStruct->itemData1 - lpCompareItemStruct->itemData2;
  return d < 0 ? -1 : d > 0 ? 1 : 0;
}
//=========================================================================
//=========================================================================
void
CShabDlg::RemoveUndo( UndoItem** ppItem )
{
  for( UndoItem* p ; p = *ppItem ; *ppItem = p->m_pNextItem, delete p );
}
//=========================================================================
void
CShabDlg::AddUndo( UndoItem* pItem )
{
  RemoveUndo( &m_RedoHead );
  pItem->m_pNextItem = m_UndoHead;
  m_UndoHead = pItem;
}
//=========================================================================
void
CShabDlg::Play( UndoItem** ppSrcHead, UndoItem** ppDstHead )
{
  UndoItem* pItem = *ppSrcHead;

  StopMode();

  if( pItem )
  {
    *ppSrcHead = pItem->m_pNextItem;
    pItem->m_pNextItem = *ppDstHead;
    *ppDstHead = pItem;
    pItem->Play( this );
  }
  else
    Beep( 440, 500 );
}
//=========================================================================
//=========================================================================
void
MovePointItem::Play( CShabDlg* pDlg )
{
  pDlg->SelectPoly( m_pCurPoly );
  pDlg->m_pCurPoint = m_pPoint;
  CPoint pnt( m_pPoint->point );
  pDlg->DrawPoly();
  m_pPoint->point = m_Pnt;
  pDlg->DrawPoly();
  m_Pnt = pnt;
}
//=========================================================================
//=========================================================================
InsertPointItem::~InsertPointItem()
{
  if( !m_bInsert )
    delete m_pPoint;
}
//=========================================================================
void
InsertPointItem::Play( CShabDlg* pDlg )
{
  if( m_bInsert ^= 1 )
  {
    *m_ppPrvPoint = m_pPoint;
    m_pCurPoly->length += 1;
  }
  else
  {
    *m_ppPrvPoint = m_pPoint->pNextPoint;
    m_pCurPoly->length -= 1;
  }
  pDlg->SelectPoly( m_pCurPoly );
}
//=========================================================================
//=========================================================================
InsertPolyItem::~InsertPolyItem()
{
  if( !m_bInsert )
    delete m_pPoly;
}
//=========================================================================
void
InsertPolyItem::Play( CShabDlg* pDlg )
{
  if( m_bInsert ^= 1 )
    pDlg->SelectPoly( *m_ppPrvPoly = m_pPoly );
  else
  {
    *m_ppPrvPoly = m_pPoly->pNextPoly;
    pDlg->SelectPoly( 0 );
  }
}
//=========================================================================
//=========================================================================
void
MovePolyItem::Play( CShabDlg* pDlg )
{
  m_Delta = -m_Delta;
  for( SD_POINT* pPoint = m_pCurPoly->pFirstPoint ; pPoint ; pPoint = pPoint->pNextPoint )
    pPoint->point.Offset( m_Delta );
  pDlg->SelectPoly( 0 );
}
//=========================================================================
