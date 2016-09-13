/* -*-mode:C++-*- */
/*
 * ksi_buf.h
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
 * Creation date: Sun Dec 17 22:56:39 2006
 *
 * $Id: ksi_buf.h,v 1.1.2.3 2009/06/27 20:39:43 ksion Exp $
 *
 */

#ifndef KSI_BUF_H
#define KSI_BUF_H

#include "ksi_gc.h"


typedef struct Ksi_Buffer *ksi_buffer_t;


#ifdef __cplusplus
extern "C" {
#endif


SI_API
ksi_buffer_t
ksi_new_buffer(size_t initial_size, size_t alloc_step);

SI_API
ksi_buffer_t
ksi_buffer_put(ksi_buffer_t buf, int c);

SI_API
ksi_buffer_t
ksi_buffer_append(ksi_buffer_t buf, const void *str, size_t len);

SI_API
size_t
ksi_buffer_len(ksi_buffer_t buf);

SI_API
void *
ksi_buffer_data(ksi_buffer_t buf);


#ifdef __cplusplus
}
#endif

#endif

/* End of file */
