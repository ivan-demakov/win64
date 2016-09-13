;;====================================================================
(define edition-color "#00f0f0")
(define stabil-color  "#404040")
;;====================================================================
(define Pi 3.14159265358979323846)
(define deg/rad (/ 1800.0 Pi))
(define (deg->rad deg) (/ deg deg/rad))
(define (rad->deg rad) (inexact->exact (* rad deg/rad)))
;;====================================================================
(define (inc v) (+ v 1))
(define (dec v) (- v 1))
;;====================================================================
(define (xor? a b) (or (and a (not b)) (and b (not a))))
;;====================================================================
(define (good-angle a)
  (cond ((> a 900) (- a 1800)) ((< a -900) (+ a 1800)) (else a)))
;;====================================================================
(define (std-angle a) (modulo (+ (modulo a 3600) 3600) 3600))
;;====================================================================
(define (remove-kwasi-cursor str)
  (let*
    ((cp (and (string? str) (string-index str kwasi-cursor-char))))
    (if cp
      (string-append (substring str 0 cp) (substring str (inc cp)))
      str)))
;;=====================================================================
(define (poly-length apl)
  (define (dist p0 p1)
    (magnitude (make-rectangular (- (car p1) (car p0)) (- (cdr p1) (cdr p0))))  )
  (if (< (length apl) 2) 0 (apply + (map dist apl (cdr apl)))))
;;====================================================================
(define (poly-length->str apl)
  (number->string (/ (inexact->exact (/ (+ 5 (poly-length apl)) 10)) 10)))
;;====================================================================
(define (sum-str-len font angle . sl)
  (let*
    ((sum 0)
     (add (lambda (x) (set! sum (+ sum (calc-string-width x font angle))))))
    (for-each add sl)
    sum))
;;====================================================================
(define (plg-point-num num plg)
  (let
    ((l (length plg)))
    (modulo (+ (modulo num l) l) l)))
;;====================================================================
(define (point coord-list num)
  (list-ref coord-list (plg-point-num (dec num) coord-list)))
;;====================================================================
(define (get-imag z)
  (if (real? z)
    0
    (imag-part z)))
;;====================================================================
(define (turn p a)
  (if (zero? a)
    (cons (inexact->exact (car p)) (inexact->exact (cdr p)))
    (let*
      ((zz (* (make-rectangular (car p) (cdr p)) (make-polar 1 (deg->rad (- a))))))
      (cons (inexact->exact (real-part zz)) (inexact->exact (get-imag zz))))))
;;====================================================================
(define (turn-point x0 y0 xc yc a)
  (shift (cons xc yc) (turn (cons (- x0 xc) (- y0 yc)) a)))
;;====================================================================
(define (turn-points lst p0 a)
  (map (lambda (p) (shift p0 (turn p a))) lst))
;;====================================================================
(define (near? x0 y0 x1 y1 radius)
  (<= (distance x0 y0 x1 y1) radius))
;;====================================================================
(define (valid-type? type? x)
  (or (and (pair? x) (symbol? (car x)) (type? (cdr x)))
      (type? x)))

(define (procedure-OR-symbol? x) (or (procedure? x) (symbol? x)))

(define (procedure-OR-boolean? x) (valid-type? (lambda (x) (or (procedure? x) (boolean? x))) x))

(define (fill-style? x) (memq x '(#:solid #:horizontal #:vertical #:fdiagonal
                                  #:bdiagonal #:cross #:diagcross)))

(define (line-style? x) (memq x '(#:solid #:dash #:dot #:dash-dot #:dash-dot-dot)))

(define (procedure-OR-symbol-OR-fill-style? x)
  (valid-type? (lambda(x)(or (fill-style? x) (procedure-OR-symbol? x))) x))

(define (procedure-OR-symbol-OR-line-style? x)
  (valid-type? (lambda(x)(or (line-style? x) (procedure-OR-symbol-OR-integer? x))) x))

(define (procedure-OR-symbol-OR-string? x)
  (valid-type? (lambda(x)(or (string? x) (procedure-OR-symbol? x))) x))

(define (procedure-OR-symbol-OR-integer? x)
  (valid-type? (lambda(x)(or (integer? x) (procedure-OR-symbol? x))) x))

(define (procedure-OR-integer? x)
  (valid-type? (lambda(x)(or (integer? x) (procedure? x))) x))

(define (procedure-OR-symbol-OR-list? x)
  (valid-type? (lambda(x)(or (list? x) (procedure-OR-symbol? x))) x))

(define (procedure-OR-symbol-OR-vector? x)
  (valid-type? (lambda(x)(or (vector? x) (procedure-OR-symbol? x))) x))

(define (pair-of-int? x)
  (and (pair? x) (integer? (car x)) (integer? (cdr x))))
;;====================================================================
(define (valid-num? x)
  (define rools '((s0 char-numeric? s1)
                  (s1 char-numeric? s1 s2)
                  (s2 #\. s4 s3)
                  (s3 #\, s4 s6)
                  (s4 char-numeric? s5)
                  (s5 char-numeric? s5 s6)
                  (s6)))
  (parse-string x rools "Параметр - не число"))
;;====================================================================
(define (valid-chan-sum? x)
  (define rools '((s0 char-numeric? s1)
                  (s1 char-numeric? s1 s2)
                  (s2 #\+ s0 s3)
                  (s3)))
  (parse-string x rools "Параметр - не сумма каналов"))
;;====================================================================
(define (kvant coord step offset)
  (if (= step 1)
    coord
    (begin
      (set! coord (+ coord (/ (if (odd? step) (dec step) step) 2)))
      (- coord (remainder (- coord offset) step)))))
;;====================================================================
(define (range xmin x xmax) (min xmax (max xmin x)))
;;====================================================================
(define (distance x0 y0 x1 y1)
  (inexact->exact (magnitude (make-rectangular (- x1 x0) (- y1 y0)))))
;;====================================================================
(define (dist p0 p1)
  (distance (car p0) (cdr p0) (car p1) (cdr p1)))
;;====================================================================
(define (make-z p0 p1)
  (make-rectangular (- (car p1) (car p0)) (- (cdr p1) (cdr p0))))
;;=====================================================================
(define (scale-dist p0 p1)
  (inexact->exact (/ (magnitude (make-z p0 p1)) obj-scale-factor)))
;;====================================================================
(define (dist-scale p0 p1)
  (inexact->exact (* (magnitude (make-z p0 p1)) obj-scale-factor)))
;;====================================================================
(define (calc-a p0 p1)
  (if (equal? p0 p1)
    0
    (- (rad->deg (angle (make-z p0 p1))))))
;;=====================================================================
(define (dist-to-line x y p0 p1)
  (let*
    ((z1 (make-z p0 p1))
     (z2 (make-z p0 (cons x y)))
     (z0 (/ z2 z1)))
    (inexact->exact (* (magnitude z1) (get-imag z0)))))
;;====================================================================
(define (dist-to-segment x y p0 p1)
  (if (equal? p0 p1)
    (distance x y (car p0) (cdr p0))
    (and-let*
      ((z1 (make-z p0 p1))
       (m (magnitude z1))
       (z0 (/ (make-z p0 (cons x y)) z1))
       (x (real-part z0))
       ((<= 0 x))
       ((<= x 1)))
      (inexact->exact (abs (* m (get-imag z0)))))))
;;====================================================================
(define (last-point pll)
  (car (last-pair pll)))
;;====================================================================
(define (unbound pll)
  (if (equal? (car pll) (last-point pll))
    (cdr pll)
    pll))
;;=====================================================================
(define (enbound bound? pl)
  (if (or (< (length pl) 3) (not bound?) (equal? (car pl) (last-point pl)))
    pl
    (cons (last-point pl) pl)))
;;=====================================================================
(define (select-segment x y pll bound?)
  (let
    ((sn 0)
     (sd #f))
    (let loop ((n 0) (pl (enbound bound? pll)))
      (if (< (length pl) 2)
        sn
        (let*
          ((d (dist-to-segment x y (car pl) (cadr pl))))
          (when (and d (or (not sd) (< d sd)))
            (set! sn n)
            (set! sd d))
          (loop (inc n) (cdr pl)))))))
;;====================================================================
(define (point-on-dist pll d)
  (define (dist p0 p1)
    (magnitude (make-rectangular (- (car p1) (car p0)) (- (cdr p1) (cdr p0)))))
  (define  (rat a b p) (inexact->exact (+ a (* p (- b a)))))
  (let loop ((pl pll) (ds d))
    (and-let*
      (((not (null? (cdr pll))))
       (p0 (car pl))
       (p1 (cadr pl))
       (l (dist p0 p1)))
      (if (> ds l)
        (loop  (cdr pl) (- ds l))
        (let*
          ((a (calc-a p0 p1))
           (r (/ ds l))
           (p (cons (rat (car p0) (car p1) r)  (rat (cdr p0) (cdr p1) r))))
          (values p a)
        )))))
;;====================================================================
(define (line-on-dist pll d0 d1)
  (receive (p0 a) (point-on-dist pll d0)
    (receive (p1 b) (point-on-dist pll d1)
      (let*
        ((n0 (select-segment (car p0) (cdr p0) pll #f))
         (n1 (select-segment (car p1) (cdr p1) pll #f)))
        (append
          (list p0)
          (list-head (list-tail pll (inc n0)) (- n1 n0))
          (list p1)
        )))))
;;====================================================================
(define (offset coord1 coord2)
  (map - coord2 coord1))
;;====================================================================
(define (shift coord1 coord2)
  (map + coord2 coord1))
;;====================================================================
(define (abs->rel coord)
  (if (null? coord)
    coord
    (cons (car coord) (map offset coord (cdr coord)))))
;;====================================================================
;;====================================================================
(define (rel->abs coord)
  (if (null? coord)
    coord
    (let loop ((rel (cdr coord)) (abs (list (car coord))))
      (if (null? rel)
        (reverse abs)
        (loop (cdr rel) (cons (shift (car abs) (car rel)) abs))))))
;;====================================================================
(define (get-all-objects point type-list)
  (let
    ((be building-env)
     (el (get-all-of-class point type-list)))
    (set! building-env be)
    (foldr
      (lambda (e l) (if (member? e l) l (cons e l)))
      '()
      el)))
;;=====================================================================
(define (remove-old lst . space )
  (for-each (lambda (x) (remove-all-of-class x space)) lst))
;;=====================================================================
(define (text-visible? str)
  (and
    str
    (positive? (string-length str))
    (not (every (lambda (l) (char=? l #\space)) (string->list str)))))
;;=====================================================================
(define (make-footnote x0 y0 x1 y1 ang str fsz limit . expar)
  (let*
    ((d (* fsz 1/5))
     (stl (calc-string-width (remove-kwasi-cursor str) (vector 0 fsz) ang))
     (lim (or limit (* 4 fsz)))
     (p0  (unscale-coord (cons x0 y0)))
     (p1  (unscale-coord (cons x1 y1)))
     (x0 (car p0))
     (y0 (cdr p0))
     (x1 (car p1))
     (y1 (cdr p1))
     (p2 (turn-point x1 (+ y1 d) x1 y1 ang))
     (x2 (car p2))
     (y2 (cdr p2))
     (p3 (turn-point (+ x1 stl) (+ y1 d) x1 y1 ang))
     (x3 (car p3))
     (y3 (cdr p3))
     (direct? (or (not (null? expar)) (< (distance x0 y0 x2 y2) (distance x0 y0 x3 y3))))
     (xp1 (if direct? x2 x3))
     (yp1 (if direct? y2 y3))
     (xp2 (if direct? x3 x2))
     (yp2 (if direct? y3 y2)))
    (if (< (distance x0 y0 xp1 yp1) lim)
      '()
      (list (cons x0 y0)
            (cons xp1 yp1)
            (cons xp2 yp2)))))
;;=====================================================================
(define (make-pll-footnote pll x1 y1 ang str fsz limit)
  (define max-delta 400)

  (define (mdlp p0 p1)
    (map (lambda (x0 x1) (/ (+ x0 x1) 2)) p0 p1))

  (define (shft p0 p1 d)
    (and-let*
       ((dst (dist p0 p1))
        ((> dst (+ d d)))
        (sh (lambda (x0 x1) (+ x0 (/ (* d (- x1 x0)) dst)))))
       (map sh p0 p1)))

  (case (length pll)
    ((0) '())
    ((1) (make-footnote (caar pll) (cdar pll) x1 y1 ang str fsz limit))
    (else
      (let*
        ((stl (calc-string-width (remove-kwasi-cursor str) (vector 0 fsz) ang))
         (p2 (turn-point (+ x1 stl) y1 x1 y1 ang))
         (x2 (car p2))
         (y2 (cdr p2))
         (xm (/ (+ x1 x2) 2))
         (ym (/ (+ y1 y2) 2)))
        (let loop ((ds -1) (d #f) (p #f) (pl pll))
          (if (null? pl)
            (if (and p (or (negative? ds) (>= ds (or limit (* 4 fsz)))))
              (make-footnote (car p) (cdr p) x1 y1 ang str fsz limit)
              '())
            (or
              (and-let*
                ((> (length pl) 1)
                 (t0 (car pl))
                 (t1 (cadr pl))
                 (p0 (if (eq? pl pll)      (or (shft t0 t1 max-delta) (mdlp t0 t1)) t0))
                 (p1 (if (= (length pl) 2) (or (shft t1 t0 max-delta) (mdlp t1 t0)) t1))
                 (td0 (distance (car p0) (cdr p0) x1 y1))
                 (td1 (distance (car p1) (cdr p1) x1 y1))
                 (tdp (min td0 td1))
                 (tds (or (dist-to-segment x1 y1 p0 p1) tdp))
                 (dsg (or (dist-to-segment x1 y1 t0 t1)
                          (dist-to-segment x2 y2 t0 t1)
                          (dist-to-segment xm ym t0 t1)
                          -1))
                 ((or (not d) (< tds d)))
                 (a (calc-a p0 p1))
                 (p2 (turn-point x1 y1 (car p0) (cdr p0) (- a)))
                 (p3 (if (< td0 td1) p0 p1))
                 (p4 (turn-point (car p2) (cdr p0) (car p0) (cdr p0) a)))
                (loop dsg tds (if (< tds tdp) p4 p3) (cdr pl)))
              (loop ds d p (cdr pl)))))))))
;;=====================================================================
(define max-delta 25)
;;=====================================================================
(define (calc-n-pnt r l)
  (if (> max-delta r)
    2
    (inexact->exact (/ l (acos (- 1.0 (/ max-delta r)))))))
;;=====================================================================
(define (make-rounded pl)
  (make-ring pl #f))
;;=====================================================================
(define (make-rectangulated pl)
  (if (< (length pl) 3)
    pl
    (let*
      ((p0 (car pl))
       (p1 (cadr pl))
       (p2 (caddr pl)))
      (list p0 p1 p2 (shift p2 (offset p1 p0)) p0))))
;;=====================================================================
(define (make-squared pl)
  (define (shift-point p0 p1 p2 m0 m1)
    (cons
      (+ (car p2) (inexact->exact (/ (* (- (car p1) (car p0)) m0) m1)))
      (+ (cdr p2) (inexact->exact (/ (* (- (cdr p1) (cdr p0)) m0) m1)))))

  (if (< (length pl) 3)
    pl
    (let*
      ((p0 (car pl))
       (p1 (cadr pl))
       (p2 (caddr pl))
       (d0 (dist p0 p1))
       (d1 (dist p1 p2))
       (dm (max d0 d1))
       (pm1 (shift-point p0 p1 p0 dm d0))
       (pm2 (shift-point p1 p2 pm1 dm d1)))
      (list p0 pm1 pm2 (shift pm2 (offset pm1 p0)) p0))))
;;=====================================================================
(define (make-splined-soft pl)
  (make-splined pl 0))
;;=====================================================================
(define (make-splined-hard pl)
  (make-splined pl 1))
;;=====================================================================
(define (make-splined-midl pl)
  (make-splined pl 1/2))
;;=====================================================================
(define (make-copy pl)
  (or (and-let*
        ((l2 (- (length pl) 2))
         ((>= l2 0))
         (t2 (list-tail pl l2))
         (ll (make-trace (car t2) (cadr t2))))
        (append (list-head pl l2) ll))
      pl))
;;=====================================================================
(define (shape-points shn)
  (define len-vec '#(65536 65536 65536 65536 3 3 2 3 3 3 2))
  (vector-ref len-vec shn))
;;=====================================================================
(define (make-shape pl shl)
  (define proc-vec (vector make-copy
                           make-splined-soft
                           make-splined-midl
                           make-splined-hard
                           make-rectangulated
                           make-squared
                           make-rounded
                           make-ellipsed
                           make-ovaled
                           make-arced
                           make-sota))

  (define (mk-shape shn rl pl)
    (append (if (null? rl) rl (list-head rl (dec (length rl))))
            ((vector-ref proc-vec shn) (if (null? rl) pl (append (last-pair rl) pl)))))

  (cond
    ((not pl) '())
    ((null? pl) '())
    (else (let loop ((rl '()) (pl pl) (shl shl) (l 0) (shn (car shl)))
            (cond
              ((null? shl)
               (mk-shape shn rl pl))
              ((or (not (= shn (car shl)))
                   (= (shape-points shn) (+ l (if (null? rl) 0 1))))
               (loop (mk-shape shn rl (list-head pl l)) (list-tail pl l) shl 0 (car shl)))
              (else
               (loop rl pl (cdr shl) (inc l) shn)))))))
;;=====================================================================
(define (improve-round pll bound?)
  (and-let*
    ((n (length pll))
     ((> n 7))
     (pll (enbound bound? pll))
     (p0 (car pll))
     (pn (last-point pll))
     ((equal? p0 pn))
     (xl (map car pll))
     (yl (map cdr pll))
     (l (apply min xl))
     (r (apply max xl))
     (t (apply min yl))
     (b (apply max yl)))
    (let*
      ((c (cons (quotient (+ l r) 2) (quotient (+ t b) 2)))
       (rd (quotient (+ (- r l) (- b t)) 4))
       (s (foldr
         (lambda (x s) (+ (abs (- rd (dist c x))) s))
         0 pll))
       (e (/ s n)))
      (and-let*
        (((< e (/ rd 20)))
         (r (make-rounded (list c (shift c (cons rd 0))))))
        (if bound?
          (unbound r)
          r)))))
;;=====================================================================
(define (improve-polyline pll bound? test-round? test-rect?)
  (if (< (length pll) 4)
    pll
    (let*
      ((ocw? (and bound? (positive? (clockwise? pll))))
       (ltl? (<= (length pll) 4096))
       (npl (cond ((< (length pll) 4) pll)
                  ((and test-round? ltl? (improve-round pll bound?)))
                  (else (improve-lined pll bound? (and ltl? test-rect?)))))
       (ncw? (and bound? (positive? (clockwise? npl)))))
      (if (or (not bound?) (not (eq? ocw? ncw?)))
        (reverse npl)
        npl))))
;;=====================================================================
(define (improve-rectanguled pll)
  (define (make-da p0 p1 p2)
    (let
      ((a (rad->deg (angle (/ (make-z p1 p0) (make-z p1 p2))))))
      (abs (- 900 (if (positive? a) a (+ a 1800))))))

  (or
    (and-let*
      ((n (length pll))
       ((>= n 4))
       ((even? n))
       (fpl (append pll (list-head pll 2)))
       (d-map (map make-da fpl (cdr fpl) (cddr fpl)))
       (every (lambda (d) (<= d 50)) d-map)
       (any (lambda (d) (> d 5)) d-map)
       (a (calc-a (car pll) (cadr pll)))
       (pl (map (lambda (p) (turn (offset (car pll) p) (- a)))
                (cons (last-point pll) pll))))
      (let loop ((pl pl))
        (when (> (length pl) 1)
          (if (> (abs (- (caar pl) (caadr pl)))
                 (abs (- (cdar pl) (cdadr pl))))
            (set-cdr! (cadr pl) (cdar pl))
            (set-car! (cadr pl) (caar pl)))
          (loop (cdr pl))))
      (map (lambda (p) (shift (turn p a) (car pll))) (cdr pl)))
    pll))
;;=====================================================================
(define (improve-lined pll bound? test-rect?)
  (define (near? p p0 p1)
    (< (abs (dist-to-line (car p) (cdr p) p0 p1)) max-delta))

  (define (find-next fpt pll)
    (let loop ((i 0) (lpt (cdr pll)))
      (cond
        ((null? lpt) i)
        ((member? (car lpt) fpt) i)
        ((member? (car lpt) (cdr lpt)) i)
        ((every (lambda (p) (near? p (car fpt) (car lpt))) (list-head pll (inc i)))
         (loop (inc i) (cdr lpt)))
        (else i))))

  (define (last-try pll)
    (let loop ((l (length pll)) (pll pll))
      (cond
        ((< l 3)
          pll)
        ((near? (car pll) (cadr pll) (list-ref pll (dec l)))
          (loop (dec l) (cdr pll)))
        ((near? (list-ref pll (dec l)) (car pll) (list-ref pll (- l 2)))
          (loop (dec l) (list-head pll (dec l))))
        (else pll))))

  (let loop ((npl (list (car pll))) (opl (cdr pll)))
    (if (null? opl)
      (begin
        (cond
          ((not bound?) npl)
          (test-rect? (improve-rectanguled (last-try npl)))
          (else (last-try npl))))
      (let*
        ((n (find-next npl opl))
         (nxt (list-tail opl n)))
        (loop (cons (car nxt) npl) (cdr nxt))))))
;;=====================================================================
(define (clockwise? pll)
  (or
    (and-let*
      ((pll (unbound pll))
       (n (length pll))
       ((> n 2)))
      (while (equal? (car pll) (caddr pll))
        (set! pll (append (cdr pll) (list-head pll 1))))
      (let
        ((p0 (point pll (dec n)))
         (p1 (point pll n))
         (s 0)
         (d 1))
        (for-each
          (lambda (p2)
            (let
              ((a (angle (/ (make-z p1 p2) (make-z p0 p1)))))
              (if (< (- Pi 1e-6) a (+ Pi 1e-6))
                (set! a (* a d -1)))
              (set! d (if (negative? a) -1 1))
              (set! s (+ s a))
              (set! p0 p1)
              (set! p1 p2)))
          pll)
        (cond ((< s -0.1) -1)
              ((> s  0.1)  1)
              (else 0))))
    1))
;;=====================================================================
(define (improve-coords obj)
  (if (list? obj)
    (cond
      ((eq? (car obj) #:polyline)
       (set-cdr! (cadr obj) (improve-polyline (cdadr obj) #f #f #f)))
      ((or (eq? (car obj) #:polygon) (eq? (car obj) #:smart-polygon))
       (set-cdr! (cadr obj) (improve-polyline (cdadr obj) #t #f #f)))
      (else (for-each improve-coords obj)))))
;;=====================================================================
(define (list-select lst prd)
  (foldr (lambda (e l) (if (prd e) (cons e l) l)) '() lst))
;;=====================================================================
(define (list-insert lst elm prd?)
  (let loop ((hd '()) (tl lst))
    (if (or (null? tl) (prd? elm (car tl)))
      (append! hd (cons elm tl))
      (loop (append! hd (list-head tl 1)) (cdr tl)))))
;;=====================================================================
(define (list-sort lst prd?)
  (foldl (lambda (e l) (list-insert l e prd?)) '() lst))
;;=====================================================================
(define (rotate-list lst n)
  (append (list-tail lst n) (list-head lst n)))
;;====================================================================
(define (make-unselfcross-poly pll)

  (define (mumu pll)
    (if (> (length pll) 5)
      (let loop ((rl '()) (pl pll))
        (if (< (length pl) 2)
            (append rl pl)
          (or
            (and-let*
              ((pp (car pl))
               (ml (member pp (reverse (cdr pl))))
               (n (length ml))
               ((> n 2))
               (k (length rl))
               ((> (+ k (- (length pl) n 1)) 2))
               (i (mumu (cdr (list-head pl n))))
               (o (mumu (append rl (list-tail pl (inc n)))))
               (tst? (if (point-in-polygon? (car i) o) negative? positive?)))
              (append (list-head o k)
                      (cons pp (if (tst? (* (clockwise? i) (clockwise? o))) i (reverse i)))
                      (cons pp (list-tail o k))))
            (loop (append rl (list-head pl 1)) (cdr pl)))))
      pll))

  (put-hint "Удаление самопересечений - пожалуйста, подождите... ")
  (if (> (length pll) 4096)
    pll
    (let loop1 ((rl '()) (pl (enbound #t (mumu pll))))
       (if (< (length pl) 4)
         (unbound (append! rl pl))
         (let loop2 ((tl (cddr pl)) (nl 2))
           (if (< (length tl) 2)
              (loop1 (append rl (list-head pl 1)) (cdr pl))
              (or (and-let*
                    ((cp (cross? (car pl) (cadr pl) (car tl) (cadr tl) #f)))
                    (loop1 rl (append (list (car pl) cp)
                                      (reverse (list-head (cdr pl) nl))
                                      (cons cp (cdr tl)))))
                  (loop2 (cdr tl) (inc nl)))))))))
;=====================================================================
(define (trim-spaces str)
  (let loop
    ((lst (string->list str))
     (rst '()))
    (cond
      ((null? lst) (list->string (reverse rst)))
      ((and (null? (cdr lst)) (char-whitespace? (car lst)) (list->string (reverse rst))))
      ((not (char-whitespace? (car lst))) (loop (cdr lst) (cons (car lst) rst)))
      ((null? rst) (loop (cdr lst) rst))
;;      ((and (not (null? (cdr lst))) (char-whitespace? (cadr lst))) (loop (cdr lst) rst))
      (else (loop (cdr lst) (cons (car lst) rst))))))
;=====================================================================
(define (step-by-line p0 p1 d)
  (let*
    ((n2 (/ d (dist p0 p1)))
     (n1 (- 1.0 n2)))
    (cons (inexact->exact (+ (* (car p0) n1) (* (car p1) n2)))
          (inexact->exact (+ (* (cdr p0) n1) (* (cdr p1) n2))))))
;=====================================================================
(define (make-line-with-text pl line-type line-width
                             tx font-size interval)

   (define (make-poly pl)
     (make <Color-Polyline>
       #:color owner->color
       #:line  line-type
       #:width line-width
       #:coord-list pl))

   (define (cut-poly pl d)

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


   (if (null? pl)
     '()
     (let*
       ((num (string-append " " (or tx "") " "))
        (apl (mk-trace pl))
        (sum (inexact->exact (poly-length apl)))
        (font (vector 0 (- font-size)))
        (ssw (* 12/10 (if tx (calc-string-width num font 0) 0)))
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
                 #:color 0
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
;=====================================================================
(define (scale-size s)
  (if (number? s)
    (set! s (cons s s)))
  (cons (inexact->exact (* (car s) obj-scale-factor))
        (inexact->exact (* (cdr s) obj-scale-factor))))

(define (scale-point p x0 y0)
  (cons (inexact->exact (+ x0  (* (- (car p) x0) obj-scale-factor)))
        (inexact->exact (+ y0  (* (- (cdr p) y0) obj-scale-factor)))))

(define (unscale-point p x0 y0)
  (cons (inexact->exact (+ x0  (/ (- (car p) x0) obj-scale-factor)))
        (inexact->exact (+ y0  (/ (- (cdr p) y0) obj-scale-factor)))))

(define (scale-coord p)
  (or (and-let*
        ((x0 (get-x0))
         (y0 (get-y0))
         ((not (= obj-scale-factor 1))))
        (scale-point p x0 y0))
       p))

(define (unscale-coord p)
  (or (and-let*
        ((x0 (get-x0))
         (y0 (get-y0))
         ((not (= obj-scale-factor 1))))
        (unscale-point p x0 y0))
       p))

(define (scale-poly pl)
  (or (and-let*
        ((x0 (get-x0))
         (y0 (get-y0))
         ((not (= obj-scale-factor 1))))
        (map (lambda (p) (scale-point p x0 y0)) pl))
       pl))

(define (unscale-poly pl)
  (or (and-let*
        ((x0 (get-x0))
         (y0 (get-y0))
         ((not (= obj-scale-factor 1))))
        (map (lambda (p) (unscale-point p x0 y0)) pl))
       pl))
;=====================================================================
(define (lim-txy lim)
  (set! lim (* obj-scale-factor (if (null? lim) 10000 (car lim))))
  (let*
    ((x0 (get-x0))
     (y0 (get-y0))
     (tx (get-tx))
     (ty (get-ty))
     (d (if (and x0 tx) (distance x0 y0 tx ty) 0)))
    (when (> d lim)
      (set! tx (+ x0 (/ (* (- tx x0) lim) d)))
      (set! ty (+ y0 (/ (* (- ty y0) lim) d)))
      (set-property! 'tx tx)
      (set-property! 'ty ty))
    (cons tx ty)))

(define (lim-tx . lim) (car (lim-txy lim)))
(define (lim-ty . lim) (cdr (lim-txy lim)))
;;=====================================================================
(define (calc-num-coord pll str)
  (let*
    ((cp (find-inner-point pll)))
    (vector
      (inexact->exact (- (car cp) (calc-string-width str (h-std-font) 0)))
      (inexact->exact (+ (cdr cp) (/ (scale-font) 2))))))
;;=====================================================================
(define (calc-x x0 y0 dx dy a)
  (car (turn-point (+ x0 dx) (+ y0 dy) x0 y0 a)))
;;=====================================================================
(define (calc-y x0 y0 dx dy a)
  (cdr (turn-point (+ x0 dx) (+ y0 dy) x0 y0 a)))
;;=====================================================================
