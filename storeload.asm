MOV R0, 12345   ; Put a value in R0
MOV R1, 100     ; Put a memory address (100) in R1

STORE R0, R1    ; Store the value from R0 (12345) into memory location 100

MOV R2, 9999
MOV R0, 0

PUSH R1         ; Save the important address (100) before overwriting R1
MOV R0, 95      ; Start address for dump
MOV R1, 10      ; Length to dump (Temporarily overwrites R1)
INT 0x22        ; Dump memory interrupt
POP R1          ; Restore the important address (100) back into R1

; --- Load back ---
LOAD R2, R1     ; Load value from memory address in R1 (should be 100 again)

; --- Print the result ---
MOV R0, R2
INT 0x01        ; Print R0 (should print 12345)
INT 0x03

HLT
