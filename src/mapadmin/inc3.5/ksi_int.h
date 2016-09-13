/* -*-mode:C++-*- */
/*
 * ksi_int.h
 * Internal structs
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
 * Creation date: Thu Jul 31 19:19:17 1997
 *
 *
 * $Id: ksi_int.h,v 1.21.4.20 2009/06/29 13:34:45 ksion Exp $
 *
 */

#ifndef KSI_INT_H
#define KSI_INT_H

#include "ksi_type.h"
#include "ksi_port.h"
#include "ksi_jump.h"
#include "ksi_evt.h"
#include "ksi_proc.h"
#include "ksi_comp.h"

#ifdef WIN32
#  include <windows.h>
#endif


enum ksi_errlog_priority_t
{
  ERRLOG_EMERG = 0,             /* system is unusable */
  ERRLOG_ALERT,                 /* action must be taken immediately */
  ERRLOG_CRIT,                  /* critical conditions */
  ERRLOG_ERROR,                 /* error conditions */
  ERRLOG_WARNING,               /* warning conditions */
  ERRLOG_NOTICE,                /* normal but significant condition */
  ERRLOG_INFO,                  /* informational */
  ERRLOG_DEBUG,                 /* debug-level messages */
  ERRLOG_ALL = ERRLOG_DEBUG
};


typedef void (*ksi_init_function) (void* client_data);

struct Ksi_Interp
{
    volatile int have_event;
    ksi_event_mgr event_mgr;
    ksi_event waiting_events, pending_events, active_events;

    ksi_stack stack;
    ksi_wind wind;
    ksi_wind exit_catch;
    ksi_obj user_quit_exn;
    ksi_obj jump_val;
    ksi_obj exit_handlers;
    ksi_obj error_handlers;

    ksi_obj curr_module;
    ksi_env glob_env;
    ksi_obj modules;

    int errlog_priority;
    ksi_obj module_priority;

    ksi_port input_port;
    ksi_port output_port;
    ksi_port error_port;
    ksi_port errlog_port;

    const char *ksi_libs_path;
    ksi_obj app_sym;

//  ksi_port		std_input_port;
//  ksi_port		std_output_port;
//  ksi_port		std_error_port;

//  ksi_obj		protected_objs;

    struct Ksi_Stat_Lib *stat_libs;
    struct Ksi_Dynl_Lib *dynl_libs;
    struct Ksi_Init_Proc *boot_procs;
    struct Ksi_Init_Proc *init_procs;

    int load_indent;
    const char *load_err_msg;
    char *loading_file;
};


struct Ksi_Dynl_Lib
{
  struct Ksi_Dynl_Lib *next;
  char *name;
  void *handle;
  unsigned count;
};

struct Ksi_Stat_Lib
{
  struct Ksi_Stat_Lib *next;
  void *data;
  ksi_init_function proc;
  char name[1];
};

struct Ksi_Init_Proc
{
  struct Ksi_Init_Proc *next;
  void *data;
  ksi_init_function proc;
};


#define errlog_emerg	KSI_MK_SINT(ERRLOG_EMERG)
#define errlog_alert	KSI_MK_SINT(ERRLOG_ALERT)
#define errlog_crit	KSI_MK_SINT(ERRLOG_CRIT)
#define errlog_error	KSI_MK_SINT(ERRLOG_ERROR)
#define errlog_warning	KSI_MK_SINT(ERRLOG_WARNING)
#define errlog_notice	KSI_MK_SINT(ERRLOG_NOTICE)
#define errlog_info	KSI_MK_SINT(ERRLOG_INFO)
#define errlog_debug	KSI_MK_SINT(ERRLOG_DEBUG)
#define errlog_all	KSI_MK_SINT(ERRLOG_ALL)


#define KSI_CHECK_EVENTS do { if (ksi_int_data && ksi_int_data->have_event) ksi_do_events (); } while (0)


#define KSI_EXN_P(x)            (KSI_OBJ_IS ((x), KSI_TAG_EXN))
#define KSI_EXN_TYPE(x)         KSI_VEC_REF ((x), 0)
#define KSI_EXN_MSG(x)          KSI_VEC_REF ((x), 1)
#define KSI_EXN_VAL(x)          KSI_VEC_REF ((x), 2)
#define KSI_EXN_PRI(x)          KSI_VEC_REF ((x), 3)


#if !defined(CAUTIOUS) || CAUTIOUS >= 1
# define KSI_CHECK(a,c,s)       ((void)((c)||KSI_ERR(a,s)))
# define KSI_EXN_CHECK(t,a,c,s) ((void)((c)||KSI_EXN_ERR(t,a,s)))
# define KSI_TYPE_CHECK(a,c,s)  KSI_EXN_CHECK("type",a,c,s)
# define KSI_RANGE_CHECK(a,c,s) KSI_EXN_CHECK("range",a,c,s)
# define KSI_WARN(a,t,s)        ((void)((t)||ksi_warn("%s: %s",s,ksi_obj2str(a))))
#else
# define KSI_CHECK(a,t,s)
# define KSI_EXN_CHECK(t,a,c,s)
# define KSI_TYPE_CHECK(a,c,s)
# define KSI_RANGE_CHECK(a,c,s)
# define KSI_WARN(a,t,s)
#endif

#if CAUTIOUS >= 2
# define KSI_CHECK2(a,c,s)       ((void)((c)||KSI_ERR(a,s)))
# define KSI_EXN_CHECK2(t,a,c,s) ((void)((c)||KSI_EXN_ERR(t,a,s)))
# define KSI_TYPE_CHECK2(a,c,s)  KSI_EXN_CHECK("type",a,c,s)
# define KSI_RANGE_CHECK2(a,c,s) KSI_EXN_CHECK("range",a,c,s)
# ifdef __GNUC__
#  define KSI_ASSERT(t)	((t) || ksi_error ("assert failed: %s [%s %d] (%s)", \
					   #t,				     \
					   __FILE__,			     \
					   __LINE__,			     \
					   __PRETTY_FUNCTION__))
# else
#  define KSI_ASSERT(t)	((t) || ksi_error ("assert failed: %s [%s %d]",	\
					   #t,				\
					   __FILE__,			\
					   __LINE__))
# endif
#else
# define KSI_CHECK2(a,t,s)
# define KSI_EXN_CHECK2(t,a,c,s)
# define KSI_TYPE_CHECK2(a,c,s)
# define KSI_RANGE_CHECK2(a,c,s)
# define KSI_ASSERT(t)
#endif

#define KSI_ERR(a,s)         (ksi_exn_error("misc",a,s))
#define KSI_EXN_ERR(t,a,s)   (ksi_exn_error(t,a,s))
#define KSI_WNA_CHECK(c,p,s)  ((void)((c)||ksi_exn_error("arity",p,ksi_wna_s,s)))



#ifdef __cplusplus
extern "C" {
#endif


/* Initialization & termination. */

SI_API
const char *
ksi_app ();

SI_API
struct Ksi_Interp*
ksi_init (const char *app, void *top_stack_addr);

SI_API
void
ksi_init_std_ports (int in_fd, int out_fd, int err_fd);

SI_API
void
ksi_add_library (const char *path, const char *file, ksi_init_function proc, void *data);

SI_API
void
ksi_term (void);

SI_API
ksi_obj
ksi_load (ksi_obj str);

SI_API
ksi_obj
ksi_try_load (ksi_obj str, ksi_obj init);

SI_API
void
ksi_load_init_file (const char* fn);


/* error handling */

SI_API
ksi_obj
ksi_open_errlog (ksi_obj priority, ksi_obj port_or_fname);

SI_API
ksi_obj
ksi_errlog_priority (ksi_obj module, ksi_obj priority);

SI_API
ksi_obj
ksi_exn_p (ksi_obj x);

SI_API
ksi_obj
ksi_exn_type (ksi_obj x);

SI_API
ksi_obj
ksi_exn_message (ksi_obj x);

SI_API
ksi_obj
ksi_exn_value (ksi_obj x);

SI_API
ksi_obj
ksi_exn_priority (ksi_obj x);

SI_API
void
ksi_errlog_msg (int pri, const char *msg);

SI_API
ksi_obj
ksi_errlog (ksi_obj module, ksi_obj priority, const char *fmt, ...);

SI_API
ksi_obj
ksi_make_exn (const char *type, ksi_obj errobj, const char *msg, ksi_obj pri);

SI_API
int
ksi_exn_error (const char *type, ksi_obj errobj, const char *fmt, ...);

SI_API
int
ksi_warn (const char *fmt, ...);

SI_API
int
ksi_error (const char *fmt, ...);

SI_API
int
ksi_crit_error (ksi_obj priority, const char* fmt, ...);

SI_API
int
ksi_handle_error (ksi_obj tag, ksi_obj exn);

SI_API
ksi_obj
ksi_add_exit_handler (ksi_obj proc);

SI_API
ksi_obj
ksi_add_error_handler (ksi_obj proc);

SI_API
ksi_obj
ksi_remove_error_handler (ksi_obj proc);


/* dump & undump ksi objects */

SI_API
void*
ksi_dump (ksi_obj x, int* size);

SI_API
ksi_obj
ksi_undump (void* data, int size);

SI_API
ksi_obj
ksi_obj2dump (ksi_obj x);

SI_API
ksi_obj
ksi_dump2obj (ksi_obj x);

SI_API
ksi_obj
ksi_read_dump (ksi_obj port);

SI_API
ksi_obj
ksi_write_dump (ksi_obj x, ksi_obj port);


/* Dynamic loading */

SI_API
ksi_obj
ksi_dynamic_link (ksi_obj mod, ksi_obj sym);

SI_API
ksi_obj
ksi_dynamic_call (ksi_obj func, ksi_obj arg_list);

SI_API
ksi_obj
ksi_dynamic_unlink (ksi_obj func);



/* internal and not public declarations */

extern
struct Ksi_Interp *ksi_int_data;

extern
const char *ksi_char_names[];

extern
const char *ksi_wna_s;


void
ksi_init_types (ksi_env env);

void
ksi_init_numbers (ksi_env env);

void
ksi_init_chars (ksi_env env);

void
ksi_init_strings (ksi_env env);

void
ksi_init_pairs (ksi_env env);

void
ksi_init_vectors (ksi_env env);

void
ksi_init_procs (ksi_env env);

void
ksi_init_comp (ksi_env env);

void
ksi_init_klos (ksi_env env);

void
ksi_init_ports (ksi_env env);

void
ksi_init_events (ksi_env env);

void
ksi_init_read (ksi_env env);

void
ksi_init_dump (ksi_env env);

void
ksi_init_dynl (ksi_env env);

const char*
ksi_dynload_file (char* fname);

void
ksi_term_dynl (void);

void
ksi_init_load (ksi_env env);

void
ksi_init_env (ksi_env env);

void
ksi_term_events (void);

void
ksi_init_hash (ksi_env env);

void
ksi_init_shel (ksi_env env);

void
ksi_init_time (ksi_env env);

void
ksi_init_signals (ksi_env env);

void
ksi_term_signals (void);


#ifdef __cplusplus
}
#endif

#endif

/* End of code */
