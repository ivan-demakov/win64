/* -*-mode:C++-*- */
/*
 * ksi_port.h
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
 * Creation date: Wed Feb 18 14:54:31 2009
 *
 * $Id: ksi_port.h,v 1.1.2.8.2.1 2009/07/05 15:50:25 ksion Exp $
 *
 */

#ifndef KSI_PORT_H
#define KSI_PORT_H

#include "ksi_type.h"


struct Ksi_Port_Ops
{
  const char *(*name) (ksi_port);
  int (*close) (ksi_port);
  int (*read) (ksi_port, char* buf, int buf_len);
  int (*write) (ksi_port, const char* buf, int buf_len);
  int (*input_ready) (ksi_port);
  int (*output_ready) (ksi_port);
  int (*flush) (ksi_port);
  int (*input_fd) (ksi_port);
  int (*output_fd) (ksi_port);
};

struct Ksi_Port
{
  unsigned itag;
  struct Ksi_Port_Ops *ops;
  int read_line, read_pos;
  char last_write_char;
  char unread_num;
  char unread_chars[6];

  unsigned input : 1;
  unsigned output : 1;
  unsigned closed : 1;
  unsigned eof : 1;

  unsigned unbuf : 1;
  unsigned linebuf : 1;
  unsigned async : 1;

  unsigned is_fd : 1;
  unsigned is_tty : 1;
  unsigned is_ext : 1;

  unsigned is_dir : 1;
  unsigned is_chr : 1;
  unsigned is_blk : 1;
  unsigned is_reg : 1;
  unsigned is_fifo : 1;
  unsigned is_lnk : 1;
  unsigned is_sock : 1;
  unsigned is_inet : 1;
};


struct Ksi_FdPort
{
  struct Ksi_Port kp;
  char *name;			/* port name */
  char *r_buf;			/* read ahead buffer */
  char *w_buf;			/* pending write buffer */
  ksi_obj evt;			/* async event */
  int fd;			/* file desc */
  int pg_size;			/* min buffer size */
  int r_pos;			/* current read position */
  int r_len;			/* number of bytes in r_buf */
  int w_num;			/* number of pending bytes */
  int w_size;			/* size of w_buf */
//  struct Ksi_FdPort *next;	/* list of all fd ports */

#if defined(WIN32)
  const char *(*error) (struct Ksi_FdPort*);
  int (*close) (struct Ksi_FdPort*);
  int (*r_ready) (struct Ksi_FdPort*, int wait);
  int (*w_ready) (struct Ksi_FdPort*, int wait);
  int (*read) (struct Ksi_FdPort*, char *buf, int len);
  int (*write) (struct Ksi_FdPort*, const char *buf, int len);
  int (*set_async) (struct Ksi_FdPort*, int async);
#endif
};

struct Ksi_StringPort
{
  struct Ksi_Port kp;
  ksi_string str;
  int size, pos;
};


#define KSI_PORT_P(x)		(KSI_OBJ_IS (x, KSI_TAG_PORT))
#define KSI_INPUT_PORT_P(x)	(KSI_PORT_P (x) && (((ksi_port) (x)) -> input))
#define KSI_OUTPUT_PORT_P(x)	(KSI_PORT_P (x) && (((ksi_port) (x)) -> output))
#define KSI_CLOSED_PORT_P(x)	(KSI_PORT_P (x) && (((ksi_port) (x)) -> closed))
#define KSI_STD_PORT_P(x)	(KSI_PORT_P (x) && (((ksi_port) (x)) -> std))
#define KSI_TTY_PORT_P(x)	(KSI_PORT_P (x) && (((ksi_port) (x)) -> tty))


#ifdef __cplusplus
extern "C" {
#endif

SI_API
ksi_obj
ksi_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_input_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_output_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_null_port (void);

SI_API
ksi_port
ksi_new_nul_port (void);

SI_API
ksi_port
ksi_new_str_port (ksi_string str);

SI_API
ksi_port
ksi_new_fd_port (int fd, const char* name, int no_init);

ksi_port
ksi_open_fd_port_int (const char* fname, const char* mode, const char* proc);

SI_API
ksi_obj
ksi_make_fd_port (int fd, const char *fname, const char *mode);

SI_API
ksi_obj
ksi_open_fd_port (const char *fname, const char *mode);

SI_API
int
ksi_port_putc (ksi_port port, int ch);

SI_API
int
ksi_port_getc (ksi_port port);

SI_API
void
ksi_port_ungetc (ksi_port port, int ch);

SI_API
int
ksi_port_read (ksi_obj port, char* ptr, int len);

SI_API
int
ksi_port_write (ksi_obj port, const char* ptr, int len);

SI_API
ksi_obj
ksi_flush_port (ksi_obj port);

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

SI_API
ksi_obj
ksi_open_file (ksi_obj fname, ksi_obj mode);

SI_API
ksi_obj
ksi_open_string (ksi_obj str, ksi_obj mode);

SI_API
ksi_obj
ksi_port_string (ksi_obj str_port);

SI_API
ksi_obj
ksi_close_port (ksi_obj port);

SI_API
ksi_obj
ksi_read_char (ksi_obj p);

SI_API
ksi_obj
ksi_peek_char (ksi_obj p);

SI_API
ksi_obj
ksi_char_ready_p (ksi_obj p);

SI_API
ksi_obj
ksi_port_ready_p (ksi_obj p);

SI_API
ksi_obj
ksi_write_char (ksi_obj o, ksi_obj p);

SI_API
ksi_obj
ksi_eof_object_p (ksi_obj x);

SI_API
ksi_obj
ksi_newline (ksi_obj port);

SI_API
ksi_obj
ksi_write (ksi_obj o, ksi_obj port);

SI_API
ksi_obj
ksi_display (ksi_obj o, ksi_obj port);

SI_API
ksi_obj
ksi_read_block (ksi_obj port, ksi_obj size);

SI_API
ksi_obj
ksi_write_block (ksi_obj port, ksi_obj str);

SI_API
ksi_obj
ksi_set_async_mode (ksi_obj x, ksi_obj async);

#ifdef __cplusplus
}
#endif



#endif

/* End of file */
