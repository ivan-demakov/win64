/* -*-mode:C++-*- */
/*
 * ksi_util.h
 *
 * Copyright (C) 2006-2009, ivan demakov.
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
 * Author:        ivan demakov <ksion@users.sourceforge.net>
 * Creation date: Fri Dec 22 22:31:09 2006
 *
 * $Id: ksi_util.h,v 1.1.2.2.2.1 2009/08/05 14:24:22 ksion Exp $
 *
 */

#ifndef KSI_UTIL_H
#define KSI_UTIL_H

#include "ksi_conf.h"

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
ksi_has_suffix (const char* fname, const char* suffix);

SI_API
const char *
ksi_tilde_expand (const char *name);

SI_API
char *
ksi_get_cwd ();

SI_API
char *
ksi_expand_file_name (const char* fname);


#ifdef __cplusplus
}
#endif

#endif

/* End of file */
