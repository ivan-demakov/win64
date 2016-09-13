;;======================================================================
;; Объекты водосетей
;;======================================================================
;;== Скважина ==========================================================
(let*
  ((r0 100)
   (r1 280)
   (w1 180)
   (c0 1)
   (c1 31)
   (font-size 500)
   (full-number (lambda () (string-append "СКВ-" (get-number))))
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
         (make <Circle>
           #:h-visible #t
           #:l-visible #t
           #:foreground c0
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r1
           #:hint    "Скважина")
         (make <Circle>
           #:h-visible #t
           #:l-visible #t
           #:dependence '(x0 y0)
           #:foreground c1
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size  r0)
         (make <Color-Ring>
           #:h-visible #t
           #:l-visible #t
           #:dependence '(x0 y0)
           #:color c1
           #:width w1
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size  r1)
         (make <Text>
            #:label 't0
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:a-grid  '(150 . 0)
            #:length  10
            #:hint "Номер скважины"
            #:text    'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object MOLE-VODO-HOLE pr0)

  (add-lord->mole-proc
    MOLE-VODO-HOLE
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-point update-pref-text) lord-list)
                 (update-lord-list (list update-point update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() '() (cadr hi) '())))))
;;======================================================================
;;== Резервуар =========================================================
(let*
  ((r0 250)
   (c0 31)
   (c1 0)
   (font-size 600)
   (pr0
     (make <Container>
       #:contents
       (list
         (make <Circle>
           #:h-visible #t
           #:l-visible #t
           #:foreground c0
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r0
           #:hint "Резервуар")
         (make <Text>
            #:color 0
            #:dependence '(x0 y0)
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle 'ta
            #:font (vector 0 (- font-size))
            #:text "Р")))))
  (add-object MOLE-VODO-RESERV pr0)

  (add-lord->mole-proc
    MOLE-VODO-RESERV
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-point update-pref-text) lord-list)
                 (update-lord-list (list update-point update-pref-text update-subline) lord-list))))
        (list (car hi) (cadr hi))))))
;;======================================================================
;;== Насосная станция ==================================================
(let*
  ((w0 360)
   (h0 230)
   (w1 310)
   (h1 180)
   (c0 31)
   (c1 31)
   (font-size 500)
   (full-number (lambda () (string-append "ВНС-" (get-number))))
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
           #:hint    "Насосная станция")
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
            #:hint "Номер станции"
            #:text 'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object MOLE-VODO-PUMP-STATION pr0)

  (add-lord->mole-proc
    MOLE-VODO-PUMP-STATION
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-rect update-pref-text) lord-list)
                 (update-lord-list (list update-rect update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() '() (cadr hi) '())))))
;;======================================================================
;;== Водопроводный колодец =============================================
(let*
  ((r0 85)
   (r1 110)
   (w1 50)
   (c0 1)
   (c1 31)
   (font-size 250)
   (full-number (lambda () (string-append "ВК-" (get-number))))
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
         (make <Circle>
           #:h-visible #t
           #:l-visible #t
           #:foreground c0
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r1
           #:hint    "Водопроводный колодец")
         (make <Color-Ring>
           #:h-visible #t
           #:l-visible #t
           #:dependence '(x0 y0)
           #:color c1
           #:width w1
           #:x-coord get-x0
           #:y-coord get-y0
           #:x-size  r1)
         (make <Text>
            #:label 't0
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:a-grid  '(150 . 0)
            #:length  10
            #:hint "Номер колодца"
            #:text    'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object MOLE-VODO-WELL pr0)

  (add-lord->mole-proc
    MOLE-VODO-WELL
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-point update-pref-text) lord-list)
                 (update-lord-list (list update-point update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() (cadr hi) '())))))
;;======================================================================
;;== Водопроводная камера ==============================================
(let*
  ((w0 150)
   (h0 100)
   (w1 105)
   (h1 50)
   (c0 31)
   (c1 1)
   (font-size 280)
   (full-number (lambda () (string-append "ВК-" (get-number))))
   (calc-x (lambda (x w h a) (+ x (car (turn (cons (- w) (- h)) a)))))
   (calc-y (lambda (y w h a) (+ y (cdr (turn (cons (- w) (- h)) a)))))
   (ask-a (lambda () (if (not (get-a)) (set-property! 'a 0)) (get-a)))
   (pr00
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>)
         (make <Circle> #:x-coord 'x0 #:y-coord 'y0)
         (make <Color-Ring>)
         (make <Text> #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>))))
   (pr00->pr0 (lambda ()
     (set-property! 'a 0)))
   (pr0
     (make <Container>
       #:version (cons pr00 pr00->pr0)
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
           #:angle (cons 'a ask-a)
           #:hint    "Водопроводная камера")
         (make <Bar>
           #:h-visible #t
           #:l-visible #t
           #:foreground c0
           #:dependence '(x0 y0)
           #:x-coord (lambda () (calc-x (get-x0) w0 h0 (ask-a)))
           #:y-coord (lambda () (calc-y (get-y0) w0 h0 (ask-a)))
           #:angle ask-a
           #:x-size  (* w0 2)
           #:y-size  (* h0 2))
         (make <Bar>
           #:h-visible #t
           #:l-visible #t
           #:foreground c1
           #:dependence '(x0 y0)
           #:x-coord (lambda () (calc-x (get-x0) w1 h1 (ask-a)))
           #:y-coord (lambda () (calc-y (get-y0) w1 h1 (ask-a)))
           #:angle ask-a
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
            #:hint "Номер станции"
            #:text 'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object MOLE-VODO-CAMERA pr0)

  (add-lord->mole-proc
    MOLE-VODO-CAMERA
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-rect update-pref-text) lord-list)
                 (update-lord-list (list update-rect update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() '() (cadr hi) '())))))
;;======================================================================
;;== Пожарный гидрант ==================================================
(let*
  ((r0 150)
   (w0 40)
   (c0 1)
   (c1 45)
   (c2 55)
   (font-size 280)
   (full-number (lambda () (string-append "ПГ-" (get-number))))
   (pr0 (lambda (c h)
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
           #:foreground c0
           #:color c
           #:width w0
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r0
           #:y-size  r0
           #:hint h)
         (make <Pie-Slice>
           #:h-visible #t
           #:l-visible #t
           #:dependence '(x0 y0)
           #:foreground c
           #:x-coord get-x0
           #:y-coord get-y0
           #:angle -900
           #:delta 1800
           #:x-size  r0
           #:y-size  r0)
         (make <Text>
            #:label 't0
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:a-grid  '(150 . 0)
            #:length  10
            #:hint "Номер гидранта"
            #:text    'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number)))))))
  (add-object MOLE-VODO-HIDRANT     (pr0 c2 "Пожарный гидрант"))
  (add-object MOLE-VODO-BAD-HIDRANT (pr0 c1 "Неисправный гидрант"))

  (add-lord->mole-proc
    (list MOLE-VODO-HIDRANT MOLE-VODO-BAD-HIDRANT)
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-point update-pref-text) lord-list)
                 (update-lord-list (list update-point update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() (cadr hi) '())))))
;;======================================================================
;;== Домовой ввод, Ответвление Выпуск ==================================
(let*
  ((r0 80)
   (c0 32)
   (r1 60)
   (c1 28)
   (r2 70)
   (c2 33)
   (pr0 (lambda (c r h)
     (make <Container>
       #:contents
       (list
         (make <Circle>
           #:h-visible #t
           #:l-visible #t
           #:foreground c
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r
           #:hint h))))))
  (add-object MOLE-VODO-HOUSE-INPUT (pr0 c0 r0 "Домовой ввод"))
  (add-object MOLE-VODO-BRANCH      (pr0 c1 r1 "Ответвление"))
  (add-object MOLE-VODO-OUTPUT      (pr0 c2 r2 "Канализ. выпуск"))

  (add-lord->mole-proc
    (list MOLE-VODO-HOUSE-INPUT MOLE-VODO-BRANCH MOLE-VODO-OUTPUT)
    (lambda (lord-list)
      (and-let*
        ((vb (or (update-lord-list (list update-point) lord-list)
                 (update-lord-list (list update-point update-text) lord-list))))
        (list (car vb))))))
;;======================================================================
;;== Водовод ===========================================================
(let*
  ((class-list (list MOLE-VODO-BAD-TRACE
                     MOLE-VODO-MAIN-TRACE
                     MOLE-VODO-MAGISTRAL-TRACE
                     MOLE-VODO-STREET-TRACE
                     MOLE-VODO-MAIN-COLLECTOR
                     MOLE-VODO-MAGISTRAL-NET
                     MOLE-VODO-GARDEN-NET
                     MOLE-VODO-DRENAGE-NET))
   (type-index
     (lambda ()
       (type->index class-list)))
   (hint
     (lambda ()
       (vector-ref '#("Нерабочий участок водовода"
                      "Главный водовод"
                      "Магистральный водовод"
                      "Уличный водовод"
                      "Главный коллектор"
                      "Магистральная сеть"
                      "Дворовая сеть"
                      "Дренажная сеть")
                   (type-index))))
   (font-size
     (lambda ()
       (vector-ref
         '#(150 250 210 150 250 210 150 150)
         (type-index))))
   (font
     (lambda ()
       (vector 0 (- (font-size)))))
   (line-color
     (lambda ()
       (vector-ref
         '#(34 34 34 34 35 35 35 36)
         (type-index))))
   (line-style
     (lambda ()
       (vector-ref
         '#(#:dash-dot #:solid #:solid #:solid 21 22 23 24)
         (type-index))))
   (line-width
     (lambda ()
       (vector-ref '#(0 110 60 35 130 80 50 50) (type-index))))
   (test (lambda (pl start?)
     (define (test-point begin?)
       (let*
         ((pl (if begin? pl (last-pair pl)))
          (vm (map
                (lambda (well)
                  (set-car! pl (cons (assq-ref well 'x0) (assq-ref well 'y0)))
                  well)
                (get-all-objects (car pl) (list (if (< (type-index) 4) MOLE-VODOSNAB-OBJ MOLE-VODOTVOD-OBJ))))))
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
       (set-property! 'ta1 (good-angle (calc-a (car pl) (cadr pl))))
       (set-property! 'ta2 (good-angle (calc-a (car pl) (cadr pl)))))
     (let*
       ((vns (test-point #t))
        (vnf (test-point #f))
        (t (if (< (type-index) 4) "водоснабжения" "водоотведения")))
       (cond
         ((= vns 0) (string-append "Участок сети " t " должен начинаться в объекте " t "!"))
         ((= vnf 0)
           (cond
             (edition? (string-append "Участок сети " t " должен заканчиваться в объекте " t "!"))
             ((eq? (tk-message-box
                      (string-append "Участок сети " t " должен заканчиваться в объекте " t "!\n\nПостроить здесь?")
                      "Внимание!" 'yes-no) 'yes)
              (set! &suspend-object? #t))
             (else #f)))
         ((> vns 1) (string-append "Неоднозначное начало участка сети " t "!"))
         ((> vnf 1) (string-append "Неоднозначный конец участка сети " t "!"))
         ((and (equal? (car pl) (car (last-pair pl))) (or (not start?) edition?)) (string-append "Участок сети " t " не должен заканчиваться в том же объекте " t "!"))
         ((not edition?)
           (set-property! 'length (poly-length->str pl)))))))
   (length-text-delta
     (lambda ()
       (if (get-property 'chanals)
         (* 3/10 (font-size) (- (string-length (get-property 'chanals))
                                (string-length (get-property 'length))))
         0)))
   (full-diam (lambda () (string-append "d" (get-property 'chanals))))
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
       (if (vrtxt? fst?)
         '()
         (make-pll-footnote
           pl
           (calc-x tx ty dx dy ta)
           (calc-y tx ty dx dy ta)
           ta
           (if fst? (get-property 'length) (full-diam))
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
            (str (get-property 'chanals))
            (stl (* (string-length str) (font-size) 55/100))
            (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
            (x2 (car p2))
            (y2 (cdr p2))
            (xm (/ (+ x1 x2) 2))
            (ym (/ (+ y1 y2) 2))
            (fn (make-footnote2))
            )
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
           #:line  line-style
           #:color line-color
           #:width line-width
           #:hint  hint
           #:coord-list 'apl)
         (make <Text>
           #:valid-test valid-num?
           #:label 't1
           #:color 0
           #:x-coord 'tx1
           #:y-coord 'ty1
           #:y-offset make-oy1
           #:x-offset make-ox1
           #:angle   'ta1
           #:hint "Длина водовода"
           #:font font
           #:text 'length)
         (make <Text>
           #:valid-test valid-num?
           #:label 't2
           #:h-visible #f
           #:l-visible #f
           #:hint "Диаметр"
           #:x-coord 'tx2
           #:y-coord 'ty2
           #:angle   'ta2
           #:text 'chanals)
         (make <Text>
           #:dependence '(tx2 ty2 ta2 chanals)
           #:color 0
           #:x-offset make-ox2
           #:y-offset make-oy2
           #:x-coord (lambda () (get-property 'tx2))
           #:y-coord (lambda () (get-property 'ty2))
           #:angle   (lambda () (get-property 'ta2))
           #:font font
           #:text full-diam)
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
           #:dependence '(apl tx2 ty2 ta2 chanals)
           #:line  #:solid
           #:color owner->color
           #:width 0
           #:coord-list (cons 'vpl2 make-footnote2))
                   ))))
  (add-object class-list pr0)

  (add-lord->mole-proc
    class-list
    (lambda (lord-list)
      (and-let*
        ((ps (or (update-lord-list (list update-polyline update-pref-text update-pref-text) lord-list)
                 (update-lord-list (list update-polyline update-pref-text update-pref-text update-subline) lord-list)
                 (update-lord-list (list update-polyline update-pref-text update-pref-text update-subline update-subline) lord-list))))
        (list (car ps) (caddr ps) (cadr ps) '() '() '()))))
)
;;======================================================================
;;== Главная насосная станция ==========================================
(let*
  ((w 550)
   (h 400)
   (c 37)
   (font-size 550)
   (full-number (lambda () (string-append "ГНС-" (get-number))))
   (calc-x (lambda (x w h a) (+ x (car (turn (cons (- w) (- h)) a)))))
   (calc-y (lambda (y w h a) (+ y (cdr (turn (cons (- w) (- h)) a)))))
   (pr00
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>)
         (make <Circle> #:x-coord 'x0 #:y-coord 'y0)
         (make <Text>   #:x-coord 'tx #:y-coord 'ty #:angle 'ta #:text 'number)
         (make <Text>))))
   (pr00->pr0 (lambda ()
     (set-property! 'a 0)))
   (pr0
     (make <Container>
       #:version (cons pr00 pr00->pr0)
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
           #:hint "Главная насосная станция")
         (make <Bar>
           #:h-visible #t
           #:l-visible #t
           #:foreground c
           #:dependence '(x0 y0 a)
           #:x-coord (lambda () (calc-x (get-x0) w h (get-a)))
           #:y-coord (lambda () (calc-y (get-y0) w h (get-a)))
           #:angle get-a
           #:x-size  (* w 2)
           #:y-size  (* h 2))
         (make <Text>
            #:label 't0
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:a-grid  '(150 . 0)
            #:length  10
            #:hint "Номер станции"
            #:text 'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object MOLE-VODO-MAIN-PUMP-STATION pr0)

  (add-lord->mole-proc
    MOLE-VODO-MAIN-PUMP-STATION
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-rect update-pref-text) lord-list)
                 (update-lord-list (list update-rect update-pref-text update-subline) lord-list))))
        (list '() (car hi) '() (cadr hi) '())))))
;;======================================================================
;;== Канализ. насосная станция =========================================
(let*
  ((r 300)
   (c 37)
   (font-size 450)
   (full-number (lambda () (string-append "КНС-" (get-number))))
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
         (make <Circle>
           #:h-visible #t
           #:l-visible #t
           #:foreground c
           #:x-coord 'x0
           #:y-coord 'y0
           #:x-size  r
           #:hint "Канализ. насосная станция")
         (make <Text>
            #:label 't0
            #:h-visible #f
            #:l-visible #f
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle   'ta
            #:a-grid  '(150 . 0)
            #:length  10
            #:hint "Номер станции"
            #:text 'number)
         (make <Text>
            #:color 0
            #:dependence '(number tx ty ta)
            #:x-coord get-tx
            #:y-coord get-ty
            #:angle   get-ta
            #:font (vector 0 (- font-size))
            #:text (cons 'full-number full-number))))))
  (add-object MOLE-VODO-CANAL-PUMP-STATION pr0)

  (add-lord->mole-proc
    MOLE-VODO-CANAL-PUMP-STATION
    (lambda (lord-list)
      (and-let*
        ((hi (or (update-lord-list (list update-point update-pref-text) lord-list)
                 (update-lord-list (list update-point update-pref-text update-subline) lord-list))))
        (list '() (car hi) (cadr hi) '())))))
;;======================================================================
;;== Дворовый колодец  Магистральный колодец ===========================
;;== Колодец на коллекторе Дренажный колодец ===========================
(let*
  ((class-list (list MOLE-VODO-GARDEN-WELL
                     MOLE-VODO-MAGISTRAL-WELL
                     MOLE-VODO-COLLECTOR-WELL
                     MOLE-VODO-DRENAGE-WELL))
   (r 95)
   (w 45)
   (c0 38)
   (c1 39)
   (font-size 220)
   (type-index (lambda () (type->index class-list)))
   (full-number (lambda ()
     (string-append (vector-ref '#("ДК" "МК" "КК" "ДР") (type-index))
                    "-" (get-number))))
   (hint (lambda ()
      (vector-ref '#("Дворовый колодец"
                     "Магистральный колодец"
                     "Колодец на коллекторе"
                     "Дренажный колодец") (type-index))))
   (color (lambda ()
     (vector-ref (vector c0 c0 c0 c1) (type-index))))
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
            #:hint "Номер колодца"
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
        ((hi (or (update-lord-list (list update-point update-pref-text) lord-list)
                 (update-lord-list (list update-point update-pref-text update-subline) lord-list))))
        (list '() (car hi) (cadr hi) '())))))
;;======================================================================
;;== Зона сан. охраны (полигон) ========================================
(let*
  ((c 30)
   (pr0
     (make <Container>
       #:contents
       (list
         (make <Smart-Polygon>
           #:h-visible #t
           #:l-visible #t
           #:color 0
           #:foreground c
           #:background -1
           #:fill #:bdiagonal
           #:coord-list 'apg
           #:hint "Зона сан. охраны")))))
  (add-object MOLE-VODO-POLYGON pr0)

  (add-lord->mole-proc
    MOLE-VODO-POLYGON
    (lambda (lord-list)
      (update-lord-list (list update-polygon) lord-list))))
;;======================================================================
(add-object MOLE-VODO-COMMENTARY (get-obj-prototype MOLE-COMMENTARY))
;;======================================================================
(add-context
  MOLE-VODOSNAB-OBJ
  MOLE-VODOSNAB-NET
  insert-line-object
  coedit-line-object)

(add-context
  MOLE-VODOSNAB-NET
  MOLE-VODO-BRANCH;;MOLE-VODOSNAB-OBJ
  insert-point-object
  copy-target)
;;======================================================================
(add-context
  MOLE-VODOTVOD-OBJ
  MOLE-VODOTVOD-NET
  insert-line-object
  coedit-line-object)

(add-context
  MOLE-VODOTVOD-NET
  MOLE-VODOTVOD-OBJ
  insert-point-object
  copy-target)
;;======================================================================
