  //=====================================================================
// �������� ��������� ����� ����������� ������
//=====================================================================
#ifndef _FILESTR_H_
#define _FILESTR_H_
//=====================================================================
#include <fstream>
#include <time.h>
#include "t.h"
#include "ptrect.h"
#include "ident.h"
//=====================================================================
#pragma pack( push, 1 )
//=====================================================================
#define HEAD_SIZE 256          // ������ ������������ ����� �����
#define IDENT_TEXT_SIZE 16     // ������ ����������������� ������
//== ��������� �� �� ���������=========================================
#define DEFVERSION  111        // ������
#define DEFINCSIZE  512        // ������ ���������� �����
#define DEFUNITDEC  1          // ���� ��. ������� � ��.
#define DEFXSIZE    1000000    // X �������
#define DEFYSIZE    1000000    // Y �������
#define DEFMAXSCALE 100        // ������������ ������� 
#define DEFMINSCALE 1000000    // ����������� ������� 
#define CLASS_RANGE    1024
#define SUBLAYER_CLASS CLASS_RANGE
#define SUBLR_RANGE    4
#define SUBSTAT_CLASS  (CLASS_RANGE+SUBLR_RANGE+1)
#define SUBSTAT_RANGE  16
#define FULL_RANGE     (SUBSTAT_CLASS+SUBSTAT_RANGE+1)
#define BIT_FULL_RANGE ((FULL_RANGE>>3)+1)
//=====================================================================
//=====================================================================
// ������:
// �������� ������������ � �������� ������� -- � �����������
//                           ������ ������� -- � �����������
// 001 - ��������
// 101 - �������� ��������� �������
// 103 - �������� ��������� ���������
// 107 - �������� ��������� ������
// 109 - �������� ����� ����
// 111 - �������� ��������� ��������������
//=====================================================================
enum
{
  OBJMAGIC = 0x41544144L,  // ��� ������ ������ ������������ �������
  EMPMAGIC = 0x54504D45L,  // ��� ������ ������ ����������   �������
  INSMAGIC = 0x52534E49L,  // ��� ������ ����� ��������
  REPMAGIC = 0x4C504552L   // ��� ������ c����������� ��������
};
//=====================================================================
class CBaseDataRec
{
public:
  virtual dword      GetMagic()     = 0; // ��� ������ ������ �������
  virtual void*      GetRecPtr()    = 0; // ������ ������
  virtual void*      GetBriefPtr()  = 0; // ������ ����������� ������
  virtual int        GetRecSize()   = 0; // ������ ������
  virtual int        GetBriefSize() = 0; // ������ ����������� ������
  virtual int        GetSize()      = 0; // ������ ������ ������ � ������
  virtual int        SizeOffset()   = 0; // �������� ������ ������
  virtual Rect&      GetBox()       = 0; // �������
  virtual ClassType  GetClass()     = 0; // �����
  virtual StatusType GetStatus()    = 0; // ������
  virtual TableType  GetTable()     = 0; // �������
  virtual EntryType  GetEntry()     = 0; // ������
  CIdent     GetIdent()    { return CIdent( GetClass(), GetStatus(), GetTable(), GetEntry()); }
  CTabIdent  GetTabIdent() { return CTabIdent( GetTable(), GetEntry()); }
	void SetTabIdent( CTabIdent id ) { SetTable( id.Table()); SetEntry( id.Entry()); }

  virtual void SetMagic( dword )          = 0;
  virtual void SetSize( int )             = 0;
  virtual void SetIdent( CIdent const& )  = 0;
  virtual void SetEntry( EntryType ent )  = 0;
  virtual void SetTable( TableType tab )  = 0;
  virtual void SetObject( EntryType obj ) = 0;
  virtual void SetBox( Rect const& )      = 0;
  virtual void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent ) = 0;

  int GetDataSize() { return GetSize() - GetRecSize(); }
  int IsObjRec() { return GetMagic() == OBJMAGIC; }
  int IsEmpRec() { return GetMagic() == EMPMAGIC; }

  virtual clock_t GetClock() { return 0; }
  virtual void SetClock( clock_t c ) {}

  virtual time_t GetTime() { return 0; }
  virtual void   SetTime( time_t t ) {}

  virtual int GetNode() { return 0; }
  virtual void SetNode( int n ) {}

  int Invalid()
  {
    return !IsObjRec() && !IsEmpRec() ? 1 :
           GetSize() <= GetRecSize() ? 2 :
           GetBox().left > GetBox(). right ||
           GetBox().top > GetBox().bottom ? 3 :
           GetClass() >= FULL_RANGE ? 4 : 0;
  }
  void CopyRec( CBaseDataRec* pSrc )
  {
    if( pSrc != this )
    {
      SetMagic( pSrc->GetMagic());
      SetSize( pSrc->GetSize() + GetRecSize() - pSrc->GetRecSize());
      SetIdent( pSrc->GetIdent());
      SetBox( pSrc->GetBox());
      SetClock( pSrc->GetClock());
      SetTime( pSrc->GetTime());
      SetNode( pSrc->GetNode());
    }
  }
};
//=====================================================================
typedef CBaseDataRec* CBaseDataRecPtr;
//=====================================================================
struct TagRec_0101
{
  dword       Magic;
  word        Size;
  TagIdentOld Ident;
  TagTabIdent TabIdent;
  Rect        Box;
};

struct TagRec_0103
{
  dword       Magic;
  dword       Size;
  TagIdentOld Ident;
  TagTabIdent TabIdent;
  time_t      Time;
  clock_t     Clock;
  Rect        Box;
};

struct TagRec_0109
{
  dword       Magic;
  dword       Size;
  word        Node;
  TagIdentOld Ident;
  TagTabIdent TabIdent;
  time_t      Time;
  clock_t     Clock;
  Rect        Box;
};

struct TagRec_0111
{
  dword       Magic;
  dword       Size;
  word        Node;
  TagIdent    Ident;
  TagTabIdent TabIdent;
  time_t      Time;
  clock_t     Clock;
  Rect        Box;
};

int const TagRecSize = max( sizeof TagRec_0101,
                       max( sizeof TagRec_0103,
                       max( sizeof TagRec_0109,
                       max( sizeof TagRec_0111, 0 ))));
//=====================================================================
class GDataRec_0101 : public CBaseDataRec
{
public:
  dword GetMagic()       { return rec.Magic; }
  void* GetBriefPtr()    { return &rec.Size; }
  void* GetRecPtr()      { return &rec; }
  int   GetRecSize()     { return sizeof rec; }
  int   GetBriefSize()   { return sizeof rec - sizeof rec.Magic - sizeof rec.Box; }
  int   GetSize()        { return rec.Size; }
  int   SizeOffset()     { return sizeof rec.Magic; }
  Rect&      GetBox()    { return rec.Box; }
  ClassType  GetClass()  { return rec.Ident.m_Class; }
  StatusType GetStatus() { return rec.Ident.m_Status; }
  TableType  GetTable()  { return rec.TabIdent.m_Table; }
  EntryType  GetEntry()  { return rec.TabIdent.m_Entry; }

  void SetMagic( dword v ) { rec.Magic = v; }
  void SetSize( int v )    { rec.Size  = v; }
  void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent )
                           { rec.Ident.m_Class  = cls;
                             rec.Ident.m_Status = sts;
                             rec.TabIdent.m_Table = tbl;
                             rec.TabIdent.m_Entry = ent; }
  void SetIdent( CIdent const& id ) { SetIdent( id.Class(), id.Status(), id.Table(), id.Entry()); }
  void SetEntry( EntryType ent )  { rec.TabIdent.m_Entry = ent; }
  void SetTable( TableType tab )  { rec.TabIdent.m_Table = tab; }
  void SetObject( EntryType obj ) { rec.Ident.m_Object = obj; }
  void SetBox( Rect const& v )    { rec.Box = v; }

private:
  TagRec_0101 rec;
};
//=====================================================================
class GDataRec_0103 : public CBaseDataRec
{
public:
  dword GetMagic()       { return rec.Magic; }
  void* GetRecPtr()      { return &rec; }
  void* GetBriefPtr()    { return &rec.Size; }
  int   GetRecSize()     { return sizeof rec; }
  int   GetBriefSize()   { return sizeof rec - sizeof rec.Magic - sizeof rec.Box; }
  int   GetSize()        { return rec.Size; }
  int   SizeOffset()     { return sizeof rec.Magic; }
  Rect&      GetBox()    { return rec.Box; }
  ClassType  GetClass()  { return rec.Ident.m_Class; }
  StatusType GetStatus() { return rec.Ident.m_Status; }
  TableType  GetTable()  { return rec.TabIdent.m_Table; }
  EntryType  GetEntry()  { return rec.TabIdent.m_Entry; }

  clock_t GetClock()          { return rec.Clock; }
  void SetClock( clock_t c  ) { rec.Clock = c; }
  time_t GetTime()            { return rec.Time; }
  void SetTime( time_t t )    { rec.Time = t; }

  void SetMagic( dword v )     { rec.Magic = v; }
  void SetSize( int v )        { rec.Size  = v; }
  void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent )
                               { rec.Ident.m_Class  = cls;
                                 rec.Ident.m_Status = sts;
                                 rec.TabIdent.m_Table = tbl;
                                 rec.TabIdent.m_Entry = ent; }
  void SetIdent( CIdent const& id ) { SetIdent( id.Class(), id.Status(), id.Table(), id.Entry()); }
  void SetEntry( EntryType ent )  { rec.TabIdent.m_Entry = ent; }
  void SetTable( TableType tab )  { rec.TabIdent.m_Table = tab; }
  void SetObject( EntryType obj ) { rec.Ident.m_Object = obj; }
  void SetBox( Rect const& v )    { rec.Box = v; }
protected:
  TagRec_0103 rec;
};
//=====================================================================
class GDataRec_0107 : public GDataRec_0103
{
public:
  int GetBriefSize() { return sizeof rec - sizeof rec.Magic; }
};
//=====================================================================
class GDataRec_0109 : public CBaseDataRec
{
public:
  dword GetMagic()       { return rec.Magic; }
  void* GetRecPtr()      { return &rec; }
  void* GetBriefPtr()    { return &rec.Size; }
  int   GetRecSize()     { return sizeof rec; }
  int   GetBriefSize()   { return sizeof rec - sizeof rec.Magic; }
  int   GetSize()        { return rec.Size; }
  int   SizeOffset()     { return sizeof rec.Magic; }
  Rect&      GetBox()    { return rec.Box; }
  ClassType  GetClass()  { return rec.Ident.m_Class; }
  StatusType GetStatus() { return rec.Ident.m_Status; }
  TableType  GetTable()  { return rec.TabIdent.m_Table; }
  EntryType  GetEntry()  { return rec.TabIdent.m_Entry; }

  clock_t GetClock()          { return rec.Clock; }
  void SetClock( clock_t c  ) { rec.Clock = c; }
  time_t GetTime()            { return rec.Time; }
  void SetTime( time_t t )    { rec.Time = t; }
  int  GetNode()              { return rec.Node; }
  void SetNode( int n )       { rec.Node = n; }

  void SetMagic( dword v )    { rec.Magic = v; }
  void SetSize( int v )       { rec.Size  = v; }
  void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent )
                              { rec.Ident.m_Class  = cls;
                                rec.Ident.m_Status = sts;
                                rec.TabIdent.m_Table = tbl;
                                rec.TabIdent.m_Entry = ent; }
  void SetIdent( CIdent const& id ) { SetIdent( id.Class(), id.Status(), id.Table(), id.Entry()); }
  void SetEntry( EntryType ent )  { rec.TabIdent.m_Entry = ent; }
  void SetTable( TableType tab )  { rec.TabIdent.m_Table = tab; }
  void SetObject( EntryType obj ) { rec.Ident.m_Object = obj; }
  void SetBox( Rect const& v )    { rec.Box = v; }

protected:
  TagRec_0109 rec;
};
//=====================================================================
class GDataRec_0111 : public CBaseDataRec
{
public:
  dword GetMagic()       { return rec.Magic; }
  void* GetRecPtr()      { return &rec; }
  void* GetBriefPtr()    { return &rec.Size; }
  int   GetRecSize()     { return sizeof rec; }
  int   GetBriefSize()   { return sizeof rec - sizeof rec.Magic; }
  int   GetSize()        { return rec.Size; }
  int   SizeOffset()     { return sizeof rec.Magic; }
  Rect&      GetBox()    { return rec.Box; }
  ClassType  GetClass()  { return rec.Ident.m_Class; }
  StatusType GetStatus() { return rec.Ident.m_Status; }
  TableType  GetTable()  { return rec.TabIdent.m_Table; }
  EntryType  GetEntry()  { return rec.TabIdent.m_Entry; }

  clock_t GetClock()          { return rec.Clock; }
  void SetClock( clock_t c  ) { rec.Clock = c; }
  time_t GetTime()            { return rec.Time; }
  void SetTime( time_t t )    { rec.Time = t; }
  int  GetNode()              { return rec.Node; }
  void SetNode( int n )       { rec.Node = n; }
  void SetMagic( dword v )    { rec.Magic = v; }
  void SetSize( int v )       { rec.Size  = v; }
  void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent )
                                         { rec.Ident.m_Class  = cls;
                                           rec.Ident.m_Status = sts;
                                           rec.TabIdent.m_Table = tbl;
                                           rec.TabIdent.m_Entry = ent; }
  void SetIdent( CIdent const& id ) { SetIdent( id.Class(), id.Status(), id.Table(), id.Entry()); }
  void SetEntry( EntryType ent )  { rec.TabIdent.m_Entry = ent; }
  void SetTable( TableType tab )  { rec.TabIdent.m_Table = tab; }
  void SetObject( EntryType obj ) {}
  void SetBox( Rect const& v )    { rec.Box = v; }
protected:
  TagRec_0111 rec;
};
//=====================================================================
// ��������� ������������ ������ �������
struct DataFileHead
{
  char Ident[IDENT_TEXT_SIZE]; // ����������������� �����
  long Version;                // ��� ������
  long FreePos;                // �������� ������� ������
  long IncSize;                // ����� ���������� �������
  long IndexValid;             // ���� ���������� ��������� ������
  long XSize;                  // ������ �� X
  long YSize;                  // ������ �� Y
  long unused;                 // �� ���.
  long UnitDec;                // ���� ��. ������� � ��.
  
  char PrjType[4];             // ����������� �������� ("MERK" ��� <�����>
  char Parallel_1[12];         // ������ ��� ���������� �������� � ��������
  char Semimajor[16];          // ������� ������� � �������� �������
  char Semiminor[16];          // ������� ������� � �������� �������

  char XC0[16];                // X-���������� ������� ����� � �������� �������
  char YC0[16];                // Y-���������� ������� ����� � �������� �������
  char LN0[16];                // ������� ������� ����� � �������� 
  char LT0[16];                // ������  ������� ����� � �������� 

  long MinScale;               // �����������  ������� �����������
  long MaxScale;               // ������������ ������� �����������
};

struct DataFileHeadRec : public DataFileHead
{
  byte Reserved[HEAD_SIZE-sizeof(DataFileHead)]; // ������
};
//=====================================================================
#pragma pack( pop )
//=====================================================================
#endif
//=====================================================================