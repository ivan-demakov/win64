#ifndef __CBOX_H__
#define __CBOX_H__
//=====================================================================
class CBox: public CRect
{
public:
  CBox() : CRect( 0, 0, 0, 0 ) {}
  CBox( CPoint p ): CRect( p.x, p.y, p.x, p.y ) {}
  CBox( CRect const& r, int width = 0 );
  CBox( CRect const& r, CPoint c, int angle, int width = 0 );
  CBox( CPoint* pPnt, int nPnt, int nStyle = 0, int width = 0, int bNow = 1 );
  CBox( CPoint tp, CSize off, int angle, char const* pStr, int len,
        int number, int height, int weight, int style );
  CBox( CPoint pt, CSize sz, int stAng, int fnAng, int w = 0, int bPie = 0 );
  CBox( int ref, CPoint pt, double cf );
  CBox& operator=( CRect p )
  {
    SetRect( p.left, p.top, p.right, p.bottom );
    return *this;
  }
  CPoint TopRight() const { return CPoint( right, top ); }
  CPoint BottomLeft() const { return CPoint( left, bottom ); }
  void operator|=( CPoint const& p );
  void operator|=( CRect r );
  operator CRect() const { return *((CRect*)( this )); }
  operator LPCRECT() { return this; }
  void Reset() { SetRect( 0, 0, 0, 0 ); }
};
//=====================================================================
#endif