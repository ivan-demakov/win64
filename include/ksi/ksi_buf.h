/* -*-mode:C++-*- */
/*
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
 */

/**
 * @file   ksi_buf.h
 * @author ivan demakov <ksion@users.sourceforge.net>
 * @date   Tue Sep 29 18:33:56 2009
 *
 * @brief char buffering.
 *
 */

#ifndef KSI_BUF_H
#define KSI_BUF_H

#include "ksi_defs.h"

/**
 * @typedef ksi_buffer_t
 *
 * @brief Opaque type that points to the buffer.
 *
 */

typedef struct Ksi_Buffer *ksi_buffer_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocates new @a ksi_buffer_t.
 *
 * @param initial_size Initial size of the buffer in chars. If <= 0, the default size used.
 * @param alloc_step If a buffer size too small to add new char, increase the buffer by this value. If <= 0, the default size used.
 *
 * @return Allocated buffer.
 */
SI_API
ksi_buffer_t
ksi_new_buffer(size_t initial_size, size_t alloc_step);


/**
 * Add char @a c to the buffer @a buf.
 *
 * @param buf The buffer to add the char @a c.
 * @param c The char to add to the @a buffer.
 *
 * @return The buffer @a buf.
 */
SI_API
ksi_buffer_t
ksi_buffer_put(ksi_buffer_t buf, wchar_t c);


/**
 * Add \a len chars from \a str to \a buf.
 *
 * @param buf The buffer to add the chars.
 * @param str The source from which to add the chars.
 * @param len The number of chars to add.
 *
 * @return The buffer \a buf.
 */
SI_API
ksi_buffer_t
ksi_buffer_append(ksi_buffer_t buf, const wchar_t *str, size_t len);


/**
 * get a length of the buffer.
 *
 * @param buf The buffer.
 *
 * @return The number of chars added to \a buf.
 */
SI_API
size_t
ksi_buffer_len(ksi_buffer_t buf);


/**
 * Get a buffered data from the buffer.
 *
 * @param buf The buffer.
 *
 * @return Pointer to a buffered data.
 */
SI_API
wchar_t *
ksi_buffer_data(ksi_buffer_t buf);


#ifdef __cplusplus
}
#endif

#endif

/* End of file */
