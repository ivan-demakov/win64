#include "stdafx.h"

#include "mview.h"
#include "rectmode.h"
#include "mathutil.h"
#include "util.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//==== CRectMode ======================================================
void 
CRectMode::Draw( CDC* pDC, int bCentered )
{
  CRect rect( *this );
  if( bCentered )
  {
    rect.left -= Size().cx;
    rect.top  -= Size().cy;
  }
  rect = pView->LP2DP( rect );
  CPen pen( PS_SOLID, 2, RGB( 0x80, 0x80, 0x80 ));
  int idc = SetTM( pDC );
  pDC->SelectStockObject( NULL_BRUSH );
  pDC->SetBkMode( TRANSPARENT );
  pDC->SelectObject( &pen );
  pDC->SetROP2( R2_XORPEN );
  pDC->Rectangle( rect );
  pDC->RestoreDC( idc );
}
//=====================================================================
void 
CRectMode::DrawA( CDC* pDC, int angle )
{
  CPoint* poly = Rect2Poly( *this, angle );
  for( int i = 5 ; --i >= 0 ; poly[i] = pView->LP2DP( poly[i] ));
  CPen pen( PS_SOLID, 3, RGB( 0x80, 0x80, 0x80 ));
  int idc = SetTM( pDC );
  pDC->SelectStockObject( NULL_BRUSH );
  pDC->SetBkMode( TRANSPARENT );
  pDC->SelectObject( &pen );
  pDC->SetROP2( R2_XORPEN );
  pDC->Polyline( poly, 5 );
  pDC->RestoreDC( idc );
}
//=====================================================================
