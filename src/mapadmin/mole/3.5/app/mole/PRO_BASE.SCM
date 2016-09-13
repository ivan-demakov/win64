;;;
;;; Pro-base.scm
;;; Base for draw primitive prototypes
;;;
;;; Copyright (C) 1997, A.Tarnovski
;;;
;;====================================================================
(define obj-scale-factor 1)
(define (set-obj-scale-factor! f)
  (set! obj-scale-factor f))
;;====================================================================
;; <Prototype>
;;
(define-class <Prototype> ()
  ((h-visible #:type procedure-OR-boolean?
              #:initform #t
              #:initarg #:h-visible
              #:getter 'h-visible?)
   (l-visible #:type procedure-OR-boolean?
              #:initform #t
              #:initarg #:l-visible
              #:getter 'l-visible?)
   (s-visible #:type procedure-OR-boolean?
              #:initform #f
              #:initarg #:s-visible
              #:getter 's-visible?)
   (omit      #:type procedure-or-boolean?
              #:initform #f
              #:initarg #:omit
              #:getter 'omit?)
   (label     #:type (lambda (x) (symbol? x))
              #:initform '?
              #:initarg #:label
              #:getter 'label-of)
  (dependence #:type (lambda (x) (list? x))
              #:initform '()
              #:initarg #:dependence
              #:getter 'dependence-of)
   (tool      #:type (lambda (x) (or (integer? x) (symbol? x)))
              #:initform -1
              #:initarg #:tool
              #:getter 'tool-of)
   (a-grid    #:type pair-of-int?
              #:initform '(1 . 0)
              #:initarg #:a-grid
              #:getter 'a-grid-of)
   (angle     #:type procedure-OR-symbol-OR-integer?
              #:initform 0
              #:initarg #:angle
              #:getter 'angle-of)
   (delayed   #:type <Boolean>
              #:initform #f
              #:initarg #:delayed
              #:getter 'delayed?
              #:setter 'set-delay!)
   (dynamic   #:type <Boolean>
              #:initform #f
              #:initarg #:dynamic
              #:getter 'dynamic?)
   (status    #:initform #:process-status
              #:initarg #:status
              #:getter 'status-of
              #:setter 'set-status!)
   (changed   #:type <Boolean>
              #:initarg #:changed
              #:initform #t
              #:getter 'changed?
              #:setter 'set-changed!)
   (hint      #:type (lambda (x) (or (string? x) (procedure? x)))
              #:initform ""
              #:initarg #:hint
              #:getter 'hint-of)))

(define-generic build (dc (self <Prototype>) main cont))
(define-generic origin-of ((self <Prototype>)))
(define-generic set-text-edit-point! ((self <Prototype>))
  #t)
(define-generic set-default! ((self <Prototype>))
  (set-property! (angle-of self) 0))

(define-generic set-text-changed! ((self <Prototype>) (ch? <boolean>))
  (set-changed! self ch?))

(define-generic a-kvant ((self <Prototype>) (coord <Number>))
  (std-angle
    (kvant coord (car (a-grid-of self)) (+ (cdr (a-grid-of self))(get-reisshina-angle)))))

(define (set-property! property . val)
  (if (and (pair? property) (symbol? (car property)))
    (set! property (car property)))
  (if (symbol? property)
    (if (null? val)
      (set! building-env (assv-remove! building-env property))
      (if (car val)
        (set! building-env (assv-set! building-env property (car val)))))))

(define (get-property-from env property)
  (if (and (pair? property) (symbol? (car property)))
    (or (get-property-from env (cdr property))
        (get-property-from env (car property)))
    (cond
      ((symbol? property) (assv-ref env property))
      ((procedure? property) (property))
      (else property))))

(define (get-property property)
  (get-property-from building-env property))

(define (built-properties? properties)
  (every get-property properties))

;; draw dc Prototype
;; draw Prototype in draw-context DC
;;
(define-generic draw (dc (self <Prototype>))
  (set-draw-mode! dc #:xor)
  (and (not (get-property (omit? self)))
       (built-properties? (dependence-of self))
       (if hi-resolution?
         (get-property (h-visible? self))
         (get-property (l-visible? self)))))

;; built? Prototype
;; test Prototype to be already built
;;
(define-generic built? ((self <Prototype>))
  (built-properties? (dependence-of self)))

(define-generic delay-item ((self <Prototype>) cont)
  (set-status! self #:stabil-status)
  (set-delay! self #t)
  (set-del-num! cont (inc (del-num-of cont))))

(define-generic reenter ((self <Prototype>) cont)
  (set-status! self #:rebuild-status)
  (set-delay! self #f)
  (set-del-num! cont (dec (del-num-of cont))))

;; pack Prototype list
;; pack Prototype to dst
;;
(define-generic pack ((self <Prototype>))
  (list (cons #:h-visible (get-property (h-visible? self)))
        (cons #:l-visible (get-property (l-visible? self)))
        (cons #:s-visible (get-property (s-visible? self)))))

;; unpack Prototype list
;; unpack Prototype from src
;;
(define-generic unpack ((self <Prototype>) src)
;  (set-property! (h-visible? self) (assv-ref src #:h-visible))
;  (set-property! (l-visible? self) (assv-ref src #:l-visible))
;  (set-property! (s-visible? self) (assv-ref src #:s-visible))
  building-env)

;; move-to Prototype x y
;; moves Prototype at (x, y) point
;;
(define-generic move-to ((self <Prototype>) (x <Number>) (y <Number>))
  (set-car! (origin-of self) x)
  (set-cdr! (origin-of self) y))

(define-generic automatic? ((self <Prototype>))
  #t)

(define-generic invisible? ((self <Prototype>))
  (not (or (get-property (h-visible? self))
           (get-property (l-visible? self)))))
;;====================================================================
;; <Color>
;;
(define-class <Color> (<Prototype>)
  ((color #:type procedure-OR-symbol-OR-integer?
          #:initform 0
          #:initarg #:color
          #:getter 'color-of)))

(define-method set-default! ((self <Color>))
  (call-next-method)
  (set-property! (color-of self) 0))

(define-method draw (dc (self <Color>))
  (and-let*
    (((call-next-method))
     (color (alloc-color dc (if (eq? (status-of self) #:process-status)
                               edition-color stabil-color))))
    (set-foreground! dc color)))

(define-method pack ((self <Color>))
  (and-let*
    ((color (get-property (color-of self)))
     (nm (call-next-method)))
    (cons (cons #:color color) nm)))

(define-method unpack ((self <Color>) (src <List>))
  (and-let*
    ((data (assv-ref src #:color))
     ((integer? data)))
    (set-property! (color-of self) data))
  (call-next-method))
;;====================================================================
;;<Fore-Back>
;;
(define-class <Fore-Back> (<Prototype>)
  ((fore #:type procedure-OR-symbol-OR-integer?
         #:initform 0
         #:initarg #:foreground
         #:getter 'foreground-of)
   (back #:type procedure-OR-symbol-OR-integer?
         #:initform 0
         #:initarg #:background
         #:getter 'background-of)))

(define-method set-default! ((self <Fore-Back>))
  (call-next-method)
  (set-property! (foreground-of self) 0)
  (set-property! (background-of self) 0))

(define-method draw (dc (self <Fore-Back>))
  (let
    ((process? (eq? (status-of self) #:process-status)))
    (and-let*
      (((call-next-method)))
      (set-foreground! dc (if process? edition-color stabil-color))
      (set-background! dc (if process? edition-color stabil-color)))))

(define-method pack ((self <Fore-Back>))
  (and-let*
    ((fore (get-property (foreground-of self)))
     (back (get-property (background-of self)))
     (nm (call-next-method)))
    (cons (cons #:fbcolor (vector fore back)) nm)))

(define-method unpack ((self <Fore-Back>) (src <List>))
  (and-let*
    ((data (assv-ref src #:fbcolor))
     ((vector? data))
     ((= 2 (vector-length data)))
     ((integer? (vector-ref data 0)))
     ((integer? (vector-ref data 1))))
    (set-property! (foreground-of self) (vector-ref data 0))
    (set-property! (background-of self) (vector-ref data 1)))
  (call-next-method))
;;====================================================================
;; <Filled>
;;
(define-class <Filled> (<Fore-Back>)
  ((fill  #:type procedure-OR-symbol-OR-fill-style?
          #:initform #:solid
          #:initarg #:fill
          #:getter 'fill-of)))

(define-method set-default! ((self <Filled>))
  (call-next-method)
  (set-property! (fill-of self) #:solid))

(define-method draw (dc (self <Filled>))
  (and
    (call-next-method)
    (set-fill-style! dc (get-property (fill-of self)))))

(define-method pack ((self <Filled>))
  (and-let*
    ((fill (get-property (fill-of self)))
     (nm (call-next-method)))
    (cons (cons #:fill fill) nm)))

(define-method unpack ((self <Filled>) (src <List>))
  (and-let*
    ((data (assv-ref src #:fill))
     ((fill-style? data)))
    (set-property! (fill-of self) data))
  (call-next-method))
;;====================================================================
;; <Stroked>
;;
(define-class <Stroked> (<Prototype>)
  ((line #:type procedure-OR-symbol-OR-line-style?
         #:initform #:solid
         #:initarg #:line
         #:getter 'line-of)

   (width #:type procedure-OR-symbol-OR-integer?
          #:initform 1
          #:initarg #:width
          #:getter 'width-of)))

(define-method set-default! ((self <Stroked>))
  (call-next-method)
  (set-property! (width-of self) 0)
  (set-property! (line-of  self) #:solid))

(define-method draw (dc (self <Stroked>))
  (and-let*
    (((call-next-method))
     (wd (* obj-scale-factor (get-property (width-of self))))
     (ln (get-property (line-of self)))
     (dsh? #t))
    (set! dsh? (and (line-style? ln) (not (eqv? ln #:solid))))
    (set-line-style! dc wd (if dsh? #:line-dash #:line-solid))
    (if dsh?
      (set-dashes! dc ln))
    #t))

(define-method pack ((self <Stroked>))
  (and-let*
    ((width (get-property (width-of self)))
     (line  (get-property (line-of  self)))
     (nm (call-next-method)))
    (cons (cons #:strok (vector width line)) nm)))

(define-method unpack ((self <Stroked>) (src <List>))
  (and-let*
    ((data (assv-ref src #:strok))
     ((vector? data))
     ((= 2 (vector-length data)))
     (v0 (vector-ref data 0))
     (v1 (vector-ref data 1))
     ((integer? v0))
     ((or (integer? v1) (line-style? v1))))
    (set-property! (width-of self) v0)
    (set-property! (line-of  self) v1))
  (call-next-method))
;;====================================================================
;;; End of code
