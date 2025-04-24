ELI

FMOV F0, 5.5
FINC F0         ; F0 = 5.5 + 1.0 = 6.5
INT 0x0E        ; Print F0 (should show 6.5)

FINC F0         ; F0 = 6.5 + 1.0 = 7.5
INT 0x0E        ; Print F0 (should show 7.5)

FDEC F0         ; F0 = 7.5 - 1.0 = 6.5
INT 0x0E        ; Print F0 (should show 6.5)

FMOV F1, -1.2
FDEC F1         ; F1 = -1.2 - 1.0 = -2.2
FMOV F0, F1     ; Move to F0 for printing
INT 0x0E        ; Print F0 (should show -2.2)

HLT
