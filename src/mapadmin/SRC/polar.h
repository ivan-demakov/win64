#ifndef __POLAR_H__
#define __POLAR_H__
//=====================================================================
class CDrawObject;
struct Polar
{
  Create( double ltg, double ltm, double lts, double lng, double lnm, double lns );
  Polar( double lt = 0, double ln = 0 ) : lat( lt ), lon( ln ) {}   
  Polar( double ltg, double ltm, double lts, double lng, double lnm, double lns )
  {
    Create( ltg, ltm, lts, lng, lnm, lns );
  }
  Polar( char const* ltg, char const* ltm, char const* lts, 
         char const* lng, char const* lnm, char const* lns )
  {
    Create( atof( ltg ), atof( ltm ), atof( lts ),
            atof( lng ), atof( lnm ), atof( lns ));
  }
  Polar( Polar const& p ) : lat( p.lat ), lon( p.lon ) {}
  Polar( CDrawObject* pObj );
  int operator==( Polar p );
  double lat, lon; 
};
//=====================================================================
struct FPoint
{
  double fx, fy;
  FPoint( double x = 0, double y = 0 ) : fx( x ), fy( y ) {}
  FPoint( FPoint& p ) : fx( p.fx ), fy( p.fy ) {}
};
//=====================================================================
struct Point3d
{ 
  Point3d(){}
  Point3d( double vx, double vy, double vz )
  {
    x = vx;
    y = vy;
    z = vz;
  }
  double x, y, z; 
};
//=====================================================================
Point3d Pol2Dec( Polar p0, double R );
//=====================================================================
#endif //__POLAR_H__
