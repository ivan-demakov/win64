#ifndef __CONDUCT_H__
#define __CONDUCT_H__
//=====================================================================
#include "ident.h"
#include "drawobj.h"
#include "scommand.h"
//=====================================================================
#ifdef _FULL_PROJECT
  #include "prjlib.h"
  #ifdef _DEBUG
    #import "..\..\bin\dbmapobj.tlb" no_namespace
  #else
    #import "..\..\binr\dbmapobj.tlb" no_namespace
  #endif
#endif
//=====================================================================
class CDrawObject;
struct SpaceQueParam
{
  enum { ms = 1024/8 };
  UINT mode;
  UINT eventCode;
  int maxSel;
  int node;
  CTabIdent id;
  int radius;
  char classMap[ms];

  void ResetClassMap() { memset( classMap, 0, ms ); }
  void SetClassMap( int t );
};
//=====================================================================
struct ObjectInfo
{
  int       ObjClass;
  CTabIdent ObjIdent;
  int       Distance; 
  ObjectInfo( int t = 0, CTabIdent i = CTabIdent( 0, 0 ), int d = 0 ) :
    ObjClass( t ),
    ObjIdent( i ),
    Distance( d )
  {}
};

typedef CArray<ObjectInfo, ObjectInfo&> ObjectInfoArray;
//=====================================================================

class CConductor
{
  long CurrUserRight;

public:

  CConductor();
  ~CConductor();

  void Create( CIdent id );
  void Create( CTabIdent tabId );
  void Create( CDrawObject* pObj );
  void Create( CTabIdent tabId1, CTabIdent tabId2 );
  void RegObjMifCoord( CDrawObject* pObj );

  void RemoveAll();
  void SetFindId( TableType table, EntryType entry );
  void SetTable( TableType table, EntryType entry = 0 );
  void SetTable( int i, TableType table, EntryType entry );
  void SetSelTable( CTabIdent tab_obj );
  void SetFindMode( int mode );
  int  GetFindMode();

  int  GetScale() { return m_Scale; }
  void SetScale( int s ) { m_Scale = s; }
  CPoint GetPosition() { return m_Position; }
  void SetPosition( CPoint p) { m_Position = p; }

  void SetClass( ClassType cls, StatusType sts = 0 );
  void SetStatus( int sts = 0 );
  void GetClass( ClassType& nClass );
  ClassType GetClass();
  CIdent    GetIdent();
  CTabIdent GetFindId();
  CTabIdent GetTable();
  int       GetNode();
  int       GetObjectNode();
  int       SetObjectNode( int n );
  CTabIdent GetSelTable();
  CTabIdent GetTableDel1();
  CTabIdent GetTableDel2();
  CTabIdent GetTable( int i );
  void AddParam( char const* name, char const* value );
  int  GetParamSize();
  char const* GetParamName( int i );
  char const* GetParamValue( int i );
  void AddLink( CTabIdent& id );
  void InsLink( CTabIdent& id );
  int  GetLinkSize();
  int  GetEvent();
  CStringArray const& GetParams();
  ksi_obj CreateParamList();
  CDrawObject* GetObject();
  ObjectInfoArray& GetSelected() { return m_Selected; }

  int  CurrentNode;
  int  SetCurrentNode( int Node );
  int  m_nNodes;
  long* m_pCurrentNodeList;
  void SetCurrentNodeList( int nNodes, long* pNodelist );

  CString CurrProjectName;
  int     SetCurrentProjectName(CString name);
  CString GetCurrentProjectName();

  int     SetCurrentSubbmpPath(CString name);
  CString GetCurrentSubbmpPath();

  int     SetCurrentPublicStaticSubbmpPath(CString name);
  CString GetCurrentPublicStaticSubbmpPath();

  int     SetCurrentUserStaticSubbmpPath(CString name);
  CString GetCurrentUserStaticSubbmpPath();

  CString CurrMapServer;
  int     SetCurrentMapServer(CString name);
  CString GetCurrentMapServer();

  long    CurrMapNum;
  int     SetCurrentMapNum( long MapNum );
  long    GetCurrentMapNum( void );

 // virtual int DeclareEvent( CONDUCT_EVENT event );
  virtual int DeclareEvent( UINT event );


  int GetUserRights( int par = 0 );
  int SetUserRight( int UserRight );

// DbMAin Interface
  int  InitConnect();
  int  DeclareRemoteEvent( int event );
  void FreeConnection();
  void ClearParams();
  void StartReport();
  int  DeclareExtEvent(BSTR Name, BSTR Var1, BSTR Var2, BSTR Var3, BSTR Var4, BSTR Var5);
//
  BOOL PreTranslateMessage( MSG* pmsg );
//

public:
  SpaceQueParam m_SpaceQueParam;

  CTabIdent GetSpaceQueFindId() { return m_SpaceQueParam.id; }

private:
  CString      m_CurrSubbmpPath;
  CString      m_PublicStaticSubbmpPath;
  CString      m_UserStaticSubbmpPath;

  int          m_FindMode;
  CIdent       m_Ident;
  int          m_ObjectNode;
  CString      m_ObjectCoord;
  CTabIdent    m_FindId;
  CTabIdent    m_SelTabId;
  CTabIdent    m_TabIdDel1;
  CTabIdent    m_TabIdDel2;
  CDrawObject* m_pObj;
  CStringArray m_Params;
  CTabIdArray  m_Links;
  ObjectInfoArray m_Selected;
  int          m_LastEvent;
  int          m_Scale;
  CPoint       m_Position;
#ifdef _FULL_PROJECT
  int          IsDbMainObjInit;
//  IDbMain1Ptr  DbMainObj;
#endif
};
//=====================================================================
inline void
CConductor::Create( CIdent id )
{
  RemoveAll();
  m_Ident = id;
}
//=====================================================================
inline void
CConductor::Create( CTabIdent tabId )
{
  RemoveAll();
  SetTable( tabId.Table(), tabId.Entry());
}
//=====================================================================
inline void
CConductor::Create( CTabIdent tabId1, CTabIdent tabId2 )
{
  m_TabIdDel1 = tabId1;
  m_TabIdDel2 = tabId2;
}
//=====================================================================
inline CIdent
CConductor::GetIdent()
{
  return m_Ident;
}
//=====================================================================
inline void
CConductor::SetFindId( TableType table, EntryType entry )
{
  m_FindId = CTabIdent( table, entry );
}
//=====================================================================
inline void
CConductor::SetTable( TableType table, EntryType entry )
{
  m_Ident.SetTable( table );
  m_Ident.SetEntry( entry );
}
//=====================================================================
inline void
CConductor::SetTable( int i, TableType table, EntryType entry )
{
  if(  i >= 0 && i < GetLinkSize())
    m_Links[i] = CTabIdent( table, entry );
}
//=====================================================================
inline void
CConductor::SetSelTable( CTabIdent tab_obj )
{
  m_SelTabId = tab_obj;
}
//=====================================================================
inline void
CConductor::SetClass( ClassType cls, StatusType sts )
{
  m_Ident.SetClass( cls );
  m_Ident.SetStatus( sts );
}
//=====================================================================
inline void
CConductor::SetStatus( int sts )
{
  m_Ident.SetStatus( sts );
}
//=====================================================================
inline CTabIdent
CConductor::GetTable()
{
  return m_Ident.TabIdent();
}
//=====================================================================
inline CTabIdent
CConductor::GetSelTable()
{
  return m_SelTabId;
}
//=====================================================================
inline void
CConductor::RemoveAll()
{
  m_Ident.SetClass( 0 );
  m_Params.RemoveAll();
  m_Links.RemoveAll();
  m_ObjectCoord = "";
//  m_Selected.RemoveAll();
  SetTable( 0, 0 );
  delete m_pObj;
  m_pObj = 0;
}
//=====================================================================
inline CStringArray const&
CConductor::GetParams()
{
  return m_Params;
}
//=====================================================================
inline void
CConductor::AddParam( char const* name, char const* value )
{
  int i = m_Params.Add( name );
  m_Params.Add( value );
  m_Params[i].MakeLower();
}
//=====================================================================
inline int
CConductor::GetParamSize()
{
  return m_Params.GetSize() / 2;
}
//=====================================================================
inline char const*
CConductor::GetParamName( int i )
{
  return i >= 0 && i < GetParamSize() ? LPCSTR( m_Params[i+i] ) : 0;
}
//=====================================================================
inline char const*
CConductor::GetParamValue( int i )
{
  return i >= 0 && i < GetParamSize() ? LPCSTR( m_Params[i+i+1] ) : 0;
}
//=====================================================================
inline int
CConductor::GetLinkSize()
{
  return m_Links.GetSize();
}
//=====================================================================
inline CTabIdent
CConductor::GetTable( int i )
{
  return i >= 0 && i < GetLinkSize() ? m_Links[i] : 0;
}
//=====================================================================
inline CTabIdent
CConductor::GetTableDel1()
{
  return m_TabIdDel1;
}
//=====================================================================
inline CTabIdent
CConductor::GetFindId()
{
  return m_FindId;
}
//=====================================================================
inline CTabIdent
CConductor::GetTableDel2()
{
  return m_TabIdDel2;
}
//=====================================================================
inline void
CConductor::AddLink( CTabIdent& id )
{
  m_Links.Add( id );
}
//=====================================================================
inline void
CConductor::InsLink( CTabIdent& id )
{
  m_Links.InsertAt( 0, id );
}
//=====================================================================
inline void
CConductor::ClearParams()
{
  m_Params.RemoveAll();
  m_Links.RemoveAll();
}
//=====================================================================
inline int
CConductor::GetEvent()
{
  return m_LastEvent;
}
//=====================================================================
inline CDrawObject*
CConductor::GetObject()
{
  return m_pObj;
}
//=====================================================================
inline void
CConductor::GetClass( ClassType& nClass )
{
  nClass = m_Ident.Class();
}
//=====================================================================
inline ClassType
CConductor::GetClass()
{
  return m_Ident.Class();
}
//=====================================================================
inline int
CConductor::SetCurrentNode( int Node )
{
  int tmp = CurrentNode;
  CurrentNode = Node;
  return tmp;
}
//=====================================================================
inline void
CConductor::SetCurrentNodeList( int nNodes, long* pNodeList )
{
  delete m_pCurrentNodeList;
  m_nNodes = nNodes;
  m_pCurrentNodeList = new long[nNodes];
  memcpy( m_pCurrentNodeList, pNodeList, nNodes * sizeof(long));
}
//=====================================================================
inline int
CConductor::GetNode()
{
#ifdef _FULL_PROJECT
  return CurrentNode;
#else
  return 0;
#endif
}
//=====================================================================
inline int
CConductor::GetObjectNode()
{
  return m_ObjectNode;
}
//=====================================================================
inline int
CConductor::SetObjectNode( int n )
{
  int tmp=m_ObjectNode;
  m_ObjectNode = n;
  return tmp;
}
//=====================================================================
inline void
CConductor::SetFindMode(int mode)
{
  m_FindMode=mode;
}
//=====================================================================
inline int
CConductor::GetFindMode()
{
  return m_FindMode;
}
//=====================================================================
inline int
CConductor::SetCurrentProjectName(CString name)
{
#ifdef _FULL_PROJECT
  CString tmp=GetProjectStartPath() + "MapProjects\\" + name + "\\project.ini" ;

  if( !FileExist( tmp ))
  {
    AfxMessageBox(tmp);
    return -1;
  }

  CurrProjectName=name;
  return 0;

#else

  CurrProjectName="?";
  return 0;

#endif
}

inline CString
CConductor::GetCurrentProjectName()
{
#ifdef _FULL_PROJECT
  return CurrProjectName;
#else
  return "?";
#endif
}
//=====================================================================
inline int
CConductor::SetCurrentMapNum(long MapNum)
{
#ifdef _FULL_PROJECT
  CurrMapNum=MapNum;
  return 0;
#else
  CurrMapNum=-1;
  return 0;
#endif
}

inline long
CConductor::GetCurrentMapNum()
{
#ifdef _FULL_PROJECT
  return CurrMapNum;
#else
  return -1;
#endif
}
//=====================================================================
inline int
CConductor::SetCurrentSubbmpPath(CString name)
{
#ifdef _FULL_PROJECT

  name = ( name == "?" ) ?  "" : name;

  m_CurrSubbmpPath=name;

  return !name.IsEmpty();

#else

  return 0;

#endif
}

inline CString
CConductor::GetCurrentSubbmpPath()
{
#ifdef _FULL_PROJECT

    return m_CurrSubbmpPath;

#else

    return "";

#endif
}
//=====================================================================
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
inline int
CConductor::SetCurrentPublicStaticSubbmpPath(CString name)
{
#ifdef _FULL_PROJECT

  name = ( name == "?" ) ?  "" : name;

  m_PublicStaticSubbmpPath=name;

  return !name.IsEmpty();

#else

  return 0;

#endif
}

inline CString
CConductor::GetCurrentPublicStaticSubbmpPath()
{
#ifdef _FULL_PROJECT

    return m_PublicStaticSubbmpPath;

#else

  return "";

#endif
}

inline int
CConductor::SetCurrentUserStaticSubbmpPath(CString name)
{
#ifdef _FULL_PROJECT

  name = ( name == "?" ) ?  "" : name;

  m_UserStaticSubbmpPath=name;

  return !name.IsEmpty();

#else

  return 0;

#endif
}

inline CString
CConductor::GetCurrentUserStaticSubbmpPath()
{
#ifdef _FULL_PROJECT

    return m_UserStaticSubbmpPath;

#else

  return "";

#endif
}
//=====================================================================
inline int
CConductor::SetCurrentMapServer(CString name)
{
#ifdef _FULL_PROJECT

  name = ( name == "?" ) ?  "" : name;

  CurrMapServer=name;

  return !name.IsEmpty();

#else

  return 0;

#endif
}

inline CString
CConductor::GetCurrentMapServer()
{
#ifdef _FULL_PROJECT

    return CurrMapServer;

#else

    return "";

#endif
}
//=====================================================================
#endif
