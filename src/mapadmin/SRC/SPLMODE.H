#ifndef __SPLMODE_H__
#define __SPLMODE_H__
//=====================================================================
#include "rectmode.h"
#include "miscmode.h"
#include "ids.h"
#include "idg.h"
//=====================================================================
class CStreetPlanMode : public CRectMode, public CAppMode
{
public:
  CStreetPlanMode() :
    CAppMode( AIDG_STREETPLAN, IDC_PRINTFRAGMENT,
              AM_FESCMODE | AM_FUNIMODE | AM_FMONOPOLY | AM_FMOUSEMOVE |
              AM_FLBDOWN | AM_FRBDOWN, IDM_STREETPLAN_MODE )
    { CReisMode::SetActive( 0, 0 ); }
  int OnLButtonDown( UINT &flags, CPoint &loc );
  int OnRButtonDown( UINT &flags, CPoint &loc );
  int OnMouseMove( UINT &flags, CPoint &loc );
  int Open();
  int Term( int bDestroy );
  int TestLButtonDown() { return 2; } 
  void Draw( CDC* pDC );
  int SavePlan( int R );
  static ksi_obj MakeKsiCont( CDrawContainer* pObj, int bHR, int bGrayScale,
                              CPoint* pRgn, int nRgn, CRect& box );
  static int SavePlan( CPoint* pRgn, int nRgn );
  static void OutObj( CDrawObject* pObj, int sc, ksi_obj ksi_port, 
                      CPoint* pRgn, int nRgn, CRect& box );
};
//=====================================================================
#endif