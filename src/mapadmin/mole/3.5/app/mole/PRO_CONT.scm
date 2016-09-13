;;;
;;; Pro_cont.scm
;;; Drawable container
;;;
;;====================================================================
;; <Coord>
;;
(define-class <Coord> (<Prototype>)
  ((x-coord #:type procedure-OR-symbol-OR-integer?
            #:initform 0
            #:initarg #:x-coord
            #:getter 'x-coord-of)
   (y-coord #:type procedure-OR-symbol-OR-integer?
            #:initform 0
            #:initarg #:y-coord
            #:getter 'y-coord-of)))

(define-method set-default! ((self <Coord>))
  (call-next-method)
  (set-property! (x-coord-of self) 0)
  (set-property! (y-coord-of self) 0))

(define-generic coord-built? ((self <Coord>))
  (and
    (get-property (x-coord-of self))
    (get-property (y-coord-of self))))

(define-method built? ((self <Coord>))
  (and
    (call-next-method)
    (coord-built? self)))

(define-method automatic? ((self <Coord>))
  (and
    (call-next-method)
    (not (or (symbol? (x-coord-of self))
             (symbol? (y-coord-of self))))))

(define-generic crush-coord! ((self <Coord>))
  (set-property! (x-coord-of self))
  (set-property! (y-coord-of self)))

(define-method pack ((self <Coord>))
  (and-let*
    ((x (get-property (x-coord-of self)))
     (y (get-property (y-coord-of self)))
     (p (unscale-coord (cons x y)))
     (nm (call-next-method)))
    (cons (cons #:coord (vector (car p) (cdr p))) nm)))

(define-method unpack ((self <Coord>) (src <List>))
  (and-let*
    ((data (assv-ref src #:coord))
     ((vector? data))
     ((= 2 (vector-length data))))
    (set-property! (x-coord-of self) (vector-ref data 0))
    (set-property! (y-coord-of self) (vector-ref data 1))
    (call-next-method)))

(define-generic mark-coord-defined! ((self <Coord>) cont)
  (let
    ((x (get-property (x-coord-of self)))
     (y (get-property (y-coord-of self)))
     (lob (select-built cont '())))
    (crush-coord! self)
    (mark-rebuilding! cont lob)
    (move-to self x y)))

(define-method origin-of ((self <Coord>))
  (cons
    (get-property (x-coord-of self))
    (get-property (y-coord-of self))))

(define-method move-to ((self <Coord>) (x <Number>) (y <Number>))
  (set-property! (x-coord-of self) x)
  (set-property! (y-coord-of self) y))
;;====================================================================
(define-class <Point> (<Coord>)
 ())

(define-method pack ((self <Point>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Point nm)))
;;====================================================================
;;
(define-class <Container> (<Prototype>)
  ((contents #:type (lambda (x) (or (list? x) (procedure? x)))
             #:initform '()
             #:initarg #:contents)
   (version  #:type <Pair>
             #:initform '(#f . #f)
             #:getter 'version-of
             #:setter 'set-version!
             #:initarg #:version)
   (del-num  #:type <Number>
             #:initform 0
             #:initarg #:del-num
             #:getter 'del-num-of
             #:setter 'set-del-num!)
   (current  #:initform '()
             #:initarg #:current
             #:setter 'set-current!
             #:getter 'current-of)))

(define-generic contents-of ((self <container>))
  (let
    ((cont (slot-ref self 'contents)))
    (if (built-properties? (dependence-of self))
      (if (procedure? cont) (cont) cont)
      '())))

(define-generic contents-built? ((self <Container>))
;;(tk-box (map built? (contents-of self)))
  (and (built-properties? (dependence-of self))
       (or (dynamic? self)
           (every (lambda (x) (and x (or (get-property (omit? x)) (built? x))))
                  (contents-of self)))))

(define-method built? ((self <Container>))
;(tk-box (contents-of self) "(contents-of self)")
;(tk-box building-env "building-env")
;(tk-box (map built? (contents-of self)))
  (and (call-next-method)
       (contents-built? self)
       (zero? (del-num-of self))))

(define (set-curr-prim! cont)
  (set! curr-prim (- (length (contents-of cont))
                     (length (memq (current-of cont) (contents-of cont))))))

(define-generic to-next-item ((self <Container>))
  (let loop ((cont (contents-of self)))
    (if (< (length cont) 2)
      #f
      (let
        ((tail (cdr (memq (current-of self) cont))))
        (set-current! self (car (if (null? tail) cont tail)))
        (set-curr-prim! self)
        (or (not (automatic? (current-of self)))
            (loop cont))))))

(define-generic to-previous-item ((self <Container>))
  (and-let*
    (((not edition?))
     (cont (contents-of self))
     ((>= (length cont) 2))
     (revl (reverse cont)))
    (let loop ((tail (cdr (memq (current-of self) revl))))
      (and-let*
        (((not (null? tail)))
         (next (car tail)))
        (if (or (automatic? next)
                (get-property (omit? next))
                (eq? (status-of next) #:process-status))
          (loop (cdr tail))
          (begin
            (set-current! self next)
            (set-curr-prim! self)))))))

(define-method set-default! ((self <Container>))
  (for-each set-default! (contents-of self)))

(define-method draw (dc (self <Container>))
  (for-each (lambda (x) (draw dc x)) (contents-of self)))

(define-generic draw-building (dc (self <Prototype>))
  (draw dc self))

(define-method draw-building (dc (self <Container>))
  (for-each
    (lambda (x)
      (if (or (dynamic? self)
              (eq? (status-of x) #:process-status))
        (draw-building dc x)))
    (contents-of self)))

(define-generic draw-built (dc (self <Prototype>))
  (draw dc self))

(define-method draw-built (dc (self <Container>))
  (for-each
    (lambda (x)
      (if (and (eq? (status-of x) #:process-status)
               (built? x)
               (not (dynamic? x)))
        (begin
          (set-status! x #:stabil-status)
          (draw-built dc x))))
    (contents-of self)))

(define-generic draw-rebuilt (dc (self <Prototype>))
  (or edition? (draw dc self)))

(define-method draw-rebuilt (dc (self <Container>))
  (for-each
    (lambda (x)
      (when (eq? (status-of x) #:rebuild-status)
        (draw-rebuilt dc x)
        (set-status! x #:process-status)))
    (contents-of self)))

(define-generic breake-building (dc (self <Container>))
  (if (not edition?)
    (unstabilize-all dc self)))

(define-generic unstabilize-all (dc (self <Container>))
  (for-each
    (lambda (x)
      (draw dc x)
      (init-building x #:process-status))
    (contents-of self))
  #f)

(define-generic set-text-changed! ((self <Container>) (ch? <boolean>))
  (for-each
    (lambda (x)
      (set-changed! x ch?))
    (contents-of self)))

(define-generic select-built ((self <Prototype>) (list-of-built <List>))
  (if (and (eq? (status-of self) #:stabil-status) (built? self))
    (cons self list-of-built)
    list-of-built))

(define-method select-built ((self <Container>) (list-of-built <List>))
  (for-each
    (lambda (x)
      (set! list-of-built (select-built x list-of-built)))
    (contents-of self))
  list-of-built)

(define-generic mark-rebuilding! ((self <Prototype>) (list-of-built <List>))
  (when (and (memq self list-of-built) (not (built? self)))
    (set-status! self #:rebuild-status)))

(define-method mark-rebuilding! ((self <Container>) (list-of-built <List>))
  (for-each
    (lambda (x)
      (mark-rebuilding! x list-of-built))
    (contents-of self)))

(define-generic init-building ((self <Prototype>) status)
  (set-status! self
    (if (or (dynamic? self) (not (built? self)))
      #:process-status
      status)))

(define-method init-building ((self <Container>) status)
  (call-next-method)
  (when (not (null? (contents-of self)))
    (set-del-num! self 0)
    (set-current! self (car (contents-of self)))
    (set! curr-prim 0)
    (for-each (lambda (p) (init-building p status)) (contents-of self))))

(define-generic delay-all ((self <Container>))
  (for-each (lambda (p) (delay-item p self)) (contents-of self)))

(define-method automatic? ((self <Container>))
  (let loop ((l (contents-of self)))
    (cond
      ((null? l) #t)
      ((automatic? (car l)) (loop (cdr l)))
      (else #f))))

(define is-packing? #f)
(define-method build (dc (self <Container>) (main <Container>) (cont <Container>))
  (set! is-packing? #f)
  (letrec
     ((loop-point #f)

      (post-fun
        (lambda (dc)
          (if (not (built? self))
            (draw-built dc main))
          (if (not (delayed? (current-of self)))
            (to-next-item self))
          (if (built? self)
            (if (eq? self main)
              (begin
                (set! is-packing? #t)
                (pack main))
              (unselect-mode dc))
            (if (eq? loop-point (current-of self))
              (unstabilize-all dc main)
              (select-mode
                (list
                  (cons 'pre  pre-fun)
                  (cons 'post post-fun))
                dc)))))

      (pre-fun
        (lambda (dc)
          (if (and (not (automatic? self))
                   (not (get-property (omit? (current-of self))))
                   (put-hint (get-property (hint-of (current-of self))))
                   (build dc (current-of self) main self))
            (begin
              (set-reisshina-active #f)
              (set! loop-point #f))
            (begin
              (if (not loop-point)
                (set! loop-point (current-of self)))
              (unselect-mode dc))))))

    (if (built? main)
      (begin
        (set! is-packing? #t)
        (pack main))
      (select-mode
        (list
          (cons 'pre  pre-fun)
          (cons 'post post-fun))
        dc))))

(define-method pack ((self <Container>))
  (and-let*
    ((tlst (list-select (contents-of self) (lambda (x) (not (get-property (omit? x))))))
     (plst (map pack tlst))
     (every (lambda (x) x) plst))
    (cons #:container (append (list plst) (call-next-method)))))

(define-method unpack ((self <Container>) (items <List>))
  (and
    (eqv? (car items) #:container)
    (or
      (automatic? self)
      (let loop ((itms (cadr items)) (cont (contents-of self)))
        (cond
          ((null? cont) (and (null? itms) (built? self) building-env))
          ((get-property (omit? (car cont))) (loop itms (cdr cont)))
          ((null? itms) #f)
          (else ;;(tk-box (cons (car cont) (car itms)) (not (not (unpack (car cont) (car itms)))))
                (and (or (is-a? (car cont) <Container>)
                         (unpack (car cont) (car itms))
                         (automatic? (car cont)))
                     (loop (cdr itms) (cdr cont))))))
      (and-let*
        ((cont (car (version-of self)))
         (conv (or (cdr (version-of self)) (lambda () #t)))
         ((set-object-type (get-class) (get-status)))
         ((unpack cont items))
         ((conv)))
        building-env))))
;;====================================================================
(define-method build (dc (self <Coord>) (main <Container>)(cont <Container>))
  (and
    (or (delayed? self)
        (and (not (built? self)) (built-properties? (dependence-of self))))
    (let
      ( (post-fun
          (lambda (dc)
            (unselect-mode dc)))

        (on-char
          (lambda (dc event)
            (case (event-key event)
              ((#\esc)
               (breake-building dc main))
              ((#\bs)
               (if (memq 'shift (event-state event))
                 (if (to-previous-item cont)
                   (begin
                     (draw-building dc main)
                     (mark-coord-defined! self main)
                     (draw-rebuilt dc main)
                     (crush-coord! self)
                     (delay-item (current-of cont) cont)
                     (unselect-mode dc)))
                 (begin
                   (draw-building dc main)
                   (mark-coord-defined! self main)
                   (draw-rebuilt dc main)
                   (move-to self (get-curr-x) (get-curr-y))
                   (draw-building dc main))))
              ((#\del)
               (if (to-previous-item cont)
                 (begin
                   (draw-building dc main)
                   (delay-item self cont)
                   (draw dc self)
                   (delay-item (current-of cont) cont)
                   (unselect-mode dc)))))))

        (on-expose
          (lambda (dc event)
            (put-hint (get-property (hint-of (current-of cont))))
            (draw dc main)))

        (on-motion
          (lambda (dc event)
            (draw-building dc main)
            (move-to self (event-x event) (event-y event))
            (draw-building dc main)))

        (on-button-press
          (lambda (dc event)
            (if (not (or (memq 'shift (event-state event))
                         (memq 'control (event-state event))))
              (case (event-button event)
                ((1)
                 (draw-building dc main)
                 (move-to self (event-x event) (event-y event))
                 ;(if (test-point-object (origin-of self) #t)
                 ;  (draw-building dc main)
                   (unselect-mode dc))
                 ;  )
                ((3)
                 (breake-building dc main)))))))

      (when (and (delayed? self) (built? self))
        (reenter self cont)
        (mark-coord-defined! self main)
        (draw-rebuilt dc main)
        (set-cursor! (origin-of self)))

      (draw-building dc main)

      (select-mode
        (list
          (cons 'post           post-fun)
          (cons 'button-press   on-button-press)
          (cons 'motion         on-motion)
          (cons 'expose         on-expose)
          (cons 'key-press      on-char))
        dc))))
;;====================================================================
;;; End of code
