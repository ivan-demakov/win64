#include "stdafx.h"

#include "assoc.h"
#include "ksi_type.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
UINT
CStrAssoc::Translate( char const* key_name ) const
{
  for( CStrAssoc const* ap = this ; ap->m_Str && *ap != key_name ; ++ap );
  return ap->m_Key;
}
//=====================================================================
UINT
CStrAssoc::KsiTranslate( ksi_obj ksi_key ) const
{
  return KSI_KEY_P( ksi_key ) ? Translate( KSI_KEY_PTR( ksi_key )) :
         KSI_STR_P( ksi_key ) ? Translate( KSI_STR_PTR( ksi_key )) : 0;
}
//=====================================================================
char const*
CStrAssoc::Translate( UINT key ) const
{
  for( CStrAssoc const* ap = this ; ap->m_Str && ap->m_Key != key ; ++ap );
  return ap->m_Str ? ap->m_Str : "";
}
//=====================================================================
ksi_obj
CStrAssoc::KsiTranslate( UINT key ) const
{
  return ksi_str02key( Translate( key ));
}
//=====================================================================
//=====================================================================
CStrAssocMap::~CStrAssocMap()
{
  if( m_pMap )
    for( int i = 0 ; m_pMap[i].m_Str ; ++i )
      delete m_pMap[i].m_Str;
  delete m_pMap;
  m_pMap = 0;
  m_Size = 0;
}
//=====================================================================
UINT
CStrAssocMap::Translate( char const* str ) const
{
  return m_pMap ? m_pMap->Translate( str ) : 0;
}
//=====================================================================
char const*
CStrAssocMap::Translate( UINT key ) const
{
  return m_pMap ? m_pMap->Translate( key ) : 0;
}
//=====================================================================
int
CStrAssocMap::Init( char const* sectName )
{
  CStrAssocMap::~CStrAssocMap();

  char buf[16384];
  int size = App.GetProfileKeys( sectName, buf, sizeof buf );
  m_pMap = new CStrAssoc[size+1];
  for( char* pb = buf ; *pb ; pb += strlen( pb ) + 1 )
  {
    CString f( App.GetProfileString( sectName, pb ));
    m_pMap[m_Size].m_Key = atoi( pb );
    strcpy( m_pMap[m_Size++].m_Str = new char[strlen(f)+1], f );
  }
  return m_Size;
}
//=====================================================================
//=====================================================================
CIntAssocMap::IntAssoc* 
CIntAssocMap::FindPos( int key ) const
{
  for( int s = 1, i = key % m_nSize ; m_pMap[i].Used() && m_pMap[i].m_Key != key ;
       i = ( i + s++ ) % m_nSize );
  return m_pMap + i;
}
//=====================================================================
int 
CIntAssocMap::FindFreeKey() const
{
  for( int key = 0 ; m_pMap[key].Used() ; ++key );
  return key;
}
//=====================================================================
long
CIntAssocMap::TranslateKey( int key ) const
{
  IntAssoc* p = FindPos( key );
  return p->m_Key == key ? p->m_Val : -1;
}
//=====================================================================
void
CIntAssocMap::Add( int key, long val )
{
  if( !m_pMap || m_nUsed >= m_nSize >> 1 )
  {
    IntAssoc* pMap = m_pMap;
    int n = m_nSize;
    m_nSize <<= 1;
    m_pMap = new IntAssoc[m_nSize];
    memset( m_pMap, 0, m_nSize * sizeof m_pMap[0] );
    if( pMap )
      while( --n >= 0 )
        if( pMap[n].Used())
          *FindPos(pMap[n].m_Key) = pMap[n];
    delete pMap;
  }

  IntAssoc* p = FindPos( key );
  m_nUsed += !p->Used();
  p->Set( key, val );
}
//=====================================================================
void 
CIntAssocMap::GetMinMaxKey( int* pMin, int* pMax ) const
{
  *pMin = 0x7fffffff;
  *pMax = 0x80000000;

  for( int i = m_nSize ; --i >= 0 ; )
    if( m_pMap[i].Used())
    {
      *pMin = min( *pMin, m_pMap[i].m_Key );
      *pMax = max( *pMax, m_pMap[i].m_Key );
    }
}
//=====================================================================
