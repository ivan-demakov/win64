#include "stdafx.h"

#include "mercator.h"
#include "mapstore.h"
#if 0
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#endif
//=====================================================================
#define DPI 96
#define _SPHERE_
//=====================================================================
Merkator MerkatorData;
//=====================================================================
Merkator::Merkator() :
  m_TrueMerkator( 0 ),
  m_Rmj( 6378137 ),
  m_SmInUnit( 100 ),
  m_MaxScale( 0 ),
  m_MinScale( 0 )
{
#ifdef _SPHERE_
  double f = 0;
#else
  double f = 1/298.257223563;
#endif
  m_Rmn = m_Rmj * ( 1 - f );
  m_ProjectSize.cx = 0,
  m_ProjectSize.cy = 0,
  m_C0.x = 0;
  m_C0.y = 0;
  m_L0.lat = 0;
  m_L0.lon = 0;
  CalcMerkator();
}
//=====================================================================
int
Merkator::CalcScale( double ScaleFactor )
{
	return DPI / ( 254. * m_SmInUnit * 1.e-2 * ScaleFactor );
}
//=====================================================================
int
Merkator::Level2Scale( int level )
{
  return 2 * Pi * m_Rmj * DPI * m_SmInUnit * 1.e-2 / ( 256. * (1 << (level+1)) * .0254 );
}
//=====================================================================
int
Merkator::Scale2Level( int scale )
{
  int level = 0;
  while( Level2Scale( level ) > scale )
		++level;
	
	return level;
}
//=====================================================================
void
Merkator::CalcMerkator()
{
  double e = m_Rmn / m_Rmj;
  m_Ex = sqrt( 1. - e * e );
  m_S1 = ::cos( m_P1 * Deg );
  m_C1 = Pol2Dec( m_L0 );
  MW = 2 * Pi * m_Rmj;
  MH = 2 * Pi * m_Rmn;
}
//=====================================================================
struct LParDef
{
  char* entry;
  long* pValue;
  long  defVal;
};
//=====================================================================
static void
ReadPar( LParDef* lpd, int lpn, char const* iniPath )
{
  for( ; --lpn >= 0 ; ++lpd )
  {
    char buf[256], ibuf[256];
    if( ::GetPrivateProfileString( "GENERAL", lpd->entry, _itoa( lpd->defVal, ibuf, 10 ), buf, sizeof buf, iniPath ))
      *lpd->pValue = atoi( buf );
  }
}
//=====================================================================
int
Merkator::Init( MapStore* ms )
{
  TGstream dst;
  DataFileHeadRec HR;
  memset( &HR, 0, sizeof HR );
  m_TrueMerkator = 0;

  if( ms->GetProjectHeadRecord( dst ) == R_OK )
  {
    memcpy( &HR, (DataFileHeadRec*)dst.str(), sizeof HR );
    m_TrueMerkator = !_strnicmp( HR.PrjType, "MERK", sizeof HR.PrjType );
  }
  else
  {
    LParDef LPD[] =
    {
      "UnitDec",   &HR.UnitDec,  DEFUNITDEC,
      "XSize",     &HR.XSize,    DEFXSIZE,
      "YSize",     &HR.YSize,    DEFYSIZE,
    };
    ReadPar( LPD, CELEM( LPD ), ms->GetIniPath());
  }

  if( !m_TrueMerkator )
  {

    struct CParDef
    {
      char* entry;
      char* pValue;
      int   size;
    };

    CParDef CPD[] =
    {
      "Type",       HR.PrjType,    sizeof HR.PrjType,
      "Parallel_1", HR.Parallel_1, sizeof HR.Parallel_1,
      "Semimajor",  HR.Semimajor,  sizeof HR.Semimajor,
      "Semiminor",  HR.Semiminor,  sizeof HR.Semiminor,
      "XC0",        HR.XC0,        sizeof HR.XC0,
      "YC0",        HR.YC0,        sizeof HR.YC0,
      "LT0",        HR.LT0,        sizeof HR.LT0,
      "LN0",        HR.LN0,        sizeof HR.LN0
    };

    for( CParDef* pd = CPD + CELEM( CPD ) ; --pd >= CPD ; )
    {
      char buf[256];
      if( ::GetPrivateProfileString( "PROJECTION", pd->entry, "", buf, sizeof buf, ms->GetIniPath()))
        memcpy( pd->pValue, buf, pd->size );
    }
    m_TrueMerkator = !_strnicmp( HR.PrjType, "MERK", sizeof HR.PrjType );
  }

  if( m_TrueMerkator )
	{
    char tmp[17];
    tmp[16] = 0;

    strncpy( tmp, HR.Semimajor, sizeof HR.Semimajor );
    m_Rmj = atof( tmp );
    strncpy( tmp, HR.Semiminor, sizeof HR.Semiminor );
    m_Rmn = atof( tmp );

    strncpy( tmp, HR.Parallel_1, sizeof HR.Parallel_1 );
    m_P1 = atof( tmp );

    strncpy( tmp, HR.LT0, sizeof HR.LT0 );
    m_L0.lat = Deg * atof( tmp );
    strncpy( tmp, HR.LN0, sizeof HR.LN0 );
    m_L0.lon = Deg * atof( tmp );

    tmp[8] = 0;
    strncpy( tmp, HR.XC0, sizeof HR.XC0 );
    m_C0.x = atoi( tmp );
    strncpy( tmp, HR.YC0, sizeof HR.YC0 );
    m_C0.y = atoi( tmp );
	}
	else
	{
		m_Rmj = m_Rmj * 1e2 / HR.UnitDec;
		m_Rmn = m_Rmn * 1e2 / HR.UnitDec;
    m_C0.x = HR.XSize / 2;
    m_C0.y = HR.YSize / 2;
	}

  if( !HR.MinScale )
  {
    LParDef LPD[] = { "MinScale", &HR.MinScale, DEFMINSCALE };
    ReadPar( LPD, CELEM( LPD ), ms->GetIniPath());
  }

  if( !HR.MaxScale )
  {
    LParDef LPD[] = { "MaxScale", &HR.MaxScale, DEFMAXSCALE };
    ReadPar( LPD, CELEM( LPD ), ms->GetIniPath());
  }

  m_SmInUnit       = HR.UnitDec;
  m_MinScale       = HR.MinScale;
  m_MaxScale       = HR.MaxScale;
  m_ProjectSize.cx = HR.XSize;
  m_ProjectSize.cy = HR.YSize;

  CalcMerkator();

  return m_TrueMerkator;
}
//=====================================================================
int
Merkator::TrueScale( int scale, Point pt )
{
  return scale / ScaleFactor( pt ) + .5;
}
//=====================================================================
double
Merkator::LogScale( int scale, Point pt )
{
  return scale * ScaleFactor( pt );
}
//=====================================================================
int
Merkator::InitLocal( Polar ll )
{
  ASSERT( !m_TrueMerkator );
  if( m_TrueMerkator )
    return 0;
  m_TrueMerkator = 1;
  m_S1 = ll.lat;
  return 1;
}
//=====================================================================
int
Merkator::DeinitLocal()
{
  ASSERT( m_TrueMerkator );
  if( !m_TrueMerkator )
    return 0;
  m_TrueMerkator = 0;
  return 1;
}
//=====================================================================
Polar
Merkator::Dec2Pol( Point pt )
{
  Polar ll;
  ll.lon = pt.x / m_Rmj;
  double t0 = 2 * atan( exp( pt.y / m_Rmn )) - .5 * Pi;
  int bNeg = pt.y < 0;

  if( m_Ex )
  {
    pt.y = abs( pt.y );
    double ey = exp( 2 * pt.y / m_Rmn );

    for( int n = 0 ; n < 1000 ; ++n )
    {
      double s = ::sin( t0 );
      double arg = 1 - ( 1 + s ) * pow( 1 - m_Ex * s, m_Ex ) / pow( 1 + m_Ex * s, m_Ex ) / ey ;
      double t1 = asin( arg );
      double e = fabs( t1 - t0 );
      t0 = t1;
      if( e < 1e-7 )
        break;
    }
  }

  ll.lat = bNeg ? -t0 : t0;
  return ll;
}
//=====================================================================
Point
Merkator::Pol2Dec( Polar ll )
{
  double esl = m_Ex * ::sin( ll.lat );
  Point pt;
  pt.x = m_Rmj * ll.lon;
  pt.y = m_Rmj * log( tan( Pi/4. + ll.lat/2. ) * pow(( 1. - esl ) / ( 1. + esl ), m_Ex/2. ));
  return pt;
}
//=====================================================================
int
Merkator::GK2Pol( Point pt, Point p0, double k0, double e, double l0, Polar& pol )
{
  if( !m_TrueMerkator )
    return 0;

  double x = pt.x - p0.x;
  double y = pt.y - p0.y;

  double a = m_Rmj;
  double e2 = e * e;

  double t = sqrt( 1 - e2 );
  double e11 = ( 1. - t ) / ( 1. + t );
  double e12 = e11 * e11;
  double const m2 = 1./4, m4 = 3./64, m6 = 5./256;
  double mu = y / k0 / a / ( 1 - e2 * ( m2 + e2 * ( m4 + e2 * m6 )));

  double const m[][4] = {{ 3./2, 0, -27./32, 0 },
                         { 0, 21./16, 0, -55./32 },
                         { 0, 0, 151./96, 0 },
                         { 0, 0, 0, 1097./512 }};

  double fi1 = mu;
  for( int i = 0 ; i < 4 ; ++i )
  {
    double s = 0;
    for( int j = 4 ; --j >= 0 ; s = s * e11 + m[i][j] );
    fi1 += ::sin(( i + 1 ) * 2 * mu ) * s * e11;
  }

  double cofi1 = ::cos( fi1 );
  double sifi1 = ::sin( fi1 );
  double tafi1 = sifi1 / cofi1;

  double es1 = 1. - e2;
  double es2 = e2 / es1;
  double C = es2 * cofi1 * cofi1, CC = C * C;
  double T = tafi1 * tafi1, TT = T * T;
  t = 1. - e2 * sifi1 * sifi1;
  double N1 = a / sqrt( t );
  double R1 = N1 * es1 / t;
  double D = x / N1 / k0, DD = D * D;

  double z0 = .5;
  double z1 = - ( 5. + 3. * T + 10. * C - 4. * CC - 9. * es2 ) / 24.;
  double z2 = ( 61. + 90. * T + 298. * C + 45. * TT - 252. * es2 - 3. * CC ) / 720.;

  double u0 = 1.;
  double u1 = - ( 1. + 2.*T + C ) / 6.;
  double u2 = ( 5. - 2. * C + 28. * T - 3 * CC + 8. * es2 + 24. * TT ) / 120.;

  pol.lat = fi1 - N1 * tafi1 / R1 *  DD * ( z0 + DD * ( z1 + DD * z2 ));
  pol.lon = l0 + D  * ( u0 + DD * ( u1 + DD * u2 )) / cofi1;

#ifdef _DEBUG
  double lat = pol.lat / Deg;
  double lon = pol.lon / Deg;
#endif // _DEBUG

  return 1;
}
//=====================================================================
int
Merkator::Pol2Prj( Polar ll, Point& pt )
{
  if( !m_TrueMerkator )
    return 0;

  if( ll.lat > 89.5 * Deg || ll.lat < -89.5 * Deg )
    return -1;

  pt = Pol2Dec( ll );
  pt = Merk2Prj( pt, 0 );
  return 1;
}
//=====================================================================
Point
Merkator::Prj2Merk( Point pt, int bMetr )
{
  pt.x = ( pt.x - m_C0.x ) / m_S1 + m_C1.x;
  pt.y = ( pt.y - m_C0.y ) /-m_S1 + m_C1.y;
  if( bMetr )
  {
    pt.x = pt.x * 1e-2 * m_SmInUnit;
    pt.y = pt.y * 1e-2 * m_SmInUnit;
  }
  return pt;
}
//=====================================================================
Point
Merkator::Merk2Prj( Point pt, int bMetr )
{
  if( bMetr )
  {
    pt.x = pt.x * 1e2 / m_SmInUnit;
    pt.y = pt.y * 1e2 / m_SmInUnit;
  }
  pt.x = ( pt.x - m_C1.x ) *  m_S1 + m_C0.x;
  pt.y = ( pt.y - m_C1.y ) * -m_S1 + m_C0.y;
  return pt;
}
//=====================================================================
void
Merkator::Merk2Prj( Point& pt, FPoint fp )
{
  pt.x = fp.fx * 1e2 / m_SmInUnit;
  pt.y = fp.fy * 1e2 / m_SmInUnit;
  pt = Merk2Prj( pt, 0 );
}
//=====================================================================
void
Merkator::Prj2Merk( Point pt, FPoint& fp )
{
  pt = Prj2Merk( pt, 0 );
  fp.fx = pt.x * 1e-2 * m_SmInUnit;
  fp.fy = pt.y * 1e-2 * m_SmInUnit;
}
//=====================================================================
int
Merkator::Prj2Pol( Point pt, Polar& ll )
{
  if( !m_TrueMerkator )
    return 0;

  pt = Prj2Merk( pt, 0 );
  ll = Dec2Pol( pt );
  return 1;
}
//=====================================================================
double
Merkator::ScaleFactor( Point pt )
{
  double sf = 1;
  Polar ll;
  if( Prj2Pol( pt, ll ))
  {
    double ecf = ::sin( ll.lat ) * m_Ex;
    sf = m_S1 * sqrt( 1 - ecf * ecf ) / ::cos( ll.lat );
  }
  return fabs( sf );
}
//=====================================================================
double
Merkator::Dist( Point pt0, Point pt1 )
{
  double dx = pt0.x - pt1.x;
  double dy = pt0.y - pt1.y;
  double ds = sqrt( dx * dx + dy * dy );
  if( !m_TrueMerkator )
    return ds;

  Polar ll0, ll1;
  Prj2Pol( pt0, ll0 );
  Prj2Pol( pt1, ll1 );
  double cf = ::cos(( ll0.lat + ll1.lat ) * .5 );

  Point t;
  t.x = ( pt0.x + pt1.x ) / 2;
  t.y = ( pt0.y + pt1.y ) / 2;
  double cf1 = ScaleFactor( t );

  Point3d p0 = ::Pol2Dec( ll0, m_Rmj );
  Point3d p1 = ::Pol2Dec( ll1, m_Rmj );
  double ang = ::acos(( p0.x * p1.x + p0.y * p1.y + p0.z * p1.z ) / m_Rmj / m_Rmj );
  double dr = m_Rmj * ang;
  ds /= cf1;
  return ang < Min ? ds : dr;
}
//=====================================================================
double
Merkator::Sq3( Point pt0, Point pt1, Point pt2 )
{
  double dx1 = pt2.x - pt0.x, dy1 = pt2.y - pt0.y;
  double dx2 = pt1.x - pt0.x, dy2 = pt1.y - pt0.y;
  double ds = .5 * ( dy1 * dx2 - dy2 * dx1 );

  if( !m_TrueMerkator )
    return ds;

  Polar ll0, ll1, ll2;
  Prj2Pol( pt0, ll0 );
  Prj2Pol( pt1, ll1 );
  Prj2Pol( pt2, ll2 );

  double cf = ::cos(( ll0.lat + ll1.lat + ll2.lat ) / 3. );
  double k = m_S1 / cf;
  ds /= k * k;

  double RR = m_Rmj * m_Rmj;

  Point3d p0( ::Pol2Dec( ll0, m_Rmj ));
  Point3d p1( ::Pol2Dec( ll1, m_Rmj ));
  Point3d p2( ::Pol2Dec( ll2, m_Rmj ));

  double a0 = ::acos(( p0.x * p1.x + p0.y * p1.y + p0.z * p1.z ) / RR );
  double a1 = ::acos(( p1.x * p2.x + p1.y * p2.y + p1.z * p2.z ) / RR );
  double a2 = ::acos(( p2.x * p0.x + p2.y * p0.y + p2.z * p0.z ) / RR );
  double ma = max( a0, max( a1, a2 ));
  double s = .5* ( a0 + a1 + a2 );
  double ss = ::sin( s );
  double s0 = ::sin( a0 );
  double s1 = ::sin( a1 );
  double s2 = ::sin( a2 );

  double A0 = 2. * ::acos( sqrt( ss * ::sin( s - a0 ) / s1 / s2 ));
  double A1 = 2. * ::acos( sqrt( ss * ::sin( s - a1 ) / s2 / s0 ));
  double A2 = 2. * ::acos( sqrt( ss * ::sin( s - a2 ) / s0 / s1 ));
  double ex = A0 + A1 + A2 - Pi;
  double rs = RR * ex * ( ds >= 0 ? 1 : -1. );
  return ma < Min ? ds : rs;
}
//=====================================================================
//=====================================================================
int
Merkator::IsProjectInRect( int x0, int y0, int x1, int y1, int scale )
{
  if( scale < m_MaxScale || scale > m_MinScale )
    return 0;

  Point p0( Merk2Prj( Point( x0, y0 ), 0 ));
  Point p1( Merk2Prj( Point( x1, y1 ), 0 ));

  Rect prjClip( p0, p1 );
  Rect prjRect( 0, 0, m_ProjectSize.cx, m_ProjectSize.cy );

  return prjRect.IntersectRect( prjClip );
}
//=====================================================================
