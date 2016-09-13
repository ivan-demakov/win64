;;======================================================================
(define lord->mole-proc-list '())
;;======================================================================
(define (add-lord->mole-proc class lord->mole-proc)
  (for-each
    (lambda (c)
      (set! lord->mole-proc-list
            (assv-set! lord->mole-proc-list c lord->mole-proc)))
    (if (list? class) class (list class))))
;;======================================================================
(define (read-line port)
  (let loop ((ln "") (ch (read-char port)))
    (cond
      ((eof-object? ch) (and (positive? (string-length ln)) ln))
      ((char=? ch #\newline) (string-append ln (string ch)))
      (else (loop (string-append ln (string ch)) (read-char port))))))
;;======================================================================
(define (read-lines port)
  (let loop ((ln-list '()) (ln (read-line port)))
    (if ln
      (loop (append! ln-list (list ln)) (read-line port))
      ln-list)))
;;======================================================================
(define (write-line ln port)
  (dotimes (i (string-length ln))
    (write-char (string-ref ln i) port)))
;;======================================================================
(define (write-lines ln-list port)
  (for-each
    (lambda (ln) (write-line ln port))
    ln-list))
;;======================================================================
(define (string-str-index str sub)
  (let loop ((i 0) (k (string-length sub)))
    (and (<= k (string-length str))
      (if (string-ci=? (substring str i k) sub)
        i
        (loop (inc i) (inc k))))))
;;======================================================================
(define (find-str ln-list str)
  (let loop ((ln-list ln-list) (i 0))
    (cond
      ((null? ln-list) #f)
      ((string-str-index (car ln-list) str) i)
      (else (loop (cdr ln-list) (inc i))))))
;;======================================================================
(define (update-params ll type params)
  (let*
    ((pt (string-append "    (" (number->string type) " "))
     (p (find-str ll pt))
     (hd (list-head ll (or p 1)))
     (tl (list-tail ll (if p (inc p) 1))))
    (append hd
            (if params
              (list (string-append pt " . " (object->string params) ")\n"))
              '())
            tl)))
;;======================================================================
(define (update-abstract! path concret? add? type params)
  (and-let*
    ((port (open-input-file path))
     (ln-list (read-lines port))
     (par-pos-st (find-str ln-list "{{parameters begin}}"))
     (par-pos-fn (find-str ln-list "{{parameters end}}"))
     ((< par-pos-st par-pos-fn))
     (lp0 (list-head ln-list par-pos-st))
     (lp1 (list-tail (list-head ln-list par-pos-fn) par-pos-st))
     (lp2 (list-tail ln-list par-pos-fn)))
    (close-input-port port)
    (set! port (open-output-file path))
    (@set-current-module! (current-module))
    (write-lines (append lp0 (update-params lp1 type params) lp2) port)
    (close-output-port port)
    (if concret?
      (if add?
        (begin
          (set! concret-param-list (assv-set! concret-param-list type params))
          (add-concret type))
        (begin
          (set! concret-param-list (assv-set! concret-param-list type params))
          (remove-abstract type)))
      (if add?
        (begin
          (set! abstract-param-list (assv-set! abstract-param-list type params))
          (add-abstract type))
        (begin
          (set! abstract-param-list (assv-set! abstract-param-list type params))
          (remove-abstract type))))))
;;======================================================================
(define default-param-list
  '((#:contents #:text)
    (#:icon-ref . 0)
    (#:hint . "")
    (#:text-hint . "")
    (#:polyline-color . 0)
    (#:polyline-width . 0)
    (#:polyline-style . #:solid)
    (#:fill-style . #:solid)
    (#:fore-color . 0)
    (#:back-color . 0)
    (#:text-select-visible . #f)
    (#:font-face . 0)
    (#:font-size . -420)
    (#:font-weight . "NORMAL")
    (#:font-style . "**")
    (#:font-color . 0)))
;;======================================================================
(define (get-param type key)
  (assv-ref (or (assv-ref concret-param-list  type)
                (assv-ref abstract-param-list type)
                default-param-list)
            key))
;;======================================================================
(define (get-hint type) (get-param type #:hint))
(define (get-text-hint type) (get-param type #:text-hint))
(define (get-text-select-visible? type) (get-param type #:text-select-visible))
(define (get-icon-ref type) (get-param type #:icon-ref))
(define (get-font-size type) (get-param type #:font-size))
(define (get-font-weight type) (get-param type #:font-weight))
(define (get-font-style type) (get-param type #:font-style))
(define (get-font-color type) (get-param type #:font-color))
(define (get-font-face type) (get-param type #:font-face))
(define (get-back-color type) (get-param type #:back-color))
(define (get-fore-color type) (get-param type #:fore-color))
(define (get-fill-style type) (get-param type #:fill-style))
(define (get-polyline-width type) (get-param type #:polyline-width))
(define (get-polyline-style type)
  (let*
    ((ps (get-param type #:polyline-style))
     (ss (keyword->string ps)))
    (if (char-ci=? (string-ref ss 0) #\t)
      (string->number (substring ss 1))
      ps)))
(define (get-polyline-color type) (get-param type #:polyline-color))
(define (get-contents type) (get-param type #:contents))
(define (icon? type) (memv? #:icon (get-contents type)))
(define (polyline? type) (memv? #:polyline (get-contents type)))
(define (bound-polyline? type) (memv? #:bound-polyline (get-contents type)))
(define (outlined-polygon? type) (memv? #:outlined-polygon (get-contents type)))
(define (smart-polygon? type) (memv? #:smart-polygon (get-contents type)))
(define (polygon? type) (memv? #:polygon (get-contents type)))
(define (text? type) (memv? #:text (get-contents type)))
(define (pure-text? type) (and (text? type) (= 1 (length (get-contents type)))))
(define (footnote? type) (and (icon? type)
                              (text? type)
                              (memv? #:footnote (get-contents type))))
(define (make-font-vector type) (vector (get-font-face   type)
                                        (get-font-size   type)
                                        (get-font-weight type)
                                        (get-font-style  type)))
;;======================================================================
(define (abstr-lord-proc lord-list)
  (and-let*
    ((type (get-class))
     (tuprc
      (memq-remove #f
        (list (and (footnote?         type) update-subline)
              (and (icon?             type) update-point)
              (and (polygon?          type) update-polygon)
              (and (outlined-polygon? type) update-polygon)
              (and (smart-polygon?    type) update-polygon)
              (and (polyline?         type) update-polyline)
              (and (bound-polyline?   type) update-polygon)
              (and (text?             type) update-text))))

      (bl
        (or (update-lord-list tuprc lord-list)
            (and-let*
              (((pure-text? type))
               (ml (update-lord-list (list update-point update-text) lord-list)))
              (list (cadr ml)))
            (and-let*
              (((pure-text? type))
               (ml (update-lord-list (list update-polyline update-text) lord-list))
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
     (if (and (text? type) (icon? type))
       (set-car! (cadr bl) (caar bl)))
     bl))
;;======================================================================
(define (abstract type)
  (define (has-body?)
    (or (icon? type)
        (polygon? type)
        (outlined-polygon? type)
        (smart-polygon? type)
        (polyline? type)
        (bound-polyline? type)))
  (make <Container>
    #:contents
      (memq-remove #f
        (list
          (and (footnote? type)
               (make <Color-Footnote>
                 #:dependence '(x0 y0 tx ty ta number)
                 #:s-visible (get-text-select-visible? type)
                 #:line  #:solid
                 #:color 0
                 #:width 0
                 #:coord-list
                   (cons 'vpl
                         (lambda ()
                           (make-footnote
                             (get-x0) (get-y0)
                             (lim-tx) (lim-ty) (get-ta)
                             (get-number)
                             (abs (get-font-size type))
                             #f)))))
          (and (icon? type)
               (make <Icon>
                 #:x-coord 'x0
                 #:y-coord 'y0
                 #:ref     (get-icon-ref type)
                 #:hint    (get-hint type)))
          (and (polygon? type)
               (make <Filled-Polygon>
                 #:shaped #t
                 #:foreground (get-fore-color type)
                 #:background (get-back-color type)
                 #:fill       (get-fill-style type)
                 #:hint       (get-hint type)
                 #:coord-list 'apg))
          (and (outlined-polygon? type)
               (make <Outlined-Polygon>
                 #:shaped #t
                 #:foreground (get-fore-color type)
                 #:background (get-back-color type)
                 #:fill       (get-fill-style type)
                 #:line       (get-polyline-style type)
                 #:color      (get-polyline-color type)
                 #:width      (get-polyline-width type)
                 #:hint       (get-hint type)
                 #:coord-list 'apg))
          (and (smart-polygon? type)
               (make <Smart-Polygon>
                 #:shaped #t
                 #:foreground (get-fore-color type)
                 #:background (get-back-color type)
                 #:fill       (get-fill-style type)
                 #:line       (get-polyline-style type)
                 #:color      (get-polyline-color type)
                 #:width      (get-polyline-width type)
                 #:hint       (get-hint type)
                 #:coord-list 'apg))
          (and (polyline? type)
               (make <Color-Polyline>
                 #:shaped #t
                 #:line  (get-polyline-style type)
                 #:color (get-polyline-color type)
                 #:width (get-polyline-width type)
                 #:hint  (get-hint type)
                 #:coord-list 'apl))
          (and (bound-polyline? type)
               (make <Bound-Color-Polyline>
                 #:shaped #t
                 #:line  (get-polyline-style type)
                 #:color (get-polyline-color type)
                 #:width (get-polyline-width type)
                 #:hint  (get-hint type)
                 #:coord-list 'apg))
          (and (text? type)
               (make <Text>
                 #:valid-test (if (has-body?)
                                std-valid-test
                                (lambda (s) (and (string? s) (positive? (string-length s)))))
                 #:s-visible (get-text-select-visible? type)
                 #:color (get-font-color type)
                 #:font  (make-font-vector type)
                 #:x-coord (cons 'tx lim-tx)
                 #:y-coord (cons 'ty lim-ty)
                 #:angle   'ta
                 #:text    'number
                 #:hint    (get-text-hint type)))
            ))))
;;======================================================================
(define (make-connected-abstract env1 env2)
  (or (make-connected-polyline env1 env2)
      (make-connected-polygon  env1 env2)))
;;======================================================================
(define (add-abstract type)
  (add-lord->mole-proc type abstr-lord-proc)
  (add-user-object type (abstract type)))
;;======================================================================
(define (add-concret type)
  (add-lord->mole-proc type abstr-lord-proc)
  (add-object type (abstract type)))
;;======================================================================
(define (remove-abstract type)
  (set! lord->mole-proc-list (assv-remove! lord->mole-proc-list type))
  (remove-user-object type))
;;======================================================================
(for-each add-abstract (map car abstract-param-list))
(for-each add-concret (map car concret-param-list))
;;======================================================================
;;======================================================================
(define (update-lord-object lord-obj)
  (and-let*
    ((rccmp (assv #:rect lord-obj))
     (rct (assv #:poly (cdr rccmp)))
     (rccrd (cdr rct))
     (p0 (car rccrd))
     (p1 (cadr rccrd))
     (x0 (car p0))
     (y0 (cdr p0))
     (x1 (car p1))
     (y1 (cdr p1))
     (rc (list p0 (cons x1 y0) p1 (cons x0 y1))))
    (set-cdr! rct rc)
    (set-car! rccmp #:refpoint))
  (and-let*
    ((rccmp (assv #:roundrect lord-obj))
     (rct (assv #:poly (cdr rccmp)))
     (rccrd (cdr rct))
     (p0 (car rccrd))
     (p1 (cadr rccrd))
     (r (caaddr rccrd))
     (x0 (car p0))
     (y0 (cdr p0))
     (x1 (car p1))
     (y1 (cdr p1))
     ((> r 0))
     (< r (abs (- x0 x1)))
     (< r (abs (- y0 y1)))
     (x2 (+ x0 r))
     (x3 (- x1 r))
     (y2 (+ y0 r))
     (y3 (- y1 r))
     (r0 (cons x2 y0))
     (r1 (cons x3 y0))
     (r2 (cons x1 y2))
     (r3 (cons x1 y3))
     (r4 (cons x3 y1))
     (r5 (cons x2 y1))
     (r6 (cons x0 y3))
     (r7 (cons x0 y2))
     (mc (/ r (sqrt 2.0)))
     (mp (lambda (p0 p1 cn)
       (cons (+ (car cn) (* mc (+ (- (car p1) (car cn)) (- (car p0) (car cn)))))
             (+ (cdr cn) (* mc (+ (- (cdr p1) (cdr cn)) (- (cdr p0) (cdr cn))))))))
     (rc (list (make-arced (list r7 (mp r7 r0 (cons x2 y2)) r0))
               (make-arced (list r1 (mp r1 r2 (cons x3 y2)) r2))
               (make-arced (list r3 (mp r3 r4 (cons x3 y3)) r4))
               (make-arced (list r5 (mp r5 r6 (cons x2 y3)) r6)))))
    (set-cdr! rct rc)
    (set-car! rccmp #:polygon))
  (and-let*
    ((elcmp (assv #:ellipse lord-obj))
     (epl (assv #:poly (cdr elcmp)))
     (elcrd (cdr epl))
     (p0 (car elcrd))
     (p1 (cadr elcrd))
     (x0 (car p0))
     (y0 (cdr p0))
     (x1 (car p1))
     (y1 (cdr p1))
     (cx (/ (+ x0 x1) 2))
     (cy (/ (+ y0 y1) 2))
     (cp (cons cx cy))
     (el (make-ellipsed (list cp (cons cx y1) p1))))
    (set-cdr! epl el)
    (set-car! elcmp #:polygon))
  (and-let*
    ((elcmp (assv #:arc lord-obj))
     (epl (assv #:poly (cdr elcmp)))
     (elcrd (cdr epl))
     (p0 (car elcrd))
     (p1 (cadr elcrd))
     (p2 (caddr elcrd))
     (p3 (cadddr elcrd))
     (x0 (car p0))
     (y0 (cdr p0))
     (x1 (car p1))
     (y1 (cdr p1))
     (cx (/ (+ x0 x1) 2))
     (cy (/ (+ y0 y1) 2))
     (cc (cons cx cy))
     (dx (abs (- x1 cx)))
     (dy (abs (- y1 cy)))
     (r (cons (max dx dy) 0))
     (a0 (std-angle (calc-a cc p2)))
     (a1 (std-angle (calc-a cc p3)))
     (a2 (/ (+ a0 a1 (if (< a0 a1) 0 -3600)) 2))
     (t0 (shift cc (turn r a0)))
     (t1 (shift cc (turn r a1)))
     (t2 (shift cc (turn r a2)))
     (el (make-arced (list t0 t1 t2))))
    (set-car! elcmp #:polyline)
    (set-cdr! epl el))
  (and-let*
    ((cls (assv-ref lord-obj 'class))
     (sts (assv-ref lord-obj 'status))
     ((set-object-type cls sts))
     (proc (find-match lord->mole-proc-list cls))
     (lst (proc (cdddddr lord-obj))))
    (list
      (cons 'class (if (number? (car lst)) (car lst) cls))
      (cons 'status sts)
      (assv 'table  lord-obj)
      (assv 'entry  lord-obj)
      (assv 'node   lord-obj)
      (cons #:container (list (if (number? (car lst)) (cdr lst) lst))))))
;;======================================================================
(define (update-lord-list proc-list lord-list)
  (and-let*
    ((proc-list (if (list? proc-list ) proc-list (list proc-list)))
     ((= (length proc-list) (length lord-list)))
     (ml (map (lambda (p l) (p l)) proc-list lord-list))
     ((every (lambda (x) x) ml)))
    ml))
;;=====================================================================
(define (ext-poly lst)
  (define (point? p)
    (and (pair? p) (number? (car p)) (number? (cdr p))))
  (and-let*
    ((pl (assv-ref lst #:poly))
     ((list? pl))
     ((every point? pl)))
    pl))
;;=====================================================================
(define (extract-poly lst type unbnd? min-num)
  (and-let*
    ((pl (ext-poly lst))
     ((>= (length pl) min-num)))
    (list type (cons #:poly (if unbnd? (unbound pl) pl)))))
;;=====================================================================
(define (update-polygon lst)
  (and (eq? (car lst) #:polygon)
       (extract-poly lst #:polygon #t 3)))
;;=====================================================================
(define (update-polyline lst)
  (and (eq? (car lst) #:polyline)
       (extract-poly lst #:polyline #f 2)))
;;=====================================================================
(define (update-subline lst)
  (and (eq? (car lst) #:polyline)
       (extract-poly lst #:polyline #f 0)))
;;=====================================================================
(define (get-poly cnt)
  (assv-ref (cadar cnt) #:poly))
;;=====================================================================
(define (make-item key . ml)
  (cons key (list->vector ml)))
;;=====================================================================
(define (update-point lst)
  (and-let*
    (((eq? (car lst) #:refpoint))
     (pl (ext-poly lst))
     (ln (length pl))
     (x (if (>= ln 1) (caar pl) 0))
     (y (if (>= ln 1) (cdar pl) 0)))
    (list (make-item #:coord x y))))
;;=====================================================================
(define (update-rect lst)
  (and-let*
    (((eq? (car lst) #:refpoint))
     (pl (ext-poly lst))
     (ln (length pl))
     (x (if (>= ln 1) (caar pl) 0))
     (y (if (>= ln 1) (cdar pl) 0))
     (a (if (>= ln 3) (rad->deg (atan (- (cdar pl) (cdaddr pl)) (- (caaddr pl) (caar pl)))) 0))
     (d (if (>= ln 3) (dist (car pl) (cadr pl)) 0)))
    (list (make-item #:coord x y)
          (make-item #:size d 0 a))))
;;=====================================================================
(define (update-true-rect lst)
  (and-let*
    ((pl (ext-poly lst))
     (ln (length pl))
     (x (if (>= ln 1) (caar pl) 0))
     (y (if (>= ln 1) (cdar pl) 0))
     (w (if (>= ln 3) (- (caaddr pl) x) 0))
     (h (if (>= ln 3) (- (cdaddr pl) y) 0)))
    (list (make-item #:coord x y)
          (make-item #:size w h 0))))
;;=====================================================================
(define (update-pref-text lst)
  (and-let*
    (((eq? (car lst) #:text))
     (str (assv-ref lst #:string))
     ((string? str))
     (pl (ext-poly lst))
     (ln (length pl))
     (x (if (>= ln 1) (caar pl) 0))
     (y (if (>= ln 1) (cdar pl) 0))
     (a (if (>= ln 2) (calc-a (car pl) (cadr pl)) 0))
     (l (string-length str))
     (i 0))
    (while (and (< i l) (not (char-numeric? (string-ref str i))))
      (set! i (inc i)))
    (list (make-item #:coord x y)
          (make-item #:string 0 0 a (substring str i)))))
;;=====================================================================
(define (update-text lst)
  (and-let*
    (((eq? (car lst) #:text))
     (str (assv-ref lst #:string))
     ((string? str))
     (pl (ext-poly lst))
     (ln (length pl))
     (x (if (>= ln 1) (caar pl) 0))
     (y (if (>= ln 1) (cdar pl) 0))
     (a (if (>= ln 2) (calc-a (car pl) (cadr pl)) 0)))
    (list #:text
          (make-item #:coord x y)
          (make-item #:string 0 0 a str))))
;;======================================================================
