;;=====================================================================
;; Disp.scm
;; Windows event dispatcher
;;
;; Copyright (C) 1997, A.Tarnovski
;;=====================================================================
;; Mode description have structure like this:
;;
;; (cons
;;   '<Symbol> ; mode-name
;;   (list
;;     (cons '<Symbol> <Procedure>) ; event-type  handler
;;     ...
;;   )
;; )
;;
;; event-type -- one of:
;;   key-press
;;   key-release
;;   button-press
;;   button-release
;;   motion
;;   expose
;;
;; two special types may be used to define pre- and
;; post-procedure for given mode:
;;   pre
;;   post
;;
;; handler -- some procedure of one argumen - event vector
;;
;;=====================================================================
(define (event-type event)      (vector-ref event 0))
(define (event-window event)    (vector-ref event 1))
(define (event-x event)         (vector-ref event 2))
(define (event-y event)         (vector-ref event 3))
(define (set-event-x! event x)  (vector-set! event 2 x))
(define (set-event-y! event y)  (vector-set! event 3 y))
(define (event-state event)     (vector-ref event 4))
(define (event-button event)    (vector-ref event 5))
(define (event-key event)       (vector-ref event 5))
(define (event-width event)     (vector-ref event 4))
(define (event-height event)    (vector-ref event 5))
(define (event-count event)     (vector-ref event 6))
;;=====================================================================
(define hi-resolution? #t)
(define curr-prim #f)
(define building-env '())
(define local-object? #f)
(define object-list '())
(define nbf-list '())
(define user-object-list '())
(define edition? #f)
(define curr-mouse '(-10000 . -10000))
(define mode-list '())
(define kwasi-cursor-char #\x00)
(define kwasi-cursor (string kwasi-cursor-char))
(define text-edit-point #f)
;;=====================================================================
(define class-sym 'class)
(define (get-class-sym) class-sym)
(define (get-class) (or (get-property class-sym) 0))
;;=====================================================================
(define status-sym 'status)
(define (get-status-sym) status-sym)
(define (get-status) (or (get-property status-sym) 0))
;;=====================================================================
(define (set-object-type class status)
  (set! building-env '())
  (set-property! class-sym class)
  (set-property! status-sym status))
;;=====================================================================
(define (get-var name)
  (assv-ref building-env name))
;;=====================================================================
(define (set-var! name value)
  (set! building-env (assv-set! building-env name value)))
;;=====================================================================
(define (get-curr-x) (car curr-mouse))
(define (get-curr-y) (cdr curr-mouse))
(define (copy-curr-mouse) (cons (get-curr-x) (get-curr-y)))
;;=====================================================================
(define (add-user-object class prototype)
  (if (not (memv? class user-object-list))
    (set! user-object-list (cons class user-object-list)))
  (add-object class prototype))
;;=====================================================================
(define (remove-user-object class)
  (set! user-object-list (memv-remove! class user-object-list))
  (set! object-list (assv-remove! object-list class)))
;;=====================================================================
(define (user-object? class)
  (memv? class user-object-list))
;;=====================================================================
(define (add-object class prototype . nbf)
  (for-each
    (lambda (c)
      (if (and (= (length nbf) 1) (not (car nbf)))
        (set! nbf-list (cons class nbf-list)))
      (set! object-list (assv-set! object-list c prototype)))
    (if (list? class) class (list class))))
;;=====================================================================
(define (buildable? class)
  (not (memq? class nbf-list)))
;;=====================================================================
(define (get-class-list)
  (map car object-list))
;;=====================================================================
(define (defined-object? class)
  (assv-ref object-list class))
;;=====================================================================
(define (get-obj-prototype class)
  (defined-object? class))
;;=====================================================================
(define (build-object dc class status var-list image local?)
; (tk-box (list class var-list image local?) "build-object")
; (tk-box class "class")
; (tk-box status "status")
; (tk-box image "image")
; (tk-box var-list "var-list")
; (tk-box local? "local?")
  (set! edition? (not (null? image)))
  (set! local-object? local?)
  (set! mode-list '())
  (set-object-type class status)

  (for-each
    (lambda (vd)
      (if (not (memq? (car vd) '(class status)))
        (set-property! (car vd) (cdr vd))))
    var-list)

  (and-let*
    ((prototype (get-obj-prototype class))
     (proto (copy-tree prototype))
     ((not (null? (contents-of proto)))))

    (init-building proto #:process-status)

    (when (and edition?
               (unpack proto (caddr image))
               (not (negative? (car image))))
      (init-building proto #:stabil-status)
      (let loop ((cont (contents-of proto)) (i 0))
        (cond
          ((get-property (omit? (car cont))) (loop (cdr cont) i))
          ((not ((if hi-resolution? h-visible? l-visible?) (car cont))) (loop (cdr cont) (+ i 1)))
          ((< i (car image)) (loop (cdr cont) (+ i 1)))
          (else (set-current! proto (car cont)))))
      (set! edition? #f)
      (set! curr-mouse (cadr image))
      (if (or (symbol? (tool-of (current-of proto))) (automatic? (current-of proto)))
        (let
          ((tool (tool-of (current-of proto))))
          (set-text-edit-point! (current-of proto))
          (if (integer? tool)
            (if (negative? tool)
              (to-previous-item proto)
              (set-current! proto (list-ref (contents-of proto) tool)))
            (let loop
              ((cnt (contents-of proto)))
              (if (not (null? cnt))
                (if (eq? (label-of (car cnt)) tool)
                  (set-current! proto (car cnt))
                  (loop (cdr cnt)))))))
        (set! text-edit-point #f))
      (set! edition? #t)
      (delay-item (current-of proto) proto)
      (set-curr-prim! proto))
    (set! image (build dc proto proto proto))
    image))
;;=====================================================================
(define (unwrap class status image)
  (let
    ((save-building-env building-env)
     (save-edition? edition?)
     (save-hi-resolution? hi-resolution?)
     (res #f))
    (set-object-type class status)
    (set! hi-resolution? (not (zero? (logand status #x80))))
    (set! edition? #t)
    (and-let*
      ((prototype (get-obj-prototype class))
       (proto (copy-tree prototype)))
      (set! res (unpack proto image))
      (and-let*
        ((plg (assv-ref res 'apg))
         ((not (assv-ref res 'apl))))
        (set! res (assv-set! res 'apl (enbound #t plg))))
      (and-let*
        ((pll (assv-ref res 'apl))
         ((equal? (car pll) (car (last-pair pll))))
         ((not (assv-ref res 'apg))))
        (set! res (assv-set! res 'apg (unbound pll)))))
    (set! building-env save-building-env)
    (set! edition? save-edition?)
    (set! hi-resolution? save-hi-resolution?)
    res))
;;=====================================================================
(define (unselect-mode dc)
  (let
    ((post-fun (assv-ref (car mode-list) 'post)))
    (set! mode-list (cdr mode-list))
    (put-hint "")
    (if post-fun
      (post-fun dc))))
;;=====================================================================
(define (select-mode mode dc)
  (set! mode-list (cons mode mode-list))
  (let
    ((pre-fun (assv-ref mode 'pre)))
    (if pre-fun
      (pre-fun dc))))
;;=====================================================================
(define (dispatch-event dc event)
  (case (event-type event)
    ((button-press button-release motion)
      (set-car! curr-mouse (event-x event))
      (set-cdr! curr-mouse (event-y event))))

  (when (not (null? mode-list))
    (let*
      ((handler (assv-ref (car mode-list) (event-type event)))
       (result (or (not handler) (handler dc event))))
      result)))
;;=====================================================================
(define (show-obj . l)
  (define (ct t) (string-append (if (string? t) t (object->string t)) " "))
  (put-hint (apply string-append (map ct l))))
;;=====================================================================
(define (rebuild-object old-class old-status
                        new-class new-status
                        var-list image)
  (define (def-txy)
    (and-let*
      ((pg (get-property 'apg))
       (st (get-property 'number))
       (cv (calc-num-coord pg st)))
      (or (get-property 'tx) (set-property! 'tx (vector-ref cv 0)))
      (or (get-property 'ty) (set-property! 'ty (vector-ref cv 1)))
      (or (get-property 'ta) (set-property! 'ta 0))))

  (define (def-prop pr1 pr0)
    (or (get-property pr1)
        (set-property! pr1 (get-property pr0))))

  (let
    ((save-building-env building-env)
     (save-edition? edition?)
     (save-hi-resolution? hi-resolution?))
    (set! hi-resolution? (not (zero? (logand old-status #x80))))
    (set! edition? #t)
    (and-let*
      ((old-prototype (get-obj-prototype old-class))
       (new-prototype (get-obj-prototype new-class))
       (old-proto (copy-tree old-prototype))
       (new-proto (copy-tree new-prototype))
       ((set-object-type old-class old-status))
       (old-e (or (unpack old-proto image) '()))
       ((set-object-type new-class new-status))
       (new-e (or (unpack new-proto image) '()))
       ((or (not (null? old-e)) (not (null? new-e)))))
      (set! building-env (append var-list new-e old-e))
      (set! hi-resolution? (not (zero? (logand new-status #x80))))
      (set-property! class-sym new-class)
      (set-property! status-sym new-status)
      (when (get-number)
        (def-txy)
        (for-each def-prop '(tx ty ta) '(x0 y0 0)))
    (set! image (pack new-proto)))
    (set! building-env save-building-env)
    (set! edition? save-edition?)
    (set! hi-resolution? save-hi-resolution?)
    image))
;;=====================================================================
(define (create-object class status var-list)
  (let
    ((save-hi-resolution? hi-resolution?))
    (and-let*
      ((prototype (get-obj-prototype class))
       (proto (copy-tree prototype)))
      (set! hi-resolution? (not (zero? (logand status #x80))))
      (set! building-env '())
      (for-each (lambda (vd) (set-property! (car vd) (cdr vd))) var-list)
      (set-property! class-sym class)
      (set-property! status-sym status)
      (set! var-list (pack proto))
      (set! hi-resolution? save-hi-resolution?)
      var-list)))
;;=====================================================================
(define (create-default-object class)
  (and-let*
    ((prototype (get-obj-prototype class))
     (proto (copy-tree prototype)))
    (set! building-env '())
    (set-default! proto)
    (set-property! class-sym class)
    (set-property! status-sym 0)
    (and (built? proto) (pack proto))))
;;=====================================================================
(define (get-contents-map class status)
  (define (test-slot slot)
    (define (inf? slot sign)
      (or (eq? slot sign)
          (and (pair? slot) (eq? (car slot) sign))))
    (cond
      ((and (is-a? slot <Text>) #:text))
      ((and (is-a? slot <Coord>)
            (inf? (x-coord-of slot) 'x0)
            (inf? (y-coord-of slot) 'y0)) #:point)
      ((and (is-a? slot <Poly-Coord>)
            (inf? (coord-list-of slot) 'vpl) #:footnote))
      ((and (is-a? slot <Poly-Coord>)
            (inf? (coord-list-of slot) 'apl) #:polyline))
      ((and (is-a? slot <Bound-Polyline>)
            (inf? (coord-list-of slot) 'apg) #:bound-polyline))
      ((and (is-a? slot <Poly-Coord>)
            (inf? (coord-list-of slot) 'apg) #:polygon))
      (else #f)))
  (let
    ((save-building-env building-env)
     (save-hi-resolution? hi-resolution?)
     (result #f))
    (set-object-type class status)
    (set! hi-resolution? (not (zero? (logand status #x80))))
    (and-let*
      ((prototype (get-obj-prototype class))
       (proto (copy-tree prototype))
       (short-cont (list-select (contents-of proto) (lambda (x) (not (get-property (omit? x)))))))
      (set! result (map test-slot short-cont)))
    (set! building-env save-building-env)
    (set! hi-resolution? save-hi-resolution?)
    result))
;======================================================================
(define (tk-box . x)
  (apply tk-message-box (map object->string x)))
;;=====================================================================
(define (smart-output file-name obj)
  (let*
    ((str (object->string obj))
     (port (open-file file-name "at"))
     (offset 0)
     (len (string-length str)))
    (dotimes (i len)
      (let
        ((c (string-ref str i)))
        (case c
          (( #\( )
            (set! offset (+ offset 2))
            (newline port)
            (dotimes (i offset) (write-char #\space port))
            (write-char c port))
          (( #\))
            (write-char c port)
            (set! offset (- offset 2)))
          (else
            (write-char c port)))))
    (close-port port)))
;;=====================================================================
(define (is-only-insertable? class) #f)
(define (true-insertable? class) #f)
;;=====================================================================
