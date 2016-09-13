#include "stdafx.h"

#include <math.h>
#include "polar.h"
//=====================================================================
Point3d
Pol2Dec( Polar p0, double R )
{
  double r = R * ::cos( p0.lat );
  return Point3d( r * ::cos( p0.lon ), r * ::sin( p0.lon ), R * ::sin( p0.lat ));
}
//=====================================================================
