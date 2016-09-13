#include "stdafx.h"

#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "cbox.h"
#include "mdoc.h"
#include "mview.h"
#include "util.h"
#include "linetmpl.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
void
CBox::operator|=( CPoint const& p )
{
  if( IsRectNull())
    SetRect( p.x, p.y, p.x, p.y );
  else
  {
    if( p.x < left )
      left = p.x;
    else
    if( p.x > right )
      right = p.x;

    if( p.y < top )
      top = p.y;
    else
    if( p.y > bottom )
      bottom = p.y;
  }
}
//=====================================================================
void
CBox::operator|=( CRect r )
{
  if( !r.IsRectNull())
  {
    *this |= r.TopLeft();
    *this |= r.BottomRight();
  }
}
//=====================================================================
CBox::CBox( CRect const& r, CPoint c, int a, int w )
{
  CPoint p[] = { TurnPoint( r.left,  r.top,    c, a ),
                 TurnPoint( r.right, r.top,    c, a ),
                 TurnPoint( r.left,  r.bottom, c, a ),
                 TurnPoint( r.right, r.bottom, c, a ) };
  
  SetRect( p->x, p->y, p->x, p->y );
  for( int i = CELEM( p ) ; --i > 0 ; 
       left   = min( p[i].x, left   ),
       right  = max( p[i].x, right  ),
       top    = min( p[i].y, top    ),
       bottom = max( p[i].y, bottom ));

  w = w / 2 + 1;
  InflateRect( w, w );
}
//=====================================================================
CBox::CBox( CRect const& r, int w ) : CRect( r ) 
{
  w = w / 2 + 1;
  InflateRect( w, w );
}
//=====================================================================
CBox::CBox( CPoint* pPnt, int nPnt, int nStyle, int w, int bNow )
{
  Reset();

  if( nPnt )
  {
    CLineTmpl* pTempl = CLineTmpl::GetTemplate( nStyle );
    if( pTempl )
    {
      int wl, wh;
      pTempl->GetRange( &wl, &wh, bNow );
      w = ::MulDiv( max( abs( wh ), abs( wl )), 14142, 10000 );
    }
    else
      w = w / 2 + 1;

    while( --nPnt >= 0 )
      *this |= *pPnt++;
  
    InflateRect( w, w );
  }
}
//=====================================================================
CBox::CBox( CPoint tp, CSize off, int angle, char const* pStr, int len,
            int number, int height, int weight, int style )
{
  CFontItem fi( number, height, weight, style );
  CText t( tp.x, tp.y, 0, fi, pStr, len, off.cx, off.cy, angle );
  SetRect( 0, 0, 0, 0 );
  t.CalcBoundBox( *this );
}
//=====================================================================
CBox::CBox( CPoint pt, CSize sz, int stAng, int fnAng, int w, int bPie )
{
  CRect rect( pt, sz );
  rect.NormalizeRect();

  int a = rect.Width()  >> 1;
  int b = rect.Height() >> 1;
  if( fnAng < 0 )
  {
    stAng += fnAng;
    fnAng = -fnAng;
  }
  fnAng %= 3600;
  stAng = (( stAng % 3600 ) + 3600 ) % 3600;
  fnAng += stAng;

  double ab = a * b;
  double ss = sin( stAng ), sc = cos( stAng );
  double fs = sin( fnAng ), fc = cos( fnAng );
  double scb = sc * b, ssa = ss * a;
  double fcb = fc * b, fsa = fs * a;
  double sf = scb * scb + ssa * ssa;
  double ff = fcb * fcb + fsa * fsa;
  double sr = sf ? ab / sqrt( sf ) : 0;
  double fr = ff ? ab / sqrt( ff ) : 0;
  CPoint C( rect.CenterPoint());
  CPoint sp( C + CSize( sr * sc, -sr * ss ));
  CPoint fp( C + CSize( fr * fc, -fr * fs ));

  *this = CBox( sp );
  *this |= fp;

  if( stAng <= 900 && fnAng >= 900 || stAng <= 4500 && fnAng >= 4500 )
    C.y -= b, *this |= C, C.y += b;
  if( stAng <= 1800 && fnAng >= 1800 || stAng <= 5400 && fnAng >= 5400 )
    C.x -= a, *this |= C, C.x += a;
  if( stAng <= 2700 && fnAng >= 2700 || stAng <= 6300 && fnAng >= 6300 )
    C.y += b, *this |= C, C.y -= b;
  if( stAng <= 3600 && fnAng >= 3600 || stAng <= 7200 && fnAng >= 7200 )
    C.x += a, *this |= C, C.x -= a;

  if( bPie )
    *this |= C;

  w = w / 2 + 1;
  InflateRect( w, w );
}
//=====================================================================
CBox::CBox( int ref, CPoint pt, double cf )
{
  CBmpDef* pBD;
  if( pBD = pDocument->GetBmpDef( ref ))
  {
    CSize s( pBD->GetBmpSize( pView->GetAltScale() / cf ));
    CSize ss( s.cx + 1 >> 1, s.cy + 1 >> 1 );
    *this = CRect( pt - ss, s );
  }
  else
    Reset();
}
//=====================================================================



