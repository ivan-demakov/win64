/*
 * ksi_type.h
 * public type, constant and macro
 *
 * Copyright (C) 1997-2011, 2014, 2015, ivan demakov
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
 * Author:        ivan demakov <ksion@users.sourceforge.net>
 * Creation date: Mon Dec 15 12:03:22 1997
 * Last Update:   Fri Apr 25 22:52:49 2014
 *
 */

#ifndef KSI_TYPE_H
#define KSI_TYPE_H

#include "ksi_defs.h"
#include "ksi_vtab.h"


enum ksi_tag_t
{
    KSI_TAG_FRAME,

    KSI_TAG_FIRST_CONST,
    KSI_TAG_IMM = KSI_TAG_FIRST_CONST,
    KSI_TAG_BIGNUM,
    KSI_TAG_FLONUM,
    KSI_TAG_SYMBOL,
    KSI_TAG_KEYWORD,
    KSI_TAG_PAIR,
    KSI_TAG_CONST_PAIR,
    KSI_TAG_VECTOR,
    KSI_TAG_CONST_VECTOR,
    KSI_TAG_STRING,
    KSI_TAG_CONST_STRING,
    KSI_TAG_CHAR,
    KSI_TAG_BYTEVECTOR,
    KSI_TAG_CONST_BYTEVECTOR,
    KSI_TAG_LAST_CONST,

    KSI_TAG_VAR0,
    KSI_TAG_VAR1,
    KSI_TAG_VAR2,
    KSI_TAG_VARN,

    KSI_TAG_FREEVAR,
    KSI_TAG_IMPORTED,
    KSI_TAG_LOCAL,

    KSI_TAG_FIRST_CODE,
    KSI_TAG_QUOTE = KSI_TAG_FIRST_CODE,
    KSI_TAG_BEGIN,
    KSI_TAG_AND,
    KSI_TAG_OR,
    KSI_TAG_IF,
    KSI_TAG_LAMBDA,
    KSI_TAG_DEFINE,
    KSI_TAG_DEFSYNTAX,
    KSI_TAG_SET,
    KSI_TAG_LET,
    KSI_TAG_LETREC,
    KSI_TAG_LETREC_STAR,
    KSI_TAG_CALL,
    KSI_TAG_IMPORT,
    KSI_TAG_SYNTAX,
    KSI_TAG_LAST_CODE,

    KSI_TAG_FIRST_OP,
    KSI_TAG_ADD = KSI_TAG_FIRST_OP,
    KSI_TAG_SUB,
    KSI_TAG_MUL,
    KSI_TAG_DIV,
    KSI_TAG_APPLY,
    KSI_TAG_CLOSE,
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
    KSI_TAG_APPEND,
    KSI_TAG_MK_VECTOR,
    KSI_TAG_LIST2VECTOR,
    KSI_TAG_NULLP,
    KSI_TAG_PAIRP,
    KSI_TAG_LISTP,
    KSI_TAG_VECTORP,
    KSI_TAG_LAST_OP,

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
    KSI_TAG_LAST_PROC,

    KSI_TAG_CORE,
    KSI_TAG_VALUES,
    KSI_TAG_ENVIRON,
    KSI_TAG_EXN,
    KSI_TAG_PORT,
    KSI_TAG_HASHTAB,
    KSI_TAG_EXIT,

    KSI_TAG_EXTENDED,
    KSI_TAG_BROKEN
};



struct Ksi_Data
{
    ksi_obj obj_nil, obj_false, obj_true, obj_void, obj_eof, obj_err;

    ksi_valtab_t symtab;
    ksi_valtab_t keytab;
    ksi_valtab_t envtab;

    ksi_obj sym_quote;
    ksi_obj sym_begin;
    ksi_obj sym_if;
    ksi_obj sym_and;
    ksi_obj sym_or;
    ksi_obj sym_lambda;
    ksi_obj sym_define;
    ksi_obj sym_defmacro;
    ksi_obj sym_set;
    ksi_obj sym_case;
    ksi_obj sym_cond;
    ksi_obj sym_else;
    ksi_obj sym_let;
    ksi_obj sym_letstar;
    ksi_obj sym_letrec;
    ksi_obj sym_letrec_star;
    ksi_obj sym_quasiquote;
    ksi_obj sym_unquote;
    ksi_obj sym_unquote_splicing;
    ksi_obj sym_syntax;
    ksi_obj sym_quasisyntax;
    ksi_obj sym_unsyntax;
    ksi_obj sym_unsyntax_splicing;
    ksi_obj sym_import;
    ksi_obj sym_export;
    ksi_obj sym_library;
    ksi_obj sym_rename;
    ksi_obj sym_prefix;
    ksi_obj sym_except;
    ksi_obj sym_only;
    ksi_obj sym_for;

    ksi_obj sym_plus;
    ksi_obj sym_minus;
    ksi_obj sym_arrow;
    ksi_obj sym_dots;
    ksi_obj sym_inactive;
    ksi_obj sym_wait;
    ksi_obj sym_ready;
    ksi_obj sym_timeout;

    ksi_obj sym_apply_generic;
    ksi_obj sym_no_next_method;
    ksi_obj sym_no_applicable_method;

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
    ksi_obj sym_after;
    ksi_obj sym_before;
    ksi_obj sym_around;
    ksi_obj sym_primary;

    ksi_obj sym_native;
    ksi_obj sym_latin1;
    ksi_obj sym_utf8;
    ksi_obj sym_utf16;
    ksi_obj sym_raise;
    ksi_obj sym_ignore;
    ksi_obj sym_replace;
    ksi_obj sym_none;
    ksi_obj sym_lf;
    ksi_obj sym_cr;
    ksi_obj sym_crlf;
    ksi_obj sym_nel;
    ksi_obj sym_crnel;
    ksi_obj sym_ls;

    ksi_obj sym_no_create;
    ksi_obj sym_no_fail;
    ksi_obj sym_no_truncate;
    ksi_obj sym_line;
    ksi_obj sym_block;

    ksi_obj sym_little;
    ksi_obj sym_big;

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

    ksi_obj not_proc;
    ksi_obj eq_proc;
    ksi_obj eqv_proc;
    ksi_obj equal_proc;
    ksi_obj list_proc;
    ksi_obj vector_proc;
    ksi_obj list2vector_proc;
    ksi_obj append_proc;
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

    wchar_t *session_id;
    ksi_obj gensym_num;

    ksi_obj Top, Object, Class;
    ksi_obj Generic, Method, Proc, Entity;
    ksi_obj Boolean, Char, String, Symbol;
    ksi_obj Number, Complex, Real, Rational, Integer;
    ksi_obj Vector, Pair, List, Null;
    ksi_obj Procedure, Keyword, Unknown;
    ksi_obj Record, Rtd;

    ksi_char_port null_port;
    ksi_char_port input_port;
    ksi_char_port output_port;
    ksi_char_port error_port;

    ksi_env syntax_env;

    struct Ksi_Dynl_Lib *dynl_libs;

    KSI_DECLARE_LOCK(lock);
};

struct Ksi_Dynl_Lib
{
    struct Ksi_Dynl_Lib *next;
    char *name;
    void *handle;
    int count;
};


struct Ksi_Context
{
    KSI_WORD *stack;
    ksi_wind wind;
    ksi_wind exit_catch;
    ksi_obj jump_val;

    ksi_char_port input_port;
    ksi_char_port output_port;
    ksi_char_port error_port;

    int errlog_priority;
    ksi_char_port errlog_port;
    void (*errlog_proc) (ksi_context_t ctx, int pri, const wchar_t *msg);

    ksi_byte_port (*loader_proc)(const char *filename, void *data);
    void *loader_data;
};


struct Ksi_ObjData
{
    unsigned itag;
    const wchar_t *annotation;
};


struct Ksi_Obj
{
    struct Ksi_ObjData o;
};

struct Ksi_Core
{
    struct Ksi_ObjData o;
    enum ksi_tag_t core;
};

struct Ksi_Symbol
{
    struct Ksi_ObjData o;
    int len;
    wchar_t ptr[1];
};

struct Ksi_Char
{
    struct Ksi_ObjData o;
    wchar_t code;
};

struct Ksi_String
{
    struct Ksi_ObjData o;
    int len;
    wchar_t *ptr;
};

struct Ksi_Bytevector
{
    struct Ksi_ObjData o;
    int len;
    char *ptr;
};

struct Ksi_Pair
{
    struct Ksi_ObjData o;
    ksi_obj car;
    ksi_obj cdr;
};

struct Ksi_Vector
{
    struct Ksi_ObjData o;
    int dim;
    ksi_obj arr[1];
};

struct Ksi_Values
{
    struct Ksi_ObjData o;
    ksi_obj vals;
};

struct Ksi_ETag
{
    const wchar_t *type_name;

    int (*equal) (struct Ksi_EObj *x1, struct Ksi_EObj *x2, int deep);
    const wchar_t* (*print) (struct Ksi_EObj *x, int slashify);
    ksi_obj (*apply) (struct Ksi_EObj *x, int argc, ksi_obj *argv);
};

struct Ksi_EObj
{
    struct Ksi_ObjData o;
    ksi_etag etag;
};


#define KSI_ASSERT(t)         ((void)((t)||ksi_exn_error(0, 0, "assert failed: %s [%s %d]", #t, __FILE__, __LINE__)))
#define KSI_CHECK(a,c,s)      ((void)((c)||ksi_exn_error(0, (a), (s))))
#define KSI_SYNTAX(a,c,s)     ((void)((c)||ksi_exn_error(L"syntax", (a), (s))))
#define KSI_WNA(c,p,s)        ((void)((c)||ksi_exn_error(0, (p), "%ls: missing or extra args", (s))))


#define ksi_nil         (ksi_data->obj_nil)
#define ksi_false       (ksi_data->obj_false)
#define ksi_true        (ksi_data->obj_true)
#define ksi_void        (ksi_data->obj_void)
#define ksi_unspec      ksi_void
#define ksi_eof         (ksi_data->obj_eof)
#define ksi_err         (ksi_data->obj_err)


/* deprecated values */
#define ksi_zero  ksi_long2num(0)
#define ksi_one   ksi_long2num(1)
#define ksi_two   ksi_long2num(2)


#define KSI_OBJ_IS(c,t) ((c) && (c)->o.itag == (t))
#define KSI_EXT_IS(c,t) (KSI_OBJ_IS((c), KSI_TAG_EXTENDED) && ((ksi_eobj) (c))->etag == (t))
#define KSI_EXT_TAG(c)  (((ksi_eobj) (c))->etag)


#define KSI_EXN_P(x)            (KSI_OBJ_IS ((x), KSI_TAG_EXN))
#define KSI_EXN_TYPE(x)         KSI_VEC_REF ((x), 0)
#define KSI_EXN_MSG(x)          KSI_VEC_REF ((x), 1)
#define KSI_EXN_VAL(x)          KSI_VEC_REF ((x), 2)
#define KSI_EXN_SRC(x)          KSI_VEC_REF ((x), 3)


#define KSI_SYM_P(s)	(KSI_OBJ_IS ((s), KSI_TAG_SYMBOL))
#define KSI_SYM_PTR(s)	(((ksi_symbol) (s)) -> ptr)
#define KSI_SYM_LEN(s)	(((ksi_symbol) (s)) -> len)


#define KSI_KEY_P(s)	(KSI_OBJ_IS((s), KSI_TAG_KEYWORD))
#define KSI_KEY_PTR(s)	(((ksi_keyword) (s)) -> ptr)
#define KSI_KEY_LEN(s)	(((ksi_keyword) (s)) -> len)


#define KSI_BIGNUM_P(x)		(KSI_OBJ_IS((x), KSI_TAG_BIGNUM))
#define KSI_FLONUM_P(x)		(KSI_OBJ_IS((x), KSI_TAG_FLONUM))
#define KSI_NUM_P(x)		(KSI_BIGNUM_P(x) || KSI_FLONUM_P(x))

#define KSI_UINT_P(x)		(KSI_TRUE_P(ksi_unsigned_integer_p((x))))
#define KSI_EINT_P(x)		(KSI_TRUE_P(ksi_exact_integer_p((x))))
#define KSI_RATIONAL_P(x)	(KSI_TRUE_P(ksi_rational_p((x))))
#define KSI_REAL_P(x)	(KSI_TRUE_P(ksi_real_p((x))))

#define KSI_CHAR_P(x)		KSI_OBJ_IS((x), KSI_TAG_CHAR)
#define KSI_CHAR_CODE(x)	(((ksi_char) (x)) -> code)

#define KSI_C_STR_P(s)		(KSI_OBJ_IS((s), KSI_TAG_CONST_STRING))
#define KSI_M_STR_P(s)		(KSI_OBJ_IS((s), KSI_TAG_STRING))
#define KSI_STR_P(s)		(KSI_C_STR_P(s) || KSI_M_STR_P (s))
#define KSI_STR_LEN(o)		(((ksi_string) (o)) -> len)
#define KSI_STR_PTR(o)		(((ksi_string) (o)) -> ptr)

#define KSI_M_BVEC_P(x)		KSI_OBJ_IS ((x), KSI_TAG_BYTEVECTOR)
#define KSI_C_BVEC_P(x)		KSI_OBJ_IS ((x), KSI_TAG_CONST_BYTEVECTOR)
#define KSI_BVEC_P(x)		(KSI_M_BVEC_P(x) || KSI_C_BVEC_P(x))
#define KSI_BVEC_LEN(x)		(((ksi_bytevector)(x)) -> len)
#define KSI_BVEC_PTR(x)		(((ksi_bytevector)(x)) -> ptr)

#define KSI_NULL_P(x)   ((x) == ksi_nil)
#define KSI_M_PAIR_P(s) KSI_OBJ_IS((s), KSI_TAG_PAIR)
#define KSI_C_PAIR_P(s) KSI_OBJ_IS((s), KSI_TAG_CONST_PAIR)
#define KSI_PAIR_P(x)   (KSI_M_PAIR_P(x) || KSI_C_PAIR_P(x))
#define KSI_PLIST_P(x)  (ksi_list_len((x)) > 0)
#define KSI_LIST_P(x)   ((x) == ksi_nil || KSI_PLIST_P(x))
#define KSI_LIST_LEN(x) ksi_list_len((x))

#define KSI_CAR(x) (((ksi_pair) (x)) -> car)
#define KSI_CDR(x) (((ksi_pair) (x)) -> cdr)

#define KSI_M_VEC_P(x)		KSI_OBJ_IS ((x), KSI_TAG_VECTOR)
#define KSI_C_VEC_P(x)		KSI_OBJ_IS ((x), KSI_TAG_CONST_VECTOR)
#define KSI_VEC_P(x)		(KSI_M_VEC_P (x) || KSI_C_VEC_P (x))
#define KSI_VEC_LEN(x)		(((ksi_vector) (x)) -> dim)
#define KSI_VEC_ARR(x)		(((ksi_vector) (x)) -> arr)
#define KSI_VEC_REF(x,n)	(((ksi_vector) (x)) -> arr[(n)])

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


#define KSI_CONS2(x1,x2) ksi_cons ((x1), (x2))

#define KSI_CONS3(x1,x2,x3) ksi_cons ((x1), KSI_CONS2(x2,x3))

#define KSI_CONS4(x1,x2,x3,x4) ksi_cons ((x1), KSI_CONS3(x2,x3,x4))

#define KSI_CONS5(x1,x2,x3,x4,x5) ksi_cons ((x1), KSI_CONS4(x2,x3,x4,x5))

#define KSI_CONS6(x1,x2,x3,x4,x5,x6) ksi_cons ((x1), KSI_CONS5(x2,x3,x4,x5,x6))

#define KSI_CONS7(x1,x2,x3,x4,x5,x6,x7) ksi_cons ((x1), KSI_CONS6(x2,x3,x4,x5,x6,x7))

#define KSI_CONS8(x1,x2,x3,x4,x5,x6,x7,x8) ksi_cons ((x1), KSI_CONS7(x2,x3,x4,x5,x6,x7,x8))

#define KSI_CONS9(x1,x2,x3,x4,x5,x6,x7,x8,x9) ksi_cons ((x1), KSI_CONS8(x2,x3,x4,x5,x6,x7,x8,x9))


#define KSI_EQV_P(x,y)		(ksi_eqv_p((x), (y)) != ksi_false)
#define KSI_EQUAL_P(x,y)	(ksi_equal_p((x), (y)) != ksi_false)

#define KSI_VOID_P(x)		((x) == ksi_void)
#define KSI_FALSE_P(x)		((x) == ksi_false)
#define KSI_TRUE_P(x)		((x) != ksi_false)



#ifdef __cplusplus
extern "C" {
#endif

/* Initialization & termination. */

SI_API
void
ksi_init_std_ports (int in_fd, int out_fd, int err_fd);

SI_API
void
ksi_init_context (ksi_context_t ctx, void *top_stack);

SI_API
ksi_context_t
ksi_set_current_context (ksi_context_t ctx);

SI_API
void
ksi_term (void);

SI_API
void
ksi_load_boot_file (const char *filename, ksi_env env);

SI_API
const char*
ksi_mk_filename (ksi_obj x, char* name);


/* standart ports */

SI_API
ksi_obj
ksi_current_input_port (void);

SI_API
ksi_obj
ksi_current_output_port (void);

SI_API
ksi_obj
ksi_current_error_port (void);

SI_API
ksi_obj
ksi_set_current_output_port (ksi_obj port);

SI_API
ksi_obj
ksi_set_current_input_port (ksi_obj port);

SI_API
ksi_obj
ksi_set_current_error_port (ksi_obj port);


/* error handling */

SI_API
ksi_obj
ksi_open_errlog (ksi_obj port_or_fname);

SI_API
ksi_obj
ksi_errlog_priority (ksi_obj priority);

SI_API
ksi_obj
ksi_exn_p (ksi_obj x);

SI_API
ksi_obj
ksi_exn_type (ksi_obj x);

SI_API
ksi_obj
ksi_exn_message (ksi_obj x);

SI_API
ksi_obj
ksi_exn_value (ksi_obj x);

SI_API
ksi_obj
ksi_exn_source (ksi_obj x);

SI_API
ksi_obj
ksi_make_exn (const wchar_t *type, ksi_obj errobj, const wchar_t *msg, const wchar_t *src);

SI_API
ksi_obj
ksi_exn_error (const wchar_t *type, ksi_obj errobj, const char *fmt, ...) __KSI_PRINTF(3,4);

SI_API
ksi_obj
ksi_src_error (const wchar_t *src, const char *fmt, ...) __KSI_PRINTF(2,3);

SI_API
ksi_obj
ksi_handle_error (ksi_context_t ctx, ksi_obj tag, ksi_obj exn);


/* dump & undump ksi objects */

SI_API
void*
ksi_dump (ksi_obj x, int* size);

SI_API
ksi_obj
ksi_undump (void* data, int size);

SI_API
ksi_obj
ksi_obj2dump (ksi_obj x);

SI_API
ksi_obj
ksi_dump2obj (ksi_obj x);

SI_API
ksi_obj
ksi_read_dump (ksi_obj port);

SI_API
ksi_obj
ksi_write_dump (ksi_obj x, ksi_obj port);


/* Dynamic loading */

SI_API
ksi_obj
ksi_dynamic_link (ksi_obj mod, ksi_obj sym);

SI_API
ksi_obj
ksi_dynamic_call (ksi_obj func, ksi_obj arg_list);

SI_API
ksi_obj
ksi_dynamic_unlink (ksi_obj func);



SI_API
int
ksi_default_tag_equal (struct Ksi_EObj *x1, struct Ksi_EObj *x2, int deep);

SI_API
const wchar_t *
ksi_default_tag_print (struct Ksi_EObj *x, int slashify);


/** Print the object.
 * The returned string can be used to read \a x back, so that
 * \code ksi_str2obj( ksi_obj2str( x ) ) \endcode is equal \a x.
 *
 * @param x object to print
 *
 * @return nul-terminated string with external representation of \a x.
 */
SI_API
const wchar_t *
ksi_obj2str (ksi_obj x);

/** Print the object
 * The returned string can be used to read \a x back.
 *
 * @param x object to print
 *
 * @return string object with external representation of \a x.
 */
SI_API
ksi_obj
ksi_object2string (ksi_obj x);

/** Print the object.
 * The returned string can not be used to read \a x back,
 * but only to show \a x to the user.
 *
 * @param x object to print
 *
 * @return nul-terminated string with simplified representation of \a x.
 */
SI_API
const wchar_t*
ksi_obj2name (ksi_obj x);


/** Read object
 *
 * @param str nul-terminated string with external representaion of the object
 *
 * @return the object
 */
SI_API
ksi_obj
ksi_str02obj (const wchar_t *str);

/** Read object
 *
 * @param str string with external representaion of the object
 * @param len length of the string
 *
 * @return the object
 */
SI_API
ksi_obj
ksi_str2obj (const wchar_t *str, int len);


SI_API
ksi_obj
ksi_setlocale (ksi_obj category, ksi_obj locale);


/** Symbol abbreviations.
 * Find all symbols that have names started from \a str.
 * The length of \a str is \a len.
 *
 * @param str pointer to a string
 * @param len length of \a str
 *
 * @return List of all interned symbols that starts from \a str.
 */
SI_API
ksi_obj
ksi_abbrev (wchar_t *str, int len);

SI_API
ksi_obj
ksi_new_imm (enum ksi_tag_t tag);

SI_API
ksi_obj
ksi_new_core (enum ksi_tag_t tag);

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
ksi_bool_eq_p (int argc, ksi_obj *argv);

SI_API
ksi_obj
ksi_not (ksi_obj x);

SI_API
ksi_obj
ksi_eqv_p (ksi_obj x1, ksi_obj x2);

SI_API
ksi_obj
ksi_equal_p (ksi_obj x1, ksi_obj x2);


/* Symbol utils */

SI_API
ksi_symbol
ksi_lookup_sym (const wchar_t* key, size_t len, int append);

#define ksi_intern(name,len)  ((ksi_obj) ksi_lookup_sym((name), (len), 1))
#define ksi_str2sym(name,len) ((ksi_obj) ksi_lookup_sym((name), (len), 1))

SI_API
ksi_obj
ksi_str02sym (const wchar_t* key);

SI_API
const wchar_t*
ksi_symbol2str (ksi_symbol sym);

SI_API
ksi_obj
ksi_symbol_p (ksi_obj x);

SI_API
ksi_obj
ksi_symbol_eq_p (int argc, ksi_obj* argv);

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
ksi_lookup_key (const wchar_t *key, size_t len, int append);

#define ksi_str2key(name,len) ((ksi_obj) ksi_lookup_key((name), (len), 1))

SI_API
ksi_obj
ksi_str02key (const wchar_t *key);

SI_API
const wchar_t*
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
ksi_int2char (wint_t code);

SI_API
ksi_obj
ksi_str2char (const wchar_t *name, int len);

SI_API
const wchar_t *
ksi_char2str (ksi_char x);

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

/** Create string object
 *
 * New string object is created with length equal to \a len.
 * If \a str is NULL, the content is unspecified,
 * otherwise the content is copied from \a str.
 * In any case the ending null char appended at the end of the string.
 *
 * @param str pointer to data
 * @param len length of data
 *
 * @return new string object
 */
SI_API
ksi_obj
ksi_str2string (const wchar_t *str, int len);

/** Create string object
 *
 * New string object is created from C null terminating wide string.
 *
 * @param str null terminating string
 *
 * @return new string object
 */
SI_API
ksi_obj
ksi_str02string (const wchar_t *str);

SI_API
ksi_obj
ksi_new_string (int argc, ksi_obj *argv);

SI_API
const wchar_t*
ksi_string2str (ksi_obj x);

SI_API
ksi_obj
ksi_make_string (int len, wchar_t fill_char);

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

SI_API
ksi_obj
ksi_string_index (ksi_obj str, ksi_obj chr, ksi_obj beg);

SI_API
ksi_obj
ksi_string_rindex (ksi_obj str, ksi_obj chr, ksi_obj beg);

SI_API
ksi_obj
ksi_string2list (ksi_obj str);

SI_API
ksi_obj
ksi_string_upcase_x (ksi_obj s);

SI_API
ksi_obj
ksi_string_downcase_x (ksi_obj s);

SI_API
ksi_obj
ksi_string_capitalize_x (ksi_obj str);

SI_API
ksi_obj
ksi_string_for_each (ksi_obj proc, ksi_obj str, int ac, ksi_obj *av);


/* bytevector utils */

SI_API
ksi_obj
ksi_native_endianness (void);

/** Create bytevector object
 *
 * New bytevector object is created with length equal to \a len.
 * If \a bytes is NULL, the content unspecified,
 * otherwise the content is copied from \a bytes.
 *
 * @param bytes pointer to data
 * @param len length of data
 *
 * @return new bytevector object
 */
SI_API
ksi_obj
ksi_str2bytevector (const char *bytes, int len);

SI_API
ksi_obj
ksi_bytevector_p (ksi_obj x);

SI_API
ksi_obj
ksi_make_bytevector (ksi_obj k, ksi_obj fill);

SI_API
ksi_obj
ksi_bytevector_length (ksi_obj v);

SI_API
ksi_obj
ksi_bytevector_eqv_p (ksi_obj x1, ksi_obj x2);

SI_API
ksi_obj
ksi_bytevector_fill_x (ksi_obj v, ksi_obj fill);

SI_API
ksi_obj
ksi_bytevector_copy_x (ksi_obj src, ksi_obj src_start, ksi_obj dst, ksi_obj dst_start, ksi_obj num);

SI_API
ksi_obj
ksi_bytevector_copy (ksi_obj v);

SI_API
ksi_obj
ksi_bytevector_u8_ref (ksi_obj v, ksi_obj k);

SI_API
ksi_obj
ksi_bytevector_u8_set_x (ksi_obj v, ksi_obj k, ksi_obj val);

SI_API
ksi_obj
ksi_bytevector_s8_ref (ksi_obj v, ksi_obj k);

SI_API
ksi_obj
ksi_bytevector_s8_set_x (ksi_obj v, ksi_obj k, ksi_obj val);

SI_API
ksi_obj
ksi_bytevector_float_ref (ksi_obj v, ksi_obj k, ksi_obj endian);

SI_API
ksi_obj
ksi_bytevector_float_set_x (ksi_obj v, ksi_obj k, ksi_obj x, ksi_obj endian);

SI_API
ksi_obj
ksi_bytevector_double_ref (ksi_obj v, ksi_obj k, ksi_obj endian);

SI_API
ksi_obj
ksi_bytevector_double_set_x (ksi_obj v, ksi_obj k, ksi_obj x, ksi_obj endian);


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
ksi_member (ksi_obj o, ksi_obj list);

SI_API
ksi_obj
ksi_memp (ksi_obj proc, ksi_obj list);

SI_API
ksi_obj
ksi_remq (ksi_obj x, ksi_obj lst);

SI_API
ksi_obj
ksi_remv (ksi_obj x, ksi_obj lst);

SI_API
ksi_obj
ksi_remove (ksi_obj x, ksi_obj lst);

SI_API
ksi_obj
ksi_remp (ksi_obj proc, ksi_obj lst);

SI_API
ksi_obj
ksi_assq (ksi_obj o, ksi_obj list);

SI_API
ksi_obj
ksi_assv (ksi_obj o, ksi_obj list);

SI_API
ksi_obj
ksi_assoc (ksi_obj o, ksi_obj list);

SI_API
ksi_obj
ksi_assp (ksi_obj proc, ksi_obj list);

SI_API
ksi_obj
ksi_assq_ref (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assv_ref (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assoc_ref (ksi_obj alist, ksi_obj key);

ksi_obj
ksi_assp_ref (ksi_obj list, ksi_obj proc);

SI_API
ksi_obj
ksi_assq_set_x (ksi_obj alist, ksi_obj key, ksi_obj val);

SI_API
ksi_obj
ksi_assv_set_x (ksi_obj alist, ksi_obj key, ksi_obj val);

SI_API
ksi_obj
ksi_assoc_set_x (ksi_obj alist, ksi_obj key, ksi_obj val);

SI_API
ksi_obj
ksi_assp_set_x (ksi_obj alist, ksi_obj proc, ksi_obj val);

SI_API
ksi_obj
ksi_assq_rem_x (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assv_rem_x (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assoc_rem_x (ksi_obj alist, ksi_obj key);

SI_API
ksi_obj
ksi_assp_rem_x (ksi_obj alist, ksi_obj proc);

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

SI_API
ksi_obj
ksi_xcons (ksi_obj cdr, ksi_obj car);

SI_API
ksi_obj
ksi_cons_a (int argc, ksi_obj* argv);


/* Vector utils */

SI_API
ksi_obj
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
ksi_vector_map (ksi_obj proc, ksi_obj vec, int ac, ksi_obj *av);

SI_API
ksi_obj
ksi_vector_for_each (ksi_obj proc, ksi_obj vec, int ac, ksi_obj *av);


/* Number utils */

SI_API
ksi_obj
ksi_str02num (const wchar_t *str, int radix);

SI_API
ksi_obj
ksi_int2num(int x);

SI_API
ksi_obj
ksi_uint2num(int x);

SI_API
ksi_obj
ksi_long2num (long v);

SI_API
ksi_obj
ksi_ulong2num (unsigned long v);

SI_API
ksi_obj
ksi_longlong2num (LONG_LONG v);

SI_API
ksi_obj
ksi_ulonglong2num (unsigned LONG_LONG v);

SI_API
ksi_obj
ksi_rectangular (double r, double i);

#define ksi_double2num(x) ksi_rectangular((x), 0.0)

SI_API
ksi_obj
ksi_polar (double x, double a);

SI_API
ksi_obj
ksi_double2exact (double d, const char *func_name);

SI_API
wchar_t*
ksi_num2str (ksi_obj x, int radix);

SI_API
int
ksi_int_p(ksi_obj x);

SI_API
int
ksi_long_p(ksi_obj x);

SI_API
int
ksi_ulong_p(ksi_obj x);

SI_API
int
ksi_longlong_p(ksi_obj x);

SI_API
int
ksi_ulonglong_p(ksi_obj x);

SI_API
int
ksi_num2int (ksi_obj x, const char *func_name);

SI_API
unsigned int
ksi_num2uint (ksi_obj x, const char *func_name);

SI_API
long
ksi_num2long (ksi_obj x, const char *func_name);

SI_API
unsigned long
ksi_num2ulong (ksi_obj x, const char *func_name);

SI_API
LONG_LONG
ksi_num2longlong (ksi_obj x, const char *func_name);

SI_API
unsigned LONG_LONG
ksi_num2ulonglong (ksi_obj x, const char *func_name);

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
ksi_obj
ksi_numerator(ksi_obj x);

SI_API
ksi_obj
ksi_denominator(ksi_obj x);

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
ksi_finite_p (ksi_obj x);

SI_API
ksi_obj
ksi_infinite_p (ksi_obj x);

SI_API
ksi_obj
ksi_nan_p (ksi_obj x);

SI_API
ksi_obj
ksi_exact_integer_p (ksi_obj x);

SI_API
ksi_obj
ksi_unsigned_integer_p (ksi_obj x);

SI_API
ksi_obj
ksi_number2string (ksi_obj num, ksi_obj rad);

SI_API
ksi_obj
ksi_string2number (ksi_obj str, ksi_obj rad);

SI_API
ksi_obj
ksi_inexact (ksi_obj x);

SI_API
ksi_obj
ksi_exact (ksi_obj x);

SI_API
ksi_obj
ksi_abs (ksi_obj x);

SI_API
ksi_obj
ksi_min (int argc, ksi_obj* args);

SI_API
ksi_obj
ksi_max (int argc, ksi_obj* args);

/** Add.
 *
 * @param a number
 * @param b numbar
 *
 * @return \f$ a + b \f$
 */
SI_API
ksi_obj
ksi_add (ksi_obj a, ksi_obj b);

/** Subtruct.
 *
 * @param a number
 * @param b number
 *
 * @return \f$ a - b \f$
 */
SI_API
ksi_obj
ksi_sub (ksi_obj a, ksi_obj b);

/** Multiply.
 *
 * @param a number
 * @param b number
 *
 * @return \f$ a * b \f$
 */
SI_API
ksi_obj
ksi_mul (ksi_obj a, ksi_obj b);

/** Divide.
 *
 * @param a number
 * @param b number
 *
 * @return \f$ a / b \f$
 */
SI_API
ksi_obj
ksi_div (ksi_obj a, ksi_obj b);

/** Exact integer devide.
 * The function produce correct result only when it is known in advance that \a d divides \a n.
 * The routine are much faster than the other division functions,
 * and are the best choice when exact division is known to occur.
 * If \a n or \a d is not exact integer, exception raised.
 * If \a n is not divided by \a d, unspecified number returned.
 *
 * @param n exact integer
 * @param d exact integer
 *
 * @return \f$ n / d \f$
 */
SI_API
ksi_obj
ksi_exact_div (ksi_obj n, ksi_obj d);

/** Integer divide.
 *
 * @param n real number
 * @param d nozero real number
 *
 * @return \f$ n div d \f$
 */
SI_API
ksi_obj
ksi_idiv (ksi_obj n, ksi_obj d);

/** Integer divide.
 *
 * @param n real number
 * @param d nozero real number
 *
 * @return \f$ n mod d \f$
 */
SI_API
ksi_obj
ksi_imod (ksi_obj n, ksi_obj d);

/** Integer divide.
 *
 * @param n real number
 * @param d nozero real number
 *
 * @return \f$ n div d \f$ and \f$ n mod d \f$
 */
SI_API
ksi_obj
ksi_idiv_and_mod (ksi_obj n, ksi_obj d);

/** Integer divide.
 *
 * @param n real number
 * @param d nozero real number
 *
 * @return \f$ n div d \f$ and \f$ n mod d \f$
 */
SI_API
ksi_obj
ksi_idiv_and_mod_who (ksi_obj n, ksi_obj d, ksi_obj who);

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
ksi_exact_sqrt (ksi_obj x);

SI_API
ksi_obj
ksi_expt (ksi_obj z1, ksi_obj z2);

SI_API
ksi_obj
ksi_exp (ksi_obj x);

SI_API
ksi_obj
ksi_log (ksi_obj x, ksi_obj base);

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
ksi_plus (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_minus (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_multiply (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_divide (int argc, ksi_obj* argv);

SI_API
ksi_obj
ksi_make_rectangular (ksi_obj x, ksi_obj y);

SI_API
ksi_obj
ksi_make_polar (ksi_obj x, ksi_obj y);


/* various utils */

ksi_obj
ksi_getenv (ksi_obj str);

SI_API
ksi_obj
ksi_syscall (ksi_obj str);

SI_API
ksi_obj
ksi_getcwd (void);

SI_API
ksi_obj
ksi_chdir (ksi_obj dir);

SI_API
ksi_obj
ksi_mkdir (ksi_obj dir, ksi_obj msk);

SI_API
ksi_obj
ksi_rmdir (ksi_obj dir);

SI_API
ksi_obj
ksi_file_exists (ksi_obj fn);

SI_API
ksi_obj
ksi_delete_file (ksi_obj fn);

SI_API
ksi_obj
ksi_rename_file (ksi_obj oldfn, ksi_obj newfn);

SI_API
ksi_obj
ksi_stat (ksi_obj x);

SI_API
ksi_obj
ksi_opendir (ksi_obj name);

SI_API
ksi_obj
ksi_readdir (ksi_obj dir);

SI_API
ksi_obj
ksi_closedir (ksi_obj dir);

SI_API
ksi_obj
ksi_exp_fname (ksi_obj x, ksi_obj dir);

SI_API
ksi_obj
ksi_split_fname (ksi_obj x);

SI_API
ksi_obj
ksi_split_path (ksi_obj x);

SI_API
ksi_obj
ksi_has_suffix_p (ksi_obj name, ksi_obj suff);


/* internal (not public) declarations */

void
ksi_init_klos (void);

void
ksi_init_dynl (void);

void
ksi_term_dynl (void);

const wchar_t *
ksi_dynload_file (char *fname);


SI_API
struct Ksi_Data *
ksi_internal_data(void);

#define ksi_data ksi_internal_data()


extern ksi_context_t ksi_current_context;


#ifdef __cplusplus
}
#endif

#endif

 /* End of file */
