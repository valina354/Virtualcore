#define SECTOR_BUFFER 1000
#define SECTORS_PER_WRITE 1

start:
    INT 0x39
    MOV R4, R0
    MOV R5, R1

    MOV R2, SECTOR_BUFFER
    MOV R3, R5
    CLR R6

zero_loop:
    CMP R3, R6
    JEQ zero_loop_end
    STORE R6, R2
    INC R2
    DEC R3
    JMP zero_loop

zero_loop_end:
    CLR R1

wipe_loop_start:
    CMP R1, R4
    JEQ wipe_loop_end
    JMPH wipe_loop_end

    MOV R0, R1
    MOV R2, SECTOR_BUFFER
    MOV R3, SECTORS_PER_WRITE
    INT 0x38

    INC R1
    JMP wipe_loop_start

wipe_loop_end:
    INT 0x35
    HLT
