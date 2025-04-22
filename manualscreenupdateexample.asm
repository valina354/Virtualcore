#define MSG_MANUAL_START_ADDR 1000
#define MSG_MANUAL_DONE_ADDR  1100

    STRMOV MSG_MANUAL_START_ADDR, "Starting manual update screen fill..."
    MOV R0, MSG_MANUAL_START_ADDR
    INT 0x02
    INT 0x03

    INT 0x18
    MOV R10, R0
    MOV R11, R1

    MOV R4, 1

    INT 0x32

    MOV R2, R11
outer_loop_manual:
    MOV R3, R10
inner_loop_manual:
    MOV R0, R10
    SUB R0, R3
    MOV R1, R11
    SUB R1, R2

    MOV R12, R4
    MOV R13, R2
    MOV R14, R3
    MOV R2, R12
    INT 0x10
    MOV R2, R13
    MOV R3, R14
    MOV R4, R12

    INC R4
    MOV R13, 16
    CMP R4, R13
    JNE color_ok2
    MOV R4, 1
color_ok2:

    DEC R3
    MOV R13, 0
    CMP R3, R13
    JMPH inner_loop_manual

    DEC R2
    MOV R13, 0
    CMP R2, R13
    JMPH outer_loop_manual

    INT 0x19

    INT 0x32
    MOV R7, R6
    SUB R7, R5

    STRMOV MSG_MANUAL_DONE_ADDR, "Manual update fill finished. Duration (ms):"
    MOV R0, MSG_MANUAL_DONE_ADDR
    INT 0x02
    MOV R0, R7
    INT 0x01
    INT 0x03

    HLT
