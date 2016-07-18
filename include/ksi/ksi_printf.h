/* -*-mode:C++-*- */
/*
 * ksi_printf.h
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
 * Creation date: Sun Dec 17 22:31:59 2006
 * Last Update:   Sat Aug 14 01:22:08 2010
 *
 */

/**
 * @file   ksi_printf.h
 * @author ksion <ksion@woks.local>
 * @date   Fri Jan 22 20:40:58 2010
 *
 * @brief printf-like functions
 *
 *
 */

#ifndef KSI_PRINTF_H
#define KSI_PRINTF_H

#include "ksi_defs.h"
#include "ksi_buf.h"


#ifdef __cplusplus
extern "C" {
#endif

/** print
 *
 * @param fmt format string
 *
 * @return allocated string with result
 */
SI_API
wchar_t *
ksi_aprintf (const char *fmt, ...) __KSI_PRINTF(1,2);

/** print
 *
 * @param fmt format string
 * @param args arguments
 *
 * @return allocated string with result
 */
SI_API
wchar_t *
ksi_avprintf(const char *fmt, va_list args) __KSI_PRINTF(1,0);


SI_API
void
ksi_bufprintf (ksi_buffer_t buf, const char *fmt, ...) __KSI_PRINTF(2,3);

SI_API
void
ksi_bufvprintf(ksi_buffer_t buf, const char *fmt, va_list args) __KSI_PRINTF(2,0);


#ifdef __cplusplus
}
#endif

#endif

/* End of file */
