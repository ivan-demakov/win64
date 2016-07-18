;;;
;;; sort.scm
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
;;; Creation date: Wed Jan 13 16:21:01 2010
;;; Last Update:   Fri Aug 13 14:58:33 2010
;;;
;;;

(library (ksi sort)
         (export list-sort list-sort! vector-sort vector-sort!)

         (import (ksi core syntax)
                 (only (ksi core number) div > = -)
                 (only (ksi core vector) vector? vector-ref vector-set! vector->list list->vector)
                 (ksi core list))


(define (list-sort! less? l)
  (let sort ((n (length l)))
    (cond ((> n 2)
           (let ((k (div n 2)))
             (let ((a (sort k)) (b (sort (- n k))))
               (define (merge r a b)
                 (if (less? (car b) (car a))
                     (begin
                       (set-cdr! r b)
                       (if (null? (cdr b))
                           (set-cdr! b a)
                           (merge b a (cdr b))))
                     (begin
                       (set-cdr! r a)
                       (if (null? (cdr a))
                           (set-cdr! a b)
                           (merge a (cdr a) b)))))

               (if (less? (car b) (car a))
                   (begin (if (null? (cdr b))
                              (set-cdr! b a)
                              (merge b a (cdr b)))
                          b)
                   (begin (if (null? (cdr a))
                              (set-cdr! a b)
                              (merge a (cdr a) b))
                          a)))))

          ((= n 2)
           (let ((x (car l)) (y (cadr l)) (p l))
             (set! l (cddr l))
             (if (less? y x)
                 (begin
                   (set-car! p y)
                   (set-car! (cdr p) x)))
             (set-cdr! (cdr p) '())
             p))

          ((= n 1)
           (let ((p l))
             (set! l (cdr l))
             (set-cdr! p '())
             p))

          (else '()))))


(define (list-sort less? v)
  (list-sort! less? (copy-list v)))


(define (vector-sort less? v)
  (or (vector? v)
      (error 'vector-sort "invalid vector" v))
  (list->vector (list-sort! less? (vector->list v))))


(define (vector-sort! less? v)
  (or (vector? v)
      (error 'vector-sort! "invalid vector" v))
  (let loop ((l (list-sort! (vector->list v))) (i 0))
    (if (null? l)
        v
        (begin (vector-set! v i (car l))
               (loop (cdr l) (+ i 1))))))

)

;;; End of code