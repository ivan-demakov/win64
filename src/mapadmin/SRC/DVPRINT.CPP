#include "stdafx.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "mdoc.h"
#include "drawprim.h"
#include "mview.h"
#include "drawobj.h"
#include "graphdoc.h"
#include "prndlg.h"
#include "pfmode.h"
#include "scale.h"
#include "util.h"
#include "mathutil.h"
#include "dwin.h"
#include "miscmode.h"
#include "mercator.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
enum { MS_GLUE, MS_OUT, MS_FRAME, MS_LINE };
//=====================================================================
int
CMapView::OnPreparePrinting( CPrintInfo* pInfo )
{
  pInfo->SetMinPage( 1 );
  pInfo->SetMaxPage( m_PrintPageNum );
  return DoPreparePrinting( pInfo );
}
//=====================================================================
void
CMapView::PrintMap( const CRect& fragment, CPoint org, int bPreview )
{
  m_bPreview = bPreview;
  m_PrintArea.CopyRect( fragment );
  m_PrintOrg = org;
  if( bPreview )
    OnFilePrintPreview();
  else
    OnFilePrint();
}
//=====================================================================
static int
CalcNum( int sz, int ss, int fs, int ns, int ls )
{
  if( sz <= ss )
    return 1;

  sz -= fs + ls;
  return 2 + sz / ns + ( sz % ns > 0 );
}
//=====================================================================
void
CMapView::CalcPrintPageNum()
{
  m_PrintPageNumX = CalcNum( m_PrintArea.Width(),  m_PrintPageSngl.cx,
                             m_PrintPageFrst.cx,   m_PrintPageNext.cx,
                             m_PrintPageLast.cx );
  m_PrintPageNumY = CalcNum( m_PrintArea.Height(), m_PrintPageSngl.cy,
                             m_PrintPageFrst.cy,   m_PrintPageNext.cy,
                             m_PrintPageLast.cy );
  m_PrintPageNum = m_PrintPageNumX * m_PrintPageNumY;
}
//=====================================================================
void
CMapView::OnBeginPrinting( CDC* pDC, CPrintInfo* pInfo )
{
  if( SublDlgIsActive())
    m_pSublDlg->ShowWindow( SW_HIDE );

  GetDocument()->OnBeginPrinting( CDocSetupPage::m_colorEnb );
  int pm = GetPrnScale() * m_ScaleFactor;
  int pd = 10 * SmInUnit;

  m_PrinterPageSize.cx = pDC->GetDeviceCaps( HORZSIZE );
  m_PrinterPageSize.cy = pDC->GetDeviceCaps( VERTSIZE );
  m_PrintPageSize.cx = MulDiv( m_PrinterPageSize.cx, pm, pd );
  m_PrintPageSize.cy = MulDiv( m_PrinterPageSize.cy, pm, pd );

  int alb = CPageSetupPage::m_layopt == LAYOUT_ALBUM;
  int ovl = alb || !CPageSetupPage::m_overlapEnb? 0 : MulDiv( CPageSetupPage::m_overlapVal, pm, pd );
  int mkf = MulDiv( 5, pm, pd );
  CSize mks( mkf, mkf );
  CSize ovs( ovl, ovl );
  CSize org;

  if( alb )
  {
    CRect rBrd( max( MulDiv( CPageSetupPage::m_spc.left,   pm, pd ), mkf * 4 ),
                max( MulDiv( CPageSetupPage::m_spc.top,    pm, pd ), mkf * 2 ),
                max( MulDiv( CPageSetupPage::m_spc.right,  pm, pd ), mkf * 1 ),
                max( MulDiv( CPageSetupPage::m_spc.bottom, pm, pd ), mkf * ( CPageSetupPage::m_numerationEnb ? 3 : 1 )));
                org = rBrd.TopLeft();
    m_PrintPageSngl = m_PrintPageFrst = m_PrintPageLast =
    m_PrintPageNext = m_PrintPageSize - org - rBrd.BottomRight();
  }
  else
  {
    int spc = CPageSetupPage::m_spaceEnb * MulDiv( CPageSetupPage::m_spaceVal, pm, pd );
    int lnv = CPageSetupPage::m_lineEnb * MulDiv( CPageSetupPage::m_lineVal, pm, pd * 10 );
    CSize lin( lnv, lnv + mkf );
    org.cx = org.cy = spc + lnv;
    m_PrintPageFrst = m_PrintPageSize - org;
    m_PrintPageSngl = m_PrintPageFrst - lin;
    m_PrintPageNext = m_PrintPageSize - mks - ovs;
    m_PrintPageLast = m_PrintPageNext - lin;
  }

  CalcPrintPageNum();
  PrintPageInfo* ppp = m_pPrintPages = new PrintPageInfo[m_PrintPageNum];

  int lt  = alb || !CPageSetupPage::m_lineEnb ? MS_LINE : MS_FRAME;

  for( int top = m_PrintArea.top, row = 1 ; row <= m_PrintPageNumY ; ++row )
  {
    int fr = row == 1, lr = row == m_PrintPageNumY, sr = fr && lr;
    int rs = sr ? m_PrintPageSngl.cy:
             fr ? m_PrintPageFrst.cy:
             lr ? m_PrintPageLast.cy + ovl:
                  m_PrintPageNext.cy + ovl;
    int bottom = min( m_PrintArea.bottom, top + rs );
    for( int left = m_PrintArea.left, col = 1 ; col <= m_PrintPageNumX ; ++col, ++ppp )
    {
      int fc = col == 1, lc = col == m_PrintPageNumX, sc = fc && lc;
      int cs = sc ? m_PrintPageSngl.cx:
               fc ? m_PrintPageFrst.cx:
               lc ? m_PrintPageLast.cx + ovl:
                    m_PrintPageNext.cx + ovl;
      int right = min( m_PrintArea.right, left + cs );
      ppp->area.SetRect( left, top, right, bottom );
      ppp->portOrg.x = alb || fc ? org.cx : mkf;
      ppp->portOrg.y = alb || fr ? org.cy : mkf;
      ppp->crmt[0] = alb || fc ? lt : MS_GLUE;
      ppp->crmt[1] = alb || fr ? lt : MS_GLUE;
      ppp->crmt[2] = alb || lc ? lt : MS_OUT;
      ppp->crmt[3] = alb || lr ? lt : MS_OUT;
      ppp->row = row;
      ppp->col = col;
      left = right - ovl;
    }
    top = bottom - ovl;
  }
}
//=====================================================================
void
CMapView::OnEndPrinting( CDC*, CPrintInfo* )
{
  if( SublDlgIsActive())
    m_pSublDlg->ShowWindow( SW_SHOW );

  GetDocument()->OnEndPrinting();
  ShowSelectedNum();
  delete[] m_pPrintPages;
}
//=====================================================================
inline void
DrawLine( CDC* pDC, int x0, int y0, int x1, int y1 )
{
  pDC->MoveTo( x0, y0 );
  pDC->LineTo( x1, y1 );
}
//=====================================================================
void
CMapView::PrintBorder( int side, PrintPageInfo* ppi, CRect const& r, CDC* pDC )
{
  int type = ppi->crmt[side];

  if( type == MS_FRAME )
    return;

  CRect tr( r );
  if( type == MS_OUT )
  {
    CPoint prs( pDC->GetDeviceCaps( HORZRES ) - 6, pDC->GetDeviceCaps( VERTRES ) - 6 );
    pDC->DPtoLP( &prs );
    tr.right  = min( prs.x, tr.right );
    tr.bottom = min( prs.y, tr.bottom );
  }
  int x0 = side == 2 ? tr.right : tr.left;
  int x1 = side == 0 ? tr.left : tr.right;
  int y0 = side == 3 ? tr.bottom : tr.top;
  int y1 = side == 1 ? tr.top : tr.bottom;

  CPen pen( type == MS_OUT || type == MS_GLUE ? PS_DOT : PS_SOLID, 0, RGB( 0, 0, 0 ));
  CPen* pOldPen = pDC->SelectObject( &pen );

  DrawLine( pDC, x0, y0, x1, y1 );

  int pm = GetPrnScale() * m_ScaleFactor;
  int pd = 10 * SmInUnit;

  if( type == MS_GLUE )
  {
    int mkf = MulDiv( 5, pm, pd );
    int rx = mkf / 2, hy = mkf / 2;
    CPoint rs( rx, rx );
    CFont nFont;
    CFont* pOldFont;
    pDC->SetTextAlign( TA_CENTER | TA_TOP );
    char sn[32] = "";
    int tl = sprintf_s( sn, "Ряд %d Лист %d", ppi->row, ppi->col );
    int dx = x0 == x1 ? -mkf : 0;
    int dy = y0 == y1 ? -mkf : 0;
    CRect r( x0, y0, x1 + dx, y1 + dy );
    r.NormalizeRect();
    CPoint c( r.CenterPoint());
    dy = r.Height();
    dx = r.Width();
    int fs = MulDiv( 4, pm, pd );
    if( dx > dy )
    {
      for( int n = dx / dy, cx0 = x0, i = 1 ; i <= n ; i++ )
      {
        int cx1 = r.left + dx * i / n;
        DrawLine( pDC, cx0, r.top, cx1, r.bottom );
        DrawLine( pDC, cx0, r.bottom, cx1, r.top );
        cx0 = cx1;
      }
      nFont.CreateFont( fs, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET,
                        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        DRAFT_QUALITY, DEFAULT_PITCH|FF_SCRIPT, "ARIAL" );
      pOldFont = pDC->SelectObject( &nFont  );
      CSize te( pDC->GetTextExtent( sn, tl ));
      int hx = te.cx / 2 + rx;
      CRect br( c.x - hx, c.y - hy, c.x + hx, c.y + hy );
      if( br.left > r.left )
      {
        pDC->RoundRect( br, rs );
        pDC->TextOut( c.x, br.top + ( mkf - te.cy ) / 2 , sn, tl );
      }
    }
    else
    {
      for( int n = dy / dx, cy0 = y0, i = 1 ; i <= n ; i++ )
      {
        int cy1 = y0 + dy * i / n;
        DrawLine( pDC, r.left, cy0, r.right, cy1 );
        DrawLine( pDC, r.left, cy1, r.right, cy0 );
        cy0 = cy1;
      }
      nFont.CreateFont( fs, 0, 900, 900, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET,
                        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        DRAFT_QUALITY, DEFAULT_PITCH|FF_SCRIPT, "ARIAL" );
      pOldFont = pDC->SelectObject( &nFont  );
      CSize te( pDC->GetTextExtent( sn, tl ));
      int hx = te.cx / 2 + rx;
      CRect br( c.x - hy, c.y - hx, c.x + hy, c.y + hx );
      if( br.top > r.top )
      {
        pDC->RoundRect( br, rs );
        pDC->TextOut( br.left + ( mkf - te.cy ) / 2, c.y, sn, tl );
      }
    }
    pDC->SelectObject( pOldFont )->DeleteObject();
  }
  pDC->SelectObject( pOldPen )->DeleteObject();
}
//=====================================================================
void
CMapView::OnPrint( CDC* pDC, CPrintInfo* pInfo )
{
  CWaitCursor wait;

  int a = GetModeAngle();
  int pageNum = pInfo->m_nCurPage - 1;
  PrintPageInfo* ppi = m_pPrintPages + pageNum;

  int sc = GetPrnScale();
  int pm = sc * m_ScaleFactor;
  int pd = 10 * SmInUnit;

  int bAlb = CPageSetupPage::m_layopt == LAYOUT_ALBUM;
  int lnv = MulDiv( CPageSetupPage::m_lineVal, pm, pd * 10 );
  CRect r( ppi->area ), r0( 0, 0, 0, 0 );
  CPoint vpo( ppi->portOrg );

  int OldMode   = pDC->SetROP2( R2_COPYPEN );
  int OldBkMode = pDC->SetBkMode( TRANSPARENT );

  pDC->LPtoDP( &vpo );

  CPoint oldWindowOrg   = pDC->SetWindowOrg( r.left, r.top );
  CPoint oldViewportOrg = pDC->SetViewportOrg( vpo );

  CGraphData data;
  GetDocument()->CreateLayers( &data );

  for( int sl = 2 ; --sl >= 0 ; )
  {
    CLayer* psl = m_ClipData.GetFirstLayer( sl );
    CLayer* pdl = data.GetFirstLayer( sl );
    while(( psl = m_ClipData.GetNextLayer()) && ( pdl = data.GetNextLayer()))
      pdl->SetVisible( psl->LayerOn());
  }

  CBox cb( r, m_PrintOrg, a );

  if( CDocSetupPage::m_selectOnlyEnb )
    for( POSITION pos = m_SelectList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = new CDrawObject( m_SelectList.GetNext( pos ));
      ScaleObject( pObj, sc );
      if( !CDocSetupPage::m_selectionEnb )
        pObj->ResetSelectFlag();
      data.AddObject( pObj );
    }
  else
	{
    LoadFromStore( &data, cb, r0, sc, r0, sc );
		AddLoadStaticSublayer( &data, sc, cb ); 
	}

  CObjList localSelect;

  if( CDocSetupPage::m_selectionEnb )
    for( POSITION pos = pView->m_SelectList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pSel = pView->m_SelectList.GetNext( pos );
      CDrawObject* pObj = data.GetObjectPtr( pSel->TabIdent());
      if( pObj )
      {
        pObj->SetSelectFlag( pSel->SelectFlag());
        data.RemoveObject( pObj );
			}
	    else
			{
        pObj = new CDrawObject( pSel );
        this->ScaleObject( pObj, sc );
				data.AddObject( pObj );
			}
      pObj->SetSpecSelection( pSel );
      localSelect.AddTail( pObj );
    }

  pDC->IntersectClipRect( r );
//???
  int ss = m_AltScale;
  m_AltScale = GetPrnScale();
  CDrawContainer::SetHR( IsHiResolution());
  m_AltScale = ss;
//???
  CCoordGridMode* pCGM = (CCoordGridMode*)GetMode( IDM_COORDGRID_MODE );
  CGridMode*      pGM  = (CGridMode*)GetMode( IDM_GRID_MODE );

  if( pGM )
    pGM->SetPrintRect( ppi->area );

  if( pCGM )
    pCGM->SetPrintRect( bAlb ? ppi->area : m_PrintArea );

  using namespace Gdiplus;
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR GdiplusToken;
  GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );

  if( a )
  {
    data.Draw( pDC, SUBLAYER, m_PrintOrg, -a );
    data.Draw( pDC, OBJLAYER, m_PrintOrg, -a );

    float cs = cos( a ), sn = sin( a );
    float dx = m_PrintOrg.x * ( 1.0 - cs ) + m_PrintOrg.y * sn;
    float dy = m_PrintOrg.y * ( 1.0 - cs ) - m_PrintOrg.x * sn;

    XFORM tf = { cs, sn, -sn, cs, dx, dy };
    for( POSITION pos = localSelect.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = localSelect.GetNext( pos );
      pObj->SpinOn( m_PrintOrg, -a );
      pObj->Draw( pDC );
      pObj->SpinOn( m_PrintOrg, a );
    }
    SetWorldTransform( pDC->GetSafeHdc(), &tf );
    DrawFuncLinks( pDC );
    SetWorldTransform( pDC->GetSafeHdc(), &tf );
    if( pGM )
      pGM->Draw( pDC );
    if( pCGM )
      pCGM->Draw( pDC );
    SetIdenticalTransform( pDC );
  }
  else
  {
    data.Draw( pDC, SUBLAYER|OBJLAYER );
    for( POSITION pos = localSelect.GetHeadPosition() ; pos ; )
         localSelect.GetNext( pos )->Draw( pDC );
    DrawFuncLinks( pDC );
    if( pGM )
      pGM->Draw( pDC );
    if( pCGM )
      pCGM->Draw( pDC );
  }

  GdiplusShutdown( GdiplusToken );

  CRect pr( CPoint( ppi->area.TopLeft() - ppi->portOrg ), m_PrintPageSize );
  pDC->SelectClipRgn( 0 );
  pDC->IntersectClipRect( pr );

  COLORREF OldTextColor = pDC->SetTextColor( 0 );

  CRect d1( 0, 0, 1, 1 );
  pDC->DPtoLP( &d1 );
  r.BottomRight() -= d1.Size();
  int fr = 0;
  for( int i = 4 ; --i >= 0 ; PrintBorder( i, ppi, r, pDC ), fr |= ppi->crmt[i] == MS_FRAME );

  if( fr )
  {
    CPen pen( PS_SOLID, lnv, RGB( 0, 0, 0 ));
    CPen* pOldPen = pDC->SelectObject( &pen );
    lnv /= 2;
    CRect r( m_PrintArea );
    r.InflateRect( lnv, lnv );
    pDC->SelectStockObject( NULL_BRUSH );
    pDC->Rectangle( &r );
    pDC->SelectObject( pOldPen );
  }

  pDC->SetTextAlign( TA_CENTER | TA_TOP | TA_NOUPDATECP  );
  CFont nFont;
  nFont.CreateFont( MulDiv( 4, pm, pd ),
                    0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    DRAFT_QUALITY, DEFAULT_PITCH|FF_SCRIPT,
                    "ARIAL" );
  CFont* pOldFont = pDC->SelectObject( &nFont  );

  pDC->SelectStockObject( BLACK_PEN );
  pDC->SelectStockObject( WHITE_BRUSH );

  int dh = MulDiv( 2, pm, pd );
  int dw = dh * 2;

  if( bAlb || ppi->col == 1 && ppi->row == m_PrintPageNumY )
  {
    int lw = ppi->crmt[3] == MS_FRAME ? lnv : 0;
    char ts[32];
    int tl = sprintf_s( ts, "Масштаб 1:%d", sc );
    pDC->SetTextAlign(( bAlb ? TA_RIGHT : TA_LEFT ) | TA_TOP );
    pDC->TextOut( bAlb ? ppi->area.right - dh / 2 : ppi->area.left,
                              ppi->area.bottom + 8 + lw, ts, tl );
  }

  if( bAlb )
  {
    int cntr = ppi->area.right + ppi->area.left >> 1;

    if( CPageSetupPage::m_subscriptEnb )
    {
      pDC->SetTextAlign( TA_CENTER | TA_TOP );
      pDC->TextOut( cntr, ppi->area.bottom + dh,
                    CPageSetupPage::m_subscriptText,
                    CPageSetupPage::m_subscriptText.GetLength());
    }

    if( CPageSetupPage::m_numerationEnb )
    {
      char nm[32];
      int tl = sprintf_s( nm, "%d", CPageSetupPage::m_startNumber + pageNum );
      pDC->SetTextAlign( TA_CENTER | TA_BOTTOM );
      pDC->TextOut( cntr, ppi->area.top - dh, nm, tl );

      if( m_PrintPageNum > 1 )
      {
        int dn[] = { -1, -m_PrintPageNumX, 1, m_PrintPageNumX, 0 };
        int b[] = { pageNum % m_PrintPageNumX,
                    pageNum >= m_PrintPageNumX,
                    ( pageNum + 1 ) % m_PrintPageNumX,
                    pageNum < m_PrintPageNum - m_PrintPageNumX, 1 };
        CRect ro( dw*2, dh*2, dw*4, dh*4 ),
              rl( 0, ro.top, ro.left, ro.bottom ),
              rt( ro.left, 0, ro.right, ro.top ),
              rr( ro.right, ro.top, ro.left+ro.right, ro.bottom ),
              rb( ro.left, ro.bottom, ro.right, ro.top+ro.bottom);

        CRect* pr[] = { &rl, &rt, &rr, &rb, &ro };

        for( int i = CELEM( pr ) ; --i >= 0 ; )
        {
          pr[i]->OffsetRect( ppi->area.left, ppi->area.bottom + dh );
          CPoint c( pr[i]->CenterPoint());
          if( b[i])
          {
            char nm[32];
            tl = sprintf_s( nm, "%d", CPageSetupPage::m_startNumber + pageNum + dn[i] );
            pDC->Rectangle( pr[i] );
            pDC->TextOut( c.x, c.y + dh, nm, tl );
          }
        }
      }
    }
  }

  pDC->SelectObject( pOldFont )->DeleteObject();
  pDC->SetROP2( OldMode );
  pDC->SetBkMode( OldBkMode );
  pDC->SetTextColor( OldTextColor );
  pDC->SetWindowOrg( oldWindowOrg );

  if( pInfo->m_bPreview )
  {
    char s[128];
    int n = sprintf_s( s, "Лист %d из %d",
                       ( ppi->row - 1 ) * m_PrintPageNumX + ppi->col, m_PrintPageNum );
    if( a )
    {
      int ma = abs( a );
      sprintf_s( s + n, sizeof s - n, ". Поворот на %s%d.%1d град.", a >= 0 ? "" : "-", ma/10, ma%10 );
    }
    ReportHint( s );
  }
}
//=====================================================================
// Printing Dialog
class CPrintingDialog : public CDialog
{
public:
  static int m_bUserAbort;
  //{{AFX_DATA(CPrintingDialog)
  enum { IDD = AFX_IDD_PRINTDLG };
  //}}AFX_DATA
  CPrintingDialog::CPrintingDialog( CWnd* pParent )
  {
    Create( CPrintingDialog::IDD, pParent );      // modeless !
    m_bUserAbort = 0;
  }
  virtual ~CPrintingDialog() {}
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
};

int CPrintingDialog::m_bUserAbort;

BOOL CALLBACK
AbortProc( HDC, int )
{
  MSG msg;
  while( !CPrintingDialog::m_bUserAbort && ::PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE ))
    if( !AfxGetThread()->PumpMessage())
      return 0;   // terminate if WM_QUIT received
  return !CPrintingDialog::m_bUserAbort;
}

BOOL
CPrintingDialog::OnInitDialog()
{
  SetWindowText( AfxGetAppName());
  CenterWindow();
  return CDialog::OnInitDialog();
}

void
CPrintingDialog::OnCancel()
{
  m_bUserAbort = 1;
  CDialog::OnCancel();
}
//=====================================================================
extern UINT AFXAPI AfxGetFileTitle( LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax );
//=====================================================================
void
CMapView::OnFilePrint()
{
  // Allocate the PRINTDLGEX structure.
  LPPRINTDLGEX pPDX = (LPPRINTDLGEX)GlobalAlloc( GPTR, sizeof( PRINTDLGEX ));
  if( !pPDX )
    return;

  // Allocate an array of PRINTPAGERANGE structures.
  LPPRINTPAGERANGE pPageRanges = (LPPRINTPAGERANGE) GlobalAlloc( GPTR, 10 * sizeof( PRINTPAGERANGE ));
  if( !pPageRanges )
    return;

  pPageRanges[0].nFromPage = 1;
  pPageRanges[0].nToPage = m_PrintPageNum;

  //  Initialize the PRINTDLGEX structure.

  pPDX->hDevMode  = App.GetDevMode();
  pPDX->hDevNames = App.GetDevNames();
  pPDX->lStructSize = sizeof(PRINTDLGEX);
  pPDX->hwndOwner = m_hWnd;
  pPDX->Flags = PD_RETURNDC | PD_ALLPAGES | PD_NOCURRENTPAGE | PD_NOSELECTION;
  pPDX->nPageRanges = 1;
  pPDX->nMaxPageRanges = 10;
  pPDX->lpPageRanges = pPageRanges;
  pPDX->nMinPage = 1;
  pPDX->nMaxPage = m_PrintPageNum;
  pPDX->nCopies = 1;
  pPDX->nStartPage = START_PAGE_GENERAL;

  //  Invoke the Print property sheet.
  HRESULT hResult = PrintDlgEx( pPDX );

  if( hResult == S_OK && pPDX->dwResultAction == PD_RESULT_PRINT )
  {
    // setup the printing DC
    CDC dcPrint;
    dcPrint.Attach( pPDX->hDC );  // attach printer dc
    dcPrint.m_bPrinting = 1;

    CSize ps( dcPrint.GetDeviceCaps( HORZSIZE ),
              dcPrint.GetDeviceCaps( VERTSIZE ));
    if( m_PrinterPageSize != ps )
    {
      InfBox( "Размер бумаги был изменён.\n"
              "Пожалуйста, повторите выбор области печати." );
      AddMode( new CPrintMapMode( m_bPreview ));
    }
    else
    {
      int bError = 0;
      int szMap = m_PrintPageNum + 1;
      bool* pPageMap = new bool[szMap];
      memset( pPageMap, pPDX->nPageRanges ? 0 : 1, szMap );
      for( int i = pPDX->nPageRanges ; --i >= 0 ; )
      {
        int cp = pPageRanges[i].nFromPage;
        int lp = pPageRanges[i].nToPage;
        if( cp > lp )
          cp ^= lp ^= cp ^= lp;
        for( ; cp <= lp ; pPageMap[cp++] = 1 );
      }

      CPrintInfo printInfo;
      printInfo.m_pPD->m_pd.hDevMode = pPDX->hDevMode;
      printInfo.m_pPD->m_pd.hDevNames = pPDX->hDevNames;

      // gather file to print to if print-to-file selected
      CString strOutput;
      int fdRes = IDOK;
      if( pPDX->Flags & PD_PRINTTOFILE )
      {
        // construct CFileDialog for browsing
        CString strDef( MAKEINTRESOURCE( AFX_IDS_PRINTDEFAULTEXT ));
        CString strPrintDef( MAKEINTRESOURCE( AFX_IDS_PRINTDEFAULT ));
        CString strFilter( MAKEINTRESOURCE( AFX_IDS_PRINTFILTER ));
        CFileDialog* fdlg =
          new CFileDialog( 0, strDef, strPrintDef, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter, this );
                           fdlg->m_ofn.lpstrTitle = "Печать в файл";
                           fdRes = DoModalDlg( fdlg, CE_PAR_DLG );
        if( fdRes == IDOK )
          strOutput = fdlg->GetPathName(); // set output device to resulting path name
        delete fdlg;
      }

      if( fdRes == IDOK )
      {
        // set up document info and start the document printing process
        CString strTitle( GetDocument()->GetTitle());
        if( strTitle.GetLength() > 31 )
          strTitle.ReleaseBuffer( 31 );

        DOCINFO docInfo;
        memset( &docInfo, 0, sizeof docInfo );
        docInfo.cbSize = sizeof docInfo;
        docInfo.lpszDocName = strTitle;
        CString strPortName;
        int nFormatID;
        if( strOutput.IsEmpty())
        {
          docInfo.lpszOutput = 0;
          strPortName = printInfo.m_pPD->GetPortName();
          nFormatID = AFX_IDS_PRINTONPORT;
        }
        else
        {
          docInfo.lpszOutput = strOutput;
          AfxGetFileTitle( strOutput,   strPortName.GetBuffer(_MAX_PATH), _MAX_PATH );
          nFormatID = AFX_IDS_PRINTTOFILE;
        }

        OnBeginPrinting( &dcPrint, 0 );

        dcPrint.SetAbortProc( AbortProc );

        // disable main window while printing & init printing status dialog
        AfxGetMainWnd()->EnableWindow( 0 );
        CPrintingDialog dlgPrintStatus( this );

        CString strTemp;
        dlgPrintStatus.SetDlgItemText( AFX_IDC_PRINT_DOCNAME, strTitle );
        dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME, printInfo.m_pPD->GetDeviceName());
        strTemp.Format( "в %s", strPortName );
        dlgPrintStatus.SetDlgItemText( AFX_IDC_PRINT_PORTNAME, strTemp );
        dlgPrintStatus.ShowWindow( SW_SHOW );
        dlgPrintStatus.UpdateWindow();

        // start document printing process
        if( dcPrint.StartDoc( &docInfo ) == SP_ERROR )
        {
          AfxMessageBox( AFX_IDP_FAILED_TO_START_PRINT );
          bError = 1;
        }

        VERIFY( strTemp.LoadString( AFX_IDS_PRINTPAGENUM ));

        // begin page printing loop

        int nCop = pPDX->nCopies;
        int bClt = pPDX->Flags & PD_COLLATE;

        for( int nc = bClt ? nCop : 1 ; -- nc >= 0 && !bError ; )
          for( printInfo.m_nCurPage = 1 ; printInfo.m_nCurPage <= m_PrintPageNum && !bError ; ++printInfo.m_nCurPage )
            if( pPageMap[printInfo.m_nCurPage] )
              for( int nc = bClt ? 1 : nCop ; -- nc >= 0 && !bError ; )
              {
                OnPrepareDC( &dcPrint, &printInfo );

                // write current page
                char szBuf[80];
                sprintf_s( szBuf, strTemp, printInfo.m_nCurPage );
                dlgPrintStatus.SetDlgItemText( AFX_IDC_PRINT_PAGENUM, szBuf );

                // attempt to start the current page
                if( dcPrint.StartPage() < 0 )
                  bError = 1;
                else
                {
                  OnPrepareDC( &dcPrint, &printInfo );
                  // page successfully started, so now render the page
                  OnPrint( &dcPrint, &printInfo );
                  bError = dcPrint.EndPage() < 0 || !AbortProc( dcPrint.m_hDC, 0 );
                }
              }
          if( !bError )
            dcPrint.EndDoc();
          else
            dcPrint.AbortDoc();

        // cleanup document printing process
        AfxGetMainWnd()->EnableWindow( 1 );

        OnEndPrinting( &dcPrint, 0 );
        dlgPrintStatus.DestroyWindow();
      }
      delete pPageMap;
    }
    dcPrint.Detach();
  }

  if( pPDX->hDC )
    DeleteDC( pPDX->hDC );
}
//=====================================================================
