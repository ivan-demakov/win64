;;;
;;; Pro-lns.scm
;;; Draw primitive prototypes
;;;
;;; Copyright (C) 1997, A.Tarnovski
;;;
;;; Comments:
;;;
;;;
;;====================================================================
;; <Color-Lines>
;;
(define-class <Color-Lines> (<Color> <Stroked> <Poly-Coord>)
  ())

(define-method build (dc (self <Color-Lines>) (main <Container>) (cont <Container>))
  #f)

(define-method automatic? ((self <Color-Lines>))
  #t)

(define-method dynamic? ((self <Color-Lines>))
  #t)

(define-method built? ((self <Color-Lines>))
  #t)

(define-method pack ((self <Color-Lines>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Lines nm)))

(define-method unpack ((self <Color-Lines>) (src <List>))
  (and (or is-convertion? (eqv? (car src) #:Lines))
       (call-next-method)))

(define-method set-default! ((self <Color-Lines>))
  (call-next-method)
  (set-property! (coord-list-of self) '()))

(define-method draw (dc (self <Color-Lines>))
  (and
    (call-next-method)
    (let loop ((l (scale-poly (get-property (coord-list-of self)))))
      (when (> (length l) 1)
        (draw-line dc (car l) (cadr l))
        (loop (cddr l))))))
;;====================================================================
;;; End of code
