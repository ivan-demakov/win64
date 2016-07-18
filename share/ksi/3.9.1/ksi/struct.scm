;;;
;;; struct.scm
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
;;; Creation date: Wed Mar 10 23:58:32 2010
;;; Last Update:   Sun Sep 19 19:48:57 2010
;;;
;;;

(library (ksi struct)
         (export define-struct)

         (import (ksi core syntax)
                 (ksi core hashtables)
                 (only (ksi core base) symbol? apply symbol->keyword)
                 (only (ksi list) list list? pair? null? length car cdr cadr caddr for-all map)
                 (only (ksi core number) = number? number->string)
                 (only (ksi core string) string-append string->symbol symbol->string)
                 (only (ksi klos) <rtd> <record> make is-a? slot-ref slot-set!)
                 (only (ksi core eval) define-macro! error))



(define (syntax-error form msg)
  (error (car form) msg form))


(define (make-string-name . xs)
  (apply string-append
         (map (lambda (x)
                (cond ((symbol? x) (symbol->string x))
                      ((number? x) (number->string x))
                      ((pair? x) (if (null? (cdr x))
                                     (make-string-name (car x))
                                     (make-string-name (car x) "-" (cdr x))))
                      (else x)))
              xs)))


(define (make-symbol-name . xs)
  (string->symbol (apply make-string-name xs)))


(define get-struct-type
  (let ((table-of-types (make-eq-hashtable)))
    (lambda (name fields)
      (let* ((type-name (make-symbol-name "<" name "-" (length fields) "-" fields ">"))
	     (entry (hashtable-ref table-of-types type-name #f)))
	(if entry
	    entry
	    (let ((new-type (make <rtd> name: name
                                  supers: (list <record>)
                                  slots: (map (lambda (x) (list x initarg: (symbol->keyword x))) fields))))
	      (hashtable-set! table-of-types type-name new-type)
	      new-type))))))


;;;
;;; (define-struct name (field ...))
;;;
(define-macro! (define-struct form env)
  (or (and (list? form) (= (length form) 3))
      (syntax-error form "invalid struct syntax"))

  (let ((name (cadr form)) (fields (caddr form)))
    (or (symbol? name)
        (syntax-error form "invalid struct name"))
    (or (list? fields)
        (syntax-error form "invalid struct fields"))
    (or (for-all symbol? fields)
        (syntax-error form "invalid struct field name"))

    (let ((constructor (make-symbol-name "make-" name))
          (predicate (make-symbol-name name "?"))
          (accesors (map (lambda (x) (make-symbol-name name "-" x)) fields))
          (mutators (map (lambda (x) (make-symbol-name "set-" name "-" x "!")) fields)))
      `(,#'begin
          (,#'define (,constructor . args) (,#'apply ,#'make (,#'get-struct-type ',name ',fields) args))

          (,#'define (,predicate x) (,#'is-a? x (,#'get-struct-type ',name ',fields)))

          ,@(map (lambda (name field)
                   `(,#'define (,name x)
                      (,#'or (,predicate x)
                          (,#'error ',name "invalid record" x))
                      (,#'slot-ref x ',field)))
                 accesors fields)

          ,@(map (lambda (name field)
                   `(,#'define (,name x v)
                      (,#'or (,predicate x)
                          (,#'error ',name "invalid record" x))
                      (,#'slot-set! x ',field v)))
                 mutators fields))

      )))

)

;;; End of code