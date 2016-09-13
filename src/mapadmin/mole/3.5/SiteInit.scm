
;;; set locale
(setlocale 'LC_CTYPE "")

;;; set initial heap size
;(ksi:gc-expand (* 2 1024 1024))

;; reader is case sensitive 
;(set-ksi-option! '*read-case-sensitive* #f)

;; print closure body
;(set-ksi-option! '*print-closure-body* #f)

(open-errlog errlog/warning)
