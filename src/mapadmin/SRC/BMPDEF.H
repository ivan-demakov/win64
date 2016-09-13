#ifndef _BMPDEF_H_
#define _BMPDEF_H_
//=====================================================================
class CBmpDef
{
  friend class CBmpDefArray;

public:
  CBmpDef( int id, char const* path, char const* fName, int single );
  ~CBmpDef();
  void Draw( CDC* pDC, CPoint const& loc, int scale );
  CSize GetBmpSize( int scale );
  CString const& GetBmpPath() { return m_BmpPath; }

  int GetBmpId() { return m_BmpId; }
  void SetBmpId( int id ) { m_BmpId = id; }
  int operator==( CBmpDef const& rBmp ) { return m_BmpId == rBmp.m_BmpId; }

private:
  void DrawBmp( HBITMAP hBmp, long mode, CDC* pDC, CPoint const& loc, int scale );

private:
  CString m_BmpPath;
  int     m_BmpId;
  HBITMAP m_hBmp;
  HBITMAP m_hMsk;
  CSize   m_Size;
  int     m_ScaleSingle;
};

class CBmpDefArray : public CArray< CBmpDef*, CBmpDef* >
{

public:
  ~CBmpDefArray() { Clean(); }
  void Clean();
  int Init();
  int GetFreeId();
  CBmpDef* GetBmpDef( int id );
};
//=====================================================================
#endif
