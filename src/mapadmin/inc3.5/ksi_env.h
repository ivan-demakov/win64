/* -*-mode:C++-*- */
/*
 * ksi_env.h
 *
 * Copyright (C) 2009, ivan demakov.
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
 * Author:        ivan demakov <ksion@users.sourceforge.net>
 * Creation date: Thu Feb 19 14:25:10 2009
 *
 * $Id: ksi_env.h,v 1.1.2.3.2.1 2009/07/05 15:50:25 ksion Exp $
 *
 */

#ifndef KSI_ENV_H
#define KSI_ENV_H

#include "ksi_type.h"

typedef struct Ksi_Module* ksi_module;
typedef struct Ksi_EnvRec *ksi_envrec;
typedef struct Ksi_Environ *ksi_env;


struct Ksi_Module
{
    unsigned itag;
    ksi_obj env;
    ksi_obj name;
    ksi_obj export;
    ksi_obj uses;
    ksi_obj opts;
};

#define KSI_MOD_P(x)		KSI_OBJ_IS ((x), KSI_TAG_MODULE)
#define KSI_MOD_ENV(x)		(((ksi_module) (x)) -> env)
#define KSI_MOD_NAME(x)		(((ksi_module) (x)) -> name)
#define KSI_MOD_EXPORT(x)	(((ksi_module) (x)) -> export)
#define KSI_MOD_USES(x)		(((ksi_module) (x)) -> uses)
#define KSI_MOD_OPTS(x)		(((ksi_module) (x)) -> opts)


struct Ksi_EnvRec
{
  ksi_obj sym;
  ksi_obj val;
};

struct Ksi_Environ
{
    unsigned itag;
    ksi_valtab_t valtab;
    ksi_obj module;
};

#define KSI_ENV_P(x)	(KSI_OBJ_IS ((x), KSI_TAG_ENVIRON))
#define KSI_ENV_MOD(x)	(((ksi_env) (x)) -> module)


#ifdef __cplusplus
extern "C" {
#endif

SI_API
ksi_obj
ksi_module_p (ksi_obj x);

SI_API
ksi_obj
ksi_make_module (ksi_obj name, ksi_obj export, ksi_obj opts, ksi_obj env);

SI_API
ksi_obj
ksi_find_module (ksi_obj name);

SI_API
ksi_obj
ksi_clear_module (ksi_obj module);

SI_API
ksi_obj
ksi_current_module (void);

SI_API
ksi_obj
ksi_set_current_module (ksi_obj mod);

SI_API
ksi_obj
ksi_get_module_opt (ksi_obj mod, ksi_obj key);

SI_API
ksi_obj
ksi_set_module_opt (ksi_obj mod, ksi_obj key, ksi_obj val);

SI_API
ksi_env
ksi_new_env (int size);

SI_API
ksi_obj*
ksi_lookup_env (ksi_env env, ksi_obj sym);

SI_API
ksi_obj*
ksi_append_env (ksi_env env, ksi_obj sym, ksi_obj val);

SI_API
ksi_obj
ksi_defsym (const char* name, ksi_obj val, ksi_obj env);

SI_API
ksi_obj
ksi_spec_val (const char* name, ksi_obj def, ksi_obj env);

SI_API
ksi_obj
ksi_glob_val (ksi_obj sym, ksi_obj def);

SI_API
ksi_obj
ksi_env_p (ksi_obj x);

SI_API
ksi_obj
ksi_global_env (void);

SI_API
ksi_obj
ksi_current_env (void);

SI_API
ksi_obj
ksi_make_env (ksi_obj src_env);

SI_API
ksi_obj
ksi_define (ksi_obj sym, ksi_obj val, ksi_obj env);

SI_API
ksi_obj
ksi_defined_p (ksi_obj sym, ksi_obj env);

SI_API
ksi_obj
ksi_var_p (ksi_obj env, ksi_obj sym);

SI_API
ksi_obj
ksi_var_ref (ksi_obj env, ksi_obj sym);

SI_API
ksi_obj
ksi_var_set (ksi_obj env, ksi_obj sym, ksi_obj val);

#ifdef __cplusplus
}
#endif


#endif

/* End of file */
