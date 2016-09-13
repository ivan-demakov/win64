;;======================================================================
;; Объекты электросетей
;;======================================================================
(define el-font-1-size 210)
(define el-font-1 (vector 0 (- el-font-1-size)))
(define (ask-chanals)
  (if (not (get-property 'chanals))
    (set-property! 'chanals "авто"))
  (get-property 'chanals))
;;======================================================================
(add-object MOLE-ELECTRO-COMMENTARY (get-obj-prototype MOLE-COMMENTARY))
;;======================================================================
(define (ts-fill-color)
  (case (logand (get-status) #x7f)
    ((1 3 5) 56)
    ((2 6) 57)
    ((4) 58)
    (else 1)))

(define (ts-line-color)
  (case (logand (get-status) #x7f)
    ((5 6) 58)
    ((3) 57)
    (else 59)))

(define (ts-full-number pfx)
  (string-append pfx " " (get-number)))
;;====== MOLE-ELECTRO-STATIONARY-TS ====================================
(let*
  ((line-width 30)
   (w 300)
   (h 200)

   (pr0 (lambda (hint pfx)
     (make <Container>
       #:contents
       (list
        (make <Color-Footnote>
          #:tool 't0
          #:dependence '(x0 y0 number tx ty ta)
          #:line  #:solid
          #:color 0
          #:width 0
          #:coord-list
            (cons
              'vpl
              (lambda ()
                (make-footnote
                  (get-x0) (get-y0)
                  (lim-tx) (lim-ty) (get-ta)
                  (ts-full-number pfx) low-font-size #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   'a
           #:a-grid  a-grid
           #:hint hint)
         (make <Outlined-Bar>
           #:dependence '(x0 y0 a)
           #:color      ts-line-color
           #:foreground ts-fill-color
           #:width      line-width
           #:x-coord (lambda () (calc-x (get-x0) (get-y0) (- w) (- h) (get-a)))
           #:y-coord (lambda () (calc-y (get-x0) (get-y0) (- w) (- h) (get-a)))
           #:x-size (* w 2)
           #:y-size (* h 2)
           #:angle  get-a)
        (make <Text>
           #:label 't0
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  10
           #:hint "Номер ТП"
           #:text    'number)
        (make <Text>
           #:color owner->color
           #:dependence '(number tx ty ta)
           #:x-coord lim-tx
           #:y-coord lim-ty
           #:angle   get-ta
           #:font    std-font
           #:text (cons 'full-number (lambda () (ts-full-number pfx)))))))))

  (add-object MOLE-ELECTRO-STATIONARY-TS (pr0 "Стационарная ТП" "ТП"))
  (add-object MOLE-ELECTRO-2BKTSN        (pr0 "2БКТПН" "ТП"))

  (add-lord->mole-proc
    (list MOLE-ELECTRO-STATIONARY-TS MOLE-ELECTRO-2BKTSN)
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-rect update-pref-text) lord-list)
                 (update-lord-list (list update-rect update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() (cadr hi) '())))))
;;======================================================================
;;====== MOLE-ELECTRO-MAST-TS ==========================================
(let*
  ((line-width 30)
   (w 100)
   (r 200)

   (pr0
     (make <Container>
       #:contents
       (list
        (make <Color-Footnote>
          #:tool 't0
          #:dependence '(x0 y0 number tx ty ta)
          #:line  #:solid
          #:color 0
          #:width 0
          #:coord-list
            (cons
              'vpl
              (lambda ()
                (make-footnote
                  (get-x0) (get-y0)
                  (lim-tx) (lim-ty) (get-ta)
                  (ts-full-number "МТП") low-font-size #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   'a
           #:a-grid  a-grid
           #:hint    "Мачтовая ТП")
         (make <Outlined-Circle>
           #:dependence '(x0 y0 a)
           #:color      ts-line-color
           #:foreground ts-fill-color
           #:width      line-width
           #:x-coord (lambda () (calc-x (get-x0) (get-y0) (- w) 0 (get-a)))
           #:y-coord (lambda () (calc-y (get-x0) (get-y0) (- w) 0 (get-a)))
           #:x-size r
           #:angle  get-a)
         (make <Outlined-Circle>
           #:dependence '(x0 y0 a)
           #:color      ts-line-color
           #:foreground ts-fill-color
           #:width      line-width
           #:x-coord (lambda () (calc-x (get-x0) (get-y0) (+ w) 0 (get-a)))
           #:y-coord (lambda () (calc-y (get-x0) (get-y0) (+ w) 0 (get-a)))
           #:x-size r
           #:angle  get-a)
        (make <Text>
           #:label 't0
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  10
           #:hint "Номер ТП"
           #:text 'number)
        (make <Text>
           #:color owner->color
           #:dependence '(number tx ty ta)
           #:x-coord lim-tx
           #:y-coord lim-ty
           #:angle   get-ta
           #:font    std-font
           #:text (cons 'full-number (lambda () (ts-full-number "МТП"))))))))

  (add-object MOLE-ELECTRO-MAST-TS pr0)

  (add-lord->mole-proc
    MOLE-ELECTRO-MAST-TS
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-rect update-pref-text) lord-list)
                 (update-lord-list (list update-rect update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() '() (cadr hi) '())))))
;;======================================================================
;;====== MOLE-ELECTRO-KTSN =============================================
(let*
  ((line-width 30)
   (w 300)
   (h 200)

   (zig-zag (lambda ()
     (map
       (lambda (p) (shift (cons (get-x0) (get-y0)) (turn p (get-a))))
       (list (cons (- w) h) (cons (* 1/2 w) (- h)) (cons w h)))))

   (pr0
     (make <Container>
       #:contents
       (list
        (make <Color-Footnote>
          #:tool 't0
          #:dependence '(x0 y0 number tx ty ta)
          #:line  #:solid
          #:color 0
          #:width 0
          #:coord-list
            (cons
              'vpl
              (lambda ()
                (make-footnote
                  (get-x0) (get-y0)
                  (lim-tx) (lim-ty) (get-ta)
                  (ts-full-number "КТПН") low-font-size #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle 'a
           #:a-grid  a-grid
           #:hint "КТПН")
         (make <Outlined-Bar>
           #:dependence '(x0 y0 a)
           #:color      ts-line-color
           #:foreground ts-fill-color
           #:width      line-width
           #:x-coord (lambda () (calc-x (get-x0) (get-y0) (- w) (- h) (get-a)))
           #:y-coord (lambda () (calc-y (get-x0) (get-y0) (- w) (- h) (get-a)))
           #:x-size (* w 2)
           #:y-size (* h 2)
           #:angle  get-a)
         (make <Color-Polyline>
           #:dependence '(x0 y0 a)
           #:color      ts-line-color
           #:width      line-width
           #:coord-list zig-zag)
        (make <Text>
           #:label 't0
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid  a-grid
           #:length  10
           #:hint "Номер ТП"
           #:text 'number)
        (make <Text>
           #:color owner->color
           #:dependence '(number tx ty ta)
           #:x-coord lim-tx
           #:y-coord lim-ty
           #:angle   get-ta
           #:font    std-font
           #:text (cons 'full-number (lambda () (ts-full-number "КТПН"))))))))

  (add-object MOLE-ELECTRO-KTSN pr0)

  (add-lord->mole-proc
    MOLE-ELECTRO-KTSN
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-rect update-pref-text) lord-list)
                 (update-lord-list (list update-rect update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() '() (cadr hi) '())))))
;;======================================================================
;;====== MOLE-ELECTRO-SUPPORT-35-ROUND =================================
;;====== MOLE-ELECTRO-INPUT ============================================
(let*
  ((line-width 30)
   (class-list (list MOLE-ELECTRO-SUPPORT-35-ROUND
                     MOLE-ELECTRO-SUPPORT-10
                     MOLE-ELECTRO-SUPPORT-6
                     MOLE-ELECTRO-SUPPORT-0.4
                     MOLE-ELECTRO-INPUT
                     MOLE-ELECTRO-WELL))
   (type-index
     (lambda ()
       (type->index class-list)))
   (hint
     (lambda ()
       (vector-ref '#("Опора 35 кВ"
                      "Опора 10 кВ"
                      "Опора 6 кВ"
                      "Опора 0.4 кВ"
                      "Ввод в дом"
                      "Колодец кабельный ")
                   (type-index))))
   (size
     (lambda ()
       (vector-ref '#(200 175 150 125 75 150) (type-index))))

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Outlined-Circle>
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  size
           #:color 59
           #:foreground 1
           #:width line-width
           #:hint hint)))))

  (add-object class-list pr0)

  (add-lord->mole-proc
    class-list
    (lambda (lord-list)
      (update-lord-list (list update-point) lord-list))))
;;======================================================================
;;====== MOLE-ELECTRO-SUPPORT-35-RECT ==================================
(let*
  ((line-width 30)
   (w 300)
   (h 200)
   (ask-a (lambda () (if (not (get-a)) (set-property! 'a 0)) (get-a)))

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle   (cons 'a ask-a)
           #:a-grid  a-grid
           #:hint    "Опора 35 кВ")
         (make <Outlined-Bar>
           #:dependence '(x0 y0)
           #:color      ts-line-color
           #:foreground ts-fill-color
           #:width      line-width
           #:x-coord (lambda () (calc-x (get-x0) (get-y0) (- w) (- h) (ask-a)))
           #:y-coord (lambda () (calc-y (get-x0) (get-y0) (- w) (- h) (ask-a)))
           #:x-size (* w 2)
           #:y-size (* h 2)
           #:angle  ask-a)
           ))))

  (add-object MOLE-ELECTRO-SUPPORT-35-RECT pr0)

  (add-lord->mole-proc
    MOLE-ELECTRO-SUPPORT-35-RECT
    (lambda (lord-list)
      (and-let*
        ((hi (update-lord-list (list update-rect) lord-list)))
        (list (car hi) '())))))
;;======================================================================
;;====== MOLE-ELECTRO-PIPE =============================================
(let*
  ((d 200)
   (c 59)
   (f 1)
   (w 0)

   (make-pipe (lambda (d)
     (or
       (and-let*
         ((apl (get-property 'apl))
          (l (length apl))
          ((> l 1))
          (p0 (car apl))
          (p1 (cadr apl))
          (a0 (calc-a p0 p1))
          (pn1 (list-ref apl (- l 2)))
          (pn (car (last-pair apl)))
          (an (calc-a pn1 pn))
          (off (cons 0 d)))
          (append
            (list (shift p0 (turn off a0)))
            (map
              (lambda ( p0 p1 p2)
                (let*
                  ((a0 (calc-a p0 p1))
                   (a1 (calc-a p1 p2))
                   (of1 (turn off a0))
                   (of2 (turn off a1))
                   (t0 (shift p0 of1))
                   (t1 (shift p1 of1))
                   (t2 (shift p1 of2))
                   (t3 (shift p2 of2)))
                  (or (cross-point t0 t1 t2 t3)
                      t1)))
              apl (cdr apl) (cddr apl))
            (list (shift pn (turn (cons 0 d) an)))))
       '())))

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>
           #:h-visible #f
           #:l-visible #f
           #:coord-list 'apl
           #:hint "Труба")
         (make <Outlined-Polygon>
           #:dependence '(apl)
           #:color c
           #:foreground f
           #:width w
           #:coord-list (lambda () (append (reverse (make-pipe d)) (make-pipe (- d)))))))))

  (add-object MOLE-ELECTRO-PIPE pr0)

  (add-lord->mole-proc
    MOLE-ELECTRO-PIPE
    (lambda (lord-list)
      (and-let*
        ((hi (update-lord-list (list update-polyline) lord-list)))
        (list (car hi) '() '())))))
;;======================================================================
;;====== MOLE-ELECTRO-CABEL ============================================
(let*
  ((class-list (list MOLE-ELECTRO-CABEL-10
                     MOLE-ELECTRO-CABEL-6
                     MOLE-ELECTRO-CABEL-0.4))
   (host-list (list MOLE-ELECTRO-TS
                    MOLE-ELECTRO-SUPPORT
                    MOLE-ELECTRO-INPUT
                    MOLE-ELECTRO-MUFTA
                    MOLE-ELECTRO-WELL))
   (type-index
     (lambda ()
       (type->index class-list)))
   (hint
     (lambda ()
       (vector-ref '#("Кабель 10 кВ"
                      "Кабель 6 кВ"
                      "Кабель 0.4 кВ")
                   (type-index))))
   (font-size
     (lambda ()
       (vector-ref '#(210 210 210) (type-index))))
   (font
     (lambda ()
       (vector 0 (- (font-size)))))
   (line-color
     (lambda ()
       (vector-ref '#(52 51 50) (type-index))))
   (line-width
     (lambda ()
       (vector-ref '#(80 70 50) (type-index))))
   (test (lambda (pl start?)
     (define (test-point begin?)
       (let*
         ((pl (if begin? pl (last-pair pl)))
          (vm (map
                (lambda (well)
                  (set-car! pl (cons (assq-ref well 'x0) (assq-ref well 'y0)))
                  well)
                (get-all-objects (car pl) host-list))))
          (length
            (list-select vm
              (lambda (well)
                (equal? (car pl)
                        (cons (assq-ref well 'x0)
                              (assq-ref well 'y0))))))))
     (let*
       ((vns (test-point #t))
        (vnf (test-point #f)))
       (cond
         ((= vns 0) "Кабель должен начинаться в объекте электросети")
         ((= vnf 0) "Кабель должен заканчиваться в объекте электросети")
         ((> vns 1) "Неоднозначное начало кабеля!")
         ((> vnf 1) "Неоднозначный конец кабеля!")
         ((and (equal? (car pl) (car (last-pair pl))) (or (not start?) edition?)) "Кабель не должен заканчиваться в том же объекте электросети")
         ((not edition?)
           (set-property! 'length (poly-length->str pl)))))))

   (length-text-delta
     (lambda ()
       (if (ask-chanals)
         (* 3/10 (font-size) (- (string-length (ask-chanals))
                                (string-length (get-property 'length))))
         0)))
   (make-oy1 (lambda ()
     (+ (font-size) (* 1/2 (line-width)))))
   (make-oy2 (lambda ()
     (- (line-width))))
   (vrtxt?
     (lambda (fst?)
       (and-let*
         ((ta1 (get-property 'ta1))
          (ta2 (get-property 'ta2))
          ((< (abs (- ta1 ta2)) 50))
          (tx1 (get-property 'tx1))
          (ty1 (get-property 'ty1))
          (tx2 (get-property 'tx2))
          (ty2 (get-property 'ty2))
          (dx (* -3/10 (font-size)))
          (x11 (calc-x tx1 ty1 dx (- (make-oy1) (font-size)) ta1))
          (y11 (calc-y tx1 ty1 dx (- (make-oy1) (font-size)) ta1))
          (x21 (calc-x tx2 ty2 dx (- (make-oy2) (font-size)) ta2))
          (y21 (calc-y tx2 ty2 dx (- (make-oy2) (font-size)) ta2))
          (x12 (calc-x tx1 ty1 dx (make-oy1) ta1))
          (y12 (calc-y tx1 ty1 dx (make-oy1) ta1))
          (x22 (calc-x tx2 ty2 dx (make-oy2) ta2))
          (y22 (calc-y tx2 ty2 dx (make-oy2) ta2))
          (p10 (cons x12 y12))
          (p11 (cons x11 y11))
          (p20 (cons x22 y22))
          (p21 (cons x21 y21)))
         (< (if fst? (dist p11 p20) (dist p10 p21)) (* (font-size) 1/2)))))
   (make-footnote (lambda (fst?)
     (and-let*
       ((pl (get-property 'apl))
        (tx (get-property (if fst? 'tx1 'tx2)))
        (ty (get-property (if fst? 'ty1 'ty2)))
        (ta (get-property (if fst? 'ta1 'ta2)))
        (dy ((if fst? make-oy1 make-oy2)))
        (dx (* -3/10 (font-size))))
       (if (or is-convertion? (vrtxt? fst?))
         '()
         (make-pll-footnote
           pl
           (calc-x tx ty dx dy ta)
           (calc-y tx ty dx dy ta)
           ta
           (if fst? (get-property 'length) (ask-chanals))
           (font-size) (* 3/2 (font-size)))))))

   (make-footnote1 (lambda ()
     (make-footnote #t)))

   (make-footnote2 (lambda ()
     (make-footnote #f)))

   (make-ox1
     (lambda ()
       (or
         (and-let*
           ((s 0)
            (x1 (get-property 'tx1))
            (y1 (get-property 'ty1))
            (ang (get-property 'ta1))
            (str (get-property 'length))
            (stl (* (string-length str) (font-size) 55/100))
            (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
            (x2 (car p2))
            (y2 (cdr p2))
            (xm (/ (+ x1 x2) 2))
            (ym (/ (+ y1 y2) 2))
            (apl (get-property 'apl))
            (fn (make-footnote1)))
           (if (or is-convertion? (null? fn))
             (let loop ((d #f) (l 0) (a ang) (pl apl))
               (if (null? pl)
                 (begin
                   (set-property! 'ta1 a)
                   (when is-convertion?
                     (set! a (deg->rad a))
                     (set-property! 'tx1 (+ x1 (* l (sin a))))
                     (set-property! 'ty1 (+ y1 (* l (cos a)))))
                   s)
                 (or (and-let*
                       ((pl)
                        (> (length pl) 1)
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
           s))
         0)))

   (make-ox2
     (lambda ()
       (or
         (and-let*
           ((s 0)
            (x1 (get-property 'tx2))
            (y1 (get-property 'ty2))
            (ang (get-property 'ta2))
            (str (ask-chanals))
            (stl (* (string-length str) (font-size) 55/100))
            (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
            (x2 (car p2))
            (y2 (cdr p2))
            (xm (/ (+ x1 x2) 2))
            (ym (/ (+ y1 y2) 2))
            (fn (make-footnote2)))
           (if (or is-convertion? (null? fn))
             (let loop ((d #f) (l 0) (a ang) (pl (get-property 'apl)))
               (if (null? pl)
                 (begin
                   (set-property! 'ta2 a)
                   (when is-convertion?
                     (set! a (deg->rad a))
                     (set-property! 'tx2 (+ x1 (* l (sin a))))
                     (set-property! 'ty2 (+ y1 (* l (cos a)))))
                   s)
                 (or (and-let*
                       ((pl)
                        (> (length pl) 1)
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
           s))
         0)))

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>
           #:valid-test test
           #:color line-color
           #:width line-width
           #:coord-list 'apl
           #:hint hint)
         (make <Text>
           #:valid-test valid-num?
           #:label 't1
           #:color 0
           #:x-coord 'tx1
           #:y-coord 'ty1
           #:y-offset make-oy1
           #:x-offset make-ox1
           #:angle   'ta1
           #:hint "Длина кабеля"
           #:font font
           #:text 'length)
         (make <Text>
           #:label 't2
           #:color 0
           #:x-coord 'tx2
           #:y-coord 'ty2
           #:y-offset make-oy2
           #:x-offset make-ox2
           #:angle   'ta2
           #:hint "Параметры кабеля"
           #:font font
           #:text (cons 'chanals ask-chanals))
         (make <Color-Footnote>
           #:tool 't1
           #:dynamic #t
           #:dependence '(apl tx1 ty1 ta1 length)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list (cons 'vpl1 make-footnote1))
         (make <Color-Footnote>
           #:tool 't2
           #:dynamic #t
           #:dependence '(apl tx2 ty2 ta2)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list (cons 'vpl2 make-footnote2))
           ))))

  (add-object class-list  pr0)

  (add-context
    host-list
    class-list
    insert-line-object
    coedit-line-object)

  (add-lord->mole-proc
    class-list
    (lambda (lord-list)
      (and-let*
        ((h (update-lord-list (list update-polyline update-text update-text) lord-list))  )
        (append h (list '() '()))))))
;;======================================================================
;;====== MOLE-ELECTRO-AIR-LINE =========================================
(let*
  ((h  70)
   (f 500)

   (class-list (list MOLE-ELECTRO-AIR-LINE-35
                     MOLE-ELECTRO-AIR-LINE-10
                     MOLE-ELECTRO-AIR-LINE-6
                     MOLE-ELECTRO-AIR-LINE-0.4))

   (host-list (list MOLE-ELECTRO-SUPPORT
                    MOLE-ELECTRO-TS
                    MOLE-ELECTRO-INPUT
                    MOLE-ELECTRO-WELL))

   (type-index
     (lambda ()
       (type->index class-list)))
   (hint
     (lambda ()
       (vector-ref '#("Участок воздушной линии 35 кВ"
                      "Участок воздушной линии 10 кВ"
                      "Участок воздушной линии 6 кВ"
                      "Участок воздушной линии 0.4 кВ")
                   (type-index))))
   (font-size
     (lambda ()
       (vector-ref '#(210 210 210 210) (type-index))))
   (font
     (lambda ()
       (vector 0 (- (font-size)))))
   (line-color
     (lambda ()
       (vector-ref '#(49 48 47 46) (type-index))))
   (line-width
     (lambda ()
       (vector-ref '#(50 50 50 50) (type-index))))
   (test (lambda (pl start?)
     (define (test-point begin?)
       (let*
         ((pl (if begin? pl (last-pair pl)))
          (vm (map
                (lambda (well)
                  (set-car! pl (cons (assq-ref well 'x0) (assq-ref well 'y0)))
                  well)
                (get-all-objects (car pl) host-list))))
          (length
            (list-select vm
              (lambda (well)
                (equal? (car pl)
                        (cons (assq-ref well 'x0)
                              (assq-ref well 'y0))))))))
     (let*
       ((vns (test-point #t))
        (vnf (test-point #f)))
       (cond
         ((= vns 0) "Участок воздушной линии должен начинаться в объекте электросети")
         ((= vnf 0) "Участок воздушной линии должен заканчиваться в объекте электросети")
         ((> vns 1) "Неоднозначное начало участка воздушной линии!")
         ((> vnf 1) "Неоднозначный конец участка воздушной линии!")
         ((and (equal? (car pl) (car (last-pair pl))) (or (not start?) edition?)) "Участок воздушной линии не должен заканчиваться в том же объекте электросети")
         ((not edition?)
           (set-property! 'length (poly-length->str pl)))))))

   (length-text-delta
     (lambda ()
       (if (ask-chanals)
         (* 3/10 (font-size) (- (string-length (ask-chanals))
                                (string-length (get-property 'length))))
         0)))
   (make-oy1 (lambda ()
     (+ (font-size) (* 1/2 (line-width)))))
   (make-oy2 (lambda ()
     (- (line-width))))
   (vrtxt?
     (lambda (fst?)
       (and-let*
         ((ta1 (get-property 'ta1))
          (ta2 (get-property 'ta2))
          ((< (abs (- ta1 ta2)) 50))
          (tx1 (get-property 'tx1))
          (ty1 (get-property 'ty1))
          (tx2 (get-property 'tx2))
          (ty2 (get-property 'ty2))
          (dx (* -3/10 (font-size)))
          (x11 (calc-x tx1 ty1 dx (- (make-oy1) (font-size)) ta1))
          (y11 (calc-y tx1 ty1 dx (- (make-oy1) (font-size)) ta1))
          (x21 (calc-x tx2 ty2 dx (- (make-oy2) (font-size)) ta2))
          (y21 (calc-y tx2 ty2 dx (- (make-oy2) (font-size)) ta2))
          (x12 (calc-x tx1 ty1 dx (make-oy1) ta1))
          (y12 (calc-y tx1 ty1 dx (make-oy1) ta1))
          (x22 (calc-x tx2 ty2 dx (make-oy2) ta2))
          (y22 (calc-y tx2 ty2 dx (make-oy2) ta2))
          (p10 (cons x12 y12))
          (p11 (cons x11 y11))
          (p20 (cons x22 y22))
          (p21 (cons x21 y21)))
         (< (if fst? (dist p11 p20) (dist p10 p21)) (* (font-size) 1/2)))))
   (make-footnote (lambda (fst?)
     (and-let*
       ((pl (get-property 'apl))
        (tx (get-property (if fst? 'tx1 'tx2)))
        (ty (get-property (if fst? 'ty1 'ty2)))
        (ta (get-property (if fst? 'ta1 'ta2)))
        (dy ((if fst? make-oy1 make-oy2)))
        (dx (* -3/10 (font-size))))
       (if (or is-convertion? (vrtxt? fst?))
         '()
         (make-pll-footnote
           pl
           (calc-x tx ty dx dy ta)
           (calc-y tx ty dx dy ta)
           ta
           (if fst? (get-property 'length) (ask-chanals))
           (font-size) (* 3/2 (font-size)))))))

   (make-footnote1 (lambda ()
     (make-footnote #t)))

   (make-footnote2 (lambda ()
     (make-footnote #f)))

   (make-ox1
     (lambda ()
       (or
         (and-let*
           ((s 0)
            (x1 (get-property 'tx1))
            (y1 (get-property 'ty1))
            (ang (get-property 'ta1))
            (str (get-property 'length))
            (stl (* (string-length str) (font-size) 55/100))
            (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
            (x2 (car p2))
            (y2 (cdr p2))
            (xm (/ (+ x1 x2) 2))
            (ym (/ (+ y1 y2) 2))
            (apl (get-property 'apl))
            (fn (make-footnote1)))
           (if (null? fn)
             (let loop ((d #f) (l 0) (a ang) (pl apl))
               (if (null? pl)
                 (begin
                   (set-property! 'ta1 a)
                   (when is-convertion?
                     (set! a (deg->rad a))
                     (set-property! 'tx1 (+ x1 (* l (sin a))))
                     (set-property! 'ty1 (+ y1 (* l (cos a)))))
                   s)
                 (or (and-let*
                       ((pl)
                        (> (length pl) 1)
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
           s))
         0)))

   (make-ox2
     (lambda ()
       (or
         (and-let*
           ((s 0)
            (x1 (get-property 'tx2))
            (y1 (get-property 'ty2))
            (ang (get-property 'ta2))
            (str (ask-chanals))
            (stl (* (string-length str) (font-size) 55/100))
            (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
            (x2 (car p2))
            (y2 (cdr p2))
            (xm (/ (+ x1 x2) 2))
            (ym (/ (+ y1 y2) 2))
            (fn (make-footnote2)))
           (if (null? fn)
             (let loop ((d #f) (l 0) (a ang) (pl (get-property 'apl)))
               (if (null? pl)
                 (begin
                   (set-property! 'ta2 a)
                   (when is-convertion?
                     (set! a (deg->rad a))
                     (set-property! 'tx2 (+ x1 (* l (sin a))))
                     (set-property! 'ty2 (+ y1 (* l (cos a)))))
                   s)
                 (or (and-let*
                       ((pl)
                        (> (length pl) 1)
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
           s))
         0)))


   (make-arrow (lambda (lft? h)
     (define cl (list (cons 0 0)
                      (cons (* -2 h ) (- h))
                      (cons (- h ) 0)
                      (cons (* -2 h ) h)))
     (or
       (and-let*
         ((pl (get-property 'apl))
          ((= (length pl) 2))
          (p0 (if lft? (car pl) (cadr pl)))
          (p1 (if lft? (cadr pl) (car pl)))
          (dr (offset p0 p1))
          (a (calc-a p0 p1))
          (d (dist p0 p1))
          ((>= d (* f 4)))
          (dd (cons (/ (* f (car dr)) d) (/ (* f (cdr dr)) d)))
          (t0 (shift p0 dd)))
         (map (lambda (p) (shift t0 (turn p a))) cl))
       '())))

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>
           #:valid-test test
           #:color line-color
           #:width line-width
           #:coord-num 2
           #:coord-list 'apl
           #:hint hint)
         (make <Text>
           #:valid-test valid-num?
           #:label 't1
           #:color 0
           #:x-coord 'tx1
           #:y-coord 'ty1
           #:y-offset make-oy1
           #:x-offset make-ox1
           #:angle   'ta1
           #:hint "Длина линии"
           #:font font
           #:text 'length)
         (make <Text>
           #:label 't2
           #:color 0
           #:x-coord 'tx2
           #:y-coord 'ty2
           #:y-offset make-oy2
           #:x-offset make-ox2
           #:angle   'ta2
           #:hint "Параметры линии"
           #:font font
           #:text (cons 'chanals ask-chanals))
         (make <Filled-Polygon>
           #:tool 0
           #:dynamic #t
           #:foreground line-color
           #:coord-list (lambda () (make-arrow #t (* (line-width) 2))))
         (make <Filled-Polygon>
           #:tool 0
           #:dynamic #t
           #:foreground line-color
           #:coord-list (lambda () (make-arrow #f (* (line-width) 2))))
         (make <Color-Footnote>
           #:tool 't1
           #:dynamic #t
           #:dependence '(apl tx1 ty1 ta1 length)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list (cons 'vpl1 make-footnote1))
         (make <Color-Footnote>
           #:tool 't2
           #:dynamic #t
           #:dependence '(apl tx2 ty2 ta2)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list (cons 'vpl2 make-footnote2))
           ))))

  (add-object class-list pr0 )

  (add-context
    host-list
    class-list
    insert-line-object
    coedit-line-object)

  (add-lord->mole-proc
    class-list
    (lambda (lord-list)
      (and-let*
        ((hi (update-lord-list (list update-polyline update-text update-text) lord-list)))
        (append hi (list '() '() '() '()))))))
;;======================================================================
;;====== MOLE-ELECTRO-MUFTA ============================================
(let*
  ((r 60)
   (c 59)

   (cabels (list MOLE-ELECTRO-CABEL-10
                 MOLE-ELECTRO-CABEL-6
                 MOLE-ELECTRO-CABEL-0.4))

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Circle>
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r
           #:foreground c
           #:hint "Муфта")))))

  (add-object MOLE-ELECTRO-MUFTA pr0)

  (add-lord->mole-proc
    MOLE-ELECTRO-MUFTA
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-point update-text) lord-list)
                 (update-lord-list (list update-point update-text update-subline) lord-list))))
        (list '() (car hi) (cadr hi)))))

  (add-context
    cabels
    MOLE-ELECTRO-MUFTA
    insert-point-object
    copy-target)
)
;;======================================================================

;;======================================================================
(add-object MOLE-ELECTRO-EXACT-ATTACHMENT (get-obj-prototype MOLE-EXACT-ATTACHMENT))
(add-object MOLE-ELECTRO-INEXACT-ATTACHMENT (get-obj-prototype MOLE-INEXACT-ATTACHMENT))
(add-object MOLE-ELECTRO-ROUND-ATTACHMENT (get-obj-prototype MOLE-ROUND-ATTACHMENT))
;;======================================================================


