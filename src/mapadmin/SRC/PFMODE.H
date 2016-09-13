#ifndef __PFMODE_H__
#define __PFMODE_H__
//=====================================================================
#include "rectmode.h"
#include "ids.h"
#include "idg.h"
//=====================================================================
class CPrintMapMode : public CRectMode, public CAppMode
{
public:
  CPrintMapMode( int bPreview ) :
    CAppMode( AIDG_PRINTFRAGMENT, IDC_PRINTFRAGMENT,
              AM_FESCMODE | AM_FUNIMODE | AM_FMONOPOLY | AM_FMOUSEMOVE |
              AM_FLBDOWN | AM_FRBDOWN, IDM_PRINT_MAP_MODE ),
    m_bPreview( bPreview )
    {}
  void Draw( CDC* pDC );
  int  Term( int bDestroy );
  int  Open();
  int  Init( CMapView *owner );
  int  OnLButtonDown( UINT &flags, CPoint &loc );
  int  OnLButtonUp( UINT &flags, CPoint &loc );
  int  OnRButtonDown( UINT &flags, CPoint &loc );
  int  OnMouseMove( UINT &flags, CPoint &loc );
  int  IsPreview() { return m_bPreview; }
  int  TestLButtonDown() { return 2; }

private:
  void ExpandSheet();

private:
  int   m_bPreview;

public:
  static int m_bAutoSize;
};
//=====================================================================
#endif
