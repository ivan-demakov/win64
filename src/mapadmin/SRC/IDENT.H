#ifndef _IDENT_H_
#define _IDENT_H_
//=====================================================================
typedef unsigned long  IdentType;
typedef unsigned short ClassType;
typedef unsigned char  StatusType;
typedef unsigned long  TableType;
typedef unsigned long  EntryType;
//=====================================================================
#pragma pack( push, 1 )
//=====================================================================
struct TagIdentOld
{
  ClassType  m_Class;
  StatusType m_Status;
  IdentType  m_Object;
};
//=====================================================================
struct TagTabIdent
{
  TableType m_Table;
  EntryType m_Entry;
};
//=====================================================================
struct TagIdent
{
  ClassType  m_Class;
  StatusType m_Status;
};
//=====================================================================
#pragma pack( pop )
//=====================================================================
class CTabIdent
{
public:
  CTabIdent( TableType table = 0, EntryType entry = 0 )
  {
    m_Tag.m_Table = table;
    m_Tag.m_Entry = entry;
  }

  CTabIdent& operator=( CTabIdent id ){ m_Tag.m_Table = id.Table();
                                        m_Tag.m_Entry = id.Entry();
                                        return *this; }
  void* GetTagPtr()  { return &m_Tag; }
  int   GetTagSize() { return sizeof m_Tag; }

  TableType Table() const { return m_Tag.m_Table;  }
  EntryType Entry() const { return m_Tag.m_Entry;  }

  void SetTable( TableType t ) { m_Tag.m_Table = t; }
  void SetEntry( EntryType e ) { m_Tag.m_Entry = e; }
  void SetNull() { m_Tag.m_Table = 0; m_Tag.m_Entry = 0; }
  int  IsNull() const { return m_Tag.m_Table == 0; }
  int  IsEmpty() const { return m_Tag.m_Table == 0 && m_Tag.m_Entry == 0; }

  int operator==( CTabIdent id ) const { return Table() == id.Table() &&
                                                Entry() == id.Entry(); }
  int operator<( CTabIdent id )  const { return Table() <  id.Table() ||
                                                Table() == id.Table() &&
                                                Entry() <  id.Entry(); }
  int operator>( CTabIdent id )  const { return Table() >  id.Table() ||
                                                Table() == id.Table() &&
                                                Entry() >  id.Entry(); }
  int operator!=( CTabIdent id ) const { return !( *this == id ); }
  int operator>=( CTabIdent id ) const { return !( *this < id ); }
  int operator<=( CTabIdent id ) const { return !( *this > id ); }

private:
  TagTabIdent m_Tag;
};
//=====================================================================
class CIdent : public CTabIdent
{
public:
  CIdent( CIdent const& id )
  {
    SetIdent( id.Class(), id.Status(), id.Table(), id.Entry());
  }

  CIdent( ClassType cls = 0, StatusType sts = 0,
          TableType tbl = 0, EntryType ent = 0 )
  {
    SetIdent( cls, sts, tbl, ent );
  }
  CIdent& operator=( CIdent const& id ) 
  {
    SetIdent( id.Class(), id.Status(), id.Table(), id.Entry());
    return *this;
  }

  void* GetTagPtr()  { return &m_Tag; }
  int   GetTagSize() { return sizeof m_Tag; }

  void SetClass( int s )  { m_Tag.m_Class  = s; }
  void SetStatus( int s ) { m_Tag.m_Status = s; }
  void SetIdent( ClassType cls = 0, StatusType sts = 0,
                 TableType tbl = 0, EntryType ent = 0 )
  {
    SetClass( cls );
    SetStatus( sts );
    SetTable( tbl );
    SetEntry( ent );
  }

  ClassType  Class()  const { return m_Tag.m_Class;  }
  StatusType Status() const { return m_Tag.m_Status; }
  CTabIdent  TabIdent() const { return CTabIdent( Table(), Entry()); }

  int operator==( CIdent id ) const { return Class() == id.Class() &&
                                             Table() == id.Table() &&
                                             Entry() == id.Entry(); }
  int operator!=( CIdent id ) const { return !( *this == id ); }

private:
  TagIdent m_Tag;
};
//=====================================================================
#endif