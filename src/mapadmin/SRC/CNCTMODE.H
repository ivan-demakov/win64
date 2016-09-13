#ifndef __CNCTMODE_H__
#define __CNCTMODE_H__
//=====================================================================
#include "appmode.h"
#include "ids.h"
#include "idg.h"
//=====================================================================
class CConnectMode : public CAppMode
{
public:
  CConnectMode() :
    CAppMode( AIDG_OBJACTIONMODE, IDC_SELECTTRACE,
              AM_FUNIMODE | AM_FLBDOWN | AM_FRBDOWN | AM_FCHAR,
              IDM_CONNECT_MODE )
  {}
  void Draw( CDC* pDC );
  int Term( int bDestroy );
  int Open();
  int Init( CMapView* pOwner );
  int OnLButtonDown( UINT &flags, CPoint &loc );
  int OnRButtonDown( UINT &flags, CPoint &loc );
  int OnChar( UINT& flags, UINT& key_code );

private:
	int  CreateUnion();
	int  CreateSubtraction();
};
//=====================================================================
#endif