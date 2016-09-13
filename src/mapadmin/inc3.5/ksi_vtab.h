/* -*-mode:C++-*- */
/*
 * ksi_vtab.h
 * string table
 *
 * Copyright (C) 1997-2009, Ivan Demakov.
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
 * Creation date: Fri Dec  5 04:39:08 1997
 *
 *
 * $Id: ksi_vtab.h,v 1.1.1.1.4.3.2.1 2009/07/05 15:50:25 ksion Exp $
 *
 */

#ifndef KSI_VTAB_H
#define KSI_VTAB_H

#include "ksi_gc.h"


typedef unsigned (*ksi_hash_f) (void* val, unsigned size, void* data);
typedef int      (*ksi_cmp_f)  (void* v1, void* v2, void* data);
typedef int      (*ksi_iter_f) (void* val, void* iter_data);


struct Ksi_Tabrec
{
    struct Ksi_Tabrec   *next;
    void                *val;
};

struct Ksi_Valtab
{
    struct Ksi_Tabrec   **table;
    unsigned            size;    /* table size */
    int                 count;   /* number of recs */
    int                 inserts; /* number of recs */

    ksi_hash_f          hash;
    ksi_cmp_f           cmp;
    void                *data;
    KSI_DECLARE_LOCK(lock);
};


typedef struct Ksi_Valtab *ksi_valtab_t;


#ifdef __cplusplus
extern "C" {
#endif

SI_API
ksi_valtab_t
ksi_new_valtab (unsigned init_size, ksi_hash_f hash, ksi_cmp_f cmp, void *data);

SI_API
void
ksi_clear_vtab (ksi_valtab_t tab);

SI_API
void *
ksi_lookup_vtab (ksi_valtab_t tab, void *val, int append);

SI_API
void *
ksi_remove_vtab (ksi_valtab_t tab, void *val);

SI_API
void *
ksi_iterate_vtab (ksi_valtab_t tab, ksi_iter_f fun, void *iter_data);

SI_API
unsigned *
ksi_get_primes(unsigned *size);

SI_API
unsigned
ksi_hash_str (const char* str, int len, unsigned n);


#ifdef __cplusplus
}
#endif


#endif

 /* End of file */
