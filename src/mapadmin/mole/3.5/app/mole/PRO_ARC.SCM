;;;
;;; Pro-arc.scm
;;; Draw primitive prototypes
;;;
;;; Copyright (C) 1997, A.Tarnovski
;;;
;;; Comments:
;;;
;;;

;;====================================================================
;; <Arc-Prototype>

(define-class <Arc> (<Rectangular>)
  ((delta #:type procedure-OR-integer?
          #:initform 0
          #:initarg #:delta
          #:getter 'delta-of)))

(define-method set-default! ((self <Arc>))
  (call-next-method)
  (set-property! (delta-of self) 900))

(define-method build (dc (self <Arc>) (main <Container>)(cont <Container>))
  #f)

(define-method automatic? ((self <Arc>))
  #t)

(define-method built? ((self <Arc>))
  (and
    (call-next-method)
    (get-property (delta-of self))))

(define-method pack ((self <Arc>))
  (and-let*
    ((delta (get-property (delta-of self)))
     (nm (call-next-method)))
    (cons (cons #:delta delta) nm)))
;;====================================================================
;; <Color-Arc>
;;
(define-class <Color-Arc>
  (<Arc> <Color> <Stroked>)())

(define-method draw (dc (self <Color-Arc>))
  (and
    (call-next-method)
    (built? self)
    (draw-arc dc
      (ellipse-coord-of self)
      (ellipse-size-of self)
      (get-property (angle-of self))
      (get-property (delta-of self)))))

(define-method unpack ((self <Color-Arc>) (src <List>))
  (and (or is-convertion? (eqv? (car src) #:Arc))
       (call-next-method)))

(define-method pack ((self <Color-Arc>))
  (cons #:Arc (call-next-method)))
;;====================================================================
;; <Pie-Slice>
;;
(define-class <Pie-Slice> (<Arc> <Filled>)())

(define-method draw (dc (self <Pie-Slice>))
  (and
    (call-next-method)
    (built? self)
    (begin
      (if (eq? (status-of self) #:process-status )
        (let*
          ((x0 (get-property (x-coord-of self)))
           (y0 (get-property (y-coord-of self)))
           (x1 (get-property (x-size-of self)))
           (y1 (get-property (y-size-of self)))
           (sa (deg->rad (get-property (angle-of self))))
           (fa (+ sa (deg->rad (get-property (delta-of self)))))
           (x2 (* x1 x1))
           (y2 (* y1 y1))
           (x2y2 (* x2 y2))
           (ss (sin sa))
           (cs (cos sa))
           (sf (sin fa))
           (cf (cos fa))
           (rs (if (zero? x2y2) 0 (sqrt (/ x2y2 (+ (* cs cs y2) (* ss ss x2))))))
           (rf (if (zero? x2y2) 0 (sqrt (/ x2y2 (+ (* cf cf y2) (* sf sf x2)))))))
          (set-line-style! dc 0 #:line-solid )
          (draw-arc dc
            (draw-ellipse-coord-of self)
            (draw-ellipse-size-of self)
            (get-property (angle-of self))
            (get-property (delta-of self)))
          (draw-line dc (cons x0 y0) (cons (+ x0 (* rs cs)) (- y0 (* rs ss))))
          (draw-line dc (cons x0 y0) (cons (+ x0 (* rf cf)) (- y0 (* rf sf)))))
        (begin
          (set-arc-mode! dc #:pie-slice)
          (fill-arc dc
            (draw-ellipse-coord-of self)
            (draw-ellipse-size-of self)
            (get-property (angle-of self))
            (get-property (delta-of self))))))))

(define-method unpack ((self <Pie-Slice>) (src <List>))
  (and (or is-convertion? (eqv? (car src) #:Pie))
       (call-next-method)))

(define-method pack ((self <Pie-Slice>))
  (cons #:Pie (call-next-method)))
;;====================================================================
;; <Chord>
;;
(define-class <Chord> (<Arc> <Filled>)())

(define-method draw (dc (self <Chord>))
  (and
    (call-next-method)
    (built? self)
    (begin
      (if #f;(eq? (status-of self) #:process-status )
        (let*
          ((x0 (get-property (x-coord-of self)))
           (y0 (get-property (y-coord-of self)))
           (x1 (get-property (x-size-of self)))
           (y1 (get-property (y-size-of self)))
           (sa (deg->rad (get-property (angle-of self))))
           (fa (+ sa (deg->rad (get-property (delta-of self)))))
           (x2 (* x1 x1))
           (y2 (* y1 y1))
           (x2y2 (* x2 y2))
           (ss (sin sa))
           (cs (cos sa))
           (sf (sin fa))
           (cf (cos fa))
           (rs (if (zero? x2y2) 0 (sqrt (/ x2y2 (+ (* cs cs y2) (* ss ss x2))))))
           (rf (if (zero? x2y2) 0 (sqrt (/ x2y2 (+ (* cf cf y2) (* sf sf x2)))))))
          (set-line-style! dc 0 #:line-solid )
          (draw-arc dc
            (draw-ellipse-coord-of self)
            (draw-ellipse-size-of self)
            (get-property (angle-of self))
            (get-property (delta-of self)))
          (draw-line dc (cons (+ x0 (* rf cf)) (- y0 (* rf sf))) (cons (+ x0 (* rs cs)) (- y0 (* rs ss)))))
      (begin
        (set-arc-mode! dc #:chord)
        (fill-arc dc
          (draw-ellipse-coord-of self)
          (draw-ellipse-size-of self)
          (get-property (angle-of self))
          (get-property (delta-of self))))))))

(define-method unpack ((self <Chord>) (src <List>))
  (and (or is-convertion? (eqv? (car src) #:Chord))
       (call-next-method)))

(define-method draw (dc (self <Chord>))
  (and
    (call-next-method)
    (built? self)
    (begin
      (if #f;(eq? (status-of self) #:process-status )
        (let*
          ((x0 (get-property (x-coord-of self)))
           (y0 (get-property (y-coord-of self)))
           (x1 (get-property (x-size-of self)))
           (y1 (get-property (y-size-of self)))
           (sa (deg->rad (get-property (angle-of self))))
           (fa (+ sa (deg->rad (get-property (delta-of self)))))
           (x2 (* x1 x1))
           (y2 (* y1 y1))
           (x2y2 (* x2 y2))
           (ss (sin sa))
           (cs (cos sa))
           (sf (sin fa))
           (cf (cos fa))
           (rs (if (zero? x2y2) 0 (sqrt (/ x2y2 (+ (* cs cs y2) (* ss ss x2))))))
           (rf (if (zero? x2y2) 0 (sqrt (/ x2y2 (+ (* cf cf y2) (* sf sf x2)))))))
          (set-line-style! dc 0 #:line-solid )
          (draw-arc dc
            (draw-ellipse-coord-of self)
            (draw-ellipse-size-of self)
            (get-property (angle-of self))
            (get-property (delta-of self)))
          (draw-line dc (cons (+ x0 (* rf cf)) (- y0 (* rf sf))) (cons (+ x0 (* rs cs)) (- y0 (* rs ss)))))
      (begin
        (set-arc-mode! dc #:chord)
        (fill-arc dc
          (draw-ellipse-coord-of self)
          (draw-ellipse-size-of self)
          (get-property (angle-of self))
          (get-property (delta-of self))))))))

(define-method pack ((self <Chord>))
  (cons #:Chord (call-next-method)))
;;====================================================================
;; <Outlined-Chord>
;;
(define-class <Outlined-Chord> (<Stroked> <Color> <Filled> <Arc>)())

(define-method draw (dc (self <Outlined-Chord>))
  (and
    (call-next-method)
    (built? self)
    (begin
      (if #f;(eq? (status-of self) #:process-status )
        (let*
          ((x0 (get-property (x-coord-of self)))
           (y0 (get-property (y-coord-of self)))
           (x1 (get-property (x-size-of self)))
           (y1 (get-property (y-size-of self)))
           (sa (deg->rad (get-property (angle-of self))))
           (fa (+ sa (deg->rad (get-property (delta-of self)))))
           (x2 (* x1 x1))
           (y2 (* y1 y1))
           (x2y2 (* x2 y2))
           (ss (sin sa))
           (cs (cos sa))
           (sf (sin fa))
           (cf (cos fa))
           (rs (if (zero? x2y2) 0 (sqrt (/ x2y2 (+ (* cs cs y2) (* ss ss x2))))))
           (rf (if (zero? x2y2) 0 (sqrt (/ x2y2 (+ (* cf cf y2) (* sf sf x2)))))))
          (set-line-style! dc 0 #:line-solid )
          (draw-arc dc
            (draw-ellipse-coord-of self)
            (draw-ellipse-size-of self)
            (get-property (angle-of self))
            (get-property (delta-of self)))
          (draw-line dc (cons (+ x0 (* rf cf)) (- y0 (* rf sf))) (cons (+ x0 (* rs cs)) (- y0 (* rs ss)))))
      (begin
        (set-arc-mode! dc #:chord)
        (fill-arc dc
          (draw-ellipse-coord-of self)
          (draw-ellipse-size-of self)
          (get-property (angle-of self))
          (get-property (delta-of self))))))))

(define-method unpack ((self <Outlined-Chord>) (src <List>))
  (and (or is-convertion? (eqv? (car src) #:Outlined-Chord))
       (call-next-method)))

(define-method pack ((self <Outlined-Chord>))
  (cons #:Outlined-Chord (call-next-method)))
;;====================================================================
;;; End of code
