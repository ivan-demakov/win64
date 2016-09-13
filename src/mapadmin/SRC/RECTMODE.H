#ifndef __RECTMODE_H__
#define __RECTMODE_H__

#include "appmode.h"

class CRectMode : public CRect
{
public:
  CRectMode()
  {
    SetRect( 0, 0, 0, 0 );
  }
protected:
  void Draw( CDC* pDC, int bCentered = 0 );
  void DrawA( CDC* pDC, int angle );
  void SetLT( CPoint point ){ SetRB( TopLeft() = point ); }
  void SetRB( CPoint point ){ BottomRight() = point; }
};

#endif