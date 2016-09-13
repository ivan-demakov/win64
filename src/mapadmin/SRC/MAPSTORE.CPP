//=====================================================================
#include "stdafx.h"

#include <math.h>
#include <crtdbg.h>
#include <io.h>
#include <time.h>
#include <string.h>
#include "mapstore.h"
#include "msutil.h"
#ifndef _FULL_PROJECT
  #include "util.h"
#endif
//=====================================================================
#ifdef _CLIENT_APP
  #ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char BASED_CODE THIS_FILE[] = __FILE__;
  #endif
#endif
//=====================================================================
char MapStore::MapHeader[] = "_TECHNOGRAD_MAP_";
char MapStore::General[]   = "General";
//=====================================================================
MapStore::MapStore() :
  MapNumber( -1 ),
  ModifiedFlag( 0 ),
  DataSize( 0 ),
  pData( 0 ),
  pDrh( 0 ),
  pNodes( 0 ),
  pLoadMap( 0 ),
  nClassScan( -1 ),
  pSelectInfo( SI ),
  FlashInterval( 3600 ),
  lastOpTime( 0 )
{
  *DataPath = 0;
}
//=====================================================================
void
MapStore::Close()
{
  if( *DataPath )
  {
    delete pData;
    pData = 0;
    WriteData( 0, &HR, sizeof HR, &DataStream );
    DataStream.close();
    char t[1024];
    sprintf( t, "Close : %s", DataPath );
    TraceError( MapNumber, R_INFORMATION, t );
    *DataPath = 0;
  }
}
//=====================================================================
MapStore::~MapStore()
{
  SaveIndexes();
  Close();
        delete pNodes;
}
//=====================================================================
ResultCode
MapStore::SetValidNodeList( long* pNds )
{
  delete pNodes;
  for( long* p = pNds ; *p++ ; );
  int n = p - pNds;
  pNodes = new long[n];
  memcpy( pNodes, pNds, n * sizeof( long ));
  return R_OK;
}
//=====================================================================
void
MapStore::SaveIndexes()
{
  if( ModifiedFlag )
  {
#ifndef _FULL_PROJECT
    HR.IndexValid = SpaceIndex.Save() == R_OK;
#endif
    ModifiedFlag = 0;
  }
}
//=====================================================================
ResultCode
MapStore::GetTimeClock( time_t* pTime, clock_t* pClock )
{
  *pTime = time( 0 );
  *pClock = clock();
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::SetLoadMap( char const* pMap )
{
  memcpy( LoadMap, pMap, sizeof LoadMap );
  SetLoadMapPrt( LoadMap );
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::IsExisting( char const* iniPath )
{
  ResultCode r = iniPath ? ExtractDataPath( iniPath ) : R_OK;
  return r != R_OK || _access( DataPath, 0 ) < 0 ? R_NOFILE : R_OK;
}
//=====================================================================
ResultCode
MapStore::CreateEmpty( char const* iniPath )
{
  struct LParDef
  {
    char* entry;
    long* pValue;
    long  defVal;
  };

  static LParDef LPD[] =
  {
    "Version",   &HR.Version,  DEFVERSION,
    "IncSize",   &HR.IncSize,  DEFINCSIZE,
    "UnitDec",   &HR.UnitDec,  DEFUNITDEC,
    "XSize",     &HR.XSize,    DEFXSIZE,
    "YSize",     &HR.YSize,    DEFYSIZE,
    "MaxScale",  &HR.MaxScale, DEFMAXSCALE
  };

  struct CParDef
  {
    char* entry;
    char* pValue;
    int   size;
  };

  static CParDef CPD[] =
  {
    "Type", HR.PrjType, sizeof HR.PrjType,
    "Parallel_1", HR.Parallel_1, sizeof HR.Parallel_1,
    "Semimajor",  HR.Semimajor,  sizeof HR.Semimajor,
    "Semiminor",  HR.Semiminor,  sizeof HR.Semiminor,
    "XC0", HR.XC0, sizeof HR.XC0,
    "YC0", HR.YC0, sizeof HR.YC0,
    "LT0", HR.LT0, sizeof HR.LT0,
    "LN0", HR.LN0, sizeof HR.LN0
  };

  ResultCode r = iniPath ? ExtractDataPath( iniPath ) : R_OK;
  if( r == R_OK )
  {
    RemoveAllIndex();
    if( iniPath )
    {
      memset( &HR, 0, sizeof HR );
      strncpy( HR.Ident, MapHeader, sizeof HR.Ident );
      for( LParDef* pld = LPD + CELEM( LPD ) ; --pld >= LPD ; )
        GetLong( iniPath, General, pld->entry, pld->pValue, pld->defVal );

      for( CParDef* pcd = CPD + CELEM( CPD ) ; --pcd >= CPD ; )
        GetString( iniPath, "PROJECTION", pcd->entry, pcd->pValue, pcd->size, "" );
    }
    HR.FreePos = sizeof HR;
    HR.IndexValid = 1;

    DataStream.open( DataPath, ios::out|ios::trunc|ios::binary, filebuf::sh_none );
    r = DataStream.bad() ?
      TraceError( MapNumber, R_BADSTREAM, DataPath ) :
      WriteData( 0, &HR, sizeof HR, &DataStream );
    DataStream.close();
  }

  return r;
}
//=====================================================================
void
MapStore::RemoveAllIndex()
{
  remove( SpaceIndex.GetPath());
  TabIdentIndex.Clear();
  SpaceIndex.Clear();
}
//=====================================================================
void
MapStore::RemoveAll()
{
  RemoveAllIndex();
  _chsize( DataStream.fd(), HR.FreePos = sizeof HR );
  HR.IndexValid = 0;
  WriteData( 0, &HR, sizeof HR, &DataStream );
}
//=====================================================================
ResultCode
MapStore::GetUniId( long* pResult )
{
  *pResult = TabIdentIndex.GetUniId( 0 );
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::GetVersion( long* pResult )
{
  if( !DataStream.good())
    return TraceError( MapNumber, R_BADSTREAM, "GetVersion" );

  *pResult = HR.Version;
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::GetUnitDec( long* pResult )
{
  if( !DataStream.good())
    return TraceError( MapNumber, R_BADSTREAM, "GetUnitDec" );

  *pResult = HR.UnitDec;
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::GetMaxScale( long* pResult )
{
  if( !DataStream.good())
    return TraceError( MapNumber, R_BADSTREAM, "GetMaxScale" );

  *pResult = HR.MaxScale;
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::GetXSize( long* pResult )
{
  if( !DataStream.good())
    return TraceError( MapNumber, R_BADSTREAM, "GetXSize" );

  *pResult = HR.XSize;
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::GetYSize( long* pResult )
{
  if( !DataStream.good())
    return TraceError( MapNumber, R_BADSTREAM, "GetYSize" );

  *pResult = HR.YSize;
  return R_OK;
}
//=====================================================================
//=====================================================================
ResultCode
MapStore::GetHeadRecord( streampos pos, istream* pSrc )
{
  static int oldPos = 0;
  ResultCode r = ReadData( pos, pDrh->GetRecPtr(), pDrh->GetRecSize(), pSrc ? pSrc : &DataStream );
  int ec = r == R_OK ? pDrh->Invalid() : 0;
  if( ec )
  {
    r = R_BADRECORDHEAD;
    if( pos - 1 != oldPos )
      TraceError( MapNumber, r, "GetHeadRecord", pos );
    oldPos = pos;
  }

  return r;
}
//=====================================================================
//=====================================================================
ResultCode
MapStore::GetRecord( streampos pos, istream* pSrc )
{
  ResultCode r = GetHeadRecord( pos, pSrc );
  if( r != R_OK )
    return r;

  int dl = pDrh->GetDataSize();

  if( pDrh->IsObjRec() && dl >= MIN_BUF_SIZE )
    r = R_HUDEOBJECT;

  if( r == R_OK && DataSize < dl )
  {
    delete pData;
    DataSize = 0;
    pData = new char[dl];
    if( !pData )
      return TraceError( MapNumber, R_OUTOFMEMORY, "GetRecord" );
    DataSize = dl;
  }

  if( r == R_OK )
    r = ReadData( pData, dl, pSrc ? pSrc : &DataStream );

  return TraceError( MapNumber, r, "GetRecord", pos );
}
//=====================================================================
ResultCode
MapStore::PutHeadRecord( streampos pos, ostream* pDst )
{
  ResultCode r = WriteData( pos, pDrh->GetRecPtr(), pDrh->GetRecSize(), pDst ? pDst : &DataStream );
  return TraceError( MapNumber, r, "PutHeadRecord", pos );
}
//=====================================================================
ResultCode
MapStore::PutRecord( streampos pos, ostream* pDst )
{
  ResultCode r = PutHeadRecord( pos, pDst );
  if( r == R_OK )
    r = WriteData( pData, pDrh->GetDataSize(), pDst ? pDst : &DataStream );
  return TraceError( MapNumber, r, "PutRecord", pos );
}
//=====================================================================
ResultCode
MapStore::AddIndex( streampos pos, Rect const& box, int node,
                    int type, CTabIdent tabid, int bSort )
{
  return
    !SpaceIndex.AddObject( box, pos, ClsInf.GetMinLimit( type ), ClsInf.GetMaxLimit( type ), node, bSort ) ?
      TraceError( MapNumber, R_OUTOFMEMORY, "SpaceIndex" ) :
    !TabIdentIndex.AddObject( tabid, pos ) ?
      TraceError( MapNumber, R_OUTOFMEMORY, "TabIdentIndex" ) : R_OK;
}
//=====================================================================
void
MapStore::RemoveIndex( streampos pos, Rect const& box, /*CIdent const& id,*/ CTabIdent tabid )
{
  TabIdentIndex.RemoveObject( tabid );
  SpaceIndex.RemoveObject( box, pos );
}
//=====================================================================
ResultCode
MapStore::SetModifiedFlag( int m )
{
  HR.IndexValid = !( ModifiedFlag = m );
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::ChangeSize( long s )
{
  filedesc fd = DataStream.fd();
  long p = _filelength( fd );
  long d = s % HR.IncSize;
  if( d )
    s += HR.IncSize - d;
  if( s != p )
  {
    int r = _chsize( fd, s );
    if( r )
      return TraceError( MapNumber, R_BADSTREAM, "ChangeSize", s );

    HR.IncSize = max( HR.IncSize, s - p );
  }
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::ExtractDataPath( char const* iniPath )
{
  if( !iniPath || _access( iniPath, 0 ) < 0 )
    return TraceError( MapNumber, R_NOFILE, iniPath );

  if( iniPath && !GetPrivateProfileString( General, "project", "", DataPath, sizeof DataPath, iniPath ))
    return TraceError( MapNumber, R_NOPROJECT, iniPath );

  MakePath( DataPath, SpaceIndex.GetPath(), ".sdx" );
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::Reinit( char const* iniPath )
{
  SaveIndexes();
  DataStream.close();
  return Init( iniPath );
}
//=====================================================================
static char SectName[] = "MAP_OBJECTS";
static char CurOrder[] = "CurOrder";
static char NewOrder[] = "NewOrder";
static char* pnm[] =
{ "Class", "Parent", "Priority", "MinLimit", "MaxLimit",
  "MinScale", "MaxScale", "Show", "Load", "Table", "Mute" };
static char OldOrder[] = "Class Parent Priority MaxLimit Show Load Table MinScale MaxScale Mute";
//=====================================================================
static int
ParamNum( char const* pn )
{
  for( int i = CELEM( pnm ) ; --i >= 0 && stricmp( pn, pnm[i] ) ; );
  return i;
}
//=====================================================================
CBaseDataRec*
MapStore::SelectDrh( int nVers )
{
  switch( nVers )
  {
    case 101 : return &Drh_0101;
    case 103 : return &Drh_0103;
    case 105 : return &Drh_0103;
    case 107 : return &Drh_0107;
    case 109 : return &Drh_0109;
    case 111 : return &Drh_0111;
    default  : return 0;
  }
}
//=====================================================================
#ifdef _GLOBAL_SPACE
void
MapStore::InitUserClassMap( char* file )
{
	char path[_MAX_PATH];
	MakePath( DataPath, path, ".scm", file );

	FILE* pf = fopen( path, "rt" );
	if( !pf )
		return;

	char buf[1024];
	while( fgets( buf, sizeof buf, pf ))
	{
		char* prp = strchr( buf, '(' );
		if( prp )
		{
			int n = atoi( prp + 1 );
			if( n )
			  UserClassMap[n] = 1;
		}
	}

  fclose( pf );
}
#endif
//=====================================================================
ResultCode
MapStore::Init( char const* iniPath )
{
  strcpy( InitPath, iniPath );
  ResultCode r = IsExisting( iniPath );
  if( r != R_OK )
    r = CreateEmpty( iniPath );
  if( r == R_OK )
  {
    DataStream.open( DataPath, ios::in|ios::out|ios::binary, filebuf::sh_read || filebuf::sh_write );
    if( !DataStream.good())
      r = TraceError( MapNumber, R_BADSTREAM, DataPath );
    else
    {
      r = ReadData( &HR, sizeof HR, &DataStream );
      if( r == R_OK )
      {
        if( strncmp( HR.Ident, MapHeader, sizeof HR.Ident ))
          r = TraceError( MapNumber, R_BADFILESTRUCT, DataPath );
        else
        {
          ClsInf.Init( iniPath );
#ifdef _GLOBAL_SPACE
          HRLimit = GetPrivateProfileInt( General, "HiResLimit", 0, iniPath );
					GetPrivateProfileString( General, "ProjectName", "?", ProjectName, sizeof ProjectName, iniPath );
					SetClassRange( 0 );
					memset( UserClassMap, 0, sizeof UserClassMap );
					InitUserClassMap( "abstract" );
					InitUserClassMap( "concret" );
          IconMap.Init( "MAP_BITMAPS", iniPath );
          IconMap.Init( "CONCRET_BITMAPS", iniPath );
#endif
          pDrh = SelectDrh( HR.Version );
          if( !pDrh )
            return TraceError( MapNumber, R_BADFILESTRUCT, iniPath );

#ifdef _FULL_PROJECT
          int bSpace = 1;
#else
          int bSpace = !HR.IndexValid || SpaceIndex.Load() != R_OK;
#endif
          r = CreateIndexes( bSpace );
          DataStream.clear();
        }
      }
    }
  }

#ifndef _FULL_PROJECT
  MapNumber = 0;
#endif
  return r;
}
//=====================================================================
ResultCode
MapStore::CreateIndexes( int bSpace )
{
  if( bSpace )
    SpaceIndex.Clear();
  TabIdentIndex.Clear();

  int rSize = 0;

  DataStream.seekg( 0, ios::end );
  long limit = min( DataStream.tellg(), HR.FreePos );
  streampos rPos;

  int bTabIndexGood = 1;
  int bSpcIndexGood = 1;

//int t = clock();

  for( rPos = sizeof HR ; rPos < HR.FreePos ; rPos += rSize )
  {
    ResultCode r = GetRecord( rPos );
    rSize = r == R_OK || r == R_HUDEOBJECT ? pDrh->GetSize() : 1;
    if( rSize == 1 )
    {
      int fs = limit - rPos;
      int n = 8;
      while( rSize < fs && GetHeadRecord( rPos + rSize ) != R_OK )
      {
        char dmp[16];
        sprintf( dmp, "%08x%c", pDrh->GetMagic(), --n > 0 ? ' ' : '\n' );
        n > 0 || ( n = 8 );
        cerr << dmp;
        ++rSize;
      }

      if( n )
        cerr << endl;

      if( rSize < pDrh->GetRecSize())
        TraceError( MapNumber, R_BADFILESTRUCT, "Can not correct record", rPos );
      else
      {
        TraceError( MapNumber, R_BADFILESTRUCT, "Corrected record", rPos );
        memset( pDrh->GetRecPtr(), 0, pDrh->GetRecSize());
        pDrh->SetMagic( EMPMAGIC );
        pDrh->SetSize( rSize );
        DataStream.clear();
        PutHeadRecord( rPos );
      }
    }
    else
    if( pDrh->IsObjRec())
    {
      if( !IsObjectValid())
        TraceError( MapNumber, R_BADOBJECT, "", rPos );
      else
      {
        if( bSpace )
          bSpcIndexGood &= SpaceIndex.AddObject( pDrh->GetBox(), rPos,
                                                 ClsInf.GetMinLimit( pDrh->GetClass()),
                                                 ClsInf.GetMaxLimit( pDrh->GetClass()),
                                                 pDrh->GetNode(), 0 );
        bTabIndexGood &= TabIdentIndex.AddObject( pDrh->GetTabIdent(), rPos );
        streampos dPos = TabIdentIndex.LastReplaced();
        if( dPos )
        {
          char m[256];
          sprintf( m, "duplicated index %d %d at ", pDrh->GetTable(), pDrh->GetEntry());
          TraceError( MapNumber, R_INFORMATION, m, dPos );
          PutRecordEmp( dPos );
        }
      }
    }
  }

//t = clock() - t;

  if( bSpace && bSpcIndexGood )
    bSpcIndexGood = SpaceIndex.Sort();

  ResultCode r = R_OK;

  if( !bTabIndexGood )
    r = TraceError( MapNumber, R_OUTOFMEMORY, "Ident index" );

  if( !bSpcIndexGood )
    r = TraceError( MapNumber, R_OUTOFMEMORY, "Space index" );

  SetModifiedFlag( bSpace );
  SaveIndexes();

  return r;
}
//=====================================================================
ResultCode
MapStore::AddObject( TGstream& src )
{
  int bNew = -1;
  dword mg = 0;
  streampos pos, currPos = sizeof mg, lastPos = src.pcount();
  src.read((char*)&mg, sizeof mg );
  switch( mg )
  {
    case INSMAGIC: bNew = 1; break;
    case REPMAGIC: bNew = 0; break;
    default: currPos = 0;
  }

  int size;

  for( ResultCode r = R_OK ; r == R_OK && currPos < lastPos ; currPos += size )
  {
    r = GetRecord( currPos, &src );
    size = pDrh->GetSize();
    if( r != R_OK || !pDrh->IsObjRec())
      r = R_BADFILESTRUCT;
    if( r == R_OK )
    {
      if( !pDrh->GetTable() && !pDrh->GetEntry())
        return TraceError( MapNumber, R_IDENT_ERR, "Zero index" );

      pos = TabIdentIndex.SelectObject( pDrh->GetTabIdent());
      if( bNew == -1 )
        bNew = pos == 0;

      if( bNew != !pos )
         return TraceError( MapNumber, bNew ? R_DUPIDENT : R_NOIDENT, "AddObject",
                            pDrh->GetTable(), pDrh->GetEntry());

      if( pos )
        r = PutRecordEmp( pos );
    }

    if( r == R_OK )
    {
      pos = HR.FreePos;
      r = ChangeSize( HR.FreePos += size );
    }

    if( r == R_OK )
      r = PutRecord( pos );
    if( r == R_OK )
      r = WriteData( 0, &HR, sizeof HR, &DataStream );
    if( r == R_OK )
      r = AddIndex( pos, pDrh->GetBox(), pDrh->GetNode(),
                    pDrh->GetClass(), pDrh->GetTabIdent(), 1 );

    SetModifiedFlag();
  }
  if( r == R_OK )
    r = FlushByTime();

  TraceError( MapNumber, R_INFORMATION, bNew ? "Insert" :"Replace", pDrh->GetTable(), pDrh->GetEntry());
  return r;
}
//=====================================================================
ResultCode
MapStore::RemoveObject( CTabIdent id )
{
  streampos pos = TabIdentIndex.SelectObject( id );
  ResultCode r = pos ? PutRecordEmp( pos ) : R_NOIDENT;
  if( r == R_OK )
    r = FlushByTime();
  TraceError( MapNumber, R_INFORMATION, "Remove", id.Table(), id.Entry());
  return r;
}
//=====================================================================
ResultCode
MapStore::AddObjectToStream( streampos pos, TGstream& dst, int* pCnt )
{
  ResultCode r = GetHeadRecord( pos );
  if( r != R_OK )
    return r;

  int sp = dst.tellp();
  int bs = pDrh->GetBriefSize();
  int ds = pDrh->GetDataSize();
  int fp = sp + bs + ds;

  if( fp > MIN_BUF_SIZE )
    return R_REPEAT;

  char* bb = dst.str() + sp;
  memcpy( bb, pDrh->GetBriefPtr(), bs );
  r = ReadData( bb + bs, ds, &DataStream );
  int bIn = r == R_OK;

  if( bIn && bClipping )
    bIn = IsObjectClipped( dst );

  dst.seekp( bIn ? sp + bs + ds : sp );
  if( pCnt )
    *pCnt += bIn;

  return r;
}
//=====================================================================
ResultCode
MapStore::AddObjectToStream( streampos pos, TGstream& dst, SelectInfo* pi0, SelectInfo* pi1, int* pCnt )
{
  ResultCode r = GetHeadRecord( pos );
  if( r != R_OK || !pDrh->IsObjRec())
    return r;

  int sp = dst.tellp();
  int bs = pDrh->GetBriefSize();
  int ds = pDrh->GetDataSize();
  int fp = sp + bs + ds;

  if( fp > MIN_BUF_SIZE )
    return R_REPEAT;

  int ct = pDrh->GetClass();

  if( ct < FULL_RANGE && pLoadMap && !( pLoadMap[ct>>3] & (1<<(ct&0x7))))
    return r;

  char* bb = dst.str() + sp;
  memcpy( bb, pDrh->GetBriefPtr(), bs );
  r = ReadData( bb + bs, ds, &DataStream );
  int bIn = r == R_OK;

  for( int i = 2 ; --i >= 0 ; )
        {
                SelectInfo* pi = i ? pi1 : pi0;
    if( bIn && pi && pi->poslist.Find( pos ))
                {
      Rect sc( Clip );
      Clip = pi->rect;
      bIn = !IsObjectClipped( dst );
      Clip = sc;
                }
  }

        bIn = bIn && IsObjectClipped( dst );

  dst.seekp( bIn ? sp + bs + ds : sp );
  *pCnt += bIn;

  return r;
}
//=====================================================================
ResultCode
MapStore::TestObject( CTabIdent id )
{
  return TabIdentIndex.SelectObject( id ) ? R_OK : R_NOIDENT;
}
//=====================================================================
ResultCode
MapStore::FindObject( CTabIdent id, TGstream& dst )
{
  int n = 1;
  return FindObjects( &id, &n, dst );
}
//=====================================================================
ResultCode
MapStore::FindObjects( CTabIdent const* pTid, int* pnId, TGstream& dst )
{
  streampos pos;
  ResultCode r = R_OK;
  bClipping = 0;
  int nId = *pnId;
  *pnId = 0;

  if( nId == 1 )
  {
    if( pos = TabIdentIndex.SelectObject( *pTid ))
      r = AddObjectToStream( pos, dst, pnId );
    return TraceError( MapNumber, pos ? r : R_NOIDENT, "FindObjects(1)", pTid->Table(), pTid->Entry());
  }

  if( pSelectInfo[0].poslist.Empty())
  {
#ifdef _FULL_PROJECT
    TraceError( MapNumber, R_INFORMATION, "Try FindObjects", 0, nId );
#endif
    while( --nId >= 0 )
    {
      pos = TabIdentIndex.SelectObject( *pTid++ );
      if( pos )
        pSelectInfo[0].poslist.Insert( pos );
      else
        TraceError( MapNumber, R_NOIDENT, "FindObjects(2)", pTid[-1].Table(), pTid[-1].Entry());
    }
    pSelectInfo[0].poslist.SetFirst();
  }

  while( r == R_OK && ( pos = pSelectInfo[0].poslist.GetCurr()))
  {
    r = AddObjectToStream( pos, dst, pnId );
    if( r != R_REPEAT )
      pSelectInfo[0].poslist.SetNext();
  }

  if( r != R_REPEAT )
    pSelectInfo[0].poslist.RemoveAll();

  return TraceError( MapNumber, r, "FindObjects" );
}
//=====================================================================
ResultCode
MapStore::FindObjectId( CTabIdent const* pTid, int nId, CIdent* pId )
{
  for( ResultCode r = R_OK ; r == R_OK && --nId >= 0 ; )
  {
    streampos pos = TabIdentIndex.SelectObject( pTid[nId] );
    r = pos ? R_OK : R_NOIDENT;
    if( r == R_OK )
    {
      r = GetHeadRecord( pos );
      if( r == R_OK && pDrh->IsObjRec() && pId )
        *pId++ = pDrh->GetIdent();
    }
  }
  return r == R_NOIDENT ? r : TraceError( MapNumber, r, "FindObjectId" );
}
//=====================================================================
ResultCode
MapStore::SelectObjects( Rect const& rect0, Rect const& rect1, int limit01,
                         Rect const& rect2, int limit2,
                         TGstream& dst, int cliCompressFrom, int* pCnt )
{
  if( pSelectInfo[0].poslist.Empty() && pSelectInfo[1].poslist.Empty())
  {
    Rect const* Rct[] = { &rect0, &rect1, &rect2 };
    int Lim[] = { limit01, limit01, limit2 };
    for( int i = 0 ; i < 3 ; ++i )
    {
      pSelectInfo[i].rect.SetRect( *Rct[i] );
      int n = SpaceIndex.SelectObjects( *Rct[i], Lim[i], pNodes, pSelectInfo[i].poslist );
      if( n < 0 )
        TraceError( MapNumber, R_OUTOFMEMORY, "SelectObjects" );
     }
  }

  ResultCode r = R_OK;
  bClipping = 1;
  int Cnt = 0;
  for( SelectInfo* pi = pSelectInfo + 2 ; r == R_OK && --pi >= pSelectInfo ; )
  {
    pi->poslist.SetFirst();
    Clip = pi->rect;
    streampos rPos;
    while( r == R_OK && ( rPos = pi->poslist.GetCurr()))
    {
      int cnt = Cnt;
      r = AddObjectToStream( rPos, dst, &pSelectInfo[2], pi == pSelectInfo ? 0 : pSelectInfo, &Cnt );
      if( r != R_REPEAT )
      {
        pi->poslist.SetNext();
        if( Cnt > cnt && pi > pSelectInfo )
          pSelectInfo->poslist.Remove( rPos );
      }
    }
  }

  if( r != R_REPEAT )
  {
    pSelectInfo[0].poslist.RemoveAll();
    pSelectInfo[1].poslist.RemoveAll();
    pSelectInfo[2].poslist.RemoveAll();
  }

  *pCnt = Cnt;
  return r;
}
//=====================================================================
ResultCode
MapStore::SelectByClass( Rect const& rect, long* pClass, long nClass,
                         TGstream& dst, int* pCnt )
{
  PositionList PosList;
  int n = SpaceIndex.SelectObjects( rect, 0, 0, PosList );
  PosList.SetFirst();
  bClipping = 0;
  for( *pCnt = 0 ; --n >= 0 ; )
  {
    streampos pos = PosList.GetNext();
    if( GetHeadRecord( pos ) == R_OK )
    {
      int t = pDrh->GetClass();
      for( int i = nClass ; --i >= 0 && t != pClass[i] ; );
      if( i >= 0 )
        AddObjectToStream( pos, dst, pCnt );
    }
  }
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::RemoveAllObjects( ClassType type )
{
  ResultCode r = R_OK;
  for( streampos pos = sizeof HR ; r == R_OK && pos < HR.FreePos ; pos += pDrh->GetSize())
    r = PutRecordEmp( pos, type );
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::PutRecordEmp( streampos pos, ClassType type )
{
  char buf[TagRecSize];
  memcpy( buf, pDrh->GetRecPtr(), pDrh->GetRecSize());

  ResultCode r = GetHeadRecord( pos );
  if( r == R_OK && ( !type || type == pDrh->GetClass()))
  {
    RemoveIndex( pos, pDrh->GetBox(), pDrh->GetTabIdent());
    pDrh->SetMagic( EMPMAGIC );
    pDrh->SetTime( time( 0 ));
    pDrh->SetClock( clock());
    r = PutHeadRecord( pos );
  }

  memcpy( pDrh->GetRecPtr(), buf, pDrh->GetRecSize());
  if( r == R_OK )
    SetModifiedFlag();
  return TraceError( MapNumber, r, "PutRecordEmp", pos );
}
//=====================================================================
ResultCode
MapStore::StartClassScan( long nClass )
{
  nClassScan = nClass;
  pPosScan = sizeof( DataFileHeadRec );
  pMaxPosScan = HR.FreePos;
  bClipping = 0;
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::NextClassScan()
{
  if( nClassScan < 0 )
    return R_EOFSTREAM;

  for( ResultCode r = R_OK ; r == R_OK ; )
  {
    r = pPosScan < pMaxPosScan ? GetHeadRecord( pPosScan ) : R_OUTOFMEMORY;
    if( r == R_OK )
    {
      pPosScan += pDrh->GetSize();
      if( pDrh->IsObjRec() && ClsInf.IsDerivedFromEx( pDrh->GetClass(), nClassScan ))
        return R_OK;
    }
  }
  return r;
}
//=====================================================================
ResultCode
MapStore::NextClassScan( CIdent* pId, CTabIdent* pTid )
{
  ResultCode r = NextClassScan();
  if( r == R_OK )
  {
    if( pId )
      *pId  = pDrh->GetIdent();
    if( pTid )
      *pTid = pDrh->GetTabIdent();
  }
  return r;
}
//=====================================================================
ResultCode
MapStore::NextClassScan( TGstream& dst )
{
  ResultCode r = NextClassScan();
  return r == R_OK ? AddObjectToStream( pPosScan - pDrh->GetSize(), dst, 0 ) : r;
}
//=====================================================================
ResultCode
MapStore::SelectObject( CTabIdent id, TGstream& dst, int* pCnt, int bRemoved )
{
  streampos pos;
  bClipping = 0;

  if( pCnt )
    *pCnt = 0;

  if( bRemoved )
  {
    int rSize = 0;
    for( pos = sizeof HR ; pos < HR.FreePos ; pos += rSize )
    {
      rSize = GetRecord( pos ) == R_OK ? pDrh->GetSize() : 1;
      if( rSize > 1 && pDrh->IsEmpRec() && pDrh->GetTabIdent() == id )
        AddObjectToStream( pos, dst, pCnt );
    }
  }
  else
  if( pos = TabIdentIndex.SelectObject( id ))
    AddObjectToStream( pos, dst, pCnt );

  return R_OK;
}
//=====================================================================
ResultCode
MapStore::GetObjectBox( CTabIdent id, Rect* rect )
{
  streampos pos = TabIdentIndex.SelectObject( id );

	if( !pos )
		return R_NOIDENT;

  ResultCode r = GetHeadRecord( pos );
  if( r == R_OK )
   *rect = pDrh->GetBox();

  return r;
}
//=====================================================================
inline float
Det( Point pd, Point cd )
{
  return float( pd.x ) * float( cd.y ) - float( pd.y ) * float( cd.x );
}
//=====================================================================
int
MapStore::IsPointInPolygon( Point c, Point const* plg, int n )
{
  static int const rm[2][2] = {{0,1},{3,2}};
  Point p0( plg[n-1] );
  Point pd( p0 - c );
  int pt = rm[pd.y<0][pd.x<0];

  for( int s = 0 ; --n >= 0 ; )
  {
    if( c == p0 )
      return 0;

    Point p1( p0 );
    p0 = *plg++;
    Point cd( p0 - c );
    int ct = rm[cd.y<0][cd.x<0];
    float det;
    switch( ct - pt )
    {
      case -3:
        ++s;
        break;
      case 3:
        --s;
        break;
      case -2:
        det = Det( pd, cd );
        if( det == 0 )
          return 0;
        if( det > 0 )
          ++s;
        break;
      case 2:
        det = Det( pd, cd );
        if( det == 0 )
          return 0;
        if( det < 0 )
          --s;
        break;
    }
    pd = cd;
    pt = ct;
  }
  return s != 0;
}
//=====================================================================
int
MapStore::HorCross( int l, int r, int y, Point p0, Point p1 )
{
  if( p1.y == p0.y ||
      p0.y < y && p1.y < y ||
      p0.y >= y && p1.y >= y ||
      p0.x < l && p1.x < l ||
      p0.x >= r && p1.x >= r )
    return 0;
  int x = p0.x + MulDiv( p1.x - p0.x, y - p0.y, p1.y - p0.y );
  return x >= l && x < r;
}
//=====================================================================
int
MapStore::VerCross( int t, int b, int x, Point p0, Point p1 )
{
  if( p1.x == p0.x ||
      p0.x < x && p1.x < x ||
      p0.x >= x && p1.x >= x ||
      p0.y < t && p1.y < t ||
      p0.y >= b && p1.y >= b )
    return 0;
  int y = p0.y + MulDiv( p1.y - p0.y, x - p0.x, p1.x - p0.x );
  return y >= t && y < b;
}
//=====================================================================
int
MapStore::PointsClipped( Point const* pPoints, int nPoints, int w )
{
  if( bClipping )
    Clip.Inflate( w );
  Point p( 0, 0 );
  int bAll = !bClipping;
  while( --nPoints >= 0 )
  {
    p = ( HR.Version & 1 ) ? *pPoints++ : p + *pPoints++;
    int r = bClipping ?
      p.x >= Clip.left && p.x < Clip.right && p.y >= Clip.top && p.y < Clip.bottom
      :
      IsPointInPolygon( p, Space, SpaceLength );
    if( r ^ bAll )
      break;
  }
  if( bClipping )
    Clip.Inflate( -w );
  return ( nPoints >= 0 ) ^ bAll;
}
//=====================================================================
int
MapStore::IsLineClipped( Point p0, Point p1 )
{
  return HorCross( Clip.left, Clip.right,  Clip.top,    p0, p1 ) ||
         HorCross( Clip.left, Clip.right,  Clip.bottom, p0, p1 ) ||
         VerCross( Clip.top,  Clip.bottom, Clip.left,   p0, p1 ) ||
         VerCross( Clip.top,  Clip.bottom, Clip.right,  p0, p1 );
}
//=====================================================================
int
MapStore::IsPolylineClipped( Point const* pPoints, int nPoints, int w, int bnd )
{
  if( PointsClipped( pPoints, nPoints, w ))
    return 1;

  if( !bClipping )
    return 0;

  Clip.Inflate( w );
  Point const* pp = pPoints;
  for( Point p0 = *pp++, p1 ; --nPoints > 0 ; p0 = p1 )
  {
    p1 = ( HR.Version & 1 ) ? *pp++ : p0 + *pp++;
    if( IsLineClipped( p0, p1 ))
      break;
  }
  int r = nPoints > 0 || bnd && IsLineClipped( p0, *pPoints );
  Clip.Inflate( -w );
  return r;
}
//=====================================================================
int
MapStore::IsPolygonClipped( Point const* pPoints, int nPoints, int w )
{
  if( IsPolylineClipped( pPoints, nPoints, w, 1 ))
    return 1;

  if( !bClipping )
    return 0;

  Clip.Inflate( w );
  int r = IsPointInPolygon( Clip.LeftTop(),     pPoints, nPoints ) ||
          IsPointInPolygon( Clip.RightTop(),    pPoints, nPoints ) ||
          IsPointInPolygon( Clip.LeftBottom(),  pPoints, nPoints ) ||
          IsPointInPolygon( Clip.RightBottom(), pPoints, nPoints );
  Clip.Inflate( -w );
  return r;
}
//=====================================================================
int
MapStore::IsTextClipped( Point tp, Point of, int angle, char const* pStr, int len, int height  )
{
  int h = abs( height ), dh = h * 5 / 4;

  Rect b( 0, -h, 0, h/4 );
  for( char const* p0 = pStr, *p1 ; len ; p0 = p1 + 1 )
  {
    p1 = (char*)memchr( p0, '\n', len );
    int l = p1 ? p1 - p0 : len;
    len -= l;
    b.right = max( b.right, ::MulDiv( l * 3, h, 5 ));
    b.bottom += p1 ? dh : 0;
  }

  b.Offset( of );

  Point poly[4];

  if( angle )
  {
    double an = Deg2Rad( angle );
    double si = sin( an ), co = cos( an );
    int ts = b.top * si;
    int tc = b.top * co;
    int ls = b.left * si;
    int lc = b.left * co;
    int rs = b.right * si;
    int rc = b.right * co;
    int bs = b.bottom * si;
    int bc = b.bottom * co;

    poly[0] = Point( lc + ts, tc - ls );
    poly[1] = Point( rc + ts, tc - rs );
    poly[2] = Point( rc + bs, bc - rs );
    poly[3] = Point( lc + bs, bc - ls );
  }
  else
  {
    poly[0] = b.LeftTop();
    poly[1] = b.RightTop();
    poly[2] = b.RightBottom();
    poly[3] = b.LeftBottom();
  }

  b.SetRect();
  for( int i = CELEM( poly ) ; --i >= 0 ; b |= ( poly[i] += tp ));

  return ( !bClipping || b.IntersectRect( Clip )) &&
         IsPolygonClipped( poly, 4, 0 );
}
//=====================================================================
int
MapStore::IsObjectClipped( TGstream& dst )
{
  if( pDrh->GetBox().Inside( Clip ))
    return 1;

  void* src = dst.str() + dst.tellp() + pDrh->GetBriefSize();
  return IsDataClipped( &src );
}
//=====================================================================
int
MapStore::IsDataClipped( void** ppSrc )
{
  PrimRecord* pPr = (PrimRecord*)*ppSrc;

  if(( pPr->type & PF_TYPE_MASK ) != PF_CONTAINER )
    return 0;

  int n = pPr->param.container.length;
  *ppSrc = pPr->param.container.prims;
  int bClipped = !bClipping;

  while( --n >= 0 && !bClipped )
  {
    pPr = (PrimRecord*)*ppSrc;
    Drawable* pObjPar  = (Drawable*)&pPr->param;

    int w = 0;
    int subType = (( pPr->type & PF_STROKED ) ? STROKED : 0 ) |
                  (( pPr->type & PF_FILLED  ) ? FILLED  : 0 ) |
                  (( pPr->type & PF_ROUND   ) ? ROUND   : 0 );
    int typeKey  = pPr->type & PF_TYPE_MASK;
    int bVisible = pPr->type & PF_VISIBLE;

    if( bVisible  )
    {
      switch( subType & OUTLINED )
      {
        case STROKED :
          w = pPr->param.stroked.pen.width;
          pObjPar = &pPr->param.stroked.primParam;
          break;
        case FILLED :
          pObjPar = &pPr->param.filled.primParam;
          break;
        case OUTLINED :
          w = pPr->param.outlined.pen.width;
          pObjPar = &pPr->param.outlined.primParam;
        break;
      }
      w = ( w + 1 ) / 2;
    }

    switch( typeKey )
    {
      case PF_CONTAINER:
         bClipped = IsDataClipped( ppSrc );
         continue;
      case PF_POLYPOINT:
      case PF_FOOTNOTE:
      {
        int nPoints = pObjPar->polypoint.length;
        Point* pPoints = (Point*)pObjPar->polypoint.points;
        *ppSrc = pPoints + nPoints;
        if( !( HR.Version & 1 ))
        {
          int n = nPoints;
          for( Point* p = pPoints ; --n > 0 ; p[1] += p[0], ++p );
        }
        bClipped =
          !bVisible ? !bClipping :
          subType == STROKED ? IsPolylineClipped( pPoints, nPoints, w ) :
                               IsPolygonClipped( pPoints, nPoints, w );
        if( !( HR.Version & 1 ))
        {
          int n = nPoints;
          for( Point* p = pPoints + n ; --n > 0 ; --p, p[0] -= p[-1] );
        }
        continue;
      }
      case PF_TEXT:
      {
        StringParam* pSP = bVisible ?
          &pPr->param.text.param.full.string :
          &pPr->param.text.param.string;
        Text::StrPar::FulPar::FntPar* pFP = &pPr->param.text.param.full.FontParams;
        *ppSrc = pSP->string + pSP->length;
        bClipped = !bVisible ?
                     !bClipping :
                     IsTextClipped( *((Point*)&pPr->param.text.org), *((Point*)&pSP->offset),
                                    pSP->angle, pSP->string, pSP->length,
                                    HR.Version < 107 ? pFP->font.height : pFP->font1.height );
        continue;
      }
      case PF_TEXTBYLINE:
      {
        TextByLine::FntPar* pFP = &pPr->param.textbyline.FontParams;
        int length = pPr->param.textbyline.length;
        TxtItem* pItem = pPr->param.textbyline.items;
        *ppSrc = pItem + length;
        for( ; !bClipped && --length >= 0 ; ++pItem )
          bClipped = IsTextClipped( *((Point*)&pItem->org), Point( 0, 0 ),
                                    pItem->angle, (char*)pItem->letter, 1,
                                    HR.Version < 107 ? pFP->font.height : pFP->font1.height );
        continue;
      }
      case PF_MULTILINE:
        *ppSrc = pObjPar->polypoint.points + pObjPar->polypoint.length;
        break;
      case PF_POINT:
        *ppSrc = &pPr->param.pnt + 1;
        break;
      case PF_ICON:
        *ppSrc = &pPr->param.icon + 1;
        break;
      case PF_ELLIPSE :
        *ppSrc = &pObjPar->ellipse + 1;
        break;
      case PF_RECTANGLE :
        *ppSrc = &pObjPar->rectangle + 1;
        break;
      case PF_ARC :
        *ppSrc = &pObjPar->arc + 1;
        break;
    }
    bClipped = bVisible || !bClipping;
  }
  return bClipped;
}
//=====================================================================
ResultCode
MapStore::ClipObjectsId( Point* pPnts, int nPnt, int nClass,
                         TGstream& dst, int* pnId )
{
  char m[FULL_RANGE/8];
  memset( m, 0, sizeof m );
  m[nClass>>3] |= 1 << ( nClass & 0x7 );
  return ClipObjectsId( pPnts, nPnt, m, dst, pnId );
}
//=====================================================================
ResultCode
MapStore::ClipObjectsId( Point* pPnts, int nPnt, char classMap[],
                         TGstream& dst, int* pnId )
{
  bClipping = 1;

  Clip.SetRect();
  for( int i = nPnt ; --i >= 0 ; Clip |= pPnts[i] );

  PositionList PosList;
  int n = SpaceIndex.SelectObjects( Clip, 0, 0, PosList );
  PosList.SetFirst();
  ResultCode r = R_OK;
  while( --n >= 0 && r == R_OK )
  {
    streampos pos = PosList.GetNext();
    r = GetHeadRecord( pos );
    if( r == R_OK )
    {
      CTabIdent id = pDrh->GetTabIdent();
      int i = pDrh->GetClass() >> 3;
      char m = 1 << ( pDrh->GetClass() & 0x7 );
      if( !( classMap[i] & m ))
        continue;

      dst.write( (char*)&id, sizeof id );
      if( pnId )
        *pnId += 1;
    }
  }
  return TraceError( MapNumber, r, "ClipObjectsId" );
}
//=====================================================================
ResultCode
MapStore::SelectByRect( Rect rect, long nClass, TGstream& dst, int* pCnt )
{
  ResultCode r = R_OK;
  *pCnt = 0;

  if( rect.IsRectNull())
  {
    streampos pos;
    if( pSelectInfo[0].poslist.Empty())
    {
      int rSize = 0;
      bClipping = 0;
      for( pos = sizeof HR ; pos < HR.FreePos ; )
      {
        rSize = GetHeadRecord( pos ) == R_OK ? pDrh->GetSize() : 1;
        if( rSize > 1 && pDrh->IsObjRec() && ClsInf.IsDerivedFromEx( pDrh->GetClass(), nClass ))
          pSelectInfo[0].poslist.Insert( pos );
        pos += rSize;
      }
      pSelectInfo[0].poslist.SetFirst();
    }

    while( r == R_OK && ( pos = pSelectInfo[0].poslist.GetCurr()))
    {
      r = AddObjectToStream( pos, dst, pCnt );
      if( r != R_REPEAT )
        pSelectInfo[0].poslist.SetNext();
    }
  }
  else
  {
    if( pSelectInfo[0].poslist.Empty())
    {
      SpaceIndex.SelectObjects( rect, 0, 0, pSelectInfo[0].poslist );
      pSelectInfo[0].poslist.SetFirst();
    }

    bClipping = 0;
    Clip = rect;
    streampos pos;
    while( r == R_OK && ( pos = pSelectInfo[0].poslist.GetCurr()))
    {
      r = GetHeadRecord( pos );
      if( r == R_OK && ClsInf.IsDerivedFromEx( pDrh->GetClass(), nClass ))
        r = AddObjectToStream( pos, dst, pCnt );
      if( r != R_REPEAT )
        pSelectInfo[0].poslist.SetNext();
    }
  }

  if( r != R_REPEAT )
    pSelectInfo[0].poslist.RemoveAll();

  return TraceError( MapNumber, r, "SelectByRect" );
}
//=====================================================================
ResultCode
MapStore::SelectByObject( CTabIdent id, long nClass, TGstream& dst, int* pCnt )
{
  ResultCode r = FindObject( id, dst );

  if( r == R_OK )
    r = SelectByRect( pDrh->GetBox(), nClass, dst, pCnt );

  return TraceError( MapNumber, r, "SelectByObject", id.Table(), id.Entry());
}
//=====================================================================
ResultCode
MapStore::AcountAllObjects( long* pTabl, Point* pRgn, int nRgnSize )
{
  Space = pRgn;
  SpaceLength = nRgnSize;
  bClipping = 0;

  Rect box;
  for( int i = nRgnSize ; --i >= 0 ; box |= pRgn[i] );

  memset( pTabl, 0, FULL_RANGE * sizeof pTabl[0] );

  streampos posScan = sizeof( DataFileHeadRec );

  for( ResultCode r = R_OK ; r == R_OK && posScan < HR.FreePos ; )
  {
    r = GetHeadRecord( posScan );
    if( r == R_OK )
    {
      posScan += pDrh->GetSize();
      if( pDrh->IsObjRec())
      {
        if( Space )
        {
          if( !pDrh->GetBox().IntersectRect( box ))
            continue;

          int dl = pDrh->GetDataSize();
          if( DataSize < dl )
          {
            delete pData;
            DataSize = 0;
            pData = new char[dl];
            if( !pData )
              return TraceError( MapNumber, R_OUTOFMEMORY, "AcountAllObjects" );
            DataSize = dl;
          }
          r = ReadData( pData, dl, &DataStream );
          if( r != R_OK )
            return r;

          void* src = pData;
          if( !IsDataClipped( &src ))
            continue;
        }
        for( int i = FULL_RANGE ; --i >= 0 ; )
          if( ClsInf.IsDerivedFromEx( pDrh->GetClass(), i ))
            ++pTabl[i];
      }
    }
  }
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::SetObjectNode( CTabIdent id, int node )
{
  ResultCode r = R_NOIDENT;
  streampos pos = TabIdentIndex.SelectObject( id );
  if( pos )
  {
    r = GetHeadRecord( pos );
    if( r == R_OK )
      pDrh->SetNode( node );
    int t = pDrh->GetClass();
    r = PutHeadRecord( pos );
    if( r == R_OK )
      SpaceIndex.ReplaceObject( pDrh->GetBox(), pos,
                                ClsInf.GetMinLimit( t ), ClsInf.GetMaxLimit( t ), node );
    TraceError( MapNumber, r, "SetObjectNode", id.Table(), id.Entry());
    SetModifiedFlag( 1 );
  }
  return r == R_OK ? FlushByTime() : r;
}
//=====================================================================
ResultCode
MapStore::FlushByTime()
{
  time_t curTime = time( 0 );
  if( !lastOpTime )
    lastOpTime = curTime;
  else
  if( curTime - lastOpTime >= FlashInterval )
  {
    lastOpTime = curTime;
    try
    {
      DataStream.flush();
      SaveIndexes();
    }
    catch(...)
    {
      return TraceError( MapNumber, R_FLUSHERROR, "FlushByTime" );
    }
    TraceError( MapNumber, R_INFORMATION, "FlushByTime" );
  }
  return R_OK;
}
//=====================================================================
ResultCode
MapStore::Flush()
{
  if( !DataPath[0] )
    return TraceError( MapNumber, R_MAPNOTOPEN, "Flush" );

  try
  {
    DataStream.flush();
  }

  catch(...)
  {
    return TraceError( MapNumber, R_FLUSHERROR, "Flush" );
  }
  return TraceError( MapNumber, R_INFORMATION, "Flush" );
}
//=====================================================================
ResultCode
MapStore::Backup()
{
  static char const opname[] = "Backup";

  if( !DataPath[0] )
    return TraceError( MapNumber, R_MAPNOTOPEN, opname );

  int bSucc = 0;
  try
  {
    char BackupPath[_MAX_PATH+15];
    char dtm[32];

    long ctm = time( 0 );
    struct tm* ct = localtime( &ctm );
    sprintf( dtm, "_%02d%02d%02d_%02d%02d", ct->tm_mday, ct->tm_mon+1, ct->tm_year-100, ct->tm_hour, ct->tm_min );

    strcpy( BackupPath, DataPath );
    char* pDot = strrchr( BackupPath, '.' );
    if( !pDot )
      pDot = strrchr( BackupPath, '\0' );

    strcpy( pDot, dtm );
    pDot += strlen( dtm );
    strcpy( pDot, ".bac" );

    DataStream.close();
    bSucc = CopyFile( DataPath, BackupPath, 0 );
    DataStream.open( DataPath, ios::in|ios::out|ios::binary, filebuf::sh_read || filebuf::sh_write );
  }

  catch(...)
  {
    bSucc = 0;
  }
  return bSucc ? TraceError( MapNumber, R_OK, opname ) :
                 TraceError( MapNumber, R_BACKUPERROR, opname );
}
//=====================================================================
ResultCode
MapStore::GetProjectHeadRecord( TGstream& dst )
{
  if( !DataStream.good())
    return TraceError( MapNumber, R_BADSTREAM, "GetProjectHeadRecord" );

  dst.write((char*)&HR, sizeof HR );
  return R_OK;
}
//=====================================================================
int
MapStore::IsObjectValid()
{
  void* src = pData;
  void* end = pData + pDrh->GetDataSize();
  int r = IsDataValid( &src, end );
//  ASSERT( r || src == end );
  return r;
}
//=====================================================================
int
MapStore::IsDataValid( void** ppSrc, void* pEndObj )
{
  PrimRecord* pPr = (PrimRecord*)*ppSrc;

  if(( pPr->type & PF_TYPE_MASK ) != PF_CONTAINER )
    return 0;

  *ppSrc = pPr->param.container.prims;
  int n = pPr->param.container.length;
  int bValid = 1;

  while( --n >= 0 && bValid )
  {
    pPr = (PrimRecord*)*ppSrc;
    Drawable* pObjPar  = (Drawable*)&pPr->param;

    int w = 0;
    int subType = (( pPr->type & PF_STROKED ) ? STROKED : 0 ) |
                  (( pPr->type & PF_FILLED  ) ? FILLED  : 0 ) |
                  (( pPr->type & PF_ROUND   ) ? ROUND   : 0 );
    int typeKey  = pPr->type & PF_TYPE_MASK;
    int bVisible = pPr->type & PF_VISIBLE;

    if( bVisible  )
    {
      switch( subType & OUTLINED )
      {
        case STROKED :
          pObjPar = &pPr->param.stroked.primParam;
          break;
        case FILLED :
          pObjPar = &pPr->param.filled.primParam;
          break;
        case OUTLINED :
          pObjPar = &pPr->param.outlined.primParam;
        break;
      }
    }

    switch( typeKey )
    {
      case PF_CONTAINER:
         bValid = IsDataValid( ppSrc, pEndObj );
         continue;
      case PF_POLYPOINT:
      case PF_FOOTNOTE:
      {
        int nPoints = pObjPar->polypoint.length;
        Point* pPoints = (Point*)pObjPar->polypoint.points;
        *ppSrc = pPoints + nPoints;
        break;
      }
      case PF_TEXT:
      {
        StringParam* pSP = bVisible ?
          &pPr->param.text.param.full.string :
          &pPr->param.text.param.string;
        Text::StrPar::FulPar::FntPar* pFP = &pPr->param.text.param.full.FontParams;
        *ppSrc = pSP->string + pSP->length;
        break;
      }
      case PF_TEXTBYLINE:
      {
        TextByLine::FntPar* pFP = &pPr->param.textbyline.FontParams;
        int length = pPr->param.textbyline.length;
        TxtItem* pItem = pPr->param.textbyline.items;
        *ppSrc = pItem + length;
        break;
      }
      case PF_MULTILINE:
        *ppSrc = pObjPar->polypoint.points + pObjPar->polypoint.length;
        break;
      case PF_POINT:
        *ppSrc = &pPr->param.pnt + 1;
        break;
      case PF_ICON:
        *ppSrc = &pPr->param.icon + 1;
        break;
      case PF_ELLIPSE :
        *ppSrc = &pObjPar->ellipse + 1;
        break;
      case PF_RECTANGLE :
        *ppSrc = &pObjPar->rectangle + 1;
        break;
      case PF_ARC :
        *ppSrc = &pObjPar->arc + 1;
        break;
    }
    bValid = *ppSrc <= pEndObj;
  }
  return bValid;
}
//=====================================================================
