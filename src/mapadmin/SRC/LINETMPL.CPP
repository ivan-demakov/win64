//=====================================================================
// line template class implementation
//=====================================================================
#include "stdafx.h"

#include <fstream>
#include <limits.h>
#include "linetmpl.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "util.h"
#include "mathutil.h"
#include "mview.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CArray<CLineTmpl*,CLineTmpl*> CLineTmpl::m_TmplArr;
TCHAR CLineTmpl::TmplFileName[] = "linetmpl.bin";
TCHAR CLineTmpl::m_Ttl0[] = "TGRAD_LINE_TEMPLATES";
TCHAR CLineTmpl::m_Ttl1[] = "Tgrad Line Templates";
TCHAR CLineTmpl::m_Th[]   = "TMPL";
//=====================================================================
void
CLineTmpl::CreateTemplates()
{
  LPCSTR TmplPath( App.GetProjectFilePath( TmplFileName ));

  std::ifstream ts( TmplPath, std::ios::in | std::ios::binary, std::filebuf::sh_none );

  if( ts.fail())
  {
    ErrBox( Compose( "Не найден файл шаблонных линий\n", TmplFileName, 0 ));
    return;
  }

  char buf[32] = "";
  ts.read( buf, sizeof m_Ttl0 );

  int v0 = !strcmp( buf, m_Ttl0 );
  int v1 = !strcmp( buf, m_Ttl1 );
  if( !v0 && !v1 )
  {
    ErrBox( "Некорректное начало файла шаблонов" );
    return;
  }

  int nt;
  ts.read( LPTSTR( &nt), sizeof( int ));

  CClientDC dc( 0 );
  int resl = dc.GetDeviceCaps( LOGPIXELSX );

  while( --nt >= 0 )
  {
    ts.read( buf, sizeof m_Th );
    if( ts.eof())
      break;

    if( strcmp( buf, m_Th ))
    {
      ErrBox( "Некорректное начало шаблона" );
      return;
    }

    int tmplIndex, scale, period, fuflo[2];
    ts.read( LPTSTR( &tmplIndex ), sizeof( int ));
    ts.read( LPTSTR( &scale ), sizeof( int ));
    ts.read( LPTSTR( &period ), sizeof( int ));
    ts.read( LPTSTR( &fuflo ), sizeof( int[CELEM(fuflo)] ));

    int ibuf[MaxTemplateLength];

    if( v0 )
      scale *= 40;

    double sz = scale * 2.54 * SmInUnit / resl;

    ibuf[0] = period * sz;

    for( int i = 1 ; i < CELEM( ibuf ) ; )
    {
      int pllc;
      ts.read( LPTSTR( &pllc ), sizeof( int ));
      ibuf[i]= pllc;
      ++i;

      if( !pllc )
        break;

      for( pllc = pllc * 2 + 1 ; --pllc >= 0 ; ++i )
      {
        int t;
        ts.read( LPTSTR( &t ), sizeof t );
        ibuf[i] = t * sz;
      }

      if( ts.eof())
      {
        ErrBox( "Конец файла в описании шаблона" );
        return;
      }
    }
    m_TmplArr.Add( new CLineTmpl( tmplIndex, i, ibuf ));
  }
}
//=====================================================================
void
CLineTmpl::DeleteTemplates()
{
  for( int i = CLineTmpl::m_TmplArr.GetSize() ; --i >= 0 ; delete m_TmplArr[i] );
  CLineTmpl::m_TmplArr.RemoveAll();
}
//=====================================================================
CLineTmpl::CLineTmpl( int index, int count, int* tmpl ) :
  m_Index( index ),
  m_pImgBuf( 0 )
{
  memcpy( m_pTemplate = new int[count], tmpl, count * sizeof( int ));

  m_yMin = INT_MAX;
  m_yMax = INT_MIN;

  int* ptmpl = m_pTemplate + 1;

  for( int np ; np = *ptmpl++ ; ptmpl += np * 2 )
  {
    int w = *ptmpl++ + 1 >> 1;
    CPoint *cp = (CPoint*)ptmpl;
    for( int n = np ; --n >= 0 ; ++cp )
    {
      m_yMax = max( m_yMax, cp->y + w );
      m_yMin = min( m_yMin, cp->y - w );
    }
  }
}
//=====================================================================
CLineTmpl::~CLineTmpl()
{
  delete m_pTemplate;
}
//=====================================================================
CLineTmpl*
CLineTmpl::GetTemplate( int index )
{
	int i;
  for(i = m_TmplArr.GetSize() ; --i >= 0 && m_TmplArr[i]->m_Index != index ; );
  return i >= 0 ? m_TmplArr[i] : 0;
}
//=====================================================================
static int
LeftOf( CPoint const& p0, CPoint const& p1, CPoint const& p2 )
{
  if( p1.x == p2.x )
    return p0.x < p1.x;

  if( p0.x < p1.x && p0.x < p2.x )
    return 1;

  if( p0.x > p1.x && p0.x > p2.x )
    return 0;

  int dx = p2.x - p1.x, dy = p2.y - p1.y;
  int dp = p0.y - p1.y - MulDiv( p0.x - p1.x, dy, dx );
  return ( dx ^ dy ^ dp ) >= 0;
}
//=====================================================================
static int
MakeCase( int* pv, int n )
{
	int c;
  for(int c = 0 ; --n >= 0 ; c = c << 1 | *pv++ );
  return c;
}
//=====================================================================
static CPoint
Ratio( CPoint p, int num, int den )
{
  return CPoint( MulDiv( p.x, num, den ), MulDiv( p.y, num, den ));
}
//=====================================================================
int
CLineTmpl::SaveImg()
{
  if( !m_pImgBuf )
    return 0;

  m_pImgBuf->write((char*)&m_Width, sizeof m_Width );
  m_pImgBuf->write((char*)&m_p0, sizeof m_p0 );
  m_pImgBuf->write((char*)&m_p1, sizeof m_p1 );
  return 1;
}
//=====================================================================
void
CLineTmpl::DrawFull( CDC* pDC, int ntempl, CPoint* pPoints, int count,
                     int orgx, int tmpLen,
                     CPoint ltop, CPoint lbtm, CPoint rtop, CPoint rbtm,
                     CPoint Start,
                     int dx, int dy, int dl, int lmg, int rmg,
                     int num, int den )
{
  int mlt = lmg ^ ltop.x, mlb = lmg ^ lbtm.x;
  int mrt = rmg ^ rtop.x, mrb = rmg ^ rbtm.x;
  for( CPoint org( orgx, 0 ) ; --ntempl >= 0 ; org.x += tmpLen )
  {
    CPoint* pp = pPoints, p1, p2( org + Ratio( *pp++, num, den ));
    for( int cnt = count ; --cnt > 0 ; )
    {
      CPoint p3( p1 = p2 ), p4( p2 = org + Ratio( *pp++, num, den ));
	  int bd, i;
      for( bd = 1, i = 2 ; --i >= 0 ; )
      {
        int v[] = { LeftOf( p3, ltop, lbtm ),
                     LeftOf( p4, ltop, lbtm ),
                     !LeftOf( p3, rtop, rbtm ),
                     !LeftOf( p4, rtop, rbtm ) };
        switch( MakeCase( v, CELEM( v )))
        {
          case 3 : case 12 : bd = 0; break;
          case 9 : CrossPoint( p3, p4, ltop, lbtm, &p3, 0 );
          case 1 : CrossPoint( p3, p4, rtop, rbtm, &p4, 0 ); break;
          case 6 : CrossPoint( p3, p4, rtop, rbtm, &p3, 0 );
          case 4 : CrossPoint( p3, p4, ltop, lbtm, &p4, 0 ); break;
          case 2 : CrossPoint( p3, p4, rtop, rbtm, &p3, 0 ); break;
          case 8 : CrossPoint( p3, p4, ltop, lbtm, &p3, 0 ); break;
        }
        ltop.x ^= mlt; lbtm.x ^= mlb; rtop.x ^= mrt; rbtm.x ^= mrb;
      }
      if( !bd )
        continue;

      m_p0 = TurnPoint( p3, dx, dy, dl ) + Start;
      m_p1 = TurnPoint( p4, dx, dy, dl ) + Start;
      if( !SaveImg())
      {
        pDC->MoveTo( m_p0 );
        pDC->LineTo( m_p1 );
      }
    }
  }
}
//=====================================================================
void
CLineTmpl::DrawPart( CDC* pDC, int ntempl, CPoint* pPoints, int count,
                     int orgx, int tmpLen,
                     CPoint top, CPoint btm,
                     CPoint Start,
                     int dx, int dy, int dl, int mgn, int bLeft,
                     int num, int den )
{
  int mmt = mgn ^ top.x, mmb = mgn ^ btm.x;
  for( CPoint org( orgx, 0 ) ; --ntempl >= 0 ; org.x += tmpLen )
  {
    CPoint* pp = pPoints, p1, p2( Ratio( *pp++, num, den ) + org );
    for( int cnt = count ; --cnt > 0 ; )
    {
      CPoint p3( p1 = p2 ), p4( p2 = Ratio( *pp++, num, den )+ org );
      for( int bd = 1, i = 2 ; --i >= 0 ; )
      {
        int v[] = { LeftOf( p3, top, btm ),
                    LeftOf( p4, top, btm ),
                    bLeft };
        switch( MakeCase( v, CELEM( v )))
        {
          case 0 : case 7 : bd = 0; break;
          case 2 : case 5 : CrossPoint( p3, p4, top, btm, &p3, 0 );
          case 1 : case 6 : break;
          case 3 : case 4 : CrossPoint( p3, p4, top, btm, &p4, 0 );
        }
        top.x ^= mmt; btm.x ^= mmb;
      }
      if( !bd )
        continue;

      m_p0 = TurnPoint( p3, dx, dy, dl ) + Start;
      m_p1 = TurnPoint( p4, dx, dy, dl ) + Start;
      if( !SaveImg())
      {
        pDC->MoveTo( m_p0 );
        pDC->LineTo( m_p1 );
      }
    }
  }
}
//=====================================================================
void
CLineTmpl::GetRange( int* pMin, int* pMax, int bNow ) const
{
  int num = 1, den = 1;

  if( bNow )
    pView->GetScaleRatio( CDrawObject::GetDrawnClass(), &num, &den );

  *pMin = MulDiv( m_yMin, num, den );
  *pMax = MulDiv( m_yMax, num, den );
}
//=====================================================================
static void
CalcMargin( CPoint p, CPoint* pT, CPoint* pB, int sg )
{
  if( p.y )
  {
    int l = hipo( p.x, p.y );
    int m = p.x * ( l + p.x * sg ) + p.y * p.y * sg ;
    int d = l * p.y;
    pT->x += MulDiv( pT->y, m, d );
    pB->x += MulDiv( pB->y, m, d );
  }
}
//=====================================================================
int
CLineTmpl::DrawTmpLine( CDC* pDC, COLORREF color, int num, int den,
                        int shift,
                        CPoint* pPrev, CPoint* pStart,
                        CPoint* pEnd, CPoint* pNext )
{
  if( *pEnd == *pStart )
    return shift;

  CSize S( *pEnd - *pStart );
  double L = hipo( S.cx, S.cy );

  int* ptmpl = m_pTemplate;
  int tmpLen = MulDiv( *ptmpl++, num , den );

  int wl = MulDiv( m_yMin, num, den );
  int wh = MulDiv( m_yMax, num, den );
  int w = wh - wl >> 1;

  CPoint LT( 0, wl ), LB( 0, wh ), RT( L, wl ), RB( L, wh );

  if( pPrev )
    CalcMargin( TurnPoint( *pPrev - *pStart, S.cx, S.cy, L ), &LT, &LB,  1 );

  if( pNext )
    CalcMargin( TurnPoint( *pNext - *pEnd,   S.cx, S.cy, L ), &RT, &RB, -1 );

  int l0 = min( LT.x, LB.x ), l1 = max( LT.x, LB.x );
  int r0 = min( RT.x, RB.x ), r1 = max( RT.x, RB.x );
  int ln = ( l1 - l0 + shift + tmpLen - 1 ) / tmpLen;
  int l2 = l0 - shift + ln * tmpLen;
  int mn = ( r0 - l2 ) / tmpLen;
  int r2 = l2 + mn * tmpLen;
  int rn = ( r1 - r2 + tmpLen - 1 ) / tmpLen;

  for( int nPoints ; nPoints = *ptmpl++ ; ptmpl += nPoints * 2 )
  {
    m_Width = MulDiv( *ptmpl++, num, den );
    LOGBRUSH lb = { BS_SOLID, color, 0 };
    CPen pen( PS_GEOMETRIC|PS_ENDCAP_ROUND|PS_JOIN_ROUND, m_Width, &lb );
    CPen* pOldPen = pDC->SelectObject( &pen );

    if( mn > 0 )
    {
      if( ln > 0 )
        DrawPart( pDC, ln, (CPoint*)ptmpl, nPoints, l0 - shift,
                  tmpLen, LT, LB, *pStart, S.cx, -S.cy, L, -w, 1, num, den );

      ASSERT( nPoints <= MaxSegmentLength );
      CPoint segmen[MaxSegmentLength];
      CPoint* pSg = segmen;
      CPoint* pp = (CPoint*)ptmpl, org( l2, 0 );
      int np = min( MaxSegmentLength, nPoints );
	  int count;
      for(count = np ; --count >= 0 ;
           *pSg++ = *pStart + TurnPoint( Ratio( *pp++, num, den ) + org, S.cx, -S.cy, L ));

      for( int n = mn, sh = 0 ; --n >= 0 ; sh += tmpLen )
      {
        CSize step( MulDiv( sh, S.cx, L ), MulDiv( sh, S.cy, L ));
        pSg = segmen;
        m_p0 = step + *pSg++;
        pDC->MoveTo( m_p0 );
        for( count = np ; --count > 0 ; m_p0 = m_p1 )
        {
          m_p1 = step + *pSg++;
          if( !SaveImg())
            pDC->LineTo( m_p1 );
        }
      }

      if( rn > 0 )
        DrawPart( pDC, rn, (CPoint*)ptmpl, nPoints, r2,
                  tmpLen, RT, RB, *pStart, S.cx, -S.cy, L, L + w, 0, num, den );
    }
    else
      DrawFull( pDC, ln + rn, (CPoint*)ptmpl, nPoints, l0 - shift,
                tmpLen, LT, LB, RT, RB, *pStart, S.cx, -S.cy, L,
                -w, L + w, num, den );

    pDC->SelectObject( pOldPen )->DeleteObject();
  }

  return ( mn > 0 ? r1 - r2 :  r1 - l0 + shift ) % tmpLen;
}
//=====================================================================
void
CLineTmpl::Draw( CDC* pDC, CPoint* points, int count )
{
  int num, den;
  pView->GetScaleRatio( CDrawObject::GetDrawnClass(), &num, &den, pDC->IsPrinting());

  LOGPEN lp;
  pDC->GetCurrentPen()->GetLogPen( &lp );

  for( int shift = 0, i = 1 ; i < count ; ++i, ++points )
    shift = DrawTmpLine( pDC, lp.lopnColor, num, den, shift,
                         i > 1 ? points - 1 : 0, points, points + 1,
                         i < count - 1 ? points + 2 : 0 );
}
//=====================================================================
void
CLineTmpl::Draw( CDC* pDC, CPoint* points, int count, COLORREF color, double cf )
{
  int den = 32768, num = den * cf;
  for( int shift = 0, i = 1 ; i < count ; ++i, ++points )
    shift = DrawTmpLine( pDC, color, num, den, shift,
                         i > 1 ? points - 1 : 0, points, points + 1,
                         i < count - 1 ? points + 2 : 0 );
}
//=====================================================================
void
CLineTmpl::InflateClipSize( CSize& size )
{
  int num, den;
  pView->GetScaleRatio( CDrawObject::GetDrawnClass(), &num, &den, 0 );

  int w = max( MulDiv( abs( m_yMax ), num, den ),
               MulDiv( abs( m_yMin ), num, den ));
  size.cx = max( size.cx, w );
  size.cy = max( size.cy, w );
}
//=====================================================================
