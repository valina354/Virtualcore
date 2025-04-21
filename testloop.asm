MOV R0, 5      ; Initialize loop counter
MOV R1, 0      ; Accumulator

loop_start:
    ADD R1, R0     ; Add current counter value to R1
    INT 0x01       ; Print R0 (should print 5, 4, 3, 2, 1)
    INT 0x03       ; Newline
    LOOP R0, loop_start ; Decrement R0, jump to loop_start if R0 != 0

MOV R0, R1     ; Move final sum to R0
INT 0x01       ; Print final sum (should be 5+4+3+2+1 = 15)
INT 0x03       ; Newline
HLT
