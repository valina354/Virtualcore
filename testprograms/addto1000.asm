MOV R0, 100
MOV R1, 100
MOV R2, 1000

addition:
ADD R0, R1
CALL Print
CMP R0, R2
JNE addition
CALL Shutdown

Print:
	INT 0x01
	RET
	
Shutdown:
	INT 0x31
