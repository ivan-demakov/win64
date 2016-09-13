#ifndef __DRAWITEM_H__
#define __DRAWITEM_H__
//=====================================================================
#include "ksi.h"
#include "cspot.h"
#include "cbox.h"
#include "assoc.h"
#include "msutil.h"
//=====================================================================
class CMapView;
class CMapDoc;
//=====================================================================
class CItem
{
public:
  static CIntAssocMap* m_pCurColorTab;
  static CIntAssocMap* m_pCurUserColorTab;
  static COLORREF      m_SelectColor[];

public:
  static COLORREF GetSpecColor( int num, WORD flags );
  static COLORREF GetColor( int num );
};
//=====================================================================
//
class CDeltaItem: public CItem
{
public:
  int m_Delta;

public:
  CDeltaItem( ksi_obj init_list );
  CDeltaItem( int dlt = 0 ): m_Delta( dlt ){}
  CDeltaItem( const CDeltaItem &p ): m_Delta( p.m_Delta ){}
  operator ksi_obj() const;
};
//=====================================================================
//
class CColorItem: public CItem
{
public:
  int m_ColorNum;

public:
  CColorItem( ksi_obj init_list );
  CColorItem( int clr = 0 ): m_ColorNum( clr ){}
	void Create( int clr ) { m_ColorNum = clr; }
  operator ksi_obj() const;
};
//=====================================================================
//
class CStrokItem: public CColorItem
{
private:
  static const  CStrAssoc m_StyleKey[];
  static const  CStrAssoc m_DashKey[];
  static CPen   m_Pen;
  static int    m_bPenCreated;
  static LOGPEN m_OldLog;

public:
  DWORD m_Style;
  int   m_Width;

public:
  CStrokItem( ksi_obj init_list );
  CStrokItem( int color = 0, DWORD stl = PS_SOLID, int wid = 1 ):
    CColorItem( color ), m_Style( stl ), 
    m_Width( TemlateStyle() ? 0 : wid ) {}
  void Create( int color, DWORD stl, int wid )
  { 
    CColorItem::Create( color ), 
    m_Style = stl; m_Width = TemlateStyle() ? 0 : wid; 
  }
  void ScaleOn( double cf );
  operator ksi_obj() const;
  CPen* CreatePen( WORD flags ) const;
  int TemlateStyle();
  int TranslateToMap( TGstream* pDst );
  static void DeletePen();
  static ksi_obj translate_style( DWORD style, int width );
};
//=====================================================================
//
class CForeBackItem: public CItem
{
public:
  int m_ForeNum;
  int m_BackNum;
public:
  CForeBackItem( ksi_obj init_list );
  CForeBackItem( short fore = 0, short back = 0 ):
    m_ForeNum( fore ), m_BackNum( back ) {}
  void Create( short fore, short back ) { m_ForeNum = fore; m_BackNum = back; }
  operator ksi_obj() const;
};
//=====================================================================
//
class CFillItem: public CForeBackItem
{
private:
  static const CStrAssoc m_FillKey[];
  static const CStrAssoc m_HatchKey[];
  static CBrush   m_Brush;
  static int     m_bBrushCreated;
  static LOGBRUSH m_OldLog;

public:
  WORD  m_Fill;
  int   m_Hatch;
public:
  CFillItem( ksi_obj init_list );
  CFillItem( CForeBackItem const& fbi, WORD fill = BS_SOLID, int hatch = 0 ):
    CForeBackItem( fbi ),m_Fill( fill ), m_Hatch( hatch ){}
  CFillItem (){}
  void Create( int fore, int back, WORD fill, int hatch )
	  { CForeBackItem::Create( fore, back ); m_Fill = fill; m_Hatch = hatch; }
  operator ksi_obj() const;
  UINT GetBkMode() { return m_BackNum < 0 ? TRANSPARENT : OPAQUE; }
  CBrush* CreateBrush( WORD flags ) const;
  int TranslateToMap( TGstream* pDst );
  static void DeleteBrush();
};
//=====================================================================
//
class CCoordItem: public CItem, public CPoint
{
public:
  CCoordItem( ksi_obj init_list );
  CCoordItem( int X = 0, int Y = 0 ): CPoint( X, Y ){};
  CCoordItem( CPoint const& p ): CPoint( p ){};
  CCoordItem( CCoordItem const& p ): CPoint( p.x, p.y ){};
  void ScaleOn( double cf, CPoint c );
  operator ksi_obj() const;
  operator CPoint() const { return CPoint( x, y ); }
};
//=====================================================================
//
class CSizeItem: public CItem, public CSize
{
public:
  int m_Angle;
public:
  void Spin( int angle ) { m_Angle += angle; }
  void Angle0() { m_Angle = 0; }
  CSizeItem( ksi_obj init_list );
  CSizeItem( int CX = 0, int CY = 0, int ang = 0 ):
    CSize( CX, CY ), m_Angle( ang ){}
  CSizeItem( CSize const& S, int ang = 0 ):
    CSize( S ), m_Angle( ang ){}
  CSizeItem( CSizeItem const& p ):
    CSize( p.cx, p.cy ), m_Angle( p.m_Angle ){}
  void ScaleOn( double cf );
  operator CSize() const { return CSize( cx, cy ); }
  operator ksi_obj() const;
};
//=====================================================================
class CPolyItem: public CItem
{
public:
  int    m_PointNum;
  int    m_bBound;
  CPoint *m_pPoints;

public:
  int  ExtPointNum() const { return m_PointNum + m_bBound; }
  void MoveOn( CSize s );
  void SpinOn( CSize const& center, int angle );
  void ScaleOn( double cf, CPoint c );
  void DrawVerts( CDC* pDC, WORD flag );
  int  GetAngle( CSpot const& spot, int* pAngle );
	int  IsBound() { return m_bBound; }
  int  TranslateToMap( TGstream* pDst );
  CPolyItem( ksi_obj init_list, int bBound = 0 );
  CPolyItem( int pn = 0, CPoint const* points = 0, int bBound = 0 );
  void SetPoints( CPoint const* points = 0 );
  void SetPoints( int pn, CPoint const* points, int bBound = 0 );
  CPolyItem( CPolyItem const& p );
  ~CPolyItem();
  operator ksi_obj() const;
};
//=====================================================================
//
class CFontItem: public CItem
{
public:
  int m_FontNum;
  int m_Height;
  int m_Weight;
  int m_Style;

public:
  static const CStrAssoc m_WeightKey[];
  static const CStrAssoc m_StileKey[];
private:
  static CFont m_Font;
  static int   m_bFontCreated;
  static int   m_OldFontNum;
  static int   m_OldHeight;
  static int   m_OldWeight;
  static int   m_OldStyle;
  static int   m_OldDir;

public:
  CFontItem( ksi_obj init_list );
  CFontItem( CFontItem const& fi ):
    m_FontNum( fi.m_FontNum ),
    m_Height( fi.m_Height ),
    m_Weight( fi.m_Weight ),
    m_Style( fi.m_Style )
    {}
  CFontItem( int fontNum = 0, int height = 0, int weight = 0, int style = 0 ):
    m_FontNum( fontNum ),
    m_Height( height ),
    m_Weight( weight ),
    m_Style( style )
    {}
  void Create( int fontNum, int height, int weight, int style )
  {
    m_FontNum = fontNum;
    m_Height = height;
    m_Weight = weight;
    m_Style = style;  
  }

  operator ksi_obj() const;          
  CFont* CreateFont( int dir );
  static void DeleteFont();
  void ScaleOn( double cf );
};
//=====================================================================
class CStringItem: public CItem
{
public:
  CSize m_Offset;
  int   m_Angle;
  char* m_String;

public:
  void  ScaleOn( double cf );
  void  SetString( const char* str )
    { SetString( str, strlen( str )); }
  void SetString( const char* str, int slen );
  CStringItem( ksi_obj init_list );
  CStringItem( const char* str = 0, int slen = 0, int CX = 0, int CY = 0, int ang = 0 ):
    m_Offset( CX, CY ), m_Angle( ang ), m_String( 0 )
    { SetString( str, slen ); }
  CStringItem ( const CStringItem& item ):
    m_Offset( item.m_Offset ), m_Angle( item.m_Angle ), m_String( 0 )
    { SetString( item.m_String ); }
  ~CStringItem();

  operator ksi_obj() const;
};
//=====================================================================
//
class CRefItem: public CItem
{
public:
  int m_Reference;

public:
  CRefItem( ksi_obj init_list );
  CRefItem( int ref = 0 ): m_Reference( ref ){}
  CRefItem( const CRefItem &p ): m_Reference( p.m_Reference ){}
  operator ksi_obj() const;
};
//=====================================================================
#endif
