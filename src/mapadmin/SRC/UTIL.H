#ifndef __UTIL_H__
#define __UTIL_H__
//=====================================================================
class CMemDC : public CDC
{
public:
  CMemDC( CDC* pDC, int bTextMode = 0 );
};
//=====================================================================
class CSmartPtr
{
  void* mPtr;
public:
  CSmartPtr( void* ptr = 0 );
  ~CSmartPtr();
  void* operator=( void* ptr );
  int operator==( void* ptr );
};
//=====================================================================
#ifndef _FULL_PROJECT
int FileExist( char const* path );
int GetProjectSLEnable();
#endif
CString Compose( char const* s, ... );
CString Length2String( int len, int psf = 0 );
void RemoveSpaces( char* str, int bReplaceNewLines = 0 );
int SetTM( CDC* pDC );
void SetIdenticalTransform( CDC* pDC );
int ErrBox( char const* m );
int AskBox( char const* m, int bCancel = 0 );
int InfBox( char const* m );
int DoModalDlg( CDialog* pDlg, UINT msg );
//=====================================================================
#endif