  //=====================================================================
// описание структуры файла графических данных
//=====================================================================
#ifndef _FILESTR_H_
#define _FILESTR_H_
//=====================================================================
//#include <fstream.h>
#include <time.h>
#include "t.h"
#include "ptrect.h"
#include "ident.h"
//=====================================================================
#pragma pack( push, 1 )
//=====================================================================
#define HEAD_SIZE 256          // размер заголовочной части файла
#define IDENT_TEXT_SIZE 16     // размер идентификационной записи
//== параметры по по умолчанию=========================================
#define DEFVERSION  111        // версия
#define DEFINCSIZE  512        // размер приращения файла
#define DEFUNITDEC  1          // цена мл. разряда в см.
#define DEFXSIZE    1000000    // X габарит
#define DEFYSIZE    1000000    // Y габарит
#define DEFMAXSCALE 100        // максимальный масштаб 
#define DEFMINSCALE 1000000    // минимальный масштаб 
#define CLASS_RANGE    1024
#define SUBLAYER_CLASS CLASS_RANGE
#define SUBLR_RANGE    4
#define SUBSTAT_CLASS  (CLASS_RANGE+SUBLR_RANGE+1)
#define SUBSTAT_RANGE  16
#define FULL_RANGE     (SUBSTAT_CLASS+SUBSTAT_RANGE+1)
#define BIT_FULL_RANGE ((FULL_RANGE>>3)+1)
//=====================================================================
//=====================================================================
// Версии:
// хранение полилинейных в нечетных версиях -- в координатах
//                           четных версиях -- в приращениях
// 001 - исходная
// 101 - изменена структура классов
// 103 - изменена структура заголовка
// 107 - изменена структура шрифта
// 109 - добавлен номер узла
// 111 - изменена структура идентификатора
//=====================================================================
enum
{
  OBJMAGIC = 0x41544144L,  // код начала записи действующего объекта
  EMPMAGIC = 0x54504D45L,  // код начала записи удаленного   объекта
  INSMAGIC = 0x52534E49L,  // код записи новых объектов
  REPMAGIC = 0x4C504552L   // код записи cуществующих объектов
};
//=====================================================================
class CBaseDataRec
{
public:
  virtual dword      GetMagic()     = 0; // код начала записи объекта
  virtual void*      GetRecPtr()    = 0; // начало записи
  virtual void*      GetBriefPtr()  = 0; // начало укороченной записи
  virtual long       GetRecSize()   = 0; // размер записи
  virtual long       GetBriefSize() = 0; // размер укороченной записи
  virtual long       GetSize()      = 0; // полный размер записи в байтах
  virtual long       SizeOffset()   = 0; // смещение размер записи
  virtual RECT       GetBox()       = 0; // габарит
  virtual ClassType  GetClass()     = 0; // класс
  virtual StatusType GetStatus()    = 0; // статус
  virtual TableType  GetTable()     = 0; // таблица
  virtual EntryType  GetEntry()     = 0; // запись
  CIdent     GetIdent()    { return CIdent( GetClass(), GetStatus(), GetTable(), GetEntry()); }
  CTabIdent  GetTabIdent() { return CTabIdent( GetTable(), GetEntry()); }
	void SetTabIdent( CTabIdent id ) { SetTable( id.Table()); SetEntry( id.Entry()); }

  virtual void SetMagic( dword )          = 0;
  virtual void SetSize( long )            = 0;
  virtual void SetIdent( CIdent const& )  = 0;
  virtual void SetEntry( EntryType ent )  = 0;
  virtual void SetTable( TableType tab )  = 0;
  virtual void SetObject( EntryType obj ) = 0;
  virtual void SetBox( RECT const& )      = 0;
  virtual void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent ) = 0;

  long GetDataSize() { return GetSize() - GetRecSize(); }
  int IsObjRec() { return GetMagic() == OBJMAGIC; }
  int IsEmpRec() { return GetMagic() == EMPMAGIC; }

  virtual dword GetClock() { return 0; }
  virtual void  SetClock( dword c ) {}

  virtual dword GetTime() { return 0; }
  virtual void  SetTime( dword t ) {}

  virtual int GetNode() { return 0; }
  virtual void SetNode( int n ) {}

  int Invalid();
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
  RECT        Box;
};

struct TagRec_0103
{
  dword       Magic;
  dword       Size;
  TagIdentOld Ident;
  TagTabIdent TabIdent;
  dword       Time;
  dword       Clock;
  RECT        Box;
};

struct TagRec_0109
{
  dword       Magic;
  dword       Size;
  word        Node;
  TagIdentOld Ident;
  TagTabIdent TabIdent;
  dword       Time;
  dword       Clock;
  RECT        Box;
};

struct TagRec_0111
{
  dword       Magic;
  dword       Size;
  word        Node;
  TagIdent    Ident;
  TagTabIdent TabIdent;
  dword       Time;
  dword       Clock;
  RECT        Box;
};

int const TagRecSize = max( sizeof TagRec_0101,
                       max( sizeof TagRec_0103,
                       max( sizeof TagRec_0109,
                       max( sizeof TagRec_0111, 0 ))));
//=====================================================================
class GDataRec_0101 : public CBaseDataRec
{
public:
  dword GetMagic()        { return rec.Magic; }
  void* GetBriefPtr()     { return &rec.Size; }
  void* GetRecPtr()       { return &rec; }
  long  GetRecSize()      { return sizeof rec; }
  long  GetBriefSize()    { return sizeof rec - sizeof rec.Magic - sizeof rec.Box; }
  long  GetSize()         { return rec.Size; }
  long  SizeOffset()      { return sizeof rec.Magic; }
  RECT  GetBox()          { return rec.Box; }
  ClassType  GetClass()   { return rec.Ident.m_Class; }
  StatusType GetStatus()  { return rec.Ident.m_Status; }
  TableType  GetTable()   { return rec.TabIdent.m_Table; }
  EntryType  GetEntry()   { return rec.TabIdent.m_Entry; }

  void SetMagic( dword v ) { rec.Magic = v; }
  void SetSize( long v )   { rec.Size  = v; }
  void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent )
                           { rec.Ident.m_Class  = cls;
                             rec.Ident.m_Status = sts;
                             rec.TabIdent.m_Table = tbl;
                             rec.TabIdent.m_Entry = ent; }
  void SetIdent( CIdent const& id ) { SetIdent( id.Class(), id.Status(), id.Table(), id.Entry()); }
  void SetEntry( EntryType ent )  { rec.TabIdent.m_Entry = ent; }
  void SetTable( TableType tab )  { rec.TabIdent.m_Table = tab; }
  void SetObject( EntryType obj ) { rec.Ident.m_Object = obj; }
  void SetBox( RECT const& v )    { rec.Box = v; }

private:
  TagRec_0101 rec;
};
//=====================================================================
class GDataRec_0103 : public CBaseDataRec
{
public:
  dword GetMagic()        { return rec.Magic; }
  void* GetRecPtr()       { return &rec; }
  void* GetBriefPtr()     { return &rec.Size; }
  long GetRecSize()       { return sizeof rec; }
  long GetBriefSize()     { return sizeof rec - sizeof rec.Magic - sizeof rec.Box; }
  long GetSize()          { return rec.Size; }
  long SizeOffset()       { return sizeof rec.Magic; }
  RECT  GetBox()          { return rec.Box; }
  ClassType  GetClass()   { return rec.Ident.m_Class; }
  StatusType GetStatus()  { return rec.Ident.m_Status; }
  TableType  GetTable()   { return rec.TabIdent.m_Table; }
  EntryType  GetEntry()   { return rec.TabIdent.m_Entry; }

  dword GetClock()            { return rec.Clock; }
  void SetClock( dword c )    { rec.Clock = c; }
  dword GetTime()             { return rec.Time; }
  void SetTime( dword t )     { rec.Time = t; }

  void SetMagic( dword v )    { rec.Magic = v; }
  void SetSize( long v )      { rec.Size  = v; }
  void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent )
                               { rec.Ident.m_Class  = cls;
                                 rec.Ident.m_Status = sts;
                                 rec.TabIdent.m_Table = tbl;
                                 rec.TabIdent.m_Entry = ent; }
  void SetIdent( CIdent const& id ) { SetIdent( id.Class(), id.Status(), id.Table(), id.Entry()); }
  void SetEntry( EntryType ent )  { rec.TabIdent.m_Entry = ent; }
  void SetTable( TableType tab )  { rec.TabIdent.m_Table = tab; }
  void SetObject( EntryType obj ) { rec.Ident.m_Object = obj; }
  void SetBox( RECT const& v )    { rec.Box = v; }
protected:
  TagRec_0103 rec;
};
//=====================================================================
class GDataRec_0107 : public GDataRec_0103
{
public:
  long  GetBriefSize() { return sizeof rec - sizeof rec.Magic; }
};
//=====================================================================
class GDataRec_0109 : public CBaseDataRec
{
public:
  dword GetMagic()        { return rec.Magic; }
  void* GetRecPtr()       { return &rec; }
  void* GetBriefPtr()     { return &rec.Size; }
  long GetRecSize()       { return sizeof rec; }
  long GetBriefSize()     { return sizeof rec - sizeof rec.Magic; }
  long GetSize()          { return rec.Size; }
  long SizeOffset()       { return sizeof rec.Magic; }
  RECT  GetBox()          { return rec.Box; }
  ClassType  GetClass()   { return rec.Ident.m_Class; }
  StatusType GetStatus()  { return rec.Ident.m_Status; }
  TableType  GetTable()   { return rec.TabIdent.m_Table; }
  EntryType  GetEntry()   { return rec.TabIdent.m_Entry; }

  dword GetClock()            { return rec.Clock; }
  void SetClock( dword c  )   { rec.Clock = c; }
  dword GetTime()             { return rec.Time; }
  void SetTime(dword t )      { rec.Time = t; }
  int  GetNode()              { return rec.Node; }
  void SetNode( int n )       { rec.Node = n; }

  void SetMagic( dword v )    { rec.Magic = v; }
  void SetSize( long v )      { rec.Size  = v; }
  void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent )
                              { rec.Ident.m_Class  = cls;
                                rec.Ident.m_Status = sts;
                                rec.TabIdent.m_Table = tbl;
                                rec.TabIdent.m_Entry = ent; }
  void SetIdent( CIdent const& id ) { SetIdent( id.Class(), id.Status(), id.Table(), id.Entry()); }
  void SetEntry( EntryType ent )  { rec.TabIdent.m_Entry = ent; }
  void SetTable( TableType tab )  { rec.TabIdent.m_Table = tab; }
  void SetObject( EntryType obj ) { rec.Ident.m_Object = obj; }
  void SetBox( RECT const& v )    { rec.Box = v; }

protected:
  TagRec_0109 rec;
};
//=====================================================================
class GDataRec_0111 : public CBaseDataRec
{
public:
  dword GetMagic()        { return rec.Magic; }
  void* GetRecPtr()       { return &rec; }
  void* GetBriefPtr()     { return &rec.Size; }
  long GetRecSize()       { return sizeof rec; }
  long GetBriefSize()     { return sizeof rec - sizeof rec.Magic; }
  long GetSize()          { return rec.Size; }
  long SizeOffset()       { return sizeof rec.Magic; }
  RECT  GetBox()          { return rec.Box; }
  ClassType  GetClass()   { return rec.Ident.m_Class; }
  StatusType GetStatus()  { return rec.Ident.m_Status; }
  TableType  GetTable()   { return rec.TabIdent.m_Table; }
  EntryType  GetEntry()   { return rec.TabIdent.m_Entry; }

  dword GetClock()            { return rec.Clock; }
  void SetClock(dword c  )    { rec.Clock = c; }
  dword GetTime()             { return rec.Time; }
  void SetTime(dword t )      { rec.Time = t; }
  int  GetNode()              { return rec.Node; }
  void SetNode( int n )       { rec.Node = n; }
  void SetMagic( dword v )    { rec.Magic = v; }
  void SetSize( long v )      { rec.Size  = v; }
  void SetIdent( ClassType cls, StatusType sts, TableType tbl, EntryType ent )
                                         { rec.Ident.m_Class  = cls;
                                           rec.Ident.m_Status = sts;
                                           rec.TabIdent.m_Table = tbl;
                                           rec.TabIdent.m_Entry = ent; }
  void SetIdent( CIdent const& id ) { SetIdent( id.Class(), id.Status(), id.Table(), id.Entry()); }
  void SetEntry( EntryType ent )  { rec.TabIdent.m_Entry = ent; }
  void SetTable( TableType tab )  { rec.TabIdent.m_Table = tab; }
  void SetObject( EntryType obj ) {}
  void SetBox( RECT const& v )    { rec.Box = v; }
protected:
  TagRec_0111 rec;
};
//=====================================================================
// структура заголовочной записи проекта
struct DataFileHead
{
  char Ident[IDENT_TEXT_SIZE]; // идентификационный текст
  long Version;                // код версии
  long FreePos;                // смещение позиции записи
  long IncSize;                // квант увеличения размера
  long IndexValid;             // флаг валидности индексных файлов
  long XSize;                  // размер по X
  long YSize;                  // размер по Y
  long unused;                 // не исп.
  long UnitDec;                // цена мл. разряда в см.
  
  char PrjType[4];             // наимнование проекции ("MERK" или <пусто>
  char Parallel_1[12];         // широта для единичного масштаба в градусах
  char Semimajor[16];          // большая полуось в единицах проекта
  char Semiminor[16];          // меньшая полуось в единицах проекта

  char XC0[16];                // X-координата базовой точки в единицах проекта
  char YC0[16];                // Y-координата базовой точки в единицах проекта
  char LN0[16];                // долгота базовой точки в градусах 
  char LT0[16];                // широта  базовой точки в градусах 

  long MinScale;               // минимальный  масштаб отображения
  long MaxScale;               // максимальный масштаб отображения
};

struct DataFileHeadRec : public DataFileHead
{
  byte Reserved[HEAD_SIZE-sizeof(DataFileHead)]; // резерв
};
//=====================================================================
#pragma pack( pop )
//=====================================================================
#endif
//=====================================================================
