#ifndef __MERKATOR_H__
#define __MERKATOR_H__
//=====================================================================
#include "ptrect.h"
#include "polar.h"
#include "MathConst.h"
//=====================================================================
class MapStore;

class Merkator
{
public:
  Merkator();

  int Init( MapStore* ms );
  int InitLocal( Polar ll );
  int DeinitLocal();
  int TrueMerkator() { return m_TrueMerkator; }
  int TrueScale( int scale, Point pt );
  double LogScale( int scale, Point pt );
  int Level2Scale( int level );
  int Scale2Level( int scale );
	int GetMinScale() { return m_MinScale; }
	int GetMaxScale() { return m_MaxScale; }
  SIZE GetProjectSize() { return m_ProjectSize; }
  int Pol2Prj( Polar ll, Point& pt );
  int Prj2Pol( Point pt, Polar& ll );
  double Dist( Point p0, Point p1 );
  double Sq3( Point p0, Point p1, Point p2 );
  double ScaleFactor( Point pt );
  double GetRmj() { return m_Rmj; }
  double GetRmn() { return m_Rmn; }

  Point  Pol2Dec( Polar ll );
  Polar  Dec2Pol( Point pt );
  Point  GetOffset() { return m_C0; }
	int    GetSmInUnit() { return m_SmInUnit; }

  Point  Prj2Merk( Point pt, int bMetr );
  void   Prj2Merk( Point pt, FPoint& fp );
  void   Merk2Prj( Point& pt, FPoint fp );
  Point  Merk2Prj( Point pt, int bMetr );
  void   CalcMerkator();

  int IsProjectInRect( int x0, int y0, int x1, int y1, int scale );

  int GK2Pol( Point pt, Point p0, double k0, double e, double l0, Polar& pol );
	int CalcScale( double ScaleFactor );

private:
  int    m_TrueMerkator;
  double m_Rmj;
  double m_Rmn;
  double m_Ex;
  double m_P1;
  double m_S1;
  double MW;
  double MH;

  Polar m_L0;
  Point m_C0;
  Point m_C1;
  SIZE  m_ProjectSize;
  int   m_SmInUnit;
  int   m_MinScale;
  int   m_MaxScale;
};

extern Merkator MerkatorData;
//=====================================================================
#endif //__MERKATOR_H__
