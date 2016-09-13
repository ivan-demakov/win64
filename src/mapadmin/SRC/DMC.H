//======================================================================
// DMC.H
//======================================================================
#ifndef _DMC_H_
#define _DMC_H_
//======================================================================
enum
{
  MSBIT         = 0x40000000,
  MSMSK         = 0x7fffffff,
  MIN_CNT1      = 63,
  MIN_CNT2      = 31,
  MAX_STATE_NUM = 32768
};
//======================================================================
struct CCompressed
{
  long Compressed;
  long Decompressed;
  char Data[0];
  CCompressed( long c = 0, long d = 0 ) :
    Compressed( c ),
    Decompressed( d )
  {}
};
//======================================================================
class DMC
{
  struct State
  {
    WORD Nxt[2];
    UINT Cnt[2];

    void Create( UINT c0, WORD n0, UINT c1, WORD n1 )
    {
      Cnt[0] = c0 ? c0 : 1;
      Cnt[1] = c1 ? c1 : 1;
      Nxt[0] = n0;
      Nxt[1] = n1;
    }

    void Create( State& s )
    {
      Create( s.Cnt[0], s.Nxt[0], s.Cnt[1], s.Nxt[1] );
    }

    void Ratio( UINT mul, UINT div )
    {
      Cnt[0] = MulDiv( Cnt[0], mul, div );
      Cnt[1] = MulDiv( Cnt[1], mul, div );
    }

    UINT Total()
    {
      return Cnt[0] + Cnt[1];
    }
  };

  public:
    DMC(){ Init( 0, 0 ); }
    void Init( char* pBuf, int len );
    void Flash();
    int  Read( char* pBuf, int len );
    int  Write( char* pBuf, int len );
    int  Count(){ return m_pByte - m_pBuf; }

    int Encode( char* pSrc, int srcLen, char* pDst, int dstLen );
    int Decode( char* pSrc, int srcLen, char* pDst, int dstLen );

  private:
    int  AddBitToModel( int b );
    void CalcMP();
    void CreateStartModel();
    int  CreateNode( int level );
    UINT HBit( UINT v ){ return v & MSBIT; }
    UINT Shift( UINT V ){ return V << 1 & MSMSK; }

    static int  GetBit( char*& pByte, char& mask );
    static void PutBit( char*& pByte, char& mask, int b );

  private:
    State  m_States[MAX_STATE_NUM];
    State* m_pCurState;
    int    m_FreeState;
    UINT   m_LB;
    UINT   m_HB;
    UINT   m_MP;
    UINT   m_InMsg;
    UINT   m_LastBit;

    char*  m_pByte;
    char   m_Mask;
    char*  m_pBuf;
    char*  m_pEnd;
    int    m_nSize;
};
//======================================================================
inline int
DMC::GetBit( char*& pByte, char& mask )
{
  static char tb = 0;
  if( mask == 1 )
    tb = *pByte++;
  int b = tb & mask;
  if( !( mask <<= 1 ))
    ++mask;
  return b != 0;
}
//======================================================================
inline void
DMC::PutBit( char*& pByte, char& mask, int b )
{
  *pByte |= mask;
  if( !b )
    *pByte ^= mask;
  if( !( mask <<= 1 ))
  {
    ++mask;
    ++pByte;
  }
}
//======================================================================
#endif // _DMC_H_
