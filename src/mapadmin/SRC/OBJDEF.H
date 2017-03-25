//=====================================================================
#ifndef _OBJDEF_H_
#define _OBJDEF_H_
//=====================================================================
#include "ids.h"
//=====================================================================
class CObjDef
{
#define nMaxName 64
private:
  struct OD
  {
    OD() : Parent( 0 ), Range( 0 ), TextInvisible( -1 ), Edit( -1 ),
           Load( -1 ), Table( -1 ), Mark( -1 ), MenuRange( 0 ),
           MinLimit( 0 ), MaxLimit( 0 ),
           MinScale( 0 ), MaxScale( 0 ){}
    char Name[nMaxName];
    int  Parent;
    int  Range;
    int  MenuRange;
    int  Edit;
    int  Load;
    int  Table;
    int  TextInvisible;
    int  Mark;
    long MinLimit;
    long MaxLimit;
    long MinScale;
    long MaxScale;
  };

public:
  CObjDef();
  int Init( char const* iniPath );
  int Save() const;
  int SaveModified();
  int Add( LPCSTR name, int* pPar );
  void Remove( UINT t );
  LPCTSTR GetName( UINT type ) const;
  long GetMinLimit( UINT type ) const;
  long GetMaxLimit( UINT type ) const;
  long GetMinScale( UINT type ) const;
  long GetMaxScale( UINT type ) const;
  int  GetTable( UINT type ) const;
  int  GetEdit( UINT t ) const;
  int  GetLoad( UINT t ) const;
  int  GetRange( UINT t ) const { return m_Oda[t].Range; }
  int  GetMenuRange( UINT t ) const { return m_Oda[t].MenuRange; }
  int  GetTextInvisible( UINT t ) const;
  int  GetMark( UINT t ) const { return m_Oda[t].Mark; }
  int  GetParent( UINT t ) const { return m_Oda[t].Parent; }
  int  IsDerivedFrom( UINT t1, UINT t2 ) const;
  int  IsDerivedFromEx( UINT t1, UINT t2 ) { return t1 == t2 || IsDerivedFrom( t1, t2 ); }
  int  GetDerived( UINT t, int* dst = 0 ) const;
  int  GetLevel( UINT t, int* dst = 0 ) const;
  int  GetBottom( UINT t, int* dst = 0 )  const;
  int  GetClass( LPCSTR name ) const;
  int  IsClass( UINT t ) const;
  int  LikeParent( UINT t ) const;
  int  IsModified() { return m_bModified; }
  int  IsCollision() { return m_bCollision; }
  void SetModified( int m ) { m_bModified = m; }
  void SetRange( UINT t, UINT order );
  void SetParent( UINT t, UINT p );
  void SetEdit( UINT t, int law );
  void SetLoad( UINT t, int load );
  void SetTable( UINT t, int tab );
  void SetTextInvisible( UINT t, int tv );
  void SetMinMaxLimit( UINT t, long minLimit, long maxLimit );
  void SetMinMaxScale( UINT t, long minScale, long maxScale );
  void SetName( UINT t, LPCSTR name );
  void MakeRange();
  void SetParentNum( int t, int v ) { m_Oda[t].Parent = v; }
  void SetMinScaleLimit( int t, int v ) { m_Oda[t].MinScale = v; }
  void SetMaxScaleLimit( int t, int v ) { m_Oda[t].MaxScale = v; }
  int  GetClassInRange( int i ){  return i < m_nCls ? m_Rng[i] : 0; }
  int const* GetRng() { return m_Rng; }
  int GetClsNum() { return m_nCls; }
  int GetSubNum() { return m_nSub; }

private:
  int  IsClassValid( UINT t ) const { return t && t < FULL_RANGE; }
  int  IsClassModifyable( UINT t );
  void SetMark( UINT t, int m )  { m_Oda[t].Mark = m; }
  int  TestLoop( UINT t );
  int  OdLt( int t0, int t1 );
private:
  int m_bCollision;
  int m_bModified;
  OD  m_Oda[FULL_RANGE];
  int m_Rng[FULL_RANGE];
  int m_nCls;
  int m_nSub;
  char const* m_IniPath;
};
//=====================================================================
#endif
