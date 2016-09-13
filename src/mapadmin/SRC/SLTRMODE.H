#ifndef __SLTRMODE_H__
#define __SLTRMODE_H__
//=====================================================================
#include "appmode.h"
#include "ident.h"
#include "ids.h"
#include "tpdef.h"
#include "drawobj.h"
#include "ksi.h"
//=====================================================================
enum ST_MODE
{
  STM_VIEW = 1,
  STM_EDIT = 2,
  STM_CALC = 3
};
//=====================================================================
class CDrawObject;
//=====================================================================
struct TraceObject
{
  CTabIdent m_ObjId;
  CTabIdent m_ObjCId;
  CString   m_ObjType;
  CString   m_ObjName;
  CString   m_ObjAddr;
  CString   m_ObjShape;
  CPoint    m_ObjPoint;
  int       m_ObjSelType;

  TraceObject( CTabIdent id, int selType, CString shape, CPoint point ) :
    m_ObjId( id ),
    m_ObjPoint( point ),
    m_ObjSelType( selType ),
    m_ObjShape( shape ) {}

  TraceObject( CTabIdent id, CTabIdent cid, 
               char const* typeName = 0, char const* objName = 0, 
               char const* objAddr = 0, int selType = 1 ) :
    m_ObjId( id ), m_ObjCId( cid ), m_ObjPoint( -1, -1 ),
    m_ObjType( typeName ? typeName : "" ),
    m_ObjName( objName ? objName : "" ), 
    m_ObjAddr( objAddr ? objAddr : "" ), 
    m_ObjSelType( selType ) {}


  TraceObject( TableType tabId, EntryType objId, 
               TableType ctabId, EntryType cobjId, 
               char const* typeName = 0, char const* objName = 0, 
               char const* objAddr = 0, int selType = 1 ) :
    m_ObjId( tabId, objId ), m_ObjCId( ctabId, cobjId ), m_ObjPoint( -1, -1 ),
    m_ObjType( typeName ? typeName : "" ),
    m_ObjName( objName ? objName : "" ), 
    m_ObjAddr( objAddr ? objAddr : "" ), 
    m_ObjSelType( selType ) {}

};
//=====================================================================
typedef CArray<TraceObject*,TraceObject*> TraceArray;
//=====================================================================
class CSelectTraceMode : public CAppMode
{
public:
  CSelectTraceMode( ST_MODE mode = STM_VIEW ) :
    CAppMode( AIDG_SELECTTRACE, IDC_SELECTTRACE, AM_FUNIMODE | AM_FMONOPOLY, IDM_EDITTRACE_MODE )
  {
    m_nMode = mode;
  }
  int  Term( int bDestroy );
  int  Open();
  int  Close();
  void Reopen();
  void InsertSelect( CDrawObject* pObj, int selType );
  int  Expand( CDrawObject* pObj );
  int  OnLButtonDown( UINT &flags, CPoint &loc );
  int  OnLButtonDblClk( UINT &flags, CPoint &loc ) { return AM_RDONE; }
  int  OnRButtonDown( UINT &flags, CPoint &loc );
  int  OnChar( UINT& flags, UINT& key_code );
  int  SwapObj( CDrawObject* pObj );
  int  AskForClose( ST_MODE m );

public:
  static void Purge();
  static void SetSelectType( int type );
  static void Add( CTabIdent id, CTabIdent cobjid, 
                   char const* pTypName = "",
                   char const* pObjName = "",
                   char const* pObjAdr = "",
                   int selType = 1 );
  static void Add( TableType tabId, EntryType objId, 
                   TableType cobj_tabId, EntryType cobj_objId, 
                   char const* pTypName = "",
                   char const* pObjName = "",
                   char const* pObjAdr = "", 
                   int selType = 1 );
  static void Rem( int ndx ); 
  static void Rem( CTabIdent id ); 
  static void SetTraceName( char const* name );
  static int  GetTraceSize(){ return m_TmpTraceSize; }
  static int  SetTraceSize( int sz );
  static LPCSTR GetTraceName() { return m_TraceName; }
  static void SetTraceIdent( CTabIdent id ) { m_TraceIdent = id; }
  static CTabIdent GetTraceIdent() { return m_TraceIdent; }
  static CTabIdent GetTraceData( int i );
  static CString GetTraceCoord( int i );
  static void SetTraceType( UINT type );
  static UINT GetTraceType();
  static int InTrace( CTabIdent id );
  static int InTrace( CDrawObject* pObj ) { return InTrace( pObj->TabIdent()); }
  static void ReselectAll();

  static void SetTraceDBName( char const* name );
  static CString GetTraceDBName();
  static CString GetCoordList( CDrawObject* pObj );
  static CString GetMifCoordList( CDrawObject* pObj );
  static CString GetCoordList( CTabIdent id );
  static CString GetCoordList( TableType table, EntryType entry )
  { return GetCoordList( CTabIdent( table, entry )); }

private:
  int  IsTraceClass( int t );
  void CalcLen();

private:
  CIntArray m_TraceObjClass;

public:
  static long      m_bNoInstructions;

private:
  static int       m_SelectType;
  static int       m_TmpTraceSize;

  static CString   m_TraceName;
  static CString   m_TraceDBName;
  static UINT      m_TraceType; 
  static CTabIdent m_TraceIdent;

  static TraceArray m_Trace;
};
//=====================================================================
inline int  
CSelectTraceMode::SetTraceSize( int n )
{ 
  m_Trace.SetSize( n, n/10 ); 
}
//=====================================================================
inline void
CSelectTraceMode::SetSelectType( int type )
{
  m_SelectType = type;
}
//=====================================================================
inline void
CSelectTraceMode::Add( TableType tabId, EntryType objId, TableType cobj_tab,EntryType cobj_id,const char *pTypName, char const* pObjName, char const* pObjAdr, int selType )
{
  Add( CTabIdent( tabId, objId ),CTabIdent( cobj_tab, cobj_id ), pTypName, pObjName, pObjAdr, selType );
}
//=====================================================================
inline void
CSelectTraceMode::SetTraceName( char const* name )
{
  m_TraceName = name;
}
//=====================================================================
inline void
CSelectTraceMode::SetTraceType( UINT type )
{
  m_TraceType = type;
}
//=====================================================================
inline UINT
CSelectTraceMode::GetTraceType()
{
  return m_TraceType;
}
//=====================================================================
inline void
CSelectTraceMode::SetTraceDBName( char const* name )
{
  m_TraceDBName = name;
}
//=====================================================================
inline CString
CSelectTraceMode::GetTraceDBName()
{
 return m_TraceDBName;
}
//=====================================================================
//=====================================================================
class CShortTraceMode : public CAppMode
{
public:  
  CShortTraceMode() :
    CAppMode( AIDG_OBJACTIONMODE, IDC_SELECTTRACE,
              AM_FUNIMODE | AM_FLBDOWN | AM_FRBDOWN | AM_FCHAR,
              IDM_SHORTTRACE_MODE )
  {}
public:
	void DeclareEndSelection();
  int  ExtSelect( CDrawObject* Obj );
  int  TstSelect( CDrawObject* Obj );
protected:
  void Draw( CDC* pDC );
  int  Term( int bDestroy );
  int  Open();
  int  Init( CMapView* pOwner );
  int  OnLButtonDown( UINT &flags, CPoint &loc );
  int  OnRButtonDown( UINT &flags, CPoint &loc );
  int  OnChar( UINT& flags, UINT& key_code );
};
//=====================================================================
#endif