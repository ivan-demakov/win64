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
;;; Creation date: Thu Jan 28 21:35:01 2010
;;; Last Update:   Sun Sep 19 19:47:55 2010
;;;
;;;

(library (ksi list)
         (export has-duplicates? remove-duplicates
                 find for-all exists filter partition iota

                 ;; reexport from (ksi core list)
                 pair? null? list?
                 list cons xcons cons*
                 car cdr
                 caar cadr cdar cddr
                 caaar caadr cadar caddr cdaar cdadr cddar cdddr
                 caaaar caaadr caadar caaddr cadaar cadadr caddar cadddr
                 cdaaar cdaadr cdadar cdaddr cddaar cddadr cdddar cddddr
                 list-head list-tail last-pair list-ref
                 set-car! set-cdr! list-set!
                 length append append! reverse reverse!
                 copy-list copy-tree make-list
                 map for-each fold-left fold-right
                 remp remq remv remove
                 memp memq memv member
                 acons
                 assp assq assv assoc
                 assp-ref assq-ref assv-ref assoc-ref
                 assp-set! assq-set! assv-set! assoc-set!
                 assp-rem! assq-rem! assv-rem! assoc-rem!)

         (import (ksi core syntax)
                 (only (ksi core base) apply)
                 (only (ksi core number) exact? integer? number? <= < + - *)
                 (only (ksi core eval) procedure-has-arity? error)
                 (ksi core list))


(define (has-duplicates? l)
  (or (list? l)
      (error 'has-duplicates? "invalid or improper list in arg1" l))
  (let loop ((l l))
    (cond [(null? l) #f]
          [(memv (car l) (cdr l)) #t]
          [else (loop (cdr l))])))


(define (remove-duplicates l)
  (or (list? l)
      (error 'remove-duplicates "invalid or improper list in arg1" l))
  (let loop ((l l) (res '()))
    (cond ((null? l) (reverse! res))
          ((memv (car l) res) (loop (cdr l) res))
          (else (loop (cdr l) (cons (car l) res))))))


(define (find pred l)
  (or (procedure-has-arity? pred 1)
      (error 'find "procedure in arg1 should have 1 argument" pred))
  (cond [(null? l) #f]
        [(pair? l) (if (pred (car l)) (car l) (find pred (cdr l)))]
        [else (error 'find "invalid or improper list in arg2" l)]))


(define (for-all pred first . rest)
  (or (null? first)
      (exists null? rest)
      (if (null? rest)
          (let every ((x (car first)) (xs (cdr first)))
            (if (null? xs)
                (pred x)
                (and (pred x) (every (car xs) (cdr xs)))))
          (let every ((x   (car first))
                      (xs  (cdr first))
                      (xr  (map car rest))
                      (xrs (map cdr rest)))
            (if (or (null? xs) (exists null? xrs))
                (apply pred x xr)
                (and (apply pred x xr)
                     (every (car xs)
                            (cdr xs)
                            (map car xrs)
                            (map cdr xrs))))))))


(define (exists pred first . rest)
    (and (pair? first)
	 (for-all pair? rest)
	 (if (null? rest)
	     (let any ((x (car first)) (xs (cdr first)))
		 (if (null? xs)
		     (pred x)
		     (or (pred x) (any (car xs) (cdr xs)))))
	     (let any ((x   (car first))
                       (xs  (cdr first))
                       (xr  (map car rest))
                       (xrs (map cdr rest)))
	       (if (or (null? xs) (exists null? xrs))
		   (apply pred x xr)
		   (or (apply pred x xr)
		       (any (car xs)
                            (cdr xs)
                            (map car xrs)
                            (map cdr xrs))))))))


(define (filter pred lst)
  (let loop ((lst lst))
    (cond [(null? lst) '()]
          [(pred (car lst)) (cons (car lst) (loop (cdr lst)))]
          [else (loop (cdr lst))])))


(define (partition pred lst)
  (let loop ((lst lst) (acc1 '()) (acc2 '()))
    (cond [(null? lst) (values (reverse! acc1) (reverse! acc2))]
          [(pred (car lst)) (loop (cdr lst) (cons (car lst) acc1) acc2)]
          [else (loop (cdr lst) acc1 (cons (car lst) acc2))])))


;;;
;;;  IOTA count [start step]  ==>  (start start+step ... start+(count-1)*step)
;;;
(define (iota count . maybe-start+step)
  (or (and (exact? count) (integer? count) (<= 0 count))
      (error 'iota "invalid exact non-negative integer in arg1" count))
  (let ((start 0) (step 1))
    (if (pair? maybe-start+step)
        (begin (set! start (car maybe-start+step))
               (if (pair? (cdr maybe-start+step))
                   (set! step (cadr maybe-start+step)))))
    (or (number? start)
        (error 'iota "invalid number in arg2" start))
    (or (number? step)
        (error 'iota "invalid number in arg3" step))
    (let loop ((count (- count 1)) (res '()))
      (if (< count 0)
          res
          (loop (- count 1) (cons (+ start (* count step)) res))))))

)

;;; End of code