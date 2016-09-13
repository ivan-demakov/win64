;;;
;;; Pro-draw.scm
;;; Drawable primitive prototypes
;;;
;;; Copyright (C) 1997, A.Tarnovski
;;;
;;; Comments:
;;;
;;;
;;====================================================================
;; <Rectangular>
;;
(define-class <Rectangular> (<Coord>)
  ((test   #:type procedure?
           #:initform (lambda (f) #t)
           #:initarg #:valid-test
           #:getter 'valid-test-of)
   (x-size #:type procedure-OR-symbol-OR-integer?
           #:initform 1
           #:initarg #:x-size
           #:getter 'x-size-of)
   (y-size #:type procedure-OR-symbol-OR-integer?
           #:initform 1
           #:initarg #:y-size
           #:getter 'y-size-of)))

(define-method set-default! ((self <Rectangular>))
  (call-next-method)
  (set-property! (x-size-of self) 1)
  (set-property! (y-size-of self) 1))

(define-generic draw-origin-of ((self <Rectangular>))
  (let*
    ((l (get-property (x-coord-of self)))
     (t (get-property (y-coord-of self)))
     (w/2 (/ (get-property (x-size-of self)) 2))
     (h/2 (/ (get-property (y-size-of self)) 2))
     (a (get-property (angle-of self)))
     (c0 (shift (cons l t) (turn (cons w/2 h/2) a)))
     (c1 (scale-coord c0)))
    (shift c1 (turn (scale-size (cons (- w/2) (- h/2))) a))))

(define-generic ellipse-size-of ((self <Rectangular>))
  (cons (* 2 (get-property (x-size-of self)))
        (* 2 (get-property (y-size-of self)))))

(define-generic draw-ellipse-size-of ((self <Rectangular>))
  (scale-size (ellipse-size-of self)))

(define-generic ring-size-of ((self <Rectangular>))
  (* 2 (max (abs (get-property (x-size-of self)))
            (abs (get-property (y-size-of self))))))

(define-generic draw-ring-size-of ((self <Rectangular>))
  (scale-size (ring-size-of self)))

(define-generic ellipse-coord-of ((self <Rectangular>))
  (cons (- (get-property (x-coord-of self))
           (get-property (x-size-of self)))
        (- (get-property (y-coord-of self))
           (get-property (y-size-of self)))))

(define-generic draw-ellipse-coord-of ((self <Rectangular>))
  (define (hp x) (/ x 2))
  (map - (scale-coord (origin-of self)) (map hp (draw-ellipse-size-of self))))

(define-generic ring-coord-of ((self <Rectangular>))
  (let
    ((s (max (abs (get-property (x-size-of self)))
             (abs (get-property (y-size-of self))))))
    (cons (- (get-property (x-coord-of self)) s)
          (- (get-property (y-coord-of self)) s))))

(define-generic draw-ring-coord-of ((self <Rectangular>))
  (define (hp x) (/ x 2))
  (map - (scale-coord (origin-of self)) (map hp (draw-ring-size-of self))))

(define-generic size-built? ((self <Rectangular>))
  (or (automatic? self)
      (and (get-property (x-size-of self))
           (get-property (y-size-of self)))))

(define-generic size-of ((self <Rectangular>))
  (cons (get-property (x-size-of self))
        (get-property (y-size-of self))))

(define-generic draw-size-of ((self <Rectangular>))
  (scale-size (size-of self)))

(define-generic set-size! ((self <Rectangular>) x y)
  (set-property! (x-size-of self) x)
  (set-property! (y-size-of self) y))

(define-generic crush-size! ((self <Rectangular>))
  (set-property! (x-size-of self))
  (set-property! (y-size-of self)))

(define-generic mark-angle-defined! ((self <Rectangular>) (cont <Container>))
  (let
    ((angle (get-property (angle-of self)))
     (lob (select-built cont '())))
    (set-property! (angle-of self))
    (mark-rebuilding! cont lob)
    (set-property! (angle-of self) angle)))

(define-generic mark-size-defined! ((self <Rectangular>) (cont <Container>))
  (let
    ((size (size-of self))
     (lob (select-built cont '())))
    (crush-size! self)
    (mark-rebuilding! cont lob)
    (set-size! self (car size) (cdr size))))

(define-method built? ((self <Rectangular>))
  (and (call-next-method)
       (size-built? self)
       (get-property (angle-of self))))

(define-method automatic? ((self <Rectangular>))
  (and (call-next-method)
       (not (or (symbol? (x-size-of self))
                (symbol? (y-size-of self))
                (symbol? (angle-of self))))))

(define-generic size-def-of? ((self <Rectangular>))
  (or (symbol? (x-size-of self)) (symbol? (y-size-of self))))

(define-generic coord-def-of? ((self <Rectangular>))
  (or (symbol? (x-coord-of self)) (symbol? (y-coord-of self))))

(define-method build (dc (self <Rectangular>) (main <Container>)(cont <Container>))
  (and
    (or (delayed? self)
        (and (not (built? self)) (built-properties? (dependence-of self))))
    (let*
      ((angle-set? #f)
       (size-set? (or (and (size-def-of? self)
                           (coord-built? self))
                      (not (coord-def-of? self))))
       (mx 0)
       (my 0)

       (post-fun
         (lambda (dc)
           (unselect-mode dc)))

        (set-offset!
          (lambda (dc event)
            (when angle-set?
              (set! angle-set? #f)
              (draw-building dc main)
              (move-to self (get-property (x-coord-of self))
                            (get-property (y-coord-of self)))
              (draw-building dc main)
              (set! mx (- (event-x event) (get-property (x-coord-of self))))
              (set! my (- (event-y event) (get-property (y-coord-of self)))))))

        (set-new-size!
          (lambda (nx ny)
            (let*
              ((a (get-property (angle-of self)))
               (ang (or a (get-reisshina-angle)))
               (x (- nx (get-property (x-coord-of self))))
               (y (- ny (get-property (y-coord-of self)))))
              (if (zero? ang)
                (set-size! self x y)
                (let*
                  ((rad (deg->rad ang))
                   (si (sin rad))
                   (co (cos rad)))
                  (set-size! self
                    (inexact->exact (- (* x co) (* y si)))
                    (inexact->exact (+ (* y co) (* x si)))))))))

        (on-char
          (lambda (dc event)
            (case (event-key event)

              ( (#\+ #\-)
                (if (memq 'shift (event-state event))
                  (begin
                    (draw-building dc main)
                    (set-property! (angle-of self)
                      (std-angle ((if (eq? (event-key event) #\+) + -)
                      (get-property (angle-of self))(car (a-grid-of self)))))
                    ((valid-test-of self) #f)
                    (draw-building dc main))))

              ( (#\esc)
                (breake-building dc main))

              ( (#\bs)
                (if (or
                      (memq 'shift (event-state event))
                      (not size-set?)
                      (not (coord-def-of? self)))
                  (if (to-previous-item cont)
                    (begin
                      (draw-building dc main)
                      (mark-angle-defined! self main)
                      (mark-size-defined! self main)
                      (mark-coord-defined! self main)
                      (draw-rebuilt dc main)
                      (crush-size! self)
                      (crush-coord! self)
                      (set-property! (angle-of self))
                      (delay-item (current-of cont) cont)
                      (unselect-mode dc)))
                  (if size-set?
                    (begin
                      (draw-building dc main)
                      (mark-angle-defined! self main)
                      (mark-size-defined! self main)
                      (mark-coord-defined! self main)
                      (draw-rebuilt dc main)
                      (crush-size! self)
                      (set! mx 0)
                      (set! my 0)
                      (set! size-set?  #f)
                      (move-to self (get-curr-x) (get-curr-y))
                      (draw-building dc main)))))

              ( (#\del)
                (if (to-previous-item cont)
                  (begin
                    (draw-building dc main)
                    (delay-item self cont)
                    (draw dc self)
                    (delay-item (current-of cont) cont)
                    (unselect-mode dc)))))))

        (on-expose
          (lambda (dc event)
            (draw dc main)))

        (on-motion
          (lambda (dc event)
            (if (memq 'shift (event-state event))
              (set! angle-set? #t)
              (set-offset! dc event))
            (if (and angle-set? (size-built? self))
              (or
                (and-let*
                  ((sx0 (get-property (x-size-of self)))
                   ((not (zero? sx0)))
                   (sy0 (get-property (y-size-of self)))
                   ((not (zero? sy0)))
                   (x0 (get-property (x-coord-of self)))
                   (y0 (get-property (y-coord-of self))))
                  (let*
                    ((sx1 (- (event-x event) x0))
                     (sy1 (- (event-y event) y0))
                     (deg (a-kvant self (rad->deg (+ (atan (- sy1) sx1) (atan sy0 sx0)))))
                     (s0 (inexact->exact(magnitude (make-rectangular sx0 sy0))))
                     (s1 (inexact->exact(magnitude (make-rectangular sx1 sy1))))
                     (angle? (not (eq? deg (get-property (angle-of self)))))
                     (size? (not (eq? s0 s1))))
                    (if (or angle? size?)
                      (begin
                        (draw-building dc main)
                        (if angle?
                          (set-property! (angle-of self) deg))
                        (if size?
                          (set-size! self
                                     (inexact->exact( / (* sx0 s1) s0))
                                     (inexact->exact( / (* sy0 s1) s0))))
                        ((valid-test-of self) #f)
                        (draw-building dc main)))))
                (set! angle-set? #f))
              (begin
                (draw-building dc main)
                (if size-set?
                  (set-new-size! (event-x event) (event-y event))
                  (move-to self (- (event-x event) mx) (- (event-y event) my)))
                ((valid-test-of self) #f)
                (draw-building dc main)))))

        (valid-rect?
          (lambda (dc)
            (let
              ((m ((valid-test-of self) #f)))
              (if (string? m)
                (begin
                  (draw-building dc main)
                  (tk-message-box m "Œ¯Ë·Í‡!")
                  (draw-building dc main)
                  #f)
                m))))

        (on-button-press
          (lambda (dc event)
            (if (not (or (memq 'shift (event-state event))
                         (memq 'control (event-state event))))
              (case (event-button event)
                ((1)
                 (set-offset! dc event)
                 (draw-building dc main)
                 (if size-set?
                   (if (valid-rect? dc)
                     (unselect-mode dc)
                     (draw-building dc main))
                   (begin
                     (move-to self (- (event-x event) mx) (- (event-y event) my))
                     (cond
;;                       ((test-point-object (origin-of self) #t)
;;                        (draw-building dc main))
                       ((size-built? self)
                        (if (valid-rect? dc)
                          (unselect-mode dc)
                          (draw-building dc main)))
                       (else
                        (set! size-set?  #t)
                        (set-new-size! (event-x event) (event-y event))
                        (draw-building dc main))))))
                ((3)
                 (breake-building dc main)))))))

      (draw-building dc main)
      (set-reisshina-active #f)

      (if (delayed? self)
        (let
          ((coord-def? (coord-def-of? self))
           (size-def?  (size-def-of? self)))
          (if (symbol? (angle-of self)) (mark-angle-defined! self main))
          (reenter self cont)
          (cond
            ((and size-def? (or (size-built? self) (not coord-def?)))
             (mark-size-defined! self main)
             (draw-rebuilt dc main)
             (set-new-size! (get-curr-x) (get-curr-y))
             (set! size-set?  #t))
            ((and coord-def? (or (not (size-built? self)) (not size-def?)))
             (mark-coord-defined! self main)
             (draw-rebuilt dc main)
             (set-cursor! (origin-of self)))))
        (begin
          (if size-set?
            (set-new-size! (get-curr-x) (get-curr-y))
            (set-cursor! (origin-of self)))
          (if (not (get-property (angle-of self)))
            (set-property! (angle-of self) (get-reisshina-angle)))))

      (draw-building dc main)

      ((valid-test-of self) #t)

      (select-mode
        (list
          (cons 'post           post-fun)
          (cons 'button-press   on-button-press)
          (cons 'motion         on-motion)
          (cons 'expose         on-expose)
          (cons 'key-press      on-char))
        dc))))

(define-generic turn-of ((self <Rectangular>))
  (let*
    ( (an (get-property (angle-of self)))
      (a (deg->rad an))
      (si (sin a))
      (co (cos a))
      (xs (get-property (x-size-of self)))
      (ys (get-property (y-size-of self)))
      (p0 (origin-of self))
      (s0 (turn (cons (/ xs 2) (/ ys 2)) an))
      (c0 (shift p0 s0))
      (t (- obj-scale-factor))
      (s1 (map * (cons t t) s0))
      (lttp (shift c0 s1))
      (xco (inexact->exact (* xs obj-scale-factor co)))
      (xsi (inexact->exact (* xs obj-scale-factor si)))
      (yco (inexact->exact (* ys obj-scale-factor co)))
      (ysi (inexact->exact (* ys obj-scale-factor si))))
    (list
      lttp
      (cons xco (- xsi))
      (cons ysi yco)
      (cons (- xco) xsi))))

(define-method pack ((self <Rectangular>))
  (and-let*
    ((x (get-property (x-size-of self)))
     (y (get-property (y-size-of self)))
     (a (get-property (angle-of self)))
     (p (origin-of self))
     (nm (call-next-method)))
    (assv-set! nm  #:coord (vector (car p) (cdr p)))
    (cons (cons #:size (vector x y a)) nm)))

(define-method unpack ((self <Rectangular>) (src <List>))
  (and-let*
    ((data (assv-ref src #:size))
     ((vector? data))
     ((= 3 (vector-length data)))
     ((number? (vector-ref data 0)))
     ((number? (vector-ref data 1)))
     ((number? (vector-ref data 2))))
    (set-property! (x-size-of self) (vector-ref data 0))
    (set-property! (y-size-of self) (vector-ref data 1))
    (set-property! (angle-of self)  (vector-ref data 2)))
    (call-next-method))
;;====================================================================
(define-class <Color-Rectangle> (<Color> <Stroked> <Rectangular>)
  ())

(define-method draw (dc (self <Color-Rectangle>))
  (and
    (call-next-method)
    (built? self)
    (let
      ((ang (get-property (angle-of self))))
      (if (or (not ang) (zero? ang))
        (draw-rectangle dc (draw-origin-of self) (draw-size-of self))
        (draw-lines dc (turn-of self) #:previous #t)))))

(define-method unpack ((self <Color-Rectangle>) (src <List>))
  (and (or is-convertion?
           (eqv? (car src) #:Rectangle))
       (call-next-method)))

(define-method pack ((self <Color-Rectangle>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Rectangle nm)))
;;====================================================================
(define-class <Bar> (<Rectangular> <Filled>)
  ())

(define-method draw (dc (self <Bar>))
  (and
    (call-next-method)
    (built? self)
    (let
      ((ang (get-property (angle-of self))))
      (if (eq? (status-of self) #:process-status )
        (begin
          (set-line-style! dc 0 #:line-solid)
          (if (or (not ang) (zero? ang))
            (draw-rectangle dc (draw-origin-of self) (draw-size-of self))
            (draw-lines dc (turn-of self) #:previous #t)))
        (if (or (not ang) (zero? ang))
          (fill-rectangle dc (draw-origin-of self) (draw-size-of self))
          (fill-polygon dc (turn-of self) #:previous))))))

(define-method unpack ((self <Bar>) (src <List>))
  (and (or is-convertion?
           (eqv? (car src) #:Bar)
           (eqv? (car src) #:Outlined-Bar))
       (call-next-method)))

(define-method pack ((self <Bar>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Bar nm)))
;;====================================================================
(define-class <Outlined-Bar> (<Stroked> <Color> <Bar>)
  ())

(define-method draw (dc (self <Outlined-Bar>))
  (and
    (call-next-method)
    (if (not (eq? (status-of self) #:process-status))
      (let
        ((ang (get-property (angle-of self))))
        (if (or (not ang) (zero? ang))
          (draw-rectangle dc (draw-origin-of self) (draw-size-of self))
          (draw-lines dc (turn-of self) #:previous #t))))))

(define-method unpack ((self <Outlined-Bar>) (src <List>))
  (and-let*
    (((or is-convertion?
          (eqv? (car src) #:Outlined-Bar)))
     ((or is-convertion? (set-car! src #:Bar)))
     (r (call-next-method))
     ((or is-convertion? (set-car! src #:Outlined-Bar))))
    r))

(define-method pack ((self <Outlined-Bar>))
  (and-let*
    ((nml (call-next-method)))
    (cons #:Outlined-Bar (cons (car nml) (cons (cadr nml) (cdddr nml))))))
;;====================================================================
;; <Ellipse>
;;
(define-class <Ellipse> (<Rectangular> <Stroked>)
  ())

(define-method draw (dc (self <Ellipse>))
  (and
    (call-next-method)
    (built? self)
    (draw-ellipse dc (draw-ellipse-coord-of self) (draw-ellipse-size-of self))))
;;====================================================================
;; <Ring>
;;
(define-class <Ring> (<Rectangular> <Stroked>)
  ())

(define-method draw (dc (self <Ring>))
  (and
    (call-next-method)
    (built? self)
    (draw-ellipse dc (draw-ring-coord-of self) (draw-ring-size-of self))))
;;====================================================================
(define-class <Color-Ellipse> (<Ellipse> <Color>)
  ())

(define-method unpack ((self <Color-Ellipse>) (src <List>))
  (and (or is-convertion?
           (eqv? (car src) #:Ellipse))
       (call-next-method)))

(define-method pack ((self <Color-Ellipse>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Ellipse nm)))
;;====================================================================
(define-class <Color-Ring> (<Ring> <Color>)
  ())

(define-method unpack ((self <Color-Ring>) (src <List>))
  (and (or is-convertion?
           (eqv? (car src) #:Ring))
       (call-next-method)))

(define-method pack ((self <Color-Ring>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Ring nm)))
;;====================================================================
(define-class <Oval> (<Rectangular> <Filled>)
  ())

(define-method draw (dc (self <Oval>))
  (and
    (call-next-method)
    (built? self)
    (if (eq? (status-of self) #:process-status )
      (begin
        (set-line-style! dc 0 #:line-solid)
        (draw-ellipse dc (draw-ellipse-coord-of self) (draw-ellipse-size-of self)))
      (fill-ellipse dc (ellipse-coord-of self) (draw-ellipse-size-of self)))))

(define-method unpack ((self <Oval>) (src <List>))
  (and (or is-convertion?
           (eqv? (car src) #:Oval)
           (eqv? (car src) #:Outlined-Oval))
       (call-next-method)))

(define-method pack ((self <Oval>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Oval nm)))
;;====================================================================
(define-class <Circle> (<Rectangular> <Filled>)
  ())

(define-method draw (dc (self <Circle>))
  (and
    (call-next-method)
    (built? self)
    (if (eq? (status-of self) #:process-status )
      (begin
        (set-line-style! dc 0 #:line-solid)
        (draw-ellipse dc (draw-ring-coord-of self) (draw-ring-size-of self)))
      (fill-ellipse dc (draw-ring-coord-of self) (draw-ring-size-of self)))))

(define-method unpack ((self <Circle>) (src <List>))
  (and (or is-convertion?
           (eqv? (car src) #:Circle)
           (eqv? (car src) #:Outlined-Circle))
       (call-next-method)))

(define-method pack ((self <Circle>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Circle nm)))
;;====================================================================
(define-class <Outlined-Oval> (<Stroked> <Color> <Oval>)
  ())

(define-method draw (dc (self <Outlined-Oval>))
  (and
    (call-next-method)
    (if (not (eq? (status-of self) #:process-status))
      (draw-ellipse dc (draw-ellipse-coord-of self) (draw-ellipse-size-of self)))))

(define-method unpack ((self <Outlined-Oval>) (src <List>))
  (and-let*
    (((or is-convertion?
          (eqv? (car src) #:Outlined-Oval)))
     ((or is-convertion? (set-car! src #:Oval)))
     (r (call-next-method))
     ((or is-convertion? (set-car! src #:Outlined-Oval))))
    r))

(define-method pack ((self <Outlined-Oval>))
  (and-let*
    ((nml (call-next-method)))
    (cons #:Outlined-Oval (cons (car nml) (cons (cadr nml) (cdddr nml))))))
;;====================================================================
(define-class <Outlined-Circle> (<Stroked> <Color> <Circle>)
  ())

(define-method draw (dc (self <Outlined-Circle>))
  (and
    (call-next-method)
    (if (not (eq? (status-of self) #:process-status))
      (draw-ellipse dc (draw-ring-coord-of self) (draw-ring-size-of self)))))

(define-method unpack ((self <Outlined-Circle>) (src <List>))
  (and-let*
    (((or is-convertion?
          (eqv? (car src) #:Outlined-Circle)))
     ((or is-convertion? (set-car! src #:Circle)))
     (r (call-next-method))
     ((or is-convertion? (set-car! src #:Outlined-Circle))))
    r))

(define-method pack ((self <Outlined-Circle>))
  (and-let*
    ((nml (call-next-method)))
    (cons #:Outlined-Circle (cons (car nml) (cons (cadr nml) (cdddr nml))))))
;;====================================================================
;; <Poly-Coord>
;;
(define-class <Poly-Coord> (<Prototype>)
  ((test       #:type (lambda (x) (procedure? x))
               #:initform (lambda (pl n) #t)
               #:initarg #:valid-test
               #:getter 'valid-test-of)
  (at-final    #:type (lambda(x)(procedure? x))
               #:initform (lambda (x) #t)
               #:initarg #:at-final
               #:getter 'at-final-of)
   (edit-point #:type (lambda(x)(integer? x))
               #:initform 0
               #:initarg #:edit-point
               #:getter 'edit-point-of
               #:setter 'set-edit-point!)
   (empty      #:type <Boolean>
               #:initform #f
               #:initarg #:empty
               #:getter 'empty?)
   (shaped     #:type procedure-OR-boolean?
               #:initform #f
               #:initarg #:shaped
               #:getter 'shaped-of)
   (coord-num  #:type procedure-OR-integer?
               #:initform -10000
               #:initarg #:coord-num
               #:getter 'coord-num-of)
   (coord-list #:type procedure-OR-symbol-OR-list?
               #:initform '()
               #:initarg #:coord-list
               #:getter 'coord-list-of)))

(define-method set-default! ((self <Poly-Coord>))
  (call-next-method)
  (let
    ((cn (get-property (coord-num-of self))))
    (if (<= cn 0)
      (set-property! (coord-list-of self) '((0 . 0)(1 . 1)))
      (do ((x 0 (+ x 1)) (y 0 (+ y 1)) (n cn (- n 1)) (l '()))
        ((zero? n) (set-property! (coord-list-of self) (reverse l)))
        (set! l (cons (cons x y) l))))))

(define-method origin-of ((self <Poly-Coord>))
  (car (get-property (coord-list-of self))))

(define-generic coord-list-built? ((self <Poly-Coord>))
  (and-let*
    ((l (get-property (coord-list-of self))))
    (>= (length l) 1)))

(define-generic shaped? ((self <Poly-Coord>))
  (and (shaped-of self) (not edition?)))

(define-method automatic? ((self <Poly-Coord>))
  (and (call-next-method)
       (not (or (symbol? (coord-list-of self))
                (symbol? (coord-num-of self))))))

(define-generic mark-poly-defined! ((self <Poly-Coord>) (main <Container>))
  (let
    ((cl (get-property (coord-list-of self)))
     (lob (select-built main '())))
    (set-property! (coord-list-of self))
    (mark-rebuilding! main lob)
    (set-property! (coord-list-of self) cl)))

(define-method built? ((self <Poly-Coord>))
  (and-let*
    (((call-next-method))
     (cl (get-property (coord-list-of self)))
     (cn (get-property (coord-num-of self)))
     (ll (length cl)))
    (or (automatic? self)
        edition?
        (>= ll (if (> cn 0) cn 2)))))

(define-method build (dc (self <Poly-Coord>) (main <Container>)(cont <Container>))
  (and
    (or (delayed? self )
        (and (not (built? self))
             (built-properties? (dependence-of self))))
    (let*
      ((num 0)
       (p0 #f)
       (p1 (copy-curr-mouse))
       (p2 #f)
       (shape-list '())
       (bound? (or (is-a? self <Polygon>)
                   (is-a? self <Outlined-Polygon>)
                   (is-a? self <Smart-Polygon>)
                   (is-a? self <Bound-Polyline>))))

      (define (get-poly)
        (get-property (coord-list-of self)))

      (define (get-num)
        (get-property (coord-num-of self)))

      (define (inc-num!)
        (set! num (inc num)))

      (define (dec-num!)
        (set! num (dec num)))

      (define (get-min)
        (if bound? 3 2))

      (define (show-diff arg)
        (if (or (get-property (h-visible? self))
                (get-property (l-visible? self))
                (get-property (s-visible? self)))
          (draw-diff arg)))

      (define (set-poly! val)
        (and p0 (set-reisshina-prev-point p0))
        (set-property! '&shape-list shape-list)
        (set-property! (coord-list-of self) val)
        (and (or (get-property (h-visible? self))
                 (get-property (l-visible? self))
                 (get-property (s-visible? self)))
             (show-diff
               (cons (edit-point-of self)
                 (enbound bound? (if (shaped? self)
                                   (make-shape (get-poly) shape-list)
                                   val))))))

      (define (add-point)
        (set-poly! (append (get-poly) (list p1)))
        (if (shaped? self)
          (set! shape-list (append! shape-list (list (get-shape-num)))))
        (inc-num!)
        (set-reisshina-active #t))

      (define (insert-left)
        (and-let*
          (((not (equal? p0 p1)))
           ((not (equal? p2 p1)))
           (cl (get-poly))
           (pt (cons (car p1) (cdr p1)))
           (ep (edit-point-of self)))
          (inc-num!)
          (set-poly!
            (case ep
              ((1)
               (set! p0 (if bound? (last-point cl) pt))
               (set! p2 (and bound? pt))
               (if (shaped? self)
                 (set! shape-list (cons (get-shape-num) shape-list)))
               (append
                 (list (car cl) pt)
                 (cdr cl)))
              ((-1)
               (set! p2 pt)
               (set-edit-point! self (dec num))
               (if (shaped? self)
                 (set! shape-list (append! (list-head shape-list (- num 2))
                                           (list (get-shape-num) (last-point shape-list)))))
               (append
                 cl
                 (list pt)))
              (else
               (set! p2 pt)
               (if (shaped? self)
                 (set! shape-list (append (list-head shape-list ep)
                                          (list (get-shape-num))
                                          (list-tail shape-list ep))))
               (append
                 (list-head cl ep)
                 (list pt)
                 (list-tail cl ep)))))
          (show-dist)))

      (define (insert-right)
        (and-let*
          (((not (equal? p0 p1)))
           ((not (equal? p2 p1)))
           (cl (get-poly))
           (pt (cons (car p1) (cdr p1)))
           (ep (edit-point-of self)))
          (inc-num!)
          (set-poly!
            (case ep
              ((1)
               (set! p2 p0)
               (set! p0 pt)
               (set-edit-point! self 2)
               (if (shaped? self)
                 (set! shape-list (append (list (car shape-list) (get-shape-num)) (cdr shape-list))))
               (append
                 (list p0 pt)
                 (list-tail cl ep)))
              ((-1)
               (set! p0 p1)
               (set! p1 (cons (car p0) (cdr p0)))
               (set! p2 (and bound? (car cl)))
               (if (shaped? self)
                 (set! shape-list (append! shape-list (list (get-shape-num)))))
               (append
                 cl
                 (list (cons (car p0) (cdr p0)))))
              (else
               (set! p0 p1)
               (set! p1 (cons (car p0) (cdr p0)))
               (set-edit-point! self (inc ep))
               (if (shaped? self)
                 (set! shape-list (append (list-head shape-list ep)
                                          (list (get-shape-num))
                                          (list-tail shape-list ep))))
               (append
                 (list-head cl ep)
                 (list (cons (car p0) (cdr p0)))
                 (list-tail cl ep)))))
          (show-dist)))

      (define (insert-glue)
        (and-let*
          ((ep (edit-point-of self))
           (pl (get-poly))
           (cl (if (shaped? self) (make-shape pl shape-list) pl))
           (sl (member-remove p1 cl))
           (dm (map (lambda (p) (dist p p1)) sl))
           ((not (null? dm)))
           (np (- (length sl) (length (member (apply min dm) dm))))
           (p (list-ref sl np))
           ((not (equal? p p0)))
           ((not (equal? p p2))))
          (inc-num!)
          (set! p0 (cons (car p) (cdr p)))
          (set-reisshina-prev-point p0)
          (set-poly!
            (case ep
              ((-1)
               (if (shaped? self)
                 (set! shape-list (append (list-head shape-list (dec (length shape-list)))
                                          (if edition? (list 0) (list 0 (get-shape-num))))))
               (append
                 (list-head pl (dec (length pl)))
                 (if edition? (list p) (list p p1))))
              (else
               (if (shaped? self)
                 (set! shape-list (append (list-head shape-list (dec ep))
                                          (list 0)
                                          (list-tail shape-list ep))))
               (append
                 (list-head pl (dec ep))
                 (list p)
                 (list-tail pl ep)))))
          (show-dist)))

      (define (delete-left)
        (let*
          ((cl (get-poly))
           (ep (edit-point-of self)))
           (cond
             ((and (> num 2) (= ep -1))
              (dec-num!)
              (if (= num 2)
                (set! p2 #f))
              (if (shaped? self)
                (set! shape-list (list-head shape-list num)))
              (set! p0 (point cl (dec num)))
              (set-poly! (append (list-head cl (dec num)) (list p1))))
             ((and (= num 2) (or (= ep -1) bound?))
              (set-edit-point! self -1)
              (set! num 0)
              (set! p0 p1)
              (set! p2 #f)
              (if (shaped? self)
                (set! shape-list '()))
              (set-poly! '())
              (set-property! (coord-list-of self)))
             ((> ep 2)
              (dec-num!)
              (set-edit-point! self (dec ep))
              (set! p0 (point cl (- ep 2)))
              (if (shaped? self)
                (set! shape-list (append (list-head shape-list (dec ep))
                                         (list-tail shape-list ep))))
              (set-poly! (append (list-head cl (- ep 2))
                                 (list p1)
                                 (list-tail cl ep))))
             ((= ep 2)
              (dec-num!)
              (set-edit-point! self 1)
              (set! p0 p2)
              (set! p2 (and bound? (> num 2) (list-ref cl num)))
              (if (shaped? self)
                (set! shape-list (cdr shape-list)))
              (set-poly! (cons p1 (cddr cl))))
             ((and (= ep 1) bound?)
              (dec-num!)
              (set! p0 (list-ref cl (dec num)))
              (if (= num 2)
                (set! p2 #f))
              (if (shaped? self)
                (set! shape-list (list-head shape-list num)))
              (set-poly! (list-head cl num))))
          (show-dist)))

      (define (delete-right)
        (let*
          ((cl (get-poly))
           (ep (edit-point-of self)))
           (cond
             ((and (= ep 1) (> num 2))
              (dec-num!)
              (if (not bound?)
                (set! p0 (point cl 3)))
              (set! p2 (and bound? (> num 2) (point cl 3)))
              (if (shaped? self)
                (set! shape-list (cons (car shape-list) (cddr shape-list))))
              (set-poly! (append (list p1) (cddr cl))))
             ((and (= num 2) (or (= ep 1) bound?))
              (set-edit-point! self -1)
              (set! num 0)
              (set! p1 p0)
              (set! p2 #f)
              (if (shaped? self)
                (set! shape-list '()))
              (set-poly! '())
              (set-property! (coord-list-of self)))

             ((and (> ep 1) (< ep (dec num)))
              (dec-num!)
              (set! p2 (point cl (+ ep 2)))
              (if (shaped? self)
                (set! shape-list (append (list-head shape-list ep)
                                         (list-tail shape-list (inc ep)))))
              (set-poly! (append (list-head cl ep)
                                 (list p2)
                                 (list-tail cl (+ ep 2)))))
             ((> ep 1)
              (dec-num!)
              (set-edit-point! self -1)
              (set! p2 (and (> num 2) bound? (car cl)))
              (if (shaped? self)
                (set! shape-list (list-head shape-list num)))
              (set-poly! (list-head cl ep)))

             ((and (= ep -1) bound?)
              (dec-num!)
              (set! p2 (and (> num 2)(cadr cl)))
              (if (shaped? self)
                (set! shape-list (cdr shape-list)))
              (set-poly! (cons (point cl 2) (list-tail cl 2)))))
          (show-dist)))

      (define (distance->str p1 p2)
        (or (and p1 p2 (len->string (scale-dist p1 p2)))
            ""))

      (define (sum-len)
        (or
          (and-let*
            ((pl (get-property (coord-list-of self)))
             (> (length pl) 1)
             (pll (enbound bound? (if (shaped? self)
                                     (make-shape pl shape-list)
                                     pl))))
            (len->string
              (apply + (map scale-dist pll (cdr pll)))))
          "0"))

      (define (show-dist)
        (put-hint (string-append (distance->str p0 p1)
                                 "  "
                                 (distance->str p1 p2)
                                 " L="
                                 (sum-len))))

      (define (change-point)
        (let*
          ((cl (get-poly))
           (ep (edit-point-of self)))
          (show-dist)
          (set-poly!
            (case ep
              ((1)
               (append (list p1) (cdr cl)))
              ((-1)
               (append (list-head cl (dec (length cl))) (list p1)))
              (else
               (append (list-head cl (dec ep)) (list p1) (list-tail cl ep)))))))

      (define (on-char dc event)
        (let
          ((ch (event-key event))
           (cl (get-poly))
           (ln (get-num))
           (ep (edit-point-of self)))
          (if (and (eq? ch #\del) (< num 2))
            (set! ch #\bs))

          (case ch
            ((#\x1a) ;; delete last point and break
             (when (and (negative? ep)
                        (not (positive? ln))
                        (> (if (shaped? self) (length (make-shape (get-poly) shape-list)) num) (get-min)))
               (draw-building dc main)
               (if (invalid-line? dc #f)
                 (draw-building dc main)
                 (begin
                   (set-poly! (list-head (get-poly) (dec num)))
                   (draw-building dc main)
                   (unselect-mode dc)))))
            ((#\space)
             (when (and cl (not (null? cl)) (or (zero? ln) (< num (abs ln))))
               (let
                 ((sp0 p0)
                  (spl (copy-list (get-poly))))
                 (draw-building dc main)
                 (insert-glue)
                 (if (and edition? (valid-line? dc #f) (>= num (get-min)))
                   (unselect-mode dc)
                   (begin
                     (if edition?
                       (begin
                         (set! p0 sp0)
                         (set-poly! spl)))
                     (draw-building dc main))))))

            ((#\esc)
             (set-edit-point! self 0)
             (show-diff #f)
             (breake-building dc main))

            ((#\del)
            (when (and (negative? ep)
                       (> num 1)
                       (to-previous-item cont))
              (draw-building dc main)
              (delay-item self main)
              (draw dc self)
              (delay-item (current-of main) main)
              (unselect-mode dc)))

            ((#\x12 #\x13) ;; left right (insert)
             (when (or (zero? ln) (< num (abs ln)))
               (draw-building dc main)
               ((if (char=? ch #\x12) insert-left insert-right))
               (draw-building dc main)))

            ((#\x18 #\x19 #\x14 #\x15) ;; up downt home end (move)
             (if (positive? num )
               (let*
                 ((ep (edit-point-of self))
                  (col (get-poly))
                  (pn (dec (if (negative? ep) num ep)))
                  (np (case ch
                        ((#\x18) (- pn 1))
                        ((#\x19) (+ pn 1))
                        ((#\x14) 0)
                        ((#\x15) (dec num))))
                  (nn (inc (if bound? (modulo (+ np num) num)
                                      (max 0 (min (dec num) np))))))
                 (set-edit-point! self (if (= nn num) -1 nn))
                 (when (not (= ep (edit-point-of self)))
                   (cond
                     ((= nn 1)
                      (set! p0 (if bound? (point col num) (cadr col)))
                      (set! p1 (car col))
                      (set! p2 (and bound? (cadr col))))
                     ((= nn num)
                      (set! p0 (point col (dec num)))
                      (set! p1 (point col num))
                      (set! p2 (and bound? (car col))))
                     (else
                       (set! p0 (point col (dec nn)))
                       (set! p1 (point col nn))
                       (set! p2 (point col (inc nn)))))
                     (set-cursor! p1)))))

            ((#\x11 #\bs) ;; delete backspace (delete)
             (if (and (char=? ch #\bs)
                      (memq 'shift (event-state event))
                      (to-previous-item cont))
               (begin
                 (draw-building dc main)
                 (mark-poly-defined! self main)
                 (draw-rebuilt dc main)
                 (set-poly! '())
                 (set-property! (coord-list-of self))
                 (delay-item (current-of cont) cont)
                 (unselect-mode dc))

               (when (>= num 2)
                 (draw-building dc main)
                 ((if (char=? ch #\x11) delete-right delete-left))
                 (if (zero? num)
                   (set-reisshina-active #f))
                 (draw-building dc main)))))))

      (define (post-fun dc)
        (if (shaped? self)
          (set-poly! (make-shape (get-poly) shape-list)))
        (if bound?
          (set-poly! (remove-selfcross (unbound (get-poly)))))
        (set-edit-point! self 0)
        (show-diff '())
        ((at-final-of self) (get-poly))
        (unselect-mode dc))

      (define (on-expose dc event)
        (draw dc main)
        (show-diff #f)
        (and ;(if hi-resolution?
             ;  (get-property (h-visible? self))
             ;  (get-property (l-visible? self)))
             (get-poly)
             (show-diff
               (cons (edit-point-of self)
                 (enbound bound? (if (shaped? self)
                                   (make-shape (get-poly) shape-list)
                                   (get-poly)))))))

      (define (on-motion dc event)
        (if (zero? num)
          #t;;(show-diff (list 0 (cons (event-x event) (event-y event))))
          (begin
            (and-let*
              (((shaped? self))
               (cl (make-shape (get-poly) shape-list))
               (ll (length cl))
               (ep (edit-point-of self))
               (sp (if (negative? ep) num ep))
               (sn (list-ref shape-list (dec sp))))
              (if (memq? sn '(1 2 3 6 7 8 9))
                (set-reisshina-active #f)
                (set-reisshina-prev-point
                  (if (or (<= ll 2) (memq? sn '(4 5)))
                    p0
                    (list-ref cl (- ll 2))))))
            (set-status! self #:stabil-status)
            (draw-building dc main)
            (set-car! p1 (event-x event))
            (set-cdr! p1 (event-y event))
            (change-point)
            (draw-building dc main)
            (set-status! self #:process-status))))

      (define (invalid-line? dc begin?)
        (let*
          ((pl (get-poly))
           (pll (if (shaped? self) (make-shape pl shape-list) pl))
           (m ((valid-test-of self) pll begin?)))
          (if (string? m)
            (begin
              (draw-building dc main)
              (tk-message-box m "Œ¯Ë·Í‡!")
              (draw-building dc main))
            (not m))))

      (define (valid-line? dc begin?)
        (not (invalid-line? dc begin?)))

      (define (on-button-press dc event)
        (if (or (not edition?)
                (zero? num))
          (begin
            (case (event-button event)
              ((1)
               (if (zero? num)
                 (begin
                   (set! p0 (cons (event-x event) (event-y event)))
                   (set! p1 (list-copy p0))
                   (if (shaped? self)
                     (set! shape-list (list (get-shape-num))))
                   (set-poly! (list p0))
                   (add-point)
                   (set! num 2)
                   (cond
                     ((invalid-line? dc #t)
                       (set! num 0)
                       (set-poly! '()))
                     (else
                       (set! p0 (car (get-poly)))
                       (draw-building dc main))))
                 (when (not (equal? p0 p1))
                   (draw-building dc main)
                   (change-point)
                   (if (and (not (zero? (get-num))) (<= (abs (get-num)) num))
                     (if (invalid-line? dc #f)
                       (draw-building dc main)
                       (unselect-mode dc))
                     (begin
                       (and-let*
                         (((insert-right))
                          ((negative? (edit-point-of self)))
                          (pl (get-poly))
                          (l2 (- (length pl) 3))
                          (t2 (list-tail pl l2))
                          (ll (make-trace (car t2) (cadr t2)))
                          (tll (length ll)))
                         (set-poly! (append (list-head pl l2) ll (last-pair pl)))
                         (if (shaped? self)
                           (set! shape-list (append shape-list (vector->list (make-vector (- tll 2) 0)))))
                         (set! num (+ num tll -2)))
                       (draw-building dc main))))))

              ((3)
               (let
                 ((cn (get-num)))
                 (cond
                   ((and (not (positive? cn)) (= num 0))
                     (draw-building dc main)
                     (set-poly! '())
                     (if (empty? self)
                       (unselect-mode dc)
                       (if (not edition?)
                         (breake-building dc main))))
                   ((if (positive? cn)
                      (= num cn)
                      (>= (if (shaped? self) (length (make-shape (get-poly) shape-list)) num) (get-min)))
                     (draw-building dc main)
                     (if (invalid-line? dc #f)
                       (draw-building dc main)
                       (begin
                         (if (equal? (last-point (get-poly)) '(0 . 0))
                           (set-poly! (list-head (get-poly) (dec num))))
                         (unselect-mode dc))))
                   ((or (positive? cn) (< num cn) (not (empty? self)))
                     (breake-building dc main)))))))
          (when (and (>= num (get-min)) (valid-line? dc (= (edit-point-of self) 1)))
            (draw-building dc main)
            (unselect-mode dc))))

      (set-edit-point! self -1)
      (set-reisshina-active #f)
      (show-diff #f)

      (if (get-poly)
        (let*
          ((col (get-poly))
           (lp (last-pair (get-poly)))
           (slp (cons (car lp) (cdr lp))))
          (if (delayed? self)
            (reenter self cont))
          (set-edit-point! self 0)
          (mark-poly-defined! self main)
          (draw-rebuilt dc main)
          (set-edit-point! self -1)
          (set! num (length col))
          (set! shape-list (vector->list (make-vector num 0)))

          (if (and (> num 1) (not edition?))
            (set-car! lp (copy-curr-mouse)))

          (if (= num 1)
            (begin
              (set! p0 (car col))
              (set! p1 (cons (car p0) (cdr p0)))
              (set-edit-point! self -1)
              (set! num 2)
              (set! shape-list (list (get-shape-num) (get-shape-num)))
              (set-poly! (list p0 p1)))
            (let loop ((ep 1)
                       (ed (dist curr-mouse (car col)))
                       (tp 2))
              (if (> tp num)
                (begin
                  (set-edit-point! self (if (= ep num) -1 ep))
                  (set! lp slp)
                  (set! p1 (set-cursor! (point col ep)))
                  (cond
                    ((= ep 1)
                     (set! p0 (point col (if bound? num 2)))
                     (set! p2 (and bound? (point col 2))))
                    ((= ep num)
                     (set! p0 (point (get-poly) (dec num)))
                     (set! p2 (and bound? (car col))))
                    (else
                      (set! p0 (point col (dec ep)))
                      (set! p2 (point col (inc ep))))))
                (let
                  ((td (dist curr-mouse (point col tp))))
                  (if (< td ed)
                    (loop tp td (inc tp))
                    (loop ep ed (inc tp)))))))
          (set-reisshina-active #t)))

      (draw-building dc main)

      (select-mode
        (list
          (cons 'post         post-fun)
          (cons 'button-press on-button-press)
          (cons 'motion       on-motion)
          (cons 'expose       on-expose)
          (cons 'key-press    on-char))
        dc)
        )))

(define-method pack ((self <Poly-Coord>))
  (and-let*
    ((poly (get-property (coord-list-of self)))
     (nm (call-next-method)))
    (cons (cons #:poly poly) nm)))

(define-method unpack ((self <Poly-Coord>) (src <List>))
  (and-let*
    ((data (or (assv-ref src #:poly) (assv-ref src #:footnote)))
     ((list? data))
     (dl (length data))
     ((> dl 1))
     ((every (lambda (p) (and (pair? p) (number? (car p)) (number? (cdr p)))) data)))
    (set-property! (coord-list-of self) data))
    (call-next-method))
;;====================================================================
;; <Polyline>
;;
(define-class <Polyline> (<Poly-Coord> <Stroked>)
  ())

(define-method draw (dc (self <Polyline>))
  (and-let*
    (((zero? (edit-point-of self)))
     ((call-next-method))
     ((coord-list-built? self)))
    (draw-lines dc (scale-poly (get-property (coord-list-of self))))))
;;====================================================================
;; <Bound-Polyline>
;;
(define-class <Bound-Polyline> (<Poly-Coord> <Stroked>)
  ())

(define-method draw (dc (self <Bound-Polyline>))
  (and-let*
    (((zero? (edit-point-of self)))
     ((call-next-method))
     ((coord-list-built? self)))
    (draw-lines dc (scale-poly (get-property (coord-list-of self))) #f #t)))
;;====================================================================
(define-class <Color-Polyline> (<Polyline> <Color>)
  ())

(define-method pack ((self <Color-Polyline>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Polyline nm)))

(define-method unpack ((self <Color-Polyline>) (src <List>))
 (and (or (null? src)
          is-convertion?
          (eq? (car src) #:Polyline))
      (call-next-method)))
;;====================================================================
(define-class <Color-Footnote> (<Polyline> <Color>)
  ())

(define-method pack ((self <Color-Footnote>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Footnote nm)))

(define-method unpack ((self <Color-Footnote>) (src <List>))
  (and (or (null? src)
           is-convertion?
           (eq? (car src) #:Footnote))
       (call-next-method)))
;;====================================================================
(define-class <Bound-Color-Polyline> (<Bound-Polyline> <Color>)
  ())

(define-method pack ((self <Bound-Color-Polyline>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Bound-Polyline nm)))

(define-method unpack ((self <Bound-Color-Polyline>) (src <List>))
  (and (or is-convertion?
           (eqv? (car src) #:Bound-Polyline))
       (call-next-method)))
;;====================================================================
;; <Polygon>
;;
(define-class <Polygon> (<Poly-Coord> <Filled>)
  ())

(define-method draw (dc (self <Polygon>))
  (and-let*
    (((zero? (edit-point-of self)))
     ((call-next-method))
     ((coord-list-built? self)))
    (fill-polygon dc (scale-poly (get-property (coord-list-of self))))))

(define-class <Filled-Polygon> (<Polygon>)
 ())

(define-method pack ((self <Filled-Polygon>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Polygon nm)))

(define-method unpack ((self <Filled-Polygon>) (src <List>))
  (and (or is-convertion? (eqv? (car src) #:Polygon)
                          (eqv? (car src) #:Outlined-Polygon)
                          (eqv? (car src) #:Smart-Polygon)
                          )
       (call-next-method)))
;;====================================================================
(define-class <Outlined-Polygon> (<Polygon> <Stroked> <Color>)
  ())

(define-method draw (dc (self <Outlined-Polygon>))
  (and (call-next-method)
       (draw-lines dc (scale-poly (get-property (coord-list-of self))) #f #t)))

(define-method pack ((self <Outlined-Polygon>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Outlined-Polygon nm)))

(define-method unpack ((self <Outlined-Polygon>) (src <List>))
 (and-let*
   (((or is-convertion?
         (eqv? (car src) #:Outlined-Polygon)
         (eqv? (car src) #:Smart-Polygon)))
    ((or is-convertion? (set-car! src #:Polygon)))
    (r (call-next-method))
    ((or is-convertion? (set-car! src #:Outlined-Polygon))))
   r))
;;====================================================================
(define-class <Smart-Polygon> (<Polygon> <Stroked> <Color>)
  ())

(define-method draw (dc (self <Smart-Polygon>))
  (and (call-next-method)
       (draw-lines dc (scale-poly (get-property (coord-list-of self))) #f #t)))

(define-method pack ((self <Smart-Polygon>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Smart-Polygon nm)))

(define-method unpack ((self <Smart-Polygon>) (src <List>))
 (and-let*
   (((or is-convertion?
         (eqv? (car src) #:Smart-Polygon)
         (eqv? (car src) #:Outlined-Polygon)))
    ((or is-convertion? (set-car! src #:Polygon)))
    (r (call-next-method))
    ((or is-convertion? (set-car! src #:Smart-Polygon))))
   r))
;;====================================================================
;;
(define (std-valid-test s)
  (string? s))

(define-class <Text> (<Coord> <Color>)
  ((test     #:type (lambda(x)(procedure? x))
             #:initform std-valid-test
             #:initarg #:valid-test
             #:getter 'valid-test-of)
(at-final    #:type (lambda(x)(procedure? x))
             #:initform std-valid-test
             #:initarg #:at-final
             #:getter 'at-final-of)
 (selectable #:type <Boolean>
             #:initarg #:selectable
             #:initform #f
             #:getter 'selectable?)
  (selection #:type procedure-OR-symbol-OR-integer?
             #:initform 0
             #:initarg #:selection
             #:getter 'selection-of
             #:setter 'set-selection!)
   (x-offset #:type procedure-OR-symbol-OR-integer?
             #:initform 0
             #:initarg #:x-offset
             #:getter 'x-offset-of)
   (y-offset #:type procedure-OR-symbol-OR-integer?
             #:initform 0
             #:initarg #:y-offset
             #:getter 'y-offset-of)
   (font     #:type procedure-OR-symbol-OR-vector?
             #:initform '#(0 -16)
             #:initarg #:font
             #:getter 'font-of)
   (length   #:type procedure-OR-symbol-OR-integer?
             #:initform 0
             #:initarg #:length
             #:getter 'length-of)
   (text     #:type procedure-OR-symbol-OR-string?
             #:initform ""
             #:initarg #:text
             #:getter 'text-of)))

(define-method set-default! ((self <Text>))
  (call-next-method)
  (set-property! (font-of self) '#(0 -16))
  (set-property! (text-of self) "0"))

(define-method origin-of ((self <Text>))
  (shift (cons (get-property (x-coord-of self))
               (get-property (y-coord-of self)))
         (turn (scale-size (cons (get-property (x-offset-of self))
                                 (get-property (y-offset-of self))))
               (get-property (angle-of self)))))

(define (remove-kwasi-cursor str)
  (let*
    ((cp (and (string? str) (string-index str kwasi-cursor-char))))
    (if cp
      (string-append (substring str 0 cp) (substring str (inc cp)))
      str)))

(define-method pack ((self <Text>))
  (and-let*
    ((font  (get-property (font-of self)))
     (x-off (get-property (x-offset-of self)))
     (y-off (get-property (y-offset-of self)))
     (angle (get-property (angle-of self)))
     (text  (get-property (text-of self)))
     (nm  (call-next-method)))
    (cons
      #:Text
      (cons
        (cons #:font font)
        (cons
          (cons #:string (vector x-off y-off angle (remove-kwasi-cursor text)))
          nm)))))

(define-method unpack ((self <Text>) (src <List>))
  (and-let*
    (((or is-convertion?
          (eqv? (car src) #:Text)))
     (font (or (assv-ref src #:font) '#(0 0)))
     ((vector? font))
     ((>= (vector-length font) 2))
     ((integer? (vector-ref font 0)))
     ((integer? (vector-ref font 1)))
     (data (assv-ref src #:string))
     ((vector? data))
     ((= (vector-length data) 4))
     (ox (vector-ref data 0))
     (oy (vector-ref data 1))
     (ta (vector-ref data 2))
     (tx (vector-ref data 3))
     ((integer? ox))
     ((integer? oy))
     ((integer? ta))
     ((string?  tx)))
    (set-property! (font-of self) font)
    (set-property! (x-offset-of self) ox)
    (set-property! (y-offset-of self) oy)
    (set-property! (angle-of self)    ta)
    (set-property! (text-of self)     tx)
    (if (selectable? self)
       (set-property! (selection-of self)
                      (find-selection (get-class)
                                      (text-of self)
                                      (get-property (text-of self))
                                      (selection-of self))))
    (call-next-method)
    (set! data (scale-coord (cons (get-property (x-coord-of self))
                                  (get-property (y-coord-of self)))))
    (set-property! (x-coord-of self) (car data))
    (set-property! (y-coord-of self) (cdr data))
    ))

(define-method built? ((self <Text>))
  (and
    (call-next-method)
    (get-property (text-of self))
    (get-property (length-of self))
    (get-property (angle-of self))
    (get-property (x-offset-of self))
    (get-property (y-offset-of self))
    (get-property (x-coord-of self))
    (get-property (y-coord-of self))))

(define-method automatic? ((self <Text>))
  (and (call-next-method)
       (not (or (symbol? (text-of self))
;                (and (pair? (text-of self))
;                     (symbol? (car (text-of self))))
                (symbol? (angle-of self))))))

(define-method draw (dc (self <Text>))
  (define (select vec i val)
    (if (> (vector-length vec) i) (vector-ref vec i) val))

  (and-let*
    (((call-next-method))
     ((built? self))
     (font (get-property (font-of self)))
     (ff (vector-ref font 0))
     (font-face (num->font-face (if (integer? ff) ff 0)))
     (font-size (inexact->exact (* obj-scale-factor (vector-ref font 1))))
     (font-weight  (select font 2 "*"))
     (font-style   (select font 3 "*"))
     (font-charset (select font 4 "Russian"))
     (afnt (vector ff font-size font-weight font-style font-charset))
     (org (origin-of self))
     (ang (get-property (angle-of self)))
     (text (get-property (text-of self)))
     (fnt (make-font font-face font-weight font-style font-charset)))
    (set-font! dc fnt font-size ang)
    (let*
      ((a (deg->rad (get-property (angle-of self))))
       (fs (abs font-size))
       (d (+ fs (quotient fs 4)))
       (dd (cons (inexact->exact (floor (* d (sin a))))
                 (inexact->exact (floor (* d (cos a)))))))
      (let loop ((t text))
        (let*
          ((i (string-index t #\newline))
           (s0 (if i (substring t 0 i) t))
           (ep (string-index s0 kwasi-cursor-char))
           (hd (and ep (substring s0 0 ep)))
           (tl (and ep (substring s0 (inc ep))))
           (st (if ep (string-append hd tl) s0)))
          (if (changed? self)
            (draw-string dc org st))
          (and-let*
            ((ep)
             (it (if (or (string-index font-style #\I)
                         (string-index font-style #\i)) 100 0))
             (pos (shift org (turn (cons (calc-string-width hd afnt ang) 0) ang))))
             (set-text-cursor fs pos (- ang it)))
          (set! org (shift org dd))
          (and i (loop (substring t (inc i)))))))))

(define-generic crush-text! ((self <Text>))
  (set-property! (text-of self))
  (set-property! (font-of self))
  (set-property! (angle-of self)))

(define-generic mark-text-defined! ((self <Text>) cont)
  (let
    ((tx (get-property (text-of self)))
     (fn (get-property (font-of self)))
     (an (get-property (angle-of self)))
     (lob (select-built cont '())))
    (crush-text! self)
    (mark-rebuilding! cont lob)
    (set-property! (text-of self) tx)
    (set-property! (font-of self) fn)
    (set-property! (angle-of self) an)))

(define-method build (dc (self <Text>) (main <Container>)(cont <Container>))
  (if (and (not (delayed? self)) (automatic? self))
     (set-property! (text-of self) ""))

  (and
    (or (delayed? self)
        (and (not (built? self))
             (built-properties? (dependence-of self))))
    (let*
      ( (edit-text? (and (symbol? (text-of self))
                         local-object?
                         (not (selectable? self))))
        (edit-coord? (or (symbol? (x-coord-of self))
                         (symbol? (y-coord-of self))
                         (symbol? (angle-of self))))
        (angle-set? #f)
        (edit-position 0)
        (mx 0)
        (my 0)

        (post-fun
          (lambda (dc)
            ((at-final-of self) (get-property (text-of self)))
            (unselect-mode dc)))

        (calc-cursor-pos
          (lambda (dc str)
            (let*
              ((font (get-property (font-of self)))
               (fs (abs (vector-ref font 1)))
               (ang (get-property (angle-of self)))
               (a (deg->rad ang))
               (d (+ fs (quotient fs 4)))
               (dd (cons (inexact->exact (* d (sin a)))
                         (inexact->exact (* d (cos a)))))
               (org (origin-of self)))
              (let loop ((t str))
                (let*
                  ((i (string-index t #\newline))
                   (st (if i (substring t 0 i) t)))
                  (if i
                    (begin
                      (set! org (shift org dd))
                      (loop (substring t (inc i))))
                    (shift org (turn (cons (calc-string-width t font ang) 0) ang))))))))

        (test-current-input
          (lambda ()
            (let*
              ((tx (get-property (text-of self)))
               (cs (trim-spaces (remove-kwasi-cursor tx)))
               (vl ((valid-test-of self) cs)))
              (put-hint (string-append (if (string? vl) vl (get-property (hint-of self))) ": " (trim-spaces cs))))))

        (end-of-input
          (lambda (dc)
            (let*
              ((cs (trim-spaces (remove-kwasi-cursor (get-property (text-of self)))))
               (vl (or ((valid-test-of self) cs) (not edit-text?))))
              (if vl
                (if (string? vl)
                  (put-hint (string-append vl ": " cs))
                  (begin
                    (draw-building dc main)
                    (set-property! (text-of self) cs)
                    (kill-text-cursor)
                    (unselect-mode dc)))))))

        (on-expose
          (lambda (dc event)
            (draw dc main)
            (test-current-input)))

        (on-motion
          (lambda (dc event)
            (if edit-coord?
              (begin
                (if (memq 'shift (event-state event))
                  (set! angle-set? (>= (string-length (get-property (text-of self))) 1))
                  (if angle-set?
                    (let*
                      ((org (origin-of self))
                       (off (cons (get-property (x-offset-of self)) (get-property (y-offset-of self))))
                       (p (turn off (get-property (angle-of self)))))
                      (set! angle-set? #f)
                      (set! mx (- (event-x event) (car org) (- (car p))))
                      (set! my (- (event-y event) (cdr org) (- (cdr p)))))))
                (if angle-set?
                  (let
                    ((deg (a-kvant self (rad->deg
                       (atan (- (get-property (y-coord-of self)) (event-y event))
                             (- (event-x event) (get-property (x-coord-of self))))))))
                    (when (not (eq? deg (get-property (angle-of self))))
                      (draw-building dc main)
                      (set-property! (angle-of self) deg)
                      (draw-building dc main)))
                  (begin
                    (draw-building dc main)
                    (move-to self (- (event-x event) mx) (- (event-y event) my))
                    (draw-building dc main)))))))

        (on-char
          (lambda (dc event)
            (let
              ((ch (event-key event)))
              (if (and edit-coord?
                       (memq 'shift (event-state event))
                       (or (eq? ch #\+)(eq? ch #\-)))
                (begin
                  (draw-building dc main)
                  (set-property! (angle-of self)
                    (std-angle ((if (eq? (event-key event) #\+) + -)
                      (get-property (angle-of self))(car (a-grid-of self)))))
                  (draw-building dc main))
                (if edit-text?
                  (let*
                    ((sz (get-property (length-of self)))
                     (cn (get-property (text-of self)))
                     (ln (string-length cn))
                     (cs-head (substring cn 0 edit-position))
                     (cs-tail (substring cn (inc edit-position) ln))
                     (cs (string-append cs-head cs-tail)))
                    (if (and (eq? ch #\del) (= ln 1))
                      (set! ch #\bs))
                    (case ch
                      ((#\return)
                        (end-of-input dc))
                      ((#\del)
                        (when (to-previous-item cont)
                          (delay-item (current-of cont) cont)
                          (draw-building dc main)
                          (if edit-text?
                            (set-property! (text-of self) cs))
                          (delay-item self cont)
                          (draw dc self)
                          (unselect-mode dc)))
                      ((#\bs)
                        (if edit-text?
                          (if (memq 'shift (event-state event))
                            (when (to-previous-item cont)
                              (delay-item (current-of cont) cont)
                              (draw-building dc main)
                              (mark-text-defined!  self main)
                              (mark-coord-defined! self main)
                              (crush-text! self)
                              (crush-coord! self)
                              (kill-text-cursor)
                              (unselect-mode dc))
                            (when (positive? edit-position)
                              (draw-building dc main)
                              (set! edit-position (dec edit-position))
                              (set-property! (text-of self)
                                (string-append
                                  (substring cs-head 0 edit-position)
                                  kwasi-cursor
                                  cs-tail))
                              (draw-building dc main)
                              (test-current-input)))))
                      ((#\x11) ; delete
                        (if (and edit-text? (< (inc edit-position) ln))
                          (begin
                            (draw-building dc main)
                            (set-property! (text-of self)
                              (string-append
                                cs-head
                                kwasi-cursor
                                (substring cs-tail 1)))
                            (draw-building dc main)
                            (test-current-input))))
                      ((#\x12) ; left
                        (if (and edit-text? (positive? edit-position))
                          (begin
                            (set-text-changed! main #f)
                            (draw-building dc main)
                            (set! edit-position (dec edit-position))
                            (set-property! (text-of self)
                              (string-append
                                (substring cs-head 0 edit-position)
                                kwasi-cursor
                                (substring cs-head edit-position (inc edit-position))
                                cs-tail))
                            (draw-building dc main)
                            (set-text-changed! main #t)
                            )))
                      ((#\x18) ; up
                        (or
                          (and-let*
                            ((edit-text?)
                             ((positive? edit-position))
                             (p1 (string-rindex cs #\newline edit-position))
                             (p0 (inc (or (string-rindex cs #\newline p1) -1)))
                             (font (get-property (font-of self)))
                             (ang (get-property (angle-of self)))
                             (l1 (calc-string-width (substring cs-head (inc p1) edit-position) font ang))
                             (d -1))
                            (let loop
                              ((ep p0))
                              (let*
                                ((l0 (calc-string-width (substring cs p0 ep) font ang))
                                 (td (abs (- l0 l1))))
                                (when (or (negative? d) (> d td))
                                  (set! edit-position ep)
                                  (set! d td))
                                (if (and (< ep p1) (< l0 l1))
                                  (loop (inc ep))
                                  (begin
                                    (set-text-changed! main #f)
                                    (draw-building dc main)
                                    (set-property! (text-of self)
                                      (string-append
                                        (substring cs 0 edit-position) kwasi-cursor (substring cs edit-position)))
                                    (draw-building dc main)
                                    (set-text-changed! main #t)
                                    ))))))
                          #t)
                      ((#\x13) ; right
                        (if (and edit-text? (< (inc edit-position) ln))
                          (begin
                            (set-text-changed! main #f)
                            (draw-building dc main)
                            (set! edit-position (inc edit-position))
                            (set-property! (text-of self)
                              (string-append
                                cs-head
                                (substring cs-tail 0 1)
                                kwasi-cursor
                                (substring cs-tail 1 (string-length cs-tail))))
                            (draw-building dc main)
                            (set-text-changed! main #t)
                            )))
                      ((#\x19) ; down
                        (or
                          (and-let*
                            ((edit-text?)
                             ((< (inc edit-position) ln))
                             (np (string-index cs #\newline edit-position))
                             (p0 (inc np))
                             (p1 (or (string-index cs #\newline p0) (dec ln)))
                             (font (get-property (font-of self)))
                             (ang (get-property (angle-of self)))
                             (sp (inc (or (string-rindex cs #\newline edit-position) -1)))
                             (l1 (calc-string-width (substring cs sp edit-position) font ang))
                             (d -1))
                            (let loop
                              ((ep p0))
                              (let*
                                ((l0 (calc-string-width (substring cs p0 ep) font ang))
                                 (td (abs (- l0 l1))))
                                (when (or (negative? d) (> d td))
                                  (set! edit-position ep)
                                  (set! d td))
                                (if (and (< ep p1)
                                         (< l0 l1))
                                  (loop (inc ep))
                                  (begin
                                    (set-text-changed! main #f)
                                    (draw-building dc main)
                                    (set-property! (text-of self)
                                      (string-append
                                        (substring cs 0 edit-position) kwasi-cursor (substring cs edit-position)))
                                    (draw-building dc main)
                                    (set-text-changed! main #t)
                                    ))))))
                          #t)
                      ((#\x14) ; home
                        (if (and edit-text? (positive? edit-position))
                          (let*
                            ((p (string-rindex cs #\newline
                              (if (char=? (string-ref cs (dec edit-position)) #\newline)
                                (dec edit-position) edit-position))))
                            (set-text-changed! main #f)
                            (draw-building dc main)
                            (set! edit-position (if p (inc p) 0))
                            (set-property! (text-of self)
                              (if p
                                (string-append
                                  (substring cs 0 (inc p)) kwasi-cursor (substring cs (inc p)))
                                (string-append
                                  kwasi-cursor cs)))
                            (draw-building dc main)
                            (set-text-changed! main #t)
                            )))
                      ((#\x15) ; end
                        (if (and edit-text? (< edit-position (dec ln)))
                          (let*
                            ((p (string-index cs #\newline
                              (if (and (positive? edit-position)
                                       (char=? (string-ref cs edit-position) #\newline))
                                (inc edit-position) edit-position))))
                            (set-text-changed! main #f)
                            (draw-building dc main)
                            (set! edit-position (if p p (dec ln)))
                            (set-property! (text-of self)
                              (if p
                                (string-append
                                  (substring cs 0 p) kwasi-cursor (substring cs p))
                                (string-append
                                  cs kwasi-cursor)))
                            (draw-building dc main)
                            (set-text-changed! main #t)
                            )))
                      ((#\x16) ; ctrl-home
                        (if (and edit-text? (positive? edit-position))
                          (begin
                            (set-text-changed! main #f)
                            (draw-building dc main)
                            (set! edit-position 0)
                            (set-property! (text-of self)
                              (string-append kwasi-cursor cs))
                            (draw-building dc main)
                            (set-text-changed! main #t)
                            )))
                      ((#\x17) ; ctrl-end
                        (if (and edit-text? (< (inc edit-position) ln))
                          (begin
                            (set-text-changed! main #f)
                            (draw-building dc main)
                            (set! edit-position (dec ln))
                            (set-property! (text-of self)
                              (string-append cs kwasi-cursor))
                            (draw-building dc main)
                            (set-text-changed! main #t)
                            )))
                      (else
                        (if
                          (and
                            edit-text?
                            (or (char>=? ch #\space)
                                (char=?  ch #\newline))
                            (or (zero? sz) (<= ln sz)))
                          (begin
                            (draw-building dc main)
                            (set! edit-position (inc edit-position))
                            (set-property! (text-of self)
                              (string-append cs-head (string ch) kwasi-cursor cs-tail))
                            (draw-building dc main)
                            (test-current-input)))))))))))

        (on-button-press
          (lambda (dc event)
            (if (not (or (memq 'shift (event-state event))
                         (memq 'control (event-state event))))
              (case (event-button event)
                ((1)
                 (end-of-input dc))
                ((3)
                 (breake-building dc main)))))))

      (if (and (delayed? self) (built? self))
        (let*
          ((org (origin-of self))
           (dx (get-property (x-offset-of self)))
           (dy (get-property (y-offset-of self)))
           (dp (turn (cons dx dy) (get-property (angle-of self)))))
          (reenter self cont)
          (mark-coord-defined! self main)
          (mark-text-defined! self main)
          (draw-rebuilt dc main)
          (set! mx (- (get-curr-x) (car org) (- (car dp))))
          (set! my (- (get-curr-y) (cdr org) (- (cdr dp))))
          )
        (begin
          (if (not (get-property (text-of self)))
            (set-property! (text-of self) ""))
          (if (not (get-property (angle-of self)))
            (set-property! (angle-of self) (get-reisshina-angle)))
          (move-to self (get-curr-x) (get-curr-y))))

      (if (selectable? self)
        (let
          ((cur-sel
             (and (get-property (text-of self))
                  (or (not edition?)
                      (positive? (string-length (get-property (text-of self)))))
                  (make-selection (get-class)
                                  edition?
                                  (text-of self)
                                  (selection-of self)
                                  (or (get-property (selection-of self)) 0)
                                  (hint-of self)))))

          (if cur-sel
            (begin
              (set-property! (selection-of self) (car cur-sel))
              (set-property! (text-of self) (cdr cur-sel)))
            (breake-building dc main))))

      (and-let*
        ((edit-text?)
         (t (get-property (text-of self)))
         (d -1))
        (if (not edition?)
          (set! edit-position (string-length t))
          (if text-edit-point
            (set! edit-position text-edit-point)
            (dotimes (i (inc (string-length t)))
              (and-let*
                ((t (dist curr-mouse (calc-cursor-pos dc (substring t 0 i))))
                 ((or (negative? d) (> d t))))
                (set! edit-position i)
                (set! d t)))))
        (set-property! (text-of self)
          (string-append
            (substring t 0 edit-position)
            kwasi-cursor
            (substring t edit-position))))

      (and
        (or edit-text? edit-coord?)
        (begin
          (draw-building dc main)
          (set-reisshina-active #f)
          (and edit-text? (test-current-input))

          (select-mode
            (list
              (cons 'button-press   on-button-press)
              (cons 'motion         on-motion)
              (cons 'expose         on-expose)
              (cons 'key-press      on-char)
              (cons 'post           post-fun))
            dc))))))
;;====================================================================
;;; End of code
