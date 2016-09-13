// TGstream.h: interface for the TGstream class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TGSTREAM_H_
#define _TGSTREAM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "strstrea.h"

#define MIN_BUF_SIZE 0x4000000

class  TGstream : public iostream
{
  strstreambuf buf;

public:
  TGstream() : buf( MIN_BUF_SIZE ), iostream((streambuf*)&buf){}
  TGstream( long GrowBy ) : buf( GrowBy ), iostream(( streambuf*)&buf){}
  ~TGstream() { buf.freeze(0); }

  int pcount() const { return rdbuf()->out_waiting(); } // not in spec.
  strstreambuf* rdbuf() const { return (strstreambuf*) ostream::rdbuf(); }
  char* str() { return rdbuf()->str(); }
};

#endif
