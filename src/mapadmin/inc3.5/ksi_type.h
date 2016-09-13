/*
 * ksi_type.h
 * public type, constant and macro
 *
 * Copyright (C) 1997-2009, ivan demakov
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
 * Creation date: Mon Dec 15 12:03:22 1997
 *
 *
 * $Id: ksi_type.h,v 1.18.4.19.2.2 2009/08/05 14:24:22 ksion Exp $
 *
 */

#ifndef KSI_TYPE_H
#define KSI_TYPE_H

#include "ksi_conf.h"
#include "ksi_vtab.h"


typedef struct Ksi_Obj *ksi_obj;
typedef struct Ksi_Symbol *ksi_symbol;
typedef struct Ksi_Symbol *ksi_keyword;
typedef struct Ksi_String *ksi_string;
typedef struct Ksi_Pair *ksi_pair;
typedef struct Ksi_Vector *ksi_vector;
typedef struct Ksi_Values *ksi_values;
typedef struct Ksi_Bignum *ksi_bignum;
typedef struct Ksi_Rational *ksi_rational;
typedef struct Ksi_Flonum *ksi_flonum;

typedef struct Ksi_ExtTag *ksi_ext_tag;
typedef struct Ksi_ExtObj *ksi_ext_obj;

typedef struct Ksi_Port *ksi_port;
typedef struct Ksi_Hashtab *ksi_hashtab;


enum ksi_tc2_t
{
    KSI_TC2_PTR = 0,
    KSI_TC2_INT = 1,
    KSI_TC2_IMM = 3
};

enum ksi_imm_t
{
    KSI_IMM_BOOL = KSI_TC2_IMM | (1 << 2),
    KSI_IMM_CHAR = KSI_TC2_IMM | (2 << 2),
    KSI_IMM_EXIT = KSI_TC2_IMM | (3 << 2),
    KSI_IMM_VOID = KSI_TC2_IMM | (4 << 2),
    KSI_IMM_NIL = KSI_TC2_IMM | (5 << 2),
    KSI_IMM_EOF = KSI_TC2_IMM | (6 << 2),
    KSI_IMM_ERR = KSI_TC2_IMM | (7 << 2),
    KSI_IMM_ANY = KSI_TC2_IMM | (8 << 2),

    KSI_IMM_LAST = KSI_TC2_IMM | (63 << 2)
};

enum ksi_tag_t
{
  KSI_TAG_FIRST_CONST,
  KSI_TAG_SYMBOL = KSI_TAG_FIRST_CONST,
  KSI_TAG_PAIR,
  KSI_TAG_CONST_PAIR,
  KSI_TAG_VECTOR,
  KSI_TAG_CONST_VECTOR,
  KSI_TAG_STRING,
  KSI_TAG_CONST_STRING,
  KSI_TAG_KEYWORD,
  KSI_TAG_BIGNUM,
  KSI_TAG_FLONUM,
  KSI_TAG_RATIONAL,
  KSI_TAG_COMPLEX,
  KSI_TAG_LAST_CONST = KSI_TAG_COMPLEX,

  KSI_TAG_VAR0,
  KSI_TAG_VAR1,
  KSI_TAG_VAR2,
  KSI_TAG_VARN,

  KSI_TAG_FREEVAR,
  KSI_TAG_LOCAL,
  KSI_TAG_IMPORTED,
  KSI_TAG_GLOBAL,

  KSI_TAG_FIRST_CODE,
  KSI_TAG_QUOTE = KSI_TAG_FIRST_CODE,
  KSI_TAG_BEGIN,
  KSI_TAG_AND,
  KSI_TAG_OR,
  KSI_TAG_IF2,
  KSI_TAG_IF3,
  KSI_TAG_DELAY,
  KSI_TAG_LAMBDA,
  KSI_TAG_DEFINE,
  KSI_TAG_SET,
  KSI_TAG_CALL,
  KSI_TAG_FRAME,
  KSI_TAG_LAST_CODE = KSI_TAG_FRAME,

  KSI_TAG_FIRST_OP,
  KSI_TAG_APPLY = KSI_TAG_FIRST_OP,
  KSI_TAG_CLOSE, /* new */
  KSI_TAG_CALL_CC,
  KSI_TAG_CALL_WITH_VALUES,
  KSI_TAG_NOT,
  KSI_TAG_EQP,
  KSI_TAG_EQVP,
  KSI_TAG_EQUALP,
  KSI_TAG_MEMQ,
  KSI_TAG_MEMV,
  KSI_TAG_MEMBER,
  KSI_TAG_CONS,
  KSI_TAG_CAR,
  KSI_TAG_CDR,
  KSI_TAG_LIST,
  KSI_TAG_MAKE_VECTOR, /* new */
  KSI_TAG_NULLP,
  KSI_TAG_PAIRP,
  KSI_TAG_LISTP,
  KSI_TAG_VECTORP,
  KSI_TAG_ADD, /* new */
  KSI_TAG_SUB, /* new */
  KSI_TAG_MUL, /* new */
  KSI_TAG_DIV, /* new */
  KSI_TAG_LAST_OP = KSI_TAG_DIV,

  KSI_TAG_FIRST_PROC,
  KSI_TAG_PRIM = KSI_TAG_FIRST_PROC,
  KSI_TAG_PRIM_0,
  KSI_TAG_PRIM_1,
  KSI_TAG_PRIM_2,
  KSI_TAG_PRIM_r,
  KSI_TAG_FUNC,
  KSI_TAG_CLOSURE,
  KSI_TAG_PRIM_CLOSURE,
  KSI_TAG_INSTANCE,
  KSI_TAG_NEXT_METHOD,
  KSI_TAG_LAST_PROC = KSI_TAG_NEXT_METHOD,

  KSI_TAG_FIRST_RUNTIME,
  KSI_TAG_PORT = KSI_TAG_FIRST_RUNTIME,
  KSI_TAG_CORE, /* new */
  KSI_TAG_PROMISE,
  KSI_TAG_VALUES,
  KSI_TAG_HASHTAB,
  KSI_TAG_ENVIRON,
  KSI_TAG_MODULE,
  KSI_TAG_ANNOTATION,
  KSI_TAG_EXN,
  KSI_TAG_EVENT,
  KSI_TAG_LAST_RUNTIME = KSI_TAG_EVENT,

  KSI_TAG_EXTENDED,
  KSI_TAG_BROKEN
};


struct Ksi_Data
{
  ksi_valtab_t symtab;
  ksi_valtab_t keytab;

  ksi_obj sym_and;
  ksi_obj sym_begin;
  ksi_obj sym_case;
  ksi_obj sym_cond;
  ksi_obj sym_define;
  ksi_obj sym_delay;
  ksi_obj sym_do;
  ksi_obj sym_else;
  ksi_obj sym_if;
  ksi_obj sym_lambda;
  ksi_obj sym_frame;
  ksi_obj sym_let;
  ksi_obj sym_letstar;
  ksi_obj sym_letrec;
  ksi_obj sym_or;
  ksi_obj sym_set;
  ksi_obj sym_quote;
  ksi_obj sym_quasiquote;
  ksi_obj sym_unquote;
  ksi_obj sym_unquote_splicing;
  ksi_obj sym_arrow;
  ksi_obj sym_dots;
  ksi_obj sym_syntax;
  ksi_obj sym_call;
  ksi_obj sym_locvar;
  ksi_obj sym_local;
  ksi_obj sym_global;
  ksi_obj sym_freevar;
  ksi_obj sym_function;
  ksi_obj sym_prefix;
  ksi_obj sym_postfix;
  ksi_obj sym_cons;
  ksi_obj sym_car;
  ksi_obj sym_cdr;
  ksi_obj sym_not;
  ksi_obj sym_eq_p;
  ksi_obj sym_eqv_p;
  ksi_obj sym_equal_p;
  ksi_obj sym_memq;
  ksi_obj sym_memv;
  ksi_obj sym_member;
  ksi_obj sym_null_p;
  ksi_obj sym_pair_p;
  ksi_obj sym_list_p;
  ksi_obj sym_list;
  ksi_obj sym_apply;
  ksi_obj sym_call_cc;
  ksi_obj sym_call_vs;
  ksi_obj sym_vector_p;
  ksi_obj sym_inactive;
  ksi_obj sym_wait;
  ksi_obj sym_ready;
  ksi_obj sym_timeout;
  ksi_obj sym_void;
  ksi_obj sym_true;
  ksi_obj sym_false;
  ksi_obj sym_loading;

  ksi_obj sym_apply_generic;
  ksi_obj sym_no_next_method;
  ksi_obj sym_no_applicable_method;
  ksi_obj sym_procedure;

  ksi_obj sym_cname;
  ksi_obj sym_dsupers;
  ksi_obj sym_dslots;
  ksi_obj sym_defargs;
  ksi_obj sym_cpl;
  ksi_obj sym_slots;
  ksi_obj sym_nfields;
  ksi_obj sym_gns;

  ksi_obj sym_gname;
  ksi_obj sym_methods;
  ksi_obj sym_arity;

  ksi_obj sym_gf;
  ksi_obj sym_specs;
  ksi_obj sym_proc;
  ksi_obj sym_combination;

  ksi_obj key_initform;
  ksi_obj key_initarg;
  ksi_obj key_defargs;
  ksi_obj key_type;
  ksi_obj key_name;
  ksi_obj key_dsupers;
  ksi_obj key_dslots;
  ksi_obj key_specs;
  ksi_obj key_proc;
  ksi_obj key_gf;
  ksi_obj key_arity;
  ksi_obj key_combination;
  ksi_obj key_after;
  ksi_obj key_before;
  ksi_obj key_around;
  ksi_obj key_primary;

  ksi_obj not_proc;
  ksi_obj eq_proc;
  ksi_obj eqv_proc;
  ksi_obj equal_proc;
  ksi_obj list_proc;
  ksi_obj nullp_proc;
  ksi_obj pairp_proc;
  ksi_obj listp_proc;
  ksi_obj cons_proc;
  ksi_obj car_proc;
  ksi_obj cdr_proc;
  ksi_obj memq_proc;
  ksi_obj memv_proc;
  ksi_obj member_proc;
  ksi_obj vectorp_proc;
  ksi_obj apply_proc;
  ksi_obj call_cc_proc; /* call-with-current-continuation */
  ksi_obj call_vs_proc; /* call-with-values */
  ksi_obj void_proc;
  ksi_obj true_proc;
  ksi_obj false_proc;

  ksi_obj gensym_num;

  ksi_obj Top, Object, Class;
  ksi_obj Generic, Method, Proc, Entity;
  ksi_obj Boolean, Char, String, Symbol;
  ksi_obj Number, Complex, Real, Integer;
  ksi_obj Vector, Pair, List, Null;
  ksi_obj Procedure, Keyword, Unknown;
  ksi_obj Record, Record_rtd;

  ksi_port null_port;

  KSI_DECLARE_LOCK(lock);
};


struct Ksi_Obj
{
  unsigned itag;
};


struct Ksi_Symbol
{
  unsigned itag;
  int len;
  const char ptr[1];
};

struct Ksi_String
{
  unsigned itag;
  int len;
  char *ptr;
};

struct Ksi_Pair
{
  unsigned itag;
  ksi_obj car;
  ksi_obj cdr;
};

struct Ksi_Vector
{
  unsigned itag;
  int dim;
  ksi_obj arr[1];
};

struct Ksi_Values
{
  unsigned itag;
  ksi_obj vals;
};

struct Ksi_Bignum
{
  unsigned itag;
  int sign;
  unsigned digs[1];
};

struct Ksi_Flonum
{
  unsigned itag;
  double real;
  double imag;
};

struct Ksi_ExtTag
{
  const char *type_name;

  int (*equal) (struct Ksi_ExtTag *, ksi_obj x1, ksi_obj x2, int deep);
  const char* (*print) (struct Ksi_ExtTag *, ksi_obj x, int slashify);
};

struct Ksi_ExtObj
{
  unsigned itag;
  ksi_ext_tag etag;
};

#define KSI_OBJ_TAG(x) (((ksi_obj)(x))->itag)
#define KSI_EXT_TAG(x) (((ksi_ext_obj)(x))->etag)


#define KSI_OBJ(c)	((ksi_obj) (c))
#define KSI_EXTOBJ(c)	((ksi_ext_obj) (c))
#define KSI_COD(c)	((KSI_INT_TYPE) (c))

#define KSI_PTR_P(c)	((KSI_COD((c)) & 1) == 0)
#if !defined(CAUTIOUS) || CAUTIOUS >= 2
# define KSI_OBJ_P(c)	(KSI_PTR_P((c)) && (c) != 0)
#else
# define KSI_OBJ_P(c)	(KSI_PTR_P((c)))
#endif

#define ksi_null	KSI_OBJ(0)

#define KSI_SINT_P(c)	((KSI_COD((c)) & 3) == KSI_TC2_INT)
#define KSI_MK_SINT(c)	(KSI_OBJ((KSI_COD(c) << 2) | KSI_TC2_INT))
#define KSI_SINT_VAL(c)	(KSI_COD(c) >> 2)
#define KSI_SINT_COD KSI_SINT_VAL

#define ksi_zero	KSI_MK_SINT(0)
#define ksi_one		KSI_MK_SINT(1)
#define ksi_two		KSI_MK_SINT(2)
#define ksi_three	KSI_MK_SINT(3)
#define ksi_four	KSI_MK_SINT(4)
#define ksi_five	KSI_MK_SINT(5)
#define ksi_six		KSI_MK_SINT(6)
#define ksi_seven	KSI_MK_SINT(7)
#define ksi_eight	KSI_MK_SINT(8)
#define ksi_nine	KSI_MK_SINT(9)
#define ksi_ten		KSI_MK_SINT(10)

#define KSI_IMM_P(c,t)	((KSI_COD((c)) & 0xff) == (t))
#define KSI_MK_IMM(c,t)	(KSI_OBJ((KSI_COD(c) << 8) | (t)))
#define KSI_IMM_VAL(c)	(KSI_COD((c)) >> 8)
#define KSI_IMM_TAG(c)	(KSI_COD((c)) & 0xff)


#define KSI_BOOL_P(x)	KSI_IMM_P((x), KSI_IMM_BOOL)
#define KSI_MK_BOOL(x)	KSI_MK_IMM((x), KSI_IMM_BOOL)
#define KSI_BOOL_VAL(x)	KSI_IMM_VAL((x))

#define ksi_false	KSI_MK_BOOL(0)
#define ksi_true	KSI_MK_BOOL(1)


#define KSI_CHAR_P(x)	KSI_IMM_P((x), KSI_IMM_CHAR)
#define KSI_MK_CHAR(x)	KSI_MK_IMM((x), KSI_IMM_CHAR)
#define KSI_CHAR_VAL(x)	KSI_IMM_VAL((x))
#define KSI_CHAR_COD KSI_CHAR_VAL

#define KSI_EXIT_P(c)	KSI_IMM_P((c), KSI_IMM_EXIT)
#define KSI_MK_EXIT(c)	KSI_MK_IMM((c), KSI_IMM_EXIT)
#define KSI_EXIT_VAL(c)	KSI_IMM_VAL((c))


#define ksi_void	KSI_MK_IMM(0, KSI_IMM_VOID)
#define ksi_unspec	ksi_void
#define ksi_nil		KSI_MK_IMM(0, KSI_IMM_NIL)
#define ksi_eof		KSI_MK_IMM(0, KSI_IMM_EOF)
#define ksi_err		KSI_MK_IMM(0, KSI_IMM_ERR)
#define ksi_any		KSI_MK_IMM(0, KSI_IMM_ANY)


#define KSI_OBJ_IS(c,t) (KSI_OBJ_P(c) && (KSI_OBJ(c)->itag == (t)))
#define KSI_EXT_IS(c,t) (KSI_OBJ_IS((c), KSI_TAG_EXTENDED) && KSI_EXTOBJ(c)->etag == (t))


#define KSI_SYM_P(s)	(KSI_OBJ_IS ((s), KSI_TAG_SYMBOL))
#define KSI_SYM_PTR(s)	(((ksi_symbol) (s)) -> ptr)
#define KSI_SYM_LEN(s)	(((ksi_symbol) (s)) -> len)


#define KSI_KEY_P(s)	(KSI_OBJ_IS ((s), KSI_TAG_KEYWORD))
#define KSI_KEY_PTR(s)	(((ksi_keyword) (s)) -> ptr)
#define KSI_KEY_LEN(s)	(((ksi_keyword) (s)) -> len)


#define KSI_BIGNUM_P(x)		(KSI_OBJ_IS((x), KSI_TAG_BIGNUM))
#define KSI_FLONUM_P(x)		(KSI_OBJ_IS((x), KSI_TAG_FLONUM))
#define KSI_COMPLEX_P(x)	(KSI_OBJ_IS((x), KSI_TAG_COMPLEX))

#define KSI_EINT_P(x)		(KSI_SINT_P(x) || KSI_BIGNUM_P(x))
#define KSI_REAL_P(x)		(KSI_EINT_P(x) || KSI_FLONUM_P(x))
#define KSI_EXACT_P(x)		(KSI_SINT_P(x) || KSI_BIGNUM_P(x))
#define KSI_INEXACT_P(x)	(KSI_FLONUM_P(x) || KSI_COMPLEX_P(x))
#define KSI_NUM_P(x)		(KSI_EXACT_P(x) || KSI_INEXACT_P(x))

#define KSI_REPART(x)		(((ksi_flonum) (x)) -> real)
#define KSI_IMPART(x)		(((ksi_flonum) (x)) -> imag)


#define KSI_C_STR_P(s)		(KSI_OBJ_IS ((s), KSI_TAG_CONST_STRING))
#define KSI_M_STR_P(s)		(KSI_OBJ_IS ((s), KSI_TAG_STRING))
#define KSI_STR_P(s)		(KSI_C_STR_P (s) || KSI_M_STR_P (s))
#define KSI_STR_PTR(o)		(((ksi_string) (o)) -> ptr)
#define KSI_STR_LEN(o)		(((ksi_string) (o)) -> len)


#define KSI_NULL_P(x)   ((x) == ksi_nil)
#define KSI_M_PAIR_P(s) KSI_OBJ_IS ((s), KSI_TAG_PAIR)
#define KSI_C_PAIR_P(s) KSI_OBJ_IS ((s), KSI_TAG_CONST_PAIR)
#define KSI_PAIR_P(x)   (KSI_M_PAIR_P (x) || KSI_C_PAIR_P (x))
#define KSI_PLIST_P(x)  (ksi_list_len ((x)) > 0)
#define KSI_LIST_P(x)   ((x) == ksi_nil || KSI_PLIST_P(x))
#define KSI_LIST_LEN(x) (ksi_list_len ((x)))


#define KSI_CAR(x) (((ksi_pair) (x)) -> car)
#define KSI_CDR(x) (((ksi_pair) (x)) -> cdr)


#define KSI_M_VEC_P(x)		KSI_OBJ_IS ((x), KSI_TAG_VECTOR)
#define KSI_C_VEC_P(x)		KSI_OBJ_IS ((x), KSI_TAG_CONST_VECTOR)
#define KSI_VEC_P(x)		(KSI_M_VEC_P (x) || KSI_C_VEC_P (x))
#define KSI_VEC_LEN(x)		(((ksi_vector) (x)) -> dim)
#define KSI_VEC_ARR(x)		(((ksi_vector) (x)) -> arr)
#define KSI_VEC_REF(x,n)	(((ksi_vector) (x)) -> arr [(n)])

#define KSI_VALUES_P(x)		(KSI_OBJ_IS ((x), KSI_TAG_VALUES))
#define KSI_VALUES_VALS(x)	(((ksi_values) (x)) -> vals)


#define KSI_LIST1(x) ksi_cons ((x), ksi_nil)

#define KSI_LIST2(x1,x2) ksi_cons ((x1), KSI_LIST1(x2))

#define KSI_LIST3(x1,x2,x3) ksi_cons ((x1), KSI_LIST2(x2,x3))

#define KSI_LIST4(x1,x2,x3,x4) ksi_cons ((x1), KSI_LIST3(x2,x3,x4))

#define KSI_LIST5(x1,x2,x3,x4,x5) ksi_cons ((x1), KSI_LIST4(x2,x3,x4,x5))

#define KSI_LIST6(x1,x2,x3,x4,x5,x6) ksi_cons ((x1), KSI_LIST5(x2,x3,x4,x5,x6))

#define KSI_LIST7(x1,x2,x3,x4,x5,x6,x7) ksi_cons ((x1), KSI_LIST6(x2,x3,x4,x5,x6,x7))

#define KSI_LIST8(x1,x2,x3,x4,x5,x6,x7,x8) ksi_cons ((x1), KSI_LIST7(x2,x3,x4,x5,x6,x7,x8))

#define KSI_LIST9(x1,x2,x3,x4,x5,x6,x7,x8,x9) ksi_cons ((x1), KSI_LIST8(x2,x3,x4,x5,x6,x7,x8,x9))


#define KSI_CONS(x1,x2) ksi_cons ((x1), (x2))

#define KSI_CONS3(x1,x2,x3) ksi_cons ((x1), KSI_CONS(x2,x3))

#define KSI_CONS4(x1,x2,x3,x4) ksi_cons ((x1), KSI_CONS3(x2,x3,x4))

#define KSI_CONS5(x1,x2,x3,x4,x5) ksi_cons ((x1), KSI_CONS4(x2,x3,x4,x5))

#define KSI_CONS6(x1,x2,x3,x4,x5,x6) ksi_cons ((x1), KSI_CONS5(x2,x3,x4,x5,x6))

#define KSI_CONS7(x1,x2,x3,x4,x5,x6,x7) ksi_cons ((x1), KSI_CONS6(x2,x3,x4,x5,x6,x7))

#define KSI_CONS8(x1,x2,x3,x4,x5,x6,x7,x8) ksi_cons ((x1), KSI_CONS7(x2,x3,x4,x5,x6,x7,x8))

#define KSI_CONS9(x1,x2,x3,x4,x5,x6,x7,x8,x9) ksi_cons ((x1), KSI_CONS8(x2,x3,x4,x5,x6,x7,x8,x9))


#define KSI_EQ_P(x,y)		((x) == (y))
#define KSI_EQV_P(x,y)		(ksi_eqv_p((x), (y)) != ksi_false)
#define KSI_EQUAL_P(x,y)	(ksi_equal_p((x), (y)) != ksi_false)

#define KSI_VOID_P(x)		((x) == ksi_void)
#define KSI_FALSE_P(x)		((x) == ksi_false)
#define KSI_TRUE_P(x)		((x) != ksi_false)


#ifdef __cplusplus
extern "C" {
#endif

SI_API
int
ksi_default_tag_equal (ksi_ext_tag, ksi_obj x1, ksi_obj x2, int deep);

SI_API
const char*
ksi_default_tag_print (ksi_ext_tag, ksi_obj x, int slashify);


SI_API
const char*
ksi_obj2str (ksi_obj x);

SI_API
const char*
ksi_obj2name (ksi_obj x);

SI_API
int
ksi_internal_format (ksi_port port, const char* fmt, int argc, ksi_obj* argv, char* nm);

SI_API
ksi_obj
ksi_format (ksi_obj port, const char* fmt, int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_abbrev (char* str, int len);


SI_API
ksi_obj
ksi_new_imm (int tag);

SI_API
ksi_obj
ksi_new_values (int num, ksi_obj* val);


/* boolean utils */

SI_API
ksi_obj
ksi_void_p (ksi_obj x);

SI_API
ksi_obj
ksi_int2bool (int x);

SI_API
int
ksi_bool2int (ksi_obj x);

SI_API
ksi_obj
ksi_bool_p (ksi_obj x);

SI_API
ksi_obj
ksi_not (ksi_obj x);

SI_API
ksi_obj
ksi_eq_p (ksi_obj x1, ksi_obj x2);

SI_API
ksi_obj
ksi_eqv_p (ksi_obj x1, ksi_obj x2);

SI_API
ksi_obj
ksi_equal_p (ksi_obj x1, ksi_obj x2);


/* Symbol utils */

SI_API
ksi_symbol
ksi_lookup_sym (const char* key, size_t len, int append);

#define ksi_intern(name,len)  ((ksi_obj) ksi_lookup_sym((name), (len), 1))
#define ksi_str2sym(name,len) ((ksi_obj) ksi_lookup_sym((name), (len), 1))
#define ksi_str02sym(name)    ((ksi_obj) ksi_lookup_sym((name), strlen(name), 1))
#define ksi_ptr2sym(name,len) ((ksi_obj) ksi_lookup_sym((name), (len), 1))
#define ksi_ptr02sym(name)    ((ksi_obj) ksi_lookup_sym((name), strlen(name), 1))

SI_API
const char*
ksi_symbol2str (ksi_symbol sym);

SI_API
ksi_obj
ksi_symbol_p (ksi_obj x);

SI_API
ksi_obj
ksi_symbol2string (ksi_obj sym);

SI_API
ksi_obj
ksi_string2symbol (ksi_obj str);

SI_API
ksi_obj
ksi_gensym (ksi_obj name, ksi_obj unused);


/* Keyword utils */

SI_API
ksi_keyword
ksi_lookup_key (const char* key, size_t len, int append);

#define ksi_str2key(name,len) ((ksi_obj) ksi_lookup_key((name), (len), 1))
#define ksi_str02key(name)    ((ksi_obj) ksi_lookup_key((name), strlen(name), 1))
#define ksi_ptr2key(name,len) ((ksi_obj) ksi_lookup_key((name), (len), 1))
#define ksi_ptr02key(name)    ((ksi_obj) ksi_lookup_key((name), strlen(name), 1))

SI_API
const char*
ksi_key2str (ksi_keyword key);

SI_API
ksi_obj
ksi_key_p (ksi_obj x);

SI_API
ksi_obj
ksi_string2keyword (ksi_obj str);

SI_API
ksi_obj
ksi_keyword2string (ksi_obj key);

SI_API
ksi_obj
ksi_symbol2keyword (ksi_obj sym);

SI_API
ksi_obj
ksi_keyword2symbol (ksi_obj key);

SI_API
ksi_obj
ksi_make_keyword (ksi_obj x);


/* Char utils */

SI_API
ksi_obj
ksi_str2char (const char* name, int len);

SI_API
const char*
ksi_char2str (ksi_obj x);

SI_API
ksi_obj
ksi_char_p (ksi_obj x);

SI_API
ksi_obj
ksi_char_eq_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_lt_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_gt_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_le_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_ge_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_ci_eq_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_ci_lt_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_ci_gt_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_ci_le_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_ci_ge_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_char_alpha_p (ksi_obj x);

SI_API
ksi_obj
ksi_char_digit_p (ksi_obj x);

SI_API
ksi_obj
ksi_char_space_p (ksi_obj x);

SI_API
ksi_obj
ksi_char_upper_p (ksi_obj x);

SI_API
ksi_obj
ksi_char_lower_p (ksi_obj x);

SI_API
ksi_obj
ksi_char_upcase (ksi_obj x);

SI_API
ksi_obj
ksi_char_downcase (ksi_obj x);

SI_API
ksi_obj
ksi_char2integer (ksi_obj x);

SI_API
ksi_obj
ksi_integer2char (ksi_obj x);


/* String utils */

SI_API
ksi_obj
ksi_new_string (int argc, ksi_obj* argv);

SI_API
const char*
ksi_string2str (ksi_obj x);

SI_API
ksi_obj
ksi_str2string (const char* str, int len);

SI_API
ksi_obj
ksi_str02string (const char* str);

SI_API
ksi_obj
ksi_ptr02string (const char* str);

SI_API
ksi_obj
ksi_make_string (int len, char fill_char);

SI_API
ksi_obj
ksi_string_p (ksi_obj x);

SI_API
ksi_obj
ksi_list2string (ksi_obj list);

SI_API
ksi_obj
ksi_string_length (ksi_obj str);

SI_API
ksi_obj
ksi_string_ref (ksi_obj str, ksi_obj k);

SI_API
ksi_obj
ksi_string_set_x (ksi_obj str, ksi_obj k, ksi_obj c);

SI_API
ksi_obj
ksi_substring (ksi_obj str, ksi_obj start, ksi_obj end);

SI_API
ksi_obj
ksi_string_append (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_string_copy (ksi_obj str);

SI_API
ksi_obj
ksi_string_fill_x (ksi_obj str, ksi_obj fill_char);

SI_API
ksi_obj
ksi_string_eqv_p (ksi_obj s1, ksi_obj s2);

SI_API
ksi_obj
ksi_string_equal_p (ksi_obj s1, ksi_obj s2);

SI_API
ksi_obj
ksi_string_ci_equal_p (ksi_obj s1, ksi_obj s2);

SI_API
ksi_obj
ksi_string_eq_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_ci_eq_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_ls_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_gt_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_le_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_ge_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_ci_ls_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_ci_gt_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_ci_le_p (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_string_ci_ge_p (int argc, ksi_obj* argv);


/* Pair & List utilites */

SI_API
ksi_obj
ksi_new_list (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_cons (ksi_obj car, ksi_obj cdr);

SI_API
ksi_obj
ksi_acons (ksi_obj car, ksi_obj cdr, ksi_obj alist);

SI_API
int
ksi_list_len (ksi_obj x);

SI_API
ksi_obj
ksi_pair_p (ksi_obj x);

SI_API
ksi_obj
ksi_null_p (ksi_obj x);

SI_API
ksi_obj
ksi_list_p (ksi_obj x);

SI_API
ksi_obj
ksi_car (ksi_obj);

SI_API
ksi_obj
ksi_cdr (ksi_obj);

SI_API
ksi_obj
ksi_set_car_x (ksi_obj pair, ksi_obj val);

SI_API
ksi_obj
ksi_set_cdr_x (ksi_obj pair, ksi_obj val);

SI_API
ksi_obj
ksi_length (ksi_obj list);

SI_API
ksi_obj
ksi_make_list (ksi_obj len, ksi_obj init);

SI_API
ksi_obj
ksi_append (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_append_x (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_reverse (ksi_obj list);

SI_API
ksi_obj
ksi_reverse_x (ksi_obj arg);

SI_API
ksi_obj
ksi_list_head (ksi_obj list, ksi_obj len);

SI_API
ksi_obj
ksi_list_tail (ksi_obj list, ksi_obj len);

SI_API
ksi_obj
ksi_list_ref (ksi_obj list, ksi_obj num);

SI_API
ksi_obj
ksi_list_set_x (ksi_obj lst, ksi_obj num, ksi_obj val);

SI_API
ksi_obj
ksi_last_pair (ksi_obj x);

SI_API
ksi_obj
ksi_copy_list (ksi_obj o);

SI_API
ksi_obj
ksi_copy_tree (ksi_obj o);

SI_API
ksi_obj
ksi_memq (ksi_obj o, ksi_obj list);

SI_API
ksi_obj
ksi_memv (ksi_obj o, ksi_obj list);

SI_API
ksi_obj
ksi_member (ksi_obj o, ksi_obj list, ksi_obj proc);

SI_API
ksi_obj
ksi_memq_remove (ksi_obj x, ksi_obj lst);

SI_API
ksi_obj
ksi_memv_remove (ksi_obj x, ksi_obj lst);

SI_API
ksi_obj
ksi_member_remove (ksi_obj x, ksi_obj lst, ksi_obj proc);

SI_API
ksi_obj
ksi_memq_remove_x (ksi_obj x, ksi_obj lst);

SI_API
ksi_obj
ksi_memv_remove_x (ksi_obj x, ksi_obj lst);

SI_API
ksi_obj
ksi_member_remove_x (ksi_obj x, ksi_obj lst, ksi_obj proc);

SI_API
ksi_obj
ksi_assq (ksi_obj o, ksi_obj list);

SI_API
ksi_obj
ksi_assv (ksi_obj o, ksi_obj list);

SI_API
ksi_obj
ksi_assoc (ksi_obj o, ksi_obj list, ksi_obj proc);

SI_API
ksi_obj
ksi_assq_ref (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assv_ref (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assoc_ref (ksi_obj alist, ksi_obj key, ksi_obj proc);

SI_API
ksi_obj
ksi_assq_set_x (ksi_obj alist, ksi_obj key, ksi_obj val);

SI_API
ksi_obj
ksi_assv_set_x (ksi_obj alist, ksi_obj key, ksi_obj val);

SI_API
ksi_obj
ksi_assoc_set_x (ksi_obj alist, ksi_obj key, ksi_obj val, ksi_obj proc);

SI_API
ksi_obj
ksi_assq_remove_x (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assv_remove_x (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assoc_remove_x (ksi_obj alist, ksi_obj key, ksi_obj proc);

SI_API
ksi_obj
ksi_map (ksi_obj proc, int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_for_each (ksi_obj proc, int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_fold_left (ksi_obj kons, ksi_obj knil, int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_fold_right (ksi_obj kons, ksi_obj knil, int argc, ksi_obj* argv);


/* Vector utils */

SI_API
ksi_vector
ksi_alloc_vector (int dim, int tag);

SI_API
ksi_obj
ksi_new_vector (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_vector_p (ksi_obj x);

SI_API
ksi_obj
ksi_copy_vector (ksi_obj x);

SI_API
ksi_obj
ksi_list2vector (ksi_obj x);

SI_API
ksi_obj
ksi_vector2list (ksi_obj v);

SI_API
ksi_obj
ksi_make_vector (ksi_obj k, ksi_obj fill);

SI_API
ksi_obj
ksi_vector_length (ksi_obj v);

SI_API
ksi_obj
ksi_vector_ref (ksi_obj vec, ksi_obj k);

SI_API
ksi_obj
ksi_vector_set_x (ksi_obj vec, ksi_obj k, ksi_obj val);

SI_API
ksi_obj
ksi_vector_fill_x (ksi_obj vec, ksi_obj fill);

SI_API
ksi_obj
ksi_vector_map (ksi_obj proc, ksi_obj vec);

SI_API
ksi_obj
ksi_vector_for_each (ksi_obj proc, ksi_obj vec);


/* Number utils */

SI_API
ksi_obj
ksi_str02num (const char* str, int radix);

SI_API
ksi_obj
ksi_int2num (long v);

SI_API
ksi_obj
ksi_uint2num (unsigned long v);

#ifdef LONG_LONG
SI_API
ksi_obj
ksi_long2num (LONG_LONG v);

SI_API
ksi_obj
ksi_ulong2num (unsigned LONG_LONG v);

#else
# define ksi_long2num  ksi_int2num
# define ksi_ulong2num ksi_uint2num
#endif

SI_API
ksi_obj
ksi_double2num (double v);

SI_API
ksi_obj
ksi_make_bignum (int sign_and_size, unsigned *digits);

SI_API
ksi_obj
ksi_rectangular (double r, double i);

SI_API
ksi_obj
ksi_polar (double x, double a);

SI_API
ksi_obj
ksi_double2exact (double d);

SI_API
char*
ksi_num2str (ksi_obj x, int radix);

SI_API
double
ksi_num2double (ksi_obj x);

SI_API
long
ksi_num2int (ksi_obj x);

SI_API
unsigned long
ksi_num2uint (ksi_obj x);

#ifdef LONG_LONG
SI_API
LONG_LONG
ksi_num2long (ksi_obj x);

SI_API
unsigned LONG_LONG
ksi_num2ulong (ksi_obj x);

#else
# define ksi_num2long  ksi_num2int
# define ksi_num2ulong ksi_num2uint
#endif

SI_API
double
ksi_real_part (ksi_obj x);

SI_API
double
ksi_imag_part (ksi_obj x);

SI_API
double
ksi_magnitude (ksi_obj x);

SI_API
double
ksi_angle (ksi_obj z);

SI_API
ksi_obj
ksi_number_p (ksi_obj x);

SI_API
ksi_obj
ksi_complex_p (ksi_obj x);

SI_API
ksi_obj
ksi_real_p (ksi_obj x);

SI_API
ksi_obj
ksi_rational_p (ksi_obj x);

SI_API
ksi_obj
ksi_integer_p (ksi_obj x);

SI_API
ksi_obj
ksi_exact_p (ksi_obj x);

SI_API
ksi_obj
ksi_inexact_p (ksi_obj x);

SI_API
ksi_obj
ksi_number2string (ksi_obj num, ksi_obj rad);

SI_API
ksi_obj
ksi_string2number (ksi_obj str, ksi_obj rad);

SI_API
ksi_obj
ksi_exact2inexact (ksi_obj x);

SI_API
ksi_obj
ksi_inexact2exact (ksi_obj x);

SI_API
ksi_obj
ksi_abs (ksi_obj x);

SI_API
ksi_obj
ksi_min (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_max (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_add (ksi_obj a, ksi_obj b);

SI_API
ksi_obj
ksi_sub (ksi_obj a, ksi_obj b);

SI_API
ksi_obj
ksi_mul (ksi_obj a, ksi_obj b);

SI_API
ksi_obj
ksi_div (ksi_obj a, ksi_obj b);

SI_API
ksi_obj
ksi_quot (ksi_obj a, ksi_obj b);

SI_API
ksi_obj
ksi_rem (ksi_obj a, ksi_obj b);

SI_API
ksi_obj
ksi_mod (ksi_obj a, ksi_obj b);

SI_API
ksi_obj
ksi_gcd (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_lcm (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_floor (ksi_obj x);

SI_API
ksi_obj
ksi_ceiling (ksi_obj x);

SI_API
ksi_obj
ksi_truncate (ksi_obj x);

SI_API
ksi_obj
ksi_round (ksi_obj x);

SI_API
ksi_obj
ksi_sqrt (ksi_obj x);

SI_API
ksi_obj
ksi_expt (ksi_obj z1, ksi_obj z2);

SI_API
ksi_obj
ksi_exp (ksi_obj x);

SI_API
ksi_obj
ksi_log (ksi_obj x);

SI_API
ksi_obj
ksi_sin (ksi_obj x);

SI_API
ksi_obj
ksi_cos (ksi_obj x);

SI_API
ksi_obj
ksi_tan (ksi_obj x);

SI_API
ksi_obj
ksi_asin (ksi_obj x);

SI_API
ksi_obj
ksi_acos (ksi_obj x);

SI_API
ksi_obj
ksi_atan (ksi_obj y, ksi_obj x);

SI_API
ksi_obj
ksi_sinh (ksi_obj x);

SI_API
ksi_obj
ksi_cosh (ksi_obj x);

SI_API
ksi_obj
ksi_tanh (ksi_obj x);

SI_API
ksi_obj
ksi_asinh (ksi_obj x);

SI_API
ksi_obj
ksi_acosh (ksi_obj x);

SI_API
ksi_obj
ksi_atanh (ksi_obj y);

SI_API
ksi_obj
ksi_zero_p (ksi_obj x);

SI_API
ksi_obj
ksi_positive_p (ksi_obj x);

SI_API
ksi_obj
ksi_negative_p (ksi_obj x);

SI_API
ksi_obj
ksi_odd_p (ksi_obj x);

SI_API
ksi_obj
ksi_even_p (ksi_obj x);

SI_API
ksi_obj
ksi_num_eqv_p (ksi_obj x1, ksi_obj x2);

#define ksi_num_equal_p(x1, x2) ksi_num_eqv_p (x1, x2)

SI_API
ksi_obj
ksi_num_eq_p (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_num_lt_p (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_num_le_p (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_num_gt_p (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_num_ge_p (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_lognot (ksi_obj x);

SI_API
ksi_obj
ksi_logior (ksi_obj x, ksi_obj y);

SI_API
ksi_obj
ksi_logxor (ksi_obj x, ksi_obj y);

SI_API
ksi_obj
ksi_logand (ksi_obj x, ksi_obj y);

SI_API
ksi_obj
ksi_logtest (ksi_obj x, ksi_obj y);

SI_API
ksi_obj
ksi_logbit_p (ksi_obj ind, ksi_obj x);

SI_API
ksi_obj
ksi_ash (ksi_obj x, ksi_obj cnt);


SI_API
ksi_obj
ksi_object2string (ksi_obj x);

SI_API
ksi_obj
ksi_read (ksi_obj port);

SI_API
ksi_obj
ksi_read_const (ksi_obj port);

SI_API
ksi_obj
ksi_str02obj (const char* str);

SI_API
ksi_obj
ksi_str2obj (const char* str, int len);


/* internal and not public declarations */

SI_API
struct Ksi_Data *
ksi_get_internal_data(void);

#define ksi_data ksi_get_internal_data()

const char*
ksi_mk_filename (ksi_obj x, char* name);


#ifdef __cplusplus
}
#endif

#endif

 /* End of file */
