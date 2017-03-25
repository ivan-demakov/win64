#include "stdafx.h"

#include <string.h>
#include <math.h>
#include "ksi.h"
#include "ksi_type.h"
#include "ksikey.h"
#include "ident.h"
#include "cspot.h"
#include "mdoc.h"
#include "drawitem.h"
#include "mview.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "ksiutil.h"
#include "mathutil.h"
#include "scale.h"
#include "util.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CIntAssocMap* CItem::m_pCurColorTab;
CIntAssocMap* CItem::m_pCurUserColorTab;
COLORREF      CItem::m_SelectColor[] =
{
  0,
  RGB( 255, 0, 0 ),
  RGB( 0, 255, 0 ),
  RGB( 0, 0, 255 ),
  RGB( 255, 255, 0 ),
  RGB( 0, 255, 255 ),
  RGB( 255, 0, 255 ),
  RGB( 127, 0, 0 )
};
//=====================================================================
static int
Invisible( ksi_obj init_list )
{
  ksi_obj l = ksi_assv_ref( init_list, keyLVis );
  ksi_obj h = ksi_assv_ref( init_list, keyHVis );
  return KSI_FALSE_P( h ) && KSI_FALSE_P( l );
}
//=====================================================================
COLORREF
CItem::GetColor( int num )
{
  return ( IsUserObject( CDrawObject::GetDrawnClass()) ? m_pCurUserColorTab : m_pCurColorTab )->TranslateKey( num );
}
//=====================================================================
COLORREF
CItem::GetSpecColor( int num, WORD flags )
{
  flags &= SF_STATUS_MASK;
  CDrawObject* pObj = CDrawObject::GetDrawnObject();
  return pObj && pObj->IsSpecSelected() ? pObj->GetSpecSelectColor() :
         flags ? m_SelectColor[flags] :
         GetColor( num );
}
//=====================================================================
//=====================================================================
CColorItem::CColorItem( ksi_obj init_list )
{
  m_ColorNum = Invisible( init_list ) ?
    0 : ksi_num2int( ksi_assv_ref( init_list, keyColor ));
}
//=====================================================================
CColorItem::operator ksi_obj() const
{
  return ksi_cons( keyColor, ksi_int2num( m_ColorNum ));
}
//=====================================================================
//=====================================================================
CDeltaItem::CDeltaItem( ksi_obj init_list )
{
  m_Delta = Invisible( init_list ) ?
    0 : ksi_num2int( ksi_assv_ref( init_list, keyDelta ));
}
//=====================================================================
CDeltaItem::operator ksi_obj() const
{
  return ksi_cons( keyDelta, ksi_int2num( m_Delta ));
}
//=====================================================================
//=====================================================================
CPen   CStrokItem::m_Pen;
int   CStrokItem::m_bPenCreated = 0;
LOGPEN CStrokItem::m_OldLog;
//=====================================================================
CStrAssoc const CStrokItem::m_StyleKey[] =
{
  CStrAssoc( "solid",        PS_SOLID      ),
  CStrAssoc( "dash",         PS_DASH       ),
  CStrAssoc( "dot",          PS_DOT        ),
  CStrAssoc( "dash-dot",     PS_DASHDOT    ),
  CStrAssoc( "dash-dot-dot", PS_DASHDOTDOT ),
  CStrAssoc( 0,              PS_SOLID      )
};
//=====================================================================
CStrokItem::CStrokItem( ksi_obj init_list ):
  CColorItem( init_list ),
  m_Width( 0 ),
  m_Style( 0 )
{
//KsiCall( "tk-box", init_list, 0 );
  if( Invisible( init_list ))
    return;

  ksi_obj vec = ksi_assv_ref( init_list, keyStrok );
  if( KSI_FALSE_P( vec ))
    return;

  ASSERT( KSI_VEC_P( vec ));
#ifdef _DEBUG
  if( !KSI_VEC_P( vec ))
    KsiCall( "tk-box", init_list, vec, 0 );
#endif //_DEBUG
  if( !KSI_VEC_P( vec ))
    return;

  int l = KSI_VEC_LEN( vec );
  ASSERT( l == 2 );
  if( l != 2  )
    return;

  ksi_obj v0 = KSI_VEC_REF( vec, 0 );
  ksi_obj v1 = KSI_VEC_REF( vec, 1 );

  m_Style = KSI_NUM_P( v1 ) ? ksi_num2int( v1 ) : m_StyleKey->KsiTranslate( v1 );
  m_Width = 0;

  ASSERT( KSI_NUM_P( v0 ));
  if( !KSI_NUM_P( v0 ))
    return;

  if(( m_Style & 0xff ) <= PS_ALTERNATE )
    m_Width = ksi_num2int( v0 );
}
//=====================================================================
int
CStrokItem::TranslateToMap( TGstream* pDst )
{
  PenParam par;
  par.width = TemlateStyle() ? 0 : m_Width;
  par.style = m_Style;
  par.color = m_ColorNum;
  return WriteData( &par, sizeof par, pDst );
}
//=====================================================================
ksi_obj
CStrokItem::translate_style( DWORD style, int width )
{
  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 2 ), ksi_false );
  int ls = style & 0xff;
  KSI_VEC_REF( vec, 0 ) = ksi_int2num( width );
  KSI_VEC_REF( vec, 1 ) = ls > PS_ALTERNATE ?
    ksi_int2num( ls ) :
    m_StyleKey->KsiTranslate( ls );
  return vec;
}
//=====================================================================
CStrokItem::operator ksi_obj() const
{
  return KSI_CONS3( this->CColorItem::operator ksi_obj(),
                    keyStrok,
                    translate_style( m_Style, m_Width ));
}
//=====================================================================
int
CStrokItem::TemlateStyle()
{
  int ls = m_Style & 0xff;
  return ls > PS_ALTERNATE ? ls : 0;
}
//=====================================================================
void
CStrokItem::ScaleOn( double cf )
{
  m_Width = m_Width * cf + .5;
}
//=====================================================================
void
CStrokItem::DeletePen()
{
  if( m_bPenCreated )
    m_Pen.DeleteObject();
  m_bPenCreated = 0;
}
//=====================================================================
CPen*
CStrokItem::CreatePen( WORD flags ) const
{
  static UINT vm[] = { IDM_OBJEDIT_MODE, IDM_CONNECT_MODE, 0 };

  int bSel = flags & SF_STATUS_MASK;
  int S = m_Style & 0xff;
  LOGPEN log = { S > PS_ALTERNATE ? PS_SOLID : S,
                 pView->GetActiveMode( vm ) && bSel ? 0 : m_Width, 0,
                 GetSpecColor( m_ColorNum, flags ) };

  if( m_bPenCreated && !memcmp( &m_OldLog, &log, sizeof log ))
    return &m_Pen;

  DeletePen();
  m_OldLog = log;
  m_bPenCreated = m_Pen.CreatePenIndirect( &log );
  return &m_Pen;
}
//=====================================================================
//=====================================================================
CForeBackItem::CForeBackItem( ksi_obj init_list )
{
  ksi_obj vec = ksi_assv_ref( init_list, keyFBColor );
  ASSERT( KSI_VEC_P( vec ));
#ifdef _DEBUG
  if( !KSI_VEC_P( vec ))
    KsiCall( "tk-box", init_list, vec, 0 );
#endif //_DEBUG
  int inv = Invisible( init_list );
  if( KSI_VEC_P( vec ))
  {
    m_ForeNum = inv ? 0 : ksi_num2int( KSI_VEC_REF( vec, 0 ));
    m_BackNum = inv ? 0 : ksi_num2int( KSI_VEC_REF( vec, 1 ));
  }
}
//=====================================================================
CForeBackItem::operator ksi_obj() const
{
  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 2 ), ksi_false );
  KSI_VEC_REF( vec, 0 ) = ksi_int2num( m_ForeNum );
  KSI_VEC_REF( vec, 1 ) = ksi_int2num( m_BackNum );
  return ksi_cons( keyFBColor, vec );
}
//=====================================================================
//=====================================================================
CBrush   CFillItem::m_Brush;
int     CFillItem::m_bBrushCreated = 0;
LOGBRUSH CFillItem::m_OldLog;
//=====================================================================
CStrAssoc const CFillItem::m_FillKey[] =
{
  CStrAssoc( "solid", BS_SOLID   ),
  CStrAssoc( 0,       BS_HATCHED )
};
//=====================================================================
CStrAssoc const CFillItem::m_HatchKey[] =
{
  CStrAssoc( "horizontal", HS_HORIZONTAL ),
  CStrAssoc( "vertical",   HS_VERTICAL   ),
  CStrAssoc( "fdiagonal",  HS_FDIAGONAL  ),
  CStrAssoc( "bdiagonal",  HS_BDIAGONAL  ),
  CStrAssoc( "cross",      HS_CROSS      ),
  CStrAssoc( "diagcross",  HS_DIAGCROSS  ),
  CStrAssoc( 0,            HS_DIAGCROSS  )
};
//=====================================================================
CFillItem::CFillItem( ksi_obj init_list ):
  CForeBackItem( init_list )
{
  ksi_obj val = ksi_assv_ref( init_list, keyFill );
  int inv = Invisible( init_list );
  m_Fill  = inv ? 0 : m_FillKey->KsiTranslate( val );
  m_Hatch = inv ? 0 : m_HatchKey->KsiTranslate( val );
}
//=====================================================================
int
CFillItem::TranslateToMap( TGstream* pDst )
{
  BrushParam par;
  par.style = m_Fill;
  par.fill = 0;
  par.hatch = m_Hatch;
  par.fore  = m_ForeNum;
  par.back  = m_BackNum;
  return WriteData( &par, sizeof par, pDst );
}
//=====================================================================
CFillItem::operator ksi_obj() const
{
  ksi_obj fl = m_Fill == BS_SOLID ? m_FillKey->KsiTranslate( m_Fill ) :
                                    m_HatchKey->KsiTranslate( m_Hatch );
  return ksi_cons( ksi_cons( keyFill, fl ),
                   CForeBackItem::operator ksi_obj());
}
//=====================================================================
void
CFillItem::DeleteBrush()
{
  if( m_bBrushCreated )
    m_Brush.DeleteObject();
  m_bBrushCreated = 0;
}
//=====================================================================
CBrush*
CFillItem::CreateBrush( WORD flags ) const
{
  LOGBRUSH log = { m_Fill, GetSpecColor( m_ForeNum, flags ), m_Hatch };
  if( m_bBrushCreated && !memcmp( &m_OldLog, &log, sizeof log ))
    return &m_Brush;

  DeleteBrush();
  m_OldLog = log;
  m_bBrushCreated = m_Brush.CreateBrushIndirect( &log );
  return &m_Brush;
}
//=====================================================================
//=====================================================================
void
CCoordItem::ScaleOn( double cf, CPoint c )
{
  if( c.x >= 0 && c.y >= 0 )
	{
    x = c.x + cf * ( x - c.x ) + .5;
    y = c.y + cf * ( y - c.y ) + .5;
	}
}
//=====================================================================
CCoordItem::CCoordItem( ksi_obj init_list )
{
  ksi_obj vec = ksi_assv_ref( init_list, keyCoord );
  ASSERT( KSI_VEC_P( vec ));
#ifdef _DEBUG
  if( !KSI_VEC_P( vec ))
    KsiCall( "tk-box", init_list, vec, 0 );
#endif //_DEBUG
  if( KSI_VEC_P( vec ))
  {
    x = ksi_num2int( KSI_VEC_REF( vec, 0 ));
    y = ksi_num2int( KSI_VEC_REF( vec, 1 ));
  }
}
//=====================================================================
CCoordItem::operator ksi_obj() const
{
  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 2 ), ksi_false );
  KSI_VEC_REF( vec, 0 ) = ksi_int2num( x );
  KSI_VEC_REF( vec, 1 ) = ksi_int2num( y );
  return ksi_cons( keyCoord, vec );
}
//=====================================================================
//=====================================================================
CSizeItem::CSizeItem( ksi_obj init_list )
{
  ksi_obj vec = ksi_assv_ref( init_list, keySize );
  ASSERT( KSI_VEC_P( vec ));
#ifdef _DEBUG
  if( !KSI_VEC_P( vec ))
    KsiCall( "tk-box", init_list, vec, 0 );
#endif //_DEBUG
  if( KSI_VEC_P( vec ))
  {
    cx = ksi_num2int( KSI_VEC_REF( vec, 0 ));
    cy = ksi_num2int( KSI_VEC_REF( vec, 1 ));
    m_Angle = (( ksi_num2int( KSI_VEC_REF( vec, 2 )) % 3600 ) + 3600 ) % 3600;
  }
}
//=====================================================================
CSizeItem::operator ksi_obj() const
{
  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 3 ), ksi_false );
  KSI_VEC_REF( vec, 0 ) = ksi_int2num( cx );
  KSI_VEC_REF( vec, 1 ) = ksi_int2num( cy );
  KSI_VEC_REF( vec, 2 ) = KSI_MK_SINT( m_Angle );
  return ksi_cons( keySize, vec );
}
//=====================================================================
void
CSizeItem::ScaleOn( double cf )
{
  cx = cx * cf + .5;
  cy = cy * cf + .5;
}
//=====================================================================
//=====================================================================
CPolyItem::CPolyItem( ksi_obj init_list, int bBound ):
  m_bBound( bBound != 0 )
{
  ksi_obj list = ksi_assv_ref( init_list, keyPoly );
  ASSERT( KSI_LIST_P( list ));
#ifdef _DEBUG
  if( !KSI_LIST_P( list ))
    KsiCall( "tk-box", init_list, list, 0 );
#endif //_DEBUG
  if( KSI_LIST_P( list ))
  {
    m_PointNum = KSI_LIST_LEN( list );
    if( m_pPoints = m_PointNum ? new CPoint[m_PointNum+m_bBound] : 0 )
    {
      CPoint *pp;
      for( pp = m_pPoints ; list != ksi_nil ; list = KSI_CDR( list ))
        *pp++ = CppPoint( KSI_CAR( list ));
      if( bBound )
        *pp = *m_pPoints;
    }
  }
}
//=====================================================================
int
CPolyItem::TranslateToMap( TGstream* pDst )
{
  PolyParam par;
  par.length = m_PointNum;
  int r = WriteData( &par.length, sizeof par.length, pDst );
  return r == R_OK ? WriteData( m_pPoints, par.length * sizeof POINT, pDst ) : r;
}
//=====================================================================
void
CPolyItem::SetPoints( CPoint const* points )
{
  m_pPoints = ExtPointNum() ? new CPoint[ExtPointNum()] : 0;
  if( m_pPoints && points )
  {
    memcpy( m_pPoints, points, m_PointNum * sizeof CPoint );
    if( m_bBound )
      m_pPoints[m_PointNum] = m_pPoints[0];
  }
}
//=====================================================================
void
CPolyItem::SetPoints( int pn, CPoint const* points, int bBound )
{
  ASSERT( points );
  ASSERT( pn );
  m_PointNum = pn;
  m_bBound = bBound;
  delete m_pPoints;
  SetPoints( points );
}
//=====================================================================
CPolyItem::CPolyItem( int pn, CPoint const* points, int bBound ) :
  m_PointNum( pn ), m_bBound( bBound != 0 )
{
  SetPoints( points );
}
//=====================================================================
CPolyItem::CPolyItem( CPolyItem const& p ) :
  m_PointNum( p.m_PointNum ), m_bBound( p.m_bBound )
{
  SetPoints( p.m_pPoints );
}
//=====================================================================
CPolyItem::~CPolyItem ()
{
  delete m_pPoints;
}
//=====================================================================
CPolyItem::operator ksi_obj() const
{
  return ksi_cons( keyPoly, KsiPoints( m_pPoints, m_PointNum ));
}
//=====================================================================
void
CPolyItem::MoveOn( CSize s )
{
  if( m_pPoints )
  {
    CPoint* pp = m_pPoints;
    for( int n = ExtPointNum() ; --n >= 0 ; *pp++ += s );
  }
}
//=====================================================================
void
CPolyItem::SpinOn( CSize const& center, int angle )
{
  if( m_pPoints )
    for( CPoint *cp = m_pPoints + ( m_PointNum + m_bBound ) ;
         --cp >= m_pPoints ;
         *cp = TurnPoint( *cp, center, angle ));
}
//=====================================================================
void
CPolyItem::ScaleOn( double cf, CPoint c )
{
  if( m_pPoints && c.x >= 0 && c.y >= 0 )
    for( CPoint *cp = m_pPoints + ( m_PointNum + m_bBound ) ; --cp >= m_pPoints ; )
    {
			CSize s( *cp - c );
      cp->x = c.x + s.cx * cf + .5;
      cp->y = c.y + s.cy * cf + .5;
    }
}
//=====================================================================
using namespace Trig;

void
CPolyItem::DrawVerts( CDC* pDC, WORD flag )
{
  static CRect r( -2, -2, 4, 4 );
  static CPoint bp0( -15, 0 ), bp1( -12, 0 );

  if( !( flag & SF_STATUS_MASK ) ||
      !pView->GetBuilding( 0 ) &&
      !pView->GetMode( IDM_CONNECT_MODE ) &&
      !( pView->GetMode( IDM_OBJEDIT_MODE ) && pView->GetSelectObject( 0 )))
    return;

  CPoint lt( pDC->GetWindowOrg());
  if( lt.x || lt.y )
    lt = pView->LP2DP( lt );

  int idc = SetTM( pDC );
  pDC->SelectStockObject( BLACK_BRUSH );
  pDC->SelectStockObject( NULL_PEN );
  CPoint* pp = m_pPoints;
  for( int i = m_PointNum ; --i >= 0 ; pDC->Ellipse( r + pView->LP2DP( *pp++ ) - lt ));
  if( m_PointNum > 1 )
  {
    CPoint p( pView->LP2DP( pp[-1] ) - lt );
    int a = atan2( pp[-1] - pp[-2] );
    CPoint arr[] = { p, p, p, p };
    arr[0] += TurnPoint( bp0, a + 200 );
    arr[2] += TurnPoint( bp0, a - 200 );
    arr[3] += TurnPoint( bp1, a );
    CBrush brh( GetSpecColor( 0, flag ));
    pDC->SelectObject( brh );
    pDC->Polygon( arr, CELEM( arr ));
  }
  pDC->RestoreDC( idc );
}
//=====================================================================
int
CPolyItem::GetAngle( CSpot const &spot, int* pAngle )
{
  int n = spot.DetectPolyline( m_pPoints, m_PointNum + m_bBound, 0, 1 );
  if( n )
  {
    CPoint* p = m_pPoints + n;
    *pAngle = atan2( p[0] - p[-1] );
  }
  return n;
}
//=====================================================================
//=====================================================================
CStringItem::CStringItem( ksi_obj init_list ) :
  m_String( 0 )
{
  ksi_obj vec = ksi_assv_ref( init_list, keyString );
  ASSERT( KSI_VEC_P( vec ));
#ifdef _DEBUG
  if( !KSI_VEC_P( vec ))
    KsiCall( "tk-box", init_list, vec, 0 );
#endif //_DEBUG
  if( KSI_VEC_P( vec ))
  {
    m_Offset.cx = ksi_num2int( KSI_VEC_REF( vec, 0 ));
    m_Offset.cy = ksi_num2int( KSI_VEC_REF( vec, 1 ));
    m_Angle     = ksi_num2int( KSI_VEC_REF( vec, 2 )) % 3600;
    vec         = KSI_VEC_REF( vec, 3 );
    ASSERT( KSI_STR_P( vec ));
    if( KSI_STR_P( vec ))
      SetString( KSI_STR_PTR( vec ));
  }
}
//=====================================================================
CStringItem::~CStringItem()
{
  delete m_String;
}
//=====================================================================
void
CStringItem::SetString( char const* str, int slen )
{
  ASSERT( str );
  delete m_String;
  if( m_String = new char[slen+1] )
  {
    strncpy( m_String, str, slen );
    m_String[slen] = 0;
  }
}
//=====================================================================
CStringItem::operator ksi_obj() const
{
  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 4 ), ksi_false );
  KSI_VEC_REF( vec, 0 ) = ksi_int2num( m_Offset.cx );
  KSI_VEC_REF( vec, 1 ) = ksi_int2num( m_Offset.cy );
  KSI_VEC_REF( vec, 2 ) = KSI_MK_SINT( m_Angle );
  KSI_VEC_REF( vec, 3 ) = ksi_str02string( m_String );
  return ksi_cons( keyString, vec );
}
//=====================================================================
void
CStringItem::ScaleOn( double cf )
{
  m_Offset.cx = m_Offset.cx * cf + .5;
  m_Offset.cy = m_Offset.cy * cf + .5;
}
//=====================================================================
//=====================================================================
CFont CFontItem::m_Font;
int  CFontItem::m_bFontCreated = 0;
int CFontItem::m_OldFontNum;
int CFontItem::m_OldHeight;
int CFontItem::m_OldWeight;
int CFontItem::m_OldStyle;
int CFontItem::m_OldDir;
//=====================================================================
void
CFontItem::DeleteFont()
{
  if( m_bFontCreated )
    m_Font.DeleteObject();
  m_bFontCreated = 0;
}
//=====================================================================
CFont*
CFontItem::CreateFont( int dir )
{
  if( m_bFontCreated            &&
      m_OldFontNum == m_FontNum &&
      m_OldHeight  == m_Height  &&
      m_OldWeight  == m_Weight  &&
      m_OldStyle   == m_Style   &&
      m_OldDir     == dir )
   return &m_Font;

  DeleteFont();
  m_OldFontNum = m_FontNum;
  m_OldHeight  = m_Height;
  m_OldWeight  = m_Weight;
  m_OldStyle   = m_Style;
  m_OldDir     = dir;
  m_bFontCreated =
    m_Font.CreateFont( m_Height, 0, dir, dir, m_Weight,
                       m_Style & 1, m_Style & 2, 0, RUSSIAN_CHARSET,
                       OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                       PROOF_QUALITY, DEFAULT_PITCH,
                       pDocument->num2fontface( m_FontNum ));
  return m_bFontCreated ? &m_Font : 0;
}
//=====================================================================
CStrAssoc const CFontItem::m_WeightKey[] =
{
  CStrAssoc( "DONTCARE"  , FW_DONTCARE   ),
  CStrAssoc( "THIN"      , FW_THIN       ),
  CStrAssoc( "EXTRALIGHT", FW_EXTRALIGHT ),
  CStrAssoc( "LIGHT"     , FW_LIGHT      ),
  CStrAssoc( "NORMAL"    , FW_NORMAL     ),
  CStrAssoc( "MEDIUM"    , FW_MEDIUM     ),
  CStrAssoc( "SEMIBOLD"  , FW_SEMIBOLD   ),
  CStrAssoc( "BOLD"      , FW_BOLD       ),
  CStrAssoc( "EXTRABOLD" , FW_EXTRABOLD  ),
  CStrAssoc( "HEAVY"     , FW_HEAVY      ),
  CStrAssoc( "ULTRALIGHT", FW_EXTRALIGHT ),
  CStrAssoc( "REGULAR"   , FW_NORMAL     ),
  CStrAssoc( "DEMIBOLD"  , FW_SEMIBOLD   ),
  CStrAssoc( "ULTRABOLD" , FW_EXTRABOLD  ),
  CStrAssoc( "BLACK"     , FW_HEAVY      ),
  CStrAssoc( 0           , FW_DONTCARE   )
};
//=====================================================================
CStrAssoc const CFontItem::m_StileKey[] =
{
  CStrAssoc( ""  ,  0x0 ),
  CStrAssoc( "I"  , 0x1 ),
  CStrAssoc( "U"  , 0x2 ),
  CStrAssoc( "IU" , 0x3 ),
  CStrAssoc( "UI" , 0x3 ),
  CStrAssoc( 0    , 0x0 )
};
//=====================================================================
CFontItem::CFontItem( ksi_obj init_list ) :
  m_FontNum( 0 ),
  m_Height( 0 ),
  m_Weight( FW_DONTCARE ),
  m_Style( 0 )
{
  ksi_obj vec = ksi_assv_ref( init_list, keyFont );

  ASSERT( KSI_VEC_P( vec ));
#ifdef _DEBUG
  if( !KSI_VEC_P( vec ))
    KsiCall( "tk-box", init_list, vec, 0 );
#endif //_DEBUG
  if( KSI_VEC_P( vec ))
  {
    ksi_obj dff = KSI_VEC_REF( vec, 0 );
    ksi_obj dfs = KSI_VEC_REF( vec, 1 );

    m_FontNum = ksi_num2int( KSI_NUM_P( dff ) ? dff : ksi_zero );

    ASSERT( KSI_NUM_P( dfs ));
    if( KSI_NUM_P( dfs ))
      m_Height = ksi_num2int( dfs );

    if( KSI_VEC_LEN( vec ) > 2 && KSI_STR_P( KSI_VEC_REF( vec, 2 )))
      m_Weight = m_WeightKey->KsiTranslate( KSI_VEC_REF( vec, 2 ));

    if( KSI_VEC_LEN( vec ) > 3 && KSI_STR_P( KSI_VEC_REF( vec, 3 )))
      m_Style = m_StileKey->Translate( KSI_STR_PTR( KSI_VEC_REF( vec, 3 )));
  }
}
//=====================================================================
CFontItem::operator ksi_obj() const
{
  int bSw = m_Weight > FW_NORMAL;
  int bSt = m_Style != 0;
  int nv = 2 + ( bSt ? 2 : bSw ? 1 : 0 );
  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( nv ), ksi_false );
  KSI_VEC_REF( vec, 0 ) = KSI_MK_SINT( m_FontNum );
  KSI_VEC_REF( vec, 1 ) = KSI_MK_SINT( m_Height );
  if( nv > 2 )
    KSI_VEC_REF( vec, 2 ) = ksi_str02string( m_WeightKey->Translate( m_Weight ));
  if( nv > 3 )
    KSI_VEC_REF( vec, 3 ) = ksi_str02string( m_StileKey->Translate( m_Style ));
  return ksi_cons( keyFont, vec );
}
//=====================================================================
void
CFontItem::ScaleOn( double cf )
{
  int sg = m_Height >= 0 ? 1 : - 1;
  m_Height = sg * ( abs( m_Height ) * cf + .5 );
}
//=====================================================================
//=====================================================================
CRefItem::CRefItem( ksi_obj init_list )
{
  m_Reference = ksi_num2int( ksi_assv_ref( init_list, keyReference ));
}
//=====================================================================
CRefItem::operator ksi_obj() const
{
  return ksi_cons( keyReference, ksi_int2num( m_Reference ));
}
//=====================================================================
