/* -*-mode:C++-*- */
/*
 * ksi_evt.h
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
 * Creation date: Wed Feb 18 19:50:00 2009
 *
 * $Id: ksi_evt.h,v 1.1.2.4.2.1 2009/07/05 15:50:25 ksion Exp $
 *
 */

#ifndef KSI_EVT_H
#define KSI_EVT_H

typedef struct Ksi_Event_Mgr	*ksi_event_mgr;
typedef struct Ksi_Event_Tag	*ksi_event_tag;
typedef struct Ksi_Event	*ksi_event;


struct Ksi_Event_Mgr
{
  void  (*init) (ksi_event_mgr mgr);
  void  (*term) (ksi_event_mgr mgr);

  void* (*wait_timer) (ksi_event_mgr mgr, ksi_event evt, double tm, int restart);
  void  (*cancel_timer) (ksi_event_mgr mgr, ksi_event evt, void *data);

  void* (*wait_input) (ksi_event_mgr mgr, ksi_event evt, int fd, int restart);
  void  (*cancel_input) (ksi_event_mgr mgr, ksi_event evt, void *data);

  void* (*wait_output) (ksi_event_mgr mgr, ksi_event evt, int fd, int restart);
  void  (*cancel_output) (ksi_event_mgr mgr, ksi_event evt, void *data);

  void* (*wait_signal) (ksi_event_mgr mgr, ksi_event evt, int sig, int restart);
  void  (*cancel_signal) (ksi_event_mgr mgr, ksi_event evt, void *data);

  void* (*wait_idle) (ksi_event_mgr mgr, ksi_event evt, int restart);
  void  (*cancel_idle) (ksi_event_mgr mgr, ksi_event evt, void *data);

  void  (*wait_event) (ksi_event_mgr mgr, double tm);

  void  (*enable_async_wait) (ksi_event_mgr mgr);
  void  (*disable_async_wait) (ksi_event_mgr mgr);

  void  (*block_wait) (ksi_event_mgr mgr);
  void  (*unblock_wait) (ksi_event_mgr mgr);
};

struct Ksi_Event_Tag
{
  const char* (*name) (ksi_event);
  void (*init) (ksi_event);
  void (*setup) (ksi_event);
  void (*cancel) (ksi_event);
  int  (*invoke) (ksi_event, void *data);
};

struct Ksi_Event
{
  int itag;
  ksi_event_tag ops;
  ksi_obj state;
  ksi_obj action;
  ksi_obj result;

  ksi_event next, prev;
  void *data;

  unsigned pending : 1;
  unsigned waiting : 1;
  unsigned active: 1;
  unsigned ready : 1;
  unsigned inited : 1;
  unsigned start : 1;
  unsigned stop : 1;
};


#define KSI_EVT_P(x)		(KSI_OBJ_IS ((x), KSI_TAG_EVENT))
#define KSI_EVT_STATE(x)	(((ksi_event) (x)) -> state)
#define KSI_EVT_ACTION(x)	(((ksi_event) (x)) -> action)
#define KSI_EVT_RESULT(x)	(((ksi_event) (x)) -> result)


SI_API
ksi_event_mgr
ksi_register_event_mgr (ksi_event_mgr mgr);

SI_API
ksi_event_mgr
ksi_current_event_mgr (void);

SI_API
void*
ksi_wait_timer (ksi_event evt, double tm, int restart);

SI_API
void
ksi_cancel_timer (ksi_event evt, void *data);

SI_API
void*
ksi_wait_input (ksi_event evt, int fd, int restart);

SI_API
void
ksi_cancel_input (ksi_event evt, void *data);

SI_API
void*
ksi_wait_output (ksi_event evt, int fd, int restart);

SI_API
void
ksi_cancel_output (ksi_event evt, void *data);

SI_API
void*
ksi_wait_signal (ksi_event evt, int sig, int restart);

SI_API
void
ksi_cancel_signal (ksi_event evt, void *data);

SI_API
void*
ksi_wait_idle (ksi_event evt, int restart);

SI_API
void
ksi_cancel_idle (ksi_event evt, void *data);

SI_API
void
ksi_run_event (ksi_event evt, void *data, int invoke);

SI_API
int
ksi_run_pending_events (void);

SI_API
int
ksi_has_pending_events (void);

SI_API
int
ksi_do_events (void);

SI_API
void
ksi_cancel_port_events (ksi_port port);


SI_API
ksi_obj
ksi_event_p (ksi_obj x);

SI_API
ksi_obj
ksi_event_state (ksi_obj x);

SI_API
ksi_obj
ksi_event_procedure (ksi_obj x);

SI_API
ksi_obj
ksi_event_result (ksi_obj x);

SI_API
ksi_obj
ksi_start_event (ksi_obj x);

SI_API
ksi_obj
ksi_stop_event (ksi_obj x);

SI_API
ksi_obj
ksi_wait_event (ksi_obj tm);

SI_API
ksi_obj
ksi_sleep (ksi_obj tm);

SI_API
ksi_obj
ksi_enable_evt (void);

SI_API
ksi_obj
ksi_disable_evt (void);

SI_API
ksi_obj
ksi_ready_event (ksi_obj proc);

SI_API
ksi_obj
ksi_timer_event (ksi_obj tm, ksi_obj proc);

SI_API
ksi_obj
ksi_input_event (ksi_obj tm, ksi_obj port, ksi_obj proc);

SI_API
ksi_obj
ksi_output_event (ksi_obj tm, ksi_obj port, ksi_obj proc);

SI_API
ksi_obj
ksi_signal_event (ksi_obj signum, ksi_obj proc);

SI_API
ksi_obj
ksi_idle_event (ksi_obj proc);


#endif

/* End of file */
