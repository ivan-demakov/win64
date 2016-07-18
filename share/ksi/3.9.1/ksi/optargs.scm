;;;
;;; optargs.scm
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
;;; Creation date: Thu Sep  2 20:31:35 2010
;;; Last Update:   Sun Sep 19 19:48:11 2010
;;;
;;;

(library (ksi optargs)
         (export let-optional)

         (import (ksi core syntax)
                 (only (ksi list) null? pair? list? list car cdr cadr cddr caddr cdddr map)
                 (only (ksi core base) gensym symbol?)
                 (only (ksi core eval) define-macro! error))


(define-macro! (let-optional form env)
  (or (and (list? form) (pair? (cdr form)) (pair? (cddr form)))
      (error (car form) "invalid syntax" form))
  (let ((args (cadr form))
        (binds (caddr form))
        (body (cdddr form)))
    (if (null? binds)
        `(,#'let () ,@body)
        (let ((sym (if (symbol? args) args (gensym 'args))))
          `(,#'let ((,sym ,args)
                    ,@(map (lambda (opt)
                             (cond ((symbol? opt)
                                    `(,opt #!void))
                                   ((and (pair? opt) (symbol? (car opt)) (pair? (cdr opt)) (null? (cddr opt)))
                                    opt)
                                   (else
                                    (error (car form) "invalid bindings" form))))
                           binds))
             ,@(map (lambda (opt)
                      (let ((var (if (pair? opt) (car opt) opt)))
                        `(,#'if (,#'pair? ,sym)
                                (,#'begin (,#'set! ,var (car ,sym))
                                          (,#'set! ,sym (cdr ,sym))))))
                    binds)
             ,@body)))))


)

;;; End of code