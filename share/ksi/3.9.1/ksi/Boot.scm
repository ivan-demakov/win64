;;;
;;; Boot.scm
;;; this file is loaded on startup
;;;
;;; Copyright (C) 1998-2010, ivan demakov
;;;
;;; The software is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU Lesser General Public License as published by
;;; the Free Software Foundation; either version 2.1 of the License, or (at your
;;; option) any later version.
;;;
;;; The software is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
;;; License for more details.
;;;
;;; You should have received a copy of the GNU Lesser General Public License
;;; along with the software; see the file COPYING.LESSER.  If not, write to
;;; the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
;;; MA 02110-1301, USA.
;;;
;;;
;;; Author:        ivan demakov <ksion@users.sourceforge.net>
;;; Creation date: Mon Mar  2 19:05:07 1998
;;; Last Update:   Sun Sep 19 19:44:37 2010
;;;
;;;

(import (ksi core syntax)
        (ksi list)
        (ksi number)
        (ksi bytevectors)
        (ksi core char)
        (ksi core string)
        (ksi core vector)
        (ksi core base)
        (ksi core eval)
        (ksi io)

        ;; ksi init
        (ksi init))

;;; set locale
(setlocale LC_CTYPE: "")

;(errlog-priority errlog/warning)
(errlog-priority errlog/debug)
