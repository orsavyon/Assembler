; file ps.as

.entry 

.extern W

.define sz 2

MAIN: mov  r6,, LIST[sz]

LOOP: jmip W

 prn # -5

 mov STR[abc], STR[2]

 sub r1, r4, r5

 cmp K, #sz,

 bneo W

L1:: inc L3

.entry LOOP

 bne LOOP

END: hlt

.define len = 4

STR: .string “abc&def”

LIST: .data  6.6, -9, len

K: .data 22

.extern L3
.entry W 