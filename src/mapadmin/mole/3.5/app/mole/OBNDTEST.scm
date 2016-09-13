;;=====================================================================
(define context-proc-list '())
(define global-context-proc-list '())
;;=====================================================================
(define (make-list cls)
  (if (list? cls) cls (list cls)))
;;=====================================================================
(define (make-test hosts tests proc-list proc)

  (define (add-test host-class test-class)
    (let*
      ((l0 (assq host-class proc-list)))
      (cond
        (l0 (set-cdr! l0 (assq-set! (cdr l0) test-class proc)) proc-list)
        (else (acons host-class (list (cons test-class proc)) proc-list)))))

  (for-each
    (lambda (m)
      (for-each
        (lambda (n)
          (set! proc-list (add-test m n)))
        (make-list tests)))
    (make-list hosts))
  proc-list)

(define (find-match lst class)
  (let loop ((l lst) (r #f))
    (cond
      ((null? l) r)
       ((= (caar l) class) (cdar l))
       ((is-derived-from-ex class (caar l)) (loop (cdr l) (cdar l)))
       (else (loop (cdr l) r)))))

(define (find-match-proc host-class test-class proc-list)
  (let loop ((l proc-list))
    (and (not (null? l))
         (or (and (is-derived-from-ex host-class (caar l))
                  (find-match (cdar l) test-class))
             (loop (cdr l))))))
;;=====================================================================
(define (add-context hosts targets p1 . p23)
  (set! context-proc-list (make-test hosts targets context-proc-list (cons p1 p23))))

(define (make-building-context host-class host-status host-image x y target-class)
  (let
    ((save-building-env building-env)
     (res #f))
    (and-let*
      ((proc (find-match-proc host-class target-class context-proc-list))
       (env (unwrap host-class host-status host-image)))
      (set! building-env env)
      (set! res ((car proc) x y)))
    (set! building-env save-building-env)
    res))

(define (get-target-class-list host-class)
  (or
    (and-let*
      ((l1 (find-match context-proc-list host-class))
       (l2 (list-select l1 (lambda (p) (not (eq? (caddr p) copy-target)))))
       (l3 (map car l2))
       (l4 (remove-duplicates (apply append (map get-bottom-class-list l3)))))
      (list-select l4 (lambda (class) (not (is-only-insertable? class)))))
    '()))

(define (get-brife-target-class-list host-class)
  (or
    (and-let*
      ((l1 (find-match context-proc-list host-class))
       (l3 (list-select l1 (lambda (p) (procedure? (cadr p)))))
       (l2 (map car l3)))
      (remove-duplicates
        (apply append (map get-bottom-class-list l2))))
    '()))

(define (get-insertable-class-list host-class)
  (if (true-insertable? host-class)
    (get-target-class-list host-class)
    '()))

(define (get-host-class-list target-class)
  (remove-duplicates
    (apply append
      (map get-bottom-class-list
        (foldr (lambda (h l)
                 (if (any (lambda (e)
                            (and (not (eq? (caddr e) copy-target))
                                 (is-derived-from-ex target-class (car e))))
                          (cdr h))
                   (cons (car h) l)
                   l))
               '()
               context-proc-list)))))

(define (get-host-class-list-ex target-class)
  (remove-duplicates
    (apply append
      (map get-bottom-class-list
        (foldr (lambda (h l)
                 (if (any
                       (lambda (e) (is-derived-from-ex target-class (car e)))
                       (cdr h))
                   (cons (car h) l)
                   l))
               '()
               context-proc-list)))))

(define (get-brife-host-class-list target-class)
  (remove-duplicates
    (foldr (lambda (h l)
             (if (any (lambda (e) (is-derived-from-ex target-class (car e))) (cdr h))
               (cons (car h) l)
               l))
           '()
           context-proc-list)))
;;=====================================================================
(define (add-global-context targets proc)
  (for-each
    (lambda (t)
      (set! global-context-proc-list (acons t proc global-context-proc-list)))
    (make-list targets)))

(define (create-in-global-context class status vars env0 env1)
  (and-let*
    ((proc (find-match global-context-proc-list class))
     (vars (proc vars env0 env1))
     ((list? vars)))
    (create-object class (or (assv-ref vars status-sym) status) vars)))
;;=====================================================================
(define (copy-target host-old host-new target) target)

(define (mid p0 p1)
  (map (lambda (x) (/ x 2))
       (map + p0 p1)))

(define (append-point-object x y)
  (and-let*
    ((spl (get-property 'apl))
     (n (select-segment x y spl #f))
     (l (length spl))
     (or (= n 0) (= n (- l 2)))
     (xy (cons x y))
     (pl (if (> l 2)
       (if (= n 0) (list-head spl 2) (reverse (list-tail spl (- l 2))))
       (if (< (dist xy (car spl)) (dist xy (car (last-pair spl)))) spl (reverse spl))))
     (a (calc-a (car pl) (cadr pl))))
    (list
      (cons 'a  (- a 900))
      (cons 'ta  (good-angle a))
      (cons 'x0 (caar pl))
      (cons 'y0 (cdar pl)))))

(define (insert-point-object x y)
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
      (cons 'a  (- a 900))
      (cons 'ta  (good-angle a))
      (cons 'x0 (+ (caar pl) (real-part z2)))
      (cons 'y0 (+ (cdar pl) (imag-part z2))))))

(define (insert-line-object x y)
  (and-let*
    ((x0 (get-property 'x0))
     (y0 (get-property 'y0)))
    (list (cons 'apl (list (cons x0 y0))))))

(define (coedit-line-object host-old host-new target)
  (let*
    ((p-old (cons (get-property-from host-old 'x0)
                  (get-property-from host-old 'y0)))
     (p-new (cons (get-property-from host-new 'x0)
                  (get-property-from host-new 'y0)))
     (apl (get-property-from target 'apl))
     (d0 (dist p-old (car apl)))
     (d1 (dist p-old (car (last-pair apl)))))
    (set-car! (if (< d0 d1) apl (last-pair apl)) p-new)
    (assv-set! target 'apl apl)))

(define (make-coedit-point-object sym)
  (lambda (host-old host-new target)
    (and-let*
      ((opl (enbound (eqv? sym 'apg) (or (get-property-from host-old sym) '())))
       (spl (enbound (eqv? sym 'apg) (or (get-property-from host-new sym) '())))
       (x (get-property-from target 'x0))
       (y (get-property-from target 'y0))
       (xy (cons x y))
       (k (select-segment x y opl #f))
       (n (min k (- (length spl) 2)))
       ((>= n 0))
       (pk (list-tail opl k))
       (q (/ (magnitude (make-z xy (car pk))) (magnitude (make-z (car pk) (cadr pk)))))
       (pl (list-tail spl n))
       (p0 (car pl))
       (p1 (cadr pl))
       (z0 (make-z p0 p1))
       (an (+ (calc-a p0 p1) (* (clockwise? spl) -900)))
       (d (offset p0 p1))
       (nx (+ (car p0) (* q (car d))))
       (ny (+ (cdr p0) (* q (cdr d))))
       )
      (set! target (assv-set! target 'y0 ny))
      (set! target (assv-set! target 'x0 nx))
      (set! target (assv-set! target 'a (- an 900))))
    target))
;;=====================================================================
(define (append-line x y)
  (and-let*
    ((apl (or (get-property 'pll) (get-property 'apl)))
     (pll apl)
     (p (cons x y))
     (d0 (dist p (car pll)))
     (dn (dist p (car (last-pair pll))))
     (sl (if (< d0 dn) pll (reverse pll)))
     (pp (car sl))
     (a (calc-a (car sl) (cadr sl))))
    (list (cons 'x0 (car pp))
          (cons 'y0 (cdr pp))
          (cons 'ta (good-angle a))
          (cons 'a (+ a 900))
          (cons 'apl (list pp)))))
;;=====================================================================
(define (make-polyline-context host-class host-status host-image)
  (and-let*
    ((env (unwrap host-class host-status host-image))
     (spl (assv-ref env 'apl))
     (n (- (length spl) 2))
     (pl (list-tail spl n))
     (a (calc-a (car pl) (cadr pl)))
     (p0 (cadr pl)))
    (list (cons 'a a)
          (cons 'ta (good-angle a))
          (cons 'x0 (car p0))
          (cons 'y0 (cdr p0)))))
;;=====================================================================
(define (copy-point x y)
  (and-let*
    ((x0 (get-property 'x0))
     (y0 (get-property 'y0)))
    (list (cons 'x0 x0)
          (cons 'y0 y0)
          (cons 'a 0)
          (cons 'apl (list (cons x0 y0))))))
;;=====================================================================
