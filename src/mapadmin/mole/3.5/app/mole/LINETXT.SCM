;;=====================================================================
(define-class <Text-By-Line> (<Color>)
  ((font     #:type procedure-OR-symbol-OR-vector?
             #:initform '#(0 -16)
             #:initarg #:font
             #:getter 'font-of)
  (item-list #:type procedure-OR-symbol-OR-list?
             #:initform '()
             #:initarg #:item-list
             #:getter 'item-list-of)))

(define-method draw (dc (self <Text-By-Line>))
  (define (select vec i val)
    (if (> (vector-length vec) i) (vector-ref vec i) val))

  (and-let*
    (((call-next-method))
     (font (get-property (font-of self)))
     (ff (vector-ref font 0))
     (font-face (num->font-face (if (integer? ff) ff 0)))
     (font-size (* obj-scale-factor (vector-ref font 1)))
     (font-size (inexact->exact (* obj-scale-factor (vector-ref font 1))))
     (font-weight  (select font 2 "*"))
     (font-style   (select font 3 "*"))
     (font-charset (select font 4 "Russian"))
     (items (get-property (item-list-of self)))
     (fnt (make-font font-face font-weight font-style font-charset)))
    (for-each
      (lambda (item)
        (if (eq? (vector-ref item 2) kwasi-cursor-char)
          (set-text-cursor (abs font-size) (vector-ref item 0) (vector-ref item 1))
          (when (changed? self)
            (set-font! dc fnt font-size (vector-ref item 1))
            (draw-string dc (vector-ref item 0) (string (vector-ref item 2))))))
      items)))

(define-method set-text-edit-point! ((self <Text-By-Line>))
  (define (di item d)
    (let*
      ((font (get-property (font-of self)))
       (fs (* .55 (abs (vector-ref font 1))))
       (p0 (vector-ref item 0))
       (a (deg->rad (vector-ref item 1)))
       (dx (* fs (cos a)))
       (dy (* fs (- (sin a))))
       (p1 (shift p0 (cons dx dy))))
      (if (< (dist curr-mouse p1) d) 1 0)))

  (let*
    ((items (get-property (item-list-of self)))
     (n (dec (length items)))
     (d #f)
     (i 0))
     (for-each
       (lambda (item)
         (and-let*
           ((t (dist curr-mouse (vector-ref item 0)))
           ((or (not d) (> d t))))
           (set! text-edit-point (+ i (if (= i n) (di item t) 0)))
           (set! d t))
         (set! i (inc i)))
       items)))

(define-method unpack ((self <Text-By-Line>) (src <List>))
  (and (or is-convertion? (eqv? (car src) #:Text-By-Line))
       (call-next-method)))

(define-method pack ((self <Text-By-Line>))
  (and-let*
    ((font (get-property (font-of self)))
     (item (get-property (item-list-of self)))
     (nm (call-next-method)))
    (cons
      #:Text-By-Line
      (cons  (cons #:font font)
             (cons (cons #:txt-item item) nm)))))
;;=====================================================================
(define (make-item-list pll str font)

  (define (make-len-list str)
    (let
      ((ep (string-index str kwasi-cursor-char))
       (lst (calc-string-width (remove-kwasi-cursor str) font 0 #t)))
      (map (lambda (x) (* 12/10 x))
        (if ep
          (append (list-head lst ep)
                  (list (list-ref lst ep))
                  (list-tail lst ep))
          lst))))

  (define (make-find-point dsm pll)
    (lambda (d)
      (let loop ((d d) (dsm dsm) (pll pll))
        (if (or (<= d (car dsm)) (<= (length pll) 2))
          (turn-point (+ (caar pll) d) (cdar pll) (caar pll) (cdar pll) (calc-a (car pll) (cadr pll)))
          (loop (- d (car dsm)) (cdr dsm) (cdr pll))))))

  (define (make-item ldd)
    (let*
      ((p (cadr ldd))
       (a (cddr ldd))
       (fs (abs (vector-ref font 1)))
       (d (shift p (turn (cons 0 (/ fs 3)) a))))
      (vector d a (car ldd))))

  (define (cal anl)
    (and-let*
      ((l (length anl))
       ((> l 1))
       (ep (string-index str kwasi-cursor-char)))
      (list-set! anl ep
        (cond
          ((zero? ep) (cadr anl))
          ((= ep (dec l)) (list-ref anl (dec ep)))
          (else (quotient (+ (list-ref anl (dec ep)) (list-ref anl (inc ep))) 2)))))
    #t)

  (or
    (and-let*
      ((pll)
       ((> (length pll) 1))
       (dsm (map dist pll (cdr pll)))
       (ptl (map (make-find-point dsm pll) (make-len-list str)))
       (anl (map calc-a ptl (cdr ptl)))
       ((cal anl))
       (ldl (map cons (string->list str) (map cons ptl anl))))
      (map make-item ldl))
    '()))

(define (make-text-by-line font color line text)
  (define (select vec i val)
    (if (> (vector-length vec) i) (vector-ref vec i) val))

  (define (calc-item-list)
    (let*
      ((font (get-property font))
       (ff (vector-ref font 0))
       (font-face (num->font-face (if (integer? ff) ff 0)))
       (font-size (inexact->exact (* obj-scale-factor (vector-ref font 1))))
       (font-weight  (select font 2 "*"))
       (font-style   (select font 3 "*"))
       (font-charset (select font 4 "Russian"))
       (afnt (vector ff font-size font-weight font-style font-charset)))
      (make-item-list (get-property line)
                      (get-property text)
                      afnt)))

  (make <Text-By-Line>
    #:dynamic #t
    #:dependence (list line text)
    #:font  font
    #:color color
    #:item-list calc-item-list))
