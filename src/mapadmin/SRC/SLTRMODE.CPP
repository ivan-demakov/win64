#include "stdafx.h"

#include "mdoc.h"
#include "dwin.h"
#include "mview.h"
#include "joke.h"
#include "ksi_type.h"
#include "instrdlg.h"
#include "sltrmode.h"
#include "drawobj.h"
#include "conduct.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
  #include "report.h"
#endif
#include "mathutil.h"
#include "ksiutil.h"
#include "util.h"
#include "t.h"
#include "TGstream.h"
//=====================================================================
CString    CSelectTraceMode::m_TraceName;
CString    CSelectTraceMode::m_TraceDBName;
CTabIdent  CSelectTraceMode::m_TraceIdent;
int        CSelectTraceMode::m_SelectType = 1;
int        CSelectTraceMode::m_TmpTraceSize = 0;
long       CSelectTraceMode::m_bNoInstructions = 0;
UINT       CSelectTraceMode::m_TraceType = 1;
TraceArray CSelectTraceMode::m_Trace;
//=====================================================================
void
CSelectTraceMode::Add( CTabIdent id, CTabIdent cobj, char const* pTypName, char const* pObjName, char const* pObjAdr, int selType )
{
  m_Trace.Add( new TraceObject( id, cobj, pTypName, pObjName, pObjAdr, selType ));
}
//=====================================================================
void
CSelectTraceMode::Rem( int ndx )
{
  delete m_Trace[ndx];
  m_Trace.RemoveAt( ndx );
}
//=====================================================================
void
CSelectTraceMode::Rem( CTabIdent id )
{
  Rem( InTrace( id ));
}
//=====================================================================
void
CSelectTraceMode::Purge()
{
  for( int i = m_Trace.GetSize() ; --i >= 0 ; delete m_Trace[i] );
  m_Trace.RemoveAll();
  m_TraceName = "";
  pView->RemoveMode( pView->m_pSTM );
}
//=====================================================================
int
CSelectTraceMode::IsTraceClass( int t )
{
  int bFound = 0;
  for( int i = m_TraceObjClass.GetSize() ; !bFound && --i >= 0 ; )
  {
    int pc = m_TraceObjClass[i];
    bFound = pc && ( t == pc || m_pOwner->m_pObjDef->IsDerivedFrom( t, pc ));
  }
  return bFound;
}
//=====================================================================
int
CSelectTraceMode::InTrace( CTabIdent id )
{
  int i = m_Trace.GetSize();
  while( --i >= 0 && m_Trace[i]->m_ObjId != id );
  return i;
}
//=====================================================================
void
CSelectTraceMode::CalcLen()
{
  if( m_nMode == STM_CALC )
  {
    m_pOwner->GetConductor()->DeclareEvent( CE_SCHEEM_LEN );
    m_pOwner->ShowSelectedNum();
  }
}
//=====================================================================
static CPoint
GetCharPoint( CDrawObject* pObj )
{
  int sType, nPnts;
  CPoint* pPnts = pObj->GetAnyPoints( &sType, &nPnts );
  if( sType == SH_NOTHING || !nPnts )
    return pObj->GetBoundBox().CenterPoint();

  CPoint p( 0, 0 );
  for( int i = nPnts ; --i >= 0 ; p += pPnts[i] );
  p.x /= nPnts;
  p.y /= nPnts;
  return p;
}
//=====================================================================
void
CSelectTraceMode::InsertSelect( CDrawObject* pObj, int selType )
{
  pObj->SetSelectFlag( selType );
  m_pOwner->ShowObjectBox( pObj );
  CObjList& Sl = m_pOwner->m_SelectList;
  POSITION pos = 0;
  if( !Sl.IsEmpty())
  {
    pos = Sl.GetTailPosition();
    CPoint p0( GetCharPoint( Sl.GetPrev( pos )));
    int d = dist( p0, GetCharPoint( pObj));
    for( ; pos && dist( p0, GetCharPoint( Sl.GetAt( pos ))) < d ; Sl.GetPrev( pos ));
  }
  if( pos )
    Sl.InsertAfter( pos, pObj );
  else
    Sl.AddHead( pObj );
}
//=====================================================================
int
CSelectTraceMode::SwapObj( CDrawObject* pObj )
{
  ASSERT( pObj );
  int bAdd = !pObj->IsSelected();

  if( m_nMode == STM_CALC )
  {
    m_pOwner->GetConductor()->DeclareEvent( CE_SCHEEM_LEN );
    m_pOwner->ShowSelectedNum();
  }

  if( bAdd )
  {
    TraceObject* pTraceObj =
      new TraceObject( pObj->TabIdent(), m_SelectType,
                       GetCoordList( pObj ), GetCharPoint( pObj ));
    m_Trace.Add( pTraceObj );
    InsertSelect( pObj, m_SelectType );
  }
  else
  {
    Rem( pObj->TabIdent());
    m_pOwner->SelectObject( pObj, 0 );
  }
  m_TmpTraceSize = m_Trace.GetSize();
  m_pOwner->GetConductor()->Create( pObj->TabIdent());
  m_pOwner->GetConductor()->DeclareEvent(
      m_nMode == STM_CALC ? ( bAdd ? CE_SCHEEM_LEN_ADD : CE_SCHEEM_LEN_SUB ) :
                            ( bAdd ? CE_TRACE_ADD : CE_TRACE_SUB ));
  return bAdd;
}
//========================================================================
CTabIdent
CSelectTraceMode::GetTraceData( int i )
{
  return i >= 0 && i < m_Trace.GetSize() ?
    m_Trace[i]->m_ObjId : CTabIdent( 0, 0 );
}
//=====================================================================
CString
CSelectTraceMode::GetTraceCoord( int i )
{
  return i >= 0 && i < m_Trace.GetSize() ?
    m_Trace[i]->m_ObjShape : "";
}
//=====================================================================
CString
CSelectTraceMode::GetCoordList( CTabIdent id )
{
  CSmartPtr ptr;
  CDrawObject* pObj = pView->GetObjectPtr( id );
  if( !pObj )
    ptr = pObj = pView->ExtractMapObj( id );

  return pObj ? GetCoordList( pObj ) : "";
}
//=====================================================================
CString
CSelectTraceMode::GetCoordList( CDrawObject* pObj )
{
  int n;
  CPoint const* pPnt = pObj->GetAnyPoints( 0, &n );
  CPoint c( pObj->GetBoundBox().CenterPoint());
  if( !pPnt )
  {
    pPnt = &c;
    n = 1;
  }

  CString s, q;
  s += "(";
  CPoint b( 0, 0 );
  while( --n >= 0 && s.GetLength() < 4000 )
  {
    CPoint p( *pPnt++ );
    CSize t( p - b );
    b = p;
    q.Format( "(%d . %d)", t.cx, t.cy );
    s += q;
  }
  s += ")";

  return s;
}
//=====================================================================
CString
CSelectTraceMode::GetMifCoordList( CDrawObject* pObj )
{
  int i, t, n;
  CPoint const* pPnt = pObj->GetAnyPoints( &t, &n );
  CPoint c( pObj->GetBoundBox().CenterPoint());
  if( !pPnt )
  {
    pPnt = &c;
    n = 1;
    t = SH_POINT;
  }

  CString s, q;
  if( t == SH_POINT )
    s.Format( "Point %d %d\n", pPnt->x, pPnt->y );
  else
  {
    int m = n + ( t != SH_POLYLINE );

    if( t == SH_POLYGON )
      s.Format( "Region 1\n%d\n", m );
    else
      s.Format( "Pline %d\n", m );

    for( i = 0 ; i < m ; ++i )
    {
       CPoint p( pPnt[i%n] );
       q.Format( "%d %d\n", p.x, p.y );
       s += q;
    }
  }
  return s;
}
//=====================================================================
void
CSelectTraceMode::Reopen()
{
  m_pOwner->RemoveMode( m_pOwner->GetMode( IDM_OBJEDIT_MODE ));

  for( int i = m_Trace.GetSize() ; --i >= 0 ; )
  {
    CDrawObject* pObj = m_pOwner->GetObjectPtr( m_Trace[i]->m_ObjId );
    if( !pObj )
    {
      pObj = m_pOwner->ExtractMapObj( m_Trace[i]->m_ObjId );
      if( pObj )
        m_pOwner->m_ClipData.AddObject( pObj );
    }
    if( pObj )
    {
      m_Trace[i]->m_ObjShape = GetCoordList( pObj );
      m_Trace[i]->m_ObjPoint = GetCharPoint( pObj );
    }
    else
      Rem( i );
  }

  m_pOwner->ClearSelection( 0, 0 );
  for( int i = 0 ; i < m_Trace.GetSize() ; ++i )
    InsertSelect( m_pOwner->GetObjectPtr( m_Trace[i]->m_ObjId ), m_Trace[i]->m_ObjSelType );

  m_TmpTraceSize = m_Trace.GetSize();
  m_pOwner->UpdateWindow();
  m_pOwner->ShowSelectedNum();
}
//=====================================================================
int
CSelectTraceMode::Open()
{
  int bSlClient = GetProjectSLEnable();

  if( !bSlClient && !m_bNoInstructions  && m_nMode != STM_CALC )
  {
    CInstrDlg dlg( &m_bNoInstructions,
                  "\r\n"
                  "  Для просмотра и выбора объектов\r\n"
                  "  Используйте следующие клавиши:\r\n"
                  "\r\n"
                  "  Home\t- перейти в начало\r\n"
                  "  End\t- перейти в конец\r\n"
                  "  Пробел\t- выбрать следующий\r\n"
                  "  BackSpace\t- исключить предыдущий\r\n"
                  "  Ctrl+A\t- перезаписать все координаты\r\n"
                  "  Esc\t- выход", 0 );
    DoModalDlg( &dlg, CE_INF_DLG );
  }
  static char dl[] = ",;";
  CString tcn[CLASS_RANGE];
  CString ent( App.GetProfileString( "General", "TraceObjects" ));
  for( char* nm = strtok( ent.GetBuffer( 1024 ), dl ) ; nm ; nm = strtok( 0, dl ))
       m_TraceObjClass.Add( atoi( nm ));

  if( m_Trace.GetSize())
  {
    TGstream buf;
    CTabIdent* pTi = new CTabIdent[m_Trace.GetSize()];
    int i = m_Trace.GetSize(), n = 0;
    while( --i >= 0 )
      if( !m_pOwner->GetObjectPtr( m_Trace[i]->m_ObjId ))
        pTi[n++] = m_Trace[i]->m_ObjId;

    if( n )
      for( ResultCode r = R_REPEAT ; r == R_REPEAT ; )
      {
        TGstream buf;
        r =  m_pOwner->m_MapStore.FindObjects( pTi, &n, buf );
        m_pOwner->LoadMap( &m_pOwner->m_ClipData, buf.str(), n, m_pOwner->GetTrueScale(), 1 );
      }

    delete pTi;

    static long ih = 0;

    for( int i = m_Trace.GetSize() ; --i >= 0 ; )
      if( !m_pOwner->GetObjectPtr( m_Trace[i]->m_ObjId ))
      {
        if( bSlClient )
        {
          CMapView::GetConductor()->SetFindId( m_Trace[i]->m_ObjId.Table(), m_Trace[i]->m_ObjId.Entry());
          CMapView::GetConductor()->DeclareEvent( CE_NOT_IN_TRACE );
        }
        else
        if( !ih )
        {
          CString b;
          b.Format( "Не найден объект:\nТаблица %d, №%d",
                    m_Trace[i]->m_ObjId.Table(), m_Trace[i]->m_ObjId.Entry());
          CInstrDlg dlg( &ih, b, 0 );
                      DoModalDlg( &dlg, CE_INF_DLG );
        }
        Rem( i );
      }
  }
  Reopen();

  m_pOwner->m_Undo.RemoveAll();
  m_pOwner->m_pSTM = this;
  if( m_Trace.GetSize())
  {
    CDrawObject* pObj = m_pOwner->GetObjectPtr( m_Trace[0]->m_ObjId );
    int sc = m_pOwner->CalcShowScale( pObj, 1 );
    CPoint cp( pObj->GetBoundBox().CenterPoint());
    m_pOwner->SetScale( sc, cp );
  }

  OnSetCursor();
  return 1;
}
//=====================================================================
int
CSelectTraceMode::Term( int bDestroy )
{
  if( !bDestroy )
    m_pOwner->ClearSelection( 0, 0 );
  m_pOwner->m_Undo.RemoveAll();
  m_pOwner->m_pSTM = 0;
  m_pOwner->ShowSelectedNum();
  Purge();
  return 0;
}
//=====================================================================
inline int
Linked( int plt, int pln, CPoint* pl, int ptt, CPoint* pt )
{
  return plt == SH_POLYLINE && ptt == SH_POINT &&
         ( pt[0] == pl[0] || pt[0] == pl[pln-1] );
}
//=====================================================================
int
CSelectTraceMode::Expand( CDrawObject* pHostObj )
{
  int ur = m_pOwner->m_Conductor.GetUserRights();
  if( ur < UR_L_EDIT && m_nMode != STM_CALC )
    return AM_RDONE;

  CObjList lst;

  lst.AddHead( pHostObj );
  int bAdd = SwapObj( pHostObj );
  int hostType, hostLen;
  CPoint* hostPnts = pHostObj->GetAnyPoints( &hostType, &hostLen );

  ksi_obj ksi_list =
    KsiCall( bAdd ? "get-host-class-list" : "get-target-class-list", KSI_MK_SINT( pHostObj->Class()), 0 );

  if( KSI_LIST_P( ksi_list ))
  {
    for( ksi_obj h = ksi_list ; h != ksi_nil ; h = KSI_CDR( h ))
    {
      UINT type = ksi_num2int( KSI_CAR( h ));
      if( bAdd && !IsTraceClass( type ))
        continue;

      CLayer* pLayer = m_pOwner->GetLayer( type );
      for( CDrawObject* pTestObj = pLayer->SetLast() ; pTestObj = pLayer->FindPrev() ; )
        if( Intersect( &pTestObj->GetBoundBox(), &pHostObj->GetBoundBox()))
        {
          int testType, testLen;
          CPoint* testPnts = pTestObj->GetAnyPoints( &testType, &testLen );

          if(( bAdd ^ pTestObj->IsSelected()) &&
             ( bAdd ? Linked( hostType, hostLen, hostPnts, testType, testPnts ) :
                      Linked( testType, testLen, testPnts, hostType, hostPnts )))
          {
            SwapObj( pTestObj );
            lst.AddHead( pTestObj );
          }
        }
    }
  }
  m_pOwner->m_Undo.AddItem( new CUndoSelectItem( m_pOwner, &lst, 0 ), m_pOwner );
  m_pOwner->ShowSelectedNum();
  return AM_RDONE;
}
//=====================================================================
int
CSelectTraceMode::AskForClose( ST_MODE m )
{
  char const* pr[] =
  {
    0,
    0,
    "Завершить работу со схемой?",
    "Завершить раcчёт?"
  };

  char const* pp = m_nMode == m ? 0 : pr[m_nMode];
  return pp ? AskBox( pp ) == IDYES : 1;
}
//=====================================================================
int
CSelectTraceMode::Close()
{
  CConductor* pCnd = m_pOwner->GetConductor();
  if( m_nMode == STM_EDIT )
  {
    pCnd->RemoveAll();
    pCnd->Create( m_TraceIdent );
    for( int i = m_Trace.GetSize() ; --i >= 0 ; pCnd->AddLink( m_Trace[i]->m_ObjId ));
  }
  pCnd->DeclareEvent( CE_SCHEEM_END );
  Purge();
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CSelectTraceMode::OnRButtonDown( UINT& flags, CPoint& loc )
{
  if( flags & MK_SHIFT )
    return AM_RPROPAGATE;

  if( !AskForClose( ST_MODE( 0 )))
    return AM_RDONE;

  return Close();
}
//=====================================================================
int
CSelectTraceMode::OnLButtonDown( UINT &flags, CPoint &loc )
{
  m_pOwner->UnselObj();

  if( !( flags & MK_SHIFT ) || m_pOwner->GetMode( IDM_OBJEDIT_MODE ))
    return AM_RPROPAGATE;

  if( m_nMode == STM_VIEW )
    return AM_RDONE;

  CDrawObject* pObj;
  int bFound = 0;
  CObjDef const* pObjDef = m_pOwner->m_pObjDef;
  for( m_pOwner->BeginGlobalSearch( m_pOwner->GetSpot()) ;
       !bFound && ( pObj = m_pOwner->NextGlobalSearch()) ;
       bFound = IsTraceClass( pObj->Class()));
  if( pObj && !pObj->Table())
  {
    char buf[512];
    sprintf_s( buf, "Объект '%s' не может быть включён в схему кабеля,\n"
                    "так как он не учетён в БД.\n\n"
                    "Проверьте и сохраните его параметры.",
               m_pOwner->m_pObjDef->GetName( pObj->Class()));
    ErrBox( buf );
    bFound = 0;
  }
  int r = bFound ? Expand( pObj ) : AM_RDONE;
  if( bFound )
    CalcLen();
  return r;
}
//=====================================================================
int
CSelectTraceMode::OnChar( UINT& flags, UINT& key_code )
{
  switch( key_code )
  {
    default :
      return AM_RPROPAGATE;

    case 0x01 : // ^A
      ReselectAll();

    case 0x14 : // Home
      m_pOwner->OnJumpToSelect( IDM_SEL_JMP_HOME );
      break;

    case 0x15 : // End
      m_pOwner->OnJumpToSelect( IDM_SEL_JMP_END );
      break;

    case VK_ESCAPE : // Esc
      if( !m_pOwner->m_pCurSelObj )
        return OnRButtonDown( flags, CPoint( 0, 0 ));
      else
      {
        CDrawObject* pObj = m_pOwner->m_pCurSelObj;
        m_pOwner->StartBlinking( 0 );
        if( InTrace( pObj ) < 0 )
          m_pOwner->SelectObject( pObj, 0 );
        break;
      }

    case VK_BACK  :
    case ' ' :
      if( m_nMode == STM_VIEW )
        break;

      int bAdd = key_code == ' ';
      while( m_Trace.GetSize())
      {
        CDrawObject* pObj = m_pOwner->GetObjectPtr( m_Trace[m_Trace.GetUpperBound()]->m_ObjId );
        if( !pObj )
          break;

        ksi_obj ksi_cls = KSI_MK_SINT( pObj->Class());
        ksi_obj ksi_list1 = KsiCall( "get-host-class-list",   ksi_cls, 0 );
        ksi_obj ksi_list2 = KsiCall( "get-target-class-list", ksi_cls, 0 );
        ksi_obj list_arr[] = { ksi_list1, ksi_list2 };
        ksi_obj ksi_list = ksi_append_x( 2, list_arr );

        CObjList tList;
        while( KSI_PLIST_P( ksi_list ))
        {
          UINT type = ksi_num2int( KSI_CAR( ksi_list ));
          ksi_list = KSI_CDR( ksi_list );
          CLayer* pLayer = m_pOwner->GetLayer( type );
          for( CDrawObject* pTestObj = pLayer->SetLast() ; pTestObj = pLayer->FindPrev() ; )
            if( Intersect( &pTestObj->GetBoundBox(), &pObj->GetBoundBox()) &&
                ( bAdd ^ pTestObj->IsSelected()) &&
                !tList.Find( pTestObj ) &&
                pTestObj->IsLinkedTo( pObj ) != LT_NOTHING )
              tList.AddHead( pTestObj );
        }
        if( tList.GetCount() != 1 )
          break;
        Expand( bAdd? tList.GetHead() : pObj );
      }
      CalcLen();
      break;
  }
  return AM_RDONE;
}
//=====================================================================
void
CSelectTraceMode::ReselectAll()
{
  CConductor* pCnd = pView->GetConductor();
  CMainWin* pmw = (CMainWin*)AfxGetMainWnd();
  int n = GetTraceSize();
  m_TmpTraceSize = 0;
  pmw->ShowProgress( 0, n );
  for( int i = 0 ; i < n ; ++i )
  {
    CDrawObject* pObj = pView->GetObjectPtr( m_Trace[i]->m_ObjId );
    m_Trace[i]->m_ObjShape = GetCoordList( pObj );
    ++m_TmpTraceSize;
    pCnd->Create( m_Trace[i]->m_ObjId );
    pCnd->DeclareEvent( CE_TRACE_ADD );
    pmw->OffsetProgress( 1 );
  }
  pmw->HideProgress();
}
//=====================================================================
//=====================================================================
static char const* m[] =
{
  0,
  "Выделите первый объект...",
  "Выделите второй объект...",
  "Нажмите Enter для соединения..."
};
//=====================================================================
void
CShortTraceMode::DeclareEndSelection()
{
  CConductor* pCnd = m_pOwner->GetConductor();
  if( m_nMode == 3 )
  {
    pCnd->ClearParams();
    pCnd->AddLink( m_pOwner->m_SelectList.GetTail()->TabIdent());
    pCnd->AddLink( m_pOwner->m_SelectList.GetHead()->TabIdent());
    pCnd->DeclareEvent( CE_SHORTTRACE_END );
  }
  else
    pCnd->DeclareEvent( CE_SHORTTRACE_CNC );
}
//=====================================================================
int
CShortTraceMode::Init( CMapView* pOwner )
{
  m_pOwner = pOwner;
  m_pOwner->ClearSelection();
  return !m_pOwner->m_pSTM;
}
//=====================================================================
int
CShortTraceMode::Term( int bDestroy )
{
  if( !bDestroy )
  {
    ClearHint();
    m_pOwner->ClearSelection();
  }
  return 1;
}
//=====================================================================
int
CShortTraceMode::Open()
{
  m_pOwner->GetConductor()->ClearParams();
  return m_nMode = 1;
}
//=====================================================================
void
CShortTraceMode::Draw( CDC* pDC )
{
  ReportHint( m[m_nMode] );
}
//=====================================================================
int
CShortTraceMode::OnLButtonDown( UINT &flags, CPoint &loc )
{
  if( flags & MK_SHIFT )
	{
    CDrawObject* pObj;
    ( pObj = m_pOwner->GetDetectedObject( 0 )) ||
    ( pObj = m_pOwner->GetDetectedObject( 1 ));
    ExtSelect( pObj );
	}
  return AM_RDONE;
}
//=====================================================================
int
CShortTraceMode::TstSelect( CDrawObject* pObj )
{
  int nType;
  pObj->GetAnyPoints( &nType );
  if( m_nMode == 3 || nType != SH_POINT )
  {
    m_pOwner->SelectObject( pObj, 1 );
    m_pOwner->UpdateWindow();
    Sleep( 200 );
    m_pOwner->SelectObject( pObj, 0 );
    return 0;
  }

  m_pOwner->SelectObject( pObj, 1 );
  ++m_nMode;
  m_pOwner->GetConductor()->DeclareEvent( CE_SHORTTRACE_ADD );
  return 1;
}
//=====================================================================
int
CShortTraceMode::ExtSelect( CDrawObject* pObj )
{
  return pObj && ( m_pOwner->m_LocalLaw[pObj->Class()] & 1 ) && TstSelect( pObj );
}
//=====================================================================
int
CShortTraceMode::OnRButtonDown( UINT &flags, CPoint &loc )
{
  m_pOwner->GetConductor()->DeclareEvent( CE_SHORTTRACE_CNC );
  return AM_RDONE | AM_RREMOVE;
}
//=====================================================================
int
CShortTraceMode::OnChar( UINT& flags, UINT& key_code )
{
  if( m_nMode == 3 && key_code == 0xd )
  {
    CDrawObject* pObj0 = m_pOwner->m_SelectList.GetTail();
    CDrawObject* pObj1 = m_pOwner->m_SelectList.GetHead();
    if( pObj0->IsObjectTemp() || pObj1->IsObjectTemp())
    {
      Beep( 440, 400 );
      ErrBox( "Попытка использования временного объекта" );
      return AM_RDONE;
    }

    DeclareEndSelection();
    return AM_RDONE;
  }

  CConductor* pCnd = m_pOwner->GetConductor();
  switch( key_code )
  {
    case VK_ESCAPE :
    {
      pCnd->DeclareEvent( CE_SHORTTRACE_CNC );
      return AM_RDONE | AM_RREMOVE;
    case VK_BACK :
      if( m_nMode > 1 )
      {
        pCnd->DeclareEvent( CE_SHORTTRACE_SAB );
        m_pOwner->SelectObject( m_pOwner->m_SelectList.GetHead(), 0 );
        --m_nMode;
      }
      return AM_RDONE;
    }
  }
  return AM_RPROPAGATE;
}
//=====================================================================
