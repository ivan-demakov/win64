;;;
;;; io.scm
;;;
;;; Copyright (C) 2009, 2010, ivan demakov.
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
;;; Creation date: Tue Dec 15 19:56:35 2009
;;; Last Update:   Sun Feb 28 22:26:09 2010
;;;
;;;

(library (ksi io)
         (export latin-1-codec utf-8-codec utf-16-codec
                 native-eol-style error-handling-mode
                 file-options buffer-mode buffer-mode?
                 make-transcoder native-transcoder
                 transcoder-codec transcoder-eol-style transcoder-handling-mode

                 transcoded-port port-transcoder
                 string->bytevector bytevector->string

                 call-with-port
                 standard-input-port standard-output-port standard-error-port

                 open-file-input-port open-file-output-port open-file-input/output-port

                 open-bytevector-input-port open-bytevector-output-port
                 get-bytevector-n get-bytevector-some get-bytevector-all
                 get-string-n get-string-all

                 ; from (ksi core io)
                 setlocale null-port object->string format
                 current-input-port current-output-port current-error-port
                 set-current-input-port set-current-output-port set-current-error-port

                 open-string-input-port open-string-output-port
                 extract-bytevector-port-data extract-string-port-data

                 eof-object eof-object? port? port-eof? close-port flush-output-port
                 input-port? output-port? textual-port? binary-port? port-buffer-mode
                 port-has-port-position? port-position port-has-set-port-position!? set-port-position!
                 get-u8 lookahead-u8 get-bytevector-n! put-u8 put-bytevector
                 get-char lookahead-char get-string-n! put-char put-string
                 get-line get-datum put-datum

                 read write display newline write-char read-char peek-char

                 annotation? annotation-source annotation-expression)

         (import (ksi core syntax)
                 (only (ksi list) null? pair? list? cons car cdr cadr cddr caddr reverse for-all)
                 (only (ksi core bytevector) bytevector? bytevector-length make-bytevector bytevector-copy!)
                 (only (ksi core vector) vector vector? vector-length vector-ref)
                 (only (ksi core string) string? list->string string-length string-ref make-string substring)
                 (only (ksi core number) = < <= + exact? integer? zero?)
                 (only (ksi core base) eq? symbol? not procedure?)
                 (only (ksi core eval) define-macro! procedure-has-arity? error)
                 (only (ksi optargs) let-optional)
                 (rename (ksi core io)
                         (core:transcoded-port transcoded-port)
                         (core:open-file-input-port open-file-input-port)
                         (core:open-file-output-port open-file-output-port)
                         (core:open-file-input/output-port open-file-input/output-port)
                         (core:open-bytevector-input-port open-bytevector-input-port)
                         (core:open-bytevector-output-port open-bytevector-output-port)
                         ))


(define (latin-1-codec)
  'latin-1)

(define (utf-8-codec)
  'utf-8)

(define (utf-16-codec)
  'utf-16)

(define (native-eol-style)
  'native)

(define-macro! (file-options form env)
  (or (and (list? form) (for-all symbol? (cdr form)))
      (error (car form) "invalid syntax" form))
  `(,#'quote ,(cdr form)) )

(define-macro! (buffer-mode form env)
  (or (and (pair? form) (pair? (cdr form)) (symbol? (cadr form)) (null? (cddr form)))
      (error (car form) "invalid syntax" form))
  `(,#'quote ,(cadr form)) )

(define (buffer-mode? mode)
  (or (eq? mode 'none) (eq? mode 'line) (eq? mode 'block)))

(define-macro! (error-handling-mode form env)
  (or (and (pair? form) (pair? (cdr form)) (symbol? (cadr form)) (null? (cddr form)))
      (error (car form) "invalid syntax" form))
  `(,#'quote ,(cadr form)) )

(define transcoder-tag '(transcoder))

(define (transcoder? x)
  (and (vector? x) (= (vector-length x) 4) (eq? (vector-ref x 0) transcoder-tag)))

(define (make-transcoder codec . opts)
  (let-optional opts ((eol-style 'native)
                      (handling-mode 'replace))
    (vector transcoder-tag codec eol-style handling-mode)))

(define (native-transcoder)
    (make-transcoder 'native 'native 'replace))

(define (transcoder-codec tr)
  (or (transcoder? tr)
      (error 'transcoder-codec "invalid transcoder in arg1" tr))
  (vector-ref tr 1))

(define (transcoder-eol-style tr)
  (or (transcoder? tr)
      (error 'transcoder-codec "invalid transcoder in arg1" tr))
  (vector-ref tr 2))

(define (transcoder-handling-mode tr)
  (or (transcoder? tr)
      (error 'transcoder-codec "invalid transcoder in arg1" tr))
  (vector-ref tr 3))


(define (transcoded-port port tr)
  (or (binary-port? port)
      (error 'transcoded-port "invalid binary port in arg1" port))
  (or (transcoder? tr)
      (error 'transcoded-port "invalid transcoder in arg2" tr))

  (core:transcoded-port port (transcoder-codec tr) (transcoder-eol-style tr) (transcoder-handling-mode tr)))


(define (port-transcoder port)
  (or (port? port)
      (error 'port-transcoder "invalid port in arg1" port))
  (and (transcoded-port? port)
       (make-transcoder (port-codec port) (port-eol-style port) (port-error-handling-mode port))))


(define (bytevector->string vec tr)
  (or (bytevector? vec)
      (error 'bytevector->string "invalid bytevectort in arg1" vec))
  (or (transcoder? tr)
      (error 'bytevector->string "invalid transcoder in arg2" tr))
  (let* ((bp (open-bytevector-input-port vec))
         (tp (core:transcoded-port bp (transcoder-codec tr) (transcoder-eol-style tr) (transcoder-handling-mode tr))))
    (let loop ((ls '())
               (ch (get-char tp)))
      (if (eof-object? ch)
          (list->string (reverse ls))
          (loop (cons ch ls) (get-char tp))))))


(define (string->bytevector str tr)
  (or (string? str)
      (error 'string->bytevector "invalid string in arg1" str))
  (or (transcoder? tr)
      (error 'string->bytevector "invalid transcoder in arg2" tr))
  (let* ((bp (open-bytevector-output-port))
         (tp (core:transcoded-port bp (transcoder-codec tr) (transcoder-eol-style tr) (transcoder-handling-mode tr))))
    (let loop ((i 0))
      (if (< i (string-length str))
          (begin (put-char tp (string-ref str i))
                 (loop (+ i 1)))
          (extract-bytevector-port-data bp)))))


(define (call-with-port port proc)
  (or (port? port)
      (error 'call-with-port "invalid port in arg1" port))
  (or (procedure? proc)
      (error 'call-with-port "invalid procedure in arg2" proc))
  (or (procedure-has-arity? proc 1)
      (error 'call-with-port "procedure in arg2 should have 1 argument" proc))
  (let ((val (proc port)))
    (close-port port)
    val))


(define (standard-input-port)
  stdin)

(define (standard-output-port)
  stdout)

(define (standard-error-port)
  stderr)


(define (open-file-input-port filename . opts)
  (let-optional opts ((file-options '())
                      (buffer-mode 'block)
                      (transcoder #f))

    (or (and (list? file-options) (for-all symbol? file-options))
        (error 'open-file-input-port "invalid file-options in arg2" file-options))
    (or (symbol? buffer-mode)
        (error 'open-file-input-port "invalid buffer-mode in arg3" buffer-mode))

    (cond ((not transcoder)
           (core:open-file-input-port filename file-options buffer-mode))

          ((transcoder? transcoder)
           (core:transcoded-port (core:open-file-input-port filename file-options buffer-mode)
                                 (transcoder-codec transcoder)
                                 (transcoder-eol-style transcoder)
                                 (transcoder-handling-mode transcoder)))

          (else 
           (error 'open-file-input-port "invalid transcoder in arg4" transcoder)))))


(define (open-file-output-port filename . opts)
  (let-optional opts ((file-options '())
                      (buffer-mode 'block)
                      (transcoder #f))

    (or (and (list? file-options) (for-all symbol? file-options))
        (error 'open-file-output-port "invalid file-options in arg2" file-options))
    (or (symbol? buffer-mode)
        (error 'open-file-output-port "invalid buffer-mode in arg3" buffer-mode))

    (cond ((not transcoder)
           (core:open-file-output-port filename file-options buffer-mode))

          ((transcoder? transcoder)
           (core:transcoded-port (core:open-file-output-port filename file-options buffer-mode)
                                 (transcoder-codec transcoder)
                                 (transcoder-eol-style transcoder)
                                 (transcoder-handling-mode transcoder)))

          (else 
           (error 'open-file-output-port "invalid transcoder in arg4" transcoder)))))


(define (open-file-input/output-port filename . opts)
  (let-optional opts ((file-options '())
                      (buffer-mode 'block)
                      (transcoder #f))

    (or (and (list? file-options) (for-all symbol? file-options))
        (error 'open-file-input/output-port "invalid file-options in arg2" file-options))
    (or (symbol? buffer-mode)
        (error 'open-file-input/output-port "invalid buffer-mode in arg3" buffer-mode))

    (cond ((not transcoder)
           (core:open-file-input/output-port filename file-options buffer-mode))

          ((transcoder? transcoder)
           (core:transcoded-port (core:open-file-input/output-port filename file-options buffer-mode)
                                 (transcoder-codec transcoder)
                                 (transcoder-eol-style transcoder)
                                 (transcoder-handling-mode transcoder)))

          (else 
           (error 'open-file-input/output-port "invalid transcoder in arg4" transcoder)))))


(define (open-bytevector-input-port bv . opts)
  (let-optional opts ((transcoder #f))
    (cond ((not transcoder)
           (core:open-bytevector-input-port bv))

          ((transcoder? transcoder)
           (core:transcoded-port (core:open-bytevector-input-port bv)
                                 (transcoder-codec transcoder)
                                 (transcoder-eol-style transcoder)
                                 (transcoder-handling-mode transcoder)))

          (else 
           (error 'open-bytevector-input-port "invalid transcoder in arg2" transcoder)))))


(define (open-bytevector-output-port . opts)
  (let-optional opts ((transcoder #f))
    (cond ((not transcoder)
           (core:open-bytevector-output-port))

          ((transcoder? transcoder)
           (core:transcoded-port (core:open-bytevector-output-port)
                                 (transcoder-codec transcoder)
                                 (transcoder-eol-style transcoder)
                                 (transcoder-handling-mode transcoder)))

          (else 
           (error 'open-bytevector-output-port "invalid transcoder in arg2" transcoder)))))


(define (get-bytevector-n port count)
  (or (and (input-port? port) (binary-port? port))
      (error 'get-bytevector-n "invalid binary input port in arg1" port))
  (or (and (exact? count) (integer? count) (<= 0 count))
      (error 'get-bytevector-n "invalid exact non-negative integer in arg2" count))
      
  (let* ((vec (make-bytevector count))
         (num (get-bytevector-n! port vec 0 count)))
    (cond ((eof-object? num) num)
          ((= num count) vec)
          (else
           (let ((vec1 (make-bytevector num)))
             (bytevector-copy! vec 0 vec1 0 num)
             vec1)))))


(define (get-bytevector-some port)
  (or (and (input-port? port) (binary-port? port))
      (error 'get-bytevector-some "invalid binary input port in arg1" port))

  (get-bytevector-n port 512))


(define (get-bytevector-all port)
  (or (and (input-port? port) (binary-port? port))
      (error 'get-bytevector-all "invalid binary input port in arg1" port))

  (let ((out (core:open-bytevector-output-port))
        (vec (make-bytevector 512)))
    (let loop ((count (get-bytevector-n! port vec 0 512)))
      (cond ((eof-object? count)
             (let ((data (extract-bytevector-port-data out)))
               (if (zero? (bytevector-length data))
                   count
                   data)))

            (else
             (put-bytevector out vec 0 count)
             (loop (get-bytevector-n! port vec 0 512)))))))


(define (get-string-n port count)
  (or (and (input-port? port) (textual-port? port))
      (error 'get-string-n "invalid textual input port in arg1" port))
  (or (and (exact? count) (integer? count) (<= 0 count))
      (error 'get-string-n "invalid exact non-negative integer in arg2" count))
      
  (let* ((str (make-string count))
         (num (get-string-n! port str 0 count)))
    (cond ((eof-object? num) num)
          ((= num count) str)
          (else
           (substring str 0 num)))))

(define (get-string-all port)
  (or (and (input-port? port) (textual-port? port))
      (error 'get-string-all "invalid textual input port in arg1" port))

  (let ((out (open-string-output-port))
        (str (make-string 512)))
    (let loop ((count (get-string-n! port str 0 512)))
      (cond ((eof-object? count)
             (let ((data (extract-string-port-data out)))
               (if (zero? (string-length data))
                   count
                   data)))

            (else
             (put-string out str 0 count)
             (loop (get-string-n! port str 0 512)))))))



)

;;; End of code