

DUPLABEL: clr r1
DUPLABEL: inc r1


EMPTYLABEL: 


.extern EXTERNLABEL
.entry EXTERNLABEL


.entry ENTRYLABEL
.extern ENTRYLABEL


mov r1, 


B: .data 5,,6
C: .data ,7


mov  r1, # -5


STR1: .string "abc$ef"
STR2: .string "ghi^jk"


D: add r1, r2, r3


1INVALIDLABEL: hlt


#1234


FLOATLABEL mov r1, r2


E: .data 5, .string "hello", 6


F: clr r1 , hlt


G: mov r2, UNDEFINED


LONG: .string "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopq"


.string: mov r2, r1


mov: add r1, r2


H:: clr r3


J: undefaction r1


K: mov       r1,        r2


L: .data 5.5


M@: clr @r1
N$: inc @r2


O: mov rX, r10
