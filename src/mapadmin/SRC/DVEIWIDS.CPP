#include "stdafx.h"

#include "mview.h"
#include "mdoc.h"
#include "scale.h"
#include "ids.h"
#include "util.h"
#ifdef _FULL_PROJECT
  #include "tgmapmole.h"
#else
  #include "mapmole.h"
#endif
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
int
CMapView::GetDsbl( int type )
{ 
  int i = type >> 3, m = 1 << ( type & 0x7 );
  int tps[CLASS_RANGE];
  int dsbl = m_pObjDef->GetDerived( type, tps );
  if( !dsbl )
    return !( m_LocalLaw[type] & 1 ) || 
           !( GetBit( m_ShowMap, type )) ||
           !IsVisibleInScale( type );
  
	while( --dsbl >= 0 && GetDsbl( tps[dsbl] ));
  return dsbl < 0;
}
//=====================================================================
void
CMapView::OnUpdateObjCommandUI( CCmdUI* pCmdUI )
{
  int type = pCmdUI->m_nID - IDM_OBJCTL_BEGIN;
  pCmdUI->Enable( !GetDsbl( type ));
  pCmdUI->SetCheck( GetMode( IDM_CLASS_BEGIN + type ) != 0 );
}
//=====================================================================
void
CMapView::OnUpdateAppCommandUI( CCmdUI* pCmdUI )
{
  static UINT aid[] =
  {
    IDM_OBJECT_ADMINISTATOR,
    IDM_BAR_ADMINISTATOR,
    IDM_LINE_TEMPL_EDITOR,
    IDM_SUBL_ADMINISTATOR,
    IDM_REFRESH_OVERVIEW,
    IDM_SELECT_POLY_MODE,
    IDM_OBJEDIT_MODE,
    IDM_CONNECT_MODE,
    IDM_RESTORE_MODE,
		IDM_FIND_DIST_MODE,
//    IDM_STREETPLAN_MODE,
    IDM_RESTORE_MODE,
    0
  };

  pCmdUI->SetCheck( GetMode( pCmdUI->m_nID ) != 0 );
  UINT* p;
  for( p = aid ; *p && pCmdUI->m_nID != *p ; ++p );
  pCmdUI->Enable( !*p || m_Conductor.GetUserRights() >= UR_L_EDIT );
}
//=====================================================================
