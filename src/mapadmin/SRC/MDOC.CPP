#include "stdafx.h"

#include "mapstore.h"
#include "msutil.h"
#include "mview.h"
#include "ksi_type.h"
#include "ksiutil.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "drawprim.h"
#include "bldmode.h"
#include "graphdoc.h"
#include "mdoc.h"
#include "dlg0.h"
#include "dwin.h"
#include "util.h"
#include "linetmpl.h"
#include "sltrmode.h"
//=====================================================================
IMPLEMENT_DYNCREATE( CMapDoc, CDocument)
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
CMapDoc* pDocument = 0;
//=====================================================================
static TCHAR ObjectSubmenuName[] = "Объект";
static TCHAR MapMenuName[] = "Карта";
//=====================================================================
void
CMapDoc::DeletePopupMenu( CMenu* pMenu, int lvl )
{
  if( !pMenu )
    return;

  for( int i = pMenu->GetMenuItemCount() ; --i >= 0 ;
       DeletePopupMenu( pMenu->GetSubMenu( i ), 1 ));
  if( lvl )
    delete pMenu;
}
//=====================================================================
static CMenu*
GetSubMenu( CMenu* pMenu, LPCSTR caption )
{
  CMenu* pDst = 0;
  if( pMenu )
    for( int i = pMenu->GetMenuItemCount() ; --i >= 0 && !pDst ; )
    {
      CString is;
      pMenu->GetMenuString( i, is, MF_BYPOSITION );
      CMenu* pSub = pMenu->GetSubMenu( i );
      if( pSub )
        pDst = is == caption ? pSub : GetSubMenu( pSub, caption );
    }
  return pDst;
}
//=====================================================================
CMenu*
CMapDoc::GetSubMenu( LPCSTR caption )
{
  CMenu* pMainMenu = ((CMainWin*)AfxGetMainWnd())->GetMenu();
  return ::GetSubMenu( pMainMenu, caption );
}
//=====================================================================
int
CMapDoc::CanCloseFrame( CFrameWnd* pFrame )
{
  if( pView->SublDlgIsActive())
    pView->m_pSublDlg->OnEndButtonClicked();

#ifndef _FULL_PROJECT
  if( pView->m_ConvDlg.IsConversion())
  {
    pView->m_ConvDlg.OnStopClicked();
    return 0;
  }
  if( pView->m_ConvDlg.IsWindowVisible())
    pView->m_ConvDlg.OnCancel();

  if( pView->m_ShabDlg.IsActive())
    pView->m_ShabDlg.OnCancel();
  
	if( pView->m_AbstractDlg.IsActive())
    pView->m_AbstractDlg.OnCancel();
#endif

  UINT f = 0;
  CPoint p;
  
  return( !pView->m_pSTM || ( pView->m_pSTM->OnRButtonDown( f, p ) & AM_RREMOVE )) &&
#ifndef _FULL_PROJECT
          !pView->m_ShabDlg.IsActive() &&
          !pView->m_AbstractDlg.IsActive() &&
#endif
          !pView->GetBuilding( 1 ) && 
          CDocument::CanCloseFrame( pFrame );
}
//=====================================================================
CMapDoc::CMapDoc() :
  m_bInited( 0 ),
  m_DocSize( 1000, 1000 ),
  m_DocName( "" )
{
  CLineTmpl::CreateTemplates();
  OnEndPrinting();
  pDocument = this;
}
//=====================================================================
void
CMapDoc::OnCloseDocument()
{
  DeletePopupMenu( GetSubMenu( ObjectSubmenuName ), 0 );
  CLineTmpl::DeleteTemplates();
  m_ObjDef.SaveModified();
	m_BmpDefArray.Clean();
	
	pView->RemoveTempObj();

  CDocument::OnCloseDocument();
}
//=====================================================================
void
CMapDoc::OnHelp()
{
}
//=====================================================================
int
CMapDoc::InitColorTab()
{
  int std = InitColorTab( "ColorTab", 
                          m_ViewColorTab, 
                          m_PrintColorTab, 
                          m_PrintBWColorTab );
  int utd = InitColorTab( "UserColorTab", 
                          m_UserViewColorTab, 
                          m_UserPrintColorTab, 
                          m_UserPrintBWColorTab );
  if( std && !utd )
  {
    int minKey, maxKey;
    m_ViewColorTab.GetMinMaxKey( &minKey, &maxKey );
    for( int k = minKey ; k <= maxKey ; ++k )
    {
      long c0 = m_ViewColorTab.TranslateKey( k );
      if( c0 < 0 )
        continue;
      long c1 = m_PrintColorTab.TranslateKey( k );
      long c2 = m_PrintBWColorTab.TranslateKey( k );
      m_UserViewColorTab.Add( k, c0 );
      m_UserPrintColorTab.Add( k, c1 );
      m_UserPrintBWColorTab.Add( k, c2 );
      char szKey[16], szCol[48];
      sprintf( szKey, "%d", k );
      sprintf( szCol, "0x%06lx 0x%06lx 0x%06lx", c0, c1, c2 );
      App.WriteProfileString( "UserColorTab", szKey, szCol );
    }
    utd = 1;
  }
  return std && utd;
}
//=====================================================================
int 
CMapDoc::InitColorTab( char const* sectName,
                       CIntAssocMap& ViewColorTab,
                       CIntAssocMap& PrintColorTab,
                       CIntAssocMap& PrintBWColorTab )
{
  char entrs[16384];
  if( !App.GetProfileKeys( sectName, entrs, sizeof entrs ))
    return 0;

  for( char* name = entrs ; *name ; name = strchr( name, 0 ) + 1 )
  {
    CString cdf( App.GetProfileString( sectName, name ));

    int key = 0, c0 = 0, c1 = 0, c2 = 0;
    if( !sscanf( name, "%d", &key ))
      continue;

    switch( sscanf( cdf, "%x%x%x", &c0, &c1, &c2 ))
    {
      case 1 : c1 = c0;
      case 2 : c2 = c1;
      case 3 : break;
      default: continue;
    }
    char b[sizeof c2];
    memcpy( b, &c2, sizeof c2 );
    b[0] = b[1] = b[2] = ( b[0] + b[1] + b[2] ) / 3;
    memcpy( &c2, b, sizeof c2 );

    ViewColorTab.Add( key, c0 );
    PrintColorTab.Add( key, c1 );
    PrintBWColorTab.Add( key, c2 );
  }
  return 1;
}
//=====================================================================
void 
CMapDoc::OnBeginPrinting( int bColor )
{ 
  CItem::m_pCurColorTab = bColor ? &m_PrintColorTab : &m_PrintBWColorTab; 
  CItem::m_pCurUserColorTab = bColor ? &m_UserPrintColorTab : &m_UserPrintBWColorTab; 
}
//=====================================================================
void 
CMapDoc::OnEndPrinting()
{
  CItem::m_pCurColorTab = &m_ViewColorTab; 
  CItem::m_pCurUserColorTab = &m_UserViewColorTab; 
}
//=====================================================================
LPTSTR
CMapDoc::SetDocType()
{
  if( !m_ObjDef.Init( App.GetIniPath()))
    return "Некорректно или отсутствует описание объектов";

	if( !App.GetSubbmpPath( 2 ).IsEmpty())
	{
    int par[12];
		memset( par, 0, sizeof par - sizeof( int ));
    par[0] = SUBSTAT_CLASS;
	  par[8] = 1;
    m_ObjDef.Add( "Подложка статическая", par );
    par[1] = par[0];
	  par[11] = SUBSTAT_RANGE;
	  par[3] = 16000000;
	  par[4] = 0;
    for( int i = 1 ; i <= SUBSTAT_RANGE ; ++i )
		{
		  par[0] += 1;
		  par[2] = i;
  	  char name[16];
		  sprintf( name, "1:%d", par[3] );
		  m_ObjDef.Add( name, par );
		  par[11] -= 1;
		  par[4] = par[3];
		  par[3] = i == 7 ? 128000 : par[3] /= 2;
		}
	}

  ReportHint( "Загрузка описания шрифтов" );
  if( !m_FontTab.Init( "Fontface" ))
    return "Некорректна или отсутствует таблица шрифтов";
  ReportHint( "Загрузка описания цветов" );
  if( !InitColorTab())
    return "Некорректна или отсутствует таблица цветов";

  static char gsm[] = "General";

  sscanf( App.GetProfileString( gsm, "SelectColors" ), "%x%x%x%x%x%x",
          CItem::m_SelectColor+1, CItem::m_SelectColor+2, CItem::m_SelectColor+3,
          CItem::m_SelectColor+4, CItem::m_SelectColor+5, CItem::m_SelectColor+6 );

  sscanf( App.GetProfileString( gsm, "MoveObjectColor", "0x0000007f" ), "%x", CItem::m_SelectColor+7 );

  ReportHint( "Загрузка описания значков" );
  m_BmpDefArray.Init();
  ModifyObjectMenu();
  m_bInited = 1;
  ClearHint();

  if( GetProjectSLEnable())
	{
		int ns[] =
		{ 
			IDM_SELECT_UZEL,
			IDM_CHANGE_UZEL,
			IDM_SPACE_QUERY_DEV,
			IDM_DEVORG
		};
    CMenu* pMM = GetSubMenu( MapMenuName );
		for( int i = CELEM( ns ) ; --i >= 0 ; )
			pMM->DeleteMenu( ns[i], MF_BYCOMMAND );
		int nc = pMM->GetMenuItemCount();
		for( i = nc ; --i >= 0 ; )
			if( pMM->GetMenuItemID( i ) == 0 && 
				  ( i == 0 || i == nc - 1 || i > 0 && pMM->GetMenuItemID( i - 1 ) == 0 ))
  			pMM->DeleteMenu( i, MF_BYPOSITION );
	}

  return 0;
}
//=====================================================================
void
CMapDoc::CreateLayers( CGraphData* pGraph )
{
  pGraph->SetLayersRange( &m_ObjDef );
  pGraph->ClipObjects( 0, 0 );
}
//=====================================================================
void
CMapDoc::ModifyObjectMenu()
{
  CMenu* pSub = GetSubMenu( ObjectSubmenuName );
  ASSERT( pSub );
  if( !pSub )
    return;

  DeletePopupMenu( pSub, 0 );
  for( int i = pSub->GetMenuItemCount() ; --i >= 0 ; pSub->RemoveMenu( i, MF_BYPOSITION ));
  SetContextMenu( 0 );
  MakePopupMenu( pSub, 0, IDM_OBJCTL_BEGIN, CObjDef::GetDerived );
}
//=====================================================================
void
CMapDoc::SetTitle( LPCTSTR docname )
{
#ifndef _FULL_PROJECT
  CDocument::SetTitle( GetDocName());
#endif
}
//=====================================================================
CString
CMapDoc::GetDocName()
{
  char buf1[_MAX_PATH], buf2[_MAX_FNAME];
  _splitpath( GetPathName(), buf1, buf1, buf2, buf1 );
  return buf2;
}
//=====================================================================
int
CMapDoc::MakePopupMenu( CMenu* pMenu, int type, UINT id, ODF fun ) const
{
  int types[CLASS_RANGE];
  return MakePopupMenu( pMenu, types, (m_ObjDef.*fun)( type, types ), id );
}
//=====================================================================
int
CMapDoc::MakePopupMenu( CMenu* pMenu, int* types, int n, UINT id ) const
{
  int i, j, t;

  for( i = n ; --i >= 0 ; )
    for( j = n ; --j >= 0 ; )
      if( m_ObjDef.IsDerivedFrom( types[i], types[j] ))
         memcpy( types + i, types + i + 1, sizeof( int ) * ( --n - i ));

  for( i = 0 ; i < n ; ++i )
	{
		int r0 = m_ObjDef.GetMenuRange( t = types[j=i] );
	
		while( --j >= 0 )
		{
			int r1 = m_ObjDef.GetMenuRange( types[j] );
			if( r1 > r0 || r1 == r0 && strcmp( m_ObjDef.GetName( types[j] ), m_ObjDef.GetName( t )) > 0 )
        types[j+1] = types[j];
			else
				break;
		}
		types[j+1] = t;
	}

  for( i = 0 ; i < n ; ++i )
  {
    int type = types[i];
    if( type >= SUBLAYER_CLASS )
      continue;

    char const* name = m_ObjDef.GetName( type );

    if( name )	
      if( m_ObjDef.GetDerived( type ))
      {
        CMenu* pPop = new CMenu;
        pPop->CreatePopupMenu();
        if( MakePopupMenu( pPop, types[i], id, CObjDef::GetDerived ))
          pMenu->AppendMenu( MF_BYPOSITION | MF_POPUP, (UINT)pPop->m_hMenu, name );
        else
          delete pPop;
      }
      else
      if( m_ObjDef.IsClass( type ) && m_ObjDef.GetLoad( type ) && 
          ( KSI_TRUE_P( KsiCall( "buildable?", KSI_MK_SINT( type ), 0 )) ||
					  m_bContextMenu && KSI_TRUE_P( KsiCall( "is-only-insertable?", KSI_MK_SINT( type ), 0 ))))
      {
        UINT f = MF_BYPOSITION | MF_STRING;
        int mas = m_ObjDef.GetMaxLimit( type );
        int mis = m_ObjDef.GetMinLimit( type );
        int i = type >> 3, m = 1 << ( type & 0x7 );
        if( mis && pView->GetTrueScale() < mis ||
            mas && pView->GetTrueScale() > mas ||
            !( pView->m_LoadMap[i] & m ) ||
            !( pView->m_ShowMap[i] & m ) ||
            !( pView->m_LocalLaw[type] & 1 ))
          f |= MF_GRAYED;
        pMenu->AppendMenu( f, id + type, name );
      }
  }
  return pMenu->GetMenuItemCount();
}
//=====================================================================
