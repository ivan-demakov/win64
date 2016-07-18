/* -*-mode:C++-*- */
/*
 * ksi_klos.h
 * ksi object system -- internal defs
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
 * Creation date: Thu Jun 12 19:51:55 1997
 * Last Update:   Sat Aug  2 04:10:49 2014
 *
 */

#ifndef KSI_KLOS_H
#define KSI_KLOS_H

typedef struct Ksi_Instance *ksi_instance;
typedef struct Ksi_NextMethod *ksi_next_method;


enum klos_instance_flags
{
    I_CLASS             = 1 << 0,
    I_GENERIC           = 1 << 1,
    I_METHOD            = 1 << 2,
    I_PURE_CLASS        = 1 << 3,
    I_PURE_GENERIC      = 1 << 4,
    I_PURE_METHOD       = 1 << 5,
    I_METHODS_SORTED    = 1 << 6
};

enum klos_class_slots
{
    S_CNAME,                    /**< class name */
    S_DSUPERS,                  /**< list of direct supers */
    S_DSLOTS,                   /**< list of direct slots */
    S_DEFARGS,                  /**< list of default initargs */
    S_CPL,                      /**< class precedence list */
    S_SLOTS,                    /**< list of all slots */
    S_NFIELDS,                  /**< number of fields */
    S_GNS,                      /**< get_n_sets */

    NUMBER_OF_CLASS_SLOTS
};

enum klos_generic_slots
{
  S_GNAME,                      /**< generic name */
  S_METHODS,                    /**< list of methods */
  S_ARITY,                      /**< list of formals */

  NUMBER_OF_GENERIC_SLOTS
};

enum klos_method_slots
{
  S_GF,                         /**< generic function */
  S_SPECS,                      /**< specializers */
  S_COMBINATION,                /**< combination */
  S_PROC,                       /**< procedure */

  NUMBER_OF_METHOD_SLOTS
};

enum klos_gns_index
{
  GNS_NAME,
  GNS_ALLOC,
  GNS_GETTER,
  GNS_SETTER,
  GNS_INIT,
  GNS_TYPE,

  GNS_SIZE
};

#define S_cname         L"name"
#define S_dsupers       L"direct-supers"
#define S_dslots        L"direct-slots"
#define S_defargs       L"default-initargs"
#define S_cpl           L"cpl"
#define S_slots         L"slots"
#define S_nfields       L"nfields"
#define S_gns           L"get-n-set"

#define S_gname         L"name"
#define S_methods       L"methods"
#define S_arity         L"arity"

#define S_gf            L"generic-function"
#define S_specs         L"specializers"
#define S_proc          L"procedure"
#define S_combination   L"combination"
#define S_before        L"before"
#define S_after         L"after"
#define S_around        L"around"
#define S_primary       L"primary"

#define K_initform      L"initform"
#define K_initarg       L"initarg"
#define K_defargs       L"default-initargs"
#define K_type          L"type"
#define K_name          L"name"
#define K_dsupers       L"supers"
#define K_dslots        L"slots"
#define K_specs         L"specializers"
#define K_proc          L"procedure"
#define K_gf            L"generic-function"
#define K_arity         L"arity"
#define K_combination   L"combination"


struct Ksi_Instance
{
  struct Ksi_ObjData o;

  int flags;
  ksi_instance klass;
  ksi_obj *slots;
};

struct Ksi_NextMethod
{
  struct Ksi_ObjData o;

  ksi_obj procs;
  ksi_obj args;
  ksi_obj gf;
};

#define KSI_INST_P(x)           KSI_OBJ_IS((x), KSI_TAG_INSTANCE)
#define KSI_NEXT_METHOD_P(x)    KSI_OBJ_IS((x), KSI_TAG_NEXT_METHOD)

#define KSI_INST_IS(x,f)        (((ksi_instance) (x)) -> flags & (f))

#define KSI_METHOD_P(x)         (KSI_INST_P(x) && KSI_INST_IS((x), I_METHOD))
#define KSI_GENERIC_P(x)        (KSI_INST_P(x) && KSI_INST_IS((x), I_GENERIC))
#define KSI_CLASS_P(x)          (KSI_INST_P(x) && KSI_INST_IS((x), I_CLASS))

#define KSI_PURE_GENERIC_P(x)   (KSI_INST_P(x) && KSI_INST_IS((x), I_PURE_GENERIC))
#define KSI_PURE_CLASS_P(x)     (KSI_INST_P(x) && KSI_INST_IS((x), I_PURE_CLASS))
#define KSI_PURE_METHOD_P(x)    (KSI_INST_P(x) && KSI_INST_IS((x), I_PURE_METHOD))

#define KSI_CLASS_OF(x)         (((ksi_instance) (x)) -> klass)
#define KSI_SLOTS_PTR(x)        (((ksi_instance) (x)) -> slots)
#define KSI_SLOT_REF(x,r)       (KSI_SLOTS_PTR (x) [r])

#define KSI_NEXT_PROCS(x)       (((ksi_next_method) (x)) -> procs)
#define KSI_NEXT_ARGS(x)        (((ksi_next_method) (x)) -> args)
#define KSI_NEXT_GF(x)          (((ksi_next_method) (x)) -> gf)


SI_API
const wchar_t *
ksi_inst2str (ksi_instance x);

SI_API
ksi_obj
ksi_instance_p (ksi_obj x);

SI_API
ksi_obj
ksi_class_p (ksi_obj x);

SI_API
ksi_obj
ksi_generic_p (ksi_obj x);

SI_API
ksi_obj
ksi_method_p (ksi_obj x);

SI_API
ksi_obj
ksi_slot_ref (ksi_obj obj, ksi_obj name);

SI_API
ksi_obj
ksi_slot_set (ksi_obj obj, ksi_obj name, ksi_obj val);

SI_API
ksi_obj
ksi_slot_bound_p (ksi_obj obj, ksi_obj name);

SI_API
ksi_obj
ksi_slot_exist_p (ksi_obj obj, ksi_obj name);

SI_API
ksi_obj
ksi_slot_exist_in_class_p (ksi_obj cls, ksi_obj name);

SI_API
ksi_obj
ksi_class_of (ksi_obj x);

SI_API
ksi_obj
ksi_type_p (ksi_obj type, ksi_obj x);

SI_API
ksi_obj
ksi_type_of (ksi_obj x);

SI_API
ksi_obj
ksi_get_arg (ksi_obj key, ksi_obj args, ksi_obj def);

SI_API
ksi_obj
ksi_new_next (ksi_obj gf, ksi_obj args, ksi_obj procs);

SI_API
void
ksi_write_inst (ksi_obj x, ksi_char_port p, int slashify);

SI_API
ksi_obj
ksi_inst_eqv_p (ksi_obj o1, ksi_obj o2);

SI_API
ksi_obj
ksi_inst_equal_p (ksi_obj o1, ksi_obj o2);

SI_API
ksi_obj
ksi_inst_slot_unbound (ksi_obj cls, ksi_obj obj, ksi_obj slot);

SI_API
ksi_obj
ksi_inst_slot_missing (ksi_obj cls, ksi_obj obj, ksi_obj slot, ksi_obj val);

SI_API
unsigned int
ksi_hash_inst (ksi_obj x, unsigned n, unsigned d);

SI_API
ksi_obj
ksi_compute_applicable_methods (ksi_obj gf, ksi_obj args, ksi_obj combination);

SI_API
ksi_obj
ksi_compute_effective_method (ksi_obj gf, ksi_obj args);


/* internal and not public declarations */

void
ksi_init_top_classes (void);

ksi_obj
ksi_klos_val (ksi_obj sym, ksi_obj def);


#endif

/* End of code */
