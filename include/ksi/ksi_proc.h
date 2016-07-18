/*
 * ksi_proc.h
 * primitive functions
 *
 * Copyright (C) 1997-2010, Ivan Demakov.
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
 * Author:        ivan demakov <ksion@users.sourceforge.net>
 * Creation date: Thu Nov 27 03:24:35 1997
 * Last Update:   Wed Oct 13 04:10:54 2010
 *
 */

/**
 * @file   ksi_proc.h
 * @author ivan demakov <ksion@users.sourceforge.net>
 * @date   Thu Nov 27 03:24:35 1997
 *
 * @brief Procedure utils.
 *
 */

#ifndef KSI_PROC_H
#define KSI_PROC_H

#include "ksi_env.h"

/** Pointer to the ::Ksi_Prim.
 *
 */
typedef struct Ksi_Prim* ksi_prim;


/** How to call the primitive function.
 *
 * For \a KSI_CALL_ARGn, where \a n is a number from 0 to 9,
 * the primitive function should return \a ksi_obj and accept \a n arguments of the type \a ksi_obj.
 * The function is called with exactly \a n arguments in any case,
 * but if a primitive is defined with a less then \a n required arguments
 * and the number of supplied arguments in a primitive call is less than \a n (but equal or more of the number of required arguments),
 * the zero value is passed for all missed arguments.
 * The maximum number of argumens for the primitive is \a n.
 *
 * For \a KSI_CALL_RESTn, where \a n is a number from 0 to 9,
 * all is true as for \a KSI_CALL_ARGn, except the maximum number of arguments is not limited.
 * Additional arguments (if any) is passed in \a argv and the number of additional arguments is pased in \a argc.
 *
 * Any other use of the primitive functions at your own risk.
 * (For example, sometimes useful to create the primitive function with argument that has other pointer type than \a ksi_obj,
 * and create the closure with the value of this type).
 */
typedef enum
{
  KSI_CALL_ARG0,                /**< ksi_obj fun(void) */
  KSI_CALL_ARG1,                /**< ksi_obj fun(ksi_obj a1) */
  KSI_CALL_ARG2,                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2) */
  KSI_CALL_ARG3,                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3) */
  KSI_CALL_ARG4,                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4) */
  KSI_CALL_ARG5,                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5) */
  KSI_CALL_ARG6,                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6) */
  KSI_CALL_ARG7,                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7 */
  KSI_CALL_ARG8,                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, ksi_obj a8) */
  KSI_CALL_ARG9,                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, ksi_obj a8, ksi_obj a9) */

  KSI_CALL_NUM,

  KSI_CALL_REST0 = KSI_CALL_NUM, /**< ksi_obj fun(int argc, ksi_obj *argv) */
  KSI_CALL_REST1,               /**< ksi_obj fun(ksi_obj a1, int argc, ksi_obj *argv) */
  KSI_CALL_REST2,               /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, int argc, ksi_obj *argv) */
  KSI_CALL_REST3,               /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, int argc, ksi_obj *argv) */
  KSI_CALL_REST4,               /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, int argc, ksi_obj *argv) */
  KSI_CALL_REST5,               /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, int argc, ksi_obj *argv) */
  KSI_CALL_REST6,               /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, int argc, ksi_obj *argv) */
  KSI_CALL_REST7,               /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, int argc, ksi_obj *argv) */
  KSI_CALL_REST8,               /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, ksi_obj a8, int argc, ksi_obj *argv) */
  KSI_CALL_REST9                /**< ksi_obj fun(ksi_obj a1, ksi_obj a2, ksi_obj a3, ksi_obj a4, ksi_obj a5, ksi_obj a6, ksi_obj a7, ksi_obj a8, ksi_obj a9, int argc, ksi_obj *argv) */
} ksi_call_t;


/** Opaque type for the primitive function.
 * The type used as general type for the all primitive functions.
 * The real type of the function should be one of \a ksi_call_argN or \a ksi_call_restN,
 * where \a N is a number from 0 to 9.
 */
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


/** Primitive function.
 * The type describes the primitive function and the primitive arguments.
 * Used internaly.
 */
struct Ksi_Prim
{
    struct Ksi_ObjData o;       /**< ksi_obj part */

    ksi_proc_t proc;            /**< pointer to the function */
    ksi_call_t call;            /**< number and type of arguments of the function */
    int reqv;                   /**< number of required arguments */
    int has_rest;               /**< has the optional arguments? */
    const wchar_t *name;        /**< the name of the primitive (if any) */
};

#define KSI_PRIM_PROC(x)	(((ksi_prim) (x)) -> proc)
#define KSI_PRIM_CALL(x)	(((ksi_prim) (x)) -> call)
#define KSI_PRIM_REQV(x)	(((ksi_prim) (x)) -> reqv)
#define KSI_PRIM_HAS_REST(x)	(((ksi_prim) (x)) -> has_rest)
#define KSI_PRIM_NAME(x)	(((ksi_prim) (x)) -> name)


/** Primitive closure.
 * The type describes the closure of a procedure.
 * Used internaly.
 */
typedef struct Ksi_Prim_Closure
{
    struct Ksi_ObjData o;       /**< ksi_obj data */

    int argc;                   /**< number of closed arguments */
    ksi_obj proc;               /**< the procedure  */
    ksi_obj argv[1];            /**< open ended array of the closed arguments */
} *ksi_prim_closure;


/** Description of the primitive.
 *
 * See ksi_reg_unit()
 */
struct Ksi_Prim_Def
{
    const wchar_t *name;        /**< name of the primitive */
    ksi_proc_t proc;            /**< pointer to the function */
    ksi_call_t call;            /**< number and type of the arguments */
    int reqv;                   /**< number of the required arguments */
};


/** Test primitive.
 * Used internaly.
 *
 * @param x the object
 *
 * @return is \a x is a primitive?
 */
#define KSI_PRIM_P(x)		(KSI_OBJ_IS ((x), KSI_TAG_PRIM)		\
				 || KSI_OBJ_IS ((x), KSI_TAG_PRIM_0)	\
				 || KSI_OBJ_IS ((x), KSI_TAG_PRIM_1)	\
				 || KSI_OBJ_IS ((x), KSI_TAG_PRIM_2)	\
				 || KSI_OBJ_IS ((x), KSI_TAG_PRIM_r)

/** Test procedure
 *
 * @param x the object
 *
 * @return is \a x a procedure?
 */
#define KSI_PROC_P(x)  (ksi_procedure_p ((x)) == ksi_true)



#ifdef __cplusplus
extern "C" {
#endif

/** Call the primitive function.
 * The fuction checks the number of the required arguments, takes care about optional arguments,
 * and calls the primitive function or raises the exception in case of missing or extra arguments.
 * Used internaly, for common use look at ksi_apply_proc(), ksi_apply(), ksi_apply_0(), ksi_apply_1(), etc.
 *
 * @param prim the primitive
 * @param ac number of the arguments
 * @param av the arguments
 *
 * @return the value returned by the primitive function
 */
SI_API
ksi_obj
ksi_apply_prim (ksi_prim prim, int ac, ksi_obj *av);


/** Create new primitive.
 * Note that with some combinations of \a call and \a reqv you can define something weird that can not be called reasonably.
 * The result of such definition is undefined up to core dump.
 *
 * @param name the name of the primitive (can be 0 if name is unnecessary)
 * @param proc the pointer to the function
 * @param call type and number of arguments of the function
 * @param reqv number of required arguments
 *
 * @return
 */
SI_API
ksi_prim
ksi_new_prim(const wchar_t *name, ksi_proc_t proc, ksi_call_t call, int reqv);


/** Create new primitive and bind it.
 * The function creates the primitive (see ksi_new_prim()),
 * binds the primitive to \a name variable and exports it.
 *
 * @param name the name of the primitive (in difference of ksi_new_prim() can not be 0)
 * @param proc the pointer to the function
 * @param call type and number of arguments of the function
 * @param reqv number of required arguments
 * @param env the environment in which the primitive is bound
 *
 * @return unspecified
 */
SI_API
ksi_obj
ksi_defun (const wchar_t *name, ksi_proc_t proc, ksi_call_t call, int reqv, ksi_env env);


/** Create and define a number of primitives.
 * The function calls ksi_defun for each element in \a prims.
 * Last element in \a prims should be 0 as a sentinel.
 *
 * @param prims the primitive descriptions
 * @param env the environment in which the primitives are bound
 */
SI_API
void
ksi_reg_unit (struct Ksi_Prim_Def* prims, ksi_env env);


/** Create closure.
 * The closure created is a procedure that, when called,
 * calls the \a proc with \a argc arguments from \a argv and arguments passed to the closure call itself.
 *
 * @param proc the procedure
 * @param argc the number of closed arguments
 * @param argv the closed arguments
 *
 * @return the created closure
 */
SI_API
ksi_obj
ksi_close_proc (ksi_obj proc, int argc, ksi_obj *argv);


/** Call the closure.
 * Used internaly, for common use look at ksi_apply_proc(), ksi_apply(), ksi_apply_0(), ksi_apply_1(), etc.
 *
 * @param clos the closure
 * @param argc the number of the arguments
 * @param argv the arguments
 *
 * @return the value returned by the closure call
 */
SI_API
ksi_obj
ksi_apply_prim_closure (ksi_prim_closure clos, int argc, ksi_obj *argv);


/** Procedure?
 * The function checks that \a x is a procedure object.
 * The KSi has a number of types of procedure objects,
 * such as primitive function, closure or other (subject to change in future releases).
 *
 * @param x the object
 *
 * @return \a ksi_true if \a x is a procedure, \a ksi_false otherwise.
 */
SI_API
ksi_obj
ksi_procedure_p (ksi_obj x);


/** Apply the procedure to the arguments.
 *
 *
 * @param proc the procedure
 * @param argc the number of arguments
 * @param argv the arguments
 *
 * @return the procedure returned value
 */
SI_API
ksi_obj
ksi_apply_proc (ksi_obj proc, int argc, ksi_obj* argv);


/** Apply the procedure without arguments.
 * The function calls \a proc passing no arguments.
 *
 * @param proc the procedure
 *
 * @return the procedure returned value
 */
SI_API
ksi_obj
ksi_apply_0 (ksi_obj proc);


/** Apply the procedure to the arguments.
 * The function calls \a proc passing one argument.
 *
 * @param proc the procedure
 * @param arg1 the 1st argument
 *
 * @return the procedure returned value
 */
ksi_obj
ksi_apply_1 (ksi_obj proc, ksi_obj arg1);


/** Apply the procedure to the arguments.
 * The function calls \a proc passing two arguments.
 *
 * @param proc the procedure
 * @param arg1 the 1st argument
 * @param arg2 the 2nd argument
 *
 * @return the procedure returned value
 */
SI_API
ksi_obj
ksi_apply_2 (ksi_obj proc, ksi_obj arg1, ksi_obj arg2);


/** Apply the procedure to the arguments.
 * The function calls \a proc passing 3 arguments.
 *
 * @param proc the procedure
 * @param arg1 the 1st argument
 * @param arg2 the 2nd argument
 * @param arg3 the 3rd argument
 *
 * @return the procedure returned value
 */
SI_API
ksi_obj
ksi_apply_3 (ksi_obj proc, ksi_obj arg1, ksi_obj arg2, ksi_obj arg3);


/** Apply the procedure to the arguments.
 * The function calls \a proc passing the arguments in \a arg_list.
 *
 *
 * @param proc the procedure
 * @param arg_list the list of arguments
 *
 * @return the procedure returned value
 */
SI_API
ksi_obj
ksi_apply (ksi_obj proc, ksi_obj arg_list);


/** Get procedure arity.
 * The arity of a procedure is the (minimal) number of arguments that the procudere accepts.
 *
 * But for some procedure objects (such as general functions, for example),
 * it is too complex or impossible to calculate the number of argument that the procedure object accepts,
 * in such cases, the functions returns zero.
 *
 * @param proc procedure
 *
 * @return non-negative integer
 */
SI_API
ksi_obj
ksi_procedure_arity (ksi_obj proc);

/** Test that procedure can be called with the number of arguments.
 * This functions returns \a false if \a proc definitely can not be called
 * with \a argnum arguments.
 *
 * For some procedures (such as general functions or procedures with optional arguments)
 * the only way is to try. In such case, the function returns \a true too.
 *
 * Sometimes, you need check that \a proc can be called with at minimum \a argum arguments
 * (for example, when you make closure that, when called, supplied with additional arguments).
 * If such, look at \a with_opts parameter.
 *
 * @param proc procedure
 * @param argnum number of arguments
 * @param with_opts if not \a false, do test in suppose that additioanal arguments can be supplied
 *
 * @return false iff the procedure can not accept \a argnum arguments, true otherwise.
 */
SI_API
ksi_obj
ksi_procedure_has_arity_p (ksi_obj proc, ksi_obj argnum, ksi_obj with_opts);


#ifdef __cplusplus
}
#endif

#endif

 /* End of file */
