//=====================================================================
#include "stdafx.h"

#include <io.h>
#include "dwin.h"
#include "mapmole.h"
#include "mdoc.h"
#include "mview.h"
#include "mapupdtr.h"
#include "rgnspace.h"
#include "mathutil.h"
#include "convdlg.h"
#include "msutil.h"
#include "util.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "ksikey.h"
#include "cbox.h"
#include "joke.h"
#include "mercator.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
void
MapUpdater::SetSize( CSize s )
{
  HR.XSize = s.cx;
  HR.YSize = s.cy;
  WriteData( 0, &HR, sizeof HR, &DataStream );
}
//=====================================================================
int 
MapUpdater::SetModified( int m )
{
  int t = ModifiedFlag;
  ModifiedFlag = m;
  return t;
}
//=====================================================================
int
MapUpdater::TestInclude( char pClassMap[128], ClassType t )
{
  return pClassMap ? pClassMap[t>>3] & ( 1 << ( t & 7 )) : 1;
}
//=====================================================================
static CPoint
UpdatePoint( double x, double y, double cf[2][3] )
{
  if( cf )
  {
    int nx = x * cf[0][0] + y * cf[0][1] + cf[0][2];
    int ny = x * cf[1][0] + y * cf[1][1] + cf[1][2];   
    return CPoint( nx, ny );
  }
  return CPoint( x, y );
}
//=====================================================================
static CPoint
UpdatePoint( CPoint pt, double cf[2][3], double utm[5], int bAbs )
{
	if( utm )
	{
		Polar pol;
		if( MerkatorData.GK2Pol( Point( pt.x, pt.y ), 
			                       Point( utm[0] * 1e2 / SmInUnit, utm[1] * SmInUnit * 1e2 / SmInUnit ), 
														 utm[3], utm[4], utm[2] * Deg, pol ))
		{
			Point p( MerkatorData.Pol2Dec( pol ));
			pt.x = p.x;
			pt.y = p.y;
		}
	}
  if( cf )
  {
    int x = pt.x * cf[0][0] + pt.y * cf[0][1] + ( bAbs ? cf[0][2] : 0 );
    int y = pt.x * cf[1][0] + pt.y * cf[1][1] + ( bAbs ? cf[1][2] : 0 );
		pt.x = x;
		pt.y = y;
  }
  return pt;
}
//=====================================================================
static Point
UpdatePoint( Point pt, double cf[2][3], double utm[5], int bAbs )
{	
	if( utm )
	{
		Polar pol;
		if( MerkatorData.GK2Pol( Point( pt.x, pt.y ), 
			                       Point( utm[0] * 1e2 / SmInUnit, utm[1] * SmInUnit * 1e2 / SmInUnit ), 
														 utm[3], utm[4], utm[2] * Deg, pol ))
		{
			Point p( MerkatorData.Pol2Dec( pol ));
			pt.x = p.x;
			pt.y = p.y;
		}
	}
  if( cf )
  {
    int x = pt.x * cf[0][0] + pt.y * cf[0][1] + ( bAbs ? cf[0][2] : 0 );
    int y = pt.x * cf[1][0] + pt.y * cf[1][1] + ( bAbs ? cf[1][2] : 0 );
		pt.x = x;
		pt.y = y;
  }
  return pt;
}
//=====================================================================
int
MapUpdater::IsObjectInProject( CBox* pBox )
{
  return pBox->left < 0 || pBox->right  > HR.XSize ||
         pBox->top  < 0 || pBox->bottom > HR.YSize;
}
//=====================================================================
int 
MapUpdater::MoveObjectToProject( void* pSrc, CBox* pBox )
{
  int dx = pBox->left < 0 || pBox->left > HR.XSize ? -pBox->left : 0;
  int dy = pBox->top  < 0 || pBox->top  > HR.YSize ? -pBox->top  : 0;

  if( !dx && !dy )
    return 0;

  double pcf[2][3] = { 1, 0, dx, 0, 1, dy };
  pBox->SetRect( 0, 0, 0, 0 );
  void* pData = pSrc;
  UpdateObjectCoord( &pSrc, pcf, 0 );
  pSrc = pData;
  *pBox = CalcBoundBox( HR.Version, &pSrc );

  return 1;
}
//=====================================================================
int 
MapUpdater::UpdateObjectCoord( void** ppSrc, double pcf[2][3], double utm[4], Point* Rgn )
{
  PrimRecord* pPr    = (PrimRecord*)*ppSrc;
  Drawable* pObjPar  = (Drawable*)&pPr->param;

  int bAbs = HR.Version & 1;
  int subType = (( pPr->type & PF_STROKED ) ? STROKED : 0 ) |
                (( pPr->type & PF_FILLED  ) ? FILLED  : 0 ) |
                (( pPr->type & PF_ROUND   ) ? ROUND   : 0 );
  int bVisible = pPr->type & PF_VISIBLE;
  int typeKey  = pPr->type & PF_TYPE_MASK;

  int w = 0;

  int da = 0;
  int rx = 0, ry = 0;
  if( pcf )
  {
    CPoint t0( UpdatePoint( CPoint( 0, 0 ), pcf, utm, 0 ));
    CPoint t1( UpdatePoint( CPoint( 10000, 0 ), pcf, utm, 0 ));
    da = atan2( t1.x - t0.x, t0.y - t1.y );

/*
    if( pcf[0][0] * pcf[1][1] < 0 )
    {
      rx = pcf[0][0] < 0;
      ry = pcf[1][1] < 0;
    }
*/  
	}

  if( bVisible  )
  {
    int t = subType & OUTLINED;
    pObjPar = t == STROKED  ? &pPr->param.stroked.primParam  :
              t == FILLED   ? &pPr->param.filled.primParam   :
              t == OUTLINED ? &pPr->param.outlined.primParam :
                              pObjPar ;
    if( subType & PF_STROKED )
      w = pPr->param.stroked.pen.width / 2 + 1;
  }

  switch( typeKey )
  {
    case PF_CONTAINER:
    {
      *ppSrc = pPr->param.container.prims;
	  int n;
      for(n = pPr->param.container.length ;
           --n >= 0 && UpdateObjectCoord( ppSrc, pcf, utm, Rgn ) ; );
      return n < 0;
    }
    case PF_POLYPOINT:
    case PF_FOOTNOTE:
    {
      int nPoints = pObjPar->polypoint.length;
      Point* pPoints = (Point*)pObjPar->polypoint.points;
      *ppSrc = pPoints + nPoints;
      if( Rgn && !bVisible )
        break;

      Point pt( 0, 0 );
      for( int i = 0 ; i < nPoints ; ++i, ++pPoints )
      {
        if( !Rgn )
          *pPoints = UpdatePoint( *pPoints, pcf, utm, bAbs || i == 0 );
        else
        if( !PntInRgn( bAbs ? *pPoints : pt += *pPoints, Rgn ))
          return 0;
      }
      break;
    }
    case PF_MULTILINE:
    {
      int nPoints = pObjPar->polypoint.length;
      Point* pPoints = (Point*)pObjPar->polypoint.points;
      *ppSrc = pPoints + nPoints;
      if( Rgn && !bVisible )
        break;

      for( int i = 0 ; i < nPoints ; ++i, ++pPoints )
      {
        int ab = bAbs || !( i & 1 );
        if( !Rgn )
          *pPoints = UpdatePoint( *pPoints, pcf, utm, ab );
        else
        if( !PntInRgn( ab ? *pPoints : pPoints[-1] + pPoints[0], Rgn ))
          return 0;
     }
      break;
    }
    case PF_POINT:
    {
      Point* pPoint = (Point*)&pPr->param.pnt;
      *ppSrc = &pPr->param.pnt + 1;
      if( !Rgn )
        *pPoint = UpdatePoint( *pPoint, pcf, utm, 1 );

      else
      if( bVisible && !PntInRgn( *pPoint, Rgn ))
        return 0;
      break;
    }
    case PF_ICON:
    {
      Point* pPoint = (Point*)&pPr->param.icon;
      *ppSrc = &pPr->param.icon + 1;
      Point p( pPr->param.icon.x, pPr->param.icon.y );
      if( !Rgn )
      {
        p = UpdatePoint( p, pcf, utm, 1 );
        pPr->param.icon.x = p.x;
        pPr->param.icon.y = p.y;
      }
      else
      if( bVisible && !PntInRgn( p, Rgn ))
        return 0;
      break;
    }
    case PF_TEXT:
    {
      Text* pText = &pPr->param.text;
      StringParam* pSP = bVisible ? &pText->param.full.string :
                                    &pText->param.string;
      *ppSrc = pSP->string + pSP->length;

      Point p0( pText->org.x, pText->org.y );

      if( !Rgn )
      {
        if( bVisible )
        {
          FontParam1 FP;
          if( HR.Version < 107 )
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

          int an = pSP->angle;
          int ox = pSP->offset.x;
          int oy = pSP->offset.y;

          CBox b( CPoint( 0, 0 ), CSize( 0, 0 ), 0, pSP->string, pSP->length,
                  FP.number, FP.height, FP.weight, FP.style );

          CPoint p0( pText->org.x, pText->org.y );
          CPoint t0( UpdatePoint( p0, pcf, utm, 1 ));
          CPoint t1( UpdatePoint( p0 + TurnPoint( ox, oy, an ), pcf, utm, 1 ));
          CPoint t2( UpdatePoint( p0 + TurnPoint( ox, oy + b.bottom + b.top, an ), pcf, utm, 1 ));
          CPoint t3( UpdatePoint( p0 + TurnPoint( ox + b.right, oy, an ), pcf, utm, 1 ));

          if( rx )
          {
            t1 -= t3;
            an = atan2( t1.x, -t1.y );
            t1 = t3;
          }
          else
          if( ry )
          {
            t3 -= t1;
            an = atan2( t3.x, -t3.y );
            t1 = t2;
          }
          else
          {
            t3 -= t1;
            an = atan2( t3.x, -t3.y );
          }

          pSP->angle = an;

          CPoint p1( TurnPoint( ox, oy, an ));
          pText->org.x = t1.x - p1.x;
          pText->org.y = t1.y - p1.y;
        }
        else
        {
          CPoint p0( pText->org.x, pText->org.y );
          CPoint t0( UpdatePoint( p0, pcf, utm, 1 ));
          int an = pSP->angle + da;
          pSP->angle = rx ? 1800 - an : ry ? -an : an;
          pText->org.x = t0.x;
          pText->org.y = t0.y;
        }
      }
      else
      if( bVisible && !PntInRgn( p0, Rgn ))
        return 0;
      break;
    }
    case PF_TEXTBYLINE:
    {
      int length = pPr->param.textbyline.length;
      TxtItem* pItem = pPr->param.textbyline.items;
      *ppSrc = pItem + length;

      CPoint p0( pItem->org.x, pItem->org.y );
      CPoint p1( UpdatePoint( p0, pcf, utm, 1 ));
      CSize dp( p1 - p0 );
      for( ; --length >= 0 ; ++pItem )
        if( !Rgn )
        {
          pItem->org.x += dp.cx;
          pItem->org.y += dp.cy;
          pItem->angle += da;
        }
        else
        if( PntInRgn( Point( pItem->org.x, pItem->org.y ), Rgn ))
          break;
      if( Rgn && length < 0 )
        return 0;
      break;
    }
    case PF_ELLIPSE :
    {
      rect* pr = &pObjPar->ellipse;
      *ppSrc = pr + 1;
      
      if( Rgn )
      {
				Point p( pr->left, pr->top );
        if( bVisible && !PntInRgn( p, Rgn ))
          return 0;
      }
      else
      {
        int w = pr->right - pr->left;
        int h = pr->bottom - pr->top;
        Point p( pr->left, pr->top );
        p = UpdatePoint( p, pcf, utm, 1 );
        pr->left   = p.x;
        pr->top    = p.y;
        pr->right  = p.x + w;
        pr->bottom = p.y + h;
      }
      break;
    }
    case PF_RECTANGLE :
    {
      RectParam* pr = &pObjPar->rectangle;
      *ppSrc = pr + 1;
      if( Rgn )
      {
        Point p( pr->left, pr->top );
        if( bVisible && !PntInRgn( p, Rgn ))
          return 0;
      }
      else
      {
        int w = pr->right - pr->left;
        int h = pr->bottom - pr->top;
        if( rx )
          w = -w;
        if( ry )
          h = -h;
        Point p( pr->left, pr->top );
        p = UpdatePoint( p, pcf, utm, 1 );
        pr->left   = p.x;
        pr->top    = p.y;
        pr->right  = p.x + w;
        pr->bottom = p.y + h;
        pr->angle += da;
        if( rx )
          pr->angle = 1800 - pr->angle;
        if( ry )
          pr->angle = -pr->angle;
      }
      break;
    }
    case PF_ARC :
    {
      ArcParam* pr = &pObjPar->arc;
      *ppSrc = pr + 1;
      if( Rgn )
      {
        Point p( pr->left, pr->top );
        if( bVisible && !PntInRgn( p, Rgn ))
          return 0;
      }
      else
      {
        int w = pr->right - pr->left;
        int h = pr->bottom - pr->top;
        if( rx )
          w = -w;
        if( ry )
          h = -h;
        Point p( pr->left, pr->top );
        p = UpdatePoint( p, pcf, utm, 1 );
        pr->left   = p.x;
        pr->top    = p.y;
        pr->right  = p.x + w;
        pr->bottom = p.y + h;
        pr->angle += da;
        if( rx )
        {
          pr->angle = 1800 - pr->angle;
          pr->delta = -pr->delta;
        }
        if( ry )
        {
          pr->angle = -pr->angle;
          pr->delta = -pr->delta;
        }
      }
      break;
    }
  }

  return 1;
}
//=====================================================================
static int
OutputKsiItem( std::ofstream& out, char const* p )
{
  out << p;
  return strlen( p );
}
//=====================================================================
static int
OutputKsiDlm( std::ofstream& out, int len )
{
  if( len >= 72 )
  {
    out << std::endl;
    len = 0;
  }
  else
  {
    out << " ";
    ++len;
  }
  return len;
}
//=====================================================================
static int
OutputKsiPair( std::ofstream& out, int x, int y )
{
  CString t;
  t.Format( "(%d . %d)", x, y );
  return OutputKsiItem( out, t );
}
//=====================================================================
static int
OutputKsiCoord( std::ofstream& out, int x, int y )
{
  CString t;
  t.Format( "(#:%s . #(%d %d))", KSI_KEY_PTR( keyCoord ), x, y );
  return OutputKsiItem( out, t );
}
//=====================================================================
static int
OutputKsiSize( std::ofstream& out, int cx, int cy, int a )
{
  CString t;
  t.Format( "(#:%s . #(%d %d %d))", KSI_KEY_PTR( keySize ), cx, cy, a );
  return OutputKsiItem( out, t );
}
//=====================================================================
static int
OutputKsiKey( std::ofstream& out, ksi_obj ksi_key, int len )
{
  CString t;
  t.Format( "(#:%s", KSI_KEY_PTR( ksi_key ));
  return OutputKsiDlm( out, len + OutputKsiItem( out, t ));
}
//=====================================================================
static int
OutputKsiKeyPair( std::ofstream& out, ksi_obj ksi_key, int i )
{
  CString t;
  t.Format( "(#:%s . %d)", KSI_KEY_PTR( ksi_key ), i );
  return OutputKsiItem( out, t );
}
//=====================================================================
static int
OutputKsiText( std::ofstream& out, int x, int y, int cx, int cy, int a, 
               char const* s, int sln, int len )
{
  ksi_obj ks = ksi_object2string( ksi_str2string( s, sln ));
  CString t;
  t.Format( "(#:%s . #(%d %d %d %s))", KSI_KEY_PTR( keyString ), cx, cy, a, KSI_STR_PTR( ks ));
  len += OutputKsiCoord( out, x, y );
  len = OutputKsiDlm( out, len );
  return len + OutputKsiItem( out, t );
}
//=====================================================================
static int
OutputKsiObject( std::ofstream& out, int vers, void** ppSrc, int len = 0, int level = 0 )
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
                      sft == FF          ? &pPr->param.filled.primParam :
                                           0;

  ksi_obj ksi_prt = PrimKeys->KsiTranslate( pPr->type & PF_SHAPE_MASK );

  if( level < 2 )
    len = OutputKsiKey( out, ksi_prt, len );

  switch( pPr->type & PF_TYPE_MASK )
  {
    case PF_CONTAINER:
    {
      *ppSrc = pPr->param.container.prims;
      int primNum = pPr->param.container.length;
      len += OutputKsiItem( out, "(" );
      for( int i = primNum ; --i >= 0 ; )
      {
        len = OutputKsiObject( out, vers, ppSrc, len, level + 1 );
        if( i && level < 1 )
          len = OutputKsiDlm( out, len );
      }
      len += OutputKsiItem( out, ")" );
      break;
    }
    case PF_POINT:
    {
      *ppSrc = &pPr->param.pnt + 1;
      if( level < 2 )
      {
        point* pPnt = &pPr->param.pnt;
        len += OutputKsiCoord( out, pPnt->x, pPnt->y );
      }
      break;
    }
    case PF_ICON:
    {
      *ppSrc = &pPr->param.icon + 1;
      if( level < 2 )
      {
        Icon* pIcon = &pPr->param.icon;
        len += OutputKsiKeyPair( out, keyReference, pIcon->id );
        len = OutputKsiDlm( out, len );
        len += OutputKsiCoord( out, pIcon->x, pIcon->y );
      }
      break;
    }
    case PF_TEXT:
    {
      Text* pText = &pPr->param.text;
      StringParam* pSP = bVisible ? &pText->param.full.string : &pText->param.string;
      *ppSrc = pSP->string + pSP->length;
      if( level < 2 )
      {
        len += OutputKsiText( out, 
                              pText->org.x, pText->org.y, 
                              pSP->offset.x, pSP->offset.y, 
                              pSP->angle, pSP->string, pSP->length,
                              len );
      }
      break;
    }
    case PF_TEXTBYLINE:
    {
      int length = pPr->param.textbyline.length;
      TxtItem* pItem = pPr->param.textbyline.items;
      *ppSrc = pItem + length;
      if( level < 2 )
        len = OutputKsiKey( out, keyTxtItem, len ) + OutputKsiItem( out, ")" );
      break;
    }
    case PF_POLYPOINT:
    case PF_FOOTNOTE:
    {
      int nPoints = pObjPar->polypoint.length;
      point* pPoints = (point*)pObjPar->polypoint.points;
      point* pp = pPoints + nPoints;
      *ppSrc = pp;
      if( level < 2 )
      {
        len = OutputKsiKey( out, keyPoly, len );
        int x = 0, y = 0;
        for( point const* p = pPoints ; p < pp ; ++p )
        {
          len += OutputKsiPair( out, bAbsPoly ? p->x : x += p->x,
                                     bAbsPoly ? p->y : y += p->y );
          if( p + 1 < pp )
            len = OutputKsiDlm( out, len );
        }
        len += OutputKsiItem( out, ")" );
      }
      break;
    }
    case PF_MULTILINE:
    {
      int nPoints = pObjPar->polypoint.length;
      point* pPoints = (point*)pObjPar->polypoint.points;
      point* pp = pPoints + nPoints;
      *ppSrc = pp;
      if( level < 2 )
      {
        len = OutputKsiKey( out, keyPoly, len );
        int s = 1;
        for( point const* p = pPoints ; p < pp ; ++p, s ^= 1 )
        {
          len += OutputKsiPair( out, 
                                s || bAbsPoly ? p[0].x : p[0].x + p[-1].x, 
                                s || bAbsPoly ? p[0].y : p[0].y + p[-1].y );
          if( p + 1 < pp )
            len = OutputKsiDlm( out, len );
        }
        len += OutputKsiItem( out, ")" );
      }
      break;
    }
    case PF_ELLIPSE :
    {
      rect* pr = &pObjPar->ellipse;
      *ppSrc = pr + 1;
      if( level < 2 )
      {
        len += OutputKsiCoord( out, pr->left, pr->top );
        len = OutputKsiDlm( out, len );
        len += OutputKsiSize( out, pr->right - pr->left, pr->bottom - pr->top, 0 );
      }
      break;
    }
    case PF_RECTANGLE :
    { 
      RectParam* pr = &pObjPar->rectangle;
      *ppSrc = pr + 1;
      if( level < 2 )
      {
        len += OutputKsiCoord( out, pr->left, pr->top );
        len = OutputKsiDlm( out, len );
        len += OutputKsiSize( out, pr->right - pr->left, pr->bottom - pr->top, pr->angle );
      }
      break;
    }
    case PF_ARC :
    {
      ArcParam* pr = &pObjPar->arc;
      *ppSrc = pr + 1;
      if( level < 2 )
      {
        len += OutputKsiCoord( out, pr->left, pr->top );
        len = OutputKsiDlm( out, len );
        len += OutputKsiSize( out, pr->right - pr->left, pr->bottom - pr->top, pr->angle );
        len = OutputKsiDlm( out, len );
        len += OutputKsiKeyPair( out, keyDelta, pr->delta );
      }
      break;
    }
  }
  
  if( level < 2 )
    len += OutputKsiItem( out, ")" );
    
  return len;
}
//=====================================================================
static int
IsValidIdent( int mode, ksi_obj ksi_ParamList,
              int TabNum, int minId, int maxId, CTabIdent id )
{
  int bd = !( mode & CM_FLTER_ID_DIAP ) ||
            id.Table() == TabNum  && ( !minId || id.Entry() >= minId ) && ( !maxId || id.Entry() < maxId );
  int bl = 1;    
  if( mode & CM_FLTER_ID_LIST )
  {
    ksi_obj ksi_id = ksi_cons( ksi_int2num( id.Table()), ksi_int2num( id.Entry()));
    bl = KSI_TRUE_P( ksi_assoc_ref( ksi_ParamList, ksi_id, 0 )) ||
			   KSI_TRUE_P( ksi_member( ksi_id, ksi_ParamList, 0 ));
  }
  return bd && bl;
}  
//=====================================================================
static int
IsValidDate( int mode, CTime t0, CTime t1, CTime t2 )
{
  return !( mode & CM_TIME_SELECT ) || t0 >= t1 && t0 <= t2;
}
//=====================================================================
void
MapUpdater::OutputObject( std::ofstream& out, void* pData )
{
  int t = pDrh->GetClass();
  out << "; " << pDocument->GetObjDef()->GetName( t ) << std::endl
      << "((class . " << t
      << ") (status . " << int( pDrh->GetStatus()) 
      << ") (table . " << pDrh->GetTable() 
      << ") (entry . " << pDrh->GetEntry()
      << ") (node . "  << pDrh->GetNode()
      << ")\n"; 
  void* pSrc = pData;
  OutputKsiObject( out, HR.Version, &pSrc );
  out <<  ")\n\n";
}
//=====================================================================
static ksi_obj
ChangeId( CBaseDataRec* pDrh, ksi_obj ksi_ParamList )
{
	CIdent id( pDrh->GetIdent());
  ksi_obj ksi_i = ksi_cons( ksi_int2num( id.Table()), ksi_int2num( id.Entry()));
	ksi_obj ksi_par = ksi_assoc_ref( ksi_ParamList, ksi_i, 0 );
	if( !KSI_PLIST_P( ksi_par ))
		return ksi_nil;

	ksi_i = ksi_assv_ref( ksi_par, keyTable );
	if( KSI_EINT_P( ksi_i ))
	  id.SetTable( ksi_num2int( ksi_i ));

	ksi_i = ksi_assv_ref( ksi_par, keyObject );
	if( KSI_EINT_P( ksi_i ))
	  id.SetEntry( ksi_num2int( ksi_i ));

	ksi_i = ksi_assv_ref( ksi_par, keyClass );
	if( KSI_EINT_P( ksi_i ))
	  id.SetClass( ksi_num2int( ksi_i ));

	ksi_i = ksi_assv_ref( ksi_par, keyStatus );
	if( KSI_EINT_P( ksi_i ))
	  id.SetStatus( ksi_num2int( ksi_i ));

	ksi_i = ksi_assv_ref( ksi_par, keyNode );
	if( KSI_EINT_P( ksi_i ))
	  pDrh->SetNode( ksi_num2int( ksi_i ));

	pDrh->SetIdent( id );
	return ksi_par;
}
//=====================================================================
static void
SetShapeKey( ksi_obj ksi_val, SHAPE_NDX* pSh )
{
	if( pSh->shape != SH_NOTHING )
	{
		ksi_obj ksi_cont = KSI_CAR( KSI_CDR( ksi_val ));
		int nShp = pSh->ndx;
		ASSERT( nShp < KSI_LIST_LEN( ksi_cont ));
		ksi_obj ksi_itm = ksi_list_ref( ksi_cont, KSI_MK_SINT( nShp ));
		ksi_set_cdr_x( ksi_itm, ksi_cons( ksi_cons( keyShape, ksi_true ), ksi_cdr( ksi_itm )));
	}
}
//=====================================================================
ResultCode
MapUpdater::GPL2GPL( CConvDlg* pDlg,
                     char const* txtPath, char const* errPath,
                     ksi_obj ksi_ParamList,
                     int nMode, 
                     int tableNum,
                     int minIdent, int maxIdent,
                     CTime time1, CTime time2,
                     int nNode,
                     char* pClassMap,
                     double pcf[2][3],
                     Point* Rgn, Rect const* pBox )
{
  char buf1[_MAX_PATH], buf2[_MAX_FNAME], buf3[_MAX_FNAME], buf4[_MAX_FNAME];

  char* pp = strrchr( strcpy( buf1, InitPath ), '\\' );
  pp[1] = 0;
  SetCurrentDirectory( buf1 );
	
	char TmpPath[_MAX_PATH];
  _splitpath( DataPath, buf1, buf2, buf3, buf4 );
  _makepath( TmpPath, buf1, buf2, buf3, ".tmp" );

	int bCpy = CopyFile( DataPath, TmpPath, 0 );
	if( !bCpy )
	{
		ErrBox( "Не удалось создать временную копию проекта" );
		return R_OK;
	}

	std::fstream TmpStream( TmpPath, std::ios::in| std::ios::binary, std::filebuf::sh_read );
  if( TmpStream.bad())
    return TraceError( MapNumber, R_NOFILE, TmpPath );

  std::ofstream err( errPath, std::ios::out | std::ios::trunc );
  if( !err.good())
    return TraceError( MapNumber, R_NOFILE, errPath );

  std::ofstream log;  
  ResultCode r = CreateLog( errPath, log );
  if( r != R_OK )
    return r;

  int nOut = 0, nBad = 0, nTotal = 0, nObj = 0, nErr = 0, rSize;
  int bSquizze = nMode & CM_SQUIZZE;
  CRect pr( 0, 0, HR.XSize, HR.YSize );

  CBaseDataRecPtr pNewDrh = pDrh;
  long newVersion = HR.Version;
  int bChangeVersion = 0;
  
  if(( nMode & CM_OBJECTS_CONVERT ) && bSquizze && !Rgn &&  !pClassMap &&
      !( nMode & ( CM_FLTER_ID_DIAP | CM_FLTER_ID_LIST | CM_TIME_SELECT )))    
  {
    GetLong( DataPath, "General", "Version", &newVersion, DEFVERSION );
    if( HR.Version < newVersion && AskBox( "Изменять версию проекта?" ) == IDYES )
    {
      bChangeVersion = 1;
      pNewDrh = SelectDrh( newVersion );
    }
  }

  RemoveAllIndex();
  SetModifiedFlag();

  std::streampos rPos, wPos, ePos;
  for(rPos = sizeof HR, wPos = rPos, ePos = HR.FreePos;
       rPos < ePos ; rPos += rSize )
	{
    pDlg->SetReport( nTotal, nObj, nErr );

    while( pDlg->GetStop() == 1 );
    if( pDlg->GetStop() == 2 &&
        AskBox( "Прерывание конвертации приведет к потере"
                "несконвертированных объектов.\n\n"
                "Прерывать?" ) == IDYES )
      break;

    rSize = GetRecord( rPos, &TmpStream ) == R_OK ? pDrh->GetSize() : 1;
    int wSize = rSize;
	  nTotal += rSize != 1;
		if( rSize > 1 && ( !bSquizze || pDrh->IsObjRec()))
    {
      int bNodeChanged = 0;

      if( pDrh->IsObjRec())
      {
        void* pSrc = pData;
        if( TestInclude( pClassMap, pDrh->GetClass()) &&
            IsValidIdent( nMode, ksi_ParamList, tableNum, minIdent, maxIdent, pDrh->GetTabIdent()) &&
            IsValidDate( nMode, pDrh->GetTime(), time1, time2 ) &&
            ( !pBox || pBox->IntersectRect( pDrh->GetBox())) &&
            ( !Rgn || UpdateObjectCoord( &pSrc, 0, 0, Rgn )))
        {
          if(( nMode & CM_SET_NODE ) && pDrh->GetNode() != nNode )
          {
            pDrh->SetNode( nNode );
            bNodeChanged = 1;
          }
					
         	CIdent oldId( pDrh->GetIdent());
					ksi_obj ksi_par = ChangeId( pDrh, ksi_ParamList );

          if( nMode & CM_COORD_CONVERT )
          {
            pSrc = pData;
            UpdateObjectCoord( &pSrc, pcf, 0 );
            pSrc = pData;
            CDrawObject::SetDrawnClass( oldId.Class());
            CBox nb( CalcBoundBox( HR.Version, &pSrc ));
            pDrh->SetBox( *((Rect*)&nb ));
            if( nb.left > nb.right || nb.top > nb.bottom )
              ++nBad;
            else
            if( !pr.PtInRect( nb.TopLeft()) || !pr.PtInRect( nb.BottomRight()))
              ++nOut;
          }

          if(( nMode & CM_OBJECTS_CONVERT ) || bChangeVersion )
          {
            int t = oldId.Class();

            if( !IsObject( t ))
            {
              OutputObject( err, pData );
              ++nErr;
              continue;
            }
    
            pSrc = pData;
            ksi_obj ksi_new_class  = KSI_MK_SINT( pDrh->GetClass()); 
            ksi_obj ksi_new_status = KSI_MK_SINT( pDrh->GetStatus());
            ksi_obj ksi_old_class  = KSI_MK_SINT( oldId.Class()); 
            ksi_obj ksi_old_status = KSI_MK_SINT( oldId.Status());
            ksi_obj ksi_ob = ConvertObject( HR.Version, &pSrc, 1 );

            ksi_defsym( "is-convertion?", ksi_true, ksi_current_env());
            ksi_obj ksi_vl = KsiCall( "rebuild-object",
                                       ksi_old_class, ksi_old_status,
                                       ksi_new_class, ksi_new_status,
                                       ( nMode & CM_OBJECTS_CONVERT ) ? ksi_par : ksi_nil,
																			 ksi_ob, 0 );
            ksi_defsym( "is-convertion?", ksi_false, ksi_current_env());
              
            if( KSI_LIST_P( ksi_vl ))
            {
              TGstream tmp;
              r = WriteData( 0, pDrh->GetRecPtr(), pDrh->GetRecSize(), &tmp );
              if( r != R_OK )
               return r;
        
              char* pTmpData = tmp.str() + pDrh->GetRecSize();
              void* pTmpSrc = pTmpData;

							SHAPE_NDX* pSh = &CDrawObject::m_ShapeNdx[pDrh->GetClass()];
							SetShapeKey( ksi_vl, pSh );

              r = ConvertObject( newVersion, ksi_vl, tmp );

              if( r != R_OK )
              {
                OutputObject( err, pData );
                ++nErr;
                continue;
              }

              CDrawObject::SetDrawnClass( pDrh->GetClass());
              CBox box( CalcBoundBox( HR.Version, &pTmpSrc ));

              pTmpSrc = pTmpData;
              if(( nMode & CM_MOVE ) && MoveObjectToProject( pTmpSrc, &box ))
              {
                OutputObject( err, pTmpData );
                ++nErr;
                continue;
              }

              pDrh->SetBox( *((Rect*)&box ));                
              wSize = tmp.tellp();
              pDrh->SetSize( wSize );
              pDrh->SetTime( time( 0 ));
              pDrh->SetClock( clock());
              r = WriteData( 0, pDrh->GetRecPtr(), pDrh->GetRecSize(), &tmp );
              tmp.seekp( wSize );
              GetRecord( 0, &tmp );
            }
            else
            {
              OutputObject( err, pData );
              ++nErr;
              continue;
            }
          }
        }
      }
      
      CBaseDataRecPtr tmpDrh = pDrh;
      if( bChangeVersion )
      {
        pNewDrh->CopyRec( pDrh );
        wSize = pNewDrh->GetSize();
        pDrh = pNewDrh;
      }
      
			PutRecord( wPos );

      SetModifiedFlag();
      AddIndex( wPos, pDrh->GetBox(), pDrh->GetNode(), pDrh->GetClass(), pDrh->GetTabIdent(), 0 );
      OutputLog( log, pDrh->GetEntry(), pData );
      ++nObj;
      pDrh = tmpDrh;
			
      wPos += wSize;
    }
	}

  SpaceIndex.Sort();

  err.close();
  log.close();

  if( nBad )
  {
    CString s;
    s.Format( "Переполнение при пересчете координат объектов: %d", nBad );
    ErrBox( s );
  }
  else
  if( nOut )
  {
    CString s;
    s.Format( "Вышло за габарит проекта объектов: %d", nOut );
    ErrBox( s );
  }

  pDlg->SetReport( nTotal, nObj, nErr, 1 );
 
  HR.Version = newVersion;
  HR.FreePos = wPos;
  WriteData( 0, &HR, sizeof HR, &DataStream );

  if( bChangeVersion )
  {
    pView->SetVersion( newVersion );
    pView->SetDrh();
  }

  TmpStream.close();
	CFile::Remove( TmpPath );

  pDrh = pNewDrh;

  return R_OK;
}
//=====================================================================
int
MapUpdater::BoxInRgn( Rect const& Box, Point* Rgn )
{
  for( int rs = Rgn++->x ; rs ; Rgn += rs, rs = Rgn++->x )
    if( MapStore::IsPointInPolygon( Box.LeftTop(), Rgn, rs ) &&
        MapStore::IsPointInPolygon( Box.RightTop(), Rgn, rs ) &&
        MapStore::IsPointInPolygon( Box.LeftBottom(), Rgn, rs ) &&
        MapStore::IsPointInPolygon( Box.RightBottom(), Rgn, rs ))
      return 1;
  return 0;
}
//=====================================================================
int
MapUpdater::PntInRgn( Point const& Pt, Point* Rgn )
{
  for( int rs = Rgn++->x ; rs ; Rgn += rs, rs = Rgn++->x )
    if( MapStore::IsPointInPolygon( Pt, Rgn, rs ))
      return 1;
  return 0;
}
//=====================================================================
static void
OutputMifCoord( std::ofstream& out, double v )
{
  CString b;
  b.Format( "%1.2f ", v * SmInUnit * 1e-2 );
  out << b;
}
//=====================================================================
static void
OutputMifPair( std::ofstream& out, int x, int y, double cf[][3] )
{
  CPoint p( CMapView::Project2Grid( CPoint( x, y )));
  OutputMifCoord( out, cf ? p.x * cf[0][0] + p.y * cf[0][1] + cf[0][2] : p.x );
  OutputMifCoord( out, cf ? p.x * cf[1][0] + p.y * cf[1][1] + cf[1][2] : p.y );
  out << std::endl;
}
//=====================================================================
static int
CalcPlan2Merk( CPoint sp, double mvxy[][N_REP+2] )
{
  RepInf rt[N_REP+1];

  int bRg = 0;

  if( FindNearesRepers( sp, rt ) == N_REP )
  {
    MerkatorData.InitLocal( rt[0].pPt );

	int i;
    for(i = N_REP ; --i >= 0 ; )
		{
			Point tp( MerkatorData.Pol2Dec( rt[i].pPt ));
      rt[i].dPt.x = tp.x;
      rt[i].dPt.y = tp.y;
		}

    for( i = N_REP ; --i >= 0 ; )
    {
      mvxy[i][0] = rt[i].sPt.x;
      mvxy[i][1] = rt[i].sPt.y;
      mvxy[i][2] = 1;
      mvxy[i][3] = rt[i].dPt.x;
      mvxy[i][4] = rt[i].dPt.y;
    }
    
    bRg = fabs( Djordan( mvxy[0], N_REP, N_REP + 2 )) > 1;
    if( !bRg )
      MerkatorData.DeinitLocal();
  }
  return bRg;
}
//=====================================================================
static void
Plan2Merk( double p[2], double mvxy[][N_REP+2] )
{
  p[0] = mvxy[0][0] * p[0] + mvxy[1][0] * p[1] + mvxy[2][0];
  p[1] = mvxy[0][1] * p[0] + mvxy[1][1] * p[1] + mvxy[2][1];
}
//=====================================================================
static void
OutputMifPoly( std::ofstream& out, point const* p, int np, 
               int bAbs, int bBound, int bWriteLen, double pcf[][3], int bGeo )
{
  if( bWriteLen )
    out << ( np + bBound ) << std::endl;

	if( bGeo )
  {
		double mvxy[N_REP][N_REP+2];

    int bMetr = bGeo & CM_OUTPUT_GEO_METR;
    int bGrad = bGeo & CM_OUTPUT_GEO_GRAD;
    int bTm = MerkatorData.TrueMerkator();
		int bRg = 0;

		if( !bTm )
		{
      point const* pp = p;
      int x = 0, y = 0;
      double sx = 0, sy = 0;
      for( int i = np ; --i >= 0 ; ++pp )
			{
        sx += bAbs ? pp->x : x += pp->x;
        sy += bAbs ? pp->y : y += pp->y;
			}

      CPoint sp(  sx / np, sy / np );
      bRg = CalcPlan2Merk( sp, mvxy );				
		}

		point const* pp = p; 
		int x = 0, y = 0;
    for( int i = np + bBound ; --i >= 0 ; ++pp )
    {
      if( bBound && !i )
      {
        pp = p;
        x = y = 0;
      }

			CPoint t( bAbs ? pp->x : x += pp->x, bAbs ? pp->y : y += pp->y );
			Point tp( t.x, t.y );
      Polar pc;

      if( bTm )
			{
        MerkatorData.Prj2Pol( tp, pc );
				if( bMetr )
				{
          tp = MerkatorData.Pol2Dec( pc );
					t.x = tp.x;
					t.y = tp.y;
				}
			}
			else
      if( bRg )
      {
        double dv[2] = { pp->x, pp->y };
        Plan2Merk( dv, mvxy );
				tp = Point( dv[0] , dv[1] );
				if( bGrad )
					pc = MerkatorData.Dec2Pol( tp );
				t.x = dv[0] * 1e2 / SmInUnit;
				t.y = dv[1] * 1e2 / SmInUnit;
      }

      if( bMetr )
			{
				OutputMifCoord( out, t.x );
				OutputMifCoord( out, t.y );
        out << std::endl;
			}
			else
			{
        CString b;
        b.Format( "%1.6f %1.6f", pc.lon / Deg, pc.lat / Deg );
        out << b << std::endl;
			}
    }
		if( bRg )
		  MerkatorData.DeinitLocal();
  }
  else
  {
    point const* sp = p;
    for( int x = 0, y = 0 ; --np >= 0 ; ++p )
      OutputMifPair( out, bAbs ? p->x : x += p->x, bAbs ? p->y : y += p->y, pcf );
    if( bBound )
      OutputMifPair( out, sp->x, sp->y, pcf );
  }
}
//=====================================================================
static int
OutputMifPoint( std::ofstream& out, int x, int y, double pcf[][3], int bGeo )
{
  out << "Point ";
  point pm;
  pm.x = x;
  pm.y = y;
  OutputMifPoly( out, &pm, 1, 1, 0, 0, pcf, bGeo );
  return 1;
}
//=====================================================================
static int
OutputMifPolyline( std::ofstream& out, point const* pp, int np, int bAbs, double pcf[][3], int bGeo )
{
  if( np )
  {
    out << "Pline " << np << std::endl;
    OutputMifPoly( out, pp, np, bAbs, 0, 0, pcf, bGeo );
    out << std::endl;
  }
  return np;
}
//=====================================================================
static int
OutputMifRegion( std::ofstream& out, point const* pp, int np, int bAbs, double pcf[][3], int bGeo )
{
  if( np )
  {
    out << "Region 1" << std::endl;
    OutputMifPoly( out, pp, np, bAbs, 1, 1, pcf, bGeo );
    out << std::endl;
  }
  return np;
}
//=====================================================================
static CString
CalcMifText( char const* pStr, int len )
{
	CString r;
  for(r; --len >= 0 ; ++pStr )
    if( *pStr == '\"' )
      r += "\"\"";
    else
    if( *pStr == '\n' )
      r += "\\n";
    else
      r += *pStr;
  return r;
}
//=====================================================================
static int
OutputMifText( std::ofstream& mifOut, Text* pText, double pcf[][3], int version, int bGeo )
{
  StringParam* pSP = &pText->param.full.string;
  FontParam1 FP;
  if( version < 107 )
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

  int h = abs( FP.height );
  int s = ( FP.weight > 0 ) | ( FP.style << 1 );

  int an = pSP->angle;
  int ox = pSP->offset.x;
  int oy = pSP->offset.y;

  CBox b( CPoint( 0, 0 ), CSize( 0, 0 ), 0, pSP->string, pSP->length,
          FP.number, FP.height, FP.weight, FP.style );

  CPoint p0( pText->org.x, pText->org.y );
	CPoint t1( UpdatePoint( p0 + TurnPoint( ox + b.left,  oy + b.top, an ), pcf, 0, 1 ));
  CPoint t2( UpdatePoint( p0 + TurnPoint( ox + b.right, oy + b.top, an ), pcf, 0, 1 ));
	CPoint t0( UpdatePoint( p0 + TurnPoint( ox + b.left,  oy + b.bottom, an ), pcf, 0, 1 ));

	t2 -= t1;
	t0 -= t1;
  an = atan2( t2.x, t2.y );
	t0 = t1 + TurnPoint( t0, an );
	t2 = t1 + TurnPoint( t2, an );

  mifOut << "Text " << '\"' << CalcMifText( pSP->string, pSP->length ) << '\"' << std::endl;

  point pm[] = {{ t0.x, t0.y }, { t2.x, t2.y }};
  OutputMifPoly( mifOut, pm, 2, 1, 0, 0, 0, bGeo );
  mifOut << "Angle " << ( an / 10.0 ) << std::endl
         << "Justify Left" << std::endl
         << "Font(\"" << pDocument->num2fontface( FP.number )
         << "\"," << s << ",0," << FP.color << ")" << std::endl;
  if( memchr( pSP->string, '\n', pSP->length ))
    mifOut << "Spacing 1" << std::endl;
  mifOut << std::endl;
  return 1;
}
//=====================================================================
static int
OutputMifObject( int pass, int level, std::ofstream* pMif, std::ofstream* pMid, double pcf[][3],
                 int vers, void** ppSrc, int type, int bGeo, ksi_obj ksi_map )
{ 
  PrimRecord* pPr = (PrimRecord*)*ppSrc;
  int bAbsPoly = vers & 1;
  int bVisible = pPr->type & PF_VISIBLE;
  
  int SF = PF_STROKED;
  int FF = PF_FILLED;
  int sft = pPr->type & ( SF | FF );

  Drawable* pObjPar = !bVisible          ? &pPr->param.drawable :
                      sft == ( SF | FF ) ? &pPr->param.outlined.primParam :
                      sft == SF          ? &pPr->param.stroked.primParam :
                      sft == FF          ? &pPr->param.filled.primParam :
                                           0;

  ksi_obj ksi_prt = PrimKeys->KsiTranslate( pPr->type & PF_SHAPE_MASK );

  int gt = pPr->type & PF_TYPE_MASK;
  
  static int bDone = 0;
  int txtCnt = 0;

  switch( gt )
  {
    case PF_CONTAINER:
    {
      bDone = 0;
      txtCnt = 2;
      *ppSrc = pPr->param.container.prims;
      int primNum = pPr->param.container.length;
      int cnvCnt = 0;
      for( int i = primNum ; --i >= 0 ; )
      {
        if( OutputMifObject( pass, level + 1, pMif, pMid, pcf, vers, ppSrc, type, bGeo,
                             KSI_PLIST_P( ksi_map ) ? KSI_CAR( ksi_map ) : ksi_false ))
        {
          cnvCnt = 1;
          bDone = !pass;
          if( pass )
            *pMid << '\"' << pDocument->GetObjDef()->GetName( type ) << "\"\n"; 
          if( KSI_PLIST_P( ksi_map ))
            ksi_set_car_x( ksi_map, ksi_false );
        }

        if( KSI_PLIST_P( ksi_map ) )
          ksi_map = KSI_CDR( ksi_map ); 
      }

      if( level == 0 && !pass )
      {
        if( !cnvCnt )
          *pMif << "NONE\n\n";

        while( --txtCnt >= 0 )
          *pMid << ";\"\"";
      }

      break;
    }
    case PF_POINT:
    {
      point* pp = &pPr->param.pnt;
      *ppSrc = pp + 1;
      if( level == 1 && KSI_EQ_P( ksi_map, keyPoint ) && ( pass || !bDone ))
        return OutputMifPoint( *pMif, pp->x, pp->y, pcf, bGeo );
      break;
    }
    case PF_ICON:
    {
      Icon* pi = &pPr->param.icon;
      *ppSrc = pi + 1;
      if( level == 1 && KSI_EQ_P( ksi_map, keyPoint ) && ( pass || !bDone ))
        return OutputMifPoint( *pMif, pi->x, pi->y, pcf, bGeo );
      break;
    }
    case PF_ELLIPSE :
    {
      rect* pe = &pObjPar->ellipse;
      *ppSrc = pe + 1;
      if( level == 1 && KSI_EQ_P( ksi_map, keyPoint ) && ( pass || !bDone ))
        return OutputMifPoint( *pMif, pe->left, pe->top, pcf, bGeo );
      break;
    }
    case PF_RECTANGLE :
    {
      RectParam* pr = &pObjPar->rectangle;
      *ppSrc = pr + 1;
      if( level == 1 && KSI_EQ_P( ksi_map, keyPoint ) && ( pass || !bDone ))
        return OutputMifPoint( *pMif, pr->left, pr->top, pcf, bGeo );
      break;
    }
    case PF_TEXT:
    {
      Text* pText = &pPr->param.text;
      StringParam* pSP = bVisible ? &pText->param.full.string : &pText->param.string;
      *ppSrc = pSP->string + pSP->length;
      if( level == 1 && !pass )
      {
        *pMid << ";\"" << CalcMifText( pSP->string, pSP->length ) << "\"";
        txtCnt -= txtCnt > 0;
      }
      if( level == 1 && !bDone && bVisible )
        return OutputMifText( *pMif, pText, pcf, vers, bGeo );
      break;
    }
    case PF_TEXTBYLINE:
    {
      int length = pPr->param.textbyline.length;
      TxtItem* pItem = pPr->param.textbyline.items;
      *ppSrc = pItem + length;

      if( pass )
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

        Text* pText = (Text*)new char[sizeof Text + 1];
				pText->param.full.FontParams.font1 = FP;
				pText->param.full.string.offset.x = 0;
				pText->param.full.string.offset.y = 0;
				pText->param.full.string.length = 1;
				for( int i = 0 ; i < length ; ++i )
				{
					pText->org = pItem[i].org;
					pText->param.full.string.angle = pItem[i].angle;
					pText->param.full.string.string[0] = pItem[i].letter[0];
					OutputMifText( *pMif, pText, pcf, vers, bGeo );
				}
				delete pText;
				return 1;
      }
      break;
    }
    case PF_POLYPOINT:
    case PF_MULTILINE:
    case PF_FOOTNOTE:
    {
      int nPoints = pObjPar->polypoint.length;
      point* pPoints = (point*)pObjPar->polypoint.points;
      *ppSrc = pPoints + nPoints;
      if( level == 1 && gt == PF_POLYPOINT  && ( pass || !bDone ))
        return KSI_EQ_P( ksi_map, keyFplg ) ||
               KSI_EQ_P( ksi_map, keyBpll ) ?
          OutputMifRegion( *pMif, pPoints, nPoints, bAbsPoly, pcf, bGeo ) :
               KSI_EQ_P( ksi_map, keyPolyline ) ?
          OutputMifPolyline( *pMif, pPoints, nPoints, bAbsPoly, pcf, bGeo ) : 0;
      if( level == 1 && gt == PF_FOOTNOTE && pass )
        return OutputMifPolyline( *pMif, pPoints, nPoints, bAbsPoly, pcf, bGeo );
      break;
    }
    case PF_ARC :
    {
      ArcParam* pr = &pObjPar->arc;
      *ppSrc = pr + 1;
      break;
    }
  }
  return 0;
}
//=====================================================================
static void
OutputMifHeader( std::ofstream* pOut, double* box, ksi_obj ksi_map, int bGeo )
{
  int nc = ksi_map ? 9 : 1;

  *pOut << "Version 300" << std::endl
        << "Charset \"WindowsCyrillic\"" << std::endl
        << "Delimiter \";\"" << std::endl;

	if( bGeo & CM_OUTPUT_GEO_GRAD )
    *pOut << "CoordSys Earth Projection 1, 104" << std::endl;
	else
	if( bGeo & CM_OUTPUT_GEO_METR )
	{
    CString bb;
    bb.Format( "(%1.2f,%1.2f) (%1.2f,%1.2f)", box[0], box[1], box[2], box[3] );
    *pOut << "CoordSys Earth Projection 10, 157, \"m\", 0 Bounds " << bb << std::endl;
	}
	else
	{
    CString bb;
    bb.Format( "(%1.2f,%1.2f) (%1.2f,%1.2f)", box[0], box[1], box[2], box[3] );
    *pOut << "CoordSys NonEarth Units \"m\" Bounds " << bb << std::endl;
	}
 
  *pOut << "Columns " << nc << std::endl
        << "GraphicsClassName Char(255)" << std::endl;

  if( ksi_map )
  {
    *pOut << "GraphicsClass Integer"  << std::endl
          << "GraphicsStatus Integer" << std::endl
          << "TableNumber Integer"    << std::endl
          << "TableObject Integer"    << std::endl
          << "NodeNumber Integer"     << std::endl
          << "CreationTime Char(31)"  << std::endl
          << "Text1 Char(256)"        << std::endl
          << "Text2 Char(256)"        << std::endl;
  }

  *pOut << "Data" << std::endl << std::endl;
}
//=====================================================================
ResultCode
MapUpdater::GPL2MIF( CConvDlg* pDlg,
                     char const* txtPath,
                     ksi_obj ksi_ParamList,
                     int nMode,
                     int tableNum,
                     int minIdent, int maxIdent,
                     CTime time1, CTime time2,
                     char* pClassMap,
                     double pcf[2][3],
										 Point* Rgn, Rect const* pBox )
{
  ksi_obj ksi_map_map[FULL_RANGE];
  for( int i = CELEM( ksi_map_map ) ; --i >= 0 ; ksi_map_map[i] = ksi_false );

  char bNewMap[FULL_RANGE];
  int  bNewAll = 0x3;

	memset( bNewMap, 0x3, sizeof bNewMap ); 

  int nTotal = 0, nObj = 0, nErr = 0;

	int bRestore = ( nMode & CM_RESTORE ) != 0;
  int byTable  = nMode & CM_ASSAMBLY_BY_TABLE;
  int byAll    = nMode & CM_ASSAMBLY_ALL;
  
  char miDrive[_MAX_DRIVE], miDir[_MAX_DIR], miName[_MAX_FNAME], tmp[_MAX_EXT];

  _splitpath( txtPath, miDrive, miDir, miName, tmp );

  pDlg->SetDlgItemText( IDC_CVD_TOTAL_EDIT, "Сортировка..." );
  pDlg->GetDlgItem( IDC_CVD_TOTAL_EDIT )->ShowWindow( 1 );

  IdentDirectHashTab PosTab;
  int l = CreatePosTab( &PosTab, bRestore, 1 );

  pDlg->GetDlgItem( IDC_CVD_TOTAL_EDIT )->ShowWindow( 0 );

	int bGeo = nMode & CM_OUTPUT_GEO;
  
	int bCvt = nMode & CM_COORD_CONVERT;
  
  ResultCode r = R_OK;
  for( i = 0 ; i < l &&  r == R_OK ; ++i )
  {
    while( pDlg->GetStop() == 1 );
    if( pDlg->GetStop() == 2 )
      break;

    std::streampos inPos = PosTab.m_pTab[i].m_Pos;
    r = GetRecord( inPos );
    pDlg->SetReport( nTotal++, nObj, nErr );

    if( r != R_OK || ( pDrh->IsEmpRec() ^ bRestore ) ||
        !TestInclude( pClassMap, pDrh->GetClass()) ||
        !IsValidDate( nMode, pDrh->GetTime(), time1, time2 ) ||
        !IsValidIdent( nMode, ksi_ParamList, tableNum, minIdent, maxIdent, pDrh->GetTabIdent()))
      continue;

    if( pBox && !pBox->IntersectRect( pDrh->GetBox()))
		  continue;

    void* pSrc = pData;
    if( Rgn && !UpdateObjectCoord( &pSrc, 0, 0, Rgn ))
		  continue;

  	int t = pDrh->GetClass();
  	int b = pDrh->GetTable();

    if( KSI_FALSE_P( ksi_map_map[t] ))
      ksi_map_map[t] = KsiCall( "get-contents-map", KSI_MK_SINT( t ), KSI_MK_SINT( pDrh->GetStatus()), 0 ); 

    ksi_obj ksi_map = ksi_copy_list( ksi_map_map[t] );
    if( KSI_FALSE_P( ksi_map ))
      continue;

    int ngr = 0;
    ksi_obj ksi_cur;
    for( ksi_cur = ksi_map ; !KSI_NULL_P( ksi_cur ) ; ksi_cur = KSI_CDR( ksi_cur ))
      ngr += !KSI_FALSE_P( KSI_CAR( ksi_cur ));

    if( ngr == 0 )
      continue;

    pSrc = pData;

    char mid1[_MAX_PATH], mid2[_MAX_PATH];
    CString name;
    
    if( byAll )
      name = miName;
    else
      name.Format( "%s_%c%d", miName, byTable ? 't' : 'c', byTable ? b : t );
    _makepath( mid1, miDrive, miDir, name, "mid" );

    if( byAll )
      name.Format( "%s_1", miName );
    else
      name.Format( "%s_%c%d_1", miName, byTable ? 't' : 'c', byTable ? b : t );
    _makepath( mid2, miDrive, miDir, name, "mid" );

    int nfm0 = ( byAll ? bNewAll :  bNewMap[byTable?b:t] ) & 1;
    int nfm1 = ( byAll ? bNewAll :  bNewMap[byTable?b:t] ) & 2;

    std::ofstream midFile1( mid1, std::ios::out | ( nfm0 ? std::ios::trunc : std::ios::app ));
	  if( !midFile1.good())
      return TraceError( MapNumber, R_NOFILE, mid1 );

    CTime tm( pDrh->GetTime());
    CString st( tm.Format( "\"%d.%m.%Y %H:%M:%S\"" ));
       
    midFile1 << '\"' << pDocument->GetObjDef()->GetName( pDrh->GetClass()) << '\"' << ";" 
             << pDrh->GetClass()       << ";" 
             << (int)pDrh->GetStatus() << ";" 
             << pDrh->GetTable()       << ";" 
             << pDrh->GetEntry()       << ";" 
             << pDrh->GetNode()        << ";" 
             << st;

    char mif1[_MAX_PATH], mif2[_MAX_PATH];
    if( nMode & CM_ASSAMBLY_ALL )
      name = miName;
    else
      name.Format( "%s_%c%d", miName, byTable ? 't' : 'c', byTable ? b : t );
    _makepath( mif1, miDrive, miDir, name, "mif" );
    
    if( nMode & CM_ASSAMBLY_ALL )
      name.Format( "%s_1", miName );
    else
      name.Format( "%s_%c%d_1", miName, byTable ? 't' : 'c', byTable ? b : t );
    _makepath( mif2, miDrive, miDir, name, "mif" );

    std::ofstream mifFile1( mif1, std::ios::out | ( nfm0 ? std::ios::trunc : std::ios::app ));
    if( !mifFile1.good())
      return TraceError( MapNumber, R_NOFILE, mif1 );

    double box[] = { 0, 0, HR.XSize, HR.YSize };
    
    if( bCvt )
    {
      double x0 =  pcf[1][2];
      double y0 =  pcf[1][2];
      double x1 =  HR.XSize * pcf[0][0] + pcf[0][2];
      double y1 =  HR.XSize * pcf[1][0] + pcf[1][2];
      double x2 =  HR.XSize * pcf[0][0] + HR.YSize * pcf[0][1] + pcf[0][2];
      double y2 =  HR.XSize * pcf[1][0] + HR.YSize * pcf[1][1] + pcf[1][2];
      double x3 =  HR.YSize * pcf[0][1] + pcf[0][2];
      double y3 =  HR.YSize * pcf[1][1] + pcf[1][2];

      box[0] = min( min( x0, x1 ), min( x2, x3 ));
      box[1] = min( min( y0, y1 ), min( y2, y3 ));
      box[2] = max( max( x0, x1 ), max( x2, x3 ));
      box[3] = max( max( y0, y1 ), max( y2, y3 ));
    }
    
    if( nfm0 )
    {
      if( bGeo && CM_OUTPUT_GEO_METR )
			{
				if( MerkatorData.TrueMerkator())
				{
					Point p0( box[0], box[1] );
					Point p1( box[2], box[3] );
					FPoint f0, f1;
					MerkatorData.Prj2Merk( p0, f0 );
					MerkatorData.Prj2Merk( p1, f1 );
          box[0] = f0.fx;
          box[1] = f0.fy;
          box[2] = f1.fx;
          box[3] = f1.fy;
				}
				else
				{
          double mvxy[N_REP][N_REP+2];
          if( CalcPlan2Merk( CPoint( HR.XSize/2, HR.YSize/2 ), mvxy ))
					{
            Plan2Merk( box, mvxy );
            Plan2Merk( box+2, mvxy );
            double x0 = min( box[0], box[2] );
            double x1 = max( box[0], box[2] );
            double y0 = min( box[1], box[3] );
            double y1 = max( box[1], box[3] );
            box[0] = x0;
            box[1] = y0;
            box[2] = x1;
            box[3] = y1;
            MerkatorData.DeinitLocal();
					}
        }
			}
      OutputMifHeader( &mifFile1, box, ksi_map, bGeo );
    }

    pSrc = pData;

    OutputMifObject( 0, 0, &mifFile1, &midFile1, ( nMode & CM_COORD_CONVERT ) ? pcf : 0,
                     HR.Version, &pSrc, pDrh->GetClass(), bGeo, ksi_map );

    mifFile1.close();
    midFile1 << endl;
    midFile1.close();
   
    std::ofstream mifFile2, midFile2;

    if( ngr > 1 )
    {
      mifFile2.open( mif2, std::ios::out | ( nfm1 ? std::ios::trunc : std::ios::app ));
      if( !mifFile2.good())
        return TraceError( MapNumber, R_NOFILE, mif2 );

      midFile2.open( mid2, std::ios::out | ( nfm1 ? std::ios::trunc : std::ios::app ));
      if( !midFile2.good())
        return TraceError( MapNumber, R_NOFILE, mid2 );
      
      if( nfm1 )
        OutputMifHeader( &mifFile2, box, 0, bGeo );
  
      pSrc = pData;
      OutputMifObject( 1, 0, &mifFile2, &midFile2, ( nMode & CM_COORD_CONVERT ) ? pcf : 0,
                       HR.Version, &pSrc, pDrh->GetClass(), bGeo, ksi_map );
      mifFile2.close();
      midFile2.close();
    }

    int m = ngr > 1 ? ~0x3 : ~0x1;
    bNewMap[byTable?b:t] &= m;
    bNewAll &= m;
    ++nObj;       
  }

  pDlg->SetReport( nTotal, nObj, nErr, 1 );

  return R_OK;
}
//=====================================================================
static int
OutputGeoCoord( std::ofstream& out, int nPnt, CPoint* pPnt,
                CIdent id, int bEnbound, int nMode ) 
{
	int bTm = MerkatorData.TrueMerkator();
	int bRg = 0;

  double mvxy[N_REP][N_REP+2];
  if( !bTm )
	{
    CPoint* pp = pPnt;
    int x = 0, y = 0;
    double sx = 0, sy = 0;
    for( int i = nPnt ; --i >= 0 ; ++pp )
		{
      sx += pp->x;
      sy += pp->y;
		}
		CPoint sp( sx / nPnt, sy / nPnt );
    bRg = CalcPlan2Merk( sp, mvxy );				
	}

  CPoint* pp = pPnt;
  for( int i = nPnt + bEnbound, n = 0 ; --i >= 0 ; ++pp, ++n )
  {
    Polar pc;
		CPoint pt( *pp );

		if( bEnbound && !i )
			pp = pPnt;

    if( bTm )
		{ 
			Point t( pp->x, pp->y );
      MerkatorData.Prj2Pol( t, pc );
      t = MerkatorData.Pol2Dec( pc );
			pt.x = t.x;
			pt.y = t.y;
		}
		else
    if( bRg )
    {
      double dv[2] = { pp->x, pp->y };
      Plan2Merk( dv, mvxy );
			Point t( dv[0], dv[1] );
			pc = MerkatorData.Dec2Pol( t );
			pt.x = dv[0] * 1e2 / SmInUnit;
			pt.y = dv[1] * 1e2 / SmInUnit;
    }
    
		out << id.Class() << ';' << id.Table() << ';' << id.Entry() << ';' << n << ';';

		if( nMode & CM_OUTPUT_GEO_METR )
		{
  		char dpx[32], dpy[32];
      sprintf( dpx, "%1.2f", pt.x  * 1e-2 * SmInUnit  );
      sprintf( dpy, "%1.2f", pt.y  * 1e-2 * SmInUnit  );
      out << dpx << ';' << dpy << std::endl;
		}
		else
		{
  		char lat[32], lon[32];
      sprintf( lat, "%1.6f", pc.lat / Deg );
      sprintf( lon, "%1.6f", pc.lon / Deg );
      out << lon << ';' << lat << std::endl;
		}
  }
	if( bRg )
	  MerkatorData.DeinitLocal();
  return 1;
}
//=====================================================================
static void
OutputObjectGeoCoord( std::ofstream& out, void* pData, CIdent id, int nMode ) 
{
  CDrawContainer* pCont = 0;
  void* pSrc = pData;

  pCont = pView->TranslateFromMap( &pSrc );
  if( pCont )
  {
    CDrawObject* pObj = new CDrawObject( id, 0, 0, 0, pCont );
    int nPnt, tp;
    CPoint* pPnt = pObj->GetAnyPoints( &tp, &nPnt );
    if( pPnt )
      OutputGeoCoord( out, nPnt, pPnt, id, tp >= SH_BPOLYLINE, nMode );
    delete pObj;
  }
}
//=====================================================================
ResultCode
MapUpdater::GPL2TXT( CConvDlg* pDlg,
                     char const* txtPath,
                     ksi_obj ksi_ParamList,
                     int nMode,
                     int tableNum,
                     int minIdent, int maxIdent,
                     CTime time1, CTime time2,
                     char* pClassMap, double pcf[2][3],
										 Point* Rgn, Rect const* pBox )
{
  char bNewMap[FULL_RANGE];
  int bNewAll = 1;

	memset( bNewMap, 1, sizeof bNewMap ); 
	
	int bRestore = ( nMode & CM_RESTORE ) != 0;
  int byTable  = nMode & CM_ASSAMBLY_BY_TABLE;
  int byAll    = nMode & CM_ASSAMBLY_ALL;

  char txDrive[_MAX_DRIVE], txDir[_MAX_DIR], txName[_MAX_FNAME], tmp[_MAX_EXT];

  _splitpath( txtPath, txDrive, txDir, txName, tmp );
  
  int nTotal = 0, nObj = 0;
  long vers;
  ResultCode r = GetVersion( &vers );

  std::ofstream out;
  CString curTxt( "" );

  IdentDirectHashTab PosTab;
  int l = CreatePosTab( &PosTab, bRestore, 0 );

  for( int i = 0 ; i < l &&  r == R_OK ; ++i )
  {
    while( pDlg->GetStop() == 1 );
    if( pDlg->GetStop() == 2 )
      break;

    std::streampos inPos = PosTab.m_pTab[i].m_Pos;
    r = GetRecord( inPos );
    if( r != R_OK )
      continue;

    pDlg->SetReport( nTotal++, nObj, 0 );

    if( r != R_OK || ( pDrh->IsEmpRec() ^ bRestore ) ||
        !TestInclude( pClassMap, pDrh->GetClass()) ||
        !IsValidDate( nMode, pDrh->GetTime(), time1, time2 ) ||
        !IsValidIdent( nMode, ksi_ParamList, tableNum, minIdent, maxIdent, pDrh->GetTabIdent()))
      continue;

    Rect Box( pDrh->GetBox());
    
    if(( nMode & CM_MOVE ) &&
        Box.left >= 0 &&
        Box.top  >= 0 &&
        Box.right  < HR.XSize &&
        Box.bottom < HR.YSize )
      continue;
    
    if( pBox && !pBox->IntersectRect( Box ))
		  continue; 

    void* pSrc = pData;

    if( Rgn &&
        !BoxInRgn( pDrh->GetBox(), Rgn ) &&
        !UpdateObjectCoord( &pSrc, 0, 0, Rgn ))
      continue;

    int t = pDrh->GetClass();
  	int b = pDrh->GetTable();

    CString name;
    char txt[_MAX_PATH];

    if( byAll )
      name = txName;
    else
      name.Format( "%s_%c%d", txName, byTable ? 't' : 'c', byTable ? b : t );
    _makepath( txt, txDrive, txDir, name, "txt" );

    int bNew = byAll ? bNewAll : bNewMap[byTable?b:t];

    if( curTxt != txt )
    {
      if( out.is_open())
        out.close();
      
      out.open( txt, std::ios::out | ( bNew ? std::ios::trunc : std::ios::app ));

      if( !out.good())
        return TraceError( MapNumber, R_NOFILE, txt );

      curTxt = txt;
    }

		if( nMode & CM_COORD_CONVERT )
    {
      pSrc = pData;
      UpdateObjectCoord( &pSrc, pcf, 0 );
      pSrc = pData;
		}

    if( nMode & CM_OUTPUT_GEO )
    {
      if( bNew )
				out << "CLASS_ID;TAB_ID;OBJ_ID;NUM;"   
					  << (( nMode & CM_OUTPUT_GEO_METR ) ? "X;Y" : "LONGITUTE;LATITUDE" )
					  << std::endl;
      OutputObjectGeoCoord( out, pData, pDrh->GetIdent(), nMode );
    }
    else
      OutputObject( out, pData );

    bNewAll = 0;
    bNewMap[byTable?b:t] = 0;
    if( nMode & CM_REMOVE )
      PutRecordEmp( inPos );

    ++nObj;
  }

  if( out.is_open())
    out.close();

  pDlg->SetReport( nTotal, nObj, 0, 1 );

  return R_OK;
}
//=====================================================================
int
MapUpdater::ksi2gpl( int nMode, int nNode, ksi_obj ksi_val, CIdent Id,
                     double pcf[2][3], double utm[4], std::ofstream& log, int oldEntry )
{
//KsiCall( "tk-box", ksi_val, 0 );
  if( !KSI_LIST_P( ksi_val ))
	  return -1;

  TGstream dst;
  dword mg = REPMAGIC;
  dst.write((char*)&mg, sizeof mg );

  ResultCode r = WriteData( 0, pDrh->GetRecPtr(), pDrh->GetRecSize(), &dst );
  if( r != R_OK )
	  return -1;

	int pos = dst.tellp();

  char* pData = dst.str() + pos;
  void* pSrc;

  SHAPE_NDX* pSh = &CDrawObject::m_ShapeNdx[Id.Class()];
	SetShapeKey( ksi_val, pSh );
	
  r = ConvertObject( HR.Version, ksi_val, dst );

  if( r != R_OK )
	  return -1;
              
  if( pcf && ( nMode & CM_COORD_CONVERT ) || utm && ( nMode & CM_UTM_CONVERT ))
  {
    pSrc = pData;
    UpdateObjectCoord( &pSrc, pcf, utm );

    pSrc = pData;
		ksi_obj ksi_cont = ConvertObject( HR.Version, &pSrc, 1 );

    ksi_obj ksi_val = KsiCall( "rebuild-object",
                                ksi_int2num( Id.Class()), ksi_int2num( Id.Status()),
                                ksi_int2num( Id.Class()), ksi_int2num( Id.Status()),
                                ksi_nil, ksi_cont, 0 );

		SetShapeKey( ksi_val, pSh );

		dst.seekp( pos );
    r = ConvertObject( HR.Version, ksi_val, dst );
	}

  pSrc = pData;
  CDrawObject::SetDrawnClass( Id.Class());
  CBox box( CalcBoundBox( HR.Version, &pSrc ));

  if( box.IsRectEmpty())
    return -2;

  CRect pr( 0, 0, HR.XSize, HR.YSize );
  
  if( !CRect().IntersectRect( &box, &pr ))
    return -3;

  pDrh->SetBox( *((Rect*)&box ));

  pDrh->SetMagic( OBJMAGIC );
  pDrh->SetSize( dst.tellp());
  pDrh->SetIdent( Id );
  pDrh->SetTime( time( 0 ));
  pDrh->SetClock( clock());
  pDrh->SetNode( nNode );
  WriteData( 0, pDrh->GetRecPtr(), pDrh->GetRecSize(), &dst );
  dst.seekp( pDrh->GetSize());

  int ds = pDrh->GetDataSize();

  if( nMode & CM_REMOVE_DUPLICATES )
  {
    Rect rct( pDrh->GetBox());
    TGstream src;
    int ccnt;
    r = SelectByRect( rct, pDrh->GetClass(), src, &ccnt );
    if( r == R_OK && ccnt )
    {
      char* pSrc = (char*)src.str();
      while( --ccnt >= 0 )
      {
        memcpy( pDrh->GetBriefPtr(), pSrc, pDrh->GetBriefSize());
        pSrc += pDrh->GetBriefSize();
        int ts = pDrh->GetDataSize();
        if( ts == ds && !memcmp( pData, pSrc, ds ))
          break;
        pSrc += ts;
      }
      if( ccnt >= 0 )
        return 0;
    }
  }

  r = AddObject( dst );
  OutputLog( log, oldEntry, pData );
  return r == R_BADSTREAM ? -R_BADSTREAM :
         r == R_SAFEARR_SEM_ERR ? -R_SAFEARR_SEM_ERR :
         r == R_OK;
}
//=====================================================================
static int
PutError( ksi_obj ksi_def, ksi_obj ksi_log, int ec = 0 )
{
  static char* ertx[] =
  { 0,
    ";Графический дубликат\n",
    ";Некорректный объект\n",
    ";Невидимый объект\n",
    ";Объект вне проекта\n"
  };
  if( ec && ec < CELEM( ertx ))
    ksi_display( ksi_str02string( ertx[abs(ec)] ), ksi_log );
  OutputKsiObject( ksi_def, ksi_log );
  ksi_newline( ksi_log );
  ksi_newline( ksi_log );
  return ec ? -1 : 0;
}
//=====================================================================
int
MapUpdater::UpdateKsiObjDef( ksi_obj ksi_def, ksi_obj ksi_log,
														 ksi_obj ksi_ParamList,
                             int nMode, int nNode,
                             ksi_obj ksi_class_cnv_proc,
                             ksi_obj ksi_table_cnv_proc,
                             ksi_obj ksi_node_cnv_proc,
                             char* pClassMap,
														 double pcf[2][3],
														 double utm[4],
                             std::ofstream& log )
{

	CSize ps( pView->GetDocument()->GetSize());
	Point pt( ps.cx /= 2, ps.cy /= 2 );
	KsiCall( "set-obj-scale-factor!", ksi_double2num( MerkatorData.ScaleFactor( pt )), 0 );

  if( !KSI_LIST_P( ksi_def ))
    return PutError( ksi_def, ksi_log );

  ksi_obj ksi_class  = ksi_assv_ref( ksi_def, ksi_keyword2symbol( keyClass ));
  ksi_obj ksi_status = ksi_assv_ref( ksi_def, ksi_keyword2symbol( keyStatus ));
  ksi_obj ksi_old_class  = ksi_class;
  ksi_obj ksi_old_status = ksi_status;
  
	ksi_obj ksi_table  = ksi_assv_ref( ksi_def, ksi_keyword2symbol( keyTable ));
  ksi_obj ksi_entry  = ksi_assv_ref( ksi_def, ksi_keyword2symbol( keyEntry ));
  ksi_obj ksi_node   = ksi_assv_ref( ksi_def, ksi_keyword2symbol( keyNode ));

	ksi_obj ksi_par = ksi_nil;
  
  if( nMode & CM_FLTER_ID_LIST )
  {
    ksi_obj ksi_id = ksi_cons( ksi_table, ksi_entry );
		ksi_obj ksi_pr = ksi_assoc_ref( ksi_ParamList, ksi_id, 0 );
    if( KSI_FALSE_P( ksi_pr ) && KSI_FALSE_P( ksi_member( ksi_id, ksi_ParamList, 0 )))
		  return 0;

		if( nMode & CM_OBJECTS_CONVERT )
		{
			ksi_par = ksi_pr;

    	ksi_obj ksi_i = ksi_assv_ref( ksi_par, keyTable );
	    if( KSI_EINT_P( ksi_i ))
	      ksi_table = ksi_i;

	    ksi_i = ksi_assv_ref( ksi_par, keyObject );
	    if( KSI_EINT_P( ksi_i ))
	      ksi_entry = ksi_i;

	    ksi_i = ksi_assv_ref( ksi_par, keyClass );
	    if( KSI_EINT_P( ksi_i ))
	      ksi_class = ksi_i;

  	  ksi_i = ksi_assv_ref( ksi_par, keyStatus );
	    if( KSI_EINT_P( ksi_i ))
	      ksi_status = ksi_i;

	    ksi_i = ksi_assv_ref( ksi_par, keyNode );
	    if( KSI_EINT_P( ksi_i ))
	      ksi_node = ksi_i;
		}
  }
	
	if( KSI_FALSE_P( ksi_node ))
    ksi_node = ksi_zero;

  if( ksi_procedure_p( ksi_node_cnv_proc ))
  {
     ksi_obj ksi_vl = ksi_apply_1_with_catch( ksi_node_cnv_proc, ksi_node );
     if( KSI_EINT_P( ksi_vl ))
       ksi_node = ksi_vl;
  }

  if( !KSI_EINT_P( ksi_node  ) ||
      !KSI_EINT_P( ksi_table ) ||
      !KSI_EINT_P( ksi_entry ) ||
      !KSI_EINT_P( ksi_class ) ||
      !KSI_EINT_P( ksi_status ))
    return PutError( ksi_def, ksi_log );

  if( ksi_procedure_p( ksi_class_cnv_proc ))
  {
     ksi_obj ksi_vl = ksi_apply_2_with_catch( ksi_class_cnv_proc, ksi_class, ksi_status );
     if( KSI_PAIR_P( ksi_vl ))
     {
       ksi_obj p0 = KSI_CAR( ksi_vl );
       ksi_obj p1 = KSI_CDR( ksi_vl );
       if( KSI_EXACT_P( p0 ))
         ksi_class = p0;
       if( KSI_EXACT_P( p1 ))
          ksi_status = p1;
     }
  }

  int tclass  = ksi_num2int( ksi_class ); 
  int tstatus = ksi_num2int( ksi_status );

  if( !TestInclude( pClassMap, tclass ))
    return 0;

  if( !( nMode & CM_SET_NODE ))
    nNode = ksi_num2int( ksi_node );
  
  if( ksi_procedure_p( ksi_table_cnv_proc ))
  {
    ksi_obj ksi_vl = ksi_apply_2_with_catch( ksi_table_cnv_proc, ksi_table, ksi_entry );
    if( KSI_PAIR_P( ksi_vl ))
    {
      ksi_obj p0 = KSI_CAR( ksi_vl );
      ksi_obj p1 = KSI_CDR( ksi_vl );
      if( KSI_EXACT_P( p0 ))
        ksi_table = p0;
      if( KSI_EXACT_P( p1 ))
        ksi_entry = p1;
    }
  }

  int ttable = ksi_num2int( ksi_table );
  int tentry = ksi_num2int( ksi_entry );

  CTabIdent tabId( ttable, tentry );
  CIdent Id( tclass, tstatus, ttable, tentry );

  int bFound = FindObjectId( &tabId, 1, 0 ) == R_OK;
  int bWrite = !bFound || ( nMode & CM_CHANGE ) || ( nMode & CM_CREATE_NEW );

  if( bFound && ( nMode & CM_CREATE_NEW ))
    Id.SetEntry( TabIdentIndex.GetUniId( ttable ));
        
  if( !bWrite && !( nMode & CM_AUTOBUILD ))
    return 0;

  if( KSI_TRUE_P( ksi_def ) && ( nMode & CM_IMPROVE ))
    KsiCall( "improve-coords", ksi_def, 0 );

  ksi_obj ksi_cont = ksi_assv( keyCont, ksi_def );
  ksi_defsym( "is-convertion?", ksi_true, ksi_current_env());

	//KsiCall( "tk-box", ksi_cont, 0 );
  ksi_obj ksi_val = KsiCall( "rebuild-object",
                              ksi_old_class, ksi_old_status,
                              ksi_class, ksi_status,
                              ksi_par, ksi_cont, 0 );
  ksi_defsym( "is-convertion?", ksi_false, ksi_current_env());
//KsiCall( "tk-box", ksi_val, 0 );

  int nObj = 0, cr = 1;
  
  if( bWrite )
  {
    cr = ksi2gpl( nMode, nNode, ksi_val, Id, pcf, utm, log, tentry );
    if( cr == 1 )
      ++nObj;
  }

  if( cr == -R_BADSTREAM || cr == -R_SAFEARR_SEM_ERR )
    return cr;

  if( cr <= 0 )
    return PutError( ksi_def, ksi_log, 1 - cr );

  if( nMode & CM_AUTOBUILD )
	{
  	ksi_obj ksi_lst = KsiCall( "build-in-context", ksi_class, ksi_status, ksi_val, 0 );
	  if( KSI_LIST_P( ksi_lst ))
		{
			while( !KSI_NULL_P( ksi_lst ))
			{
 			  ksi_obj ksi_val = KSI_CAR( ksi_lst );
				ksi_lst = KSI_CDR( ksi_lst );
				if( KSI_LIST_LEN( ksi_val ) != 3 )
  				continue;
	  		
        ksi_class = KSI_CAR( ksi_val );
		  	ksi_status = KSI_CAR( KSI_CDR( ksi_val ));
			  ksi_cont = KSI_CAR( KSI_CDR( KSI_CDR( ksi_val )));
        ksi_cont = KsiCall( "create-object",
                            ksi_class, ksi_status,
                            ksi_cont, 0 );
        CIdent Id( ksi_num2int( ksi_class ), ksi_num2int( ksi_status ), 0, TabIdentIndex.GetUniId( 0 ));
        nObj += cr = ksi2gpl( nMode, nNode, ksi_cont, Id, pcf, utm, log, Id.Entry());
        if( cr == -1 )
          return -R_BADSTREAM;
      }
	  }
  }
  return nObj;
}
//=====================================================================
ResultCode
MapUpdater::TXT2GPL( CConvDlg* pDlg,
                     char const* txtPath, char const* errPath,
										 ksi_obj ksi_ParamList,
                     int nMode, int nNode, char* pClassMap,
										 double pcf[2][3], double utm[4] )
{
  if( _access( txtPath, 0 ) < 0 )
    return TraceError( MapNumber, R_NOFILE, txtPath );

  ksi_obj ksi_log;

  std::ofstream tstStream( errPath, std::ios::out | std::ios::trunc );
  if( tstStream.good())
  {
    tstStream.close();
    ksi_log = ksi_open_file( ksi_str02string( errPath ), ksi_str02string( "wt" ));
  }
  else
    return TraceError( MapNumber, R_NOFILE, errPath );

  std::ofstream log;  
  ResultCode r = CreateLog( errPath, log );
  if( r != R_OK )
    return r;

  if( nMode & ( CM_CLASS_CONVERT | CM_TABLE_CONVERT ))
    ksi_load( ksi_str02string( "mole_cnv" ));

  ksi_obj ksi_class_cnv_proc = ( nMode & CM_CLASS_CONVERT ) ? 
    ksi_eval_str_with_catch( "class-cnv" ) : ksi_false;

  ksi_obj ksi_table_cnv_proc = ( nMode & CM_TABLE_CONVERT ) ?
    ksi_eval_str_with_catch( "table-cnv" ) : ksi_false;

  ksi_obj ksi_node_cnv_proc = ( nMode & CM_NODE_CONVERT ) ?
    ksi_eval_str_with_catch( "node-cnv" ) : ksi_false;

  ksi_obj in_port = ksi_open_file( ksi_ptr02string( txtPath ), ksi_ptr02string( "rt" ));

  r = R_OK;
  int nErr = 0, nObj = 0, nTotal = 0;

  while( r == R_OK )
  {
    while( pDlg->GetStop() == 1 );
    if( pDlg->GetStop() == 2 )
      break;

    ksi_obj ksi_def = ksi_read( in_port );

    if( KSI_TRUE_P( ksi_eof_object_p( ksi_def )))
      break;

    pDlg->SetReport( nTotal++, nObj, nErr );

    int cr = UpdateKsiObjDef( ksi_def, ksi_log, 
			                        ksi_ParamList,
              			          nMode, nNode,
                              ksi_class_cnv_proc,
                              ksi_table_cnv_proc,
                              ksi_node_cnv_proc,
                              pClassMap, pcf, utm, log );
    if( cr == -R_BADSTREAM )
      r = R_BADSTREAM;
    else
    if( cr < 0 )
      ++nErr;
    else
      nObj += cr;
  }

  ksi_close_port( ksi_log );
  ksi_close_port( in_port );
  log.close();

  pDlg->SetReport( nTotal, nObj, nErr, 1 );

  return r;
}
//=====================================================================
static char*
ExtractSubs( char* rp, char dlm, int ns )
{
  static char sp[] = " \t", un[] = ".", e[] = "", q[] = "\\\"";
  static char tmp[FULL_RANGE];
  char* wp = tmp;  

  struct branch
  {
    int   NextState; // -1 -- Good stop ; -2 -- Bad stop
    char* Prefix;    // "" -- Copy char
  };
  #define B -2
  #define G -1

  static branch ST[][5] = 
  { 
 // '\0'       ';'         '\"'     ' '     'a'
    {{ B, 0 }, { G, e }, { 2, e }, { 0, e }, { 1, 0 }}, /* 0 */
    {{ G, e }, { G, e }, { B, 0 }, { 1, 0 }, { 1, 0 }}, /* 1 */
    {{ B, 0 }, { 2, 0 }, { 3, e }, { 2, 0 }, { 2, 0 }}, /* 2 */
    {{ G, e }, { G, e }, { 2, q }, { 4, e }, { B, 0 }}, /* 3 */
    {{ G, e }, { G, e }, { B, 0 }, { 4, e }, { B, 0 }}  /* 4 */
  };


  while( --ns >= 0 )
  {
    wp = tmp;
    for( int s = 0 ;; )
    {
      char c = *rp++;
      int t = c == 0 ? 0 : c == dlm ? 1 : c == '\"' ? 2 : strchr( sp, c ) ? 3 : 4;
      branch* bp = &ST[s][t];
      s = bp->NextState;
      if( s == B )
        return un;
      if( bp->Prefix )
      {
        strcpy( wp, bp->Prefix );
        wp += strlen( bp->Prefix );
      }
      else
        *wp++ = c;
      if( s == G )
        break;
    }
  }
  return wp == tmp ? un : tmp;
}
//=====================================================================
static enum KEY_WORD
{
  KW_NONE,
  KW_REGION,
  KW_PLINE, 
  KW_LINE, 
  KW_POINT, 
  KW_TEXT, 
  KW_RECT, 
  KW_ELLIPSE,
  KW_ARC,
  KW_ROUNDRECT,
  KW_COLLECTION
};
//=====================================================================
static KEY_WORD
Keyword2num( CString const& s )
{
  static char* ttl[] =  
  { 
    "none",
    "region", 
    "pline", 
    "line", 
    "point", 
    "text", 
    "rect", 
    "ellipse",
    "arc",
    "roundrect",
    "collection"
  };
  for( int i = CELEM( ttl ) ; --i >= 0 && s != ttl[i] ; );
  return KEY_WORD( i );
}
//=====================================================================
static int
FindKey( std::ifstream& Stream, char const* key, int bMess = 0 )
{
  Stream.seekg( 0 );
  Stream.clear( 0 );
  char tst[FULL_RANGE];
  CString s;

  while( !Stream.eof() && s != key )
  {
     Stream >> tst;
     s = tst;
     s.MakeLower();
  }
  if( Stream.eof() && bMess )
  {
    CString t( "Ошибка в заголовке файла:\n"
               "не найдено предложение - " );
    ErrBox( t + key );
  }
  
  return !Stream.eof();
}
//=====================================================================
void
ksi_add_item( ksi_obj lst, ksi_obj item )
{
  ksi_set_cdr_x( ksi_last_pair( lst ), KSI_LIST1( item ));
}
//=====================================================================
void
ksi_add_point( ksi_obj lst, CPoint p )
{
  ksi_add_item( lst, KsiPoint( p ));
}
//=====================================================================
static ksi_obj
MakePref( int cl, int st, int tb, int en, int nd )
{
  static ksi_obj class_key  = ksi_str02sym( "class"  );
  static ksi_obj status_key = ksi_str02sym( "status" );
  static ksi_obj table_key  = ksi_str02sym( "table"  );
  static ksi_obj entry_key  = ksi_str02sym( "entry"  );
  static ksi_obj node_key   = ksi_str02sym( "node"   );

  return KSI_LIST5( ksi_cons( class_key,  ksi_int2num( cl )),
                    ksi_cons( status_key, ksi_int2num( st )),
                    ksi_cons( table_key,  ksi_int2num( tb )),
                    ksi_cons( entry_key,  ksi_int2num( en )),
                    ksi_cons( node_key,   ksi_int2num( nd )));
}
//=====================================================================
static struct SubParam
{
  int SubNum;
  int xVal;
  int yVal;
  int aVal;
  SubParam() : SubNum( 0 ), xVal( 0 ), yVal( 0 ), aVal( 0 ) {}
};
//=====================================================================
static ksi_obj
MakePost( ksi_obj ksi_def, char* mid = 0, SubParam* psp = 0, double pcf[2][3] = 0, int nSub = 0, char dlm = 0,
          CPoint cp = CPoint( 0, 0 ), int fs = 0 )
{
  if( nSub && mid )
  {
    fs = abs( fs );
    int x, y = cp.y - fs / 2;
    for( int i = 0 ; i < nSub ; ++i )
    {
      CString Subs = "\n";
      x = cp.x - fs * Subs.GetLength() / 4;
      y += fs;
      ksi_obj ksi_pl = KSI_LIST2( keyPoly, ksi_cons( ksi_int2num( x ), ksi_int2num( y )));
      if( psp )
      {
        SubParam* ps = psp + i;
        Subs = ExtractSubs( mid, dlm, ps->SubNum );
        if( ps->xVal && ps->yVal )
        {
          double xx = atof( ExtractSubs( mid, dlm, ps->xVal ));
          double yy = atof( ExtractSubs( mid, dlm, ps->yVal ));
          CPoint p( UpdatePoint( xx, yy, pcf ));
          x = p.x;
          y = p.y;
          ksi_obj ksi_pl = KSI_LIST2( keyPoly, ksi_cons( ksi_int2num( x ), ksi_int2num( y )));
        }
        if( ps->aVal )
        {
          int a = 10 * atoi( ExtractSubs( mid, dlm, ps->aVal ));
          int x1 = x + 1000 * cos( a ), y1 = y - 1000 * sin( a );
          ksi_pl = KSI_LIST3( keyPoly, ksi_cons( ksi_int2num( x ), ksi_int2num( y )), 
                                       ksi_cons( ksi_int2num( x1 ), ksi_int2num( y1 )));
        }
      }
      ksi_obj ksi_tl = 
        KSI_LIST3( keyText, ksi_pl,
                   ksi_cons( keyString, ksi_str02string( Subs )));
      ksi_add_item( ksi_def, ksi_tl );
    }
  }
  return ksi_def;
}
//=====================================================================
typedef CList<CPoint*,CPoint*&> PtlList;
//=====================================================================
typedef CList<RegionDef*,RegionDef*&> RegionList;
//=====================================================================
ResultCode 
MapUpdater::MIF2GPL( CConvDlg* pDlg,
                     char const* mifPath, char const* errPath,
                     int nMode, int nNode, 
                     int classNumber, int statusNumber, 
                     int tableNumber, int entryNumber,
                     CString subsNumber,
                     int keyNumber, CString keyStr,
                     double pcf[2][3], int decomposeSize )
{
  CWaitCursor wc;

  SHAPE_TYPE shape = CDrawObject::GetShapeType( classNumber );
  
  int bNeedRegion = shape == SH_POLYGON;
  int bNeedBPline = shape == SH_BPOLYLINE;
  int bNeedPline  = shape == SH_POLYLINE;
  int bNeedAnyLns = 1 || bNeedRegion || bNeedBPline || bNeedPline;

  ResultCode r = R_OK;
  int mMode = nMode & ~CM_IMPROVE & ~CM_COORD_CONVERT;
  if( !( nMode & CM_SET_NODE ))
    nNode = 0;
  
  OFSTRUCT ofs;
  if( OpenFile( mifPath, &ofs, OF_EXIST ) < 0 )
  {
    ErrBox( "Не найден MIF-файл!" );
    return R_OK;
  }

  char iv[_MAX_DRIVE], ip[_MAX_DIR], in[_MAX_FNAME], ie[_MAX_EXT];
  char midPath[_MAX_PATH];
  char mndPath[_MAX_PATH];
  _splitpath( mifPath, iv, ip, in, ie );
  _makepath( midPath, iv, ip, in, ".mid" );
  _makepath( mndPath, iv, ip, in, ".mnd" );

  std::ifstream mifstr( mifPath, std::ios::in, std::filebuf::sh_read );

  ksi_obj ksi_log;

  std::ofstream tstStream( errPath, std::ios::out | std::ios::trunc );
  if( tstStream.good())
  {
    tstStream.close();
    ksi_log = ksi_open_file( ksi_str02string( errPath ), ksi_str02string( "wt" ));
  }
  else
    return TraceError( MapNumber, R_NOFILE, errPath );

  std::ofstream log;  
  r = CreateLog( errPath, log );
  if( r != R_OK )
    return r;

  char tst[FULL_RANGE];
  char mid[FULL_RANGE];
  CString s;
  char dlm = ';';
  int  clm = 0; 

  if( !( nMode & CM_COORD_CONVERT ))
  {
    double k = 100;
    if( FindKey( mifstr, "units" ))
    {
      mifstr >> tst;
      int l = strlen( tst );
      if( l > 2 && tst[0] == '\"' && tst[l-1] == '\"' )
      {
        static CString un[] = { "cm", "dm", "m" };
        static double  cf[] = {   1,   10, 100, 100 };
        tst[l-1] = 0;
        for( int i = 0 ; i < CELEM( un ) && un[i] != tst + 1 ; ++i );

        long SmInUnit;
        GetUnitDec( &SmInUnit );
        if( SmInUnit <= 0 )
          SmInUnit = 1;

        k = cf[i] / SmInUnit;
      }
    }
    pcf[0][0] = pcf[1][1] = k;
    pcf[0][1] = pcf[0][2] = pcf[1][0] = pcf[1][2] = 0;
  }
  
  if( FindKey( mifstr, "delimiter" ))
  {
    mifstr >> tst;
    dlm = tst[1];
  }

  std::ifstream midstr;
  std::ofstream mndstr;

  SubParam SN[8];
  int nSub = 0;
  char const sdl[] = ",;";
  char cop[256];

  strncpy( cop, subsNumber, sizeof cop );
  char* np;
  char* p = strtok( cop, sdl );
  while( p && nSub < CELEM( SN ))
  {
    np = strtok( 0, sdl );

    SubParam* ps = &SN[nSub++];

    if( *p == '(' )
    {
      char* e = strchr( p, ')' );
      if( !e || np && e > np )
      {
        ErrBox( "Некорректный формат данных подписей!" );
        return R_OK;
      }
      *e = 0;
      sscanf( p + 1, "%d%d%d%d", &ps->SubNum, &ps->xVal, &ps->yVal, &ps->aVal );
    }
    else
      ps->SubNum = atoi( p );

    p = np;
  }
  
  int bHasMid = OpenFile( midPath, &ofs, OF_EXIST ) >= 0;
  int bUseKey = nMode & CM_KEY_SELECT; 

  if(( bUseKey || nSub > 0 ) && !bHasMid )
  {
    ErrBox( "Не найден MID-файл!" );
    return R_OK;
  }

  if( bHasMid )
  {
    midstr.open( midPath, std::ios::in, std::filebuf::sh_read );
    mndstr.open( mndPath, std::ios::out| std::ios::trunc );
  }

  if( FindKey( mifstr, "columns" ))
    mifstr >> clm;

  if( bUseKey && keyNumber > clm )
  {
    CString b;
    b.Format( "Отсутствует поле №%d в MID-файле!", keyNumber );
    ErrBox( b );
    return R_OK;
  }

  CStringArray Keys;
  if( bUseKey )
  {
    char const sdl[] = ";|";
    strncpy( cop, keyStr, sizeof cop );
    int nSub = 0;
    for( char* p = strtok( cop, sdl ) ; p ; p = strtok( 0, sdl ))
      Keys.Add( p );
  }

  int i;
  for( i = nSub ; --i >= 0 ; )
  {
    CString a ("В MID-файле отсутствует поле " );
    CString b;
    if( SN[i].SubNum > clm )
      b.Format( "%d", SN[i].SubNum );
    else
    if( SN[i].xVal > clm )
      b.Format( "%d", SN[i].xVal );
    else
    if( SN[i].yVal > clm )
      b.Format( "%d", SN[i].yVal );
    else
    if( SN[i].aVal > clm )
      b.Format( "%d", SN[i].aVal );
    
    if( !b.IsEmpty())
    {
      ErrBox( a + b );
      return R_OK;
    }
  }

  if( !( nMode & CM_NUMERATE_FROM ) && entryNumber > clm )
  {
    CString b;
    b.Format( "Отсутствует поле №%d в MID-файле!", entryNumber );
    ErrBox( b );
    return R_OK;
  }

  mifstr.getline( tst, sizeof tst );

  mndstr << "TG_ID" << dlm;
  for( i = clm ; --i >= 0 ; ) 
  {
    mifstr.getline( tst, sizeof tst );
    char w[256];
    sscanf( tst, "%s", w );    
    mndstr << w;
    if( i )
      mndstr << dlm;
  }  
  mndstr << std::endl;

  if( !FindKey( mifstr, "data", 1 ))
    return R_OK;
  
  int ObjCnt = 0;

  int fieldNum = entryNumber;
  int ColCnt = 0;

  int nErr = 0, nObj = 0, nTotal = 0;
  int kwn = -1;

  int fontSize = 0;
  ksi_obj ksi_cln = KSI_MK_SINT( classNumber );
  ksi_obj ksi_fsk = ksi_str02key( "font-size" );
  for( i = 2 ; --i >= 0 && fontSize == 0 ; )
  {
    ksi_obj par_lst = ksi_eval_str( i ? "abstract-param-list" : "concret-param-list" );
    if( !KSI_PLIST_P( par_lst ))
      continue;
    ksi_obj ksi_par = ksi_assv_ref( par_lst, ksi_cln );
    if( !KSI_PLIST_P( ksi_par ))
      continue;
    ksi_par = ksi_assv_ref( ksi_par, ksi_fsk );
    if( KSI_NUM_P( ksi_par ))
      fontSize = ksi_num2int( ksi_par );    
  }

  while( r == R_OK && !mifstr.eof())
  {
    while( pDlg->GetStop() == 1 );
    if( pDlg->GetStop() == 2 )
      break;

    if( kwn < 0 )
    {
      mifstr >> tst;
      s = tst;
      s.MakeLower();
    }

    if( mifstr.eof())
      break;

    int i = kwn < 0 ? Keyword2num( s ) : kwn;
    kwn = -1;

    if( i >= 0 && bHasMid )
    {
      if( bUseKey || ColCnt == 0 )
        midstr.getline( mid, sizeof mid );

      if( bUseKey )
      {
        char* key = ExtractSubs( mid, dlm, keyNumber );
		int i;
        for(i = Keys.GetSize() ;  --i >= 0 && Keys[i] != key ; );
        if( i < 0 )
          continue;
      }
      
      if( ColCnt == 0 )
      {
        if( !( nMode & CM_NUMERATE_FROM ))
          entryNumber = atoi( ExtractSubs( mid, dlm, fieldNum ));
        ColCnt = 1;
      }

      --ColCnt;
    }

    if( tableNumber == 0 )
      entryNumber = TabIdentIndex.GetUniId( 0 );

    ksi_obj ksi_def, ksi_def_lst = ksi_nil;

    switch( i )
    {
      default:
        continue;

      case KW_NONE :
        ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
        ksi_def = MakePost( ksi_def, mid, SN, pcf, 0, dlm );
        ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
        break;

      case KW_REGION :
      {
        int npl, np = 0;
        mifstr >> npl;

        if( npl == 0 )
          break;

        RegionList rgns;
        RegionDef* rgnc;
        RgnPool mrgn, irgn, ssl;

        for( i = 0 ; i < npl ; ++i )
        {
          int npt;
          mifstr >> npt;

					if( mifstr.eof())
					{
						npl = i;
						break;
					}
          
          CPoint* pp = (CPoint*)new CPoint[npt+1];
          CPoint* p0 = pp;
          *pp++ = CPoint( npt, 0 );
          if( npt == 1 )
          {
            mifstr >> tst;
            if( _stricmp( tst, "point" ))
             {
               double x, y;
               x = atof( tst );
               mifstr >> y;
               *pp++ = UpdatePoint( x, y, pcf );
               --npt;
            }
          }
          while( --npt >= 0 )
          {
            double x, y;
            mifstr >> x >> y;
            CPoint tp( UpdatePoint( x, y, pcf ));
            if( pp == p0 + 1 || pp[-1] != tp )
              *pp++ = tp;
            else
              p0->x -= 1;
          }

          p0->x = ImprovePoly( p0 + 1, p0->x - ( p0[1] == pp[-1] ));
          if( !p0->x )
            continue;

          int h = pDlg->GetDlgItemInt( IDC_CVD_IMPROVE_POLY_PARAM_EDIT );          
          if(( bNeedRegion || bNeedBPline ) && ( nMode & CM_IMPROVE ))
          {
            int n = p0->x;
            int m = DecimatePoly( p0 + 1, n, h ? h : 250 );
            p0->x = m;
            if( m < n )
            {
              CPoint* np = new CPoint[m+1];
              memcpy( np, p0, n + 1 );
              delete p0;
              p0 = np;
            }
          }

          rgnc = new RegionDef( p0 );

          if( !rgnc->nCw || rgnc->IsRectEmpty())
            delete rgnc;
          else
            rgns.AddTail( rgnc );
        }

        npl = rgns.GetCount();
        if( !npl )
          continue;

        if( bNeedRegion || bNeedBPline )
        {
          int NPL = npl;

          if( bNeedRegion )
          {
            POSITION pos;
            CWaitCursor wc;

            int NP = ( npl - 1 ) * 2; 
            int bDD = 0;

            for( pos = rgns.GetHeadPosition() ; !bDD && pos ;
                 NP += rgns.GetNext( pos )->pPoly->x, bDD = NP >= 65536 );
          
            if( bDD )
              nMode |= CM_SEPARATE_POLY;

            mrgn.AddRgn( rgns.RemoveHead());
            for( pos = rgns.GetHeadPosition() ; pos ; irgn.AddRgn( rgns.GetNext( pos )));

            if(( nMode & CM_SEPARATE_POLY ))
            {
              // splitting by inners
              while( rgnc = mrgn.RemoveHead())
              {              
                pDlg->SetDecomp( mrgn.Size(), ssl.Size());
								if( rgnc->SplitByInner( irgn, mrgn ))
								  delete rgnc;
								else
									ssl.AddRgn( rgnc );
              }
              
							// updating crosses
							for( RegionDef** ppr = irgn.GetHeadPtr() ; *ppr ; )
              {
 								RegionDef* pr = *ppr;
                RgnPool trgn;
                pDlg->SetDecomp( irgn.Size(), ssl.Size());
                while( rgnc = ssl.RemoveHead())
								{
									if( rgnc->SubtractCrossed( pr, trgn ))
                    delete rgnc;
									else
										trgn.AddRgn( rgnc );
								}
                ssl = trgn;
                if( pr->pPoly->y )
								{
									*ppr = pr->pNext;
									irgn.DecSize();
                  delete pr;
								}
								else
									ppr = &pr->pNext;
              }

              // splitting by size
              if( decomposeSize )
							{
                decomposeSize = max( 64, decomposeSize );
                while( rgnc = ssl.GetNext())
                  if( rgnc->pPoly->x > decomposeSize && rgnc->Split( ssl ))
                    delete rgnc;
                  else
                    mrgn.AddRgn( rgnc );
							}
							else
								mrgn = ssl;

							while( rgnc = irgn.RemoveHead())
                mrgn.AddRgn( rgnc );
            }
						else
						{
							// updating inners
              while( rgnc = mrgn.RemoveHead())
							{ 
							  rgnc->SubtractInner( irgn );
                pDlg->SetDecomp( ssl.Size(), mrgn.Size());
                ssl.AddRgn( rgnc );
							}

							mrgn = ssl;

							while( rgnc = irgn.RemoveHead())
                mrgn.AddRgn( rgnc );
            }

            NPL = npl = mrgn.Size();
          }
          else
            while( !rgns.IsEmpty())
              mrgn.AddRgn( rgns.RemoveHead());

          pDlg->SetDecomp( 0, 0 );

          for( i = 0 ; rgnc = mrgn.RemoveHead() ; ++i )
          {
            if( rgnc->pPoly )
            {
              ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
              ksi_obj ksi_lst = KSI_LIST1( keyPoly );
              CPoint* pp = rgnc->pPoly;
              int npt = (*pp++).x;
              CBox box;
              np = npt;
              while( --npt >= 0 )
              {
                box |= *pp;
                ksi_add_point( ksi_lst, *pp++ );
              }
            
              ksi_add_item( ksi_def, KSI_LIST2( keyFplg, ksi_lst ));
              ksi_def = MakePost( ksi_def, mid, i == 0 ? SN : 0, pcf, nSub, dlm, box.CenterPoint(), fontSize );
              ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
 
              if( --NPL && bHasMid )
                mndstr << entryNumber++ << dlm << mid << std::endl;
            }
            delete rgnc;
          }
        }
        else
        {
          ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
          ksi_obj ksi_lst = KSI_LIST1( keyPoly );
          rgnc = rgns.RemoveHead();
          CPoint* pp = rgnc->pPoly;
          int npt = (*pp++).x;
          CBox box;
          np = npt;
          while( --npt >= 0 )
          {
            box |= *pp;
            ksi_add_point( ksi_lst, *pp++ );
          }
          ksi_add_item( ksi_def, KSI_LIST2( keyFplg, ksi_lst ));
          ksi_def = MakePost( ksi_def, mid, SN, pcf, nSub, dlm, box.CenterPoint(), fontSize );
          ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
          delete rgnc;
        }
      }
      break;
      
      case KW_PLINE :
      {
        int npl = 1;
        int npt = 0;
        char b[1024];
        mifstr >> b;
        s = b;
        s.MakeLower();

        if( s == "multiple" )
          mifstr >> npl >> npt;
        else
          npt = atoi( b );

        for( int nl = npl ; -- nl >= 0 ; )
        {
          ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
          ksi_obj ksi_lst = KSI_LIST1( keyPoly );

          CBox box;
          CPoint* pnts = bNeedAnyLns ? new CPoint[npt] : 0;
          int np = 0;
          while( --npt >= 0 )
          {
            double x, y;
            mifstr >> b;
            s = b;
            s.MakeLower();
            if( s == "point" )
              mifstr >> x;
            else
              x = atof( b );
            mifstr >> b;
            y = atof( b );
            
            if( x == 0 || y == 0 )
              continue;
            else
            if( bNeedAnyLns )
              pnts[np++] = UpdatePoint( x, y, pcf );
          }

          if( bNeedAnyLns )
          {
            if( nMode & CM_IMPROVE )
            {
              int h = pDlg->GetDlgItemInt( IDC_CVD_IMPROVE_POLY_PARAM_EDIT );
              np = DecimatePoly( pnts, np, h ? h : 25 );
            }
            for( i = 0 ; i < np ; ++i )
            {
              ksi_add_point( ksi_lst, pnts[i] );
              box |= pnts[i]; 
            }
          }

          if(( bNeedRegion || bNeedBPline ) && npl == 1 && pnts[0] == pnts[np-1] )
            ksi_add_item( ksi_def, KSI_LIST2( keyFplg, ksi_lst ));
          else
            ksi_add_item( ksi_def, KSI_LIST2( keyPolyline, ksi_lst ));


          delete pnts;

          ksi_def = MakePost( ksi_def, mid, SN, pcf, nSub, dlm, box.CenterPoint(), fontSize );
          ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
          if( nl && bHasMid )
          {
            mndstr << entryNumber++ << dlm << mid << std::endl;
            mifstr >> npt;
          }
        }
        break;
      }
      
      case KW_LINE :
      {
        char b[64];
        mifstr >> b;
        if( isalpha( b[0] ))
          continue;

        double x0 = atof( b ), y0, x1, y1;
        mifstr >> y0 >> x1 >> y1;
        ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
        ksi_obj ksi_lst = KSI_LIST1( keyPoly );
        CPoint p0( UpdatePoint( x0, y0, pcf ));
        CPoint p1( UpdatePoint( x1, y1, pcf ));
        CPoint s( p0 + p1 );
        ksi_add_point( ksi_lst, p0 );
        ksi_add_point( ksi_lst, p1 );
        ksi_add_item( ksi_def, KSI_LIST2( keyPolyline, ksi_lst ));
        ksi_def = MakePost( ksi_def, mid, SN, pcf, nSub, dlm, CPoint( s.x / 2, s.y / 2 ), fontSize );
        ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
        break;
      }

      case KW_POINT :
      {
        double x, y;
        ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
        ksi_obj ksi_lst = KSI_LIST1( keyPoly );
        mifstr >> x >> y;
        CPoint p( UpdatePoint( x, y, pcf ));
        ksi_add_point( ksi_lst, p );
        ksi_add_item( ksi_def, KSI_LIST2( keyRefpoint, ksi_lst ));
        ksi_def = MakePost( ksi_def, mid, SN, pcf, nSub, dlm, p, fontSize );
        ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
        break;
      }
      
      case KW_TEXT :
      {
        ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
        char t[1024], r[1024];
        char* p1;
        char* p2;
        
        do
        {
          mifstr.getline( t, sizeof t );
          p1 = strchr( t, '\"' );
          p2 = strrchr( t, '\"' );
        }
        while( !mifstr.eof() && !( p1 && p2 && p1 < p2 ));
        
        if( mifstr.eof())
          continue;

        int ns = 1;
        p2[1] = 0;

        char* pb = p1;
        char* pd;
        while(( pb = strchr( pb, '\\' )) && pb[1] != '\\' && ( pd = strchr( pb, ';' )))
          *( pb = p1 = pd ) = '\"';

        char* pw = r;
        int sq = 0;
        for( char* pr = p1 + 1 ; pr < p2 ; ++pr )
          if( *pr == '\"' )
          {
            if( sq )
              continue;
            sq = 1;
            *pw++ = '\\';
            *pw++ = '\"';
          }
          else
          {
            sq = 0;
            int lf = pr[0] == '\\' && pr[1] == 'n';
            ns += lf;
            if( lf )
            {
              *pw++ = '\n';
              ++pr;
            }
            else
              *pw++ = *pr;
          }
        *pw++ = 0;
        
        double x0, y0, x1, y1, an = 0;
        double* pc[] = { &x0, &y0, &x1, &y1 };

        kwn  = -1;
        for( i = 0 ; kwn < 0 && !mifstr.eof() && i < CELEM( pc ) ; ++i )
        {
          mifstr >> tst;
          s = tst;
          s.MakeLower();
          kwn = Keyword2num( s );
          *pc[i] = atof( s );
        }

        if( kwn >= 0 )
          continue;
        
        do
        {
          mifstr >> tst;
          s = tst;
          s.MakeLower();
          kwn = Keyword2num( s );
        }
        while( !mifstr.eof() && s != "angle" && kwn < 0 );
        if( s == "angle" )
          mifstr >> an;

        int rx = 0, ry = 0;
        if( pcf[0][0] * pcf[1][1] < 0 )
        {
          rx = pcf[0][0] < 0;
          ry = pcf[1][1] < 0;
        }
        
        double W = x1 - x0, H = y1 - y0, h = H / ns;
//        double lh = h * .1, hh = h * .9;
        double lh = 0, hh = h;
        double a = Deg2Rad( an ), si = ::sin( a ), co = ::cos( a );
        
        double xs0 = x0 + hh * si;
        double ys0 = y1 - hh * co;
        double xs1 = x0 + lh * si;
        double ys1 = y1 - lh * co;
        double xs2 = xs1 + W * co;
        double ys2 = ys1 + W * si;

        CPoint t0( UpdatePoint( xs0, ys0, pcf ));
        CPoint t1( UpdatePoint( xs1, ys1, pcf ));
        CPoint t2( UpdatePoint( xs2, ys2, pcf ));
        CPoint t3( t0 + ( t2 - t1 ));

        CPoint b0( rx ? t3 : ry ? t0 : t1 );
        CPoint b1( rx ? t0 : ry ? t3 : t2 );
        
        ksi_obj ksi_tl = 
          KSI_LIST3( keyText, 
                     KSI_LIST3( keyPoly, KsiPoint( b0 ), KsiPoint( b1 )),
                     ksi_cons( keyString, ksi_str02string( r )));
        ksi_add_item( ksi_def, ksi_tl );
        ksi_def = MakePost( ksi_def );
        ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
        break;
      }

      case KW_RECT :
      case KW_ELLIPSE :
      {
        ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
        ksi_obj ksi_lst = KSI_LIST1( keyPoly );
        double x0, y0, x1, y1;
        mifstr >> x0 >> y0 >> x1 >> y1;
        CPoint p0( UpdatePoint( x0, y0, pcf ));
        CPoint p1( UpdatePoint( x1, y1, pcf ));
        CPoint p00( min( p0.x, p1.x ), min( p0.y, p1.y ));
        CPoint p01( max( p0.x, p1.x ), max( p0.y, p1.y ));
        CPoint s( p0 + p1 );
        ksi_add_point( ksi_lst, p00 );
        ksi_add_point( ksi_lst, p01 );
        ksi_add_item( ksi_def, KSI_LIST2( i == 6 ? keyRect : keyEllipse, ksi_lst ));
        ksi_def = MakePost( ksi_def, mid, SN, pcf, nSub, dlm, CPoint( s.x / 2, s.y / 2 ), fontSize );
        ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
        break;
      }

      case KW_ARC :
      {
        ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
        ksi_obj ksi_lst = KSI_LIST1( keyPoly );
        double x0, y0, x1, y1, an;
        mifstr >> x0 >> y0 >> x1 >> y1;
        CPoint p0( UpdatePoint( x0, y0, pcf ));
        CPoint p1( UpdatePoint( x1, y1, pcf ));
        CPoint s( p0 + p1 );
        int sx = s.x / 2;
        int sy = s.y / 2;

        int dx = abs( p1.x - p0.x );
        int dy = abs( p1.y - p0.y );
        int r = max( dx, dy ) / 2;
        
        ksi_add_point( ksi_lst, p0 );
        ksi_add_point( ksi_lst, p1 );

        mifstr >> an;
        int a0 = an * -10;
        p0.x = sx + r * cos( a0 );
        p0.y = sy + r * sin( a0 );
        ksi_add_point( ksi_lst, p0 );

        mifstr >> an;
        int a1 = an * -10;
        p1.x = sx + r * cos( a1 );
        p1.y = sy + r * sin( a1 );
        ksi_add_point( ksi_lst, p1 );
        
        ksi_add_item( ksi_def, KSI_LIST2( keyArc , ksi_lst ));
        ksi_def = MakePost( ksi_def, mid, SN, pcf, nSub, dlm, CPoint( sx, sy ), fontSize );
        ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
        break;
      }

      case KW_ROUNDRECT :
      {
        ksi_def = MakePref( classNumber, statusNumber, tableNumber, entryNumber, nNode );
        ksi_obj ksi_lst = KSI_LIST1( keyPoly );
        double x0, y0, x1, y1, r;
        mifstr >> x0 >> y0 >> x1 >> y1 >> r;
        CPoint p0( UpdatePoint( x0, y0, pcf ));
        CPoint p1( UpdatePoint( x1, y1, pcf ));
        CPoint p2( UpdatePoint( x0 + r, y0 + r, pcf ));
        p2 -= p0;
        p2.x = max( p2.x, p2.y );
        p2.y = 0;

        CPoint s( p0 + p1 );
        ksi_add_point( ksi_lst, p0 );
        ksi_add_item( ksi_lst, KsiPoint( p1 ));
        ksi_add_point( ksi_lst, p2 );
        ksi_add_item( ksi_def, KSI_LIST2( keyRoundrect , ksi_lst ));
        ksi_def = MakePost( ksi_def, mid, SN, pcf, nSub, dlm, CPoint( s.x / 2, s.y / 2 ), fontSize );
        ksi_def_lst = ksi_cons( ksi_def, ksi_def_lst );
        break;
      }
      case KW_COLLECTION :
        mifstr >> ColCnt;
        continue;        
    }

    pDlg->SetReport( nTotal++, nObj, nErr );

    while( KSI_PLIST_P( ksi_def_lst ))
    {
      ksi_def = KSI_CAR( ksi_def_lst );
//KsiCall( "tk-box", ksi_def, 0 ); 
      ksi_def_lst = KSI_CDR( ksi_def_lst ); 
      ksi_obj ksi_dff = KsiCall( "update-lord-object", ksi_def, 0 );
      int cr = KSI_PLIST_P( ksi_dff ) ?
        UpdateKsiObjDef( ksi_dff, ksi_log, ksi_nil, mMode, nNode,
                         ksi_false, ksi_false, ksi_false, 0, 0, 0, log ) :
        PutError( ksi_def, ksi_log, 2 );
      if( cr == -R_BADSTREAM )
        r = R_BADSTREAM;
      else
      if( cr < 0 )
        ++nErr;
      else
			{
        nObj += cr;
				if( bHasMid )
          mndstr << entryNumber++ << dlm << mid << std::endl;
			}

      pDlg->SetReport( nTotal, nObj, nErr );
    }
  }

  mifstr.close();
  mndstr.close();
  ksi_close_port( ksi_log );
  log.close();

  pDlg->SetReport( nTotal, nObj, nErr, 1 );

  return r;
}
//=====================================================================
ResultCode 
MapUpdater::OutputLog( std::ofstream& out, int oldEntry, void* pSrc )
{
  static char dlm[] = ";";

  SYSTEMTIME st;
  GetSystemTime( &st );
  char date[1024];
  sprintf( date, "%02d.%02d.%04d %02d:%02d", 
           st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute );

  ksi_obj ksiObj = ConvertObject( HR.Version, &pSrc, 1 );
  ksi_obj ksiVar = KsiCall( "unwrap", KSI_MK_SINT( pDrh->GetClass()),
                            KSI_MK_SINT( pDrh->GetStatus()), ksiObj, 0 );
  ksi_obj ksiNum = ksi_assv_ref( ksiVar, ksi_str02sym( "number" ));

  out << pDrh->GetTable() << dlm                                  //TableNumber
      << pDrh->GetEntry() << dlm                                  //TableObject 
      << pDrh->GetClass() << dlm;                                 //GraphicsClass
  if( KSI_STR_P( ksiNum ))                                        //Numobj;
  {
    char t[1024];
    strcpy( t, KSI_STR_PTR( ksiNum ));
    for( char* p = t ; p = strchr( p, '\n' ) ; *p++ = ' ' );
    out << '\"' << t << '\"' << dlm;                          
  }
  else
    out << pDrh->GetEntry() << dlm;                                
  out << dlm                                                       //Text1  
      << dlm                                                       //Text2
      << date << dlm                                               //CreationTime
      << "?" << dlm                                                //Username
      << "?" << dlm                                                //Comm
      << 0 << dlm                                                  //Prdel
      << pDrh->GetNode() << dlm                                    //NodeNumber
      << 1000 << dlm                                               //CmdID
      << pDocument->GetObjDef()->GetName( pDrh->GetClass()) << dlm //GraphicsClassName
      << "Конвертация" << dlm                                      //CmdName
      << "?" << dlm                                                //TableName
      << date << dlm                                               //CreationTime
      << oldEntry << std::endl;                                         //TableObjectOld

  return R_OK;
}
//=====================================================================
ResultCode 
MapUpdater::CreateLog( char const* errPath, std::ofstream& out )
{
  static char hd[] = "tab_id;obj_id;cls_id;numobj;par1;par2;"
                     "datemodif;username;comm;prdel;uzel;cmdid;"
                     "class_name;cmd_name;tab_name;datemodif2;obj_id_old";
  char logPath[_MAX_PATH];
  char buf1[_MAX_PATH], buf2[_MAX_FNAME], buf3[_MAX_FNAME], buf4[_MAX_FNAME];
  _splitpath( errPath, buf1, buf2, buf3, buf4 );
  _makepath( logPath, buf1, buf2, buf3, ".log" );
  out.open( logPath, std::ios::out| std::ios::trunc, std::filebuf::sh_read );
  ResultCode r = out.bad() ? TraceError( MapNumber, R_NOFILE, logPath ) : R_OK;
  if( r == R_OK )
    out << hd << std::endl;
  return r;
}
//=====================================================================
int
MapUpdater::CreatePosTab( IdentDirectHashTab * pIdTab, int bDelMode, int bIdentMode )
{
  ResultCode r = R_OK;
  for( std::streampos inPos = sizeof( DataFileHeadRec ) ;
       r == R_OK && inPos < HR.FreePos ; inPos += pDrh->GetSize())
  {
    r = GetRecord( inPos );
    if( r == R_OK && bDelMode == pDrh->IsEmpRec())
      pIdTab->AddObject( pDrh->GetTabIdent(), inPos );
  }

  IdentDirectHashTab ::IdentItem* pSrc = pIdTab->m_pTab;
  IdentDirectHashTab ::IdentItem* pDst = pSrc;
  IdentDirectHashTab ::IdentItem* pEnd = pSrc + pIdTab->m_nSize;
  while( pSrc < pEnd )
    if( pSrc->Used())
      *pDst++ = *pSrc++;
    else
      ++pSrc;

  int l = pDst - pIdTab->m_pTab;
  
  pSrc = pIdTab->m_pTab;

  for( int bs = 1 ; bs < l ; bs <<= 1 )
  {
    IdentDirectHashTab ::IdentItem* p0 = pSrc;
    IdentDirectHashTab ::IdentItem* d0 = pDst;
    pSrc = d0;
    pDst = p0;
    for( int ts, dn = l ; dn > 0 ; )
    {
      dn -= ts = min( bs, dn );
      IdentDirectHashTab ::IdentItem* e0 = p0 + ts;
      IdentDirectHashTab ::IdentItem* p1 = e0;
      dn -= ts = min( bs, dn );
      IdentDirectHashTab ::IdentItem* e1 = p1 + ts;
      while( p0 < e0 && p1 < e1 )
        if( bIdentMode )
          *d0++ = p0->m_Id < p1->m_Id ? *p0++ : *p1++;
        else
          *d0++ = p0->m_Pos < p1->m_Pos ? *p0++ : *p1++;
      while( p0 < e0 )
        *d0++ = *p0++;
      while( p1 < e1 )
        *d0++ = *p1++;
      p0 = p1;
    }
  }

  if( pSrc != pIdTab->m_pTab )
    memcpy( pIdTab->m_pTab, pSrc, l * sizeof pSrc[0] );

  return l;
}
//=====================================================================
