// TGstream.h: interface for the TGstream class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TGSTREAM_H_
#define _TGSTREAM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <strstream>
#include <iostream>
#include <sstream>
using namespace std;

#define MIN_BUF_SIZE 0x400000
#if 1
class  TGstream : public iostream
{
  strstreambuf buf;

public:
  TGstream( long size = MIN_BUF_SIZE ) : buf( size ), iostream( (streambuf*)&buf ) {}
  ~TGstream() { buf.freeze(0); }

  streampos pcount() const { return rdbuf()->pcount(); }
  strstreambuf* rdbuf() const { return (strstreambuf*)ostream::rdbuf(); }
  char* str() { return rdbuf()->str(); }
};
#else
class  TGstream : public iostream
{
  stringbuf buf;

public:
  TGstream( long size = MIN_BUF_SIZE ) : 
    iostream( &buf )
  {}

  streampos pcount() const { return rdbuf()->str().length(); }
  stringbuf * rdbuf() const { return (stringbuf *)ostream::rdbuf(); }
  char* str() { return (char*)rdbuf()->str().data(); }
};
#endif
#endif
