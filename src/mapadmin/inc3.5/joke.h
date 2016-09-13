/* -*-mode:C++-*- */
/*
 * joke.h
 * joke library main include file
 *
 * Copyright (C) 1997-2000, 2009, Ivan Demakov
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without a written agreement
 * is hereby granted, provided that the above copyright notice and this
 * paragraph and the following two paragraphs appear in all copies.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 *
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
 * DOCUMENTATION, EVEN IF THE AUTHORS HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAS NO OBLIGATIONS
 * TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 *
 * Author:        Ivan Demakov <demakov@users.sourceforge.net>
 * Creation date: Sat Jul  5 15:45:47 1997
 * Last Update:   Tue Mar 21 19:25:14 2000
 *
 *
 * $Id: joke.h,v 1.14 2000/04/07 03:19:03 ivan Exp $
 *
 */

#ifndef JOKE_H
#define JOKE_H

#ifndef KSI_H
#  include "ksi.h"
#endif

#if defined(_MSC_VER)
#  if defined(MAKE_joke_LIB)
#    define JOKE_API __declspec(dllexport)
#  else
#    define JOKE_API __declspec(dllimport)
#  endif
#endif

#ifndef JOKE_API
#  define JOKE_API extern
#endif

#if defined(unix)
#  define X11_GRAPH 1
#elif defined(MSWIN32)
#  define WIN_GRAPH 1
#else
#  error Unsupported graphics
#endif


struct Ksi_DC;
struct Ksi_Font;

typedef struct Ksi_DC		*ksi_dc;
typedef struct Ksi_Font		*ksi_font;


#if defined(X11_GRAPH)

#include <X11/Xlib.h>

typedef unsigned long		ksi_pixel; /* pixel value */
typedef unsigned short		ksi_color; /* red, green, blue value */

#define MAX_COLOR		0xffff

#elif defined(WIN_GRAPH)

#include <windows.h>

typedef COLORREF		ksi_pixel; /* pixel value */
typedef unsigned short		ksi_color; /* red, green, blue value */

#define MAX_COLOR		0xffff

#endif


#ifdef __cplusplus
extern "C" {
#endif


JOKE_API
const char*
ksi_safe_string_ptr (ksi_obj x);

JOKE_API
ksi_obj
ksi_get_arg (ksi_obj key, ksi_obj args, ksi_obj def);


/* initialization */

#if defined(X11_GRAPH)

JOKE_API
int
ksi_joke_x11_init (Display* dpy, int argc, char** argv);

#elif defined(WIN_GRAPH)

JOKE_API
int
ksi_joke_win_init (HINSTANCE hInstance, PSTR szCmdLine);

#endif

JOKE_API
void
ksi_joke_term (void);


/* DC utils */

#if defined(X11_GRAPH)

JOKE_API
ksi_obj
ksi_make_x11_dc (Window wnd, Drawable drawable, GC gc);

#elif defined(WIN_GRAPH)

JOKE_API
ksi_obj
ksi_make_win_dc (HWND hwnd, HDC hdc);

#endif

JOKE_API
ksi_obj
ksi_dc_p (ksi_obj x);

JOKE_API
ksi_obj
ksi_free_dc (ksi_obj dc);

JOKE_API
ksi_obj
ksi_sync_dc (ksi_obj dc, ksi_obj discard);

JOKE_API
ksi_obj
ksi_clear_window (ksi_obj dc, ksi_obj expose);

JOKE_API
ksi_obj
ksi_clear_area (ksi_obj dc, ksi_obj x, ksi_obj y,
		ksi_obj width, ksi_obj height, ksi_obj expose);

JOKE_API
ksi_obj
ksi_set_foreground (ksi_obj dc, ksi_obj color);

JOKE_API
ksi_obj
ksi_set_background (ksi_obj dc, ksi_obj color);

JOKE_API
ksi_obj
ksi_set_line_style (ksi_obj dc, ksi_obj width, ksi_obj style);

JOKE_API
ksi_obj
ksi_set_cap_style (ksi_obj dc, ksi_obj style);

JOKE_API
ksi_obj
ksi_set_join_style (ksi_obj dc, ksi_obj style);

JOKE_API
ksi_obj
ksi_set_dashes (ksi_obj dc, ksi_obj dash_list);

JOKE_API
ksi_obj
ksi_set_fill_style (ksi_obj dc, ksi_obj style);

JOKE_API
ksi_obj
ksi_set_fill_rule (ksi_obj dc, ksi_obj style);

JOKE_API
ksi_obj
ksi_set_arc_mode (ksi_obj dc, ksi_obj style);

JOKE_API
ksi_obj
ksi_set_font (ksi_obj dc, ksi_obj font, ksi_obj size, ksi_obj angle);


/* colors */

JOKE_API
ksi_obj
ksi_alloc_color (ksi_obj dc, ksi_obj color);

JOKE_API
ksi_obj
ksi_free_color (ksi_obj dc, ksi_obj pixval);


/* fonts */

JOKE_API
ksi_obj
ksi_make_font (ksi_obj family, ksi_obj weigth, ksi_obj style, ksi_obj charset);


/* draw */

JOKE_API
ksi_obj
ksi_draw_point (ksi_obj dc, ksi_obj x, ksi_obj y);

JOKE_API
ksi_obj
ksi_draw_points (ksi_obj dc, ksi_obj pnt, ksi_obj coord_mode);

JOKE_API
ksi_obj
ksi_draw_line (ksi_obj dc, ksi_obj coord1, ksi_obj coord2);

JOKE_API
ksi_obj
ksi_draw_lines (ksi_obj dc, ksi_obj pnt, ksi_obj coord_mode, ksi_obj close);

JOKE_API
ksi_obj
ksi_draw_rect (ksi_obj dc, ksi_obj c1, ksi_obj c2);

JOKE_API
ksi_obj
ksi_draw_ellipse (ksi_obj dc, ksi_obj c1, ksi_obj c2);

JOKE_API
ksi_obj
ksi_draw_arc (ksi_obj dc, ksi_obj c1, ksi_obj c2, ksi_obj a1, ksi_obj a2);

JOKE_API
ksi_obj
ksi_fill_rect (ksi_obj dc, ksi_obj c1, ksi_obj c2);

JOKE_API
ksi_obj
ksi_fill_polygon (ksi_obj dc, ksi_obj pnt, ksi_obj coord_mode);

JOKE_API
ksi_obj
ksi_fill_ellipse (ksi_obj dc, ksi_obj c1, ksi_obj c2);

JOKE_API
ksi_obj
ksi_fill_arc (ksi_obj dc, ksi_obj c1, ksi_obj c2, ksi_obj a1, ksi_obj a2);

JOKE_API
ksi_obj
ksi_draw_string (ksi_obj dc, ksi_obj pnt, ksi_obj str);

JOKE_API
ksi_obj
ksi_string_width (ksi_obj dc, ksi_obj str);

JOKE_API
ksi_obj
ksi_string_height (ksi_obj dc, ksi_obj str);


/* toolkit utils */

JOKE_API
ksi_obj
ksi_tk_event_loop (ksi_obj test_proc);

#if defined(X11_GRAPH)

JOKE_API
ksi_obj
ksi_make_x11_event (XEvent* event);

#elif defined(WIN_GRAPH)

JOKE_API
ksi_obj
ksi_make_win_left_down (HWND wnd, int x, int y, unsigned state);

JOKE_API
ksi_obj
ksi_make_win_middle_down (HWND wnd, int x, int y, unsigned state);

JOKE_API
ksi_obj
ksi_make_win_right_down (HWND wnd, int x, int y, unsigned state);

JOKE_API
ksi_obj
ksi_make_win_left_up (HWND wnd, int x, int y, unsigned state);

JOKE_API
ksi_obj
ksi_make_win_middle_up (HWND wnd, int x, int y, unsigned state);

JOKE_API
ksi_obj
ksi_make_win_right_up (HWND wnd, int x, int y, unsigned state);

JOKE_API
ksi_obj
ksi_make_win_mouse_move (HWND wnd, int x, int y, unsigned state);

JOKE_API
ksi_obj
ksi_make_win_key_down (HWND wnd, unsigned keycode);

JOKE_API
ksi_obj
ksi_make_win_key_up (HWND wnd, unsigned keycode);

JOKE_API
ksi_obj
ksi_make_win_char (HWND wnd, unsigned keycode);

JOKE_API
ksi_obj
ksi_make_win_paint (HWND wnd, int x, int y, int width, int height);

ksi_obj
ksi_make_win_windowposchanged (HWND wnd, int x, int y, int w, int h);

ksi_obj
ksi_make_win_destroy (HWND wnd);


#endif


#ifdef __cplusplus
}
#endif

#endif

/* End of file */
