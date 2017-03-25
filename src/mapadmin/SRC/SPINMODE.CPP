#include "stdafx.h"

#include "dwin.h"
#include "mview.h"
#include "idg.h"
#include "ids.h"
#include "util.h"
#include "mathutil.h"
#include "spinmode.h"
#include "drawobj.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CSpinMode::CSpinMode( CPoint startPoint ) :
  CAppMode( AIDG_MOVMODE, IDC_TURN, AM_FRBUP | AM_FMOUSEMOVE,
            IDM_MOVE_MODE ),
  m_Center( startPoint ),
  m_Angle( 0 )
{}
//=====================================================================
void
CSpinMode::CopySpec()
{
  CObjList& Sl = m_pOwner->m_SelectList;
  for( POSITION pos = Sl.GetTailPosition() ; pos ; Sl.GetPrev( pos ))
  {
    CDrawObject* pObj = Sl.GetAt( pos );
    if( pObj->IsSpecSelected())
    {
      m_TmpList.AddHead( pObj );
      Sl.RemoveAt( pos );
    }
  }
}
//=====================================================================
void
CSpinMode::SaveSpec()
{
  CObjList& Sl = m_pOwner->m_SelectList;
  for( POSITION pos = m_TmpList.GetTailPosition() ; pos ;
       Sl.AddHead( m_TmpList.GetPrev( pos )));
}
//=====================================================================
using namespace Trig;

int
CSpinMode::Open()
{
  for( POSITION pos = m_pOwner->m_SelectList.GetHeadPosition() ; pos ; )
    if( !m_pOwner->IsObjectInLegalRegion( m_pOwner->m_SelectList.GetNext( pos ), "Поворот", 1 ))
    {
      m_pOwner->RemoveMode( this );
      return 0;
    }

  CopySpec();
  m_pOwner->ShowSelected( 0 );
  m_Box = m_pOwner->CalcSelectBox();
  CPoint C( m_Box.CenterPoint());
  m_StartAngle = atan2( m_Center - C );
  m_Center = C;
  ShowAngle();
  m_nMode = 1;
  m_pBlinking = m_pOwner->StartBlinking( 0 );
  if( m_pBlinking )
    m_pBlinking = m_pBlinking->CopyObject();
  OnSetCursor();
  return 1;
}
//=====================================================================
void
CSpinMode::ShowAngle()
{
  int a = abs( m_Angle );
  char s[40];
  sprintf_s( s, " Поворот на %c%d.%1d град.", m_Angle >= 0 ? ' ' : '-', a/10, a%10 );
  ReportHint( s );
}
//=====================================================================
void
CSpinMode::Draw( CDC* pDC )
{
  CBox sBox( m_Box, m_Center, m_Angle );
  CRect full, box;
  m_pOwner->GetClientRect( &full );
  pDC->DPtoLP( &full );
  box.IntersectRect( &full, &sBox );
  CRect rec( box );
  pDC->LPtoDP( rec );
  rec.InflateRect( 1, 1 );
  CSize bmps = rec.Size();
  CBitmap PictBmp;
  PictBmp.CreateCompatibleBitmap( pDC, bmps.cx, bmps.cy );
  CMemDC memDC( pDC );
  memDC.SelectObject( &PictBmp );
  pDC->DPtoLP( &bmps );
  memDC.PatBlt( 0, 0, bmps.cx * 2, bmps.cy * 2, WHITENESS );
  float cs = cos( m_Angle ), sn = -sin( m_Angle );
  float dx = m_Center.x * ( 1.0 - cs ) + m_Center.y * sn - box.left;
  float dy = m_Center.y * ( 1.0 - cs ) - m_Center.x * sn - box.top;
  XFORM tf = { cs, sn, -sn, cs, dx, dy };
  SetGraphicsMode( memDC.GetSafeHdc(), GM_ADVANCED );
  SetWorldTransform( memDC.GetSafeHdc(), &tf );
  m_pOwner->DrawSelect( &memDC );
  SetIdenticalTransform( &memDC );
  pDC->BitBlt( box.left, box.top, bmps.cx, bmps.cy, &memDC, 0, 0, 0x00990066 );
}
//=====================================================================
int
CSpinMode::OnRButtonUp( UINT& flags, CPoint& loc )
{
  m_nMode = 0;
  if( m_pOwner->SpinSelectOn( m_Center, m_Angle ))
    m_pOwner->m_Undo.AddItem( new CUndoSpinItem( m_pOwner, m_Center, m_Angle ), m_pOwner );
  m_pOwner->ShowSelected( 1, 1, 1 );
  m_pOwner->UpdateWindow();
  if( m_pBlinking )
    for( POSITION pos = m_pOwner->m_SelectList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pObj = m_pOwner->m_SelectList.GetNext( pos );
      if( pObj->Ident() != m_pBlinking->Ident())
        continue;
      m_pOwner->StartBlinking( pObj );
      delete m_pBlinking;
    }
  SaveSpec();
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
using namespace Trig;

int
CSpinMode::OnMouseMove( UINT& flags, CPoint& loc )
{
  m_Angle = atan2( loc - m_Center ) - m_StartAngle;
  ShowAngle();
  return AM_RPROPAGATE;
}
//=====================================================================
int
CSpinMode::OnChar( UINT& flags, UINT& key_code )
{
  if( key_code != VK_ESCAPE )
    return AM_RPROPAGATE;

  m_pOwner->ShowSelected( 1, 0 );
  SaveSpec();
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
