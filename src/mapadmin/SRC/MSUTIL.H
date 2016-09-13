#ifndef _MSUTIL_H_
#define _MSUTIL_H_
//=====================================================================
#include "mapstore.h"
#ifdef _CLIENT_APP
#include "ksi.h"
#include "assoc.h"
#include "cbox.h"
//=====================================================================
ResultCode ConvertObject( int vers, ksi_obj init_obj, TGstream& dst, int = 0 );
ksi_obj    ConvertObject( int vers, void** ppSrc, int bFull = 0 );
CBox       CalcBoundBox( int vers, void** ppSrc );

ksi_obj    MakeCoord( int x, int y, int x0, int y0, int nm, int dn );
ksi_obj    MakeSize( int cx, int cy, int an );
ksi_obj    KsiCall( char const* proc, ... );
//#else
double     Deg2Rad( int deg );
#endif
char const* GetDT();
ResultCode TraceError( int mapNum, ResultCode r, char const* opName, long p0 = 0, long p1 = 0 );
ResultCode ReadData( void* pData, int size, istream* pSrc );
ResultCode ReadData( streampos pos, void* pData, int size, istream* pSrc );
ResultCode WriteData( void* pData, int size, ostream* pDst );
ResultCode WriteData( streampos pos, void* pData, int size, ostream* pDst );
char* MakePath( char const* path, char* buf, char const* ext, char const* name = 0 );
int GetLong( char const* path, char const* section, char const* entry, long* par, long def );
int GetString( char const* path, char const* section, char const* entry, char* par, int size, char* def );
extern const CStrAssoc PrimKeys[];
//=====================================================================
#endif