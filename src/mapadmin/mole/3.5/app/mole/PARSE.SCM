;;syntax parse of string function

(define (parse-string source rool-list mess)
  (define (apply-rool rool val)
    (let
      ((tst (car rool)))
      (or (eqv? tst val)
          (and-let*
            (((symbol? tst))
             (proc (eval tst (@current-environment)))
             ((procedure? proc)))
            (proc val)))))
  (define (get-rool label)
    (assv-ref rool-list label))
  (define (next-rool rool)
    (and (> (length rool) 1)
         (get-rool (cadr rool))))
  (define (else-rool rool)
    (and (> (length rool) 2)
         (get-rool (caddr rool))))
  (and
    (string? source)
    (not (null? rool-list))
    (let*
     ((ep (string-index source kwasi-cursor-char))
      (txt (if ep (string-append (substring source 0 ep) (substring source (inc ep))) source)))
     (let loop ((rool (cdar rool-list)) (sl (string->list txt)))
       (cond
         ((not rool) mess)
         ((null? sl) (or ep (null? rool) (loop (else-rool rool) sl)))
         ((null? rool) mess)
         ((and (apply-rool rool (car sl))
               (loop (next-rool rool) (cdr sl))))
         (else (loop (else-rool rool) sl)))))))
