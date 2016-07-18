;;;
;;; list.scm
;;;
;;; Copyright (C) 2010, ivan demakov.
;;;
;;; This code is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU Lesser General Public License as published by
;;; the Free Software Foundation; either version 2.1 of the License, or (at your
;;; option) any later version.
;;;
;;; This code is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
;;; License for more details.
;;;
;;; You should have received a copy of the GNU Lesser General Public License
;;; along with this code; see the file COPYING.LESSER.  If not, write to
;;; the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
;;; MA 02110-1301, USA.
;;;
;;;
;;; Author:        ivan demakov <ksion@users.sourceforge.net>
;;; Creation date: Wed Mar  3 20:35:25 2010
;;; Last Update:   Fri Aug 13 14:47:53 2010
;;;
;;;

(library (rnrs list)
         (export find for-all exists filter partition
                 fold-left fold-right
                 remp remove remv remq
                 memp member memv memq
                 assp assoc assv assq cons*)

         (import (only (ksi list)
                       find for-all exists filter partition
                       fold-left fold-right cons*
                       remp remq remv remove
                       memp memq memv member
                       assp assq assv assoc))

)

;;; End of code