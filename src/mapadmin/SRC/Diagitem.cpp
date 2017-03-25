#include "stdafx.h"

#include "util.h"
#include "cbox.h"
#include "cspot.h"
#include "mview.h"
#include "diagitem.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CLegend::CLegend( COLORREF Color, int bRound, int Width, float PixelValue,
                  int nLegend, LegendItem* pLegend ) :
  m_FillColor( Color ),
  m_bRound( bRound ),
  m_Width( Width > DIAG_MAX_W ? DIAG_MAX_W : Width < DIAG_MIN_W ? DIAG_MIN_W : Width ),
  m_PixelValue( PixelValue > 0 ? PixelValue : 1 )
{
	if( pLegend )
    for( ; --nLegend >= 0 ; ++pLegend )
      m_Legend.Add( LegendItem( pLegend->m_FillColor, pLegend->m_Prefix, pLegend->m_Postfix ));
}
//=====================================================================
CLegend::CLegend( CLegend* pLegend ) :
  m_FillColor( 0 )
{
  if( pLegend )
	{
    m_FillColor  = pLegend->m_FillColor;
    m_bRound     = pLegend->m_bRound;
    m_Width      = pLegend->m_Width;
    m_PixelValue = pLegend->m_PixelValue;
    int n = pLegend->m_Legend.GetSize();
    for( m_Legend.SetSize( n ) ; --n >= 0 ; m_Legend[n] = pLegend->m_Legend[n] );
	}
}
//=====================================================================
int
CLegend::operator==( CLegend const& rLegend ) const
{
  if( m_FillColor != rLegend.m_FillColor  ||
     m_bRound     != rLegend.m_bRound     ||
     m_Width      != rLegend.m_Width      ||
     m_PixelValue != rLegend.m_PixelValue ||
     m_Legend.GetSize() != rLegend.m_Legend.GetSize())
    return 0;

  int i = m_Legend.GetSize();
  while( --i >= 0 && m_Legend[i] == rLegend.m_Legend[i] );
  return i < 0;
}
//=====================================================================
CLegend& 
CLegend::operator=( CLegend const& rLegend )
{
  m_FillColor  = rLegend.m_FillColor;
  m_bRound     = rLegend.m_bRound;
  m_Width      = rLegend.m_Width;
  m_PixelValue = rLegend.m_PixelValue;
  m_Legend.SetSize( rLegend.m_Legend.GetSize());
  for( int i = m_Legend.GetSize() ; --i >= 0 ; m_Legend[i] = rLegend.m_Legend[i] );
  return *this;
}
//=====================================================================
int
CLegend::AddLegendItem( COLORREF Color, LPCSTR Prefix, LPCSTR Postfix )
{
	return m_Legend.GetSize() < MAX_LEGEND_SIZE ? 
		m_Legend.Add( LegendItem( Color, Prefix, Postfix )) : - 1;
}
//=====================================================================
static COLORREF
MakeDark( COLORREF c, float k )
{
  int r = c & 0xff;
  int g = ( c >> 8 ) & 0xff;
  int b = ( c >> 16 ) & 0xff;
  r *= k;
  g *= k;
  b *= k;
  return ((( b << 8 ) | g ) << 8 ) | r;
}
//=====================================================================
void
CLegend::Draw( CDC* pDC, CPoint cp, DiagItem* pItem ) const
{
  float S0 = 0, S1 = 0;
	
	double pcf = pDC->IsPrinting() ? pDC->GetDeviceCaps( LOGPIXELSX ) / pView->GetDC()->GetDeviceCaps( LOGPIXELSX ) : 1;
	double ppv = m_PixelValue * pcf;
  
	for( int n = GetSize() ; --n >= 1 ; S0 += pItem[n].m_Value );

  CSize t( CSize( m_Width * ppv, S0 * ppv ));
	pDC->DPtoLP( &t );

  int h = t.cy;
  int w = t.cx;

  int w1 = w / 3;
  int h1 = w1 / 2;
  int w0 = w - h1;

  COLORREF lc = RGB( 64, 64, 64 );
  CPen lp( PS_SOLID, 0, lc );
  CPen* pOldPen = pDC->SelectObject( &lp );

  if( m_bRound )
  {
    int l = cp.x - w / 2;
    int r = l + w;
    int b = cp.y;
    int t = b - h;

    CRect r0( l, t - h1, r, t + h1 );
    CRect r1( l, b - h1, r, b + h1 );
    
    CPoint lt( l, t );
    CPoint rt( r, t );
    CPoint lb( l, b );
    CPoint rb( r, b );

    int a0, a1 = 900;
    CPoint p0, sp( cp.x, r0.top ), p1( sp );
    for( int n = GetSize() ; --n >= 0 ; )
    {
      DiagItem* pI = &pItem[n];
      if( pI->m_Value <= 0 )
        continue;

      S1 += pItem->m_Value;
      int bLast = S1 == S0;
      a0 = a1;
      p0 = p1;
      a1 = bLast ? 4500 : a0 + pI->m_Value * 3600 / S0;
      double co = cos( a1 ), si = sin( a1 );
      p1 = bLast ? sp : CPoint( cp.x + w * co / 2, cp.y - h - h1 * si );
      pI->m_Pg[0][0] = r0.TopLeft();
      pI->m_Pg[0][1] = r0.BottomRight();
      pI->m_Pg[0][2] = CPoint( a0, a1 - a0 );

      COLORREF c = m_Legend[n].m_FillColor;      
      CBrush fb( c );
      CBrush fd( MakeDark( c, a0 >= 2700 ? .8 : .9 ));
      CBrush* pOldBrush = (CBrush*)pDC->SelectObject( &fb );
      if( pItem->m_Value == S0 )
      {
        pDC->SelectStockObject( NULL_PEN );
        pDC->SelectObject( &fd );
        pDC->Chord( &r1, lb, rb );
        pI->m_Pg[1][0] = lb;
        pI->m_Pg[1][1] = lt;
        pI->m_Pg[1][2] = rt;
        pI->m_Pg[1][3] = rb;
        pDC->Polygon( pI->m_Pg[1], 4 );
        pDC->SelectObject( &fb );
        pDC->SelectObject( &lp );
        pDC->Ellipse( &r0 );
      }
      else
      {
        if( a1 <= 1800 || a0 >= 3600 )
        {
          pDC->SelectObject( &lp );
          pDC->SelectObject( &fb );
          pDC->Pie( &r0, p0, p1 );
          memset( pI->m_Pg[1], 0, sizeof pI->m_Pg[1] );
        }
        else
        {
          CPoint tl( a0 > 1800 && a0 < 3600 ? p0 : lt );
          CPoint tr( a1 > 1800 && a1 < 3600 ? p1 : rt );
          CPoint bl( tl.x, tl.y + h );
          CPoint br( tr.x, tr.y + h );
          pI->m_Pg[1][0] = tl;
          pI->m_Pg[1][1] = tr;
          pI->m_Pg[1][2] = br;
          pI->m_Pg[1][3] = bl;
          pDC->SelectStockObject( NULL_PEN );
          pDC->SelectObject( &fd );
          pDC->Polygon( pI->m_Pg[1], 4 );
          pDC->Chord( r1, bl, br );
          pDC->SelectObject( &lp );
          pDC->SelectObject( &fb );
          pDC->Pie( &r0, p0, p1 );
          pDC->MoveTo( tl );
          pDC->LineTo( bl );
        }        
      }
      pDC->SelectObject( pOldBrush );
    }
    pDC->MoveTo( lt );
    pDC->LineTo( lb );
    pDC->MoveTo( rt );
    pDC->LineTo( rb );
    pDC->Arc( &r1, lb, rb );
  }
  else
  {
    int l = cp.x - w / 2;
    int r = l + w0;
    int b = cp.y + h1;
    int t = b;
    CPoint p0( l, b );
    CPoint p1( l, t );
    CPoint p2( r, t );
    CPoint p3( r, b );
    CPoint p4( r + h1, b - h1 );
    CPoint p5( r + h1, t - h1 );
    CPoint p6( l + h1, t - h1 );

    for( int n = 0 ; n < GetSize() ; ++n )
    {
      DiagItem* pI = &pItem[n];
      if( pI->m_Value <= 0 )
        continue;

      CSize ts( pView->DP2LP( CSize( pI->m_Value * ppv, 0 )));
			if( ts.cx == 0 )
				ts.cx = 1;

      p1.y -= ts.cx;
      p2.y -= ts.cx;
      p5.y -= ts.cx;
      p6.y -= ts.cx;

      pI->m_Pg[0][0] = p0;
      pI->m_Pg[0][1] = pI->m_Pg[2][0] = p1;
      pI->m_Pg[0][2] = pI->m_Pg[1][0] = pI->m_Pg[2][1] = p2;
      pI->m_Pg[0][3] = pI->m_Pg[1][1] = p3;
      pI->m_Pg[1][2] = p4;
      pI->m_Pg[1][3] = pI->m_Pg[2][2] = p5;
      pI->m_Pg[2][3] = p6;
      
      COLORREF c = m_Legend[n].m_FillColor;      
      CBrush fb( c );
      CBrush fd( MakeDark( c, .9 ));
      CBrush fr( MakeDark( c, .8 ));

      CBrush* pOldBrush = (CBrush*)pDC->SelectObject( &fd );
      pDC->Polygon( pI->m_Pg[0], 4 );
      pDC->SelectObject( &fb );
      pDC->Polygon( pI->m_Pg[2], 4 );
      pDC->SelectObject( &fr );
      pDC->Polygon( pI->m_Pg[1], 4 );
      pDC->SelectObject( pOldBrush );
      p0 = p1;
      p3 = p2;
      p4 = p5;
    }
  }
  pDC->SelectObject( pOldPen );
}
//=====================================================================
CString const*
CLegend::Detect( CSpot const& spot, DiagItem* pItem ) const
{
  static CString s;
  int bTop = 1;
  DiagItem* pI;

  int i = GetSize();
  while( --i >= 0 )
  {
    pI = &pItem[i];
    if( !pI->m_Value )
      continue;

    if( m_bRound ? spot.DetectPie( *((CRect*)pI->m_Pg[0] ), pI->m_Pg[0][2].x, pI->m_Pg[0][2].y ) ||
                   spot.DetectPolygon( pI->m_Pg[1], 4 )
                   :
                   spot.DetectPolygon( pI->m_Pg[0], 4 ) ||
                   spot.DetectPolygon( pI->m_Pg[1], 4 ) ||
                   bTop && spot.DetectPolygon( pI->m_Pg[2], 4 ))
      break;
    bTop = 0;
  }

  if( i < 0 )
    return 0;

  LegendItem const* pL = &m_Legend[i];
  s = pL->m_Prefix.IsEmpty() ? pI->m_Text : pL->m_Prefix + " " + pI->m_Text;
  if( !pL->m_Postfix.IsEmpty())
    s += " ", s += pL->m_Postfix;

  return &s;
}
//=====================================================================
CBox
CLegend::CalcBox( CPoint cp, DiagItem* pItem ) const
{
  CBox box;
  float S0 = 0;
  for( int n = GetSize(); --n >= 0 ; S0 += pItem[n].m_Value );

  CSize s( pView->DP2LP( CSize( m_Width, S0 * m_PixelValue )));
	if( s.cy == 0 )
		s.cy = 1;

  int h = s.cy;
  int w = s.cx;
  int h1 = w / 4;

  int l = cp.x - w / 2;
  int r = l + w;
  int b = cp.y + h1;
  int t = cp.y - h1 - h;
    
  box |= CPoint( l, t ); 
  box |= CPoint( r, b ); 
  return box;
}
//=====================================================================
//=====================================================================
int
CLegendCollection::AddLegend( CLegend* pLegend )
{
	if( m_Size == MAX_LEGEND_SIZE )
		return -1;

  int i = m_Size;
  while( --i >= 0 && m_Content[i] != *pLegend );
  if( i >= 0 )
 	  return i;

	m_Content[m_Size++] = *pLegend;
 	return m_Size - 1;
}
//=====================================================================
//=====================================================================
CLegend*
CLegendCollection::GetLegend( int ndx )
{
	return ndx < m_Size ? &m_Content[ndx] : 0;
}
//=====================================================================
int
CMapView::AddLegendItem( int nLegend, COLORREF Color, LPCSTR Prefix, LPCSTR Postfix )
{
	CLegend* pLg = GetLegend( nLegend );
	if( !pLg )
		return 0;
	pLg->AddLegendItem( Color, Prefix, Postfix );
	return 1;
}
//=====================================================================
