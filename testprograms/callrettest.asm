MOV R0, 1
MOV R1, 5
ADD R0, R1
CALL PRINT
ADD R0, R1
CALL PRINT
HLT

PRINT:
	INT 0x01
	RET
