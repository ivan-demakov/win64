/* -*-mode:C++-*- */
/*
 * ksi_jump.h
 * ksi long jumps
 *
 * Copyright (C) 1997-2009, Ivan Demakov
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
 * Creation date: Fri Aug  8 00:22:29 1997
 * Last Update:   Tue Mar 27 19:57:51 2001
 *
 *
 * $Id: ksi_jump.h,v 1.4.4.9.2.1 2009/07/05 15:50:25 ksion Exp $
 *
 */

#ifndef KSI_JUMP_H
#define KSI_JUMP_H

#include <setjmp.h>

typedef KSI_STACK_ELEM *ksi_stack;

typedef struct Ksi_Wind *ksi_wind;
typedef struct Ksi_Jump *ksi_jump;


#ifdef sparc
#  define FLUSH_REGISTER_WINDOWS asm("ta 3")
#else
#  define FLUSH_REGISTER_WINDOWS
#endif


typedef enum _ksi_rethrow_t
{
  KSI_RETHROW_NORMAL,
  KSI_RETHROW_ERROR,
  KSI_RETHROW_FATAL,
  KSI_RETHROW_EXIT
} ksi_rethrow_t;


struct Ksi_Jump
{
  jmp_buf	j_buf;
  ksi_stack	stack;
  ksi_stack	from;
  int		size;
  ksi_obj	module;
  ksi_wind	wind;
};


struct Ksi_Catch
{
  struct Ksi_Jump	jmp;
  ksi_obj		handler;
  ksi_obj		tag;
  ksi_obj		thrown;
  ksi_obj		value;
  ksi_obj		retry;
  ksi_rethrow_t		rethrow;
};


struct Ksi_Wind
{
  ksi_wind		cont;
  struct Ksi_Catch*	the_catch;
  ksi_obj		pre;
  ksi_obj		post;
};


#ifdef KSI_STACK_GROWS_UP
#  define KSI_STK_CMP(a,b) ((ksi_stack) (a) > (ksi_stack) (b))
#else
#  define KSI_STK_CMP(a,b) ((ksi_stack) (a) < (ksi_stack) (b))
#endif


#ifdef __cplusplus
extern "C" {
#endif

SI_API
void
ksi_init_jump (ksi_jump b, ksi_stack start, ksi_stack here);

SI_API
ksi_obj
ksi_set_jump (ksi_jump buf, ksi_stack start);

SI_API
void
ksi_long_jump (ksi_jump buf, ksi_obj val);

SI_API
ksi_obj
ksi_new_continuation (ksi_stack here);

SI_API
ksi_obj
ksi_continuation (ksi_obj *esc);

SI_API
ksi_wind
ksi_add_catch (ksi_obj tag, ksi_obj handler, int may_retry);

SI_API
void
ksi_del_catch (ksi_wind the_catch);

SI_API
ksi_wind
ksi_find_catch (ksi_obj tag);

SI_API
ksi_obj
ksi_throw_to_catch (ksi_wind wind, int retry, ksi_obj tag, ksi_obj val, ksi_rethrow_t rethrow);

SI_API
ksi_obj
ksi_rethrow (ksi_wind wind);

SI_API
ksi_obj
ksi_throw_error (ksi_obj exn);

SI_API
ksi_obj
ksi_throw_fatal (ksi_obj exn);

SI_API
ksi_obj
ksi_exit (ksi_obj x);

SI_API
ksi_obj
ksi_quit (void);

SI_API
ksi_obj
ksi_throw (ksi_obj tag, ksi_obj val);

SI_API
ksi_obj
ksi_dynamic_wind (ksi_obj pre, ksi_obj act, ksi_obj post);

SI_API
ksi_obj
ksi_call_cc (ksi_obj proc);

SI_API
ksi_obj
ksi_catch (ksi_obj tag, ksi_obj body, ksi_obj handler);

SI_API
ksi_obj
ksi_catch_with_retry (ksi_obj tag, ksi_obj body, ksi_obj handler);

SI_API
int
ksi_catch_exit (int (*proc) (void *data), int (*handler) (void *data), void *data);


#ifdef __cplusplus
}
#endif

#endif
