#include "stdafx.h"

#include <crtdbg.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "filestr.h"
#include "gpl.h"
#include "msutil.h"
#ifndef _CLIENT_APP
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
double const Pi = 3.14159265358979323846;
double const d1800 = 1800.0;
//=====================================================================
double
Deg2Rad( int angle )
{
  return angle * Pi / d1800;
}
//=====================================================================
#else
 #include "ksikey.h"
 #include "ksiutil.h"
 #include "util.h"
 #include "ksi_type.h"
 #include "assoc.h"
 #include "mathutil.h"
 #include "ksiutil.h"
//=====================================================================
#ifdef _FULL_PROJECT
enum
{
  STROKED        = 1,
  FILLED         = 2,
  ROUND          = 4,
  OUTLINED       = STROKED | FILLED,
  ROUND_STROKED  = ROUND | STROKED,
  ROUND_FILLED   = ROUND | FILLED,
  ROUND_OUTLINED = ROUND | OUTLINED
};
#endif
//=====================================================================
static const CStrAssoc FillStyle[] =
{
  CStrAssoc( "solid", 0 ),
  CStrAssoc( 0,       2 )
};
//=====================================================================
static const CStrAssoc HatchStyle[] =
{
  CStrAssoc( "horizontal", 0 ),
  CStrAssoc( "vertical",   1 ),
  CStrAssoc( "fdiagonal",  2 ),
  CStrAssoc( "bdiagonal",  3 ),
  CStrAssoc( "cross",      4 ),
  CStrAssoc( "diagcross",  5 ),
  CStrAssoc( 0,            5 )
};
//=====================================================================
static const CStrAssoc LineStyle[] =
{
  CStrAssoc( "solid",        0 ),
  CStrAssoc( "dash",         1 ),
  CStrAssoc( "dot",          2 ),
  CStrAssoc( "dash-dot",     3 ),
  CStrAssoc( "dash-dot-dot", 4 ),
  CStrAssoc( 0,              1 )
};
//=====================================================================
const CStrAssoc PrimKeys[] =
{
  CStrAssoc( "container",          PF_CONTAINER ),
  CStrAssoc( "point",              PF_POINT ),
  CStrAssoc( "icon",               PF_ICON ),
  CStrAssoc( "text",               PF_TEXT ),
  CStrAssoc( "text-by-line",       PF_TEXTBYLINE ),

  CStrAssoc( "polyline",           PF_POLYPOINT | PF_STROKED ),
  CStrAssoc( "footnote",           PF_FOOTNOTE  | PF_STROKED ),
  CStrAssoc( "bound-polyline",     PF_POLYPOINT | PF_BOUND | PF_STROKED ),
  CStrAssoc( "polygon",            PF_POLYPOINT | PF_FILLED ),
  CStrAssoc( "outlined-polygon",   PF_POLYPOINT | PF_FILLED | PF_BOUND | PF_STROKED ),
  CStrAssoc( "smart-polygon",      PF_POLYPOINT | PF_FILLED | PF_BOUND | PF_STROKED | PF_SMART ),

  CStrAssoc( "lines",              PF_MULTILINE | PF_STROKED ),

  CStrAssoc( "rectangle",          PF_RECTANGLE | PF_STROKED ),
  CStrAssoc( "bar",                PF_RECTANGLE | PF_FILLED ),
  CStrAssoc( "outlined-bar",       PF_RECTANGLE | PF_FILLED | PF_STROKED ),

  CStrAssoc( "ellipse",            PF_ELLIPSE | PF_STROKED ),
  CStrAssoc( "oval",               PF_ELLIPSE | PF_FILLED ),
  CStrAssoc( "outlined-oval",      PF_ELLIPSE | PF_FILLED | PF_STROKED ),

  CStrAssoc( "ring",               PF_ELLIPSE | PF_ROUND | PF_STROKED ),
  CStrAssoc( "circle",             PF_ELLIPSE | PF_ROUND | PF_FILLED ),
  CStrAssoc( "outlined-circle",    PF_ELLIPSE | PF_ROUND | PF_FILLED | PF_STROKED ),

  CStrAssoc( "arc",                PF_ARC | PF_STROKED ),
  CStrAssoc( "pie",                PF_ARC | PF_FILLED ),
  CStrAssoc( "chord",              PF_ARC | PF_FILLED | PF_CHORD ),
  CStrAssoc( "outlined-chord",     PF_ARC | PF_FILLED | PF_STROKED | PF_CHORD ),

  CStrAssoc( 0,                    PF_NOTHING )
};
//=====================================================================
static const CStrAssoc WeightKeys[] =
{
  CStrAssoc( "DONTCARE"  ,   0 ),
  CStrAssoc( "THIN"      , 100 ),
  CStrAssoc( "EXTRALIGHT", 200 ),
  CStrAssoc( "ULTRALIGHT", 200 ),
  CStrAssoc( "LIGHT"     , 300 ),
  CStrAssoc( "NORMAL"    , 400 ),
  CStrAssoc( "REGULAR"   , 400 ),
  CStrAssoc( "MEDIUM"    , 500 ),
  CStrAssoc( "SEMIBOLD"  , 600 ),
  CStrAssoc( "DEMIBOLD"  , 600 ),
  CStrAssoc( "BOLD"      , 700 ),
  CStrAssoc( "EXTRABOLD" , 800 ),
  CStrAssoc( "ULTRABOLD" , 800 ),
  CStrAssoc( "HEAVY"     , 900 ),
  CStrAssoc( "BLACK"     , 900 ),
  CStrAssoc( 0           , 400 )
};
//=====================================================================
static const CStrAssoc StyleKeys[] =
{
  CStrAssoc( ""   , 0x0 ),
  CStrAssoc( "I"  , 0x1 ),
  CStrAssoc( "U"  , 0x2 ),
  CStrAssoc( "IU" , 0x3 ),
  CStrAssoc( "UI" , 0x3 ),
  CStrAssoc( 0    , 0x0 )
};
//=====================================================================
static int
IsNumVect( ksi_obj vec, int sz, int nn = 0 )
{
  ASSERT( nn <= sz );
  for( int r = KSI_VEC_P( vec ) && KSI_VEC_LEN( vec ) >= sz ; r && --nn >= 0 ;
       r = KSI_NUM_P( KSI_VEC_REF( vec, nn )));
//  ASSERT( r );
  return r;
}
//=====================================================================
static int
IsNumPair( ksi_obj pair )
{
  return KSI_PAIR_P( pair ) && KSI_NUM_P( KSI_CAR( pair )) && KSI_NUM_P( KSI_CDR( pair ));
}
//=====================================================================
static Point
TurnPoint( int x, int y, double co, double si )
{
  return Point(  x * co + y * si, y * co - x * si );
}
//=====================================================================
ResultCode
ConvertObject( int vers, ksi_obj init_obj, TGstream& dst, int level )
{
  if( !KSI_PLIST_P( init_obj ))
    return R_CONVERTERROR;

//  KsiCall( "tk-box", init_obj, 0 );

  PrimRecord rec;
  rec.type = PrimKeys->KsiTranslate( KSI_CAR( init_obj ));

  init_obj = KSI_CDR( init_obj );

  rec.type |= ( KSI_TRUE_P( ksi_assv_ref( init_obj, keyShape )) ? PF_SHAPE    : 0 ) |
              ( KSI_TRUE_P( ksi_assv_ref( init_obj, keyLVis  )) ? PF_LVISIBLE : 0 ) |
              ( KSI_TRUE_P( ksi_assv_ref( init_obj, keyHVis  )) ? PF_HVISIBLE : 0 ) |
              ( KSI_TRUE_P( ksi_assv_ref( init_obj, keySVis  )) ? PF_SVISIBLE : 0 );

  int bVisible = rec.type & PF_VISIBLE;

  ResultCode r = WriteData( &rec.type, sizeof rec.type, &dst );
  if( r != R_OK )
    return r;

  if( bVisible && ( rec.type & PF_STROKED ))
  {
    PenParam par;
    ksi_obj pv = ksi_assv_ref( init_obj, keyStrok );
    if( !IsNumVect( pv, 2, 1 ) ||
        !KSI_NUM_P( KSI_VEC_REF( pv, 1 )) && !KSI_KEY_P( KSI_VEC_REF( pv, 1 )))
      return R_CONVERTERROR;
    par.width = ksi_num2int( KSI_VEC_REF( pv, 0 ));
    par.style = KSI_NUM_P( KSI_VEC_REF( pv, 1 )) ?
      ksi_num2int( KSI_VEC_REF( pv, 1 )) :
      LineStyle->Translate( KSI_KEY_PTR( KSI_VEC_REF( pv, 1 )));
    ksi_obj cl = ksi_assv_ref( init_obj, keyColor );
    if( !KSI_EXACT_P( cl ))
      return R_CONVERTERROR;
    par.color = ksi_num2int( cl );
    r = WriteData( &par, sizeof par, &dst );
    if( r != R_OK )
      return r;
  }

  if( bVisible && ( rec.type & PF_FILLED ))
  {
    BrushParam par;
    ksi_obj fl = ksi_assv_ref( init_obj, keyFill );
    par.style = FillStyle->Translate( KSI_KEY_PTR( fl ));
    par.hatch = HatchStyle->Translate( KSI_KEY_PTR( fl ));
    par.fill  = 0;

    ksi_obj fb = ksi_assv_ref( init_obj, keyFBColor );
    if( !IsNumVect( fb, 2, 2 ))
      return R_CONVERTERROR;

    par.fore = ksi_num2int( KSI_VEC_REF( fb, 0 ));
    par.back = ksi_num2int( KSI_VEC_REF( fb, 1 ));

    r = WriteData( &par, sizeof par, &dst );
    if( r != R_OK )
      return r;
  }

  switch( rec.type & PF_TYPE_MASK )
  {
    case PF_CONTAINER :
    {
      if( !KSI_PLIST_P( init_obj ))
        return R_CONVERTERROR;
      init_obj = KSI_CAR( init_obj );
      Container par;
      par.length = KSI_LIST_LEN( init_obj );
      if( par.length == 0 )
        return R_CONVERTERROR;
      r = WriteData( &par.length, sizeof par.length, &dst );
      for( int i = par.length ; --i >= 0 && r == R_OK ;
           r = ConvertObject( vers, KSI_CAR( init_obj ), dst, level + 1 ),
           init_obj = KSI_CDR( init_obj ));
      break;
    }
    case PF_POINT :
    {
      ksi_obj pt = ksi_assv_ref( init_obj, keyCoord );
      if( !IsNumVect( pt, 2, 2 ))
        return R_CONVERTERROR;
      point par;
      par.x = ksi_num2int( KSI_VEC_REF( pt, 0 ));
      par.y = ksi_num2int( KSI_VEC_REF( pt, 1 ));
      r = WriteData( &par, sizeof par, &dst );
      break;
    }
    case PF_ICON :
    {
      ksi_obj pt = ksi_assv_ref( init_obj, keyCoord );
      ksi_obj id = ksi_assv_ref( init_obj, keyReference );
      if( !IsNumVect( pt, 2, 2 ) || !KSI_EXACT_P( id ))
        return R_CONVERTERROR;
      Icon par;
      par.id = ksi_num2int( id );
      par.x = ksi_num2int( KSI_VEC_REF( pt, 0 ));
      par.y = ksi_num2int( KSI_VEC_REF( pt, 1 ));
      r = WriteData( &par, sizeof par, &dst );
      break;
    }
    case PF_TEXT :
    {
      ksi_obj pt = ksi_assv_ref( init_obj, keyCoord );
      if( !IsNumVect( pt, 2, 2 ))
        return R_CONVERTERROR;
      Text par;
      par.org.x = ksi_num2int( KSI_VEC_REF( pt, 0 ));
      par.org.y = ksi_num2int( KSI_VEC_REF( pt, 1 ));

      StringParam* pSP = bVisible ? &par.param.full.string : &par.param.string;

      ksi_obj st = ksi_assv_ref( init_obj, keyString );
      if( !IsNumVect( st, 4, 3 ) || !KSI_STR_P( KSI_VEC_REF( st, 3 )))
        return R_CONVERTERROR;

      char* pb = KSI_STR_PTR( KSI_VEC_REF( st, 3 ));
      char* pe = pb + KSI_STR_LEN( KSI_VEC_REF( st, 3 ));
      if( bVisible && level == 1 )
      {
        while( pb < pe && *pb == ' ' )
          ++pb;
        while( pb < pe && pe[-1] == ' ' )
          --pe;
        if( pb == pe )
				{
					ksi_vector_set_x( st, ksi_three, ksi_ptr02string( "." ));
//          return R_CONVERTERROR;
				}
      }

      pSP->offset.x = ksi_num2int( KSI_VEC_REF( st, 0 ));
      pSP->offset.y = ksi_num2int( KSI_VEC_REF( st, 1 ));
      pSP->angle    = ksi_num2int( KSI_VEC_REF( st, 2 ));
      pSP->length   = pe - pb;

      ksi_obj fn = ksi_assv_ref( init_obj, keyFont );
      if( !IsNumVect( fn, 2, 2 ) ||
          KSI_VEC_LEN( fn ) > 2 && !KSI_KEY_P( KSI_VEC_REF( fn, 2 )) && !KSI_STR_P( KSI_VEC_REF( fn, 2 )) ||
          KSI_VEC_LEN( fn ) > 3 && !KSI_KEY_P( KSI_VEC_REF( fn, 3 )) && !KSI_STR_P( KSI_VEC_REF( fn, 3 )))
        return R_CONVERTERROR;

      if( bVisible )
      {
        ksi_obj cl = ksi_assv_ref( init_obj, keyColor );
        if( !KSI_EXACT_P( cl ))
          return R_CONVERTERROR;

        int ff = ksi_num2int( KSI_VEC_REF( fn, 0 ));
        int fh = ksi_num2int( KSI_VEC_REF( fn, 1 ));
        int fw = KSI_VEC_LEN( fn ) > 2 ? WeightKeys->KsiTranslate( KSI_VEC_REF( fn, 2 )) : 0;
        int fs = KSI_VEC_LEN( fn ) > 3 ? StyleKeys->KsiTranslate( KSI_VEC_REF( fn, 3 )) : 0;
        int fc = ksi_num2int( cl );
        if( vers < 107 )
        {
          par.param.full.FontParams.font.number = ff;
          par.param.full.FontParams.font.height = fh;
          par.param.full.FontParams.font.weight = fw;
          par.param.full.FontParams.font.style  = fs;
          par.param.full.FontParams.font.color  = fc;
        }
        else
        {
          par.param.full.FontParams.font1.number = ff;
          par.param.full.FontParams.font1.height = fh;
          par.param.full.FontParams.font1.weight = fw;
          par.param.full.FontParams.font1.style  = fs;
          par.param.full.FontParams.font1.color  = fc;
        }
      }
      r = WriteData( &par, sizeof point, &dst );
      if( r == R_OK )
        r = WriteData( bVisible ? (void*)&par.param.full : (void*)&par.param.string,
                       bVisible ?  sizeof par.param.full :  sizeof par.param.string,
                       &dst );
      if( r == R_OK )
        r = WriteData( pb, pSP->length, &dst );
      break;
    }
    case PF_TEXTBYLINE :
    {
      TextByLine par;

      ksi_obj fn = ksi_assv_ref( init_obj, keyFont );
      if( !IsNumVect( fn, 2, 2 ) ||
          KSI_VEC_LEN( fn ) > 2 && !KSI_KEY_P( KSI_VEC_REF( fn, 2 )) && !KSI_STR_P( KSI_VEC_REF( fn, 2 )) ||
          KSI_VEC_LEN( fn ) > 3 && !KSI_KEY_P( KSI_VEC_REF( fn, 3 )) && !KSI_STR_P( KSI_VEC_REF( fn, 3 )))
        return R_CONVERTERROR;

      ksi_obj cl = ksi_assv_ref( init_obj, keyColor );
      if( !KSI_EXACT_P( cl ))
        return R_CONVERTERROR;

      ksi_obj ksi_ims = ksi_assv_ref( init_obj, keyTxtItem );
      if( !KSI_PLIST_P( ksi_ims ))
        return R_CONVERTERROR;

      int ff = ksi_num2int( KSI_VEC_REF( fn, 0 ));
      int fh = ksi_num2int( KSI_VEC_REF( fn, 1 ));
      int fw = KSI_VEC_LEN( fn ) > 2 ? WeightKeys->KsiTranslate( KSI_VEC_REF( fn, 2 )) : 0;
      int fs = KSI_VEC_LEN( fn ) > 3 ? StyleKeys->KsiTranslate( KSI_VEC_REF( fn, 3 )) : 0;
      int fc = ksi_num2int( cl );

      par.length = KSI_LIST_LEN( ksi_ims );
      if( vers < 107 )
      {
        par.FontParams.font.number = ff;
        par.FontParams.font.height = fh;
        par.FontParams.font.weight = fw;
        par.FontParams.font.style  = fs;
        par.FontParams.font.color  = fc;
      }
      else
      {
        par.FontParams.font1.number = ff;
        par.FontParams.font1.height = fh;
        par.FontParams.font1.weight = fw;
        par.FontParams.font1.style  = fs;
        par.FontParams.font1.color  = fc;
      }
      r = WriteData( &par, sizeof par, &dst );

      while( r == R_OK && KSI_PLIST_P( ksi_ims ))
      {
        TxtItem item;
        ksi_obj ksi_itm = KSI_CAR( ksi_ims );
        ksi_obj ksi_org = KSI_VEC_REF( ksi_itm, 0 );
        item.org.x = ksi_num2int( KSI_CAR( ksi_org ));
        item.org.y = ksi_num2int( KSI_CDR( ksi_org ));
        item.angle = ksi_num2int( KSI_VEC_REF( ksi_itm, 1 ));
        item.letter[0] = ksi_num2int( ksi_char2integer( KSI_VEC_REF( ksi_itm, 2 )));
        item.letter[1] = 0;
        r = WriteData( &item, sizeof item, &dst );
        ksi_ims = KSI_CDR( ksi_ims );
      }
      break;
    }
    case PF_FOOTNOTE  :
    case PF_POLYPOINT :
    {
      ksi_obj lst = ksi_assv_ref( init_obj, keyPoly );
      if( !KSI_LIST_P( lst ))
        return R_CONVERTERROR;
      ksi_obj list = ksi_nil;
      for( lst = ksi_reverse( lst ); !KSI_NULL_P( lst ) ; )
      {
        ksi_obj h = KSI_CAR( lst );
        lst = KSI_CDR( lst );
        if( !IsNumPair( h ))
          return R_CONVERTERROR;

        if( KSI_NULL_P( lst ) || !KSI_EQUAL_P( h, KSI_CAR( lst )))
          list = ksi_cons( h, list );
      }
      int len = KSI_LIST_LEN( list );
      PolyParam par;
      par.length = len;
      r = WriteData( &par.length, sizeof par.length, &dst );
			while( r == R_OK && KSI_PLIST_P( list ))
      {
        ksi_obj h = KSI_CAR( list );
        list = KSI_CDR( list );
        Point c( ksi_num2int( KSI_CAR( h )), ksi_num2int( KSI_CDR( h )));
        r = WriteData( &c, sizeof Point, &dst );
        --len;
      }
      break;
    }
    case PF_MULTILINE :
    {
      PolyParam par;
      ksi_obj lst = ksi_assv_ref( init_obj, keyPoly );
      if( !KSI_LIST_P( lst ))
        return R_CONVERTERROR;

      par.length = KSI_LIST_LEN( lst );
      if( par.length & 1 )
        return R_CONVERTERROR;

      r = WriteData( &par, sizeof par, &dst );
      while(r == R_OK && KSI_PLIST_P( lst ))
      {
        ksi_obj h = KSI_CAR( lst );
        lst = KSI_CDR( lst );
        if( !IsNumPair( h ))
          return R_CONVERTERROR;
        Point c( ksi_num2int( KSI_CAR( h )), ksi_num2int( KSI_CDR( h )));
        r = WriteData( &c, sizeof Point, &dst );
      }
      break;
    }

    default :
    {
      ksi_obj pt = ksi_assv_ref( init_obj, keyCoord );
      if( !IsNumVect( pt, 2, 2 ))
        return R_CONVERTERROR;

      ksi_obj sz = ksi_assv_ref( init_obj, keySize );
      if( !IsNumVect( sz, 3, 3 ))
        return R_CONVERTERROR;

      long l = ksi_num2int( KSI_VEC_REF( pt, 0 ));
      long t = ksi_num2int( KSI_VEC_REF( pt, 1 ));
      long w = ksi_num2int( KSI_VEC_REF( sz, 0 ));
      long h = ksi_num2int( KSI_VEC_REF( sz, 1 ));
      long a = ksi_num2int( KSI_VEC_REF( sz, 2 ));

      if( rec.type & PF_ROUND )
        w = h = max( w, h );
      long r = l + w;
      long b = t + h;
      switch( rec.type & PF_TYPE_MASK )
      {
        case PF_RECTANGLE :
        {
          RectParam par;
          par.left   = l;
          par.top    = t;
          par.right  = r;
          par.bottom = b;
          par.angle  = a;
          r = WriteData( &par, sizeof par, &dst );
          break;
        }
        case PF_ELLIPSE :
        {
          rect par;
          par.left   = l;
          par.top    = t;
          par.right  = r;
          par.bottom = b;
          r = WriteData( &par, sizeof par, &dst );
          break;
        }
        case PF_ARC :
        case PF_CHORD :
        {
          ArcParam par;
          ksi_obj dt = ksi_assv_ref( init_obj, keyDelta );
          if( !KSI_EXACT_P( dt ))
            return R_CONVERTERROR;
          par.left   = l;
          par.top    = t;
          par.right  = r;
          par.bottom = b;
          par.angle  = a;
          par.delta  = ksi_num2int( dt );
          r = WriteData( &par, sizeof par, &dst );
          break;
        }
      }
      break;
    }
  }
  return r;
}

//=====================================================================
ksi_obj
MakeCoord( int x, int y )
{
  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 2 ), ksi_false );
  KSI_VEC_REF( vec, 0 ) = ksi_long2num( x );
  KSI_VEC_REF( vec, 1 ) = ksi_long2num( y );
  return ksi_cons( keyCoord, vec );
}
//=====================================================================
ksi_obj
MakeSize( int cx, int cy, int an )
{
  ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 3 ), ksi_false );
  KSI_VEC_REF( vec, 0 ) = ksi_long2num( cx );
  KSI_VEC_REF( vec, 1 ) = ksi_long2num( cy );
  KSI_VEC_REF( vec, 2 ) = ksi_long2num( an );
  return ksi_cons( keySize, vec );
}
//=====================================================================
ksi_obj
ConvertObject( int vers, void** ppSrc, int bFull )
{
  PrimRecord* pPr = (PrimRecord*)*ppSrc;
  int bAbsPoly = vers & 1;
  int bVisible = pPr->type & PF_VISIBLE;
  int SF = PF_STROKED;
  int FF = PF_FILLED;
  int sft = pPr->type & ( SF | FF );
  Drawable* pObjPar = !bVisible          ? &pPr->param.drawable :
                      sft == ( SF | FF ) ? &pPr->param.outlined.primParam :
                      sft == SF          ? &pPr->param.stroked.primParam  :
                      sft == FF          ? &pPr->param.filled.primParam   : 0;

  ksi_obj ksi_stroked = ksi_nil;
  ksi_obj ksi_filled  = ksi_nil;
  ksi_obj ksi_prim    = ksi_nil;

  if( bFull )
  {
    int bStroked = pPr->type & SF;
    int bFilled  = pPr->type & FF;

    ksi_prim = KSI_LIST3(
      ksi_cons( keyHVis, KSI_MK_BOOL( pPr->type & PF_HVISIBLE )),
      ksi_cons( keyLVis, KSI_MK_BOOL( pPr->type & PF_LVISIBLE )),
      ksi_cons( keySVis, KSI_MK_BOOL( pPr->type & PF_SVISIBLE )));

    if( bFilled && bVisible )
    {
      BrushParam* pBrush = &pPr->param.filled.brush;
      ksi_obj vec = ksi_make_vector( KSI_MK_SINT( 2 ), ksi_false );
      KSI_VEC_REF( vec, 0 ) = ksi_int2num( pBrush->fore );
      KSI_VEC_REF( vec, 1 ) = ksi_int2num( pBrush->back );
      ksi_obj fl = pBrush->style == 0 ? FillStyle ->KsiTranslate( pBrush->style ):
                                        HatchStyle->KsiTranslate( pBrush->hatch );

      if( pBrush->fill )
        ksi_prim = ksi_cons( ksi_cons( keyFBColor, vec ),
                   ksi_cons( ksi_cons( keyFill, fl ),
                   ksi_prim ));
      else
        ksi_prim = ksi_cons( ksi_cons( keyFBColor, vec ),
                   ksi_cons( ksi_cons( keyFill, fl ),
                   ksi_prim ));
    }

    if( bStroked && bVisible )
    {
      PenParam* pPen = &pPr->param.stroked.pen;
      ksi_obj vec = ksi_make_vector( ksi_two, ksi_false );
      int ls = pPen->style & 0xFF;
      KSI_VEC_REF( vec, 0 ) = ksi_int2num( pPen->width );
      KSI_VEC_REF( vec, 1 ) = ls > 8 ?
        ksi_int2num( ls ) :
        LineStyle->KsiTranslate( ls );
      ksi_prim = ksi_cons( ksi_cons( keyStrok, vec ),
                 ksi_cons( ksi_cons( keyColor, ksi_int2num( pPen->color )),
                           ksi_prim ));
    }
  }

  ksi_obj ksi_prt = PrimKeys->KsiTranslate( pPr->type & PF_SHAPE_MASK );

  int tt = pPr->type & PF_TYPE_MASK;
  switch( tt )
  {
    case PF_CONTAINER:
    {
      *ppSrc = pPr->param.container.prims;
      int primNum = pPr->param.container.length;
      ksi_obj ksi_lst = ksi_nil;
      for( int i = primNum ; --i >= 0 ;
           ksi_lst = ksi_cons( ConvertObject( vers, ppSrc, bFull ), ksi_lst ));
      ksi_obj ksi_arr[] = { KSI_LIST1( ksi_reverse( ksi_lst )), ksi_prim };
      ksi_prim = ksi_append_x( 2, ksi_arr );
      break;
    }
    case PF_POINT:
    {
      *ppSrc = &pPr->param.pnt + 1;
      point* pPnt = &pPr->param.pnt;
      ksi_prim = ksi_cons( MakeCoord( pPnt->x, pPnt->y ),
                           ksi_prim );
      break;
    }
    case PF_ICON:
    {
      *ppSrc = &pPr->param.icon + 1;
      Icon* pIcon = &pPr->param.icon;
      ksi_prim = ksi_cons( MakeCoord( pIcon->x, pIcon->y ),
                 ksi_cons( ksi_cons( keyReference, ksi_long2num( pIcon->id )),
                           ksi_prim ));
      break;
    }
    case PF_TEXT:
    {
      Text* pText = &pPr->param.text;
      StringParam* pSP = bVisible ? &pText->param.full.string : &pText->param.string;

      char* pStr = pSP->string;
      int   len  = pSP->length;
      *ppSrc = pStr + len;

      ksi_obj ksi_str = ksi_make_vector( KSI_MK_SINT( 4 ), ksi_false );
      KSI_VEC_REF( ksi_str, 0 ) = ksi_int2num( pSP->offset.x );
      KSI_VEC_REF( ksi_str, 1 ) = ksi_int2num( pSP->offset.y );
      KSI_VEC_REF( ksi_str, 2 ) = ksi_int2num( pSP->angle );
      KSI_VEC_REF( ksi_str, 3 ) = ksi_str2string( pStr, len );
      ksi_str = ksi_cons( keyString, ksi_str );
      if( bFull && bVisible )
      {
        ksi_obj ksi_color;
        ksi_obj ksi_fnt = ksi_make_vector( KSI_MK_SINT( 5 ), ksi_false );
        if( vers < 107 )
        {
          KSI_VEC_REF( ksi_fnt, 0 ) = ksi_int2num( pText->param.full.FontParams.font.number );
          KSI_VEC_REF( ksi_fnt, 1 ) = ksi_int2num( pText->param.full.FontParams.font.height );
          KSI_VEC_REF( ksi_fnt, 2 ) = ksi_str02string( WeightKeys->Translate( pText->param.full.FontParams.font.weight ));
          KSI_VEC_REF( ksi_fnt, 3 ) = ksi_str02string( StyleKeys->Translate( pText->param.full.FontParams.font.style ));
          KSI_VEC_REF( ksi_fnt, 4 ) = ksi_str02string( "Russian" );
          ksi_color = ksi_int2num( pText->param.full.FontParams.font.color );
        }
        else
        {
          KSI_VEC_REF( ksi_fnt, 0 ) = ksi_int2num( pText->param.full.FontParams.font1.number );
          KSI_VEC_REF( ksi_fnt, 1 ) = ksi_int2num( pText->param.full.FontParams.font1.height );
          KSI_VEC_REF( ksi_fnt, 2 ) = ksi_str02string( WeightKeys->Translate( pText->param.full.FontParams.font1.weight ));
          KSI_VEC_REF( ksi_fnt, 3 ) = ksi_str02string( StyleKeys->Translate( pText->param.full.FontParams.font1.style ));
          KSI_VEC_REF( ksi_fnt, 4 ) = ksi_str02string( "Russian" );
          ksi_color = ksi_int2num( pText->param.full.FontParams.font1.color );
        }

        ksi_prim = ksi_cons( ksi_cons( keyFont, ksi_fnt ),
                   ksi_cons( MakeCoord( pText->org.x, pText->org.y ),
                   ksi_cons( ksi_str,
                   ksi_cons( ksi_cons( keyColor, ksi_color ),
                             ksi_prim ))));
      }
      else
        ksi_prim = ksi_cons( ksi_str,
                   ksi_cons( MakeCoord( pText->org.x, pText->org.y ),
                             ksi_prim ));
      break;
    }
    case PF_TEXTBYLINE:
    {
      int length = pPr->param.textbyline.length;
      TxtItem* pItem = pPr->param.textbyline.items;
      *ppSrc = pItem + length;

      if( bFull && bVisible )
      {
        ksi_obj ksi_color;
        ksi_obj ksi_fnt = ksi_make_vector( KSI_MK_SINT( 5 ), ksi_false );
        if( vers < 107 )
        {
          KSI_VEC_REF( ksi_fnt, 0 ) = ksi_int2num( pPr->param.textbyline.FontParams.font.number );
          KSI_VEC_REF( ksi_fnt, 1 ) = ksi_int2num( pPr->param.textbyline.FontParams.font.height );
          KSI_VEC_REF( ksi_fnt, 2 ) = ksi_str02string( WeightKeys->Translate( pPr->param.textbyline.FontParams.font.weight ));
          KSI_VEC_REF( ksi_fnt, 3 ) = ksi_str02string( StyleKeys->Translate( pPr->param.textbyline.FontParams.font.style ));
          KSI_VEC_REF( ksi_fnt, 4 ) = ksi_str02string( "Russian" );
          ksi_color = ksi_int2num( pPr->param.textbyline.FontParams.font.color );
        }
        else
        {
          KSI_VEC_REF( ksi_fnt, 0 ) = ksi_int2num( pPr->param.textbyline.FontParams.font1.number );
          KSI_VEC_REF( ksi_fnt, 1 ) = ksi_int2num( pPr->param.textbyline.FontParams.font1.height );
          KSI_VEC_REF( ksi_fnt, 2 ) = ksi_str02string( WeightKeys->Translate( pPr->param.textbyline.FontParams.font1.weight ));
          KSI_VEC_REF( ksi_fnt, 3 ) = ksi_str02string( StyleKeys->Translate( pPr->param.textbyline.FontParams.font1.style ));
          KSI_VEC_REF( ksi_fnt, 4 ) = ksi_str02string( "Russian" );
          ksi_color = ksi_int2num( pPr->param.textbyline.FontParams.font1.color );
        }

        ksi_obj ksi_itms = ksi_nil;
        for( ; --length >= 0 ; ++pItem )
        {
          ksi_obj ksi_itm = ksi_make_vector( ksi_three, ksi_false );
          KSI_VEC_REF( ksi_itm, 0 ) = ksi_cons( ksi_int2num( pItem->org.x ),
                                                ksi_int2num( pItem->org.y ));
          KSI_VEC_REF( ksi_itm, 1 ) = ksi_int2num( pItem->angle );
          KSI_VEC_REF( ksi_itm, 2 ) = ksi_integer2char( ksi_int2num( UCHAR( pItem->letter[0] )));
          ksi_itms = ksi_cons( ksi_itm, ksi_itms );
        }

        ksi_prim = ksi_cons( ksi_cons( keyFont, ksi_fnt ),
                   ksi_cons( ksi_cons( keyColor, ksi_color ),
                   ksi_cons( ksi_cons( keyTxtItem, ksi_itms ),
                             ksi_prim )));
      }
      else
        ksi_prim = ksi_cons( ksi_cons( keyTxtItem, ksi_nil ),
                             ksi_prim );
      break;
    }
    case PF_POLYPOINT:
    case PF_FOOTNOTE:
    {
      point* p = pObjPar->polypoint.points;
      point* pp = p + pObjPar->polypoint.length;
      *ppSrc = pp;
      ksi_obj list = ksi_nil;
      int x = 0, y = 0;
      for( ; p < pp ; ++p )
        list = ksi_cons( KsiPoint( bAbsPoly ? p->x : x += p->x,
                                   bAbsPoly ? p->y : y += p->y ),
                           list );
      ksi_prim = ksi_cons( ksi_cons( tt == PF_POLYPOINT ? keyPoly : keyFootnote, ksi_reverse( list )),
                           ksi_prim );
      break;
    }
    case PF_MULTILINE:
    {
      point* p0 = pObjPar->polypoint.points;
      point* pp = p0 + pObjPar->polypoint.length;
      *ppSrc = pp;
      ksi_obj list = ksi_nil;
      while(( pp -= 2 ) >= p0 )
        list = ksi_cons( KsiPoint( pp->x, pp->y ),
               ksi_cons( KsiPoint( bAbsPoly ? pp[1].x : pp->x + pp[1].x,
                                   bAbsPoly ? pp[1].y : pp->y + pp[1].y ),
                         list ));
      ksi_prim = ksi_cons( ksi_cons( keyPoly, list ),
                           ksi_prim );
      break;
    }
    case PF_ELLIPSE :
    {
      rect* pr = &pObjPar->ellipse;
      *ppSrc = pr + 1;
      ksi_prim = ksi_cons( MakeCoord( pr->left, pr->top ),
                 ksi_cons( MakeSize( pr->right - pr->left, pr->bottom - pr->top, 0 ),
                           ksi_prim ));
      break;
    }
    case PF_RECTANGLE :
    {
      RectParam* pr = &pObjPar->rectangle;
      *ppSrc = pr + 1;
      ksi_prim = ksi_cons( MakeCoord( pr->left, pr->top ),
                 ksi_cons( MakeSize( pr->right - pr->left, pr->bottom - pr->top, pr->angle ),
                           ksi_prim ));
      break;
    }
    case PF_ARC :
    {
      ArcParam* pr = &pObjPar->arc;
      *ppSrc = pr + 1;
      ksi_prim = ksi_cons( MakeCoord( pr->left, pr->top ),
                 ksi_cons( MakeSize( pr->right - pr->left, pr->bottom - pr->top, pr->angle ),
                 ksi_cons( ksi_cons( keyDelta, ksi_int2num( pr->delta )),
                           ksi_prim )));
      break;
    }
    default :
      return ksi_false;
  }

  return ksi_cons( ksi_prt, ksi_prim );
}
//=====================================================================
CBox
CalcBoundBox( int vers, void** ppSrc )
{
  PrimRecord* pPr   = (PrimRecord*)*ppSrc;
  Drawable* pObjPar = (Drawable*)&pPr->param;

  int subType = (( pPr->type & PF_STROKED ) ? STROKED : 0 ) |
                (( pPr->type & PF_FILLED  ) ? FILLED  : 0 ) |
                (( pPr->type & PF_ROUND   ) ? ROUND   : 0 );
  int bVisible = pPr->type & PF_VISIBLE;
  int typeKey = pPr->type & PF_TYPE_MASK;

  int width = 0;
  int style = 0;

  if( bVisible  )
  {
    int t = subType & OUTLINED;
    pObjPar = t == STROKED  ? &pPr->param.stroked.primParam  :
              t == FILLED   ? &pPr->param.filled.primParam   :
              t == OUTLINED ? &pPr->param.outlined.primParam : pObjPar;
    if( subType & STROKED )
    {
      width = pPr->param.stroked.pen.width;
      style = pPr->param.stroked.pen.style;
    }
  }

  switch( typeKey )
  {
    case PF_CONTAINER:
    {
      CBox box;
      *ppSrc = pPr->param.container.prims;
      for( int n = pPr->param.container.length ; --n >= 0 ; )
        box |= CalcBoundBox( vers, ppSrc );
      return box;
    }
    case PF_POLYPOINT:
    case PF_MULTILINE:
    case PF_FOOTNOTE:
    {
      int nPoints = pObjPar->polypoint.length;
      Point* pPoints = (Point*)pObjPar->polypoint.points;
      *ppSrc = pPoints + nPoints;
      if( bVisible )
        return CBox((CPoint*)pPoints, nPoints, style, width, 0 );
      break;
    }
    case PF_POINT:
    {
      *ppSrc = &pPr->param.pnt + 1;
      if( bVisible )
        return CBox( *((CPoint*)&pPr->param.pnt ));
      break;
    }
    case PF_ICON:
    {
      *ppSrc = &pPr->param.icon + 1;
      if( bVisible )
      {
        CBox box( pPr->param.icon.id, *((CPoint*)&pPr->param.icon ), 1 );
//        box.InflateRect( 5, 5 );
        return box;
      }
      break;
    }
    case PF_TEXT:
    {
      Text* pText = &pPr->param.text;
      StringParam* pSP = bVisible ? &pText->param.full.string :
                                    &pText->param.string;
      *ppSrc = pSP->string + pSP->length;
      if( bVisible )
      {
        FontParam1 FP;
        if( vers < 107 )
        {
          FontParam* pFP = &pText->param.full.FontParams.font;
          FP.color  = pFP->color;
          FP.style  = pFP->style;
          FP.height = pFP->height;
          FP.number = pFP->number;
          FP.weight = pFP->weight;
        }
        else
          memcpy( &FP, &pText->param.full.FontParams.font1, sizeof FP );

        CPoint pt( pText->org.x, pText->org.y );
        CSize  of( pSP->offset.x, pSP->offset.y );
        return CBox( pt, of, pSP->angle, pSP->string, pSP->length,
                     FP.number, FP.height, FP.weight, FP.style );
      }
      break;
    }
    case PF_TEXTBYLINE:
    {
      int length = pPr->param.textbyline.length;
      TxtItem* pItem = pPr->param.textbyline.items;
      *ppSrc = pItem + length;

      if( bVisible )
      {
        FontParam1 FP;
        if( vers < 107 )
        {
          FontParam* pFP = &pPr->param.textbyline.FontParams.font;
          FP.color  = pFP->color;
          FP.style  = pFP->style;
          FP.height = pFP->height;
          FP.number = pFP->number;
          FP.weight = pFP->weight;
        }
        else
          memcpy( &FP, &pPr->param.textbyline.FontParams.font1, sizeof FP );

        CBox box;
        for( ; --length >= 0 ; ++pItem )
          box |= CBox( CPoint( pItem->org.x, pItem->org.y ), CSize( 0, 0 ),
                       pItem->angle, pItem->letter, 1,
                       FP.number, FP.height, FP.weight, FP.style );
        return box;
      }
      break;
    }
    case PF_ELLIPSE :
    {
      rect* pr = &pObjPar->ellipse;
      CPoint lt( pr->left, pr->top );
      CPoint rb( pr->right, pr->bottom );
      CSize sz( rb - lt );
      lt -= sz;
      sz += sz;
      *ppSrc = pr + 1;
      if( bVisible )
        return CBox( CRect( lt, sz ), width );
      break;
    }
    case PF_RECTANGLE :
    {
      RectParam* pr = &pObjPar->rectangle;
      *ppSrc = pr + 1;
      if( bVisible )
        return CBox( *((CRect*)pr ), *((CPoint*)pr ), pr->angle, width );
      break;
    }
    case PF_ARC :
    {
      ArcParam* pr = &pObjPar->arc;
      CPoint c( pr->left, pr->top );
      CPoint s( pr->right, pr->bottom );
      bool bPie = ( subType & PF_FILLED ) && !( subType & PF_CHORD );
      s -= c;
      c -= s;
      s += s;
      *ppSrc = pr + 1;
      if( bVisible )
        return CBox( c, s, pr->angle, pr->delta, width, bPie );
      break;
    }
  }
  return CBox();
}
//=====================================================================
#endif
//=====================================================================
char const*
GetDT()
{
  SYSTEMTIME st;
  GetLocalTime( &st );
  static char dtm[256];
  sprintf( dtm, " %02d.%02d.%4d, %02d:%02d:%02d ",
           st.wDay, st.wMonth, st.wYear,
           st.wHour, st.wMinute, st.wSecond );
  return dtm;
}
//=====================================================================
ResultCode
TraceError( int mapNum, ResultCode r, char const* opName, long p0, long p1 )
{
  static char* Err[] =
  {
    0,                          // R_OK
    0,                          // R_REPEAT
    "File struct error",        // R_BADFILESTRUCT
    "Memory alloc error",       // R_OUTOFMEMORY
    "Bad stream",               // R_BADSTREAM
    "Unexpected end of stream", // R_EOFSTREAM
    "Failed stream",            // R_FAILSTREAM
    "File not found",           // R_NOFILE
    "Project path not found",   // R_NOPROJECT
    "Object not found",         // R_NOIDENT
    "Convert data error",       // R_CONVERTERROR
    "Unknown error",            // R_UNKNOWNERROR
    "Flush error",              // R_FLUSHERROR
    "Map not open",             // R_MAPNOTOPEN
    "Duplicated index",         // R_DUPIDENT
    "Bad record head",          // R_BADRECORDHEAD
    "",                         // R_INFORMATION
    "Login user",               // R_LOGUSER
    "Stop user",                // R_STOPUSER
    "Too large object",         // R_HUDEOBJECT
    "Incorrect object"          // R_BADOBJECT
  };

  char* msg;
  char tmp[16];
  if( r < CELEM( Err ))
    msg = Err[r];
  else
    sprintf( msg = tmp, "#%d", r );

  if( msg )
  {
    char t[1024], m[16] = "--";
    if( mapNum >= 0 )
      sprintf( m, "[%d]", mapNum );
    int n = sprintf( t, "%s%s%s %s", m, GetDT(), opName, msg );
    if( p0 && p1 )
      sprintf( t + n, " : %d %d", p0, p1 );
    else
    if( p0 )
      sprintf( t + n, " : %08x", p0 );
    else
    if( p1 )
      sprintf( t + n, " : %d", p1 );
    #ifndef _FULL_PROJECT
      if( r != R_INFORMATION )
        ErrBox( t );
    #else
      cerr << t << endl;
    #endif
  }
  return r;
}
//=====================================================================
ResultCode
ReadData( void* pData, int size, std::istream* pSrc )
{
  pSrc->read((char*)pData, size );
  return pSrc->good() ? R_OK :
         pSrc->eof()  ? R_EOFSTREAM :
         pSrc->bad()  ? R_BADSTREAM :
         pSrc->fail() ? R_FAILSTREAM :
                        R_UNKNOWNERROR;
}
//=====================================================================
ResultCode
WriteData( void* pData, int size, std::ostream* pDst )
{
  pDst->write((char*)pData, size );
  return pDst->good() ? R_OK :
         pDst->eof()  ? R_EOFSTREAM :
         pDst->bad()  ? R_BADSTREAM :
         pDst->fail() ? R_FAILSTREAM :
                        R_UNKNOWNERROR;
}
//=====================================================================
ResultCode
ReadData( std::streampos pos, void* pData, int size, std::istream* pSrc )
{
  pSrc->seekg( pos );
  return pSrc->tellg() == pos ? ReadData( pData, size, pSrc ) : R_FAILSTREAM;
}
//=====================================================================
ResultCode
WriteData( std::streampos pos, void* pData, int size, std::ostream* pDst )
{
  pDst->seekp( pos );
  return pDst->tellp() == pos ? WriteData( pData, size, pDst ) : R_FAILSTREAM;
}
//=====================================================================
char*
MakePath( char const* path, char* buf, char const* ext, char const* name )
{
  char iv[_MAX_DRIVE], ip[_MAX_DIR], in[_MAX_FNAME], ie[_MAX_EXT];
  _splitpath( path, iv, ip, in, ie );
  _makepath( buf, iv, ip, name ? name : in, ext );
  return buf;
}
//=====================================================================
int
GetLong( char const* path, char const* section, char const* entry, long* par, long def )
{
  char buf[1024];
  int n = GetPrivateProfileString( section, entry, "", buf, sizeof buf, path );
  *par = n ? atol( buf ) : def;
  return n;
}
//=====================================================================
int
GetString( char const* path, char const* section, char const* entry, char* par, int size, char* def )
{
  return GetPrivateProfileString( section, entry, def, par, size, path );
}
//=====================================================================
