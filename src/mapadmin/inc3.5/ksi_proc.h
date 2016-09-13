/*
 * ksi_proc.h
 * primitive functions
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
 * Creation date: Thu Nov 27 03:24:35 1997
 *
 * $Id: ksi_proc.h,v 1.1.2.4.2.1 2009/07/05 15:50:25 ksion Exp $
 *
 */

#ifndef KSI_PROC_H
#define KSI_PROC_H

#include "ksi_env.h"


typedef struct Ksi_Prim           *ksi_prim;

typedef enum
{
  KSI_CALL_ARG0,
  KSI_CALL_ARG1,
  KSI_CALL_ARG2,
  KSI_CALL_ARG3,
  KSI_CALL_ARG4,
  KSI_CALL_ARG5,
  KSI_CALL_ARG6,
  KSI_CALL_ARG7,
  KSI_CALL_ARG8,
  KSI_CALL_ARG9,

  KSI_CALL_NUM,

  KSI_CALL_REST0 = KSI_CALL_NUM,
  KSI_CALL_REST1,
  KSI_CALL_REST2,
  KSI_CALL_REST3,
  KSI_CALL_REST4,
  KSI_CALL_REST5,
  KSI_CALL_REST6,
  KSI_CALL_REST7,
  KSI_CALL_REST8,
  KSI_CALL_REST9
} ksi_call_t;


typedef ksi_obj (*ksi_proc_t) ();

typedef ksi_obj (*ksi_call_arg0) (void);
typedef ksi_obj (*ksi_call_arg1) (ksi_obj a1);
typedef ksi_obj (*ksi_call_arg2) (ksi_obj a1, ksi_obj a2);
typedef ksi_obj (*ksi_call_arg3) (ksi_obj a1, ksi_obj a2, ksi_obj a3);
typedef ksi_obj (*ksi_call_arg4) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4);
typedef ksi_obj (*ksi_call_arg5) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5);
typedef ksi_obj (*ksi_call_arg6) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6);
typedef ksi_obj (*ksi_call_arg7) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7);
typedef ksi_obj (*ksi_call_arg8) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, ksi_obj a8);
typedef ksi_obj (*ksi_call_arg9) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, ksi_obj a8, ksi_obj a9);

typedef ksi_obj (*ksi_call_rest0) (int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest1) (ksi_obj a1, int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest2) (ksi_obj a1, ksi_obj a2, int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest3) (ksi_obj a1, ksi_obj a2, ksi_obj a3, int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest4) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest5) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest6) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest7) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest8) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, ksi_obj a8, int ac, ksi_obj *av);
typedef ksi_obj (*ksi_call_rest9) (ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, ksi_obj a8, ksi_obj a9, int ac, ksi_obj *av);


struct Ksi_Prim
{
  unsigned itag;
  ksi_proc_t proc;
  ksi_call_t call;
  int reqv, has_rest;
  const char *name;
};

#define KSI_PRIM_PROC(x)	(((ksi_prim) (x)) -> proc)
#define KSI_PRIM_CALL(x)	(((ksi_prim) (x)) -> call)
#define KSI_PRIM_REQV(x)	(((ksi_prim) (x)) -> reqv)
#define KSI_PRIM_HAS_REST(x)	(((ksi_prim) (x)) -> has_rest)
#define KSI_PRIM_NAME(x)	(((ksi_prim) (x)) -> name)


typedef struct Ksi_Prim_Closure
{
  int itag;
  int argc;
  ksi_obj prim;
  ksi_obj argv[1];
} *ksi_prim_closure;


struct Ksi_Prim_Def
{
  const char *name;	/* name of primitive */
  ksi_proc_t proc;	/* function */
  ksi_call_t call;	/* call type */
  int reqv;		/* number of required args */
};


#define KSI_PRIM_P(x)		(KSI_OBJ_IS ((x), KSI_TAG_PRIM)		\
				 || KSI_OBJ_IS ((x), KSI_TAG_PRIM_0)	\
				 || KSI_OBJ_IS ((x), KSI_TAG_PRIM_1)	\
				 || KSI_OBJ_IS ((x), KSI_TAG_PRIM_2)	\
				 || KSI_OBJ_IS ((x), KSI_TAG_PRIM_r)

#define KSI_PROC_P(x)	(ksi_procedure_p ((x)) == ksi_true)



#ifdef __cplusplus
extern "C" {
#endif

SI_API
ksi_obj
ksi_apply_prim (ksi_prim prim, int ac, ksi_obj *av);

SI_API
ksi_prim
ksi_new_prim(const char *name, ksi_proc_t proc, ksi_call_t call, int reqv);

SI_API
ksi_obj
ksi_defun (const char *name, ksi_proc_t proc, ksi_call_t call, int reqv, ksi_env env);

SI_API
void
ksi_reg_unit (struct Ksi_Prim_Def* prims, ksi_env env);

SI_API
ksi_obj
ksi_close_proc (ksi_obj prim, int argc, ksi_obj *argv);

SI_API
ksi_obj
ksi_apply_prim_closure (ksi_prim_closure clos, int argc, ksi_obj *argv);

SI_API
ksi_obj
ksi_procedure_p (ksi_obj x);

SI_API
ksi_obj
ksi_apply_proc (ksi_obj proc, int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_apply_proc_with_catch (ksi_obj proc, int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_apply_0 (ksi_obj proc);

SI_API
ksi_obj
ksi_apply_1 (ksi_obj proc, ksi_obj arg1);

SI_API
ksi_obj
ksi_apply_2 (ksi_obj proc, ksi_obj arg1, ksi_obj arg2);

SI_API
ksi_obj
ksi_apply_3 (ksi_obj proc, ksi_obj arg1, ksi_obj arg2, ksi_obj arg3);

SI_API
ksi_obj
ksi_apply (ksi_obj proc, ksi_obj arg_list);

SI_API
ksi_obj
ksi_apply_0_with_catch (ksi_obj proc);

SI_API
ksi_obj
ksi_apply_1_with_catch (ksi_obj proc, ksi_obj arg1);

SI_API
ksi_obj
ksi_apply_2_with_catch (ksi_obj proc, ksi_obj arg1, ksi_obj arg2);

SI_API
ksi_obj
ksi_apply_3_with_catch (ksi_obj proc, ksi_obj arg1, ksi_obj arg2, ksi_obj arg3);

SI_API
ksi_obj
ksi_apply_with_catch (ksi_obj proc, ksi_obj arg_list);

SI_API
ksi_obj
ksi_procedure_arity (ksi_obj proc);

SI_API
ksi_obj
ksi_procedure_has_arity_p (ksi_obj proc, ksi_obj min_args, ksi_obj max_args);


#ifdef __cplusplus
}
#endif

#endif

 /* End of file */
