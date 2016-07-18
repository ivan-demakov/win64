/* -*-mode:C++-*- */
/*
 * ksi_gc.h
 * ksi garbage collector interface
 *
 * Copyright (C) 1997-2010, 2014, ivan demakov
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
 * Creation date: Thu Jul 31 21:49:27 1997
 * Last Update:   Mon Oct  6 10:40:20 2014
 *
 */


#ifndef KSI_GC_H
#define KSI_GC_H

#include "ksi_defs.h"


typedef void (*ksi_finalizer_function) (void *obj, void *client_data);
typedef void (*ksi_gc_warn_function) (char* msg, KSI_WORD arg);


#ifdef __cplusplus
extern "C" {
#endif


SI_API
void*
ksi_malloc (size_t sz);

SI_API
void*
ksi_malloc_data (size_t sz);

#define ksi_malloc_atomic ksi_malloc_data

SI_API
void*
ksi_malloc_eternal (size_t sz);

SI_API
void*
ksi_realloc (void *ptr, size_t sz);

SI_API
void
ksi_free (void *ptr);

SI_API
char *
ksi_strdup (const char *ptr);

SI_API
void*
ksi_base_ptr(void* ptr);

SI_API
void
ksi_register_finalizer (void *obj, ksi_finalizer_function proc, void *data);

#define ksi_unregister_finalizer(obj) ksi_register_finalizer ((obj), 0, 0)

SI_API
int
ksi_expand_heap (unsigned sz);

SI_API
void
ksi_set_max_heap (unsigned sz);

SI_API
unsigned
ksi_get_heap_size (void);

SI_API
unsigned
ksi_get_heap_free (void);

SI_API
unsigned
ksi_gcollections (void);

SI_API
int
ksi_gcollect (int full);

SI_API
void
ksi_enable_gc (void);

SI_API
void
ksi_disable_gc (void);

SI_API
void
ksi_set_gc_warn(ksi_gc_warn_function warn);

SI_API
void
ksi_init_gc (void);

#ifdef __cplusplus
}
#endif


#endif

/* End of code */
