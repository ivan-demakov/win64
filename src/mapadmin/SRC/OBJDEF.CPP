#include "stdafx.h"

#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "objdef.h"
#include "util.h"
#include "ksiutil.h"
#include "ids.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
static char SectName[] = "MAP_OBJECTS";
static char CurRange[] = "CurOrder";
static char NewRange[] = "NewOrder";
static char* pnm[] =
{ "Class", "Parent", "Priority", "MinLimit", "MaxLimit", 
  "MinScale", "MaxScale", "Show", "Load", "Table", "Mute" };
static char OldRange[] = "Class Parent Priority MaxLimit Show Load Table MinScale MaxScale Mute";
//=====================================================================
static int
ParamNum( char const* pn )
{
	int i;
  for(i = CELEM( pnm ) ; --i >= 0 && _stricmp( pn, pnm[i] ) ; );
  return i;
}
//=====================================================================
CObjDef::CObjDef() :
  m_nCls( 0 ),
  m_IniPath( 0 ),
  m_bModified( 0 ), 
	m_bCollision( 0 )
{}
//=====================================================================
int
CObjDef::IsClass( UINT t ) const
{ 
  return !GetMark( t ) && m_Oda[t].Name[0] && ( GetDerived( t ) || IsObject( t ));
}
//=====================================================================
int
CObjDef::OdLt( int t0, int t1 )
{
  return t0 >= SUBSTAT_CLASS ? t1 < SUBSTAT_CLASS || t0 < t1 :
         t0 >= SUBLAYER_CLASS && t1 < SUBLAYER_CLASS ||
         GetRange( t0 ) < GetRange( t1 );
}
//=====================================================================
void
CObjDef::MakeRange()
{
  int t, ts = 0, ct[FULL_RANGE];

  for( t = FULL_RANGE ; -- t > 0 ; )
    if( m_Oda[t].Name[0] )
    {
      int i, p = GetRange( t );
      for( i = ts++ ; --i >= 0 && OdLt(  t, ct[i] ) ; ct[i+1] = ct[i] );
      int q = i >= 0 ? GetRange( ct[i] ) : 0;
      m_bCollision |= q && q == p;
      ct[i+1] = t;
    }

  while( --ts >= 0 )
  {
    int t = ct[ts];
    int od = m_Oda[t].Range;
    m_Oda[t].Range = ts;
    m_bModified |= od != ts;
  }

  m_nCls = 0;
  for( t = 0 ; t < FULL_RANGE ; ++t )
    if( IsClass( t ) && !GetDerived( t ))
		{
		  int i, r = GetRange( t );
	      for( i = m_nCls ; --i >= 0 && GetRange( m_Rng[i] ) > r ; m_Rng[i+1] = m_Rng[i] );
			m_Rng[i+1] = t;
			++m_nCls;
		}
	m_Rng[m_nCls] = 0;
}
//=====================================================================
int
CObjDef::Init( char const* iniPath )
{
  m_IniPath = iniPath;
	 
	int i = FULL_RANGE;
  while( --i >= 0 )
  {
    OD* pOd = m_Oda + i;
    pOd->Name[0] = 0;
    pOd->Mark = -1;
  }
  
  char entrs[16384];
  
	GetPrivateProfileString( SectName, 0, "", entrs, sizeof entrs, iniPath );
	int n = 0;
  for( char* pe = entrs ; *pe ; pe += strlen( pe ) + 1, ++n );

  if( n == 0 )
    return 0;

  char buf[512];
  GetPrivateProfileString( SectName, CurRange, OldRange, buf, sizeof buf, iniPath );

  int kd[CELEM(pnm)];
  int nk = 0;
  char* pTok; 

  for( pTok = strtok( buf, " " ) ; pTok && nk < CELEM( pnm ) ; pTok = strtok( 0, " " ))
    if(( kd[nk++] = ParamNum( pTok )) < 0 )
      return 0;

  if( kd[0] != 0 || kd[1] != 1 || kd[2] != 2 )
    return 0;

  for( char* name = entrs ; *name ; name += strlen( name ) + 1 )
  {
    if( !_stricmp( name, CurRange ) ||
        !_stricmp( name, NewRange ))
      continue;

    int k = 0;
    int par[] = { 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 0 };
    GetPrivateProfileString( SectName, name, "", buf, sizeof buf, iniPath );
    for( char* pTok = strtok( buf, " " ) ; pTok && k < nk ; pTok = strtok( 0, " " ))
      par[kd[k++]] = atoi( pTok );
    Add( name, par );
		par[11] += 1;
  }

  m_bModified = 0;

  for( int t = FULL_RANGE ; --t > 0 ; )
    if( TestLoop( t ))
      return 0;

  MakeRange();

#ifndef _FULL_PROJECT
  int bvm = 0;
  GetPrivateProfileString( SectName, NewRange, "", buf, sizeof buf, iniPath );
  int k = 0;
  for( pTok = strtok( buf, " " ) ; !bvm && pTok && k < CELEM( pnm ) ; pTok = strtok( 0, " " ))
    bvm = k > nk || ParamNum( pTok ) != kd[k++];
  m_bModified |= bvm;
#endif

  return 1;
}
//=====================================================================
int 
CObjDef::LikeParent( UINT t ) const
{
  if( !IsClassValid( t ))
    return 0;

  int p = GetParent( t );
  return p && 
         GetMinLimit( t )      == GetMinLimit( p )      && 
         GetMaxLimit( t )      == GetMaxLimit( p )      && 
         GetMinScale( t )      == GetMinScale( p )      && 
         GetMaxScale( t )      == GetMaxScale( p )      && 
         GetEdit( t )          == GetEdit( p )          &&
         GetLoad( t )          == GetLoad( p )          &&
         GetTable( t )         == GetTable( p )         &&
         GetTextInvisible( t ) == GetTextInvisible( p );
}
//=====================================================================
int 
CObjDef::Save() const
{
	if( !m_IniPath )
		return 0;

  char buf[512];
  GetPrivateProfileString( SectName, NewRange, "", buf, sizeof buf, m_IniPath );
  int bHasNew = buf[0] != 0;
  if( !bHasNew )
    GetPrivateProfileString( SectName, CurRange, OldRange, buf, sizeof buf, m_IniPath );

  int kd[CELEM(pnm)];
  int nk = 0;
  char* pTok; 

  char tmp[512];
  memcpy( tmp, buf, sizeof buf );
  for( pTok = strtok( tmp, " " ) ; pTok && nk < CELEM( pnm ) ; pTok = strtok( 0, " " ))
    if(( kd[nk++] = ParamNum( pTok )) < 0 )
      return 0;

  if( kd[0] != 0 || kd[1] != 1 || kd[2] != 2 )
    return 0;

  int n;
  for( n = 0, t = 1 ; t < FULL_RANGE ; ++t )
  {
    CString const& name = m_Oda[t].Name;
    if( name.IsEmpty())
      continue;
    if( GetMark( t ) < 0 )
      WritePrivateProfileString( SectName, name, 0, m_IniPath );
    else
    {
      int par[] = 
      { 
        t, GetParent( t ), GetRange( t ), 
        GetMinLimit( t ), GetMaxLimit( t ), 
        GetMinScale( t ), GetMaxScale( t ),
        GetEdit( t ), GetLoad( t ), GetTable( t ),
        GetTextInvisible( t )
      };
      CString buf;
      if( LikeParent( t ))
        buf.Format( "%d %d %d", par[0], par[1], par[2] );
      else
      {
        for( int i = 0 ; i < nk ; ++i )
        {
          char tmp[16];
          sprintf( tmp, "%d ", par[kd[i]] );
          buf += tmp;
        }
      }
      WritePrivateProfileString( SectName, name, buf, m_IniPath );
      ++n;
    }
  }

  if( bHasNew )
  {
    WritePrivateProfileString( SectName, NewRange, 0, m_IniPath );
    WritePrivateProfileString( SectName, CurRange, buf, m_IniPath );
  }  
  
  return n;
}
//=====================================================================
int 
CObjDef::SaveModified()
{
	if( !m_IniPath )
		return 0;

  int m = m_bModified;
  m_bModified = 0;
  return m ? Save() : 0;
}
//=====================================================================
int 
CObjDef::Add( LPCSTR name, int* par )
{
  if( !IsClassValid( par[0] ) || par[1] >= FULL_RANGE || par[1] == par[0] )
    return 0;

  OD* pOd            = m_Oda + par[0];
  pOd->Parent        = par[1];
  pOd->Range         = par[2];
  pOd->MinLimit      = par[3];
  pOd->MaxLimit      = par[4];
  pOd->MinScale      = min( par[5], par[6] );
  pOd->MaxScale      = max( par[5], par[6] );
  pOd->Edit          = par[7];
  pOd->Load          = par[8];
  pOd->Table         = par[9];
  pOd->TextInvisible = par[10];
  pOd->MenuRange     = par[11]; 
  pOd->Mark          = 0;
  m_bModified        = 1;
  strncpy( pOd->Name, name, nMaxName );  
  
  return 1;
}
//=====================================================================
int 
CObjDef::TestLoop( UINT t )
{
  if( !t )
    return 0;

  int b = 1, m = GetMark( t );
  if( m != 1 )
  {
    SetMark( t, 1 );
    b = TestLoop( GetParent( t ));
    SetMark( t, m );
  }
  return b;
}
//=====================================================================
void 
CObjDef::Remove( UINT t ) 
{ 
  if( IsClassModifyable( t ))
  {
    OD* pOD = &m_Oda[t];
    pOD->Mark = -1;
    App.WriteProfileString( SectName, pOD->Name, 0 );
    pOD->Name[0] = 0;
  }
}
//=====================================================================
LPCTSTR
CObjDef::GetName( UINT t ) const
{
  static char b[32];
  sprintf( b, "#%d", t );
  return 
    t == 0 ? "Все объекты" :
    IsClassValid( t ) && m_Oda[t].Name[0] ? m_Oda[t].Name : b;
}
//=====================================================================
int
CObjDef::GetEdit( UINT t ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( m_Oda[t].Edit >= 0 )
      return m_Oda[t].Edit;
  return 0;
}
//=====================================================================
int
CObjDef::GetLoad( UINT t ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( m_Oda[t].Load >= 0 )
      return m_Oda[t].Load;
  return 0;
}
//=====================================================================
int
CObjDef::GetTable( UINT t ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( m_Oda[t].Table >= 0 )
      return m_Oda[t].Table;
  return 0;
}
//=====================================================================
long
CObjDef::GetMinLimit( UINT t ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( m_Oda[t].MinLimit >= 0 )
      return m_Oda[t].MinLimit;
  return 0;
}
//=====================================================================
long
CObjDef::GetMaxLimit( UINT t ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( m_Oda[t].MaxLimit >= 0 )
      return m_Oda[t].MaxLimit;
  return 0;
}
//=====================================================================
long
CObjDef::GetMinScale( UINT t ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( m_Oda[t].MinScale >= 0 )
      return m_Oda[t].MinScale;
  return 0;
}
//=====================================================================
long
CObjDef::GetMaxScale( UINT t ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( m_Oda[t].MaxScale >= 0 )
      return m_Oda[t].MaxScale;
  return 0;
}
//=====================================================================
int  
CObjDef::GetTextInvisible( UINT t ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( m_Oda[t].TextInvisible >= 0 )
      return m_Oda[t].TextInvisible;
  return 0;
}
//=====================================================================
int 
CObjDef::IsDerivedFrom( UINT t, UINT p ) const
{
  for( ; IsClassValid( t ) ; t = GetParent( t ))
    if( GetParent( t ) == p )
      return 1;
  return 0;
}
//=====================================================================
int 
CObjDef::GetDerived( UINT t, int* dst ) const
{
  int n = 0;
  if( t < FULL_RANGE )
    for( int i = FULL_RANGE ; --i >= 0 ; )
      if( GetParent( i ) == t && !GetMark( i ))
        if( dst )
          dst[n++] = i;
        else
          return 1;
  return n;  
}
//=====================================================================
int 
CObjDef::GetLevel( UINT t, int* dst ) const
{
  int n = 0;
  if( t < FULL_RANGE )
  {
    UINT p = GetParent( t );
    for( int i = FULL_RANGE ; --i >= 0 ; )
      if( GetParent( i ) == p && !GetDerived( i ))
        if( dst )
          dst[n++] = i;
        else
          return 1;
  }
  return n;  
}
//=====================================================================
int 
CObjDef::GetBottom( UINT t, int* dst ) const
{
  int n = 0;
  if( t < FULL_RANGE )
  {
    for( int i = FULL_RANGE ; --i >= 0 ; )
      if( IsDerivedFrom( i, t ) && !GetDerived( i ))
        if( dst )
          dst[n++] = i;
        else
          return 1;
  }
  if( !n )
  {
    if( dst )
      *dst = t;
    n = 1;
  }
  return n;  
}
//=====================================================================
int 
CObjDef::GetClass( LPCSTR name ) const
{
  int t;
  for(t = FULL_RANGE ; --t > 0 && strcmp( m_Oda[t].Name, name ) ; );
  return t;  
}
//=====================================================================
void
CObjDef::SetParent( UINT t, UINT p )
{ 
  if( IsClassModifyable( t ))
    m_Oda[t].Parent = p;
}
//=====================================================================
void
CObjDef::SetRange( UINT t, UINT o )
{ 
  if( IsClassModifyable( t ))
    m_Oda[t].Range = o;
}
//=====================================================================
void 
CObjDef::SetName( UINT t, LPCSTR name )
{
  if( IsClassModifyable( t ))
    strncpy( m_Oda[t].Name, name, nMaxName );
}
//=====================================================================
void 
CObjDef::SetEdit( UINT t, int law )
{ 
  if( IsClassModifyable( t ))
    m_Oda[t].Edit = law;
}
//=====================================================================
void 
CObjDef::SetLoad( UINT t, int load )
{ 
  if( IsClassModifyable( t ))
    m_Oda[t].Load = load;
}
//=====================================================================
void 
CObjDef::SetTable( UINT t, int tab )
{ 
  if( IsClassModifyable( t ))
    m_Oda[t].Table = tab;
}
//=====================================================================
void 
CObjDef::SetTextInvisible( UINT t, int tv )
{ 
  if( IsClassModifyable( t ))
    m_Oda[t].TextInvisible = tv;
}
//=====================================================================
void 
CObjDef::SetMinMaxLimit( UINT t, long minLimit, long maxLimit )
{ 
  if( IsClassModifyable( t ))
    m_Oda[t].MinLimit = minLimit,
    m_Oda[t].MaxLimit = maxLimit;
}
//=====================================================================
void 
CObjDef::SetMinMaxScale( UINT t, long minScale, long maxScale )
{ 
  if( IsClassModifyable( t ))
    m_Oda[t].MinScale = minScale,
    m_Oda[t].MaxScale = maxScale;
}
//=====================================================================
int  
CObjDef::IsClassModifyable( UINT t )
{ 
  return IsClassValid( t ) && IsClass( t ) ? ( m_bModified = 1 ) : 0; 
}
//=====================================================================
