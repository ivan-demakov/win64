/* -*-mode:C++-*- */
/*
 * ksi_port.h
 *
 * Copyright (C) 2009-2010, ivan demakov.
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
 * Creation date: Wed Feb 18 14:54:31 2009
 * Last Update:   Thu Sep 16 19:43:52 2010
 *
 */

#ifndef KSI_PORT_H
#define KSI_PORT_H

#include "ksi_type.h"


struct Ksi_Port
{
    struct Ksi_ObjData o;

    const wchar_t *port_id;

    unsigned binary : 1;
    unsigned input : 1;
    unsigned output : 1;
    unsigned closed : 1;

    unsigned can_getpos : 1;
    unsigned can_setpos : 1;
    unsigned unbuf : 1;
    unsigned linebuf : 1;

    unsigned is_bytevector : 1;
    unsigned is_string : 1;
    unsigned is_fd : 1;
    unsigned is_transcoded : 1;

    unsigned eof : 1;
};


struct Ksi_Byte_Port_Ops
{
    int (*close) (ksi_byte_port port);
    int (*flush) (ksi_byte_port port);
    int (*read) (ksi_byte_port port, char *buf, int buf_len);
    int (*lookahead) (ksi_byte_port port);
    int (*write) (ksi_byte_port port, const char *buf, int buf_len);
    int (*getpos) (ksi_byte_port port, long *pos);
    int (*setpos) (ksi_byte_port port, long pos);
};

struct Ksi_Byte_Port
{
    struct Ksi_Port kp;
    struct Ksi_Byte_Port_Ops *ops;
};


struct Ksi_Char_Port_Ops
{
    int (*close) (ksi_char_port port);
    int (*flush) (ksi_char_port port);
    int (*read) (ksi_char_port port, wchar_t *buf, int buf_len);
    int (*lookahead) (ksi_char_port port);
    int (*write) (ksi_char_port port, const wchar_t *buf, int buf_len);
    int (*getpos) (ksi_char_port port, long *pos);
    int (*setpos) (ksi_char_port port, long pos);
};

struct Ksi_Char_Port
{
    struct Ksi_Port kp;
    struct Ksi_Char_Port_Ops *ops;

    const char *file_name;
    int file_line, line_pos;
    wchar_t last_write_char;
};


#define KSI_PORT_P(x)             (KSI_OBJ_IS(((ksi_obj)(x)), KSI_TAG_PORT))
#define KSI_BIN_INPUT_PORT_P(x)   (KSI_PORT_P(x) && (((struct Ksi_Port *) (x)) -> binary) && (((struct Ksi_Port *) (x)) -> input))
#define KSI_BIN_OUTPUT_PORT_P(x)  (KSI_PORT_P(x) && (((struct Ksi_Port *) (x)) -> binary)&& (((struct Ksi_Port *) (x)) -> output))
#define KSI_TEXT_INPUT_PORT_P(x)  (KSI_PORT_P(x) && !(((struct Ksi_Port *) (x)) -> binary) && (((struct Ksi_Port *) (x)) -> input))
#define KSI_TEXT_OUTPUT_PORT_P(x) (KSI_PORT_P(x) && !(((struct Ksi_Port *) (x)) -> binary)&& (((struct Ksi_Port *) (x)) -> output))
#define KSI_CLOSED_PORT_P(x)      (KSI_PORT_P(x) && (((struct Ksi_Port *) (x)) -> closed))
#define KSI_EOF_PORT_P(x)         (KSI_PORT_P(x) && (((struct Ksi_Port *) (x)) -> eof))


#ifdef __cplusplus
extern "C" {
#endif

SI_API
ksi_obj
ksi_null_port (void);

SI_API
ksi_char_port
ksi_new_null_port (void);

SI_API
ksi_byte_port
ksi_new_bytevector_input_port (ksi_obj bytevec);

SI_API
ksi_obj
ksi_open_bytevector_input_port (ksi_obj bytevec);

SI_API
ksi_obj
ksi_open_bytevector_output_port (void);

SI_API
ksi_obj
ksi_extract_bytevector_port_data (ksi_obj p);

SI_API
ksi_char_port
ksi_new_string_input_port (ksi_obj str, const char *file_name, int file_line);

SI_API
ksi_obj
ksi_open_string_input_port (ksi_obj str);

SI_API
ksi_obj
ksi_open_string_output_port (void);

SI_API
ksi_obj
ksi_extract_string_port_data (ksi_obj p);

SI_API
ksi_byte_port
ksi_new_fd_input_port (int fd, const char *file_name);

SI_API
ksi_byte_port
ksi_new_fd_output_port (int fd, const char *file_name);

SI_API
ksi_byte_port
ksi_new_fd_inout_port (int fd, const char *file_name);

SI_API
ksi_byte_port
ksi_new_fd_append_port (int fd, const char *file_name);

SI_API
ksi_obj
ksi_open_file_input_port (ksi_obj filename, ksi_obj mode, ksi_obj bufmode);

SI_API
ksi_obj
ksi_open_file_output_port (ksi_obj filename, ksi_obj mode, ksi_obj bufmode);

SI_API
ksi_obj
ksi_open_file_inout_port (ksi_obj filename, ksi_obj mode, ksi_obj bufmode);

SI_API
ksi_char_port
ksi_new_transcoded_port (ksi_byte_port port, ksi_obj codec, ksi_obj eol, ksi_obj handling, const char *file_name, int file_line);

SI_API
ksi_obj
ksi_transcoded_port (ksi_obj port, ksi_obj codec, ksi_obj eol, ksi_obj handling);

SI_API
ksi_obj
ksi_port_codec (ksi_obj port);

SI_API
ksi_obj
ksi_set_port_codec_x (ksi_obj port, ksi_obj codec);

SI_API
ksi_obj
ksi_port_eol_style (ksi_obj p);

SI_API
ksi_obj
ksi_port_error_handling_mode (ksi_obj p);


SI_API
ksi_obj
ksi_eof_object (void);

SI_API
ksi_obj
ksi_eof_object_p (ksi_obj x);

SI_API
ksi_obj
ksi_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_text_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_bin_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_input_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_output_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_fd_port_p (ksi_obj port);

SI_API
ksi_obj
ksi_transcoded_port_p (ksi_obj x);

SI_API
ksi_obj
ksi_port_can_getpos_p (ksi_obj x);

SI_API
ksi_obj
ksi_port_can_setpos_p (ksi_obj x);

SI_API
ksi_obj
ksi_port_getpos (ksi_obj port);

SI_API
ksi_obj
ksi_port_setpos (ksi_obj port, ksi_obj pos);

SI_API
ksi_obj
ksi_close_port (ksi_obj port);

SI_API
ksi_obj
ksi_flush_output_port (ksi_obj port);

SI_API
ksi_obj
ksi_port_buffer_mode (ksi_obj port);

SI_API
ksi_obj
ksi_port_eof_p (ksi_obj port);


SI_API
int
ksi_get_byte (ksi_byte_port port);

SI_API
int
ksi_lookahead_byte (ksi_byte_port port);

SI_API
ksi_obj
ksi_get_bytevector (ksi_obj port, ksi_obj bv, ksi_obj start, ksi_obj count);

SI_API
void
ksi_put_byte (ksi_byte_port port, int byte);

SI_API
ksi_obj
ksi_put_bytevector (ksi_obj port, ksi_obj bv, ksi_obj start, ksi_obj count);


SI_API
wint_t
ksi_get_char (ksi_char_port port);

SI_API
wint_t
ksi_lookahead_char (ksi_char_port port);

SI_API
ksi_obj
ksi_get_string (ksi_obj port, ksi_obj bv, ksi_obj start, ksi_obj count);

SI_API
ksi_obj
ksi_get_line (ksi_obj port);

SI_API
void
ksi_put_char (ksi_char_port port, wchar_t ch);

SI_API
ksi_obj
ksi_put_string (ksi_obj port, ksi_obj str, ksi_obj start, ksi_obj count);


SI_API
ksi_obj
ksi_get_datum (ksi_obj port);

SI_API
ksi_obj
ksi_put_datum (ksi_obj port, ksi_obj x);


SI_API
ksi_obj
ksi_read_char (ksi_obj p);

SI_API
ksi_obj
ksi_peek_char (ksi_obj p);

SI_API
ksi_obj
ksi_read (ksi_obj port);

SI_API
int
ksi_port_write (ksi_char_port port, const wchar_t *ptr, int len);

SI_API
ksi_obj
ksi_write (ksi_obj o, ksi_obj port);

SI_API
ksi_obj
ksi_display (ksi_obj o, ksi_obj port);

SI_API
ksi_obj
ksi_newline (ksi_obj port);

SI_API
ksi_obj
ksi_write_char (ksi_obj o, ksi_obj p);


/** Format output
 *
 * @param port port for output
 * @param fmt format string
 * @param argc argument count
 * @param argv argumet array
 * @param nm name of caller
 *
 * @return unspecified value
 */
SI_API
int
ksi_internal_format (ksi_char_port port, const wchar_t *fmt, int argc, ksi_obj* argv, char* nm);

/** Format output
 *
 * @param port port for output
 * @param fmt format string
 * @param argc argument count
 * @param argv argumet array
 *
 * @return unspecified value
 */
SI_API
ksi_obj
ksi_format (ksi_obj port, const wchar_t *fmt, int argc, ksi_obj* argv);


#ifdef __cplusplus
}
#endif



#endif

/* End of file */
