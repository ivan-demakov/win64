/* -*-mode:C++-*- */
/*
 * ksi_defs.h
 * ksi defines
 *
 * Copyright (C) 1997-2010, 2014, 2015, ivan demakov
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
 * along with the software; see the file COPYING.LESSER.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 * Author:        Ivan Demakov <ksion@users.sourceforge.net>
 * Creation date: Thu Jul 31 19:27:23 1997
 * Last Update:   Mon Oct  6 10:07:15 2014
 *
 */

#ifndef KSI_DEFS_H
#define KSI_DEFS_H

#include "ksi_ver.h"

#if defined(_MSC_VER)
#  if !defined(WIN32)
#    define WIN32 1
#  endif
#  if !defined(WIN32_THREADS)
#    define WIN32_THREADS 1
#  endif
#endif

#if defined(WIN32) && !defined(MSWIN32)
#  define MSWIN32 1
#endif

#if defined(MSWIN32) && !defined(WIN32)
#  define WIN32 1
#endif

#if !defined(unix) && (defined(__unix) || defined(__unix__))
#  define unix 1
#endif

#if !defined(CYGWIN) && (defined(__CYGWIN32__) || defined(__CYGWIN__))
#  define CYGWIN 1
#endif


#ifdef HAVE_CONFIG_H
#  include <config.h>
#  if SIZEOF_LONG_LONG > 0
#    define HAVE_LONG_LONG	1
#  endif
#elif defined(__GNUC__)
#  define HAVE_WCHAR_H		1
#  define HAVE_LIMITS_H		1
#  define HAVE_STDARG_H		1
#  define HAVE_STDDEF_H		1
#  define HAVE_STDLIB_H		1
#  define HAVE_STDIO_H		1
#  define HAVE_STRING_H		1
#  define HAVE_UNISTD_H		1
#  define HAVE_SYS_TYPES_H	1
#  define HAVE_TIME_H		1
#  define HAVE_SYS_TIMEB_H	1
#  define HAVE_SYS_STAT_H	1
#  define HAVE_FCNTL_H		1
#  define HAVE_ERRNO_H		1
#  define HAVE_MEMSET		1
#  define HAVE_MEMCPY		1
#  define HAVE_MEMMOVE		1
#  define HAVE_FTIME		1
#  define HAVE_LONG_LONG	1
#elif defined(_MSC_VER)
#  define HAVE_WCHAR_H		1
#  define HAVE_LIMITS_H		1
#  define HAVE_STDARG_H		1
#  define HAVE_STDDEF_H		1
#  define HAVE_STDLIB_H		1
#  define HAVE_STDIO_H		1
#  define HAVE_STRING_H		1
#  define HAVE_MALLOC_H		1
#  define HAVE_SYS_TYPES_H	1
#  define HAVE_TIME_H		1
#  define HAVE_SYS_TIMEB_H	1
#  define HAVE_SYS_STAT_H	1
#  define HAVE_FCNTL_H		1
#  define HAVE_ERRNO_H		1
#  define HAVE_MEMSET		1
#  define HAVE_MEMCPY		1
#  define HAVE_MEMMOVE		1
#  define HAVE_FTIME		1
#  define HAVE_RENAME		1
#  define HAVE_LONG_LONG	1
#  ifndef __cplusplus
#    define inline __inline
#  endif
#elif defined(__STDC__)
#  define HAVE_WCHAR_H		1
#  define HAVE_LIMITS_H		1
#  define HAVE_STDARG_H		1
#  define HAVE_STDDEF_H		1
#  define HAVE_STDLIB_H		1
#  define HAVE_STDIO_H		1
#  define HAVE_STRING_H		1
#  define HAVE_FCNTL_H		1
#  define HAVE_ERRNO_H		1
#  define HAVE_MEMSET		1
#  define HAVE_MEMCPY		1
#  define HAVE_MEMMOVE		1
#else
#  error cannot auto configure ksi
#endif


#ifdef HAVE_ALLOCA_H
#  include <alloca.h>
#endif
#ifdef HAVE_WCHAR_H
#  include <wchar.h>
#endif
#ifdef HAVE_LIMITS_H
#  include <limits.h>
#endif
#ifdef HAVE_MEMORY_H
#  include <memory.h>
#endif
#ifdef HAVE_STDARG_H
#  include <stdarg.h>
#endif
#ifdef HAVE_STDDEF_H
#  include <stddef.h>
#endif
#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#  include <stdio.h>
#endif
#ifdef HAVE_STRING_H
#  include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#  include <strings.h>
#endif
#ifdef HAVE_MALLOC_H
#  include <malloc.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#endif

#ifdef HAVE_ERRNO_H
#  include <errno.h>
#elif defined(HAVE_SYS_ERRNO_H)
#  include <sys/errno.h>
#else
#  ifdef __cplusplus
     extern "C" int errno;
#  else
     extern int errno;
#  endif
#endif

#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif

#ifdef WIN32
#  include <windows.h>
#  include <process.h>
#endif

#ifdef _MSC_VER
#  include <io.h>
#  define F_OK 0
#  define W_OK 2
#  define R_OK 4
#  define X_OK R_OK
#endif

#if defined(_MSC_VER)
#  define S_ISDIR(x)  (((x) & _S_IFMT) == _S_IFDIR)
#  define S_ISCHR(x)  (((x) & _S_IFMT) == _S_IFCHR)
#  define S_ISREG(x)  (((x) & _S_IFMT) == _S_IFREG)
#  define S_ISFIFO(x) (((x) & _S_IFMT) == _S_IFIFO)
#endif

#ifdef POSIX_THREADS
#  include <pthread.h>
#  ifdef HAVE_POSIX_RWLOCK
#    define KSI_DECLARE_LOCK(lock) pthread_rwlock_t lock
#    define KSI_INIT_LOCK(lock)    pthread_rwlock_init(&lock, NULL)
#    define KSI_LOCK_R(lock)       pthread_rwlock_rdlock(&lock)
#    define KSI_UNLOCK_R(lock)     pthread_rwlock_unlock(&lock)
#    define KSI_LOCK_W(lock)       pthread_rwlock_wrlock(&lock)
#    define KSI_UNLOCK_W(lock)     pthread_rwlock_unlock(&lock)
#    define KSI_FINI_LOCK(lock)    pthread_rwlock_destroy(&lock)
#  else
#    define KSI_DECLARE_LOCK(lock) pthread_mutex_t lock
#    define KSI_INIT_LOCK(lock)    pthread_mutex_init(&lock, NULL)
#    define KSI_LOCK_R(lock)       pthread_mutex_lock(&lock)
#    define KSI_UNLOCK_R(lock)     pthread_mutex_unlock(&lock)
#    define KSI_LOCK_W(lock)       pthread_mutex_lock(&lock)
#    define KSI_UNLOCK_W(lock)     pthread_mutex_unlock(&lock)
#    define KSI_FINI_LOCK(lock)    pthread_mutex_destroy(&lock)
#  endif
#elif defined(WIN32_THREADS)
#  define KSI_DECLARE_LOCK(lock)	CRITICAL_SECTION lock
#  define KSI_INIT_LOCK(lock)		InitializeCriticalSection(&lock)
#  define KSI_LOCK_R(lock)		EnterCriticalSection(&lock)
#  define KSI_UNLOCK_R(lock)		LeaveCriticalSection(&lock)
#  define KSI_LOCK_W(lock)		EnterCriticalSection(&lock)
#  define KSI_UNLOCK_W(lock)		LeaveCriticalSection(&lock)
#  define KSI_FINI_LOCK(lock)		DeleteCriticalSection(&lock)
#else
#  define KSI_DECLARE_LOCK(lock)	void *lock
#  define KSI_INIT_LOCK(lock)		((void) (lock))
#  define KSI_LOCK_R(lock)		((void) (lock))
#  define KSI_UNLOCK_R(lock)		((void) (lock))
#  define KSI_LOCK_W(lock)		((void) (lock))
#  define KSI_UNLOCK_W(lock)		((void) (lock))
#  define KSI_FINI_LOCK(lock)		((void) (lock))
#endif


/*
 * KSI_DL_SUFFIX
 *	default suffix for dynamic libraries
 */

#ifndef KSI_DL_SUFFIX
#  if defined(HAVE_LIBDL) || defined(HAVE_DLOPEN)
#    define KSI_DL_SUFFIX	".so"
#  elif defined(WIN32) || defined(OS2)
#    define KSI_DL_SUFFIX	".dll"
#  endif
#endif


#ifndef KSI_HOST
# if defined(unix)
#  define KSI_HOST	"unix"
# elif defined(CYGWIN)
#  define KSI_HOST	"cygwin"
# elif defined(WIN32)
#  define KSI_HOST	"windows"
# elif defined(OS2)
#  define KSI_HOST	"os/2"
# else
#  define KSI_HOST	"unknown"
# endif
#endif


#if defined(__GNUC__)
#  define __KSI_PRINTF(n,m) __attribute__((format(gnu_printf, n, m)))
#else
#  define __KSI_PRINTF(n,m)
#endif


/*
 * Some platform characteristics
 *
 */

#ifndef SIZEOF_INT
#  ifdef INT_MAX
#    if INT_MAX == 32767
#      define SIZEOF_INT 2
#    elif INT_MAX == 2147483647
#      define SIZEOF_INT 4
#    elif INT_MAX == 9223372036854775807
#      define SIZEOF_INT 8
#    endif
#  endif
#endif
#ifndef SIZEOF_INT
#  error cannot guess the sizeof(int)
#endif

#ifndef SIZEOF_LONG
#  ifdef LONG_MAX
#    if LONG_MAX == 32767L
#      define SIZEOF_LONG 2
#    elif LONG_MAX == 2147483647L
#      define SIZEOF_LONG 4
#    elif LONG_MAX == 9223372036854775807L
#      define SIZEOF_LONG 8
#    endif
#  endif
#endif
#ifndef SIZEOF_LONG
#  error cannot guess the sizeof(long)
#endif

#ifdef HAVE_LONG_LONG
#ifndef SIZEOF_LONG_LONG
#  ifdef LLONG_MAX
#    if LLONG_MAX == 32767
#      define SIZEOF_LONG_LONG 2
#    elif LLONG_MAX == 2147483647L
#      define SIZEOF_LONG_LONG 4
#    elif LLONG_MAX == 9223372036854775807LL
#      define SIZEOF_LONG_LONG 8
#    endif
#  endif
#endif
#ifndef SIZEOF_LONG_LONG
#  error cannot guess the sizeof(long long)
#endif
#endif

#ifdef HAVE_LONG_LONG
#  if defined(_MSC_VER)
#    ifdef LLONG_MAX
#      define LONG_LONG		long long
#    else
#      define LONG_LONG		__int64
#      define LLONG_MAX		_I64_MAX
#      define LLONG_MIN		_I64_MIN
#    endif
#  else
#    define LONG_LONG		long long
#  endif
#else
# define LONG_LONG		long
#endif

#ifdef _WIN64
#  ifdef __int64
     typedef unsigned __int64 KSI_WORD;
#  else
     typedef unsigned long long KSI_WORD;
#  endif
#else
  typedef unsigned long KSI_WORD;
#endif


#if defined (__GNUC__)
#define KSI_DECLSPEC_EXPORT  __declspec(__dllexport__)
#define KSI_DECLSPEC_IMPORT  __declspec(__dllimport__)
#endif
#if defined (_MSC_VER) || defined (__BORLANDC__)
#define KSI_DECLSPEC_EXPORT  __declspec(dllexport)
#define KSI_DECLSPEC_IMPORT  __declspec(dllimport)
#endif
#ifdef __WATCOMC__
#define KSI_DECLSPEC_EXPORT  __export
#define KSI_DECLSPEC_IMPORT  __import
#endif
#ifdef __IBMC__
#define KSI_DECLSPEC_EXPORT  _Export
#define KSI_DECLSPEC_IMPORT  _Import
#endif

#if defined(__MINGW32__) || defined(__CYGWIN__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__IBMC__)
#  if defined(MAKE_si_LIB)
#    define SI_API  KSI_DECLSPEC_EXPORT
#    define SI_DATA KSI_DECLSPEC_EXPORT
#  else
#    define SI_API  KSI_DECLSPEC_IMPORT
#    define SI_DATA KSI_DECLSPEC_IMPORT
#  endif
#endif

#ifndef SI_API
#  define SI_API extern
#endif

#ifndef SI_DATA
#  define SI_DATA extern
#endif


#if defined(_MSC_VER)
#  define copysign(x,y) _copysign((x), (y))
#  define strcasecmp    _stricmp
#endif

#if defined(WIN32) || defined(OS2) || defined(CYGWIN)
#  define strpathcmp strcasecmp
#else
#  define strpathcmp strcmp
#endif


#define KSI_NORMAL_EXIT_CODE  0
#define KSI_ERROR_EXIT_CODE   1


/*
 * MAX_PATH_LENGTH
 *	Maximal length of path name.
 *	On Posix platform (ie unix) _POSIX_PATH_MAX must be specified.
 *	Some Windows compilers specifies _MAX_PATH,
 *	and as last 260 used (this value valid for ms-dos).
 */

#ifdef _POSIX_PATH_MAX
# define KSI_MAX_PATH_LENGTH _POSIX_PATH_MAX
#else
# if defined(_MAX_PATH)
#  define KSI_MAX_PATH_LENGTH _MAX_PATH
# else
#  define KSI_MAX_PATH_LENGTH 260
# endif
#endif


typedef struct Ksi_Context *ksi_context_t;

typedef struct Ksi_Obj *ksi_obj;
typedef struct Ksi_Core *ksi_core;
typedef struct Ksi_Symbol *ksi_symbol;
typedef struct Ksi_Symbol *ksi_keyword;
typedef struct Ksi_Char *ksi_char;
typedef struct Ksi_String *ksi_string;
typedef struct Ksi_Bytevector *ksi_bytevector;
typedef struct Ksi_Pair *ksi_pair;
typedef struct Ksi_Vector *ksi_vector;
typedef struct Ksi_Values *ksi_values;
typedef struct Ksi_Bignum *ksi_bignum;
typedef struct Ksi_Flonum *ksi_flonum;

//typedef struct Ksi_Port *ksi_port;
typedef struct Ksi_Byte_Port *ksi_byte_port;
typedef struct Ksi_Char_Port *ksi_char_port;
typedef struct Ksi_Hashtab *ksi_hashtab;
typedef struct Ksi_Environ *ksi_env;

typedef struct Ksi_ETag *ksi_etag;
typedef struct Ksi_EObj *ksi_eobj;

typedef struct Ksi_Wind *ksi_wind;
typedef struct Ksi_Jump *ksi_jump;

typedef struct Ksi_EnvRec *ksi_envrec;


enum ksi_errlog_priority_t
{
    KSI_ERRLOG_FATAL,                 /* fatal error conditions */
    KSI_ERRLOG_ERROR,                 /* error conditions */
    KSI_ERRLOG_WARNING,               /* warning conditions */
    KSI_ERRLOG_NOTICE,                /* normal but significant condition */
    KSI_ERRLOG_INFO,                  /* informational message */
    KSI_ERRLOG_DEBUG,                 /* debug message */
    KSI_ERRLOG_ALL
};


#ifdef __cplusplus
extern "C" {
#endif

SI_API
const char*
ksi_version (void);

SI_API
int
ksi_major_version (void);

SI_API
int
ksi_minor_version (void);

SI_API
int
ksi_patch_level ();

SI_API
const char*
ksi_cpu ();

SI_API
const char*
ksi_os ();

SI_API
const char*
ksi_host ();

SI_API
void
ksi_set_scheme_lib_dir (const char *dir);

SI_API
const char*
ksi_scheme_lib_dir ();

SI_API
const char*
ksi_instal_include_dir ();

SI_API
const char*
ksi_instal_lib_dir ();

SI_API
const char*
ksi_instal_bin_dir ();

SI_API
const char*
ksi_instal_info_dir ();

SI_API
const char*
ksi_instal_man_dir ();

SI_API
const char*
ksi_build_cflags ();

SI_API
const char*
ksi_build_libs ();

#ifdef WIN32
SI_API
const char*
ksi_get_last_error (char *msg);
#endif

SI_API
int
ksi_debug (const char *fmt, ...) __KSI_PRINTF(1,2);

SI_API
int
ksi_info (const char *fmt, ...) __KSI_PRINTF(1,2);

SI_API
int
ksi_notice (const char *fmt, ...) __KSI_PRINTF(1,2);

SI_API
int
ksi_warn (const char *fmt, ...) __KSI_PRINTF(1,2);

SI_API
void
ksi_errlog_msg (ksi_context_t ctx, int pri, const wchar_t *msg);

SI_API
ksi_obj
ksi_errlog (ksi_context_t ctx, int priority, const char *fmt, ...) __KSI_PRINTF(3,4);


#ifdef __cplusplus
}
#endif

#endif


/* End of file */
