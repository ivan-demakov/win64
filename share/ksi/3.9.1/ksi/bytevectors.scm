;;;
;;; bytevectors.scm
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
;;; Creation date: Wed Sep 22 16:58:50 2010
;;; Last Update:   Thu Sep 23 18:56:22 2010
;;;
;;;

(library (ksi bytevectors)
         (export endianness
                 bytevector->u8-list u8-list->bytevector
                 bytevector->uint-list bytevector->sint-list uint-list->bytevector sint-list->bytevector

                 bytevector-uint-ref bytevector-sint-ref bytevector-uint-set! bytevector-sint-set!
                 bytevector-u16-ref bytevector-s16-ref bytevector-u16-native-ref bytevector-s16-native-ref
                 bytevector-u16-set! bytevector-s16-set! bytevector-u16-native-set! bytevector-s16-native-set!
                 bytevector-u32-ref bytevector-s32-ref bytevector-u32-native-ref bytevector-s32-native-ref
                 bytevector-u32-set! bytevector-s32-set! bytevector-u32-native-set! bytevector-s32-native-set!
                 bytevector-u64-ref bytevector-s64-ref bytevector-u64-native-ref bytevector-s64-native-ref
                 bytevector-u64-set! bytevector-s64-set! bytevector-u64-native-set! bytevector-s64-native-set!

                 bytevector-ieee-single-ref bytevector-ieee-single-set!
                 bytevector-ieee-single-native-ref bytevector-ieee-single-native-set!
                 bytevector-ieee-double-ref bytevector-ieee-double-set!
                 bytevector-ieee-double-native-ref bytevector-ieee-double-native-set!

                 ; from (ksi core bytevector)
                 native-endianness
                 bytevector-u8-ref bytevector-s8-ref bytevector-u8-set! bytevector-s8-set!
                 bytevector? make-bytevector bytevector-length bytevector=?
                 bytevector-fill! bytevector-copy! bytevector-copy)

         (import (ksi core syntax)
                 (ksi core bytevector)
                 (only (ksi core list) null? pair? list? cons car cdr cadr cddr length)
                 (only (ksi core base) eq? symbol? void call-with-values)
                 (only (ksi core number) + - * = < <= >= > zero? negative? expt div-and-mod)
                 (only (ksi core eval) define-macro! error))


(define-macro! (endianness form env)
  (or (and (pair? form) (pair? (cdr form)) (symbol? (cadr form)) (null? (cddr form)))
      (error (car form) "invalid syntax" form))
  (let ((value (cadr form)))
    (if (eq? value 'native)
        `[,#'quote ,(native-endianness)]
        (begin (or (eq? value 'little)
                   (eq? value 'big)
                   (error (car form) "invalid endianness" value))
               `[,#'quote ,value]))))


(define (bytevector-uint-ref* bv index endien size function)
  (or (bytevector? bv)
      (error function "invalid bytevector in arg1" bv))
  (or (and (exact? index) (integer? index))
      (error function "invalid exact integer in arg2"))
  (or (and (exact? size) (integer? size) (positive? size))
      (error function "invalid positive exact integer in arg4"))
  (or (<= 0 index (- (bytevector-length bv) size))
      (error function "index or size out of range" index))

  (cond ((eq? endien (endianness big))
         (let ((end (+ index size)))
           (let loop ((i index) (res 0))
             (if (>= i end)
                 res
                 (loop (+ i 1) (+ (* 256 res) (bytevector-u8-ref bv i)))))))

        ((eq? endien (endianness little))
         (let loop ((i (+ index size -1)) (res 0))
           (if (< i index)
               res
               (loop (- i 1) (+ (* 256 res) (bytevector-u8-ref bv i))))))

        (else
         (error function "invalid endianness in arg3" endien))))


(define (bytevector-sint-ref* bv index endien size function)
  (or (bytevector? bv)
      (error function "invalid bytevector in arg1" bv))
  (or (and (exact? index) (integer? index))
      (error function "invalid exact integer in arg2"))
  (or (and (exact? size) (integer? size) (positive? size))
      (error function "invalid positive exact integer in arg4"))
  (or (<= 0 index (- (bytevector-length bv) size))
      (error function "index or size out of range" index))

  (cond ((eq? endien (endianness big))
         (if (> (bytevector-u8-ref bv index) 127)
             (- (bytevector-uint-ref bv index endien size) (expt 256 size))
             (bytevector-uint-ref* bv index endien size function)))

        ((eq? endien (endianness little))
         (if (> (bytevector-u8-ref bv (+ index size -1)) 127)
             (- (bytevector-uint-ref bv index endien size) (expt 256 size))
             (bytevector-uint-ref* bv index endien size function)))

        (else
         (error function "invalid endianness in arg3" endien))))


(define (bytevector-uint-set!* bv index val endien size function)
  (or (bytevector? bv)
      (error function "invalid bytevector in arg1" bv))
  (or (and (exact? index) (integer? index))
      (error function "invalid exact integer in arg2"))
  (or (and (exact? size) (integer? size) (positive? size))
      (error function "invalid positive exact integer in arg4"))
  (or (<= 0 index (- (bytevector-length bv) size))
      (error function "index or size out of range" index))

  (cond ((zero? val)
         (let ((end (+ index size)))
           (let loop ((i index))
             (if (< i end)
                 (begin
                   (bytevector-u8-set! bv i 0)
                   (loop (+ i 1)))))))

        ((< 0 val (expt 256 size))
         (cond ((eq? endien (endianness big))
                (let ((start (- (+ index size) 1)))
                  (let loop ((i start) (res val))
                    (if (>= i index)
                        (call-with-values (lambda () (div-and-mod res 256))
                          (lambda (d m)
                            (bytevector-u8-set! bv i m)
                            (loop (- i 1) d)))))))

               ((eq? endien (endianness little))
                (let ((end (+ index size)))
                  (let loop ((i index) (res val))
                    (if (< i end)
                        (call-with-values (lambda () (div-and-mod res 256))
                          (lambda (d m)
                            (bytevector-u8-set! bv i m)
                            (loop (+ i 1) d)))))))))

        (else
         (error function "value out of range in arg3" val))))


(define (bytevector-sint-set!* bv index val endien size function)
  (let* ((p-bound (expt 2 (- (* size 8) 1)))
         (n-bound (- (+ p-bound 1))))
    (if (< n-bound val p-bound)
        (if (> val 0)
            (bytevector-uint-set!* bv index val endien size function)
            (bytevector-uint-set!* bv index (+ val (expt 256 size)) endien size function))
        (error function "value out of range in arg3" val))))


(define (bytevector->u8-list bv)
  (or (bytevector? bv)
      (error 'bytevector->u8-list "invalid bytevector in arg1" bv))

  (let loop ((i (- (bytevector-length bv) 1)) (res '()))
    (if (negative? i)
        res
        (loop (- i 1) (cons (bytevector-u8-ref bv i) res)))))


(define (u8-list->bytevector lst)
  (or (list? lst)
      (error 'u8-list->bytevector "invalid list in arg1" bv))

  (let ((bv (make-bytevector (length lst))))
    (let loop ((i 0) (lst lst))
      (cond ((null? lst) bv)
            (else
             (bytevector-u8-set! bv i (car lst))
             (loop (+ i 1) (cdr lst)))))))


(define (bytevector->uint-list bv endien size)
  (or (bytevector? bv)
      (error 'bytevector->uint-list "invalid bytevector in arg1" bv))

  (let loop ((i (- (bytevector-length bv) size)) (res '()))
    (if (> i -1)
        (loop (- i size) (cons (bytevector-uint-ref* bv i endien size 'bytevector->uint-list) res))
        (if (= i (- size))
            res
            (error 'bytevector->uint-list "inappropriate element size in arg3" size)))))


(define (bytevector->sint-list bv endien size)
  (or (bytevector? bv)
      (error 'bytevector->sint-list "invalid bytevector in arg1" bv))

  (let loop ((i (- (bytevector-length bv) size)) (acc '()))
    (if (> i -1)
        (loop (- i size) (cons (bytevector-sint-ref* bv i endien size 'bytevector->sint-list) acc))
        (if (= i (- size))
            acc
            (error 'bytevector->sint-list "inappropriate element size in arg3" size)))))


(define (uint-list->bytevector lst endien size)
  (or (list? lst)
      (error 'uint-list->bytevector "invalid list in arg1" bv))

  (let ((bv (make-bytevector (* size (length lst)))))
    (let loop ((i 0) (lst lst))
      (cond ((null? lst) bv)
            (else
             (bytevector-uint-set!* bv i (car lst) endien size 'uint-list->bytevector)
             (loop (+ i size) (cdr lst)))))))


(define (sint-list->bytevector lst endien size)
  (or (list? lst)
      (error 'sint-list->bytevector "invalid list in arg1" bv))

  (let ((bv (make-bytevector (* size (length lst)))))
    (let loop ((i 0) (lst lst))
      (cond ((null? lst) bv)
            (else
             (bytevector-sint-set!* bv i (car lst) endien size 'sint-list->bytevector)
             (loop (+ i size) (cdr lst)))))))


(define (bytevector-uint-ref bv index endien size)
  (bytevector-uint-ref* bv index endien size 'bytevector-uint-ref))

(define (bytevector-sint-ref bv index endien size)
  (bytevector-sint-ref* bv index endien size 'bytevector-sint-ref))

(define (bytevector-uint-set! bv index val endien size)
  (bytevector-uint-set!* bv index endien size 'bytevector-uint-set!))

(define (bytevector-sint-set! bv index val endien size)
  (bytevector-sint-set!* bv index endien size 'bytevector-sint-set!))


(define (bytevector-u16-ref bv index endien)
  (bytevector-uint-ref* bv index endien 2 'bytevector-u16-ref))

(define (bytevector-s16-ref bv index endien)
  (bytevector-sint-ref* bv index endien 2 'bytevector-s16-ref))

(define (bytevector-u16-set! bv index val endien size)
  (bytevector-uint-set!* bv index endien 2 'bytevector-u16-set!))

(define (bytevector-s16-set! bv index val endien size)
  (bytevector-sint-set!* bv index endien 2 'bytevector-s16-set!))


(define (bytevector-u16-native-ref bv index endien)
  (bytevector-uint-ref* bv index (endianness native) 2 'bytevector-u16-native-ref))

(define (bytevector-s16-native-ref bv index endien)
  (bytevector-sint-ref* bv index (endianness native) 2 'bytevector-s16-native-ref))

(define (bytevector-u16-native-set! bv index val endien size)
  (bytevector-uint-set!* bv index (endianness native) 2 'bytevector-u16-native-set!))

(define (bytevector-s16-native-set! bv index val endien size)
  (bytevector-sint-set!* bv index (endianness native) 2 'bytevector-s16-native-set!))


(define (bytevector-u32-ref bv index endien)
  (bytevector-uint-ref* bv index endien 4 'bytevector-u32-ref))

(define (bytevector-s32-ref bv index endien)
  (bytevector-sint-ref* bv index endien 4 'bytevector-s32-ref))

(define (bytevector-u32-set! bv index val endien size)
  (bytevector-uint-set!* bv index endien 4 'bytevector-u32-set!))

(define (bytevector-s32-set! bv index val endien size)
  (bytevector-sint-set!* bv index endien 4 'bytevector-s32-set!))


(define (bytevector-u32-native-ref bv index endien)
  (bytevector-uint-ref* bv index (endianness native) 4 'bytevector-u32-native-ref))

(define (bytevector-s32-native-ref bv index endien)
  (bytevector-sint-ref* bv index (endianness native) 4 'bytevector-s32-native-ref))

(define (bytevector-u32-native-set! bv index val endien size)
  (bytevector-uint-set!* bv index (endianness native) 4 'bytevector-u32-native-set!))

(define (bytevector-s32-native-set! bv index val endien size)
  (bytevector-sint-set!* bv index (endianness native) 4 'bytevector-s32-native-set!))


(define (bytevector-u64-ref bv index endien)
  (bytevector-uint-ref* bv index endien 8 'bytevector-u64-ref))

(define (bytevector-s64-ref bv index endien)
  (bytevector-sint-ref* bv index endien 8 'bytevector-s64-ref))

(define (bytevector-u64-set! bv index val endien size)
  (bytevector-uint-set!* bv index endien 8 'bytevector-u64-set!))

(define (bytevector-s64-set! bv index val endien size)
  (bytevector-sint-set!* bv index endien 8 'bytevector-s64-set!))


(define (bytevector-u64-native-ref bv index endien)
  (bytevector-uint-ref* bv index (endianness native) 8 'bytevector-u64-native-ref))

(define (bytevector-s64-native-ref bv index endien)
  (bytevector-sint-ref* bv index (endianness native) 8 'bytevector-s64-native-ref))

(define (bytevector-u64-native-set! bv index val endien size)
  (bytevector-uint-set!* bv index (native-endianness) 8 'bytevector-u64-native-set!))

(define (bytevector-s64-native-set! bv index val endien size)
  (bytevector-sint-set!* bv index (endianness native) 8 'bytevector-s64-native-set!))


(define (bytevector-ieee-single-native-ref bv index)
  (bytevector-float-ref bv index))

(define (bytevector-ieee-single-ref bv index endian)
  (bytevector-float-ref bv index endian))

(define (bytevector-ieee-double-native-ref bv index)
  (bytevector-double-ref bv index))

(define (bytevector-ieee-double-ref bv index endian)
  (bytevector-double-ref bv index endian))

(define (bytevector-ieee-single-native-set! bv index x)
  (bytevector-float-set! bv x index))

(define (bytevector-ieee-single-set! bv index x endian)
  (bytevector-float-set! bv index x endian))

(define (bytevector-ieee-double-native-set! bv index x)
  (bytevector-double-set! bv index x))

(define (bytevector-ieee-double-set! bv index x endian)
  (bytevector-double-set! bv index x endian))

)

;;; End of code