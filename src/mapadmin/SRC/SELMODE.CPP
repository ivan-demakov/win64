#include "stdafx.h"

#include "lawdlg.h"
#include "dlg0.h"
#include "mview.h"
#include "mdoc.h"
#include "dwin.h"
#include "miscmode.h"
#include "selmode.h"
#include "movemode.h"
#include "drawobj.h"
#include "spinmode.h"
#include "bldmode.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "mathutil.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "util.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
//==== CSelectMode ====================================================
int
CSelectMode::OnLButtonDblClk( UINT &flags, CPoint &loc )
{
  if(( flags & MK_SHIFT ) && !m_pOwner->WaitingEdit())
  {
    CDrawObject* pObj = m_pOwner->GetSelectObject( 0 );
    if( pObj && !pObj->IsExtObject())
    {
      if( pObj->IsObjectTemp())
      {
        Beep( 440, 400 );
        ErrBox( "Попытка изменения временного объекта" );
      }
      else
        m_pOwner->UpdateSetObjParam( pObj, 1 );
    }
    return AM_RDONE;
  }
  return AM_RPROPAGATE;
}
//=====================================================================
int
CSelectMode::TestEdition( int bLeft, int bDel )
{
  CWaitCursor wc;
  ReportHint( "Проверка корректности операции..." );
  int n = 0;

  int bCpy = bLeft && CMoveMode::CopyMode();

  if( bLeft && bCpy && !m_pOwner->TestSelected())
    return 0;

  int t = 0;

  for( POSITION pos = m_pOwner->m_SelectList.GetHeadPosition() ; pos && !n ; )
  {
    CDrawObject* pObj = m_pOwner->m_SelectList.GetNext( pos );
    n = pObj->IsObjectTemp()                                 ? 4 :
        pObj->IsSpecSelected()                               ? 3 :
        !pObj->IsSublayer() &&
        !( m_pOwner->m_LocalLaw[pObj->Class()] & 1 )         ? 1 :
         bCpy && ( t = m_pOwner->IsCopyingAllowable())       ? 2 :
        !bCpy && ( t = m_pOwner->IsEditionAllowable( bDel )) ? 2 : 0;
  }

  if( n )
  {
    int bs = m_pOwner->m_SelectList.GetCount() == 1;
    char* pf = bDel  ? "удалить" :
               bCpy  ? "скопировать" :
               bLeft ? "переместить" :
                       "повернуть";
    Beep( 440, 400 );
    ErrBox(
      Compose( "Извините!\nВы не можете ", pf,
               bs ? " выделенный объект" : " выделенные объекты",
               ",\nтак как ",
               n == 1 ? Compose( "для ", bs ? "него" : "одного из них", " не включено редактирование", 0 ) :
               n == 2 ? "при этом нарушается связность объекта\n" :
               n == 3 ? "имеется тематическое выделение" :
               n == 4 ? ( bs ? "объект временный" : "в их составе имеется временный объект" ) :
                        "причина не найдена",
               t ? Compose( "'", m_pOwner->m_pObjDef->GetName( t ), "'", 0 ) : "",
               "!", 0 ));
  }

  m_pOwner->ShowSelectedNum();
  return !n;
}
//=====================================================================
int
CSelectMode::OnButtonDown( UINT &flags, CPoint& loc, int bLeft )
{
  if( flags & MK_SHIFT )
  {
    SetLT( m_pOwner->GetPosition());
    m_nMode = flags;

    m_pOwner->KillUndoItem();
    m_pOwner->m_pUndoItem = new CUndoSelectItem( m_pOwner );

    m_nCursor = !( flags & MK_CONTROL ) ? IDC_GETSELECT :
                bLeft                   ? IDC_ADDSELECT :
                                          IDC_SUBSELECT;
    OnSetCursor();
    return AM_RDONE;
  }

  static UINT ma[] = { IDM_OBJEDIT_MODE, IDM_EDITTRACE_MODE, IDM_CONNECT_MODE, 0 };

  if( !m_pOwner->m_SelectList.IsEmpty())
  {
    CDrawObject* pObj = m_pOwner->GetDetectedObject();

    if( !pObj || pObj->IsSpecSelected() || pObj->IsExtObject())
      return AM_RPROPAGATE;

    if( !m_pOwner->GetMode( ma ) && pObj && pObj->IsSelected())
    {
      if( TestEdition( bLeft ))
        if( bLeft )
          m_pOwner->QueryUserRights( UR_MOVEMODE, new CMoveMode( loc ));
        else
          m_pOwner->QueryUserRights( UR_SPINMODE, new CSpinMode( m_pOwner->GetPosition()));
      return AM_RDONE;
    }
  }

  return AM_RPROPAGATE;
}
//=====================================================================
int
CSelectMode::OnButtonUp( UINT &flags, CPoint &loc, int bLeft )
{
  if( m_nMode != ( flags | ( bLeft ? MK_LBUTTON : MK_RBUTTON )))
  {
    m_pOwner->KillUndoItem();
    return AM_RPROPAGATE;
  }

  SetRB( m_pOwner->GetPosition());

   if( !( flags & MK_CONTROL ))
  {
    if( m_pOwner->m_pUndoItem )
      m_pOwner->m_pUndoItem->SetList( &m_pOwner->m_SelectList, 0 );
    m_pOwner->ClearSelection( 0, 0 );
  }

  if( bLeft )
    m_pOwner->AddSelectRect( this );
  else
    m_pOwner->SubSelectRect( this );

  if( m_pOwner->m_pUndoItem )
          m_pOwner->m_Undo.AddItem( m_pOwner->m_pUndoItem, m_pOwner );

  return AM_RDONE;
}
//=====================================================================
int
CSelectMode::OnMouseMove( UINT &flags, CPoint &loc )
{
  if( m_nMode == flags )
    SetRB( m_pOwner->GetPosition());
  else
    m_nMode = 0;
  return AM_RPROPAGATE;
}
//=====================================================================
int
CSelectMode::OnChar( UINT& flags, UINT& key_code )
{
  if( m_pOwner->GetMode( IDM_MOVE_MODE ) ||
            m_pOwner->GetMode( IDM_SHORTTRACE_MODE ) ||
                        m_pOwner->GetMode( IDM_FIND_DIST_MODE ))
    return AM_RPROPAGATE;

  if( key_code == VK_ESCAPE )
  {
    if( !m_pOwner->GetActiveMode( IDM_DRAG_MODE ) &&
        !m_pOwner->StartBlinking( 0 ))
    {
      m_pOwner->ClearSelection();
      m_pOwner->HideObject( CIdent( 0, 0, 0 ), 1 );
    }
    return AM_RPROPAGATE;
  }

  CObjList* pSL = &m_pOwner->m_SelectList;

  switch( key_code )
  {
    case 0x14 : // VK_HOME
    case 0x15 : // VK_END
    {
      CDrawObject* pObj = m_pOwner->GetSelectObject( 1 );
      if( pObj )
      {
        int n;
        CPoint* pPnt = pObj->GetPolyline( &n );
        if( !pPnt )
          Beep( 440 , 400 );
        else
          m_pOwner->SetPosition( pPnt[key_code==0x14?0:n-1] );
      }
      return AM_RDONE;
    }

    case 0x12 : // VK_LEFT
    case 0x13 : // VK_RIGHT
    {
      if( !pSL->GetCount())
        return AM_RPROPAGATE;

      if( TestEdition( 0 ))
        m_pOwner->QueryUserRights( ( key_code & 1 ) ? UR_SPINRIGHT : UR_SPINLEFT, 0 );

      return AM_RDONE;
    }

    case 0x11 : // VK_DELETE
      {
        int n = TestEdition( 0, 1 ) ? pSL->GetCount() : 0;

        if( n == 1 )
        {
          CDrawObject* pObj = pSL->GetHead();
          if( !pObj->IsSublayer() &&
              !( m_pOwner->m_LocalLaw[pObj->Class()] & 1 ) ||
              !m_pOwner->IsObjectInLegalRegion( pObj, "Удаление", 1 ))
            return AM_RDONE;

          m_pOwner->GetConductor()->Create( pObj );
          char m[256];
          sprintf_s( m, "Удалить объект '%s'?",
                     m_pOwner->m_pObjDef->GetName( pObj->Class()));
          if( AskBox( m ) == IDYES )
          {
            if( GetProjectSLEnable())
            {
              if( pObj->Table())
                m_pOwner->GetConductor()->DeclareEvent( CE_OBJ_DELETE );
              else
                m_pOwner->QueryUserRights( UR_DELETE, 0 );
              n = 0;
            }
            else
            if( pObj->Table() && m_pOwner->GetConductor()->DeclareEvent( CE_OBJ_DELETE ))
              n = 0;
          }
          else
            n = 0;
        }

        int nTabs = 0;

        if( n > 1 )
        {
          for( POSITION pos = pSL->GetHeadPosition() ; pos && nTabs < 2 ;
               nTabs += !pSL->GetNext( pos )->TabIdent().IsNull());
          if( !nTabs  )
          {
            for( POSITION pos = pSL->GetHeadPosition() ; pos ; )
              if( !m_pOwner->IsObjectInLegalRegion( pSL->GetNext( pos ), "Удаление", 1 ))
                return AM_RDONE;

            CString s;
            int k = n/10%10 == 1 ? 0 : n%10;
            char* e = k == 1 ? "" : k > 0 && k < 5 ? "а" : "ов";
            s.Format( "Удалить %d объект%s?", n, e );
            if( AskBox( s ) != IDYES )
              n = 0;
          }
          else
          {
            ErrBox( "Групповое удаление запрещено" );
            n = 0;
          }
        }

        if( n )
        {
          m_pOwner->m_Undo.AddItem( new CUndoAddItem( m_pOwner, 0, &m_pOwner->m_SelectList ), m_pOwner );
          m_pOwner->ClearSelection( 1, 0 );
        }
        return AM_RDONE;
      }

    default:
      return AM_RPROPAGATE;
  }
}
//=====================================================================
void
CSelectMode::Draw( CDC* pDC )
{
  CRectMode::Draw( pDC );
}
//=====================================================================
//=====================================================================
void
CChangeSelectMode::Draw( CDC* pDC )
{
  CRectMode::Draw( pDC );
}
//=====================================================================
int
CChangeSelectMode::OnRButtonDown( UINT &flags, CPoint& )
{
  if( m_pOwner->m_SelectList.IsEmpty() || m_pOwner->m_pSTM )
    return AM_RREMOVE | AM_RDONE;

  m_pOwner->ClearSelection();
  return AM_RDONE;
}
//=====================================================================
int
CChangeSelectMode::OnLButtonDown( UINT &flags, CPoint& )
{
  m_nMode = 1;
  SetLT( m_pOwner->GetPosition());
  ASSERT( !m_pOwner->m_pUndoItem );
  m_pOwner->m_pUndoItem = new CUndoSelectItem( m_pOwner );
  return AM_RDONE;
}
//=====================================================================
int
CChangeSelectMode::OnMouseMove(UINT &, CPoint &)
{
  if( m_nMode )
    SetRB( m_pOwner->GetPosition());
  return AM_RDONE;
}
//=====================================================================
//=====================================================================
int
CGetSelectMode::OnLButtonDblClk( UINT &flags, CPoint &loc )
{
  CDrawObject* pObj = m_pOwner->GetSelectObject( 0 );
  if( pObj )
    m_pOwner->UpdateSetObjParam( pObj, 1 );
  return AM_RDONE;
}
//=====================================================================
//=====================================================================
int
CChangeSelectMode::OnLButtonUp( UINT& flags, CPoint& )
{
  if( !m_nMode )
    return AM_RPROPAGATE;

  SetRB( m_pOwner->GetPosition());
  switch( m_nID )
  {
    case IDM_GETSELECT_MODE :
      m_pOwner->m_pUndoItem->SetList( &m_pOwner->m_SelectList, 0 );
      m_pOwner->ClearSelection( 0, 0 );
    case IDM_ADDSELECT_MODE : m_pOwner->AddSelectRect( this ); break;
    case IDM_SUBSELECT_MODE : m_pOwner->SubSelectRect( this ); break;
  }
  m_pOwner->m_Undo.AddItem( m_pOwner->m_pUndoItem, m_pOwner );
  m_nMode = 0;
  return AM_RDONE;;
}//=====================================================================
//=====================================================================
int
CChangeClassMode::Init( CMapView* pOwner )
{
  m_pOwner = pOwner;

  CSelectClassDlg dlg( "Выбор класса", pOwner->m_pObjDef, 1 );
  if( DoModalDlg( &dlg, CE_PAR_DLG ) != IDOK )
    return 0;

  m_nClass = dlg.GetClass();
  return m_nMode = 1;
}
//=====================================================================
int
CChangeClassMode::OnLButtonDown( UINT &flags, CPoint &loc )
{
  if( !( flags & MK_SHIFT ))
    return AM_RPROPAGATE;

  CDrawObject* pObj = m_pOwner->GetDetectedObject();
  if( !pObj )
    return AM_RPROPAGATE;

  if( pObj->IsObjectTemp() || pObj->IsExtObject())
  {
    Beep( 440, 400 );
    ErrBox( "Попытка изменения временного объекта" );
    return AM_RDONE;
  }

  if( pObj->Class() == m_nClass )
  {
    Beep( 440, 400 );
    return AM_RDONE;
  }

  ksi_obj ksi_img = *pObj;
  ksi_obj ksi_par = KsiCall( "unwrap",
                             KSI_MK_SINT( pObj->Class()),
                             KSI_MK_SINT( pObj->Status()),
                             ksi_img, 0 );

  int bSaveOld = 1;

  switch( AskBox( "Сохранить прототип?", 1 ))
  {
    case IDCANCEL: return AM_RDONE;
    case IDNO:     bSaveOld = 0;
  }

  if( !bSaveOld )
  {
    m_pOwner->m_Conductor.Create( pObj );
    if( m_pOwner->m_Conductor.DeclareEvent( CE_OBJ_DELETE ))
      bSaveOld = 1;
  }

  if( !bSaveOld )
    m_pOwner->m_pDelObj = pObj;

  int ent = m_pOwner->m_pObjDef->GetTable( m_nClass ) ? 0 : m_pOwner->GetUniId();
  int sts = m_pOwner->IsHiResolution() ? 0x80 : 0;
  CIdent id( m_nClass, sts, 0, ent );

  m_pOwner->AddMode(
    new CBuildMode( m_nClass, id, m_pOwner->m_Conductor.GetNode(),
                    ksi_par, ksi_nil ));

  return AM_RDONE;;
}
//=====================================================================
int
CChangeClassMode::OnRButtonDown( UINT &flags, CPoint &loc )
{
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
//=====================================================================
int
CMagnitMode::OnButtonDown( UINT &flags, CPoint &loc )
{
  if( m_pPoint )
    m_pOwner->m_Position = loc = *m_pPoint;

  return AM_RPROPAGATE;
}
//=====================================================================
void
CMagnitMode::Select( CPoint pt )
{
  int d = dist( pt, m_CurPos );
  if( m_CurDist <= d )
    return;

  m_CurDist = d;
  m_CurPoint = pt;
  m_pPoint = &m_CurPoint;
}
//=====================================================================
void
CMagnitMode::Select( CPoint* pPt, int n )
{
  int i, d = abs( DistPointToPolyline( m_CurPos, pPt, n ));
  for( i = m_nLine ; --i >= 0 && dl[i] > d ; dl[i+1] = dl[i], svl[i+1] = svl[i], snl[i+1] = snl[i] )
  dl[i+1] = d;
  svl[i+1] = pPt;
  snl[i+1] = n;
  m_nLine = min( m_nLine + 1, 2 );
}
//=====================================================================
void
CMagnitMode::Select( CRect const& r )
{
  CPoint c( r.CenterPoint());
  int R = abs( r.Width() / 2 );
  int d = abs( dist( c, m_CurPos ) - R );

  if( m_CurDist <= d )
    return;
  
  int i;
  for( i = m_nRect ; --i >= 0 && dr[i] > d ; dr[i+1] = dr[i], svr[i+1] = svr[i] );
  dr[i+1] = d;
  svr[i+1] = r;
  m_nRect = min( m_nRect + 1, 3 );
}
//=====================================================================
static int
PtInRing( CPoint pt, CRect const& r )
{
  CPoint c( r.CenterPoint());
  int R = abs( r.Width() / 2 );
  return dist( c, pt ) < R;
}
//=====================================================================
int
CMagnitMode::OnMouseMove( UINT &flags, CPoint &loc )
{
  m_pPoint = 0;
  CSize d( m_pOwner->m_Sensibility + 1, 0 );
  d = m_pOwner->DP2LP( d );
  m_CurDist = d.cx;
  m_CurPos = loc;
  CObjList lst;
  if( m_nSelectMode == 2 )
    m_pOwner->m_Sensibility *= 2;
  CSpot spot( m_pOwner->GetSpot());
  if( m_nSelectMode == 2 )
    m_pOwner->m_Sensibility /= 2;
  m_pOwner->m_ClipData.SelectBySpot( spot, lst );

  m_nRect = 0;
  m_nLine = 0;

  while( !lst.IsEmpty())
  {
    CDrawObject* pObj = lst.RemoveHead();
    
		if( m_nSelectMode & 0x1 )
    {
      int np;
      CPoint* pPnt = pObj->GetAnyPoints( 0, &np );
      if( pPnt )
      	while( --np >= 0 )
          Select( *pPnt++ );
    }

    if( m_nSelectMode & 0x2 )
    {
      int np;
      CPoint* pPnt;
			( pPnt = pObj->GetPolyline( &np )) ||
			( pPnt = pObj->GetPolygon( &np ));
			if( pPnt && np >= 2 )
        Select( pPnt, np );
    }
    
    if( m_nSelectMode & 0xc )
    {
      CRect r;
      if( pObj->GetRing( spot, &r ))
        Select( r );
    }
  }

  if(( m_nSelectMode & 0x2 ) && m_nLine == 2 )
    for( ; --snl[0] > 0 ; ++svl[0] )
    {
      CPoint* p1 = svl[1];
      for( int k = snl[1] ; --k > 0 ; ++p1 )
      {
        CPoint cp;
        if( CrossPoint( svl[0], p1, &cp, 1 ))
        {
          int td = dist( m_CurPos, cp );
          if( td < m_CurDist )
          {
            m_CurDist = td;
            m_CurPoint = cp;
            m_pPoint = &m_CurPoint;
          }
        }
      }
    }
  
  if(( m_nSelectMode & 0x4 ) && m_nRect >= 2 )
  {
    CPoint tp[2];
    int np = FindCrossPoints( svr[0], svr[1], tp );
    while( --np >= 0 )
    {
      int td = dist( m_CurPos, tp[np] );
      if( td < m_CurDist )
      {
        m_CurDist = td;
        m_CurPoint = tp[np];
        m_pPoint = &m_CurPoint;
      }
    }
  }

  if(( m_nSelectMode & 0x8 ) && m_nRect == 3 )
  {
    CPoint tp[6];
    int np = FindCrossPoints( svr[0], svr[1], &tp[0] ) +
             FindCrossPoints( svr[0], svr[2], &tp[2] ) +
             FindCrossPoints( svr[1], svr[2], &tp[4] );
    if( np == 6 )
    {
      for( int m = 0 ; m < 8 ; ++m )
      {
        double sx = 0, sy = 0;
        int i, k;
        for( i = 1, k = 0 ; k < 6 ; i <<= 1, k += 2  )
        {
          int j = k + (( m & i ) ? 1 : 0 );
          sx += tp[j].x;
          sy += tp[j].y;
        }
        CPoint pp( sx / 3., sy / 3. );
        int d = 0;
        for( i = 1, k = 0 ; k < 6 ; i <<= 1, k += 2  )
        {
          int j = k + (( m & i ) ? 1 : 0 );
          d = max( d, dist( pp, tp[j] ));
        }
        if( d > m_CurDist )
          continue;

        int td = dist( m_CurPos, pp );
        if( td < m_CurDist )
        {
          m_CurDist = td;
          m_CurPoint = pp;
          m_pPoint = &m_CurPoint;
        }
      }
    }
  }

  if( m_pPoint )
    m_pOwner->m_Position = loc = *m_pPoint;

  return AM_RPROPAGATE;
}
//=====================================================================
void
CMagnitMode::Draw( CDC* pDC )
{
  if( !m_pPoint )
    return;

  CPoint c( m_CurPoint );
  pDC->LPtoDP( &c );
  CRect R( c.x - 8, c.y - 8, c.x + 8, c.y + 8 );

  if( m_nMode == 1 )
  {
    int sdc = SetTM( pDC );
    CBrush br1( RGB( 0, -1, 0 ));
    CBrush br2( RGB( -1, 0, 0 ));
    pDC->SelectObject( &br1 );
    pDC->SelectStockObject( BLACK_PEN );
    pDC->Ellipse( &R );
    pDC->SelectObject( &br2 );
    pDC->Ellipse( c.x - 4, c.y - 4, c.x + 4, c.y + 4 );
    pDC->RestoreDC( sdc );
    m_nMode = 2;
  }
  else
  {
    CRect box( R );
    box.InflateRect( 1, 1 );
    m_pOwner->InvalidateRect( &box, 0 );
    m_nMode = 1;
  }
}
//=====================================================================
