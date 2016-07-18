;;;
;;; klos.scm
;;; The ksi object system
;;;
;;; Copyright (C) 1997-2010, ivan demakov
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
;;; Creation date: Tue Jun 17 20:29:19 1997
;;; Last Update:   Sun Sep 19 19:47:36 2010
;;;
;;;

(library (ksi klos)
         (export
          ;; classes defined in "C" code
          <top> <object> <boolean> <char> <string> <symbol> <keyword> <list> <pair> <null>
          <vector> <number> <complex> <real> <rational> <integer> <unknown>
          <class> <method> <generic> <proc> <entity> <procedure>
          <rtd> <record>

          ;; constants defined in "C" code
          ;; FIXME: should be reexported ?
          ;;gns-name gns-allocation gns-getter gns-setter gns-init gns-type gns-size

          ;; utilites defined in "C" code
          instance? class? generic? method? class-of type-of type?
          slot-ref slot-set! slot-bound? slot-exist? slot-exist-in-class?
          class-name class-direct-supers class-direct-slots class-precedence-list
          class-slots class-default-initargs class-get-n-set append-method

          ;; functions and macro defined here
          subclass? is-a? make
          make-instance allocate-instance initialize-instance initialize initialize-slots
          define-generic define-method
          apply-generic compute-applicable-methods method-applicable? method-more-specific?
          no-next-method no-applicable-method
          compute-slot-option compute-get-n-set
          define-class ensure-metaclass define-slot-getters define-slot-setters
          compute-old-slots compute-new-slots change-class
          slot-unbound slot-missing
          instance-eqv? instance-equal?
          write-instance display-instance

          ;; classes defined here
          <composite-class>)

         (import (ksi core syntax)
                 (ksi core base)
                 (ksi list)
                 (ksi core hashtables)
                 (only (ksi core string) string->symbol symbol->string)
                 (only (ksi core vector) vector vector? vector-ref vector-length)
                 (only (ksi core number) even? exact? integer? < <= +)
                 (only (ksi core eval) define-macro! procedure-has-arity? bound-identifier? eval-core error)
                 (only (ksi core io) format)
                 (only (ksi sort) list-sort!)
                 (ksi core klos))


(define (subclass? class super)
  (if (memq super (class-precedence-list class)) #t #f))


(define (is-a? obj class)
  (subclass? (class-of obj) class))


;;;
;;; report syntax error
;;;
(define (klos-error form msg)
  (error (car form) msg form))


;;;
;;; first of all, define the `make' procedure -- the instance constructor.
;;;
;;; `make' computes INITARGS, and creates an instance of the CLASS.
;;;
;;; To compute INITARGS `make' uses primitive `@merge-args' (implemented
;;; in C-code) to merge args passed to `make' with default CLASS args.
;;; To create instance `make' calls generic function
;;; `make-instance' (defined bellow).
;;;
;;; Standard method for `make-instance' calls generic functions
;;; `allocate-instance' to allocate the "row" object, and, then,
;;; `initialize-instance' to initialize it.
;;; The `initialize-instance' standard method initialize instance slots
;;; and call generic function `initialize'.
;;;
;;; Note, that `make-instance' and `allocate-instance' are called with CLASS as a first argument,
;;; but `initialize-instance' and `initialize' with (allocated) instance of the CLASS.
;;;
;;; Note, that `make' relies on the existance of these generic
;;; functions.  But usually generic functions (as well as all other instances)
;;; are created using the `make'.  To avoid the egg-and-kitchen problem,
;;; the functions that `make' uses are created and initialized
;;; manualy (without using of `make').
;;;

(define (make class . initargs)
  (if (class? class)
      (make-instance class (@merge-args initargs (class-default-initargs class)))
      (error 'make "invalid class in arg1" class)))


;;;
;;; define and initialize generic function `make-instance',
;;; and its standard method.
;;;
;;; make-instance (CLASS <class>) (INITARGS <top>)
;;;
;;; create an instance of the CLASS.
;;;
;;; Standard behavior of the `make-instance' is to apply generic function
;;; `allocate-instance' to CLASS and INITARGS arguments,
;;; and, then, to apply generic function `initialize-instance' to
;;; instance returned by `allocate-instance' and INITARGS.
;;; Finally, `make-instance' returns the created instance.
;;; `make-instance' itself doesn't interprete INITARGS.
;;;

(define make-instance
  (let ((generic (@allocate-instance <generic>))
	(method (@allocate-instance <method>))
	(proc (lambda (call-next-method next-method? class initargs)
		(let ((instance (allocate-instance class initargs)))
		  (initialize-instance instance initargs)
		  instance))))

    ;; initialize the generic's slots
    (slot-set! generic 'name 'make-instance)
    (slot-set! generic 'methods (list method))
    (slot-set! generic 'arity (list <class> #t))

    ;; initialize the method's slots
    (slot-set! method 'generic-function generic)
    (slot-set! method 'specializers (list <class> #t))
    (slot-set! method 'procedure proc)
    (slot-set! method 'combination 'primary)

    generic))


;;;
;;; define and initialize generic function `allocate-instance',
;;; and its standard method.
;;;
;;; allocate-instance (CLASS <class>) (INITARGS <top>)
;;;
;;; Allocate memory for instance of the CLASS.
;;;
;;; Standard method of `allocate-instance' allocate object
;;; by calling primitive procedure `@allocate-instance'.
;;;

(define allocate-instance
  (let ((generic (@allocate-instance <generic>))
	(method (@allocate-instance <method>))
	(proc (lambda (call-next-method next-method? class initargs)
		(@allocate-instance class))))

    ;; initialize the generic slots
    (slot-set! generic 'name 'allocate-instance)
    (slot-set! generic 'methods (list method))
    (slot-set! generic 'arity (list <class> #t))

    ;; initialize the method slots
    (slot-set! method 'generic-function generic)
    (slot-set! method 'specializers (list <class> #t))
    (slot-set! method 'procedure proc)
    (slot-set! method 'combination 'primary)

    generic))


;;;
;;; define and initialize generic function `initialize-instance',
;;; and its standard method.
;;;
;;; initialize-instance (INSTANCE <object>) (INITARGS <top>)
;;;
;;; Initialize the INSTANCE.
;;;
;;; Standard method of `initialize-instance' initializes
;;; slots of instance by calling procedure `initialize-slots
;;; and then calls generic function `initialize'
;;;

(define initialize-instance
  (let ((generic (@allocate-instance <generic>))
	(method (@allocate-instance <method>))
	(proc (lambda (call-next-method next-method? instance initargs)
		(initialize-slots #t instance initargs)
		(initialize instance initargs))))

    ;; initialize the generic slots
    (slot-set! generic 'name 'initialize-instance)
    (slot-set! generic 'methods (list method))
    (slot-set! generic 'arity (list #t #t))

    ;; initialize method slots
    (slot-set! method 'generic-function generic)
    (slot-set! method 'specializers (list <object> #t))
    (slot-set! method 'procedure proc)
    (slot-set! method 'combination 'primary)

    generic))


;;;
;;; initialize-slots SLOT-LIST INSTANCE INITARGS
;;;
;;; Sets INSTANCE slots from the SLOT-LIST to initial values.
;;; If SLOT-LIST is #t, all INSTANCE slots that have 'instance allocation
;;  are initialized.
;;;
;;; Used in generic-function `initialize-instance' as
;;; standard slot initializer.
;;;

(define (initialize-slots slot-list instance initargs)

  (define (init-slot slot)
    (let* ((args (assq-ref (class-slots (class-of instance)) (vector-ref slot gns-name)))
	   (val  (@get-arg (@get-arg initarg: args) initargs)))
      (if (void? val)
	  (if (not (void? (vector-ref slot gns-init)))
	      (slot-set! instance (vector-ref slot gns-name) (vector-ref slot gns-init)))
	  (slot-set! instance (vector-ref slot gns-name) val))))

  (cond ((eq? slot-list #t)
	 (for-each
	  (lambda (slot)
	      (if (eq? (vector-ref slot gns-allocation) 'instance)
		  (init-slot slot)))
	  (class-get-n-set (class-of instance))))

	((pair? slot-list)
	 (for-each
	  (lambda (slot)
	    (if (memq (vector-ref slot gns-name) slot-list)
		(init-slot slot)))
	  (class-get-n-set (class-of instance))))))


;;;
;;; define and initialize generic function `initialize',
;;; and its standard method.
;;;
;;; initialize (INSTANCE <object>) (INITARGS <top>)
;;;
;;; 'Initialize' is intended to be called after slot initialization.
;;; It may be used for a class specific initialization of the instance.
;;;
;;; Standard method of `initialize' do nothing.
;;;

(define initialize
  (let ((generic (@allocate-instance <generic>))
	(method (@allocate-instance <method>))
	(proc (lambda (call-next-method next-method? instance initargs) instance)))

    ;; initialize the generic slots
    (slot-set! generic 'name 'initialize)
    (slot-set! generic 'methods (list method))
    (slot-set! generic 'arity (list <object> #t))

    ;; initialize the method slots
    (slot-set! method 'generic-function generic)
    (slot-set! method 'specializers (list <object> #t))
    (slot-set! method 'procedure proc)
    (slot-set! method 'combination 'primary)

    generic))


;;;
;;; At this point we have defined minimal set of generic functions
;;; and methods to use procedure `make' for all classes except
;;; the `<class>', i.e. we cannot define new classes yet.
;;; Now we will define macros for defining generic functions and
;;; methods.  After that, we can define new method by using that
;;; macros.
;;;

(define (get-arg-names form args)
  (cond ((null? args) '())

        ((symbol? args) args)

        ((and (pair? args) (symbol? (car args)))
         (cons (car args) (get-arg-names form (cdr args))))

        ((and (pair? args) (pair? (car args)) (symbol? (caar args)) (pair? (cdar args)) (null? (cddar args)))
         (cons (caar args) (get-arg-names form (cdr args))))

        (else
         (klos-error form "invalid argument list"))))


(define (get-arg-specs form args)
  (cond ((null? args) '())

        ((symbol? args) #t)

        ((and (pair? args) (symbol? (car args)))
         (cons #t (get-arg-specs form (cdr args))))

        ((and (pair? args) (pair? (car args)) (symbol? (caar args)) (pair? (cdar args)) (null? (cddar args)))
         (cons (list 'unquote (cadar args)) (get-arg-specs form (cdr args))))

        (else
         (klos-error form "invalid argument list"))))

;;;
;;; define-generic NAME (generic-class: GENERIC-CLASS) ARGS BODY ...
;;; define-generic NAME (generic-class: GENERIC-CLASS) ARGS
;;; define-generic NAME (generic-class: GENERIC-CLASS)
;;; define-generic NAME ARGS BODY ...
;;; define-generic NAME ARGS
;;; define-generic NAME
;;;
;;; Define a generic function NAME.
;;;

(define-macro! (define-generic form env)
  (or (and (list? form) (pair? (cdr form)))
      (klos-error form "invalid syntax"))

  (let ((name (cadr form)) (args (cddr form)))
    (or (symbol? name)
        (klos-error form "generic name should be a symbol"))

    (if (null? args)
        `(,#'define ,name (,#'make ,#'<generic> name: ',name arity: #t))

        (let ((class '<generic>) (args (car args)) (body (cdr args)))
          (if (and (pair? args) (keyword? (car args)))
              (let ((opts args))
                (if (null? body)
                    (set! args 'any) ; set args to any symbol
                    (begin (set! args (car body))
                           (set! body (cdr body))))

                (set! class (@get-arg generic-class: opts '<generic>))))

          (let ((specs (list 'quasiquote (get-arg-specs form args))))
            (if (null? body)
                `(,#'define ,name (,#'make ,class name: ',name arity: ,specs))

                `(,#'begin (,#'define ,name (,#'make ,class name: ',name arity: ,specs))
                        (,#'append-method ,name
                                       (,#'make ,#'<method>
                                         generic-function: ,name
                                         specializers: ,specs
                                         combination: 'primary
                                         procedure: (,#'lambda (call-next-method next-method? ,@(get-arg-names form args)) ,@body)))
                        )))))))


;;;
;;; define-method NAME (combination: COMBINATION) ARGS BODY ...
;;; define-method NAME primary: ARGS BODY ...
;;; define-method NAME before: ARGS BODY ...
;;; define-method NAME after: ARGS BODY ...
;;; define-method NAME around: ARGS BODY ...
;;; define-method NAME ARGS BODY ...
;;;
;;; Define a method of generic function NAME.
;;;

(define-macro! (define-method form env)
  (if (and (pair? form) (pair? (cdr form)) (pair? (cddr form)) (pair? (cdddr form)))
      (let ((name (cadr form))
            (args (caddr form))
            (body (cdddr form)))
        (if (symbol? name)
            (let ((combination 'primary))
              (cond ((keyword? args)
                     (set! combination (keyword->symbol args))
                     (set! args (car body))
                     (set! body (cdr body)))

                    ((and (pair? args) (eq? (car args) combination:) (pair? (cdr args)) (null? (cddr args)))
                     (set! combination (cadr args))
                     (set! args (car body))
                     (set! body (cdr body)))

                    ((or (symbol? args) (and (pair? args) (symbol? (car args))) (and (pair? args) (pair? (car args)) (symbol? (caar args)))))

                    (else
                     (klos-error form "invalid syntax")))

              (cond ((null? body)
                     (klos-error form "empty method body"))

                    ((not (bound-identifier? name env))
                     (klos-error form "generic function not defined"))

                    ((or (eq? combination 'before) (eq? combination 'after))
                     `(,#'begin
                        (,#'if (,#'is-a? ,name ,#'<generic>)
                            (,#'append-method ,name (,#'make ,#'<method>
                                                   generic-function: ,name
                                                   specializers: ,(list #'quasiquote (get-arg-specs form args))
                                                   combination: ',combination
                                                   procedure: (,#'lambda ,(get-arg-names form args) ,@body)))
                            (error 'define-method "not a generic function" ',name))))

                    ((or (eq? combination 'primary) (eq? combination 'around))
                     `(,#'begin
                        (,#'if (,#'is-a? ,name ,#'<generic>)
                            (,#'append-method ,name (,#'make ,#'<method>
                                                   generic-function: ,name
                                                   specializers: ,(list #'quasiquote (get-arg-specs form args))
                                                   combination: ',combination
                                                   procedure: (,#'lambda (call-next-method next-method? ,@(get-arg-names form args)) ,@body)))
                            (error 'define-method "not a generic function" ',name))))

                    (else
                     (klos-error form "invalid method combination"))))

            (klos-error form "invalid generic name")))

      (klos-error form "invalid syntax")))


;;;
;;; Method `apply-generic' defines protocol for calling generic functions. 
;;; This method is not used for "pure" generic functions (generic functions
;;; whose class is the <generic>), in this case used a completly "C" coded protocol.
;;; The method `apply-generic' is called by the interpreter when a subclass
;;; of the <generic> is applied.
;;;

(define-generic apply-generic ((gf <generic>) args)

  (define (procs-of combination rev?)
    (let ((procs (map (lambda (x) (slot-ref x 'procedure))
		      (compute-applicable-methods gf args combination))))
      (if rev? (reverse! procs) procs)))

  (define (next procs args)
    (lambda new-args
      (let ((real-args (if (null? new-args) args new-args)))
	(if (null? procs)
	    (no-next-method gf real-args)
	    (apply (car procs)
		   (next (cdr procs) real-args)
		   (if (pair? (cdr procs)) true false)
		   real-args)))))

  ;; Verify that this function has any associated methods
  (if (null? (slot-ref gf 'methods))
      (no-applicable-method gf args)

      (let ((primary (procs-of 'primary #f))
	    (around  (procs-of 'around  #f))
	    (before  (procs-of 'before  #f))
	    (after   (procs-of 'after   #t)))

	(cond
	 ((null? primary)
	  (if (null? around)
	      (no-applicable-method gf args)
	      (apply (car around)
		     (next (cdr around) args)
		     (if (pair? (cdr around)) true false)
		     args)))

	 ((and (null? before) (null? after))
	  (let ((procs (append! around primary)))
	    (apply (car procs)
		   (next (cdr procs) args)
		   (if (pair? (cdr procs)) true false)
		   args)))

	 (else
	  (let* ((first (lambda (next next? . args)
			  (for-each (lambda (x) (apply x args)) before)
			  (let ((res (apply (car primary) next next? args)))
			    (for-each (lambda (x) (apply x args)) after)
			    res)))
		 (procs (append! around (list first) (cdr primary))))
	    (apply (car procs)
		   (next (cdr procs) args)
		   (if (pair? (cdr procs)) true false)
		   args)))))))

;;;
;;; compute-applicable-methods GF ARGS COMBINATION
;;;
;;; Computes a list of applicable to arguments ARGS methods
;;; of generic function GF.
;;; More specific methods should be placed in the list before less
;;; specific methods.
;;;
;;; This method is used in protocol for calling standard generic
;;; function (but see note for `apply-generic') to build list
;;; of applicable methods.
;;;
;;; It uses generic functions `method-applicable?' to determine that
;;; method is applicable and `method-more-specific?' to sort them.
;;;

(define-generic compute-applicable-methods ((gf <generic>) args combination)
  (let loop ((methods (slot-ref gf 'methods))
	     (applicable '()))

    (if (null? methods)
	(list-sort! (lambda (a b) (method-more-specific? a b args)) applicable)
	(loop (cdr methods)
	      (if (and (eqv? (slot-ref (car methods) 'combination) combination)
		       (method-applicable? (car methods) args))
		  (cons (car methods) applicable)
		  applicable)))))


;;;
;;; method-applicable? METHOD ARGS
;;;
;;; Test if the METHOD is applicable to the arguments ARGS.
;;;

(define-generic method-applicable? ((m <method>) args)
  (@method-applicable? m args))


;;;
;;; method-more-apecific? METHOD1 METHOD2 ARGS
;;;
;;; Test if METHOD1 is more specific than METHOD2 for arguments ARGS.
;;;

(define-generic method-more-specific? ((m1 <method>) (m2 <method>) args)
  (@method-more-specific? m1 m2 args))


;;;
;;; Methods for the possible error we can encounter when calling
;;; a generic function.
;;;

(define-generic no-next-method ((gf <generic>) args)
  (error "No next method when calling ~S with ~S" gf args))

(define-generic no-applicable-method ((gf <generic>) args)
  (error "No applicable method for ~S in call ~S"  gf (append (cons (slot-ref gf 'name) args))))


;;;
;;; Utilities for class definition
;;;

;;;
;;; compute-cpl CLASS
;;;
;;; Computes class-precedence-list of the CLASS.
;;; Called before the CLASS is fully initialized.
;;;

(define (compute-cpl class)
  (let* ((filter (lambda (cpl)
		   (let loop ((cpl cpl) (res '()))
		     (cond ((null? cpl) res)
			   ((memq (car cpl) (cdr cpl))
			    (loop (cdr cpl) res))
			   (else
			    (loop (cdr cpl) (cons (car cpl) res)))))))
	 (supers (class-direct-supers class))
	 (cpl (apply append! (map compute-cpl supers))))
    (reverse! (filter (cons class cpl)))))


;;;
;;; allocate-slot-index CLASS
;;;
;;; Allocates next offset for instance slot of CLASS.
;;;

(define (allocate-slot-index class)
  (let ((index (slot-ref class 'nfields)))
    (slot-set! class 'nfields (+ index 1))
    index))


;;;
;;; compute-slots CLASS
;;;
;;; Compute slots of the CLASS.
;;;
;;; Called after a class-precedence-list of CLASS computed, but before
;;; the CLASS is fully initialized.
;;;
;;; Return list of all slots in the CLASS.  Each element of the list
;;; should be a list, first element of which is slot name and other
;;; elements are slot options.
;;;

(define (compute-slots class)
  (let loop ((slots (apply append (map class-direct-slots (reverse (class-precedence-list class)))))
	     (res '()))
    (if (null? slots)
	(reverse! res)
	(let* ((slot (car slots)) (name (car slot)))
	  (loop (cdr slots)
		(if (assq name res)
		    res
		    (cons
		     (let next ((slots slots) (res '()))
		       ;; build list of slot definitions
		       ;; in class precedence order
		       (if (null? slots)
			   (merge-all-slot-options class res)
			   (if (eq? name (caar slots))
			       (next (cdr slots) (cons (car slots) res))
			       (next (cdr slots) res))))
		     res)))))))


;;;
;;; merge-all-slot-options CLASS SLOTS
;;;
;;; Merge slot options of the SLOTS of the CLASS.
;;;
;;; Called from compute-slots.
;;;
;;; Each slot in the list SLOTS should be a list, first element of
;;; which is a slot name and other elements are slot options.
;;; All slot names should be equal and slots should be in class precedence
;;; order.
;;;
;;; Return a list in which first element is the slot name and
;;; other elements are slot options for the given CLASS.
;;;

(define (merge-all-slot-options class slots)
  (let* ((name (caar slots)))
    (let loop ((opts (map cdr slots)) (res '()))
      (if (null? opts)
	  (cons name res)
	  (loop (cdr opts)
		(let next ((opts (car opts)) (res res))
		  (if (null? opts)
		      res
		      (next (cddr opts)
			    (if (void? (@get-arg (car opts) res))
				(cons (car opts) (cons (merge-slot-option class (car opts) slots) res))
				res)))))))))


;;;
;;; merge-slot-option CLASS OPTION SLOTS
;;;
;;; Merge values of the slot OPTION from the SLOTS of the CLASS.
;;;
;;; Called from merge-all-slot-options.
;;;
;;; Return a value for the OPTION.
;;;

(define (merge-slot-option class option slots)
  (let ((name (caar slots)))
    (let loop ((slots slots) (res '()))
      (if (null? slots)
	  (compute-slot-option (class-of class) class name option (reverse! res))
	  (loop (cdr slots)
		(cons (let next ((opts (cdar slots)) (vals '()))
			(if (null? opts)
			    (reverse! vals)
			    (next (cddr opts)
				  (if (eq? (car opts) option)
				      (cons (cadr opts) vals)
				      vals))))
		      res))))))


(define (get-n-set-valid? class gns)
  (define (accessor? x n)
    (or (and (integer? x) (exact? x) (< -1 x (slot-ref class 'nfields)))
	(and (procedure? x) (procedure-has-arity? x n))))

  (and (vector? gns)
       (<= gns-size (vector-length gns))
       (symbol? (vector-ref gns gns-name))
       (symbol? (vector-ref gns gns-allocation))
       (accessor? (vector-ref gns gns-getter) 1)
       (accessor? (vector-ref gns gns-setter) 2)))


;;;
;;; define method `initialize' for `<class>'.
;;;

(define-method initialize ((class <class>) initargs)
  (if (next-method?)
      (call-next-method))

  ;; Set unbound slots to default values.
  (if (not (slot-bound? class 'name))
      (slot-set! class 'name '?))
  (if (not (slot-bound? class 'direct-supers))
      (slot-set! class 'direct-supers '()))
  (if (not (slot-bound? class 'direct-slots))
      (slot-set! class 'direct-slots '()))
  (if (not (slot-bound? class 'default-initargs))
      (slot-set! class 'default-initargs '()))

  ;; Compute class-precedence-list.
  (slot-set! class 'cpl (compute-cpl class))

  ;; Compute default-initargs
  (slot-set! class 'default-initargs (apply @merge-args (map class-default-initargs (class-precedence-list class))))

  ;; Test that slots are defined corectly.
  (let loop ((slots (class-direct-slots class)))
    (cond ((null? slots))

          ((not (list (car slots)))
           (error "initialize class ~a: invalid slot definition: ~s" (class-name class) (car slots)))

          ((not (symbol? (caar slots)))
           (error "initialize class ~a: slot name should be a symbol: ~s" (class-name class) (car slots)))

          ((even? (length (car slots)))
           (error "initialize class ~a: missing value for slot option: ~s" (class-name class) (car slots)))

          (else (loop (cdr slots)))))

  ;; Compute slots.
  (slot-set! class 'slots (compute-slots class))

  ;; Now we can compute get-n-set.
  ;; First, we should set `nfield' slot to zero.
  (slot-set! class 'nfields 0)

  ;; In process of computation of the `get-n-set', `nfield' will be changed.
  (slot-set! class 'get-n-set
	     (map (lambda (slot)
		    (let ((gns (compute-get-n-set class slot initargs)))
		      (if (not (get-n-set-valid? class gns))
			  (error "initialize class ~a: invalid gns: ~s" (class-name class) gns))
		      gns))
		  (class-slots class))))



;;; 
;;; compute-slot-option (METACLASS <class>) (CLASS <class>) NAME VALS
;;;
;;; Combine the values of the slot option NAME collected
;;; from all superclasses of a newly created class of the METACLASS
;;; into value of that option for given class.
;;; VALS should be a list (in class precedence order) of lists
;;; of values for the slot NAME.
;;;
;;; Standard slot options for the metaclass `<class>' :
;;;
;;; allocation:		:=	instance | class | virtual
;;; initarg:		:=	<keyword> | <symbol>
;;; initform:		:=	<expr>
;;; type:		:=	<type specifier>
;;; getter:		:=	<symbol>
;;; setter:		:=	<symbol>
;;; slot-ref:		:=	<procedure>
;;; slot-set!:		:=	<procedure>
;;;

(define-generic compute-slot-option ((meta <class>) (self <class>) slot name vals)
  (case name
    ((allocation:)
     ;; Type of allocation for slot is not inherited and defaulted to `instance'.
     (if (null? (car vals))
	 'instance
	 (caar vals)))

    ((slot-ref: slot-set!:)
     ;; slot-ref and slot-set! is not inherited and not defaulted.
     (if (null? (car vals))
	 '()
	 (caar vals)))

    ((initarg:)
     ;; Initargs are inherited from all superclasses.
     (union-option-values vals))

    ((initform: type:)
     ;; Initform and type are inherited from most precedent superclass.
     (let loop ((vals vals))
       (cond ((null? vals) #f)
	     ((null? (car vals)) (loop (cdr vals)))
	     (else (caar vals)))))

    ((getter: setter:)
     ;; Getters and setters are not inherited.
     ;; (Protocol for applying generic functions cause implicit inheritance).
     (union-option-values (car vals)))

    (else
     (error "unknown slot option ~S for slot ~S" name slot))))


(define (union-option-values lst)
  (let loop ((lst lst) (res '()))
    (cond ((null? lst) res)

	  ((null? (car lst))
	   (loop (cdr lst) res))

	  ((pair? (car lst))
	   (loop (cdr lst) (loop (car lst) res)))

	  ((memq (car lst) res)
	   (loop (cdr lst) res))

	  (else
	   (loop (cdr lst) (cons (car lst) res))))))



;;;
;;; compute-get-n-set (CLASS <class>) SLOT INITARGS
;;;
;;; Compute get-n-set for the SLOT of the CLASS.
;;; get-n-set is a vector of 5 elements, 1'st of which is a name of slot,
;;; 2'nd is getter, 3'rd is setter, 4'th is initializator and 4'th is a type
;;; of the slot.
;;;

(define (class-direct-slot? class slot-name)
  (let loop ((slots (class-direct-slots class)))
    (and (pair? slots)
	 (or (eq? (caar slots) slot-name)
	     (loop (cdr slots))))))


(define-generic compute-get-n-set ((class <class>) slot initargs)
  (let ((allocation (@get-arg allocation: (cdr slot))))

    (case (if (void? allocation) 'instance allocation)

      ;; Instance slot: accessor is just its offset
      ((instance)
       (let ((idx (allocate-slot-index class)))
	 (vector (car slot)
		 'instance
		 idx
		 idx
		 (@get-arg initform: (cdr slot))
		 (@get-arg type: (cdr slot)))))

      ;; Class slot: Class-slots accessor are implemented
      ;; as 2 closures around a Scheme variable.
      ((class)
       (if (class-direct-slot? class (car slot))
	   ; Create new shared cell.
	   (let ((cell (@get-arg initform: (cdr slot))))
	     (vector (car slot)
		     'class
		     (lambda (o)   cell)
		     (lambda (o v) (set! cell v))
		     cell
		     (@get-arg type: (cdr slot))))

	   ; Slot is inherited. Share it with superclass.
	   (let loop ((l (cdr (class-precedence-list class))))
	     (let next ((g (class-get-n-set (car l))))
	       (if (null? g)
		   (loop (cdr l))
		   (if (eq? (car slot) (vector-ref (car g) 0))
		       (car g)
		       (next (cdr g))))))))

      ;; No allocation: slot-ref and slot-set! function
      ;; must be given by the user
      ((virtual)
       (let ((get (@get-arg slot-ref:  (cdr slot)))
	     (set (@get-arg slot-set!: (cdr slot))))
	 (if (or (void? get) (void? set))
	     (error "You must supply slot-ref and slot-set! for slot ~S" (car slot)))
	 (vector (car slot)
		 'virtual
		 get
		 set
		 (@get-arg initform: (cdr slot))
		 (@get-arg type: (cdr slot)))))

      (else
       (error "Unknown allocation ~S for slot ~S" allocation (car slot))))))



;;;
;;; Now we have all required for defining the macro `define-class'.
;;;

(define (ensure-metaclass supers meta)
  (cond (meta meta)
	((null? supers) <class>)
	(else
	 (let* ((all-metas (map class-of supers))
		(all-cpls (apply append (map (lambda (m) (cdr (class-precedence-list m))) all-metas))))

	   ;; Find the most specific metaclasses.
	   ;; The new metaclass will be a subclass of these.
	   (let loop ((all-metas all-metas) (needed-metas '()))
	     (if (null? all-metas)
		 ;; Now return a subclass of the metaclasses we found.
		 ;; If there's only one, just use it.
		 (if (null? (cdr needed-metas))
		     (car needed-metas)
		     (ensure-metaclass-with-supers (reverse! needed-metas)))

		 (loop (cdr all-metas)
		       (if (or (memq (car all-metas) all-cpls) (memq (car all-metas) needed-metas))
			   needed-metas
			   (cons (car all-metas) needed-metas)))))))))


(define ensure-metaclass-with-supers
  (let ((table-of-metas (make-eq-hashtable)))
    (lambda (meta-supers)
      (let* ((name (string->symbol (apply string-append (map (lambda (x) (symbol->string (class-name x))) meta-supers))))
	     (entry (hashtable-ref table-of-metas name #f)))
	(if entry
	    entry
	    (let ((new-metaclass (make <class> supers: meta-supers slots: '() name: (gensym name))))
	      (hashtable-set! table-of-metas name new-metaclass)
	      new-metaclass))))))


;;;
;;; define-class NAME (metaclass: METACLASS) SUPERS SLOTS OPTIONS ...
;;; define-class NAME SUPERS SLOTS OPTIONS ...
;;;
;;; First, define the class NAME itself.
;;; Then, define slot getters and setters.
;;;
;;; Note, that getters and setters cannot be defined before the class
;;; is fully initialized (getters and setters are calculated in process
;;; of class initialization), so we cannot define them at macro-expand time.
;;; 

(define-macro! (define-class form env)
  (or (and (list? form) (pair? (cdr form)) (pair? (cddr form)) (pair? (cdddr form)))
      (klos-error form "invalid syntax"))

  (let ((name (cadr form)) (supers (caddr form)) (slots (cadddr form)) (args (cddddr form)) (meta #f))
    (or (symbol? name)
        (klos-error form "class name should be a symbol"))

    (if (and (pair? supers) (keyword? (car supers)))
        (let ((opts supers))
          (or (pair? args)
              (klos-error form "invalid syntax"))
          (set! supers slots)
          (set! slots (car args))
          (set! args (cdr args))

          (set! meta (@get-arg metaclass: opts #f))))

    (if (null? supers)
        (set! supers '(<object>))
        (or (list? supers)
            (klos-error form "invalid list of supers")))

    (or (and (list? slots) (for-all (lambda (slot) (and (list? slot) (symbol? (car slot)))) slots))
        (klos-error form "invalid list of slots"))

    `(,#'begin (,#'define ,name
              (,#'let ((supers ('#@list ,@supers)))
                (,#'make (,#'ensure-metaclass supers ,meta)
                  name: ',name
                  supers: supers
                  slots: ('#@list ,@(map (lambda (x) `('#@list ',(car x) ,@(cdr x))) slots))
                  ,@args)))
            ;(define-slot-getters ,name ',env)
            ;(define-slot-setters ,name ',env)
            )))




(define (define-slot-getters class env)
  (define (def name slot)
    (if (not (bound? name env))
        (eval-core `(define ,name (make <generic> name: ',name)) env))
    (let ((gfun (environment-ref env name)))
      (if (is-a? gfun <generic>)
          (append-method gfun
                         (make <method>
                           generic-function: gfun
                           specializers: (list class)
                           procedure: (lambda (next next? self) (slot-ref self slot))))
          (error 'define-slot-getter "cannot define slot getter ~s: symbol is bound to ~s that is not generic function" name gfun))))

  (for-each (lambda (s)
	      (let loop ((getters (@get-arg getter: (cdr s))))
		(cond ((pair? getters)
		       (def (car getters) (car s))
		       (loop (cdr getters)))
		      ((symbol? getters)
		       (def getters (car s))))))
	    (class-slots class)))


(define (define-slot-setters class env)
  (define (def name slot)
    (if (not (bound? name env))
        (eval-core `(define ,name (make <generic> name: ',name)) env))
    (let ((gfun (environment-ref env name)))
      (if (is-a? gfun <generic>)
          (append-method gfun
                         (make <method>
                           generic-function: gfun
                           specializers: (list class #t)
                           procedure: (lambda (next next? self val) (slot-set! self slot val))))
          (error 'define-slot-setter "cannot define slot setter ~s: symbol is bound to ~s that is not generic function" name gfun))))

  (for-each (lambda (s)
	      (let loop ((setters (@get-arg setter: (cdr s))))
		(cond ((pair? setters)
		       (def (car setters) (car s))
		       (loop (cdr setters)))
		      ((symbol? setters)
		       (def setters (car s))))))
	    (class-slots class)))



;;;
;;; Change-class
;;;

(define-generic compute-old-slots ((old-class <class>) (new-class <class>))
  (fold-left
   (lambda (gns slots)
     (let ((name (vector-ref gns gns-name)))
       (if (and (slot-exist-in-class? new-class name)
		(eq? (vector-ref (class-get-n-set new-class name) gns-allocation) instance:))
	   (cons (vector-ref gns gns-name) slots)
	   slots)))
   '()
   (class-get-n-set old-class)))

(define-generic compute-new-slots ((old-class <class>) (new-class <class>))
  (fold-left
   (lambda (gns slots)
     (let ((name (vector-ref gns gns-name)))
       (if (and (not (slot-exist-in-class? old-class name))
		(eq? (vector-ref gns gns-allocation) instance:))
	   (cons (vector-ref gns gns-name) slots)
	   slots)))
   '()
   (class-get-n-set new-class)))


(define-generic change-class ((old-instance <object>) (new-class <class>) . initargs)
  (let ((new-instance (allocate-instance new-class initargs))
	(old-slots (compute-old-slots (class-of old-instance) new-class))
	(new-slots (compute-new-slots (class-of old-instance) new-class)))
    
    ;; Set all the common slots to their old value
    (for-each
     (lambda (slot)
       (if (slot-bound? old-instance slot)
	   (slot-set! new-instance slot (slot-ref old-instance slot))
	   (set! new-slots (cons slot new-slots))))
     old-slots)

    (initialize-slots new-slots
                      new-instance
                      (@merge-args initargs (class-default-initargs new-class)))

    (@modify-instance! old-instance new-instance)))



;;;
;;; slot access
;;;

(define-generic slot-unbound)

(define-method slot-unbound ((c <class>) (o <object>) s)
  (error "slot `~s' is unbound in object ~s" s o))

(define-generic slot-missing)

(define-method slot-missing ((c <class>) (o <object>) s)
  (error 'slot-ref "no slot `~s' in object ~s" s o))

(define-method slot-missing ((c <class>) (o <object>) s v)
  (error 'slot-set! "no slot `~s' in object ~s" s o))


;;;
;;; Methods to compare objects
;;;

(define-generic instance-eqv? (x y) #f)
(define-generic instance-equal? (x y) #f)


;;;
;;; Methods to display/write an object
;;;

(define-generic write-instance (object port)
  (format port "#<~A ~X>" (class-name (class-of object)) object))

(define-method write-instance ((self <class>) port)
  (format port  "#<~A ~A>" (class-name (class-of self)) (class-name self)))

(define-method write-instance ((self <generic>) port)
  (format port "#<~A ~A ~A>"
	  (class-name (class-of self))
	  (slot-ref self 'name)
	  (let ((arity (slot-ref self 'arity)))
	    (if (pair? arity)
		(map (lambda (x)
		       (if (class? x) (class-name x) x))
		     arity)
		arity))))

(define-method write-instance ((self <method>) port)
  (format port "#<~A ~A ~A>"
	  (class-name (class-of self))
          (slot-ref self 'generic-function)
	  (let ((specs (slot-ref self 'specializers)))
	    (if (pair? specs)
		(map (lambda (x)
		       (if (class? x) (class-name x) x))
		     specs)
		specs))))

;;;
;;; Display (do the same thing as write by default)
;;;

(define-generic display-instance (object port)
  (write-instance object port))


;;;
;;; Define <composite-class> metaclass.
;;; 

(define-class <composite-class> (<class>) ())


;;;
;;; Slot options for metaclass `<composite-class>'
;;;
;;; allocation:  	propagated:
;;; propagate-to:	<slot>
;;;

(define-method compute-slot-option
  ((meta <class>) (self <composite-class>) slot name vals)

  (case name
    ((propagate-to:)
     ;; Propagations are inherited from all superclasses.
     (union-option-values vals))

    (else
     (call-next-method))))


(define-method compute-get-n-set ((class <composite-class>) slot initargs)
  (define (propagation? prop)
    (or (and (symbol? prop)
	     (assq prop (class-slots class)))
	(and (list? prop)
	     (= (length prop) 2)
	     (symbol? (car prop))
	     (assq (car prop) (class-slots class))
	     (symbol? (cadr prop)))))

  (case (@get-arg allocation: (cdr slot))
    ((propagated:)
     (let ((index (allocate-slot-index class))
	   (props (@get-arg propagate-to: (cdr slot))))
       (if (or (void? props) (null? props))
	   (error "Propagation not specified for slot ~s" (car slot)))
       (if (not (and (list? props) (every propagation? props)))
	   (error "Invalid propagation for slot ~s" (car slot)))
       (vector (car slot)
	       instance:
	       index
	       (lambda (obj val)
		 (for-each (lambda (prop)
			     (if (pair? prop)
				 (slot-set! (slot-ref obj (car prop)) (cadr prop) val)
				 (slot-set! (slot-ref obj prop) (car slot) val)))
			   props)
		 (slot-set! obj index val))
	       (@get-arg initform: (cdr slot))
	       (@get-arg type: (cdr slot)))))

    (else
     (call-next-method))))


;;;
;;; records
;;;

(define-method initialize-instance ((self <record>) initargs)
  (initialize-slots #t self initargs)
  self)

(define-method slot-unbound ((c <rtd>) (o <record>) s)
  (error "field `~s' is not initialized in record ~s" s o))

(define-method slot-missing ((c <rtd>) (o <record>) s)
  (error 'slot-ref "no field `~s' in record ~s" s o))

(define-method slot-missing ((c <rtd>) (o <record>) s v)
  (error 'slot-set! "no field `~s' in record ~s" s o))

(define-method write-instance ((self <rtd>) port)
  (format port  "#<~A ~A>" (class-name (class-of self)) (class-name self)))

(define-method write-instance ((self <record>) port)
  (format port "#<~a" (class-name (class-of self)))
  (let loop ((slots (class-slots (class-of self))))
    (if (null? slots)
        (format port ">")
        (let ((slot (caar slots)))
          (if (slot-bound? self slot)
              (format port " ~A: ~S" slot (slot-ref self slot)))
          (loop (cdr slots))))))


) ;;; End of code
