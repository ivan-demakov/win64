;;;
;;; getopt.scm
;;; command line options
;;;
;;; Copyright (C) 1998-2010, Ivan Demakov.
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
;;; Author:        Ivan Demakov <ksion@users.sourceforge.net>
;;; Creation date: Fri Sep 11 19:00:52 1998
;;; Last Update:   Fri Aug 13 14:51:13 2010
;;;
;;;

(library (ksi getopt)
         (export get-option parse-options)

         (import (ksi core syntax)
                 (ksi core base)
                 (ksi core list)
                 (ksi core string)
                 (ksi core number))


(define (get-option argv kw-opts kw-args)
  (cond
   ((null? argv)
    (values '() 'end-args '()))

   ((or (not (eqv? #\- (string-ref (car argv) 0))) (eqv? (string-length (car argv)) 1))
    (values (cdr argv) 'normal-arg (car argv)))

   ((eqv? #\- (string-ref (car argv) 1))
    (if (= (string-length (car argv)) 2)
        (values '() 'end-args (cdr argv))
        (let ((kw-arg-pos (string-index (car argv) #\=)))
          (if kw-arg-pos
              (let ((kw (string->keyword (substring (car argv) 2 kw-arg-pos))))
                (if (memq kw kw-args)
                    (values (cdr argv)
                            (string->keyword (substring (car argv) 2 kw-arg-pos))
                                     (substring (car argv) (+ kw-arg-pos 1) (string-length (car argv))))
                    (values (cdr argv) 'usage-error kw)))

              (let ((kw (string->keyword
                         (substring (car argv) 2 (string-length (car argv))))))
                (cond ((memq kw kw-opts)
                       (values (cdr argv) kw #f))
                      ((and (memq kw kw-args) (pair? (cdr argv)))
                       (values (cddr argv) kw (cadr argv)))
                      (else
                       (values (cdr argv) 'usage-error kw))))))))

   (else
    (let ((kw (string->keyword (substring (car argv) 1 2))) (rest-kw (substring (car argv) 2 (string-length (car argv)))))
      (cond
       ((memq kw kw-opts)
        (values (if (zero? (string-length rest-kw))
                    (cdr argv)
                    (cons (string-append "-" rest-kw) (cdr argv)))
                kw #f))

       ((memq kw kw-args)
        (if (zero? (string-length rest-kw))
            (if (null? (cdr argv))
                (values (cdr argv) 'usage-error kw)
                (values (cddr argv) kw (cadr argv)))
            (values (cdr argv) kw rest-kw)))

       (else (values (cdr argv) 'usage-error kw)))))))


(define (parse-options argv kw-opts kw-args do-opt)
  (letrec ((next-option (lambda (argv kw arg)
                          (let ((res (do-opt kw arg argv)))
                            (if res
                                (begin
                                  (if (list? res)
                                      (set! argv res))
                                  (call-with-values
                                      (lambda () (get-option argv kw-opts kw-args))
                                    next-option)))))))

    (call-with-values
        (lambda () (get-option argv kw-opts kw-args))
      next-option)))

)


;;; End of code
