//=====================================================================
#ifndef _KSIUTIL_H
#define _KSIUTIL_H
//=====================================================================
#include "ksi.h"
#include "objdef.h"
//=====================================================================
//#define MAX_LEN 72
//=====================================================================
ksi_obj ksi_put_hint( ksi_obj text );
ksi_obj ksi_get_reisshina_angle();
ksi_obj ksi_set_reisshina_active( ksi_obj active );
ksi_obj ksi_set_reisshina_prev_point( ksi_obj loc );
ksi_obj ksi_get_all_of_class( ksi_obj ksi_type, ksi_obj ksi_class_list );
ksi_obj ksi_get_bottom_list( ksi_obj ksi_type );
ksi_obj ksi_is_derived_from_ex( ksi_obj ksi_type1, ksi_obj ksi_type2 );
ksi_obj ksi_num2fontface( ksi_obj ksi_fontnum );
ksi_obj translate_color( COLORREF color );
COLORREF translate_color( ksi_obj str );
ksi_obj cons_with_sym( char const* sym, int val );
ksi_obj cons_with_sym( char const* sym, char const* val );
ksi_obj cons_with_sym( char const* sym, CPoint* pts, int np );
int GetTargets( UINT t, int* dst );
int IsObject( int type );
int IsAbstractObject( int type );
int IsConcretObject( int type );
int IsUserObject( int type );
ksi_obj KsiCall( char const* proc, ... );
ksi_obj ksi_get_shape_num();

CPoint CppPoint( ksi_obj pnt );
CPoint CppPoint( ksi_obj x, ksi_obj y );
CPoint* CppPoints( ksi_obj pnt_lst, CPoint* pPnt, int nPnt );
ksi_obj KsiPoint( CPoint pnt );
ksi_obj KsiPoint( int x, int y );
ksi_obj KsiPoints( CPoint* pPnt, int nPnt );

ksi_obj ksi_draw_verts( ksi_obj ksi_pll );
ksi_obj ksi_draw_diff( ksi_obj ksi_pl1 );
ksi_obj ksi_draw_icon( ksi_obj ksi_icon_ref, ksi_obj ksi_coord, ksi_obj ksi_cf );
void SetKsiHiResolutionX( int v );
ksi_obj ksi_len2string( ksi_obj ksi_len );
ksi_obj ksi_set_cursor( ksi_obj ksi_pnt );
ksi_obj ksi_set_text_cursor( ksi_obj ksi_size, ksi_obj ksi_pos, ksi_obj ksi_ang );
ksi_obj ksi_kill_text_cursor();
int OutputKsiObject( ksi_obj obj, ksi_obj port, int len = 0 );
ksi_obj ksi_remove_selfcross( ksi_obj ksi_list );
ksi_obj ksi_error_handler( int argc, ksi_obj* argv );
ksi_obj ksi_calc_string_width( ksi_obj str, ksi_obj font, ksi_obj angle, ksi_obj do_list );
ksi_obj ksi_mk_trace( ksi_obj kp0, ksi_obj kp1 );
ksi_obj ksi_mk_ring( ksi_obj ksi_pl, ksi_obj ksj_spc );
ksi_obj ksi_mk_ellipse( ksi_obj ksi_pl );
ksi_obj ksi_mk_oval( ksi_obj ksi_pl );
ksi_obj ksi_mk_spline( ksi_obj ksi_pl, ksi_obj ksi_h );
ksi_obj ksi_mk_arc( ksi_obj ksi_pl );
ksi_obj calc_arc_param( ksi_obj ksi_pl );
ksi_obj ksi_mk_sota( ksi_obj ksi_pl );
ksi_obj ksi_true_dist( ksi_obj ksi_p0, ksi_obj ksi_p1 );
ksi_obj ksi_find_inner_point( ksi_obj pnt_lst );
ksi_obj ksi_test_point_object( ksi_obj ksi_pt, ksi_obj ksi_bms );
//=====================================================================
#endif