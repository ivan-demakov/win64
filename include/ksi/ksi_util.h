/* -*-mode:C++-*- */
/*
 * ksi_util.h
 *
 * Copyright (C) 2006-2010, ivan demakov.
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
 * Author:        ivan demakov <ksion@users.sourceforge.net>
 * Creation date: Fri Dec 22 22:31:09 2006
 * Last Update:   Sat Aug 14 01:27:56 2010
 *
 */

#ifndef KSI_UTIL_H
#define KSI_UTIL_H

/**
 * @file   ksi_util.h
 * @author ivan demakov <ksion@users.sourceforge.net>
 * @date   Fri Dec 22 22:31:09 2006
 *
 * @brief useful utilities
 *
 *
 */


#include "ksi_defs.h"

# ifdef TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
# else
#  ifdef HAVE_SYS_TIME_H
#   include <sys/time.h>
#  else
#   ifdef HAVE_TIME_H
#    include <time.h>
#   endif
#  endif
# endif

#ifdef HAVE_SYS_TIMES_H
# include <sys/times.h>
#endif

#ifdef HAVE_SYS_TIMEB_H
# include <sys/timeb.h>
#endif

#ifndef __STDC__
# define time_t long
#endif


#ifdef __cplusplus
extern "C" {
#endif

/** Generate random bits.
 *
 * @param buf The buffer that is filled with the random bits.
 * @param size The size of the buffer.
 *
 */
SI_API
void
ksi_random_bits (char *buf, int size);


/** base64 encoding
 *
 * Encode @a buf as Base64.
 * The algorithm used to encode Base64-encoded data is defined in \l{RFC 2045}.
 *
 * @param buf The buffer that is encoded.
 * @param size The size of the buffer.
 *
 * @return byte array, encoded as Base64 and enclosed by '\0'.
 */
SI_API
char *
ksi_base64 (const char *buf, int size);


/** convert string to unicode
 *
 * @param str the string in local encoding
 *
 * @return wide-character string
 */
SI_API
wchar_t *
ksi_utf (const char *str);


/** Convert byte array to unicode
 *
 * @param str byte array in local encoding
 * @param size length of the array
 *
 * @return wide-character string enclosed by L'\0'
 */
SI_API
wchar_t *
ksi_to_utf (const char *str, int size);


/** convert unicode to string
 *
 * @param str wide-character string
 *
 * @return string in local encoding enclosed by '\0'
 */
SI_API
char *
ksi_local (const wchar_t *str);


/** convert unicode array to string
 *
 * @param str wide-character array
 * @param size length of the array
 *
 * @return string in local encoding enclosed by '\0'
 */
SI_API
char *
ksi_to_local (const wchar_t *str, int size);


SI_API
double
ksi_cpu_time();

SI_API
double
ksi_eval_time();

SI_API
double
ksi_real_time();

SI_API
struct tm *
ksi_gmtime (time_t it, struct tm *bt);

SI_API
struct tm *
ksi_localtime (time_t it, struct tm *bt);

SI_API
int
ksi_has_suffix (const char *fname, const char *suffix);

SI_API
const char *
ksi_tilde_expand (const char *name);

SI_API
char *
ksi_get_cwd ();

SI_API
char *
ksi_expand_file_name (const char *fname);


SI_API
unsigned
ksi_hash_str (const wchar_t *str, int len, unsigned n);


#ifdef __cplusplus
}
#endif

#endif

/* End of file */
