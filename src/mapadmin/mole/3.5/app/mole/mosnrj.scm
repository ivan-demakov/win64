;======================================================================
(define (get-x0)(get-property 'x0))
(define (get-y0)(get-property 'y0))
(define (get-x1)(get-property 'x1))
(define (get-y1)(get-property 'y1))
(define (get-x2)(get-property 'x2))
(define (get-y2)(get-property 'y2))
(define (get-a)(or (get-property 'a) 0))
(define (get-a0)(or (get-property 'a0) 0))
(define (get-a1)(or (get-property 'a1) 0))
(define (get-a2)(or (get-property 'a2) 0))
(define (get-tx)(get-property 'tx))
(define (get-ty)(get-property 'ty))
(define (get-ta)(or (get-property 'ta) 0))
(define (get-number) (get-property 'number))
(define (get-apl) (get-property 'apl))
(define (copy-target host-old host-new target) target)
;======================================================================
(define mufta-r 75)
;======================================================================
(define (scale-font) 125)
(define (std-font) (vector 0 (- (scale-font))))
(define (h-std-font) (vector 0 (- (scale-font))))

(define low-font-size 420)
(define low-font-size/2 (/ low-font-size 3))
;======================================================================
(define BASE-SUBLAYER 1024)

(define nrj-comment            1)
(define nrj-text               2)
(define nrj-text-by-line       3)
(define nrj-size-line          4)
(define nrj-stvor              5)
(define nrj-radius-line        6)
(define nrj-reper              7)

(define nrj-sity-objects      11)
(define nrj-building          12)
(define nrj-fence             13)
(define nrj-fence-concret     14)
(define nrj-fence-metall      15)

(define nrj-ngnr-commn        21)
(define nrj-water-pipe        22)
(define nrj-water-streem      23)
(define nrj-gas-pipe          24)
(define nrj-drenage-pipe      25)
(define nrj-chanal            26)
(define nrj-phone-chanal      27)
(define nrj-warm-pipe         28)

(define nrj-objects          301)

(define nrj-cbl-objects      302)

(define nrj-cab-objects      303)
(define nrj-cab-110-220-isol 304)
(define nrj-cab-110-220-pipe 305)
(define nrj-cab-110-lops     306)
(define nrj-cab-110-220-tele 307)
(define nrj-cab-35           308)
(define nrj-cab-6-20-feed    309)
(define nrj-cab-6-20-dist    310)
(define nrj-cab-1-distr      311)
(define nrj-cab-cast         312)
(define nrj-cab-prox         313)

(define nrj-cab-condition   314)
(define nrj-cab-ddvn        315)
(define nrj-cab-pipe        316)
(define nrj-cab-block       317)
(define nrj-GNB             318)

(define nrj-mufta           319)
(define nrj-mufta-2         320)
(define nrj-mufta-3         321)

(define nrj-arm             322)
(define nrj-arm-wooden      323)
(define nrj-arm-wooden-1    324)
(define nrj-arm-wooden-2    325)
(define nrj-arm-wooden-3    326)
(define nrj-arm-wooden-4    327)
(define nrj-arm-metal       328)
(define nrj-arm-metal-1     329)
(define nrj-arm-metal-2     330)
(define nrj-arm-metal-3     331)
(define nrj-arm-metal-4     332)
(define nrj-arm-metal-5     333)
(define nrj-arm-concr       334)
(define nrj-arm-concr-1     335)
(define nrj-arm-concr-2     336)
(define nrj-arm-concr-3     337)
(define nrj-arm-illum       338)
(define nrj-arm-illum-1     339)
(define nrj-arm-illum-2     340)
(define nrj-arm-illum-3     341)
(define nrj-arm-illum-4     342)
(define nrj-EL              343)
(define nrj-EL-1            344)
(define nrj-EL-2            345)
(define nrj-EL-3            346)
(define nrj-EL-4            347)
(define nrj-collector       348)
(define nrj-well            349)
(define nrj-well-1          350)
(define nrj-well-2          351)
(define nrj-well-3          352)
(define nrj-well-4          353)
(define nrj-well-5          354)
(define nrj-well-6          355)
(define nrj-netobj          356)
(define nrj-netobj-1        357)
(define nrj-netobj-2        358)
(define nrj-netobj-3        359)
(define nrj-netobj-4        360)
(define nrj-netobj-5        361)
(define nrj-netobj-6        362)
(define nrj-input           363)
;======================================================================
(define (calc-last-a)
  (or
    (and-let*
      ((pl (get-apl))
       (l (length pl))
       ((>= l 2)))
      (calc-a (car (list-tail pl (- l 2))) (car (last-pair pl))))
    0))
;======================================================================
; nrj-cab-objects =====================================================
(define cabels (list nrj-cab-110-220-isol ;304
                     nrj-cab-110-220-pipe ;305
                     nrj-cab-110-lops     ;306
                     nrj-cab-110-220-tele ;307
                     nrj-cab-35           ;308
                     nrj-cab-6-20-feed    ;309
                     nrj-cab-6-20-dist    ;310
                     nrj-cab-1-distr      ;311
                     nrj-cab-cast         ;312
                     nrj-cab-block        ;317
                     ))
(let*
  ((tl cabels)
   (text-color 0)
   (font-size 125)
   (font (vector 0 (- font-size)))
   (sl (list '(#:solid . 19)
             '(#:solid . 19)
             '(#:solid . 19)
             '10
             #:solid
             #:solid
             #:solid
             #:solid
             11
             12))
   (cl (list '(304 . 0)
             '(305 . 304)
             '(306 . 0)
             307
             308
             309
             310
             311
             312
             317))
   (tc (list 0
             0
             0
             307
             0
             309
             310
             311
             312
             0))
   (hl (list "КЛ 110-220кВ с ПЭ изоляцией"
             "КЛ 110-220кВ высокого давления в трубе"
             "КЛ 110кВ низкого давления"
             "КЛ телесигнализации 110-220кВ"
             "КЛ 35кВ"
             "КЛ питающей сети 6-20кВ"
             "КЛ распределительной сети 6-20кВ"
             "КЛ распределительной сети до 1кВ"
             "КЛ другой организации"
             "КЛ в блочной канализиции"
             )))

  (define (mk-shape)
    (let*
      ((pl (get-apl))
       (shl (or (get-property '&shape-list) (map (lambda (x) 0) pl))))
      (make-shape pl shl)))

   (define (test pl start?)

     (define (mufta-points mufta)
       (let*
         ((type (assq-ref mufta class-sym))
          (x0 (assq-ref mufta 'x0))
          (y0 (assq-ref mufta 'y0))
          (p0 (cons x0 y0)))
         (if (memq? type (list nrj-cab-prox nrj-input))
           (list p0)
           (let*
             ((a  (assq-ref mufta 'a))
              (t0 (shift p0 (turn (cons mufta-r 0) a)))
              (t1 (shift p0 (turn (cons (- mufta-r) 0) a)))
              (t2 (shift p0 (turn (cons 0 (- mufta-r)) a))))
             (if (= type nrj-mufta-2)
               (list t0 t1)
               (list t0 t1 t2))))))

     (define (set-point! pl pnts)
       (and-let*
         ((tt (car pl))
          (dm (map (lambda (p) (dist p tt)) pnts))
          ((not (null? dm)))
          (md (apply min dm)))
         (for-each
           (lambda (d p)
             (if (= d md)
               (set-car! pl p)))
           dm pnts)))

     (define (test-point begin?)
       (let*
         ((pl (if begin? pl (last-pair pl)))
          (mumu (get-all-objects (car pl) (list nrj-mufta-2 nrj-mufta-3 nrj-cab-prox nrj-input)))
          (pnts (apply append (map mufta-points mumu))))
         (set-point! pl pnts)
         (list-select mumu
           (lambda (mufta)
             (member? (car pl) (mufta-points mufta))))))

     (let*
       ((mns (test-point #t))
        (mnf (test-point #f))
        (vns (length mns))
        (vnf (length mnf)))

;       (and-let*
;         (((not start?))
;          ((not edition?))
;          (ln (length pl))
;          ((>= ln 2)))
;         (set-property! 'apl pl))

       (cond
         ((and (= vns 0) (= vnf 0)))
         ((= vns 0) "Кабель должен начинаться в устройстве" )
         ((= vnf 0) ;"Кабель должен заканчиваться в устройстве")
           (cond
             (edition? "Кабель должен заканчиваться в устройстве")
             ((eq? (tk-message-box
                      "Кабель должен заканчиваться в устройстве\n\nПостроить здесь?"
                      "Внимание!" 'yes-no) 'yes)
              (set! &suspend-object? #t))
             (else #f)))
         ((and start? (> vns 1)) "Неоднозначное начало кабеля!")
         ((and (not start?) (> vnf 1)) "Неоднозначный конец кабеля!")
         ((and (equal? (car mns) (car mnf)) (or (not start?) edition?)) "Кабель не должен заканчиваться в том же устройстве!")
         )))

  (define (calc-len)
    (or
      (and-let*
        ((pl (get-apl)))
        (poly-length->str pl))
      ""))

  (define (make-cabel st lc tc h)
    (make <Container>
      #:contents
      (append
        (if (pair? st)
          (list
            (make <Color-Polyline>
              #:shaped #t
              #:color (car lc)
              #:line (car st)
              #:width 13
              #:hint h
              #:valid-test test
              #:coord-list 'apl)
            (make <Color-Polyline>
              #:dependence '(apl)
              #:color (cdr lc)
              #:line (cdr st)
              #:width 0
              #:coord-list mk-shape)
              )
          (list
            (make <Color-Polyline>
              #:shaped #t
              #:color lc
              #:line st
              #:width 12
              #:hint h
              #:valid-test test
              #:coord-list 'apl)))
        (list
          (make <Text>
            #:color   tc
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:dependence '(apl)
            #:hint    "Длина"
            #:font font
            #:text (cons 'number calc-len))
          (make <Color-Footnote>
            #:dynamic #t
            #:dependence '(apl tx ty 'number)
            #:color 0
            #:width 0
            #:coord-list
              (cons
                'vpl
                (lambda ()
                  (make-pll-footnote
                    (get-apl) (get-tx) (get-ty) (get-ta)
                    (get-number) font-size font-size))))))))

  (define (insert-mufta-2 x y)
    (and-let*
      ((spl (get-property 'apl))
       (n (select-segment x y spl #f))
       (l (length spl))
       (or (= n 0) (= n (- l 2)))
       (xy (cons x y))
       (pl (if (> l 2)
         (if (= n 0) (list-head spl 2) (reverse (list-tail spl (- l 2))))
         (if (< (dist xy (car spl)) (dist xy (car (last-pair spl)))) spl (reverse spl))))
       (a (calc-a (car pl) (cadr pl)))
       (p0 (step-by-line (car pl) (cadr pl) (- mufta-r))))
      (list
        (cons 'a  a)
        (cons 'x0 (car p0))
        (cons 'y0 (cdr p0)))))

  (define (insert-cab-ddvn x y)
    (and-let*
      ((spl (get-property 'apl))
       (n (select-segment x y spl #f))
       (pl (list-tail spl n))
       (a (calc-a (car pl) (cadr pl)))
       (z0 (make-z (car pl) (cadr pl)))
       (z1 (/ (make-z (car pl) (cons x y)) z0))
       (z2 (* (make-rectangular (real-part z1) 0) z0))
       (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
       (p1 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a)))
      (list
        (cons 'a0  (good-angle a))
        (cons 'a1  (good-angle a))
        (cons 'x0 (+ (caar pl) (real-part z2)))
        (cons 'y0 (+ (cdar pl) (imag-part z2)))
        )))

  (for-each
    (lambda (t s lc tc h)
      (add-object t (make-cabel s lc tc h)))
    tl sl cl tc hl)

  (add-context
    cabels
    (list nrj-mufta-2 nrj-mufta-3)
    insert-mufta-2
    copy-target
    )

  (add-context
    cabels
    (list nrj-cab-prox nrj-input)
    append-point-object
    copy-target)

  (add-context
    cabels
    nrj-cab-ddvn
    insert-cab-ddvn
    copy-target)
)
; nrj-cab-prox ========================================================
(let
  ((r 18))

  (define cont
    (make <Container>
      #:contents
      (list
        (make <Circle>
          #:x-coord 'x0
          #:y-coord 'y0
          #:x-size r
          #:hint "Муфта-2"))))

  (add-object nrj-cab-prox cont)

  (add-context
    (list nrj-cab-prox nrj-input)
    cabels
    insert-line-object
    coedit-line-object)
)
; nrj-mufta-2 =========================================================
(let*
  ((fill-color 0)
   (a-grid '(15 . 0))
   (r mufta-r)
   (cp .3)
   (h (* r .4))
   (w (inexact->exact (* r cp)))
   (w/2 (/ w 2)))

  (define (make-shape-2)
    (let
      ((a (get-a))
       (p0 (cons (get-x0) (get-y0))))
      (define (mp p)
        (shift p0 (turn p a)))
      (do
        ((dx (- r) (+ dx 20))
         (lst0 '())
         (lst1 '()))
        ((>= dx r) (map mp (append lst0 lst1)))
         (set! lst0 (cons (cons dx (* cp (sqrt (- (* r r) (* dx dx))))) lst0))
         (set! lst1 (cons (cons (- dx) (* cp (- (sqrt (- (* r r) (* dx dx)))))) lst1)))))

  (define (make-shape-21)
    (turn-points
      (list (cons (- w/2 r) (- h)) (cons (- w/2 r) h)
            (cons (- r w/2) (- h)) (cons (- r w/2) h))
      (cons (get-x0) (get-y0))
      (get-a)))

  (define cont-2
    (make <Container>
      #:contents
      (list
        (make <Color-Rectangle>
          #:h-visible #f
          #:l-visible #f
          #:x-coord 'x0
          #:y-coord 'y0
          #:a-grid  a-grid
          #:angle   'a
          #:hint "Муфта-2")
        (make <Filled-Polygon>
          #:foreground fill-color
          #:dependence '(x0 y0 a)
          #:coord-list make-shape-2)
        (make <Color-Lines>
          #:color fill-color
          #:width w
          #:dependence '(x0 y0 a)
          #:coord-list make-shape-21))))

  (define (make-shape-3)
    (let
      ((a (get-a))
       (p0 (cons (get-x0) (get-y0))))
      (define (mp p)
        (shift p0 (turn p a)))
      (do
        ((dy (- r) (+ dy 20))
         (dx 0)
         (lst0 '())
         (lst1 '()))
        ((>= dy -10) (map mp (append lst0 (cdr (reverse lst1)))))
         (set! dx (* cp (sqrt (- (* r r) (* dy dy)))))
         (set! lst0 (cons (cons dx dy) lst0))
         (set! lst1 (cons (cons (- dx) dy) lst1))
         )))

  (define (make-shape-31)
    (turn-points
        (list (cons (- w/2 r) (- h)) (cons (- w/2 r) h)
              (cons (- r w/2) (- h)) (cons (- r w/2) h)
              (cons (- h) (- w/2 r)) (cons h (- w/2 r)))
      (cons (get-x0) (get-y0))
      (get-a)))

  (define cont-3
    (make <Container>
      #:contents
      (list
        (make <Color-Rectangle>
          #:h-visible #f
          #:l-visible #f
          #:x-coord 'x0
          #:y-coord 'y0
          #:a-grid  a-grid
          #:angle   'a
          #:hint "Муфта-3")
        (make <Filled-Polygon>
          #:foreground fill-color
          #:dependence '(x0 y0 a)
          #:coord-list make-shape-2)
        (make <Filled-Polygon>
          #:foreground fill-color
          #:dependence '(x0 y0 a)
          #:coord-list make-shape-3)
        (make <Color-Lines>
          #:color fill-color
          #:width w
          #:dependence '(x0 y0 a)
          #:coord-list make-shape-31))))

  (add-object nrj-mufta-2 cont-2)
  (add-object nrj-mufta-3 cont-3)

  (add-context
    nrj-mufta-2
    cabels
    (lambda (x y)
      (let*
        ((tt (cons x y))
         (p0 (cons (get-x0) (get-y0)))
         (a (get-a))
         (t0 (shift p0 (turn (cons r 0) a)))
         (t1 (shift p0 (turn (cons (- r) 0) a)))
         (d0 (dist tt t0))
         (d1 (dist tt t1))
         (p1 (if (< d0 d1) t0 t1)))
        (list (cons 'apl (list p1)))))
    (lambda (host-old host-new target)
      (and-let*
        ((p0-old (cons (assv-ref host-old 'x0)
                       (assv-ref host-old 'y0)))
         (a-old (assv-ref host-old 'a))
         (p0-new (cons (assv-ref host-new 'x0)
                       (assv-ref host-new 'y0)))
         (a-new (assv-ref host-new 'a))
         (apl (assv-ref target 'apl))
         (t0-old (shift p0-old (turn (cons r 0) a-old)))
         (t1-old (shift p0-old (turn (cons (- r) 0) a-old)))
         (t0-new (shift p0-new (turn (cons r 0) a-new)))
         (t1-new (shift p0-new (turn (cons (- r) 0) a-new)))
         (d0 (dist t0-old (car apl)))
         (d1 (dist t1-old (car apl)))
         (d2 (dist t0-old (car (last-pair apl))))
         (d3 (dist t1-old (car (last-pair apl))))
         (dm (min d0 d1 d2 d3)))
        (cond
          ((= dm d0) (set-car! apl t0-new))
          ((= dm d1) (set-car! apl t1-new))
          ((= dm d2) (set-car! (last-pair apl) t0-new))
          ((= dm d3) (set-car! (last-pair apl) t1-new)))
        (assv-set! target 'apl apl)))
    (lambda (host test)
      (and-let*
        ((x0 (assv-ref host 'x0))
         (y0 (assv-ref host 'y0))
         (a (assv-ref host 'a))
         (apl (assv-ref test 'apl))
         (p0 (cons x0 y0))
         (t0 (shift p0 (turn (cons r 0) a)))
         (t1 (shift p0 (turn (cons (- r) 0) a))))
        (cond
          ((equal? t0 (car apl)) '(#:undef . #:begin))
          ((equal? t1 (car apl)) '(#:undef . #:begin))
          ((equal? t0 (car (last-pair apl))) '(#:undef . #:end))
          ((equal? t1 (car (last-pair apl))) '(#:undef . #:end))
          ))))

  (add-context
    nrj-mufta-3
    cabels
    (lambda (x y)
      (let*
        ((tt (cons x y))
         (p0 (cons (get-x0) (get-y0)))
         (a (get-a))
         (t0 (shift p0 (turn (cons r 0) a)))
         (t1 (shift p0 (turn (cons (- r) 0) a)))
         (t2 (shift p0 (turn (cons 0 (- r)) a)))
         (d0 (dist tt t0))
         (d1 (dist tt t1))
         (d2 (dist tt t2))
         (dd (min d0 d1 d2))
         (p1 (cond ((= dd d0) t0)
                   ((= dd d1) t1)
                   ((= dd d2) t2))))
        (list (cons 'apl (list p1)))))
    (lambda (host-old host-new target)
      (and-let*
        ((p0-old (cons (assv-ref host-old 'x0)
                       (assv-ref host-old 'y0)))
         (a-old (assv-ref host-old 'a))
         (p0-new (cons (assv-ref host-new 'x0)
                       (assv-ref host-new 'y0)))
         (a-new (assv-ref host-new 'a))
         (apl (assv-ref target 'apl))
         (t0-old (shift p0-old (turn (cons r 0) a-old)))
         (t1-old (shift p0-old (turn (cons (- r) 0) a-old)))
         (t2-old (shift p0-old (turn (cons 0 (- r)) a-old)))
         (t0-new (shift p0-new (turn (cons r 0) a-new)))
         (t1-new (shift p0-new (turn (cons (- r) 0) a-new)))
         (t2-new (shift p0-new (turn (cons 0 (- r)) a-new)))
         (d0 (dist t0-old (car apl)))
         (d1 (dist t1-old (car apl)))
         (d2 (dist t2-old (car apl)))
         (d3 (dist t0-old (car (last-pair apl))))
         (d4 (dist t1-old (car (last-pair apl))))
         (d5 (dist t2-old (car (last-pair apl))))
         (dm (min d0 d1 d2 d3 d4 d5)))
        (cond
          ((= dm d0) (set-car! apl t0-new))
          ((= dm d1) (set-car! apl t1-new))
          ((= dm d2) (set-car! apl t2-new))
          ((= dm d3) (set-car! (last-pair apl) t0-new))
          ((= dm d4) (set-car! (last-pair apl) t1-new))
          ((= dm d5) (set-car! (last-pair apl) t2-new)))
        (assv-set! target 'apl apl)))
    (lambda (host test)
      (and-let*
        ((x0 (assv-ref host 'x0))
         (y0 (assv-ref host 'y0))
         (a (assv-ref host 'a))
         (apl (assv-ref test 'apl))
         (p0 (cons x0 y0))
         (t0 (shift p0 (turn (cons r 0) a)))
         (t1 (shift p0 (turn (cons (- r) 0) a)))
         (t2 (shift p0 (turn (cons 0 (- r)) a))))
        (cond
          ((equal? t0 (car apl)) '(#:undef . #:begin))
          ((equal? t1 (car apl)) '(#:undef . #:begin))
          ((equal? t2 (car apl)) '(#:undef . #:begin))
          ((equal? t0 (car (last-pair apl))) '(#:undef . #:end))
          ((equal? t1 (car (last-pair apl))) '(#:undef . #:end))
          ((equal? t2 (car (last-pair apl))) '(#:undef . #:end)))))))
; =====================================================================
(let*
  ((line-color 0)
   (line-width 5)
   (text-color 0)
   (r 200)
   (font-size 125)
   (font (vector 0 (- font-size)))
   )

  (define beg-ln '((0 . 0) (0 . -100) (90 . -100)))
  (define beg-ar '((100 . -100) (70 . -115) (70 . -85)))
  (define end-ln '((0 . 0) (0 . -100) (-90 . -100)))
  (define end-ar '((-100 . -100) (-70 . -115) (-70 . -85)))
  (define (mk-subs) (string-append "гл." (or (get-number) "?") "м" ))

  (define (make-beg)
    (list
      (make <Color-Polyline>
        #:color line-color
        #:width line-width
        #:dependence '(x1 y1 a1)
        #:coord-list (lambda () (turn-points beg-ln (cons (get-x1) (get-y1)) (get-a1))))
      (make <Filled-Polygon>
        #:foreground line-color
        #:dependence '(x1 y1 a1)
        #:coord-list (lambda () (turn-points beg-ar (cons (get-x1) (get-y1)) (get-a1))))
      (make <Text>
;;        #:dynamic #t
        #:tool 'l0
        #:color line-color
        #:dependence '(x1 y1 a1)
        #:x-coord get-x1
        #:y-coord get-y1
        #:angle get-a1
        #:x-offset 100
        #:y-offset -10
        #:font font
        #:text mk-subs)
        ))

  (define (make-end)
    (list
      (make <Color-Polyline>
        #:color line-color
        #:width line-width
        #:dependence '(x2 y2 a2)
        #:coord-list (lambda () (turn-points end-ln (cons (get-x2) (get-y2)) (get-a2))))
      (make <Filled-Polygon>
        #:foreground line-color
        #:dependence '(x2 y2 a2)
        #:coord-list (lambda () (turn-points end-ar (cons (get-x2) (get-y2)) (get-a2))))
      (make <Text>
        #:dynamic #t
        #:tool 'l0
        #:color line-color
        #:dependence '(x2 y2 a2)
        #:x-coord get-x2
        #:y-coord get-y2
        #:angle get-a2
        #:x-offset (lambda () (- -100 (calc-string-width (mk-subs) font (get-a2))))
        #:y-offset -10
        #:font font
        #:text mk-subs)))

  (define cont
    (make <Container>
      #:contents
      (append
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x1
            #:y-coord 'y1
            #:x-size 100
            #:angle   'a1
            #:hint "Отклонение (начало)"))
        (make-beg)
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x2
            #:y-coord 'y2
            #:x-size -100
            #:angle   'a2
            #:hint "Отклонение (конец)"))
        (make-end)
        (list
          (make <Text>
            #:h-visible #f
            #:l-visible #f
            #:label 'l0
            #:text 'number
            #:hint "Глубина (м)"))
        )))

  (add-object nrj-cab-ddvn cont)
)
; nrj-arm =============================================================
(let*
  ((line-color 0)
   (line-width 5)
   (fill-color 1)
   (a-grid '(15 . 0))
   (r 50)
   (h -175)
   (h1 (/ h 2))
   (w 40)
   (fc (cons (- w) (- w)))
   )

  (define (make-stolb)
    (let*
      ((p0 (cons (get-x0) (get-y0))))
      (list p0 (shift p0 (turn (cons 0 h) (get-a))))))

  (define (cont-1 fill hint)
    (make <Container>
      #:contents
      (list
        (make <Color-Rectangle>
          #:h-visible #f
          #:l-visible #f
          #:x-coord 'x0
          #:y-coord 'y0
          #:a-grid  a-grid
          #:angle   'a
          #:hint hint)
        (make <Color-Polyline>
          #:color line-color
          #:width line-width
          #:dependence '(x0 y0 a)
          #:coord-list make-stolb)
        (make <Outlined-Circle>
          #:color line-color
          #:width line-width
          #:foreground fill
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:x-size r)
          )))

  (define (make-stolbik)
    (let*
      ((p0 (cons (get-x0) (get-y0)))
       (p1 (cons (get-x1) (get-y1))))
      (list p0 p1)))

  (define (cont-2)
    (make <Container>
      #:contents
      (list
        (make <Color-Rectangle>
          #:h-visible #f
          #:l-visible #f
          #:x-coord 'x0
          #:y-coord 'y0
          #:a-grid  a-grid
          #:angle   'a
          #:hint "Столб деревянный с подкосом")
        (make <Color-Polyline>
          #:color line-color
          #:width line-width
          #:dependence '(x0 y0 a)
          #:coord-list make-stolb)
        (make <Color-Polyline>
          #:color line-color
          #:width 0
          #:line #:dash
          #:dependence '(x0 y0 x1 y1)
          #:coord-list make-stolbik)
        (make <Outlined-Circle>
          #:color line-color
          #:width line-width
          #:foreground fill-color
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:x-size r)
        (make <Outlined-Circle>
          #:color line-color
          #:width line-width
          #:foreground fill-color
          #:x-coord 'x1
          #:y-coord 'y1
          #:x-size (/ r 2)
          #:hint "Подкос")
          )))

  (define (cont-3 line-style)
    (make <Container>
      #:contents
      (list
        (make <Color-Polyline>
          #:color line-color
          #:width 0
          #:line line-style
          #:dependence '(x0 y0 x1 y1)
          #:coord-list make-stolbik)
        (make <Outlined-Circle>
          #:color line-color
          #:width line-width
          #:foreground fill-color
          #:x-coord 'x0
          #:y-coord 'y0
          #:x-size r
          #:hint "Столб 1")
        (make <Outlined-Circle>
          #:color line-color
          #:width line-width
          #:foreground fill-color
          #:x-coord 'x1
          #:y-coord 'y1
          #:x-size r
          #:hint "Столб 2")
          )))

  (define (calc-w)
    (abs (* 2 (max (get-x1) (get-y1)))))

  (define (calc-farm)
    (let*
      ((p0 (cons (get-x0) (get-y0)))
       (w (/ (calc-w) -2)))
      (shift p0 (turn (cons w w) (get-a)))))

  (define (make-farm fill)
    (let*
      ((f0 '(-1 . -1))
       (f1 '(-1 . 1))
       (f2 '(1 . -1))
       (f3 '(1 . 1)))

      (define (calc-c off)
        (let*
          ((a (get-a))
           (p0 (cons (get-x0) (get-y0)))
           (w (/ (calc-w) -2)))
          (shift
            (shift p0 (turn (cons (* w (car off)) (* w (cdr off))) a))
            (turn fc a))))

      (map
        (lambda (off)
          (make <Outlined-Bar>
            #:foreground fill
            #:color line-color
            #:width line-width
            #:dependence '(x0 y0 x1 y1 a)
            #:x-coord (lambda () (car (calc-c off)))
            #:y-coord (lambda () (cdr (calc-c off)))
            #:x-size (* 2 w)
            #:y-size (* 2 w)
            #:angle get-a))
        (list f0 f1 f2 f3))))

  (define (cont-4 fill hint)
    (make <Container>
      #:contents
      (append
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:x-size 'x1
            #:y-size 'y1
            #:a-grid  a-grid
            #:angle   'a
            #:hint hint)
          (make <Color-Rectangle>
            #:color line-color
            #:width 0
            #:line #:dash
            #:dependence '(x0 y0 x1 y1 a)
            #:x-coord (lambda () (car (calc-farm)))
            #:y-coord (lambda () (cdr (calc-farm)))
            #:x-size calc-w
            #:y-size calc-w
            #:angle get-a))
        (make-farm fill))))

  (define (make-met-stolb k)
    (let*
      ((a (get-a))
       (p0 (cons (get-x0) (get-y0))))
      (list (shift p0 (turn (cons 0 (* k h1)) a))
            (shift p0 (turn (cons 0 (* k (- h1))) a)))))

  (define (make-met-2 k)
    (let*
      ((a (get-a))
       (p0 (cons (get-x0) (get-y0))))
      (shift p0 (turn (cons (- w) (- (* k w))) a))))

  (define (cont-5 k hint)
    (make <Container>
      #:contents
      (list
        (make <Color-Rectangle>
          #:h-visible #f
          #:l-visible #f
          #:x-coord 'x0
          #:y-coord 'y0
          #:a-grid  a-grid
          #:angle   'a
          #:hint hint)
        (make <Color-Polyline>
          #:color line-color
          #:width line-width
          #:dependence '(x0 y0 a)
          #:coord-list (lambda () (make-met-stolb k)))
        (make <Bar>
          #:color line-color
          #:foreground 0
          #:dependence '(x0 y0 a)
          #:x-coord (lambda () (car (make-met-2 k)))
          #:y-coord (lambda () (cdr (make-met-2 k)))
          #:x-size (* 2 w)
          #:y-size (* 2 w k)
          #:angle   get-a)
          )))

  (define (clc-a)
    (or
      (and-let*
        ((x0 (get-x0))
         (x1 (get-x1))
         (y0 (get-y0))
         (y1 (get-y1)))
        (calc-a (cons x1 y1) (cons x0 y0)))
      0))

  (define (calc-c1)
    (let*
      ((p0 (cons (get-x0) (get-y0))))
      (shift p0 (turn fc (clc-a)))))

  (define (calc-c2)
    (let*
      ((p1 (cons (get-x1) (get-y1))))
      (shift p1 (turn fc (clc-a)))))

  (define (cont-6)
    (make <Container>
      #:contents
      (list
        (make <Color-Polyline>
          #:color line-color
          #:width 0
          #:line #:dash
          #:dependence '(x0 y0 x1 y1)
          #:coord-list make-stolbik)
        (make <Point>
          #:x-coord 'x0
          #:y-coord 'y0
          #:hint "Опора 1")
        (make <Bar>
          #:dynamic #t
          #:foreground 0
          #:dependence '(x0 y0)
          #:x-coord (lambda () (car (calc-c1)))
          #:y-coord (lambda () (cdr (calc-c1)))
          #:x-size (* 2 w)
          #:y-size (* 2 w)
          #:angle clc-a)
        (make <Point>
          #:x-coord 'x1
          #:y-coord 'y1
          #:hint "Опора 2")
        (make <Bar>
          #:dynamic #t
          #:foreground 0
          #:dependence '(x0 y0 x1 y1)
          #:x-coord (lambda () (car (calc-c2)))
          #:y-coord (lambda () (cdr (calc-c2)))
          #:x-size (* 2 w)
          #:y-size (* 2 w)
          #:angle clc-a)
          )))

  (define (mk-light)
    (define (calc-c)
      (let*
        ((p0 (cons (get-x0) (get-y0))))
        (shift p0 (turn (cons r (+ 10 h)) (get-a)))))

      (list
        (make <Outlined-Chord>
          #:color 0
          #:width line-width
          #:foreground 1
          #:dependence '(x0 y0)
          #:x-coord (lambda () (car (calc-c)))
          #:y-coord (lambda () (cdr (calc-c)))
          #:x-size r
          #:y-size r
          #:angle (lambda () (+ (get-a) 1800))
          #:delta 1800)))

  (define (cont-7 sqr? blk? lgt? hint)
    (make <Container>
      #:contents
      (append
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:a-grid  a-grid
            #:angle   'a
            #:hint hint)
          (make <Color-Polyline>
            #:color line-color
            #:width line-width
            #:dependence '(x0 y0 a)
            #:coord-list make-stolb)
          (if sqr?
            (make <Outlined-Bar>
              #:color line-color
              #:width line-width
              #:foreground (if blk? 0 1)
              #:dependence '(x0 y0)
              #:angle get-a
              #:x-coord (lambda () (car (make-met-2 1)))
              #:y-coord (lambda () (cdr (make-met-2 1)))
              #:x-size (* 2 w)
              #:y-size (* 2 w))
            (make <Outlined-Circle>
              #:color line-color
              #:width line-width
              #:foreground (if blk? 0 1)
              #:dependence '(x0 y0)
              #:x-coord get-x0
              #:y-coord get-y0
              #:x-size r))
          (make <Outlined-Circle>
            #:color line-color
            #:width line-width
            #:foreground 1
            #:dependence '(x0 y0)
            #:x-coord get-x0
            #:y-coord get-y0
            #:x-size (/ r 5)))
        (if lgt? (mk-light) '())
        )))

  (define (cont-8)
    (make <Container>
      #:contents
      (list
        (make <Color-Rectangle>
          #:h-visible #f
          #:l-visible #f
          #:x-coord 'x0
          #:y-coord 'y0
          #:a-grid  a-grid
          #:angle   'a
          #:hint "ж/б фермовый")
        (make <Color-Polyline>
          #:color line-color
          #:width line-width
          #:dependence '(x0 y0 a)
          #:coord-list (lambda () (make-met-stolb 1)))
        (make <Outlined-Bar>
          #:color line-color
          #:foreground 1
          #:dependence '(x0 y0 a)
          #:x-coord (lambda () (car (make-met-2 1)))
          #:y-coord (lambda () (cdr (make-met-2 1)))
          #:x-size (* 2 w)
          #:y-size (* 2 w)
          #:angle   get-a)
        (make <Outlined-Circle>
          #:color line-color
          #:width line-width
          #:foreground 1
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:x-size (/ r 5))
        )))

  (add-object nrj-arm-wooden-1 (cont-1 1 "Столб деревянный"))
  (add-object nrj-arm-wooden-2 (cont-2))
  (add-object nrj-arm-wooden-3 (cont-3 #:dash))
  (add-object nrj-arm-wooden-4 (cont-4 1 "Ферма деревянная"))

  (add-object nrj-arm-metal-1  (cont-1 0 "Столб металлический"))
  (add-object nrj-arm-metal-2  (cont-5 1 "Столб мет. фермовый"))
  (add-object nrj-arm-metal-3  (cont-5 2 "Столб мет. фермовый широкий"))
  (add-object nrj-arm-metal-4  (cont-6))
  (add-object nrj-arm-metal-5  (cont-4 0 "Ферма мет. 4 опоры"))

  (add-object nrj-arm-concr-1  (cont-7 #f #f #f "Столб ж/б"))
  (add-object nrj-arm-concr-2  (cont-8))
  (add-object nrj-arm-concr-3  (cont-3 #:dot))

  (add-object nrj-arm-illum-1  (cont-7 #f #f #t "Опора осв. на ж/б столбе"))
  (add-object nrj-arm-illum-2  (cont-7 #f #t #t "Опора осв. на мет. столбе"))
  (add-object nrj-arm-illum-3  (cont-7 #t #f #t "Опора осв. на ж/б ферме"))
  (add-object nrj-arm-illum-4  (cont-7 #t #t #t "Опора осв. на мет. ферме"))
)
; nrj-well ============================================================
(let*
  ((r 50)
   (line-width 5)
   (line-color 0))

  (define insd
    '#(((-20 . -20) (20 . -20) (-20 . 20) (20 . 20))
       ((-20 . -20) (20 . -20) (0 . -20) (0 . 20))
       ((-10 . -20) (10 . -20) (-10 . -20) (-10 . 20))
       ((-50 . 0) (-10 . 0) (10 . 0) (50 . 0) (0 . -50) (0 . -10) (0 . 10) (0 . 50))
       ((0 . -50) (0 . 50) (-50 . 0) (50 . 0))
       ()))

  (define (mkw i)
    (list
      (make <Color-Lines>
        #:color line-color
        #:width line-width
        #:dependence '(x0 y0 a)
        #:coord-list (lambda () (turn-points (vector-ref insd i) (cons (get-x0) (get-y0)) (get-a))))))

  (define (cont ndx hint)
    (make <Container>
      #:contents
      (append
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:angle   'a
            #:hint hint)
          (make <Outlined-Circle>
            #:color line-color
            #:width line-width
            #:foreground 1
            #:dependence '(x0 y0)
            #:x-coord get-x0
            #:y-coord get-y0
            #:x-size r))
           (mkw ndx))))

  (add-object nrj-well-1 (cont 0 "Колодец телефонный"))
  (add-object nrj-well-2 (cont 1 "Колодец теплосети"))
  (add-object nrj-well-3 (cont 2 "Колодец газопровода"))
  (add-object nrj-well-4 (cont 3 "Колодец дренажный"))
  (add-object nrj-well-5 (cont 4 "Колодец водопровода"))
  (add-object nrj-well-6 (cont 5 "Колодец канализации"))
)
; nrj-EL ===============================================================
(let*
  ((line-width 3)
   (line-color 0)
   (l 250)
   (m (/ l 2)))

  (define arr-1
    (vector
      (list (cons 0 0) (cons l 0)
            (cons (- l 10) 0) (cons (- l 30) -10)
            (cons (- l 10) 0) (cons (- l 30) 10)
            (cons (- l 30) 0) (cons (- l 50) -10)
            (cons (- l 30) 0) (cons (- l 50) 10))
      (list (cons 0 0) (cons l 0)
            (cons (- l 10) 0) (cons (- l 30) -10)
            (cons (- l 10) 0) (cons (- l 30) 10))
      (list (cons 0 0) (cons l 0)
            (cons (- l 10) 0) (cons (- l 30) -10)
            (cons (- l 10) 0) (cons (- l 30) 10)
            (cons (- l 30) 0) (cons (- l 50) -10)
            (cons (- l 30) 0) (cons (- l 50) 10))
      (list (cons 0 0) (cons l 0)
            (cons (- l 10) 0) (cons (- l 30) -10)
            (cons (- l 10) 0) (cons (- l 30) 10))  ))

  (define arr-2
    (vector
      (list (cons 0 0) (cons l 0)
            (cons (- l 10) 0) (cons (- l 30) -10)
            (cons (- l 10) 0) (cons (- l 30) 10)
            (cons (- l 30) 0) (cons (- l 50) -10)
            (cons (- l 30) 0) (cons (- l 50) 10))
      (list (cons 0 0) (cons l 0)
            (cons (- l 10) 0) (cons (- l 30) -10)
            (cons (- l 10) 0) (cons (- l 30) 10))
      (list (cons 0 0) (cons m 0)
            (cons m -20) (cons m 20))
      (list (cons 0 0) (cons m 0)
            (cons m -20) (cons m 20))  ))

  (define (mk-arr-1 ndx)
    (let*
      ((p0 (cons (get-x0) (get-y0)))
       (p1 (cons (get-x1) (get-y1)))
       (a (calc-a p0 p1)))
      (turn-points (vector-ref arr-1 ndx) p0 a)))

  (define (mk-arr-2 ndx)
    (let*
      ((p0 (cons (get-x0) (get-y0)))
       (p1 (cons (get-x2) (get-y2)))
       (a (calc-a p0 p1)))
      (turn-points (vector-ref arr-2 ndx) p0 a)))

  (define (cont ndx hint)
    (make <Container>
      #:contents
        (list
          (make <Point>
            #:x-coord 'x0
            #:y-coord 'y0
            #:hint hint)
          (make <Point>
            #:x-coord 'x1
            #:y-coord 'y1)
          (make <Color-Lines>
            #:color line-color
            #:width line-width
            #:dependence '(x0 y0 x1 y1)
            #:coord-list (lambda () (mk-arr-1 ndx)))
          (make <Point>
            #:x-coord 'x2
            #:y-coord 'y2)
          (make <Color-Lines>
            #:color line-color
            #:width line-width
            #:dependence '(x0 y0 x2 y2)
            #:coord-list (lambda () (mk-arr-2 ndx))))
           ))

  (add-object nrj-EL-1 (cont 0 "ЛЭП ВН"))
  (add-object nrj-EL-2 (cont 1 "ЛЭП НН"))
  (add-object nrj-EL-3 (cont 2 "Переход от возд. ВН к каб."))
  (add-object nrj-EL-4 (cont 3 "Переход от возд. НН к каб."))
)
; nrj-GNB =============================================================
(let*
  ((line-color 0)
   (line-width 5)
   (mh 25)
   (dh 50)
   )

  (define (calc-h)
    (or
      (and-let*
        ((ch (get-y1)))
        (max mh (abs ch)))
      mh))

  (define (calc-last-x)
    (or
      (and-let*
        ((pl (get-apl))
         (l (length pl))
         ((>= l 2)))
        (caar (last-pair pl)))
      0))

  (define (calc-last-y)
    (or
      (and-let*
        ((pl (get-apl))
         (l (length pl))
         ((>= l 2)))
        (cdar (last-pair pl)))
      0))

  (define (mk-line h)

    (define (calc-p1 pl)
      (shift (car pl) (turn (cons 0 h) (calc-a (car pl) (cadr pl)))))

    (define (calc-pn pl)
      (shift (cadr pl) (turn (cons 0 h) (calc-a (car pl) (cadr pl)))))

    (define (calc-pm pl)

      (define (cp tl)
        (let*
          ((ll (cdr tl))
           (p0 (calc-p1 tl))
           (p1 (calc-pn tl))
           (p2 (calc-p1 ll))
           (p3 (calc-pn ll)))
        (crs-point p0 p1 p2 p3)))

      (let loop ((l '()) (tl pl))
        (cond
          ((null? l) (loop (list (calc-p1 tl)) tl))
          ((>= (length tl) 3) (loop (append l (list (cp tl))) (cdr tl)))
          (else (append l (list (calc-pn tl)))))))

    (or
      (and-let*
        ((pl (get-apl)))
        (calc-pm pl))
      '()))

  (define (mk-ln2)
    (let*
      ((pl (get-apl))
       (a0 (calc-a (car pl) (cadr pl)))
       (a1 (calc-last-a))
       (p0 (car pl))
       (p1 (car (last-pair pl)))
       (h (calc-h))
       (l0 (list (cons 0 (- h)) (cons 0 (- 0 h dh))
                 (cons 0 h) (cons 0 (+ h dh)))))
      (append
        (turn-points l0 p0 a0)
        (turn-points l0 p1 a1))))

  (define cont
    (make <Container>
      #:contents
        (list
          (make <Color-Polyline>
            #:h-visible #f
            #:l-visible #f
            #:hint ""
            #:coord-list 'apl)
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:dependence '(apl)
            #:x-coord calc-last-x
            #:y-coord calc-last-y
            #:angle calc-last-a
            #:x-size 'x1
            #:y-size 'y1
            #:hint "ГНБ")
          (make <Outlined-Polygon>
            #:dynamic #t
            #:foreground 1
            #:color line-color
            #:width line-width
            #:dependence '(apl)
            #:coord-list (lambda () (append (mk-line (calc-h)) (reverse (mk-line (- (calc-h)))))))
          (make <Color-Lines>
            #:color line-color
            #:width line-width
            #:dependence '(apl)
            #:coord-list mk-ln2)
           )))

  (add-object nrj-GNB cont)
)
; nrj-cab-pipe ========================================================
(let*
  ((line-color 0)
   (line-width 20)
   (h 50)
   )

  (define (mk-ln2)
    (let*
      ((pl (get-apl))
       (a0 (calc-a (car pl) (cadr pl)))
       (a1 (calc-last-a))
       (p0 (car pl))
       (p1 (car (last-pair pl)))
       (l0 (list (cons 0 (- h)) (cons 0 h))))
      (append
        (turn-points l0 p0 a0)
        (turn-points l0 p1 a1))))

  (define cont
    (make <Container>
      #:contents
        (list
          (make <Color-Polyline>
            #:color line-color
            #:width line-width
            #:hint "Труба"
            #:coord-list 'apl)
          (make <Color-Lines>
            #:color line-color
            #:width line-width
            #:dependence '(apl)
            #:coord-list mk-ln2)
           )))

  (add-object nrj-cab-pipe cont)
)
;  nrj-netobj =========================================================
(let*
  ((line-color-1 357)
   (line-color-3 359)
   (line-color-4 360)
   (fill-color-1 1)
   (fill-color-2 0)
   (r1 120)
   (r2 60)
   (r3 100)
   (r4 200)
   (r5 300)
   (r6 100)
   (line-width-1 30)
   (line-width-3 5)
   (line-width-4 5)
   (text-color 0)
   (font-size 125)
   (font (vector 0 (- font-size)))
  )

  (define cont-1
    (make <Container>
      #:contents
        (list
          (make <Outlined-Circle>
            #:color line-color-1
            #:foreground fill-color-1
            #:width line-width-1
            #:x-coord 'x0
            #:y-coord 'y0
            #:x-size r1
            #:hint "Распр. пункт питающей сети")
          (make <Circle>
            #:foreground fill-color-2
            #:x-coord get-x0
            #:y-coord get-y0
            #:x-size r2)
          (make <Text>
            #:color   text-color
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:hint    "Обозначение"
            #:font font
            #:text 'number)
           )))

  (define cont-2
    (make <Container>
      #:contents
        (list
          (make <Circle>
            #:foreground fill-color-2
            #:x-coord 'x0
            #:y-coord 'y0
            #:x-size r2
            #:hint "Тр. подстанция распр. сети")
          (make <Text>
            #:color   text-color
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:hint    "Обозначение"
            #:font font
            #:text 'number)
           )))

  (define (mk-brd)
    (let*
      ((k (* 2.5 r3)))
      (turn-points
        (list (cons 0 0) (cons (- r3) r3)
              (cons (- r3) r3) (cons (- r3) k)
              (cons 0 0) (cons 0 k)
              (cons 0 0) (cons r3 r3)
              (cons  r3 r3) (cons r3 k))
        (cons (get-x0) (get-y0)) (get-a))))

  (define cont-3
    (make <Container>
      #:contents
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:angle 'a
            #:hint "РП, ТП, РТП с отводом возд. линий")
          (make <Color-Lines>
            #:dependence '(x0 y0 a)
            #:color line-color-3
            #:width line-width-3
            #:coord-list mk-brd)
          (make <Text>
            #:color   text-color
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:hint    "Обозначение"
            #:font font
            #:text 'number)
           )))

  (define (mk-triangle)
    (let*
      ((d (* r4 (/ (sqrt 3) 2))))
      (turn-points
        (list (cons (- d) (/ r4 2)) (cons 0  (- r4)) (cons  d (/ r4 2)))
        (cons (get-x0) (get-y0)) (get-a))))

  (define cont-4
    (make <Container>
      #:contents
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:angle 'a
            #:hint "РП, ТП, РТП с отводом возд. линий")
          (make <Outlined-Polygon>
            #:dependence '(x0 y0 a)
            #:color line-color-4
            #:foreground fill-color-1
            #:width line-width-4
            #:coord-list mk-triangle)
          (make <Circle>
            #:foreground fill-color-2
            #:x-coord get-x0
            #:y-coord get-y0
            #:x-size r2)
          (make <Text>
            #:color   text-color
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:hint    "Обозначение"
            #:font font
            #:text 'number)
           )))

  (define cont-5
    (make <Container>
      #:contents
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:angle 'a
            #:hint "ТП с указанием секций шин")
          (make <Circle>
            #:foreground fill-color-2
            #:x-coord get-x0
            #:y-coord get-y0
            #:x-size r2)
          (make <Text>
            #:color   text-color
            #:x-coord get-x0
            #:y-coord get-y0
            #:angle   get-a
            #:x-offset (inexact->exact (* r2 -3.5))
            #:y-offset (/ r2 2)
            #:font font
            #:text "А")
          (make <Text>
            #:color   text-color
            #:x-coord get-x0
            #:y-coord get-y0
            #:angle   get-a
            #:x-offset (* r2 2)
            #:y-offset (/ r2 2)
            #:font font
            #:text "Б")
          (make <Text>
            #:color   text-color
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:hint    "Обозначение"
            #:font font
            #:text 'number)
           )))

  (define (calc-x1) (max r1 (get-x1)))
  (define (calc-y1) (max r1 (get-y1)))

  (define (calc-off-0)
    (shift (cons (get-x0) (get-y0)) (turn (cons (+ 0 r2 (calc-x1)) 0) (get-a))))

  (define (calc-off-1)
    (shift (cons (get-x0) (get-y0)) (turn (cons (- 0 r2 (calc-x1)) 0) (get-a))))

  (define (calc-off)
    (shift (cons (get-x0) (get-y0)) (turn (cons (calc-x1) (calc-y1)) (get-a))))

  (define (mk-zig)
    (turn-points
      (list (cons (- r6) (- r6)) (cons r6 (- r6))
            (cons (- r6) r6) (cons r6 r6))
      (cons (get-x0) (get-y0))
      (get-a)))

  (define (mk-arr)
    (turn-points
      (list (cons r6 r6) (cons (- r6 25) (- r6 15)) (cons (- r6 25) (+ r6 15)))
      (cons (get-x0) (get-y0))
      (get-a)))

  (define cont-6
    (make <Container>
      #:contents
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:x-size 'x1
            #:y-size 'y1
            #:angle 'a
            #:hint "ТП с указанием секций шин")
          (make <Outlined-Bar>
            #:dependence '(x0 y0 x1 y1 a)
            #:foreground 1
            #:color 0
            #:width line-width-4
            #:x-coord (lambda () (car (calc-off)))
            #:y-coord (lambda () (cdr (calc-off)))
            #:x-size (lambda () (* -2 (calc-x1)))
            #:y-size (lambda () (* -2 (calc-y1)))
            #:angle get-a)
          (make <Color-Polyline>
            #:color 0
            #:width line-width-4
            #:dependence '(x0 y0 x1 y1 a)
            #:coord-list mk-zig)
          (make <Filled-Polygon>
            #:foreground 0
            #:dependence '(x0 y0 x1 y1 a)
            #:coord-list mk-arr)
          (make <Circle>
            #:dependence '(x0 y0 x1 y1 a)
            #:foreground fill-color-2
            #:x-coord (lambda () (car (calc-off-0)))
            #:y-coord (lambda () (cdr (calc-off-0)))
            #:x-size r2)
          (make <Circle>
            #:dependence '(x0 y0 x1 y1 a)
            #:foreground fill-color-2
            #:x-coord (lambda () (car (calc-off-1)))
            #:y-coord (lambda () (cdr (calc-off-1)))
            #:x-size r2)
          (make <Text>
            #:color   text-color
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:hint    "Обозначение"
            #:font font
            #:text 'number)
           )))

  (add-object nrj-netobj-1 cont-1)
  (add-object nrj-netobj-2 cont-2)
  (add-object nrj-netobj-3 cont-3)
  (add-object nrj-netobj-4 cont-4)
  (add-object nrj-netobj-5 cont-5)
  (add-object nrj-netobj-6 cont-6)
)
; nrj-collector =======================================================
(let
  ((line-color 0))

  (define cont
    (make <Container>
      #:contents
        (list
          (make <Color-Polyline>
            #:shaped #t
            #:color line-color
            #:line 13
            #:coord-list 'apl
            #:hint "Контур коллектора.колодца"))))

  (add-object nrj-collector cont)
)
; nrj-comment =========================================================
(let*
  ((text-color 0)
   (font-size 125)
   (font (vector 0 (- font-size)))
  )

   (define (test s) (and (string? s)(positive? (string-length s))))

   (define cont-1
     (make <Container>
       #:contents
       (list
         (make <Text>
           #:valid-test test
           #:font font
           #:color text-color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:hint    "Текст"
           #:text    'number))))

   (define cont-2
     (make <Container>
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
         (make-text-by-line font 0 'apl 'number))))


  (add-object nrj-text cont-1)
  (add-object nrj-text-by-line cont-2)
)
; nrj-size ============================================================
(let*
  ((line-color 0)
   (line-width 0)
   (text-color 0)
   (font-size 125)
   (font (vector 0 (- font-size)))
   (dl 25)
   (ll 100))

  (define (calc-par fun)

    (define arr '((0 . 0) (100 . -30) (100 . 30)))

    (define (calc-p t0 t1 x y)
      (let*
        ((d (dist-to-line x y t0 t1))
         (a (calc-a t0 t1)))
        (shift t0 (turn (cons 0 d) a))))

    (define (calc-lns1 p0 ds sw dt a arr-out?)
      (turn-points
        (if arr-out?
          (list (cons (- dl) 0)
                (cons (- 0 ll ll) 0)
                (cons (+ ds dl) 0)
                (cons (+ ds ll ll) 0))
          (list (cons (+ dl dl) 0)
                (cons (- dt dl) 0)
                (cons (+ dt sw dl) 0)
                (cons (- ds dl dl) 0)))
        p0 a))

    (define (calc-lns2 t0 t1 x y)
      (or
        (and-let*
          (((and x y))
           (d (dist-to-line x y t0 t1))
           ((> (abs d) ll))
           (a (calc-a t0 t1))
           (d1 ((if (positive? d) + -) d ll))
           (dp (turn (cons 0 d1) a)))
          (list t0 (shift t0 dp) t1 (shift t1 dp)))
        '()))

    (let*
      ((apl (get-apl))
       (t0 (car apl))
       (t1 (cadr apl))
       (dx (get-tx))
       (dy (get-ty))
       (d (if dx (dist-to-line dx dy t0 t1) 0))
       (l2? (> (abs d) ll))
       (p0 (if l2? (calc-p t0 t1 dx dy) t0))
       (p1 (if l2? (calc-p t1 t0 dx dy) t1))
       (ds (dist p0 p1))
       (a0 (calc-a p0 p1))
       (a1 (calc-a p1 p0))
       (ga (good-angle a0))
       (dr? (= a0 ga))
       (st (remove-kwasi-cursor (or (get-number) "")))
       (sw (calc-string-width st font ga))
       (tp (cons (or (get-tx) 0) (or (get-ty) 0)))
       (s0 (turn (offset p0 tp) (- a0)))
       (arr-out? (< (- ds 400 sw) 0))
       (dt (min (- ds 200 sw)  (max 200 (car s0)))))
      (cond
       ((eq? fun #:beg-arr)
         (turn-points arr p0 (if arr-out? a1 a0)))
       ((eq? fun #:end-arr)
         (turn-points arr p1 (if arr-out? a0 a1)))
       ((eq? fun #:txt-ang) ga)
       ((eq? fun #:txt-x) (car p0))
       ((eq? fun #:txt-y) (cdr p0))
       ((eq? fun #:txt-ofy)
         (if arr-out? (- dl) (- (/ font-size 2) dl)))
       ((eq? fun #:txt-ofx)
         (if arr-out?
           (if dr? (/ (- ds sw) 2)  (/ (+ ds sw) -2))
           (if dr? dt (- 0 dt sw))))
       ((eq? fun #:lines)
         (append (calc-lns1 p0 ds sw dt a0 arr-out?)
                 (calc-lns2 t0 t1 dx dy))))))

  (define (calc-number pl)
    (let*
      ((d (dist (car pl) (cadr pl))))
      (set-property! 'number (number->string (/ (inexact->exact (+ d .5)) 100)))))

  (define (calc-line len)
    (let*
      ((c (or (string-index len #\,) -1))
       (ln (if (>= c 0) (string-append (substring len 0 c) "." (substring len (inc c))) len))
       (l (or (string->number ln) 10))
       (pl (get-apl))
       (dx (- (caadr pl) (caar pl)) )
       (dy (- (cdadr pl) (cdar pl)) )
       (d (dist (car pl) (cadr pl)))
       (x (inexact->exact (/ (* 100 l dx) d)))
       (y (inexact->exact (/ (* 100 l dy) d)))
       (lst (list (car pl) (shift (car pl) (cons x y)))))
      (set-property! 'apl lst)))

  (define cont-1
    (make <Container>
      #:contents
        (list
          (make <Color-Polyline>
            #:h-visible #f
            #:l-visible #f
            #:coord-num 2
            #:at-final calc-number
            #:coord-list 'apl
            #:hint "Размерая линия")
          (make <Color-Lines>
            #:foreground text-color
            #:dependence '(apl)
            #:coord-list (lambda () (calc-par #:lines)))
          (make <Filled-Polygon>
            #:dynamic #t
            #:foreground text-color
            #:dependence '(apl)
            #:coord-list (lambda () (calc-par #:beg-arr)))
          (make <Filled-Polygon>
            #:dynamic #t
            #:foreground text-color
            #:dependence '(apl)
            #:coord-list (lambda () (calc-par #:end-arr)))
          (make <Text>
            #:h-visible #f
            #:l-visible #f
            #:valid-test valid-num?
            #:at-final calc-line
            #:x-coord  'tx
            #:y-coord  'ty
            #:hint "Размер"
            #:text 'number)
          (make <Text>
            #:dependence '(apl tx ty number)
            #:color   text-color
            #:x-coord  (lambda () (calc-par #:txt-x))
            #:y-coord  (lambda () (calc-par #:txt-y))
            #:angle    (lambda () (calc-par #:txt-ang))
            #:x-offset (lambda () (calc-par #:txt-ofx))
            #:y-offset (lambda () (calc-par #:txt-ofy))
            #:font font
            #:text get-number)
           )))

  (define (calc-ta)
    (let
      ((pl (get-apl)))
    (good-angle (calc-a (car pl) (cadr pl)))))

  (define (clc-toff)
    (let*
      ((apl (get-apl))
       (p0 (car apl))
       (p1 (cadr apl))
       (ds (dist p0 p1))
       (a0 (calc-a p0 p1))
       (ga (good-angle a0))
       (dr? (= a0 ga))
       (st (remove-kwasi-cursor (or (get-number) "")))
       (sw (calc-string-width st font ga))
       (tp (cons (or (get-tx) 0) (or (get-ty) 0)))
       (s0 (turn (offset p0 tp) (- a0)))
       (dt (min (- ds 200 sw)  (max 200 (car s0)))))
      (cons (if dr? dt (- 0 dt sw))
             (- dl))))

  (define cont-2
    (make <Container>
      #:contents
        (list
          (make <Color-Polyline>
            #:s-visible #t
            #:coord-num 2
            #:at-final calc-number
            #:coord-list 'apl
            #:hint "Размерая линия круговая")
          (make <Color-Ring>
            #:color line-color
            #:width 0
            #:dependence '(apl)
            #:x-coord  (lambda () (caar (get-apl)))
            #:y-coord  (lambda () (cdar (get-apl)))
            #:x-size  (lambda () (poly-length (get-apl))))
          (make <Text>
            #:h-visible #f
            #:l-visible #f
            #:valid-test valid-num?
            #:at-final calc-line
            #:hint "Размер"
            #:x-coord  'tx
            #:y-coord  'ty
            #:text 'number)
          (make <Text>
            #:s-visible #t
            #:dependence '(apl number tx ty)
            #:color   text-color
            #:font font
            #:x-coord  (lambda () (caar (get-apl)))
            #:y-coord  (lambda () (cdar (get-apl)))
            #:x-offset  (lambda () (car (clc-toff)))
            #:y-offset  (lambda () (cdr (clc-toff)))
            #:angle    calc-ta
            #:hint "Размер"
            #:text get-number)
           )))

  (define cont-3
    (make <Container>
      #:contents
        (list
          (make <Color-Polyline>
            #:color text-color
            #:width 0
            #:line #:dot
            #:coord-list 'apl
            #:coord-num 2)
           )))

  (add-object nrj-size-line cont-1)
  (add-object nrj-radius-line cont-2)
  (add-object nrj-stvor cont-3)
)
; nrj-reper============================================================
(let*
  ((line-color 0)
   (line-width 12)
   (dl 200))

  (define rl
    '((-100 . 0) (100 . 0)
      (0 . 0) (0 . -200)))

  (define cont
    (make <Container>
      #:contents
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:angle   'a
            #:hint "Репер")
          (make <Color-Lines>
            #:dependence '(x0 y0 a)
            #:color line-color
            #:width line-width
            #:x-coord get-x0
            #:y-coord get-y0
            #:angle   get-a
            #:coord-list (lambda () (turn-points rl (cons (get-x0) (get-y0)) (get-a))))
           )))

  (add-object nrj-reper cont)
)
; nrj-sity-objects ====================================================
(let*
  ((text-color 0)
   (font (h-std-font))
   (line-color 0)
   (fill-color 1)
   (line-width 20)
  )

  (define cont-1
    (make <Container>
      #:contents
      (list
        (make <Smart-Polygon>
          #:shaped     #t
          #:color      line-color
          #:foreground 2
          #:background -1
          #:fill       #:diagcross
          #:width      line-width
          #:hint       "Строение"
          #:coord-list 'apg)
       (make <Text>
         #:color text-color
         #:font font
         #:x-coord 'tx
         #:y-coord 'ty
         #:angle   'ta
         #:hint    "Номер/обозначение"
         #:text    'number))))

  (define (cont-2 lt hint)
    (make <Container>
      #:contents
        (list
          (make <Color-Polyline>
            #:shaped #t
            #:color line-color
            #:line lt
            #:coord-list 'apl
            #:hint hint))))

  (add-object nrj-building cont-1)
  (add-object nrj-fence-concret (cont-2 15 "Ограда каменная/железобетонная"))
  (add-object nrj-fence-metall (cont-2 22 "Ограда металлическая"))
)
;======================================================================
;;===== SUBLAYER ======================================================
(let*
  ((pr
     (make <Container>
       #:contents
       (list
         (make <Bar>
           #:x-coord 'x0
           #:y-coord 'y0
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
           #:dependence '( x0 y0 a)
           #:color 20
           #:x-coord (lambda () (calc-x (get-x0) (get-y0) low-font-size/2 low-font-size (get-a)))
           #:y-coord (lambda () (calc-y (get-x0) (get-y0) low-font-size/2 low-font-size (get-a)))
           #:angle get-a
           #:font std-font
           #:text 'number)
           ))))
  (for-each (lambda (c) (add-object c pr))
            (list (+ BASE-SUBLAYER 1)
                  (+ BASE-SUBLAYER 2)
                  (+ BASE-SUBLAYER 3)
                  (+ BASE-SUBLAYER 4))))
; nrj-ngnr-commn ======================================================
(let*
  (
  )

  (define type-list
    (list nrj-water-pipe
          nrj-water-streem
          nrj-gas-pipe
          nrj-drenage-pipe
          nrj-chanal
          nrj-phone-chanal
          nrj-warm-pipe))
  (define clr-list
    (list 0
          0
          0
          0
          0
          27
          0))
  (define line-list
    (list 14
          16
          18
          17
          20
          21
          23))
  (define hint-list
    (list "Водопровод"
          "Водосток"
          "Газопровод"
          "Дренаж"
          "Канализация"
          "Телефонная канализация"
          "Теплоровод"))

  (define (cont c l h)
    (make <Container>
      #:contents
        (list
          (make <Color-Polyline>
            #:shaped #t
            #:color c
            #:line l
            #:coord-list 'apl
            #:hint h))))

  (for-each
    (lambda (t c l h)
      (add-object t (cont c l h)))
    type-list
    clr-list
    line-list
    hint-list))
; nrj-input============================================================
(let*
  ((fill-color 1)
   (line-color 0)
   (line-width 6)
   (r 18)
   (s (* r .5 (sqrt 3)))
  )

  (define rl
    (list
      (cons (- s) (* r -.5))
      (cons  s  (* r -.5))
      (cons 0 r)))

  (define cont
    (make <Container>
      #:contents
        (list
          (make <Color-Rectangle>
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'x0
            #:y-coord 'y0
            #:angle   'a
            #:hint "Репер")
          (make <Outlined-Polygon>
            #:dependence '(x0 y0 a)
            #:color line-color
            #:width line-width
            #:foreground fill-color
            #:coord-list (lambda () (turn-points rl (cons (get-x0) (get-y0)) (get-a))))
           )))

  (add-object nrj-input cont)

  (add-context
    nrj-building
    nrj-input
    (lambda (x y)
      (and-let*
        ((apg (get-property 'apg))
         (spl (enbound #t apg))
         (n (select-segment x y spl #f))
         (pl (list-head (list-tail spl n) 2))
         (a (calc-a (car pl) (cadr pl)))
         (da (if (positive? (clockwise? spl)) 0 1800))
         (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
         (p1 (turn-point (car p0) (+ (cdar pl) 150) (caar pl) (cdar pl) a))
         (p2 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a)))
        (list
          (cons 'x0 (car p2))
          (cons 'y0 (cdr p2))
          (cons 'a (+ a da)))))
  ;;  (make-coedit-point-object 'apg)
    copy-target)
)
;======================================================================
