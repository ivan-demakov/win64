#include "stdafx.h"

#include <stdio.h>
#include "mview.h"
#include <stdarg.h>
#include "dwin.h"
#include "util.h"
#include "scale.h"
//=====================================================================
CSmartPtr::CSmartPtr( void* ptr ) : mPtr( ptr ) {}
//=====================================================================
CSmartPtr::~CSmartPtr() { delete mPtr; }
//=====================================================================
void*
CSmartPtr::operator=( void* ptr )
{
  delete mPtr;
  return mPtr = ptr;
}
//=====================================================================
int
CSmartPtr::operator==( void* ptr )
{
  return mPtr == ptr;
}
//=====================================================================
#ifndef _FULL_PROJECT
int
FileExist( char const* path )
{
  OFSTRUCT ofs;
  return OpenFile( path, &ofs, OF_EXIST ) >= 0;
}

int
GetProjectSLEnable()
{
  return 0;
}
#endif
//=====================================================================
int
ErrBox( char const* m )
{
  CMapView::GetConductor()->DeclareEvent( CE_ERR_DLG );
  return pView->MessageBox( m, "Ошибка!", MB_ICONSTOP | MB_OK );
}
//=====================================================================
int
AskBox( char const* m, int bCancel )
{
  CMapView::GetConductor()->DeclareEvent( CE_ASK_DLG );
  return pView->MessageBox( m, "Внимание!", MB_ICONQUESTION | ( bCancel ? MB_YESNOCANCEL : MB_YESNO ));
}
//=====================================================================
int
InfBox( char const* m )
{
  CMapView::GetConductor()->DeclareEvent( CE_INF_DLG );
  return pView->MessageBox( m, "Информация!", MB_ICONINFORMATION | MB_OK );
}
//=====================================================================
int
DoModalDlg( CDialog* pDlg, UINT msg )
{
  CMapView::GetConductor()->DeclareEvent( msg );
  int r = pDlg->DoModal();
  CMapView::GetConductor()->DeclareEvent( CE_MODAL_DLG_END );
  return r;
}
//=====================================================================
CMemDC::CMemDC( CDC* pDC, int bTextMode ) :
  CDC()
{
  CreateCompatibleDC( pDC );
  SetMapMode( bTextMode ? MM_TEXT : pDC->GetMapMode());
  SetWindowExt( pDC->GetWindowExt());
  SetViewportExt( pDC->GetViewportExt());
  if( bTextMode )
  {
    SetWindowOrg( 0, 0 );
    SetViewportOrg( 0, 0 );
  }
}
//=====================================================================
CString
Compose( char const* s, ... )
{
  va_list vptr;
  CString comp( "" );
  for( va_start( vptr, s ) ; s ; comp += s, s = va_arg( vptr, char const* ));
  va_end( vptr );
  return comp;
}
//=====================================================================
CString
Length2String( int len, int psf )
{
  char* sg = len >= 0 ? "" : "-";
  int c = SmInUnit < 10 ? 1 : SmInUnit < 100 ? 10 : 100;
  len = MulDiv( abs( len ), SmInUnit, c );
  char* pf = psf ? "м" : "";

  CString s;
  switch( c )
  {
    case  1 : s.Format( "%s%d.%02d%s", sg, len / 100, len % 100, pf );
              break;
    case 10 : s.Format( "%s%d.%01d%s", sg, len / 10, len % 10, pf );
              break;
    default : s.Format( "%s%d%s", sg, len, pf );
  }
  return s;
}
//=====================================================================
void
RemoveSpaces( char* str, int bReplaceNewLines )
{
  if( bReplaceNewLines )
    for( char* t = str ; *t ; ++t )
      if( *t == '\n' )
        *t = ' ';
  for( char* p = str ; *p ; ++p )
    if( *p == ' ' )
    {
      for( char* q = p + 1 ; *q == ' ' ; ++q );
      if( q - p > 1 )
        strcpy( p + 1, q );
    }
  if( p[-1] == ' ' )
    p[-1] = 0;
  if( str[0] == ' ' )
    strcpy( str, str + 1 );
}
//=====================================================================
int
SetTM( CDC* pDC )
{
  int idc = pDC->SaveDC();
  pDC->SetViewportOrg( 0, 0 );
  pDC->SetWindowOrg( 0, 0 );
  pDC->SetMapMode( MM_TEXT );
  return idc;
}
//=====================================================================
void
SetIdenticalTransform( CDC* pDC )
{
  static XFORM t0 = { 1, 0, 0, 1, 0, 0 };
  SetWorldTransform( pDC->GetSafeHdc(), &t0 );
}
//=====================================================================
