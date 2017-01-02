// TGstream.h: interface for the TGstream class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TGSTREAM_H_
#define _TGSTREAM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "strstream"

#define MIN_BUF_SIZE 0x4000000

class  TGstream : public std::iostream
{
  std::strstreambuf buf;

public:
  TGstream() : buf( MIN_BUF_SIZE ), std::iostream((std::streambuf*)&buf){}
  TGstream( long GrowBy ) : buf( GrowBy ), std::iostream(( std::streambuf*)&buf){}
  ~TGstream() { buf.freeze(0); }

#ifndef ivan
  int pcount() const { return 0; } // not in spec.
#else
  int pcount() const { return rdbuf()->out_waiting(); } // not in spec.
#endif // ivan

  std::strstreambuf* rdbuf() const { return (std::strstreambuf*) std::ostream::rdbuf(); }
  char* str() { return rdbuf()->str(); }
};

#endif
