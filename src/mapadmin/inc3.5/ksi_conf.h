/* -*-mode:C++-*- */
/*
 * ksi_conf.h
 * ksi configuration
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
 * Creation date: Thu Jul 31 19:27:23 1997
 *
 *
 * $Id: ksi_conf.h,v 1.13.4.13 2009/06/29 13:37:41 ksion Exp $
 *
 */

#ifndef KSI_CONF_H
#define KSI_CONF_H

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
#    define LONG_LONG		long long
#  endif
#elif defined(__GNUC__)
#  define HAVE_CTYPE_H		1
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
#  define LONG_LONG		long long
#elif defined(_MSC_VER)
#  define HAVE_CTYPE_H		1
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
#  define LONG_LONG		__int64
#  ifndef LLONG_MAX
#    define LLONG_MAX		_I64_MAX
#  endif
#  ifndef LLONG_MIN
#    define LLONG_MIN		_I64_MIN
#  endif
#  define SIZEOF_LONG_LONG	8
#  ifndef __cplusplus
#    define inline __inline
#  endif
#elif defined(__STDC__)
#  define HAVE_CTYPE_H		1
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


#ifndef __GNUC__
# if HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifdef _AIX
#    pragma alloca
#  else
#   ifndef alloca /* predefined by HP cc +Olibcalls */
      char *alloca ();
#   endif
#  endif
# endif
#endif

#ifdef HAVE_CTYPE_H
#  include <ctype.h>
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
 * CAUTIOUS: 0 - no checks, fastest but dangerous
 *           1 - normal checks, default
 *           2 - paranoid checks, more error messages
 *           3 - debug the interpreter
 */

#ifndef CAUTIOUS
#  define CAUTIOUS 1
#endif


/* STRICT_R5RS: 0 - relax syntax, default
 *              1 - aditional syntax checks for strong R5RS compliance
 *
 */

#ifndef STRICT_R5RS
#  define STRICT_R5RS 0
#endif


/*
 * KSI_ENV_LIB
 *	Environ variable name where ksi library path stored
 */

#ifndef KSI_ENV_LIB
# define KSI_ENV_LIB "KSI_LIBRARY"
#endif

/*
 * KSI_SCM_SUFFIX	suffux for files with Scheme code
 * KSI_KO_SUFFIX	suffix for compiled code
 */

#ifndef KSI_SCM_SUFFIX
# define KSI_SCM_SUFFIX ".scm"
#endif

#ifndef KSI_KO_SUFFIX
# define KSI_KO_SUFFIX ".ko"
#endif

/*
 * KSI_DL_SUFFIX
 *	default suffix for dynamic libraries
 */

#ifndef KSI_DL_SUFFIX
#  if defined(HAVE_LIBDL) || defined(HAVE_DLOPEN)
#    define KSI_DL_SUFFIX	".so"
#  elif defined(HAVE_LIBDLD)
#    define KSI_DL_SUFFIX	".so" /* FIXME: what the suffix ? */
#  elif defined(HAVE_SHL_LOAD)
#    define KSI_DL_SUFFIX	".shl" /* FIXME: what the suffix ? */
#  elif defined(WIN32) || defined(OS2) || defined(MSDOS)
#    define KSI_DL_SUFFIX	".dll"
#  endif
#endif


/*
 * KSI_BOOT_FILE	File that loaded at first stage of startup
 * KSI_INIT_FILE	File that loaded at second stage of startup
 */

#ifndef KSI_BOOT_FILE
# define KSI_BOOT_FILE "Boot"
#endif

#ifndef KSI_INIT_FILE
# define KSI_INIT_FILE "Init"
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
# elif defined(MSDOS)
#  define KSI_HOST	"dos"
# else
#  define KSI_HOST	"unknown"
# endif
#endif


/*
 * Some platform characteristics
 *
 * Should be defined in ANSI <limits.h>, or in <config.h> by confugure script.
 * If not do guess.
 */

#ifndef CHAR_BIT
#  define CHAR_BIT 8
#endif

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

/*
 * It is difficult to guess the sizeof(void*) without any hint.
 *
 * I can do the only:
 *   if int is 64-bit size, most likely the void* too
 *   if long is 32-bit size, most likely the void* too
 *
 * If it is not work for you, define SIZEOF_VOIDP manualy (for example by configure script).
 */

#ifndef SIZEOF_VOIDP
#  ifdef INT_MAX
#    if INT_MAX != 32767 && INT_MAX != 2147483647
#      if INT_MAX == 9223372036854775807
#        define SIZEOF_VOIDP 8
#      endif
#    endif
#  endif
#endif
#ifndef SIZEOF_VOIDP
#  ifdef LONG_MAX
#    if LONG_MAX == 2147483647L
#      define SIZEOF_VOIDP 4
#    endif
#  endif
#endif
#ifndef SIZEOF_VOIDP
#  error cannot guess the sizeof(void*)
#endif

#if !defined(KSI_INT_TYPE)
#  if SIZEOF_VOIDP == SIZEOF_INT
#    define KSI_INT_TYPE		int
#    define KSI_UINT_TYPE		unsigned int
#    define KSI_SIZEOF_INT		SIZEOF_INT
#    define KSI_MAX_INT			(INT_MAX >> 2)
#  elif SIZEOF_VOIDP == SIZEOF_LONG
#    define KSI_INT_TYPE		long
#    define KSI_UINT_TYPE		unsigned long
#    define KSI_SIZEOF_INT		SIZEOF_LONG
#    define KSI_MAX_INT			(LONG_MAX >> 2)
#  else
#    error "unknown integer type"
#  endif
#endif

#define KSI_MIN_INT  (-KSI_MAX_INT)
#define KSI_INT_BIT (KSI_SIZEOF_INT * CHAR_BIT)



#if defined(_MSC_VER)
#    if defined(MAKE_si_LIB)
#      define SI_API  __declspec(dllexport)
#      define SI_DATA __declspec(dllexport)
#    else
#      define SI_API  __declspec(dllimport)
#      define SI_DATA __declspec(dllimport)
#    endif
#endif

#ifndef SI_API
#  define SI_API extern
#endif

#ifndef SI_DATA
#  define SI_DATA extern
#endif

#ifndef KSI_STACK_ELEM
# define KSI_STACK_ELEM KSI_INT_TYPE
#endif


#ifndef HAVE_STRCASECMP
#  if defined(_MSC_VER)
#    define strcasecmp _stricmp
#  endif
#endif

#if defined(_MSC_VER)
#  define ABS(x)		(abs((x)))
#  define copysign(x,y)		_copysign((x), (y))
#else
#  ifndef ABS
#    define ABS(x)		((x) < 0 ? -(x) : (x))
#  endif
#endif

#if defined(__GNUC__) && defined(__cplusplus)
#  define MAX(a,b) ((a) >? (b))
#  define MIN(a,b) ((a) <? (b))
#else
#  define MAX(a,b) ((a) > (b) ? (a) : (b))
#  define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#if defined(WIN32) || defined(MSDOS) || defined(OS2) || defined(CYGWIN)
#  define strpathcmp strcasecmp
#else
#  define strpathcmp strcmp
#endif


#define KSI_NORMAL_EXIT_CODE	0
#define KSI_ERROR_EXIT_CODE	1


/*
 * MAX_PATH_LENGTH
 *	Maximal length of path name.
 *	On Posix platform (ie unix) _POSIX_PATH_MAX must be specified.
 *	Some Windows compilers specifies _MAX_PATH,
 *	and as last 260 used (this value valid for ms-dos).
 */

#ifdef _POSIX_PATH_MAX
# define MAX_PATH_LENGTH _POSIX_PATH_MAX
#else
# if defined(_MAX_PATH)
#  define MAX_PATH_LENGTH _MAX_PATH
# else
#  define MAX_PATH_LENGTH 260
# endif
#endif


#if defined(unix) || defined(CYGWIN)

# define ABSOLUTE_PATH(p)			\
  (p[0] == '/' ||				\
   (p[0] == '.' && p[1] == '/') ||		\
   (p[0] == '.' && p[1] == '.' && p[2] == '/'))

# define IS_DIR_SUFFIX(c)	((c) == '/')
# define DIR_SEP		"/"
# define PATH_SEP		":"

#elif defined(MSDOS) || defined(WIN32) || defined(OS2)

# define ABSOLUTE_PATH(p)                                               \
  ((p[0] == '/' || p[0] == '\\') ||                                     \
   p[0] == '.' && (p[1] == '/' || p[1] == '\\') ||                      \
   p[0] == '.' && p[1] == '.' && (p[2] == '/' || p[2] == '\\') ||       \
   p[0] != '\0' && p[1] == ':' && (p[2] == '/' || p[2] == '\\'))

# define IS_DIR_SUFFIX(c)	((c) == '/' || (c) == '\\')
# define DIR_SEP		"\\"
# define PATH_SEP		";"

#else

# define ABSOLUTE_PATH(p) (0)
# define IS_DIR_SUFFIX(c) (0)

#endif


/*
 * KSI_STACK_GROWS_UP
 *	Direction of stack growing.
 *	Normaly defined in <config.h> by configure script.
 */

#ifndef HAVE_CONFIG_H
# ifndef KSI_STACK_GROWS_UP
#  if defined(hp9000s800) || defined(pyr) || defined(nosve) || defined(_UNICOS)
#   define KSI_STACK_GROWS_UP 1
#  endif
# endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_BZERO
#  ifdef HAVE_MEMSET
#    define bzero(p,n) memset((p),0,(size_t)(n))
#  else
#    define KSI_DEFINE_BZERO 1
     SI_API void bzero(void* p, int n);
#  endif
#endif

#ifndef HAVE_MEMMOVE
#  ifdef HAVE_BCOPY
#    define memmove(dst,src,len) bcopy((src),(dst),(len))
#  else
#    define KSI_DEFINE_MEMMOVE 1
     SI_API void memmove(void*, const void*, int);
#  endif
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
const char*
ksi_scm_lib_dir ();

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


#ifdef __cplusplus
}
#endif

#endif


/* End of file */
