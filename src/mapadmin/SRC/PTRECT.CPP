//=====================================================================
#include "stdafx.h"

#include "ptrect.h"
#include "msutil.h"
//=====================================================================
void
PtRect::Rect::AddRect( Rect const& r, int angle )
{
  if( angle == 0 )
    *this |= r;
  else
  {
    int w = r.Width();
    int h = r.Height();
    Point s( r.LeftTop());

    double a = Deg2Rad( angle );
    double si = sin( a );
    double co = cos( a );

    Point hr(  w * co, -w * si );
    Point vt( -h * si, -h * co );
    *this |= s;
    *this |= s += hr;
    *this |= s += vt;
    *this |= s -= hr;
  }
}
//=====================================================================
