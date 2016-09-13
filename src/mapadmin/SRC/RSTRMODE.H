#ifndef __RESTOREMODE_H__
#define __RESTOREMODE_H__
//=====================================================================
#include "appmode.h"
#include "ident.h"
//=====================================================================
class CRestoreMode : public CAppMode
{
public:
  CRestoreMode( CTabIdent id );
  int Init( CMapView* pOwner );
  int OnChar( UINT& flags, UINT& key_code );
  int OnRButtonDown( UINT& flags, CPoint& loc ) { return ExitMode(); }
  int Term( int bDestroy );

private:
  int ShowObject( CDrawObject* pObj, int bDeleted );
	int ExitMode();

private:
  int          m_bLeft;
  CTabIdent    m_Id;
  CDrawObject* m_pObj;
  CDrawObject* m_pCurObj;
  CObjList     m_ObjList;
  POSITION     m_CurPos;

public:
  static long m_bNoInstructions;
};
//=====================================================================
#endif

