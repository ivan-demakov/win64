//=====================================================================
#include "stdafx.h"

#include "mapstore.h"
#include "msutil.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
#ifdef _JSON
//=====================================================================
#ifdef _CLIENT_APP
  #ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char BASED_CODE THIS_FILE[] = __FILE__;
  #endif
#endif
//=====================================================================
static char
Quote = '"',
Unkn = '?',
ValSep = ',',
PrpSep = ':',
BlockBeg = '{',
BlockEnd = '}',
ListBeg = '[',
ListEnd = ']',
False[] = "false",
True[] = "true",
MapNum[] = "MapNum",
PrjType[] = "ProjectType",
Parallel_1[] = "Parallel_1",
Semimajor[] = "Semimajor",
Semiminor[] = "Semiminor",
XC0[] = "XC0",
YC0[] = "YC0",
LN0[] = "LN0",
LT0[] = "LT0",
LayerStruct[] = "LayerStruct",
Name[] = "Name",
Id[] = "id",
ParentId[] = "ParentId",
Content[] = "Content",
Table[] = "Table",
Object[]= "Object",
Status[] = "Status",
Type[] = "Type",
Pen[] = "Pen",
Color[] = "ColorNum",
Style[] = "Style",
Width[] = "Width",
Brush[] = "Brush",
BkColor[] = "BkColor",
Hatch[] = "Hatch",
Font[] = "Font",
Fonts[] = "Fonts",
FaceNum[] = "FaceNum",
Height[] = "Height",
Weight[] = "Weight",
Italic[] = "Italic",
Underline[] = "Underline",
Icon[] = "Icon",
Coord[] = "Coord",
Size[] = "Size",
Text[] = "Text",
String[] = "String",
Angle[] = "Angle",
Span[] = "Span",
TextByLine[] = "TextByLine",
Footnote[] = "Footnote",
Plylin[] = "Polyline",
Plygon[] = "Polygon",
Mline[] = "MultiLine",
MEllipse[] = "Ellipse",
MRectangle[] = "Rectangle",
MArc[] = "Arc",
MChord[] = "Chord",
MPie[] = "Pie",
Red[] = "Red",
Green[] = "Green",
Blue[] = "Blue",

Merk[] = "MERK";
//=====================================================================
ResultCode
MapStore::SelectMapFonts( TGstream& dst )
{
  if( MapNumber < 0 )
    return R_MAPNOTOPEN;

  char SectName[] = "FontFace";
  char entrs[16384], buf[512];

  dst << BlockBeg << Quote << Fonts << Quote << PrpSep << ListBeg << endl;

  if( GetPrivateProfileString( SectName, 0, "", entrs, sizeof entrs, InitPath ))
  {
    int nl;
    int bNext = 0;
    for( char* name = entrs ; *name ; name += nl )
    {
      nl = strlen( name ) + 1;
      GetPrivateProfileString( SectName, name, "", buf, sizeof buf, InitPath );

      if( bNext )
        dst << ValSep << endl;
      dst << BlockBeg
          << Quote << Id << Quote << PrpSep << name << ValSep
          << Quote << Font << Quote << PrpSep << Quote << buf << Quote << BlockEnd;
      bNext = 1;
    }
  }

  dst << ListEnd << BlockEnd << endl << endl;

  return R_OK;
}
//=====================================================================
ResultCode
MapStore::SelectMapColors( TGstream& dst, int bUserColors )
{
  if( MapNumber < 0 )
    return R_MAPNOTOPEN;

  char* SectName = bUserColors ? "UserColorTab" : "ColorTab";
  char entrs[16384], buf[512];

  dst << BlockBeg << Quote << SectName << Quote << PrpSep << ListBeg << endl;

  if( GetPrivateProfileString( SectName, 0, "", entrs, sizeof entrs, InitPath ))
  {
    int nl;
    int bNext = 0;
    for( char* name = entrs ; *name ; name += nl )
    {
      nl = strlen( name ) + 1;
      GetPrivateProfileString( SectName, name, "", buf, sizeof buf, InitPath );

      int c = 0;
      sscanf( buf, "%x", &c );
      int r = c & 0xff;
      int g = ( c >> 8 ) & 0xff;
      int b = ( c >> 16 ) & 0xff;

      if( bNext )
        dst << ValSep << endl;

      dst << BlockBeg
          << Quote << Id << Quote << PrpSep << name << ValSep
          << Quote << Red << Quote << PrpSep << r << ValSep
          << Quote << Green << Quote << PrpSep << g << ValSep
          << Quote << Blue << Quote << PrpSep << b << BlockEnd;
      bNext = 1;
    }
  }

  dst << ListEnd << BlockEnd << endl << endl;

  return R_OK;
}
//=====================================================================
ResultCode
MapStore::SelectMapColors( TGstream& dst )
{
  if( MapNumber < 0 )
    return R_MAPNOTOPEN;

  SelectMapColors( dst, 0 );
  SelectMapColors( dst, 1 );
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::SelectMapPrj( TGstream& dst )
{
  if( MapNumber < 0 )
    return R_MAPNOTOPEN;

  dst << BlockBeg;
  if( !strncmp( HR.PrjType, Merk, sizeof HR.PrjType ))
  {
    dst << Quote << PrjType    << Quote << PrpSep << Quote << Merk << Quote << ValSep << endl;
    dst << Quote << Parallel_1 << Quote << PrpSep << HR.Parallel_1 << ValSep << endl;
    dst << Quote << Semimajor  << Quote << PrpSep << HR.Semimajor << ValSep << endl;
    dst << Quote << Semiminor  << Quote << PrpSep << HR.Semiminor << ValSep << endl;
    dst << Quote << XC0 << Quote << PrpSep << HR.XC0 << ValSep << endl;
    dst << Quote << YC0 << Quote << PrpSep << HR.YC0 << ValSep << endl;
    dst << Quote << LN0 << Quote << PrpSep << HR.LN0 << ValSep << endl;
    dst << Quote << LT0 << Quote << PrpSep << HR.LT0;
  }
  else
    dst << Quote << PrjType << Quote << PrpSep << Quote << Unkn << Quote;
  dst << BlockEnd << endl << endl;
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::SelectMapLayers( TGstream& dst )
{
  if( MapNumber < 0 )
    return R_MAPNOTOPEN;

  int i;
  int ln[CLASS_RANGE];
  memset( ln, 0, sizeof ln );
  for( i = 0 ; i < CLASS_RANGE ; ++i )
  {
    int n = GetOrderNum( i );
    ln[n] = i;
  }

  dst << BlockBeg << Quote << LayerStruct  << Quote << PrpSep << ListBeg << endl;
  int bFst = 1;
  for( i = 0 ; i < CLASS_RANGE ; ++i )
  {
    int t = ln[i];
    ClassInfo* pCl = &ClsInf[t];
    if( pCl->Name )
    {
      if( !bFst )
        dst << ValSep << endl;
      dst << BlockBeg
          << Quote << Name << Quote << PrpSep << Quote << pCl->Name << Quote << ValSep
          << Quote << Id << Quote << PrpSep << t << ValSep
          << Quote << ParentId << Quote << PrpSep << pCl->ParentNum << BlockEnd;
      bFst = 0;
    }
  }
  dst << ListEnd << BlockEnd << endl << endl;
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::SelectMapRect( Rect const& rect, char* pMask, int Scale, int bHr, int bSl, TGstream& dst )
{
  if( MapNumber < 0 )
    return R_MAPNOTOPEN;

  int i;
  int ln[CLASS_RANGE];
  TGstream Str[CLASS_RANGE];
  TGstream* pStr[CLASS_RANGE];

  memset( pStr, 0, sizeof pStr );

  for( i = 0 ; i < CLASS_RANGE ; ++i )
  {
    int n = GetOrderNum( i );
    pStr[n] = &Str[i];
    ln[n] = i;
  }

  Clip = rect;

  PositionList PosList;
  int n = SpaceIndex.SelectObjects( rect, Scale, 0, PosList );
  PosList.SetFirst();

  while( --n >= 0 && ObjRecord2Json( PosList.GetNext(), pMask, bHr, bSl, Str ) == R_OK  );

  dst << BlockBeg;
  dst << Quote << MapNum << Quote << PrpSep << MapNumber << ValSep << endl;
  dst << Content << PrpSep << ListBeg << endl;

  int bNext = 0;

  for( i = 0 ; i < CLASS_RANGE ; ++i )
  {
    TGstream* ps = pStr[i];
    if( !ps )
      continue;
    int size = ps->pcount();
    if( size )
    {
      if( bNext )
        dst << ValSep << endl;
      bNext = 1;
      dst << endl << BlockBeg
          << Quote << Id << Quote << PrpSep << ln[i] << ValSep << endl
          << Quote << Content << Quote << PrpSep << ListBeg << endl;
      dst.write( ps->str(), size );
      dst << endl << ListEnd << BlockEnd;
    }
  }
  dst << ListEnd << BlockEnd << endl;

  return R_OK;
}
//=====================================================================
ResultCode
MapStore::ObjRecord2Json( streampos pos, char* pMask, int bHr, int bSl, TGstream* pDst )
{
  ResultCode r =  GetRecord( pos );
  if( r != R_OK )
    return r;

  int t = pDrh->GetClass();
  if( !pMask[t] )
    return r;

  void* pSrc = pData;
  void* pTmp = pSrc;
  if( !IsDataClipped( &pTmp ))
    return r;

  TGstream& dst = pDst[t];
  int bNext = dst.pcount() != 0;

  UINT tb = pDrh->GetTable();
  UINT en = pDrh->GetEntry();
  UINT st = pDrh->GetStatus();

  if( bNext )
    dst << ValSep << endl;
  dst << endl << BlockBeg;
  if( st )
    dst << Quote << Status  << Quote << PrpSep << st << ValSep << endl;
  if( tb )
    dst << Quote << Table   << Quote << PrpSep << tb << ValSep << endl;
  dst << Quote << Object  << Quote << PrpSep << en << ValSep << endl;
  dst << Quote << Content << Quote << PrpSep << ListBeg << endl;
  Rec2Json( !bNext, &pSrc, bHr, bSl, dst );
  dst << ListEnd << BlockEnd;

  return r;
}
//=====================================================================
static void
WritePoints( TGstream& dst, char* name, point* pPnt, int nPnt, int bBound = 0 )
{
  point* bPnt = pPnt;
  dst << Quote << name << Quote << PrpSep;
  if( nPnt > 1 )
    dst << ListBeg;
  for( int np = 3 ; --nPnt >= 0 ; ++pPnt )
  {
    dst << ListBeg << pPnt->x << ValSep << pPnt->y << ListEnd;
    if( nPnt || bBound )
      dst << ValSep;
    if( --np == 0 && nPnt )
    {
      np = 4;
      dst << endl;
    }
  }
  if( bBound )
    dst << ListBeg << bPnt->x << ValSep << bPnt->y << ListEnd;
  if( nPnt > 1 )
    dst << ListEnd;
}
//=====================================================================
static void
WritePenBrush( TGstream& dst, PenParam* pPen, BrushParam* pBrush )
{
  if( pPen )
  {
    dst << ValSep << endl << Quote << Pen << Quote << PrpSep << BlockBeg
        << Quote << Color << Quote << PrpSep << pPen->color;
    int w = pPen->style > PS_ALTERNATE ? 0 : pPen->width;
    if( w > 0 )
      dst << ValSep << Quote << Width << Quote << PrpSep << w;
    if( pPen->style != 0 )
      dst << ValSep << Quote << Style << Quote << PrpSep << pPen->style;
    dst << BlockEnd;
  }

  if( pBrush )
  {
    dst << ValSep << endl << Quote << Brush << Quote << PrpSep << BlockBeg;
    dst << Quote << Color << Quote << PrpSep << pBrush->fore;
    if( pBrush->fill == BS_HATCHED && pBrush->back >= 0 )
      dst << ValSep << Quote << BkColor << Quote << PrpSep << pBrush->back;
    if( pBrush->fill == BS_HATCHED )
      dst << ValSep << Quote << Hatch << Quote << PrpSep << pBrush->hatch;
    dst << BlockEnd;
  }
}
//=====================================================================
static void
WriteFont( TGstream& dst, PrimRecord* pPr, int vers, int bText )
{
  int font, height, weight, style, color;

  if( vers < 107 )
  {
    FontParam* pFp = bText ? &pPr->param.text.param.full.FontParams.font :
                             &pPr->param.textbyline.FontParams.font;
    font = pFp->number;
    height = pFp->height;
    weight = pFp->weight;
    style = pFp->style;
    color = pFp->color;
  }
  else
  {
    FontParam1* pFp = bText ? &pPr->param.text.param.full.FontParams.font1 :
                              &pPr->param.textbyline.FontParams.font1;
    font = pFp->number;
    height = abs( pFp->height );
    weight = pFp->weight;
    style = pFp->style;
    color = pFp->color;
  }

  dst << ValSep << endl << Quote << Font << Quote  << PrpSep
      << BlockBeg << Quote << FaceNum << Quote << PrpSep << font
      << ValSep   << Quote << Color   << Quote << PrpSep << color
      << ValSep   << Quote << Height  << Quote << PrpSep << height;
  if( weight )
    dst << ValSep << Quote << Weight  << Quote << PrpSep << weight;
  if( style & 1 )
    dst << ValSep << Quote << Italic << Quote << PrpSep << True;
  if( style & 2 )
    dst << ValSep << Quote << Underline << Quote << PrpSep << True;
  dst << BlockEnd;
}
//=====================================================================
static char*
MkAngle( int a )
{
  static char t[16];
  sprintf( t, "%1.1f", a / 10. );
  return t;
}
//=====================================================================
ResultCode
MapStore::Rec2Json( int bFirst, void** ppSrc, int bHr, int bSl, TGstream& dst )
{
  enum
  {
    STROKED        = 0x01,
    FILLED         = 0x02,
    ROUND          = 0x04,
    SMART          = 0x08,
    CHORD          = 0x10,
    OUTLINED       = STROKED  | FILLED,
    SMART_OUTLINED = OUTLINED | SMART,
    ROUND_STROKED  = ROUND    | STROKED,
    ROUND_FILLED   = ROUND    | FILLED,
    ROUND_OUTLINED = ROUND    | OUTLINED
  };

  ResultCode r =  R_OK;

  PrimRecord* pPr = (PrimRecord*)*ppSrc;

  if(( pPr->type & PF_TYPE_MASK ) == PF_CONTAINER )
  {
    int n = pPr->param.container.length;
    *ppSrc = pPr->param.container.prims;

    int bNext = 0;
    while( --n >= 0 )
    {
      pPr = (PrimRecord*)*ppSrc;
      int typeKey = pPr->type & PF_TYPE_MASK;
      Drawable* pObjPar  = (Drawable*)&pPr->param;
      PenParam*   pPen   = 0;
      BrushParam* pBrush = 0;
      int subType = (( pPr->type & PF_STROKED ) ? STROKED : 0 ) |
                    (( pPr->type & PF_FILLED  ) ? FILLED  : 0 ) |
                    (( pPr->type & PF_CHORD   ) ? CHORD   : 0 ) |
                    (( pPr->type & PF_SMART   ) ? SMART   : 0 ) |
                    (( pPr->type & PF_ROUND   ) ? ROUND   : 0 );

      int bVisible = pPr->type & PF_VISIBLE;

      if( bVisible )
        switch( subType & OUTLINED )
        {
          case STROKED :
            pPen    = bFirst ? &pPr->param.stroked.pen : 0;
            pObjPar = &pPr->param.stroked.primParam;
            break;
          case FILLED :
            pBrush  = bFirst ? &pPr->param.filled.brush : 0;
            pObjPar = &pPr->param.filled.primParam;
            break;
          case OUTLINED :
            pPen    = bFirst ? &pPr->param.outlined.pen : 0;
            pBrush  = bFirst ? &pPr->param.outlined.brush : 0;
            pObjPar = &pPr->param.outlined.primParam;
            break;
        }

      if(( pPr->type & PF_SVISIBLE ) && !bSl )
        bVisible = 0;

      switch( typeKey )
      {
        case PF_CONTAINER:
          if( bNext )
            dst << ValSep << endl;
          r = Rec2Json( bFirst, ppSrc, bHr, bSl, dst );
          break;

        default:
          return R_BADFILESTRUCT;

        case PF_POINT:
          *ppSrc = &pPr->param.pnt + 1;
          break;

        case PF_ICON:
          if( bVisible )
          {
            if( bNext )
              dst << ValSep << endl;
            dst << BlockBeg << Quote << Type << Quote << PrpSep << Quote << Icon << Quote
                << ValSep << Quote << Id << Quote << PrpSep << pPr->param.icon.id
                << ValSep << endl;
            WritePoints( dst, Coord, &pPr->param.pnt, 1 );
            dst << BlockEnd << endl;
            bNext = 1;
          }
          *ppSrc  = &pPr->param.icon + 1;
          break;

        case PF_TEXT:
        {
          StringParam* pSP = bVisible ? &pPr->param.text.param.full.string :
                                        &pPr->param.text.param.string;
          if( bVisible && pSP->length )
          {
            if( bNext )
              dst << ValSep << endl;
            dst << BlockBeg;
            dst << Quote << Type << Quote << PrpSep << Quote << Text << Quote;
            if( bFirst )
              WriteFont( dst, pPr, HR.Version, 1 );
            dst << ValSep << endl << Quote << String << Quote << PrpSep << Quote;
            dst.write( pSP->string, pSP->length );
            dst << Quote << ValSep;
            double angle = Deg2Rad( pSP->angle );
            double vcos = cos( angle ), vsin = sin( angle );
            point org;
            org.x = pPr->param.text.org.x + pSP->offset.x * vcos + pSP->offset.y * vsin;
            org.y = pPr->param.text.org.y + pSP->offset.y * vcos - pSP->offset.x * vsin;
            WritePoints( dst, Coord, &org, 1 );
            if( pSP->angle )
               dst << ValSep << Quote << Angle << Quote << PrpSep << MkAngle( pSP->angle );
            dst << BlockEnd;
            bNext = 1;
          }
          *ppSrc = pSP->string + pSP->length;
          break;
        }

        case PF_TEXTBYLINE:
        {
          int length = pPr->param.textbyline.length;
          TxtItem* pItem = pPr->param.textbyline.items;

          if( bVisible && length )
          {
            if( bNext )
              dst << ValSep << endl;
            for( int i = 0 ; i < length ; ++i, ++pItem )
            {
              if( i )
                dst << ValSep << endl;
              dst << BlockBeg << Quote << Type << Quote << PrpSep << Quote << Text << Quote;

              if( bFirst )
              {
                WriteFont( dst, pPr, HR.Version, 0 );
                bFirst = 0;
              }
              dst << ValSep << Quote << String << Quote << PrpSep << Quote;
              dst.write( pItem->letter, 1 );
              dst << Quote << ValSep;
              WritePoints( dst, Coord, &pItem->org, 1 );
              dst << ValSep << Quote << Angle << Quote << PrpSep << MkAngle( pItem->angle ) << BlockEnd;
            }
            bNext = 1;
          }
          *ppSrc = pItem + length;
          break;
        }

        case PF_FOOTNOTE:
        case PF_POLYPOINT:
        {
          int nPoints = pObjPar->polypoint.length;
          point* pPnt = pObjPar->polypoint.points;
          if( bVisible && nPoints )
          {
            if( bNext )
              dst << ValSep << endl;
            int bBound = 0;
            char* cType = 0;
            switch( subType )
            {
              case STROKED : cType = typeKey == PF_FOOTNOTE ? Footnote : Plylin;
                             break;
              case SMART_OUTLINED :
              case OUTLINED :
              case FILLED : cType = Plygon;
                            bBound = 1;
                            break;
            }
            if( !( HR.Version & 1 ))
              for( int i = 0 ; ++i < nPoints ; pPnt[i].x += pPnt[i-1].x, pPnt[i].y += pPnt[i-1].y );
            dst << BlockBeg;
            dst << Quote << Type << Quote << PrpSep << Quote << cType << Quote;
            if( subType != PF_FOOTNOTE )
              WritePenBrush( dst, pPen, pBrush );
            dst << ValSep << endl;
            WritePoints( dst, Coord, pPnt, nPoints, bBound );
            dst << BlockEnd;
            bNext = 1;
          }
          *ppSrc = pPnt + nPoints;
          break;
        }

        case PF_MULTILINE:
        {
          int nPoints = pObjPar->polypoint.length;
          point* pPnt = pObjPar->polypoint.points;
          if( bVisible && nPoints )
          {
            if( bNext )
              dst << ValSep << endl;
            if( !( HR.Version & 1 ))
              for( int i = 0 ; ++i < nPoints ; pPnt[i].x += pPnt[i-1].x, pPnt[i].y += pPnt[i-1].y );
            dst << BlockBeg;
            dst << Quote << Type << Quote << PrpSep << Quote << Mline << Quote;
            WritePenBrush( dst, pPen, pBrush );
            dst << ValSep << endl;
            WritePoints( dst, Coord, pPnt, nPoints );
            dst << BlockEnd;
            bNext = 1;
          }
          *ppSrc = pPnt + nPoints;
          break;
        }

        case PF_ELLIPSE :
        {
          rect* pr = &pObjPar->ellipse;
          if( bVisible )
          {
            if( bNext )
              dst << ValSep << endl;
            dst << BlockBeg << Quote << Type << Quote << PrpSep << Quote << MEllipse << Quote;
            point org, size;
            org.x = pr->left;
            org.y = pr->top;
            size.x = pr->right - pr->left;
            size.y = pr->bottom - pr->top;
            WritePenBrush( dst, pPen, pBrush );
            dst << ValSep;
            WritePoints( dst, Coord, &org, 1 );
            dst << ValSep;
            WritePoints( dst, Size, &size, 1 );
            dst << BlockEnd;
            bNext = 1;
          }
          *ppSrc = pr + 1;
          break;
        }

        case PF_RECTANGLE :
        {
          RectParam* pr = &pObjPar->rectangle;
          if( bVisible )
          {
            if( bNext )
              dst << ValSep << endl;
            dst << BlockBeg << Quote << Type << Quote << PrpSep << Quote << MRectangle << Quote;
            point org, size;
            org.x = pr->left;
            org.y = pr->top;
            size.x = pr->right - pr->left;
            size.y = pr->bottom - pr->top;
            WritePenBrush( dst, pPen, pBrush );
            WritePoints( dst, Coord, &org, 1 );
            dst << ValSep;
            WritePoints( dst, Size, &size, 1 );
            dst << ValSep << Quote << Angle << Quote << PrpSep << MkAngle( pr->angle ) << BlockEnd;
            bNext = 1;
          }
          *ppSrc = pr + 1;
          break;
        }

        case PF_ARC :
        {
          ArcParam* pr = &pObjPar->arc;
          char* type = subType == FILLED ? MPie :
                       subType == STROKED ? MArc :
                       subType == FILLED|CHORD ? MPie : 0;
          if( bVisible && type )
          {
            if( bNext )
              dst << ValSep << endl;
            dst << BlockBeg << Quote << Type << Quote << PrpSep << Quote << MArc << Quote;
            point org, size;
            org.x = pr->left;
            org.y = pr->top;
            size.x = pr->right - pr->left;
            size.y = pr->bottom - pr->top;
            WritePenBrush( dst, pPen, pBrush );
            WritePoints( dst, Coord, &org, 1 );
            dst << ValSep;
            WritePoints( dst, Size, &size, 1 );
            dst << ValSep << Quote << Angle << Quote << PrpSep << MkAngle( pr->angle )
                << ValSep << Quote << Span  << Quote << PrpSep << MkAngle( pr->delta )
                << BlockEnd;
            bNext = 1;
          }
          *ppSrc = pr + 1;
          break;
        }
      }
    }
  }

  return r;
}
//=====================================================================
#endif// _JSON

