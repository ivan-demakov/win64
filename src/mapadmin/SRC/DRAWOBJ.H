#ifndef DRAWOBJ_H
#define DRAWOBJ_H
//=====================================================================
#include "drawprim.h"
#include "tpdef.h"
#include "diagitem.h"
#include "ids.h"
//=====================================================================
enum LINK_TYPE
{
  LT_NOTHING   = 0,
  LT_BEGIN     = 1,
  LT_UNDEF     = 2,
  LT_END       = 3
};

enum SHAPE_TYPE
{
  SH_NOTHING   = 0,
  SH_POINT     = 1,
  SH_POLYLINE  = 2,
  SH_BPOLYLINE = 3,
  SH_POLYGON   = 4
};

struct SHAPE_NDX
{
  SHAPE_TYPE shape;
  int        ndx;
};
//=====================================================================
class CDrawContainer : public CVisiblePrim
{
  friend class CDrawObject;

protected:
  CVisiblePrim* m_pHead;
  CVisiblePrim* m_pTail;
  int           m_nCount;

public:
  static long m_bShowAsSelected;
  static long m_bShowBox;
  static char m_bSASMap[BIT_FULL_RANGE];
  static int m_bSAS;

private:
  static CVisiblePrim* MakePrim( ksi_obj init_list );

protected:
  static int m_bHR;
  static int m_bTHR;

public:
  CDrawContainer() : m_pHead( 0 ), m_pTail( 0 ), m_nCount( 0 )
    { SetFlags( SF_LOW_VISIBLE | SF_HIG_VISIBLE ); }
  CDrawContainer( ksi_obj init_list );
  CDrawContainer( CDrawContainer* p );
  ~CDrawContainer();
  CVisiblePrim* MakeCopy() { return new CDrawContainer( this ); }
  void Draw( CDC* pDC );
  void InflateClipSize( CSize& size );
  int Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
	int GetRing( CSpot const& spot, CRect* rct );
  void GetAngle( CSpot const& spot, int* pAngle, int* pDist );
  operator ksi_obj();
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void ScaleOn( double cf, CPoint c );
  void GetParams( CDrawContainer* pPrim );
  void CalcBoundBox( CBox& box );
  void SetSelectFlag( unsigned char flags );
  int IsEmpty() { return !m_pHead; }
  int GetCount() { return m_nCount; }
  int GetType() { return KEY_CONTAINER; }
  int TranslateToMap( TGstream* pDst, int t = 0 );
  CVisiblePrim* GetHead() { return m_pHead; }
  CVisiblePrim* AddTail( CVisiblePrim* pPrim );
  CVisiblePrim* AddHead( CVisiblePrim* pPrim );
  CVisiblePrim* RemoveHead();
  CVisiblePrim* FindIndex( int nIndex );
  static void SetHR( int bHR ) { m_bHR = bHR; }
};
//=====================================================================
//=====================================================================
class CDrawObject//: public CDrawContainer
{

  friend class MapUpdater;

public:
  CDrawObject( ksi_obj init_list, CIdent id, int node );
  CDrawObject( CIdent id, int node, time_t time, clock_t clock, CDrawContainer* p );
  CDrawObject( CDrawObject* p );
  ~CDrawObject();

  int  IsObjectTemp() { return m_Ident.Entry() == -1 || m_Node == -1; }
  int  Detected( CSpot const& spot, int bStrict = 0, int* pDist = 0 );
	int  GetRing( CSpot const& spot, CRect* rct );
  void GetAngle( CSpot const& spot, int* pAngle, int* pDist );
  int  IsHiRes();
  int  TranslateToMap( TGstream* pDst, int t = 0 );
  int IsEqual( CDrawObject* pObj );
  int IsEqualTime( CDrawObject* pObj ) { return m_Time == pObj->m_Time && m_Clock == pObj->m_Clock; }
  CDrawObject* CopyObject()       { return new CDrawObject( this ); }
  CIdent const& Ident()           { return m_Ident; }
  ClassType  Class()              { return m_Ident.Class(); }
  StatusType Status()             { return m_Ident.Status(); }
  CTabIdent  TabIdent()           { return CTabIdent( m_Ident.Table(), m_Ident.Entry()); }
  TableType  Table()              { return m_Ident.Table(); }
  EntryType  Entry()              { return m_Ident.Entry(); }
  int        Node()               { return m_Node; }          
  CBox& GetBoundBox( )            { return m_BoundBox; }
  CBox& GetOriginBox()            { return m_OriginBox; }
  long GetClock()                 { return m_Clock; }
  long GetTime()                  { return m_Time; }
  void SetIdent( CIdent id )      { m_Ident = id; }
  void SetStatus( int s )         { m_Ident.SetStatus( s ); }
  void SetTable( int t )          { m_Ident.SetTable( t ); }
  void SetEntry( int e )          { m_Ident.SetEntry( e ); }
  void SetNode( int n )           { m_Node = n; }
  void SetTabId( CTabIdent id )   { m_Ident.SetTable( id.Table()); m_Ident.SetEntry( id.Entry()); }
  void SetBoundBox( CBox* pBox )  { m_BoundBox = *pBox; }
  void SetOriginBox( CBox* pBox ) { m_OriginBox = *pBox; }
  CDrawObject* Rebuild(  ksi_obj ksi_par = ksi_nil, int newClass = -1, int newStatus = -1 );
  void Draw( CDC* pDC );
  int DrawSubl( CDC* pDC );
  int SplitSubl( int bCreateAll, void* pBmp = 0 );
  int RemoveSplitSubl();
  int IsAccessable();
  void InflateClipSize( CSize& size );
  void GetParams( CDrawContainer* pPrim );
  void ScaleOn( double cf, CPoint c );
  void MoveOn( CSize const& s );
  void SpinOn( CSize const& center, int angle );
  void SetPoly( CPoint const* points, int pn );
  int  PresetHR();
  void RestoreHR( int hr );
  void SetSAS();
  void SetTimeClock( long t, long c ){ m_Time = t; m_Clock = c; }
  CBox& CalcBoundBox();
  int DrawToPoint( CSpot const& spot, CPoint* pPoint, int mode );
  CPoint* GetPoint( int* pnPnt = 0 );
  CPoint* GetPolyline( int* pnPnt = 0 );
  CPoint* GetPolygon( int* pnPnt = 0 );
  CPoint* GetAnyPoints( int* pTyp = 0, int* pnPnt = 0 );
  SHAPE_TYPE GetShapeType() { return m_ShapeNdx[Class()].shape; }
  LINK_TYPE IsLinkedTo( CDrawObject* pObj );
  int IsHostOf( CDrawObject* pObj );
  int IsHostLink( CDrawObject* pObj, LINK_TYPE lt );
  int IsSublayer();
  int IsSublayerStatic();
  void DrawSpecial( CDC* pDC );
  CString const* DetectedSpecial( CSpot const& spot );
  int IsSpecSelected()  { return m_pLegend != 0; }
  CLegend const* SetLegend( CLegend const* pLg );
  int IsAnySelected()  { return m_pLegend != 0 || m_pActualCont->SelectFlag() != 0; }
  COLORREF GetSpecSelectColor() { return m_pLegend ? m_pLegend->GetFillColor() : 0; }
	CString  GetSpecSelectText() { return m_pLegend && m_pLegend->GetSize() ? m_pDiagItem->m_Text : ""; }
  void UnselectSpecial( int bRemove = 0 );
  void SetSpecSelection( CLegend const* pLegend = 0, DiagItem* pi = 0 );
  void SetSpecSelection( CDrawObject* pSel );
  operator ksi_obj();
	int SelectFlag() { return m_pActualCont->SelectFlag(); }
	int IsSelected() { return m_pActualCont->IsSelected(); }
	void ResetSelectFlag() { m_pActualCont->ResetSelectFlag(); }
	void SetSelectFlag( int f ){ m_pActualCont->SetSelectFlag( f ); }
  CVisiblePrim* FindIndex( int nIndex ) { return m_pActualCont->FindIndex( nIndex ); }
	CDrawContainer* GetActualCont() { return m_pActualCont; }
  int operator<( CDrawObject& pObj );

  int IsExtObject() { return m_Node == -1; }

	static void InitShapeNdx( int n );
  static void InitShapeNdx();
  static void SetDrawnObject( CDrawObject* pObj ) { m_pDrawnObject = pObj; }
  static int  GetDrawnClass() { return m_pDrawnObject ? m_pDrawnObject->Class() : m_nDrawnClass; }
  static SHAPE_TYPE GetShapeType( int n ) { return m_ShapeNdx[n].shape; }
  static CDrawObject* GetDrawnObject()    { return m_pDrawnObject; }
  static void SetDrawnClass( int n ) { m_pDrawnObject = 0; m_nDrawnClass = n; }

private:
  CVisiblePrim* GetItem( int n );

private:
	CDrawContainer* m_pOriginCont;
	CDrawContainer* m_pScaledCont;
	CDrawContainer* m_pActualCont;

  CBox      m_BoundBox;
  CBox      m_OriginBox;
  CIdent    m_Ident;
  int       m_Node;
  long      m_Time;
  long      m_Clock;
  DiagItem* m_pDiagItem;
  CLegend const* m_pLegend;
  static    CDrawObject* m_pDrawnObject;
  static    int m_nDrawnClass;
  static SHAPE_NDX m_ShapeNdx[];
};
//=====================================================================
class CSmartObjPtr
{
  CDrawObject* mPtr;
public:
  CSmartObjPtr( CDrawObject* ptr = 0 ) : mPtr( ptr ) {}
  ~CSmartObjPtr();
  void* operator=( CDrawObject* ptr );
  CDrawObject* operator()() { return mPtr; }
};
//=====================================================================
#endif
