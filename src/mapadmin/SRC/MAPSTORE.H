#ifndef _MAPSTORE_H_
#define _MAPSTORE_H_
//=====================================================================
#include <stdlib.h>
#include <fstream>
#include "TGstream.h"
#include "filestr.h"
#include "spctree.h"
#include "ident.h"
#include "idntreeb.h"
#include "rcode.h"
#include "gpl.h"
#include "t.h"
#include "objdef.h"
//=====================================================================
#ifdef _DEBUG
#define _GLOBAL_SPACE
#endif
//=====================================================================
#ifdef _GLOBAL_SPACE
//=====================================================================
#include "mercator.h"
//=====================================================================
//=====================================================================
#endif// _GLOBAL_SPACE
//=====================================================================
enum
{
  STROKED        = 1,
  FILLED         = 2,
  ROUND          = 4,
  CHORD          = 8,
  OUTLINED       = STROKED | FILLED
};
//=====================================================================
struct SelectInfo
{
  Rect rect;
  PositionList poslist;
};
//=====================================================================
struct CIconMap
{
	struct IcnItem
	{
		IcnItem(){}
		int m_Key;
		int m_Sngl;
		int NativeXSize;
		int NativeYSize;
		char m_Str[64];
	};

  CIconMap() : m_pMap( 0 ), m_nSize( 0 ) {}
  ~CIconMap() { delete m_pMap; }
  void Init( char const* sectName, char const* initPath );
	IcnItem* GetItem( int key );

  IcnItem* m_pMap;
  UINT     m_nSize;
};
//=====================================================================
class MapStore
{
  friend class CMapView;
  friend class CConvDlg;
  friend class CAbstractDialog;

public:
  MapStore();
  ~MapStore();

  ResultCode GetTimeClock( time_t* pTime, clock_t* pClock );
  ResultCode IsExisting( char const* iniPath = 0 );
  ResultCode CreateEmpty( char const* iniPath = 0 );
  ResultCode Init( char const* iniPath = 0 );
  ResultCode Reinit( char const* iniPath );
  ResultCode AddObject( TGstream& src );
  ResultCode RemoveObject( CTabIdent id );
  ResultCode RemoveAllObjects( ClassType type );
  ResultCode TestObject( CTabIdent id );
  ResultCode FindObject( CTabIdent id, TGstream& dst );
  ResultCode FindObjects( CTabIdent const* pTid, int* pnId, TGstream& dst );
  ResultCode FindObjectId( CTabIdent const* pTid, int nId, CIdent* pId );
  ResultCode GetProjectHeadRecord( TGstream& dst );
  ResultCode GetVersion( long* pResult );
  ResultCode GetMaxScale( long* pResult );
  ResultCode GetUnitDec( long* pResult );
  ResultCode GetXSize( long* pResult );
  ResultCode GetYSize( long* pResult );
  ResultCode GetUniId( long* pResult );
  ResultCode SetLoadMap( char const* pMap );
  ResultCode SelectObjects( Rect const& rect1, Rect const& rect2, Rect const& rect0,
                            TGstream& dst, int limit, int compressFrom, int* pCnt );
  ResultCode SelectObjects( Rect const& rect1, Rect const& rect2, int limit1,
                            Rect const& rect0, int limit0,
                            TGstream& dst, int compressFrom, int* pCnt );
  ResultCode SelectByObject( CTabIdent id, long nClass,
                             TGstream& dst, int* pCnt );
  ResultCode SelectByRect( Rect rect, long nClass, TGstream& dst, int* pCnt );
  ResultCode SelectByClass( Rect const& rect, long* pClassArray, long nClass,
                            TGstream& dst, int* pCnt );
  ResultCode SelectObject( CTabIdent id, TGstream& dst, int* pCnt,
                           int bRemoved = 0 );
  ResultCode GetObjectBox( CTabIdent id, Rect* rect );
  ResultCode StartClassScan( long nClass );
  ResultCode NextClassScan( CIdent* id, CTabIdent* pTid );
  ResultCode NextClassScan( TGstream& dst );
  ResultCode ClipObjectsId( Point* pPnts, int nPnt, int nClass,
                            TGstream& dst, int* pnId );
  ResultCode ClipObjectsId( Point* pPnts, int nPnt, char classMap[],
                            TGstream& dst, int* pnId );
  ResultCode AcountAllObjects( long* pTabl, Point* pRgn = 0, int nRgnSize = 0 );
  ResultCode SetValidNodeList( long* pNodes );
  ResultCode SetObjectNode( CTabIdent id, int node );
  ResultCode Squeeze();
  ResultCode Flush();
  ResultCode FlushByTime();
  ResultCode Backup();
  void       Close();
  void       SetLoadMapPrt( char* ptr ) { pLoadMap = ptr; }
  void       SetSelectInfoPtr( SelectInfo* ptr ) { pSelectInfo = ptr; }
  void       SaveIndexes();
  long       GetPosScan()        { return pPosScan; }
  long       GetMaxPosScan()     { return pMaxPosScan; };
  void       SetMapNum( int n )        { MapNumber = n; }
  int        GetMapNum()               { return MapNumber; }
  void       SetFlashInterval( int t ) { FlashInterval = t; }
	char const* GetIniPath() { return InitPath; }

protected:
  ResultCode ChangeSize( long s );
  ResultCode SetModifiedFlag( int m = 1 );
  ResultCode CreateIndexes( int bSpace );
  ResultCode ExtractDataPath( char const* iniPath );
  ResultCode AddObjectTostream( std::streampos pos, TGstream& dst, int* pCnt );
  ResultCode AddObjectTostream( std::streampos pos, TGstream& dst, SelectInfo* pi0, SelectInfo* pi1, int* pCnt );
  ResultCode GetHeadRecord( std::streampos pos, std::istream* pSrc = 0 );
  ResultCode PutHeadRecord( std::streampos pos, std::ostream* pDst = 0 );
  ResultCode GetRecord( std::streampos pos, std::istream* pSrc = 0 );
  ResultCode PutRecord( std::streampos pos, std::ostream* pDst = 0 );
  ResultCode PutRecordEmp( std::streampos pos, ClassType type = 0 );
  ResultCode AddIndex( std::streampos pos, Rect const& box, int node,
                       int type, CTabIdent tabid, int bSort );
  ResultCode NextClassScan();
  void       RemoveIndex( std::streampos pos, Rect const& box, CTabIdent tabid );
  void       RemoveAllIndex();
  void       RemoveAll();

  CBaseDataRec* SelectDrh( int nVers );

protected:
  static int IsPointInPolygon( Point c, Point const* plg, int n );
  static int HorCross( int l, int r, int y, Point p0, Point p1 );
  static int VerCross( int t, int b, int x, Point p0, Point p1 );

private:
  int IsObjectClipped( TGstream& dst );
  int IsDataClipped( void** ppSrc );
  int IsLineClipped( Point p0, Point p1 );
  int IsPolygonClipped( Point const* pPoints, int nPoints, int w );
  int IsPolylineClipped( Point const* pPoints, int nPoints, int w, int bnd = 0 );
  int IsMultilineClipped( Point const* pPoints, int nPoints, int w );
  int PointsClipped( Point const* pPoints, int nPoints, int w = 0 );
  int IsTextClipped( Point tp, Point off, int angle, char const* pStr, int len, int height );

  int IsObjectValid();
  int IsDataValid( void** ppSrc, void* pEndObj );

protected:
  int       MapNumber;
  char      DataPath[_MAX_PATH];
  char      InitPath[_MAX_PATH];
  std::fstream   DataStream;

  IdentLinkedHashTab TabIdentIndex;
  
  SpaceTree SpaceIndex;
  int       ModifiedFlag;
  char*     pData;
  int       DataSize;
  CObjDef   ClsInf;

  char      LoadMap[BIT_FULL_RANGE];
  char*     pLoadMap;
  long      nClassScan;
  long      pPosScan;
  long      pMaxPosScan;
  Rect      Clip;
  Point*    Space;
  int       SpaceLength;
  int       bClipping;
  long*     pNodes;
  time_t    lastOpTime;
  time_t    FlashInterval;

  GDataRec_0101   Drh_0101;
  GDataRec_0103   Drh_0103;
  GDataRec_0107   Drh_0107;
  GDataRec_0109   Drh_0109;
  GDataRec_0111   Drh_0111;
  CBaseDataRec*   pDrh;
  DataFileHeadRec HR;
  SelectInfo      SI[3];
  SelectInfo*     pSelectInfo;
  
#ifdef _GLOBAL_SPACE
	char ProjectName[64];
	CIconMap IconMap;
	int UserClassMap[FULL_RANGE];
	int ClassRange[FULL_RANGE];

	int    HRLimit;
	int    Scale;
	double ScaleFactorX;
	double ScaleFactorY;
	double ScaleFactor;
	double ClsScaleFactor;

private:
	void InitUserClassMap( char* fileName );
	ResultCode SetClassRange( int const* pClassNum );
	ResultCode TranslateObject( word mvb, CTabIdent id, TGstream& dst );
	ResultCode TranslatePrim( void** ppSrc, word mvb, point*& pPoint, TGstream& dst );
	void TranslatePoint( point* pPnt );
	int  TranslatePolypoint( point* pPoints, int nPoints, point* pCp );
#if 0
	int  IsPrimDetected( char** ppSrc, Point pnt, int dst );
  static int IsPointInEllipse( Point c, Rect r );
  static int IsPointInRect( Point c, Rect* r, int ang );
  static int IsPointOnRect( Point c, Rect* r, int ang, int d );
  static int IsPointInSector( Point c, Rect r, int ang, int spn );
  static int IsPointInChord( Point c, Rect r, int ang, int spn );
  static int IsPointOnLine( Point c, Point p0, Point p1, int d );
  static int IsPointOnPolyline( Point c, Point* pPnt, int nPnt, int d, int ps = 1 );
	static int TestPie( Point pnt, Rect rct, int start, int delta, int dst );
	static int TestChord( Point pnt, Rect rect, int start, int delta, int dst );
	static int IsTextDetected( StringParam sp, char* string, int h, Rect box );
#endif // 0
  ResultCode GetGlobalRect( int x0, int y0, int x1, int y1, int w, int h, TGstream& dst );
  ResultCode GetGlobalGeoRect( double x0, double y0, double x1, double y1, int w, int h, TGstream& dst );
  ResultCode SelGlobalRect( int x0, int y0, int x1, int y1, int w, int h, TGstream& dst,
		                        int sx0, int sy0, int sx1, int sy1 );
  ResultCode SelGlobalGeoRect( double x0, double y0, double x1, double y1, int w, int h, TGstream& dst,
		                           double sx0, double sy0, double sx1, double sy1 );
  ResultCode GetGeoObjectBox( CTabIdent id, double* pRect );
  ResultCode PRJ2XML( TGstream& dst );
  ResultCode ClsDef2XML( TGstream& dst, int lvl );
  ResultCode ClrDef2XML( TGstream& dst, int lvl );
  ResultCode FntDef2XML( TGstream& dst, int lvl );
  ResultCode IcnDef2XML( TGstream& dst, int lvl );
  ResultCode TmplDef2XML( TGstream& dst, int lvl );

#endif// _GLOBAL_SPACE

protected:
  static char General[];
  static char MapHeader[];
};
//=====================================================================
#endif