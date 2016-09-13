;;=====================================================================
(add-lord->mole-proc
  MOLE-BASE-BUILDING
  (lambda (lord-list)
    (and-let*
      ((bl (update-lord-list (list update-polygon update-text) lord-list)))
      (set-cdr! (cadadr bl) (calc-num-coord (cdadar bl) (vector-ref (cdaddadr bl) 3)))
      bl)))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-BASE-QUARTER MOLE-TERRITORY)
  (lambda (lord-list)
    (update-lord-list update-polygon lord-list)))
;;======================================================================
(add-lord->mole-proc
  MOLE-GREEN-PLANT
  (lambda (lord-list)
    (update-lord-list update-polygon lord-list)))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-FENCE MOLE-RAILWAY MOLE-TRAMWAY)
  (lambda (lord-list)
    (update-lord-list update-polyline lord-list)))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-COMMENTARY MOLE-VODO-COMMENTARY MOLE-TEPLO-COMMENTARY)
  (lambda (lord-list)
    (or
      (update-lord-list update-text lord-list)
      (and-let*
        ((ml (update-lord-list (list update-point update-text) lord-list)))
        (list (cadr ml)))
      (and-let*
        ((ml (update-lord-list (list update-polyline update-text) lord-list))
         (pl (assv-ref ml #:polyline))
         (ll (assv-ref pl #:poly))
         (an (if (<= (length ll) 1) 0 (calc-a (car ll) (cadr ll))                   ))
         (tx (assv-ref ml #:text))
         (st (assv-ref tx #:string)))
        (assv-set! tx #:poly (car pl))
        (assv-set! tx #:coord (vector (caar ll) (cdar ll)))
        (vector-set! st 2 an)
        (list (cadr ml)))
        )))
;;======================================================================
(add-lord->mole-proc
  MOLE-COMMENTARY-BY-LINE
  (lambda (lord-list)
    (and-let*
      ((ml (update-lord-list (list update-subline update-text) lord-list)))
      (append! ml '(())))))
;;======================================================================
(add-lord->mole-proc
  MOLE-STREET-NAME
  (lambda (lord-list)
    (and-let*
      ((cl (update-lord-list update-text lord-list)))
      (list '() (list (caar cl)) (car cl)))))
;;======================================================================
(add-lord->mole-proc
  MOLE-STREET
  (lambda (lord-list)
    (update-lord-list (list update-polyline update-text) lord-list)))
;;======================================================================
(add-lord->mole-proc
  MOLE-BASE-RIVER
  (lambda (lord-list)
    (and-let*
      ((rv (or (update-lord-list update-polyline lord-list) '()))
       (lk (or (update-lord-list update-polygon lord-list) '()))
       (l (append rv lk))
       ((not (null? l))))
      (list
        (if (null? rv) MOLE-LAKE MOLE-RIVER)
        (list (if (null? lk) #:polyline #:polygon)
              (cons #:poly (unbound (cdadar l))))))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-RW-STATION)
  (lambda (lord-list)
    (update-lord-list update-point lord-list)))
;;======================================================================
(add-lord->mole-proc
  MOLE-BASE-SAFE
  (lambda (lord-list)
    (and-let*
      ((at (update-lord-list (list update-rect update-text) lord-list))
       (txt (assv-ref at #:text))
       (str (assv-ref txt #:string))
       (an (vector-ref str 3))
       (pos (or (string-rindex an #\space) (string-rindex an #\-) -1)))
      (vector-set! str 3 (substring an (+ 1 pos)))
      (list '() (car at) '() '() '() '() (cadr at) '()))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-ATS MOLE-MODULE-ATS MOLE-MINI-ATS)
  (lambda (lord-list)
    (and-let*
      ((at (update-lord-list (list update-rect update-text) lord-list))
       (txt (assv-ref at #:text))
       (str (assv-ref txt #:string))
       (an (vector-ref str 3))
       (pos (or (string-rindex an #\-) (string-rindex an #\space) -1)))
      (vector-set! str 3 (substring an (+ 1 pos)))
      (list '() (car at) '() '() '() '() (cadr at) '()))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-BASE-WELL MOLE-BASE-CAB-INPUT)
  (lambda (lord-list)
    (and-let*
      ((at (update-lord-list (list update-rect update-text) lord-list))
       (txt (assv-ref at #:text))
       (str (assv-ref txt #:string))
       (an (vector-ref str 3))
       (pos (or (string-rindex an #\space) -1)))
      (vector-set! str 3 (substring an (+ 1 pos)))
      (list '() (car at) '() '() '() (cadr at) '()))))
;;======================================================================
(add-lord->mole-proc
  MOLE-STAND-ARM
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list update-point lord-list)))
      (list (car sa) '() '()))))
;======================================================================
(add-lord->mole-proc
  MOLE-JOINT-POINT
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list (list update-point update-text) lord-list)))
      (list '() (car sa) (cadr sa)))))
;======================================================================
(add-lord->mole-proc
  MOLE-TAXOFON-BASE
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list (list update-rect update-text) lord-list)))
      (list '() (car sa) '() (cadr sa)))))
;======================================================================
(add-lord->mole-proc
  (list MOLE-CABEL-BOX)
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list (list update-rect update-text) lord-list)))
      (list '() (car sa) '() '() '() '() (cadr sa)))))
;======================================================================
(add-lord->mole-proc
  MOLE-ELECTRIC-TRACE
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list (list update-polyline update-text) lord-list)))
      (list (car sa) '()  (cadr sa) '()))))
;======================================================================
(add-lord->mole-proc
  (list MOLE-SUPPORT MOLE-MINISUPPORT)
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list (list update-point update-text) lord-list))
       (txt (assv-ref sa #:text))
       (str (assv-ref txt #:string)))
      (vector-set! str 0 (+ 120 (vector-ref str 0)))
      (list '() (car sa) '() (cadr sa)))))
;======================================================================
(add-lord->mole-proc
  (list MOLE-SUPPORT-WITH-BOX MOLE-MINISUPPORT-WITH-BOX)
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list (list update-point update-text) lord-list))
       (txt (assv-ref sa #:text))
       (str (assv-ref txt #:string)))
      (vector-set! str 0 (+ 220 (vector-ref str 0)))
      (list '() (car sa) '() (cadr sa)))))
;======================================================================
(add-lord->mole-proc
  MOLE-ELECTRIC-SUPPORT
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list update-point lord-list)))
      (list (car sa)
            '()))))
;======================================================================
(add-lord->mole-proc
  MOLE-LIGHT-SUPPORT
  (lambda (lord-list)
    (and-let*
      ((sa (update-lord-list update-rect lord-list)))
      (list (car sa)
            '()'()'()))))
;======================================================================
(define (find-max pll)
  (let*
    ((lm (map dist pll (cdr pll)))
     (mm (memv (apply max lm) lm)))
    (list-tail pll (- (length lm) (length mm)))))
;======================================================================
(add-lord->mole-proc
  (list MOLE-ARMOUR-CABEL-TRENCH
        MOLE-SUBBUILDING-LINE
        MOLE-UNDERWATER-CABEL-TRENCH)
  (lambda (lord-list)
    (and-let*
      ((ps (update-lord-list (list update-polyline update-text) lord-list))
       (pll (assv-ref (car ps) #:poly))
       ((>= (length pll) 2))
       (pl (find-max pll))
       (t0 (car pl))
       (t1 (cadr pl))
       (a (calc-a t0 t1))
       (ga (good-angle a))
       (p0 (if (= ga a) t0 t1))
       (p1 (if (= ga a) t1 t0))
       (l (dist p0 p1))
       (txt (assv-ref ps #:text))
       (str (assv-ref txt #:string))
       (t1 (vector-ref str 3))
       (tl (* (string-length t1) (scale-font) 55/100))
       (p2 (shift p0 (turn (cons (/ (- l tl) 2) (* -1/4 (scale-font))) ga))))
      (set-cdr! (car txt) (vector (car p2) (cdr p2)))
      (vector-set! str 2 ga)
      (list (car ps) (cadr ps) '()))))
;;======================================================================
(define (update-num-txt txt)
  (or
    (and-let*
      ((n0 (string->number txt))
       (n1 (inexact->exact (* n0 10))))
      (number->string (/ n1 10)))
    txt))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-COLLECTOR-SECTION
        MOLE-SMALL-SQUARE-COLLECTOR-SECTION
        MOLE-AIR-CONNECTION-LINE
        MOLE-HANGING-CABEL
        MOLE-ON-BRIGE-LINE
        MOLE-OTHER-PASSAGE)
  (lambda (lord-list)
    (and-let*
      ((ps (update-lord-list (list update-polyline update-text) lord-list))
       (pl (assv-ref (car ps) #:poly))
       ((>= (length pl) 2))
       (t0 (car pl))
       (t1 (car (last-pair pl)))
       (a (calc-a t0 t1))
       (ga (good-angle a))
       (p0 (if (= ga a) t0 t1))
       (p1 (if (= ga a) t1 t0))
       (l (dist p0 p1))
       (txt (assv-ref ps #:text))
       (str (assv-ref txt #:string))
       (tx1 (update-num-txt (vector-ref str 3)))
       (tl (* (string-length tx1) (scale-font) 55/100))
       (p2 (shift p0 (turn (cons (/ (- l tl) 2) (* -1/4 (scale-font))) ga)))
       )
      (set-cdr! (car txt) (vector (car p2) (cdr p2)))
      (set-cdr! (cadar ps) (list p0 p1))
      (vector-set! str 2 ga)
      (vector-set! str 3 tx1)
      (list (car ps) (cadr ps) '()))))
;;======================================================================
(add-lord->mole-proc
  MOLE-CABEL-PASSAGE
  (lambda (lord-list)
    (and-let*
      ((ps (update-lord-list (list update-polyline update-text update-text) lord-list))
       (pll (assv-ref (car ps) #:poly))
       (pl (find-max pll))
       (t0 (car pl))
       (t1 (cadr pl))
       (a (calc-a t0 t1))
       (ga (good-angle a))
       (p0 (if (= ga a) t0 t1))
       (p1 (if (= ga a) t1 t0))
       (l (dist p0 p1))
       (txt (assv-ref (cdr ps) #:text))
       (str (assv-ref txt #:string))
       (tx1 (update-num-txt (vector-ref str 3)))
       (p2 (shift p0 (turn (cons (/ l 2) (* -1/4 (scale-font))) ga))))
      (set-cdr! (car txt) (vector (car p2) (cdr p2)))
      (vector-set! str 2 ga)
      (vector-set! str 3 tx1)
      (list (car ps) (cadr ps) (caddr ps) '()))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-EXACT-ATTACHMENT MOLE-ROUND-ATTACHMENT
        MOLE-VODO-EXACT-ATTACHMENT MOLE-VODO-ROUND-ATTACHMENT
        MOLE-TEPLO-EXACT-ATTACHMENT MOLE-TEPLO-ROUND-ATTACHMENT)
  (lambda (lord-list)
    (and-let*
      ((ps (update-lord-list (list update-polyline update-text) lord-list)))
      (list (car ps) (cadr ps) '()))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-INEXACT-ATTACHMENT
        MOLE-VODO-INEXACT-ATTACHMENT
        MOLE-TEPLO-INEXACT-ATTACHMENT)
  (lambda (lord-list)
    (and-let*
      ((ps (update-lord-list (list update-polyline update-text) lord-list)))
      (list (car ps) (cadr ps)))))
;;======================================================================
(add-lord->mole-proc
  MOLE-ISOLINE
  (lambda (lord-list)
    (and-let*
      ((ps (update-lord-list (list update-polyline update-text) lord-list))
       (pl (car ps))
       (tx (cadr ps))
       (p0 (car (assv-ref pl #:poly)))
       (tc (assv-ref tx #:coord))
       (ts (assv-ref tx #:string)))
      (vector-set! tc 0 (car p0))
      (vector-set! tc 1 (cdr p0))
      (vector-set! ts 2 0)
      (list (car ps) '(#:container) (cadr ps) '()))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-ATS-REGION MOLE-NODE-REGION)
  (lambda (lord-list)
    (and-let*
      ((ps (update-lord-list (list update-polygon update-text) lord-list))
       (pl (car ps))
       (tx (cadr ps))
       (p0 (car (assv-ref pl #:poly)))
       (tc (assv-ref tx #:coord))
       (ts (assv-ref tx #:string)))
      (vector-set! tc 0 (car p0))
      (vector-set! tc 1 (cdr p0))
      (vector-set! ts 2 0)
      (list (car ps) '(#:container) (cadr ps) '()))))
;;======================================================================
(add-lord->mole-proc
  MOLE-BASE-SUBSTAT
  (lambda (lord-list)
    (and-let*
      ((sl (update-lord-list (list update-true-rect update-text) lord-list))
       (txt (assv-ref sl #:text))
       (str (assv-ref txt #:string))
       (name (vector-ref str 3))
       (num (string-length name)))
      (vector-set! str 3 (string-append name ".jpg"))
      (list
        (+ MOLE-BASE-SUBSTAT num -3)
        (cons #:bar (car sl))
        (cons #:text (cadr sl))))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-MAG-LIVING-POINT)
  (lambda (lord-list)
    (and-let*
      ((sl (update-lord-list (list update-rect update-text) lord-list))
       (tx (assv-ref sl #:text))
       (st (assv-ref tx #:coord))
       (x (vector-ref st 0)))
      (vector-set! st 0 (+ x 10))
      sl)))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-MAG-NET-PUNCT-163
        MOLE-MAG-NET-PUNCT-154
        MOLE-MAG-NET-PUNCT-157
        MOLE-MAG-MUFTA-STVOR
        MOLE-MAG-MUFTA-CONNECT
        MOLE-MAG-MUFTA-BRANCH)
  (lambda (lord-list)
    (and-let*
      ((sl (update-lord-list (list update-rect update-text update-text) lord-list))
       (tx (assv-ref sl #:text))
       (st (assv-ref tx #:coord))
       (x (vector-ref st 0)))
      (vector-set! st 0 (+ x 50))
      (list '() (car sl) '() (cadr sl) (caddr sl)))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-MAG-NET-PUNCT-192)
  (lambda (lord-list)
    (and-let*
      ((sl (update-lord-list (list update-rect update-text update-text) lord-list))
       (tx (assv-ref sl #:text))
       (st (assv-ref tx #:coord))
       (x (vector-ref st 0)))
      (vector-set! st 0 (+ x 50))
      (list '() (car sl) '() '() '() (cadr sl) (caddr sl)))))
;;======================================================================
(add-lord->mole-proc
  (list MOLE-MAG-NET-PUNCT-156
        MOLE-MAG-NET-PUNCT-191
        MOLE-MAG-NET-PUNCT-203
        MOLE-MAG-NET-PUNCT-187
        MOLE-MAG-NET-PUNCT-188
        MOLE-MAG-NET-PUNCT-197
        MOLE-MAG-NET-PUNCT-198
        MOLE-MAG-NET-PUNCT-199
        MOLE-MAG-NET-PUNCT-200
        MOLE-MAG-NET-PUNCT-201
        MOLE-MAG-NET-PUNCT-195
        MOLE-MAG-NET-PUNCT-196
        MOLE-MAG-NET-PUNCT-202
        MOLE-MAG-NET-PUNCT-204
        MOLE-MAG-NET-PUNCT-205
        MOLE-MAG-NET-PUNCT-155
        MOLE-MAG-NET-PUNCT-183
        MOLE-MAG-NET-PUNCT-194
        MOLE-MAG-NET-PUNCT-193
        MOLE-MAG-NET-PUNCT-190)
  (lambda (lord-list)
    (and-let*
      ((sl (update-lord-list (list update-rect update-text update-text) lord-list))
       (tx (assv-ref sl #:text))
       (st (assv-ref tx #:coord))
       (x (vector-ref st 0)))
      (vector-set! st 0 (+ x 50))
      (list '() (car sl) '() '() (cadr sl) (caddr sl)))))
;;======================================================================
(add-lord->mole-proc
  MOLE-MAG-LINE
  (lambda (lord-list)
    (let
      ((sl (update-lord-list (list update-polyline update-text) lord-list)))
      (list '() (car sl) (cadr sl)'()))))
;;======================================================================

