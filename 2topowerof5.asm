MOV R2, 2
MOV R0, 2
MOV R1, 4

MultiplyLoop:
    MUL R0, R2
    LOOP R1, MultiplyLoop

INT 0x01
HLT
