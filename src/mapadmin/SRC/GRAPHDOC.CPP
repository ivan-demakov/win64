#include "stdafx.h"

#include "cspot.h"
#include "ident.h"
#include "ksi.h"
#include "drawobj.h"
#include "graphdoc.h"
#include "movemode.h"

#include "dwin.h"
#include "mview.h"
#include "mdoc.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "mathutil.h"
#include "prndlg.h"
#include "util.h"
#include <gdiplus.h>
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CDrawObject*
CLayer::FindNext( CSpot const* pSpot )
{
  CDrawObject* pObj = 0;
	int dist = MAX_INT;
  if( m_bVisible )
    while(( pObj = GetNext()) && pSpot && !pObj->Detected( *pSpot, 0, &dist ));
  return pObj;
}
//=====================================================================
CDrawObject*
CLayer::FindPrev( CSpot const* pSpot )
{
  CDrawObject* pObj = 0;
	int dist = MAX_INT;
  if( m_bVisible )
    while(( pObj = GetPrev()) && pSpot && !pObj->Detected( *pSpot, 0, &dist ));
  return pObj;
}
//=====================================================================
void
CLayer::SelectBySpot( CSpot const& spot, CObjList& list )
{
  if( !m_bVisible )
		return;

  for( CDrawObject* pObj = SetFirst() ; pObj = GetNext() ; )
    if( pObj->Detected( spot, 0 ))
      list.AddHead( pObj );
}
//=====================================================================
void
CLayer::SelectByRect( CRect const& rect, CObjList& list )
{
  if( !m_bVisible )
		return;

  for( CDrawObject* pObj = SetFirst() ; pObj = GetNext() ; )
    if( rect.PtInRect( pObj->GetBoundBox().TopLeft()) &&
			  rect.PtInRect( pObj->GetBoundBox().BottomRight()))
      list.AddHead( pObj );
}
//=====================================================================
CDrawObject*
CLayer::SelectBySpot( CSpot const& spot, int bStrict, int* pDist  )
{
  if( !m_bVisible )
		return 0;

  CDrawObject* pFnd = 0;
  CDrawObject* pObj = SetLast();
  while(( pObj = GetPrev()) && *pDist )
    if( pObj->Detected( spot, bStrict, pDist ))
      pFnd = pObj;

  return pFnd;
}
//=====================================================================
void
CLayer::GetAngle( CSpot const& spot, int* pAngle, int* pDist, int bPresel )
{
  if( m_bVisible )
  {
    CDrawObject* pObj = SetLast();
    while( pObj = GetPrev())
			if( bPresel == pObj->IsSelected())
        pObj->GetAngle( spot, pAngle, pDist );
  }
}
//=====================================================================
void
CLayer::Draw( CDC* pDC )
{
  if( m_bVisible && !IsEmpty())
  {
    CDrawObject* pObj = GetFirst();
    pObj->SetSAS();
    while( pObj )
    {
      if( !pObj->SelectFlag() && pDC->RectVisible( &pObj->GetBoundBox()))
        pObj->Draw( pDC );
      pObj = GetNext();
    }
  }
}
//=====================================================================
void
CLayer::Draw( CDC* pDC, CPoint const& center, int angle )
{
  if( m_bVisible && !IsEmpty())
  {
    CDrawObject* pObj = GetFirst();
    pObj->SetSAS();
    while( pObj )
    {
      if( !pObj->SelectFlag())
      {
        pObj->SpinOn( center, angle );
        if( pDC->RectVisible( &pObj->GetBoundBox()))
          pObj->Draw( pDC );
      }
      pObj = GetNext();
    }
  }
}
//=====================================================================
void
CLayer::CalcBoundBox( CBox& rBox )
{
  for( CDrawObject* pObj = SetFirst() ; pObj = GetNext() ; rBox |= pObj->GetBoundBox());
}
//=====================================================================
//=====================================================================
CBox
CGraphData::CalcBoundBox()
{
  CBox box;
  for( CLayer* pl = GetFirstLayer( 0 ) ; pl = GetNextLayer() ; pl->CalcBoundBox( box ));
  return box;
}
//=====================================================================
CLayer*
CGraphData::GetFirstLayer( int bSub )
{
  m_bSub = bSub;
  m_LayerNum = bSub ? 0 : m_SubNum;
  return 0;
}
//=====================================================================
CLayer*
CGraphData::GetLastLayer( int bSub )
{
  m_bSub = bSub;
  m_LayerNum = bSub ? m_SubNum : m_nLayers;
  return 0;
} 
//=====================================================================
CLayer*
CGraphData::GetPrevLayer()
{
  CLayer* pLayer = m_LayerNum > ( m_bSub ? 0 : m_SubNum ) ? m_pLayer[--m_LayerNum] : 0;
  if( pLayer && !pLayer->IsEmpty())
    pLayer->GetFirst()->SetSAS();
  return pLayer;
}
//=====================================================================
CLayer*
CGraphData::GetNextLayer()
{
  CLayer* pLayer = m_LayerNum < ( m_bSub ? m_SubNum : m_nLayers ) ? m_pLayer[m_LayerNum++] : 0;
  if( pLayer && !pLayer->IsEmpty())
    pLayer->GetFirst()->SetSAS();
  return pLayer;
}
//=====================================================================
void
CGraphData::SetLayersRange( CObjDef* pObjDef )
{
  pObjDef->MakeRange();

	memset( m_pLayer, 0, sizeof m_pLayer );
	m_nLayers = pObjDef->GetClsNum();
	m_SubNum  = pObjDef->GetSubNum();
  for( int i = 0 ; i < m_nLayers ; ++i )
	{
		int t = pObjDef->GetClassInRange( i );
		m_pLayer[i] = m_Layers + t;
	}	

  m_pLayer[m_nLayers++] = &m_Layers[0];
}
//=====================================================================
CLayer* 
CGraphData::GetLayerByClass( int type )
{
  return &m_Layers[type];
}
//=====================================================================
CDrawObject*
CGraphData::RemoveObject( CDrawObject* pObj )
{
  return pObj ? RemoveObject( pObj->Ident()) : 0;
}
//=====================================================================
CDrawObject*
CGraphData::RemoveObject( CIdent const& id )
{
  return GetLayerByClass( id.Class())->RemoveObject( id.Entry());
}
//=====================================================================
void
CGraphData::ClipObjects( CRect const* pRect, int bSaveSel )
{
  for( int i = m_nLayers ; --i >= 0 ; m_pLayer[i]->RemoveAll( pRect, bSaveSel ));
}
//=====================================================================
void
CGraphData::Draw( CDC* pDC, int msk )
{
  if( !m_nLayers )
    return;

  if(( msk & OBJLAYER ) && !pDC->IsPrinting())
  {
    CSize s( 0, 0 );
    for( POSITION pos = pView->m_SelectList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = pView->m_SelectList.GetNext( pos );
      if( pDC->RectVisible( &pObj->GetBoundBox()))
        pObj->InflateClipSize( s );
    }    
    CRect scb;
    pDC->GetClipBox( &scb );
    scb.InflateRect( s );
    pDC->SelectClipRgn( 0 );
    pDC->IntersectClipRect( &scb );
  }

  int OldMode = pDC->SetROP2( R2_COPYPEN );
  for( int lt = SUBLAYER ; lt <= OBJLAYER ; ++lt )
    if( msk & lt )
      for( CLayer* pl = GetFirstLayer( lt == SUBLAYER ) ; pl = GetNextLayer() ; pl->Draw( pDC ));
  pDC->SetROP2( OldMode );
}
//=====================================================================
void
CGraphData::Draw( CDC* pDC, int msk, CPoint const& center, int angle )
{
  int OldMode = pDC->SetROP2( R2_COPYPEN );
  for( int lt = SUBLAYER ; lt <= OBJLAYER ; ++lt )
    if( msk & lt )
    {
      int sdc = pDC->SaveDC();
      for( CLayer* pl = GetFirstLayer( lt == SUBLAYER ) ; pl = GetNextLayer() ; pl->Draw( pDC, center, angle ));
      pDC->RestoreDC( sdc );
    }
  pDC->SetROP2( OldMode );
}
//=====================================================================
CDrawObject*
CGraphData::GetObjectPtr( CIdent const& id )
{
  return GetLayerByClass( id.Class())->Find( id.TabIdent());
}
//=====================================================================
CDrawObject*
CGraphData::GetObjectPtr( CTabIdent id )
{
  CDrawObject* pObj;
  for( int i = FULL_RANGE ; --i >= 0 ; )
  {
    CLayer* pl = GetLayerByClass( i );
    if( pl && ( pObj = pl->Find( id )))
      return pObj;
  }
  return 0;
}
//=====================================================================
CDrawObject*
CGraphData::AddObject( CDrawObject* pObj )
{
  pView->m_bSublayerChanged |= pObj->IsSublayer();
  return GetLayerByClass( pObj->Class())->AddObject( pObj );
}
//=====================================================================
void
CGraphData::BeginGlobalSearch( CSpot const& spot )
{
  m_SearchSpot = spot;
  GetLastLayer( pView->SublDlgIsActive());
  if( GetPrevLayer())
    m_pLayer[m_LayerNum]->SetLast();
}
//=====================================================================
CDrawObject*
CGraphData::NextGlobalSearch()
{
  if( !m_pLayer )
    return 0;

  CDrawObject* pObj;
  for( pObj = 0 ; !( pObj = m_pLayer[m_LayerNum]->FindPrev( &m_SearchSpot )) && GetPrevLayer() ;
       m_pLayer[m_LayerNum]->SetLast());
  return pObj;
}
//=====================================================================
void
CGraphData::SelectByRect( CRect const& rect, CObjList& list )
{
  CWaitCursor wait;
  for( CLayer* pl = GetFirstLayer( pView->SublDlgIsActive()) ; pl = GetNextLayer() ; pl->SelectByRect( rect, list ));       
}
//=====================================================================
void
CGraphData::SelectBySpot( CSpot const& spot, CObjList& list )
{
  CWaitCursor wait;
  for( CLayer* pl = GetFirstLayer( pView->SublDlgIsActive()) ; pl = GetNextLayer() ; pl->SelectBySpot( spot, list ));       
}
//=====================================================================
CDrawObject*
CGraphData::SelectBySpot( CSpot const& spot, int bStrict )
{
  CWaitCursor wait;

 	int dist = MAX_INT;
	CDrawObject* pf = 0;
	for( CLayer* pl = GetLastLayer( pView->SublDlgIsActive()) ; ( pl = GetPrevLayer()) && !pf && dist ;
       pf = pl->SelectBySpot( spot, bStrict, &dist ));
  return pf;
}
//=====================================================================
CObjList&
CGraphData::SelectByClass( ClassType type, CObjList& list )
{
  CLayer* pl = GetLayerByClass( type );
  for( CDrawObject* pObj = pl->SetFirst() ; pObj = pl->FindNext() ; list.AddHead( pObj ));
  return list;
}
//=====================================================================
int
CGraphData::GetAngle( CSpot const& spot, int* pAngle )
{
  int dist = MAX_INT;
	for( int bSel = 2 ; --bSel >= 0; )
	{
    for( int ln = m_nLayers ; --ln >= 0 ; m_pLayer[ln]->GetAngle( spot, pAngle, &dist, bSel ));
    if( dist != MAX_INT )
			return 1;
	}
	return 0;
}
//=====================================================================
int
CGraphData::TestPointObject( CPoint tp )
{
	CLayer* pl = GetLastLayer( 0 );
  while(( pl = GetPrevLayer()) && !pl->TestPointObject( tp ));
  return pl != 0;
}
//=====================================================================
int
CLayer::TestPointObject( CPoint tp )
{
  for( CDrawObject* pObj = SetFirst() ; pObj = GetNext() ; )
	{
  	CPoint* pp = pObj->GetPoint();
	  if( !pp )
		  break;

		if( tp == *pp )
		  return 1;
	}
	return 0;
}
//=====================================================================
