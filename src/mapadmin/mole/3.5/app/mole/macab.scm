;;
;; New Magistral objects
;;
(let*
  ((bnd-width 1)
   (white-color 1)
   (black-color 0)
   (rrl-nod-color 502)
   (mag-nod-color 503)
   (rgn-nod-color 504)
   (brt-nod-color 505)
   (psp-nod-color 507)
   (main-color    540)
   (yellow-color  541)
   (node-text-color 0)
   (node-text-fh  32)
  )

  (define (node-text-font k)
    (vector 0 (- (* k -1 node-text-fh))))

  (define (move-poly pll)
    (let
      ((p0 (cons (get-x0) (get-y0))))
      (map (lambda (p) (shift p p0)) pll)))

  (define (make-triangle r)
    (let*
      ((x2 (inexact->exact(* r 0.866025403784438647)))
       (x0 (- x2))
       (y0 (inexact->exact(* r .5)))
       (x1 0)
       (y1 (- r)))
      (move-poly (list (cons x0 y0 ) (cons x1 y1) (cons x2 y0)))))

  (define (make-sptri r)
    (let*
      ((f (deg->rad 300))
       (tn (tan f))
       (x2 (* r (cos f)))
       (x0 (- x2))
       (y0 (* r .5))
       (x1 0)
       (y1 (- r))
       (y3 (- y0 (* r .33)))
       (x4 (* tn (- y3 y1)))
       (x3 (- x4))
       (dx (/ (- x4 x3) 3))
       (x5 (- x2 dx))
       (x6 (- x4 dx))
       (x7 (- x5 dx))
       (x8 (- x6 dx))
       (x9 (- x7 dx)))
      (move-poly
        (list (cons x0 y0) (cons x1 y1)
              (cons x1 y1) (cons x2 y0)
              (cons x2 y0) (cons x0 y0)
              (cons x3 y3) (cons x4 y3)
              (cons x4 y3) (cons x5 y0)
              (cons x6 y3) (cons x7 y0)
              (cons x8 y3) (cons x9 y0)))))

  (define (make-trap r)
    (let*
      ((f (deg->rad 300))
       (tn (tan f))
       (x2 (* r (cos f)))
       (x0 (- x2))
       (y0 (* r .5))
       (y1 (- r))
       (y3 (- y0 (* r .33)))
       (x4 (* tn (- y3 y1)))
       (x3 (- x4)))
      (move-poly
        (list (cons x0 y0) (cons x3 y3)
              (cons x4 y3) (cons x2 y0)))))

  (define (get-num1) (or (get-property 'number)  "Номер"))
  (define (get-num2) (or (get-property 'number1) "Доп.инф."))
  (define (calc-txy2 k)
    (let*
      ((x (get-tx))
       (y (get-ty))
       (a (get-ta)))
      (shift (cons x y) (turn (cons 0  (* k 24 obj-scale-factor)) a))))
  (define (calc-tx2 k) (car (calc-txy2 k)))
  (define (calc-ty2 k) (cdr (calc-txy2 k)))

  (define (mk-vers sl)
    (make <Container>
      #:contents
      (append!
        (list
          (make <Color-Footnote>)
          (make <Point>
            #:x-coord 'x0
            #:y-coord 'y0))
        sl
        (list
          (make <Text>
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle 'ta
            #:text 'number)
          (make <Text>
            #:text 'number1)))))

  (define (mk-tmpl tk sl . ver)
    (make <Container>
      #:version (cons (and (not (null? ver)) (car ver)) #f)
      #:contents
      (append!
        (list
          (make <Color-Footnote>
            #:l-visible #f
            #:tool 't0
            #:dependence '(x0 y0 tx ty ta)
            #:color 0
            #:width 0
            #:coord-list
              (cons
                'vpl
                (lambda ()
                  (make-footnote
                    (get-x0) (get-y0)
                    (lim-tx 500) (lim-ty 500) (get-ta)
                    (get-num1)
                    (* node-text-fh tk)
                    #f))))
          (make <Point>
            #:x-coord 'x0
            #:y-coord 'y0
            #:hint "Разместите объект"))
        sl
        (list
          (make <Text>
            #:l-visible #f
            #:label 't0
            #:color node-text-color
            #:x-coord 'tx
            #:y-coord 'ty
            #:angle 'ta
            #:font (node-text-font tk)
            #:text (cons 'number get-num1)
            #:hint "Разместите обозначение")
          (make <Text>
            #:tool 't0
            #:s-visible #t
            #:l-visible #f
            #:dependence '(tx ty ta)
            #:x-coord (lambda () (calc-tx2 tk))
            #:y-coord (lambda () (calc-ty2 tk))
            #:angle get-ta
            #:font (node-text-font (* .7 tk))
            #:text (cons 'number1 get-num2))))))

  (define (make-tmpl-0 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Circle>
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:foreground fll-clr
          #:color bnd-clr
          #:x-size r)
          )
      (mk-vers
        (list
          (make <Circle>)
          (make <Circle>)))
          ))

  (define (make-tmpl-1 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Polygon>
          #:dependence '(x0 y0)
          #:width bnd-width
          #:color bnd-clr
          #:foreground white-color
          #:coord-list (lambda () (make-triangle r)))
        (make <Color-Lines>
          #:dependence '(x0 y0)
          #:width bnd-width
          #:color bnd-clr
          #:coord-list (lambda () (make-sptri r)))
    )))

  (define (make-tmpl-11 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Polygon>
          #:dependence '(x0 y0)
          #:width bnd-width
          #:color bnd-clr
          #:foreground white-color
          #:coord-list (lambda () (make-triangle r)))
        (make <Filled-Polygon>
          #:dependence '(x0 y0)
          #:foreground fll-clr
          #:coord-list (lambda () (make-trap r)))
    )
      (mk-vers
        (list
          (make <Polygon>)
          (make <Outlined-Polygon>)
          (make <Polygon>)))
    ))

  (define (make-tmpl-2 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Circle>
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:color bnd-clr
          #:foreground white-color
          #:x-size (+ r 1))
        (make <Filled-Polygon>
          #:dependence '(x0 y0)
          #:foreground fll-clr
          #:coord-list (lambda () (make-triangle r)))
          )
      (mk-vers
        (list
          (make <Circle>)
          (make <Ring>)
          (make <Polygon>)))
          ))

  (define (make-tmpl-3 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Circle>
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:width bnd-width
          #:color bnd-clr
          #:foreground white-color
          #:x-size r)
        (make <Pie-Slice>
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:foreground fll-clr
          #:x-size r
          #:y-size r
          #:angle 900
          #:delta 1800)
          )
      (mk-vers
        (list
          (make <Circle>)
          (make <Outlined-Circle>)
          (make <Pie-Slice>)))
          ))

  (define (make-tmpl-4 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Polygon>
          #:dependence '(x0 y0)
          #:width bnd-width
          #:foreground white-color
          #:color bnd-clr
          #:coord-list (lambda () (make-triangle (+ r 1))))
        (make <Circle>
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:foreground fll-clr
          #:x-size (/ r 2)))
      (mk-vers
        (list
          (make <Polygon>)
          (make <Bound-Polyline>)
          (make <Circle>)))
          ))

  (define (make-tmpl-5 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Circle>
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:width bnd-width
          #:foreground white-color
          #:color bnd-clr
          #:x-size r)
        (make <Circle>
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:x-size (inexact->exact (* r .7))
          #:foreground fll-clr)
          )
      (mk-vers
        (list
          (make <Circle>)
          (make <Ring>)
          (make <Circle>)))
          ))

  (define (make-tmpl-6 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Bar>
          #:dependence '(x0 y0)
          #:foreground white-color
          #:color bnd-clr
          #:width bnd-width
          #:x-coord (lambda () (- (get-x0) r))
          #:y-coord (lambda () (- (get-y0) r))
          #:x-size (+ r r)
          #:y-size (+ r r))
        (make <Bar>
          #:dependence '(x0 y0)
          #:foreground fll-clr
          #:x-coord (lambda () (- (get-x0) r))
          #:y-coord (lambda () (+ (get-y0) (/ r 2)))
          #:x-size (+ r r)
          #:y-size (round (/ r 2)))
          )
      (mk-vers
        (list
          (make <Bar>)
          (make <Color-Rectangle>)
          (make <Bar>)))
          ))

  (define (make-tmpl-7 r bnd-clr fll-clr)
    (define w (* r 3))
    (define h (inexact->exact (* r 1.2)))
    (define c (cons (* w -.5) (* h -.5)))
    (mk-tmpl 1
      (list
        (make <Outlined-Bar>
          #:dependence '(x0 y0)
          #:color white-color
          #:foreground black-color
          #:x-coord (lambda () (+ (get-x0) (car (turn c -450))))
          #:y-coord (lambda () (+ (get-y0) (cdr (turn c -450))))
          #:angle -450
          #:x-size w
          #:y-size h)
        (make <Outlined-Circle>
          #:dependence '(x0 y0)
          #:color bnd-clr
          #:foreground fll-clr
          #:x-coord get-x0
          #:y-coord get-y0
          #:x-size r)
          )))

  (define (make-tmpl-9 r bnd-clr fll-clr)
    (mk-tmpl 1
      (list
        (make <Outlined-Circle>
          #:dependence '(x0 y0)
          #:color bnd-clr
          #:width 0
          #:foreground fll-clr
          #:x-coord get-x0
          #:y-coord get-y0
          #:x-size r)
          )
      (mk-vers
        (list
          (make <Polygon>)
          (make <Outlined-Polygon>)))
          ))

  (define (make-tmpl-10 r bnd-clr fll-clr)
    (define d (inexact->exact (* r .4)))
    (define s (inexact->exact (* r .125)))
    (define (zig)
      (map (lambda (p) (shift p (cons (get-x0) (get-y0))))
           (list (cons (- d) 0) (cons d (- d)) (cons 0 d))))
    (mk-tmpl 1
      (list
        (make <Outlined-Chord>
          #:dependence '(x0 y0)
          #:x-coord get-x0
          #:y-coord get-y0
          #:x-size r
          #:y-size r
          #:angle 1500
          #:delta 1500
          #:color bnd-clr
          #:foreground fll-clr)
        (make <Color-Polyline>
          #:dependence '(x0 y0)
          #:width 0
          #:color bnd-clr
          #:coord-list zig)
        (make <Circle>
          #:dependence '(x0 y0)
          #:foreground bnd-clr
          #:x-coord (lambda () (+ (get-x0) d))
          #:y-coord (lambda () (- (get-y0) d))
          #:x-size s
          #:y-size s)
          )))

  (add-object MOLE-MAG-NET-PUNCT-163 (make-tmpl-0 16 mag-nod-color psp-nod-color))
  (add-object MOLE-MAG-NET-PUNCT-154 (make-tmpl-0 16 mag-nod-color mag-nod-color))
  (add-object MOLE-MAG-NET-PUNCT-157 (make-tmpl-0 16 mag-nod-color rrl-nod-color))

  (add-object MOLE-MAG-NET-PUNCT-156 (make-tmpl-1 20 mag-nod-color mag-nod-color) #f)

  (add-object MOLE-MAG-NET-PUNCT-191 (make-tmpl-2 20 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-203 (make-tmpl-2 20 mag-nod-color rrl-nod-color) #f)

  (add-object MOLE-MAG-NET-PUNCT-187 (make-tmpl-3 10 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-188 (make-tmpl-3 10 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-197 (make-tmpl-3 10 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-198 (make-tmpl-3 10 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-199 (make-tmpl-3 10 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-200 (make-tmpl-3 10 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-201 (make-tmpl-3 10 mag-nod-color mag-nod-color) #f)

  (add-object MOLE-MAG-NET-PUNCT-195 (make-tmpl-4 16 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-196 (make-tmpl-4 16 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-202 (make-tmpl-4 16 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-204 (make-tmpl-4 16 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-205 (make-tmpl-4 16 mag-nod-color mag-nod-color) #f)

  (add-object MOLE-MAG-NET-PUNCT-155 (make-tmpl-5 10 mag-nod-color mag-nod-color) #f)

  (add-object MOLE-MAG-NET-PUNCT-183 (make-tmpl-6 8 mag-nod-color mag-nod-color) #f)
  (add-object MOLE-MAG-NET-PUNCT-194 (make-tmpl-6 10 mag-nod-color mag-nod-color) #f)

  (add-object MOLE-MAG-NET-PUNCT-193 (make-tmpl-7 16 mag-nod-color rrl-nod-color) #f)

  (add-object MOLE-MAG-MUFTA-CONNECT (make-tmpl-9 8 mag-nod-color rgn-nod-color) #f)
  (add-object MOLE-MAG-MUFTA-STVOR   (make-tmpl-9 8 mag-nod-color rrl-nod-color) #f)
  (add-object MOLE-MAG-MUFTA-BRANCH  (make-tmpl-9 8 mag-nod-color brt-nod-color))

  (add-object MOLE-MAG-NET-PUNCT-192 (make-tmpl-10 20 mag-nod-color rrl-nod-color) #f)

  (add-object MOLE-MAG-NET-PUNCT-190 (make-tmpl-11 25 mag-nod-color mag-nod-color) #f)
)

(define cb-lines
  (list MOLE-MAG-LINE-MAGISTRAL
        MOLE-MAG-LINE-INTRAZONE
        MOLE-MAG-LINE-INPLACE
        MOLE-MAG-LINE-ACCESS
        MOLE-MAG-LINE-UNKNOWN
        MOLE-MAG-LINE-PROJECT
        MOLE-MAG-LINE-MYSTERY
        MOLE-MAG-LINE-INTERNATIONAL))

(let*
  ((cab-text-color 0)
   (cab-text-fh  25)
   (cab-line-width 5)
   (rrl-line-width 3)
   (cab-line-color0 501)
   (cab-line-color1 531)
   (cab-line-color2 532)
   (cab-line-color3 533)
   (cab-line-color4 534)
   (cab-line-color5 535)
   (cab-line-color6 536)
   (cab-line-color7 537)
   (cab-line-color8 538)
   (rrl-line-color 502)
   (cab-text-font (vector 0 (- cab-text-fh)))
   (stvor-status 16)
   (stvor-style 16)
  )

  (define cb-nodes
    (list MOLE-MAG-NET-PUNCT-154
          MOLE-MAG-NET-PUNCT-155
          MOLE-MAG-NET-PUNCT-156
          MOLE-MAG-NET-PUNCT-157
          MOLE-MAG-NET-PUNCT-163
          MOLE-MAG-NET-PUNCT-183
          MOLE-MAG-NET-PUNCT-187
          MOLE-MAG-NET-PUNCT-188
          MOLE-MAG-NET-PUNCT-190
          MOLE-MAG-NET-PUNCT-191
          MOLE-MAG-NET-PUNCT-194
          MOLE-MAG-NET-PUNCT-195
          MOLE-MAG-NET-PUNCT-196
          MOLE-MAG-NET-PUNCT-197
          MOLE-MAG-NET-PUNCT-198
          MOLE-MAG-NET-PUNCT-199
          MOLE-MAG-NET-PUNCT-200
          MOLE-MAG-NET-PUNCT-201
          MOLE-MAG-NET-PUNCT-202
          MOLE-MAG-NET-PUNCT-204
          MOLE-MAG-NET-PUNCT-205
          MOLE-MAG-NET-PUNCT-203
          MOLE-MAG-NET-PUNCT-192))

  (define cb-muftes (list MOLE-MAG-MUFTA-BRANCH MOLE-MAG-MUFTA-STVOR))
  (define rl-nodes  (list MOLE-MAG-NET-PUNCT-193))

  (define (make-test cb? nlst)

    (define (add-cnct lst) (cons MOLE-MAG-MUFTA-CONNECT lst))

    (let*
      ((h0 (if cb? "Линия должна заканчиваться в узле или разв. муфте!" "РРЛ должна заканчиваться в узле!"))
       (h1 (if cb? "Линия должна начинаться в узле или разв. муфте!"    "РРЛ должна начинаться в узле!"))
       (h2 (if cb? "Неоднозначные начало и конец линии!" "Неоднозначные начало и конец РРЛ!"))
       (h3 (if cb? "Неоднозначное начало линии!"         "Неоднозначное начало РРЛ!"))
       (h4 (if cb? "Неоднозначный конец линии!"          "Неоднозначный конец РРЛ!"))
       (h5 (if cb? "Линия не должна заканчиваться в той же точке!" "РРЛ не должна заканчиваться в той же точке!")))
      (lambda (pl start?)

        (define (test-point begin?)
         (let*
           ((pl (if begin? pl (last-pair pl)))
            (pm (map
                  (lambda (point)
                    (set-car! pl (cons (assq-ref point 'x0)
                                       (assq-ref point 'y0)))
                     point)
                  (get-all-objects (car pl) (if edition? (add-cnct nlst) nlst)))))
            (length
              (list-select pm
                (lambda (point)
                  (equal? (car pl) (cons (assq-ref point 'x0)
                                         (assq-ref point 'y0))))))))

       (set-property! 'apl pl)

       (let*
         ((ns (test-point #t))
          (nf (test-point #f)))
         (and-let*
           (((not start?))
            ((not edition?))
            (ln (length pl))
            ((>= ln 2)))
           (set-property! 'apl pl)
           (set-property! 'ta1 (good-angle (calc-a (car pl) (car (last-pair pl))))))

         (cond
           ((= ns 0) h1)
           ((= nf 0) h0)
           ((and (> ns 1) (> nf 1)) h2)
           ((> ns 1) h3)
           ((> nf 1) h4)
           ((and (equal? (car pl) (car (last-pair pl))) (or (not start?) edition?)) h5)
         )))))

  (define cb-ndes1  (cons MOLE-MAG-MUFTA-BRANCH cb-nodes))
  (define cab-nodes (cons MOLE-MAG-MUFTA-STVOR cb-ndes1))
  (define all-nodes (append cb-nodes rl-nodes))

  (define test0 (make-test #t cab-nodes))
  (define test1 (make-test #f all-nodes))
  (define test2 (make-test 0  cb-ndes1))

  (define (make-line num test width fc font)
    (make <Container>
      #:contents
      (list
        (make <Color-Polyline>
          #:dependence '(apl)
          #:color 1
          #:width (* width 2)
          #:coord-list (lambda () (get-property 'apl)))
        (make <Color-Polyline>
          #:color fc
          #:width width
          #:line (lambda () (if (= (logand #x7f (get-status)) stvor-status) stvor-style #:solid))
          #:valid-test test
          #:coord-num num
          #:coord-list 'apl
          #:hint "Трасса кабеля")
        (make <Text>
          #:s-visible #t
          #:x-coord 'tx
          #:y-coord 'ty
          #:angle 'ta
          #:font font
          #:text 'number
          #:hint "Обозначение")
        (make <Color-Footnote>
          #:s-visible #t
          #:dynamic #t
          #:dependence '(apl tx ty)
          #:line  #:solid
          #:color 0
          #:width 0
          #:coord-list
            (cons
              'vpl
              (lambda ()
                (make-pll-footnote
                  (get-property 'apl)
                  (get-tx) (get-ty) (get-ta)
                  (get-number)
                  cab-text-fh cab-text-fh)
                  )))
          )))

  (add-object MOLE-MAG-LINE-RRL
    (make-line 2 test1 rrl-line-width rrl-line-color cab-text-font))

  (add-object MOLE-MAG-LINE-PROJECT
    (make-line 0 test2 cab-line-width cab-line-color5 cab-text-font))

   (add-object MOLE-MAG-LINE-MAGISTRAL
    (make-line 0 test0 cab-line-width cab-line-color0 cab-text-font) #f)

  (add-object MOLE-MAG-LINE-INTRAZONE
    (make-line 0 test0 cab-line-width cab-line-color1 cab-text-font) #f)

  (add-object MOLE-MAG-LINE-INTERNATIONAL
    (make-line 0 test0 cab-line-width cab-line-color7 cab-text-font) #f)

  (add-object MOLE-MAG-LINE-INPLACE
    (make-line 0 test0 cab-line-width cab-line-color2 cab-text-font) #f)

  (add-object MOLE-MAG-LINE-ACCESS
    (make-line 0 test0 cab-line-width cab-line-color3 cab-text-font) #f)

  (add-object MOLE-MAG-LINE-UNKNOWN
    (make-line 0 test0 cab-line-width cab-line-color6 cab-text-font) #f)

  (add-object MOLE-MAG-LINE-MYSTERY
    (make-line 0 test0 cab-line-width cab-line-color4 cab-text-font) #f)

  (add-context
    MOLE-MAG-NET-PUNCT
    MOLE-MAG-LINE
    insert-line-object
    coedit-line-object)

  (add-global-context
    MOLE-MAG-LINE
    (lambda (vars env0 env1)

      (define sz 25)
      (define main-ln #f)

      (define (calc-sz p0 p1)

        (define (dst im)
          (let*
            ((pl (assv-ref im 'apl))
             (dm (map (lambda (p) (dist-to-line (car p) (cdr p) p0 p1)) pl)))
            (apply max dm)))

        (define (cmpf im0 im1)
          (let*
            ((d0 (dst im0))
             (d1 (dst im1))
             (md (max d0 d1)))
            (= md d0)))

        (define (my-img? im)
          (and-let*
            ((st (assv-ref im status-sym))
             (pl (assv-ref im 'apl))
             (s (car pl))
             (f (car (last-pair pl))))
            (or (and (equal? s p0) (equal? f p1))
                (and (equal? f p0) (equal? s p1)))))

        (define (ln-img? im)
          (not (eqv? stvor-status (logand (assv-ref im status-sym) #x7f))))

        (or
          (and-let*
            ((ls0 (get-all-objects p0 MOLE-MAG-LINE))
             (ls1 (list-select ls0 my-img?))
             (lsn (list-select ls1 ln-img?))
             ((not (null? ls1)))
             (ls2 (list-sort ls1 cmpf)))
             (if (not (null? lsn))
               (set! main-ln (assv-ref (car lsn) 'apl)))
             (+ (dst (car ls2)) sz))
          sz))

      (define (mk-stvor p0 p1 a sz)
        (let*
          ((of (offset p0 p1))
           (t3 (turn of (- a)))
           (sg (if (< (car t3) 0) -1 1))
           (l (abs (car t3)))
           (dx (* sg (min sz (/ (max 0 (- l (* sz 2))) 2))))
           (t1 (cons dx sz))
           (t2 (cons (- (car t3) dx) sz)))
          (list
            p0
            (shift p0 (turn t1 a))
            (shift p0 (turn t2 a))
            p1)))

      (and-let*
       ((x0 (assv-ref env0 'x0))
        (y0 (assv-ref env0 'y0))
        (x1 (assv-ref env1 'x0))
        (y1 (assv-ref env1 'y0))
        (p0 (cons x0 y0))
        (p1 (cons x1 y1))
        (a (calc-a p0 p1))
        (ta (good-angle a))
        (d0 (if (= a ta) p0 p1))
        (d1 (if (= a ta) p1 p0))
        (sh (if (assv-ref vars 'stvor) (calc-sz d0 d1) 0))
        (tpl (if (zero? sh) (list p0 p1)  (mk-stvor p0 p1 ta sh)))
        (apl (if (and main-ln (not (equal? (car main-ln) p0))) (reverse tpl) tpl))
        (mp (cons (/ (+ x0 x1) 2) (/ (+ y0 y1) 2)))
        (of (shift mp (turn (cons 0 (+ 20 sh)) ta)))
        (num (assv-ref vars 'number)))
        (list (cons 'apl apl)
              (cons status-sym (if (assv-ref vars 'stvor) stvor-status 0))
              (cons 'number num)
              (cons 'tx (car of))
              (cons 'ty (cdr of))
              (cons 'ta ta)))))

  (add-context
    cb-lines
    (list MOLE-MAG-MUFTA-CONNECT MOLE-MAG-MUFTA-BRANCH)
    insert-point-object
    (make-coedit-point-object 'apl))

  (add-context
    cb-muftes
    cb-lines
    insert-line-object
    coedit-line-object)

  (add-context
    MOLE-MAG-MUFTA-CONNECT
    cb-lines
    #f;insert-line-object
    coedit-line-object)
)

(begin

  (define (make-town range)
    (define param-vec
      '#( #(50 10 510 515 520 #(0 -200))
          #(45 10 511 516 521 #(0 -175))
          #(40 10 512 517 522 #(0 -150))
          #(35 10 513 518 523 #(0 -125))
          #(30 10 514 519 524 #(0 -100))))

      (define (get-par n)
        (vector-ref (vector-ref param-vec range) n))

    (make <Container>
      #:contents
      (list
        (make <Outlined-Circle>
          #:x-coord 'x0
          #:y-coord 'y0
          #:x-size     (get-par 0)
          #:width      (get-par 1)
          #:foreground (get-par 2)
          #:color      (get-par 3)
          #:hint "Разместите объект")
        (make <Text>
          #:dependence '(x0 y0)
          #:s-visible #t
          #:x-coord (lambda () (+ (get-x0) (get-par 0) (* 2 (get-par 1))))
          #:y-coord get-y0
          #:angle 'ta
          #:angle 'a
          #:color (get-par 4)
          #:font  (get-par 5)
          #:text 'number
          #:hint "Разместите название"))))

  (add-object MOLE-MAG-CAPITAL     (make-town 0))
  (add-object MOLE-MAG-MEGAPOLICE  (make-town 1))
  (add-object MOLE-MAG-CITY        (make-town 2))
  (add-object MOLE-MAG-TOWN        (make-town 3))
  (add-object MOLE-MAG-WILLAGE     (make-town 4))
)
;;== COMMENTARY =======================================================
(let*
  ((comm-text-fh 32)
   (text-color 0)
   (comm-text-font (vector 0 (- comm-text-fh)))
   (test (lambda (s)
     (and (string? s)
          (positive?  (string-length s)))))
   (pr0
     (make <Container>
       #:contents
       (list
         (make <Text>
           #:valid-test test
           #:font comm-text-font
           #:color text-color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid '(5 . 0)
           #:hint    "Комментарий"
           #:text    'number))))

   (pr1
     (make <Container>
       #:contents
       (list
         (make <Color-Footnote>
           #:tool 't0
           #:dependence '(x0 y0 tx ty ta)
           #:color 0
           #:width 0
           #:coord-list
             (cons
               'vpl
               (lambda ()
                 (make-footnote
                   (get-x0) (get-y0)
                   (get-tx) (get-ty) (get-ta)
                   (get-number)
                   comm-text-fh
                   0))))
         (make <Point>
           #:x-coord 'x0
           #:y-coord 'y0
           #:hint "Зафиксируйте выноску")
         (make <Text>
           #:valid-test test
           #:font comm-text-font
           #:color text-color
           #:x-coord 'tx
           #:y-coord 'ty
           #:angle   'ta
           #:a-grid '(5 . 0)
           #:hint    "Комментарий"
           #:text    'number))))
       )

   (add-object MOLE-MAG-COMMENTARY pr0)
   (add-object MOLE-MAG-COMM-SUBL  pr1)
   )
;;=====================================================================
(define (is-only-insertable? class)
  (memv? class (list MOLE-MAG-MUFTA-CONNECT
                     MOLE-MAG-MUFTA-BRANCH
                     )))

(define (true-insertable? class)
  (not (memv? class
              (list MOLE-MAG-NET-PUNCT-163
                    MOLE-MAG-NET-PUNCT-154
                    MOLE-MAG-NET-PUNCT-157
                    MOLE-MAG-NET-PUNCT-156
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
                    MOLE-MAG-MUFTA-STVOR
                    MOLE-MAG-NET-PUNCT-192
                    MOLE-MAG-NET-PUNCT-190))))
;;=====================================================================
#!

(let*
  ((white-color 1)
   (black-color 0)
   (wd 5)
   (ln 40)
   (tn 10)
   (pass-text-fh 32)
   (pass-font (vector 0 (- pass-text-fh)))
   )

  (define (make-pass clr tmpl)

    (define (clc-a)
      (good-angle (get-a)))

    (define (calc-p0)
      (shift (cons (get-x0) (get-y0)) (turn (cons (/ ln -2) 0) (clc-a))))

    (define (mk-line)
      (let*
        ((p0 (calc-p0)))
        (list p0 (shift p0 (turn (cons ln 0) (clc-a))))))

    (define (calc-txy)
      (shift (calc-p0) (turn (cons (+ ln tn)  0) (clc-a))))

    (define (clc-tx) (car (calc-txy)))
    (define (clc-ty) (cdr (calc-txy)))

    (make <Container>
      #:contents
      (list
        (make <Color-Rectangle>
          #:x-coord 'x0
          #:y-coord 'y0
          #:angle   'a)
        (make <Color-Polyline>
          #:dependence '(x0 y0 a)
          #:line tmpl
          #:width wd
          #:color clr
          #:coord-list mk-line)
        (make <Text>
          #:dependence '(x0 y0 a)
          #:x-coord clc-tx
          #:y-coord clc-ty
          #:angle   clc-a
          #:color 0
          #:font  pass-font
          #:text 'number)
          )))

  (define (make-cond clr tmpl)

    (define (clc-a)
      (good-angle (get-a)))

    (define (calc-txy)
      (shift (calc-p0) (turn (cons (+ ln tn)  0) (clc-a))))

    (define (clc-tx) (car (calc-txy)))
    (define (clc-ty) (cdr (calc-txy)))

    (make <Container>
      #:contents
      (list
        (make <Color-Polyline>
          #:line tmpl
          #:width wd
          #:color clr
          #:coord-list 'apl)
;        (make <Text>
;          #:dependence '(apl)
;          #:x-coord clc-tx
;          #:y-coord clc-ty
;          #:angle   clc-a
;          #:color 0
;          #:font  pass-font
;          #:text 'number)
          )))


  (add-object MOLE-MAG-PASS-WATER (make-pass 540 #:solid))

  (add-object MOLE-MAG-EQV-PIPE (make-cond 0 #:solid))

  (add-context
    cb-lines
    (list MOLE-MAG-PASS-WATER)
    insert-point-object
    (make-coedit-point-object 'apl)
    )

(add-global-context
  (list MOLE-MAG-CAB-PASS)
  (lambda (vars env0 env1)
    (and-let*
     ((pl (assv-ref env0 'apl))
      (pq (assv-ref vars 'rat))
      (nm (assv-ref vars 'number))
      (qp (string->number pq))
      ((<= 0 qp 1.))
      (ln (* qp (poly-length pl))))
     (receive (p a) (point-on-dist pl ln)
      (list (cons 'x0 (car p))
            (cons 'y0 (cdr p))
            (cons 'a (- a 900))
            (cons 'number nm )
            )))))


(add-global-context
  (list MOLE-MAG-EQV-PIPE)
  (lambda (vars env0 env1)
    (and-let*
     ((pl (assv-ref env0 'apl))
      (q0 (assv-ref vars 'rat0))
      (q1 (assv-ref vars 'rat1))
      (nm (assv-ref vars 'number))
      (l0 (string->number q0))
      (l1 (string->number q1))
      ((<= 0 l0 1.))
      ((<= 0 l1 1.))
      (l2 (+ l0 l1))
      ((<= l1 l2 1.))
      (fln (poly-length pl))
      (d0 (* l0 fln))
      (d1 (* l2 fln))
      (tpl (line-on-dist pl d0 d1)))
      (list (cons 'number nm )
            (cons 'apl tpl)
            ))))


)
!#

