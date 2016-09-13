/* -*-mode:C++-*- */
/*
 * ksi_comp.h
 * low level control structures
 *
 * Copyright (C) 1997-2009, Ivan Demakov
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
 * Creation date: Wed Oct 22 20:44:58 1997
 *
 * $Id: ksi_comp.h,v 1.3.4.7.2.1 2009/07/05 15:50:25 ksion Exp $
 *
 */

#ifndef KSI_COMP_H
#define KSI_COMP_H

typedef struct Ksi_Frame *ksi_frame;
typedef struct Ksi_Closure *ksi_closure;
typedef struct Ksi_Promise *ksi_promise;
typedef struct Ksi_Code *ksi_code;
typedef struct Ksi_Varbox *ksi_varbox;
typedef struct Ksi_Freevar *ksi_freevar;

typedef union Ksi_Var *ksi_var;


struct Ksi_Frame
{
  int num;
  ksi_frame next;
  ksi_obj env;
  ksi_obj vals[1];
};

struct Ksi_Closure
{
  unsigned itag;
  int nums, nary, opts;
  ksi_frame frm;
  ksi_obj body;
  ksi_obj doc;
};

struct Ksi_Promise
{
  unsigned itag;
  ksi_obj val;
  ksi_frame frm;
  ksi_obj body;
};

struct Ksi_Code
{
  unsigned itag;
  int num;
  ksi_obj val [1];
};

struct Ksi_Varbox
{
  unsigned itag;
  int lev;
  int num;
};

struct Ksi_Freevar
{
  unsigned itag;
  ksi_obj sym;
  ksi_obj *val;
};

union Ksi_Var
{
  unsigned itag;
  struct Ksi_Code code;
  struct Ksi_Freevar var;
  struct Ksi_Varbox box;
};


#define KSI_CODE_P(x)    (KSI_OBJ_P (x) && KSI_TAG_FIRST_CODE <= KSI_OBJ_TAG (x) && KSI_OBJ_TAG (x) <= KSI_TAG_LAST_CODE)
#define KSI_FREEVAR_P(x) (KSI_OBJ_P (x) && KSI_TAG_FREEVAR <= KSI_OBJ_TAG (x) && KSI_OBJ_TAG (x) <= KSI_TAG_GLOBAL)
#define KSI_LOCVAR_P(x)  (KSI_OBJ_P (x) && KSI_TAG_VAR0 <= KSI_OBJ_TAG (x) && KSI_OBJ_TAG (x) <= KSI_TAG_VARN)
#define KSI_IMMOP_P(x)   (KSI_OBJ_P (x) && KSI_TAG_FIRST_OP <= KSI_OBJ_TAG (x) && KSI_OBJ_TAG (x) <= KSI_TAG_LAST_OP)


#define KSI_VAR_NUM(x)    (((ksi_varbox) (x)) -> num)
#define KSI_VAR_LEV(x)    (((ksi_varbox) (x)) -> lev)
#define KSI_VAR_SYM(x)    (((ksi_freevar) (x)) -> sym)
#define KSI_VAR_VAL(x)    (*(((ksi_freevar) (x)) -> val))

#define KSI_CODE_NUM(x)   (((ksi_code) (x)) -> num)
#define KSI_CODE_VAL(x,n) (((ksi_code) (x)) -> val [n])

#define KSI_QUOTE_VAL(x)  KSI_CODE_VAL ((x), 0)

#define KSI_IF_TEST(x)    KSI_CODE_VAL ((x), 0)
#define KSI_IF_THEN(x)    KSI_CODE_VAL ((x), 1)
#define KSI_IF_ELSE(x)    KSI_CODE_VAL ((x), 2)

#define KSI_CLOS_NUMS(x)  (((ksi_closure) (x)) -> nums)
#define KSI_CLOS_NARY(x)  (((ksi_closure) (x)) -> nary)
#define KSI_CLOS_OPTS(x)  (((ksi_closure) (x)) -> opts)
#define KSI_CLOS_FRM(x)   (((ksi_closure) (x)) -> frm)
#define KSI_CLOS_BODY(x)  (((ksi_closure) (x)) -> body)
#define KSI_CLOS_DOC(x)  (((ksi_closure) (x)) -> doc)

#define KSI_PROMISE_P(x)    (KSI_OBJ_IS ((x), KSI_TAG_PROMISE))
#define KSI_PROMISE_VAL(x)  (((ksi_promise) (x)) -> val)
#define KSI_PROMISE_FRM(x)  (((ksi_promise) (x)) -> frm)
#define KSI_PROMISE_BODY(x) (((ksi_promise) (x)) -> body)


#ifdef __cplusplus
extern "C" {
#endif

SI_API
ksi_frame
ksi_new_frame (int size, ksi_frame frm);

SI_API
ksi_obj
ksi_new_code (int n, int tag);

SI_API
ksi_obj
ksi_new_quote (ksi_obj x);

SI_API
ksi_obj
ksi_new_freevar (ksi_obj sym);

SI_API
ksi_obj
ksi_new_varbox (int lev, int num);

SI_API
ksi_closure
ksi_new_closure (int nums, int nary, int opts, ksi_frame frm, ksi_obj body);

SI_API
ksi_obj
ksi_comp (ksi_obj form, ksi_obj env);

SI_API
ksi_obj
ksi_eval_sexp (ksi_obj sexp, ksi_obj env);

SI_API
ksi_obj
ksi_eval_code (ksi_obj form, ksi_frame frm);

SI_API
ksi_obj
ksi_eval (ksi_obj form, ksi_obj env);

SI_API
ksi_obj
ksi_eval_with_catch (ksi_obj form, ksi_obj env);

SI_API
ksi_obj
ksi_eval_str (const char* scheme_code);

SI_API
ksi_obj
ksi_eval_str_with_catch (const char* scheme_code);

SI_API
ksi_obj
ksi_call_with_values (ksi_obj thunk, ksi_obj proc);

SI_API
ksi_obj
ksi_force (ksi_obj x);

#ifdef __cplusplus
}
#endif


#endif

 /* End of file */
