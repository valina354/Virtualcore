_main:
    MOV R0, 5      ; Initialize R0 to 5
    MOV R1, 10     ; Initialize R1 to 10

    IF R0 > R1     ; Check if R0 is greater than R1
        MOV R2, 1   ; If true, set R2 to 1
    ELSE
        MOV R2, 0   ; If false, set R2 to 0
    END

    MOV R0, R2   ; Move the value of R2 to R0
    INT 0x01      ; Print the value of R0 (either 1 or 0)
    HLT
