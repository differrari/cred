(load "~/redbuild/v3/redbuild.lisp")

(defvar *beyond-lib* nil)
(defvar *beyond-interpreter* t)

(defmacro exname () (if (eq *beyond-interpreter* t) "imaginal" "cred"))
(defmacro extype () (if (eq *beyond-lib* t) :lib :bin))
(defmacro if-comp (a b) (if (eq *beyond-interpreter* nil) a b))

(redbuild:quick-build (make-instance `redbuild:redmod
    :name "rulegen"
    :type :bin
    :target (redbuild:native)
    :srcs (list 
		"ir/manual_gen.c" 
		"codegen/codeformat.c" 
		"codegen/codegen.c" 
		"codegen/identity_transform.c"
		"ir/general.c" 
		"rule_generator/rulecodegen.c" 
		"rule_generator/ruleparser.c" 
		"rule_generator/rule_transform.c"
	)
    :flags (list 
        "-DNOCODETRANSFORM"
        "-DRULECODEGEN"
        "-DRULETRANSFORM"
    )
) :add-dependencies t :run t :run-args (if-comp "languages/cred.rules" "languages/lisp.rules") :success (lambda () 
    (redbuild:quick-cred "semantic/sem_enum.cred" "semantic/semantic_rules")
    (redbuild:quick-cred "codegen/codegen.cred" "codegen/codegen")
    (redbuild:quick-build (make-instance `redbuild:redmod
        :name (exname)
        :type (extype)
        :target (redbuild:dyn-target)
        :srcs (redbuild:all-sources-ignoring "c" (list "output.c" "build.c" (if-comp "imaginal.c" "cred_main.c") (if-comp "interpreter/repl.c" nil) "ruleparser.c"))
        :flags (list 
            "-DCCODEGEN"
            "-DCTRANS"
            "-DNORULETRANSFORM"
        )
    ) :add-dependencies t :run t :run-args (if-comp "street.cred" "test.lisp") :success (lambda () (redbuild:emit-compile-commands)))
))