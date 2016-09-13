(define (class-cnv c s)
  (define t
    '(
;      (1002 . 1003) ;; (����-����� . ����-�����)
     ))
  (cons (or (assv-ref t c) c) s))


(define (table-cnv t o)
  (define b
    '(
;      (1002 .  (0 . 0)) ;; (����� .  (����-�����-⠡���� . ���饭��-�����-��ꥪ�))
     ))

  (or
    (and-let*
      ((p (assv-ref b t))
       (t (car p))
       (o (+ (cdr p) o)))
      (cons t o))
    (cons t o)))

(define (node-cnv n)
  (define t
    '(
;      (1 . 3) ;; (����-����� . ����-�����)
      (0 . 9) ;; (����-����� . ����-�����)
     ))
  (or (assv-ref t n) n))

