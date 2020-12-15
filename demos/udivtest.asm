;@plugin filename=lc3_udiv vector=x80

; Demo that shows how to divide two numbers using UDIV.
; UDIV is a custom trap implemented in C++ and will be available at trap vector x80.
; UDIV preconditions R0 = A, R1 = B
; UDIV postconditions R0 = A / B, R1 = A % B
; CC is unchanged, % result has the same behaviour as C and Java in regards to negative numbers
.orig x3000
	LD R0, A
	LD R1, B
	udiv ; or trap x80
	HALT

A .fill 2000
B .fill 8

.end

