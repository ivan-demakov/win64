/* -*-mode:C++-*- */
/*
 * ksi_printf.h
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
 * Creation date: Sun Dec 17 22:31:59 2006
 *
 * $Id: ksi_printf.h,v 1.1.2.3 2009/06/25 11:22:57 ksion Exp $
 *
 */

#ifndef KSI_PRINTF_H
#define KSI_PRINTF_H

#include "ksi_gc.h"


#ifdef __cplusplus
extern "C" {
#endif

SI_API
char *
ksi_avprintf(const char *fmt, va_list args);

SI_API
char *
ksi_aprintf (const char* fmt, ...);


#ifdef __cplusplus
}
#endif

#endif

/* End of file */
