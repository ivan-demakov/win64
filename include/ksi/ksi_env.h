/* -*-mode:C++-*- */
/*
 * ksi_env.h
 *
 * Copyright (C) 2009-2010, 2015, ivan demakov.
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
 * Last Update:   Tue Jun  2 16:40:21 2015
 *
 */

/**
 * @file   ksi_env.h
 * @author ivan demakov <ksion@users.sourceforge.net>
 * @date   Sat Dec 26 22:02:48 2009
 *
 * @brief Environment utils.
 *
 */

#ifndef KSI_ENV_H
#define KSI_ENV_H

#include "ksi_type.h"

struct Ksi_EnvRec
{
    ksi_obj sym;
    ksi_obj val;
    unsigned imported : 1;
    unsigned exported : 1;
    unsigned syntax : 1;
    unsigned macro : 1;
};

struct Ksi_Environ
{
    struct Ksi_ObjData o;

    ksi_valtab_t valtab;
    ksi_env parent;
    ksi_obj name;
    ksi_obj exported;
};

/** Check environment
 *
 * @param x object
 *
 * @return true if \a x is environment, false otherwise
 */
#define KSI_ENV_P(x)		(KSI_OBJ_IS ((x), KSI_TAG_ENVIRON))

#define KSI_ENV_NAME(x)		(((ksi_env) (x)) -> name)
#define KSI_ENV_EXPORT(x)	(((ksi_env) (x)) -> exported)


#ifdef __cplusplus
extern "C" {
#endif

/** Check environment
 *
 *
 * @param x object
 *
 * @return ksi_true if \a x is environment, ksi_false otherwise
 */
SI_API
ksi_obj
ksi_env_p (ksi_obj x);


/** Create new empty environment.
 * The function creates environment without any bindings (including core bindings, such as \a if, \a lambda, etc).
 * Should be used only if you know what you do.
 *
 * @param size initial size of the hash table in the environment
 * @param parent parent environment
 *
 * @return new environment
 */
SI_API
ksi_env
ksi_new_env (int size, ksi_env parent);


/** Create new top level environment.
 * The function creates new environmnent that can be used as a top level environmnet.
 *
 * @return new top level environment
 */
SI_API
ksi_env
ksi_top_level_env (void);


/** Get a library environment.
 * The function looks up for the library \a name and if found returns the library environment.
 * Otherwise if \a create_new is false, the fuction return 0,
 * or if \a create_new if true, the new empty environment returned,
 * and all successive lookups for the library return this environment.
 *
 * @param name library name
 * @param created_new create new environment if the library is not found
 *
 * @return library environment
 */
SI_API
ksi_env
ksi_lib_env (ksi_obj name, int create_new);


/** Get library environment.
 * The function looks for the library \code (name ...) \endcode and if found returns the library environment.
 * If library is not found, new environment is created and all successive lookups for the library return this environment.
 * See also ksi_lib_env().
 *
 * @param name part of the library name.  Last argument should be \a 0.
 *
 * @return library environment
 */
SI_API
ksi_env
ksi_get_lib_env (const wchar_t *name, ...);


/** Search a binding.
 *
 * @param env environment
 * @param sym variable
 *
 * @return the binding of \a sym in \a env, or 0 if \a sym is not bound
 */
SI_API
ksi_envrec
ksi_lookup_env (ksi_env env, ksi_obj sym);

/** Append a binding.
 *
 * @param env environment
 * @param sym variable name
 * @param val variable value
 *
 * @return the binding of \a sym in \a env
 */
SI_API
ksi_envrec
ksi_append_env (ksi_env env, ksi_obj sym, ksi_obj val);


/** Define new variable.
 * The function create new (global) variable \a sym and bound it to \a val in \a env.
 * Before defining the variable the function checks that the variable can be defined in \a env
 * and raise an exception if cannot.
 * The variable cannot be defined if it already defined in \a env, and exported or imported
 *
 * @param sym variable name
 * @param val varaible value
 * @param env environment
 *
 * @return unspesified
 */
SI_API
ksi_obj
ksi_define (ksi_obj sym, ksi_obj val, ksi_env env);

/** Define new local variable.
 * The function create new (global) variable \a sym and bound it to \a val in \a env.
 * Before defining the variable the function checks that the variable can be defined in \a env
 * and raise an exception if cannot.
 * The local variable cannot be defined if it already defined in \a env and exported.
 *
 * @param sym variable name
 * @param val varaible value
 * @param env environment
 *
 * @return unspesified
 */
SI_API
ksi_obj
ksi_define_local (ksi_obj sym, ksi_obj val, ksi_env env);

/** The function checks that \a sym is bound in \a env.
 * The \a sym is bound if it is localy defined in \a env, or imported in.
 * @param sym variable name
 * @param env environment
 *
 * @return ksi_true if \a sym is bound in \a env, ksi_false otherwise
 */
SI_API
ksi_obj
ksi_bound_p (ksi_obj sym, ksi_env env);

/** The function checks that \a sym is a variable in \a env.
 * The \a sym is variable if it is localy defined in \a env and not imported or exported.
 * @param sym variable name
 * @param env environment
 *
 * @return ksi_true if \a sym is bound in \a env, ksi_false otherwise
 */
SI_API
ksi_obj
ksi_var_p (ksi_env env, ksi_obj sym);

/** The function returns the value bound to \a sym in \a env.
 * If \a sym is not bound the function raise an exception.
 * @param sym variable name
 * @param env environment
 *
 * @return Value bound to \a sym in \a env
 */
SI_API
ksi_obj
ksi_var_ref (ksi_env env, ksi_obj sym);

/** The function set new value to \a sym in \a env.
 * If \a sym is not local variable the function raise an exception.
 * @param sym variable name
 * @param env environment
 *
 * @return Value bound to \a sym in \a env
 */
SI_API
ksi_obj
ksi_var_set (ksi_env env, ksi_obj sym, ksi_obj val);

/** The function gets the value bound to \a sym in \a env, or \a def if \a sym is not bound.
 * @param sym variable name
 * @param env environment
 * @param def default value
 *
 * @return Value bound to \a sym in \a env, or \a def if \a sym is not bound
 */
SI_API
ksi_obj
ksi_var_value (ksi_env env, ksi_obj sym, ksi_obj def);



/** Export a variable or syntax.
 *
 *
 * @param env environment
 * @param insym internal name
 * @param exsym exteranal name (if 0, used \a insym)
 *
 * @return unspecified
 */
SI_API
ksi_obj
ksi_export (ksi_env env, ksi_obj insym, ksi_obj exsym);

/** Define new variable.
 * The function creates new (global) variable \a name in \a env and bound it to \a val.
 * (see also ksi_define()).
 *
 * @param name nul-terminatting string with name of the variable
 * @param val value of the variable
 * @param env environment in whith \a name is bound to \a val
 *
 * @return unspecified
 */
SI_API
ksi_obj
ksi_defsym (const wchar_t *name, ksi_obj val, ksi_env env);

/** Define syntax.
 * The function creates new syntactic keyword \a sym and bound it to \a val in environment \a env.
 * \a val can be one of:
 *   - Special value used internaly in the ksi for the core syntax.
 *   - Procedure that take 2 arguments (syntactic form and environment)
 *     and return transformed form that can be evaluated later (hopely).
 *     This can be used to define low-level macros.
 *   - Symbol used for auxiliary syntactic keywords (like symbol \a else in \a cond form).
 *   - Any other value that ignored by the KSi and \a sym is defined as a syntactic keyword without meaneang,
 *
 * @param sym syntactic keyword (a symbol)
 * @param val value
 * @param env environment in which the \a sym bound to \a val
 * @param export if not zero, export \a sym after definening
 *
 * @return unspecified
 */
SI_API
ksi_obj
ksi_defsyntax (ksi_obj sym, ksi_obj val, ksi_env env, int export_p);

SI_API
ksi_obj
ksi_exported_p (ksi_env env, ksi_obj sym, ksi_obj external);


/** Import the variable or syntax.
 *
 *
 * @param libenv library environment
 * @param libsym library exported name of the variable or syntax
 * @param env environment into which the variable or syntax is imported
 * @param sym the name of the variable or syntax that used in \a env
 *
 * @return unspecified
 */
SI_API
ksi_obj
ksi_import (ksi_env libenv, ksi_obj libsym, ksi_env env, ksi_obj sym);


/** Enumerate all bindings.
 * Call \a proc for each binding in \a env.
 *
 * @param proc procedure
 * @param env environment
 *
 * @return unspecified
 */
SI_API
ksi_obj
ksi_env_for_each (ksi_obj proc, ksi_obj env);


#ifdef __cplusplus
}
#endif


#endif

/* End of file */
