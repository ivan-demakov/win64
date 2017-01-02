#ifndef __ASSOC_H__
#define __ASSOC_H__
//=====================================================================
#include "ksi.h"
//=====================================================================
class CStrAssoc
{
public:
  char* m_Str;
  UINT  m_Key;

  CStrAssoc( char* p = 0, UINT k = 0 ) : m_Str( p ), m_Key( k ) {}

  UINT KsiTranslate( ksi_obj ksi_key ) const;
  ksi_obj KsiTranslate( UINT key ) const;

  UINT Translate( char const* key_name ) const;
  char const* Translate( UINT key ) const;
  int operator !=( char const* name ) const { return _stricmp( name, m_Str ); }
};
//=====================================================================
class CStrAssocMap
{
public:
  CStrAssocMap() : m_pMap( 0 ), m_Size( 0 ) {}
  ~CStrAssocMap();

  UINT Translate( char const* key_name ) const;
  char const* Translate( UINT key ) const;
  int  Init( char const* sectName );

  char const* GetStr( UINT i ) { return m_pMap && i < m_Size ? m_pMap[i].m_Str : 0; }
  UINT GetKey( UINT i ) { return m_pMap && i < m_Size ? m_pMap[i].m_Key : -1; }
  int  GetSize() { return m_Size; }

private:
  CStrAssoc* m_pMap;
  UINT       m_Size;
};
//=====================================================================
class CIntAssocMap
{
  struct IntAssoc
  {
  public:
    int  m_Key;
    long m_Val;
    int m_bUsed;
    void Set( int k, long v ) { m_Key = k; m_Val = v; m_bUsed = 1; }
    int Used() { return m_bUsed; }
  };

public:
  CIntAssocMap( int size = 8 ) :
    m_pMap( 0 ),
    m_nSize( size ),
    m_nUsed( 0 )
  {}  
  ~CIntAssocMap() { delete m_pMap; }
  void Add( int key, long val );
  int  FindFreeKey() const;
  long TranslateKey( int key ) const;
  void GetMinMaxKey( int* pMin, int* pMax ) const;

private:
  int Empty() { return !m_nUsed; }
  IntAssoc* FindPos( int key ) const;

private:
  IntAssoc* m_pMap;
  int       m_nSize;
  int       m_nUsed;
};
//=====================================================================
#endif