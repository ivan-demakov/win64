#ifndef GRAPHDOC_H
#define GRAPHDOC_H
//=====================================================================
#include "tpdef.h"
#include "ident.h"
#include "cspot.h"
#include "objdef.h"
#include "ids.h"

#include "objtree.h"
//=====================================================================
class CDrawObject;
class CIdent;
class CBox;
//=====================================================================
class CLayer : public CObjTreeList
{
public:
  CLayer() : 
    m_bVisible( 1 )
  {}
  void CalcBoundBox( CBox& );
  void SetVisible( int vsb ) { m_bVisible = vsb; }
  void Draw( CDC* pDC );
  void Draw( CDC* pDC, CPoint const& center, int angle );
  void SelectByRect( CRect const& rect, CObjList& list );
  void SelectBySpot( CSpot const& spot, CObjList& list );
  CDrawObject* SelectBySpot( CSpot const& spot, int bStrict, int* pDist );
  void GetAngle( CSpot const& spot, int* pAngle, int* pDist, int bPresel );
  int  LayerOn() { return m_bVisible; }
  int  TestPointObject( CPoint tp );
  CDrawObject* FindPrev( CSpot const* pSpot = 0 );
  CDrawObject* FindNext( CSpot const* pSpot = 0 );

protected:
  int m_bVisible;
};
//=====================================================================
// array of layers

enum
{
  SUBLAYER = 1,
  OBJLAYER = 2
};

class CGraphData
{
public:

  CGraphData() : m_LayerNum( 0 ), m_SubNum( 0 ), m_nLayers( 0 ) {}

  void ClipObjects( CRect const* pRect, int bSaveSel );
  void SetLayersRange( CObjDef* pObjDef );
  void BeginGlobalSearch( CSpot const &spot );
  void Draw( CDC* pDC, int msk );
  void Draw( CDC* pDC, int msk, CPoint const& center, int angle );
  int  GetAngle( CSpot const& spot, int* pAngle );
  int  TestPointObject( CPoint tp );

  CBox CalcBoundBox();

  CLayer* GetFirstLayer( int bSub );
  CLayer* GetLastLayer( int bSub );
  CLayer* GetNextLayer();
  CLayer* GetPrevLayer();
  CLayer* GetLayerByClass( int type );

  void SelectByRect( CRect const& rect, CObjList& list );
  void SelectBySpot( CSpot const& spot, CObjList& list );
  CDrawObject* SelectBySpot( CSpot const& spot, int bStrict );
  CObjList& SelectByClass( ClassType type, CObjList& list );

  CDrawObject* NextGlobalSearch();
  CDrawObject* AddObject( CDrawObject* pObj );
  CDrawObject* GetObjectPtr( CIdent const& id );
  CDrawObject* GetObjectPtr( CTabIdent id );
  CDrawObject* RemoveObject( CIdent const& id );
  CDrawObject* RemoveObject( CDrawObject* pObj );

private:
  CLayer  m_Layers[FULL_RANGE];
  CLayer* m_pLayer[FULL_RANGE];
  CSpot   m_SearchSpot;
  int     m_LayerNum;
  int     m_SubNum;
  int     m_nLayers;
  int     m_bSub;
};
//=====================================================================
#endif
