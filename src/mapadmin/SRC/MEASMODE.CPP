#include "stdafx.h"

#include "drawobj.h"
#include "miscmode.h"
#include "measmode.h"
#include "splmode.h"
#include "mview.h"
#include "util.h"
#include "mathutil.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "dwin.h"
#include "cspot.h"
#include "ksi.h"
#include "ksiutil.h"
#include "mercator.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
int
CMeasureMode::OnSetCursor()
{
	if( m_nMode == 2 )
		return 0;
  
	SetCursor( App.LoadCursor( IDC_DRAW ));
	return 1;
}
//=====================================================================
void
CMeasureMode::ClearAll()
{ 
  if( m_pPoly && m_nPointNum )
  {
    CalcClipRect( m_CurPoint, m_pPoly[m_nPointNum-1] );
    m_pOwner->ShowBox( m_ClipRect );
    m_CurPoint = m_pPoly[0];
    for( int i = m_nPointNum ; --i >= 0 ; )
    {
      CalcClipRect( m_CurPoint, m_pPoly[i] );
      m_CurPoint = m_pPoly[i];
      m_pOwner->ShowBox( m_ClipRect );
    }
  }
  m_ClipRect.SetRectEmpty();
}
//=====================================================================
int
CMeasureMode::Term( int bDestroy )
{
  if( !bDestroy )
  {
    if( m_bSelect || m_nID == IDM_UNRECTPLAN_MODE )
      SaveRegion();

    ClearAll();
    m_pOwner->ShowSelectedNum();
  }
  return 0;
}
//=====================================================================
int
CMeasureMode::Restore( CMapView* pView )
{
  static UINT mm[] = 
  { 
    IDM_LINE_MEASURE_MODE, IDM_SQUARE_MEASURE_MODE,
    IDM_SELECT_POLY_MODE, IDM_UNRECTPLAN_MODE, 0
  };
  CMeasureMode* pMM = (CMeasureMode*)pView->GetMode( mm );
  if( !pMM )
    return 0;

  if( pMM->m_pPoly && pMM->m_pPoly != pMM->m_Polyline )
  {
    CDrawObject* pObj = pView->GetObjectPtr( pMM->m_ExtId );

		pMM->m_pPoly = pObj->GetPolygon( &pMM->m_nPointNum );
    pMM->m_bDrawFinal = pMM->m_pPoly != 0;
    if( !pMM->m_bSquare && !pMM->m_pPoly )
      pMM->m_pPoly = pObj->GetPolyline( &pMM->m_nPointNum );
  }
  
  pMM->ResetClipRect();
  pMM->CalcSum();
  return 1;
}
//=====================================================================
int
CMeasureMode::OnMouseMove( UINT& flags, CPoint& loc )
{
  if( m_pPoly == m_Polyline && m_nPointNum )
    m_CurPoint = loc;
  return AM_RDONE;
}
//=====================================================================
void
CMeasureMode::CalcClipRect( CPoint p0, CPoint p1 )
{
  m_ClipRect.SetRect( p0, p1 );
  m_ClipRect.NormalizeRect();
  CSize fs( 16, 0 );
  fs = m_pOwner->DP2LP( fs );
  m_ClipRect.InflateRect( fs.cx, fs.cx );
}
//=====================================================================
int
CMeasureMode::OnLButtonDown( UINT& flags, CPoint& loc )
{
  if( m_nMode == 1 )
  {
    ClearAll();
    m_pPoly = m_Polyline;
    m_nPointNum = 0;
    CReisMode::SetActive( 0, 0 );
    m_nMode = 2;
  }
  
  m_bPolylineFound = 0;

  if( flags & MK_SHIFT )
  {
    ClearAll();
    CDrawObject* pObj = m_pOwner->GetDetectedObject();

    m_nMode = 1;
    if( pObj )
    {
      m_ExtId = pObj->Ident();
      m_pPoly = pObj->GetPolygon( &m_nPointNum );
      m_bDrawFinal = m_pPoly != 0;
      if( !m_bSquare && !m_pPoly )
      {
        m_pPoly = pObj->GetPolyline( &m_nPointNum );
        m_bPolylineFound = m_pPoly != 0;
      }
    }
    if( !pObj || !m_pPoly || m_nPointNum > MAX_POINT_NUM )
    {
      m_nPointNum = 0;
      Beep( 440, 400 );
      return AM_RDONE;
    }
  }
  else
  {
    m_ClipRect.SetRect( 0, 0, -1, -1 );
    m_CurPoint = loc;

    if( m_pPoly != m_Polyline )
    {
      m_pPoly = m_Polyline;
      m_nPointNum = 0;
    }
    if( m_nPointNum < MAX_POINT_NUM )
    {
      CReisMode::SetPrevPoint( loc );
      m_Polyline[m_nPointNum++] = loc;
      m_Polyline[m_nPointNum] = m_Polyline[0];
      if( m_nPointNum > 1 && m_Polyline[m_nPointNum-1] == m_Polyline[m_nPointNum-2] )
      {
        Beep( 440, 400 );
        m_Polyline[--m_nPointNum] = m_Polyline[0];
      }
      else
        CalcClipRect( m_Polyline[m_nPointNum-2], m_Polyline[m_nPointNum-1] );
    }
  }

  CalcSum();
  
  return AM_RDONE;
}
//=====================================================================
int
CMeasureMode::OnRButtonDown( UINT& flags, CPoint& loc )
{
  if( m_nMode == 1 )
    return AM_RREMOVE | AM_RDONE;

  OnLButtonDown( flags, loc );
  
  CRect r( m_ClipRect );    
  CalcClipRect( m_Polyline[0], m_Polyline[m_nPointNum-1] );
  m_ClipRect |= r;

  CalcSum();
  m_nMode = 1;
  m_bDrawFinal = m_bSquare;
  return m_nID == IDM_UNRECTPLAN_MODE ? AM_RREMOVE | AM_RDONE : AM_RDONE;
}
//=====================================================================
int
CMeasureMode::OnChar( UINT& flags, UINT& key_code )
{
  if( key_code == VK_ESCAPE )
    return AM_RREMOVE | AM_RDONE;

  if( key_code == VK_BACK )
  {
    CalcClipRect( m_Polyline[m_nPointNum-2], m_Polyline[m_nPointNum-1] );
    m_pOwner->ShowBox( m_ClipRect );
    if( m_pPoly == m_Polyline && m_nPointNum )
    {
      if( --m_nPointNum )
      {
        CReisMode::SetPrevPoint( m_Polyline[m_nPointNum-1] );
        m_Polyline[m_nPointNum] = m_Polyline[0];
      }
      else
        m_nMode= 1;
      CalcSum();
    }
    return AM_RDONE;
  }

  return AM_RPROPAGATE;
}
//=====================================================================
//=====================================================================
void
CMeasureMode::DrawSegments( CDC* pDC, CPoint* lp, int n )
{
  CSize fs( 18, 6 );
  pDC->DPtoLP( &fs );
  int rh = fs.cx + fs.cy;
  int h0 = rh / 2, h1 = h0 - fs.cy;
  pDC->Polyline( lp, n );
  CSize RS( ER, ER );
  pDC->DPtoLP( &RS );
  CFont nFont;
  nFont.CreateFont( fs.cx, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    DRAFT_QUALITY, DEFAULT_PITCH|FF_SCRIPT,
                    "ARIAL" );
  CFont* pOldFont = pDC->SelectObject( &nFont  );
  SetGraphicsMode( pDC->GetSafeHdc(), GM_ADVANCED );
  int oldBkMode = pDC->SetBkMode( TRANSPARENT );

  for( ; --n >= 0 ; ++lp )
  {
    pDC->Ellipse( CRect( lp[0] - RS, RS + RS ));
		if( n )
		{
      CRect r( lp[0], lp[1] );
      CPoint cp( r.CenterPoint());
      int a = atan2( r.Size());
      int ang = a > 900 ? a - 1800 : a < -900 ? a + 1800 : a;
      double co = cos( ang ), si = sin( ang );

			Point p0( lp[0].x, lp[0].y );
			Point p1( lp[1].x, lp[1].y );
      int len = MerkatorData.Dist( p0, p1 ) * SmInUnit + 5;
    
      int lip = len / 100, ldp = len % 100 / 10;
      char b[80];
      int bl = ldp && len < 10000 ? sprintf( b, "%d.%d", lip, ldp ) :
                                    sprintf( b, "%d", lip + ( ldp >= 50 ));
      CSize ext( pDC->GetTextExtent( b, bl ));
      int rw = ext.cx + fs.cy + fs.cy;
    
      if( rw <= dist( r.TopLeft(), r.BottomRight()))    
      {
        XFORM tf = { co, -si, si, co, cp.x, cp.y };
        SetWorldTransform( pDC->GetSafeHdc(), &tf );
        int x1 = -rw /2, y1 = -h0, x2 = x1 + rw, y2 = h0, r = h0 * 2;
        pDC->RoundRect( x1, y1, x2, y2, r, r );      
        pDC->TextOut( 0, fs.cy, b, bl );
        SetIdenticalTransform( pDC );
			}
    }
  }
  pDC->SetBkMode( oldBkMode );
  pDC->SelectObject( pOldFont )->DeleteObject();
}
//=====================================================================
static int
DupNum( CPoint* p, int n )
{
  CPoint* pp = p++;
  for( int i = 0 ; ++i < n && *pp != *p++ ; );
  return i < n ? i : 0;
}
//=====================================================================
void
CMeasureMode::DrawLoop( CDC* pDC, CPoint* pp, int pn )
{
  int d1 = DupNum( pp, ++pn );
  for( CPoint* ps = pp ; --pn > 0 ; )
  {
    int d0 = d1 - 1;
    d1 = DupNum( ++pp, pn );
    if( d1 == d0 - 1 )
    {
      DrawSegments( pDC, ps, pp - ps - 1 + m_bDrawFinal );
      DrawLoop( pDC, pp, d1 );
      pn -= d0;
      ps = pp += d0;
    }
  }
  DrawSegments( pDC, ps, pp - ps + m_bDrawFinal );
}
//=====================================================================
static double
CalcLoopLength( CPoint* pp, int pn )
{
  double S = 0;
  if( !pp )
    return S;

  int d1 = DupNum( pp, ++pn );
  for( CPoint* ps = pp ; --pn > 0 ; )
  {
    int d0 = d1 - 1;
    d1 = DupNum( ++pp, pn );
    if( d1 == d0 - 1 )
    {
      S += CalcLength( ps, pp - ps - 1 ) + CalcLoopLength( pp, d1 );
      pn -= d0;
      ps = pp += d0;
    }
  }
  return S + CalcLength( ps, pp - ps + 1 );
}
//=====================================================================
void
CMeasureMode::PutHint( int bDraw )
{
  if( m_bSelect )
    return;

  double Length1 = 0, Length2 = 0;
	if( bDraw && m_nPointNum )
	{
    Point p0( m_CurPoint.x, m_CurPoint.y );
		Point p1( m_Polyline[m_nPointNum-1].x, m_Polyline[m_nPointNum-1].y );
		Point p2( m_Polyline[0].x, m_Polyline[0].y );
    Length1 = ( .5 + MerkatorData.Dist( p0, p1 ) * SmInUnit ) *1e-2;
    Length2 = ( .5 + MerkatorData.Dist( p0, p2 ) * SmInUnit ) *1e-2;
	}
	
  CString CurHint, tmp;
  if( m_bSquare && m_nPointNum >= 2 )
  {
    int bL = m_Square >= 1e6 ? ( m_Square *= 1e-3, 1 ) : 0;
    int S = m_Square + .5;

    if( bL )
      CurHint.Format( "Площадь = %d.%dкв.км, ", S/1000, S%1000 );
    else
      CurHint.Format( "Площадь = %dкв.м, ", S );
  
		double Length = m_Length + Length1 + Length2;
    if( Length < 100 )
      tmp.Format( "%1.1fм", Length );
    else
    if( Length < 1000 )
      tmp.Format( "%1.0fм", Length );
    else
      tmp.Format( "%1.3fкм", Length/1000 );

		CurHint += "Периметр: ";
  	CurHint += tmp;

	  if( bDraw )
		{
		 	tmp.Format( ", Сегменты: %1.1fм, %1.1fм", Length1, Length2 );
	  	CurHint += tmp;
		}
  }
  else
	{
		double Length = m_Length + Length1 ;
    if( Length < 100 )
      tmp.Format( "%1.1fм", Length );
    else
    if( Length < 1000 )
      tmp.Format( "%1.0fм", Length );
    else
      tmp.Format( "%1.3fкм", Length/1000 );
    CurHint.Format( "%s = %s", m_bSquare ? "Периметр" : "Длина", tmp );
  	if( bDraw )
		{
      tmp.Format( ", Сегмент: %1.1fм", Length1 );
	  	CurHint += tmp;
		}
	}
  ReportHint( CurHint );
}
//=====================================================================
void
CMeasureMode::Draw( CDC* pDC )
{
  static COLORREF clr = RGB( 127, 127, 127 );
  static COLORREF bck = RGB( 255, 255, 255 );

  CSize ps( ER - 1, 0 );
  pDC->DPtoLP( &ps );

  CPen pen( PS_SOLID, ps.cx, clr );
  int oldBkMode  = pDC->SetBkMode( OPAQUE );
  int oldROPMode = pDC->SetROP2( R2_COPYPEN );
  int oldTextAlign = pDC->SetTextAlign( TA_CENTER | TA_BASELINE | TA_NOUPDATECP );
  COLORREF oldTextColor = pDC->SetTextColor( 0 );
  COLORREF oldBkColor = pDC->SetBkColor( bck );
  pDC->SelectStockObject( WHITE_BRUSH );
  CPen* oldPen = pDC->SelectObject( &pen );

  CRect cb;
  pDC->GetClipBox( &cb );
  if( !m_ClipRect.IsRectNull())
    pDC->IntersectClipRect( &m_ClipRect );

  if( m_bSelect )
    pDC->Polyline( m_pPoly, m_nPointNum + m_bDrawFinal );
  else
  if( m_pPoly == m_Polyline )
    DrawSegments( pDC, m_pPoly, m_nPointNum + m_bDrawFinal );
  else
    DrawLoop( pDC, m_pPoly, m_nPointNum );

  pDC->SelectClipRgn( 0 );
  pDC->IntersectClipRect( &cb );

  if( m_pPoly == m_Polyline && m_nPointNum && m_nMode == 2 )
  {
    pDC->SetROP2( R2_NOTXORPEN );
    pDC->MoveTo( m_Polyline[m_nPointNum-1] );
    pDC->LineTo( m_CurPoint );
    if( m_CurPoint != m_Polyline[m_nPointNum-1] )
    {
      int a = atan2( m_CurPoint - m_Polyline[m_nPointNum-1] );
      CSize p( -3 * ER, 0 );
      pDC->DPtoLP( &p );
      pDC->MoveTo( m_CurPoint + TurnPoint( p, a + 200 ));
      pDC->LineTo( m_CurPoint );
      pDC->LineTo( m_CurPoint + TurnPoint( p, a - 200 ));
    }
    if( m_bSquare && m_nPointNum >= 2 )
    {
			pDC->MoveTo( m_Polyline[0] );
      pDC->LineTo( m_CurPoint );
		}
		PutHint( 1 );
  }

  m_bDrawFinal &= m_nMode == 1;
  m_ClipRect.SetRect( 0, 0, -1, -1 );

  pDC->SetBkMode( oldBkMode );
  pDC->SetROP2( oldROPMode );
  pDC->SetTextAlign( oldTextAlign );
  pDC->SetTextColor( oldTextColor );
  pDC->SetBkColor( oldBkColor );
  pDC->SelectObject( oldPen );
}
//=====================================================================
void
CMeasureMode::CalcSum()
{
  if( m_bSelect )
    return;

  CWaitCursor wc;

  m_Length = 
    m_pPoly == m_Polyline ? CalcLength( m_pPoly, m_nPointNum ) :
    m_bPolylineFound      ? CalcLength( m_pPoly, m_nPointNum ) :
                            CalcLoopLength( m_pPoly, m_nPointNum );

  if( m_pPoly == m_Polyline && !m_bSquare && m_nPointNum )
	{
    Point p0( m_CurPoint.x, m_CurPoint.y );
		Point p1( m_Polyline[m_nPointNum-1].x, m_Polyline[m_nPointNum-1].y );
    m_Length += MerkatorData.Dist( p0, p1 );
	}

  m_Length = m_Length * SmInUnit / 100.;

  if( m_bSquare )
    m_Square = CalcSquare();

	PutHint( 0 );
}
//=====================================================================
static double
CalcS( CPoint* poly, int nPointNum )
{
  double S = 0;

  if( nPointNum < 3 )
    return S;

  int i, j, bFound = 0;
  for( j = nPointNum ; --j > 0 && !bFound ; )
    for( i = 0 ; i < j && !bFound ; bFound = poly[i++] == poly[j] );      

  if( bFound )
  {
    int n = j - i;
    S = CalcS( poly + i, n );
    memcpy( poly + i - 1, poly + j + 1, ( nPointNum - j - 1 ) * sizeof( CPoint ));
    return S + CalcS( poly, nPointNum - n - 2 );
  }

  int bCW = Clockwise( poly, nPointNum ) > 0;

  for( int i0 = 0, nPnt = nPointNum ; nPnt >= 3 ; )
  {
    int bFound = 0;
    int i1 = ( i0 + 1 ) % nPnt;
    int i2 = ( i1 + 1 ) % nPnt;

    CPoint p[] = { poly[i0], poly[i1], poly[i2] };

    for( int c = nPnt ; --c >= 0 && !bFound ; )
    {
      double ds = MerkatorData.Sq3( Point( p[0].x, p[0].y ), 
				                            Point( p[1].x, p[1].y ),
																		Point( p[2].x, p[2].y ));
      
      if( !ds || ds > 0 == bCW && 
                 !IsAnyPointInTriangle( poly, nPnt, p ) &&
                 !IsLineCrossPolygon( p[0], p[2], poly, nPnt ))
      {
        S += ds;
        bFound = 1;
      }
      else
      if( bFound )
        break;
      
      if( !bFound )
      {
        i0 = i1;
        p[0] = p[1];
      }
      i1 = i2; 
      p[1] = p[2];
      i2 = ( i2 + 1 ) % nPnt;
      p[2] = poly[i2];
    }

    int in = i0 < i1; 
    int np = ( in ? 0 : nPnt ) + i1 - i0 - 1;
    if( !np )
      break;
    i0 = in ? i0 + 1 : 0;
    memcpy( poly + i0, poly + i1, ( nPnt - i1 ) * sizeof CPoint );
    nPnt -= np;
  }
  return S * SmInUnit * SmInUnit * 1e-4;
}
//=====================================================================
double
CMeasureMode::CalcSquare()
{
  if( m_nPointNum > 3 && 
     ( IsSelfCrossPolygon( m_pPoly, m_nPointNum ) ||
       !Clockwise( m_pPoly, m_nPointNum )))
  {
    Beep( 440, 400 );
    return 0;
  }
  memcpy( m_TmpPoly, m_pPoly, m_nPointNum * sizeof CPoint );
  return fabs( CalcS( m_TmpPoly, m_nPointNum ));
}
//=====================================================================
static CString
m2s( int m )
{
  CString s;
  s.Format( "%1.2f", m / 100.0 );
  return s;
}
//=====================================================================
void
CMeasureMode::SaveRegion()
{
  if( m_nPointNum < 3 ||
      ( !Clockwise( m_pPoly, m_nPointNum ) ||
        IsSelfCrossPolygon( m_pPoly, m_nPointNum )))
  {
    ErrBox( "Некорректный регион!" );
    return;
  }

  if( m_nID == IDM_UNRECTPLAN_MODE )
  {
    CPoint* pPoly = new CPoint[m_nPointNum];
    memcpy( pPoly, m_pPoly, m_nPointNum * sizeof( CPoint ));
    CStreetPlanMode::SavePlan( pPoly, m_nPointNum );
    delete pPoly;
    return;
  }
  
  CString path( App.GetProjectFilePath( "region.txt" ));
  UINT rsm = FileExist( path ) ? AskBox( "Добавить к существующему?", 1 ) : IDNO;
  if( rsm == IDCANCEL )
    return;

  UINT fom = rsm == IDYES ? ios::app : ios::out | ios::trunc;
  ofstream polyFile;
  polyFile.open( path, fom );
  if( !polyFile.good())
  {
    ErrBox( Compose( "Невозможно открыть файл\n", path, 0 ));
    return;
  }

  int bnc = m_pPoly[0] != m_pPoly[m_nPointNum-1];

  polyFile << "Region 1" << endl
           << ( m_nPointNum + bnc ) << endl;

  for( int i = 0 ; i < m_nPointNum ; ++i )
    polyFile << m2s( m_pPoly[i].x )
             << " " 
             << m2s( m_pPoly[i].y )
             << endl;

  if( bnc )
    polyFile << m2s( m_pPoly[0].x )
             << " " 
             << m2s( m_pPoly[0].y )
             << endl;

  polyFile.close();

  InfBox( Compose( "Регион сохранен в файле\n", path, 0 ));
}
//=====================================================================
int
CLinkMeasureMode::OnLButtonDown( UINT &flags, CPoint &loc )
{
  if( m_nMode == 1 )
  {
    ClearAll();
    m_nPointNum = 0;
    CReisMode::SetActive( 0, 0 );
    m_nMode = 2;
  }

  if( flags & MK_SHIFT )
  {
    CDrawObject* pObj = m_pOwner->GetDetectedObject();

    if( !pObj )
      return AM_RDONE;

		int pn;
    CPoint* pPoly = pObj->GetPolyline( &pn );
		int nt = pn + m_nPointNum;
		if( !pObj || !pPoly || nt > MAX_POINT_NUM )
    {
      Beep( 440, 400 );
      return AM_RDONE;
    }

		if( m_nPointNum )
		{
			int d00 = dist( m_Polyline[0], pPoly[0] );
			int d01 = dist( m_Polyline[0], pPoly[pn-1] );
			int d10 = dist( m_Polyline[m_nPointNum-1], pPoly[0] );
			int d11 = dist( m_Polyline[m_nPointNum-1], pPoly[pn-1] );
			int dm = min( min( d00, d01 ), min( d10, d11 ));
			if( dm == d00 )
			{
				ReversePoly( m_Polyline, m_nPointNum );
				memcpy( m_Polyline + m_nPointNum, pPoly, pn * sizeof( CPoint ));
			}
			else
			if( dm == d11 )
			{
			  memcpy( m_Polyline + m_nPointNum, pPoly, pn * sizeof( CPoint ));
				ReversePoly( m_Polyline + m_nPointNum, pn );
			}
			else
			if( dm == d01 )
			{
				ReversePoly( m_Polyline, m_nPointNum );
			  memcpy( m_Polyline + m_nPointNum, pPoly, pn * sizeof( CPoint ));
				ReversePoly( m_Polyline + m_nPointNum, pn );
			}
			else
			  memcpy( m_Polyline + m_nPointNum, pPoly, pn * sizeof( CPoint ));


			CPoint p0 = m_Polyline[m_nPointNum-1]; 
			while( --pn >= 0 )
			{
				CPoint p1 = *pPoly++;
				CalcClipRect( p0, p1 );
				m_pOwner->ShowBox( m_ClipRect );
				p0 = p1;
			}

			m_nPointNum = nt;
		}
		else
		{
			m_nMode = 2;
			m_nPointNum = pn;
			memcpy( m_Polyline, pPoly, pn * sizeof( CPoint ));
		}
		m_CurPoint = m_Polyline[m_nPointNum-1];
  }
  else
  {
    m_ClipRect.SetRect( 0, 0, -1, -1 );
    m_CurPoint = loc;
		
    if( m_nPointNum < MAX_POINT_NUM )
    {
  		m_nMode = 2;
      CReisMode::SetPrevPoint( loc );
      m_Polyline[m_nPointNum++] = loc;
      CalcClipRect( m_Polyline[m_nPointNum-2], m_Polyline[m_nPointNum-1] );
    }
		else
    {
      Beep( 440, 400 );
      return AM_RDONE;
    }
  }

  CalcSum();
  
  return AM_RDONE;
}
//=====================================================================
int
CLinkMeasureMode::OnRButtonDown( UINT &flags, CPoint &loc )
{
  if( m_nMode == 1 )
    return AM_RREMOVE | AM_RDONE;

  m_nMode = 1;
  ClearAll();
  return AM_RDONE;
}
//=====================================================================
int
CLinkMeasureMode::OnMouseMove( UINT &flags, CPoint &loc )
{
  if( m_nPointNum )
    m_CurPoint = loc;
  return AM_RDONE;
}
//=====================================================================
void
CLinkMeasureMode::CalcSum()
{
  m_Length = CalcLength( m_pPoly, m_nPointNum ) * SmInUnit * 1e-2;
	PutHint( 0 );
}
//=====================================================================
//=====================================================================
int
CFindDistanceMode::Open()
{
  m_pOwner->ClearSelection();
  m_nMode = 1;
  return 1;
}
//=====================================================================
int
CFindDistanceMode::OnLButtonDown( UINT &flags, CPoint &loc )
{
  if( !( flags & MK_SHIFT ))
    return AM_RPROPAGATE;

  CDrawObject* pObj;
  ( pObj = m_pOwner->GetDetectedObject( 0 )) ||
  ( pObj = m_pOwner->GetDetectedObject( 1 ));

  if( pObj )
  {
    CObjList tl;
    tl.AddHead( pObj );
    if( pObj->IsSelected())
    {
      m_pOwner->SelectObject( pObj, 0 );
      --m_nMode;
    }
    else
    if( m_nMode < 3 )
    {
      m_pOwner->SelectObject( pObj, 1 );
			if( m_nMode == 1 )
				m_Id0 = pObj->TabIdent();
			else
			if( m_nMode == 2 )
			{
				CDrawObject* pObj0 = m_pOwner->GetObjectPtr( m_Id0 );
				if( !pObj0 )
				{
					ErrBox( "Пропал первый объект!" );
					m_pOwner->SelectObject( pObj, 0 );
					return AM_RDONE;
				}
				LINK_TYPE lt = pObj->IsLinkedTo( pObj0 );
				if( lt == LT_NOTHING || lt == LT_UNDEF )
				{
					ErrBox( "Второй объект не связан с первым!" );
					m_pOwner->SelectObject( pObj, 0 );
					return AM_RDONE;
				}
				m_Id1 = pObj->TabIdent();
			}
      ++m_nMode;
    }
		if( m_nMode == 3 )
		{
			CSetDistDialog dlg( "Расстояние до точки", "fuflo" );
			if( dlg.DoModal() == IDOK )
			{
				++m_nMode;
				m_Dist = dlg.GetDist();
       	CDrawObject* pObj0 = m_pOwner->GetObjectPtr( m_Id0 );
	      if( pObj0 )
				{
#ifdef _TabIdentHashTab_
					TabIdentHashTab IdList;
#else
         	CTabIdList IdList;
#endif // _TabIdentHashTab_
					int r;
					{
						CWaitCursor wc;
            ReportHint( "Выполняется поиск точки. Пожалуйста, подождите..." );
					  FindPoint( pObj0, IdList );
					}
	        if( !m_pSolution )
            ErrBox( "Не найдена точка на заданном удалении!" );
					else
	        if( m_nSolution )
            ErrBox( "Найдена первая точка из нескольких!" );
				}
			}
			else
				return AM_RDONE | AM_RREMOVE;
		}	
		
  }
  return AM_RDONE;
}
//=====================================================================
int
CFindDistanceMode::Term( int bDestroy )
{
	delete m_pSolution;
  m_pOwner->ClearSelection();
  return 0;
}
//=====================================================================
int
CFindDistanceMode::OnRButtonDown( UINT &flags, CPoint &loc )
{
  return AM_RDONE | AM_RREMOVE;
}
//=====================================================================
int
#ifdef _TabIdentHashTab_
CFindDistanceMode::FindPoint( CDrawObject* pCurr, TabIdentHashTab& IdList )
#else
CFindDistanceMode::FindPoint( CDrawObject* pCurr, CTabIdList & IdList )
#endif // _TabIdentHashTab_
{
	int obn = pCurr->Entry();
		
#ifdef _TabIdentHashTab_
	IdList.Add( pCurr->TabIdent());
#else
	IdList.AddHead( pCurr->TabIdent());
#endif // _TabIdentHashTab_

	int nt, pn;
	CPoint* pPnt = pCurr->GetAnyPoints( &nt, &pn );
	if( nt == SH_POINT || nt == SH_POLYLINE && m_nPointNum + pn <= MAX_POINT_NUM )
	{
		if( m_nPointNum )
		{
			int d00 = dist( m_Polyline[0], pPnt[0] );
			int d01 = dist( m_Polyline[0], pPnt[pn-1] );
			int d10 = dist( m_Polyline[m_nPointNum-1], pPnt[0] );
			int d11 = dist( m_Polyline[m_nPointNum-1], pPnt[pn-1] );
			int dm = min( min( d00, d01 ), min( d10, d11 ));
			if( dm == d00 )
			{
				ReversePoly( m_Polyline, m_nPointNum );
				memcpy( m_Polyline + m_nPointNum, pPnt, pn * sizeof( CPoint ));
			}
			else
			if( dm == d11 )
			{
			  memcpy( m_Polyline + m_nPointNum, pPnt, pn * sizeof( CPoint ));
				ReversePoly( m_Polyline + m_nPointNum, pn );
			}
			else
			if( dm == d01 )
			{
				ReversePoly( m_Polyline, m_nPointNum );
				memcpy( m_Polyline + m_nPointNum, pPnt, pn * sizeof( CPoint ));
				ReversePoly( m_Polyline + m_nPointNum, pn );
			}
			else
			  memcpy( m_Polyline + m_nPointNum, pPnt, pn * sizeof( CPoint ));

			m_nPointNum += pn;
		}
		else
		{
			m_nPointNum = pn;
			memcpy( m_Polyline, pPnt, pn * sizeof( CPoint ));
		}	
	}
	int L = CalcLength( m_Polyline, m_nPointNum );
	if( L < m_Dist )
	{
    CObjList lst;
		if( IdList.GetCount() == 1 )
			lst.AddHead( m_pOwner->GetObjectPtr( m_Id1 ));
		else
		{
  	  m_pOwner->GetTargetsGlobal( pCurr, &lst );
	    m_pOwner->GetHostsGlobal( pCurr, &lst );
		}
    while( !lst.IsEmpty())
		{
	    CDrawObject* pNxt = lst.RemoveHead();
	    if( !IdList.Find( pNxt->TabIdent()) && FindPoint( pNxt, IdList ))
				return 1;
		}
	}
	else
	if( !m_pSolution )
	{
		m_SolutionSize = m_nPointNum;
		m_pSolution = new CPoint[m_nPointNum];
		memcpy( m_pSolution, m_Polyline, m_nPointNum * sizeof( CPoint ));
	  while( L > m_Dist )
			L = CalcLength( m_pSolution, --m_SolutionSize );
		CPoint p0( m_pSolution[m_SolutionSize-1] );
		CPoint p1( m_pSolution[m_SolutionSize] );
		m_pSolution[m_SolutionSize++] = StepByLine( p0, p1, m_Dist - L );
		m_nMode = 5;  	  
    m_pOwner->ClearSelection();
	}
	else
		m_nSolution = 1;

#ifdef _TabIdentHashTab_
	IdList.Remove( pCurr->TabIdent());
#else
	IdList.RemoveHead();
#endif // _TabIdentHashTab_
	m_nPointNum -= pn;
	return m_nSolution;
}
//=====================================================================
void
CFindDistanceMode::Draw( CDC* pDC )
{
  static char const* m[] =
  {
    "Выделите первый объект - начало поиска",
    "Выделите рядом второй объект - направление поиска",
    "Введите удаление в метрах"
  };

  if( m_nMode <= 3 )
	{
    ReportHint( m[m_nMode-1] );
		return;
	}

  static COLORREF clr = RGB( 255, 0, 0 );
  static COLORREF bck = RGB( 255, 255, 255 );

  CSize ps( ER - 1, 0 );
  pDC->DPtoLP( &ps );

  CPen pen( PS_SOLID, ps.cx, clr );
  int oldBkMode  = pDC->SetBkMode( OPAQUE );
  int oldROPMode = pDC->SetROP2( R2_COPYPEN );
  int oldTextAlign = pDC->SetTextAlign( TA_CENTER | TA_BASELINE | TA_NOUPDATECP );
  COLORREF oldTextColor = pDC->SetTextColor( 0 );
  COLORREF oldBkColor = pDC->SetBkColor( bck );
  pDC->SelectStockObject( WHITE_BRUSH );
  CPen* oldPen = pDC->SelectObject( &pen );

  CMeasureMode::DrawSegments( pDC, m_pSolution, m_SolutionSize );

  pDC->SetBkMode( oldBkMode );
  pDC->SetROP2( oldROPMode );
  pDC->SetTextAlign( oldTextAlign );
  pDC->SetTextColor( oldTextColor );
  pDC->SetBkColor( oldBkColor );
  pDC->SelectObject( oldPen );
}
//=====================================================================
//=====================================================================
#ifdef _TabIdentHashTab_
enum
{ 
  START_SIZE = 8192,
  LINK_SIZE_LIMIT = 8
};
//=====================================================================
TabIdentHashTab::TabIdentHashTab() :
  m_nSize( START_SIZE ),
  m_nLinks( 0 ),
  m_nUsed( 0 ),
  m_pTab( 0 )
{}
//=====================================================================
TabIdentHashTab::IdentItem**
TabIdentHashTab::FindPos( CTabIdent const& id )
{
  m_pHead = &m_pTab[(( id.Table() << 8 ) + id.Entry()) & ( m_nSize - 1 )];
  pItem* ppos = m_pHead;
  while( *ppos && (*ppos)->m_Id < id )
    ppos = &(*ppos)->m_pNext;
  
  return ppos;
}
//=====================================================================
int
TabIdentHashTab::Remove( CTabIdent const& id )
{
  if( !m_nUsed )
    return 0;

  pItem* ppos = FindPos( id );
  pItem pFnd = *ppos;
  if( !pFnd || pFnd->m_Id != id )
    return 0;
  
  *ppos = pFnd->m_pNext;
  delete pFnd;

  m_nUsed -= 1;
  m_nLinks -= !*m_pHead;

  return 1;
}
//=====================================================================
int
TabIdentHashTab::Add( CTabIdent const& id )
{
  int mls = m_nLinks ? m_nUsed / m_nLinks : LINK_SIZE_LIMIT + 1;
  if( mls > LINK_SIZE_LIMIT )
  {
    int oldSize = m_nSize;
    m_nSize <<= 1;
    pItem* pOld = m_pTab;
    m_pTab = new pItem[m_nSize];
    if( !m_pTab )
      return 0;
  
    memset( m_pTab, 0, m_nSize * sizeof pItem );
    if( pOld )
    {
      m_nLinks = 0;
      for( pItem* cp = pOld + oldSize ; --cp >= pOld ; )
        for( pItem pos = *cp ; pos ; )
        {
          pItem nxt = pos->m_pNext;
          pItem* ppos = FindPos( pos->m_Id );
          m_nLinks += !*m_pHead;
          pos->m_pNext = *ppos;
          *ppos = pos;
          pos = nxt;
        }
      delete pOld;
    }
  }

  pItem* ppos = FindPos( id );
  int il = !*m_pHead;

  if( *ppos && (*ppos)->m_Id == id )
    ;
  else
  {
    pItem pNew = new IdentItem( id, *ppos );
    if( !pNew )
      return 0;

    *ppos = pNew;
    ++m_nUsed;
    m_nLinks += il;
  }

  return 1;
}
//=====================================================================
int
TabIdentHashTab::Find( CTabIdent const& id )
{
  if( !m_nUsed )
    return 0;

  pItem pos = *FindPos( id );
  return pos && pos->m_Id == id;
}
//=====================================================================
void
TabIdentHashTab::Clear()
{
  if( m_pTab )
  { 
    for( pItem* cp = m_pTab + m_nSize ; --cp >= m_pTab ; )
      while( *cp )
      {
        pItem pTmp = *cp;
        *cp = pTmp->m_pNext;
        delete pTmp;
      }
    delete m_pTab;
    m_pTab = 0;
    m_nUsed = 0;
    m_nLinks = 0;
    m_nSize = START_SIZE;
  }
}
//=====================================================================
#endif // _TabIdentHashTab_
