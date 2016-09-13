;;;
;;; Joke.scm
;;; Joke init
;;;
;;; Copyright (C) 1997-2000, Ivan Demakov
;;;
;;; Permission to use, copy, modify, and distribute this software and its
;;; documentation for any purpose, without fee, and without a written agreement
;;; is hereby granted, provided that the above copyright notice and this
;;; paragraph and the following two paragraphs appear in all copies.
;;; Modifications to this software may be copyrighted by their authors
;;; and need not follow the licensing terms described here, provided that
;;; the new terms are clearly indicated on the first page of each file where
;;; they apply.
;;;
;;; IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
;;; FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
;;; INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
;;; DOCUMENTATION, EVEN IF THE AUTHORS HAS BEEN ADVISED OF THE
;;; POSSIBILITY OF SUCH DAMAGE.
;;;
;;; THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIMS ANY WARRANTIES,
;;; INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
;;; AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
;;; ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAS NO OBLIGATIONS
;;; TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
;;;
;;;
;;; Author:        Ivan Demakov <demakov@users.sourceforge.net>
;;; Creation date: Tue Aug 19 18:55:06 1997
;;; Last Update:   Tue Mar 21 18:46:19 2000
;;;
;;;
;;; $Id: Joke.scm,v 1.8 2000/04/07 03:19:00 ivan Exp $
;;;
;;;
;;; Comments:
;;;
;;;

(set-default-font-weight  "*")
(set-default-font-style   "*-*-")
(set-default-font-charset (case (ksi:host)
			     ((unix) "koi8-r")
			     ((win32) "russian")
			     (else "*")))

(define (event-type event)
  (vector-ref event 0))

(define (event-window event)
  (vector-ref event 1))

(define (event-x event)
  (vector-ref event 2))

(define (event-y event)
  (vector-ref event 3))

(define (event-state event)
  (vector-ref event 4))

(define (event-button event)
  (vector-ref event 5))

(define (event-key event)
  (vector-ref event 5))

(define (event-width event)
  (vector-ref event 4))

(define (event-height event)
  (vector-ref event 5))

(define (event-count event)
  (vector-ref event 6))

;;; End of code