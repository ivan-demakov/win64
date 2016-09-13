;;== Котельная ЦТП =====================================================
(let*
  ((class-list (list MOLE-TEPLO-KOTEL
                     MOLE-TEPLO-CENTRAL-PUNCT))
   (r 250)
   (w 90)
   (c0 29)
   (c1 43)
   (font-size 350)
   (type-index (lambda () (type->index class-list)))
   (full-number (lambda ()
     (string-append (vector-ref '#("К" "ЦТП") (type-index))
                    "-" (get-number))))
   (hint (lambda ()
      (vector-ref '#("Котельная"
                     "ЦТП") (type-index))))
   (thint (lambda ()
      (vector-ref '#("Номер котельной"
                     "Номер ЦТП") (type-index))))
   (color (lambda ()
     (vector-ref (vector c0 c1) (type-index))))

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
                   (get-tx) (get-ty) (get-ta)
                   (full-number) font-size #f))))
         (make <Outlined-Circle>
           #:h-visible #t
           #:l-visible #t
           #:color color
           #:width w
           #:foreground 1
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size r
           #:hint hint)
         (make <Text>
            #:label 't0
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:a-grid  '(150 . 0)
            #:length  10
            #:hint thint
            #:text 'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object class-list pr0)

  (add-lord->mole-proc
    class-list
    (lambda (lord-list)
      (and-let*
        ((kt (or (update-lord-list (list update-point update-pref-text) lord-list)
                 (update-lord-list (list update-point update-pref-text update-subline) lord-list))))
        (list '() (car kt) (cadr kt) '())))))
;;======================================================================
;;== Ввод в дом ========================================================
;;== Изменение диаметра трубопровода ===================================
;;== Ответвление внутриквартального трубопровода =======================
(let*
  ((r0 140)
   (c0 40)
   (r1 160)
   (c1 44)
   (w  50)
   (r2 120)
   (c2 1)
   (pr0 (lambda (t c r h)
     (make <Container>
       #:contents
       (list
         (make t
           #:h-visible #t
           #:l-visible #t
           #:foreground c
           #:color c1
           #:width w
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r
           #:hint h))))))
  (add-object MOLE-TEPLO-HOUSE-INPUT      (pr0 <Circle> c0 r0 "Ввод в дом"))
  (add-object MOLE-TEPLO-PIPE-SIZE-CHANGE (pr0 <Circle> c1 r1 "Изменение диаметра трубопровода"))
  (add-object MOLE-TEPLO-QUAR-PIPE-BRANCH (pr0 <Outlined-Circle> c2 r2 "Ответвление внутриквартального трубопровода"))

  (add-lord->mole-proc
    (list MOLE-TEPLO-HOUSE-INPUT MOLE-TEPLO-PIPE-SIZE-CHANGE MOLE-TEPLO-QUAR-PIPE-BRANCH)
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-point) lord-list)
                 (update-lord-list (list update-point update-point) lord-list))))
        (list (car hi))))))
;;======================================================================
;;== Камера магистрального трубопровода ================================
(let*
  ((w0 200)
   (h0 200)
   (w1 100)
   (h1 100)
   (c0 44)
   (c1 1)
   (font-size 280)
   (full-number (lambda () (string-append "ТКм-" (get-number))))
   (calc-x (lambda (x w h a) (+ x (car (turn (cons (- w) (- h)) a)))))
   (calc-y (lambda (y w h a) (+ y (cdr (turn (cons (- w) (- h)) a)))))
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
                   (get-tx) (get-ty) (get-ta)
                   (full-number) font-size #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle 'a
           #:hint "Камера магистрального трубопровода")
         (make <Bar>
           #:h-visible #t
           #:l-visible #t
           #:foreground c0
           #:dependence '(x0 y0 a)
           #:x-coord (lambda () (calc-x (get-x0) w0 h0 (get-a)))
           #:y-coord (lambda () (calc-y (get-y0) w0 h0 (get-a)))
           #:angle get-a
           #:x-size  (* w0 2)
           #:y-size  (* h0 2))
         (make <Bar>
           #:h-visible #t
           #:l-visible #t
           #:foreground c1
           #:dependence '(x0 y0 a)
           #:x-coord (lambda () (calc-x (get-x0) w1 h1 (get-a)))
           #:y-coord (lambda () (calc-y (get-y0) w1 h1 (get-a)))
           #:angle get-a
           #:x-size  (* w1 2)
           #:y-size  (* h1 2))
         (make <Text>
            #:label 't0
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:a-grid  '(150 . 0)
            #:length  10
            #:hint "Номер камеры"
            #:text 'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object MOLE-TEPLO-MAG-PIPE-CAM pr0)

  (add-lord->mole-proc
    MOLE-TEPLO-MAG-PIPE-CAM
    (lambda (lord-list)
      (and-let*
        ((tc (or (update-lord-list (list update-rect update-pref-text) lord-list)
                 (update-lord-list (list update-rect update-pref-text update-subline) lord-list))))
        (list '() (car tc) '() '() (cadr tc) '())))))
;;======================================================================
;;== Камера квартального трубопровода ==================================
(let*
  ((w0 150)
   (h0 150)
   (c0 44)
   (font-size 280)
   (full-number (lambda () (string-append "ТК-" (get-number))))
   (calc-x (lambda (x w h a) (+ x (car (turn (cons (- w) (- h)) a)))))
   (calc-y (lambda (y w h a) (+ y (cdr (turn (cons (- w) (- h)) a)))))
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
                   (get-tx) (get-ty) (get-ta)
                   (full-number) font-size #f))))
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle 'a
           #:hint "Камера квартального трубопровода")
         (make <Bar>
           #:h-visible #t
           #:l-visible #t
           #:foreground c0
           #:dependence '(x0 y0 a)
           #:x-coord (lambda () (calc-x (get-x0) w0 h0 (get-a)))
           #:y-coord (lambda () (calc-y (get-y0) w0 h0 (get-a)))
           #:angle get-a
           #:x-size  (* w0 2)
           #:y-size  (* h0 2))
         (make <Text>
            #:label 't0
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:a-grid  '(150 . 0)
            #:length  10
            #:hint "Номер камеры"
            #:text 'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object MOLE-TEPLO-QUAR-PIPE-CAM pr0)

  (add-lord->mole-proc
    MOLE-TEPLO-QUAR-PIPE-CAM
    (lambda (lord-list)
      (let*
        ((tk (or (update-lord-list (list update-rect update-pref-text) lord-list)
                 (update-lord-list (list update-rect update-pref-text update-subline) lord-list))))
        (list '() (car tk) '() (cadr tk) '())))))
;;======================================================================
;;== Неподвижная опора Компенсатор =====================================
(let*
  ((l0 '((   0 .  200) (0 .   -200)
         (-110 .  150) (110 .  250)
         (-110 .  250) (110 .  150)
         (-110 . -150) (110 . -250)
         (-110 . -250) (110 . -150)))
   (l1 '((-350 . -200) (-150 . -200)
         (-150 . -200) (-150 . -350)
         (-150 . -350) (150 . -350)
         (150 . -350) (150 . -200)
         (150 . -200) (350 . -200)))
   (clc (lambda (cl)
     (let
       ((p0 (cons (get-x0) (get-y0)))
        (a (get-a)))
       (map
         (lambda (p) (shift p0 (turn p a)))
         cl))))
   (pr0 (lambda (h cl)
     (make <Container>
       #:contents
       (list
         (make <Color-Rectangle>
           #:h-visible #f
           #:l-visible #f
           #:x-coord 'x0
           #:y-coord 'y0
           #:angle 'a
           #:hint h)
         (make <Color-Lines>
           #:color 44
           #:dependence '(x0 y0 a)
           #:width 50
           #:coord-list (cons 'apl (lambda () (clc cl)))))))))

  (add-object MOLE-TEPLO-FIXED-SUPPORT (pr0 "Неподвижная опора" l0))
  (add-object MOLE-TEPLO-COMPENSATOR   (pr0 "Компенсатор" l1))

  (add-lord->mole-proc
    (list MOLE-TEPLO-FIXED-SUPPORT MOLE-TEPLO-COMPENSATOR)
    (lambda (lord-list)
      (and-let*
        ((tp (update-lord-list (list update-rect) lord-list)))
        (list (car tp) '())))))
;;======================================================================
;;== Сеть магистральная Сеть внутриквартальная =========================
;;== Нерабочий участок сети ============================================
(let*
  ((class-list (list MOLE-TEPLO-MAGISTRAL-NET
                     MOLE-TEPLO-QUARTAL-NET-2
                     MOLE-TEPLO-QUARTAL-NET-3
                     MOLE-TEPLO-QUARTAL-NET-4
                     MOLE-TEPLO-OUT-OF-WORK-NET))
   (t-list (list MOLE-TEPLO-KOTEL
                 MOLE-TEPLO-CENTRAL-PUNCT
                 MOLE-TEPLO-CAMS
                 MOLE-TEPLO-HOUSE-INPUT
                 MOLE-TEPLO-QUAR-PIPE-BRANCH
                 MOLE-TEPLO-PIPE-SIZE-CHANGE))
   (type-index
     (lambda ()
       (type->index class-list)))
   (hint
     (lambda ()
       (vector-ref '#("Сеть магистральная"
                      "Сеть внутриквартальная (2)"
                      "Сеть внутриквартальная (3)"
                      "Сеть внутриквартальная (4)"
                      "Нерабочий участок сети")
                   (type-index))))
   (font-size
     (lambda ()
       (vector-ref '#(300 260 260 260 260) (type-index))))
   (font
     (lambda ()
       (vector 0 (- (font-size)))))
   (line-color
     (lambda ()
       (vector-ref '#(41 42 42 42 42) (type-index))))
   (line-style
     (lambda ()
       (vector-ref '#(25 26 27 28 #:dash) (type-index))))
   (line-width
     (lambda ()
       (vector-ref '#(170 40 60 90 0) (type-index))))
   (test (lambda (pl start?)
     (define (test-point begin?)
       (let*
         ((pl (if begin? pl (last-pair pl)))
          (vm (map
                (lambda (well)
                  (set-car! pl (cons (assq-ref well 'x0) (assq-ref well 'y0)))
                  well)
                (get-all-objects (car pl) t-list))))
          (length
            (list-select vm
              (lambda (well)
                (equal? (car pl)
                        (cons (assq-ref well 'x0)
                              (assq-ref well 'y0))))))))
     (and-let*
       (((not start?))
        ((not edition?))
        (ln (length pl))
        ((>= ln 2))
        (pl (list-tail pl (- ln 2))))
       (set-property! 'ta (good-angle (calc-a (car pl) (cadr pl)))))

     (let*
       ((vns (test-point #t))
        (vnf (test-point #f)))
       (cond
         ((= vns 0) "Участок теплосети должен начинаться в объекте теплосети")
         ((= vnf 0)
           (cond
             (edition? "Участок теплосети должен заканчиваться в объекте теплосети!")
             ((eq? (tk-message-box
                      "Участок теплосети должен заканчиваться в объекте теплосети!\n\nПостроить здесь?"
                      "Внимание!" 'yes-no) 'yes)
              (set! &suspend-object? #t))
             (else #f)))
         ((> vns 1) "Неоднозначное начало участка теплосети!")
         ((> vnf 1) "Неоднозначный конец участка теплосети!")
         ((and (equal? (car pl) (car (last-pair pl))) (or (not start?) edition?)) "Участок теплосети не должен заканчиваться в том же объекте теплосети")
         ((not edition?)
           (set-property! 'length (poly-length->str pl)))))))

   (s-visible? (lambda ()
     (= (type-index) 4)))

   (full-diam (lambda () (string-append "d" (get-property 'chanals))))

   (make-footnote (lambda ()
     (let
       ((tx (get-tx))
        (ty (get-ty))
        (ta (get-ta))
        (dm (get-property 'chanals))
        (ln (get-property 'length))
        (dx (* -3/10 (font-size)))
        (dy (- (line-width))))
       (make-pll-footnote
         (get-property 'apl)
         (calc-x tx ty dx dy ta)
         (calc-y tx ty dx dy ta)
         ta
         (if (and dm (> (string-length dm) (string-length ln))) dm ln)
         (font-size) (* 2 (line-width))))))

   (make-ox
     (lambda ()
       (let*
         ((s 0)
          (x1 (get-tx))
          (y1 (get-ty))
          (ang (get-ta))
          (ln (get-property 'length))
          (dm (get-property 'chanals))
          (str (if (and dm (> (string-length dm) (string-length ln))) dm ln))
          (stl (* (string-length str) (font-size) 55/100))
          (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
          (x2 (car p2))
          (y2 (cdr p2))
          (xm (/ (+ x1 x2) 2))
          (ym (/ (+ y1 y2) 2)))
         (if (null? (make-footnote))
           (let loop ((d #f) (l 0) (a ang) (pl (get-property 'apl)))
             (if (null? pl)
               (begin
                 (set-property! 'ta a)
                 (when is-convertion?
                   (set! a (deg->rad a))
                   (set-property! 'tx1 (+ x1 (* l (sin a))))
                   (set-property! 'ty1 (+ y1 (* l (cos a)))))
                 s)
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

   (pr0
     (make <Container>
       #:contents
       (list
         (make <Color-Polyline>
           #:valid-test test
           #:line  line-style
           #:color line-color
           #:width line-width
           #:hint  hint
           #:coord-list 'apl)
         (make <Text>
           #:s-visible s-visible?
           #:label 't0
           #:dependence '(apl)
           #:color 0
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:x-offset make-ox
           #:y-offset (cons 'oy1 font-size)
           #:hint "Длина"
           #:font font
           #:valid-test valid-num?
           #:text 'length)
         (make <Text>
           #:h-visible #f
           #:l-visible #f
           #:hint "Диаметр"
           #:valid-test valid-num?
           #:text 'chanals)
         (make <Text>
           #:s-visible s-visible?
           #:dependence '(apl length tx chanals)
           #:color 0
           #:x-coord get-tx
           #:y-coord get-ty
           #:y-offset (cons 'oy2 (lambda () (- (line-width))))
           #:angle   get-ta
           #:font font
           #:text (cons 'full-diam full-diam))
         (make <Color-Footnote>
           #:s-visible s-visible?
           #:tool 't0
           #:dynamic #t
           #:dependence '(apl tx ty ta)
           #:line  #:solid
           #:color 0
           #:width 0
           #:coord-list (cons 'vpl make-footnote))
                   ))))

  (add-object class-list pr0)

  (add-lord->mole-proc
    MOLE-TEPLO-MAGISTRAL-NET
    (lambda (lord-list)
      (and-let*
        ((ps (or (update-lord-list (list update-polyline update-pref-text update-pref-text) lord-list)
                 (update-lord-list (list update-polyline update-pref-text update-pref-text update-subline) lord-list)))
         (li (caddr ps))
         (cv (assv-ref li #:coord))
         (sv (assv-ref li #:string))
         (x (vector-ref cv 0))
         (y (vector-ref cv 1))
         (a (vector-ref sv 2))
         (p (shift (cons x y) (turn (cons 0 -380) a))))
        (vector-set! cv 0 (car p))
        (vector-set! cv 1 (cdr p))
        (list (car ps) li (cadr ps) '() ()))))

  (add-lord->mole-proc
    (list MOLE-TEPLO-QUARTAL-NET-2 MOLE-TEPLO-QUARTAL-NET-3  MOLE-TEPLO-QUARTAL-NET-4)
    (lambda (lord-list)
      (and-let*
        ((ps (or (update-lord-list (list update-polyline update-pref-text update-pref-text) lord-list)
                 (update-lord-list (list update-polyline update-pref-text update-pref-text update-subline) lord-list)))
         (li (caddr ps))
         (cv (assv-ref li #:coord))
         (sv (assv-ref li #:string))
         (x (vector-ref cv 0))
         (y (vector-ref cv 1))
         (a (vector-ref sv 2))
         (p (shift (cons x y) (turn (cons 0 -400) a))))
        (vector-set! cv 0 (car p))
        (vector-set! cv 1 (cdr p))
        (list (car ps) li (cadr ps) '() ()))))

  (add-lord->mole-proc
    MOLE-TEPLO-OUT-OF-WORK-NET
    (lambda (lord-list)
      (update-lord-list (list update-polyline) lord-list)))

  (add-context
    t-list
    MOLE-TEPLO-NET
    insert-line-object
    coedit-line-object)

  (add-context
    MOLE-TEPLO-NET
    (list MOLE-TEPLO-QUAR-PIPE-BRANCH MOLE-TEPLO-PIPE-SIZE-CHANGE)
    insert-point-object
    copy-target)

 (add-context
   MOLE-TEPLO-NET
   (list MOLE-TEPLO-FIXED-SUPPORT MOLE-TEPLO-COMPENSATOR)
   (lambda (x y)
     (and-let*
       ((spl (get-property 'apl))
        (n (select-segment x y spl #f))
        (pl (list-tail spl n))
        (a (calc-a (car pl) (cadr pl)))
        (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
        (p2 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a)))
       (list (cons 'x0 (car p2))
             (cons 'y0 (cdr p2))
             (cons 'a a))))
   (lambda (host-old host-new target)
     (let*
       ((spl (or (get-property-from host-new 'apl) '()))
        (x (get-property-from target 'x0))
        (y (get-property-from target 'y0))
        (n (select-segment x y spl #f)))
       (if (not n)
         target
         (let*
           ((pl (list-tail spl n))
            (a (calc-a (car pl) (cadr pl)))
            (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
            (p2 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a)))
           (assv-set! target 'x0 (car p2))
           (assv-set! target 'y0 (cdr p2))
           (assv-set! target 'a a)))))))
;;======================================================================
(add-object MOLE-TEPLO-COMMENTARY (get-obj-prototype MOLE-COMMENTARY))
;;======================================================================

