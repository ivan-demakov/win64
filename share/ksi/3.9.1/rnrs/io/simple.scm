;;;
;;; simple.scm
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
;;; Creation date: Sat Sep 18 00:19:38 2010
;;; Last Update:   Sat Sep 18 00:44:06 2010
;;;
;;;

(library (rnrs io simple (6))
  (export ;&i/o make-i/o-error i/o-error?
          ;&i/o-read make-i/o-read-error i/o-read-error?
          ;&i/o-write make-i/o-write-error i/o-write-error?
          ;&i/o-invalid-position make-i/o-invalid-position-error i/o-invalid-position-error? i/o-error-position
          ;&i/o-filename make-i/o-filename-error i/o-filename-error? i/o-error-filename
          ;&i/o-file-protection make-i/o-file-protection-error i/o-file-protection-error?
          ;&i/o-file-is-read-only make-i/o-file-is-read-only-error i/o-file-is-read-only-error?
          ;&i/o-file-already-exists make-i/o-file-already-exists-error i/o-file-already-exists-error?
          ;&i/o-file-does-not-exist make-i/o-file-does-not-exist-error i/o-file-does-not-exist-error?
          ;&i/o-port make-i/o-port-error i/o-port-error? i/o-error-port
          ;&i/o-decoding make-i/o-decoding-error i/o-decoding-error?
          ;&i/o-encoding make-i/o-encoding-error i/o-encoding-error? i/o-encoding-error-char

          current-input-port current-output-port current-error-port
          eof-object eof-object? input-port? output-port?
          open-input-file open-output-file close-input-port close-output-port
          call-with-input-file call-with-output-file
          with-input-from-file with-output-to-file
          read-char peek-char write-char
          read newline display write)

  (import (ksi core syntax)

          (only (ksi core base)
                dynamic-wind)

          (only (ksi io)
                current-input-port current-output-port current-error-port
                set-current-input-port! set-current-output-port!
                eof-object eof-object? input-port? output-port?
                open-file-input-port open-file-output-port close-port
                file-options buffer-mode native-transcoder
                call-with-port read-char peek-char write-char
                read newline display write))


(define (open-input-file filename)
  (open-file-input-port filename (file-options) (buffer-mode block) (native-transcoder)))

(define (open-output-file filename)
  (open-file-output-port filename (file-options) (buffer-mode block) (native-transcoder)))

(define (close-input-port port)
  (close-port port))

(define (close-output-port port)
  (close-port port))

(define (call-with-input-file filename proc)
  (call-with-port (open-input-file filename) proc))

(define (call-with-output-file filename proc)
  (call-with-port (open-output-file filename) proc))

(define (with-input-from-file filename thunk)
  (let ((port (open-input-file filename)) (save (current-input-port)))
    (dynamic-wind
        (lambda () (set-current-input-port! port))
        (lambda () (let ((res (thunk))) (close-input-port port) res))
        (lambda () (set-current-input-port! save)))))

(define (with-output-to-file filename thunk)
  (let ((port (open-output-file filename)) (save (current-output-port)))
    (dynamic-wind
        (lambda () (set-current-output-port! port))
        (lambda () (let ((res (thunk))) (close-output-port port) res))
        (lambda () (set-current-output-port! save)))))

)

;;; End of code