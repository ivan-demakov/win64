#ifndef __FULINK_H__
#define __FULINK_H__
//=====================================================================
#include "ident.h"

enum ARROWMASK {NO_ARROW=0, ARROW_ID0=1, ARROW_ID1=2, ARROW_ALL=3};

class CFuncLink
{
  CTabIdent m_Id0;
  CTabIdent m_Id1;
  int       m_ArrowMask;
  COLORREF  m_Color;
  CString   m_Subscr;
  CString   m_Topscr;
  CPoint    m_Pt[2];

  friend int operator==( CFuncLink const& fl0, CFuncLink const& fl1 )
  { 
    return fl0.m_Id0 == fl1.m_Id0 && fl0.m_Id1 == fl1.m_Id1 ||
           fl0.m_Id0 == fl1.m_Id1 && fl0.m_Id1 == fl1.m_Id0; }
  friend int operator!=( CFuncLink const& fl0, CFuncLink const& fl1 ) { return !( fl0 == fl1 ); }

public:
  CFuncLink() {} 
  CFuncLink( CTabIdent Id0, CTabIdent Id1, int ArrowMask = 0, COLORREF Color = 0,
             CString Subscr = "", CString Topscr = ""  ) :
    m_Id0( Id0 ),
    m_Id1( Id1 ),
    m_ArrowMask( ArrowMask ),
    m_Color( Color ),
    m_Subscr( Subscr ),
    m_Topscr( Topscr )
  {}

  CRect GetBox() const;
  CPoint const* GetPoints() { return m_Pt; }
  int DefPoints( CMapView* pView );
  void Draw( CDC* pDC );
  int IsMyObject( CTabIdent Id );
  CString const* Detected( CSpot const &spot );
  void Show( CMapView* pView, int bShow = 1 );
};
//=====================================================================
typedef CArray< CFuncLink, CFuncLink&> CLinkArray;
//=====================================================================
#endif