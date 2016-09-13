// linetmpl.h 28.10.98
// line template class definition

#ifndef _LINETMPL_H_
#define _LINETMPL_H_

#include "drawitem.h"
//=====================================================================
// Line template stucture:
// <template>     ::= <scale-factor> <length> { <polyline> }
// <scale-factor> ::= <int>
// <length>       ::= <int>
// <polyline>     ::= <count> <width> { <point> }
// <count>        ::= <int>
// <width>        ::= <int>
// <point>        ::= <int> <int>
// the first polyline with count == 0 marks the end of template
//=====================================================================
class CLineTmpl : public CItem
{
private:
  enum 
  { 
    MaxSegmentLength = 1024,
    MaxTemplateLength = 4096
  };

public:
  CLineTmpl( int index, int count, int* tmpl );
  ~CLineTmpl();
  void Draw( CDC* pDC, CPoint* points, int count );
  void Draw( CDC* pDC, CPoint* points, int count, COLORREF color, double cf );
  void InflateClipSize( CSize& size );
  void GetRange( int* pMin, int* pMax, int bNow = 1 ) const;
  void SetImgBuf( TGstream* pBuf ) { m_pImgBuf = pBuf; }
  int  SaveImg();
  int const* GetLR() const { return m_pTemplate; }

public:
  static TCHAR TmplFileName[];
  static TCHAR m_Ttl0[];
  static TCHAR m_Ttl1[];
  static TCHAR m_Th[];
  static CLineTmpl* GetTemplate( int index );
  static void CreateTemplates();
  static void DeleteTemplates();

private:
  void DrawPart( CDC* pDC, int ntempl, CPoint* pPoints, int count,
                 int orgx, int tmpLen,
                 CPoint top, CPoint btm,
                 CPoint Start,
                 int dx, int dy, int dl, int mgn, int bLeft,
                 int num, int den );
  void DrawFull( CDC* pDC, int ntempl, CPoint* pPoints, int count,
                 int orgx, int tmpLen,
                 CPoint ltop, CPoint lbtm, CPoint rtop, CPoint rbtm,
                 CPoint Start,
                 int dx, int dy, int dl, int lmg, int rmg,
                 int num, int den );
  int DrawTmpLine( CDC* pDC, COLORREF color, int num, int den, 
                   int shift,
                   CPoint* pPrev, CPoint* pStart,
                   CPoint* pEnd, CPoint* pNext );

private:
  int  m_Index;
  int* m_pTemplate;
  int  m_yMax;
  int  m_yMin;
  int  m_Width;
  CPoint m_p0;
  CPoint m_p1;
  TGstream* m_pImgBuf;
  static CArray<CLineTmpl*, CLineTmpl*> m_TmplArr;
};
//=====================================================================
#endif
