;;;
;;; init.scm
;;;
;;; Copyright (C) 2009-2010, ivan demakov.
;;;
;;; This code is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU Lesser General Public License as published by
;;; the Free Software Foundation; either version 2.1 of the License, or (at your
;;; option) any later version.
;;;
;;; This code is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
;;; License for more details.
;;;
;;; You should have received a copy of the GNU Lesser General Public License
;;; along with this code; see the file COPYING.LESSER.  If not, write to
;;; the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
;;; MA 02110-1301, USA.
;;;
;;;
;;; Author:        ivan demakov <ksion@users.sourceforge.net>
;;; Creation date: Tue Dec 15 18:17:00 2009
;;; Last Update:   Sun Sep 19 19:46:41 2010
;;;
;;;

(library (ksi init)
         (export command-line)

         (import (ksi core syntax)
                 (ksi core base)
                 (ksi core list)
                 (ksi core string)
                 (ksi core eval)
                 (ksi getopt)
                 (prefix (ksi core system) ksi:)
                 (ksi io))

;; parse program arguments
(define command-line-arguments '())
(define internal-command-line-arguments '())
(define (command-line) internal-command-line-arguments)
(define continue-after-script? #f)

(define (usage)
  (for-each display
    (list "Usage: " ksi:application " OPTION ...

Evaluate Scheme code, interactively or from script.

Without arguments the program loads 'Boot.scm' (from KSi library location)
and run interactively.

Options:

  -b [FILE]      load FILE, and run interactively.
                 Should be first argument in command line (otherwise ignored).
                 Other arguments (if any) ignored but can be parsed in FILE.

  -c EXPRESSION  eval Scheme EXPRESION, and exit

  -h, --help     display this help and exit

  -v, --version  display version information and exit

  --             stop scanning arguments and run interactively
")))

(let ((name (car ksi:argv))
      (argv (cdr ksi:argv)))
  (letrec ((apply-proc (lambda (proc . args)
                         (catch #t
                                (lambda () (apply proc args))
                                (lambda (tag val)
                                  (if (exn? val)
                                      (format (current-error-port) "~s: ~a: ~s~%" ksi:application (exn-message val) (exn-value val))
                                      (format (current-error-port) "~s: uncatched ~s: ~a~%" ksi:application tag val))
                                  (exit #f)))))

           (next-option (lambda (kw arg rest-opts)
                          (case kw
                            ((h: help:)
                             (usage)
                             (display "\nSend bug reports to <ksion@users.sourceforge.net>\n\n")
                             (exit))

                            ((v: version:)
                             (display "KSi ")
                             (display (ksi:version))
                             (newline)
                             (display (ksi:copyright))
                             (newline)
                             (exit))

                            ((c:)
                             (set! command-line-arguments rest-opts)
                             (set! internal-command-line-arguments (cons name rest-opts))
                             (apply-proc (lambda (x) (eval (call-with-input-string (string-append "(begin " x ")") read)
                                                           (environment '(ksi base) '(ksi io))))
                                         arg)
                             (exit))

                            ((end-args)
                             (set! command-line-arguments arg)
                             (set! internal-command-line-arguments (cons name arg))
                             #f)

                            ((usage-error)
                             (format (current-error-port) "~a: unrecognized option `~a'~%" ksi:application (keyword->string arg))
                             (exit #f))

                            (else
                             (format (current-error-port) "~a: invalid argument `~a'~%" ksi:application arg)
                             (exit #f))))))

    (set! command-line-arguments argv)
    (set! internal-command-line-arguments (cons name argv))
    (parse-options argv
                   '(h: help: v: version:)
                   '(c:)
                   next-option)))

;; Show notice
(if (ksi:interactive)
    (format #t
	    "~a~%Release ~a [~a.~a.~a]~%~%"
	    (ksi:banner)
	    ;(ksi:copyright)
	    (ksi:version) (ksi:cpu) (ksi:os) (ksi:host)))

)

;;; End of code