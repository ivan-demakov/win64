//=====================================================================
// файл "rgnspace.h"
//=====================================================================
#ifndef _RGNSPCT_H_
#define _RGNSPCT_H_
//=====================================================================
class RegionDef;
//=====================================================================
class RgnPool : public CRect
{
public:
  RgnPool();
  RgnPool& operator=( RgnPool& p )
  {
    PoolSize = p.PoolSize;
    pPool = p.pPool;
    return *this;
  }
  void AddRgn( RegionDef* pObj );
  void AddRgn( CPoint* pPnt );
  RegionDef* GetHead() { return pPool; }
  RegionDef** GetHeadPtr() { return &pPool; }
  RegionDef* RemoveHead();
  RegionDef* GetNext();
  RegionDef* Connect();
  void GlueRgn();
  int Size() { return PoolSize; }
  void DecSize() { --PoolSize; }

private:
  int     PoolSize; // размер списка
  RegionDef* pPool; // голова списка
};
//=====================================================================
// элемент списка
class RegionDef : public CRect
{
private:
  int Split( CPoint c, RgnPool& rst, int bHorz );

public:
  ~RegionDef();
  RegionDef( CPoint* pPoint );
  RegionDef( CPoint* pPoint, CRect& box, int cw );
  RegionDef( RegionDef& pr );
  int IsCrossedByLine( CPoint p0, CPoint p1 );
  int SubtractCrossed( RegionDef* pr1, RgnPool& Rst );
  int SplitByInner( RgnPool& Inner, RgnPool& rst );
  int Split( RgnPool& rst );
  CPoint FindInnerPoint();
	CPoint FindCross( CPoint c, int bHorz, int bFar, int sideMask, int& nPoint );
	void SubtractInner( RgnPool& Inner );

  CPoint* Close();

  CPoint*    pPoly;
  int        nCw;
  RegionDef* pNext;
  CPoint     InnerPoint;
};
//=====================================================================
#endif
