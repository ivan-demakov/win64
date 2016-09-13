#ifndef __SPINMODE_H__
#define __SPINMODE_H__
//=====================================================================
#include "appmode.h"
#include "drawprim.h"
//=====================================================================
class CSpinMode : public CAppMode
{
public:
  CSpinMode( CPoint startPoint );
  int  Open();
  int  OnRButtonUp( UINT &flags, CPoint &loc );
  int  OnMouseMove( UINT &flags, CPoint &loc );
  int  OnChar( UINT& flags, UINT& key_code );
  int TestLButtonDown() { return 0; } 
  void Draw( CDC* pDC );
  void CopySpec();
  void SaveSpec();

private:
  void ShowAngle();

private:
  CBox   m_Box; 
  CPoint m_Center;
  int    m_StartAngle;
  int    m_Angle;

  CDrawObject* m_pBlinking;
  CObjList m_TmpList;
};
//=====================================================================
#endif

