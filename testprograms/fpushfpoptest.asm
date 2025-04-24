ELI

FMOV F0, 1.234
FMOV F1, 5.678

FPUSH F0        ; Push 1.234 onto stack
FPUSH F1        ; Push 5.678 onto stack

; Overwrite F0 and F1 to prove POP works
FMOV F0, 0.0
FMOV F1, -1.0

FPOP F1         ; Pop into F1 (should get 5.678)
INT 0x0E        ; Print F1
FMOV F0, F1
INT 0x0E

FPOP F0         ; Pop into F0 (should get 1.234)
INT 0x0E        ; Print F0

HLT
