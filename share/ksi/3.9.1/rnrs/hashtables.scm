;;;
;;; hashtables.scm
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
;;; Creation date: Sat Mar  6 22:26:56 2010
;;; Last Update:   Fri Aug 13 14:47:06 2010
;;;
;;;

(library (rnrs hashtables (6))
  (export make-eq-hashtable make-eqv-hashtable make-hashtable
          hashtable? hashtable-size hashtable-ref
          hashtable-set! hashtable-delete!
          hashtable-contains? hashtable-update!
          hashtable-copy hashtable-clear!
          hashtable-keys hashtable-entries
          hashtable-equivalence-function
          hashtable-hash-function
          hashtable-mutable?
          equal-hash string-hash string-ci-hash symbol-hash)

  (import (ksi core syntax)
          (ksi core hashtables)
          (only (ksi core vector) list->vector)
          (only (ksi core string) string-copy string-downcase!)
          (only (ksi core base) values))


(define (hashtable-update! tab key proc def)
  (hashtable-set! tab key (proc (hashtable-ref tab key def))))


(define (hashtable-keys tab)
  (let ((keys '()))
    (hashtable-for-each (lambda (key val) (set! keys (cons key keys)))
                        tab)

    (list->vector keys)))


(define (hashtable-entries tab)
  (let ((keys '()) (vals '()))
    (hashtable-for-each (lambda (key val) (set! keys (cons key keys))
                                (set! vals (cons val vals)))
                        tab)

    (values (list->vector keys)
            (list->vector vals))))


(define (string-hash x)
  (or (string? x)
      (error 'string-hash "invalid string in arg1" x))
  (equal-hash x))


(define (string-ci-hash x)
  (or (string? x)
      (error 'string-hash "invalid string in arg1" x))
  (equal-hash (string-downcase! (string-copy x))))


(define (symbol-hash x)
  (or (symbol? x)
      (error 'string-hash "invalid symbol in arg1" x))
  (equal-hash x))

)


;;; End of code