(var a 6)
(var a -7)
; a is -7

((label fibl (lambda (n)
    (cond
        ((eq n 0) 0)
        ((eq n 1) 0)
        ((eq n 2) 1)
        (t (add (fibl (sub n 1)) (fibl (sub n 2))))
    )
)) 6)

(fun fibfn (n) 
    (add 2 45)
    (add 2 6)
    (add 2 3)
    (cond
        ((eq n 0) 0)
        ((eq n 1) 0)
        ((eq n 2) 1)
        (t (add (fibfn (sub n 1)) (fibfn (sub n 2))))
    )
)
; Might not wanna get this higher than 10, it's not optimized

(let ((x (add 3 6)) (y 10)) 
    (add x x)
    (add x y)
)

(const b 6)
(const b 7)
;b is 6, set only once

a
b

(fibfn 6)