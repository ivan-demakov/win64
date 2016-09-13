;;  "MAP"

(load "utils")
(load "parse")
(load "disp")
(load "pro_base")
(load "pro_cont")
(load "pro_draw")
(load "pro_arc")
(load "pro_icon")
(load "pro_lns")
(load "testutil")
(load "obndtest")
(load "linetxt")
(load "adm")

(define phone? #f)
(define teplo? #f)
(define elect? #f)
(define vodo?  #f)
(define mrtpr? #f)
(define moesk? #t)

(when phone?
  (load "mole_obj")
  (load "all")
  (load "bincntxt")
  (load "lord_cnv")
  (load "context")
  (load "mole_cnv"))

(when vodo?
  (load "vodo"))

(when teplo?
  (load "teplo"))

(when elect?
  (load "electro"))

(when mrtpr?
  (load "macab"))

(when moesk?
  (load "mosnrj"))

;;; End of code
