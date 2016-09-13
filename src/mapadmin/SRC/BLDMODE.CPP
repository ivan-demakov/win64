#include "stdafx.h"

#include "mview.h"
#include "joke.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "bldmode.h"
#include "movemode.h"
#include "mdoc.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "mview.h"
#include "conduct.h"
#include "util.h"
#include "sltrmode.h"
#include "mercator.h"
//========================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
// Mode CBuildMode -- object building
//========================================================================
int    CBuildMode::m_bContextBld = 0;
ksi_obj CBuildMode::m_KsiContext = ksi_false;
CIdent  CBuildMode::m_ContextId;
CPoint  CBuildMode::m_LastLoc;
CRect   CBuildMode::m_ClipRect;
//=====================================================================
CBuildMode::CBuildMode( UINT nID, CIdent id, int nNode,
                        ksi_obj paramDef, ksi_obj imageDef ) :
  CAppMode( AIDG_BLDMODE, IDC_DRAW, AM_FESCMODE | AM_FUNIMODE | AM_FMONOPOLY, nID ),
  KSI_Dispatch( ksi_eval_str( "dispatch-event" )),
  m_paramDef( paramDef ),
  m_imageDef( imageDef ),
  m_ObjectIdent( id ),
  m_nNode( nNode ),
  m_SelStatus( 0 ),
  m_bDragging( 0 ),
  m_bCursorOn( 0 ),
  m_bCursorShown( 0 )
{
  ResetClipRect();
}
//========================================================================
void 
CBuildMode::DrawTextCursor()
{
  if( IsDragging())
    return;

  CPoint p0( m_CursorPos + TurnPoint( 0, m_CursorSize / 4, m_CursorAngle ));
  CPoint p1( m_CursorPos + TurnPoint( 0, -m_CursorSize, m_CursorAngle ));
  
  CViewDC dc( pView );
  if( !m_ClipRect.IsRectNull())
    dc.IntersectClipRect( &m_ClipRect );
  CPen pen( PS_SOLID, m_CursorSize / 24, RGB( 255, 0, 0 ));
  CPen* pOldPen = (CPen*)dc.SelectObject( &pen );
  dc.SetROP2( R2_NOTXORPEN );
  dc.MoveTo( p0 );
  dc.LineTo( p1 );
  dc.SelectObject( pOldPen );
  m_bCursorShown = !m_bCursorShown;
}
//========================================================================
void 
CBuildMode::SetTextCursor( int CursorSize, CPoint CursorPos, int CursorAngle )
{
  if( IsDragging())
    return;

  if( m_bCursorOn )
  {
    pView->KillTimer( IDM_TEXTCURSORTIMER );
    m_bCursorOn = 0;
  }

  if( m_bCursorShown )
    DrawTextCursor();

  if( CursorSize )
  {
    m_CursorPos   = CursorPos;
    m_CursorAngle = CursorAngle;
    m_CursorSize  = CursorSize;
    m_bCursorShown = m_bCursorOn = 1;
    DrawTextCursor();
    pView->SetTimer( IDM_TEXTCURSORTIMER, 500, 0 );
  }
}
//========================================================================
int
CBuildMode::OnTimer( UINT& nID )
{
  if( nID != IDM_TEXTCURSORTIMER )
    return AM_RPROPAGATE;

  DrawTextCursor();
  return AM_RDONE;
}
//=====================================================================
int 
CBuildMode::IsEdition() 
{ 
  return KSI_PLIST_P( m_imageDef ); 
}
//========================================================================
int
CBuildMode::UpdateContainer( ksi_obj val, ksi_obj ksi_dc )
{
  if( m_pOwner->m_CoeditStep )
    m_pOwner->ChangeCoedition( ksi_dc );

  if( KSI_PLIST_P( val ) && KSI_PLIST_P( KSI_CDR( val )))
  {
//KsiCall( "tk-box", ksi_eval_str( "building-env" ), 0 );
//KsiCall( "tk-box", val, 0 );
    CDrawObject* pObj = new CDrawObject( val, m_ObjectIdent,
      IsEdition() ? m_nNode : m_pOwner->m_Conductor.GetNode());
    m_pOwner->EndCoedition( pObj );
    SetContext();
    return AM_RREMOVE | AM_RDONE;
  }

  return KSI_FALSE_P( val ) ? AM_RREMOVE | AM_RDONE : AM_RDONE;
}
//========================================================================
int
CBuildMode::Init( CMapView* pOwner )
{
  CAppMode::Init( pOwner );
  m_nMode = 1;
  return 1;
}
//========================================================================
int
CBuildMode::Term( int bDestroy )
{
  if( !bDestroy )
  {
    m_pOwner->KillUndoItem();
    if( m_pOwner->m_pSTM )
      m_pOwner->m_pSTM->Reopen();
    m_pOwner->ShowSelectedNum();
  }
  return 1;
}
//========================================================================
void 
CBuildMode::SetContext( int cb, CDrawObject* pObj, CPoint loc )
{
  m_bContextBld = cb;
  m_KsiContext = pObj ? *pObj : ksi_false;
  m_ContextId  = pObj ? pObj->Ident() : CIdent();
  m_LastLoc = loc;
}
//========================================================================
int
CBuildMode::Open()
{
  ksi_obj env = ksi_current_env();
  ksi_defsym( "trace-line",       ksi_false, env );
  ksi_defsym( "&suspend-object?", ksi_false, env );
  ksi_defsym( "last-trace-point", ksi_false, env );

  m_pOwner->m_HostClass = m_ObjectIdent.Class();
  m_pOwner->m_LastHostList.RemoveAll();
  m_pOwner->m_LastEditList.RemoveAll();

  CDrawObject* pObj = IsEdition() ? m_pOwner->GetObjectPtr( m_ObjectIdent ) : 0;
  CViewDC dc( m_pOwner );
  ksi_obj ksi_dc = ksi_make_win_dc( m_pOwner->GetSafeHwnd(), dc.GetSafeHdc());

  if( KSI_TRUE_P( m_KsiContext ))
  {
    ksi_obj params =
      KsiCall( "make-building-context",
               KSI_MK_SINT( m_ContextId.Class()),
               KSI_MK_SINT( m_ContextId.Status()),
               m_KsiContext,
               ksi_int2num( m_LastLoc.x ),
               ksi_int2num( m_LastLoc.y ),
               KSI_MK_SINT( m_ObjectIdent.Class()), 0 );
    if( KSI_LIST_P( params ))
    {
      ksi_obj list_arr[] = { m_paramDef, params };
      m_paramDef = ksi_append_x( 2, list_arr );
    }
  }

  if( !m_pOwner->m_pUndoItem )
    m_pOwner->m_pUndoItem = new CUndoAddItem( m_pOwner );

  int t = m_ObjectIdent.Class();

  double sf = m_pOwner->CalcObjScaleFactor( t, m_pOwner->GetTrueScale(), 1 );
  KsiCall( "set-obj-scale-factor!", ksi_double2num( sf ), 0 );
	m_pOwner->SetKsiHiResolution();

  ksi_obj val =
    KsiCall( "build-object",
             ksi_dc,
             KSI_MK_SINT( t ),
             KSI_MK_SINT( m_ObjectIdent.Status()),
             m_paramDef,
             m_imageDef,
             KSI_MK_BOOL( m_ObjectIdent.IsNull()),
             0 );

  if( KSI_PAIR_P( val ))
  {
    if( IsEdition() && pObj )
      m_pOwner->BeginCoedition( pObj, this );
    m_pOwner->EndCoedition( new CDrawObject( 
      val, m_ObjectIdent, m_pOwner->m_Conductor.GetNode()));
    m_pOwner->RemoveMode( this );
  }
  else
  if( val == ksi_false )
  {
    ErrBox( "Oбъект не определен" );
    m_pOwner->RemoveMode( this );
  }

  return 0;
}
//========================================================================
void
CBuildMode::Draw( CDC* pDC )
{
  ksi_obj ksi_dc = ksi_make_win_dc( m_pOwner->GetSafeHwnd(), pDC->GetSafeHdc());
  if( m_pOwner->m_CoeditStep )
  {
    double sf = m_pOwner->CalcObjScaleFactor( m_ObjectIdent.Class(), m_pOwner->GetTrueScale(), 1 );
    KsiCall( "set-obj-scale-factor!", ksi_double2num( sf ), 0 );
    SetTextCursor( 0 );
    pDC->GetClipBox( &m_ClipRect );
    ksi_apply_2( KSI_Dispatch, ksi_dc,
                 ksi_make_win_paint( m_pOwner->GetSafeHwnd(), 0, 0, 0, 0 ));
    ResetClipRect();
  }
  m_pOwner->m_CoeditStep = 0;
  m_pOwner->ChangeCoedition( ksi_dc );
}
//========================================================================
int
CBuildMode::UpdateMouseEvent( UINT &flags, CPoint &loc, ksi_event_fun fun )
{
  m_pOwner->ResetCrtMsg();

  m_LastLoc = loc;
  CViewDC dc( m_pOwner );
  ksi_obj ksi_dc = ksi_make_win_dc( m_pOwner->GetSafeHwnd(), dc.GetSafeHdc());
  return UpdateContainer( ksi_apply_2( KSI_Dispatch, ksi_dc,
                                       fun( m_pOwner->GetSafeHwnd(), loc.x, loc.y, flags )), 
                          ksi_dc );
}
//========================================================================
int
CBuildMode::OnLButtonDown( UINT &flags, CPoint &loc )
{
  if(( flags & MK_SHIFT ) && !KSI_PLIST_P( m_imageDef ))
  {
    ksi_obj ksi_pl = ksi_false;

    CDrawObject* pOldObj = m_pOwner->GetSelectObject( 0 );
    CDrawObject* pNewObj = m_pOwner->GetDetectedObject();

    if( pOldObj )
      m_pOwner->SelectObject( pOldObj, 0 );

    if( pNewObj && pNewObj != pOldObj )
    {
      int np, tp;
      CPoint* pPnt = pNewObj->GetAnyPoints( &tp, &np );
      if( pPnt && np > 2 )
      {
        ksi_pl = KsiPoints( pPnt, np );
        if( tp == SH_POLYGON )
          ksi_pl = ksi_cons( KsiPoint( pPnt[np-1] ), ksi_pl );
        m_pOwner->SelectObject( pNewObj, 2 );
      }
    }

    ksi_defsym( "trace-line", ksi_pl, ksi_current_env());
    ksi_defsym( "last-trace-point", ksi_false, ksi_current_env());

    if( pNewObj )
    {
      m_pOwner->ShowBox( pNewObj->GetBoundBox());
      return AM_RDONE;
    }
  }

  return UpdateMouseEvent( flags, loc, ksi_make_win_left_down );
}
//=====================================================================
int
CBuildMode::OnChar( UINT& flags, UINT& key_code )
{
  m_pOwner->ResetCrtMsg();

  if( key_code == VK_ESCAPE )
  {
    m_pOwner->BreakeCoedition();
    return AM_RREMOVE | AM_RDONE;
  }

  CViewDC dc( m_pOwner );
  ksi_obj ksi_dc = ksi_make_win_dc( m_pOwner->GetSafeHwnd(), dc.GetSafeHdc());
  ksi_obj ksi_ch = ksi_make_win_char( m_pOwner->GetSafeHwnd(), key_code );
  ksi_obj ksi_res = ksi_apply_2( KSI_Dispatch, ksi_dc, ksi_ch );
  return UpdateContainer( ksi_res, ksi_dc );
}
//=====================================================================
// Object action mode class
//========================================================================
int
CObjActionMode::OnLButtonDown( UINT& flags, CPoint& loc )
{
  m_pOwner->ResetCrtMsg();
  
	m_ObjId = CIdent( 0, 0 );  
  
  if( flags & MK_SHIFT )
    return AM_RPROPAGATE;

  CDrawObject* pObj = m_pOwner->GetDetectedObject( 1, m_nID == IDM_OBJEDIT_MODE );
  if( !pObj )
    return AM_RPROPAGATE;
  
  m_FirstLoc = loc;

	if( !m_pOwner->IsVisibleInScale( pObj ))
    pObj = 0;

  CSpot spot( m_pOwner->GetSpot());
  if( pObj )
  {
    pObj->SetSAS();
    if( !pObj->Detected( spot, m_nID == IDM_OBJEDIT_MODE ))
    pObj = 0;
  }

  if( pObj )
    m_ObjId = pObj->Ident();
  
  return pObj ? AM_RDONE : AM_RPROPAGATE;
}
//========================================================================
int
CObjActionMode::OnRButtonDown( UINT &flags, CPoint& loc )
{
  return ( flags & MK_SHIFT ) ? AM_RPROPAGATE : AM_RREMOVE | AM_RDONE;
}
//========================================================================
int
CObjActionMode::OnLButtonUp( UINT &flags, CPoint& loc )
{
  if( flags & MK_SHIFT )
    return AM_RPROPAGATE;

  CSize ds( m_pOwner->LP2DP( m_FirstLoc - loc ));

  if( max( abs( ds.cx ), abs( ds.cy )) >= m_pOwner->m_Sensibility )
    return AM_RDONE;

  CDrawObject* pObj = m_pOwner->GetObjectPtr( m_ObjId );
  if( !pObj )
    return AM_RDONE;

  m_pOwner->m_Conductor.Create( pObj );

  switch( m_nID )
  {
    case IDM_OBJEDIT_MODE :
    {
      if( pObj->IsObjectTemp()|| pObj->IsExtObject())
      {
        Beep( 440, 400 );
        ErrBox( "Попытка редактирования временного объекта" );
        return AM_RDONE;
      }

      if(( pObj->IsSublayer() ?
            !m_pOwner->SublDlgIsActive() :
            !( m_pOwner->m_LocalLaw[pObj->Class()] & 1 )) ||
          !m_pOwner->IsObjectInLegalRegion( pObj, "Редактирование", 1 ))
        return AM_RDONE;

      if( !m_pOwner->IsHiResolution() && pObj->IsHiRes())
      {
        char m[1024];
        sprintf( m, "Объект '%s' построен в увеличенном масштабе.\n"
                    "Возможна потеря графической информации!\n\n"
                    "Продолжить редактирование?",
                 m_pOwner->m_pObjDef->GetName( pObj->Class()));

        if( AskBox( m ) != IDYES )
          return AM_RDONE;
      }

      int oldScale = m_pOwner->GetAltScale();
      ksi_obj ksi_img = *pObj;

      ksi_obj ksi_val = KsiCall( "unwrap",
                                 KSI_MK_SINT( pObj->Class()),
                                 KSI_MK_SINT( pObj->Status()),
																 ksi_img, 0 );

      if( KSI_FALSE_P( ksi_val ))
      {
        char m[1024];
        sprintf( m, "Объект '%s' имеет устаревшую структуру.\n"
                    "Редактирование объекта невозможно!",
                 m_pOwner->m_pObjDef->GetName( pObj->Class()));
        ErrBox( m );
        return AM_RDONE;
      }
      CSpot spot( m_pOwner->GetSpot( m_FirstLoc ));
      int nd = pObj->Detected( spot, 1 );
      ksi_img = KSI_LIST3( KSI_MK_SINT( nd - 1 ),
                           ksi_cons( KSI_MK_SINT( loc.x ), KSI_MK_SINT( loc.y )),
                           ksi_img );
      CBuildMode::SetContext();
      
      CBuildMode* pBM = new CBuildMode( IDM_CLASS_BEGIN + pObj->Class(),
                                        pObj->Ident(), 
                                        pObj->Node(),
                                        ksi_nil, ksi_img );
      pBM->SetSelStatus( pObj->SelectFlag());
			m_pOwner->QueryUserRights( UR_BLDMODE, pBM );
      break;
    }
    case IDM_CHANGEPARAM_MODE :
      if( pObj->IsObjectTemp()|| pObj->IsExtObject())
      {
        Beep( 440, 400 );
        ErrBox( "Попытка изменения временного объекта" );
      }
      else
			{
				m_pOwner->UpdateSetObjParam( pObj, 1 );
			}
      break;
    case IDM_QUERYPARAM_MODE :
      if( pObj->IsObjectTemp()|| pObj->IsExtObject())
      {
        Beep( 440, 400 );
        ErrBox( "Попытка запроса параметров временного объекта" );
      }
      else
        m_pOwner->m_Conductor.DeclareEvent( CE_OBJ_QUERY );
     break;
  }
  m_FirstLoc.x = m_FirstLoc.y = -1;
  return AM_RDONE;
}
//=====================================================================
// Context building mode
//========================================================================
int
CContextMode::OnRButtonDown( UINT &flags, CPoint &loc )
{
  if( flags & MK_SHIFT )
    return AM_RPROPAGATE;

	CDrawObject* pObj = 0;
  int tgts[CLASS_RANGE];
	int ntgt = 0;
  for( m_pOwner->BeginGlobalSearch( m_pOwner->GetSpot()) ; 
	     !ntgt && ( pObj = m_pOwner->NextGlobalSearch()) ; 
			 ntgt = GetTargets( pObj->Class(), tgts ));

	if( !pObj )
    return AM_RDONE;		

  if( !m_pOwner->IsObjectInLegalRegion( pObj, "Построение", 1 ))
    return AM_RDONE;		

  if( pObj->IsObjectTemp() || pObj->IsExtObject())
  {
    Beep( 440, 400 );
    ErrBox( "Попытка контекстного построения от временного объекта" );
    return AM_RDONE;
  }

  CBuildMode::SetContext();
  CBuildMode::SetContext( 1, pObj, m_pOwner->GetPosition());

  CMenu popup;
  popup.CreatePopupMenu();
	CMapDoc* pDoc = m_pOwner->GetDocument();
	pDoc->SetContextMenu( 1 );
  pDoc->MakePopupMenu( &popup, tgts, ntgt, IDM_CONTEXT_BEGIN );
  if( popup.GetMenuItemCount())
  {
    CPoint p;
    GetCursorPos( &p );
    popup.TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, p.x, p.y, m_pOwner );
    m_pOwner->GetDocument()->DeletePopupMenu( &popup, 0 );
    popup.DestroyMenu();
  }

  return AM_RDONE;
}
//========================================================================
