;;=====================================================================
(define auto-context-proc-list '())
(define auto-build-param 0)
;;=====================================================================
(define (add-auto-context hosts proc)
  (for-each
    (lambda (class) (set! auto-context-proc-list (assv-set! auto-context-proc-list class proc)))
    (if (list? hosts) hosts (list hosts))))

(define (build-in-context host-class host-status host-image)
  (and-let*
    ((l (assv-ref auto-context-proc-list host-class))
     (env (unwrap host-class host-status host-image)))
    (set! building-env env)
    (l)))
;;=====================================================================
;;=====================================================================
(define (make-street-names)
  (define (find-point apl d)
    (let loop ((apl apl) (d d))
      (and-let*
        (((> (length apl) 1))
         (p0 (car apl))
         (p1 (cadr apl))
         (of (offset p0 p1))
         (l (dist p0 p1)))
        (if (<= l d)
          (loop (cdr apl) (- d l))
          (shift p0 (cons (inexact->exact (/ (* d (car of)) l))
                          (inexact->exact (/ (* d (cdr of)) l))))))))

  (or
    (and-let*
      ((spc-len auto-build-param)
       ((positive? spc-len))
       (apl (get-property 'apl))
       (txt (get-property 'number))
       ((text-visible? txt))
       (sl (string-length txt))
       ((> sl 1))
       (txt-len (inexact->exact (* sl 0.55 low-font-size)))
       (pll-len (apply + (map dist apl (cdr apl))))
       ((positive? pll-len))
       (n (max 1 (inexact->exact (/ (- pll-len spc-len) (+ txt-len spc-len)))))
       (spc-new (inexact->exact (/ (- pll-len (* txt-len n)) (inc n))))
       (d0 spc-new)
       (d1 (+ spc-new txt-len))
       (lst '()))
      (dotimes (i n)
        (and-let*
          (
           (p0 (or (find-point apl d0) (car apl)))
           (p1 (or (find-point apl d1) (car (last-pair apl))))
           (a (calc-a p0 p1))
           (ga (good-angle a))
           (p (if (= a ga) p0 p1)))
          (set! lst
            (cons
              (list
                MOLE-STREET-NAME-BY-LINE
                0
                (list
                  (cons 'number txt)
                  (cons 'apl (calc-pl-for-text apl txt (car p) (cdr p)))))
              lst)))
          (set! d0 (+ d1 spc-new))
          (set! d1 (+ d0 txt-len)))
    lst)
    '())) 
;;=====================================================================
(add-auto-context MOLE-STREET make-street-names)
;;=====================================================================
