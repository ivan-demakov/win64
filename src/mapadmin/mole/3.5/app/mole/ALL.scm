;;======================================================================
;; Объекты телефонных сетей
;;======================================================================
(define (get-x)(get-property 'x))
(define (get-y)(get-property 'y))
(define (get-x0)(get-property 'x0))
(define (get-y0)(get-property 'y0))
(define (get-x1)(get-property 'x1))
(define (get-y1)(get-property 'y1))
(define (get-a)(or (get-property 'a) 0))
(define (get-tx)(get-property 'tx))
(define (get-ty)(get-property 'ty))
(define (get-ta)(or (get-property 'ta) 0))
(define (get-number) (get-property 'number))
(define a-grid '(10 . 0))
(define (ask-number)
  (if (not (get-number))
    (set-property! 'number "авто"))
  (get-number))

(define (ask-ta)
  (if is-convertion?
    (set-property! 'ta (get-property 'vsb-a)))
  (get-ta))
;;=====================================================================
(define (owner->color)
  (case (logand (get-status) #x7f)
    ((0)  10000)
    ((1)  10001)    ; 0000a0
    ((2)  10002)    ; 0000a0
    ((3)  10003)    ; 00a000
    ((4)  10004)    ; 00a000
    ((5)  10005)    ; 00a000
    ((6)  10006)    ; 00a000
    ((7)  10007)    ; 00a000
    ((8)  10008)    ; 00a000
    ((9)  10009)    ; 00a000
    ((10) 10010)    ; 00a000
    ((11) 10011)    ; 00a000
    ((12) 10012)    ; 00a000
    ((13) 10013)    ; 00a000
    ((14) 10014)    ; 00a000
    ((15) 10015)    ; 00a000
    ((16) 10016)    ; 00a000
    ((17) 10017)    ; 00a000
    ((18) 10018)    ; 00a000
    ((19) 10019)    ; 00a000
    ((20) 10020)    ; 00a000
    (else 10021)))  ; a00000
;;======================================================================
(define scale-font-factor 2)
(define text-color 0)
(define low-font-size 420)
(define low-font-size/2 (/ low-font-size 3))
(define low-font/2 (vector 0 (- low-font-size/2 )))
(define (scale-font) (if hi-resolution? (quotient low-font-size scale-font-factor) low-font-size))
(define std-font (vector 0 (- low-font-size)))
(define (h-std-font) (vector 0 (- (scale-font))))
(define (h-std-font/2) (vector 0 (inexact->exact (* -1/2 (scale-font)))))
;;=====================================================================
(define (type->index lst)
  (and-let*
    ((ml (memq (get-class) lst)))
    (- (length lst) (length ml))))
;;=====================================================================
(define (calc-angle pr)
  (let
    ((pl (get-property pr)))
    (if (and (list? pl) (> (length pl) 1))
      (calc-a (car pl) (cadr pl))
      0)))
;;=====================================================================
(define (out-of-line? pl pw)
   (let*
     ((p0 (car pl))
      (p1 (cadr pl))
      (h (if (negative? (car p1)) (cdr pw) (- (car pw))))
      (delta (* low-font-size 1))
      (p2 (shift p0 (turn-point (+ h delta) 0 0 0 (+ 900 (good-angle (calc-angle 'apl))))))
      (d (dist-to-line (get-property 'tx1) (get-property 'ty1) p2 p1)))
     (> (abs d) delta)))
;;=====================================================================
(define (calc-plg pl k m)
  (let
    ((p0 (cons (get-x0) (get-y0)))
     (a (get-a)))
    (define (clcp pt)
      (shift p0 (turn (map (lambda (v) (/ (* v k) m)) pt) a)))
    (map clcp pl)))
;;== ATS ==============================================================
(let*
  ((scale-factor 6)
   (line-width (* 5 scale-factor))
   (line-width-h (* 2 scale-factor))
   (line-color 0)
   (back-color 1)
   (k (sqrt 3.0))
   (r (* 30 scale-factor))
   (rr (/ r 2))
   (r-h (* 15 scale-factor))
   (rr-h (/ r-h 2))
   (mr (* 24 scale-factor))
   (rmr (/ mr 2))
   (mr-h (* 12 scale-factor))
   (rmr-h (/ mr-h 2))

   (tri
     (lambda (r)
       (let*
         ((d (inexact->exact (* k r))))
         (turn-points
           (list (cons d r) (cons 0 (* -2 r)) (cons (- d) r))
           (cons (get-x0) (get-y0))
           (get-a)))))

   (crs
     (lambda (r)
       (let*
         ((dy (* r .3))
          (dx (* r .8)))
         (turn-points
           (list (cons (- dx) 0) (cons dx 0)
                 (cons (- dx) dy) (cons dx dy)
                 (cons (- dx) (+ dy dy)) (cons dx (+ dy dy)))
           (cons (get-x0) (get-y0))
           (get-a)))))

   (rrl
     (lambda (r)
       (let*
         ((dy (* r .5))
          (dx (* r 1.4)))
         (turn-points
           (list (cons (- dx) dy) (cons dx dy)
                 (cons 0 (* r -2)) (cons 0 (* r -3))
                 (cons 0 (* r -3)) (cons r (* r -3)))
           (cons (get-x0) (get-y0))
           (get-a)))))

   (tri-m
     (lambda (r mr)
       (let*
         ((d (inexact->exact (* k r)))
          (md (inexact->exact (* k mr)))
          (dr (- (* 3 r) (* 2 mr))))
         (turn-points
           (list (cons md mr) (cons 0 (* -2 mr)) (cons (- md) mr)
                 (cons md mr) (cons d dr) (cons (- d) dr) (cons (- md) mr))
           (cons (get-x0) (get-y0))
           (get-a)))))

   (cross (lambda () (crs r)))
   (cross-h (lambda () (crs r-h)))
   (rrls (lambda () (rrl rr)))
   (rrls-h (lambda () (rrl rr-h)))

   (full-number (lambda (type)
     (lambda ()
       (string-append
         (cond
           ((= type MOLE-ATS) "АТС ")
           ((= type MOLE-CROSS) "Кросс ")
           ((= type MOLE-RRS) "РРС ")
           (else ""))
         (get-number)))))

   (mk-hint0 (lambda (type)
     (cond
       ((= type MOLE-ATS) "АТС")
       ((= type MOLE-CROSS) "Кросс")
       ((= type MOLE-RRS) "РРС")
       (else ""))
     ))

   (mk-hint (lambda (type)
     (cond
       ((= type MOLE-ATS) "Номер АТС")
       ((= type MOLE-CROSS) "Номер кросса")
       ((= type MOLE-RRS) "Номер РРС")
       (else ""))
     ))

   (old-old-ats
     (make <Container>
       #:contents
       (list
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Polygon>)
         (make <Color-Ring>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>)
         (make <Color-Polyline>))))

   (old-ats
     (make <Container>
       #:version (cons old-old-ats #f)
       #:contents
       (list
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Polygon>)
         (make <Outlined-Polygon>)
         (make <Color-Ring>)
         (make <Color-Ring>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>)
         (make <Color-Polyline>))))

   (pr0 (lambda (type)
     (make <Container>
       #:version (cons old-ats #f)
       #:contents
       (append
         (list
           (make <Color-Footnote>
             #:tool 't0
             #:dependence '(x0 y0 number tx ty ta)
             #:line  #:solid
             #:color owner->color
             #:width 0
             #:coord-list
               (cons
                 'vpl
                 (lambda ()
                   (make-footnote
                     (get-x0) (get-y0)
                     (lim-tx) (lim-ty) (get-ta)
                     ((full-number type)) (scale-font) #f))))
            (make <Color-Rectangle>
              #:h-visible #f
              #:l-visible #f
              #:x-coord 'x0
              #:y-coord 'y0
              #:angle   'a
              #:a-grid  a-grid
              #:hint (mk-hint0 type))
            (make <Outlined-Polygon>
              #:h-visible #f
              #:l-visible #t
              #:color      owner->color;;line-color
              #:width      line-width
              #:foreground back-color
              #:background back-color
              #:dependence '(x0 y0 a)
              #:coord-list (lambda () (tri (if (= type MOLE-RRS) rr r))))
            (make <Outlined-Polygon>
              #:h-visible #t
              #:l-visible #f
              #:color      owner->color;;line-color
              #:width      line-width-h
              #:foreground back-color
              #:background back-color
              #:dependence '(x0 y0 a)
              #:coord-list (lambda () (tri (if (= type MOLE-RRS) rr-h r-h)))))
         (cond
           ((= type MOLE-ATS)
           (list
             (make <Color-Ring>
               #:h-visible #f
               #:l-visible #t
               #:color      owner->color;;line-color
               #:width      line-width
               #:dependence '(x0 y0)
               #:x-coord    get-x0
               #:y-coord    get-y0
               #:x-size     r)
             (make <Color-Ring>
               #:h-visible #t
               #:l-visible #f
               #:color      owner->color;;line-color
               #:width     line-width-h
               #:dependence '(x0 y0)
               #:x-coord    get-x0
               #:y-coord    get-y0
               #:x-size     r-h)))
           ((= type MOLE-RRS)
           (list
             (make <Color-Arc>
               #:h-visible #f
               #:l-visible #t
               #:color      owner->color;;line-color
               #:width      line-width
               #:dependence '(x0 y0 a)
               #:x-coord  (lambda () (+ (get-x0) (car (turn (cons (* rr 2) (* rr -3)) (get-a)))))
               #:y-coord  (lambda () (+ (get-y0) (cdr (turn (cons (* rr 2) (* rr -3)) (get-a)))))
               #:x-size     rr
               #:y-size     rr
               #:angle      (lambda () (+ (get-a) 1000))
               #:delta      1600)
             (make <Color-Arc>
               #:h-visible #t
               #:l-visible #f
               #:color      owner->color;;line-color
               #:width      line-width-h
               #:dependence '(x0 y0 a)
               #:x-coord    (lambda () (+ (get-x0) (car (turn (cons (* rr-h 2) (* rr-h -3)) (get-a)))))
               #:y-coord    (lambda () (+ (get-y0) (cdr (turn (cons (* rr-h 2) (* rr-h -3)) (get-a)))))
               #:x-size     rr-h
               #:y-size     rr-h
               #:angle      (lambda () (+ (get-a) 1000))
               #:delta      1600)
             (make <Color-Lines>
               #:h-visible #f
               #:l-visible #t
               #:color      owner->color;;line-color
               #:width      line-width
               #:dependence '(x0 y0 a)
               #:coord-list rrls)
             (make <Color-Lines>
               #:h-visible #t
               #:l-visible #f
               #:color      owner->color;;line-color
               #:width      line-width-h
               #:dependence '(x0 y0 a)
               #:coord-list rrls-h)
               ))
           ((= type MOLE-CROSS)
           (list
             (make <Color-Lines>
               #:h-visible #f
               #:l-visible #t
               #:color      owner->color;;line-color
               #:width      line-width
               #:dependence '(x0 y0 a)
               #:coord-list cross)
             (make <Color-Lines>
               #:h-visible #t
               #:l-visible #f
               #:color      owner->color;;line-color
               #:width      line-width-h
               #:dependence '(x0 y0 a)
               #:coord-list cross-h))))
         (list
           (make <Text>
              #:label 't0
              #:h-visible #f
              #:l-visible #f
              #:x-coord 'tx
              #:y-coord 'ty
              #:angle   'ta
              #:a-grid  a-grid
              #:length  10
              #:hint (mk-hint type)
              #:text    'number)
           (make <Text>
              #:color owner->color
              #:dependence '(number tx ty ta)
              #:x-coord lim-tx
              #:y-coord lim-ty
              #:angle   (cons 'vsb-a ask-ta)
              #:font h-std-font ;std-font
              #:text (cons 'full-number (full-number type)))
         )))))

  (full-number-m
     (lambda (mini?)
       (string-append (if mini? "мАТС " "вАТС ") (get-number)))))

  (define (make-m-ats mini?)
    (make <Container>
      #:contents
      (list
       (make <Color-Footnote>
         #:tool 't0
         #:dependence '(x0 y0 number tx ty ta)
         #:line  #:solid
         #:color owner->color
         #:width 0
         #:coord-list
           (cons
             'vpl
             (lambda ()
               (make-footnote
                 (get-x0) (get-y0)
                 (lim-tx) (lim-ty) (get-ta)
                 (full-number-m mini?) (scale-font) #f))))
        (make <Color-Rectangle>
          #:h-visible #f
          #:l-visible #f
          #:x-coord 'x0
          #:y-coord 'y0
          #:angle   'a
          #:a-grid  a-grid
          #:hint  (if mini? "Мини-АТС" "Вынос АТС"))
        (make <Outlined-Polygon>
          #:h-visible #f
          #:l-visible #t
          #:color      owner->color;;line-color
          #:width      line-width
          #:foreground back-color
          #:background back-color
          #:dependence '(x0 y0 a)
          #:coord-list (lambda () (tri-m r mr)))
        (make <Outlined-Polygon>
          #:h-visible #t
          #:l-visible #f
          #:color      owner->color;;line-color
          #:width      line-width-h
          #:foreground back-color
          #:background back-color
          #:dependence '(x0 y0 a)
          #:coord-list (lambda () (tri-m r-h mr-h)))
        (make (if mini? <Circle> <Color-Ring>)
          #:h-visible #f
          #:l-visible #t
          #:color      owner->color;;line-color
          #:foreground owner->color;;line-color
          #:background owner->color;;line-color
          #:width      line-width
          #:dependence '(x0 y0 a)
          #:x-coord    get-x0
          #:y-coord    get-y0
          #:x-size     mr)
        (make (if mini? <Circle> <Color-Ring>)
          #:h-visible #t
          #:l-visible #f
          #:color      owner->color;;line-color
          #:foreground owner->color;;line-color
          #:background owner->color;;line-color
          #:width      line-width-h
          #:dependence '(x0 y0 a)
          #:x-coord    get-x0
          #:y-coord    get-y0
          #:x-size     mr-h)
       (make <Text>
          #:label 't0
          #:h-visible #f
          #:l-visible #f
          #:x-coord 'tx
          #:y-coord 'ty
          #:angle   'ta
          #:a-grid  a-grid
          #:length  10
          #:hint  (if mini? "Номер Мини-АТС" "Номер Выноса АТС")
          #:text    'number)
       (make <Text>
          #:color owner->color
          #:dependence '(number tx ty ta)
          #:x-coord lim-tx
          #:y-coord lim-ty
          #:angle   (cons 'vsb-a ask-ta)
          #:font h-std-font ;std-font
          #:text (cons 'full-number (lambda () (full-number-m mini?))))))
  )

  (add-object MOLE-ATS (pr0 MOLE-ATS))
  (add-object MOLE-CROSS (pr0 MOLE-CROSS))
  (add-object MOLE-RRS (pr0 MOLE-RRS))
  (add-object MOLE-MODULE-ATS (make-m-ats #f))
  (add-object MOLE-MINI-ATS   (make-m-ats #t)))

;;== УМСД =============================================================
(let*
  ((scale-factor 6)
   (line-width (* 5 scale-factor))
   (line-width-h (/ (* 5 scale-factor) 2))
   (line-color 0)
   (back-color 1)
   (rl (* 30 scale-factor))
   (rh (/ rl 2))
   (rrl (* 36 scale-factor))
   (rrh (/ rrl 2))
   (dl (inexact->exact (/ rl (sqrt 2))))
   (dh (/ dl 2))

   (full-number
     (lambda ()
       (string-append "УМСД " (get-number))))

   (pr1
     (make <Container>
       #:contents
       (list
        (make <Color-Footnote>
          #:tool 't0
          #:dependence '(x0 y0 number tx ty ta)
          #:line  #:solid
          #:color owner->color
          #:width 0
          #:coord-list
            (cons
              'vpl
              (lambda ()
                (make-footnote
                  (get-x0) (get-y0)
                  (lim-tx) (lim-ty) (get-ta)
                  (full-number) (scale-font) #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   'a
           #:a-grid  a-grid
           #:hint    "УМСД в помещении")
         (make <Outlined-Bar>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color;;line-color
           #:width      line-width
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:x-coord (lambda ()(calc-x (get-x0) (get-y0) (- rl) (- rl) (get-a)))
           #:y-coord (lambda ()(calc-y (get-x0) (get-y0) (- rl) (- rl) (get-a)))
           #:x-size (* rl 2)
           #:y-size (* rl 2)
           #:angle   get-a)
         (make <Outlined-Bar>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color;;line-color
           #:width      line-width-h
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:x-coord (lambda ()(calc-x (get-x0) (get-y0) (- rh) (- rh) (get-a)))
           #:y-coord (lambda ()(calc-y (get-x0) (get-y0) (- rh) (- rh) (get-a)))
           #:x-size (* rh 2)
           #:y-size (* rh 2)
           #:angle   get-a)
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color;;line-color
           #:width      line-width
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:x-coord (lambda ()(calc-x (get-x0) (get-y0) (- dl) (- dl) (+ (get-a) 450)))
           #:y-coord (lambda ()(calc-y (get-x0) (get-y0) (- dl) (- dl) (+ (get-a) 450)))
           #:x-size (* dl 2)
           #:y-size (* dl 2)
           #:angle (lambda () (+ (get-a) 450)))
         (make <Color-Rectangle>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color;;line-color
           #:width      line-width-h
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:x-coord (lambda ()(calc-x (get-x0) (get-y0) (- dh) (- dh) (+ (get-a) 450)))
           #:y-coord (lambda ()(calc-y (get-x0) (get-y0) (- dh) (- dh) (+ (get-a) 450)))
           #:x-size (* dh 2)
           #:y-size (* dh 2)
           #:angle (lambda () (+ (get-a) 450)))
        (make <Text>
           #:label 't0
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  10
           #:hint "Номер УМСД"
           #:text    'number)
        (make <Text>
           #:color owner->color
           #:dependence '(number tx ty ta)
           #:x-coord lim-tx
           #:y-coord lim-ty
           #:angle   (cons 'vsb-a ask-ta)
           #:font h-std-font
           #:text (cons 'full-number full-number))
  )))

   (pr2
     (make <Container>
       #:contents
       (list
        (make <Color-Footnote>
          #:tool 't0
          #:dependence '(x0 y0 number tx ty ta)
          #:line  #:solid
          #:color owner->color
          #:width 0
          #:coord-list
            (cons
              'vpl
              (lambda ()
                (make-footnote
                  (get-x0) (get-y0)
                  (lim-tx) (lim-ty) (get-ta)
                  (full-number) (scale-font) #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   'a
           #:a-grid  a-grid
           #:hint    "УМСД уличный")
         (make <Outlined-Circle>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color;;line-color
           #:width      line-width
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size rrl)
         (make <Outlined-Circle>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color;;line-color
           #:width      line-width-h
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size rrh)
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color;;line-color
           #:width      line-width
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:x-coord (lambda ()(calc-x (get-x0) (get-y0) (- dl) (- dl) (+ (get-a) 450)))
           #:y-coord (lambda ()(calc-y (get-x0) (get-y0) (- dl) (- dl) (+ (get-a) 450)))
           #:x-size (* dl 2)
           #:y-size (* dl 2)
           #:angle (lambda () (+ (get-a) 450)))
         (make <Color-Rectangle>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color;;line-color
           #:width      line-width-h
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:x-coord (lambda ()(calc-x (get-x0) (get-y0) (- dh) (- dh) (+ (get-a) 450)))
           #:y-coord (lambda ()(calc-y (get-x0) (get-y0) (- dh) (- dh) (+ (get-a) 450)))
           #:x-size (* dh 2)
           #:y-size (* dh 2)
           #:angle (lambda () (+ (get-a) 450)))
        (make <Text>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  10
           #:hint "Номер УМСД"
           #:text    'number)
        (make <Text>
           #:color owner->color
           #:dependence '(number tx ty ta)
           #:x-coord lim-tx
           #:y-coord lim-ty
           #:angle   (cons 'vsb-a ask-ta)
           #:font h-std-font
           #:text (cons 'full-number full-number))
  ))))

  (add-object MOLE-INSIDE-MSU pr1)
  (add-object MOLE-OUTSIDE-MSU pr2))

;;== SAFE =============================================================
(let*
  ((scale-factor 4)
   (line-width (* 5 scale-factor))
   (line-color 0)
   (back-color 1)
   (h (* scale-factor 80))
   (w (* scale-factor 40))
   (rw 40)
   (rh 100)
   (DS (list MOLE-OUTSIDE-SAFE MOLE-OUTSIDE-OPTIC-SAFE))
   (OS (list MOLE-INSIDE-OPTIC-SAFE MOLE-OUTSIDE-OPTIC-SAFE))

   (inout (lambda (w h alt?)
     (define ptsn
        '#((1 2 4 5)
           (1 0 6 5)
           (0 4 6 2)
           (2 6 0 4)
           (0 2 4)
           (0 6 4)
           (1 3 5 7)
           (0 2 4 6 7 5 3 1 7)))
     (let*
       ((ot? (memq? (get-class) DS))
        (op? (memq? (get-class) OS))
        (h/2 (/ h 2))
        (w/2 (/ w 2))
        (-h/2 (- h/2))
        (-w/2 (- w/2))
        (p0 (cons (get-x0) (get-y0)))
        (a (get-a))
        (d0 (cons -w/2 -h/2))
        (d1 (cons    0 -h/2))
        (d2 (cons  w/2 -h/2))
        (d3 (cons  w/2    0))
        (d4 (cons  w/2  h/2))
        (d5 (cons    0  h/2))
        (d6 (cons -w/2  h/2))
        (d7 (cons -w/2    0))
        (dv (vector d0 d1 d2 d3 d4 d5 d6 d7))
        (cs (+ (if op? 4 0) (if ot? 2 0) (if alt? 1 0))))
       (map
         (lambda (n) (shift p0 (turn (vector-ref dv n) a)))
         (vector-ref ptsn cs)))))

   (full-number
     (lambda ()
       (string-append
         (assv-ref
           `((,MOLE-OUTSIDE-SAFE       . "РШ " )
             (,MOLE-INSIDE-SAFE        . "РШ " )
             (,MOLE-OUTSIDE-OPTIC-SAFE . "ШТК ")
             (,MOLE-INSIDE-OPTIC-SAFE  . "ШТК ")
             (,MOLE-MUFT-SAFE . "ШРМ "))
            (get-class))
         (get-number))))

   (hint
     (lambda ()
         (assv-ref
           `((,MOLE-OUTSIDE-SAFE       . "Шкаф уличный")
             (,MOLE-INSIDE-SAFE        . "Шкаф в помещении")
             (,MOLE-OUTSIDE-OPTIC-SAFE . "Шкаф телекоммуникационный уличный")
             (,MOLE-INSIDE-OPTIC-SAFE  . "Шкаф телекоммуникационный в помещении")
             (,MOLE-MUFT-SAFE          . "Шкаф для размещения муфт"))
            (get-class))))

   (old-old-safe
     (make <Container>
       #:contents
       (list
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Bar>)
         (make <Filled-Polygon>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>)
         (make <Color-Polyline>))))

   (old-safe
     (make <Container>
       #:version (cons old-old-safe #f)
       #:contents
       (list
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Bar>)
         (make <Outlined-Bar>)
         (make <Filled-Polygon>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>)
         (make <Color-Polyline>))))

   (old-safe-1
     (make <Container>
       #:version (cons old-safe #f)
       #:contents
       (list
         (make <Color-Polyline>)
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Bar>)
         (make <Outlined-Bar>)
         (make <Filled-Polygon>)
         (make <Filled-Polygon>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>))))

   (old-safe-2
     (make <Container>
       #:version (cons old-safe-1 #f)
       #:contents
       (list
         (make <Color-Polyline>)
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Bar>)
         (make <Outlined-Bar>)
         (make <Filled-Polygon>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>))))

   (old-safe-2->safe
     (lambda ()
       (set-property! 'x1 100)
       (set-property! 'y1 100)))

   (pr
     (make <Container>
       #:version (cons old-safe-2 old-safe-2->safe)
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (full-number) (scale-font) #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   'a
           #:a-grid  a-grid
           #:hint hint)
         (make <Outlined-Polygon>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color;;line-color
           #:foreground owner->color;;line-color
           #:background owner->color;;line-color
           #:dependence '(x0 y0 a)
           #:coord-list (lambda () (inout w h #f)))
         (make <Outlined-Polygon>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color;;line-color
           #:foreground owner->color;;line-color
           #:background owner->color;;line-color
           #:dependence '(x0 y0 a)
           #:coord-list (lambda () (inout rw rh #f)))
         (make <Outlined-Polygon>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color;;line-color
           #:foreground 1
           #:background 1
           #:dependence '(x0 y0 a)
           #:coord-list (lambda () (inout w h #t)))
         (make <Outlined-Polygon>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color;;line-color
           #:foreground 1
           #:background 1
           #:dependence '(x0 y0 a)
           #:coord-list (lambda () (inout rw rh #t)))
         (make <Text>
           #:label 't0
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  10
           #:hint "Номер шкафа"
           #:text    'number)
         (make <Text>
           #:color owner->color
           #:dependence '(tx ty ta number)
           #:x-coord lim-tx
           #:y-coord lim-ty
           #:angle   (cons 'vsb-a ask-ta)
           #:font h-std-font ;std-font
           #:text (cons 'full-number full-number)))))

   (prm
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (full-number) (scale-font) #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   'a
           #:a-grid  a-grid
           #:hint hint)
         (make <Outlined-Bar>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color;;line-color
           #:foreground 1
           #:dependence '(x0 y0 a)
           #:x-coord  (lambda () (calc-x (get-x0) (get-y0) (/ w -2) (/ h -2) (get-a)))
           #:y-coord  (lambda () (calc-y (get-x0) (get-y0) (/ w -2) (/ h -2) (get-a)))
           #:angle get-a
           #:x-size w
           #:y-size h)
         (make <Outlined-Bar>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color;;line-color
           #:foreground 1
           #:dependence '(x0 y0 a)
           #:x-coord  (lambda () (calc-x (get-x0) (get-y0) (/ rw -2) (/ rh -2) (get-a)))
           #:y-coord  (lambda () (calc-y (get-x0) (get-y0) (/ rw -2) (/ rh -2) (get-a)))
           #:angle get-a
           #:x-size rw
           #:y-size rh)
         (make <Circle>
           #:h-visible #f
           #:l-visible #t
           #:foreground 0
           #:dependence '(x0 y0 a)
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size (/ w 4))
         (make <Circle>
           #:h-visible #t
           #:l-visible #f
           #:foreground 0
           #:dependence '(x0 y0 a)
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size (/ rw 4))
         (make <Text>
           #:label 't0
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  10
           #:hint "Номер шкафа"
           #:text    'number)
         (make <Text>
           #:color owner->color
           #:dependence '(tx ty ta number)
           #:x-coord lim-tx
           #:y-coord lim-ty
           #:angle   (cons 'vsb-a ask-ta)
           #:font h-std-font ;std-font
           #:text (cons 'full-number full-number))))

  ))

  (add-object
    (list MOLE-INSIDE-SAFE
          MOLE-OUTSIDE-SAFE
          MOLE-INSIDE-OPTIC-SAFE
          MOLE-OUTSIDE-OPTIC-SAFE)
    pr)
  (add-object MOLE-MUFT-SAFE prm))
;;== WELLS ================================================================
(let*
  ((back-color 1)
   (line-color 0)
   (line-width 7)
   (number-size 15)
   (well-list (list MOLE-KKS1
                    MOLE-KKS2
                    MOLE-KKS3
                    MOLE-KKS4
                    MOLE-KKS5
                    MOLE-KKSS1
                    MOLE-KKSS2))

   (input-list (list MOLE-OTHER-INSPECT-DEV
                     MOLE-CABEL-INPUT-IN-CELLAR
                     MOLE-CABEL-INPUT-ON-WALL
                     MOLE-AIR-CABEL-INPUT
                     MOLE-SOLDER-POINT))

   (choice (lambda (p1 p2)
     (or (get-property p1)
         (set-property! p1 (get-property p2)))))

   (type-index
     (lambda ()
       (or (type->index well-list)
           (and (type->index input-list)
                (+ (length well-list) (type->index input-list)))
           0)))

   (well-type
     (lambda ()
        (vector-ref
          '#("1" "2" "3" "4" "5" "С1" "С2" "12" "91" "92" "93" "7")
          (type-index))))

   (roof-size
     (lambda ()
        (vector-ref
          '#(35 35 35 35 35 35 35 35 25 25 25 35)
          (type-index))))

   (roof-bound
     (lambda ()
        (vector-ref
          '#(20 20 20 20 20 20 20 20 10 10 10 20)
          (type-index))))

   (scale-factor
     (lambda ()
       (if (<= 7 (type-index)) 6 8)))

   (hint
     (lambda ()
       (vector-ref '#("ККС-1"
                      "ККС-2"
                      "ККС-3"
                      "ККС-4"
                      "ККС-5"
                      "ККСС-1"
                      "ККСС-2"
                      "Другое смотровое устройство"
                      "Каб. ввод в подвал"
                      "Каб. ввод на стену"
                      "Каб. ввод воздушный"
                      "Специальное смотровое устройство")
                    (type-index))))

   (full-number
     (lambda (type)
       (string-append "(" type ") " (ask-number))))

   (well-plan
     (lambda ()
       (define coord-list-vec
         `#(((-38 . -38) (38 . -38) (38 . 38) (-38 . 38) (-38 . -38))
            ((-35 . -68) (-52 . -45) (-52 . 45) (-35 . 68) (35 . 68) (52 . 45) (52 . -45) (35 . -68) (-35 . -68))
            ((-39 . -98) (-58 . -55) (-58 . 55) (-39 . 98) (39 . 98) (58 . 55) (58 . -55) (39 . -98) (-39 . -98))
            ((-43 . -120) (-65 . -80) (-65 . 80) (-43 . 120) (43 . 120) (65 . 80) (65 . -80) (43 . -120) (-43 . -120))
            ((-53 . -150) (-80 . -100) (-80 . 100) (-53 . 150) (53 . 150) (80 . 100) (80 . -100) (53 . -150) (-53 . -150))
            ((-105 . -168) (105 . -168) (105 . 228) (-105 . 228) (-105 . -168))
            ((-105 . -192) (105 . -192) (105 . 384) (-105 . 384) (-105 . -192))
            ()
            ((-60 . 60) (-60 . -60) (60 . -60) (60 . 60))
            ((-87 . 0) (43 . -74) (43 . 74))
            ()
            ()))

       (define (calc-coord p)
         (shift (cons (get-x0) (get-y0)) (turn p (get-a))))

       (if (or (<= 7 (type-index))
               hi-resolution?)
         (map calc-coord (vector-ref coord-list-vec (type-index)))
         '())))

   (old-old-old-well
     (make <Container>
       #:contents
       (list
         (make <Outlined-Circle> #:x-coord 'x0 #:y-coord 'y0)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>)
         (make <Color-Polyline>))))

   (old-old-old->old-old
     (lambda()
       (set-property! 'a 0)))

   (old-old-well
     (make <Container>
       #:version (cons old-old-old-well old-old-old->old-old)
       #:contents
       (list
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Circle>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>)
         (make <Color-Polyline>))))

   (old-well
     (make <Container>
       #:version (cons old-old-well #f)
       #:contents
       (list
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Polygon>)
         (make <Outlined-Circle>)
         (make <Outlined-Circle>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>)
         (make <Color-Polyline>))))

   (std-well
     (make <Container>
       #:version (cons old-well #f)
       #:contents
       (list
         (make <Color-Polyline>)
         (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0 #:angle 'a)
         (make <Outlined-Polygon>)
         (make <Outlined-Circle>)
         (make <Outlined-Circle>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>))))

   (nstd-well
     (make <Container>
       #:version (cons std-well (lambda () (choice 'x2 'x0)
                                           (choice 'y2 'y0)
                                           (choice 'x3 'x0)
                                           (choice 'y3 'y0)))
       #:contents
         (list
           (make <Color-Footnote>)
           (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0
                                   #:x-size 'x1 #:y-size 'y1 #:angle 'a)
           (make <Outlined-Bar>)
           (make <Outlined-Circle>)
           (make <Outlined-Circle> #:x-coord 'x2 #:y-coord 'y2)
           (make <Outlined-Circle> #:x-coord 'x3 #:y-coord 'y3)
           (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
           (make <Text>))))

   (nstd-well-1
     (make <Container>
       #:version (cons nstd-well (lambda () (choice 'x2 'x0)
                                            (choice 'y2 'y0)))
       #:contents
         (list
           (make <Color-Footnote>)
           (make <Circle> #:x-coord 'x0 #:y-coord 'y0)
           (make <Color-Rectangle> #:x-size 'x1 #:y-size 'y1 #:angle 'a)
           (make <Outlined-Bar>)
           (make <Outlined-Circle>)
           (make <Bar>)
           (make <Outlined-Circle> #:x-coord 'x2 #:y-coord 'y2)
           (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
           (make <Text>))))

   (nstd-well-2
     (make <Container>
       #:version (cons nstd-well-1 (lambda () (choice 'x3 'x0)
                                              (choice 'y3 'y0)))
       #:contents
         (list
           (make <Color-Footnote>)
           (make <Circle> #:x-coord 'x0 #:y-coord 'y0)
           (make <Color-Rectangle> #:x-size 'x1 #:y-size 'y1 #:angle 'a)
           (make <Outlined-Bar>)
           (make <Outlined-Circle>)
           (make <Bar>)
           (make <Outlined-Circle> #:x-coord 'x2 #:y-coord 'y2)
           (make <Outlined-Circle> #:x-coord 'x3 #:y-coord 'y3)
           (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
           (make <Text>))))

   (make-well (lambda (input?)
     (make <Container>
       #:version (cons nstd-well-2 #f)
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:s-visible input?
           #:dependence '(x0 y0 number tx ty ta)
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (full-number (well-type))
                   (if input?
                     (quotient (scale-font) 2)
                     (scale-font))
                   #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  200
           #:y-size  200
           #:a-grid  a-grid
           #:angle   'a
           #:hint hint)
         (if input?
           (make <Outlined-Circle>
             #:color     owner->color;;line-color
             #:width     roof-bound
             #:foreground back-color
             #:background back-color
             #:dependence '(x0 y0 a)
             #:x-coord    get-x0
             #:y-coord    get-y0
             #:x-size     90)
           (make <Outlined-Polygon>
             #:h-visible #t
             #:l-visible #f
             #:color     owner->color;;line-color
             #:width     1
             #:foreground back-color
             #:background back-color
             #:dependence '(x0 y0 a)
             #:coord-list (cons 'plg well-plan))
         )
         (make <Outlined-Circle>
           #:h-visible #f
           #:l-visible (not input?)
           #:s-visible input?
           #:width (* (scale-factor) line-width)
           #:color      owner->color;;line-color
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0)
           #:x-coord    get-x0
           #:y-coord    get-y0
           #:x-size (cons 'r (* (scale-factor) (if input? 11 15))))
         (if input?
           (make <Filled-Polygon>
             #:foreground owner->color
             #:background owner->color
             #:dependence '(x0 y0 a)
             #:coord-list well-plan)
           (make <Outlined-Circle>
             #:h-visible #t
             #:l-visible #f
             #:width      roof-bound
             #:color      owner->color;;line-color
             #:foreground back-color
             #:background back-color
             #:dependence '(x0 y0)
             #:x-coord    get-x0
             #:y-coord    get-y0
             #:x-size     roof-size)
         )
         (make <Text>
           #:label 't0
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  number-size
           #:hint    "Номер"
           #:text   (if input? (cons 'number ask-number) 'number))
         (make <Text>
           #:s-visible input?
           #:color owner->color
           #:dependence '(tx ty ta number)
           #:x-coord lim-tx
           #:y-coord lim-ty
           #:angle   (cons 'vsb-a ask-ta)
           #:font (if input? h-std-font/2 h-std-font) ;std-font
           #:text (cons 'full-number (lambda () (full-number (well-type)))))
                   ))))

   (calc-corner (lambda (dx dy)
      (shift (cons (get-x0) (get-y0))
             (turn (cons (- dx (abs (get-x1))) (- dy (abs (get-y1)))) (get-a)))))

   (n-well
     (make <Container>
       #:version (cons (make-well #f) (lambda () (choice 'x2 'x0)
                                                 (choice 'y2 'y0)
                                                 (choice 'a 0)
                                                 (choice 'x1 200)
                                                 (choice 'y1 200)))
       #:contents
         (list
           (make <Color-Footnote>)
           (make <Color-Rectangle> #:x-coord 'x0 #:y-coord 'y0
                                   #:x-size 'x1 #:y-size 'y1 #:angle 'a)
           (make <Outlined-Bar>)
           (make <Outlined-Circle>)
           (make <Outlined-Circle> #:x-coord 'x2 #:y-coord 'y2)
           (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
           (make <Text>))))

   (nn-well (lambda (nn?)
     (make <Container>
       #:version (cons n-well (lambda () (choice 'x3 'x0)
                                         (choice 'y3 'y0)))
       #:contents
         (append
           (list
             (make <Color-Footnote>)
             (make <Circle> #:x-coord 'x0 #:y-coord 'y0)
             (make <Color-Rectangle> #:x-size 'x1 #:y-size 'y1 #:angle 'a)
             (make <Outlined-Bar>)
             (make <Outlined-Circle>)
             (make <Bar>)
             (make <Outlined-Circle> #:x-coord 'x2 #:y-coord 'y2))
           (if nn?
             (list
               (make <Outlined-Circle> #:x-coord 'x3 #:y-coord 'y3))
               '())
           (list
             (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
             (make <Text>))))))

   (s-x0 0)
   (s-y0 0)
   (s-x1 0)
   (s-y1 0)
   (s-an 0)

   (roof-test (lambda (st?)
     (define (pir? t p)
       (point-in-rect? (car t) (cdr t)
                       (car p) (cdr p)
                       (- (* (abs (get-x1)) 2) 40)
                       (- (* (abs (get-y1)) 2) 40)
                       (get-a)))
     (let*
       ((x0 (get-x0))
        (y0 (get-y0))
        (x1 (get-x1))
        (y1 (get-y1))
        (an (get-a))
        (x2 (get-property 'x2))
        (y2 (get-property 'y2))
        (x3 (get-property 'x3))
        (y3 (get-property 'y3))
        (p (and x0 x1 (calc-corner 40 40)))
        (t1 (cons x2 y2))
        (t2 (cons x3 y3)))
       (cond
         (st?
           (set! s-x0 #f)
           (set! s-y0 #f)
           (set! s-x1 #f)
           (set! s-y1 #f)
           (set! s-an #f)
            #t)
         (else
           (or s-x0 (set! s-x0 x0))
           (or s-y0 (set! s-y0 y0))
           (or s-x1 (set! s-x1 x1))
           (or s-y1 (set! s-y1 y1))
           (or s-an (set! s-an an))
           (and-let*
             (((= s-x1 x1))
              ((= s-y1 y1))
              ((= s-an an))
              (dx (- x0 s-x0))
              (dy (- y0 s-y0))
              ((not (and (zero? dx) (zero? dy)))))
             (set! s-x0 x0)
             (set! s-y0 y0)
             (when x2
               (set! t1 (cons  (+ x2 dx) (+ y2 dy)))
               (set-property! 'x2 (car t1))
               (set-property! 'y2 (cdr t1)))
             (when x3
               (set! t2 (cons  (+ x3 dx) (+ y3 dy))))
               (set-property! 'x3 (car t2))
               (set-property! 'y3 (cdr t2)))
           (or (not hi-resolution?)
               (not p)
               (and (or (not x2) (pir? t1 p))
                    (or (not x3) (pir? t2 p)))
               "Горловина не в колодце!"))))))

   (predef! (lambda ()
     (choice 'a 0)
     (choice 'x1 (and (not hi-resolution?) 200))
     (choice 'y1 (and (not hi-resolution?) 200))
     (choice 'x2 'x0)
     (choice 'y2 'y0)
     (choice 'x3 'x0)
     (choice 'y3 'y0)
     #f))

   (px-size (lambda () (/ (abs (get-x1)) 5)))
   (py-size (lambda () (/ (abs (get-y1)) 5)))

   (make-kkssn (lambda (nn?)
     (make <Container>
       #:version (cons (nn-well (not nn?)) #f)
       #:contents
       (append
         (list
           (make <Color-Footnote>
             #:tool 't0
             #:dependence '(x0 y0 number tx ty ta)
             #:color owner->color
             #:width 0
             #:coord-list
               (cons
                 'vpl
                 (lambda ()
                   (make-footnote
                     (get-x0) (get-y0)
                     (lim-tx) (lim-ty) (get-ta)
                     (full-number "8")
                     (scale-font) #f))))
           (make <Circle>
             #:label 'l0
             #:h-visible #f
             #:l-visible #f
             #:valid-test roof-test
             #:x-coord 'x0
             #:y-coord 'y0
             #:hint "Центр колодца")
           (make <Color-Rectangle>
             #:omit predef!
             #:valid-test roof-test
             #:h-visible #f
             #:l-visible #f
             #:dependence '(x0 y0)
             #:x-coord get-x0
             #:y-coord get-y0
             #:x-size  'x1
             #:y-size  'y1
             #:angle  'a
             #:hint "Размеры и ориентация колодца")
          (make <Outlined-Bar>
             #:h-visible #t
             #:l-visible #f
             #:color     owner->color;;line-color
             #:width     1
             #:foreground back-color
             #:background back-color
             #:dependence '(x0 y0 x1 y1 a)
             #:angle   get-a
             #:x-coord (cons 'left   (lambda () (car (calc-corner 0 0))))
             #:y-coord (cons 'top    (lambda () (cdr (calc-corner 0 0))))
             #:x-size  (cons 'width  (lambda () (* (abs (get-x1)) 2)))
             #:y-size  (cons 'height (lambda () (* (abs (get-y1)) 2))))
           (make <Outlined-Circle>
             #:h-visible #f
             #:l-visible #t
             #:tool 'l0
             #:width (* (scale-factor) line-width)
             #:color owner->color;;line-color
             #:foreground back-color
             #:background back-color
             #:dependence '(x0 y0)
             #:x-coord get-x0
             #:y-coord get-y0
             #:x-size (cons 'r (lambda () (* (scale-factor) 15))))
           (make <Bar>
             #:tool 'l0
             #:h-visible #t
             #:l-visible #f
             #:foreground 0
             #:background 0
             #:dependence '(x0 y0 x1 y1)
             #:x-coord (lambda () (car (calc-corner 0 0)))
             #:y-coord (lambda () (cdr (calc-corner 0 0)))
             #:angle   get-a
             #:x-size px-size
             #:y-size py-size)
           (make <Outlined-Circle>
             #:dynamic #t
             #:hint "Горловина"
             #:h-visible #t
             #:l-visible #f
             #:valid-test roof-test
             #:width 20
             #:color owner->color;;line-color
             #:foreground back-color
             #:background back-color
             #:x-coord 'x2
             #:y-coord 'y2
             #:x-size  35))
         (if nn?
           (list
             (make <Outlined-Circle>
               #:dynamic #t
               #:hint "Вторая горловина"
               #:h-visible #t
               #:l-visible #f
               #:valid-test roof-test
               #:width 20
               #:color owner->color;;line-color
               #:foreground back-color
               #:background back-color
               #:x-coord 'x3
               #:y-coord 'y3
               #:x-size  35))
             '())
         (list
           (make <Text>
             #:label 't0
             #:h-visible #f
             #:l-visible #f
             #:x-coord 'tx
             #:y-coord 'ty
             #:angle   'ta
             #:a-grid  a-grid
             #:length  number-size
             #:hint    "Номер колодца"
             #:text    'number)
           (make <Text>
             #:color owner->color
             #:dependence '(tx ty ta number)
             #:x-coord lim-tx
             #:y-coord lim-ty
             #:angle   (cons 'vsb-a ask-ta)
             #:font h-std-font ;std-font
             #:text (cons 'full-number (lambda () (full-number (if nn? "Н2" "Н1")))))
        )))))

  (nup-w 100)
  (nup-h 100)
  (nup-ww 200)
  (nup-hh 200)
  (nup-l 25)
  (nup-ll 50)

   (nup
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number)
                   (scale-font) #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  200
           #:y-size  200
           #:a-grid  a-grid
           #:angle   'a
           #:hint "НУП")
         (make <Outlined-Bar>
           #:h-visible #t
           #:l-visible #f
           #:color     owner->color;;line-color
           #:width     nup-l
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:angle   (cons 'angle get-a)
           #:x-coord (cons 'left   (lambda () (calc-x (get-x0) (get-y0) (/ nup-w -2) (/ nup-h -2) (get-a))))
           #:y-coord (cons 'top    (lambda () (calc-y (get-x0) (get-y0) (/ nup-w -2) (/ nup-h -2) (get-a))))
           #:x-size  (cons 'width  nup-w)
           #:y-size  (cons 'height nup-h))
         (make <Circle>
            #:h-visible #t
            #:l-visible #f
            #:foreground owner->color
            #:background owner->color
            #:dependence '(x0 y0)
            #:x-coord get-x0
            #:y-coord get-y0
            #:x-size  (/ nup-w 4))
         (make <Outlined-Bar>
           #:l-visible #t
           #:h-visible #f
           #:color     owner->color;;line-color
           #:width     nup-ll
           #:foreground back-color
           #:background back-color
           #:dependence '(x0 y0 a)
           #:angle   (cons 'angle get-a)
           #:x-coord (cons 'left   (lambda () (calc-x (get-x0) (get-y0) (/ nup-ww -2) (/ nup-hh -2) (get-a))))
           #:y-coord (cons 'top    (lambda () (calc-y (get-x0) (get-y0) (/ nup-ww -2) (/ nup-hh -2) (get-a))))
           #:x-size  (cons 'width  nup-ww)
           #:y-size  (cons 'height nup-hh))
         (make <Circle>
            #:l-visible #t
            #:h-visible #f
            #:foreground owner->color
            #:background owner->color
            #:dependence '(x0 y0)
            #:x-coord get-x0
            #:y-coord get-y0
            #:x-size  (/ nup-ww 4))
         (make <Text>
           #:label 't0
           #:color     owner->color;;line-color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  8
           #:hint "Номер"
           #:font h-std-font
           #:text 'number)))
   ))

  (add-object well-list   (make-well #f))
  (add-object input-list  (make-well #t))
  (add-object MOLE-KKSSN  (make-kkssn #f))
  (add-object MOLE-KKSSNN (make-kkssn #t))
  (add-object MOLE-NUP nup))

;;== MOLE-JOINT-POINT =================================================
(let*
  ((point-radius 75)
   (jp (make <Container>
         #:contents
         (list
           (make <Color-Footnote>
             #:s-visible #t
             #:tool 't0
             #:dependence '(x0 y0 number tx ty ta)
             #:color owner->color
             #:width 0
             #:coord-list
               (cons
                 'vpl
                 (lambda ()
                   (make-footnote
                     (get-x0) (get-y0)
                     (lim-tx) (lim-ty) (get-ta)
                     (get-number)
                     (scale-font) #f))))
           (make <Circle>
             #:hint "Фиктивное СУ"
             #:foreground 1000
             #:x-coord    'x0
             #:y-coord    'y0
             #:x-size     point-radius)
           (make <Text>
             #:s-visible #t
             #:label 't0
             #:x-coord 'tx
             #:y-coord 'ty
             #:angle   'ta
             #:a-grid  a-grid
             #:length  15
             #:font h-std-font/2 ;std-font
             #:hint    "Номер"
             #:text    (cons 'number ask-number))
       ))))
  (add-object MOLE-JOINT-POINT jp))

;;== SUPPORT ==========================================================

(let*
  ((r 120)
   (p (/ r 2))
   (cbr 240)
   (q (* 2 (inexact->exact (/ r 2 (sqrt 2)))))
   (line-width 20)

   (old-support
     (make <Container>
       #:contents
       (list
         (make <Circle>  #:x-coord 'x0  #:y-coord 'y0))))

   (ver0
     (make <Container>
       #:version (cons old-support #f)
       #:contents
       (list
         (make <Circle>  #:x-coord 'x0  #:y-coord 'y0)
         (make <Circle>))))

   (ver1
     (make <Container>
       #:version (cons ver0 #f)
       #:contents
       (list
         (make <Circle>  #:x-coord 'x0  #:y-coord 'y0)
         (make <Circle>)
         (make <Bar>)
         (make <Bar>))))

   (ver2
     (make <Container>
       #:version (cons ver1 #f)
       #:contents
       (list
         (make <Point>  #:x-coord 'x0  #:y-coord 'y0)
         (make <Bar>)
         (make <Bar>))))

   (ver3
     (make <Container>
       #:version (cons ver2 #f)
       #:contents
       (list
         (make <Point>  #:x-coord 'x0  #:y-coord 'y0)
         (make <Bar>)
         (make <Bar>)
         (make <Bar>)
         (make <Bar>))))

   (ver3->ver4 (lambda ()
     (set-property! 'number ".")
     (set-property! 'tx (+ (get-x0) 100))
     (set-property! 'ty (get-y0))
     (set-property! 'ta 0)))

   (ver4
     (make <Container>
       #:version (cons ver3 ver3->ver4)
       #:contents
       (list
         (make <Color-Polyline>)
         (make <Point>  #:x-coord 'x0  #:y-coord 'y0)
         (make <Bar>)
         (make <Bar>)
         (make <Bar>)
         (make <Bar>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number))))

   (ver5
     (make <Container>
       #:version (cons ver4 #f)
       #:contents
       (list
         (make <Color-Polyline>)
         (make <Circle>  #:x-coord 'x0  #:y-coord 'y0)
         (make <Circle>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number))))

   (ver6
     (make <Container>
       #:version (cons ver5 #f)
       #:contents
       (list
         (make <Color-Polyline>)
         (make <Circle>  #:x-coord 'x0  #:y-coord 'y0)
         (make <Circle>)
         (make <Bar>)
         (make <Bar>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number))))

   (a900 (lambda ()
     (+ 900 (get-a))))

   (make-support (lambda (cb?)
     (make <Container>
       #:version (cons ver6 #f)
       #:contents
       (append
         (list
           (make <Color-Footnote>
             #:s-visible #t
             #:tool 't0
             #:dependence '(x0 y0 number tx ty ta)
             #:color owner->color
             #:width 0
             #:coord-list
               (cons
                 'vpl
                 (lambda ()
                   (make-footnote
                     (get-x0) (get-y0)
                     (lim-tx) (lim-ty) (get-ta)
                     (ask-number)
                     (quotient (scale-font) 2)
                     #f))))
           (make <Circle>
             #:l-visible #t
             #:h-visible #f
             #:foreground owner->color
             #:background owner->color
             #:x-coord 'x0
             #:y-coord 'y0
             #:x-size  (cons 'r r)
             #:hint    "Опора")
           (make <Circle>
             #:l-visible #f
             #:h-visible #t
             #:dependence '(x0 y0)
             #:foreground owner->color
             #:background owner->color
             #:x-coord get-x0
             #:y-coord get-y0
             #:x-size p))
         (if cb?
           (list
             (make <Bar>
               #:l-visible #t
               #:h-visible #f
               #:dependence '(x0 y0)
               #:foreground owner->color
               #:background owner->color
               #:x-coord (lambda () (+ (get-x0) r))
               #:y-coord (lambda () (- (get-y0) r))
               #:x-size  p
               #:y-size  (* r 2))
             (make <Bar>
               #:l-visible #f
               #:h-visible #t
               #:dependence '(x0 y0)
               #:foreground owner->color
               #:background owner->color
               #:x-coord (lambda () (+ (get-x0) p))
               #:y-coord (lambda () (- (get-y0) p))
               #:x-size  (/ r 4)
               #:y-size  r)
             (make <Text>
               #:s-visible #t
               #:label 't0
               #:font low-font/2
               #:x-coord 'tx
               #:y-coord 'ty
               #:angle   'ta
               #:color owner->color
               #:a-grid  a-grid
               #:length  0
               #:hint "Номер опоры"
               #:text (cons 'number ask-number)))
           (list
             (make <Text>
               #:s-visible #t
               #:label 't0
               #:font low-font/2
               #:x-coord 'tx
               #:y-coord 'ty
               #:angle   'ta
               #:color owner->color
               #:a-grid  a-grid
               #:length  0
               #:hint "Номер опоры"
               #:text 'number)))))))

   (make-minisupport (lambda (cb?)
     (make <Container>
       #:version (cons ver5 #f)
       #:contents
       (append
         (if cb?
           (list
             (make <Color-Footnote>
               #:tool 't0
               #:dependence '(x0 y0 number tx ty ta)
               #:color owner->color
               #:width 0
               #:coord-list
                 (cons
                   'vpl
                   (lambda ()
                     (make-footnote
                       (get-x0) (get-y0)
                       (lim-tx) (lim-ty) (get-ta)
                       (ask-number) (scale-font) #f)))))
           '())
         (list
           (make <Point>
             #:x-coord 'x0
             #:y-coord 'y0
             #:hint    "Опора")
           (make <Bar>
             #:l-visible #t
             #:h-visible #f
             #:dependence '(x0 y0)
             #:foreground owner->color
             #:background owner->color
             #:x-coord (lambda () (- (get-x0) q))
             #:y-coord (lambda () (- (get-y0) q))
             #:x-size  (cons 'r (* q 2))
             #:y-size  (* q 2))
           (make <Bar>
             #:l-visible #f
             #:h-visible #t
             #:dependence '(x0 y0)
             #:foreground owner->color
             #:background owner->color
             #:x-coord (lambda () (- (get-x0) (/ q 2)))
             #:y-coord (lambda () (- (get-y0) (/ q 2)))
             #:x-size  q
             #:y-size  q))
         (if cb?
           (list
             (make <Bar>
               #:l-visible #t
               #:h-visible #f
               #:dependence '(x0 y0)
               #:foreground owner->color
               #:background owner->color
               #:x-coord    (lambda () (+ (get-x0) q))
               #:y-coord    (lambda () (- (get-y0) r))
               #:x-size     p
               #:y-size     (* r 2))
             (make <Bar>
               #:l-visible #f
               #:h-visible #t
               #:dependence '(x0 y0)
               #:foreground owner->color
               #:background owner->color
               #:x-coord    (lambda () (+ (get-x0) (/ q 2)))
               #:y-coord    (lambda () (- (get-y0) p))
               #:x-size     (/ r 4)
               #:y-size     r)
             (make <Text>
               #:s-visible #t
               #:label 't0
               #:font low-font/2
               #:x-coord 'tx
               #:y-coord 'ty
               #:color owner->color
               #:angle   'ta
               #:a-grid  a-grid
               #:length  0
               #:hint "Номер опоры"
               #:text (cons 'number ask-number)))
           (list
             (make <Text>
               #:s-visible #t
               #:label 't0
               #:font low-font/2
               #:x-coord 'tx
               #:y-coord 'ty
               #:angle   'ta
               #:color owner->color
               #:a-grid  a-grid
               #:length  0
               #:hint "Номер опоры"
               #:text 'number)))))))

   (make-cabel-box
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number) (scale-font) #f))))
         (make <Color-Rectangle>
           #:l-visible #f
           #:h-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   'a
           #:hint    "Кабельный ящик")
         (make <Pie-Slice>
           #:l-visible #t
           #:h-visible #f
           #:foreground owner->color
           #:background owner->color
           #:dependence '(x0 y0 a)
           #:delta 1800
           #:angle a900
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size  (cons 'r r)
           #:y-size  (cons 'r r))
         (make <Pie-Slice>
           #:l-visible #f
           #:h-visible #t
           #:foreground owner->color
           #:background owner->color
           #:dependence '(x0 y0 a)
           #:delta 1800
           #:angle a900
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size  p
           #:y-size  p)
         (make <Bar>
           #:l-visible #t
           #:h-visible #f
           #:foreground owner->color
           #:background owner->color
           #:dependence '(x0 y0 a)
           #:angle a900
           #:x-coord (lambda () (calc-x (get-x0) (get-y0) (- cbr) 0 (a900)))
           #:y-coord (lambda () (calc-y (get-x0) (get-y0) (- cbr) 0 (a900)))
           #:x-size  (* 2 cbr)
           #:y-size  (/ cbr 2))
         (make <Bar>
           #:l-visible #f
           #:h-visible #t
           #:foreground owner->color
           #:background owner->color
           #:dependence '(x0 y0 a)
           #:angle a900
           #:x-coord (lambda () (calc-x (get-x0) (get-y0) (- (/ cbr 2)) 0 (a900)))
           #:y-coord (lambda () (calc-y (get-x0) (get-y0) (- (/ cbr 2)) 0 (a900)))
           #:x-size  cbr
           #:y-size  (/ cbr 4))
         (make <Text>
           #:s-visible #t
           #:label 't0
           #:color owner->color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:font low-font/2
           #:length  10
           #:hint "Номер кабельного ящика"
           #:text 'number))))

   (make-mast-support
     (make <Container>
       #:contents
       (list
         (make <Outlined-Circle>
           #:l-visible #t
           #:h-visible #f
           #:width line-width
           #:color owner->color
           #:foreground 1
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  (cons 'r r)
           #:hint    "Опора мачтовая")
         (make <Outlined-Circle>
           #:l-visible #f
           #:h-visible #t
           #:dependence '(x0 y0)
           #:width (/ line-width 2)
           #:color owner->color
           #:foreground 1
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size  p)
         (make <Color-Lines>
           #:l-visible #t
           #:h-visible #f
           #:color owner->color
           #:width line-width
           #:dependence '(x0 y0)
           #:coord-list
             (lambda ()
               (let
                 ((x0 (get-x0))
                  (y0 (get-y0)))
                 (list
                   (cons (- x0 r) (- y0 r)) (cons (+ x0 r) (+ y0 r))
                   (cons (- x0 r) (+ y0 r)) (cons (+ x0 r) (- y0 r))))))
         (make <Color-Lines>
           #:h-visible #t
           #:l-visible #f
           #:color owner->color
           #:width line-width
           #:dependence '(x0 y0)
           #:coord-list
             (lambda ()
               (let
                 ((x0 (get-x0))
                  (y0 (get-y0)))
                 (list
                   (cons (- x0 p) (- y0 p)) (cons (+ x0 p) (+ y0 p))
                   (cons (- x0 p) (+ y0 p)) (cons (+ x0 p) (- y0 p))))))
         (make <Text>
           #:s-visible #t
           #:label 't0
           #:font low-font/2
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:color owner->color
           #:a-grid  a-grid
           #:length  0
           #:hint "Номер опоры"
           #:text 'number)
           )))

   (make-elstric-support
     (make <Container>
       #:contents
       (list
           (make <Outlined-Circle>
             #:color owner->color
             #:width line-width
             #:foreground 1
             #:background 1
             #:x-coord 'x0
             #:y-coord 'y0
             #:x-size  (cons 'r r)
             #:hint    "Опора электосети")
           (make <Color-Lines>
             #:dependence '(x0 y0)
             #:color owner->color
             #:width line-width
             #:coord-list
               (lambda ()
                 (let
                   ((x0 (get-x0))
                    (y0 (get-y0)))
                   (list
                     (cons x0 (- y0 r)) (cons (- x0 q) (+ y0 q))
                     (cons x0 (- y0 r)) (cons (+ x0 q) (+ y0 q))
                    )))))))

   (make-cont-support (lambda (pl h)

     (make <Container>
       #:contents
       (list
         (make <Outlined-Circle>
           #:l-visible #t
           #:h-visible #f
           #:width line-width
           #:color owner->color
           #:foreground 1
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  (cons 'r r)
           #:hint h)
         (make <Outlined-Circle>
           #:l-visible #f
           #:h-visible #t
           #:dependence '(x0 y0)
           #:width (/ line-width 2)
           #:color owner->color
           #:foreground 1
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size p
           #:hint h)
         (make <Filled-Polygon>
           #:l-visible #t
           #:h-visible #f
           #:foreground owner->color
           #:dependence '(x0 y0)
           #:coord-list (lambda () (calc-plg pl r 10)))
         (make <Filled-Polygon>
           #:h-visible #t
           #:l-visible #f
           #:foreground owner->color
           #:dependence '(x0 y0)
           #:coord-list (lambda () (calc-plg pl r 20)))
           ))))

  (lsup '(
    (-1 . 7) (-3 . 7) (-3 . -6) (5 . -6) (5 . -5)
    (4 . -4) (3 . -4) (2 . -5) (-1 . -5)))

  (gsup '((-6 . 6) (6 . 6) (6 . -6) (-6 . -6)))

  (csup '((1 . 7) (-1 . 7) (-1 . -3) (-3 . -3)
          (-4 . -2) (-5 . -2) (-6 . -3) (-6 . -4) (-5 . -5) (-4 . -5)
          (-3 . -4) (-1 . -4) (-1 . -6) (1 . -6) (1 . -4) (3 . -4)
          (4 . -5) (5 . -5) (6 . -4) (6 . -3) (5 . -2) (4 . -2) (3 . -3)
          (1 . -3))))

  (add-object MOLE-SUPPORT (make-support #f))
  (add-object MOLE-SUPPORT-WITH-BOX (make-support #t))
  (add-object MOLE-MINISUPPORT (make-minisupport #f))
  (add-object MOLE-MINISUPPORT-WITH-BOX (make-minisupport #t))
  (add-object MOLE-CABEL-BOX make-cabel-box)
  (add-object MOLE-ELECTRIC-SUPPORT make-elstric-support)
  (add-object MOLE-MAST-SUPPORT make-mast-support)
  (add-object MOLE-LIGHT-SUPPORT   (make-cont-support lsup "Опора осветительная"))
  (add-object MOLE-CONTACT-SUPPORT (make-cont-support csup "Опора контактная"))
  (add-object MOLE-GAI-SUPPORT   (make-cont-support gsup "Светофор"))
)
;;== STAND-ARM ========================================================
(let*
  ((scale-factor 4)
   (line-width (* scale-factor 6))
   (line-width-h (/ line-width 2))
   (w (* scale-factor 20))

   (old-stand-arm
     (make <Container>
       #:contents
       (list
         (make <Point> #:x-coord 'x0 #:y-coord 'y0)
         (make <Color-Lines>))))

   (make-sa (lambda (type pl h)

     (make <Container>
       #:version (cons old-stand-arm #f)
       #:contents
       (list
         (make <Point>
           #:x-coord 'x0
           #:y-coord 'y0
           #:hint h)
         (make type
           #:l-visible #t
           #:h-visible #f
           #:dependence '(x0 y0)
           #:color owner->color
           #:width line-width
           #:coord-list (lambda () (calc-plg pl w 1)))
         (make type
           #:l-visible #f
           #:h-visible #t
           #:dependence '(x0 y0)
           #:color owner->color
           #:width line-width-h
           #:coord-list (lambda () (calc-plg pl w 2)))))))

   (sts '((-1 . -2) (-1 . 0) (1 . -2) (1 . 0)
          (-1 . 0) (1 . 0) (0 . 0) (0 . 2)))

   (str '((-1 . -2) (-1 . 0) (1 . -2) (1 . 0)
          (-1 . 1) (1 . 1) (-1 . 0) (1 . 0) (0 . 0) (0 . 2)))

   (sst '((-1/2 . -2) (-1/2 . 2) (1/2 . -2) (1/2 . 2))))

  (add-object MOLE-PHONE-STAND-ARM (make-sa <Color-Lines> sts "Стойка телефонная"))
  (add-object MOLE-RADIO-STAND-ARM (make-sa <Color-Lines> str "Стойка радио"))
  (add-object MOLE-STACK-STAND-ARM (make-sa <Color-Lines> sst "Крепление штыревое"))
)
;;== PASSAGE ==========================================================

(let*
  ((class-list (list MOLE-CABEL-PASSAGE
                     MOLE-COLLECTOR-SECTION
                     MOLE-SMALL-SQUARE-COLLECTOR-SECTION
                     MOLE-ARMOUR-CABEL-TRENCH
                     MOLE-AIR-CONNECTION-LINE
                     MOLE-HANGING-CABEL
                     MOLE-SUBBUILDING-LINE
                     MOLE-ON-BRIGE-LINE
                     MOLE-OTHER-PASSAGE
                     MOLE-UNDERWATER-CABEL-TRENCH
                     MOLE-TRAY-PASSAGE))

   (scale-factor
     (lambda ()
       (if hi-resolution? 3 12)))

   (type-index
     (lambda ()
       (type->index class-list)))

   (hint
     (lambda ()
       (vector-ref '#("Пролёт кабельной канализации"
                      "Участок коллектора"
                      "Участок коллектора малого сечения"
                      "Трасса кабеля в грунте"
                      "Воздушная линия связи"
                      "Подвесной кабель"
                      "Трасса вне канализации"
                      "Трасса на мосту"
                      "Иной пролёт"
                      "Трасса кабеля под водой"
                      "Лотковая канализация")
                   (type-index))))

   (line-style
     (lambda ()
       (vector-ref
         '#(#:solid 10 11 12 13 14 15 16 #:solid 20 30)
         (type-index))))

   (line-width (lambda ()
     (* (scale-factor) 8)))

   (dev-list (list MOLE-INSPECT-DEV MOLE-BASE-SAFE MOLE-BASE-SUPPORT
                   MOLE-OUTSIDE-MSU MOLE-INSIDE-MSU MOLE-STAND-ARM))

   (test (lambda (pl start?)
     (define (test-point begin?)
       (let*
         ((pl (if begin? pl (last-pair pl)))
          (vm (map
                (lambda (well)
                  (set-car! pl (cons (assq-ref well 'x0) (assq-ref well 'y0)))
                  well)
                (get-all-objects (car pl) dev-list))))
          (length
            (list-select vm
              (lambda (well)
                (equal? (car pl)
                        (cons (assq-ref well 'x0)
                              (assq-ref well 'y0))))))))

     (set-property! 'apl pl)

     (let*
       ((vns (test-point #t))
        (vnf (test-point #f)))
       (and-let*
         (((not start?))
          ((not edition?))
          (ln (length pl))
          ((>= ln 2)))
         (set-property! 'apl pl)
         (and (> (type-index) 0) (set-property! 'chanals ""))
         (set-property! 'length (poly-length->str pl))
         (set-property! 'ta1 (good-angle (calc-a (car pl) (car (last-pair pl))))))

       (cond
         ((= vns 0) "Пролёт должен начинаться в смотровом устройстве!")
         ((= vnf 0)
           (cond
             (edition? "Пролёт должен заканчиваться в смотровом устройстве!")
             ((eq? (tk-message-box
                      "Пролёт должен заканчиваться в смотровом устройстве!\n\nПостроить здесь?"
                      "Внимание!" 'yes-no) 'yes)
              (set! &suspend-object? #t))
             (else #f)))
         ((and (> vns 1) (> vnf 1)) "Неоднозначные начало и конец пролета!")
         ((> vns 1) "Неоднозначное начало пролета!")
         ((> vnf 1) "Неоднозначный конец пролета!")
         ((and (equal? (car pl) (car (last-pair pl))) (or (not start?) edition?))
          "Пролет не должен заканчиваться в том же смотровом устройстве!")
           ))))

   (old-pr
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline> #:coord-list 'apl)
         (make <Text> #:text 'length #:x-coord 'tx1 #:y-coord 'ty1)
         (make <Text> #:text 'chanals)
         (make <Color-Polyline>))))

   (old-prol->prol (lambda ()
     (and-let*
       ((pll (get-property 'apl))
        ((> (length pll) 1)))
       (set-property! 'chanals "")
       (set-property! 'ta1 (good-angle (calc-a (car pll) (cadr pll)))))))

   (is-prolet?
     (lambda ()
       (and (get-property 'apl)
            (> (length (get-property 'apl)) 1))))

   (make-footnote
     (lambda ()
       (if (and is-convertion? (null? (get-property 'vpl)))
         '()
         (let
           ((tx1 (get-property 'tx1))
            (ty1 (get-property 'ty1))
            (ta1 (get-property 'ta1))
            (dx (* -3/10 (scale-font)))
            (ch (get-property 'chanals))
            (ln (get-property 'length)))
           (make-pll-footnote
             (get-property 'apl)
             (calc-x tx1 ty1 dx 0 ta1)
             (calc-y tx1 ty1 dx 0 ta1)
             ta1
             (if (and ch (> (string-length ch) (string-length ln))) ch ln)
             (scale-font)
             (* (if is-convertion? 8/10 1/8) (scale-font)))))))

   (make-oy1
     (lambda ()
       (let*
         ((s (+ (scale-font) (* 1/2 (line-width))))
          (x1 (get-property 'tx1))
          (y1 (get-property 'ty1))
          (ang (get-property 'ta1))
          (ch (get-property 'chanals))
          (ln (get-property 'length))
          (str (if (and ch (> (string-length ch) (string-length ln))) ch ln))
          (stl  (calc-string-width str (h-std-font) ang))
          (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
          (x2 (car p2))
          (y2 (cdr p2))
          (xm (/ (+ x1 x2) 2))
          (ym (/ (+ y1 y2) 2)))
         (if (null? (make-footnote))
           (let loop ((d #f) (l 0) (a ang) (pl (get-property 'apl)))
             (if (null? pl)
               (begin
                 (set-property! 'ta1 a)
                 (if is-convertion?
                   (begin
                     (set! a (deg->rad a))
                     (set-property! 'tx1 (+ x1 (* l (sin a))))
                     (set-property! 'ty1 (+ y1 (* l (cos a))))
                     s)
                   (+ l s)))
               (or (and-let*
                     ((> (length pl) 1)
                      (p0 (car pl))
                      (p1 (cadr pl))
                      (ds (or (dist-to-segment x1 y1 p0 p1)
                              (dist-to-segment x2 y2 p0 p1)
                              (dist-to-segment xm ym p0 p1)))
                      ((or (not d) (< ds d)))
                      (a (calc-a p0 p1))
                      (ga (good-angle a))
                      (l ((if (= a ga) - +) (dist-to-line x1 y1 p0 p1))))
                     (loop ds l ga (cdr pl)))
                   (loop d l a (cdr pl)))))
              s))))

   (make-oy2
     (lambda ()
       (- (make-oy1)
          (scale-font)
          (* (if (null? (make-footnote)) 2 1/2) (line-width)))))

   (old-pr1
     (make <Container>
       #:version (cons old-pr old-prol->prol)
       #:contents
       (list
         (make <Color-Polyline> #:coord-list 'apl)
         (make <Text> #:x-coord 'tx1 #:y-coord 'ty1 #:angle 'ta1 #:text 'length)
         (make <Text> #:omit (lambda () (> (type-index) 0)) #:text 'chanals)
         (make <Color-Footnote>))))

   (make-arrow
     (lambda ()
        (or
          (and-let*
            ((apl (get-property 'apl))
             ((> (length apl) 1))
             (lm (map (lambda (p0 p1) (dist p0 p1)) apl (cdr apl)))
             (md (apply max lm))
             (mn (memv md lm))
             (n (- (length lm) (length mn)))
             (p0 (list-ref apl n))
             (p1 (list-ref apl (+ n 1)))
             (w (* (line-width) (if hi-resolution? 3 2)))
             (k 3/4)
             ((> (dist p0 p1) (* 4 w)))
             (a (calc-a p0 p1))
             (p0 (map (lambda (x) (/ x 2)) (map + p0 p1))))
            (map (lambda (p) (shift p0 (turn p a)))
                 (list (cons (* w 3/2) 0)
                       (cons (* w -1/2) (* w k))
                       (cons 0 0)
                       (cons (* w -1/2) (* w k -1)))))
          '())))

   (pr
     (make <Container>
       #:version (cons old-pr1 #f)
       #:contents
       (list
         (make <Color-Polyline>
           #:valid-test test
           #:line  line-style
           #:color owner->color;;line-color
           #:width line-width
           #:hint  hint
           #:coord-num -1000 ;(lambda () (case (type-index) ((0 3 6 9) -1000) (else 2)))
           #:coord-list 'apl)
         (make <Filled-Polygon>
           #:omit (lambda () (> (type-index) 0))
           #:s-visible #t
           #:foreground owner->color;;line-color
           #:dependence '(apl)
           #:coord-list make-arrow)
         (make <Text>
           #:h-visible is-prolet?
           #:l-visible is-prolet?
           #:label 't0
           #:dependence '(apl)
           #:color   owner->color
           #:x-coord 'tx1
           #:y-coord 'ty1
           #:angle   'ta1
           #:y-offset (cons 'oy1 make-oy1)
           #:hint    "Длина пролёта"
           #:font h-std-font ;std-font
           #:valid-test valid-num?
           #:text 'length)
         (make <Text>
           #:tool 't0
           #:omit (lambda () (> (type-index) 0))
           #:h-visible is-prolet?
           #:l-visible is-prolet?
           #:dependence '(apl length tx1)
           #:color   owner->color
           #:x-coord (lambda () (get-property 'tx1))
           #:y-coord (lambda () (get-property 'ty1))
           #:y-offset (cons 'oy2 make-oy2)
           #:angle   (lambda () (get-property 'ta1))
           #:hint    "Сумма каналов по блокам"
           #:font h-std-font ;std-font
           #:valid-test valid-chan-sum?
           #:text 'chanals)
         (make <Color-Footnote>
           #:tool 't0
           #:dynamic #t
           #:dependence '(apl tx1 ty1)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list (cons'vpl make-footnote))
                   ))))

  (add-object class-list pr)
)
;;== QUARTERS =========================================================
(let*
  ((scale-factor 10)
   (line-width 1)
   (class-list (list MOLE-ENTERPRISE-TERRITORY
                     MOLE-LIVING-TERRITORY
                     MOLE-ADMINISTRATIVE-TERRITORY
                     MOLE-PRIVATE-SECTOR
                     MOLE-OTHER-TERRITORY))

   (type-index
     (lambda ()
       (type->index class-list)))

   (hint
     (lambda ()
       (vector-ref '#("Территория предприятия"
                      "Жилой квартал"
                      "Административная территория"
                      "Частный сектор"
                      "Иной квартал")
                   (type-index))))
   (fill-color
     (lambda ()
       (vector-ref '#(21 7 22 8 26) (type-index))))

   (pr
     (make <Container>
       #:contents
       (list
         (make <Filled-Polygon>
           #:shaped     #t
           #:foreground fill-color
           #:background fill-color
           #:hint       hint
           #:coord-list 'apg)
   ))))

  (add-object class-list pr)
)
;;== TERRITORY ========================================================

(let*
  ((scale-factor 10)
   (pr
     (make <Container>
       #:contents
       (list
         (make <Bound-Color-Polyline>
           #:shaped     #t
           #:color      10
           #:line       #:dash
           #:width      0
           #:hint       "Территория"
           #:coord-list 'apg)
   ))))

  (add-object MOLE-TERRITORY pr)
)

;;== BUILDING =========================================================

(let*
  ((scale-factor 20)
   (line-width (* 1 scale-factor ))
   (class-list (list MOLE-BUILDING
                     MOLE-ADMINISTRATIVE-BUILDING
                     MOLE-LIVING-BUILDING
                     MOLE-SCHOOL
                     MOLE-PRESCHOOL
                     MOLE-PRIVATE-BUILDING
                     MOLE-BUILDING-BUILDING
                     MOLE-ENTERPRISE-BUILDING))

   (type-index
     (lambda ()
       (type->index class-list)))

   (hint
     (lambda ()
       (vector-ref
          '#("Здание"
             "Административное здание"
             "Жилой дом"
             "Школа"
             "Дошкольное учреждение"
             "Частный дом"
             "Строящееся здание"
             "Промышленное сооружение")
         (type-index))))

   (fill-color
     (lambda ()
       (vector-ref
         '#(2 13 16 15 14 5 12 9)
         (type-index))))

   (line-color
     (lambda ()
       (vector-ref
         '#(0 0 0 0 0 0 6 0)
         (type-index))))

   (pr
     (make <Container>
       #:contents
       (list
         (make <Smart-Polygon>
           #:shaped     #t
           #:color      line-color
           #:foreground fill-color
           #:background fill-color
           #:width      line-width
           #:hint       hint
           #:coord-list 'apg)
        (make <Text>
          #:color text-color
          #:font (lambda () (vector 0 (quotient low-font-size (if (= (get-class) MOLE-PRIVATE-BUILDING) -2 -1))))
          #:x-coord 'tx
          #:y-coord 'ty
          #:angle   'ta
          #:hint    "Номер дома"
          #:text    'number)
   ))))

  (add-object class-list pr)
)
;;== LAKE ============================================================

(let*
  ((fill-color (lambda () (if (= (get-class) MOLE-LAKE) 23 18)))
   (bound-color (lambda () (if (= (get-class) MOLE-LAKE) 27 18)))
   (hint (lambda () (if (= (get-class) MOLE-LAKE) "Водоём" "Зеленые насаждения")))
   (pr
     (make <Container>
       #:contents
       (list
         (make <Filled-Polygon>
           #:shaped  #t
           #:foreground fill-color
           #:background fill-color
           #:color bound-color
           #:hint hint
           #:coord-list 'apg)
   ))))

  (add-object (list MOLE-LAKE MOLE-GREEN-PLANT) pr)
)

;;== RIVER ============================================================

(let*
  ((fill-color 24)

   (pr
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>
           #:shaped  #t
           #:color   fill-color
           #:hint    "Водоток"
           #:coord-list 'apl)
   ))))

  (add-object MOLE-RIVER pr)
)
;;== RAILWAY TRAMWAY FENCE ============================================

(let*
  ((scale-factor 50)
   (make-linear (lambda (line-type line-width line-color hint)
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>
           #:shaped     #t
           #:color      line-color
           #:line       line-type
           #:width      (* scale-factor line-width)
           #:hint       hint
           #:coord-num -1000
           #:coord-list 'apl)
   )))))
   (add-object MOLE-RAILWAY (make-linear 17 1 0 "Железная дорога"))
   (add-object MOLE-TRAMWAY (make-linear 18 1 0 "Трамвайные пути"))
   (add-object MOLE-FENCE   (make-linear #:solid 1 61 "Ограждение"))
   (add-object MOLE-COMMENTARY-AS-LINE  (make-linear #:solid 0 0 "Комментарий линейный"))
)
;;== STREET ===========================================================
(let*
  ((scale-factor 50)
   (line-color 19)
   (line-width (* scale-factor 4))
   (line-style #:solid)

   (old-street
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>)
         (make <Color-Lines> #:coord-list 'pll)
         (make <Text> #:text 'number))))

   (old-street->street
     (lambda ()
       (set-property! 'apl (lines->poly (get-property 'pll)))))

   (pr
     (make <Container>
       #:version (cons old-street old-street->street)
       #:contents
       (list
         (make <Color-Polyline>
           #:shaped #t
           #:color line-color
           #:line  line-style
           #:width line-width
           #:hint  "Улица"
           #:coord-num  0
           #:coord-list 'apl)
         (make <Text>
            #:h-visible #f
            #:l-visible #f
            #:text (cons 'number ask-number)
            #:hint "Название")
   ))))

   (add-object MOLE-STREET pr)
   )
;;== RAILWAY-STATION ==================================================

(let*
  ((scale-factor 50)
   (back-color 20)
   (r (* scale-factor 20))
   (pr
     (make <Container>
       #:contents
       (list
         (make <Circle>
           #:background back-color
           #:foreground back-color
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r
           #:hint    "Железнодорожная станция")
   ))))
  (add-object MOLE-RW-STATION pr)
)

;;== ATTACHMENT =======================================================

(let*
  ((scale-factor 50)
   (line-width (quotient scale-factor 3))

   (text-font (vector 0 (- low-font-size/2) "NORMAL" "I"))

   (line-test (lambda (pl start?)
     (or (and edition? (= (get-class) MOLE-INEXACT-ATTACHMENT))
         start?
         (set-property! 'number (number->string (/ (scale-dist (car pl) (cadr pl)) 100))))))

   (calc-number (lambda ()
     (and-let*
       ((pl (get-property 'apl))
        ((= 2 (length pl)))
        (nm (get-property 'number))
        (c (or (string-index nm #\,) -1))
        (ln0 (if (>= c 0) (string-append (substring nm 0 c) "." (substring nm (inc c))) nm))
        ((not (string->number ln0)))
        (d (scale-dist (car pl) (cadr pl))))
       (set-property! 'number (number->string (/ (inexact->exact (/ (+ d 5) 10)) 10))))
     2))

   (make-attachment (lambda ()
     (let*
       ((len (get-number))
        (pl (get-property 'apl))
        (c (and len (string-index len #\,)))
        (i (and len (string-index len kwasi-cursor-char))))
       (if (or (not len) (and curr-prim (= curr-prim 0)))
         pl
         (and-let*
           (((= 2 (length pl)))
            (ln0 (if c (string-append (substring len 0 c) "." (substring len (inc c))) len))
            (ln (if i (string-append (substring ln0 0 i) (substring ln0 (inc i))) ln0))
            ((not (string? (valid-num? ln))))
            (l (or (string->number ln) 5))
            (dx (- (caadr pl) (caar pl)) )
            (dy (- (cdadr pl) (cdar pl)) )
            (d (scale-dist (car pl) (cadr pl)))
            (x (inexact->exact (/ (* 100 l dx) d)))
            (y (inexact->exact (/ (* 100 l dy) d)))
            (lst (list (car pl) (shift (car pl) (cons x y)))))
           (set-property! 'apl lst)
           lst)))))

   (old-attachment
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline> #:coord-list 'apl)
         (make <Text>  #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Color-Polyline>))))

   (epr (lambda (k)
    (make <Container>
      #:contents
      (list
        (make <Color-Polyline>
          #:label      'l1
          #:h-visible   #f
          #:l-visible   #f
          #:valid-test line-test
          #:hint       "Привязка точная"
          #:color      17
          #:line       #:solid
          #:width      line-width
          #:coord-num  calc-number
          #:coord-list 'apl)
        (make <Text>
          #:dependence '(apl)
          #:valid-test valid-num?
          #:s-visible #t
          #:color text-color
          #:font text-font
          #:x-coord 'tx
          #:y-coord 'ty
          #:angle   (cons 'ta (lambda () (good-angle (calc-angle 'apl))))
          #:hint    "Расстояние"
          #:text    'number)
        (make <Color-Polyline>
          #:tool       'l1
          #:dynamic    #t
          #:dependence '(apl)
          #:color      17
          #:line       #:solid
          #:width      line-width
          #:coord-list (cons 'pll make-attachment))))))

   (ipr (lambda (k)
    (make <Container>
      #:version (cons old-attachment #f)
      #:contents
      (list
        (make <Color-Polyline>
          #:valid-test line-test
          #:hint       "Привязка условная"
          #:color      17
          #:line       #:solid
          #:width      line-width
          #:coord-num  calc-number
          #:coord-list 'apl)
        (make <Text>
          #:dependence '(apl)
          #:valid-test valid-num?
          #:s-visible #t
          #:color text-color
          #:font text-font
          #:x-coord 'tx
          #:y-coord 'ty
          #:angle   (cons 'ta (lambda () (good-angle (calc-angle 'apl))))
          #:hint    "Расстояние"
          #:text    'number)))))

   (rpr (lambda (k)
    (make <Container>
      #:contents
      (list
        (make <Color-Polyline>
          #:label      'l1
          #:s-visible   #t
          #:h-visible   #t
          #:l-visible   #t
          #:valid-test line-test
          #:hint       "Привязка круговая"
          #:line       #:solid
          #:color      17
          #:width      line-width
          #:coord-num  calc-number
          #:coord-list 'apl)
        (make <Text>
          #:dependence '(apl)
          #:valid-test valid-num?
          #:s-visible #t
          #:color text-color
          #:font text-font
          #:x-coord 'tx
          #:y-coord 'ty
          #:angle   'ta
          #:hint    "Радиус"
          #:text    'number)
        (make <Color-Ring>
          #:tool       'l1
          #:dynamic    #t
          #:dependence '(apl)
          #:color      17
          #:line       #:solid
          #:width      line-width
          #:x-coord (cons 'x0 (lambda () (caar (get-property 'apl))))
          #:y-coord (cons 'y0 (lambda () (cdar (get-property 'apl))))
          #:x-size  (lambda () (let ((l (or (make-attachment) (get-property 'apl)))) (scale-dist (car l) (cadr l))))))))))

  (add-object MOLE-EXACT-ATTACHMENT   (epr 1/2))
  (add-object MOLE-INEXACT-ATTACHMENT (ipr 1/2))
  (add-object MOLE-ROUND-ATTACHMENT   (rpr 1/2))

  (add-object MOLE-VODO-EXACT-ATTACHMENT   (epr 1/2))
  (add-object MOLE-VODO-INEXACT-ATTACHMENT (ipr 1/2))
  (add-object MOLE-VODO-ROUND-ATTACHMENT   (rpr 1/2))

  (add-object MOLE-TEPLO-EXACT-ATTACHMENT   (epr 1/2))
  (add-object MOLE-TEPLO-INEXACT-ATTACHMENT (ipr 1/2))
  (add-object MOLE-TEPLO-ROUND-ATTACHMENT   (rpr 1/2))

  (add-object MOLE-ELECTRO-EXACT-ATTACHMENT   (epr 1/2))
  (add-object MOLE-ELECTRO-INEXACT-ATTACHMENT (ipr 1/2))
  (add-object MOLE-ELECTRO-ROUND-ATTACHMENT   (rpr 1/2))
)
;;== COMMENTARY =======================================================
(let*
  ((test (lambda (s)
     (and (string? s)
          (positive?  (string-length s)))))

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Text>
           #:valid-test test
           #:font h-std-font ;std-font
           #:color text-color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:hint    "Комментарий"
           #:text    'number))))

   (pr1-old
      (make <Container>
        #:contents
        (list
           (make <Color-Polyline> #:coord-list 'apl)
           (make <Text> #:text    'number)
           (make <Container>))))

   (pr1
      (make <Container>
        #:version (cons pr1-old #f)
        #:contents
        (list
           (make <Color-Polyline>
             #:s-visible #t
             #:shaped #t
             #:coord-list 'apl)
           (make <Text>
             #:valid-test test
             #:l-visible #f
             #:h-visible #f
             #:text 'number)
           (make-text-by-line h-std-font 0 'apl 'number)))))
   (add-object MOLE-COMMENTARY pr0)
   (add-object MOLE-COMMENTARY-BY-LINE pr1))
;;== STREET-NAME ======================================================

(let*
  ((old-street-name
     (make <Container>
       #:contents
       (list
         (make <Point>  #:x-coord 'x0 #:y-coord 'y0)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Color-Polyline>))))

   (pr
     (make <Container>
       #:version (cons old-street-name #f)
       #:contents
       (list
        (make <Color-Footnote>
          #:tool 't0
          #:dependence '(x0 y0 tx ty ta number)
          #:line  #:solid
          #:color text-color
          #:width 0
          #:coord-list
            (cons
              'vpl
              (lambda ()
                (make-footnote
                  (get-x0) (get-y0)
                  (lim-tx) (lim-ty) (get-ta)
                  (or (get-number) "")
                  low-font-size low-font-size))))
         (make <Point>
           #:x-coord 'x0
           #:y-coord 'y0)
         (make <Text>
           #:label 't0
           #:editable #f
           #:color text-color
           #:hint "Название улицы"
           #:font std-font
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:text    (cons 'number ask-number))
  ))))
  (add-object MOLE-STREET-NAME pr)
)

(let*
  ((pr-old
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline> #:coord-list 'apl)
         (make <Text> #:text    'number)
         (make <Container>))))
   (pr
     (make <Container>
       #:version (cons pr-old #f)
       #:contents
       (list
          (make <Color-Polyline>
            #:s-visible #t
            #:coord-list 'apl)
          (make <Text>
            #:l-visible #f
            #:h-visible #f
            #:text (cons 'number ask-number))
          (make-text-by-line std-font 0 'apl 'number)))))
   (add-object MOLE-STREET-NAME-BY-LINE pr)
)
;;== ISOLINE ==========================================================
(let*
  ((make-isoline (lambda (sym line-type line-width font-size interval)

     (define (make-poly pl)
       (make <Color-Polyline>
         #:color owner->color
         #:line  line-type
         #:width line-width
         #:coord-list pl))

     (define (cut-poly pl d)

       (define (step-by-line p0 p1 d)
         (let*
           ((n2 (/ d (dist p0 p1)))
            (n1 (- 1.0 n2)))
           (cons (inexact->exact (+ (* (car p0) n1) (* (car p1) n2)))
                 (inexact->exact (+ (* (cdr p0) n1) (* (cdr p1) n2))))))

       (let loop ((hd (list-head pl 1)) (pl pl) (d d))
         (and-let*
           (((> (length pl) 1))
            (p0 (car pl))
            (p1 (cadr pl))
            (s (dist p0 p1))
            (r (- d s))
            (pp (if (>= r 0) p1 (step-by-line p0 p1 d)))
            (hd (append hd (list pp))))
           (cond
              ((positive? r) (loop hd (cdr pl) r))
              ((zero? r) (cons hd (cdr pl)))
              (else (cons hd (cons pp (cdr pl))))))))

     (define (mk-trace pl)
       (append
         (apply append
           (map
             (lambda (p0 p1)
               (or (make-trace p0 p1) (list p0)))
             pl (cdr pl)))
         (last-pair pl)))


     (if (null? (get-property sym))
       '()
       (let*
         ((nu (get-number))
          (num (string-append " " (or nu "") " "))
          (pl (mk-trace (get-property sym)))
          (shl (or (get-property '&shape-list) (map (lambda (x) 0) pl)))
          (apl (enbound (eq? sym 'apg) (if nu pl (make-shape pl shl))))
          (sum (inexact->exact (poly-length apl)))
          (font (vector 0 (- font-size)))
          (ssw (* 12/10 (if nu (calc-string-width num font 0) 0)))
          (n-step (inexact->exact (round (/ sum (+ interval ssw)))))
          (step (if (positive? n-step) (quotient sum n-step) 1))
          (interval (- step ssw))
          (lst '()))

         (define (make-subs pl)
           (let*
             ((p0 (car pl))
              (p1 (cadr pl))
              (a (calc-a p0 p1))
              (ga (good-angle a))
              (fd? (= ga a))
              (p (shift (if fd? p0 p1) (turn (cons (* 4/10 font-size) 0) (- ga 900)))))
             (list
               (if (< (dist p0 p1) ssw)
                 (make-text-by-line font text-color (if fd? pl (reverse pl)) num)
                 (make <Text>
                   #:color owner->color
                   #:font font
                   #:x-coord (car p)
                   #:y-coord (cdr p)
                   #:angle ga
                   #:text num)))))

         (if (zero? ssw)
           (list (make-poly apl))
           (dotimes (i n-step (append! lst (list (make-poly apl))))
             (set! apl (cut-poly apl (if (zero? i) (/ interval 2) interval)))
             (set! lst (append! lst (list (make-poly (car apl)))))
             (set! apl (cut-poly (cdr apl) ssw))
             (set! lst (append! lst (make-subs (car apl))))
             (set! apl (cdr apl)))))))
)

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline> #:coord-list 'apg)
         (make <Container>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Color-Footnote>))))


   (pr (lambda (class type sym num hint txt-hint font-size test line-type line-width interval)
     (make <Container>
       #:version (cons pr0 #f)
       #:contents
       (list
         (make type
           #:h-visible (lambda () (and is-packing? (= class MOLE-WF-ZONE)))
           #:l-visible (lambda () (and is-packing? (= class MOLE-WF-ZONE)))
           #:shaped #t
           #:color      owner->color
           #:foreground 9
           #:background -1
           #:fill #:cross
           #:hint hint
           #:coord-list sym)
         (make <Container>
           #:dependence (list sym)
           #:dynamic  #t
           #:contents (lambda () (make-isoline sym line-type line-width font-size interval)))
         (make <Text>
           #:label 't0
           #:s-visible #t
           #:hint txt-hint
           #:valid-test test
           #:color owner->color
           #:font (vector 0 (- font-size))
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:text 'number)
         (make <Color-Footnote>
           #:tool 't0
           #:s-visible #t
           #:dependence (list sym 'tx 'ty 'ta 'number)
           #:line  #:solid
           #:color text-color
           #:width 0
           #:coord-min num
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (if (get-property sym)
                   (make-footnote
                     (caar (get-property sym))
                     (cdar (get-property sym))
                     (get-tx) (get-ty) (get-ta)
                     (get-number) font-size font-size)
                   '())))))))))

  (add-object MOLE-ISOLINE    (pr MOLE-ISOLINE    <Color-Polyline>       'apl 2 "Изолиния"          "Уровень"      420 valid-num?     #:solid 0 20000))
  (add-object MOLE-ATS-REGION (pr MOLE-ATS-REGION <Bound-Color-Polyline> 'apg 3 "Зона обслуживания" "Номер АТС"    420 std-valid-test #:dash  0 10000))
  (add-object MOLE-WF-ZONE    (pr MOLE-WF-ZONE    <Filled-Polygon>       'apg 3 "Зона покрытия WF"  "Номер точки"  420 std-valid-test #:solid 0 10000))
  )
;;=====================================================================
;;== MOLE-NODE-REGION =================================================
(let*
  ((line-color 402)
   (fore-color 402)
   (back-color -1)
   (line-width 0)
   (text-color 25)
   (font-size -175000)
   (pr
     (make <Container>
       #:contents
       (list
         (make <Smart-Polygon>
           #:shaped     #t
           #:color      line-color
           #:foreground fore-color
           #:background back-color
           #:width      line-width
           #:fill       #:bdiagonal
           #:hint       "Территория узла связи"
           #:coord-list 'apg)
        (make <Text>
          #:color text-color
          #:font (vector 0 font-size)
          #:x-coord 'tx
          #:y-coord 'ty
          #:angle   'ta
          #:hint    "Наименование узла связи"
          #:text    'number)
          ))))

  (add-object MOLE-NODE-REGION pr))
;;=====================================================================
;;== TAXOFON ==========================================================

(let*
  ((scale-factor 8)
   (w (* scale-factor 16))
   (h (* scale-factor 20))
   (w/2 (/ w 2))
   (h/2 (/ h 2))
   (line-width (* scale-factor 6))
   (back-color 1)
   (taxofon-list (list MOLE-TAXOFON-1 MOLE-TAXOFON-2 MOLE-TAXOFON-3 MOLE-TAXOFON-4))

   (type-index
     (lambda ()
       (type->index taxofon-list)))

   (hint
     (lambda ()
       (vector-ref
          '#("Таксофон-1" "Таксофон-2" "Таксофон-3" "Таксофон-4")
          (type-index))))

   (line-color
     (lambda ()
       (vector-ref
          '#(0 3 4 11)
          (type-index))))

   (pr
     (make <Container>
       #:movable #f
       #:contents
       (list
        (make <Color-Footnote>
          #:tool 't0
          #:dependence '(x0 y0 number tx ty ta)
          #:color text-color
          #:width 0
          #:coord-list
            (cons
              'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number)
                   (scale-font) #f))))
         (make <Color-Rectangle>
           #:l-visible #f
           #:h-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  100
           #:y-size  10
           #:angle   'a
           #:hint hint)
         (make <Outlined-Bar>
           #:color      line-color
           #:foreground back-color
           #:background back-color
           #:width      line-width
           #:dependence '(x0 y0 a)
           #:angle      get-a
           #:x-coord    (lambda () (calc-x (get-x0) (get-y0) (- w/2) (- h/2) (get-a)))
           #:y-coord    (lambda () (calc-y (get-x0) (get-y0) (- w/2) (- h/2) (get-a)))
           #:x-size     (cons 'r w)
           #:y-size     h)
        (make <Text>
           #:label 't0
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  10
           #:hint    "Номер таксофона"
           #:font    h-std-font
           #:text    'number)
           ))))

  (add-object taxofon-list pr))

;;== CORD =============================================================

(let*
  ((scale-factor 8)
   (line-width (* scale-factor 6))
   (line-color 0)

   (dev-list (list MOLE-ATS MOLE-MODULE-ATS MOLE-BASE-SAFE MOLE-KOROBKA
                   MOLE-TAXOFON-1 MOLE-TAXOFON-2 MOLE-TAXOFON-3 MOLE-TAXOFON-4))

   (dev-str " в одном из устройств:\nАТС, Вынос АТС, Шкаф, Коробка, Таксофон!")
   (cab-str "Кабель таксофона должен " )

   (test (lambda (pl start?)

     (define (test-point begin?)
       (let*
         ((pl (if begin? pl (last-pair pl)))
          (vm (map
                (lambda (well)
                  (set-car! pl (cons (assq-ref well 'x0) (assq-ref well 'y0)))
                  well)
                (get-all-objects (car pl) dev-list))))
          (length
            (list-select vm
              (lambda (well)
                (equal? (car pl)
                        (cons (assq-ref well 'x0)
                              (assq-ref well 'y0))))))))

     (set-property! 'apl pl)

     (let*
       ((vns (test-point #t))
        (vnf (test-point #f)))

       (and-let*
         (((not start?))
          ((not edition?))
          (ln (length pl))
          ((>= ln 2)))
         (set-property! 'apl pl)
         (set-property! 'ta1 (good-angle (calc-a (car pl) (car (last-pair pl))))))

       (cond
         ((= vns 0) (string-append cab-str "начинаться" dev-str))
         ((= vnf 0)
           (cond
             (edition? (string-append cab-str "заканчиваться" dev-str))
             ((eq? (tk-message-box
                      (string-append cab-str "заканчиваться" dev-str "\n\nПостроить здесь?")
                      "Внимание!" 'yes-no) 'yes)
              (set! &suspend-object? #t))
             (else #f)))
         ((and start? (> vns 1)) "Неоднозначное начало кабеля таксофона!")
         ((and (not start?) (> vnf 1)) "Неоднозначный конец кабеля таксофона!")
         ((and (equal? (car pl) (car (last-pair pl))) (or (not start?) edition?)) "Кабель таксофона не должен заканчиваться в том же устройстве!")
         ((not edition?)
           (set-property! 'length (poly-length->str pl)))))))

   (make-footnote
     (lambda ()
       (if (and is-convertion? (null? (get-property 'vpl)))
         '()
         (let
           ((tx1 (get-property 'tx1))
            (ty1 (get-property 'ty1))
            (ta1 (get-property 'ta1))
            (dx (* -3/10 (scale-font)))
            (ln (get-property 'length)))
           (make-pll-footnote
             (get-property 'apl)
             (calc-x tx1 ty1 dx 0 ta1)
             (calc-y tx1 ty1 dx 0 ta1)
             ta1 ln
             (scale-font)
             (* (if is-convertion? 8/10 1/8) (scale-font)))))))

   (make-oy1
     (lambda ()
       (let*
         ((s (* (if (null? (make-footnote)) -2 -1/2) line-width))
          (x1 (get-property 'tx1))
          (y1 (get-property 'ty1))
          (ang (get-property 'ta1))
          (str (get-property 'length))
          (stl  (calc-string-width str (h-std-font) ang))
          (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
          (x2 (car p2))
          (y2 (cdr p2))
          (xm (/ (+ x1 x2) 2))
          (ym (/ (+ y1 y2) 2)))
         (if (null? (make-footnote))
           (let loop ((d #f) (l 0) (a ang) (pl (get-property 'apl)))
             (if (null? pl)
               (begin
                 (set-property! 'ta1 a)
                 (if is-convertion?
                   (begin
                     (set! a (deg->rad a))
                     (set-property! 'tx1 (+ x1 (* l (sin a))))
                     (set-property! 'ty1 (+ y1 (* l (cos a))))
                     s)
                   (+ l s)))
               (or (and-let*
                     ((> (length pl) 1)
                      (p0 (car pl))
                      (p1 (cadr pl))
                      (ds (or (dist-to-segment x1 y1 p0 p1)
                              (dist-to-segment x2 y2 p0 p1)
                              (dist-to-segment xm ym p0 p1)))
                      ((or (not d) (< ds d)))
                      (a (calc-a p0 p1))
                      (ga (good-angle a))
                      (l ((if (= a ga) - +) (dist-to-line x1 y1 p0 p1))))
                     (loop ds l ga (cdr pl)))
                   (loop d l a (cdr pl)))))
              s))))

   (pr
     (make <Container>
       #:movable #f
       #:contents
       (list
         (make <Color-Polyline>
           #:valid-test test
           #:hint "Кабель таксофона"
           #:line  #:solid
           #:width line-width
           #:color line-color
           #:coord-list 'apl)
         (make <Text>
           #:label 't0
           #:s-visible #t
           #:dependence '(apl)
           #:color   text-color
           #:x-coord 'tx1
           #:y-coord 'ty1
           #:y-offset (cons 'oy1 make-oy1)
           #:angle   'ta1
           #:hint    "Длина кабеля"
           #:font    h-std-font
           #:text    'length)
         (make <Color-Footnote>
           #:tool 't0
           #:s-visible #t
           #:dependence '(apl tx1 ty1)
           #:line  #:solid
           #:color text-color
           #:width 0
           #:coord-list (cons 'vpl make-footnote))
          ))))
  (add-object MOLE-CORD pr))

;;=== KOROBKA =========================================================
(let*
  ((scale-factor 10)
   (korob-color 0)
   (korob-diameter (* scale-factor 10))

   (pr
     (make <container>
       #:movable #f
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:s-visible #t
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color text-color
           #:width 0
           #:coord-list
            (cons
              'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number) (scale-font) #f))))
         (make <color-rectangle>
           #:l-visible #f
           #:h-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle 'a
           #:a-grid  a-grid
           #:hint "Коробка")
         (make <Pie-Slice>
           #:foreground korob-color
           #:background korob-color
           #:dependence '(x0 y0 a)
           #:delta   1800
           #:angle   get-a
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size  (cons 'r korob-diameter)
           #:y-size  (cons 'r korob-diameter))
         (make <Text>
           #:label 't0
           #:s-visible #t
           #:font h-std-font
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:hint    "Номер коробки"
           #:text    'number)))))

  (add-object MOLE-KOROBKA pr))

;;== MOLE_ATTACHMENT_POINT ============================================

(let*
  ((text-color 60)
   (pr
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:s-visible #t
           #:dependence '(x0 y0 number tx ty ta)
           #:color text-color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (get-tx) (get-ty) (get-ta)
                   (get-number)
                   (scale-font) #f))))
         (make <Circle>
           #:l-visible #t
           #:h-visible #t
           #:foreground text-color
           #:background text-color
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  (cons 'r 60)
           #:hint    "Точка привязки")
         (make <Text>
           #:label 't0
           #:s-visible #t
           #:font h-std-font
           #:color text-color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:hint    "Подпись"
           #:text    'number)
   ))))
   (add-object MOLE-ATTACHMENT-POINT pr))

;;== MOLE_REPER =======================================================

(let*
  ((point-color 0)
   (text-color 0)
   (deg-s "\xb0")
   (min-s "'")
   (sec-s "\"")
   (dlm-s ", ")
   (is-lat-deg?
     (lambda (str)
       (or (and (= 0 (string-length str))
                "Введите градусы широты")
           (and-let*
            ((val (string->number str)))
            (<= -90 val 90))
          "Некорректные градусы широты")))
   (is-lon-deg?
     (lambda (str)
       (or (and (= 0 (string-length str))
                "Введите градусы долготы")
           (and-let*
             ((val (string->number str)))
             (<= -180 val 180))
           "Некорректные градусы долготы")))
   (is-lat-min?
     (lambda (str)
       (or (and (= 0 (string-length str))
                "Введите минуты широты")
           (and-let*
             ((val (string->number str)))
             (<= 0 val 59))
           "Некорректные минуты широты")))
   (is-lon-min?
     (lambda (str)
       (or (and (= 0 (string-length str))
                "Введите минуты долготы")
           (and-let*
             ((val (string->number str)))
             (<= 0 val 59))
           "Некорректные минуты долготы")))
   (is-lat-sec?
     (lambda (str)
       (or (and (= 0 (string-length str))
                "Введите секунды широты")
           (and-let*
             ((val (string->number str)))
             (<= 0 val 59.9999999))
           "Некорректные секунды широты")))
   (is-lon-sec?
     (lambda (str)
       (or (and (= 0 (string-length str))
                "Введите секунды долготы")
           (and-let*
             ((val (string->number str)))
             (<= 0 val 59.9999999))
           "Некорректные секунды долготы")))
   (calc-dep (lambda (n)
     (define dl '(tx ty ta lat-deg lat-min lat-sec lon-deg lon-min lon-sec))
     (list-head dl (+ n 3))))

   (calc-off
     (lambda (n)
       (let
         ((pl (list std-font
                    (get-a)
                    (get-property 'lat-deg) deg-s
                    (get-property 'lat-min) min-s
                    (get-property 'lat-sec) sec-s
                    dlm-s
                    (get-property 'lon-deg) deg-s
                    (get-property 'lon-min) min-s
                    (get-property 'lon-sec) sec-s
                    ")")))
         (if (zero? n)
           0
           (apply sum-str-len (list-head pl (+ n 2)))))))
   (pr0
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>)
         (make <Circle> #:x-coord 'x0 #:y-coord 'y0)
         (make <Text>)
         (make <Text> #:text 'lat-deg #:x-coord 'tx #:y-coord 'ty #:angle 'ta)
         (make <Text>)
         (make <Text> #:text 'lat-min)
         (make <Text>)
         (make <Text> #:text 'lat-sec)
         (make <Text>)
         (make <Text>)
         (make <Text> #:text 'lon-deg)
         (make <Text>)
         (make <Text> #:text 'lon-min)
         (make <Text>)
         (make <Text> #:text 'lon-sec)
         (make <Text>)
         (make <Text>))))

   (calc-num (lambda ()
     (define (clcg g m s)
       (or (and-let*
             ((g (string->number g))
              (m (string->number m))
              (s (string->number s))
              (f (+ g (/ m 60.) (/ s 3600.))))
             (number->string f))
           ""))
     (let
       ((lat-deg (or (get-property 'lat-deg) "0"))
        (lat-min (or (get-property 'lat-min) "0"))
        (lat-sec (or (get-property 'lat-sec) "0"))
        (lon-deg (or (get-property 'lon-deg) "0"))
        (lon-min (or (get-property 'lon-min) "0"))
        (lon-sec (or (get-property 'lon-sec) "0")))
       (string-append
         (clcg lat-deg lat-min lat-sec)
         ";"
         (clcg lon-deg lon-min lon-sec)))))

   (pr0->pr1 (lambda ()
     (set-property! 'number (calc-num))))

   (pr1
     (make <Container>
       #:version (cons pr0 pr0->pr1)
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:s-visible #t
           #:dependence '(x0 y0 tx ty ta)
           #:color text-color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (get-tx) (get-ty) (get-ta)
                   (apply
                     string-append
                     (map (lambda (s)
                            (or (remove-kwasi-cursor (get-property s)) ""))
                          (list 'lat-deg deg-s 'lat-min min-s 'lat-sec sec-s
                                'lon-deg deg-s 'lon-min min-s 'lon-sec sec-s)))
                   low-font-size #f))))
         (make <Circle>
           #:foreground point-color
           #:x-coord    'x0
           #:y-coord    'y0
           #:x-size     (cons 'r 50)
           #:hint       "Репер")
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 0)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (- (sum-str-len std-font (get-a)  "(" )))
           #:text    "(")
         (make <Text>
           #:s-visible #t
           #:font std-font
           #:color text-color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:hint    "Широта (град.)"
           #:valid-test is-lat-deg?
           #:text 'lat-deg)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 1)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 1))
           #:text  deg-s)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 1)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 2))
           #:hint    "Широта (мин.)"
           #:valid-test is-lat-min?
           #:text 'lat-min)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 2)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 3))
           #:text min-s)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 2)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 4))
           #:hint    "Широта (сек.)"
           #:valid-test is-lat-sec?
           #:text 'lat-sec)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 3)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 5))
           #:text sec-s)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 3)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 6))
           #:text dlm-s)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 3)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 7))
           #:hint    "Долгота (град.)"
           #:valid-test is-lon-deg?
           #:text 'lon-deg)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 4)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 8))
           #:text  deg-s)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 4)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 9))
           #:hint    "Долгота (мин.)"
           #:valid-test is-lon-min?
           #:text 'lon-min)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 5)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 10))
           #:text min-s)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 5)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 11))
           #:hint    "Долгота (сек.)"
           #:valid-test is-lon-sec?
           #:text 'lon-sec)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 6)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 12))
           #:text sec-s)
         (make <Text>
           #:s-visible #t
           #:dependence (calc-dep 6)
           #:font std-font
           #:color text-color
           #:x-coord get-tx
           #:y-coord get-ty
           #:angle   get-ta
           #:x-offset (lambda () (calc-off 13))
           #:text ")")
         (make <Text>
           #:l-visible #f
           #:h-visible #f
           #:text (cons 'number calc-num))
   ))))

   (add-object MOLE-REPER pr1))

;;====== MOLE-TRACK ===================================================

(let*
  ((pr
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>
           #:hint       "Трек"
           #:color      17
           #:width      10
           #:coord-list 'apl)
   ))))
   (add-object MOLE-TRACK pr))

;;====== MOLE-ELECTRIC-TRACE ==========================================
(let*
  ((line-width 0)
   (supp-width 50)
   (st 200)
   (sb 300)
   (sw 100)
   (dh 20)
   (d1 50)
   (d2 75)
   (d3 100)
   (lim 225)
   (make-line
     (lambda (l0 l1)
       (and-let*
         ((p00 (car  l0))
          (p01 (cadr l0))
          (p10 (car  l1))
          (p11 (cadr l1))
          (d00 (dist p00 p10))
          (d01 (dist p00 p11))
          (d10 (dist p01 p10))
          (d11 (dist p01 p11))
          (dm (min d00 d01 d10 d11))
          (p0 (if (or (= dm d00) (= dm d01)) p00 p01))
          (p1 (if (or (= dm d00) (= dm d10)) p10 p11))
          (dd (dist p0 p1))
          ((> dd lim))
          (a (calc-a p0 p1)))
         (make <Color-Lines>
           #:color owner->color
           #:width line-width
           #:coord-list
             (map
                (lambda (p) (shift p0 (turn p a)))
                (list
                  (cons d1 (- dh)) (cons d1 dh)
                  (cons d2 (- dh)) (cons d2 dh)
                  (cons (- dd d1) (- dh)) (cons (- dd d1) dh)
                  (cons (- dd d2) (- dh)) (cons (- dd d2) dh)
                  (cons d3 0) (cons (- dd d3) 0)))))))

   (mk-supp
     (lambda (p0 p1 p2)
       (let*
         ((x (car p1))
          (y (cdr p1))
          (a0 (and p0 (calc-a p0 p1)))
          (a2 (and p2 (calc-a p2 p1)))
          (a1 (and a0 a2 (/ (+ a0 a2 1800) 2)))
          (a (good-angle (or a1 a0 a2))))
         (map (lambda (p) (shift p1 (turn p a)))
              (list (cons (- sw) 0) (cons sw 0)
                    (cons (- sw) sb) (cons 0 (- st))
                    (cons sw sb) (cons 0 (- st)))))))

   (make-elline
     (lambda ()
       (or
         (and-let*
           ((pl (get-property 'apl))
            ((>= (length pl) 2))
            (sp (map mk-supp (cons #f pl) pl (append (cdr pl) (list #f)))))
           (append
             (map (lambda (lst)
                    (make <Color-Lines>
                       #:color owner->color
                       #:width supp-width
                       #:coord-list lst))
                  sp)
             (memq-remove #f (map make-line sp (cdr sp)))))
         '())))

   (pr
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>
           #:h-visible #f
           #:l-visible #f
           #:hint  "ЛЭП"
           #:coord-min 2
           #:coord-list 'apl)
         (make <Container>
           #:dependence '(apl)
           #:dynamic #t
           #:contents make-elline)
         (make <Text>
           #:dependence '(apl)
           #:color owner->color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:hint "Напряжение"
           #:font h-std-font ;std-font
           #:text 'number)
         (make <Color-Footnote>
           #:dynamic #t
           #:dependence '(apl tx)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-pll-footnote
                   (get-property 'apl)
                   (get-tx) (get-ty) (get-ta)
                   (get-number)
                   (scale-font) low-font-size)
                   )))))))

  (add-object MOLE-ELECTRIC-TRACE pr)
)
;;===== MOLE-CABEL-BRANCH =============================================
(let*
  ((l0 60)
   (h0 120)
   (w0 200)

   (cross (list (cons (- w0) (- h0)) (cons w0 h0)
                (cons w0 (- h0)) (cons (- w0) h0)))

   (lns (list (cons (- w0) (- h0)) (cons w0 (- h0))
              (cons (- w0) h0) (cons w0 h0)))

   (make-test
     (lambda (txt)
       (lambda (st?)
         (let*
           ((x (get-x0))
            (y (get-y0))
            (p (cons x y)))
           (or
             (not x)
             (any
               (lambda (prl)
                 (and-let*
                   ((spl (assv-ref prl 'apl))
                    ((point-on-polyline? p spl 100))
                    ((not (member? p spl)))
                    (n (select-segment x y spl #f))
                    (pl (list-tail spl n))
                    (a (calc-a (car pl) (cadr pl)))
                    (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
                    (p2 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a)))
                   (set-property! 'x0 (car p2))
                   (set-property! 'y0 (cdr p2))
                   (set-property! 'a (+ a 900))
                   #t))
               (get-all-objects p (list MOLE-BASE-PASSAGE)))
             txt)))))

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>
           #:valid-test (make-test "Отвод кабеля должен находиться на пролете")
           #:tool 't0
           #:s-visible #t
           #:dependence '(x0 y0 number tx ty ta)
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number)
                   (quotient (scale-font) 2)
                   #f))))
         (make <Outlined-Circle>
           #:valid-test (make-test "Отвод кабеля должен находиться на пролете")
           #:color     owner->color
           #:width     20
           #:foreground 1
           #:background 1
           #:x-coord    'x0
           #:y-coord    'y0
           #:x-size     90)
         (make <Text>
           #:valid-test (make-test "Отвод кабеля должен находиться на пролете")
           #:label 't0
           #:s-visible #t
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  0
           #:hint "Параметры"
           #:font h-std-font/2
           #:text (cons 'number ask-number)))))

   (pr1
     (make <Container>
       #:contents
       (list
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:valid-test (make-test "Метка должна находиться на пролете")
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle 'a)
         (make <Color-Lines>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color
           #:width      l0
           #:dependence '(x0 y0 a)
           #:coord-list (lambda () (calc-plg cross 1 1)))
         (make <Color-Lines>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color
           #:width      l0
           #:dependence '(x0 y0 a)
           #:coord-list (lambda () (calc-plg cross 1 2)))
           )))

   (pr2
     (make <Container>
       #:contents
       (list
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:valid-test (make-test "Метка должна находиться на пролете")
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle 'a)
         (make <Color-Lines>
           #:h-visible #f
           #:l-visible #t
           #:color      owner->color
           #:width      l0
           #:dependence '(x0 y0 a)
           #:coord-list (lambda () (calc-plg lns 1 1)))
         (make <Color-Lines>
           #:h-visible #t
           #:l-visible #f
           #:color      owner->color
           #:width      l0
           #:dependence '(x0 y0 a)
           #:coord-list (lambda () (calc-plg lns 1 2)))
           )))
           )

  (add-object MOLE-CABEL-BRANCH pr0)
  (add-object MOLE-DESTRUCTED-LABEL pr1)
  (add-object MOLE-UNUSED-LABEL pr2)
)
;;===== SUBLAYER ======================================================
(let*
  ((pr
     (make <Container>
       #:contents
       (list
         (make <Bar>
           #:x-coord 'x
           #:y-coord 'y
           #:x-size  'x1
           #:y-size  'y1
           #:angle 'a
           #:foreground 999
           #:valid-test
             (lambda (st?)
               (or (and (not (get-x1))
                        (not (get-y1)))
                   (begin
                     (if (< (get-x1) 1000)
                       (set-property! 'x1 1000))
                     (if (< (get-y1) 1000)
                       (set-property! 'y1 1000))))))
         (make <Text>
           #:hint "Файл"
           #:h-visible #f
           #:l-visible #f
           #:x-coord 0
           #:y-coord 0
           #:text 'source)
         (make <Text>
           #:s-visible #t
           #:hint "Обозначение"
           #:dependence '( x y a)
           #:color 20
           #:x-coord (lambda () (calc-x (get-x) (get-y) low-font-size/2 low-font-size (get-a)))
           #:y-coord (lambda () (calc-y (get-x) (get-y) low-font-size/2 low-font-size (get-a)))
           #:angle get-a
           #:font std-font
           #:text 'number)
           ))))
  (for-each (lambda (c) (add-object c pr))
            (list (+ MOLE-BASE-SUBLAYER 1)
                  (+ MOLE-BASE-SUBLAYER 2)
                  (+ MOLE-BASE-SUBLAYER 3)
                  (+ MOLE-BASE-SUBLAYER 4))))
;;===== SUBLAYER STATIC ===============================================
(let*
  ((pr
     (make <Container>
       #:contents
       (list
         (make <Bar>
           #:x-coord 'x
           #:y-coord 'y
           #:x-size  'x1
           #:y-size  'y1
           #:angle 'a
           #:foreground 0)
         (make <Text>
           #:hint "Файл"
           #:h-visible #f
           #:l-visible #f
           #:text 'source)))))
  (for-each (lambda (c) (add-object c pr))
            (do ((i (+ MOLE-BASE-SUBSTAT 1) (inc i))
                 (l '() (cons i l)))
                 ((= i (+ MOLE-BASE-SUBSTAT 17)) l))))
;;===== MOLE-R-STATION ================================================
(let*
  ((w 250)
   (h 250)
   (zig
     (lambda (k)
       (let*
         ((w (inexact->exact (* k w)))
          (h (inexact->exact (* k h))))
         (turn-points
           (list (cons 0 (- h)) (cons (* w -1/2) 0)
                 (cons (* w -1/2) 0) (cons (* w 1/2) 0)
                 (cons (* w 1/2) 0) (cons 0 h))
           (cons (get-x0) (get-y0))
           (get-a)))))
   (pr
     (make <Container>
       #:contents
       (list
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  1
           #:y-size  1
           #:angle 'a
           #:hint "Станция")
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number) low-font-size/2 #f))))
         (make <Outlined-Bar>
           #:dependence '(x0 y0 a)
           #:x-coord (lambda ()(calc-x (get-x0) (get-y0) (- w) (- h) (get-a)))
           #:y-coord (lambda ()(calc-y (get-x0) (get-y0) (- w) (- h) (get-a)))
           #:x-size (+ w w)
           #:y-size (+ h h)
           #:angle get-a
           #:width 30
           #:color owner->color
           #:foreground 1)
         (make <Color-Lines>
           #:dependence '(x0 y0 a)
           #:x-coord get-x0
           #:y-coord get-y0
           #:angle get-a
           #:width 30
           #:color owner->color
           #:coord-list (lambda () (zig .8)))
         (make <Text>
           #:label 't0
           #:hint "Номер"
           #:color owner->color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle 'ta
           #:font std-font
           #:text 'number)
           ))))
  (add-object MOLE-R-STATION pr)
)
;;===== MOLE-R-MAST ===================================================
(let*
  ((r 120)
   (zig
     (lambda (k)
       (let*
         ((r (inexact->exact (* k r)))
          (p0 (cons (get-x0) (get-y0))))
         (map
           (lambda (p) (shift p0 p))
           (list (cons 0 (- r)) (cons (* r -1/2) 0)
                 (cons (* r -1/2) 0) (cons (* r 1/2) 0)
                 (cons (* r 1/2) 0) (cons 0 r))))))
   (pr
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number) low-font-size/2 #f))))
         (make <Outlined-Circle>
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size r
           #:color owner->color
           #:foreground 1
           #:width 30
           #:hint "Мачта")
         (make <Color-Lines>
           #:dependence '(x0 y0)
           #:x-coord get-x0
           #:y-coord get-y0
           #:angle get-a
           #:width 20
           #:color owner->color
           #:coord-list (lambda () (zig .8)))
         (make <Text>
           #:label 't0
           #:s-visible #t
           #:hint "Номер"
           #:color owner->color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle 'ta
           #:font low-font/2
           #:text 'number)
           ))))
  (add-object MOLE-R-MAST pr)
)
;;===== MOLE-R-ANTENNA ================================================
(let*
  ((r 10)
   (h 360)
   (k 2)
   (d 40)
   (zig
     (lambda ()
       (map
         (lambda (p) (shift p (cons (get-x0) (get-y0))))
         (list '(0 . 0) (cons d h) (cons (- d) h)))))
   (make-arcs
     (lambda ()
       (map
         (lambda (r)
           (make <Color-Arc>
             #:x-coord get-x0
             #:y-coord get-y0
             #:x-size  r
             #:y-size  r
             #:color owner->color
             #:angle (* k r -1)
             #:delta (* k r 2)
             #:width 10
            ))
          (list 25 50 75))))
   (pr
     (make <Container>
       #:contents
       (list
         (make <point>
           #:x-coord 'x0
           #:y-coord 'y0
           #:hint "Антенна")
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (+ (get-y0) 300)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number) low-font-size/2 #f))))
         (make <Filled-Polygon>
           #:dependence '(x0 y0)
           #:foreground owner->color
           #:coord-list zig)
         (make <Container>
           #:dynamic #t
           #:dependence '(x0 y0)
           #:contents make-arcs)
         (make <Text>
           #:label 't0
           #:s-visible #t
           #:hint "Номер"
           #:color owner->color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle 'ta
           #:font low-font/2
           #:text 'number)
           ))))
  (add-object MOLE-R-ANTENNA pr)
)
;;===== MOLE-S-ANTENNA ================================================
(let*
  ((r 120)
   (d 48)
   (s 15)
   (zig
     (lambda ()
       (map
         (lambda (p) (shift p (cons (get-x0) (get-y0))))
         (list (cons (- d) 0) (cons d (- d)) (cons 0 d)))))
   (pr
     (make <Container>
       #:contents
       (list
         (make <point>
           #:x-coord 'x0
           #:y-coord 'y0
           #:hint "Антенна")
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (- (get-x0) d)
                   (+ (get-y0) d)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number) low-font-size/2 #f))))
         (make <Chord>
           #:dependence '(x0 y0)
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size r
           #:y-size r
           #:angle 1500
           #:delta 1500
           #:foreground owner->color)
         (make <Color-Polyline>
           #:dependence '(x0 y0)
           #:width 0
           #:color owner->color
           #:coord-list zig)
         (make <Circle>
           #:dependence '(x0 y0)
           #:foreground owner->color
           #:x-coord (lambda () (+ (get-x0) d))
           #:y-coord (lambda () (- (get-y0) d))
           #:x-size s
           #:y-size s)
         (make <Text>
           #:label 't0
           #:s-visible #t
           #:hint "Номер"
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle 'ta
           #:font low-font/2
           #:text 'number)
           ))))
  (add-object MOLE-S-ANTENNA pr)
)
;;===== MOLE-WF-POINT =================================================
(let*
  ((pr
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number) (scale-font) #f))))
         (make <Icon>
           #:x-coord 'x0
           #:y-coord 'y0
           #:ref 0
           #:hint "Точка доступа")
         (make <Text>
           #:label 't0
           #:s-visible #t
           #:hint "Номер"
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle 'ta
           #:font h-std-font
           #:text 'number)
           ))))
  (add-object MOLE-WF-POINT pr)
)
;;===== MOLE-BASE-STATION =============================================

(let*
  ((w2 120)
   (h2 480)
   (w3 80)
   (h31 60)
   (h32 80)
   (lw 30)

   (bas (lambda ()
     (turn-points (list (cons (- w2) h2) (cons 0 (- h2)) (cons w2 h2))
                  (cons (get-x0) (get-y0))
                  (get-a))))

   (tre (lambda ()
     (turn-points
       (list
         (cons (- w3) (- 0 h2 h31 h31))
         (cons (- w3) (- 0 h2 h31))
         (cons 0 (- 0 h2 h31 h32))
         (cons 0 (- 0 h2))
         (cons w3 (- 0 h2 h31 h31))
         (cons w3 (- 0 h2 h31))
         (cons (- w3) (- 0 h2 h31))
         (cons w3 (- 0 h2 h31)))
       (cons (get-x0) (get-y0))
       (get-a))))

   (pr
     (make <Container>
       #:contents
       (list
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   'a
           #:hint "Базовая станция")
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 number tx ty ta)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0)
                   (get-y0)
                   (lim-tx) (lim-ty) (get-ta)
                   (get-number) low-font-size/2 #f))))
         (make <Outlined-Polygon>
           #:dependence '(x0 y0 a)
           #:width lw
           #:color owner->color
           #:foreground 1
           #:coord-list bas)
         (make <Color-Lines>
           #:dependence '(x0 y0 a)
           #:width lw
           #:color owner->color
           #:coord-list tre)
         (make <Text>
           #:label 't0
           #:hint "Номер"
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle 'ta
           #:font h-std-font
           #:text 'number)
           ))))
  (add-object MOLE-BASE-STATION pr)
)
;;=====================================================================
