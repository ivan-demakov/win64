/* -*-mode:C++-*- */
/*
 * ksi_comp.h
 * low level control structures
 *
 * Copyright (C) 1997-2011, 2014, Ivan Demakov
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
 * Creation date: Wed Oct 22 20:44:58 1997
 * Last Update:   Fri Apr 25 23:01:58 2014
 *
 */

#ifndef KSI_COMP_H
#define KSI_COMP_H

#include "ksi_type.h"


typedef struct Ksi_Frame *ksi_frame;
typedef struct Ksi_Closure *ksi_closure;
typedef struct Ksi_Code *ksi_code;
typedef struct Ksi_Varbox *ksi_varbox;
typedef struct Ksi_Freevar *ksi_freevar;
typedef struct Ksi_EnvInfo *ksi_envinfo;

typedef union Ksi_Var *ksi_var;


struct Ksi_Frame
{
    ksi_frame next;
    ksi_env env;
    int num;
    ksi_obj vals[1];
};

struct Ksi_Closure
{
    struct Ksi_ObjData o;

    int nums, nary, opts;
    ksi_frame frm;
    ksi_obj body;
    ksi_obj doc;
};

struct Ksi_Code
{
    struct Ksi_ObjData o;

    int num;
    ksi_obj val[1];
};

struct Ksi_Varbox
{
    struct Ksi_ObjData o;

    int lev, num;
};

struct Ksi_Freevar
{
    struct Ksi_ObjData o;

    ksi_obj sym;
    ksi_env env;
    ksi_envrec val;
};


#define KSI_CODE_P(x)       (KSI_TAG_FIRST_CODE <= (x)->o.itag && (x)->o.itag < KSI_TAG_LAST_CODE)
#define KSI_CODE_NUM(x)     (((ksi_code) (x)) -> num)
#define KSI_CODE_VAL(x,n)   (((ksi_code) (x)) -> val [n])

#define KSI_QUOTE_VAL(x)    KSI_CODE_VAL ((x), 0)
#define KSI_IF_TEST(x)      KSI_CODE_VAL ((x), 0)
#define KSI_IF_THEN(x)      KSI_CODE_VAL ((x), 1)
#define KSI_IF_ELSE(x)      KSI_CODE_VAL ((x), 2)

#define KSI_FREEVAR_P(x)   (KSI_TAG_FREEVAR <= (x)->o.itag && (x)->o.itag <= KSI_TAG_LOCAL)
#define KSI_FREEVAR_SYM(x) (((ksi_freevar) (x)) -> sym)
#define KSI_FREEVAR_ENV(x) (((ksi_freevar) (x)) -> env)
#define KSI_FREEVAR_VAL(x) (((ksi_freevar) (x)) -> val)

#define KSI_VARBOX_P(x)    (KSI_TAG_VAR0 <= (x)->o.itag && (x)->o.itag <= KSI_TAG_VARN)
#define KSI_VARBOX_NUM(x)  (((ksi_varbox) (x)) -> num)
#define KSI_VARBOX_LEV(x)  (((ksi_varbox) (x)) -> lev)

#define KSI_CLOS_NUMS(x)   (((ksi_closure) (x)) -> nums)
#define KSI_CLOS_NARY(x)   (((ksi_closure) (x)) -> nary)
#define KSI_CLOS_OPTS(x)   (((ksi_closure) (x)) -> opts)
#define KSI_CLOS_FRM(x)    (((ksi_closure) (x)) -> frm)
#define KSI_CLOS_BODY(x)   (((ksi_closure) (x)) -> body)
#define KSI_CLOS_DOC(x)    (((ksi_closure) (x)) -> doc)


#ifdef __cplusplus
extern "C" {
#endif

SI_API
ksi_frame
ksi_new_frame (int size, ksi_frame frm);

SI_API
ksi_code
ksi_new_code (int n, int tag);

SI_API
ksi_obj
ksi_new_quote (ksi_obj x);

SI_API
ksi_obj
ksi_new_freevar (ksi_obj sym, ksi_env env);

SI_API
ksi_obj
ksi_new_varbox (int lev, int num);

SI_API
ksi_closure
ksi_new_closure (int nums, int nary, int opts, ksi_frame frm, ksi_obj body);

SI_API
ksi_obj
ksi_environment (int argc, ksi_obj *argv);

SI_API
ksi_obj
ksi_call_with_values (ksi_obj thunk, ksi_obj proc);

SI_API
ksi_obj
ksi_closure_env (ksi_obj clos);

SI_API
ksi_obj
ksi_closure_body (ksi_obj clos);

SI_API
ksi_obj
ksi_closure_p (ksi_obj clos);

SI_API
ksi_obj
ksi_call_in_context (ksi_context_t ctx, void *data, ksi_obj (*proc)(ksi_context_t, void*), ksi_obj (*error)(ksi_context_t, ksi_obj, ksi_obj));

SI_API
ksi_obj
ksi_eval_code (ksi_obj code, ksi_frame frm);

SI_API
ksi_obj
ksi_comp (ksi_obj form, ksi_env env);

SI_API
ksi_obj
ksi_eval (ksi_obj form, ksi_env env);

SI_API
ksi_obj
ksi_comp_string (const wchar_t *scheme_code, ksi_env env, const char *file_name, int file_line);

SI_API
ksi_obj
ksi_eval_string (const wchar_t *scheme_code, ksi_env env, const char *file_name, int file_line, ksi_obj *compiled_code);

SI_API
ksi_obj
ksi_comp_bytes (const char *scheme_code, int len, ksi_env env, const char *file_name, int file_line);

SI_API
ksi_obj
ksi_eval_bytes (const char *scheme_code, int len, ksi_env env, const char *file_name, int file_line, ksi_obj *compiled_code);


/* internal functions */

ksi_obj
ksi_bound_identifier_p (ksi_obj id, ksi_envinfo env);

ksi_obj
ksi_letstar_macro (ksi_obj form, ksi_envinfo env);

ksi_obj
ksi_case_macro (ksi_obj form, ksi_envinfo env);

ksi_obj
ksi_cond_macro (ksi_obj form, ksi_envinfo env);

ksi_obj
ksi_quasiquote_macro (ksi_obj form, ksi_envinfo env);

ksi_obj
ksi_quasisyntax_macro (ksi_obj form, ksi_envinfo env);


#ifdef __cplusplus
}
#endif


#endif

 /* End of file */
