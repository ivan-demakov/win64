;;=====================================================================
(add-context
  MOLE-ATTACHMENT-POINT
  (list MOLE-BASE-WELL
        MOLE-BASE-CAB-INPUT
        MOLE-BASE-SUPPORT
        MOLE-STAND-ARM
        MOLE-BASE-SAFE
        MOLE-BASE-ATTACHMENT)
  copy-point
  copy-target)
;;=====================================================================
(define on-house-list
  (list MOLE-CABEL-INPUT-IN-CELLAR
        MOLE-CABEL-INPUT-ON-WALL
        MOLE-AIR-CABEL-INPUT
        MOLE-BASE-SAFE))

(add-context
  MOLE-BASE-BUILDING
  on-house-list
  (lambda (x y)
    (and-let*
      ((apg (get-property 'apg))
       (spl (enbound #t apg))
       (n (select-segment x y spl #f))
       (pl (list-head (list-tail spl n) 2))
       (a (calc-a (car pl) (cadr pl)))
       (da (* (clockwise? spl) -900))
       (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
       (p1 (turn-point (car p0) (+ (cdar pl) 150) (caar pl) (cdar pl) a))
       (p2 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a)))
      (list
        (cons 'x0 (car p2))
        (cons 'y0 (cdr p2))
        (cons 'a (+ a da)))))
;;  (make-coedit-point-object 'apg)
  copy-target)
;;=====================================================================
(add-context
  (list MOLE-BASE-WELL
        MOLE-BASE-CAB-INPUT
        MOLE-BASE-SUPPORT
        MOLE-STAND-ARM
        MOLE-BASE-SAFE)
  MOLE-BASE-ATTACHMENT
  copy-point
  copy-target)
;;=====================================================================
(add-context
  (list MOLE-EXACT-ATTACHMENT
        MOLE-INEXACT-ATTACHMENT)
  (list MOLE-BASE-WELL
        MOLE-BASE-CAB-INPUT
        MOLE-BASE-SUPPORT
        MOLE-STAND-ARM
        MOLE-BASE-SAFE
        MOLE-BASE-ATTACHMENT)
  append-line
  copy-target)
;;=====================================================================
(add-context
  (list MOLE-BASE-WELL
        MOLE-BASE-CAB-INPUT
        MOLE-BASE-SUPPORT
        MOLE-STAND-ARM
        MOLE-BASE-SAFE
        MOLE-OUTSIDE-MSU
        MOLE-INSIDE-MSU)
  MOLE-BASE-PASSAGE
  insert-line-object
  coedit-line-object)

(add-context
  MOLE-CABEL-PASSAGE
  MOLE-BASE-WELL
  insert-point-object
  copy-target)

(add-context
  MOLE-ARMOUR-CABEL-TRENCH
  (list MOLE-INSPECT-DEV MOLE-BASE-SUPPORT)
  insert-point-object
  copy-target)

(add-context
  MOLE-HANGING-CABEL
  MOLE-BASE-SUPPORT
  insert-point-object
  copy-target)
;;=====================================================================
(add-context
  MOLE-STREET
  MOLE-STREET-NAME
  (lambda (x y)
    (and-let*
      ((spl (get-property 'apl))
       (n (select-segment x y spl #f))
       (pl (list-tail spl n))
       (a (calc-a (car pl) (cadr pl)))
       (ga (good-angle a))
       (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
       (p1 (turn-point (car p0) ((if (= ga a) + -) (cdar pl) 140) (caar pl) (cdar pl) a))
       (p2 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a)))
      (list
        (cons 'number (get-property 'number))
        (cons 'x0 (car p2))
        (cons 'y0 (cdr p2))
        (cons 'ta ga)
        (cons 'tx (car p1))
        (cons 'ty (cdr p1)))))

  (lambda (host-old host-new target)
    (or
      (and-let*
        ((spl (get-property-from host-new 'apl))
         (x (get-property-from target 'x0))
         (y (get-property-from target 'y0))
         (n (select-segment x y spl #f))
         (pl (list-head (list-tail spl n) 2))
         (a (calc-a (car pl) (cadr pl)))
         (ga (good-angle a))
         (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
         (p1 (turn-point (car p0) ((if (= ga a) + -) (cdar pl) 140) (caar pl) (cdar pl) a))
         (p2 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a)))
        (list
          (cons 'number (get-property-from target 'number))
          (cons 'x0 (car p2))
          (cons 'y0 (cdr p2))
          (cons 'ta ga)
          (cons 'tx (car p1))
          (cons 'ty (cdr p1))))
      target)))
;;=====================================================================
(define (same x) x)

(define (calc-pl-for-text apl txt x y)
  (let*
    ((n (select-segment x y apl #f))
     (pl (list-tail apl n))
     (a (calc-a (car pl) (cadr pl)))
     (g (good-angle a))
     (p0 (turn-point x y (caar pl) (cdar pl) (- a)))
     (p1 (turn-point (car p0) (cdar pl) (caar pl) (cdar pl) a))
     (pl1 (list p1))
     (pl2 (list-head apl (inc n)))
     (ppl
       (if (equal? (car apl) (last-point apl))
         ((if (= a g) same reverse) (append pl1 (cdr pl) pl2 pl1))
         (cons p1 (if (= a g) (cdr pl) (reverse pl2))))))
    (let loop ((pl ppl)
               (l (* 12/10 (calc-string-width txt std-font 0)))
               (r (list-head ppl 1)))
      (if (< (length pl) 2)
        r
        (let
          ((d (dist (car pl) (cadr pl))))
          (if (> l d)
            (loop (cdr pl) (- l d) (append r (list (cadr pl))))
            (let*
              ((m (- d l))
              (x (inexact->exact (/ (+ (* (caar pl) m) (* (caadr pl) l)) d)))
              (y (inexact->exact (/ (+ (* (cdar pl) m) (* (cdadr pl) l)) d))))
              (append r (list (cons x y))))))))))

(add-context
  MOLE-STREET
  MOLE-STREET-NAME-BY-LINE
  (lambda (x y)
    (and-let*
      ((apl (get-property 'apl))
       (txt (get-property 'number)))
      (list
        (cons 'number txt)
        (cons 'apl (calc-pl-for-text apl txt x y)))))

  (lambda (host-old host-new target)
    (and-let*
      ((apl (get-property-from host-new 'apl))
       (tpl (get-property-from target 'apl))
       ((>= (length tpl) 1))
       (x (caar tpl))
       (y (cdar tpl))
       (txt (get-property-from target 'number)))
      (list
        (cons 'number txt)
        (cons 'apl (calc-pl-for-text apl txt x y))))))
;;=====================================================================
(add-context
  (list MOLE-ATS MOLE-MODULE-ATS MOLE-BASE-SAFE MOLE-KOROBKA
        MOLE-TAXOFON-1 MOLE-TAXOFON-2 MOLE-TAXOFON-3 MOLE-TAXOFON-4)
  MOLE-CORD
  insert-line-object

  (lambda (host-old host-new target)
    (and-let*
      ((sx0 (get-property-from host-old 'x0))
       (sy0 (get-property-from host-old 'y0))
       (hx0 (get-property-from host-new 'x0))
       (hy0 (get-property-from host-new 'y0))
       (p-old (cons sx0 sy0))
       (p-new (cons hx0 hy0))
       (pl (get-property-from target 'apl))
       (d0 (dist p-old (car pl)))
       (dn (dist p-old (last-point pl))))
      (list-set! pl (if (< d0 dn) 0 (- (length pl) 1)) p-new)
      (assv-set! target 'apl pl))))
;;=====================================================================
(add-global-context
  (list MOLE-TAXOFON-BASE MOLE-BASE-SAFE MOLE-CABEL-BOX)
  (lambda (vars env0 env1)
    (and-let*
     ((x (assv-ref env0 'tx))
      (y (assv-ref env0 'ty))
      (p (cons x y))
      (a (assv-ref env0 'ta))
      (px (shift p (turn (cons   0 320) a)))
      (pt (shift p (turn (cons 200 400) a))))
      (list (cons 'x0 (car px))
            (cons 'y0 (cdr px))
            (cons 'a a)
            (cons 'tx (car pt))
            (cons 'ty (cdr pt))
            (cons 'ta a)))))
;;=====================================================================
(add-global-context
  (list MOLE-INSPECT-DEV)
  (lambda (vars env0 env1)
    (define (get-objects pt . types)
      (define (get type)
        (define (get-pt well)
          (cons (assv-ref well 'x0) (assv-ref well 'y0)))
        (memq-remove
          #f
          (map
            (lambda (w) (equal? pt (get-pt w)))
            (get-all-of-class pt (list type)))))
      (apply append (map get types)))

    (and-let*
     ((pll (assv-ref env0 'apl))
      (p0 (car pll))
      (p1 (car (last-pair pll)))
      (l0 (get-objects p0 (list MOLE-INSPECT-DEV MOLE-BASE-SAFE)))
      (l1 (get-objects p1 (list MOLE-INSPECT-DEV MOLE-BASE-SAFE)))
      ((or (null? l0) (null? l1)))
      ((not (and (null? l0) (null? l1))))
      (pt (if (null? l0) p0 p1)))
      (list (cons 'x0 (car pt))
            (cons 'y0 (cdr pt))
            ))))
;;=====================================================================
(add-context
  MOLE-CABEL-BRANCH
  MOLE-BASE-PASSAGE
  insert-line-object
  coedit-line-object)

(add-context
  MOLE-CABEL-BRANCH
  MOLE-BASE-ATTACHMENT
  copy-point
  copy-target)

(add-context
  (list MOLE-CABEL-PASSAGE
        MOLE-COLLECTOR-SECTION
        MOLE-TRAY-PASSAGE
        MOLE-SMALL-SQUARE-COLLECTOR-SECTION
        MOLE-UNDERWATER-CABEL-TRENCH
        MOLE-ARMOUR-CABEL-TRENCH
        MOLE-AIR-CONNECTION-LINE
        MOLE-HANGING-CABEL
        MOLE-SUBBUILDING-LINE
        MOLE-ON-BRIGE-LINE
        MOLE-OTHER-PASSAGE)
  MOLE-CABEL-BRANCH
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
  (make-coedit-point-object 'apl))
;;=====================================================================
(add-context
  (list MOLE-CABEL-PASSAGE
        MOLE-COLLECTOR-SECTION
        MOLE-TRAY-PASSAGE
        MOLE-SMALL-SQUARE-COLLECTOR-SECTION
        MOLE-UNDERWATER-CABEL-TRENCH
        MOLE-ARMOUR-CABEL-TRENCH
        MOLE-AIR-CONNECTION-LINE
        MOLE-HANGING-CABEL
        MOLE-SUBBUILDING-LINE
        MOLE-ON-BRIGE-LINE
        MOLE-OTHER-PASSAGE)
  (list MOLE-DESTRUCTED-LABEL MOLE-UNUSED-LABEL)
  insert-point-object
  (make-coedit-point-object 'apl))
;;=====================================================================
