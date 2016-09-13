#ifndef __MOVEMODE_H__
#define __MOVEMODE_H__
//=====================================================================
#include "appmode.h"
//=====================================================================
class CMoveMode : public CAppMode
{
  friend class CMapView;

public:
  CMoveMode( CPoint startPoint );
  ~CMoveMode();
  int  Open();
  int  BreakCopy();
  int  OnLButtonDown( UINT &flags, CPoint &loc ) 
    { return OnLButtonUp( flags, loc ); }
  int  OnLButtonUp( UINT &flags, CPoint &loc );
  int  OnMouseMove( UINT &flags, CPoint &loc );
  int  OnChar( UINT& flags, UINT& key_code );
  int TestLButtonDown() { return 0; } 
  void Draw( CDC* pDC );

  static int  CopyMode();
  static void OnCopyMode();

private:
  int OnButtonUp( UINT &flags, CPoint &loc );

private:
  static int m_bCopy;

private:
  int      m_Scale;
  CBox     m_Box; 
  CSize    m_Bmps;
  CSize    m_PictOff;
  CPoint   m_LastPos;
  CPoint   m_StartPos;
  CBitmap  m_PictBmp;
  CDrawObject* m_pBlinking;
  CObjList m_TmpList;
};
//=====================================================================
#endif

