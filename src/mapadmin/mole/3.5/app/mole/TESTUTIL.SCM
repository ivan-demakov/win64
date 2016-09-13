;;;
;;; object binding test routines
;;;
;;=====================================================================
(define (point-in-rect? x y x0 y0 w h a)
  (let*
    ((p (turn-point x y x0 y0 (- a)))
     (x (- (car p) x0))
     (y (- (cdr p) y0)))
    (and (positive? (* x w))
         (positive? (* y h))
         (< (abs x) (abs w))
         (< (abs y) (abs h)))))
;;=====================================================================
(define (point-on-line? x y p0 p1 . lim)
  (set! lim (if (null? lim) 0 (car lim)))
  (or (near? x y (car p0) (cdr p0) lim)
      (near? x y (car p1) (cdr p1) lim)
      (let
        ((d (dist-to-segment x y p0 p1)))
        (and d (<= d lim)))))
;;=====================================================================
(define (point-on-polyline? p pll . lim)
  (any
    (lambda (p0 p1)
      (apply point-on-line? (append (list (car p) (cdr p) p0 p1) lim)))
    pll
    (cdr pll)))
;;=====================================================================
(define (point-on-polygon? p pll . lim)
  (apply point-on-polyline? (append (list p (enbound #t pll)) lim)))
;;=====================================================================
(define (crs-point p00 p01 p10 p11)
  (and-let*
    ((z0 (make-z p00 p01))
     (z1 (/ (make-z p00 p10) z0))
     (z2 (/ (make-z p00 p11) z0))
     (x1 (real-part z1))
     (y1 (get-imag z1))
     (x2 (real-part z2))
     (y2 (get-imag z2))
     (x (cond
          ((not (= y1 y2)) (- x1 (/ (* y1 (- x2 x1)) (- y2 y1))))
          (else 0)))
     (zz (* z0 x))
     (cp (shift p00 (cons (inexact->exact (real-part zz))
                          (inexact->exact (get-imag zz))))))
    cp))
;;=====================================================================
(define (cross-point p00 p01 p10 p11)
  (and-let*
    ((z0 (make-z p00 p01))
     (m (/ 1 (magnitude z0)))
     (-m (- m))
     (z1 (/ (make-z p00 p10) z0))
     (z2 (/ (make-z p00 p11) z0))
     (x1 (real-part z1))
     (y1 (get-imag z1))
     (x2 (real-part z2))
     (y2 (get-imag z2))
     (x (cond
          ((not (= y1 y2)) (- x1 (/ (* y1 (- x2 x1)) (- y2 y1))))
          ((and (< -m x1) (< x1 (+ 1 m))) x1)
          ((and (< -m x2) (< x2 (+ 1 m))) x2)
          (else #f)))
     (zz (* z0 x))
     (cp (shift p00 (cons (inexact->exact (real-part zz))
                          (inexact->exact (get-imag zz))))))
    cp))
;;=====================================================================
(define (cross? p00 p01 p10 p11 touch?)
  (or
    (and touch?
         (or (and (equal? p00 p10) p00)
             (and (equal? p00 p11) p00)
             (and (equal? p01 p10) p01)
             (and (equal? p01 p11) p01)))
    (and-let*
      (((>= (max (car p00) (car p01)) (min (car p10) (car p11))))
       ((>= (max (car p10) (car p11)) (min (car p00) (car p01))))
       ((>= (max (cdr p00) (cdr p01)) (min (cdr p10) (cdr p11))))
       ((>= (max (cdr p10) (cdr p11)) (min (cdr p00) (cdr p01))))
       (z0 (make-z p00 p01))
       (m (/ 1 (magnitude z0)))
       (-m (- m))
       (z1 (/ (make-z p00 p10) z0))
       (z2 (/ (make-z p00 p11) z0))
       (x1 (real-part z1))
       (y1 (get-imag z1))
       (x2 (real-part z2))
       (y2 (get-imag z2))
       ((not (and (> y1  m) (> y2  m))))
       ((not (and (< y1 -m) (< y2 -m))))
       (x (cond
            ((not (= y1 y2)) (- x1 (/ (* y1 (- x2 x1)) (- y2 y1))))
            ((not touch?) #f)
            ((and (< -m x1) (< x1 (+ 1 m))) x1)
            ((and (< -m x2) (< x2 (+ 1 m))) x2)
            (else #f)))
       ((and (< -m x) (< x (+ 1 m))))
       (zz (* z0 x))
       (cp (shift p00 (cons (inexact->exact (real-part zz))
                            (inexact->exact (get-imag zz)))))
       ((or touch? (not (or (equal? cp p00) (equal? cp p01)
                            (equal? cp p10) (equal? cp p11))))))
      cp)))
;;=====================================================================
(define (point-in-polygon? pt plg)
  (and-let*
    (((not (point-on-polygon? pt plg)))
     (x (car pt))
     (y (cdr pt))
     (s 0)
     (n (length plg))
     (p0 #t)
     (p1 (point plg (dec n)))
     (p2 (point plg n)))
    (dotimes (i n (odd? s))
      (set! p0 p1)
      (set! p1 p2)
      (set! p2 (point plg (inc i)))
      (while (and (< i n) (= (car p1) (car p2)))
        (set! p1 p2)
        (set! i (inc i))
        (set! p2 (point plg (inc i))))
      (and-let*
        ((< i n)
         (y1 (cdr p1))
         (y2 (cdr p2))
         ((<= y (max y1 y2)))
         (x0 (car p0))
         (x1 (car p1))
         (x2 (car p2))
         ((or (and (< x x2) (or (< x1 x) (and (= x1 x) (< x0 x))))
              (and (> x x2) (or (> x1 x) (and (= x1 x) (> x0 x))))))
         ((> (/ (* (- y2 y1) (- x x1)) (- x2 x1)) (- y y1))))
        (set! s (inc s))))))
;;=====================================================================
;;
;; End of code
;;
