#include "stdafx.h"

#include "mview.h"
#include "idg.h"
#include "rstrmode.h"
#include "instrdlg.h"
#include "undo.h"
#include "mathutil.h"
#include "ids.h"
#include "dwin.h"
#include "util.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
static char bndHint[] = "Дальнейший просмотр невозможен!";
//=====================================================================
long CRestoreMode::m_bNoInstructions = 0;
//=====================================================================
CRestoreMode::CRestoreMode( CTabIdent id ) :
  CAppMode( AIDG_MOVMODE, IDC_RESTORE, AM_FUNIMODE | AM_FMONOPOLY | AM_FNOTHING,
            IDM_RESTORE_MODE ),
  m_Id( id )
{}
//=====================================================================
int
CRestoreMode::Term( int bDestroy )
{
  if( m_nMode )
 	  m_ObjList.RemoveAt( m_ObjList.Find( m_pObj, 0 ));

  while( !m_ObjList.IsEmpty()) 
    delete m_ObjList.RemoveHead();

  m_pOwner->ShowSelectedNum();
  return 0;
}
//=====================================================================
int
CRestoreMode::Init( CMapView* pOwner )
{
  m_pOwner = pOwner;
  
  TGstream buf;
  int nObj, n1;
  ResultCode r;

  {
    CWaitCursor wc;
    ReportHint( "Поиск удалённых объектов..." );
    r = m_pOwner->m_MapStore.SelectObject( m_Id, buf, &nObj, 1 );
    ClearHint();
  }

  if( r == R_OK && nObj )
  {
    m_pOwner->m_MapStore.SelectObject( m_Id, buf, &n1, 0 );

    ASSERT( buf.str());
    if( !buf.str())
      return 0;

    if( !m_bNoInstructions )
		{
      CInstrDlg dlg( &m_bNoInstructions,
                     "\r\n"
                     "  Для просмотра истории объекта\r\n"
                     "  Используйте следующие клавиши:\r\n"
                     "\r\n"
                     "  Home\t- в начало истории\r\n"
                     "  End\t- в конец истории\r\n"
                     "  <--\t- на шаг назад\r\n"
                     "  -->\t- на шаг вперед\r\n"
                     "  Esc\t- выход" );
       DoModalDlg( &dlg, CE_INF_DLG );
		}

    char* pSrc = buf.str();
    CBaseDataRec* pDrh = m_pOwner->GetDataRecHeadPtr();
    for( int n = nObj + n1 ; --n >= 0 ; )
    {
      memcpy( pDrh->GetBriefPtr(), pSrc, pDrh->GetBriefSize());
      pSrc += pDrh->GetBriefSize();
      void* pData = pSrc;
      CDrawContainer* pCont = m_pOwner->TranslateFromMap( &pData );
      pSrc = (char*)pData;
      if( pCont )
      {
        CDrawObject* pObj = new CDrawObject( pDrh->GetIdent(), pDrh->GetNode(),
                                             pDrh->GetTime(), pDrh->GetClock(),
                                             pCont );
        m_ObjList.AddHead( pObj );
        m_pOwner->ScaleObject( pObj, m_pOwner->GetTrueScale());
        pObj->CalcBoundBox();
      }
    }
	  m_pCurObj = n1 ? m_ObjList.GetHead() : 0;
		if( m_pCurObj )
		  delete m_pOwner->m_ClipData.AddObject( m_pCurObj );

		m_CurPos = m_ObjList.GetHeadPosition();
    ShowObject( m_ObjList.GetNext( m_CurPos ), !n1 );
    m_bLeft = 1;
		m_nMode = 1;
		return 1;
  }

  InfBox( "История объекта отсутствует!" );
  return 0;
}
//=====================================================================
int 
CRestoreMode::ShowObject( CDrawObject* pObj, int bDeleted )
{
  pObj->SetSelectFlag( 1 );
	m_pOwner->m_SelectList.GetHead() = m_pObj = pObj;

  if( !Intersect( &pObj->GetBoundBox(), m_pOwner->m_Clip ))
    m_pOwner->SetScale( m_pOwner->GetAltScale(), pObj->GetBoundBox().CenterPoint(), 1 );
  CDrawObject* pExc = m_pOwner->m_ClipData.AddObject( pObj );
  m_pOwner->ShowObjectBox( pExc );
  m_pOwner->ShowObjectBox( pObj );
  char s[256];
	m_pOwner->ResumeObject( pObj, bDeleted, s );
  ReportHint( s );

	return 1;
}
//=====================================================================
int  
CRestoreMode::ExitMode()
{
  if( m_pObj != m_pCurObj &&
      AskBox( "Сохранить?" ) == IDYES )
  {
 	  m_ObjList.RemoveAt( m_ObjList.Find( m_pObj, 0 ));
  	ShowObject( m_pObj, 0 );
	  m_pOwner->AddObject( m_pObj, 0 );
  }
	else
  if( m_pCurObj )
  {
	  m_ObjList.RemoveAt( m_ObjList.Find( m_pCurObj, 0 ));
    ShowObject( m_pCurObj, 0 );
	  m_pOwner->m_ClipData.AddObject( m_pCurObj );
  }
	else
	{
	  m_pOwner->m_ClipData.RemoveObject( m_pObj );
    m_pOwner->ShowObjectBox( m_pObj );
	}
      
	m_pOwner->ShowSelectedNum();
	m_nMode = 0;
  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CRestoreMode::OnChar( UINT& flags, UINT& key_code )
{ 
  int d = m_CurPos != m_ObjList.GetHeadPosition();
  switch( key_code )
  {
    case VK_ESCAPE :
		  return ExitMode();

    case 0x14 : // VK_HOME 
      m_CurPos = m_ObjList.GetTailPosition();
			m_bLeft = ShowObject( m_ObjList.GetNext( m_CurPos ), 1 );
      return AM_RDONE;

    case 0x15 : // VK_END 
      m_CurPos = m_ObjList.GetHeadPosition();
			m_bLeft = !ShowObject( m_ObjList.GetPrev( m_CurPos ), 0 );
      return AM_RDONE;

    case 0x12 : // VK_LEFT
      if( !m_CurPos && !m_bLeft )
			{
        m_CurPos = m_ObjList.GetHeadPosition();
				m_ObjList.GetNext( m_CurPos );
			}
      if( !m_CurPos && m_bLeft )
     		ReportHint( bndHint );
      else
        m_bLeft = ShowObject( m_ObjList.GetNext( m_CurPos ), d );
      return AM_RDONE;

    case 0x13 : // VK_RIGHT
      if( !m_CurPos && m_bLeft )
			{
        m_CurPos = m_ObjList.GetTailPosition();
				m_ObjList.GetPrev( m_CurPos );
      }
      if( !m_CurPos && !m_bLeft )
     		ReportHint( bndHint );
      else
        m_bLeft = !ShowObject( m_ObjList.GetPrev( m_CurPos ), d );
      return AM_RDONE;
  }
  return AM_RPROPAGATE;  
}
//=====================================================================
