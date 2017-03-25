
#include "stdafx.h"

#include "msutil.h"
#include "mapstore.h"
#ifdef _GLOBAL_SPACE
//=====================================================================
//=====================================================================
CIconMap::IcnItem*
CIconMap::GetItem( int key )
{
  int i = m_nSize;
  while( --i >= 0 && m_pMap[i].m_Key != key );
    return i < 0 ? 0 : m_pMap + i;
}
//=====================================================================
void
CIconMap::Init( char const* sectName, char const* initPath )
{
  char buf[16384];
  char* pb = buf;

  GetPrivateProfileString( sectName, 0, "", buf, sizeof buf, initPath );
  int i = 0, size;
  for( size = 0 ; *pb ; pb += strlen( pb ) + 1, ++size );
  if( m_pMap )
  {
    IcnItem* m_pOldMap = m_pMap;
    m_pMap = new IcnItem[m_nSize+size];
    for( ; i < m_nSize ; ++i )
    m_pMap[i] = m_pOldMap[i];
    m_nSize += size;
    delete m_pOldMap;
  }
  else
    m_pMap = new IcnItem[m_nSize=size];

  for( pb = buf ; *pb ; pb += strlen( pb ) + 1 )
  {
    char tb[256], sb[256];
    int s;
    int key = atoi( pb );
    GetPrivateProfileString( sectName, pb, "", tb, sizeof tb, initPath );
    IcnItem* pItem = GetItem( key );
    if( pItem )
      m_nSize -= 1;
    else
      pItem = &m_pMap[i++];
    pItem->m_Key = key;
    sscanf( tb, "%s%d", sb, &s );
    strncpy( pItem->m_Str, sb, sizeof pItem->m_Str );
    pItem->m_Sngl = s;

    char path[_MAX_PATH];
    strcpy_s( path, initPath );
    char* p;
    ( p = strrchr( path, '\\' )) || ( p = strrchr( path, '/' ));
    strcpy_s( p, sizeof path - ( p - path ), "\\bitmaps\\" );
    MakePath( path, path, ".bmp", pItem->m_Str );

    HBITMAP hBmp = (HBITMAP)LoadImage( 0, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

    if( hBmp )
    {
      BITMAP bmps;
      CBitmap::FromHandle( hBmp )->GetBitmap( &bmps );
      pItem->NativeXSize = bmps.bmWidth;
      pItem->NativeYSize = bmps.bmHeight;
      DeleteObject( hBmp );
    }
    else
    {
      pItem->NativeXSize = 0;
      pItem->NativeYSize = 0;
    }
  }
}
//=====================================================================
static ResultCode
AddDataToStream( void* pSrc, int nSrcLen, TGstream& dst )
{
  dst.write((char const*)pSrc, nSrcLen );
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::SetClassRange( int const* pClassNum )
{
  pClassNum || ( pClassNum = ClsInf.GetRng());
  memcpy( ClassRange, pClassNum, sizeof ClassRange );
  memset( LoadMap, 0, sizeof LoadMap );
  for( int t ; t = *pClassNum++ ; )
    if( t && t < CLASS_RANGE )
      LoadMap[t>>3] |= (1<<(t&0x7));
  pLoadMap = LoadMap;
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::GetGeoObjectBox( CTabIdent id, double* pRect, int* pClass )
{
  RECT R;
  ResultCode r = GetObjectBox( id, &R, pClass );
  if( r != R_OK )
    return r;

  POINT pt0, pt1;
  Polar pl0, pl1;

  if( MerkatorData.TrueMerkator())
  {
    pt0.x = R.left;
    pt0.y = R.top;
    pt0 = MerkatorData.Prj2Merk( pt0, 0 );
    pt1.x = R.right;
    pt1.y = R.bottom;
    pt1 = MerkatorData.Prj2Merk( pt1, 0 );
  }
  else
  {
    SIZE ps = MerkatorData.GetProjectSize();
    int dx = ps.cx / 2;
    int dy = ps.cy / 2;

    pt0.x = R.left - dx;
    pt0.y = -R.top + dy;
    pt1.x = R.right - dx;
    pt1.y = -R.bottom + dy;
  }

  pl0 = MerkatorData.Dec2Pol(pt0);
  pl1 = MerkatorData.Dec2Pol(pt1);

  pRect[0] = pl0.lon / Deg;
  pRect[1] = pl0.lat / Deg;
  pRect[2] = pl1.lon / Deg;
  pRect[3] = pl1.lat / Deg;

  return R_OK;
}
//=====================================================================
ResultCode
MapStore::GetGlobalGeoRect( double Ln0, double Lt0, double Ln1, double Lt1,
                            int w, int h, TGstream& dst )
{
  Polar pl0( Lt0 * Deg, Ln0 * Deg );
  Polar pl1( Lt1 * Deg, Ln1 * Deg );
  POINT p0, p1;

  if( MerkatorData.TrueMerkator())
  {
    if( !MerkatorData.Pol2Prj( pl0, p0 ))
      return R_CONVERTERROR;
    if( !MerkatorData.Pol2Prj( pl1, p1 ))
      return R_CONVERTERROR;
    p0 = MerkatorData.Prj2Merk( p0, 0 );
    p1 = MerkatorData.Prj2Merk( p1, 0 );
  }
  else
  {
    POINT t0 = MerkatorData.Pol2Dec( pl0 );
    POINT t1 = MerkatorData.Pol2Dec( pl1 );
    SIZE ps = MerkatorData.GetProjectSize();
    int dx = ps.cx / 2;
    int dy = ps.cy / 2;
    p0.x = min( t0.x, t1.x ) + dx;
    p0.y = min( t0.y, t1.y ) + dy;
    p1.x = max( t0.x, t1.x ) + dx;
    p1.y = max( t0.y, t1.y ) + dy;
  }

  return GetGlobalRect( p0.x, p0.y, p1.x, p1.y, w, h, dst );
}
//=====================================================================
ResultCode
MapStore::SelGlobalGeoRect( double Ln0, double Lt0, double Ln1, double Lt1,
                            int w, int h, TGstream& dst,
                                                                                                                double Lns0, double Lts0, double Lns1, double Lts1 )
 {
   Polar pl0( Lt0 * Deg, Ln0 * Deg );
   Polar pl1( Lt1 * Deg, Ln1 * Deg );
   POINT p0, p1;
   if( !MerkatorData.Pol2Prj( pl0, p0 ))
     return R_CONVERTERROR;
   if( !MerkatorData.Pol2Prj( pl1, p1 ))
     return R_CONVERTERROR;
   p0 = MerkatorData.Prj2Merk( p0, 0 );
   p1 = MerkatorData.Prj2Merk( p1, 0 );

   Polar pls0( Lts0 * Deg, Lns0 * Deg );
   Polar pls1( Lts1 * Deg, Lns1 * Deg );
   POINT ps0, ps1;
   if( !MerkatorData.Pol2Prj( pls0, ps0 ))
     return R_CONVERTERROR;
   if( !MerkatorData.Pol2Prj( pls1, ps1 ))
     return R_CONVERTERROR;
   ps0 = MerkatorData.Prj2Merk( ps0, 0 );
   ps1 = MerkatorData.Prj2Merk( ps1, 0 );

   return SelGlobalRect( p0.x, p0.y, p1.x, p1.y, w, h, dst, ps0.x, ps0.y, ps1.x, ps1.y );
 }
//=====================================================================
#if 0
static int
Dist( Point p0, Point p1 )
{
  double dx = p0.x - p1.x;
  double dy = p0.y - p1.y;
  return sqrt( dx * dx + dy * dy );
}
//=====================================================================
static double
hip( double x, double y )
{
  return sqrt( x * x + y * y );
}
//=====================================================================
static int
TestEllipse( Point pt, Rect rect )
{ // return dist to ellipse
  double dx = pt.x - ( rect.right + rect.left ) / 2.;
  double dy = pt.y - ( rect.bottom + rect.top ) / 2.;
  double a = abs( rect.right - rect.left );
  double b = abs( rect.bottom - rect.top );

  if( a == b )
    return  hip( dx, dy ) - a / 2;

  if( a < b )
  {
    double t;
    t = dx; dx = dy; dy = t;
    t = a; a = b; b = t;
  }

  double df = sqrt(( a + b ) * ( a - b )) / 2.;

  return ( hip( dx - df, dy ) + hip( dx + df, dy ) - a ) / 2;
}
//=====================================================================
#endif
ResultCode
MapStore::GetGlobalRect( int x0, int y0, int x1, int y1, int w, int h, TGstream& dst )
{
  POINT p0, p1;
  p0.x = x0;
  p1.y = y0;
  p1.x = x1;
  p1.y = y1;
  
  if( MerkatorData.TrueMerkator())
  {
    p0 = MerkatorData.Merk2Prj( p0, 0 );
    p1 = MerkatorData.Merk2Prj( p1, 0 );
  }

  Clip.left   = min( p0.x, p1.x );
  Clip.top    = min( p0.y, p1.y );
  Clip.right  = max( p0.x, p1.x );
  Clip.bottom = max( p0.y, p1.y );

  POINT ss;
  ss.x = Clip.right - Clip.left;
  ss.y = Clip.bottom - Clip.top;
  POINT C0;
  C0.x = Clip.left + Clip.right >> 1;
  C0.y = Clip.top + Clip.bottom >> 1;
  double MSF = MerkatorData.ScaleFactor( C0 );

  ScaleFactorX = double(w) / ss.x;
  ScaleFactorY = double(h) / ss.y;
  ScaleFactor = max( ScaleFactorX, ScaleFactorY );

  Scale = MerkatorData.CalcScale( ScaleFactor );

  streampos pos;
  ResultCode r = R_OK;
  bClipping = 1;

  TGstream* pTmpStr[CLASS_RANGE];
  memset( pTmpStr, 0, sizeof pTmpStr );

  int cnt[CLASS_RANGE];
  memset( cnt, 0, sizeof cnt );

  static char mnt[] = "TGMP";
  AddDataToStream( mnt, sizeof mnt - 1, dst );
  AddDataToStream( &MapNumber, sizeof MapNumber, dst );
  AddDataToStream( &w, sizeof w, dst );
  AddDataToStream( &h, sizeof h, dst );

  pSelectInfo[0].poslist.RemoveAll();
  SpaceIndex.SelectObjects( Clip, Scale, pNodes, pSelectInfo[0].poslist );
  pSelectInfo[0].poslist.SetFirst();

  while( r == R_OK && ( pos = pSelectInfo[0].poslist.GetNext()))
  {
    r = GetRecord( pos );
    if( r == R_OK )
    {
      CIdent id(pDrh->GetIdent());
      int ct = id.Class();

      if( ct <= 0 || ct >= CLASS_RANGE )
        continue;

      if( !GetBit( LoadMap, ct ))
        continue;

      void* pSrc = pData;
      if (!IsDataClipped(&pSrc))
        continue;

      int minLimit = ClsInf.GetMinLimit(ct);
      int maxLimit = ClsInf.GetMaxLimit(ct);
      int tscle = Scale;

      if( minLimit && tscle < minLimit )
        tscle = minLimit;
      if( maxLimit && tscle > maxLimit )
        tscle = maxLimit;

      ClsScaleFactor = double(Scale) / double(tscle) * MSF;

      TGstream* pps = pTmpStr[ct];
      if (!pps)
        pps = pTmpStr[ct] = new TGstream(0x1000);

      word mvb = (pDrh->GetStatus() & 0x80) && Scale <= HRLimit ? PF_HVISIBLE : PF_LVISIBLE;
      r = TranslateObject(mvb, id.TabIdent(), *pps);
      cnt[ct] += r == R_OK;
      }
    }

  static char lrt[] = "LAYR";
  int t;
  for (int const* p = ClassRange; t = *p++; )
  {
    if( t >= CLASS_RANGE || cnt[t] == 0 )
      continue;

    AddDataToStream(lrt, sizeof lrt - 1, dst);
    AddDataToStream(&t, sizeof t, dst);
    AddDataToStream(&cnt[t], sizeof cnt[t], dst);
    AddDataToStream(pTmpStr[t]->str(), pTmpStr[t]->pcount(), dst);
  }

  for( int i = CLASS_RANGE ; --i >= 0 ; delete pTmpStr[i] );

  return r;
}
//=====================================================================
ResultCode
MapStore::SelGlobalRect( int x0, int y0, int x1, int y1,
                         int w, int h, TGstream& dst,
                         int sx0, int sy0, int sx1, int sy1 )
{
  POINT p0 = { x0, y0 };
  POINT p1 = { x1, y1 };
  POINT ps0 = { sx0, sy0 };
  POINT ps1 = { sx1, sy1 };

  if (MerkatorData.TrueMerkator())
  {
    p0  = MerkatorData.Merk2Prj( p0,  0 );
    p1  = MerkatorData.Merk2Prj( p1,  0 );
    ps0 = MerkatorData.Merk2Prj( ps0, 0 );
    ps1 = MerkatorData.Merk2Prj( ps1, 0 );
  }

  Clip.left   = min( p0.x, p1.x );
  Clip.top    = min( p0.y, p1.y );
  Clip.right  = max( p0.x, p1.x );
  Clip.bottom = max( p0.y, p1.y );

  RECT SRct = { min( ps0.x, ps1.x ),
                min( ps0.y, ps1.y ),
                max( ps0.x, ps1.x ),
                max( ps0.y, ps1.y ) };

  POINT ss = { Clip.right - Clip.left, Clip.bottom - Clip.top };
  POINT C0 = { Clip.left + Clip.right >> 1, Clip.top + Clip.bottom >> 1 };
  double MSF = MerkatorData.ScaleFactor(C0);

  int bSelectPoint = sx0 == sx1 && sy0 == sy1;

  ScaleFactorX = double(w) / ss.x;
  ScaleFactorY = double(h) / ss.y;
  ScaleFactor = max(ScaleFactorX, ScaleFactorY);

  Scale = MerkatorData.CalcScale(ScaleFactor);

  streampos pos;
  ResultCode r = R_OK;
  bClipping = 1;

  TGstream* pTmpStr[CLASS_RANGE];
  memset(pTmpStr, 0, sizeof pTmpStr);

  int cnt[CLASS_RANGE];
  memset(cnt, 0, sizeof cnt);

  static char mnt[] = "TGMP";
  AddDataToStream(mnt, sizeof mnt - 1, dst);
  AddDataToStream(&MapNumber, sizeof MapNumber, dst);
  AddDataToStream(&w, sizeof w, dst);
  AddDataToStream(&h, sizeof h, dst);

  pSelectInfo[0].poslist.RemoveAll();
  int count = SpaceIndex.SelectObjects(SRct, Scale, pNodes, pSelectInfo[0].poslist);
  pSelectInfo[0].poslist.SetFirst();

  while (r == R_OK && (pos = pSelectInfo[0].poslist.GetNext()))
  {
    r = GetRecord(pos);
    if (r == R_OK)
    {
      CIdent id(pDrh->GetIdent());

      int ct = id.Class();

      if( ct <= 0 || ct >= CLASS_RANGE )
        continue;

      if( !GetBit( LoadMap, ct ))
        continue;

      RECT box = pDrh->GetBox();

      if( !bSelectPoint )
        continue;

      if( box.left   < SRct.left  ||
          box.top    < SRct.top   ||
          box.right  > SRct.right ||
          box.bottom > SRct.bottom )
        continue;

      char* pSrc = pData;

      int minLimit = ClsInf.GetMinLimit( ct );
      int maxLimit = ClsInf.GetMaxLimit( ct );
      int tscle = Scale;

      if( minLimit && tscle < minLimit )
        tscle = minLimit;
      if( maxLimit && tscle > maxLimit )
        tscle = maxLimit;

      ClsScaleFactor = double(Scale) / double(tscle) * MSF;

      TGstream* pps = pTmpStr[ct];
      if (!pps)
        pps = pTmpStr[ct] = new TGstream(0x1000);

      word mvb = Scale <= HRLimit ? PF_HVISIBLE : PF_LVISIBLE;
      r = TranslateObject(mvb, id.TabIdent(), *pps);
      cnt[ct] += r == R_OK;
    }
  }

  static char lrt[] = "LAYR";
  int t;
  for( int const* p = ClassRange ; t = *p++ ; )
  {
    if( t >= CLASS_RANGE || cnt[t] == 0 )
      continue;

    AddDataToStream( lrt, sizeof lrt - 1, dst );
    AddDataToStream( &t, sizeof t, dst );
    AddDataToStream( &cnt[t], sizeof cnt[t], dst );
    AddDataToStream( pTmpStr[t]->str(), pTmpStr[t]->pcount(), dst );
  }

  for( int i = CLASS_RANGE ; --i >= 0 ; delete pTmpStr[i] );

  return r;
}
//=====================================================================
ResultCode
MapStore::TranslateObject( word mvb, CTabIdent id, TGstream& dst )
{
  void* pSrc = pData;
  dword dm = OBJMAGIC;
  ResultCode r = AddDataToStream( &dm, sizeof dm, dst );
  r == R_OK && ( r = AddDataToStream( &id, sizeof id, dst ));
  pSrc = pData;
  POINT* tcp = 0;
  return r == R_OK ? TranslatePrim( &pSrc, mvb, tcp, dst) : r;
}
//=====================================================================
ResultCode
MapStore::TranslatePrim( void** ppSrc, word mvb, POINT*& pPoint, TGstream& dst )
{
  static POINT CentrPoint;

  PrimRecord* pPr = (PrimRecord*)*ppSrc;
  Drawable* pObjPar = (Drawable*)&pPr->param;
  word typeKey = pPr->type & PF_TYPE_MASK;
  int subType = (( pPr->type & PF_STROKED ) ? STROKED : 0 ) |
                (( pPr->type & PF_FILLED  ) ? FILLED  : 0 ) |
                (( pPr->type & PF_ROUND   ) ? ROUND   : 0 );

  int bAnyVsbl = pPr->type & PF_VISIBLE;

  int bShape = pPr->type & PF_SHAPE;

  int bVisible = ( pPr->type & mvb ) && !( pPr->type & PF_SVISIBLE );

  ResultCode r = R_OK;

  streampos startPos = dst.tellp();

  if( bVisible )
    r = AddDataToStream( &pPr->type, sizeof pPr->type, dst );

  if( bAnyVsbl )
  {
    PenParam*   ppp = 0;
    BrushParam* pbp = 0;
    switch( subType & OUTLINED )
    {
      case STROKED :
        pObjPar = &pPr->param.stroked.primParam;
        if( bVisible )
          ppp = &pPr->param.stroked.pen;
        break;
      case FILLED :
        pObjPar = &pPr->param.filled.primParam;
        if( bVisible )
          pbp = &pPr->param.filled.brush;
        break;
      case OUTLINED :
        pObjPar = &pPr->param.outlined.primParam;
        if( bVisible )
        {
          ppp = &pPr->param.outlined.pen;
          pbp = &pPr->param.outlined.brush;
        }
        break;
    }
    if( ppp )
    {
      ExPenParam epp;
      epp.style = ppp->style;
      epp.width = ppp->width * ClsScaleFactor * ScaleFactor;
      epp.color = ppp->color;
      r = AddDataToStream( &epp, sizeof( epp ), dst );
    }
    if( pbp )
    {
      ExBrushParam ebb;
      ebb.style = pbp->style;
      ebb.hatch = pbp->hatch;
      if( ebb.fill = pbp->fore != (word)-1 )
      {
        ebb.fore  = pbp->fore;
        ebb.back  = pbp->back;
      }
      else
       ebb.fore = ebb.back = 0;
       r = AddDataToStream( &ebb, sizeof( ebb ), dst );
    }
  }

  switch( typeKey )
  {
    case PF_CONTAINER:
    {
      int n = pPr->param.container.length;
      *ppSrc = pPr->param.container.prims;
      while( r == R_OK && --n >= 0 )
        r = TranslatePrim( ppSrc, mvb, pPoint, dst );
      word t = 0;
      r == R_OK && ( r = AddDataToStream( &t, sizeof t, dst ));
      break;
    }

    case PF_POINT:
      if( bShape )
      {
        CentrPoint = pPr->param.pnt;
        pPoint = &CentrPoint;
      }
      *ppSrc = &pPr->param.pnt + 1;
       dst.seekp( startPos );
      break;

    case PF_ICON:
    {
      if( bShape )
      {
        CentrPoint = pPr->param.icon;
        pPoint = &CentrPoint;
      }
      *ppSrc  = &pPr->param.icon + 1;
      if( !bVisible )
        break;

      TranslatePoint( &pPr->param.icon );

      CIconMap::IcnItem* pItem = IconMap.GetItem( pPr->param.icon.id );
      int sNgl = pItem ? pItem->m_Sngl : 0;

      ExIcon xIcn;
      xIcn.id = pPr->param.icon.id;
      xIcn.x = pPr->param.icon.x;
      xIcn.y = pPr->param.icon.y;
      xIcn.xSize = MulDiv( pItem->NativeXSize, sNgl, Scale ) * ClsScaleFactor;
      xIcn.ySize = MulDiv( pItem->NativeYSize, sNgl, Scale ) * ClsScaleFactor;

      r == R_OK && ( r = AddDataToStream( &xIcn, sizeof xIcn, dst ));
      break;
    }
    case PF_TEXT:
    {
      StringParam* pSP = bAnyVsbl ? &pPr->param.text.param.full.string :
                                    &pPr->param.text.param.string;
      *ppSrc = pSP->string + pSP->length;
      if( !bVisible )
        break;

      FontParam tFp;
      FontParam* pFp = &tFp;
      if( HR.Version >= 107 )
      {
        FontParam1* oFp = &pPr->param.text.param.full.FontParams.font1;
        pFp->number = oFp->number;
        int h = abs( int( oFp->height ));
        pFp->height = h * ClsScaleFactor * ScaleFactor;
        pFp->weight = oFp->weight;
        pFp->style  = oFp->style;
        pFp->color  = oFp->color;
      }
      else
      {
        pFp = &pPr->param.text.param.full.FontParams.font;
        pFp->color = pFp->color;
        int h = abs( pFp->height );
        pFp->height = h * ClsScaleFactor * ScaleFactor;
      }

      double a = Deg2Rad( pSP->angle );
      double co = ::cos(a), si = ::sin(a);
      POINT off = { pSP->offset.x * co + pSP->offset.y * si, pSP->offset.y * co - pSP->offset.x * si };
      pSP->offset.x = off.x + pPr->param.text.org.x;
      pSP->offset.y = off.y + pPr->param.text.org.y;
      if( pPoint )
      {
        pSP->offset.x = pPoint->x + ( pSP->offset.x - pPoint->x ) * ClsScaleFactor;
        pSP->offset.y = pPoint->y + ( pSP->offset.y - pPoint->y ) * ClsScaleFactor;
      }
      TranslatePoint( &pSP->offset );

      r == R_OK && ( r = AddDataToStream( pFp, sizeof( *pFp ), dst ));
      r == R_OK && ( r = AddDataToStream( pSP, sizeof( *pSP ), dst ));
      r == R_OK && ( r = AddDataToStream( pSP->string, pSP->length, dst ));
      break;
    }

    case PF_TEXTBYLINE:
    {
      int length = pPr->param.textbyline.length;
      TxtItem* pItem = pPr->param.textbyline.items;
      *ppSrc = pItem + length;
      if( !bVisible )
        break;

      FontParam tFp;
      FontParam* pFp = &tFp;
      if( HR.Version >= 107 )
      {
        FontParam1* oFp = &pPr->param.textbyline.FontParams.font1;
        pFp->number = oFp->number;
        int h = abs( int( oFp->height ));
        pFp->height = h * ClsScaleFactor * ScaleFactor;
        pFp->weight = oFp->weight;
        pFp->style  = oFp->style;
        pFp->color  = oFp->color;
      }
      else
      {
        pFp = &pPr->param.textbyline.FontParams.font;
        pFp->color = pFp->color;
        int h = abs( pFp->height );
        pFp->height = h * ClsScaleFactor * ScaleFactor;
      }

      for( int i = length ; --i >= 0 ; TranslatePoint( &pItem[i].org ));
      r == R_OK && ( r = AddDataToStream( pFp, sizeof( *pFp ), dst ));
      r == R_OK && ( r = AddDataToStream( &pPr->param.textbyline.length, sizeof( pPr->param.textbyline.length ), dst ));
      r == R_OK && ( r = AddDataToStream( pItem, length * sizeof( *pItem ), dst ));
      break;
    }

    case PF_FOOTNOTE:
    case PF_POLYPOINT:
    case PF_MULTILINE:
    {
      int nPoints = pObjPar->polypoint.length;
      POINT* pPoints = pObjPar->polypoint.points;
      *ppSrc = pPoints + nPoints;
      if( !bVisible )
        break;

        if( nPoints == 0 )
        dst.seekp( startPos );
      else
      {
        word n = TranslatePolypoint( pPoints, nPoints, pPoint );
        r = AddDataToStream( &n, sizeof n, dst );
        if( n >= 2 )
          r = AddDataToStream( pPoints, n * sizeof( POINT ), dst );
        else
          dst.seekp( startPos );
      }
      break;
    }

    case PF_RECTANGLE :
    {
      if( bShape )
      {
        CentrPoint.x = pObjPar->rectangle.left,
        CentrPoint.y = pObjPar->rectangle.right;
        pPoint = &CentrPoint;
      }

      *ppSrc = &pObjPar->rectangle + 1;
      if( !bVisible )
        break;

      RectParam rct = pObjPar->rectangle;

      if( pPoint )
      {
        rct.left   = pPoint->x + ( rct.left   - pPoint->x ) * ClsScaleFactor;
        rct.top    = pPoint->y + ( rct.top    - pPoint->y ) * ClsScaleFactor;
        rct.right  = pPoint->x + ( rct.right  - pPoint->x ) * ClsScaleFactor;
        rct.bottom = pPoint->y + ( rct.bottom - pPoint->y ) * ClsScaleFactor;
      }

      rct.left   = ( rct.left   - Clip.left ) * ScaleFactorX;
      rct.top    = ( rct.top    - Clip.top  ) * ScaleFactorY;
      rct.right  = ( rct.right  - Clip.left ) * ScaleFactorX;
      rct.bottom = ( rct.bottom - Clip.top  ) * ScaleFactorY;
      r = AddDataToStream( &rct, sizeof rct, dst );
      break;
    }

    case PF_ELLIPSE :
    {
      if( bShape )
      if( bShape )
      {
        CentrPoint.x = pObjPar->ellipse.left,
        CentrPoint.y = pObjPar->ellipse.right;
        pPoint = &CentrPoint;
      }

      *ppSrc = &pObjPar->ellipse + 1;
      if( !bVisible )
        break;

      RECT rct = pObjPar->ellipse;

      int dx = rct.right - rct.left;
      int dy = rct.bottom - rct.top;
      if( subType & ROUND )
        dx = dy = max( dx, dy );

      rct.left -= dx;
      rct.top  -= dy;

      if( pPoint )
      {
        rct.left   = pPoint->x + ( rct.left   - pPoint->x ) * ClsScaleFactor;
        rct.top    = pPoint->y + ( rct.top    - pPoint->y ) * ClsScaleFactor;
        rct.right  = pPoint->x + ( rct.right  - pPoint->x ) * ClsScaleFactor;
        rct.bottom = pPoint->y + ( rct.bottom - pPoint->y ) * ClsScaleFactor;
      }

      rct.left   = ( rct.left   - Clip.left ) * ScaleFactorX;
      rct.top    = ( rct.top    - Clip.top  ) * ScaleFactorY;
      rct.right  = ( rct.right  - Clip.left ) * ScaleFactorX;
      rct.bottom = ( rct.bottom - Clip.top  ) * ScaleFactorY;
      r = AddDataToStream( &rct, sizeof rct, dst );
      break;
    }

    case PF_ARC :
    {
      *ppSrc = &pObjPar->arc + 1;
      if( !bVisible )
        break;

      ArcParam rct = pObjPar->arc;

      int dx = rct.right - rct.left;
      int dy = rct.bottom - rct.top;

      if( subType & ROUND )
        dx = dy = max( dx, dy );

      rct.left -= dx;
      rct.top  -= dy;

      if( pPoint )
      {
        rct.left   = pPoint->x + ( rct.left   - pPoint->x ) * ClsScaleFactor;
        rct.top    = pPoint->y + ( rct.top    - pPoint->y ) * ClsScaleFactor;
        rct.right  = pPoint->x + ( rct.right  - pPoint->x ) * ClsScaleFactor;
        rct.bottom = pPoint->y + ( rct.bottom - pPoint->y ) * ClsScaleFactor;
      }

      rct.left   = ( rct.left   - Clip.left ) * ScaleFactorX;
      rct.top    = ( rct.top    - Clip.top  ) * ScaleFactorY;
      rct.right  = ( rct.right  - Clip.left ) * ScaleFactorX;
      rct.bottom = ( rct.bottom - Clip.top  ) * ScaleFactorY;
      r = AddDataToStream( &rct, sizeof rct, dst );
      break;
    }
  }

  return R_OK;
}
//=====================================================================
void
MapStore::TranslatePoint( POINT* pPnt )
{
  pPnt->x = ( pPnt->x - Clip.left ) * ScaleFactorX;
  pPnt->y = ( pPnt->y - Clip.top  ) * ScaleFactorY;
}
//=====================================================================
int
MapStore::TranslatePolypoint( POINT* pPoints, int nPoint, POINT* pCp )
{
  for( int i = nPoint ; --i >= 0 ; )
  {
    if( pCp )
    {
      pPoints[i].x = pCp->x + ( pPoints[i].x - pCp->x ) * ClsScaleFactor;
      pPoints[i].y = pCp->y + ( pPoints[i].y - pCp->y ) * ClsScaleFactor;
    }
    TranslatePoint( &pPoints[i] );
  }
  POINT* p0 = pPoints;
  POINT* p2 = pPoints + nPoint;
  for( POINT* p1 = p0 + 1 ; p1 < p2 ; ++p1 )
    if( p0->x != p1->x || p0->y != p1->y )
  *++p0 = *p1;

  return p0 - pPoints + 1;
}
//=====================================================================
static void
PutSpace( TGstream& dst, int lvl )
{
  static char const sp[] = "                                                               ";
  dst << endl;
  dst.write( sp, min( lvl, sizeof sp - 1 ));
}
//=====================================================================
static char const ap = '\'';
//=====================================================================
ResultCode
MapStore::ClsDef2XML( TGstream& dst, int lvl )
{
  static char const tag[] = "Objects";
  PutSpace( dst, lvl );
  dst << "<" << tag << ">";
  lvl += 2;
  for( int t = 1 ; t < CLASS_RANGE ; ++t )
  {
    if( !ClsInf.IsClass( t ))
      continue;

    int maxLvl = ClsInf.GetMinLimit(t);
    int minLvl = ClsInf.GetMaxLimit(t);

    maxLvl = MerkatorData.Scale2Level( maxLvl ? maxLvl : 100);

    if( minLvl )
      minLvl = MerkatorData.Scale2Level( minLvl );

    PutSpace( dst, lvl );
    dst << "<" << "Class";
    dst << " Name=" << ap << ClsInf.GetName(t) << ap;
    dst << " Id=" << ap << t << ap;
    dst << " Prnt=" << ap << ClsInf.GetParent(t) << ap;
    dst << " Rng=" << ap << ClsInf.GetRange(t) << ap;
    dst << " MinLvl=" << ap << minLvl << ap;
    dst << " MaxLvl=" << ap << maxLvl << ap;
    dst << " User=" << ap << ( UserClassMap[t] ? "true" : "false" ) << ap;
    dst << "/>";
  }
  lvl -= 2;
  PutSpace( dst, lvl );
  dst << "</" << tag << ">\n";
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::ClrDef2XML( TGstream& dst, int lvl )
{
  for( int i = 2 ; --i >= 0 ; )
  {
    char const* sectName = !i ? "UserColorTab" : "ColorTab";
    char const* tag      = !i ? "UserColors"   : "Colors";
    PutSpace( dst, lvl );
    dst << "<" << tag << ">";
    lvl += 2;
    char buf[16384];
    GetPrivateProfileString( sectName, 0, "", buf, sizeof buf, InitPath );
    for( char* pb = buf ; *pb ; pb += strlen( pb ) + 1 )
    {
      char const tag[] = "Color";
      char tb[256];
      GetPrivateProfileString( sectName, pb, "", tb, sizeof tb, InitPath );
      long cr;
      sscanf( tb, "%x", &cr );
      int key = atoi( pb );
      char cr16[16];
      sprintf_s( cr16, "%06x", cr );
      PutSpace( dst, lvl );
      dst << "<" << tag;
      dst << " Id="  << ap << key  << ap;
      dst << " RGB=" << ap << cr16 << ap;
      dst << "/>";
    }
    lvl -= 2;
    PutSpace( dst, lvl );
    dst << "</" << tag << ">\n";
  }
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::FntDef2XML( TGstream& dst, int lvl )
{
  static char const sectName[] = "FontFace";
  static char const tag[] = "fonts";
  PutSpace( dst, lvl );
  dst << "<" << tag << ">";
  lvl += 2;
  char buf[16384];
  char* pb = buf;
  GetPrivateProfileString( sectName, 0, "", buf, sizeof buf, InitPath );
  for( pb = buf ; *pb ; pb += strlen( pb ) + 1 )
  {
    static char const tag[] = "font";
    char face[32];
    GetPrivateProfileString( sectName, pb, "", face, sizeof face, InitPath );
    PutSpace( dst, lvl );
    dst << "<" << tag;
                dst << " Id="   << ap << pb   << ap;
                dst << " Face=" << ap << face << ap;
    dst << "/>";
  }
  lvl -= 2;
  PutSpace( dst, lvl );
  dst << "</" << tag << ">\n";
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::IcnDef2XML( TGstream& dst, int lvl )
{
  static char const tag[] = "Icons";
  PutSpace( dst, lvl );
  dst << "<" << tag << ">";
  lvl += 2;
  for( int i = 0 ; i < IconMap.m_nSize ; ++i )
  {
    static char const tag[] = "icon";
    CIconMap::IcnItem* pItem = &IconMap.m_pMap[i];
    PutSpace( dst, lvl );
    dst << "<" << tag;
                dst << " Id="   << ap << pItem->m_Key << ap;
                dst << " Name=" << ap << pItem->m_Str << ap;
    dst << "/>";
  }
  lvl -= 2;
  PutSpace( dst, lvl );
  dst << "</" << tag << ">\n";
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::TmplDef2XML( TGstream& dst, int lvl )
{
  char TmplPath[_MAX_PATH];
  strcpy_s( TmplPath, InitPath );
  char *p;
  ( p = strrchr( TmplPath, '\\' )) ||
  ( p = strrchr( TmplPath, '/'  )) ||
  ( p = TmplPath );
  p += p != TmplPath;
  strcpy_s( p, sizeof TmplPath - ( p - TmplPath ), "linetmpl.bin" );

  ifstream ts( TmplPath, ios::in | ios::binary );

  if( ts.fail())
    return R_OK;

  char buf[32] = "";
  char const Ttl0[] = "TGRAD_LINE_TEMPLATES";
  char const Ttl1[] = "Tgrad Line Templates";
  char const Th[]   = "TMPL";

  ts.read( buf, sizeof Ttl0 );

  int v0 = !strcmp( buf, Ttl0 );
  int v1 = !strcmp( buf, Ttl1 );
  if( !v0 && !v1 )
    return R_OK;

  int nt;
  ts.read( LPTSTR( &nt), sizeof( int ));

  static char const tag[] = "templates";
  PutSpace( dst, lvl );
  dst << "<" << tag << ">";
  lvl += 2;

  while( --nt >= 0 )
  {
    ts.read( buf, sizeof Th );
    if( ts.eof())
      break;

    if( strcmp( buf, Th ))
      break;

    int tmplIndex, scale, period, fuflo[2];
    ts.read( LPTSTR( &tmplIndex ), sizeof( int ));
    ts.read( LPTSTR( &scale ), sizeof( int ));
    ts.read( LPTSTR( &period ), sizeof( int ));
    ts.read( LPTSTR( &fuflo ), sizeof( int[CELEM(fuflo)] ));

    static char const tag[] = "style";
    PutSpace( dst, lvl );
    dst << "<" << tag;
                dst << " Id="    << ap << tmplIndex  << ap;
                dst << " Scale=" << ap << scale      << ap;
    dst << ">";
    lvl += 2;
    PutSpace( dst, lvl );
    dst << "<body><![CDATA[";
    lvl += 2;

    PutSpace( dst, lvl );
    dst << period << " ";
    int ni = 1;

    for(;;)
    {
      int pllc;
      ts.read( LPTSTR( &pllc ), sizeof( int ));
      if( ni >= 20 )
      {
        ni = 0;
        PutSpace( dst, lvl );
      }
      dst << pllc << " ";
      ni;

      if( !pllc )
        break;

      for( pllc = pllc * 2 + 1 ; --pllc >= 0 ; )
      {
        int t;
        ts.read( LPTSTR( &t ), sizeof t );
        if( ni >= 20 )
        {
          ni = 0;
          PutSpace( dst, lvl );
        }
        dst << t << " ";
        ++ni;
      }

      if( ts.eof())
        break;
    }
    lvl -= 2;
    PutSpace( dst, lvl );
    dst << "]]></body>";
    lvl -= 2;
    PutSpace( dst, lvl );
    dst << "</" << tag << ">";
  }
  lvl -= 2;
  PutSpace( dst, lvl );
  dst << "</" << tag << ">\n";
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::PRJ2XML( TGstream& dst )
{
  static char const tag[] = "project";
  dst << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  dst << "<" << tag << endl;
  dst << "  Name='" << ProjectName << "'\n";
  dst << "  Id='" << MapNumber << "'\n";

  SIZE ps = MerkatorData.GetProjectSize();
  Polar pl0, pl1;
  POINT p0 = { 0, 0 }, p1 = { ps.cx, ps.cy };
  MerkatorData.Prj2Pol( p0, pl0 );
  MerkatorData.Prj2Pol( p1, pl1 );
  int lvl0 = MerkatorData.Scale2Level( MerkatorData.GetMinScale());
  int lvl1 = MerkatorData.Scale2Level( MerkatorData.GetMaxScale());
  char* tmv;

  if( MerkatorData.TrueMerkator())
  {
    tmv = "'true'";
    MerkatorData.Prj2Pol( p0, pl0 );
    MerkatorData.Prj2Pol( p1, pl1 );
  }
  else
  {
    tmv = "'false'";
    p0.x = -ps.cx / 2;
    p0.y = -ps.cy / 2;
    pl0 = MerkatorData.Dec2Pol( p0 );
    pl1.lat = -pl0.lat;
    pl1.lon = -pl0.lon;
  }

  pl0.lon /= Deg;
  pl0.lat /= Deg;
  pl1.lon /= Deg;
  pl1.lat /= Deg;

  dst << "  Merkator=" << tmv << "\n";
  dst << "  MinLon='" << pl0.lon << "'\n";
  dst << "  MinLat='" << pl1.lat << "'\n";
  dst << "  MaxLon='" << pl1.lon << "'\n";
  dst << "  MaxLat='" << pl0.lat << "'\n";
  dst << "  MinLevel='" << lvl0 << "'\n";
  dst << "  MaxLevel='" << lvl1 << "'>\n";

  ClsDef2XML( dst, 2 );
  ClrDef2XML( dst, 2 );
  FntDef2XML( dst, 2 );
  IcnDef2XML( dst, 2 );
  TmplDef2XML( dst, 2 );

  dst << endl << "</" << tag << ">";

  return R_OK;
}
//=====================================================================
#endif _GLOBAL_SPACE
//=====================================================================
