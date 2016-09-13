/* -*-mode:C++-*- */
/*
 * ksi_hash.h
 * hash tables
 *
 * Copyright (C) 1998-2009, Ivan Demakov.
 * All rights reserved.
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
 * Creation date: Tue Feb 17 18:42:42 1998
 *
 *
 * $Id: ksi_hash.h,v 1.1.1.1.4.2 2009/06/25 11:22:57 ksion Exp $
 *
 */

#ifndef KSI_HASH_H
#define KSI_HASH_H

#include "ksi_vtab.h"


struct Ksi_Hashtab
{
  unsigned itag;
  ksi_valtab_t table;
  ksi_obj hash;
  ksi_obj cmp;
};

#define KSI_HASHTAB_P(x)	(KSI_OBJ_IS ((x), KSI_TAG_HASHTAB))
#define KSI_HASHTAB_TAB(x)	(((ksi_hashtab) (x)) -> table)
#define KSI_HASHTAB_HASH(x)	(((ksi_hashtab) (x)) -> hash)
#define KSI_HASHTAB_CMP(x)	(((ksi_hashtab) (x)) -> cmp)


SI_API
unsigned
ksi_hash_obj (ksi_obj obj, unsigned n);

SI_API
ksi_obj
ksi_hash (ksi_obj x, ksi_obj n);

SI_API
ksi_obj
ksi_hashv (ksi_obj x, ksi_obj n);

SI_API
ksi_obj
ksi_hashq (ksi_obj x, ksi_obj n);

SI_API
ksi_obj
ksi_hashtab_p (ksi_obj x);

SI_API
ksi_obj
ksi_new_hashtab (int size, ksi_obj hash, ksi_obj cmp);

SI_API
ksi_obj
ksi_make_hashtab (ksi_obj size, ksi_obj hash, ksi_obj cmp);

SI_API
ksi_obj
ksi_hash_ref (ksi_obj tab, ksi_obj key, ksi_obj def);

SI_API
ksi_obj
ksi_hash_set_x (ksi_obj tab, ksi_obj key, ksi_obj val);

SI_API
ksi_obj
ksi_hash_remove_x (ksi_obj tab, ksi_obj key);

SI_API
ksi_obj
ksi_hash_map (ksi_obj proc, ksi_obj tab);

SI_API
ksi_obj
ksi_hash_for_each (ksi_obj proc, ksi_obj tab);


#endif

 /* End of file */
