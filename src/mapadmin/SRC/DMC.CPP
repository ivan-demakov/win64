#ifdef _FULL_PROJECT
//======================================================================
// DMC.CPP
//======================================================================
#include "stdafx.h"

#include <atlbase.h>
#include "dmc.h"
//======================================================================
//======================================================================
void
DMC::Init( char* pBuf, int len )
{
  m_LastBit = 0x80000000;
  m_Mask    = 1;
  m_LB      = 0;
  m_InMsg   = 0;
  m_HB      = MSMSK;
  m_pByte   = pBuf;
  m_pBuf    = pBuf;
  m_pEnd    = pBuf + len;
  m_nSize   = len;
  CreateStartModel();
}
//======================================================================
int
DMC::CreateNode( int level )
{
  int n = level-- ? m_FreeState++ : 0;
  if( level >= 0 )
    m_States[n].Create( 1, CreateNode( level ), 1, CreateNode( level ));
  return n;
}
//======================================================================
inline void
DMC::CreateStartModel()
{
  m_FreeState = 0;
  m_pCurState = m_States + CreateNode( 8 );
}
//======================================================================
int
DMC::AddBitToModel( int b )
{
  UINT nxt = m_pCurState->Nxt[b];
  State* pNxt = m_States + nxt;
  UINT cCnt = m_pCurState->Cnt[b] += 1;
  UINT nxtCnt = pNxt->Cnt[0] + pNxt->Cnt[1];
  UINT nxtPrd = nxtCnt - cCnt;
  if( nxtCnt > MIN_CNT1 && nxtPrd > MIN_CNT2 )
  {
    if( m_FreeState == MAX_STATE_NUM )
    {
      CreateStartModel();
      return AddBitToModel( b );
    }
    State s( *pNxt );
    s.Ratio( cCnt, nxtCnt );
    nxt = m_pCurState->Nxt[b] = m_FreeState++;
    m_States[nxt].Create( s );
    pNxt->Ratio( nxtPrd, nxtCnt );
  }
  m_pCurState = m_States + nxt;
  return b;
}
//======================================================================
void
DMC::CalcMP()
{
  UINT pp = m_pCurState->Total();
  m_MP = ( Int32x32To64( m_HB, m_pCurState->Cnt[0] ) +
           Int32x32To64( m_LB, m_pCurState->Cnt[1] ) + pp - 1 ) / pp;
  m_MP += m_MP == m_LB;
}
//======================================================================
int
DMC::Write( char* pBuf, int len )
{
  ATLASSERT( m_pBuf );

  char mask = 1;
  int b;

  for( int n = 0 ; n < len ; ++n )
    for( int i = 8 ; --i >= 0 ; )
    {
      CalcMP();

      if( AddBitToModel( GetBit( pBuf, mask )))
        m_LB = m_MP;
      else
        m_HB = m_MP - 1;

      while(( b = HBit( m_LB )) == HBit( m_HB ))
        if( m_pByte < m_pEnd )
        {
          PutBit( m_pByte, m_Mask, b );
          m_LB = Shift( m_LB );
          m_HB = Shift( m_HB ) | 1;
        }
        else
          return n;
    }

  return n;
}
//======================================================================
void
DMC::Flash()
{
  ATLASSERT( m_pBuf );
  for( Write( "", 1 ) ; m_MP && m_MP != MSBIT && m_pByte < m_pEnd ;
       PutBit( m_pByte, m_Mask, HBit( m_MP )), m_MP = Shift( m_MP ));
}
//======================================================================
int
DMC::Read( char* pBuf, int len )
{
  ATLASSERT( m_pBuf );

  char mask = 1;
  int b;

  for( int n = 0 ; n < len ; ++n )
    for( int i = 8 ; --i >= 0 ; )
    {
      for( CalcMP() ;; )
        if(( m_InMsg | ( m_LastBit - 1 )) < m_MP )
        {
          b = 0;
          m_HB = m_MP - 1;
          break;
        }
        else
        if( m_InMsg >= m_MP )
        {
          b = 1;
          m_LB = m_MP;
          break;
        }
        else
        if( m_pByte < m_pEnd )
        {
          m_LastBit >>= 1;
          m_InMsg |= GetBit( m_pByte, m_Mask ) ? m_LastBit : 0;
        }
        else
          return n;

      while( HBit( m_LB ) == HBit( m_HB ))
      {
        m_LB = Shift( m_LB );
        m_HB = Shift( m_HB ) | 1;
        m_InMsg = Shift( m_InMsg );
        m_LastBit <<= 1;
      }

      PutBit( pBuf, mask, AddBitToModel( b ));
    }
  return n;
}
//======================================================================
int
DMC::Encode( char* pSrc, int srcLen, char* pDst, int dstLen )
{
  Init( pDst, dstLen );
  int n = Write( pSrc, srcLen );
  Flash();
  return n;
}
//======================================================================
int
DMC::Decode( char* pSrc, int srcLen, char* pDst, int dstLen )
{
//  int p = MulDiv( srcLen, 100, dstLen );
  Init( pSrc, srcLen );
  return Read( pDst, dstLen );
}
//======================================================================
// end of code
#endif
