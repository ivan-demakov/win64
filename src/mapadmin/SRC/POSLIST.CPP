//=====================================================================
// פאיכ "poslist.cpp"
//=====================================================================
#include "stdafx.h"
#include "poslist.h"
//=====================================================================
#ifdef _CLIENT_APP
  #ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char BASED_CODE THIS_FILE[] = __FILE__;
  #endif
#endif
//=====================================================================
PositionList::~PositionList()
{
  delete m_pTab;
}
//=====================================================================
PositionList::PositionList() :
  m_pTab( 0 ),
  m_nUsed( 0 ),
  m_nCurr( -1 ),
  m_nSize( MIN_SIZE )
{
}
//=====================================================================
void
PositionList::RemoveAll()
{
  if( m_pTab )
  {
    delete m_pTab;
    m_pTab = 0;
    m_nUsed = 0;
    m_nCurr = -1;
    m_nSize >>= m_nSize > MIN_SIZE * 4 ? 2 : 1;
  }
}
//=====================================================================
std::streampos*
PositionList::FindPos( std::streampos pos )
{
  int n1 = 0, n2 = m_nSize << 1, msk = m_nSize - 1;
  while( n2 > 1 )
  {
    std::streampos* psn = m_pTab + (( pos + n1 ) & msk );
    if( *psn == pos || !*psn )
      return psn;
    if(( n1 += n2 ) >= m_nSize )
      n1 = ( n2 >>= 1 ) >> 1;
  }

  std::cerr << "PositionList::FindPos() failed!" << std::endl;  
  return 0; //it is impossible!!!
}
//=====================================================================
int
PositionList::Insert( std::streampos pos )
{
  if( !m_pTab || m_nUsed >= m_nLimit )
  {
    int newSize = m_nSize << 1;
    std::streampos* pNew = new std::streampos[newSize];
    if( !pNew )
    {
		std::cerr << " Mem alloc (" << newSize << ") for position list failed!" << std::endl;
      return -1;
    }
    m_nLimit = m_nSize;
    m_nSize = newSize;
    memset( pNew, 0, m_nSize * sizeof std::streampos );
    std::streampos* pOld = m_pTab;
    m_pTab = pNew;
    if( pOld )
    {
      for( std::streampos* p = pOld + m_nLimit ; --p >= pOld ; )
      {
        int pos = *p;
        if( pos > 0 )
          *FindPos( pos ) = pos;
      }
      delete pOld;
    }
  }
  std::streampos* p = FindPos( pos );
  m_nUsed += *p == 0;
  *p = pos;

  return 1;
}
//=====================================================================
int
PositionList::Remove( std::streampos pos )
{
  if( !m_nUsed )
    return 0;

  std::streampos* p = FindPos( pos );
  if( *p != pos )
    return 0;

  --m_nUsed;
  *p = -1;    

  return 1;
}
//=====================================================================
int
PositionList::Find( std::streampos pos )
{
  return m_nUsed && *FindPos( pos ) == pos;
}
//=====================================================================
void
PositionList::SetFirst()
{
  if( m_nUsed && m_nCurr < 0 )
  {
    std::streampos* pSrc = m_pTab;
    std::streampos* pDst = pSrc;
    std::streampos* pEnd = pSrc + m_nSize;
    while( pSrc < pEnd )
    {
      int t = *pSrc++;
      if( t > 0 )
        *pDst++ = t;
    }
    pSrc = m_pTab;
    for( int bs = 1 ; bs < m_nUsed ; bs <<= 1 )
    {
      std::streampos* p0 = pSrc;
      std::streampos* d0 = pDst;
      pSrc = d0;
      pDst = p0;
      for( int ts, dn = m_nUsed ; dn > 0 ; )
      {
        dn -= ts = min( bs, dn );
        std::streampos* e0 = p0 + ts;
        std::streampos* p1 = e0;
        dn -= ts = min( bs, dn );
        std::streampos* e1 = p1 + ts;
        while( p0 < e0 && p1 < e1 )
          *d0++ = *p0 < *p1 ? *p0++ : *p1++;
        while( p0 < e0 )
          *d0++ = *p0++;
        while( p1 < e1 )
          *d0++ = *p1++;
        p0 = p1;
      }
    }
    m_pSrt = pSrc;
    m_nCurr = 0;
  }
}
//=====================================================================
void
PositionList::SetNext()
{
  if( m_nCurr >= 0 && m_nCurr < m_nUsed )
    ++m_nCurr;
}
//=====================================================================
std::streampos
PositionList::GetCurr()
{
  return m_nCurr >= 0 && m_nCurr < m_nUsed ? m_pSrt[m_nCurr] : 0;
}
//=====================================================================
std::streampos
PositionList::GetNext()
{
  std::streampos pos = GetCurr();
  if( pos )
    ++m_nCurr;
  return pos;
}
//=====================================================================
