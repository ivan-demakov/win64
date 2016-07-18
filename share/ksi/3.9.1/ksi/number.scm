;;;
;;; number.scm
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
;;; Creation date: Tue Mar  2 00:09:39 2010
;;; Last Update:   Wed Aug 25 13:40:32 2010
;;;
;;;


(library (ksi number)
         (export number? complex? real? rational? integer?
                 = < > <= >= zero? positive? negative? odd? even?
                 finite? infinite? nan? inexact? exact? inexact exact
                 + - * max min abs numerator denominator gcd lcm
                 / div div0 mod mod0 div-and-mod div0-and-mod0
                 floor ceiling truncate round
                 exp expt log sin cos tan asin acos atan sqrt
                 exact-integer-sqrt rationalize
                 real-valued? rational-valued? integer-valued?
                 make-rectangular make-polar
                 real-part imag-part angle magnitude
                 number->string string->number

                 ; non r6rs
                 exact-div div-and-mod* sinh cosh tanh asinh acosh atanh
                 sign quotient reminder modulo)

         (import (ksi core syntax)
                 (ksi core number)
                 (only (ksi core base) call-with-values values))


(define real-valued? real?)

(define rational-valued? rational?)

(define integer-valued? integer?)


(define (sign x)
  (or (real? x)
      (error 'sign "invalid real number in arg1" x))
  (cond ((zero? x) 0)
        ((positive? x) 1)
        (else -1)))


(define (quotient n1 n2)
  (define who 'quotient)
  (if (zero? n2)
      (error who "divide by zero" n2))
  (or (integer? n1)
      (error who "invalid integer number in arg1" n1))
  (or (integer? n2)
      (error who "invalid integer number in arg2" n2))

  (* (sign n1) (sign n2) (div (abs n1) (abs n2))))


(define (reminder n1 n2)
  (define who 'reminder)
  (if (zero? n2)
      (error who "divide by zero" n2))
  (or (integer? n1)
      (error who "invalid integer number in arg1" n1))
  (or (integer? n2)
      (error who "invalid integer number in arg2" n2))

  (* (sign n1) (mod (abs n1) (abs n2))))


(define (modulo n1 n2)
  (define who 'modulo)
  (if (zero? n2)
      (error who "divide by zero" n2))
  (or (integer? n1)
      (error who "invalid integer number in arg1" n1))
  (or (integer? n2)
      (error who "invalid integer number in arg2" n2))

  (* (sign n2) (mod (* (sign n2) (abs n1)) (abs n2))))


(define (div0-and-mod0 x y)
  (call-with-values (lambda () (div-and-mod* x y 'div0-and-mod0))
    (lambda (d m)
      (if (> y 0)
          (if (< m (/ y 2))
              (values d m)
              (values (+ d 1) (- m y)))
          (if (> m (/ y -2))
              (values (- d 1) (+ m y))
              (values d m))))))


(define (div0 x y)
  (call-with-values (lambda () (div-and-mod* x y 'div0))
    (lambda (d m)
      (if (> y 0)
          (if (< m (/ y 2))
              d
              (+ d 1))
          (if (> m (/ y -2))
              (- d 1)
              d)))))


(define (mod0 x y)
  (call-with-values (lambda () (div-and-mod* x y 'mod0))
    (lambda (d m)
      (if (> y 0)
          (if (< m (/ y 2))
              m
              (- m y))
          (if (> m (/ y -2))
              (+ m y)
              m)))))


(define (rationalize x e)
  (define who 'rationalize)
  (or (real? x)
      (error who "invalid real number in arg1" x))
  (or (real? e)
      (error who "invalid real number in arg2" e))

  (cond [(infinite? e) (if (infinite? x) +nan.0 0.0)]
        [(zero? x) x]
        [(= x e) (- x e)]
        [(negative? x) (- (rationalize (- x) e))]
        (else
         (let ((e (abs e)))
           (let loop ((bottom (- x e)) (top (+ x e)))
             (if (= bottom top)
                 bottom
                 (let ((x (ceiling bottom)))
                   (if (< x top)
                       x
                       (let ((a (- x 1)))
                         (+ a (/ 1 (loop (/ 1 (- top a)) (/ 1 (- bottom a))))))))))))))

)


;;; End of code