#include "stdafx.h"

#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
  #include "tgmapcliv1.h"
  #include "tgmapmoleclicp.h"
#endif
#include "mdoc.h"
#include "ids.h"
#include "mview.h"
#include "drawobj.h"
#include "conduct.h"
#include "ksi.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "util.h"
#include "sltrmode.h"
//=====================================================================
#ifdef _FULL_PROJECT
static int DbConn;
static CTGMapCliV1 *pmapobj;
IDbMain1Ptr  DbMainObj;
#endif
//=====================================================================
CConductor::CConductor()
{
  CurrUserRight = -1;
  CurrentNode = 0;

#ifdef _FULL_PROJECT
  IsDbMainObjInit = 0;
  m_SelTabId = CTabIdent( 0, -1 );

  m_pCurrentNodeList = new long[2];
  m_pCurrentNodeList[0] = -1;
  m_pCurrentNodeList[1] = 0;
  m_nNodes = 1;
#endif
}
//=====================================================================
CConductor::~CConductor()
{
//  DbMainObj->Release();

  delete m_pCurrentNodeList;
  m_pCurrentNodeList = 0;

  RemoveAll();
}
//=====================================================================
void
CConductor::RegObjMifCoord( CDrawObject* pObj )
{
#ifdef _FULL_PROJECT
  if( GetProjectSLEnable())
  {
    long nTable  = pObj->Table();
    long nEntry  = pObj->Entry();
    CString mifCoordList( CSelectTraceMode::GetMifCoordList( pObj ));
    BSTR str = mifCoordList.AllocSysString();
    CTGMapCliV1::FireSetObjectCoord( nTable, nEntry, str );
    SysFreeString( str );
  }
#endif
}
//=====================================================================
void
CConductor::Create( CDrawObject* pObj )
{
  RemoveAll();
  m_Ident = pObj->Ident();
  m_ObjectNode = pObj->Node();
  m_pObj = pObj->CopyObject();
  ksi_obj ksi_img = *pObj;

  ksi_obj env = KsiCall( "unwrap",
                         KSI_MK_SINT( pObj->Class()),
                         KSI_MK_SINT( pObj->Status()),
                         ksi_img, 0 );

  while( KSI_PLIST_P( env ))
  {
    ksi_obj pair = KSI_CAR( env );
    env = KSI_CDR( env );
    if( KSI_STR_P( KSI_CDR( pair )))
    {
      m_Params.Add( KSI_SYM_PTR( KSI_CAR( pair )));
      m_Params.Add( KSI_STR_PTR( KSI_CDR( pair )));
    }
  }

  CTabIdList lst;
  for( int n = pView->GetHostIds( pObj, &lst ) ; --n >= 0 ;
       m_Links.Add( lst.RemoveHead()));

  m_ObjectCoord = CSelectTraceMode::GetCoordList( pObj );
}
//=====================================================================
ksi_obj
CConductor::CreateParamList()
{
  ksi_obj ksiPar = ksi_nil;
  for( int n = GetParamSize(), i = 0 ; i < n ; ++i )
    ksiPar = ksi_assv_set_x( ksiPar,
                             ksi_str02sym( GetParamName( i )),
                             ksi_str02string( GetParamValue( i )));
  return ksiPar;
}
//=====================================================================
int
CConductor::GetUserRights( int par )
{ // return: 0 -- none; 1 -- view; 2 -- edition

#ifndef _FULL_PROJECT
  if( par == 2 )
    pView->PostMessage( WM_COMMAND, IDM_USER_RIGHTS_OK );
//    pView->PostMessage( WM_COMMAND, IDM_USER_RIGHTS_CANCEL );

  return UR_ADMN;

#else
  if( !DbConn || !IsDbMainObjInit )
    return 1;

  if( GetProjectSLEnable())
  {
    if( par == 2 )
    {
      DeclareEvent( CE_GET_USER_RIGHTS );
//      pView->PostMessage( WM_COMMAND, IDM_USER_RIGHTS_OK );
    }
    return CurrUserRight == -1 ? UR_L_EDIT : CurrUserRight;
  }

  unsigned short ObjClass = 0;
  long TabId = 0;
  long IdObj = 0;

  if( par == 1 || CurrUserRight == -1 )
  {
    CurrUserRight = DbMainObj->GetEditRight( ObjClass, TabId, IdObj );
    TRACE1( "\nuser right=%d", CurrUserRight );
  }

  if( par == 2 )
  {
    GetClass( ObjClass );
    CTabIdent tid = GetTable();
    TabId = tid.Table();
    IdObj = tid.Entry();
    int tmpr = DbMainObj->GetEditRight( ObjClass, TabId, IdObj );
    TRACE1( "\nuser tmp right=%d", tmpr );
                pView->PostMessage( WM_COMMAND, tmpr >= UR_L_EDIT ? IDM_USER_RIGHTS_OK : IDM_USER_RIGHTS_CANCEL );
    return tmpr;
  }
  return CurrUserRight;
#endif
}
//=====================================================================
int
CConductor::SetUserRight( int UserRight ) // 0 -- none; 1 -- view; 2 -- edition
{
  CurrUserRight = UserRight;
  return UserRight;
}
//=====================================================================
int
CConductor::DeclareEvent( UINT event )
{
#ifdef _FULL_PROJECT

  m_LastEvent = event;
  return !DbConn || !IsDbMainObjInit ? 0 : DeclareRemoteEvent( event );

#else

  return 0;

#endif
}
//=====================================================================
int
CConductor::InitConnect()
{

#ifdef _FULL_PROJECT

 IsDbMainObjInit = 0;

 DbConn = 1;

 if( !DbConn )
   return 0;

 int Sl_Client=GetProjectSLEnable();

 if( Sl_Client )
 {
   IsDbMainObjInit=1;
   return 1;
 }

 if( !IsDbMainObjInit )
 {
   try
   {

     HRESULT hr = DbMainObj.CreateInstance( __uuidof( DbMain1 ));
     if( FAILED( hr ))
     {
       AfxMessageBox( "Невозможно установить связь с объектом ФОРМЫ" );
       IsDbMainObjInit = 0;
       return -1;
     }

     pmapobj = new CComObjectNoLock<CTGMapCliV1>;

     if( !pmapobj )
     {
       IsDbMainObjInit = 0;
       TRACE( "Не создан объект класса CTGMapCliV1" );
       return -2;
     }
     IDispatch* pI;

     hr = pmapobj->QueryInterface( IID_IDispatch,(void **)&pI );
     if( FAILED( hr ))
     {
       IsDbMainObjInit = 0;
       TRACE( "Не получен IDispatch от IMapmole1" );
       pmapobj->Release();
       return -4;
     }

     long status = DbMainObj->Initialize();

     if( status != 0 )
     {
       IsDbMainObjInit = 0;
       TRACE( "Ошибка метода  СDbMain1::Initialize()" );
       return -3;
     }

     DbMainObj->SetMapObj( pI );

     pI->Release();

     status = DbMainObj->Start();

     if( status !=0 )
     {
       IsDbMainObjInit = 0;
       TRACE( "Ошибка метода CDmain1::Start" );
       return -5;
     }

     long rt = DbMainObj->GetEditRight( 0, 0, 0 );


     IsDbMainObjInit = 1;

     GetUserRights( 1 );

     return  rt;
    }
    catch( _com_error& e )
    {
      //AfxMessageBox( Compose((wchar_t const*)e.Source(), "\n",
        //                     (wchar_t const*)e.Description(), 0 ));
      IsDbMainObjInit = 0;
      return -1;
    }
  }

#endif

  return 1;
}
//=====================================================================
int
CConductor::DeclareRemoteEvent( int event )
{
#ifdef _FULL_PROJECT

  if( !DbConn )
    return 0;

  long rt = 0;
  int i;
  int Sl_Client = GetProjectSLEnable();

  if( event >= CE_MODAL_DLG && event <= CE_MODAL_DLG_END )
    return Sl_Client ? CTGMapCliV1::FireStartForm( 0, long( event )) : 0;

  try
  {
    CTGMapCliV1::FireTest( event );

    unsigned short ObjClass;
    GetClass( ObjClass );

    CTabIdent tt[] = { GetTable(), GetTableDel1(), GetTableDel2() };

    if( Sl_Client )
    {
      for( i = 0 ; i < 3 ; ++i )
        CTGMapCliV1::FireSetParam( ObjClass, tt[i].Table(), tt[i].Entry(), i );

      for( i = GetParamSize() ; --i >= 0 ; )
        CTGMapCliV1::FirePreSetObjectParam( _bstr_t( GetParamName( i )), _bstr_t( GetParamValue( i )));

      for( i = GetLinkSize() ; --i >= 0 ; )
      {
        CTabIdent tid( GetTable( i ));
        CTGMapCliV1::FireSetLinkParam( tid.Table(), tid.Entry(), i );
      }

      CTGMapCliV1::FireSetCurrentMapUzel( long( GetObjectNode()));
      CTGMapCliV1::FireStartForm( long( ObjClass ), long( event ));
    }
    else
    {
      for( i = 0 ; i < 3 ; ++i )
        if( rt = DbMainObj->SetParam( ObjClass, tt[i].Table(), tt[i].Entry(), i ))
        {
          AfxMessageBox( "Ошибка установки TabId, ObjId", MB_OK | MB_ICONSTOP );
          return 0;
        }

      for( i = GetParamSize() ; --i >= 0 ; )
      {
        rt = DbMainObj->PreSetObjectParam( _bstr_t( GetParamName ( i )), _bstr_t( GetParamValue( i )));
        if( rt < 0 )
          return -1;
      }

      for( i = GetLinkSize() ; --i >= 0 ; )
      {
        CTabIdent tid( GetTable( i ));
        rt = DbMainObj->SetLinkParam( tid.Table(), tid.Entry(), i );
        if( rt < 0 )
          return -2;
      }

      rt = DbMainObj->SetCurrentMapUzel( long( GetObjectNode()));
      rt = DbMainObj->StartForm( long( ObjClass ), long( event ));
    }
  }

/*
    for( i = 0 ; i < 3 && !rt ; ++i )
    {
      if( Sl_Client )
        CTGMapCliV1::FireSetParam( ObjClass, tt[i].Table(), tt[i].Entry(), i );
      else
        rt = DbMainObj->SetParam( ObjClass, tt[i].Table(), tt[i].Entry(), i );
    }

    if( rt )
    {
      AfxMessageBox( "Ошибка установки TabId, ObjId", MB_OK | MB_ICONSTOP );
      return 0;
    }

    for( i = GetParamSize() ; --i >= 0 ; )
    {
      if( Sl_Client )
        CTGMapCliV1::FirePreSetObjectParam( _bstr_t( GetParamName ( i )),_bstr_t( GetParamValue( i )));
      else
        rt = DbMainObj->PreSetObjectParam( _bstr_t( GetParamName ( i )), _bstr_t( GetParamValue( i )));

      if( rt < 0 )
        return -1;
    }

    for( i = GetLinkSize() ; --i >= 0 ; )
    {
      CTabIdent tid( GetTable( i ));

      if( Sl_Client )
        CTGMapCliV1::FireSetLinkParam( tid.Table(), tid.Entry(), i );
      else
        rt = DbMainObj->SetLinkParam( tid.Table(), tid.Entry(), i );

      if( rt < 0 )
        return -2;
    }

    if( Sl_Client )
      CTGMapCliV1::FireSetCurrentMapUzel( long( GetObjectNode()));
    else
      rt = DbMainObj->SetCurrentMapUzel( long( GetObjectNode()));


    if( Sl_Client )
      CTGMapCliV1::FireStartForm( long( ObjClass ), long( event ));
    else
      rt = DbMainObj->StartForm( long( ObjClass ), long( event ));
  }
*/
  catch(...)
  {
    return IsDbMainObjInit = 0;
  }

  return rt;

#else

  return 0;

#endif
}
//=====================================================================
void
CConductor::FreeConnection()
{
#ifdef _FULL_PROJECT
 int Sl_Client = GetProjectSLEnable();

 if( DbConn && !Sl_Client )
 {
   //while(pmapobj->Release());
   delete pmapobj;
   pmapobj = 0;
 }
#endif
}
//=====================================================================
void
CConductor::StartReport()
{
#ifdef _FULL_PROJECT
 int Sl_Client = GetProjectSLEnable();
 if( DbConn && !Sl_Client)
   DbMainObj->Report( "start" );
#endif
}
//=====================================================================
int
CConductor::DeclareExtEvent(BSTR Name, BSTR Var1, BSTR Var2, BSTR Var3, BSTR Var4, BSTR Var5)
{
#ifdef _FULL_PROJECT
   int Sl_Client = GetProjectSLEnable();
   return DbConn && !Sl_Client ? DbMainObj->DeclareExtEvent( Name, Var1, Var2, Var3, Var4, Var5 ) : -100;
#endif
   return 0;
}
//=====================================================================
BOOL
CConductor::PreTranslateMessage( MSG* pMsg )
{
#ifdef _FULL_PROJECT
  if( DbConn && !GetProjectSLEnable())
    return DbMainObj->PreTranslateMessage( 0,
                                           pMsg->message,
                                           pMsg->wParam,
                                           pMsg->lParam,
                                           pMsg->time,
                                           pMsg->pt.x,
                                           pMsg->pt.y );
  return -100;
#endif

  return 0;
}
//=====================================================================
void
SpaceQueParam::SetClassMap( int t )
{
  int cla[CLASS_RANGE];
  for( int n = pView->GetDocument()->GetObjDef()->GetBottom( t, cla ) ; --n >= 0 ; )
  {
    t = cla[n];
    classMap[(t>>3)&(ms-1)] |= 1<<(t&7);
  }
}
//=====================================================================
