;;====================================================================
;; <Reference>
;;
(define-class <Reference> (<Prototype>)
  ((refer #:type procedure-OR-integer?
          #:initform 0
          #:initarg #:ref
          #:getter 'ref-of)))

(define-method pack ((self <Reference>))
  (cons (cons #:reference (get-property (ref-of self)))
        (call-next-method)))
;;====================================================================
;; <Icon>
;;
(define-class <Icon> (<Reference> <Coord>)
  ())

(define-method set-default! ((self <Icon>))
  (call-next-method)
  (set-property! (ref-of self) 0))

(define-method pack ((self <Icon>))
  (and-let*
    ((nm (call-next-method)))
    (cons #:Icon nm)))

(define-method unpack ((self <Icon>) (src <List>))
  (and (or is-convertion? (eqv? (car src) #:Icon))
       (call-next-method)))

(define-method draw (dc (self <Icon>))
  (and
    (call-next-method)
    (built? self)
    (draw-icon (get-property (ref-of self)) (origin-of self) obj-scale-factor)))
;;====================================================================
