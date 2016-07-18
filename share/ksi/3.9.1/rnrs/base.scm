;;;
;;; base.scm
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
;;; Creation date: Sun Feb 28 21:39:57 2010
;;; Last Update:   Fri Aug 13 14:45:19 2010
;;;
;;;

(library (rnrs base)
         (export define lambda and or if set! begin
                 quote quasiquote unquote unquote-splicing
                 cond else => case let let* letrec letrec*
                 ;let-values let*-values
                 ;define-syntax syntax-rules identifier-syntax
                 ;let-syntax letrec-syntax _ ...

                 boolean? boolean=? not eq? eqv? equal?

                 null? pair? list? cons list length append reverse
                 list-tail list-ref map for-each
                 car cdr
                 caar cadr cdar cddr
                 caaar caadr cadar caddr cdaar cdadr cddar cdddr
                 caaaar caaadr caadar caaddr cadaar cadadr caddar cadddr
                 cdaaar cdaadr cdadar cdaddr cddaar cddadr cdddar cddddr

                 number? complex? real? rational? integer?
                 = < > <= >= zero? positive? negative? odd? even?
                 rationalize real-valued? rational-valued? integer-valued?
                 finite? infinite? nan? inexact? exact? inexact exact
                 + - * max min abs numerator denominator gcd lcm
                 / div div0 mod mod0 div-and-mod div0-and-mod0
                 floor ceiling truncate round
                 exp log sin cos tan asin acos atan expt sqrt
                 exact-integer-sqrt
                 make-rectangular make-polar
                 real-part imag-part angle magnitude
                 number->string string->number

                 symbol? symbol=? symbol->string string->symbol

                 char? char=? char<? char>? char<=? char>=?
                 char->integer integer->char

                 string? string=? string<? string>? string<=? string>=?
                 make-string string string-length string-ref substring
                 string-append string->list list->string string-for-each
                 string-copy

                 vector?
                 make-vector vector vector-length vector-ref vector-set!
                 vector->list list->vector vector-fill!
                 vector-map vector-for-each

                 ;error assertion-violation assert

                 procedure? apply call-with-current-continuation call/cc
                 values call-with-values dynamic-wind)

         (import (ksi core syntax)
                 (only (ksi core list)
                       null? pair? list? cons list length append reverse
                       list-tail list-ref map for-each
                       car cdr
                       caar cadr cdar cddr
                       caaar caadr cadar caddr cdaar cdadr cddar cdddr
                       caaaar caaadr caadar caaddr cadaar cadadr caddar cadddr
                       cdaaar cdaadr cdadar cdaddr cddaar cddadr cdddar cddddr)


                 (only (ksi number)
                       number? complex? real? rational? integer?
                       = < > <= >= zero? positive? negative? odd? even?
                       real-valued? rational-valued? integer-valued?
                       finite? infinite? nan? inexact? exact? inexact exact
                       + - * max min abs numerator denominator gcd lcm
                       / div div0 mod mod0 div-and-mod div0-and-mod0
                       floor ceiling truncate round rationalize
                       real-valued? rational-valued? integer-valued?
                       exp log sin cos tan asin acos atan sqrt expt
                       exact-integer-sqrt 
                       make-rectangular make-polar
                       real-part imag-part angle magnitude
                       number->string string->number)

                 (only (ksi core char)
                       char? char=? char<? char>? char<=? char>=?
                       char->integer integer->char)

                 (only (ksi core string)
                       string? string=? string<? string>? string<=? string>=?
                       make-string string string-length string-ref substring
                       string-append string->list list->string string-for-each
                       string-copy symbol->string string->symbol)

                 (only (ksi core vector)
                       vector?
                       make-vector vector vector-length vector-ref vector-set!
                       vector->list list->vector vector-fill!
                       vector-map vector-for-each)

                 (only (ksi core base)
                       boolean? boolean=? not eq? eqv? equal? symbol? symbol=?

                       ;error assertion-violation assert

                       procedure? apply call-with-current-continuation call/cc
                       values call-with-values dynamic-wind)
                 )

)

;;; End of code