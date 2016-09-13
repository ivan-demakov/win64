/*
 * ksi_numb.h
 * bignum defs
 *
 * Copyright (C) 1997-2009, ivan demakov
 *
 * The software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * The software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the software; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 * Author:        Ivan Demakov <ksion@users.sourceforge.net>
 * Creation date: Mon Jun  2 19:53:06 1997
 *
 * $Id: ksi_numb.h,v 1.5.2.3 2009/06/25 11:22:57 ksion Exp $
 *
 */

#ifndef KSI_NUM_H
#define KSI_NUM_H

#include "ksi_conf.h"
#include <math.h>

#ifdef __GNUC__
#if defined(__i386__) || defined(__i486__) || defined(__i586__)
#define ADD_LI(sh, sl, ah, al, bh, bl)		\
  __asm__ ("addl %5,%1; adcl %3,%0"		\
           : "=r" ((sh)),			\
             "=&r" ((sl))			\
           : "%0" ((unsigned) (ah)),		\
             "g" ((unsigned) (bh)),		\
             "%1" ((unsigned) (al)),		\
             "g" ((unsigned) (bl)))

#define SUB_LI(sh, sl, ah, al, bh, bl)		\
  __asm__ ("subl %5,%1; sbbl %3,%0"		\
           : "=r" ((sh)),			\
             "=&r" ((sl))			\
           : "0" ((unsigned) (ah)),		\
             "g" ((unsigned) (bh)),		\
             "1" ((unsigned) (al)),		\
             "g" ((unsigned) (bl)))

#define MUL_LI(ph,pl,m1,m2)			\
  __asm__ ("mull %3"				\
           : "=a" ((pl)),			\
             "=d" ((ph))			\
           : "%0" ((unsigned) (m1)),		\
             "rm" ((unsigned) (m2)))

#define DIV_LI(q,r,dh,dl,d)			\
  __asm__ ("divl %4"				\
           : "=a" ((q)),			\
             "=d" ((r))				\
           : "0"  ((unsigned) (dl)),		\
             "1"  ((unsigned) (dh)),		\
             "rm" ((unsigned) (d)))

#define CNT_ZERO(cnt,n)						\
  do {								\
    unsigned long tmp;						\
    __asm__ ("bsrl %1,%0"					\
             : "=r" (tmp) : "rm" ((unsigned long) (n)));	\
    (cnt) = tmp ^ 31;						\
  } while (0)

#endif /* __I386__ */
#endif /* __GNUC__ */

#ifdef __WATCOMC__

unsigned __cnt_zero__ (unsigned);
#pragma aux __cnt_zero__ =			\
        "bsr eax,ebx"				\
        parm [ebx]				\
        value [eax]				\
        modify exact nomemory [eax];

unsigned __add_long__ (unsigned, unsigned, unsigned, unsigned, unsigned*);
#pragma aux __add_long__ =			\
        "add eax,ebx"				\
        "adc edx,ecx"				\
        "mov [esi],edx"				\
        parm [eax] [ebx] [edx] [ecx] [esi]	\
        value [eax]				\
        modify exact [eax edx];

unsigned __sub_long__ (unsigned, unsigned, unsigned, unsigned, unsigned*);
#pragma aux __sub_long__ =			\
        "sub eax,ebx"				\
        "sbb edx,ecx"				\
        "mov [esi],edx"				\
        parm [eax] [ebx] [edx] [ecx] [esi]	\
        value [eax]				\
        modify exact [eax edx];

unsigned __mul_long__ (unsigned, unsigned, unsigned*);
#pragma aux __mul_long__ =			\
        "mul edx"				\
        "mov [esi],edx"				\
        parm [eax] [edx] [esi]			\
        value [eax]				\
        modify exact [eax edx];

unsigned __div_long__ (unsigned, unsigned, unsigned, unsigned*);
#pragma aux __div_long__ =			\
        "div ebx"				\
        "mov [esi],edx"				\
        parm [eax] [edx] [ebx] [esi]		\
        value [eax]				\
        modify exact [eax edx];

#define ADD_LI(sh, sl, ah, al, bh, bl) (sl) = __add_long__ ((al), (bl), (ah), (bh), &(sh))
#define SUB_LI(sh, sl, ah, al, bh, bl) (sl) = __sub_long__ ((al), (bl), (ah), (bh), &(sh))
#define MUL_LI(ph,pl,m1,m2) (pl) = __mul_long__ ((m1), (m2), &(ph))
#define DIV_LI(q,r,dh,dl,d) (q) = __div_long__ ((dl), (dh), (d), &(r))
#define CNT_ZERO(cnt,n) (cnt) = (__cnt_zero__ (n) ^ 31)

#endif /* __WATCOMC__ */

#ifdef _MSC_VER

__inline unsigned
__cnt_zero__ (unsigned x)
{
  __asm mov ebx,x
  __asm bsr eax,ebx
}

__inline unsigned
__add_long__ (unsigned a, unsigned b, unsigned d, unsigned c, unsigned *s)
{
  __asm push esi
  __asm mov esi,s
  __asm mov eax,a
  __asm mov edx,d
  __asm add eax,b
  __asm adc edx,c
  __asm mov [esi],edx
  __asm pop esi
}

__inline unsigned
__sub_long__ (unsigned a, unsigned b, unsigned d, unsigned c, unsigned* s)
{
  __asm push esi
  __asm mov esi,s
  __asm mov eax,a
  __asm mov edx,d
  __asm sub eax,b
  __asm sbb edx,c
  __asm mov [esi],edx
  __asm pop esi
}

__inline unsigned
__mul_long__ (unsigned a, unsigned d, unsigned* s)
{
  __asm push esi
  __asm mov esi,s
  __asm mov eax,a
  __asm mul d
  __asm mov [esi],edx
  __asm pop esi
}

__inline unsigned
__div_long__ (unsigned a, unsigned d, unsigned b, unsigned* s)
{
  __asm push esi
  __asm mov esi,s
  __asm mov eax,a
  __asm mov edx,d
  __asm div b
  __asm mov [esi],edx
  __asm pop esi
}

#define ADD_LI(sh, sl, ah, al, bh, bl) (sl) = __add_long__ ((al), (bl), (ah), (bh), &(sh))
#define SUB_LI(sh, sl, ah, al, bh, bl) (sl) = __sub_long__ ((al), (bl), (ah), (bh), &(sh))
#define MUL_LI(ph,pl,m1,m2) (pl) = __mul_long__ ((m1), (m2), &(ph))
#define DIV_LI(q,r,dh,dl,d) (q) = __div_long__ ((dl), (dh), (d), &(r))
#define CNT_ZERO(cnt,n) (cnt) = __cnt_zero__ (n) ^ 31

#endif /* _MSC_VER */


#if SIZEOF_LONG >= SIZEOF_INT * 2
#  define DBL_INT_TYPE long
#elif defined(LONG_LONG) && (SIZEOF_LONG_LONG >= SIZEOF_INT * 2)
#  define DBL_INT_TYPE LONG_LONG
#endif

#if !defined(ADD_LI)
#define ADD_LI(sh, sl, ah, al, bh, bl)		\
  do {						\
    unsigned __x;				\
    __x = (al) + (bl);				\
    (sh) = (ah) + (bh) + (__x < (al));		\
    (sl) = __x;					\
  } while (0)
#endif

#if !defined(SUB_LI)
#define SUB_LI(sh, sl, ah, al, bh, bl)          \
  do {						\
    unsigned __x;				\
    __x = (al) - (bl);				\
    (sh) = (ah) - (bh) - (__x > (al));		\
    (sl) = __x;					\
  } while (0)
#endif


#ifdef DBL_INT_TYPE

#ifndef MUL_LI
#define MUL_LI(ph,pl,m1,m2)                             \
  do {							\
    unsigned DBL_INT_TYPE pp = (m1); pp *= (m2);	\
    (ph) = (unsigned) (pp >> (SIZEOF_INT * CHAR_BIT));	\
    (pl) = (unsigned) pp;				\
  } while (0)
#endif

#ifndef DIV_LI
#define DIV_LI(q,r,dh,dl,d)                                                             \
  do {                                                                                  \
    unsigned DBL_INT_TYPE pp = (dh); pp = (pp << (SIZEOF_INT * CHAR_BIT)) | (dl);       \
    (q) = (unsigned) (pp / (d));                                                        \
    (r) = (unsigned) (pp % (d));                                                        \
  } while (0)
#endif

#endif /* DBL_INT_TYPE */


#define __BITS4 (SIZEOF_INT * CHAR_BIT / 4)
#define __ll_B  (1U << (SIZEOF_INT * CHAR_BIT / 2))
#define __us_B  (1U << (SIZEOF_INT * CHAR_BIT - 1))
#define __ll_lowpart(t) ((unsigned) (t) % __ll_B)
#define __ll_highpart(t) ((unsigned) (t) / __ll_B)

#ifndef MUL_LI
#define MUL_LI(w1, w0, u, v)                                                    \
  do {                                                                          \
    unsigned __x0, __x1, __x2, __x3;                                            \
    unsigned __ul, __vl, __uh, __vh;                                            \
                                                                                \
    __ul = __ll_lowpart (u);                                                    \
    __uh = __ll_highpart (u);                                                   \
    __vl = __ll_lowpart (v);                                                    \
    __vh = __ll_highpart (v);                                                   \
                                                                                \
    __x0 = __ul * __vl;                                                         \
    __x1 = __ul * __vh;                                                         \
    __x2 = __uh * __vl;                                                         \
    __x3 = __uh * __vh;                                                         \
                                                                                \
    __x1 += __ll_highpart (__x0);                                               \
    __x1 += __x2;                                                               \
    if (__x1 < __x2)                                                            \
      __x3 += __ll_B;                                                           \
                                                                                \
    (w1) = __x3 + __ll_highpart (__x1);                                         \
    (w0) = ((__x1) << (SIZEOF_INT * CHAR_BIT / 2)) + __ll_lowpart (__x0);       \
  } while (0)
#endif

#ifndef DIV_LI
#define DIV_LI(q, r, n1, n0, d)                         \
  do {                                                  \
    unsigned __d1, __d0, __q1, __q0, __r1, __r0, __m;   \
    __d1 = __ll_highpart (d);                           \
    __d0 = __ll_lowpart (d);                            \
                                                        \
    __q1 = (n1) / __d1;                                 \
    __r1 = (n1) - __q1 * __d1;                          \
    __m = __q1 * __d0;                                  \
    __r1 = __r1 * __ll_B | __ll_highpart (n0);          \
    if (__r1 < __m)                                     \
      {                                                 \
        __q1--, __r1 += (d);                            \
        if (__r1 >= (d))                                \
          if (__r1 < __m)                               \
            __q1--, __r1 += (d);                        \
      }                                                 \
    __r1 -= __m;                                        \
                                                        \
    __q0 = __r1 / __d1;                                 \
    __r0 = __r1 - q0 * __d1;                            \
    __m = __q0 * __d0;                                  \
    __r0 = __r0 * __ll_B | __ll_lowpart (n0);           \
    if (__r0 < __m)                                     \
      {                                                 \
        __q0--, __r0 += (d);                            \
        if (__r0 >= (d))                                \
          if (__r0 < __m)                               \
            __q0--, __r0 += (d);                        \
      }                                                 \
    __r0 -= __m;                                        \
                                                        \
    (q) =  __q1 * __ll_B | __q0;                        \
    (r) = __r0;                                         \
  } while (0)
#endif

#ifndef CNT_ZERO
#define CNT_ZERO(cnt,x)                         \
  do {                                          \
    unsigned t = (x), i = 0;                    \
    for (; (t & __us_B) == 0; t <<= 1, i++);    \
    (cnt) = i;                                  \
  } while (0)
#endif

#endif


/* End of file */
