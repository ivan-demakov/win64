#include "stdafx.h"

#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
#endif
#include "bmpdef.h"
#include "scale.h"
#include "mdoc.h"
#include "mview.h"
#include "util.h"
#include "msutil.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
int
CBmpDefArray::GetFreeId()
{
  for( int id = 0 ; GetBmpDef( id ) ; ++id );
  return id;
}
//=====================================================================
CBmpDef*
CBmpDefArray::GetBmpDef( int id )
{
  for( int i = GetSize() ; --i >= 0 && GetAt( i )->m_BmpId != id ; );
  return i >= 0 ? GetAt( i ) : 0;
}
//=====================================================================
void
CBmpDefArray::Clean()
{
  for( int i = GetSize() ; --i >= 0 ; delete GetAt( i ));
  RemoveAll();
}
//=====================================================================
int
CBmpDefArray::Init()
{
  Clean();

  CString path( App.GetProjectFilePath( "bitmaps\\*.*" ));

	char const ubs[] = "MAP_BITMAPS";
	char const cbs[] = "CONCRET_BITMAPS";

  for( int bUser = 0 ; bUser < 2 ; ++bUser )
	{
    char const* sect = bUser ? ubs : cbs;
    char buf[16384];

    App.GetProfileKeys( sect, buf, sizeof buf );

    for( char* pb = buf ; *pb ; pb += strlen( pb ) + 1 )
    {
      char fn[256] = "";
      int sn = 0, id = atoi( pb );
	  	CString S( bUser ? App.GetProfileString( cbs, pb ) : "" );
		  if( S.IsEmpty())
			  S = App.GetProfileString( sect, pb );

      int ns = sscanf( S, "%s%d", fn, &sn );
      if( ns == 2 )
        Add( new CBmpDef( id, path, fn, sn ));
		}
  }
  return GetSize();
}
//=====================================================================
CSize
CBmpDef::GetBmpSize( int scale )
{
  CSize ps( pView->DP2LP( m_Size ));
  ps.cx = MulDiv( ps.cx, m_ScaleSingle, scale );
  ps.cy = MulDiv( ps.cy, m_ScaleSingle, scale );
  return ps;
}
//=====================================================================
void
CBmpDef::DrawBmp( HBITMAP hBmp, long mode,
                  CDC* pDC, CPoint const& loc, int scale )
{
  if( !hBmp )
    return;

  CDC bmpDC;
  bmpDC.CreateCompatibleDC( 0 );
  bmpDC.SelectObject( CBitmap::FromHandle( hBmp ));
  CSize S( GetBmpSize( scale ));
  pDC->StretchBlt( loc.x - S.cx / 2, loc.y - S.cy / 2, S.cx, S.cy,
                   &bmpDC, 0, 0, m_Size.cx, m_Size.cy, mode );
}
//=====================================================================
void
CBmpDef::Draw( CDC* pDC, CPoint const& loc, int scale )
{
  pDC->SetStretchBltMode( STRETCH_HALFTONE );
  if( m_hMsk )
    DrawBmp( m_hMsk, SRCAND, pDC, loc, scale );
  if( m_hBmp )
    DrawBmp( m_hBmp, m_hMsk ? SRCINVERT : SRCCOPY, pDC, loc, scale );
}
//=====================================================================
CBmpDef::CBmpDef( int id, char const* path, char const* fName, int single ) :
  m_BmpId( id ),
  m_ScaleSingle( single )
{
  char bPath[_MAX_PATH], mPath[_MAX_PATH];
  MakePath( path, bPath, "bmp", fName );
  MakePath( path, mPath, "msk", fName );
  m_BmpPath = bPath;

  m_hBmp = (HBITMAP)LoadImage( 0, bPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
  m_hMsk = (HBITMAP)LoadImage( 0, mPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

  if( m_hBmp )
  {
    BITMAP bmps;
    CBitmap::FromHandle( m_hBmp )->GetBitmap( &bmps );
    m_Size.cx = bmps.bmWidth;
    m_Size.cy = bmps.bmHeight;
  }
  else
    m_Size.cx = m_Size.cy = 0;
}
//=====================================================================
CBmpDef::~CBmpDef()
{
  DeleteObject( m_hBmp );
  DeleteObject( m_hMsk );
}
//=====================================================================
