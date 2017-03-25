//=====================================================================
#include <math.h>
#include "stdafx.h"
#include "rgnspace.h"
#include "mathutil.h"
#include "cbox.h"
#include "dwin.h"
//=====================================================================
#ifdef _CLIENT_APP
  #ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char BASED_CODE THIS_FILE[] = __FILE__;
  #endif
#endif
//=====================================================================	
RgnPool::RgnPool() : pPool( 0 ), PoolSize( 0 ) 
{}
//=====================================================================
int
RegionDef::IsCrossedByLine( CPoint p0, CPoint p1 )
{
  if( !PtInRect( p0 ) || !PtInRect( p1 ))
    return 0;

  return IsLineCrossPolygon( p0, p1, pPoly+1, pPoly->x );
}
//=====================================================================
void
RgnPool::AddRgn( RegionDef* pObj )
{
  if( !pPool )
    CopyRect( *pObj );
  else
  {
    if( pObj->nCw != pPool->nCw )
      ReversePoly( pObj->pPoly+1, pObj->pPoly->x );
    *this |= *pObj;
  }
  pObj->pNext = pPool;
  pPool = pObj;
  ++PoolSize;
}
//=====================================================================
void
RgnPool::AddRgn( CPoint* pPnt )
{
  RegionDef* pObj = new RegionDef( pPnt );
  AddRgn( pObj );
}
//=====================================================================
RegionDef*
RgnPool::RemoveHead()
{
  if( !pPool )
    return 0;
  
  RegionDef* h = pPool;
  pPool = pPool->pNext;
  --PoolSize;
  h->pNext = 0;
  return h;
}
//=====================================================================
RegionDef*
RgnPool::GetNext()
{
  if( !pPool )
    return 0;
  
  RegionDef* h = pPool;
  pPool = pPool->pNext;
  --PoolSize;
  return h;
}
//=====================================================================
void
RgnPool::GlueRgn()
{
  int bs = 1;
  while( pPool && bs )
  {
    RegionDef* prg0 = RemoveHead();
    CPoint* pp0 = prg0->pPoly;

    RgnPool tmp;
    bs = 0;
    while( pPool )
    {
      RegionDef* prg1 = RemoveHead();
      CPoint* pp1 = prg1->pPoly;
      CPoint* pp2 = SmartSubConPolygons( pp0 + 1, pp0->x, pp1 + 1, pp1->x, 2 );
      if( pp2 )
      {
        delete pp0;
        prg0->pPoly = pp0 = pp2;
        delete prg1;
        bs = 1;
      }
      else
        tmp.AddRgn( prg1 );
    }
    AddRgn( prg0 );
    while( prg0 = tmp.RemoveHead())
      AddRgn( prg0 );
  }
}
//=====================================================================
RegionDef*
RgnPool::Connect()
{
  if( PoolSize == 0 )
    return 0;

  RegionDef* prg0 = RemoveHead();
  CPoint* pp0 = prg0->pPoly;
  
  int nn[1024][2];
  
  int nTry = 0;
  int nRot = 0;
  
  while( pPool )
  {
    RegionDef* prg1 = RemoveHead();
    CPoint* pp1 = prg1->pPoly;

    int bs = ( nTry + 1 ) * 4;
    int bs2 = bs * 16;

    int in = FindPointToPoint( pp0 + 1, pp0->x, pp1 + 1, pp1->x, nn, 1024, bs2, bs );
    int n0, n1, i;

    for( i = nTry * 4 ; i < in ; ++i )
    {
      n0 = nn[i][0];
      n1 = nn[i][1];
      CPoint p0( pp0[n0] );
      CPoint p1( pp1[n1] );
      if( prg0->IsCrossedByLine( p0, p1 ) ||
          prg1->IsCrossedByLine( p0, p1 ))
        continue;

      RegionDef* tsp;
      for( tsp = pPool ; tsp ; tsp = tsp->pNext )
        if( tsp->IsCrossedByLine( p0, p1 ))
          break;

      if( !tsp )
        break;
    }

    if( i < in || nTry == 15 )
    {
      if( nTry == 15 )
      {
        n0 = nn[0][0];
        n1 = nn[0][1];
      }
      CPoint* pp2 = SubConPolygons( pp0 + 1, pp0->x, pp1 + 1, pp1->x, n0, n1 );
      delete pp0;
      prg0->pPoly = pp0 = pp2;
      delete prg1;
    }
    else
    {
      RegionDef** ppr;
      for( ppr = &pPool ; *ppr ; ppr = &(*ppr)->pNext );
      (*ppr) = prg1;
      ++PoolSize;

      if( ++nRot >= PoolSize )
      {
        nRot = 0;
        ++nTry;
      }
    }
  }

  return prg0;
}
//=====================================================================
RegionDef::~RegionDef()
{
  delete pPoly;
}
//=====================================================================
RegionDef::RegionDef( CPoint* pPnt ) :
  pPoly( pPnt ),
  pNext( 0 )
{
	if( pPoly )
	{
    int n = (*pPnt++).x;
    nCw = Clockwise( pPnt, n );
    CBox box( pPnt, n );
    CopyRect( &box );
	  InnerPoint = FindInnerPoint();
	}
}
//=====================================================================
RegionDef::RegionDef( CPoint* pPnt, CRect& box, int cw ) :
  pPoly( pPnt ),
  pNext( 0 ),
  nCw( cw )
{
	if( pPoly )
	{
    CopyRect( &box );
	  InnerPoint = FindInnerPoint();
	}
}
//=====================================================================
RegionDef::RegionDef( RegionDef& pr ) :
  pPoly( pr.pPoly ),
  nCw( pr.nCw ),
  pNext( 0 ),
  InnerPoint( pr.InnerPoint )
{
  CopyRect( pr );
}
//=====================================================================
CPoint*
RegionDef::Close()
{
  CPoint* p = pPoly;
  pPoly = 0;
  delete this;
  return p;
}
//=====================================================================
int
RegionDef::SubtractCrossed( RegionDef* pr1, RgnPool& Rst )
{
  if( !Intersect( this, pr1 ))
    return 0;

  CPoint* pp0 = pPoly + 1;
  CPoint* pp1 = pr1->pPoly + 1;
  int n0 = pPoly->x;
  int n1 = pr1->pPoly->x;
  int n = 0;

  if( nCw == pr1->nCw )
  {
    ReversePoly( pp1, n1 );
    pr1->nCw = -pr1->nCw;
  }

  if( IsPolygonCrossPolygon( pp0, n0, pp1, n1, 0 ))
  {
    pr1->pPoly->y = 1;

    LItemHash2 lh( n0 + n1 );
    SmartClipPlg( pp1, n1, pp0, n0, &lh, 0, 1 );
    SmartClipPlg( pp0, n0, pp1, n1, &lh, 0, 0 );

    LsList pgs;
    CPoint p0, p1;
    while( lh.GetHead( p0, p1 ))
    {
      PtList* pplg = new PtList;
      pplg->AddHead( p0 );
      pplg->AddTail( p1 );
      for(;;)
      {
        CPoint np;
        if( lh.GetNext( p0, np ))
          if( np == p1 )
          {
            lh.Add( p0, np );
            break;
          }
          else
            pplg->AddHead( p0 = np );
        else
        if( lh.GetNext( p1, np ))
          if( np == p0 )
          {
            lh.Add( p1, np );
            break;
          }
          else
            pplg->AddTail( p1 = np );
        else
          break;
      }

      if( pplg->GetCount() >= 2 && pplg->GetHead() == pplg->GetTail())
        pplg->RemoveTail();

      if( pplg->GetCount() > 2 )
        pgs.AddTail( pplg );
      else
        delete pplg;
    }

    while( !pgs.IsEmpty())
    {
      PtList* pplg = pgs.RemoveHead();
      int np = pplg->GetCount();
      pp1 = (CPoint*)new CPoint[np+1];
      pp1[0] = CPoint( np, 0 );
      CPoint* p = &pp1[1];
      for( int i = np ; --i >= 0 ; *p++ = pplg->RemoveHead());
      delete pplg;
      Rst.AddRgn( pp1 );
      ++n;
    }
  }
  return n;
}
//=====================================================================
CPoint
RegionDef::FindInnerPoint()
{
  CPoint c( CenterPoint());
  CPoint* pp0 = pPoly + 1;
  int n0 = pPoly->x;
	int w = Width(), h = Height();
  if( w < h )
	{
    for( ; w > 1 ; w /= 2 )
      for( c.x = left + w / 2 ; c.x < right ; c.x += w )
				if( IsPointInPolygon( c, pp0, n0, 0 ))
          return c;
	}
  else
	{
    for( ; h > 1 ; h /= 2 )
      for( c.y = top + h / 2 ; c.y < bottom ; c.y += h )
				if( IsPointInPolygon( c, pp0, n0, 0 ))
          return c;
	}
  return c;
}
//=====================================================================
int
RegionDef::SplitByInner( RgnPool& Inner, RgnPool& rst )
{
  for( RegionDef* pr = Inner.GetHead() ; pr ; pr = pr->pNext )
	{
    if( pr->pPoly->y || 
	  	 !PtInRect( pr->TopLeft()) ||
		   !PtInRect( pr->BottomRight()) ||
//		   !IsPointInPolygon( pr->InnerPoint, pPoly + 1, pPoly->x, 0 ))
		   !IsAllPointsInPolygon( pr->pPoly + 1, pr->pPoly->x, pPoly + 1, pPoly->x, 0 ))
			 continue;
		
		if( pr->pPoly->y = Split( pr->InnerPoint, rst, Width() < Height()))
			return 1;
	}
	
  return 0;
}
//=====================================================================
int
RegionDef::Split( RgnPool& rst )
{
  CPoint* pp0 = pPoly + 1;
  int n0 = pPoly->x;
  CPoint c( CenterPoint());
  int w = Width(), h = Height();
  if( w < h )
	{
    for( ; w > 1 ; w /= 2 )
      for( c.x = left + w / 2 ; c.x < right ; c.x += w )
        if( IsPointInPolygon( c, pp0, n0, 0 ) && Split( c, rst, 1 ))
          return 1;
	}
  else
	{
    for( ; h > 1 ; h /= 2 )
      for( c.y = top + h / 2 ; c.y < bottom ; c.y += h )
        if( IsPointInPolygon( c, pp0, n0, 0 ) && Split( c, rst, 0 ))
          return 1;
	}
  return 0;
}
//=====================================================================
int 
RegionDef::Split( CPoint c, RgnPool& rst, int bHorz )
{
  CPoint* pp0 = pPoly + 1;
  int n0 = pPoly->x;

  int i0 = -1, i1 = -1;
  int x0, x1;
  int y0, y1;
  
  CPoint p1, p0( pp0[0] );
  int i = n0;
  while( --i >= 0 )
  {
    p1 = p0;
    p0 = pp0[i];

    if( bHorz )
    {
      if( p0.y > c.y == p1.y > c.y )
        continue;
      int x = p0.x;
      if( p0.y != p1.y )
        x += double( p1.x - p0.x ) / double( p1.y - p0.y ) * double( c.y - p0.y );
      if( x < c.x && ( i0 < 0 || x > x0 ))
        i0 = i, x0 = x;
      else
      if( x > c.x && ( i1 < 0 || x < x1 ))
        i1 = i, x1 = x;
    }
    else
    {
      if( p0.x > c.x == p1.x > c.x )
        continue;
      int y = p0.y;
      if( p0.x != p1.x )
        y += double( p1.y - p0.y ) / double( p1.x - p0.x ) * double( c.x - p0.x );
      if( y < c.y && ( i0 < 0 || y > y0 ))
        i0 = i, y0 = y;
      else
      if( y > c.y && ( i1 < 0 || y < y1 ))
        i1 = i, y1 = y;
    }
  }

  if( i0 < 0 || i1 < 0 )
    return 0;

  p0 = p1 = c;

  if( bHorz )
    p0.x = x0, p1.x = x1;
  else
    p0.y = y0, p1.y = y1;

  if( i0 > i1 )
  {
    i = i0; i0 = i1; i1 = i;
    c = p0; p0 = p1; p1 = c;
  }

	int d = i1 - i0;
	if( d <= 2 || n0 - d <= 2 )
    return 0;

  CPoint* p2;
  CPoint* pt;

  d = i1 - i0 + 2;
  pt = p2 = new CPoint[d+1];
  *pt++ = CPoint( d, 0 );
  *pt++ = p0;
  d = i1 - i0;
  memcpy( pt, pp0 + i0 + 1, d * sizeof CPoint );
  pt += d;
  *pt = p1;
  rst.AddRgn( p2 );
  
  d = n0 - i1 + i0 + 2;
  pt = p2 = new CPoint[d+1];
  *pt++ = CPoint( d, 0 );
  *pt++ = p1;
  d = n0 - i1 - 1;
  memcpy( pt, pp0 + i1 + 1, d * sizeof CPoint );
  pt += d;
  d = i0 + 1;
  memcpy( pt, pp0, d * sizeof CPoint );
  pt += d;
  *pt = p0;
  rst.AddRgn( p2 );

  return 1;
}
//=====================================================================
static RegionDef*
SortRegionDef( RegionDef*& p, int n, int bY )
{
	switch( n )
	{
		case 0:
		  return 0;
		case 1:
		{
		  RegionDef* p0 = p;
		  p = p0->pNext;
		  p0->pNext = 0;
		  return p0;
		}
		case 2:
		{
		  RegionDef* p0 = p;
		  RegionDef* p1 = p0->pNext;
		  p = p1->pNext;
      if( bY ? p1->top < p0->top : p1->left < p0->left )
			{
			  p1->pNext = p0;
			  p0->pNext = 0;
			  return p1;
			}
		  else
			{
  		  p1->pNext = 0;
			  return p0;
			}		
		}
		default:
		{
    	RegionDef* p0 = SortRegionDef( p, n / 2, bY );
	    RegionDef* p1 = SortRegionDef( p, n - n / 2, bY );
	    RegionDef hd( 0 );
	    RegionDef* tl = &hd;
	    
			for(;;)
			{
	      if( !p0 )
				{
			    tl->pNext = p1;
			    break;
				}
	      if( !p1 )
				{
			    tl->pNext = p0;
			    break;
				}
  	    if( bY ? p0->top < p1->top : p0->left < p1->left )
				{
			    tl = tl->pNext = p0;
			    p0 = p0->pNext;
				}
		    else
				{
			    tl = tl->pNext = p1;
			    p1 = p1->pNext;
				}
			}
    	return hd.pNext;
		}
	}
}
//=====================================================================
CPoint
RegionDef::FindCross( CPoint c, int bHorz, int bFar, int bLT, int& nPoint )
{
  CPoint* pp0 = pPoly + 1;
  int n = pPoly->x;

	int d = bFar ? -1 : 0x7ffffff;
	CPoint fp( c );

	nPoint = -1;
	CPoint p0, p1 = pp0[n-1];
	int i = n - 2;
	while( --n >= 0 )
	{
		p0 = p1;
		p1 = *pp0++;
		++i;
		if( bHorz )
		{
			if( p1.y == p0.y ||  p0.y > c.y && p1.y > c.y  || p0.y < c.y && p1.y < c.y )
				continue;

			int x = p0.x + ::MulDiv( p1.x - p0.x, c.y - p0.y, p1.y - p0.y );
			if( p0.x > x && p1.x > x || p0.x < x && p1.x < x || bLT >= 0 && ( bLT ? x > c.x : x < c.x ))
				continue;

			int t = abs( x - c.x );
			if( bFar ? t > d : t < d )
			{
				d = t;
				fp.x = x;
				nPoint = i % pPoly->x;
			}
		}
		else
		{
			if( p1.x == p0.x || p0.x > c.x && p1.x > c.x || p0.x < c.x && p1.x < c.x )
				continue;

			int y = p0.y + ::MulDiv( p1.y - p0.y, c.x - p0.x, p1.x - p0.x );
			if( p0.y > y && p1.y > y || p0.y < y && p1.y < y || bLT >= 0 && ( bLT ? y > c.y : y < c.y ))
				continue;

			int t = abs( y - c.y );
			if( bFar ? t > d : t < d )
			{
				d = t;
				fp.y = y;
				nPoint = i % pPoly->x;
			}
		}
	}
	return fp;
}
//=====================================================================
void
RegionDef::SubtractInner( RgnPool& Inner )
{
  // селекция внутренних
	RgnPool tmp;
	RegionDef* pr1;
	for( RegionDef** ppr1 = Inner.GetHeadPtr() ; pr1 = *ppr1 ;  )
  {
		if( !pr1->pPoly->y && IsAllPointsInPolygon( pr1->pPoly + 1, pr1->pPoly->x, pPoly + 1, pPoly->x, 0 ))
		{
			*ppr1 = pr1->pNext;
      tmp.AddRgn( pr1 );
			Inner.DecSize();
		}
		else
			ppr1 = &pr1->pNext;
	}

	int bHorz = Width() < Height();
	*tmp.GetHeadPtr() = SortRegionDef( *tmp.GetHeadPtr(), tmp.Size(), bHorz );

	for(;;)
  {
    // текущая дырка
    RegionDef** ppr1 = tmp.GetHeadPtr();
    RegionDef* pr1 = *ppr1;
		if( !pr1 )
			break;

    CPoint* pp1 = pr1->pPoly + 1;
    int n1 = pr1->pPoly->x;

    if( nCw == pr1->nCw )
    {
      ReversePoly( pp1, n1 );
      pr1->nCw = -pr1->nCw;
    }
    
    // ищем ближнюю точку на внешнем контуре
    int pn0, pn2, d;
    CPoint p0( FindCross( pr1->InnerPoint, bHorz, 0, -1, pn0 )), p2;

    int bLT = bHorz ? p0.x < pr1->InnerPoint.x : p0.y < pr1->InnerPoint.y;

    // проверяем остальные дырки на пересечение и заменяем, если надо
		for( RegionDef** ppr2 = &pr1->pNext ;; ppr2 = &(*ppr2)->pNext )
    {
      RegionDef* pr2 = *ppr2;
      if( !pr2 || ( bHorz ? pr2->top > pr1->InnerPoint.y : pr2->left > pr1->InnerPoint.x ))
        break;
 
			p2 = pr2->FindCross( pr1->InnerPoint, bHorz, 0, bLT, pn2 );
      if( pn2 >= 0 && ( bHorz ? p2.x > pr1->InnerPoint.x == p0.x > pr1->InnerPoint.x :
                                p2.y > pr1->InnerPoint.y == p0.y > pr1->InnerPoint.y ))
			{
        int tn0;
        CPoint t0( FindCross( pr2->InnerPoint, bHorz, 0, bLT, tn0 ));
				if( tn0 >= 0 )
				{
  			  ppr1 = ppr2;
          pr1 = pr2;
          n1 = pr1->pPoly->x;
          pp1 = pr1->pPoly + 1;
					p0 = t0;
					pn0 = tn0;
				}
			}
    }

    p2 = pr1->FindCross( pr1->InnerPoint, bHorz, 1, bLT, pn2 );
    if( pn2 < 0 )
    {
			*ppr1 = pr1->pNext;
      Inner.AddRgn( pr1 );
      continue;
    }
    
		// внешний контур
		int n0 = pPoly->x;
  	CPoint* pp0 = pPoly + 1;

    // врезаем дырку
    int ns = n0 + n1 + 4;
    CPoint* pp = new CPoint[ns+1];
    CPoint* cp = pp;
    cp->y = 0;
    cp->x = ns;
    ++cp;
    *cp++ = p0;
    *cp++ = p2;
    d = n1 - pn2 - 1;
    memcpy( cp, &pp1[pn2+1], d * sizeof( CPoint ));
    cp += d;
    d = pn2 + 1;
    memcpy( cp, pp1, d * sizeof( CPoint ));
    cp += d;
    *cp++ = p2;
    *cp++ = p0;
    d = n0 - pn0 - 1;
    memcpy( cp, &pp0[pn0+1], d * sizeof( CPoint ));
    cp += d;
    d = pn0 + 1;
    memcpy( cp, pp0, d * sizeof( CPoint ));

  	*ppr1 = pr1->pNext;
    delete pPoly;
    delete pr1;
    pPoly = pp;
  }
}
//=====================================================================
