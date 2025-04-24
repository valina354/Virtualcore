MOV R0, 0 ; sanity check
MOV R1, 5

start:
	CALL enable_interrupts
	INC R0
	CMP R0, R1
	INT 0x01
	JMPNE start
	HLT
	
enable_interrupts:
	ELI
	RET
