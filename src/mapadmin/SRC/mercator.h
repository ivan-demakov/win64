#ifndef __MERKATOR_H__
#define __MERKATOR_H__
//=====================================================================
#include "ptrect.h"
#include "polar.h"
#include "MathConst.h"
//=====================================================================
class MapStore;

enum { MIN_TILE_LEVEL = 4 };

class Merkator
{
public:
  Merkator();

  int Init( MapStore* ms );
  int InitLocal( Polar ll );
  int DeinitLocal();
  int TrueMerkator() { return m_TrueMerkator; }
  int TrueScale( int scale, POINT pt );
  double LogScale( int scale, POINT pt );
  int Level2Scale( int level );
  int Scale2Level( int scale );
  int GetMinScale() { return m_MinScale; }
  int GetMaxScale() { return m_MaxScale; }
  SIZE GetProjectSize() { return m_ProjectSize; }
  int Pol2Prj( Polar ll, POINT& pt );
  int Prj2Pol( POINT pt, Polar& ll );
  double Dist( POINT p0, POINT p1 );
  double Sq3( POINT p0, POINT p1, POINT p2 );
  double ScaleFactor( POINT pt );
  double GetRmj() { return m_Rmj; }
  double GetRmn() { return m_Rmn; }

  POINT  Pol2Dec( Polar ll );
  Polar  Dec2Pol( POINT pt );
  POINT  GetOffset() { return m_C0; }
  int    GetSmInUnit() { return m_SmInUnit; }

  POINT  Prj2Merk( POINT pt, int bMetr );
  void   Prj2Merk( POINT pt, FPoint& fp );
  void   Merk2Prj( POINT& pt, FPoint fp );
  POINT  Merk2Prj( POINT pt, int bMetr );
  void   CalcMerkator();

  int IsProjectInRect( int x0, int y0, int x1, int y1, int scale );

  int GK2Pol( POINT pt, POINT p0, double k0, double e, double l0, Polar& pol );
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
  POINT m_C0;
  POINT m_C1;
  SIZE  m_ProjectSize;
  int   m_SmInUnit;
  int   m_MinScale;
  int   m_MaxScale;
};

extern Merkator MerkatorData;
//=====================================================================
#endif //__MERKATOR_H__
