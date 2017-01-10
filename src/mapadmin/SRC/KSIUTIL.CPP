#include "stdafx.h"

#include "ksiutil.h"
#include "ksi_type.h"
#include "mview.h"
#include "mdoc.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "miscmode.h"
#include "drawobj.h"
#include "dwin.h"
#include "mathutil.h"
#include "util.h"
#include "bldmode.h"
#include "mercator.h"
//========================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
double const Pi2 = 2 * 3.14159265358979323846;
//=====================================================================
int
IsObject( int type )
{
  return KSI_TRUE_P( ksi_assv_ref( ksi_eval_str( "object-list" ), KSI_MK_SINT( type )));
}
//=====================================================================
int
IsAbstractObject( int type )
{
  return KSI_TRUE_P( ksi_assv_ref( ksi_eval_str( "abstract-param-list" ), KSI_MK_SINT( type )));
}
//=====================================================================
int
IsConcretObject( int type )
{
  return KSI_TRUE_P( ksi_assv_ref( ksi_eval_str( "concret-param-list" ), KSI_MK_SINT( type )));
}
//=====================================================================
int
IsUserObject( int type )
{
	ksi_obj ksi_typ = KSI_MK_SINT( type );
  return KSI_TRUE_P( ksi_assv_ref( ksi_eval_str( "abstract-param-list" ), ksi_typ )) ||
		     KSI_TRUE_P( ksi_assv_ref( ksi_eval_str( "concret-param-list" ), ksi_typ ));
}
//=====================================================================
ksi_obj
ksi_get_shape_num()
{
  return KSI_MK_SINT( pView->GetShapeNum());
}
//=====================================================================
ksi_obj
cons_with_sym( char const* sym, int val )
{
  return ksi_cons( ksi_str02sym( sym ), ksi_int2num( val ));
}
//=====================================================================
ksi_obj
cons_with_sym( char const* sym, char const* val )
{
  return ksi_cons( ksi_str02sym( sym ), ksi_str02string( val ));
}
//=====================================================================
ksi_obj
cons_with_sym( char const* sym, CPoint* pts, int np )
{
  return ksi_cons( ksi_str02sym( sym ), KsiPoints( pts, np ));
}
//=====================================================================
ksi_obj
translate_color( COLORREF color )
{
  char c[8];
  UINT R = color & 0xff;
  color >>= 8;
  UINT G = color &
  0xff; color >>= 8;
  UINT B = color & 0xff;
  wsprintf( c, "#%02x%02x%02x", R, G, B );
  return ksi_str02string( c );
}
//=====================================================================
COLORREF
translate_color( ksi_obj str )
{
  int r = 0, g = 0, b = 0;
  if( KSI_STR_P( str ))
  {
    char* ptr = KSI_STR_PTR( str );
    DWORD color = 0x8000000;
    if( *ptr++ == '#' )
    {
      int i = strlen( ptr );
      if( i >= 3 && i <= 12 && i % 3 == 0 )
      {
        i /= 3;
        char fmt[16];
        sprintf( fmt, "%%%dx%%%dx%%%dx", i, i, i );
        sscanf( ptr, fmt, &r, &g, &b );
        i = ( i - 2 ) << 2;
        if( i < 0 )
          r <<= -i, g <<= -i, b <<= -i;
        else
        if( i > 0 )
          r >>= i, g >>= i, b >>= i;
      }
    }
  }
  return RGB( r, g, b );
}
//=====================================================================
ksi_obj
ksi_get_reisshina_angle()
{
  return ksi_int2num( pView->GetMode( IDM_REISSHINA_MODE ) ? pView->GetCrossAngle() : 0 );
}
//=====================================================================
ksi_obj
ksi_set_reisshina_active( ksi_obj active )
{
  CReisMode::SetActive( KSI_TRUE_P( active ));
  return ksi_unspec;
}
//=====================================================================
ksi_obj
ksi_set_reisshina_prev_point( ksi_obj loc )
{
  CReisMode::SetPrevPoint( CPoint( ksi_num2int( KSI_CAR( loc )),
                           ksi_num2int( KSI_CDR( loc )) ));
  return ksi_unspec;
}
//=====================================================================
ksi_obj
ksi_put_hint( ksi_obj text )
{
  char* sp = KSI_STR_PTR( text );
  if( KSI_STR_P( text ))
    ReportHint( KSI_STR_PTR( text ));
  return ksi_unspec;
}
//=====================================================================
ksi_obj
ksi_get_bottom_list( ksi_obj ksi_type )
{
  int types[CLASS_RANGE];
  int n = pDocument->GetObjDef()->GetBottom( ksi_num2int( ksi_type ), types );
  ksi_obj ksiList = n ? ksi_nil : KSI_LIST1( ksi_type );
  while( --n >= 0 )
    ksiList = ksi_cons( KSI_MK_SINT( types[n] ), ksiList );
  return ksiList;
}
//=====================================================================
ksi_obj
ksi_is_derived_from_ex( ksi_obj ksi_type1, ksi_obj ksi_type2 )
{
  int t1 = ksi_num2int( ksi_type1 );
  int t2 = ksi_num2int( ksi_type2 );
  return KSI_MK_BOOL( t1 == t2 || pDocument->GetObjDef()->IsDerivedFrom( t1, t2 ));
}
//=====================================================================
ksi_obj
ksi_get_all_of_class( ksi_obj ksi_point, ksi_obj ksi_class_list )
{
  CWaitCursor wc;
  int bInPoint = KSI_PAIR_P( ksi_point );
  CSpot spot;
  if( bInPoint )
  {
    spot = pView->GetSpot( CppPoint( ksi_point ));
    if( !pView->m_Clip.PtInRect( spot ))
    {
      CRect r0( 0, 0, 0, 0 );
      pView->LoadFromStore( &pView->m_ClipData, spot, r0, 0, r0, 0 );
    }
  }

  ksi_obj objList = ksi_nil;

  if( KSI_NUM_P( ksi_class_list ))
     ksi_class_list = KSI_LIST1( ksi_class_list );

  while( KSI_PLIST_P( ksi_class_list ))
  {
    int t = ksi_num2int( KSI_CAR( ksi_class_list ));
    ksi_class_list = KSI_CDR( ksi_class_list );
    int types[CLASS_RANGE];
    int n = pView->m_pObjDef->GetBottom( t, types );
    while( --n >= 0 )
    {
      int type = types[n];
      CLayer* pLayer = pView->GetLayer( type );
      if( pLayer )
        for( CDrawObject* pObj = pLayer->SetFirst() ; pObj = pLayer->FindNext() ; )
          if( !bInPoint || pObj->Detected( spot ))
          {
            ksi_obj ksi_img = *pObj;
            objList = ksi_cons( KsiCall( "unwrap", ksi_int2num( pObj->Class()),
                                                   ksi_int2num( pObj->Status()),
                                                   ksi_img, 0 ),
                                objList );
          }
    }
  }
  return ksi_memq_remove( ksi_false, objList );
}
//=========================================================================
ksi_obj
ksi_num2fontface( ksi_obj ksi_fontnum )
{
  return ksi_str02string( pDocument->num2fontface( ksi_num2int( ksi_fontnum )));
}
//=========================================================================
int
GetTargets( UINT t, int* dst )
{
  int n = 0;
  if( t < CLASS_RANGE )
  {
    ksi_obj tl = KsiCall( "get-brife-target-class-list", KSI_MK_SINT( t ), 0 );
    for( ; KSI_PLIST_P( tl ) ; tl = KSI_CDR( tl ))
      if( KSI_NUM_P( KSI_CAR( tl )))
        if( dst )
          dst[n++] = ksi_num2int( KSI_CAR( tl ));
        else
          return 1;
  }
  return n;
}
//=====================================================================
ksi_obj
KsiCall( char const* proc, ... )
{
  va_list vptr;
  va_start( vptr, proc );
  ksi_obj pl = ksi_nil;
  for( ksi_obj p ; p = va_arg( vptr, ksi_obj ) ; pl = ksi_cons( p, pl ));
  va_end( vptr );

  pl = ksi_apply_with_catch( ksi_eval_str( proc ), ksi_reverse( pl ));
  if( KSI_EXN_P( pl ))
  {
    char* s = ksi_aprintf( "%s\n%s",
                           KSI_STR_PTR( KSI_EXN_MSG( pl )),
                           ksi_obj2str( KSI_EXN_VAL( pl )));
    ErrBox( s );
    return ksi_false;
  }
  return pl;
}
//=====================================================================
CPoint
CppPoint( ksi_obj x, ksi_obj y )
{
  ASSERT( KSI_NUM_P( x ) && KSI_NUM_P( y ));
  return CPoint( ksi_num2int( x ), ksi_num2int( y ));
}
//=====================================================================
CPoint
CppPoint( ksi_obj pnt )
{
  ASSERT( KSI_PAIR_P( pnt ) && KSI_NUM_P( KSI_CAR( pnt )) && KSI_NUM_P( KSI_CDR( pnt )));
  return CPoint( ksi_num2int( KSI_CAR( pnt )), ksi_num2int( KSI_CDR( pnt )));
}
//=====================================================================
ksi_obj
KsiPoint( int x, int y )
{
  return ksi_cons( ksi_int2num( x ), ksi_int2num( y ));
}
//=====================================================================
ksi_obj
KsiPoint( CPoint pnt )
{
  return ksi_cons( ksi_int2num( pnt.x ), ksi_int2num( pnt.y ));
}
//=====================================================================
ksi_obj
KsiPoints( CPoint* pPnt, int nPnt )
{
	ksi_obj ksi_lst;
  for( ksi_lst = ksi_nil ;
       --nPnt >= 0 ;
       ksi_lst = ksi_cons( KsiPoint( pPnt[nPnt] ), ksi_lst ));
  return ksi_lst;
}
//=====================================================================
CPoint*
CppPoints( ksi_obj pnt_lst, CPoint* pPnt, int nPnt )
{
  for( CPoint* p = pPnt ; --nPnt >= 0 && KSI_PLIST_P( pnt_lst ) ; pnt_lst = KSI_CDR( pnt_lst ))
	{
		ksi_obj pnt = KSI_CAR( pnt_lst );
    *p++ = CPoint( ksi_num2int( KSI_CAR( pnt )), ksi_num2int( KSI_CDR( pnt )));
	}
  ASSERT( nPnt < 0 );
  return nPnt < 0 ? pPnt : 0;
}
//=====================================================================
ksi_obj
ksi_draw_diff( ksi_obj ksi_new_pl )
{
  static CPoint* old_pl = 0;

  CBuildMode* pBM = pView->GetBuilding( 0 );
  if( pBM && pBM->IsDragging())
    return ksi_unspec;

  CPoint* new_pl = 0;

  if( KSI_PLIST_P( ksi_new_pl ))
  {
    int ep = ksi_num2int( KSI_CAR( ksi_new_pl ));
    ksi_new_pl = KSI_CDR( ksi_new_pl );
    int n = KSI_LIST_LEN( ksi_new_pl );
    new_pl = (CPoint*) new CPoint[n+1];
    new_pl->x = n;
    new_pl->y = ep > 0 ? ep : n;
    CppPoints( ksi_new_pl, new_pl + 1, n );
  }

  CViewDC dc( pView );
  CRect cr( pBM->GetClipRect());
  if( !cr.IsRectNull())
    dc.IntersectClipRect( cr );

  CSize bp0( -15, 0 ), bp1( -12, 0 ), s0( -3, -3 ), s1( 5, 5 );
  dc.DPtoLP( &bp0 );
  dc.DPtoLP( &bp1 );
  dc.DPtoLP( &s0 );
  dc.DPtoLP( &s1 );
  CRect r( s0.cx, s0.cy, s1.cx, s1.cy );

  dc.SetROP2( R2_NOTXORPEN );
  COLORREF sc = CItem::GetSpecColor( 0, 1 );
  CPen pen( PS_SOLID, 0, sc );
  CBrush brh( sc );

  for( int s = KSI_LIST_P( ksi_new_pl ) ? 2 : 0 ; --s >= 0 ; )
  {
    CPoint* pl0 = s ? new_pl : old_pl;
    if( !pl0 )
      continue;

    CPoint* pl1 = s ? old_pl : new_pl;
    int n0 = pl0->x - 1;
    int ep = pl0++->y - 1;
    int n1 = pl1 ? pl1++->x - 1 : 0;
    int bc = pl0[0] == pl0[n0];
    ep += ep == n0 ? bc ? -n0 : -1 : bc ?   1 :  0;

    CPoint t0( pl0[ep] );
    CPoint t1( pl0[ep+1] );

    if( t0 != t1 )
    {
      int a = atan2( t1 - t0 );
      CPoint arr[] = { t1,
                       t1 + TurnPoint( bp0, a - 200 ),
                       t1 + TurnPoint( bp1, a ),
                       t1 + TurnPoint( bp0, a + 200 ) };
      dc.SelectObject( &brh );
      dc.SelectStockObject( NULL_PEN );
      dc.Polygon( arr, CELEM( arr ));
    }

    dc.SelectStockObject( BLACK_BRUSH );

    if( !bc && ep != n0 - 1 || n0 == 0 )
      dc.Ellipse( pl0[n0] + r );

    while( --n0 >= 0 )
    {
      CPoint* tp1 = pl1;
	  int k1;
      for( k1 = n1 ; --k1 >= 0 && ( pl0[0] != tp1[0] || pl0[1] != tp1[1] ); ++tp1 );
      if( k1 >= 0 )
        for( ++pl0, ++tp1 ; n0 > 0 && k1 > 0 && pl0[1] == tp1[1] ; ++pl0, ++tp1, --n0, --k1 );
      else
      if( ++pl0, pl0[-1] != pl0[0] )
      {
        dc.SelectStockObject( NULL_PEN );
        dc.Ellipse( pl0[-1] + r );
        dc.SelectObject( &pen );
        dc.MoveTo( pl0[-1] );
        dc.LineTo( pl0[0] );
      }
    }
  }

  delete old_pl;
  old_pl = new_pl;

  return ksi_unspec;
}
//=====================================================================
ksi_obj
ksi_draw_icon( ksi_obj ksi_icon_ref, ksi_obj ksi_coord, ksi_obj ksi_cf )
{
  if( !KSI_REAL_P( ksi_cf ) ||
      !KSI_EINT_P( ksi_icon_ref ) ||
      !KSI_PAIR_P( ksi_coord ) ||
      !KSI_EINT_P( KSI_CAR( ksi_coord )) ||
      !KSI_EINT_P( KSI_CDR( ksi_coord )))
    return ksi_unspec;

  double cf = ksi_num2double( ksi_cf );
  int scale = pView->GetAltScale() / cf;

  int ir = ksi_num2int( ksi_icon_ref );
  CBmpDef* pBD = pDocument->GetBmpDef( ir );
  if( pBD )
  {
    CViewDC DC( pView );
    CMemDC memDC( &DC );

    CPoint c( CppPoint( ksi_coord ));
    CBox bb( ir, c, cf );
    CSize bs( bb.Size());
    CSize b2( bs.cx / 2, bs.cy / 2 );
    CSize bmps( bs );
    DC.LPtoDP( &bmps );

    CBitmap bmp;
    bmp.CreateCompatibleBitmap( &memDC, bmps.cx, bmps.cy );
    memDC.SelectObject( &bmp );
    memDC.PatBlt( 0, 0, bb.Width(), bb.Height(), WHITENESS );
    pBD->Draw( &memDC, b2, scale );
    DC.BitBlt( bb.left, bb.top, bb.Width(), bb.Height(), &memDC, 0, 0, 0x00990066 );
  }
  return ksi_unspec;
}
//=====================================================================
void
SetKsiHiResolutionX( int v )
{
  ksi_defsym( "hi-resolution?", KSI_MK_BOOL( v ), ksi_current_env());
}
//=====================================================================
ksi_obj
ksi_len2string( ksi_obj ksi_len )
{
  CString s( Length2String( ksi_num2int( ksi_len ), 1 ));
  return ksi_str02string( s );
}
//=====================================================================
ksi_obj
ksi_set_cursor( ksi_obj pnt )
{
  if( KSI_PAIR_P( pnt ) && KSI_NUM_P( KSI_CAR( pnt )) && KSI_NUM_P( KSI_CDR( pnt )))
    pView->SetPosition( CppPoint( pnt ));
  return pnt;
}
//=====================================================================
ksi_obj
ksi_set_text_cursor( ksi_obj ksi_size, ksi_obj ksi_pos, ksi_obj ksi_ang )
{
  CBuildMode* pBM = pView->GetBuilding( 0 );
  if( pBM )
    pBM->SetTextCursor(
      ksi_num2int( ksi_size ),
      CPoint( ksi_num2int( KSI_CAR( ksi_pos )), ksi_num2int( KSI_CDR( ksi_pos ))),
      ksi_num2int( ksi_ang ));
  return ksi_unspec;
}
//=====================================================================
ksi_obj
ksi_kill_text_cursor()
{
  CBuildMode* pBM = pView->GetBuilding( 0 );
  if( pBM )
    pBM->SetTextCursor( 0 );
  return ksi_unspec;
}
//=====================================================================
int
OutputKsiObject( ksi_obj obj, ksi_obj port, int len )
{
  static ksi_obj ksi_lp = KSI_MK_CHAR( '(' );
  static ksi_obj ksi_rp = KSI_MK_CHAR( ')' );
  static ksi_obj ksi_sp = KSI_MK_CHAR( ' ' );
  if( KSI_PLIST_P( obj ))
  {
    ksi_display( ksi_lp, port );
    ++len;
    while( !KSI_NULL_P( obj ))
    {
      len = OutputKsiObject( KSI_CAR( obj ), port, len );
      obj = KSI_CDR( obj );
      if( !KSI_NULL_P( obj ))
      {
        ksi_display( ksi_sp, port  );
        ++len;
      }
    }
    ksi_display( ksi_rp, port  );
    ++len;
  }
  else
  {
    int dl = KSI_STR_LEN( ksi_object2string( obj ));
    if( len > 0 && len + dl >= 72 )
    {
      ksi_newline( port );
      len = 0;
    }
    ksi_write( obj, port );
    len += dl;
  }

  return len;
}
//=====================================================================
static int
ArePointsNear( CPoint p0, CPoint p1 )
{
  CSize d( p0 - p1 );
  return abs( d.cx ) <= 1 && abs( d.cy ) <= 1;
}
//=====================================================================
ksi_obj
ksi_remove_selfcross( ksi_obj rl )
{
  if( KSI_LIST_LEN( rl ) >= 1024 )
    return rl;

  ksi_obj ll = KSI_LIST1( KSI_CAR( ksi_last_pair( rl )));

  CPoint p[2] = { CppPoint( KSI_CAR( ksi_last_pair( ll ))) };
  while( KSI_LIST_LEN( rl ) >= 3 )
  {
    p[1] = p[0];
    p[0] = CppPoint( KSI_CAR( rl ));
    ksi_obj fl = KSI_CDR( rl );
    CPoint t[2] = { CppPoint( KSI_CAR( fl )) };
    while( KSI_LIST_LEN( fl ) > 1 )
    {
      t[1] = t[0];
      t[0] = CppPoint( KSI_CAR( KSI_CDR( fl )));
      CPoint cp;
      if( TrueCrossPoint( p, t, &cp ) &&
          !ArePointsNear( p[0], cp ) &&
          !ArePointsNear( t[0], cp ) &&
          !ArePointsNear( p[1], cp ) &&
          !ArePointsNear( t[1], cp ))
      {
        ksi_obj tl = ksi_cons( KsiPoint( cp ), KSI_CDR( fl ));
        ksi_set_cdr_x( fl, ksi_nil );
        rl = ksi_cons( KsiPoint( cp ), ksi_reverse( rl ));
        ksi_set_cdr_x( ksi_last_pair( rl ), tl );
        fl = tl;
        p[0] = t[0] = cp;
      }
      else
        fl = KSI_CDR( fl );
    }
    ksi_set_cdr_x( ksi_last_pair( ll ), KSI_LIST1( KSI_CAR( rl )));
    rl = KSI_CDR( rl );
  }

  ksi_set_cdr_x( ksi_last_pair( ll ), rl );
  return KSI_CDR( ll );
}
//=====================================================================
ksi_obj
ksi_error_handler( int argc, ksi_obj* argv )
{
  char const* msg = "Unspecified error";
  UINT res;

  if( argc >= 1 )
    if( KSI_STR_P( argv[0] ))
      msg = KSI_STR_PTR( argv[0] );
    else
    if( KSI_EXN_P( argv[0] ))
      msg = ksi_aprintf( "%s: %s", KSI_STR_PTR( KSI_EXN_MSG( argv[0] )),
                         ksi_obj2str( KSI_EXN_VAL( argv[0] )));

  res = MessageBox( 0, msg, "Ksi Error", MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OKCANCEL );

  if( res == IDCANCEL )
    ksi_exit( ksi_false );

  return ksi_false;
}
//=====================================================================
ksi_obj
ksi_calc_string_width( ksi_obj str, ksi_obj font, ksi_obj angle, ksi_obj do_list )
{
  int bDoList = do_list && KSI_TRUE_P( do_list );

  if( !KSI_STR_P( str ))
    return bDoList ? KSI_LIST1( ksi_zero ) : ksi_zero;

  ASSERT( KSI_VEC_P( font ));
  ASSERT( KSI_EXACT_P( angle ));

  int a = ksi_num2int( angle );

  LOGFONT Log = { 0, 0, a, a, 0, 0, 0, 0, RUSSIAN_CHARSET, OUT_TT_PRECIS,
                  CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH };

  if( KSI_VEC_P( font ))
  {
    ksi_obj ksi_fn = KSI_VEC_REF( font, 0 );
    ASSERT( KSI_EXACT_P( ksi_fn ));
    if( KSI_EXACT_P( ksi_fn ))
      strcpy( Log.lfFaceName, pDocument->num2fontface( ksi_num2int( ksi_fn )));
    Log.lfHeight = ksi_num2int( KSI_VEC_REF( font, 1 ));
    if( KSI_VEC_LEN( font ) > 2 )
      Log.lfWeight = CFontItem::m_WeightKey->KsiTranslate( KSI_VEC_REF( font, 2 ));
    if( KSI_VEC_LEN( font ) > 3 )
    {
      UINT style = CFontItem::m_StileKey->Translate( KSI_STR_PTR( KSI_VEC_REF( font, 3 )));
      Log.lfItalic    = ( style & 1 ) ? 1 : 0;
      Log.lfUnderline = ( style & 2 ) ? 1 : 0;
    }
  }

  int height = Log.lfHeight;
  int h = abs( height ), dy = MulDiv( h, 5, 4 );
  int M = 4096;

  if( h < M )
    M = h;
  else
    Log.lfHeight = height > 0 ? M : -M;

  CFont fnt;
  fnt.CreateFontIndirect( &Log );
  CClientDC dc( 0 );
  dc.SelectObject( &fnt );

  char* sp = KSI_STR_PTR( str );
  int sl = KSI_STR_LEN( str );

  if( bDoList )
  {
	  ksi_obj ksi_lst;
    for( ksi_lst = ksi_nil ; sl > 0 ; --sl )
    {
      CSize ext( dc.GetOutputTextExtent( sp, sl ));
      ksi_lst = ksi_cons( ksi_int2num( MulDiv( ext.cx, h, M )), ksi_lst );
    }
    return ksi_cons( ksi_zero, ksi_lst );
  }

  CSize ext( dc.GetOutputTextExtent( sp, sl ));
  return ksi_int2num( MulDiv( ext.cx, h, M ));
}
//=====================================================================
static ksi_obj
ksi_remove_loops( ksi_obj rl )
{
  CPoint t0[2], t1[2], cp;
  ksi_obj l0 = rl;
  while( KSI_LIST_LEN( l0 ) >= 4 )
  {
    t0[0] = CppPoint( KSI_CAR( l0 ));
    t0[1] = CppPoint( KSI_CAR( KSI_CDR( l0 )));
    ksi_obj l1 = KSI_CDR( KSI_CDR( l0 ));
    while( KSI_LIST_LEN( l1 ) >= 2 )
    {
      t1[0] = CppPoint( KSI_CAR( l1 ));
      t1[1] = CppPoint( KSI_CAR( KSI_CDR( l1 )));
      l1 = KSI_CDR( l1 );
      if( CrossPoint( t0, t1, &cp ))
        ksi_set_cdr_x( l0, ksi_cons( KsiPoint( t0[1] = cp ), l1 ));
    }
    l0 = KSI_CDR( l0 );
  }
  return rl;
}
//=====================================================================
static int
DistToLine( CPoint t0, CPoint p0, CPoint p1 )
{
  t0 -= p0;
  p1 -= p0;
  CPoint t( TurnPoint( t0, p1.x, p1.y, hipo( p1.x, p1.y )));
  return t.y;
}
//=====================================================================
static int
NearestPointNum( CPoint p, CPoint* pl, int np )
{
  int d = -1, n = -1;
  for( int i = 0 ; --np >= 0 ; ++i  )
  {
    int t = dist( *pl++, p );
    if( d < 0 || t < d )
    {
      d = t;
      n = i;
    }
  }
  return n;
}
//=====================================================================
static CPoint
Clcp1( CPoint p0, CPoint p1, int d )
{
  CSize s( p1 - p0 );
  CSize t( TurnPoint( CPoint( 0, d ), s.cx, -s.cy, hipo( s.cx, s.cy )));
  return p0 + t;
}
//=====================================================================
static CPoint
Clcp2( CPoint p0, CPoint p1, int d )
{
  CSize s( p1 - p0 );
  CSize t( TurnPoint( CPoint( 0, d ), s.cx, -s.cy, hipo( s.cx, s.cy )));
  return p1 + t;
}
//=====================================================================
static ksi_obj
Clcp3( CPoint p0, CPoint p1, CPoint p2, int d )
{
  CPoint p00( Clcp1( p0, p1, d ));
  CPoint p01( Clcp1( p1, p0, -d ));
  CPoint p10( Clcp1( p1, p2, d ));
  CPoint p11( Clcp1( p2, p1, -d ));
  CPoint cp;
  int bc = CrossPoint( p00, p01,p10, p11, &cp, 0 );
  return KSI_LIST1( KsiPoint( bc ? cp : p01 ));
}
//=====================================================================
inline int
NDX( int n, int m )
{
  return n < 0 ? n + m : n < m ? n : n - m;
}
//=====================================================================
ksi_obj
ksi_mk_trace( ksi_obj kp0, ksi_obj kp1 )
{
  if( KSI_TRUE_P( ksi_eval_str( "is-convertion?" )))
    return ksi_false;

  if( pView->GetShapeNum())
    return ksi_false;

  CDrawObject* pTob = pView ->GetSelectObject( 0 );
  if( !pTob )
    return ksi_false;

  int np, sh;
  CPoint* pp = pTob->GetAnyPoints( &sh, &np );
  int bBound = sh == SH_POLYGON || sh == SH_BPOLYLINE;

  if( !pp || np < 2 )
    return ksi_false;

  CPoint p0( CppPoint( kp0 ));
  CPoint p1( CppPoint( kp1 ));
  int td = pView->m_Deviation;
  int sb = max( td, pView->m_Sensibility );

  int st = NearestPointNum( p0, pp, np );
  int fn = NearestPointNum( p1, pp, np );
  int step = 1;

  if( st < 0 || fn < 0 || st == fn )
    return ksi_false;

  if( bBound )
  {
    int d0 = NDX( fn - st, np ), d1 = np - d0;
    if( d1 < d0 )
      step = -1;
  }
  else
  if( st > fn )
    step = -1;

  int s0 = DistToLine( p0, pp[st], pp[NDX(st+step,np)] );
  if( abs( s0 ) > sb )
    return ksi_false;

  int s1 = DistToLine( p1, pp[fn], pp[NDX(fn-step,np)] );
  if( abs( s1 ) > sb )
    return ksi_false;

  int n = NDX( step > 0 ? fn - st : st - fn, np );

  if( !td )
  {
    ksi_obj rl = KSI_LIST1( KsiPoint( pp[fn] ));
    while( --n >= 0 )
      rl = ksi_cons( KsiPoint( pp[fn=NDX(fn-step,np)] ), rl );	
    return rl;
  }

  int bPrev = bBound || ( step > 0 ? st > 0 : st < np - 1 );
  int bNext = bBound || ( step > 0 ? fn < np - 1 : fn > 0 );
  CPoint mpp( pp[bPrev?NDX(st-step,np):0] );

  CPoint mp0( pp[st] );
  CPoint mp1( pp[NDX(st+=step,np)] );
  CPoint mp2( pp[NDX(st+=step,np)] );
  int d0 = DistToLine( p0, mp0, mp1 );
  int d = d0 > 0 ? td : -td;
  ksi_obj rl = bPrev ? Clcp3( mpp, mp0, mp1, d ) : KSI_LIST1( KsiPoint( Clcp1( mp0, mp1, d ))	);
  for( int i = 1 ; i <= n ; ++i  )
  {
    ksi_obj fl = i < n || bNext ? Clcp3( mp0, mp1, mp2, d ) :
                                  KSI_LIST1( KsiPoint( Clcp2( mp0, mp1, d )));
    ksi_set_cdr_x( ksi_last_pair( rl ), fl );
    mp0 = mp1;
    mp1 = mp2;
    mp2 = pp[NDX(st+=step,np)];
  }
  return ksi_remove_loops( rl );
}
//=====================================================================
static int
calc_n_pnt( double rad, double rng )
{
  static long* pd = &pView->m_MaxDelta;
  return *pd > rad ? 2 : fabs( rng ) / ::acos( 1. - *pd /rad );
}
//=====================================================================
ksi_obj
ksi_mk_ring( ksi_obj ksi_pl, ksi_obj ksj_spc )
{
  ASSERT( KSI_LIST_P( ksi_pl ));
  if( KSI_LIST_LEN( ksi_pl ) < 2 )
    return ksi_pl;

  CPoint p[2];
  CppPoints( ksi_pl, p, CELEM( p ));

  if( p[0] == p[1] )
    return ksi_pl;

  p[1] -= p[0];
  int a0 = atan2( p[1] );
  double r = hipo( p[1] );
  int n = KSI_NUM_P( ksj_spc ) ? ksi_num2int( ksj_spc ) : max( 8, calc_n_pnt( r, Pi2 ));
  if( KSI_BOOL_P( ksj_spc ) && KSI_TRUE_P( ksj_spc ))
    n = ( n & ~3 ) - 2;

  ksi_pl = ksi_nil;
  for( int i = 0 ; i <= n ; )
  {
    int a = a0 + MulDiv( ++i, i3600, n );
    ksi_pl = ksi_cons( KsiPoint( p[0].x + r * cos( a ), p[0].y - r * sin( a )), ksi_pl );
  }

  return ksi_pl;
}
//=====================================================================
ksi_obj
ksi_mk_sota( ksi_obj ksi_pl )
{
  return ksi_mk_ring( ksi_pl, KSI_MK_SINT( 6 ));
}
//=====================================================================
static ksi_obj
ksi_mk_ovel( ksi_obj ksi_pl, int bEllipse )
{
  ASSERT( KSI_LIST_P( ksi_pl ));
  if( KSI_LIST_LEN( ksi_pl ) < 3 )
    return ksi_mk_ring( ksi_pl, ksi_false );

  CPoint p[3];
  CppPoints( ksi_pl, p, CELEM( p ));

  p[1] -= p[0];
  p[2] -= p[0];
  int a0 = pView->GetModeAngle();
  double a = hipo( p[1] );
  double b = hipo( p[2] ) * cos( atan2( p[2] ) - a0 );
  double k = bEllipse ? b / a : max( b - a, 0 );
  int n = ( max( 8, calc_n_pnt( a, Pi2 )) & ~3 ) - 2;
  int n0 = n / 4, n1 = n - n0;

  ksi_pl = ksi_nil;
  for( int i = 0 ; i <= n ; )
  {
    int da = MulDiv( ++i, i3600, n );
    CPoint t( a * cos( da ), -a * sin( da ));
    if( bEllipse )
      t.x *= k;
    else
      t.x += i <= n0 || i >= n1 ? k : -k;
    ksi_pl = ksi_cons( KsiPoint( p[0] + TurnPoint( t, a0 )), ksi_pl );
  }
  return ksi_pl;
}
//=====================================================================
ksi_obj
ksi_mk_ellipse( ksi_obj ksi_pl )
{
  return ksi_mk_ovel( ksi_pl, 1 );
}
//=====================================================================
ksi_obj
ksi_mk_oval( ksi_obj ksi_pl )
{
  return ksi_mk_ovel( ksi_pl, 0 );
}
//=====================================================================
static double
Polynom( double* pcf, double arg )
{
  double p = 0;
  for( int i = 4 ; --i >= 0 ; p = p * arg + pcf[i] );
  return p;
}
//=====================================================================
static ksi_obj
InsertPoint( double arg0, double arg1,
             double ax[], double ay[],
             ksi_obj lst, int bDirIns )
{
  double arg = .5 * ( arg0 + arg1 );
  CPoint p( Polynom( ax, arg ), Polynom( ay, arg ));
  if( bDirIns ||
      !IsPointOnLine( p, CppPoint( KSI_CAR( lst )),
                      CppPoint( KSI_CAR( KSI_CDR( lst ))),
                      pView->m_MaxDelta ))
  {
    ksi_set_cdr_x( lst, ksi_cons( KsiPoint( p ), KSI_CDR( lst )));
    InsertPoint( arg, arg1, ax, ay, KSI_CDR( lst ), 0 );
    InsertPoint( arg0, arg, ax, ay, lst, 0 );
  }
  return lst;
}
//=====================================================================
static ksi_obj
Spline( CPoint p[4], double h, double s )
{
  double x0h = ( p[0].x + 4. * p[1].x + p[2].x ) / 6.;
  double x0s = p[1].x;
  double x1h = ( p[2].x - p[0].x ) * .5;
  double x1s = ( -2. * p[0].x - 3. * p[1].x + 6. * p[2].x - p[3].x ) / 6.;
  double ax[] =
  {
    h * x0h + s * x0s,
    h * x1h + s * x1s,
    ( p[0].x - 2. * p[1].x + p[2].x ) * .5,
    ( -p[0].x + 3. * p[1].x - 3. * p[2].x + p[3].x ) / 6.
  };

  double y0h = ( p[0].y + 4. * p[1].y + p[2].y ) / 6.;
  double y0s = p[1].y;
  double y1h = ( p[2].y - p[0].y ) * .5;
  double y1s = ( -2. * p[0].y - 3. * p[1].y + 6. * p[2].y - p[3].y ) / 6.;
  double ay[] =
  {
    h * y0h + s * y0s,
    h * y1h + s * y1s,\
    ( p[0].y - 2. * p[1].y + p[2].y ) * .5,
    ( -p[0].y + 3. * p[1].y - 3. * p[2].y + p[3].y ) / 6.
  };

  ksi_obj p0 = KsiPoint( Polynom( ax, 0 ), Polynom( ay, 0 ));
  ksi_obj p1 = KsiPoint( Polynom( ax, 1 ), Polynom( ay, 1 ));

  return InsertPoint( 0., 1., ax, ay, KSI_LIST2( p0, p1 ), 1 );
}
//=====================================================================
ksi_obj
ksi_mk_spline( ksi_obj ksi_pl, ksi_obj ksi_h )
{
  if( KSI_LIST_LEN( ksi_pl ) < 3 )
    return ksi_pl;

  double h = ksi_num2double( ksi_h );
  double s = 1. - h;
  ksi_obj ksi_rpl = KSI_LIST1( KSI_CAR( ksi_pl ));
  CPoint p[4];
  p[2] = p[1] = CppPoint( KSI_CAR( ksi_pl ));
  ksi_pl = KSI_CDR( ksi_pl );
  p[3] = CppPoint( KSI_CAR( ksi_pl ));
  ksi_pl = KSI_CDR( ksi_pl );
  for( int bMore = 1 ; bMore ; )
  {
    p[0] = p[1];
    p[1] = p[2];
    p[2] = p[3];
    if( bMore = !KSI_NULL_P( ksi_pl ))
    {
      p[3] = CppPoint( KSI_CAR( ksi_pl ));
      ksi_pl = KSI_CDR( ksi_pl );
    }
    ksi_set_cdr_x( ksi_last_pair( ksi_rpl ), Spline( p, h, s ));
  }
  ksi_set_cdr_x( ksi_last_pair( ksi_rpl ), KSI_LIST1( KsiPoint( p[3] )));
  return ksi_rpl;
}
//=====================================================================
int
calc_arc_param( ksi_obj ksi_pl, double* pPar )
{
  if( KSI_LIST_LEN( ksi_pl ) < 3 )
    return 0;

  CPoint p[3];
  CppPoints( ksi_pl, p, CELEM( p ));
  double dy1 = p[0].x - p[2].x;
  double dx1 = p[2].y - p[0].y;
  double dy2 = p[1].x - p[2].x;
  double dx2 = p[2].y - p[1].y;
  double d = dy1 * dx2 - dx1 * dy2;

  if( d == 0 )
    return -1;

  double x1 = .5 * ( p[0].x + p[2].x );
  double y1 = .5 * ( p[0].y + p[2].y );
  double x2 = .5 * ( p[1].x + p[2].x );
  double y2 = .5 * ( p[1].y + p[2].y );
  double d1 =  dx2 * ( y2 - y1 ) - dy2 * ( x2 - x1 );
  double k = d1 / d;
  double x0 = x1 + k * dx1;
  double y0 = y1 + k * dy1;

  double qx0 = p[0].x - x0;
  double qy0 = p[0].y - y0;
  double a0 = atan2( -qy0, qx0 );

  double qx1 = p[1].x - x0;
  double qy1 = p[1].y - y0;
  double a1 = atan2( -qy1, qx1 );

  double qx2 = p[2].x - x0;
  double qy2 = p[2].y - y0;
  double a2 = atan2( -qy2, qx2 );
  double dg = a1 < a0 && a0 < a2 || a2 < a1 && a1 < a0 ?  Pi2:
              a0 < a1 && a1 < a2 || a2 < a0 && a0 < a1 ? -Pi2 :
              0;
  double sp = a1 - a0 + dg;

  double r = sqrt( qx0 * qx0 + qy0 * qy0 );

  pPar[0] = x0;
  pPar[1] = y0;
  pPar[2] = r;
  pPar[3] = a0;
  pPar[4] = sp;
  return 1;
}
//=====================================================================
ksi_obj
calc_arc_param( ksi_obj ksi_pl )
{
  double par[5];
  if( !calc_arc_param( ksi_pl, par ))
    return ksi_false;

  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 5 ), ksi_false );
  for( int i = 5 ; --i >= 0 ; )
  {
    int p = i >= 3 ? Rad2Deg( par[i] ) : par[i];
    KSI_VEC_REF( vec, i ) = ksi_int2num( p );
  }
  return vec;
}
//=====================================================================
ksi_obj
ksi_mk_arc( ksi_obj ksi_pl )
{
  double par[5];
  switch( calc_arc_param( ksi_pl, par ))
  {
    case  0 : return ksi_pl;
    case -1 : return ksi_list_head( ksi_pl, ksi_two );
  }
  double x = par[0];
  double y = par[1];
  double r = par[2];
  double a = par[3];
  double s = par[4];

  int n = calc_n_pnt( r, s );
  a += s;
  s /= n;
  ksi_obj ksi_rl = KSI_LIST1( KSI_CAR( KSI_CDR( ksi_pl )));
  for( int i = n ; --i > 0 ; )
  {
    a -= s;
    ksi_rl = ksi_cons( KsiPoint( x + r * ::cos( a ), y - r * ::sin( a )), ksi_rl );
  }
  return ksi_cons( KSI_CAR( ksi_pl ), ksi_rl );
}
//=====================================================================
ksi_obj
ksi_true_dist( ksi_obj ksi_p0, ksi_obj ksi_p1 )
{
	Point p0( ksi_num2int( KSI_CAR( ksi_p0 )), ksi_num2int( KSI_CDR( ksi_p0 )));
	Point p1( ksi_num2int( KSI_CAR( ksi_p1 )), ksi_num2int( KSI_CDR( ksi_p1 )));
  int d = MerkatorData.Dist( p0, p1 );
  return ksi_int2num( d );
}
//=====================================================================
ksi_obj
ksi_find_inner_point( ksi_obj pnt_lst )
{
  ASSERT( KSI_PLIST_P( pnt_lst ));
  int nPnt = KSI_LIST_LEN( pnt_lst );
  ASSERT( nPnt >= 3 );
  CPoint pPnt[4096];
  CppPoints( pnt_lst, pPnt, min( nPnt, CELEM( pPnt )));
  return KsiPoint( FindInnerPoint( pPnt, nPnt ));
}
//=====================================================================
ksi_obj
ksi_test_point_object( ksi_obj ksi_pt, ksi_obj ksi_bms )
{
  ASSERT( KSI_PAIR_P( ksi_pt ));
  ASSERT( KSI_EINT_P( KSI_CAR( ksi_pt )));
  ASSERT( KSI_EINT_P( KSI_CDR( ksi_pt )));
	int r = pView->TestPointObject( CppPoint( ksi_pt ));
	if( r && KSI_TRUE_P( ksi_bms ))
		ErrBox( "Эта точка уже занята" );
	return KSI_MK_BOOL( r );
}
//=====================================================================
