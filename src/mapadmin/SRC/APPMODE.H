#ifndef __APPMODE_H__
#define __APPMODE_H__
//=====================================================================
#define AM_FNOTHING     0x0000
#define AM_FLBDOWN      0x0001
#define AM_FLBUP        0x0002
#define AM_FLBDBLCLK    0x0004
#define AM_FRBDOWN      0x0008
#define AM_FRBUP        0x0010
#define AM_FRBDBLCLK    0x0020
#define AM_FMBDOWN      0x0040
#define AM_FMBUP        0x0080
#define AM_FMBDBLCLK    0x0100
#define AM_FMOUSEMOVE   0x0200
#define AM_FCHAR        0x0400
#define AM_FTIMER       0x0800
#define AM_FMONOPOLY    0x1000
#define AM_FUNIMODE     0x2000
#define AM_FESCMODE     0x4000
#define AM_FALL         0x1fff
#define AM_ALL          0xffff
#define AM_FBDOWN       (AM_FLBDOWN|AM_FRBDOWN|AM_FMBDOWN)
#define AM_FBUP         (AM_FLBUP|AM_FRBUP|AM_FMBUP)
#define AM_FBALL        (AM_FBDOWN|AM_FBUP)

#define AM_RPROPAGATE   0
#define AM_RDONE        1
#define AM_RREMOVE      2
//=====================================================================
class CMapView;
//=====================================================================
class CAppMode
{
  friend class CMapView;

public:
  CAppMode( UINT nGroup, UINT nCursor, UINT nFlags, UINT nID );
  virtual ~CAppMode() {}

  UINT GetId() { return m_nID; }

  virtual void Select() {}
  virtual void Draw( CDC* pDC ) {}
  virtual int Term( int bDestroy = 0 ) { return 0; }
  virtual int Open() { return 1; }
  virtual int Init( CMapView* pOwner )                    { m_pOwner = pOwner; return 1; }
  virtual int OnLButtonDown( UINT& flags, CPoint& loc )   { return MT(); }
  virtual int OnLButtonUp( UINT& flags, CPoint& loc )     { return MT(); }
  virtual int OnLButtonDblClk( UINT& flags, CPoint& loc ) { return MT(); }
  virtual int OnRButtonDown( UINT& flags, CPoint& loc )   { return MT(); }
  virtual int OnRButtonUp( UINT& flags, CPoint& loc )     { return MT(); }
  virtual int OnRButtonDblClk( UINT& flags, CPoint& loc ) { return MT(); }
  virtual int OnMButtonDown( UINT& flags, CPoint& loc )   { return MT(); }
  virtual int OnMButtonUp( UINT& flags, CPoint& loc )     { return MT(); }
  virtual int OnMButtonDblClk( UINT& flags, CPoint& loc ) { return MT(); }
  virtual int OnMouseMove( UINT& flags, CPoint& loc )     { return MT(); }
  virtual int OnTimer( UINT& nIDEvent )                   { return AM_RPROPAGATE; }
  virtual int OnChar( UINT& flags, UINT& key_code );
  virtual int TestLButtonDown()                           { return -1;} 
  virtual int OnSetCursor(); 

private:
  int MT() { return IsMonMode() ? AM_RDONE : AM_RPROPAGATE; }
  int IsUniMode() { return m_nFlags & AM_FUNIMODE; }
  int IsMonMode() { return m_nFlags & AM_FMONOPOLY; }

protected:
  CMapView* m_pOwner;
  CAppMode* m_pNext;
  UINT      m_nID;
  UINT      m_nCursor;
  UINT      m_nGroup;
  UINT      m_nFlags;
  int       m_nMode;
};
//=====================================================================
#endif
