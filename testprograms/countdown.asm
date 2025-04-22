MOV R0, 1
MOV R2, 50
MOV R3, 1
addition:
ADD R0, R3
INT 0x01
CMP R0, R2
JNE addition
